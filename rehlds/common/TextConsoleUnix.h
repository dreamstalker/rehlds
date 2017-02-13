#ifndef TEXTCONSOLE_UNIX_H
#define TEXTCONSOLE_UNIX_H
#ifdef _WIN32
#pragma once
#endif

#include <termios.h>
#include "textconsole.h"

enum escape_sequence_t
{
	ESCAPE_CLEAR,
	ESCAPE_RECEIVED,
	ESCAPE_BRACKET_RECEIVED
};

class CTextConsoleUnix: public CTextConsole {
public:
	virtual ~CTextConsoleUnix();

	bool Init(IBaseSystem *system = nullptr);
	void ShutDown();
	void PrintRaw(char *pszMsg, int nChars = 0);
	void Echo(char *pszMsg, int nChars = 0);
	char *GetLine();
	int GetWidth();

private:
	int kbhit();

	struct termios termStored;
	FILE *tty;
};

extern CTextConsoleUnix console;

#endif // TEXTCONSOLE_UNIX_H
