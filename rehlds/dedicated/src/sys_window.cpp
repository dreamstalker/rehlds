#include "precompiled.h"

class CSys: public ISys {
public:
	CSys();
	virtual ~CSys();

	void Sleep(int msec) override;
	bool GetExecutableName(char *out) override;
	void ErrorMessage(int level, const char *msg) override;

	void WriteStatusText(char *szText) override;
	void UpdateStatus(int force) override;

	long LoadLibrary(char *lib) override;
	void FreeLibrary(long library) override;

	bool CreateConsoleWindow() override;
	void DestroyConsoleWindow() override;

	void ConsoleOutput(char *string) override;
	char *ConsoleInput() override;
	void Printf(char *fmt, ...) override;
};

CSys g_Sys;
ISys *sys = &g_Sys;

CSys::CSys()
{
	// Startup winock
	WORD version = MAKEWORD(2, 0);
	WSADATA wsaData;
	WSAStartup(version, &wsaData);
}

CSys::~CSys()
{
	WSACleanup();
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

BOOL WINAPI ConsoleCtrlHandler(DWORD CtrlType)
{
	switch (CtrlType) {
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		g_bAppHasBeenTerminated = true;
		return TRUE;
	default:
		break;
	}

	return FALSE;
}

bool Sys_SetupConsole()
{
	console.SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);

	if (!SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE)) {
		sys->ErrorMessage(0, "Unable to set control handler.");
		return false;
	}

	return true;
}

void Sys_PrepareConsoleInput()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (!GetMessage(&msg, NULL, 0, 0)) {
			break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Sys_InitPingboost()
{
	
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (ShouldLaunchAppViaSteam(lpCmdLine, STDIO_FILESYSTEM_LIB, STDIO_FILESYSTEM_LIB))
		return 0;

	//auto command = CommandLineToArgvW(GetCommandLineW(), (int *)&lpCmdLine);
	auto ret = StartServer(lpCmdLine);
	//LocalFree(command);
	return ret;
}
