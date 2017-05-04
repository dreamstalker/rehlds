#pragma once

#ifdef _WIN32
#define VGUI
#endif

#define LAUNCHER_ERROR	-1
#define LAUNCHER_OK		0

typedef void (*NET_Sleep_t)();
typedef void (*SleepType)(int msec);

extern bool g_bVGui;
extern IDedicatedServerAPI *engineAPI;

#ifdef LAUNCHER_FIXES
extern IRehldsApi *rehldsApi;
extern const RehldsFuncs_t* rehldsFuncs;
#endif

bool Sys_SetupConsole();
void Sys_PrepareConsoleInput();
void Sys_InitPingboost();
