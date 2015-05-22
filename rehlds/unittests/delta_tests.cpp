#include "precompiled.h"
#include "cppunitlite/TestHarness.h"

#pragma pack(push, 1)
struct delta_test_struct_t {
	uint8 b_00; //0
	uint8 b_01; //1
	uint16 s_02; //2
	uint32 i_04; //3
	float f_08; //4
	float w8_0C; //5
	uint8 b_10; //6
	uint8 b_11; //7
	uint16 s_12; //8
	uint32 i_14; //9
	float f_18; //10
	float w8_1C; //11
	float wb_20; //12
	char s_24[41]; //13
	uint8 b_4D; //14
	int i_4E; //15
	uint8 b_52; //16
	char s_53[9]; //17
	uint8 b_5C; //18
	int i_5D; //19
	uint8 b_61; //20
};
#pragma pack(pop)

struct delta_res_t
{
	qboolean sendfields;
	int bits[2];
	int bytecount;
};

NOINLINE void _InitDeltaField(delta_description_t* fieldDesc, int expectedOffset, int type, const char* name, int off, short sz, int bits, float preMult, float postMult) {
	if (expectedOffset != off) {
		rehlds_syserror("%s: Expected and real offset mismatch (%d != %d)", expectedOffset, off);
	}
	
	fieldDesc->fieldType = type;
	strcpy(fieldDesc->fieldName, name);
	fieldDesc->fieldOffset = off;
	fieldDesc->fieldSize = sz;
	fieldDesc->significant_bits = bits;
	fieldDesc->premultiply = preMult;
	fieldDesc->postmultiply = postMult;
	fieldDesc->flags = 0;
	memset(&fieldDesc->stats, 0, sizeof(fieldDesc->stats));
}

NOINLINE void _FillTestDelta(delta_test_struct_t* data, char val) {
	memset(data, val, sizeof(delta_test_struct_t));
	data->s_24[ARRAYSIZE(data->s_24) - 1] = 0;
	data->s_53[ARRAYSIZE(data->s_53) - 1] = 0;
	data->w8_0C = (float)val;
	data->w8_1C = (float)val;
	data->wb_20 = (float)val;
}

NOINLINE qboolean _DoMarkFields(void* src, void* dst, delta_t* delta, bool useJit) {
	qboolean sendfields;
	if (useJit) {
		DELTA_ClearFlags(delta);
		return DELTAJit_Fields_Clear_Mark_Check((unsigned char*)src, (unsigned char*)dst, delta);
	} else {
		DELTA_ClearFlags(delta);
		DELTA_MarkSendFields((unsigned char*)src, (unsigned char*)dst, delta);
		sendfields = DELTA_CountSendFields(delta);
	}

	return sendfields;
}

NOINLINE bool _CheckFieldMarked(delta_t* delta, int fieldIdx, int usejit) {
	if (usejit) {
		return DELTAJit_IsFieldMarked(delta, fieldIdx) != 0;
	} else {
		return delta->pdd[fieldIdx].flags & FDT_MARK;
	}
}

NOINLINE void _EnsureFieldsChanged(const char* callsite, const char* action, int usejit, bool changed, delta_t* delta, ...) {
	va_list vargs;
	va_start(vargs, delta);
	const char* fieldName = va_arg(vargs, const char*);
	while (fieldName) {
		delta_description_t* field = NULL;
		int idx = 0;
		for (; idx < delta->fieldCount; idx++) {
			if (!strcmp(fieldName, delta->pdd[idx].fieldName)) {
				field = &delta->pdd[idx];
				break;
			}
		}

		if (field == NULL) {
			rehlds_syserror("%s: %s: Could not find delta field '%s'", callsite, action, fieldName);
		}

		if (field->flags & 0x80) {
			rehlds_syserror("%s: %s: Field '%s' is marked as processed", callsite, action, fieldName);
		}


		if (_CheckFieldMarked(delta, idx, usejit) ^ changed) {
			rehlds_syserror("%s: %s: Field '%s' is expected to be marked", callsite, action, fieldName);
		}

		field->flags |= 0x80;

		fieldName = va_arg(vargs, const char*);
	}

	for (int i = 0; i < delta->fieldCount; i++) {
		delta_description_t* field = &delta->pdd[i];
		if (field->flags & 0x80)
			continue;

		if (_CheckFieldMarked(delta, i, usejit) ^ !changed) {
			rehlds_syserror("%s: %s: Field '%s' is expected to be unmarked", callsite, action, field->fieldName);
		}

	}
}

