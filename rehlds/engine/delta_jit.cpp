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

class CDeltaClearMarkFieldsJIT : public jitasm::function<int, CDeltaClearMarkFieldsJIT, void*, void*, void*> {
public:
	deltajitdata_t *jitdesc;
	deltajit_marked_count_type_t countType;


	CDeltaClearMarkFieldsJIT(deltajitdata_t *_jitdesc, deltajit_marked_count_type_t _countType)
		: jitdesc(_jitdesc), countType(_countType) {
	}

	void checkFieldMask(jitasm::Frontend::Reg32& mask, deltajit_memblock_field* jitField);
	Result main(Addr src, Addr dst, Addr delta);
	void processStrings(Addr src, Addr dst, Addr delta);
	void callConditionalEncoder(Addr src, Addr dst, Addr delta);
	void countMarkedFields();
};

void CDeltaClearMarkFieldsJIT::checkFieldMask(jitasm::Frontend::Reg32& mask, deltajit_memblock_field* jitField) {
	test(mask, (uint16)jitField->mask);
	setnz(al);
	movzx(dx, al);
}

void CDeltaClearMarkFieldsJIT::callConditionalEncoder(Addr src, Addr dst, Addr delta) {
	//This generator expects that following registers are already initialized:
	// esi = src
	// edi = dst

	int condEncoderOffset = (offsetof(delta_t, conditionalencode));
	mov(eax, ptr[delta]);
	mov(ecx, dword_ptr[eax + condEncoderOffset]);
	If(ecx != 0);
		push(edi);
		push(esi);
		push(eax);
		
		call(ecx);
		add(esp, 12);
	EndIf();
}

void CDeltaClearMarkFieldsJIT::countMarkedFields() {
	//This generator expects that following registers are already initialized:
	// ebx = delta->pdd
	//
	//Returns value: 'changed flag' or count in eax

	xor_(eax, eax);

	if (countType == DJ_M_DONT_COUNT) {	
		return;
	}

	for (unsigned int i = 0; i < jitdesc->numFields; i++) {
		int fieldId = jitdesc->fields[i].id;
		int flagsOffset = (fieldId * sizeof(delta_description_t) + offsetof(delta_description_t, flags));

		if (i & 1) { //rotate between cx and dx to decrease instruction result dependencies
			mov(cx, word_ptr[ebx + flagsOffset]);
			and_(cx, 1);
			or_(ax, cx);
		} else {
			mov(dx, word_ptr[ebx + flagsOffset]);
			and_(dx, 1);
			or_(ax, dx);
		}

		//insert 'is changed' check every 8 fields
		if ((i & 7) == 0) {
			jnz("countMarkedFields_finish");
		}
	}

	L("countMarkedFields_finish");
}

CDeltaClearMarkFieldsJIT::Result CDeltaClearMarkFieldsJIT::main(Addr src, Addr dst, Addr delta)
{

#ifndef REHLDS_FIXES
	sub(esp, 12); //some local storage is required for precise DT_TIMEWINDOW marking
#endif

	mov(esi, ptr[src]);
	mov(edi, ptr[dst]);
	mov(ebx, ptr[delta]);
	int fieldsOffset = (offsetof(delta_t, pdd));
	mov(ebx, dword_ptr[ebx + fieldsOffset]);
	movdqu(xmm3, xmmword_ptr[esi]);
	movdqu(xmm4, xmmword_ptr[edi]);

	auto zero_xmm = xmm2;
	pxor(zero_xmm, zero_xmm);

	auto mask = ecx;
	xor_(mask, mask);
	for (unsigned int i = 0; i < jitdesc->numblocks; i++) {
		movdqa(xmm0, xmm3);
		movdqa(xmm1, xmm4);

		//prefetch next blocks
		if (i < jitdesc->numblocks) {
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
				mov(word_ptr[ebx + flagsOffset], dx);
			}
			else {
				or_(word_ptr[ebx + flagsOffset], dx);
			}
		}
	}

	processStrings(src, dst, delta);

	callConditionalEncoder(src, dst, delta);

	countMarkedFields();

#ifndef REHLDS_FIXES
	add(esp, 12); //some local storage is required for precise DT_TIMEWINDOW marking
#endif

	return eax;
}

void CDeltaClearMarkFieldsJIT::processStrings(Addr src, Addr dst, Addr delta) {
	//This generator expects that following registers are already initialized:
	// ebx = delta->pdd
	// esi = src
	// edi = dst

	//strings
	for (unsigned int i = 0; i < jitdesc->numFields; i++) {
		auto jitField = &jitdesc->fields[i];
		if (jitField->type != DT_STRING)
			continue;

		mov(eax, esi);
		mov(edx, edi);
		add(eax, jitField->offset);
		add(edx, jitField->offset);

		push(eax);
		push(edx);
		mov(ecx, (size_t)&Q_stricmp);
		call(ecx);
		add(esp, 8);
		test(eax, eax);
		setnz(cl);
		movzx(cx, cl);


		int flagsOffset = (jitField->id * sizeof(delta_description_t) + offsetof(delta_description_t, flags));
		mov(word_ptr[ebx + flagsOffset], cx);
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

	CDeltaClearMarkFieldsJIT* cleanMarkCheckFunc = new CDeltaClearMarkFieldsJIT(&data, DJ_M_CHECK);
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
	func(from, to, pFields);
}

NOINLINE int DELTAJit_Feilds_Clear_Mark_Check(unsigned char *from, unsigned char *to, delta_t *pFields) {
	CDeltaJit* deltaJit = g_DeltaJitRegistry.GetJITByDelta(pFields);
	if (!deltaJit) {
		rehlds_syserror("%s: JITted delta encoder not found for delta %p", __FUNCTION__, pFields);
		return 0;
	}

	CDeltaClearMarkFieldsJIT &func = *deltaJit->cleanMarkCheckFunc;
	return func(from, to, pFields);
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
