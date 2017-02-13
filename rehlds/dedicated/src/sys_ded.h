#ifndef SYS_DED_H
#define SYS_DED_H
#ifdef _WIN32
#pragma once
#endif

#include "isys.h"
#include "dedicated.h"
#include "dll_state.h"

#if !defined(_WIN32)
#include <signal.h>
#endif // !defined(_WIN32)

int RunServer();
void Load3rdParty();
void Sys_Printf_Safe(char *text);
void Sys_Printf(char *fmt, ...);
void _log(const char *fmt, ...);

extern long hDLLThirdParty;
extern char *gpszCvars;
extern bool gbAppHasBeenTerminated;
extern IFileSystem *g_pFileSystemInterface;
extern CSysModule *g_pEngineModule;
extern CSysModule *g_pFileSystemModule;
extern CreateInterfaceFn g_FilesystemFactoryFn;

#ifdef _WIN32
BOOL WINAPI MyHandlerRoutine(DWORD CtrlType);
#endif // _WIN32

#endif // SYS_DED_H
