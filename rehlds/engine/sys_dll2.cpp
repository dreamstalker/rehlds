/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#include "precompiled.h"

IDedicatedExports *dedicated_;
qboolean g_bIsWin95;
qboolean g_bIsWin98;
double g_flLastSteamProgressUpdateTime;

//CEngineAPI __g_CEngineAPI_singleton;
//InterfaceReg __g_CreateCEngineAPIIEngineAPI_reg;

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

char *szCommonPreloads = "multiplayer_preloads";
char *szReslistsBaseDir = "reslists";
char *szReslistsExt = ".lst";

#else //HOOK_ENGINE

char *szCommonPreloads;
char *szReslistsBaseDir;
char *szReslistsExt;

#endif //HOOK_ENGINE

//CDedicatedServerAPI __g_CDedicatedServerAPI_singleton;
//InterfaceReg __g_CreateCDedicatedServerAPIIDedicatedServerAPI_reg

/* <8fce7> ../engine/sys_dll2.cpp:150 */
const char *GetCurrentSteamAppName(void)
{
	if (!Q_stricmp(com_gamedir, "cstrike") || !Q_stricmp(com_gamedir, "cstrike_beta"))
		return "Counter-Strike";

	else if (!Q_stricmp(com_gamedir, "valve"))
		return "Half-Life";

	else if (!Q_stricmp(com_gamedir, "ricochet"))
		return "Ricochet";

	else if (!Q_stricmp(com_gamedir, "dod"))
		return "Day of Defeat";

	else if (!Q_stricmp(com_gamedir, "tfc"))
		return "Team Fortress Classic";

	else if (!Q_stricmp(com_gamedir, "dmc"))
		return "Deathmatch Classic";

	else if (!Q_stricmp(com_gamedir, "czero"))
		return "Condition Zero";

	return "Half-Life";
}

/* <90345> ../engine/sys_dll2.cpp:184 */
NOXREF void SetRateRegistrySetting(const char *pchRate)
{
	registry->WriteString("rate", pchRate);
}

/* <9036c> ../engine/sys_dll2.cpp:189 */
NOXREF const char *GetRateRegistrySetting(const char *pchDef)
{
	return registry->ReadString("rate", pchDef);
}

/* <90397> ../engine/sys_dll2.cpp:198 */
void EXPORT F(IEngineAPI **api)
{
	CreateInterfaceFn fn;
	fn = Sys_GetFactoryThis();
	*api = (IEngineAPI *)fn("VENGINE_LAUNCHER_API_VERSION002", NULL);
}

/* <903c2> ../engine/sys_dll2.cpp:245 */
void Sys_GetCDKey(char *pszCDKey, int *nLength, int *bDedicated)
{
	char key[65];
	char hostname[4096];

	if (CRehldsPlatformHolder::get()->gethostname(hostname, sizeof(hostname)))
		Q_snprintf(key, sizeof(key), "%u", RandomLong(0, 0x7FFFFFFF));
	else
	{
		struct hostent *hostinfo;
		hostinfo = CRehldsPlatformHolder::get()->gethostbyname(hostname);
		if (hostinfo && hostinfo->h_length == 4 && *hostinfo->h_addr_list != NULL)
		{
			Q_snprintf(key, sizeof(key), "%u.%u.%u.%u",
				(*hostinfo->h_addr_list)[0],
				(*hostinfo->h_addr_list)[1],
				(*hostinfo->h_addr_list)[2],
				(*hostinfo->h_addr_list)[3]);
		}
		else
		{
			CRC32_t crc;
#ifdef REHLDS_FIXES
			crc = 0;
#endif
			CRC32_ProcessBuffer(&crc, hostname, Q_strlen(hostname));
			Q_snprintf(key, sizeof(key), "%u", crc);
		}
	}
	key[64] = 0;
	Q_strcpy(pszCDKey, key);

	if (nLength)
		*nLength = Q_strlen(key);

	if (bDedicated)
		*bDedicated = 0;
}

/* <9049d> ../engine/sys_dll2.cpp:287 */
NOXREF void Legacy_ErrorMessage(int nLevel, const char *pszErrorMessage)
{
}

