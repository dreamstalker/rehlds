#include "precompiled.h"
#include "rehlds_tests_shared.h"
#include "cppunitlite/TestHarness.h"

#pragma pack(push, 1)
struct delta_test_struct_t {
	uint8 b_00;    // 0
	uint8 b_01;    // 1
	uint16 s_02;   // 2
	uint32 i_04;   // 3
	float f_08;    // 4
	float w8_0C;   // 5
	uint8 b_10;    // 6
	uint8 b_11;    // 7
	uint16 s_12;   // 8
	uint32 i_14;   // 9
	float f_18;    // 10
	float w8_1C;   // 11
	float wb_20;   // 12
	char s_24[41]; // 13
	uint8 b_4D;    // 14
	int i_4E;      // 15
	uint8 b_52;    // 16
	char s_53[9];  // 17
	uint8 b_5C;    // 18
	int i_5D;      // 19
	uint8 b_61;    // 20
};
#pragma pack(pop)

typedef delta_test_struct_t dts_t;

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

NOINLINE void _FillTestDelta(delta_test_struct_t* data, unsigned char val) {
	memset(data, val, sizeof(delta_test_struct_t));
	data->s_24[ARRAYSIZE(data->s_24) - 1] = 0;
	data->s_53[ARRAYSIZE(data->s_53) - 1] = 0;
	data->w8_0C = (float)val;
	data->w8_1C = (float)val;
	data->wb_20 = (float)val;
}

NOINLINE qboolean _DoMarkFields(void* src, void* dst, delta_t* delta, bool useJit) {
	qboolean sendfields;
#ifdef REHLDS_JIT
	if (useJit) {
		return DELTAJit_Fields_Clear_Mark_Check((unsigned char*)src, (unsigned char*)dst, delta, NULL);
	} else
#endif
	{
		DELTA_ClearFlags(delta);
		DELTA_MarkSendFields((unsigned char*)src, (unsigned char*)dst, delta);
		sendfields = DELTA_CountSendFields(delta);
	}

	return sendfields;
}


NOINLINE void _MarkAndEnsureCorrectResults(const char* action, delta_t* delta, void* src, void* dst, int useJit, bool changed, const char* szFields) {
	delta_description_t* fields[DELTA_MAX_FIELDS];
	int numFields = 0;

	char localFieldsStr[512];
	strcpy(localFieldsStr, szFields);

	// parse fields
	int prevEnd = -1;
	for (char* pcc = localFieldsStr; *pcc; pcc++) {
		if (*pcc == ' ') {
			*pcc = 0;
			int fIdx = DELTA_FindFieldIndex(delta, &localFieldsStr[prevEnd + 1]);
			if (fIdx == -1) {
				rehlds_syserror("%s: Coult not find field '%s'", &localFieldsStr[prevEnd + 1]);
			}
			fields[numFields++] = &delta->pdd[fIdx];
			prevEnd = pcc - localFieldsStr;
		}
	}

	if (localFieldsStr[0] != 0) {
		int fIdx = DELTA_FindFieldIndex(delta, &localFieldsStr[prevEnd + 1]);
		if (fIdx == -1) {
			rehlds_syserror("%s: Coult not find field '%s'", &localFieldsStr[prevEnd + 1]);
		}
		fields[numFields++] = &delta->pdd[fIdx];
	}

	// build expected mask
	delta_marked_mask_t expectedMask; expectedMask.u64 = 0;
	for (int i = 0; i < numFields; i++) {
		delta_description_t* f = fields[i];
		int fieldId = f - delta->pdd;
		expectedMask.u8[fieldId >> 3] |= (1 << (fieldId & 0x7));
	}

	if (!changed) {
		// invert mask
		uint64 existingFieldsMask = 0xFFFFFFFFFFFFFFFF;
		existingFieldsMask = existingFieldsMask >> (64 - delta->fieldCount);

		expectedMask.u64 = ~expectedMask.u64;
		expectedMask.u64 &= existingFieldsMask;
	}

	// calculate expected bytecount
	int expectedBytecount = 0;
	for (int i = 0; i < ARRAYSIZE(expectedMask.u8); i++) {
		if (expectedMask.u8[i]) {
			expectedBytecount = i + 1;
		}
	}

	// do marking
	qboolean markResult = _DoMarkFields(src, dst, delta, useJit != 0);

	// check marking result
	if ((markResult != 0) != (expectedMask.u64 != 0)) {
		rehlds_syserror("%s: DoMarkFields returned invalid value %d, expected %s", action, markResult, (expectedMask.u64 == 0) ? "0" : "!0");
	}

	delta_marked_mask_t returnedMask;
	int returnedBytecount;
#ifdef REHLDS_JIT
	if (useJit) {
		DELTAJit_SetSendFlagBits(delta, (int*)returnedMask.u32, &returnedBytecount);
	} else
#endif
	{
		DELTA_SetSendFlagBits(delta, (int*)returnedMask.u32, &returnedBytecount);
	}

	// check per-field marks
	if (returnedMask.u64 != expectedMask.u64) {
		rehlds_syserror("%s: DoMarkFields returned invalid mask %llX, expected %llX", action, returnedMask.u64, expectedMask.u64);
	}

	// check bytecount
	if (returnedBytecount != expectedBytecount) {
		rehlds_syserror("%s: DoMarkFields returned invalid bytecount %d, expected %d", action, returnedBytecount, expectedBytecount);
	}
}

