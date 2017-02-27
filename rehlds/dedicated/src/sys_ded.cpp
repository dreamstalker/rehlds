#include "precompiled.h"

bool g_bVGui = false;
char *gpszCvars = nullptr;
bool g_bAppHasBeenTerminated = false;

CSysModule *g_pEngineModule = nullptr;
IDedicatedServerAPI *engineAPI = nullptr;
#ifdef LAUNCHER_FIXES
IRehldsApi *rehldsApi = nullptr;
const RehldsFuncs_t* rehldsFuncs = nullptr;
#endif

IFileSystem *g_pFileSystemInterface = nullptr;
CSysModule *g_pFileSystemModule = nullptr;

CreateInterfaceFn g_FilesystemFactoryFn;

void Sys_Printf_Safe(char *text)
{
	if (sys)
	{
		sys->Printf("%s", text);
	}
}

void Sys_Printf(char *fmt, ...)
{
	// Dump text to debugging console.
	va_list argptr;
	char szText[1024];

	va_start(argptr, fmt);
	_vsnprintf(szText, sizeof(szText), fmt, argptr);
	va_end(argptr);

	// Get Current text and append it.
#ifdef VGUI
	if (g_bVGui)
	{
		VGUIPrintf(szText);
	}
	else
#endif
	{
		console.Print(szText);
	}
}

void ProcessConsoleInput()
{
	if (!engineAPI)
		return;

	char *inputLine = console.GetLine();
	if (inputLine)
	{
		char szBuf[256];
		_snprintf(szBuf, sizeof szBuf, "%s\n", inputLine);
		engineAPI->AddConsoleText(szBuf);
	}
}

char *UTIL_GetBaseDir()
{
	return ".";
}