NOINLINE void _GetBitmaskAndBytecount(delta_t* delta, int* bits, int* bytecount, int usejit) {
	if (usejit) {
		DELTAJit_SetSendFlagBits(delta, bits, bytecount);
	}
	else {
		DELTA_SetSendFlagBits(delta, bits, bytecount);
	}
}

NOINLINE void _CompareDeltaResults(const char* callsite, delta_res_t* def, delta_res_t* jit, int testscount)
{
	for (int i = 0; i < testscount; i++)
	{
		if (!!def[i].sendfields != !!jit[i].sendfields)
			rehlds_syserror("%s: Test %i: !!sendfields not equals %i|%i", callsite, i, !!def[i].sendfields, !!jit[i].sendfields);
		if (memcmp(def[i].bits, jit[i].bits, 8))
			rehlds_syserror("%s: Test %i: bits not equals %p.%p|%p.%p", callsite, i, def[i].bits[0], def[i].bits[1], jit[i].bits[0], jit[i].bits[1]);
		if (def[i].bytecount != jit[i].bytecount)
			rehlds_syserror("%s: Test %i: bytecount not equal %i|%i", callsite, i, def[i].bytecount, jit[i].bytecount);
	}
}

NOINLINE delta_t* _CreateTestDeltaDesc() {
	static delta_description_t _fields[32];
	delta_test_struct_t d; d; // "use" d variable

	_InitDeltaField(&_fields[0], 0x00, DT_BYTE, "b_00", offsetof(delta_test_struct_t, b_00), 1, 8, 1.0f, 1.0f);
	_InitDeltaField(&_fields[1], 0x01, DT_BYTE, "b_01", offsetof(delta_test_struct_t, b_01), 1, 8, 1.0f, 1.0f);
	_InitDeltaField(&_fields[2], 0x02, DT_SHORT, "s_02", offsetof(delta_test_struct_t, s_02), 2, 16, 1.0f, 1.0f);
	_InitDeltaField(&_fields[3], 0x04, DT_INTEGER, "i_04", offsetof(delta_test_struct_t, i_04), 4, 32, 1.0f, 1.0f);
	_InitDeltaField(&_fields[4], 0x08, DT_FLOAT, "f_08", offsetof(delta_test_struct_t, f_08), 4, 32, 1.0f, 1.0f);
	_InitDeltaField(&_fields[5], 0x0C, DT_TIMEWINDOW_8, "w8_0C", offsetof(delta_test_struct_t, w8_0C), 4, 32, 1.0f, 1.0f);

	_InitDeltaField(&_fields[6], 0x10, DT_BYTE, "b_10", offsetof(delta_test_struct_t, b_10), 1, 8, 1.0f, 1.0f);
	_InitDeltaField(&_fields[7], 0x11, DT_BYTE, "b_11", offsetof(delta_test_struct_t, b_11), 1, 8, 1.0f, 1.0f);
	_InitDeltaField(&_fields[8], 0x12, DT_SHORT, "s_12", offsetof(delta_test_struct_t, s_12), 2, 16, 1.0f, 1.0f);
	_InitDeltaField(&_fields[9], 0x14, DT_INTEGER, "i_14", offsetof(delta_test_struct_t, i_14), 4, 32, 1.0f, 1.0f);
	_InitDeltaField(&_fields[10], 0x18, DT_FLOAT, "f_18", offsetof(delta_test_struct_t, f_18), 4, 32, 1.0f, 1.0f);
	_InitDeltaField(&_fields[11], 0x1C, DT_TIMEWINDOW_8, "w8_1C", offsetof(delta_test_struct_t, w8_1C), 4, 32, 1.0f, 1.0f);

	_InitDeltaField(&_fields[12], 0x20, DT_TIMEWINDOW_BIG, "wb_20", offsetof(delta_test_struct_t, wb_20), 4, 32, 1.0f, 1.0f);
	_InitDeltaField(&_fields[13], 0x24, DT_STRING, "s_24", offsetof(delta_test_struct_t, s_24), ARRAYSIZE(d.s_24), 0, 1.0f, 1.0f);

	_InitDeltaField(&_fields[14], 0x4D, DT_BYTE, "b_4D", offsetof(delta_test_struct_t, b_4D), 1, 8, 1.0f, 1.0f);
	_InitDeltaField(&_fields[15], 0x4E, DT_INTEGER, "i_4E", offsetof(delta_test_struct_t, i_4E), 4, 32, 1.0f, 1.0f);
	_InitDeltaField(&_fields[16], 0x52, DT_BYTE, "b_52", offsetof(delta_test_struct_t, b_52), 1, 8, 1.0f, 1.0f);
	_InitDeltaField(&_fields[17], 0x53, DT_STRING, "s_53", offsetof(delta_test_struct_t, s_53), ARRAYSIZE(d.s_53), 0, 1.0f, 1.0f);
	_InitDeltaField(&_fields[18], 0x5C, DT_BYTE, "b_5C", offsetof(delta_test_struct_t, b_5C), 1, 8, 1.0f, 1.0f);
	_InitDeltaField(&_fields[19], 0x5D, DT_INTEGER, "i_5D", offsetof(delta_test_struct_t, i_5D), 4, 32, 1.0f, 1.0f);
	_InitDeltaField(&_fields[20], 0x61, DT_BYTE, "b_61", offsetof(delta_test_struct_t, b_61), 1, 8, 1.0f, 1.0f);

	delta_t* delta = (delta_t*) Mem_ZeroMalloc(sizeof(delta_t));
	delta->dynamic = false;
	delta->fieldCount = 21;
	delta->pdd = &_fields[0];

	delta_info_t* dinfo = (delta_info_t*)Mem_ZeroMalloc(sizeof(delta_info_t));
	dinfo->delta = delta;
	dinfo->loadfile = Mem_Strdup("__fake_delta_test_struct_t");
	dinfo->name = Mem_Strdup("delta_test_struct_t");

	dinfo->next = g_sv_delta;
	g_sv_delta = dinfo;

	g_DeltaJitRegistry.CreateAndRegisterDeltaJIT(delta);

	return delta;
};

