#include "precompiled.h"


/* ============================================================================
								external function hooks
 ============================================================================*/
uint32 __cdecl time_hooked(uint32* pTime)
{
	return CRehldsPlatformHolder::get()->time(pTime);
}

struct tm* __cdecl localtime_hooked(uint32* pTime)
{
	if (pTime == NULL)
		rehlds_syserror("%s: pTime is NULL", __func__);

	return CRehldsPlatformHolder::get()->localtime(*pTime);
}

void __cdecl srand_hooked(uint32 seed)
{
	CRehldsPlatformHolder::get()->srand(seed);
}

int __cdecl rand_hooked()
{
	return CRehldsPlatformHolder::get()->rand();
}

void WINAPI Sleep_hooked(DWORD msec)
{
	CRehldsPlatformHolder::get()->Sleep(msec);
}

BOOL WINAPI QueryPerfCounter_hooked(LARGE_INTEGER* counter)
{
	return CRehldsPlatformHolder::get()->QueryPerfCounter(counter);
}

BOOL WINAPI QueryPerfFreq_hooked(LARGE_INTEGER* freq)
{
	return CRehldsPlatformHolder::get()->QueryPerfFreq(freq);
}

DWORD WINAPI GetTickCount_hooked()
{
	return CRehldsPlatformHolder::get()->GetTickCount();
}

void WINAPI GetLocalTime_hooked(LPSYSTEMTIME time)
{
	CRehldsPlatformHolder::get()->GetLocalTime(time);
}

void WINAPI GetSystemTime_hooked(LPSYSTEMTIME time)
{
	CRehldsPlatformHolder::get()->GetSystemTime(time);
}

void WINAPI GetTimeZoneInfo_hooked(LPTIME_ZONE_INFORMATION tzinfo)
{
	CRehldsPlatformHolder::get()->GetTimeZoneInfo(tzinfo);
}

SOCKET __stdcall socket_hooked(int af, int type, int protocol)
{
	return CRehldsPlatformHolder::get()->socket(af, type, protocol);
}

int __stdcall ioctlsocket_hooked(SOCKET s, long cmd, u_long *argp)
{
	return CRehldsPlatformHolder::get()->ioctlsocket(s, cmd, argp);
}

int __stdcall setsockopt_hooked(SOCKET s, int level, int optname, const char* optval, int optlen)
{
	return CRehldsPlatformHolder::get()->setsockopt(s, level, optname, optval, optlen);
}

int __stdcall closesocket_hooked(SOCKET s)
{
	return CRehldsPlatformHolder::get()->closesocket(s);
}

int __stdcall recvfrom_hooked(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int *fromlen)
{
	return CRehldsPlatformHolder::get()->recvfrom(s, buf, len, flags, from, fromlen);
}

int __stdcall sendto_hooked(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen)
{
	return CRehldsPlatformHolder::get()->sendto(s, buf, len, flags, to, tolen);
}

int __stdcall bind_hooked(SOCKET s, const struct sockaddr* addr, int namelen)
{
	return CRehldsPlatformHolder::get()->bind(s, addr, namelen);
}

int __stdcall getsockname_hooked(SOCKET s, struct sockaddr* name, int* namelen)
{
	return CRehldsPlatformHolder::get()->getsockname(s, name, namelen);
}

int __stdcall WSAGetLastError_hooked()
{
	return CRehldsPlatformHolder::get()->WSAGetLastError();
}

struct hostent* __stdcall gethostbyname_hooked(const char *name)
{
	return CRehldsPlatformHolder::get()->gethostbyname(name);
}

int __stdcall gethostname_hooked(char *name, int namelen)
{
	return CRehldsPlatformHolder::get()->gethostname(name, namelen);
}

void __cdecl SteamAPI_SetMiniDumpComment_hooked(const char *pchMsg)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void __cdecl SteamAPI_WriteMiniDump_hooked(uint32 uStructuredExceptionCode, void* pvExceptionInfo, uint32 uBuildID)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void __cdecl SteamAPI_RegisterCallback_hooked(class CCallbackBase *pCallback, int iCallback)
{
	return CRehldsPlatformHolder::get()->SteamAPI_RegisterCallback(pCallback, iCallback);
}

void __cdecl SteamAPI_RunCallbacks_hooked()
{
	CRehldsPlatformHolder::get()->SteamAPI_RunCallbacks();
}

bool __cdecl SteamAPI_Init_hooked()
{
	return CRehldsPlatformHolder::get()->SteamAPI_Init();
}

