#pragma once

#include "maintypes.h"

#define DELTAJIT_MAX_BLOCKS 32
#define DELTAJIT_MAX_FIELDS 56

struct deltajit_field {
	unsigned int id;
	unsigned int offset;
	unsigned int length;
	int type;
	unsigned int numBlocks;
};

struct deltajit_memblock_field {
	deltajit_field* field;
	uint16 mask;
	bool first;
	bool last;
};

struct deltajit_memblock {
	unsigned int numFields;
	deltajit_memblock_field fields[24];
};

struct deltajitdata_t {
	unsigned int numblocks;
	deltajit_memblock blocks[DELTAJIT_MAX_BLOCKS];

	unsigned int numFields;
	deltajit_field fields[DELTAJIT_MAX_FIELDS];
};

enum deltajit_marked_count_type_t {
	DJ_M_DONT_COUNT,
	DJ_M_CHECK,
	//DJ_M_COUNT, //not implemented yet
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

extern void DELTAJit_ClearAndMarkSendFields(unsigned char *from, unsigned char *to, delta_t *pFields, int* bits, int* bytecount, int force);
extern int DELTAJit_Feilds_Clear_Mark_Check(unsigned char *from, unsigned char *to, delta_t *pFields);
