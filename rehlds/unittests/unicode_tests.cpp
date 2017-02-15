#include "precompiled.h"
#include "rehlds_tests_shared.h"
#include "cppunitlite/TestHarness.h"

TEST(UTF8ToFromUChar32, UNICODE_STRTOOLS, 1000)
{
	struct testdata_t {
		uint8 data[16];
		bool err;
		uint32 glyphId;
		int charWidth;
	};

	testdata_t testData[] = {
		{ { 0x61, 0 },                   false, 0x61,    1 },
		{ { 0x62, 0 },                   false, 0x62,    1 },
		{ { 0x63, 0 },                   false, 0x63,    1 },
		{ { 0x64, 0 },                   false, 0x64,    1 },
		{ { 0xCB, 0x85, 0 },             false, 0x2C5,   2 },
		{ { 0xCB, 0x84, 0 },             false, 0x2C4,   2 },
		{ { 0xE0, 0xBF, 0x80, 0 },       false, 0xFC0,   3 },
		{ { 0xE0, 0xBF, 0x8F, 0 },       false, 0xFCF,   3 },
		{ { 0xE1, 0x8E, 0x92, 0 },       false, 0x1392,  3 },
		{ { 0xE1, 0xB5, 0xA6, 0 },       false, 0x1d66,  3 },
		{ { 0xE2, 0x86, 0x80, 0 },       false, 0x2180,  3 },
		{ { 0xE2, 0x8C, 0x82, 0 },       false, 0x2302,  3 },
		{ { 0xE2, 0xB5, 0x98, 0 },       false, 0x2D58,  3 },
		{ { 0xE3, 0x80, 0xB0, 0 },       false, 0x3030,  3 },
		{ { 0xE7, 0xBF, 0xB0, 0 },       false, 0x7ff0,  3 },
		{ { 0xE8, 0x80, 0x80, 0 },       false, 0x8000,  3 },
		{ { 0xE8, 0x80, 0x90, 0 },       false, 0x8010,  3 },
		{ { 0xEF, 0xBF, 0xA0, 0 },       false, 0xffe0,  3 },
		{ { 0xF0, 0x90, 0x80, 0x80, 0 }, false, 0x10000, 4 },
		{ { 0xF0, 0x90, 0x80, 0x90, 0 }, false, 0x10010, 4 },
		{ { 0xF0, 0x90, 0x85, 0x80, 0 }, false, 0x10140, 4 },
		{ { 0xF0, 0x90, 0xA0, 0x80, 0 }, false, 0x10800, 4 },

		//Invalid sequences
		{ { 0x81, 0 },                   true,  0x00,    1 },
		{ { 0xC1, 0 },                   true,  0x00,    1 },
		{ { 0xC1, 0xC1, 0 },             true,  0x00,    1 },
		{ { 0xE1, 0 },                   true,  0x00,    1 },
		{ { 0xE1, 0xC1, 0 },             true,  0x00,    1 },
		{ { 0xE1, 0x80, 0xC1, 0 },       true,  0x00,    2 },
	};

	for (int i = 0; i < ARRAYSIZE(testData); i++) {
		testdata_t* d = &testData[i];

		//UTF8 => Char32
		{
			uchar32 glyphId = 0xFFFFFFFF;
			bool err = true;
			int charWidth = Q_UTF8ToUChar32((char*)d->data, glyphId, err);
			CHECK("UTF8 => Char32: Parsing error check", err == d->err);
			UINT32_EQUALS("UTF8 => Char32: Char width check", d->charWidth, charWidth);
			if (!err) {
				UINT32_EQUALS("UTF8 => Char32: GlyphId check", d->glyphId, glyphId);
			}
		}

		//Char32 => UTF8
		if (!d->err) {
			uint8 localBuf[8] = { 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC };
			int charWidth = Q_UChar32ToUTF8(d->glyphId, (char*)&localBuf[1]);

			UINT32_EQUALS("Char32 => UTF8: Char width check", d->charWidth, charWidth);
			MEM_EQUAL("Char32 => UTF8: Bytes check", d->data, &localBuf[1], d->charWidth);

			CHECK("Char32 => UTF8: pre guard byte check", localBuf[0] == 0xCC);
			CHECK("Char32 => UTF8: post guard byte check", localBuf[1 + charWidth] == 0xCC);
		}

		//Char32 => UTF8 Length
		if (!d->err) {
			int charWidth = Q_UChar32ToUTF8Len(d->glyphId);
			UINT32_EQUALS("Char32 => UTF8 Length: Char width check", d->charWidth, charWidth);
		}
	}
}

TEST(UnicodeValidateAndRepair, UNICODE_STRTOOLS, 1000)
{
	struct testdata_t {
		uint8 indata[16];
		uint8 outdata[16];
		qboolean valid;
	};

	testdata_t testData[] = {
		{ { 0x61, 0x62, 0 }, { 0x61, 0x62, 0 }, TRUE },
		{ { 0x61, 0x81, 0x62, 0 }, { 0x61, 0x62, 0 }, FALSE },
		{ { 0x81, 0 }, { 0 }, FALSE },
	};

	for (int i = 0; i < ARRAYSIZE(testData); i++) {
		testdata_t* d = &testData[i];

		char localbuf[256];
		strcpy(localbuf, (char*) d->indata);

		qboolean valid = Q_UnicodeValidate(localbuf);
		CHECK("UnicodeValidate: invalid result", valid == d->valid);

		Q_UnicodeRepair(localbuf);
		ZSTR_EQUAL("UnicodeRepair: invalid result", (char*)d->outdata, localbuf);
	}
}
