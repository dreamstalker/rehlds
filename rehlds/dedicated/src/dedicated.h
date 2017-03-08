#ifndef DEDICATED_H
#define DEDICATED_H
#ifdef _WIN32
#pragma once
#endif

typedef void (*NET_Sleep_t)();
typedef void (*SleepType)(int msec);

extern bool g_bVGui;
extern IDedicatedServerAPI *engineAPI;
#ifdef LAUNCHER_FIXES
extern IRehldsApi *rehldsApi;
extern const RehldsFuncs_t* rehldsFuncs;
#endif
#endif // DEDICATED_H
