#include "precompiled.h"
#include "jitasm.h"

CDeltaJitRegistry g_DeltaJitRegistry;

uint32 DELTAJIT_CreateMask(int startBit, int endBit) {
	if (startBit < 0) startBit = 0;
	if (endBit < 0) endBit = 0;
	if (startBit > 32) startBit = 32;
	if (endBit > 32) endBit = 32;

	uint32 res = 0xFFFFFFFF;
	res &= (0xFFFFFFFF << startBit);
	res &= (0xFFFFFFFF >> (32 - endBit));
	return res;
}

unsigned int DELTAJIT_GetFieldSize(delta_description_t* desc) {
	switch (desc->fieldType & ~DT_SIGNED) {
	case DT_BYTE: return 1;
	case DT_SHORT: return 2; 
	case DT_FLOAT:
	case DT_INTEGER:
	case DT_ANGLE:
	case DT_TIMEWINDOW_8:
	case DT_TIMEWINDOW_BIG:
		return 4;

	case DT_STRING:
		return desc->fieldSize;

	default:
		rehlds_syserror("%s: Unknown delta field type %d", __FUNCTION__, desc->fieldType);
		return 0;
	}
}

void DELTAJIT_CreateDescription(delta_t* delta, deltajitdata_t &jitdesc) {

	//calculate max offset and number of memblocks
	unsigned int maxOffset = 0;
	for (int i = 0; i < delta->fieldCount; i++) {
		delta_description_t* desc = &delta->pdd[i];

		unsigned fieldMaxOff = DELTAJIT_GetFieldSize(desc);
		fieldMaxOff += desc->fieldOffset;
		if (fieldMaxOff > maxOffset) {
			maxOffset = fieldMaxOff;
		}
	}

	unsigned int numMemBlocks = maxOffset / 16;
	if (maxOffset % 16 || numMemBlocks == 0) {
		numMemBlocks++;
	}

	//sanity checks & pre-clean
	if (numMemBlocks >= DELTAJIT_MAX_BLOCKS) {
		rehlds_syserror("%s: numMemBlocks >= DELTAJIT_MAX_BLOCKS (%d >= %d)", __FUNCTION__, numMemBlocks, DELTAJIT_MAX_BLOCKS);
		return;
	}

	if (delta->fieldCount >= DELTAJIT_MAX_FIELDS) {
		rehlds_syserror("%s: fieldCount >= DELTAJIT_MAX_FIELDS (%d >= %d)", __FUNCTION__, delta->fieldCount, DELTAJIT_MAX_FIELDS);
		return;
	}

	memset(&jitdesc, 0, sizeof(jitdesc));
	jitdesc.numblocks = numMemBlocks;
	jitdesc.numFields = delta->fieldCount;

	//create descriptions (primitive types)
	for (int i = 0; i < delta->fieldCount; i++) {
		delta_description_t* fieldDesc = &delta->pdd[i];

		unsigned int blockId = fieldDesc->fieldOffset / 16;
		unsigned int blockStart = blockId * 16;

		unsigned int fieldSize = DELTAJIT_GetFieldSize(fieldDesc);
		

		auto jitField = &jitdesc.fields[i];
		jitField->id = i;
		jitField->offset = fieldDesc->fieldOffset;
		jitField->type = fieldDesc->fieldType;
		jitField->length = fieldSize;

		if (fieldDesc->fieldType != DT_STRING) {
			bool firstBlock = true;
			deltajit_memblock_field* blockField = NULL;
			while (blockStart < fieldDesc->fieldOffset + fieldSize) {
				deltajit_memblock* memblock = &jitdesc.blocks[blockId];
				uint32 mask = DELTAJIT_CreateMask(fieldDesc->fieldOffset - blockStart, fieldDesc->fieldOffset + fieldSize - blockStart);
				blockField = &memblock->fields[memblock->numFields++];
				blockField->field = jitField;
				jitField->numBlocks++;
				blockField->first = firstBlock;
				blockField->mask = mask;

				blockStart += 16;
				blockId++;
				firstBlock = false;
			}
			blockField->last = true;
		}
	}

}