ISteamUser* __cdecl SteamUser_hooked()
{
	rehlds_syserror("%s: not implemented", __func__);
	//return NULL;
}

ISteamFriends* __cdecl SteamFriends_hooked()
{
	rehlds_syserror("%s: not implemented", __func__);
	//return NULL;
}

void __cdecl SteamGameServer_RunCallbacks_hooked()
{
	CRehldsPlatformHolder::get()->SteamGameServer_RunCallbacks();
}

void __cdecl SteamAPI_Shutdown_hooked()
{
	rehlds_syserror("%s: not implemented", __func__);
}

void __cdecl SteamGameServer_Shutdown_hooked()
{
	CRehldsPlatformHolder::get()->SteamGameServer_Shutdown();
}

bool __cdecl SteamGameServer_Init_hooked(uint32 unIP, uint16 usSteamPort, uint16 usGamePort, uint16 usQueryPort, EServerMode eServerMode, const char *pchVersionString)
{
	return CRehldsPlatformHolder::get()->SteamGameServer_Init(unIP, usSteamPort, usGamePort, usQueryPort, eServerMode, pchVersionString);
}

void __cdecl SteamAPI_UnregisterCallback_hooked(class CCallbackBase *pCallback)
{
	CRehldsPlatformHolder::get()->SteamAPI_UnregisterCallback(pCallback);
}

ISteamGameServer* __cdecl SteamGameServer_hooked()
{
	return CRehldsPlatformHolder::get()->SteamGameServer();
}

void __cdecl SteamAPI_SetBreakpadAppID_hooked(uint32 unAppID)
{
	CRehldsPlatformHolder::get()->SteamAPI_SetBreakpadAppID(unAppID);
}

void __cdecl SteamAPI_RegisterCallResult_hooked(class CCallbackBase *pCallback, SteamAPICall_t hAPICall)
{
	rehlds_syserror("%s: not implemented", __func__);
}

ISteamHTTP* __cdecl SteamHTTP_hooked()
{
	rehlds_syserror("%s: not implemented", __func__);
	//return NULL;
}

void __cdecl SteamAPI_UnregisterCallResult_hooked(class CCallbackBase *pCallback, SteamAPICall_t hAPICall)
{
	CRehldsPlatformHolder::get()->SteamAPI_UnregisterCallResult(pCallback, hAPICall);
}

ISteamApps* __cdecl SteamApps_hooked()
{
	return CRehldsPlatformHolder::get()->SteamApps();
}

void __cdecl SteamAPI_UseBreakpadCrashHandler_hooked(char const *pchVersion, char const *pchDate, char const *pchTime, bool bFullMemoryDumps, void *pvContext, PFNPreMinidumpCallback m_pfnPreMinidumpCallback)
{
	CRehldsPlatformHolder::get()->SteamAPI_UseBreakpadCrashHandler(pchVersion, pchDate, pchTime, bFullMemoryDumps, pvContext, m_pfnPreMinidumpCallback);
}

/* ============================================================================
							   Hooks installation
 ============================================================================*/
HMODULE getModuleHandleOrDie(const char* moduleName) {
	HMODULE res = GetModuleHandleA(moduleName);
	if (res == NULL) {
		rehlds_syserror("getModuleHandleOrDie(): module not found: '%s'", moduleName);
	}

	return res;
}

void* getProcAddressOrDie(HMODULE hModule, const char* procName) {
	void* res = GetProcAddress(hModule, procName);
	if (res == NULL) {
		rehlds_syserror("getProcAddressOrDie(): procedure not found: '%s'", procName);
	}

	return res;
}

void InstallImportTableHook(PIMAGE_THUNK_DATA thunk, void* func)
{
	DWORD oldProtect;
	VirtualProtect(thunk, 4, PAGE_READWRITE, &oldProtect);
	thunk->u1.Function = (DWORD)func;
}

