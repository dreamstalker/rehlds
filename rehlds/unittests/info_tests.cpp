#include "precompiled.h"
#include "cppunitlite/TestHarness.h"

TEST(PrefixedKeysRemove, Info, 1000) {
	struct testdata_t {
		const char* inData;
		const char* outData;
	};

	testdata_t testdata[] = {
		{ "", "" },
		{ "key\\value", "key\\value" },
		{ "\\key\\value", "\\key\\value" },
		{ "_key\\value", "" },
		{ "\\_key\\value", "" },
		{ "\\k\\v\\_u\\t\\y\\z", "\\k\\v\\y\\z" },
		{ "\\_k\\v\\u\\t\\y\\z", "\\u\\t\\y\\z" },
		{ "\\k\\v\\u\\t\\_y\\z", "\\k\\v\\u\\t" },
		{ "\\k\\v\\_u\\t\\*yyyyyyy\\zzzzzzz", "\\k\\v\\*yyyyyyy\\zzzzzzz" },
		{ "\\cl_updaterate\\60\\topcolor\\30\\_vgui_menus\\1\\_ah\\1\\_gm\\1ba5\\rate\\30000\\name\\aiaiaiaiaia\\*sid\\42893935\\model\\urban", "\\cl_updaterate\\60\\topcolor\\30\\rate\\30000\\name\\aiaiaiaiaia\\*sid\\42893935\\model\\urban" }
	};

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		testdata_t* d = &testdata[i];
		char localInfo[256];
		strcpy(localInfo, d->inData);
		Info_RemovePrefixedKeys(localInfo, '_');
		ZSTR_EQUAL("Invalid info string", d->outData, localInfo);
	}

}

TEST(SetValueForStarKey, Info, 1000) {
	struct testdata_t {
		const char* initialInfo;
		const char* key;
		const char* value;
		const char* finalInfo;
	};

	testdata_t testdata[] = {
		// Behavior
		{ "", "a", "b", "\\a\\b" },
		{ "\\a\\b\\c\\d", "a", "b", "\\c\\d\\a\\b" },
		{ "a\\b\\c\\d", "a", "b", "\\c\\d\\a\\b" },
		{ "\\a\\b\\c\\d", "e", "f", "\\a\\b\\c\\d\\e\\f" },
		{ "a\\b\\c\\d", "e", "f", "a\\b\\c\\d\\e\\f" },
		{ "\\a\\b\\c\\d", "b", "c", "\\a\\b\\c\\d\\b\\c" },
		{ "\\a\\b\\c\\d\\e\\f", "c", "q", "\\a\\b\\e\\f\\c\\q" },
		

		{ "\\a\\b\\c", "e", "f", "\\a\\b\\c\\e\\f" },
		{ "\\a\\b\\c\\", "e", "f", "\\a\\b\\c\\\\e\\f" },
		{ "\\a\\b\\\\c", "e", "f", "\\a\\b\\\\c\\e\\f" },

		
		//limits
		{ //do nothing since 'team' is not important key
			"\\cl_updaterate\\100\\topcolor\\60\\name\\abcdefghijklmnop\\*sid\\12332432525345\\_vgui_menus\\1\\model\\urban\\somelargeuselesskey\\12312321321323123123123213123123123123123123123123123123123123123dasdsad\\_cl_autowepswitch\\1",
			"team",
			"1234567890123456789012345678901234567890",
			"\\cl_updaterate\\100\\topcolor\\60\\name\\abcdefghijklmnop\\*sid\\12332432525345\\_vgui_menus\\1\\model\\urban\\somelargeuselesskey\\12312321321323123123123213123123123123123123123123123123123123123dasdsad\\_cl_autowepswitch\\1",
		},

		{ 
			"\\cl_updaterate\\100\\topcolor\\60\\name\\abcdefghijklmnop\\*sid\\12332432525345\\_vgui_menus\\1\\model\\urban\\somelargeuselesskey\\12312321321323123123123213123123123123123123123123123123123123123dasdsad\\_cl_autowepswitch\\1",
			"*team",
			"12345678901234567890123456789012345678",
			"\\cl_updaterate\\100\\topcolor\\60\\name\\abcdefghijklmnop\\*sid\\12332432525345\\_vgui_menus\\1\\model\\urban\\_cl_autowepswitch\\1\\*team\\12345678901234567890123456789012345678",
		},
		
		{
			"\\cl_updaterate\\100\\topcolor\\60\\name\\abcdefghijklmnop\\*sid\\12332432525345\\_vgui_menus\\1\\model\\urban\\somelargeuselesskey\\12312321321323123123123213123123123123123123123123123123123123123dasdsad\\_cl_autowepswitch\\1",
			"*team",
			"1234567890123456789012345678901234567",
			"\\cl_updaterate\\100\\topcolor\\60\\name\\abcdefghijklmnop\\*sid\\12332432525345\\_vgui_menus\\1\\model\\urban\\somelargeuselesskey\\12312321321323123123123213123123123123123123123123123123123123123dasdsad\\_cl_autowepswitch\\1\\*team\\1234567890123456789012345678901234567",
		}
	};

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		testdata_t* d = &testdata[i];
		char localInfo[257];
		strcpy(localInfo, d->initialInfo);
		localInfo[256] = 0;
		localInfo[255] = 0;
		Info_SetValueForStarKey(localInfo, d->key, d->value, 256);
		int infoLen = strlen(localInfo);
		CHECK("info string length", infoLen < 256);
		ZSTR_EQUAL("Invalid info string", d->finalInfo, localInfo);
	}
}

