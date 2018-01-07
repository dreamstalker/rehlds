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

HANDLE heventDone;
HANDLE hfileBuffer;
HANDLE heventChildSend;
HANDLE heventParentSend;
HANDLE hStdout;
HANDLE hStdin;

BOOL SetConsoleCXCY(HANDLE hStdout, int cx, int cy)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	COORD coordMax = GetLargestConsoleWindowSize(hStdout);

	if (cy > coordMax.Y)
		cy = coordMax.Y;

	if (cx > coordMax.X)
		cx = coordMax.X;

	if (!GetConsoleScreenBufferInfo(hStdout, &info))
		return FALSE;

	// height
	info.srWindow.Left = 0;
	info.srWindow.Right = info.dwSize.X - 1;
	info.srWindow.Top = 0;
	info.srWindow.Bottom = cy - 1;

	if (cy < info.dwSize.Y)
	{
		if (!SetConsoleWindowInfo(hStdout, TRUE, &info.srWindow))
			return FALSE;

		info.dwSize.Y = cy;

		if (!SetConsoleScreenBufferSize(hStdout, info.dwSize))
			return FALSE;
	}
	else if (cy > info.dwSize.Y)
	{
		info.dwSize.Y = cy;

		if (!SetConsoleScreenBufferSize(hStdout, info.dwSize))
			return FALSE;

		if (!SetConsoleWindowInfo(hStdout, TRUE, &info.srWindow))
			return FALSE;
	}

	if (!GetConsoleScreenBufferInfo(hStdout, &info))
		return FALSE;

	// width
	info.srWindow.Left = 0;
	info.srWindow.Right = cx - 1;
	info.srWindow.Top = 0;
	info.srWindow.Bottom = info.dwSize.Y - 1;

	if (cx < info.dwSize.X)
	{
		if (!SetConsoleWindowInfo(hStdout, TRUE, &info.srWindow))
			return FALSE;

		info.dwSize.X = cx;

		if (!SetConsoleScreenBufferSize(hStdout, info.dwSize))
			return FALSE;
	}
	else if (cx > info.dwSize.X)
	{
		info.dwSize.X = cx;

		if (!SetConsoleScreenBufferSize(hStdout, info.dwSize))
			return FALSE;

		if (!SetConsoleWindowInfo(hStdout, TRUE, &info.srWindow))
			return FALSE;
	}

	return TRUE;
}