/* <90286> ../engine/sys_dll2.cpp:291 */
void Legacy_Sys_Printf(char *fmt, ...)
{
	va_list argptr;
	char text[1024];

	va_start(argptr, fmt);
	Q_vsnprintf(text, sizeof(text), fmt, argptr);
	va_end(argptr);

	if (dedicated_)
		dedicated_->Sys_Printf(text);
}

/* <904d8> ../engine/sys_dll2.cpp:307 */
NOXREF void Legacy_MP3subsys_Suspend_Audio(void)
{
}

/* <904f0> ../engine/sys_dll2.cpp:311 */
NOXREF void Legacy_MP3subsys_Resume_Audio(void)
{
}

/* <90508> ../engine/sys_dll2.cpp:315 */
void Sys_SetupLegacyAPIs(void)
{
#ifndef SWDS
	VID_FlipScreen = Sys_VID_FlipScreen;
	D_SurfaceCacheForRes = Sys_GetSurfaceCacheSize;
#endif // SWDS
	Launcher_ConsolePrintf = Legacy_Sys_Printf;
}

/* <90518> ../engine/sys_dll2.cpp:335 */
NOXREF int Sys_IsWin95(void)
{
#ifdef _WIN32
	return g_bIsWin95;
#else
	// TODO: no need to check is win
	return 0;
#endif // _WIN32
}

/* <90534> ../engine/sys_dll2.cpp:340 */
NOXREF int Sys_IsWin98(void)
{
#ifdef _WIN32
	return g_bIsWin98;
#else
	// TODO: no need to check is win
	return 0;
#endif // _WIN32
}

/* <90550> ../engine/sys_dll2.cpp:348 */
#ifdef _WIN32
NOXREF void Sys_CheckOSVersion(void)
{
	struct _OSVERSIONINFOA verInfo;

	Q_memset(&verInfo, 0, sizeof(verInfo));
	verInfo.dwOSVersionInfoSize = sizeof(verInfo);
	if (!GetVersionExA(&verInfo))
		Sys_Error("Couldn't get OS info");

	g_WinNTOrHigher = verInfo.dwMajorVersion >= 4;
	if (verInfo.dwPlatformId == 1 && verInfo.dwMajorVersion == 4)
	{
		if (verInfo.dwMinorVersion)
		{
			if (verInfo.dwMinorVersion < 90)
				g_bIsWin98 = 1;
		}
		else
		{
			g_bIsWin95 = 1;
		}
	}
}
#endif // _WIN32

/* <8fd53> ../engine/sys_dll2.cpp:397 */
void Sys_Init(void)
{
#ifndef SWDS
	Sys_InitFloatTime();
#endif
}

/* <8fd7b> ../engine/sys_dll2.cpp:445 */
void Sys_Shutdown(void)
{
#ifndef SWDS
	Sys_ShutdownFloatTime();
#endif // SWDS
	Steam_ShutdownClient();
#ifdef _WIN32
	if (g_PerfCounterInitialized)
	{
		DeleteCriticalSection(&g_PerfCounterMutex);
		g_PerfCounterInitialized = 0;
	}
#else
	#ifndef SWDS
		GL_Shutdown(*pmainwindow, maindc, baseRC);
	#endif // SWDS

#endif // _WIN32
}

