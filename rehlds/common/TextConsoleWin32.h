#ifndef TEXTCONSOLE_WIN32_H
#define TEXTCONSOLE_WIN32_H
#ifdef _WIN32
#pragma once
#endif

#include <windows.h>
#include "TextConsole.h"

class CTextConsoleWin32: public CTextConsole {
public:
	virtual ~CTextConsoleWin32();

	bool Init(IBaseSystem *system = nullptr);
	void ShutDown();

	void SetTitle(char *pszTitle);
	void SetStatusLine(char *pszStatus);
	void UpdateStatus();

	void PrintRaw(char * pszMsz, int nChars = 0);
	void Echo(char * pszMsz, int nChars = 0);
	char *GetLine();
	int GetWidth();

	void SetVisible(bool visible);
	void SetColor(WORD);

private:
	HANDLE hinput;		// standard input handle
	HANDLE houtput;		// standard output handle
	WORD Attrib;		// attrib colours for status bar

	char statusline[81];	// first line in console is status line
};

extern CTextConsoleWin32 console;

#endif // TEXTCONSOLE_WIN32_H