TEST(MarkFieldsTest_Simple_Primitives, Delta, 1000) {

	delta_t* delta = _CreateTestDeltaDesc();

	delta_test_struct_t d1;
	delta_test_struct_t d2;
	delta_res_t res[2][10];

	memset(res[0], 0xCC, sizeof(res) / 2);
	memset(res[1], 0xDD, sizeof(res) / 2);

	for (int usejit = 0; usejit <= 1; usejit++) {

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		res[usejit][0].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][0].bits, &res[usejit][0].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "SimpleUnchangedAll", usejit, true, delta, NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x72);
		res[usejit][1].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][1].bits, &res[usejit][1].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "SimpleUnchangedAll", usejit, false, delta, NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.b_01 = 0;
		res[usejit][2].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][2].bits, &res[usejit][2].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Byte_0BlockCheck", usejit, true, delta, "b_01", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.b_11 = 0;
		res[usejit][3].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][3].bits, &res[usejit][3].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Byte_1BlockCheck", usejit, true, delta, "b_11", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.s_02 = 0;
		res[usejit][4].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][4].bits, &res[usejit][4].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Short_0BlockCheck", usejit, true, delta, "s_02", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.s_12 = 0;
		res[usejit][5].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][5].bits, &res[usejit][5].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Short_1BlockCheck", usejit, true, delta, "s_12", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.i_04 = 0;
		res[usejit][6].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][6].bits, &res[usejit][6].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Int_0BlockCheck", usejit, true, delta, "i_04", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.i_14 = 0;
		res[usejit][7].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][7].bits, &res[usejit][7].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Int_1BlockCheck", usejit, true, delta, "i_14", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.f_08 = 0;
		res[usejit][8].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][8].bits, &res[usejit][8].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Float_0BlockCheck", usejit, true, delta, "f_08", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.f_18 = 0;
		res[usejit][9].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][9].bits, &res[usejit][9].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Float_0BlockCheck", usejit, true, delta, "f_18", NULL);
	}

	_CompareDeltaResults(__FUNCTION__, res[0], res[1], 10);
	SV_Shutdown();
}

