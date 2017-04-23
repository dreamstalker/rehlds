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

#if !defined(_WIN32)

#include "TextConsoleUnix.h"
#include "icommandline.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <signal.h>

CTextConsoleUnix console;

CTextConsoleUnix::~CTextConsoleUnix()
{
	CTextConsoleUnix::ShutDown();
}

bool CTextConsoleUnix::Init(IBaseSystem *system)
{
	static struct termios termNew;
	sigset_t block_ttou;

	sigemptyset(&block_ttou);
	sigaddset(&block_ttou, SIGTTOU);
	sigprocmask(SIG_BLOCK, &block_ttou, NULL);

	tty = stdout;

	// this code is for echo-ing key presses to the connected tty
	// (which is != STDOUT)
	if (isatty(STDIN_FILENO))
	{
		tty = fopen(ctermid(NULL), "w+");
		if (!tty)
		{
			printf("Unable to open tty(%s) for output\n", ctermid(NULL));
			tty = stdout;
		}
		else
		{
			// turn buffering off
			setbuf(tty, NULL);
		}
	}
	else
	{
		tty = fopen("/dev/null", "w+");
		if (!tty)
		{
			tty = stdout;
		}
	}

	tcgetattr(STDIN_FILENO, &termStored);

	memcpy(&termNew, &termStored, sizeof(struct termios));

	// Disable canonical mode, and set buffer size to 1 byte
	termNew.c_lflag &= (~ICANON);
	termNew.c_cc[ VMIN ] = 1;
	termNew.c_cc[ VTIME ] = 0;

	// disable echo
	termNew.c_lflag &= (~ECHO);

	tcsetattr(STDIN_FILENO, TCSANOW, &termNew);
	sigprocmask(SIG_UNBLOCK, &block_ttou, NULL);

	return CTextConsole::Init();
}

void CTextConsoleUnix::ShutDown()
{
	sigset_t block_ttou;

	sigemptyset(&block_ttou);
	sigaddset(&block_ttou, SIGTTOU);
	sigprocmask(SIG_BLOCK, &block_ttou, NULL);
	tcsetattr(STDIN_FILENO, TCSANOW, &termStored);
	sigprocmask(SIG_UNBLOCK, &block_ttou, NULL);

	CTextConsole::ShutDown();
}

// return 0 if the kb isn't hit
int CTextConsoleUnix::kbhit()
{
	fd_set rfds;
	struct timeval tv;

	// Watch stdin (fd 0) to see when it has input.
	FD_ZERO(&rfds);
	FD_SET(STDIN_FILENO, &rfds);

	// Return immediately.
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	// Must be in raw or cbreak mode for this to work correctly.
	return select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv) != -1 && FD_ISSET(STDIN_FILENO, &rfds);
}

char *CTextConsoleUnix::GetLine()
{
	// early return for 99.999% case :)
	if (!kbhit())
		return NULL;

	escape_sequence_t es;

	es = ESCAPE_CLEAR;
	sigset_t block_ttou;

	sigemptyset(&block_ttou);
	sigaddset(&block_ttou, SIGTTOU);
	sigaddset(&block_ttou, SIGTTIN);
	sigprocmask(SIG_BLOCK, &block_ttou, NULL);

	while (true)
	{
		if (!kbhit())
			break;

		int nLen;
		char ch = 0;
		int numRead = read(STDIN_FILENO, &ch, 1);
		if (!numRead)
			break;

		switch (ch)
		{
		case '\n': // Enter
			es = ESCAPE_CLEAR;

			nLen = ReceiveNewline();
			if (nLen)
			{
				sigprocmask(SIG_UNBLOCK, &block_ttou, NULL);
				return m_szConsoleText;
			}
			break;

		case 127:  // Backspace
		case '\b': // Backspace
			es = ESCAPE_CLEAR;
			ReceiveBackspace();
			break;

		case '\t': // TAB
			es = ESCAPE_CLEAR;
			ReceiveTab();
			break;

		case 27:  // Escape character
			es = ESCAPE_RECEIVED;
			break;

		case '[': // 2nd part of escape sequence
		case 'O':
		case 'o':
			switch (es)
			{
			case ESCAPE_CLEAR:
			case ESCAPE_BRACKET_RECEIVED:
				es = ESCAPE_CLEAR;
				ReceiveStandardChar(ch);
				break;

			case ESCAPE_RECEIVED:
				es = ESCAPE_BRACKET_RECEIVED;
				break;
			}
			break;
		case 'A':
			if (es == ESCAPE_BRACKET_RECEIVED)
			{
				es = ESCAPE_CLEAR;
				ReceiveUpArrow();
			}
			else
			{
				es = ESCAPE_CLEAR;
				ReceiveStandardChar(ch);
			}
			break;
		case 'B':
			if (es == ESCAPE_BRACKET_RECEIVED)
			{
				es = ESCAPE_CLEAR;
				ReceiveDownArrow();
			}
			else
			{
				es = ESCAPE_CLEAR;
				ReceiveStandardChar(ch);
			}
			break;
		case 'C':
			if (es == ESCAPE_BRACKET_RECEIVED)
			{
				es = ESCAPE_CLEAR;
				ReceiveRightArrow();
			}
			else
			{
				es = ESCAPE_CLEAR;
				ReceiveStandardChar(ch);
			}
			break;
		case 'D':
			if (es == ESCAPE_BRACKET_RECEIVED)
			{
				es = ESCAPE_CLEAR;
				ReceiveLeftArrow();
			}
			else
			{
				es = ESCAPE_CLEAR;
				ReceiveStandardChar(ch);
			}
			break;
		default:
			// Just eat this char if it's an unsupported escape
			if (es != ESCAPE_BRACKET_RECEIVED)
			{
				// dont' accept nonprintable chars
				if ((ch >= ' ') && (ch <= '~'))
				{
					es = ESCAPE_CLEAR;
					ReceiveStandardChar(ch);
				}
			}
			break;
		}

		fflush(stdout);
	}

	sigprocmask(SIG_UNBLOCK, &block_ttou, NULL);
	return NULL;
}

void CTextConsoleUnix::PrintRaw(char *pszMsg, int nChars)
{
	if (nChars == 0)
	{
		printf("%s", pszMsg);
	}
	else
	{
		for (int nCount = 0; nCount < nChars; nCount++)
		{
			putchar(pszMsg[ nCount ]);
		}
	}
}

void CTextConsoleUnix::Echo(char *pszMsg, int nChars)
{
	if (nChars == 0)
	{
		fputs(pszMsg, tty);
	}
	else
	{
		for (int nCount = 0; nCount < nChars; nCount++)
		{
			fputc(pszMsg[ nCount ], tty);
		}
	}
}

int CTextConsoleUnix::GetWidth()
{
	struct winsize ws;
	int nWidth = 0;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)
	{
		nWidth = (int)ws.ws_col;
	}

	if (nWidth <= 1)
	{
		nWidth = 80;
	}

	return nWidth;
}

#endif // !defined(_WIN32)
