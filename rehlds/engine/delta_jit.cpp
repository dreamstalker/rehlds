/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#include "precompiled.h"
#include "jitasm.h"

#ifdef REHLDS_JIT
CDeltaJitRegistry g_DeltaJitRegistry;

uint32 DELTAJIT_CreateMask(int startBit, int endBit) {
	if (startBit < 0) startBit = 0;
	if (endBit < 0) endBit = 0;
	if (startBit > 32) startBit = 32;
	if (endBit > 32) endBit = 32;

	uint32 res = 0xFFFFFFFF;
	res &= startBit < 32 ? (0xFFFFFFFF << startBit) : 0;
	res &= endBit > 0 ? (0xFFFFFFFF >> (32 - endBit)) : 0;
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
		return 0;

	default:
		Sys_Error("%s: Unknown delta field type %d", __func__, desc->fieldType);
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

	// sanity checks & pre-clean
	if (numMemBlocks > DELTAJIT_MAX_BLOCKS) {
		Sys_Error("%s: numMemBlocks > DELTAJIT_MAX_BLOCKS (%d > %d)", __func__, numMemBlocks, DELTAJIT_MAX_BLOCKS);
	}

	if (delta->fieldCount > DELTAJIT_MAX_FIELDS) {
		Sys_Error("%s: fieldCount > DELTAJIT_MAX_FIELDS (%d > %d)", __func__, delta->fieldCount, DELTAJIT_MAX_FIELDS);
	}

	Q_memset(&jitdesc, 0, sizeof(jitdesc));
	jitdesc.numblocks = numMemBlocks;
	jitdesc.numFields = delta->fieldCount;

	//create descriptions
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
		jitField->significantBits = fieldDesc->significant_bits;

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

			if (blockField) {
				blockField->last = true;
			}
		}
	}

	//calculate blocks that we should check
	for (unsigned int i = 0; i < jitdesc.numblocks; i++) {
		if (jitdesc.blocks[i].numFields > 0) {
			jitdesc.itrBlocks[jitdesc.numItrBlocks].memblockId = i;
			jitdesc.itrBlocks[jitdesc.numItrBlocks].memblock = &jitdesc.blocks[i];
			jitdesc.itrBlocks[jitdesc.numItrBlocks].prefetchBlockId = -1;
			jitdesc.numItrBlocks++;
		}
	}

	//decide which blocks we should prefetch
	for (unsigned int i = 0; i < jitdesc.numItrBlocks; i++) {
		unsigned int prefetchBlkId = (i + 1) * 4;
		if (prefetchBlkId >= jitdesc.numblocks)
			break;

		jitdesc.itrBlocks[i].prefetchBlockId = prefetchBlkId;
	}
}

class CUniqueLabel {
public:
	CUniqueLabel(const char* name) : m_name(name) {
		m_name += std::to_string(m_unique_index++);
	}

	operator std::string&() {
		return m_name;
	}

private:
	std::string m_name;
	static size_t m_unique_index;
};
size_t CUniqueLabel::m_unique_index;

class CDeltaClearMarkFieldsJIT;
class CDeltaTestDeltaJIT;

class CDeltaJit {
public:
	CDeltaClearMarkFieldsJIT* cleanMarkCheckFunc;
	CDeltaTestDeltaJIT* testDeltaFunc;
	delta_t* delta;
	delta_marked_mask_t markedFieldsMask;
	delta_marked_mask_t originalMarkedFieldsMask; //mask based on data, before calling the conditional encoder
	int markedFieldsMaskSize;

	CDeltaJit(delta_t* _delta, CDeltaClearMarkFieldsJIT* _cleanMarkCheckFunc, CDeltaTestDeltaJIT* _testDeltaFunc);

	virtual ~CDeltaJit();
};

class CDeltaCheckJIT : public jitasm::function<void, CDeltaCheckJIT>
{
public:
	void main() {}
	virtual void onFieldChecked(deltajit_field* jitField) {}
	virtual void onStringChecked(deltajit_field* jitField) {}