/* <90588> ../engine/sys_dll2.cpp:475 */
void Sys_InitArgv(char *lpCmdLine)
{
	static char *argv[MAX_COMMAND_LINE_PARAMS];
	unsigned char c;
#ifdef REHLDS_FIXES
	bool inQuotes;
#endif

	argv[0] = "";
	c = *lpCmdLine;
	for (host_parms.argc = 1; c && host_parms.argc < MAX_COMMAND_LINE_PARAMS; c = *(++lpCmdLine))
	{
#ifdef REHLDS_FIXES
		// Skip whitespace
		while (c && c <= ' ')
		{
			lpCmdLine++;
			c = *lpCmdLine;
		}
		if (!c)
		{
			break;
		}

		// TODO: Add MultiByteToWideChar conversion under Windows, to correctly get UTF8, but need to alloc memory to store it
		// Store arg pointer
		argv[host_parms.argc] = lpCmdLine;
		host_parms.argc++;

		// Find end of the argument
		inQuotes = false;
		while (c > ' ' || (c && inQuotes))	// FIXED: Do not break quoted arguments
		{
			if (c == '"')
			{
				inQuotes = !inQuotes;
			}
			lpCmdLine++;
			c = *lpCmdLine;
		}
#else // REHLDS_FIXES
		// Skip whitespace and UTF8
		while (c && (c <= ' ' || c > '~'))
		{
			lpCmdLine++;
			c = *lpCmdLine;
		}
		if (!c)
		{
			break;
		}

		// Store arg pointer
		argv[host_parms.argc] = lpCmdLine;
		host_parms.argc++;

		// Find end of the argument
		while (c > ' ' && c <= '~')
		{
			lpCmdLine++;
			c = *lpCmdLine;
		}
#endif // REHLDS_FIXES
		if (!c)
		{
			break;
		}
		*lpCmdLine = 0;
	}

	host_parms.argv = argv;
	COM_InitArgv(host_parms.argc, argv);
	host_parms.argc = com_argc;
	host_parms.argv = com_argv;
}

/* <8fd20> ../engine/sys_dll2.cpp:514 */
NOXREF void Sys_ShutdownArgv(void)
{
}

/* <9066a> ../engine/sys_dll2.cpp:530 */
void Sys_InitMemory(void)
{
	int i;
	//bool bDidDefault;//unsued?

	i = COM_CheckParm("-heapsize");
	if (i && i < Cmd_Argc() - 1)
		host_parms.memsize = Q_atoi(Cmd_Argv(i + 1)) * 1024;

#ifdef _WIN32
	MEMORYSTATUS lpBuffer;
	if (host_parms.memsize < MINIMUM_WIN_MEMORY)
	{
		lpBuffer.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&lpBuffer);

		if (lpBuffer.dwTotalPhys)
		{
			if (lpBuffer.dwTotalPhys < FIFTEEN_MB)
				Sys_Error("Available memory less than 15MB!!! %i", i);

			host_parms.memsize = (int)(lpBuffer.dwTotalPhys >> 1);
			if ((signed int)host_parms.memsize < MINIMUM_WIN_MEMORY)
				host_parms.memsize = MINIMUM_WIN_MEMORY;
		}
		else
			host_parms.memsize = 0x8000000;

		if (g_bIsDedicatedServer)
			host_parms.memsize = MAXIMUM_DEDICATED_MEMORY;
	}
#else
	if ((signed int)host_parms.memsize <= 0xDFFFFFu)
		host_parms.memsize = MAXIMUM_DEDICATED_MEMORY;

#endif // _WIN32

	if ((signed int)host_parms.memsize > 0x8000000)
		host_parms.memsize = 0x8000000;

	if (COM_CheckParm("-minmemory"))
		host_parms.memsize = MINIMUM_WIN_MEMORY;
#ifdef _WIN32
	host_parms.membase = (void *)GlobalAlloc(GMEM_FIXED, host_parms.memsize);
#else
	host_parms.membase = Mem_Malloc(host_parms.memsize);
#endif // _WIN32

	if (!host_parms.membase)
		Sys_Error("Unable to allocate %.2f MB\n", (float)(host_parms.memsize) / (1024.0f * 1024.0f));
}

/* <906c2> ../engine/sys_dll2.cpp:626 */
void Sys_ShutdownMemory(void)
{
#ifdef _WIN32
	GlobalFree((HGLOBAL)host_parms.membase);
#else
	Mem_Free(host_parms.membase);
#endif // _WIN32
	host_parms.membase = NULL;
	host_parms.memsize = 0;
}

/* <906dd> ../engine/sys_dll2.cpp:644 */
void Sys_InitLauncherInterface(void)
{
#ifdef _WIN32
	//TODO: client-side code
	Launcher_ConsolePrintf = Legacy_Sys_Printf;
#else
	#ifndef SWDS
		gHasMMXTechnology = TRUE;
	#endif

	Sys_SetupLegacyAPIs();
#endif // _WIN32
}

