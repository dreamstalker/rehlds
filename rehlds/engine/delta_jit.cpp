#include "precompiled.h"
#include "jitasm.h"

CDeltaJitRegistry g_DeltaJitRegistry;

bool deltajit_memblock::isEmpty() const
{
	if (numFields == 0)
		return true;

	for (size_t i = 0; i < numFields; i++)
	{
		if (fields[i].mask != 0)
			return false;
	}

	return true;
}

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

class CDeltaClearMarkFieldsJIT;

class CDeltaJit {
public:
	CDeltaClearMarkFieldsJIT* cleanMarkCheckFunc;
	delta_t* delta;

	int markedFieldsMaskSize;

	int marked_fields_mask[2];
	int sse_highbits[2]; //High 64 bits for manipulating marked_fields_mask via SSE registers 

	CDeltaJit(delta_t* _delta, CDeltaClearMarkFieldsJIT* _cleanMarkCheckFunc);

	virtual ~CDeltaJit();
};

class CDeltaClearMarkFieldsJIT : public jitasm::function<int, CDeltaClearMarkFieldsJIT, void*, void*, void*> {
public:
	deltajitdata_t *jitdesc;
	deltajit_marked_count_type_t countType;

	XmmReg marked_fields_mask = xmm5;


	CDeltaClearMarkFieldsJIT(deltajitdata_t *_jitdesc, deltajit_marked_count_type_t _countType)
		: jitdesc(_jitdesc), countType(_countType) {
	}

	void checkFieldMask(jitasm::Frontend::Reg32& mask, deltajit_memblock_field* jitField);
	Result main(Addr src, Addr dst, Addr deltaJit);
	void processStrings(Addr src, Addr dst);
	void callConditionalEncoder(Addr src, Addr dst, Addr deltaJit);
	void calculateBytecount();
};

void CDeltaClearMarkFieldsJIT::checkFieldMask(jitasm::Frontend::Reg32& mask, deltajit_memblock_field* jitField) {
	test(mask, (uint16)jitField->mask);
	setnz(al);
	movzx(edx, al);
}

void CDeltaClearMarkFieldsJIT::callConditionalEncoder(Addr src, Addr dst, Addr deltaJit) {
	//This generator expects that following registers are already initialized:
	// esi = src
	// edi = dst

	int deltaOffset = (offsetof(CDeltaJit, delta));
	int condEncoderOffset = (offsetof(delta_t, conditionalencode));
	mov(eax, ptr[deltaJit]);
	mov(eax, ptr[eax + deltaOffset]);
	mov(ecx, dword_ptr[eax + condEncoderOffset]);
	If(ecx != 0);
		push(edi);
		push(esi);
		push(eax);
		
		call(ecx);
		add(esp, 12);
	EndIf();
}

void CDeltaClearMarkFieldsJIT::calculateBytecount() {
	//This generator expects that following registers are already initialized:
	//ebx = delta

	size_t delta_markbits_offset = offsetof(CDeltaJit, marked_fields_mask);
	mov(eax, dword_ptr[ebx + delta_markbits_offset]);
	xor_(edx, edx);

	// 0-7
	mov(ecx, 1);
	test(eax, 0xFF);
	cmovnz(edx, ecx);

	// 8-15
	if (jitdesc->numFields > 7)
	{
		mov(esi, 2);
		test(eax, 0xFF00);
		cmovnz(edx, esi);
	}

	// 16-23
	if (jitdesc->numFields > 15)
	{
		mov(ecx, 3);
		test(eax, 0xFF0000);
		cmovnz(edx, ecx);
	}

	// 24-31
	if (jitdesc->numFields > 23)
	{
		mov(esi, 4);
		test(eax, 0xFF000000);
		cmovnz(edx, esi);
	}

	if (jitdesc->numFields > 31)
	{
		mov(eax, dword_ptr[ebx + delta_markbits_offset + 4]);

		// 32-39
		mov(ecx, 5);
		test(eax, 0xFF);
		cmovnz(edx, ecx);

		// 40-47
		if (jitdesc->numFields > 39)
		{
			mov(esi, 6);
			test(eax, 0xFF00);
			cmovnz(edx, esi);
		}

		// 48-55
		if (jitdesc->numFields > 47)
		{
			mov(ecx, 7);
			test(eax, 0xFF0000);
			cmovnz(edx, ecx);
		}

		// maxfields is 56
	}

	size_t delta_masksize_offset = offsetof(CDeltaJit, markedFieldsMaskSize);
	mov(dword_ptr[ebx + delta_masksize_offset], edx);
}