	void checkFieldMask(jitasm::Frontend::Reg32& mask, deltajit_memblock_field* jitField);
	void iterateBlocks(deltajitdata_t *jitdesc);
	void iterateStrings(deltajitdata_t *jitdesc);

private:
	jitasm::XmmReg xmm_tmp = xmm6;
};

void CDeltaCheckJIT::checkFieldMask(jitasm::Frontend::Reg32& mask, deltajit_memblock_field* jitField)
{
	test(mask, jitField->mask);
	setnz(al);
	movzx(edx, al);
}

void CDeltaCheckJIT::iterateBlocks(deltajitdata_t *jitdesc)
{
#ifndef REHLDS_FIXES
	sub(esp, 12); //some local storage is required for precise DT_TIMEWINDOW marking
#endif

	/*
		Registers usage:
			esi = src
			edi = dst
			ecx = blockMask
			xmm0-xmm2: loaded src
			xmm3-xmm5: loaded dst
			xmm6: temp
		Already initialized:
			esi, edi
	*/

	int dataXmmCounter = 0; // from 0 to 2 => 3 pairs of registers
	jitasm::Frontend::XmmReg src_xmm[3] = {xmm0, xmm1, xmm2};
	jitasm::Frontend::XmmReg dst_xmm[3] = {xmm3, xmm4, xmm5};

	if (jitdesc->numItrBlocks > 0) {
		movdqu(src_xmm[0], xmmword_ptr[esi + ( jitdesc->itrBlocks[0].memblockId * 16 )]);
		movdqu(dst_xmm[0], xmmword_ptr[edi + ( jitdesc->itrBlocks[0].memblockId * 16 )]);
	}
	if (jitdesc->numItrBlocks > 1) {
		movdqu(src_xmm[1], xmmword_ptr[esi + ( jitdesc->itrBlocks[1].memblockId * 16 )]);
		movdqu(dst_xmm[1], xmmword_ptr[edi + ( jitdesc->itrBlocks[1].memblockId * 16 )]);
	}
	if (jitdesc->numItrBlocks > 2) {
		movdqu(src_xmm[2], xmmword_ptr[esi + ( jitdesc->itrBlocks[2].memblockId * 16 )]);
		movdqu(dst_xmm[2], xmmword_ptr[edi + ( jitdesc->itrBlocks[2].memblockId * 16 )]);
	}

	auto blockMask = ecx;
	xor_(blockMask, blockMask);

	for (unsigned int i = 0; i < jitdesc->numItrBlocks; i++) {
		auto block = jitdesc->itrBlocks[i].memblock;
		auto itrBlock = &jitdesc->itrBlocks[i];

		//do far prefetch
		if (itrBlock->prefetchBlockId != -1) {
			prefetcht0(byte_ptr[esi + ( itrBlock->prefetchBlockId * 16 )]);
			prefetcht0(byte_ptr[edi + ( itrBlock->prefetchBlockId * 16 )]);
		}

		// create mask for changed bytes
		pcmpeqb(src_xmm[dataXmmCounter], dst_xmm[dataXmmCounter]); // 0..15: byte[s] = (byte[s] == byte[d]) ? 0xFF : 0x00
		pmovmskb(blockMask, src_xmm[dataXmmCounter]);
		not_(blockMask);

		//preload next blocks
		if (i + 3 < jitdesc->numItrBlocks) {
			movdqu(src_xmm[dataXmmCounter], xmmword_ptr[esi + ( jitdesc->itrBlocks[i + 3].memblockId * 16 )]);
			movdqu(dst_xmm[dataXmmCounter], xmmword_ptr[edi + ( jitdesc->itrBlocks[i + 3].memblockId * 16 )]);
		}

		dataXmmCounter++;
		if (dataXmmCounter > 2) {
			dataXmmCounter -= 3;
		}

		// iterate fields contained in block
		for (unsigned int j = 0; j < block->numFields; j++) {
			auto jitField = &block->fields[j];

#ifndef REHLDS_FIXES
			// precise floats comparison
			if (jitField->field->type == DT_TIMEWINDOW_8 || jitField->field->type == DT_TIMEWINDOW_BIG) {
				if (jitField->last) {

					float multiplier = ( jitField->field->type == DT_TIMEWINDOW_8 ) ? 100.0f : 1000.0f;
					uint32 m32 = *(uint32*)( &multiplier );
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
				checkFieldMask(blockMask, jitField);
			}
#else
			checkFieldMask(blockMask, jitField);
#endif
			// call handler
			onFieldChecked(jitField->field);
		}
	}

#ifndef REHLDS_FIXES
	add(esp, 12); //some local storage is required for precise DT_TIMEWINDOW marking
#endif // REHLDS_FIXES
}

void CDeltaCheckJIT::iterateStrings(deltajitdata_t *jitdesc)
{
	// This generator expects that following registers are already initialized:
	// esi = src
	// edi = dst
	size_t pushed_size = 0;

	for (unsigned int i = 0; i < jitdesc->numFields; i++) {
		auto jitField = &jitdesc->fields[i];
		if (jitField->type != DT_STRING)
			continue;

		// will be parallel
		lea(eax, ptr[esi + jitField->offset]);
		lea(edx, ptr[edi + jitField->offset]);

		mov(ecx, (size_t)&Q_stricmp);
		push(eax);
		push(edx);
		call(ecx);

		// call handler
		onStringChecked(jitField);

		pushed_size += sizeof(int) * 2;
	}

	if (pushed_size)
		add(esp, pushed_size);
}

class CDeltaClearMarkFieldsJIT : public jitasm::function<int, CDeltaClearMarkFieldsJIT, void*, void*, void*, void*>
{
public:
	CDeltaClearMarkFieldsJIT(deltajitdata_t *_jitdesc);

	Result main(Addr src, Addr dst, Addr deltaJit, Addr pForceMarkMask);

	void callConditionalEncoder(Addr src, Addr dst, Addr deltaJit);
	void calculateBitcount();

	// first two virtual functions must be same as in CDeltaCheckJIT
	virtual void onFieldChecked(deltajit_field* jitField);
	virtual void onStringChecked(deltajit_field* jitField);

	deltajitdata_t *jitdesc;

private:
	jitasm::XmmReg xmm_tmp = xmm6;
	jitasm::XmmReg marked_fields_mask = xmm7;
};

CDeltaClearMarkFieldsJIT::CDeltaClearMarkFieldsJIT(deltajitdata_t *_jitdesc) : jitdesc(_jitdesc)
{
}

void CDeltaClearMarkFieldsJIT::callConditionalEncoder(Addr src, Addr dst, Addr deltaJit) {
	// This generator expects that following registers are already initialized:
	// esi = src
	// edi = dst

	CUniqueLabel no_encoder("no_encoder");
	int deltaOffset = (offsetof(CDeltaJit, delta));
	int condEncoderOffset = (offsetof(delta_t, conditionalencode));
	mov(eax, ptr[deltaJit]);
	mov(eax, ptr[eax + deltaOffset]);
	mov(ecx, dword_ptr[eax + condEncoderOffset]);

	jecxz(no_encoder);
		push(edi);
		push(esi);
		push(eax);
		
		call(ecx);
		add(esp, sizeof(int) * 3);
	L(no_encoder);
}

void CDeltaClearMarkFieldsJIT::calculateBitcount() {
	// This generator expects that following registers are already initialized:
	// ebx = delta

	size_t delta_markbits_offset = offsetof(CDeltaJit, markedFieldsMask);
	mov(eax, dword_ptr[ebx + delta_markbits_offset]);

	// bits 0-7
	test(al, al);
	setnz(dl);

	for (size_t i = 1; i < DELTAJIT_MAX_FIELDS / CHAR_BIT; i++) {
		const int byteid = i & 3;

		if (byteid == 0)
			mov(eax, dword_ptr[ebx + delta_markbits_offset + i]);

		auto reg = (i & 1) ? ecx : esi;
		auto prev = (i & 1) ? esi : ecx;

		if (jitdesc->numFields > i * CHAR_BIT - 1) {
			mov(reg, i + 1);
			if(i != 1) cmovnz(edx, prev);

			switch (byteid) {
			case 0: test(al, al); break;
			case 1: test(ah, ah); break;
			default: test(eax, 0xFF << (byteid * CHAR_BIT));
			}
		}
		else if (jitdesc->numFields > unsigned(i * CHAR_BIT - 9)) {
			cmovnz(edx, prev);
			break;
		}
	}
	if (jitdesc->numFields > DELTAJIT_MAX_FIELDS - 9) {
		cmovnz(edx, (DELTAJIT_MAX_FIELDS / CHAR_BIT & 1) ? esi : ecx);
	}

	// return bitcount in edx
}

void CDeltaClearMarkFieldsJIT::onFieldChecked(deltajit_field* field)
{
	// This generator expects that following registers are already initialized:
	// edx = is field changed

	movd(xmm_tmp, edx); // set bit in send mask
	psllq(xmm_tmp, field->id); // shift left
	por(marked_fields_mask, xmm_tmp);
}

void CDeltaClearMarkFieldsJIT::onStringChecked(deltajit_field* field)
{
	// This generator expects that following registers are already initialized:
	// eax = result of strings comparison
	// ebx = deltaJit

	size_t delta_markbits_offset = offsetof(CDeltaJit, markedFieldsMask);

	test(eax, eax);
	setnz(cl);

	shl(cl, field->id & (CHAR_BIT - 1));
	or_(byte_ptr[ebx + delta_markbits_offset + (field->id / CHAR_BIT)], cl);
}

CDeltaClearMarkFieldsJIT::Result CDeltaClearMarkFieldsJIT::main(Addr src, Addr dst, Addr deltaJit, Addr pForceMarkMask)
{
	/*
		Registers usage:
			esi = src
			edi = dst
			xmm0-xmm2: loaded src
			xmm3-xmm5: loaded dst
			xmm6: temp
			xmm7: marked fields mask	
	*/
	mov(esi, ptr[src]);
	mov(edi, ptr[dst]);
	
	pxor(marked_fields_mask, marked_fields_mask);

	// check changed blocks
	reinterpret_cast<CDeltaCheckJIT*>(this)->iterateBlocks(jitdesc);

	// apply 'force mark' mask if it's present
	CUniqueLabel no_forcemask("no_forcemask");
	mov(eax, ptr[pForceMarkMask]);
	test(eax, eax);
	jz(no_forcemask);
		movq(xmm_tmp, qword_ptr[eax]);
		por(marked_fields_mask, xmm_tmp);
	L(no_forcemask);

	size_t delta_markbits_offset = offsetof(CDeltaJit, markedFieldsMask);

	// save mask from SSE register to CDeltaJit::markedFieldsMask because it can be overwritten in Q_stricmp
	mov(ebx, ptr[deltaJit]);
	movq(qword_ptr[ebx + delta_markbits_offset], marked_fields_mask);

	// check changed strings
	reinterpret_cast<CDeltaCheckJIT*>(this)->iterateStrings(jitdesc);

	// save originalMarkedFieldsMask before calling conditional encoder
	movq(marked_fields_mask, qword_ptr[ebx + delta_markbits_offset]);
	movq(qword_ptr[ebx + offsetof(CDeltaJit, originalMarkedFieldsMask)], marked_fields_mask);

	// emit conditional encoder call
	callConditionalEncoder(src, dst, deltaJit);
	
	// check if mask is empty
	mov(edi, dword_ptr[ebx + delta_markbits_offset]);
	xor_(edx, edx); // set size to 0
	or_(edi, dword_ptr[ebx + delta_markbits_offset + 4]);

	CUniqueLabel no_markedbits("no_markedbits");

	// test(edi, edi); // flags are already set
	jz(no_markedbits);
		calculateBitcount();
	L(no_markedbits);

	mov(dword_ptr[ebx + offsetof(CDeltaJit, markedFieldsMaskSize)], edx);
	return edx;
}

class CDeltaTestDeltaJIT : public jitasm::function<int, CDeltaTestDeltaJIT, void*, void*, void*>
{
public:
	CDeltaTestDeltaJIT(deltajitdata_t *_jitdesc);

	Result main(Addr src, Addr dst, Addr deltaJit);

	virtual void onFieldChecked(deltajit_field* jitField);
	virtual void onStringChecked(deltajit_field* jitField);

	deltajitdata_t *jitdesc;

private:
	jitasm::Reg32 neededBits = ebx;
	jitasm::Reg32 highestBit = ebp;
	size_t highest_id = 0;
};

CDeltaTestDeltaJIT::CDeltaTestDeltaJIT(deltajitdata_t *_jitdesc) : jitdesc(_jitdesc)
{
}

void CDeltaTestDeltaJIT::onFieldChecked(deltajit_field* field)
{
	mov(eax, field->id);

	if (field->id >= highest_id)
	{
		// setted highestBit can't be > field->id
		highest_id = field->id;
	}
	else
	{
		// select greatest from field->id and highestBit
		cmp(eax, highestBit);
		cmovl(eax, highestBit); // eax = id < highestBit ? highestBit : id
	}

	// edx = 1 if field changed, otherwise 0
	neg(edx);
	cmovnz(highestBit, eax);
	and_(edx, field->significantBits);
	add(neededBits, edx);
}

void CDeltaTestDeltaJIT::onStringChecked(deltajit_field* field)
{
	// This generator expects that following registers are already initialized:
	// eax = result of strings comparison
	// ebp = highestBit
	// [esp] = dest string

	CUniqueLabel not_changed("not_changed");

	test(eax, eax);
	jz(not_changed); // changed
		mov(ecx, (size_t)&Q_strlen);
		call(ecx); // dest already in top of stack

		lea(neededBits, ptr[neededBits + eax * 8 + 8]); // add size of string in bits + EOS byte

		mov(eax, field->id);
		cmp(eax, highestBit);
		cmovg(highestBit, eax);
	L(not_changed);
}

CDeltaClearMarkFieldsJIT::Result CDeltaTestDeltaJIT::main(Addr src, Addr dst, Addr deltaJit)
{
	/*
		Registers usage:
			esi = src
			edi = dst
			ebx = neededBits;
			ebp = highestBit;
			xmm0-xmm2: loaded src
			xmm3-xmm5: loaded dst
			xmm6: temp
	*/
	mov(esi, ptr[src]);
	mov(edi, ptr[dst]);

	// neededBits 0; highestBit = -1
	xor_(highestBit, highestBit);
	xor_(neededBits, neededBits);
	dec(highestBit);

	// can save some operations
	highest_id = 0;

	// check changed fields
	reinterpret_cast<CDeltaCheckJIT*>(this)->iterateBlocks(jitdesc);

#ifdef REHLDS_FIXES
	// check changed strings
	reinterpret_cast<CDeltaCheckJIT*>(this)->iterateStrings(jitdesc);
#endif

	CUniqueLabel highest_not_set("highest_not_set");

	test(highestBit, highestBit);
	js(highest_not_set);
		//neededBits += highestBit / 8 * 8 + 8;
		shr(highestBit, 3);
		lea(neededBits, ptr[neededBits + highestBit * 8 + 8]);
	L(highest_not_set);

	return neededBits;
}

CDeltaJit::CDeltaJit(delta_t* _delta, CDeltaClearMarkFieldsJIT* _cleanMarkCheckFunc, CDeltaTestDeltaJIT* _testDeltaFunc) {
	delta = _delta;
	cleanMarkCheckFunc = _cleanMarkCheckFunc;
	testDeltaFunc = _testDeltaFunc;
	markedFieldsMaskSize = 0;
}

CDeltaJit::~CDeltaJit() {
	if (cleanMarkCheckFunc) {
		delete cleanMarkCheckFunc;
		cleanMarkCheckFunc = NULL;
	}
	if (testDeltaFunc) {
		delete testDeltaFunc;
		testDeltaFunc = NULL;
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

	CDeltaClearMarkFieldsJIT* cleanMarkCheckFunc = new CDeltaClearMarkFieldsJIT(&data);
	cleanMarkCheckFunc->Assemble();
	cleanMarkCheckFunc->jitdesc = nullptr;

	CDeltaTestDeltaJIT* testDeltaFunc = new CDeltaTestDeltaJIT(&data);
	testDeltaFunc->Assemble();
	testDeltaFunc->jitdesc = nullptr;

	// align to 16
	CDeltaJit* deltaJit = new CDeltaJit(delta, cleanMarkCheckFunc, testDeltaFunc);
	RegisterDeltaJit(delta, deltaJit);
}

CDeltaJit* DELTAJit_LookupDeltaJit(const char* callsite, delta_t *pFields) {
	CDeltaJit* deltaJit = g_DeltaJitRegistry.GetJITByDelta(pFields);

#ifndef REHLDS_FIXES
	// only for testing
	if (!deltaJit) {
		Sys_Error("%s: JITted delta encoder not found for delta %p", callsite, pFields);
	}
#endif // REHLDS_FIXES

	return deltaJit;
}

NOINLINE int DELTAJit_Fields_Clear_Mark_Check(unsigned char *from, unsigned char *to, delta_t *pFields, void* pForceMarkMask) {
	CDeltaJit* deltaJit = DELTAJit_LookupDeltaJit(__func__, pFields);
	CDeltaClearMarkFieldsJIT &func = *deltaJit->cleanMarkCheckFunc;
	return func(from, to, deltaJit, pForceMarkMask);
}

NOINLINE int DELTAJit_TestDelta(unsigned char *from, unsigned char *to, delta_t *pFields)
{
	CDeltaJit* deltaJit = DELTAJit_LookupDeltaJit(__func__, pFields);
	CDeltaTestDeltaJIT &func = *deltaJit->testDeltaFunc;
	return func(from, to, deltaJit);
}

void DELTAJit_SetSendFlagBits(delta_t *pFields, int *bits, int *bytecount) {
	CDeltaJit* deltaJit = DELTAJit_LookupDeltaJit(__func__, pFields);

	bits[0] = deltaJit->markedFieldsMask.u32[0];
	bits[1] = deltaJit->markedFieldsMask.u32[1];
	*bytecount = deltaJit->markedFieldsMaskSize;
}

void DELTAJit_SetFieldByIndex(delta_t *pFields, int fieldNumber)
{
	CDeltaJit* deltaJit = DELTAJit_LookupDeltaJit(__func__, pFields);
	deltaJit->markedFieldsMask.u32[fieldNumber >> 5] |= (1 << (fieldNumber & 31));
}

void DELTAJit_UnsetFieldByIndex(delta_t *pFields, int fieldNumber)
{
	CDeltaJit* deltaJit = DELTAJit_LookupDeltaJit(__func__, pFields);
	deltaJit->markedFieldsMask.u32[fieldNumber >> 5] &= ~(1 << (fieldNumber & 31));
}

qboolean DELTAJit_IsFieldMarked(delta_t* pFields, int fieldNumber)
{
	CDeltaJit* deltaJit = DELTAJit_LookupDeltaJit(__func__, pFields);
	return deltaJit->markedFieldsMask.u32[fieldNumber >> 5] & (1 << (fieldNumber & 31));
}

uint64 DELTAJit_GetOriginalMask(delta_t* pFields) {
	CDeltaJit* deltaJit = DELTAJit_LookupDeltaJit(__func__, pFields);
	return deltaJit->originalMarkedFieldsMask.u64;
}

uint64 DELTAJit_GetMaskU64(delta_t* pFields) {
	CDeltaJit* deltaJit = DELTAJit_LookupDeltaJit(__func__, pFields);
	return deltaJit->markedFieldsMask.u64;
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
#endif