TEST(MarkFieldsTest_InterBlock, Delta, 1000) {

	delta_t* delta = _CreateTestDeltaDesc();

	delta_test_struct_t d1;
	delta_test_struct_t d2;
	delta_res_t res[2][14];

	memset(res[0], 0xCC, sizeof(res) / 2);
	memset(res[1], 0xDD, sizeof(res) / 2);

	for (int usejit = 0; usejit <= 1; usejit++) {
		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.b_4D = d2.b_52 = 0;
		res[usejit][0].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][0].bits, &res[usejit][0].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock1_guards", usejit, true, delta, "b_4D", "b_52", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.b_4D = d2.b_52 = 0;
		d2.i_4E = 0;
		res[usejit][1].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][1].bits, &res[usejit][1].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock1_guards_and_val", usejit, true, delta, "b_4D", "b_52", "i_4E", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.i_4E = 0;
		res[usejit][2].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][2].bits, &res[usejit][2].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock1_val", usejit, true, delta, "i_4E", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.i_4E &= 0xFFFFFF00;
		res[usejit][3].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][3].bits, &res[usejit][3].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock1_val_0b", usejit, true, delta, "i_4E", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.i_4E &= 0xFFFF00FF;
		res[usejit][4].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][4].bits, &res[usejit][4].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock1_val_1b", usejit, true, delta, "i_4E", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.i_4E &= 0xFF00FFFF;
		res[usejit][5].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][5].bits, &res[usejit][5].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock1_val_2b", usejit, true, delta, "i_4E", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.i_4E &= 0x00FFFFFF;
		res[usejit][6].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][6].bits, &res[usejit][6].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock1_val_3b", usejit, true, delta, "i_4E", NULL);


		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.b_5C = d2.b_61 = 0;
		res[usejit][7].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][7].bits, &res[usejit][7].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock2_guards", usejit, true, delta, "b_5C", "b_61", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.b_5C = d2.b_61 = 0;
		d2.i_5D = 0;
		res[usejit][8].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][8].bits, &res[usejit][8].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock2_guards_and_val", usejit, true, delta, "b_5C", "b_61", "i_5D", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.i_5D = 0;
		res[usejit][9].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][9].bits, &res[usejit][9].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock2_val", usejit, true, delta, "i_5D", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.i_5D &= 0xFFFFFF00;
		res[usejit][10].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][10].bits, &res[usejit][10].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock2_val_0b", usejit, true, delta, "i_5D", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.i_5D &= 0xFFFF00FF;
		res[usejit][11].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][11].bits, &res[usejit][11].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock2_val_1b", usejit, true, delta, "i_5D", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.i_5D &= 0xFF00FFFF;
		res[usejit][12].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][12].bits, &res[usejit][12].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock2_val_2b", usejit, true, delta, "i_5D", NULL);

		_FillTestDelta(&d1, 0x71); _FillTestDelta(&d2, 0x71);
		d2.i_5D &= 0x00FFFFFF;
		res[usejit][13].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][13].bits, &res[usejit][13].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Interblock2_val_3b", usejit, true, delta, "i_5D", NULL);
	}

	_CompareDeltaResults(__FUNCTION__, res[0], res[1], 14);
	SV_Shutdown();
}