NOINLINE void _GetBitmaskAndBytecount(delta_t* delta, int* bits, int* bytecount, int usejit) {
#ifdef REHLDS_JIT
	if (usejit) {
		DELTAJit_SetSendFlagBits(delta, bits, bytecount);
	}
	else
#endif
	{
		DELTA_SetSendFlagBits(delta, bits, bytecount);
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

#ifdef REHLDS_JIT
	g_DeltaJitRegistry.CreateAndRegisterDeltaJIT(delta);
#endif
	return delta;
};

struct delta_simpletest_data_t {
	const char* action;
	int fill1, fill2;
	bool changed;
	const char* fields;
	std::function<void(void)> act;
};

delta_test_struct_t dst1, dst2;

void _DeltaSimpleTest(delta_simpletest_data_t* t, delta_t* delta, int jit) {
	_FillTestDelta(&dst1, t->fill1); _FillTestDelta(&dst2, t->fill2);
	t->act();
	_MarkAndEnsureCorrectResults(t->action, delta, &dst1, &dst2, jit, t->changed, t->fields);
}

void _DeltaSimpleTests(delta_t* delta, delta_simpletest_data_t* tests, int testsCount) {
	for (int usejit = 0; usejit <= 1; usejit++) {
		for (int i = 0; i < testsCount; i++) {
			_DeltaSimpleTest(tests + i, delta, usejit);
		}
	}
}

TEST(MarkFieldsTest_Simple_Primitives, Delta, 1000) {
	EngineInitializer engInitGuard;

	delta_t* delta = _CreateTestDeltaDesc();

	delta_simpletest_data_t testdata[] = {
			{ "SimpleUnchangedAll1", 0x71, 0x71, true, "", [](){} },
			{ "SimpleUnchangedAll2", 0x71, 0x72, false, "", [](){} },
			{ "Byte_0BlockCheck", 0x71, 0x71, true, "b_01", []() { dst2.b_01 = 0; } },
			{ "Byte_1BlockCheck", 0x71, 0x71, true, "b_11", []() { dst2.b_11 = 0; } },
			{ "Short_0BlockCheck", 0x71, 0x71, true, "s_02", []() { dst2.s_02 = 0; } },
			{ "Short_1BlockCheck", 0x71, 0x71, true, "s_12", []() { dst2.s_12 = 0; } },
			{ "Int_0BlockCheck", 0x71, 0x71, true, "i_04", []() { dst2.i_04 = 0; } },
			{ "Int_1BlockCheck", 0x71, 0x71, true, "i_14", []() { dst2.i_14 = 0; } },
			{ "Float_0BlockCheck", 0x71, 0x71, true, "f_08", []() { dst2.f_08 = 0; } },
			{ "Float_1BlockCheck", 0x71, 0x71, true, "f_18", []() { dst2.f_18 = 0; } },
	};
	_DeltaSimpleTests(delta, testdata, ARRAYSIZE(testdata));
}

TEST(MarkFieldsTest_InterBlock, Delta, 1000) {
	EngineInitializer engInitGuard;

	delta_t* delta = _CreateTestDeltaDesc();

	delta_simpletest_data_t testdata[] = {
			{ "Interblock1_guards", 0x71, 0x71, true, "b_4D b_52", []() { dst2.b_4D = dst2.b_52 = 0; } },
			{ "Interblock1_guards_and_val", 0x71, 0x71, true, "b_4D b_52 i_4E", []() { dst2.b_4D = dst2.b_52 = 0; dst2.i_4E = 0; } },
			{ "Interblock1_val", 0x71, 0x71, true, "i_4E", []() { dst2.i_4E = 0; } },
			{ "Interblock1_val_0b", 0x71, 0x71, true, "i_4E", []() { dst2.i_4E &= 0xFFFFFF00; } },
			{ "Interblock1_val_1b", 0x71, 0x71, true, "i_4E", []() { dst2.i_4E &= 0xFFFF00FF; } },
			{ "Interblock1_val_2b", 0x71, 0x71, true, "i_4E", []() { dst2.i_4E &= 0xFF00FFFF; } },
			{ "Interblock1_val_3b", 0x71, 0x71, true, "i_4E", []() { dst2.i_4E &= 0x00FFFFFF; } },

			{ "Interblock2_guards", 0x71, 0x71, true, "b_5C b_61", []() { dst2.b_5C = dst2.b_61 = 0; } },
			{ "Interblock2_guards_and_val", 0x71, 0x71, true, "b_5C b_61 i_5D", []() { dst2.b_5C = dst2.b_61 = 0; dst2.i_5D = 0; } },
			{ "Interblock2_val", 0x71, 0x71, true, "i_5D", []() { dst2.i_5D = 0; } },
			{ "Interblock2_val_0b", 0x71, 0x71, true, "i_5D", []() { dst2.i_5D &= 0xFFFFFF00; } },
			{ "Interblock2_val_1b", 0x71, 0x71, true, "i_5D", []() { dst2.i_5D &= 0xFFFF00FF; } },
			{ "Interblock2_val_2b", 0x71, 0x71, true, "i_5D", []() { dst2.i_5D &= 0xFF00FFFF; } },
			{ "Interblock2_val_3b", 0x71, 0x71, true, "i_5D", []() { dst2.i_5D &= 0x00FFFFFF; } },
	};
	_DeltaSimpleTests(delta, testdata, ARRAYSIZE(testdata));
}

TEST(MarkFieldsTest_Strings, Delta, 1000) {
	EngineInitializer engInitGuard;

	delta_t* delta = _CreateTestDeltaDesc();

	delta_simpletest_data_t testdata[] = {
			{ "Str_empty", 'c', 'c', true, "s_24", []() { dst2.s_24[0] = 0; } },
			{ "Str_empty2", 'c', 'c', true, "s_24", []() { dst1.s_24[0] = 0; } },
			{ "Str_both_empty_with_garbage", 'c', 'c', true, "", []() {
				dst1.s_24[0] = 0;   dst2.s_24[0] = 0;
				dst1.s_24[1] = 'd'; dst2.s_24[1] = 'e';
			}},
			{ "Str_case_check", 'c', 'c', true, "", []() { dst1.s_24[0] = 'C'; } },
	};
	_DeltaSimpleTests(delta, testdata, ARRAYSIZE(testdata));
}

TEST(MarkFieldsTest_TimeWindow, Delta, 1000) {
	EngineInitializer engInitGuard;

#ifdef REHLDS_FIXES
	bool rehlds_fixes = true;
#else
	bool rehlds_fixes = false;
#endif

	delta_t* delta = _CreateTestDeltaDesc();

	delta_simpletest_data_t testdata[] = {
			{ "TimeWindow_Below_Precision", 'c', 'c', true, (rehlds_fixes ? "w8_1C" : ""), []() { dst2.w8_1C = 0.001f; dst1.w8_1C = 0.0011f; } },
			{ "TimeWindow_Above_Precision1", 'c', 'c', true, (rehlds_fixes ? "w8_1C" : ""), []() { dst2.w8_1C = 0.1f; dst1.w8_1C = 0.11f; } }, //precision check, 0.11f is actually 0.10999
			{ "TimeWindow_Above_Precision2", 'c', 'c', true, "w8_1C", []() { dst2.w8_1C = 0.1f; dst1.w8_1C = 0.12f; } },
	};
	_DeltaSimpleTests(delta, testdata, ARRAYSIZE(testdata));
}

TEST(TestDelta_Test, Delta, 1000) {
	EngineInitializer engInitGuard;

	delta_t* delta = _CreateTestDeltaDesc();

	delta_test_struct_t testdata[4], from;
	int result[4];

	for (size_t i = 0; i < 4; i++)
		_FillTestDelta(&testdata[i], 0xCC);
	_FillTestDelta(&from, 0xCC);

	// equal
	result[0] = 0;

	// change byte + short + float
	testdata[1].b_01 = 1;
	testdata[1].s_12 = 1.0;
	testdata[1].f_08 = 1.0;
	result[1] = delta->pdd[1].significant_bits + delta->pdd[8].significant_bits + delta->pdd[4].significant_bits + (8 / 8 * 8 + 8);

	// change float + float + string
	testdata[2].f_18 = 2.0;
	testdata[2].wb_20 = 2.0;
	strcpy(testdata[2].s_24, "TestDelta_Test");

	result[2] = delta->pdd[10].significant_bits + delta->pdd[12].significant_bits + strlen(testdata[2].s_24) * 8 + 8 + (13 / 8 * 8 + 8);

	// change byte + int + float + short
	testdata[3].b_4D = 4;
	testdata[3].i_14 = 4;
	testdata[3].w8_0C = 4.0;
	testdata[3].s_12 = 4;
	result[3] = delta->pdd[14].significant_bits + delta->pdd[9].significant_bits + delta->pdd[5].significant_bits + delta->pdd[8].significant_bits + (14 / 8 * 8 + 8);

	for (size_t i = 0; i < 4; i++)
	{
		int tested = DELTA_TestDelta((uint8 *)&from, (uint8 *)&testdata[i], delta);
		CHECK(va("TestDelta_Test: returned bitcount %i is not equal to true value %i on iter %i", tested, result[i], i), tested == result[i]);
	}
}
