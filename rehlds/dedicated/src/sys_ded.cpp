#include "precompiled.h"

long hDLLThirdParty = 0L;

bool g_bVGui = false;
char *gpszCvars = nullptr;
bool gbAppHasBeenTerminated = false;

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
	if (g_bVGui)
	{
		VGUIPrintf(szText);
	}
	else
	{
		console.Print(szText);
	}
}

void Load3rdParty()
{
	// Only do this if the server operator wants the support.
	// ( In case of malicious code, too )
	if (CommandLine()->CheckParm("-usegh"))
	{
		hDLLThirdParty = sys->LoadLibrary("ghostinj.dll");
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
		_snprintf(szBuf, sizeof(szBuf), "%s\n", inputLine);
		engineAPI->AddConsoleText(szBuf);
	}
}

char *UTIL_GetBaseDir()
{
	return ".";
}

// Server Frame
int RunServer()
{
#ifdef _WIN32
	// TODO: finish me!
	/*if (g_bVGui)
	{
		vgui::ivgui()->SetSleep(0);
	}*/
#endif

	while (true)
	{
		if (gbAppHasBeenTerminated)
			break;

		CreateInterfaceFn engineFactory = Sys_GetFactory(g_pEngineModule);
		RunVGUIFrame();

		if (engineFactory)
		{
			engineAPI = (IDedicatedServerAPI *)engineFactory(VENGINE_HLDS_API_VERSION, NULL);
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
		if (!engineAPI)
			return -1;

		if (!engineAPI->Init(UTIL_GetBaseDir(), (char *)CommandLine()->GetCmdLine(), Sys_GetFactoryThis(), g_FilesystemFactoryFn)) {
			return -1;
		}

		RunVGUIFrame();

#ifdef _WIN32
		// TODO: finish me!
		/*if (g_bVGui)
		{
			g_pFileSystemInterface->AddSearchPath("platform", "PLATFORM");

			// find our configuration directory
			char szConfigDir[512];
			const char *steamPath = getenv("SteamInstallPath");
			if (steamPath)
			{
				// put the config dir directly under steam
				_snprintf(szConfigDir, sizeof(szConfigDir), "%s/config", steamPath);
			}
			else
			{
				// we're not running steam, so just put the config dir under the platform
				strncpy(szConfigDir, "platform/config", sizeof(szConfigDir));
			}
		}*/
#endif // _WIN32

		RunVGUIFrame();

		if (gpszCvars) {
			engineAPI->AddConsoleText(gpszCvars);
		}

		VGUIFinishedConfig();
		RunVGUIFrame();

		bool bDone = false;
		while (!bDone)
		{
			// Running really fast, yield some time to other apps
			sys->Sleep(1);

#ifdef _WIN32
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!GetMessage(&msg, NULL, 0, 0))
				{
					bDone = true;
					break;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (gbAppHasBeenTerminated)
				break;

			if (g_bVGui)
			{
				RunVGUIFrame();
			}
			else
#endif // _WIN32
			{
				ProcessConsoleInput();
			}

			if (!engineAPI->RunFrame())
			{
				bDone = true;
			}

			sys->UpdateStatus(0 /* don't force */);
		}

		if (g_bVGui)
		{
			RunVGUIFrame();
			StopVGUI();
		}
		else
		{
			sys->DestroyConsoleWindow();
			console.ShutDown();
		}

		auto iret = engineAPI->Shutdown();
		Sys_UnloadModule(g_pEngineModule);
		VGUIFinishedConfig();

		if (iret == DLL_CLOSE)
			return iret;
	}

	return 0;
}
