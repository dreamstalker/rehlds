#include "precompiled.h"

class CSys: public ISys {
public:
	virtual ~CSys();

	void Sleep(int msec);
	bool GetExecutableName(char *out);
	void ErrorMessage(int level, const char *msg);

	void WriteStatusText(char *szText);
	void UpdateStatus(int force);

	long LoadLibrary(char *lib);
	void FreeLibrary(long library);

	bool CreateConsoleWindow();
	void DestroyConsoleWindow();

	void ConsoleOutput(char *string);
	char *ConsoleInput();
	void Printf(char *fmt, ...);
};

CSys g_Sys;
ISys *sys = &g_Sys;

CSys::~CSys()
{
	sys = nullptr;
}

void CSys::Sleep(int msec)
{
	::Sleep(msec);
}

bool CSys::GetExecutableName(char *out)
{
	if (!::GetModuleFileName((HINSTANCE)GetModuleHandle(NULL), out, 256))
		return false;

	return true;
}

void CSys::ErrorMessage(int level, const char *msg)
{
	MessageBox(NULL, msg, "Half-Life Dedicated Server Error", MB_OK);
	PostQuitMessage(0);
}

void CSys::WriteStatusText(char *szText)
{
	SetConsoleTitle(szText);
}

void CSys::UpdateStatus(int force)
{
	static double tLast = 0.0;
	double tCurrent;
	char szStatus[256];
	int n, nMax;
	char szMap[32];
	float fps;

	if (!engineAPI)
		return;

	tCurrent = (double)timeGetTime() * 0.001;
	engineAPI->UpdateStatus(&fps, &n, &nMax, szMap);

	if (!force)
	{
		if ((tCurrent - tLast) < 0.5f)
			return;
	}

	tLast = tCurrent;
	_snprintf(szStatus, sizeof(szStatus), "%.1f fps %2i/%2i on %16s", fps, n, nMax, szMap);

	console.SetStatusLine(szStatus);
	console.UpdateStatus();
}

long CSys::LoadLibrary(char *lib)
{
	void *hDll = ::LoadLibrary(lib);
	return (long)hDll;
}

void CSys::FreeLibrary(long library)
{
	if (!library)
		return;

	::FreeLibrary((HMODULE)library);
}

bool CSys::CreateConsoleWindow()
{
#if 0
	if (!AllocConsole())
	{
		return false;
	}
#endif

	InitConProc();
	return true;
}

void CSys::DestroyConsoleWindow()
{
	FreeConsole();

	// shut down QHOST hooks if necessary
	DeinitConProc();
}

void CSys::ConsoleOutput(char *string)
{
	if (g_bVGui)
	{
		VGUIPrintf(string);
	}
	else
	{
		console.Print(string);
	}
}

char *CSys::ConsoleInput()
{
	return console.GetLine();
}

void CSys::Printf(char *fmt, ...)
{
	// Dump text to debugging console.
	va_list argptr;
	char szText[1024];

	va_start(argptr, fmt);
	_vsnprintf(szText, sizeof(szText), fmt, argptr);
	va_end(argptr);

	// Get Current text and append it.
	ConsoleOutput(szText);
}

int StartServer()
{
	// Startup winock
	WORD version = MAKEWORD(2, 0);
	WSADATA wsaData;
	WSAStartup(version, &wsaData);

	CommandLine()->CreateCmdLine(GetCommandLine());

	// Load engine
	g_pEngineModule = Sys_LoadModule(ENGINE_LIB);
	if (!g_pEngineModule)
	{
		MessageBox(NULL, "Unable to load engine, image is corrupt.", "Half-Life Dedicated Server Error", MB_OK);
		return -1;
	}

	g_pFileSystemModule = Sys_LoadModule(STDIO_FILESYSTEM_LIB);

	// Get FileSystem interface
	g_FilesystemFactoryFn = Sys_GetFactory(g_pFileSystemModule);
	if (!g_FilesystemFactoryFn)
		return -1;

	IFileSystem *pFullFileSystem = (IFileSystem *)g_FilesystemFactoryFn(FILESYSTEM_INTERFACE_VERSION, NULL);
	if (!pFullFileSystem)
		return -1;

	pFullFileSystem->Mount();

	char *pszValue = nullptr;
	if (CommandLine()->CheckParm("-steam")
	|| (CommandLine()->CheckParm("-console", &pszValue) && !pszValue)) {
		g_bVGui = true;
		StartVGUI();
	}
	else
	{
		if (!console.Init()) {
			MessageBox(NULL, "Failed to initialize console.", "Half-Life Dedicated Server Error", MB_OK);
			return 0;
		}

		if (!sys->CreateConsoleWindow()) {
			return 0;
		}

		console.SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);

		if (!SetConsoleCtrlHandler(MyHandlerRoutine, TRUE)) {
			MessageBox(NULL, "Unable to set control handler", "Half-Life Dedicated Server Error", MB_OK);
			return 0;
		}
	}

	gbAppHasBeenTerminated = false;
	Load3rdParty();

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
	{
		RunServer();
	}

	if (gpszCvars)
		free(gpszCvars);

	if (pFullFileSystem)
		pFullFileSystem->Unmount();

	Sys_UnloadModule(g_pFileSystemModule);

	if (hDLLThirdParty)
	{
		Sys_UnloadModule((CSysModule *)hDLLThirdParty);
		hDLLThirdParty = 0L;
	}

	WSACleanup();
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (ShouldLaunchAppViaSteam(lpCmdLine, STDIO_FILESYSTEM_LIB, STDIO_FILESYSTEM_LIB))
		return 0;

	auto command = CommandLineToArgvW(GetCommandLineW(), (int *)&lpCmdLine);
	auto ret = StartServer();
	LocalFree(command);
	return ret;
}

BOOL WINAPI MyHandlerRoutine(DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		gbAppHasBeenTerminated = true;
		return TRUE;
	}

	return FALSE;
}