/* <90702> ../engine/sys_dll2.cpp:666 */
NOXREF void Sys_ShutdownLauncherInterface(void)
{
}

/* <90712> ../engine/sys_dll2.cpp:673 */
void Sys_InitAuthentication(void)
{
	Sys_Printf("STEAM Auth Server\r\n");
}

/* <8fd71> ../engine/sys_dll2.cpp:678 */
NOXREF void Sys_ShutdownAuthentication(void)
{
}

/* <9073d> ../engine/sys_dll2.cpp:694 */
void Sys_ShowProgressTicks(char *specialProgressMsg)
{
	static bool recursionGuard = false;
	static int32 numTics = 0;

	double currentTime;
	if (!recursionGuard)
	{
		recursionGuard = true;
		if (COM_CheckParm("-steam"))
		{
			currentTime = Sys_FloatTime();

			if (g_flLastSteamProgressUpdateTime + 2.0 <= currentTime)
			{
				g_flLastSteamProgressUpdateTime = currentTime;
				numTics++;
				if (g_bIsDedicatedServer)
				{
					if (g_bMajorMapChange)
					{
						g_bPrintingKeepAliveDots = TRUE;
						Sys_Printf(".");
						recursionGuard = false;
						return;
					}
				}
				else
				{
					int i;
					int numTicsToPrint = (numTics % 5 + 1);
					char msg[128];

					Q_strncpy(msg, "Updating game resources", sizeof(msg) - 1);
					msg[sizeof(msg) - 1] = '\0';

					for (i = 1; i < numTicsToPrint; i++)
					{
						Q_strncat(msg, ".", sizeof(msg) - 1);
						msg[sizeof(msg) - 1] = '\0';
					}
					SetLoadingProgressBarStatusText(msg);
				}
			}
		}
		recursionGuard = false;
	}
}

/* <907de> ../engine/sys_dll2.cpp:748 */
int Sys_InitGame(char *lpOrgCmdLine, char *pBaseDir, void *pwnd, int bIsDedicated)
{
	host_initialized = FALSE;

#ifndef SWDS
	if (!bIsDedicated)
	{
		pmainwindow = (HWND *)pwnd;
#ifdef _WIN32
		videomode->UpdateWindowPosition();
#endif // _WIN32
	}
#endif // SWDS
	g_bIsDedicatedServer = bIsDedicated;
	Q_memset(&gmodinfo, 0, sizeof(modinfo_t));
	SV_ResetModInfo();
	TraceInit("Sys_Init()", "Sys_Shutdown()", 0);

#ifdef _WIN32
	Sys_InitHardwareTimer();
#endif // _WIN32

	Sys_CheckCpuInstructionsSupport();

#ifndef SWDS
	Sys_InitFloatTime();
#endif // SWDS
	FS_LogLevelLoadStarted("Launcher");
	SeedRandomNumberGenerator();
	TraceInit("Sys_InitMemory()", "Sys_ShutdownMemory()", 0);
	Sys_InitMemory();
	TraceInit("Sys_InitLauncherInterface()", "Sys_ShutdownLauncherInterface()", 0);
	Sys_InitLauncherInterface();

#ifndef SWDS
	if (!GL_SetMode(*pmainwindow, &maindc, &baseRC))
		return 0;
#endif // SWDS
	TraceInit("Host_Init( &host_parms )", "Host_Shutdown()", 0);
	Host_Init(&host_parms);
	if (!host_initialized)
		return 0;

	TraceInit("Sys_InitAuthentication()", "Sys_ShutdownAuthentication()", 0);
	Sys_InitAuthentication();
	if (g_bIsDedicatedServer)
	{
		Host_InitializeGameDLL();
		NET_Config(TRUE);
	}

#ifndef SWDS
	else
		ClientDLL_ActivateMouse();

	char MessageText[512];
	const char en_US[12];

	Q_strcpy(en_US, "en_US.UTF-8");
	en_US[16] = 0;

	char *cat = setlocale(6, NULL);
	if (!cat)
		cat = "c";

	if (Q_stricmp(cat, en_US) )
	{
		Q_snprintf(MessageText, sizeof(MessageText), "SetLocale('%s') failed. Using '%s'.\nYou may have limited glyph support.\nPlease install '%s' locale.", en_US, cat, en_US);
		SDL_ShowSimpleMessageBox(0, "Warning", MessageText, *pmainwindow);
	}
#endif // SWDS
	return 1;
}