void TestSuite_InstallHooks(const Module* engine) {
	HMODULE hKernel32 = getModuleHandleOrDie("kernel32.dll");
	HMODULE hWinSock32 = getModuleHandleOrDie("wsock32.dll");
	HMODULE hSteamApi = getModuleHandleOrDie("steam_api.dll");


	void* QueryPerfCounter_addr = getProcAddressOrDie(hKernel32, "QueryPerformanceCounter");
	void* QueryPerfFreq_addr = getProcAddressOrDie(hKernel32, "QueryPerformanceFrequency");
	void* Sleep_addr = getProcAddressOrDie(hKernel32, "Sleep");
	void* GetTickCount_addr = getProcAddressOrDie(hKernel32, "GetTickCount");
	void* GetLocalTime_addr = getProcAddressOrDie(hKernel32, "GetLocalTime");
	void* GetSystemTime_addr = getProcAddressOrDie(hKernel32, "GetSystemTime");
	void* GetTimeZoneInformation_addr = getProcAddressOrDie(hKernel32, "GetTimeZoneInformation");

	void* socket_addr = getProcAddressOrDie(hWinSock32, "socket");
	void* ioctlsocket_addr = getProcAddressOrDie(hWinSock32, "ioctlsocket");
	void* setsockopt_addr = getProcAddressOrDie(hWinSock32, "setsockopt");
	void* closesocket_addr = getProcAddressOrDie(hWinSock32, "closesocket");
	void* recvfrom_addr = getProcAddressOrDie(hWinSock32, "recvfrom");
	void* sendto_addr = getProcAddressOrDie(hWinSock32, "sendto");
	void* bind_addr = getProcAddressOrDie(hWinSock32, "bind");
	void* getsockname_addr = getProcAddressOrDie(hWinSock32, "getsockname");
	void* gethostname_addr = getProcAddressOrDie(hWinSock32, "gethostname");
	void* gethostbyname_addr = getProcAddressOrDie(hWinSock32, "gethostbyname");
	void* WSAGetLastError_addr = getProcAddressOrDie(hWinSock32, "WSAGetLastError");

	void* SteamAPI_SetMiniDumpComment_addr = getProcAddressOrDie(hSteamApi, "SteamAPI_SetMiniDumpComment");
	void* SteamAPI_WriteMiniDump_addr = getProcAddressOrDie(hSteamApi, "SteamAPI_WriteMiniDump");
	void* SteamAPI_RegisterCallback_addr = getProcAddressOrDie(hSteamApi, "SteamAPI_RegisterCallback");
	void* SteamAPI_RunCallbacks_addr = getProcAddressOrDie(hSteamApi, "SteamAPI_RunCallbacks");
	void* SteamAPI_Init_addr = getProcAddressOrDie(hSteamApi, "SteamAPI_Init");
	void* SteamUser_addr = getProcAddressOrDie(hSteamApi, "SteamUser");
	void* SteamFriends_addr = getProcAddressOrDie(hSteamApi, "SteamFriends");
	void* SteamGameServer_RunCallbacks_addr = getProcAddressOrDie(hSteamApi, "SteamGameServer_RunCallbacks");
	void* SteamAPI_Shutdown_addr = getProcAddressOrDie(hSteamApi, "SteamAPI_Shutdown");
	void* SteamGameServer_Shutdown_addr = getProcAddressOrDie(hSteamApi, "SteamGameServer_Shutdown");
	void* SteamGameServer_Init_addr = getProcAddressOrDie(hSteamApi, "SteamGameServer_Init");
	void* SteamAPI_UnregisterCallback_addr = getProcAddressOrDie(hSteamApi, "SteamAPI_UnregisterCallback");
	void* SteamGameServer_addr = getProcAddressOrDie(hSteamApi, "SteamGameServer");
	void* SteamAPI_SetBreakpadAppID_addr = getProcAddressOrDie(hSteamApi, "SteamAPI_SetBreakpadAppID");
	void* SteamAPI_RegisterCallResult_addr = getProcAddressOrDie(hSteamApi, "SteamAPI_RegisterCallResult");
	void* SteamHTTP_addr = getProcAddressOrDie(hSteamApi, "SteamHTTP");
	void* SteamAPI_UnregisterCallResult_addr = getProcAddressOrDie(hSteamApi, "SteamAPI_UnregisterCallResult");
	void* SteamApps_addr = getProcAddressOrDie(hSteamApi, "SteamApps");
	void* SteamAPI_UseBreakpadCrashHandler_addr = getProcAddressOrDie(hSteamApi, "SteamAPI_UseBreakpadCrashHandler");

	
	PIMAGE_DOS_HEADER peHeader = (PIMAGE_DOS_HEADER)engine->base;
	PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(engine->base + peHeader->e_lfanew);
	
	PIMAGE_IMPORT_DESCRIPTOR impDesc = (PIMAGE_IMPORT_DESCRIPTOR)(ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress + engine->base);
	for (; impDesc->Name; impDesc++)
	{
		const char* libName = (const char*)(engine->base + impDesc->Name);
		bool isKernel32 = !_stricmp("kernel32.dll", libName);
		bool isWSock32 = !_stricmp("wsock32.dll", libName);
		bool isSteamApi = !_stricmp("steam_api.dll", libName);

		PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)(engine->base + impDesc->FirstThunk);
		for (; thunk->u1.Function; ++thunk)
		{
			void* fptr = (void**)(thunk->u1.Function);
			if (isKernel32)
			{
				if (fptr == Sleep_addr)
				{
					InstallImportTableHook(thunk, &Sleep_hooked);
				}
				else if (fptr == QueryPerfCounter_addr)
				{
					InstallImportTableHook(thunk, &QueryPerfCounter_hooked);
				}
				else if (fptr == QueryPerfFreq_addr)
				{
					InstallImportTableHook(thunk, &QueryPerfFreq_hooked);
				}
				else if (fptr == GetTickCount_addr)
				{
					InstallImportTableHook(thunk, &GetTickCount_hooked);
				}
				else if (fptr == GetLocalTime_addr)
				{
					InstallImportTableHook(thunk, &GetLocalTime_hooked);
				}
				else if (fptr == GetSystemTime_addr)
				{
					InstallImportTableHook(thunk, &GetSystemTime_hooked);
				}
				else if (fptr == GetTimeZoneInformation_addr)
				{
					InstallImportTableHook(thunk, &GetTimeZoneInfo_hooked);
				}
			}
			else if (isWSock32)
			{
				if (fptr == socket_addr)
				{
					InstallImportTableHook(thunk, &socket_hooked);
				}
				else if (fptr == ioctlsocket_addr) {
					InstallImportTableHook(thunk, &ioctlsocket_hooked);
				}
				else if (fptr == setsockopt_addr) {
					InstallImportTableHook(thunk, &setsockopt_hooked);
				}
				else if (fptr == closesocket_addr) {
					InstallImportTableHook(thunk, &closesocket_hooked);
				}
				else if (fptr == recvfrom_addr) {
					InstallImportTableHook(thunk, &recvfrom_hooked);
				}
				else if (fptr == sendto_addr) {
					InstallImportTableHook(thunk, &sendto_hooked);
				}
				else if (fptr == bind_addr) {
					InstallImportTableHook(thunk, &bind_hooked);
				}
				else if (fptr == getsockname_addr) {
					InstallImportTableHook(thunk, &getsockname_hooked);
				}
				else if (fptr == WSAGetLastError_addr) {
					InstallImportTableHook(thunk, &WSAGetLastError_hooked);
				}
				else if (fptr == gethostbyname_addr) {
					InstallImportTableHook(thunk, &gethostbyname_hooked);
				}
				else if (fptr == gethostname_addr) {
					InstallImportTableHook(thunk, &gethostname_hooked);
				}
			}
			else if (isSteamApi)
			{
				if (fptr == SteamAPI_SetMiniDumpComment_addr)
				{
					InstallImportTableHook(thunk, &SteamAPI_SetMiniDumpComment_hooked);
				}
				else if (fptr == SteamAPI_WriteMiniDump_addr)
				{
					InstallImportTableHook(thunk, &SteamAPI_WriteMiniDump_hooked);
				}
				else if (fptr == SteamAPI_RegisterCallback_addr)
				{
					InstallImportTableHook(thunk, &SteamAPI_RegisterCallback_hooked);
				}
				else if (fptr == SteamAPI_RunCallbacks_addr)
				{
					InstallImportTableHook(thunk, &SteamAPI_RunCallbacks_hooked);
				}
				else if (fptr == SteamAPI_Init_addr)
				{
					InstallImportTableHook(thunk, &SteamAPI_Init_hooked);
				}
				else if (fptr == SteamUser_addr)
				{
					InstallImportTableHook(thunk, &SteamUser_hooked);
				}
				else if (fptr == SteamFriends_addr)
				{
					InstallImportTableHook(thunk, &SteamFriends_hooked);
				}
				else if (fptr == SteamGameServer_RunCallbacks_addr)
				{
					InstallImportTableHook(thunk, &SteamGameServer_RunCallbacks_hooked);
				}
				else if (fptr == SteamAPI_Shutdown_addr)
				{
					InstallImportTableHook(thunk, &SteamAPI_Shutdown_hooked);
				}
				else if (fptr == SteamGameServer_Shutdown_addr)
				{
					InstallImportTableHook(thunk, &SteamGameServer_Shutdown_hooked);
				}
				else if (fptr == SteamGameServer_Init_addr)
				{
					InstallImportTableHook(thunk, &SteamGameServer_Init_hooked);
				}
				else if (fptr == SteamAPI_UnregisterCallback_addr)
				{
					InstallImportTableHook(thunk, &SteamAPI_UnregisterCallback_hooked);
				}
				else if (fptr == SteamGameServer_addr)
				{
					InstallImportTableHook(thunk, &SteamGameServer_hooked);
				}
				else if (fptr == SteamAPI_SetBreakpadAppID_addr)
				{
					InstallImportTableHook(thunk, &SteamAPI_SetBreakpadAppID_hooked);
				}
				else if (fptr == SteamAPI_RegisterCallResult_addr)
				{
					InstallImportTableHook(thunk, &SteamAPI_RegisterCallResult_hooked);
				}
				else if (fptr == SteamHTTP_addr)
				{
					InstallImportTableHook(thunk, &SteamHTTP_hooked);
				}
				else if (fptr == SteamAPI_UnregisterCallResult_addr)
				{
					InstallImportTableHook(thunk, &SteamAPI_UnregisterCallResult_hooked);
				}
				else if (fptr == SteamApps_addr)
				{
					InstallImportTableHook(thunk, &SteamApps_hooked);
				}
				else if (fptr == SteamAPI_UseBreakpadCrashHandler_addr)
				{
					InstallImportTableHook(thunk, &SteamAPI_UseBreakpadCrashHandler_hooked);
				}
			}
		}
	}
}