CDeltaClearMarkFieldsJIT::Result CDeltaClearMarkFieldsJIT::main(Addr src, Addr dst, Addr deltaJit)
{
#ifndef REHLDS_FIXES
	sub(esp, 12); //some local storage is required for precise DT_TIMEWINDOW marking
#endif

	mov(esi, ptr[src]);
	mov(edi, ptr[dst]);

	for (unsigned int i = 0; i < jitdesc->numblocks; i++) {
		if (!jitdesc->blocks[i].isEmpty()) {
			movdqu(xmm3, xmmword_ptr[esi + (i * 16)]);
			movdqu(xmm4, xmmword_ptr[edi + (i * 16)]);
			break;
		}
	}

	auto mask = ecx;
	xor_(mask, mask);
	
	pxor(marked_fields_mask, marked_fields_mask);

	for (unsigned int i = 0; i < jitdesc->numblocks; i++) {
		auto block = &jitdesc->blocks[i];

		if (block->isEmpty())
			continue;

		movdqa(xmm0, xmm3);
		movdqa(xmm1, xmm4);

		//prefetch next blocks
		for (unsigned int j = i + 1; j < jitdesc->numblocks; j++) {
			if (!jitdesc->blocks[j].isEmpty()) {
				movdqu(xmm3, xmmword_ptr[esi + (j * 16)]);
				movdqu(xmm4, xmmword_ptr[edi + (j * 16)]);
				break;
			}
		}

		pcmpeqb(xmm0, xmm1);
		pmovmskb(mask, xmm0);
		not_(mask);

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
					movzx(edx, al);

				} else {
					continue;
				}
			} else {
				checkFieldMask(mask, jitField);
			}
#else
			checkFieldMask(mask, jitField);
#endif

			// set bit in send mask
			movd(xmm0, edx);
			psllq(xmm0, jitField->field->id);
			por(marked_fields_mask, xmm0);
		}
	}

	size_t delta_markbits_offset = offsetof(CDeltaJit, marked_fields_mask);

	//Before calling the condition encoder we have to save 'marked fields' mask 
	//from SSE register into the CDeltaJit::marked_fields_mask, because conditional encoder can modify the mask
	mov(ebx, ptr[deltaJit]);
	movdqu(xmmword_ptr[ebx + delta_markbits_offset], marked_fields_mask);

	processStrings(src, dst);

	//emit conditional encoder call
	callConditionalEncoder(src, dst, deltaJit);
	
	// check if mask is empty
	mov(edi, dword_ptr[ebx + delta_markbits_offset]);
	or_(edi, dword_ptr[ebx + delta_markbits_offset + 4]);

	If(edi != 0);
		calculateBytecount();
	Else();
		//set maskSize to 0 if there are no marked fields
		size_t delta_masksize_offset = offsetof(CDeltaJit, markedFieldsMaskSize);
		xor_(edx, edx);
		mov(dword_ptr[ebx + delta_masksize_offset], edx);
	EndIf();

#ifndef REHLDS_FIXES
	add(esp, 12); //some local storage is required for precise DT_TIMEWINDOW marking
#endif // REHLDS_FIXES

	return edx;
}