TEST(MarkFieldsTest_Strings, Delta, 1000) {

	delta_t* delta = _CreateTestDeltaDesc();

	delta_test_struct_t d1;
	delta_test_struct_t d2;
	delta_res_t res[2][4];

	memset(res[0], 0xCC, sizeof(res) / 2);
	memset(res[1], 0xDD, sizeof(res) / 2);

	for (int usejit = 0; usejit <= 1; usejit++) {

		_FillTestDelta(&d1, 'c'); _FillTestDelta(&d2, 'c');
		d2.s_24[0] = 0;
		res[usejit][0].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][0].bits, &res[usejit][0].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Str_empty", usejit, true, delta, "s_24", NULL);

		_FillTestDelta(&d1, 'c'); _FillTestDelta(&d2, 'c');
		d1.s_24[0] = 0;
		res[usejit][1].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][1].bits, &res[usejit][1].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Str_empty2", usejit, true, delta, "s_24", NULL);

		_FillTestDelta(&d1, 'c'); _FillTestDelta(&d2, 'c');
		d1.s_24[0] = d2.s_24[0] = 0;
		d1.s_24[1] = 'd'; d2.s_24[1] = 'e';
		res[usejit][2].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][2].bits, &res[usejit][2].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Str_both_empty_with_garbage", usejit, true, delta, NULL);

		_FillTestDelta(&d1, 'c'); _FillTestDelta(&d2, 'c');
		d1.s_24[1] = 'C';
		res[usejit][3].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][3].bits, &res[usejit][3].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "Str_case_check", usejit, true, delta, NULL);
	}

	_CompareDeltaResults(__FUNCTION__, res[0], res[1], 4);
	SV_Shutdown();
}

TEST(MarkFieldsTest_TimeWindow, Delta, 1000) {
#ifdef REHLDS_FIXES
	bool rehds_fixes = true;
#else
	bool rehds_fixes = false;
#endif

	delta_t* delta = _CreateTestDeltaDesc();

	delta_test_struct_t d1;
	delta_test_struct_t d2;
	delta_res_t res[2][3];

	memset(res[0], 0xCC, sizeof(res) / 2);
	memset(res[1], 0xDD, sizeof(res) / 2);

	for (int usejit = 0; usejit <= 1; usejit++) {

		_FillTestDelta(&d1, 'c'); _FillTestDelta(&d2, 'c');
		d2.w8_1C = 0.001f; d1.w8_1C = 0.0011f;
		res[usejit][0].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][0].bits, &res[usejit][0].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "TimeWindow_Below_Precision", usejit, true, delta, rehds_fixes ? "w8_1C" : NULL, NULL);

		_FillTestDelta(&d1, 'c'); _FillTestDelta(&d2, 'c');
		d2.w8_1C = 0.1f; d1.w8_1C = 0.11f; //precision check, 0.11f is actually 0.10999
		res[usejit][1].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][1].bits, &res[usejit][1].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "TimeWindow_Above_Precision1", usejit, true, delta, rehds_fixes ? "w8_1C" : NULL, NULL);

		_FillTestDelta(&d1, 'c'); _FillTestDelta(&d2, 'c');
		d2.w8_1C = 0.1f; d1.w8_1C = 0.12f;
		res[usejit][2].sendfields = _DoMarkFields(&d1, &d2, delta, usejit != 0);
		_GetBitmaskAndBytecount(delta, res[usejit][2].bits, &res[usejit][2].bytecount, usejit);
		_EnsureFieldsChanged(__FUNCTION__, "TimeWindow_Above_Precision2", usejit, true, delta, "w8_1C", NULL);
	}

	_CompareDeltaResults(__FUNCTION__, res[0], res[1], 3);
	SV_Shutdown();
}