/* <90871> ../engine/sys_dll2.cpp:841 */
void Sys_ShutdownGame(void)
{
	if (!g_bIsDedicatedServer)
		ClientDLL_DeactivateMouse();

	TraceShutdown("Host_Shutdown()", 0);
	Host_Shutdown();

	if (g_bIsDedicatedServer)
		NET_Config(FALSE);

	TraceShutdown("Sys_ShutdownLauncherInterface()", 0);
	TraceShutdown("Sys_ShutdownAuthentication()", 0);
	TraceShutdown("Sys_ShutdownMemory()", 0);
	Sys_ShutdownMemory();
	TraceShutdown("Sys_Shutdown()", 0);
	Sys_Shutdown();
}

/* <9089f> ../engine/sys_dll2.cpp:869 */
void ClearIOStates(void)
{
#ifndef SWDS
	int i;
	for (i = 0; i < 256; i++)
	{
		Key_Event(i, false);
	}
	Key_ClearStates();
	ClientDLL_ClearStates();
#endif // SWDS
}


/* <8f74a> ../engine/sys_dll2.cpp:982 */
class CEngineAPI : public IEngineAPI
{
public:
	/* <8f797> ../engine/sys_dll2.cpp:1004 */
	int Run(void *instance, char *basedir, char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
	{
		return 0;
	}
};

static CEngineAPI g_CEngineAPI;

/* <8ff0c> ../engine/sys_dll2.cpp:1034 */
IBaseInterface *CreateCEngineAPI(void)
{
	return &g_CEngineAPI;
};

InterfaceReg g_CreateCEngineAPI = InterfaceReg(CreateCEngineAPI, "VENGINE_LAUNCHER_API_VERSION002");

/* <908b7> ../engine/sys_dll2.cpp:1070 */
/* Needs rechecking
NOXREF int BuildMapCycleListHints(char **hints)
{
	char szMap[262];
	unsigned int length;
	char *pFileList;
	char szMod[MAX_PATH];
	char cszMapCycleTxtFile[MAX_PATH];
	FileHandle_t pFile;

	COM_FileBase(com_gamedir, szMod);
	Q_sprintf(cszMapCycleTxtFile, "%s/%s", szMod, mapcyclefile.string);
	pFile = FS_Open(cszMapCycleTxtFile, "rb");
	if (!pFile)
	{
		Con_Printf("Unable to open %s\n", cszMapCycleTxtFile);
		return 0;
	}
	Q_sprintf(szMap, "%s\\%s\\%s%s\r\n", szReslistsBaseDir, GetCurrentSteamAppName(), szCommonPreloads, szReslistsExt);

	*hints = (char *)Mem_Malloc(strlen(szMap) + 1);
	if (*hints == NULL)
	{
		Con_Printf("Unable to allocate memory for map cycle hints list");
		return 0;
	}
	Q_strcpy(*hints, szMap);
	length = FS_Size(pFile);
	if (length)
	{
		pFileList = (char *)malloc(length);
		if (pFileList && FS_Read(pFileList, length, 1, pFile) != 1)
		{
			while (1)
			{
				pFileList = COM_Parse(pFileList);
				if (strlen(com_token) <= 0)
				{
					Mem_Free(*hints);
					break;
				}

				Q_strncpy(szMap, com_token, sizeof(szMap) - 1);
				szMap[sizeof(szMap) - 1] = '\0';

				if (COM_TokenWaiting(pFileList))
					pFileList = COM_Parse(pFileList);

				char mapLine[sizeof(szMap)];
				Q_snprintf(mapLine, sizeof(mapLine), "%s\\%s\\%s%s\r\n", szReslistsBaseDir, szMod, szMap, szReslistsExt);

				*hints = (char *)Mem_Realloc(*hints, strlen(*hints) + 1 + strlen(mapLine) + 1);
				if (*hints == NULL)
				{
					Con_Printf("Unable to reallocate memory for map cycle hints list");
					return 0;
				}
				Q_strcat(*hints, mapLine);
			}
		}
	}
	FS_Close(pFile);
	Q_sprintf(szMap, "%s\\%s\\mp_maps.txt\r\n", szReslistsBaseDir, GetCurrentSteamAppName());
	*hints = (char *)Mem_Realloc(*hints, strlen(*hints) + 1 + strlen(szMap) + 1);
	Q_strcat(*hints, szMap);
	return 1;
}
*/

bool CDedicatedServerAPI::Init(char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	return Init_noVirt(basedir, cmdline, launcherFactory, filesystemFactory);
}

bool CDedicatedServerAPI::Init_noVirt(char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	dedicated_ = (IDedicatedExports *)launcherFactory(VENGINE_DEDICATEDEXPORTS_API_VERSION, NULL);
	if (!dedicated_)
		return false;

#ifdef REHLDS_CHECKS
	Q_strncpy(this->m_OrigCmd, cmdline, ARRAYSIZE(this->m_OrigCmd));
	this->m_OrigCmd[ARRAYSIZE(this->m_OrigCmd) - 1] = 0;
#else
	Q_strcpy(this->m_OrigCmd, cmdline);
#endif
	if (!Q_strstr(cmdline, "-nobreakpad"))
	{
		CRehldsPlatformHolder::get()->SteamAPI_UseBreakpadCrashHandler(va("%d", build_number()), "Aug  8 2013", "11:17:26", 0, 0, 0);
	}
	TraceInit("Sys_InitArgv( m_OrigCmd )", "Sys_ShutdownArgv()", 0);
	Sys_InitArgv(m_OrigCmd);
	eng->SetQuitting(IEngine::QUIT_NOTQUITTING);
	registry->Init();

	g_bIsDedicatedServer = TRUE;
	TraceInit("FileSystem_Init(basedir, (void *)filesystemFactory)", "FileSystem_Shutdown()", 0);
	if (FileSystem_Init(basedir, filesystemFactory) && game->Init(0) && eng->Load(true, basedir, cmdline))
	{
		char text[256];
		Q_snprintf(text, ARRAYSIZE(text), "exec %s\n", servercfgfile.string);
		text[255] = 0;
		Cbuf_InsertText(text);
		return true;
	}

	return false;
}

int CDedicatedServerAPI::Shutdown(void)
{
	return Shutdown_noVirt();
}

int CDedicatedServerAPI::Shutdown_noVirt(void)
{
	eng->Unload();
	game->Shutdown();

	TraceShutdown("FileSystem_Shutdown()", 0);
	FileSystem_Shutdown();

	registry->Shutdown();

	TraceShutdown("Sys_ShutdownArgv()", 0);
	dedicated_ = NULL;
	return giActive;
}

bool CDedicatedServerAPI::RunFrame(void)
{
	return RunFrame_noVirt();
}

bool CDedicatedServerAPI::RunFrame_noVirt(void)
{
	if (eng->GetQuitting())
		return false;

	eng->Frame();
	return true;
}

void CDedicatedServerAPI::AddConsoleText(char *text)
{
	AddConsoleText_noVirt(text);
}

void CDedicatedServerAPI::AddConsoleText_noVirt(char *text)
{
	Cbuf_AddText(text);
}

void CDedicatedServerAPI::UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, char *pszMap)
{
	UpdateStatus_noVirt(fps, nActive, nMaxPlayers, pszMap);
}

void CDedicatedServerAPI::UpdateStatus_noVirt(float *fps, int *nActive, int *nMaxPlayers, char *pszMap)
{
	Host_GetHostInfo(fps, nActive, NULL, nMaxPlayers, pszMap);
}

#ifndef HOOK_ENGINE

EXPOSE_SINGLE_INTERFACE(CDedicatedServerAPI, IDedicatedServerAPI, VENGINE_HLDS_API_VERSION);

#endif //HOOK_ENGINE