void CDeltaClearMarkFieldsJIT::processStrings(Addr src, Addr dst) {
	//This generator expects that following registers are already initialized:
	// esi = src
	// edi = dst
	// ebx = deltaJit

	size_t delta_markbits_offset = offsetof(CDeltaJit, marked_fields_mask);

	//strings
	for (unsigned int i = 0; i < jitdesc->numFields; i++) {
		auto jitField = &jitdesc->fields[i];
		if (jitField->type != DT_STRING)
			continue;

		// will be parallel
		lea(eax, ptr[esi + jitField->offset]);
		lea(edx, ptr[edi + jitField->offset]);

		push(eax);
		push(edx);
		mov(ecx, (size_t)&Q_stricmp);
		call(ecx);
		add(esp, 8);
		xor_(ecx, ecx);
		test(eax, eax);
		setnz(cl);

		shl(ecx, jitField->id & 31);
		or_(ptr[ebx + delta_markbits_offset + ((jitField->id > 31) ? 4 : 0)], ecx);
	}
}

CDeltaJit::CDeltaJit(delta_t* _delta, CDeltaClearMarkFieldsJIT* _cleanMarkCheckFunc) {
	delta = _delta;
	cleanMarkCheckFunc = _cleanMarkCheckFunc;
}

CDeltaJit::~CDeltaJit() {
	if (cleanMarkCheckFunc) {
		delete cleanMarkCheckFunc;
		cleanMarkCheckFunc = NULL;
	}
}

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

CDeltaJit* DELTAJit_LookupDeltaJit(const char* callsite, delta_t *pFields) {
	CDeltaJit* deltaJit = g_DeltaJitRegistry.GetJITByDelta(pFields);
	if (!deltaJit) {
		rehlds_syserror("%s: JITted delta encoder not found for delta %p", callsite, pFields);
		return NULL;
	}

	return deltaJit;
}

NOINLINE int DELTAJit_Fields_Clear_Mark_Check(unsigned char *from, unsigned char *to, delta_t *pFields) {
	CDeltaJit* deltaJit = DELTAJit_LookupDeltaJit(__FUNCTION__, pFields);
	CDeltaClearMarkFieldsJIT &func = *deltaJit->cleanMarkCheckFunc;
	return func(from, to, deltaJit);
}

void DELTAJit_SetSendFlagBits(delta_t *pFields, int *bits, int *bytecount) {
	CDeltaJit* deltaJit = DELTAJit_LookupDeltaJit(__FUNCTION__, pFields);

	bits[0] = deltaJit->marked_fields_mask[0];
	bits[1] = deltaJit->marked_fields_mask[1];
	*bytecount = deltaJit->markedFieldsMaskSize;
}

void DELTAJit_SetFieldByIndex(struct delta_s *pFields, int fieldNumber) {
	CDeltaJit* deltaJit = DELTAJit_LookupDeltaJit(__FUNCTION__, pFields);

	if (fieldNumber > 31)
		deltaJit->marked_fields_mask[1] |= (1 << (fieldNumber & 0x1F));
	else
		deltaJit->marked_fields_mask[0] |= (1 << fieldNumber);

}

void DELTAJit_UnsetFieldByIndex(struct delta_s *pFields, int fieldNumber) {
	CDeltaJit* deltaJit = DELTAJit_LookupDeltaJit(__FUNCTION__, pFields);

	if (fieldNumber > 31)
		deltaJit->marked_fields_mask[1] &= ~(1 << (fieldNumber & 0x1F));
	else
		deltaJit->marked_fields_mask[0] &= ~(1 << fieldNumber);
}

qboolean DELTAJit_IsFieldMarked(delta_t* pFields, int fieldNumber) {
	CDeltaJit* deltaJit = DELTAJit_LookupDeltaJit(__FUNCTION__, pFields);

	if (fieldNumber > 31)
		return deltaJit->marked_fields_mask[1] & (1 << (fieldNumber & 0x1F));

	return deltaJit->marked_fields_mask[0] & (1 << fieldNumber);
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