class CDeltaClearMarkFieldsJIT : public jitasm::function<void, CDeltaClearMarkFieldsJIT, void*, void*, void*> {
public:
	deltajitdata_t *jitdesc;


	CDeltaClearMarkFieldsJIT(deltajitdata_t *_jitdesc) : jitdesc(_jitdesc) {
	}

	void checkFieldMask(jitasm::Frontend::Reg32& mask, deltajit_memblock_field* jitField);
	void main(Addr src, Addr dst, Addr fields);
	void processStrings(Addr src, Addr dst, Addr fields);
};

void CDeltaClearMarkFieldsJIT::checkFieldMask(jitasm::Frontend::Reg32& mask, deltajit_memblock_field* jitField) {
	test(mask, (uint16)jitField->mask);
	setnz(al);
	movzx(dx, al);
}

void CDeltaClearMarkFieldsJIT::main(Addr src, Addr dst, Addr fields)
{

#ifndef REHLDS_FIXES
	sub(esp, 12); //some local storage is required for precise DT_TIMEWINDOW marking
#endif

	mov(esi, ptr[src]);
	mov(edi, ptr[dst]);
	mov(ecx, ptr[fields]);
	movdqu(xmm3, xmmword_ptr[esi]);
	movdqu(xmm4, xmmword_ptr[edi]);

	auto zero_xmm = xmm2;
	pxor(zero_xmm, zero_xmm);

	auto mask = ebx;
	xor_(mask, mask);
	for (unsigned int i = 0; i < jitdesc->numblocks; i++) {
		movdqa(xmm0, xmm3);
		movdqa(xmm1, xmm4);

		//prefetch next blocks
		if (i < jitdesc->numblocks) {
			mov(esi, ptr[src]);
			mov(edi, ptr[dst]);
			movdqu(xmm3, xmmword_ptr[esi + ((i + 1) * 16)]);
			movdqu(xmm4, xmmword_ptr[edi + ((i + 1) * 16)]);
		}

		pxor(xmm0, xmm1);
		pcmpeqb(xmm0, xmm2);
		pmovmskb(mask, xmm0);
		not_(mask);


		auto block = &jitdesc->blocks[i];
		for (unsigned int j = 0; j < block->numFields; j++) {
			auto jitField = &block->fields[j];

#ifndef REHLDS_FIXES
			if (jitField->field->type == DT_TIMEWINDOW_8 || jitField->field->type == DT_TIMEWINDOW_BIG) {
				if (jitField->last) {
					
					float multiplier = (jitField->field->type == DT_TIMEWINDOW_8) ? 100.0f : 1000.0f;
					uint32 m32 = *(uint32*)(&multiplier);
					mov(dword_ptr[esp], m32);

					fld(dword_ptr[esi + jitField->field->offset]);
					fld(dword_ptr[esp]);
					fmulp();
					fstp(qword_ptr[esp + 4]);
					cvttsd2si(eax, mmword_ptr[esp + 4]);

					fld(dword_ptr[edi + jitField->field->offset]);
					fld(dword_ptr[esp]);
					fmulp();
					fstp(qword_ptr[esp + 4]);
					cvttsd2si(edx, mmword_ptr[esp + 4]);

					cmp(eax, edx);
					setne(al);
					movzx(dx, al);

				} else {
					continue;
				}
			} else {
				checkFieldMask(mask, jitField);
			}
#else
			checkFieldMask(mask, jitField);
#endif

			int flagsOffset = (jitField->field->id * sizeof(delta_description_t) + offsetof(delta_description_t, flags));
			if (jitField->first) {
				mov(word_ptr[ecx + flagsOffset], dx);
			}
			else {
				or_(word_ptr[ecx + flagsOffset], dx);
			}
		}
	}

	processStrings(src, dst, fields);

#ifndef REHLDS_FIXES
	add(esp, 12); //some local storage is required for precise DT_TIMEWINDOW marking
#endif
}