void TestSuite_InstallCStdHooks(const AddressRef* funcRefs) {
	const AddressRef* curFunc = funcRefs;
	FunctionHook fhook;
	while (curFunc->symbolName)
	{
		if (!strcmp("time", curFunc->symbolName)) {
			fhook.originalAddress = curFunc->originalAddress;
			fhook.handlerFunc = (size_t) &time_hooked;
			HookFunction(NULL, &fhook);
		}
		else if (!strcmp("localtime", curFunc->symbolName)) {
			fhook.originalAddress = curFunc->originalAddress;
			fhook.handlerFunc = (size_t)&localtime_hooked;
			HookFunction(NULL, &fhook);
		}
		else if (!strcmp("srand", curFunc->symbolName)) {
			fhook.originalAddress = curFunc->originalAddress;
			fhook.handlerFunc = (size_t)&srand_hooked;
			HookFunction(NULL, &fhook);
		}
		else if (!strcmp("rand", curFunc->symbolName)) {
			fhook.originalAddress = curFunc->originalAddress;
			fhook.handlerFunc = (size_t)&rand_hooked;
			HookFunction(NULL, &fhook);
		}
		curFunc++;
	}
}

void TestSuite_InitAnonymizer(CAnonymizingEngExtInterceptor& a) {

}