TEST(RemoveKeyValue, Info, 1000) {
	struct testdata_t {
		const char* initialInfo;
		const char* key;
		const char* finalInfo;
	};

	testdata_t testdata[] = {
		{ "", "a", "" },
		{ "\\a\\b", "a", "" },
		{ "\\a\\", "a", "" },
		{ "\\a\\\\", "a", "\\" },
		{ "\\a", "a", "" },
		{ "a", "a", "" },
		{ "a\\", "a", "" },
		{ "a\\b", "a", "" },
		{ "a\\b\\", "a", "\\" },
		{ "\\a\\b\\c\\d\\e\\f", "d", "\\a\\b\\c\\d\\e\\f" },
		{ "\\a\\b\\c\\d\\e\\f", "c", "\\a\\b\\e\\f" },
		{ "a\\b\\c\\d\\e\\f", "d", "a\\b\\c\\d\\e\\f" },
		{ "a\\b\\c\\d\\e\\f", "c", "a\\b\\e\\f" },
		{ "a\\b\\c\\d\\e\\f", "a", "\\c\\d\\e\\f" },
	};

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		testdata_t* d = &testdata[i];
		char localInfo[256];
		strcpy(localInfo, d->initialInfo);
		Info_RemoveKey(localInfo, d->key);
		ZSTR_EQUAL("Invalid info string", d->finalInfo, localInfo);
	}
}

TEST(GetKeyValue, Info, 1000) {
	struct testdata_t {
		const char* info;
		const char* key;
		const char* result;
	};

	testdata_t testdata[] = {
		{ "", "a", "" },
		{ "\\a\\b", "a", "b" },
		{ "\\a\\", "a", "" },
		{ "\\a\\\\", "a", "" },
		{ "\\a", "a", "" },
		{ "a", "a", "" },
		{ "a\\", "a", "" },
		{ "a\\b", "a", "b" },
		{ "a\\b\\", "a", "b" },
		{ "\\a\\b\\c\\d\\e\\f", "d", "" },
		{ "\\a\\b\\c\\d\\e\\f", "c", "d" },
		{ "a\\b\\c\\d\\e\\f", "d", "" },
		{ "a\\b\\c\\d\\e\\f", "c", "d" },

		{ "a\\b\\c\\d\\e\\f", "e", "f" },
		{ "\\a\\b\\c\\d\\e\\f", "e", "f" },

	};

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		testdata_t* d = &testdata[i];

		const char* res = Info_ValueForKey(d->info, d->key);
		ZSTR_EQUAL("Invalid info value", d->result, res);
	}
}