int RunEngine()
{
#ifdef VGUI
	if (g_bVGui)
	{
		// TODO: finish VGUI
#endif

	CreateInterfaceFn engineFactory = Sys_GetFactory(g_pEngineModule);
	RunVGUIFrame();

	if (engineFactory)
	{
		engineAPI = (IDedicatedServerAPI *)engineFactory(VENGINE_HLDS_API_VERSION, nullptr);
#ifdef LAUNCHER_FIXES
		rehldsApi = (IRehldsApi *)engineFactory(VREHLDS_HLDS_API_VERSION, NULL);
		if (rehldsApi)
		{
			if (rehldsApi->GetMajorVersion() != REHLDS_API_VERSION_MAJOR || rehldsApi->GetMinorVersion() < REHLDS_API_VERSION_MINOR)
			{
				rehldsApi = nullptr;
			}
			else
			{
				rehldsFuncs = rehldsApi->GetFuncs();
			}
		}
#endif
	}
	RunVGUIFrame();
	if (!engineAPI || !engineAPI->Init(UTIL_GetBaseDir(), (char *)CommandLine()->GetCmdLine(), Sys_GetFactoryThis(), g_FilesystemFactoryFn)) {
		return LAUNCHER_ERROR;
	}

	RunVGUIFrame();

#ifdef VGUI
	// TODO: finish me!
	/*if (g_bVGui) {
		g_pFileSystemInterface->AddSearchPath("platform", "PLATFORM");

		// find our configuration directory
		char szConfigDir[512];
		const char *steamPath = getenv("SteamInstallPath");
		if (steamPath) {
			// put the config dir directly under steam
			_snprintf(szConfigDir, sizeof(szConfigDir), "%s/config", steamPath);
		}
		else {
			// we're not running steam, so just put the config dir under the platform
			strncpy(szConfigDir, "platform/config", sizeof(szConfigDir));
		}
	}*/
#endif // VGUI

	RunVGUIFrame();

	if (gpszCvars) {
		engineAPI->AddConsoleText(gpszCvars);
	}

	VGUIFinishedConfig();
	RunVGUIFrame();

	bool bDone = false;
	while (!bDone) {
		// Running really fast, yield some time to other apps
		sys->Sleep(1);

		Sys_PrepareConsoleInput();


		if (g_bAppHasBeenTerminated)
			break;

#ifdef VGUI
		if (g_bVGui) {
			RunVGUIFrame();
		}
		else
#endif
		{
			ProcessConsoleInput();
		}

		bDone = !engineAPI->RunFrame();
		sys->UpdateStatus(FALSE /* don't force */);
	}

#ifdef VGUI
	if (g_bVGui) {
		RunVGUIFrame();
		StopVGUI();
	}
	else
#endif
	{
		sys->DestroyConsoleWindow();
		console.ShutDown();
	}

	int iret = engineAPI->Shutdown();
	VGUIFinishedConfig();

	if (iret == DLL_CLOSE)
		g_bAppHasBeenTerminated = true;

	return LAUNCHER_OK;
}

int StartServer(char* cmdline)
{
	g_bAppHasBeenTerminated = false;

	do {
		CommandLine()->CreateCmdLine(cmdline);
		CommandLine()->AppendParm("-steam", nullptr);

		Sys_InitPingboost();

		// Load engine
		g_pEngineModule = Sys_LoadModule(ENGINE_LIB);
		if (!g_pEngineModule) {
			sys->ErrorMessage(0, "Unable to load engine, image is corrupt.");
			return LAUNCHER_ERROR;
		}

		// Load filesystem
		g_pFileSystemModule = Sys_LoadModule(STDIO_FILESYSTEM_LIB);
		if (!g_pFileSystemModule) {
			sys->ErrorMessage(0, "Unable to load filesystem, image is corrupt.");
			return LAUNCHER_ERROR;
		}

		// Get filesystem factory
		g_FilesystemFactoryFn = Sys_GetFactory(g_pFileSystemModule);
		if (!g_FilesystemFactoryFn) {
			sys->ErrorMessage(0, "Unable to get filesystem factory.");
			return LAUNCHER_ERROR;
		}

		// Retrieve filesystem interface
		IFileSystem *pFullFileSystem = (IFileSystem *)g_FilesystemFactoryFn(FILESYSTEM_INTERFACE_VERSION, nullptr);
		if (!pFullFileSystem) {
			sys->ErrorMessage(0, "Can not retrive filesystem interface version '" FILESYSTEM_INTERFACE_VERSION "'.\n");
			return LAUNCHER_ERROR;
		}

		// Mount filesystem
		pFullFileSystem->Mount();

		// Init VGUI or Console mode
#ifdef VGUI
		char *pszValue = nullptr;
		if (!CommandLine()->CheckParm("-console", &pszValue)) {
			g_bVGui = true;
			StartVGUI();
		}
		else
#endif
		{
			if (!console.Init()) {
				sys->ErrorMessage(0, "Failed to initialize console."); // TODO: add more details
				return LAUNCHER_ERROR;
			}

			if (!sys->CreateConsoleWindow()) {
				sys->ErrorMessage(0, "Failed to create console window.");
				return LAUNCHER_ERROR;
			}

			if (!Sys_SetupConsole())
				return LAUNCHER_ERROR;
		}

#ifdef VGUI
		// TODO: finish me!
		/*// run vgui
		if (g_bVGui)
		{
		while (VGUIIsInConfig()	&& VGUIIsRunning())
		{
		RunVGUIFrame();
		}
		}
		else*/
#endif
		{
			int ret = RunEngine();

			if (ret == LAUNCHER_ERROR) {
				sys->ErrorMessage(0, "Failed to launch engine.\n");
				return LAUNCHER_ERROR;
			}
		}

		if (gpszCvars)
			free(gpszCvars);

		// Unmount filesystem
		pFullFileSystem->Unmount();

		// Unload modules
		Sys_UnloadModule(g_pFileSystemModule);
		Sys_UnloadModule(g_pEngineModule);

	} while (!g_bAppHasBeenTerminated);

	return LAUNCHER_OK;
}
