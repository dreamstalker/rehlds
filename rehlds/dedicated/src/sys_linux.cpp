#include "precompiled.h"

class CSys: public ISys {
public:
	virtual ~CSys();

	void Sleep(int msec);
	bool GetExecutableName(char *out);
	NORETURN void ErrorMessage(int level, const char *msg);

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
char g_szEXEName[MAX_PATH];

SleepType Sys_Sleep;
NET_Sleep_t NET_Sleep_Timeout = NULL;

CSys::~CSys()
{
	sys = nullptr;
}

// this checks if pause has run yet,
// tell the compiler it can change at any time
volatile bool g_bPaused = false;

void CSys::Sleep(int msec)
{
	usleep(msec * 1000);
}

void Sleep_Old(int msec)
{
	usleep(msec * 1000);
}

void Sleep_Select(int msec)
{
	struct timeval tv;

	// Assumes msec < 1000
	tv.tv_sec = 0;
	tv.tv_usec = 1000 * msec;

	select(1, NULL, NULL, NULL, &tv);
}

void Sleep_Net(int msec)
{
	if (NET_Sleep_Timeout)
	{
		NET_Sleep_Timeout();
		return;
	}

	// NET_Sleep_Timeout isn't hooked yet, fallback to the old method
	Sleep_Old(msec);
}

// linux runs on a 100Hz scheduling clock, so the minimum latency from
// usleep is 10msec. However, people want lower latency than this..
//
// There are a few solutions, one is to use the realtime scheduler in the
// kernel BUT this needs root privelleges to start. It also can play
// unfriendly with other programs.
//
// Another solution is to use software timers, they use the RTC of the
// system and are accurate to microseconds (or so).
//
// timers, via setitimer() are used here
void Sleep_Timer(int msec)
{
	struct itimerval tm;

	tm.it_value.tv_sec = msec / 1000;		// convert msec to seconds
	tm.it_value.tv_usec = (msec % 1000) * 1E3;	// get the number of msecs and change to micros
	tm.it_interval.tv_sec = 0;
	tm.it_interval.tv_usec = 0;

	g_bPaused = false;

	// set the timer to trigger
	if (setitimer(ITIMER_REAL, &tm, NULL)) {
		// wait for the signal
		pause();
	}

	g_bPaused = true;
}

void alarmFunc(int num)
{
	// reset the signal handler
	signal(SIGALRM, alarmFunc);

	// paused is 0, the timer has fired before the pause was called... Lets queue it again
	if (!g_bPaused)
	{
		struct itimerval itim;
		itim.it_interval.tv_sec = 0;
		itim.it_interval.tv_usec = 0;
		itim.it_value.tv_sec = 0;
		itim.it_value.tv_usec = 1000; // get it to run again real soon
		setitimer(ITIMER_REAL, &itim, 0);
	}

}

bool CSys::GetExecutableName(char *out)
{
	strcpy(out, g_szEXEName);
	return true;
}

// Engine is erroring out, display error in message box
void CSys::ErrorMessage(int level, const char *msg)
{
	puts(msg);
	exit(-1);
}

void CSys::WriteStatusText(char *szText)
{
}

void CSys::UpdateStatus(int force)
{
}

long CSys::LoadLibrary(char *lib)
{
	char cwd[1024];
	char absolute_lib[1024];

	if (!getcwd(cwd, sizeof(cwd)))
		ErrorMessage(1, "Sys_LoadLibrary: Couldn't determine current directory.");

	if (cwd[strlen(cwd) - 1] == '/')
		cwd[strlen(cwd) - 1] = '\0';

	_snprintf(absolute_lib, sizeof(absolute_lib), "%s/%s", cwd, lib);

	void *hDll = dlopen(absolute_lib, RTLD_NOW);
	if (!hDll)
	{
		ErrorMessage(1, dlerror());
	}

	return (long)hDll;
}

void CSys::FreeLibrary(long library)
{
	if (!library)
		return;

	dlclose((void *)library);
}

bool CSys::CreateConsoleWindow()
{
	return true;
}

void CSys::DestroyConsoleWindow()
{
}

// Print text to the dedicated console
void CSys::ConsoleOutput(char *string)
{
	printf("%s", string);
	fflush(stdout);
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

#define MAX_LINUX_CMDLINE 2048
static char linuxCmdline[ MAX_LINUX_CMDLINE ];

void BuildCmdLine(int argc, char **argv)
{
	int len;
	int i;

	for (len = 0, i = 1; i < argc; i++)
	{
		len += strlen(argv[i]) + 1;
	}

	if (len > MAX_LINUX_CMDLINE)
	{
		printf("command line too long, %i max\n", MAX_LINUX_CMDLINE);
		exit(-1);
		return;
	}

	linuxCmdline[0] = '\0';
	for (i = 1; i < argc; i++)
	{
		if (i > 1) {
			strcat(linuxCmdline, " ");
		}

		strcat(linuxCmdline, argv[ i ]);
	}
}

char *GetCommandLine()
{
	return linuxCmdline;
}

int main(int argc, char *argv[])
{
	_snprintf(g_szEXEName, sizeof(g_szEXEName), "%s", argv[0]);
	BuildCmdLine(argc, argv);

	CommandLine()->CreateCmdLine(::GetCommandLine());
	CommandLine()->AppendParm("-steam", NULL);

	// Load engine
	g_pEngineModule = Sys_LoadModule(ENGINE_LIB);
	if (!g_pEngineModule)
	{
		sys->ErrorMessage(1, "Unable to load engine, image is corrupt.");
		return -1;
	}

	Sys_Sleep = Sleep_Old;

	char *pPingType;
	int type;
	if (CommandLine()->CheckParm("-pingboost", &pPingType) && pPingType)
	{
		type = atoi(pPingType);
		switch (type)
		{
		case 1:
			signal(SIGALRM, alarmFunc);
			Sys_Sleep = Sleep_Timer;
			break;
		case 2:
			Sys_Sleep = Sleep_Select;
			break;
		case 3:
			Sys_Sleep = Sleep_Net;

			// we Sys_GetProcAddress NET_Sleep() from
			//engine_i486.so later in this function
			NET_Sleep_Timeout = (NET_Sleep_t)Sys_GetProcAddress(g_pEngineModule, "NET_Sleep_Timeout");
			break;
		// just in case
		default:
			Sys_Sleep = Sleep_Old;
			break;
		}
	}

	char *fsmodule;
	if (CommandLine()->CheckParm("-pidfile", &fsmodule) && fsmodule)
	{
		FILE *pidFile = fopen(fsmodule, "w");
		if (pidFile) {
			fprintf(pidFile, "%i\n", getpid());
			fclose(pidFile);
		}
		else
			printf("Warning: unable to open pidfile (%s)\n", pPingType);
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

	if (!console.Init()) {
		puts("Failed to initilise console.");
		return 0;
	}

	gbAppHasBeenTerminated = false;
	RunServer();

	if (gpszCvars)
		free(gpszCvars);

	if (pFullFileSystem)
		pFullFileSystem->Unmount();

	Sys_UnloadModule(g_pFileSystemModule);

	exit(0);
	return 0;
}
