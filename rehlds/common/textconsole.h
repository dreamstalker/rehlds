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

#pragma once

#include "IBaseSystem.h"

#define MAX_CONSOLE_TEXTLEN 256
#define MAX_BUFFER_LINES 30

class CTextConsole {
public:
	virtual ~CTextConsole() {}

	virtual bool Init(IBaseSystem *system = nullptr);
	virtual void ShutDown();
	virtual void Print(char *pszMsg);

	virtual void SetTitle(char *pszTitle) {}
	virtual void SetStatusLine(char *pszStatus) {}
	virtual void UpdateStatus() {}

	// Must be provided by children
	virtual void PrintRaw(char *pszMsg, int nChars = 0) = 0;
	virtual void Echo(char *pszMsg, int nChars = 0) = 0;
	virtual char *GetLine() = 0;
	virtual int GetWidth() = 0;

	virtual void SetVisible(bool visible);
	virtual bool IsVisible();

protected:
	char m_szConsoleText[MAX_CONSOLE_TEXTLEN];			// console text buffer
	int m_nConsoleTextLen;						// console textbuffer length
	int m_nCursorPosition;						// position in the current input line

	// Saved input data when scrolling back through command history
	char m_szSavedConsoleText[MAX_CONSOLE_TEXTLEN];			// console text buffer
	int m_nSavedConsoleTextLen;					// console textbuffer length

	char m_aszLineBuffer[MAX_BUFFER_LINES][MAX_CONSOLE_TEXTLEN];	// command buffer last MAX_BUFFER_LINES commands
	int m_nInputLine;						// Current line being entered
	int m_nBrowseLine;						// current buffer line for up/down arrow
	int m_nTotalLines;						// # of nonempty lines in the buffer

	bool m_ConsoleVisible;

	IBaseSystem *m_System;

	int ReceiveNewline();
	void ReceiveBackspace();
	void ReceiveTab();
	void ReceiveStandardChar(const char ch);
	void ReceiveUpArrow();
	void ReceiveDownArrow();
	void ReceiveLeftArrow();
	void ReceiveRightArrow();
};

#include "SteamAppStartUp.h"

#if defined(_WIN32)
	#include "TextConsoleWin32.h"
#else
	#include "TextConsoleUnix.h"
#endif // defined(_WIN32)

void Sys_Printf(char *fmt, ...);
