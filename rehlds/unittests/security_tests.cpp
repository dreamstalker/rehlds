#include "precompiled.h"
#include "rehlds_tests_shared.h"
#include "cppunitlite/TestHarness.h"

TEST(IsSafeFileExtension, SecurityChecks, 1000)
{
	struct testdata_t
	{
		const char* filename;
		qboolean safe;
	};

	testdata_t testdata[] = {
		{"radio/go.wav", TRUE},
		{"radio/go.WAV", TRUE},
		{"textures.wad", TRUE},
#ifdef REHLDS_FIXES
		{"!QWERTY", TRUE},
		// TODO:
		//{"file.dll2", TRUE},
		//{"noext", FALSE},
		//{".hidden", FALSE},
		//{"subdir/.hidden", FALSE},
#else
		{"file.dll2", FALSE},
#endif
		{"../file.txt", FALSE},
		{"/home/file.txt", FALSE},
		{"C:/Windows/file.txt", FALSE},
		{"models\\terror.mdl", FALSE},
		{"file~.mdl", FALSE},
		{"file.wav.", FALSE},
		{"file.dll.txt", FALSE},
		{"halflife.wad", FALSE},
		{"pak0.pak", FALSE},
		{"xeno.wad", FALSE},
		{"file.cfg", FALSE},
		{"file.lst", FALSE},
		{"file.exe", FALSE},
		{"file.vbs", FALSE},
		{"file.com", FALSE},
		{"file.bat", FALSE},
		{"file.dll", FALSE},
		{"file.ini", FALSE},
		{"file.log", FALSE},
		{"file.so", FALSE},
		{"file.dylib", FALSE},
		{"file.sys", FALSE},
		{"file.SYS", FALSE},
	};

	for (int i = 0; i < ARRAYSIZE(testdata); i++) {
		testdata_t* d = &testdata[i];
		qboolean result = IsSafeFileToDownload(d->filename);

		char msg[256];
		Q_snprintf(msg, sizeof msg, "IsSafeFileToDownload(%s) check", d->filename);
		CHECK(msg, result == d->safe);
	}

}