void TestSuite_Init(const Module* engine, const Module* executable, const AddressRef* funcRefs) {
	bool needInstallImportTableHooks = false;

	if (g_RehldsRuntimeConfig.testPlayerMode == TPM_RECORD)
	{
		CRehldsPlatformHolder::set(new CRecordingEngExtInterceptor(g_RehldsRuntimeConfig.testRecordingFileName, CRehldsPlatformHolder::get()));
		needInstallImportTableHooks = true;
	}
	else if (g_RehldsRuntimeConfig.testPlayerMode == TPM_PLAY)
	{
		CRehldsPlatformHolder::set(new CPlayingEngExtInterceptor(g_RehldsRuntimeConfig.testRecordingFileName, true));
		needInstallImportTableHooks = true;
	}
	else if (g_RehldsRuntimeConfig.testPlayerMode == TPM_ANONYMIZE)
	{
		char fname[260];
		sprintf(fname, "%s.new", g_RehldsRuntimeConfig.testRecordingFileName);
		CRehldsPlatformHolder::set(new CPlayingEngExtInterceptor(g_RehldsRuntimeConfig.testRecordingFileName, false));
		auto anonymizer = new CAnonymizingEngExtInterceptor(CRehldsPlatformHolder::get());
		CRehldsPlatformHolder::set(anonymizer);
		CRehldsPlatformHolder::set(new CRecordingEngExtInterceptor(fname, CRehldsPlatformHolder::get()));
		TestSuite_InitAnonymizer(*anonymizer);
		needInstallImportTableHooks = true;
	}

	if (needInstallImportTableHooks) {
		if (engine != NULL) {
			TestSuite_InstallHooks(engine);
		}

		TestSuite_InstallHooks(executable);

		if (funcRefs != NULL) {
			TestSuite_InstallCStdHooks(funcRefs);
		}
	}
}

