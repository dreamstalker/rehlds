#pragma once

void Load3rdParty();
void Sys_Printf_Safe(char *text);
void Sys_Printf(char *fmt, ...);
void _log(const char *fmt, ...);

extern char *gpszCvars;
extern bool g_bAppHasBeenTerminated;
extern IFileSystem *g_pFileSystemInterface;
extern CSysModule *g_pEngineModule;
extern CSysModule *g_pFileSystemModule;
extern CreateInterfaceFn g_FilesystemFactoryFn;

#ifdef _WIN32
BOOL WINAPI MyHandlerRoutine(DWORD CtrlType);
#endif // _WIN32

int RunEngine();
int StartServer(char* cmdline);