void CDeltaClearMarkFieldsJIT::processStrings(Addr src, Addr dst, Addr fields) {
	//strings
	for (unsigned int i = 0; i < jitdesc->numFields; i++) {
		auto jitField = &jitdesc->fields[i];
		if (jitField->type != DT_STRING)
			continue;

		mov(esi, ptr[src]);
		mov(edi, ptr[dst]);
		add(esi, jitField->offset);
		add(edi, jitField->offset);

		push(esi);
		push(edi);
		mov(ecx, (size_t)&Q_stricmp);
		call(ecx);
		add(esp, 8);
		test(eax, eax);
		setnz(cl);
		movzx(cx, cl);

		mov(edx, ptr[fields]);
		int flagsOffset = (jitField->id * sizeof(delta_description_t) + offsetof(delta_description_t, flags));
		mov(word_ptr[edx + flagsOffset], cx);
	}
}

class CDeltaJit {
public:
	CDeltaClearMarkFieldsJIT* cleanMarkCheckFunc;
	delta_t* delta;

	CDeltaJit(delta_t* _delta, CDeltaClearMarkFieldsJIT* _cleanMarkCheckFunc) {
		delta = _delta;
		cleanMarkCheckFunc = _cleanMarkCheckFunc;
	}

	virtual ~CDeltaJit() {
		if (cleanMarkCheckFunc) {
			delete cleanMarkCheckFunc;
			cleanMarkCheckFunc = NULL;
		}
	}
};

CDeltaJitRegistry::CDeltaJitRegistry() {
	
}

void CDeltaJitRegistry::RegisterDeltaJit(delta_t* delta, CDeltaJit* deltaJit) {
	void* key = delta;
#ifndef REHLDS_FIXES
	m_DeltaToJITMap.put(key, deltaJit);
#else
	delta->jit = deltaJit;
#endif
}

CDeltaJit* CDeltaJitRegistry::GetJITByDelta(delta_t* delta) {
#ifndef REHLDS_FIXES
	void* key = delta;
	auto node = m_DeltaToJITMap.get(key);
	return (node != NULL) ? node->val : NULL;
#else
	return delta->jit;
#endif
}

void CDeltaJitRegistry::CreateAndRegisterDeltaJIT(delta_t* delta) {
	deltajitdata_t data;
	DELTAJIT_CreateDescription(delta, data);

	CDeltaClearMarkFieldsJIT* cleanMarkCheckFunc = new CDeltaClearMarkFieldsJIT(&data);
	cleanMarkCheckFunc->Assemble();
	cleanMarkCheckFunc->jitdesc = NULL;

	CDeltaJit* deltaJit = new CDeltaJit(delta, cleanMarkCheckFunc);
	RegisterDeltaJit(delta, deltaJit);
}

NOINLINE void DELTAJit_ClearAndMarkSendFields(unsigned char *from, unsigned char *to, delta_t *pFields) {
	CDeltaJit* deltaJit = g_DeltaJitRegistry.GetJITByDelta(pFields);
	if (!deltaJit) {
		rehlds_syserror("%s: JITted delta encoder not found for delta %p", __FUNCTION__, pFields);
		return;
	}

	CDeltaClearMarkFieldsJIT &func = *deltaJit->cleanMarkCheckFunc;
	func(from, to, pFields->pdd);

	if (pFields->conditionalencode)
		pFields->conditionalencode(pFields, from, to);

}

void CDeltaJitRegistry::Cleanup() {
#ifndef REHLDS_FIXES
	for (auto itr = m_DeltaToJITMap.iterator(); itr.hasElement(); itr.next()) {
		auto node = itr.current();
		CDeltaJit* deltaJit = node->val;
		delete deltaJit;
	}

	m_DeltaToJITMap.clear();
#else
	delta_info_t* cur = g_sv_delta;
	while (cur) {
		delete cur->delta->jit;
		cur->delta->jit = NULL;
		cur = cur->next;
	}
#endif
}
