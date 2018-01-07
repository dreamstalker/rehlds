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

#pragma once

#include "maintypes.h"

#ifdef REHLDS_JIT
const int DELTAJIT_MAX_BLOCKS = 32;
const int DELTAJIT_MAX_FIELDS = 56;

struct deltajit_field {
	unsigned int id;
	unsigned int offset;
	unsigned int length;
	int type;
	unsigned int numBlocks;
	unsigned int significantBits;
};

struct deltajit_memblock_field {
	deltajit_field* field;
	uint16 mask; // one bit for each memblock byte
	bool first;
	bool last;
};

struct deltajit_memblock {
	unsigned int numFields;
	deltajit_memblock_field fields[24];
};

struct deltajit_memblock_itr_t {
	int memblockId;
	deltajit_memblock* memblock;
	int prefetchBlockId;
};

struct deltajitdata_t {
	unsigned int numblocks;
	deltajit_memblock blocks[DELTAJIT_MAX_BLOCKS];

	unsigned int numFields;
	deltajit_field fields[DELTAJIT_MAX_FIELDS];

	unsigned int numItrBlocks;
	deltajit_memblock_itr_t itrBlocks[DELTAJIT_MAX_BLOCKS];
};

class CDeltaJit;

class CDeltaJitRegistry {
private:
#ifndef REHLDS_FIXES
	CStaticMap<void*, CDeltaJit*, 4, 64> m_DeltaToJITMap;
#endif

public:
	CDeltaJitRegistry();
	void RegisterDeltaJit(delta_t* delta, CDeltaJit* deltaJit);
	CDeltaJit* GetJITByDelta(delta_t* delta);
	void CreateAndRegisterDeltaJIT(delta_t* delta);
	void Cleanup();
};

extern CDeltaJitRegistry g_DeltaJitRegistry;

extern int DELTAJit_Fields_Clear_Mark_Check(unsigned char *from, unsigned char *to, delta_t *pFields, void* pForceMarkMask);
extern int DELTAJit_TestDelta(unsigned char *from, unsigned char *to, delta_t *pFields);
extern void DELTAJit_SetSendFlagBits(delta_t *pFields, int *bits, int *bytecount);
extern void DELTAJit_SetFieldByIndex(delta_t *pFields, int fieldNumber);
extern void DELTAJit_UnsetFieldByIndex(delta_t *pFields, int fieldNumber);
extern qboolean DELTAJit_IsFieldMarked(delta_t* pFields, int fieldNumber);

/* Returns original mask, before it was changed by the conditional encoder */
extern uint64 DELTAJit_GetOriginalMask(delta_t* pFields);
extern uint64 DELTAJit_GetMaskU64(delta_t* pFields);
#endif
