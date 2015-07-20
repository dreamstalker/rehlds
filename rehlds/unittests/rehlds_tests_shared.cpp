#include "precompiled.h"
#include "rehlds_tests_shared.h"

static uint8 g_TestMemoryBuf[1024 * 1024 * 4];

void Tests_InitEngine() {
	Memory_Init(g_TestMemoryBuf, sizeof(g_TestMemoryBuf));

	FR_Init();

#ifdef REHLDS_FLIGHT_REC
	FR_Rehlds_Init();
#endif //REHLDS_FLIGHT_REC

	Cbuf_Init();
	Cmd_Init();
	Cvar_Init();
	Cvar_CmdInit();
}

void Tests_ShutdownEngine() {
	Cvar_Shutdown();
	Cmd_Shutdown();

	mainzone = NULL;
	hunk_base = NULL;
	hunk_size = 0;

	FR_Shutdown();

	SV_Shutdown();
}
