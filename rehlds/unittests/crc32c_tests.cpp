#include "precompiled.h"
#include "rehlds_tests_shared.h"
#include "cppunitlite/TestHarness.h"
#include <iostream>

TEST(CRC32C_Hash, CRC32C, 1000) {

	Sys_CheckCpuInstructionsSupport();
	CHECK_WARNING_OUT("SSE4.2 Support", cpuinfo.sse4_2);

	struct testdata_t {
		const char* src;
		uint32 hash0;
		uint32 hashFF;
	};

	testdata_t testdata[] = {			
			{ "a", 0x93AD1061, 0x3E2FBCCF },
			{ "ab", 0x13C35EE4, 0x1D5DD6C9 },
			{ "abc", 0x562F9CCD, 0xC9B4C048 },
			{ "abcd", 0xDAAF41F6, 0x6D37F5CE },
			{ "abcde", 0x8122A0A2, 0x3BAF2968 },
			{ "abcdef", 0x0496937B, 0xAC43100E },
			{ "abcdefg", 0x5D199E2C, 0x19D80BBE },
			{ "abcdefgh", 0x86BC933D, 0xF56BDE48 },
			{ "abcdefghi", 0x9639F15F, 0xD2236603 },
			{ "abcdefghij", 0x0584645C, 0x19A66BC8 },
	};

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		uint32 pureCksum0 = crc32c_t_nosse(0, (const uint8*)testdata[i].src, strlen(testdata[i].src));
		uint32 sseCksum0 = crc32c_t_sse(0, (const uint8*)testdata[i].src, strlen(testdata[i].src));

		uint32 pureCksumFF = crc32c_t_nosse(-1, (const uint8*)testdata[i].src, strlen(testdata[i].src));
		uint32 sseCksumFF = crc32c_t_sse(-1, (const uint8*)testdata[i].src, strlen(testdata[i].src));
		
		UINT32_EQUALS("Pure crc32c checksum-0 mismatch", testdata[i].hash0, pureCksum0);
		UINT32_EQUALS("SSE crc32c checksum-0 mismatch", testdata[i].hash0, sseCksum0);

		UINT32_EQUALS("Pure crc32c checksum-FF mismatch", testdata[i].hashFF, pureCksumFF);
		UINT32_EQUALS("SSE crc32c checksum-FF mismatch", testdata[i].hashFF, sseCksumFF);
	}
}