LPVOID GetMappedBuffer(HANDLE hfileBuffer)
{
	return MapViewOfFile(hfileBuffer, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
}

void ReleaseMappedBuffer(LPVOID pBuffer)
{
	UnmapViewOfFile(pBuffer);
}

BOOL GetScreenBufferLines(int *piLines)
{
	CONSOLE_SCREEN_BUFFER_INFO info;

	BOOL bRet = GetConsoleScreenBufferInfo(hStdout, &info);
	if (bRet)
		*piLines = info.dwSize.Y;

	return bRet;
}

BOOL SetScreenBufferLines(int iLines)
{
	return SetConsoleCXCY(hStdout, 80, iLines);
}

BOOL ReadText(LPTSTR pszText, int iBeginLine, int iEndLine)
{
	COORD coord;
	DWORD dwRead;

	coord.X = 0;
	coord.Y = iBeginLine;

	BOOL bRet = ReadConsoleOutputCharacter(hStdout, pszText, 80 * (iEndLine - iBeginLine + 1), coord, &dwRead);

	// Make sure it's null terminated.
	if (bRet)
		pszText[dwRead] = '\0';

	return bRet;
}

int CharToCode(char c)
{
	char upper = toupper(c);
	switch (c)
	{
	case 13:
		return 28;
	default:
		break;
	}

	if (isalpha(c))
		return (30 + upper - 65);

	if (isdigit(c))
		return (1 + upper - 47);

	return c;
}

BOOL WriteText(LPCTSTR szText)
{
	DWORD dwWritten;
	INPUT_RECORD rec;
	char upper, *sz;

	sz = (LPTSTR)szText;

	while (*sz)
	{
		// 13 is the code for a carriage return (\n) instead of 10.
		if (*sz == '\n')
			*sz = '\r';

		upper = toupper(*sz);

		rec.EventType = KEY_EVENT;
		rec.Event.KeyEvent.bKeyDown = TRUE;
		rec.Event.KeyEvent.wRepeatCount = 1;
		rec.Event.KeyEvent.wVirtualKeyCode = upper;
		rec.Event.KeyEvent.wVirtualScanCode = CharToCode(*sz);
		rec.Event.KeyEvent.uChar.AsciiChar = *sz;
		rec.Event.KeyEvent.uChar.UnicodeChar = *sz;
		rec.Event.KeyEvent.dwControlKeyState = isupper(*sz) ? 0x80 : 0x0;

		WriteConsoleInput(hStdin, &rec, 1, &dwWritten);
		rec.Event.KeyEvent.bKeyDown = FALSE;
		WriteConsoleInput(hStdin, &rec, 1, &dwWritten);
		sz++;
	}

	return TRUE;
}

unsigned __stdcall RequestProc(void *arg)
{
	HANDLE heventWait[2];
	int iBeginLine, iEndLine;

	heventWait[0] = heventParentSend;
	heventWait[1] = heventDone;

	while (true)
	{
		DWORD dwRet = WaitForMultipleObjects(2, heventWait, FALSE, INFINITE);

		// heventDone fired, so we're exiting.
		if (dwRet == WAIT_OBJECT_0 + 1)
			break;

		int *pBuffer = (int *)GetMappedBuffer(hfileBuffer);

		// hfileBuffer is invalid. Just leave.
		if (!pBuffer)
		{
			sys->Printf("Request Proc:  Invalid -HFILE handle\n");
			break;
		}

		switch (pBuffer[0])
		{
		case CCOM_WRITE_TEXT:
			pBuffer[0] = WriteText((LPCTSTR)(pBuffer + 1));
			break;

		case CCOM_GET_TEXT:
			iBeginLine = pBuffer[1];
			iEndLine = pBuffer[2];
			pBuffer[0] = ReadText((LPTSTR)(pBuffer + 1), iBeginLine, iEndLine);
			break;

		case CCOM_GET_SCR_LINES:
			pBuffer[0] = GetScreenBufferLines(&pBuffer[1]);
			break;

		case CCOM_SET_SCR_LINES:
			pBuffer[0] = SetScreenBufferLines(pBuffer[1]);
			break;
		}

		ReleaseMappedBuffer(pBuffer);
		SetEvent(heventChildSend);
	}

	_endthreadex(0);
	return 0;
}

void DeinitConProc()
{
	if (heventDone)
	{
		SetEvent(heventDone);
	}
}

void InitConProc()
{
	unsigned threadAddr;
	HANDLE hFile = (HANDLE)0;
	HANDLE heventParent = (HANDLE)0;
	HANDLE heventChild = (HANDLE)0;
	int WantHeight = 50;
	char *p;

	// give external front ends a chance to hook into the console
	if (CommandLine()->CheckParm("-HFILE", &p) && p)
	{
		hFile = (HANDLE)Q_atoi(p);
	}

	if (CommandLine()->CheckParm("-HPARENT", &p) && p)
	{
		heventParent = (HANDLE)Q_atoi(p);
	}

	if (CommandLine()->CheckParm("-HCHILD", &p) && p)
	{
		heventChild = (HANDLE)Q_atoi(p);
	}

	// ignore if we don't have all the events.
	if (!hFile || !heventParent || !heventChild)
	{
		//sys->Printf ("\n\nNo external front end present.\n");
		return;
	}

	sys->Printf("\n\nInitConProc:  Setting up external control.\n");

	hfileBuffer = hFile;
	heventParentSend = heventParent;
	heventChildSend = heventChild;

	// So we'll know when to go away.
	heventDone = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!heventDone)
	{
		sys->Printf("InitConProc:  Couldn't create heventDone\n");
		return;
	}

	if (!_beginthreadex(nullptr, 0, RequestProc, nullptr, 0, &threadAddr))
	{
		CloseHandle(heventDone);
		sys->Printf("InitConProc:  Couldn't create third party thread\n");
		return;
	}

	// save off the input/output handles.
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdin = GetStdHandle(STD_INPUT_HANDLE);

	if (CommandLine()->CheckParm("-conheight", &p) && p)
	{
		WantHeight = Q_atoi(p);
	}

	// Force 80 character width, at least 25 character height
	SetConsoleCXCY(hStdout, 80, WantHeight);
}
