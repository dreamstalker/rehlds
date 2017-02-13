#ifndef ISYS_H
#define ISYS_H
#ifdef _WIN32
#pragma once
#endif

class ISys {
public:
	virtual ~ISys() {}

	virtual	void Sleep(int msec) = 0;
	virtual bool GetExecutableName(char *out) = 0;
	virtual void ErrorMessage(int level, const char *msg) = 0;

	virtual void WriteStatusText(char *szText) = 0;
	virtual void UpdateStatus(int force) = 0;

	virtual long LoadLibrary(char *lib) = 0;
	virtual void FreeLibrary(long library) = 0;

	virtual bool CreateConsoleWindow(void) = 0;
	virtual void DestroyConsoleWindow(void) = 0;

	virtual void ConsoleOutput(char *string) = 0;
	virtual char *ConsoleInput(void) = 0;
	virtual void Printf(char *fmt, ...) = 0;
};

extern ISys *sys;

#endif // ISYS_H
