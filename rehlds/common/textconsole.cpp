#include <stdio.h>
#include <port.h>

#include "textconsole.h"
#include "ObjectList.h"

bool CTextConsole::Init(IBaseSystem *system)
{
	// NULL or a valid base system interface
	m_System = system;

	memset(m_szConsoleText, 0, sizeof(m_szConsoleText));
	m_nConsoleTextLen = 0;
	m_nCursorPosition = 0;

	memset(m_szSavedConsoleText, 0, sizeof(m_szSavedConsoleText));
	m_nSavedConsoleTextLen = 0;

	memset(m_aszLineBuffer, 0, sizeof(m_aszLineBuffer));
	m_nTotalLines = 0;
	m_nInputLine = 0;
	m_nBrowseLine = 0;

	// these are log messages, not related to console
	Sys_Printf("\n");
	Sys_Printf("Console initialized.\n");

	m_ConsoleVisible = true;

	return true;
}

void CTextConsole::SetVisible(bool visible)
{
	m_ConsoleVisible = visible;
}

bool CTextConsole::IsVisible()
{
	return m_ConsoleVisible;
}

void CTextConsole::ShutDown()
{
	;
}

void CTextConsole::Print(char *pszMsg)
{
	if (m_nConsoleTextLen)
	{
		int nLen = m_nConsoleTextLen;
		while (nLen--)
		{
			PrintRaw("\b \b");
		}
	}

	PrintRaw(pszMsg);

	if (m_nConsoleTextLen)
	{
		PrintRaw(m_szConsoleText, m_nConsoleTextLen);
	}

	UpdateStatus();
}

int CTextConsole::ReceiveNewline()
{
	int nLen = 0;

	Echo("\n");

	if (m_nConsoleTextLen)
	{
		nLen = m_nConsoleTextLen;

		m_szConsoleText[ m_nConsoleTextLen ] = '\0';
		m_nConsoleTextLen = 0;
		m_nCursorPosition = 0;

		// cache line in buffer, but only if it's not a duplicate of the previous line
		if ((m_nInputLine == 0) || (strcmp(m_aszLineBuffer[ m_nInputLine - 1 ], m_szConsoleText)))
		{
			strncpy(m_aszLineBuffer[ m_nInputLine ], m_szConsoleText, MAX_CONSOLE_TEXTLEN);
			m_nInputLine++;

			if (m_nInputLine > m_nTotalLines)
				m_nTotalLines = m_nInputLine;

			if (m_nInputLine >= MAX_BUFFER_LINES)
				m_nInputLine = 0;

		}

		m_nBrowseLine = m_nInputLine;
	}

	return nLen;
}

void CTextConsole::ReceiveBackspace()
{
	int nCount;

	if (m_nCursorPosition == 0)
	{
		return;
	}

	m_nConsoleTextLen--;
	m_nCursorPosition--;

	Echo("\b");

	for (nCount = m_nCursorPosition; nCount < m_nConsoleTextLen; ++nCount)
	{
		m_szConsoleText[ nCount ] = m_szConsoleText[ nCount + 1 ];
		Echo(m_szConsoleText + nCount, 1);
	}

	Echo(" ");

	nCount = m_nConsoleTextLen;
	while (nCount >= m_nCursorPosition)
	{
		Echo("\b");
		nCount--;
	}

	m_nBrowseLine = m_nInputLine;
}

void CTextConsole::ReceiveTab()
{
#ifndef LAUNCHER_FIXES
	if (!m_System)
		return;
#else
	if (!rehldsFuncs || !m_nConsoleTextLen)
	{
		return;
	}
#endif
	ObjectList matches;
	m_szConsoleText[ m_nConsoleTextLen ] = '\0';
#ifndef LAUNCHER_FIXES
	m_System->GetCommandMatches(m_szConsoleText, &matches);
#else
	rehldsFuncs->GetCommandMatches(m_szConsoleText, &matches);
#endif
	if (matches.IsEmpty())
		return;

	if (matches.CountElements() == 1)
	{
		char *pszCmdName = (char *)matches.GetFirst();
		char *pszRest = pszCmdName + strlen(m_szConsoleText);

		if (pszRest)
		{
			Echo(pszRest);
			strcat(m_szConsoleText, pszRest);
			m_nConsoleTextLen += strlen(pszRest);

			Echo(" ");
			strcat(m_szConsoleText, " ");
			m_nConsoleTextLen++;
		}
	}
	else
	{
		int nLongestCmd = 0;
		int nCurrentColumn;
		int nTotalColumns;
		char szCommonCmd[256];//Should be enough.
		char szFormatCmd[256];
		char *pszLongestCmd;
		char *pszCurrentCmd = (char *)matches.GetFirst();
		while (pszCurrentCmd)
		{
			if ((int)strlen(pszCurrentCmd) > nLongestCmd)
			{
				nLongestCmd = strlen(pszCurrentCmd);
				pszLongestCmd = pszCurrentCmd;
			}

			pszCurrentCmd = (char *)matches.GetNext();
		}

		nTotalColumns = (GetWidth() - 1) / (nLongestCmd + 1);
		nCurrentColumn = 0;

		Echo("\n");
		Q_strcpy(szCommonCmd, pszLongestCmd);
		// Would be nice if these were sorted, but not that big a deal
		pszCurrentCmd = (char *)matches.GetFirst();
		while (pszCurrentCmd)
		{
			if (++nCurrentColumn > nTotalColumns)
			{
				Echo("\n");
				nCurrentColumn = 1;
			}

			_snprintf(szFormatCmd, sizeof(szFormatCmd), "%-*s ", nLongestCmd, pszCurrentCmd);
			Echo(szFormatCmd);
			for (char *pCur = pszCurrentCmd, *pCommon = szCommonCmd; (*pCur&&*pCommon); pCur++, pCommon++)
			{
				if (*pCur != *pCommon)
				{
					*pCommon = 0;
					break;
				}
			}
			pszCurrentCmd = (char *)matches.GetNext();
		}

		Echo("\n");
		if (Q_strcmp(szCommonCmd, m_szConsoleText))
		{
			Q_strcpy(m_szConsoleText, szCommonCmd);
			m_nConsoleTextLen = Q_strlen(szCommonCmd);	
		}

		Echo(m_szConsoleText);
	}

	m_nCursorPosition = m_nConsoleTextLen;
	m_nBrowseLine = m_nInputLine;
}

void CTextConsole::ReceiveStandardChar(const char ch)
{
	int nCount;

	// If the line buffer is maxed out, ignore this char
	if (m_nConsoleTextLen >= (sizeof(m_szConsoleText) - 2))
	{
		return;
	}

	nCount = m_nConsoleTextLen;
	while (nCount > m_nCursorPosition)
	{
		m_szConsoleText[ nCount ] = m_szConsoleText[ nCount - 1 ];
		nCount--;
	}

	m_szConsoleText[ m_nCursorPosition ] = ch;

	Echo(m_szConsoleText + m_nCursorPosition, m_nConsoleTextLen - m_nCursorPosition + 1);

	m_nConsoleTextLen++;
	m_nCursorPosition++;

	nCount = m_nConsoleTextLen;
	while (nCount > m_nCursorPosition)
	{
		Echo("\b");
		nCount--;
	}

	m_nBrowseLine = m_nInputLine;
}

void CTextConsole::ReceiveUpArrow()
{
	int nLastCommandInHistory = m_nInputLine + 1;
	if (nLastCommandInHistory > m_nTotalLines)
		nLastCommandInHistory = 0;

	if (m_nBrowseLine == nLastCommandInHistory)
		return;

	if (m_nBrowseLine == m_nInputLine)
	{
		if (m_nConsoleTextLen > 0)
		{
			// Save off current text
			strncpy(m_szSavedConsoleText, m_szConsoleText, m_nConsoleTextLen);
			// No terminator, it's a raw buffer we always know the length of
		}

		m_nSavedConsoleTextLen = m_nConsoleTextLen;
	}

	m_nBrowseLine--;
	if (m_nBrowseLine < 0)
	{
		m_nBrowseLine = m_nTotalLines - 1;
	}

	// delete old line
	while (m_nConsoleTextLen--)
	{
		Echo("\b \b");
	}

	// copy buffered line
	Echo(m_aszLineBuffer[ m_nBrowseLine ]);

	strncpy(m_szConsoleText, m_aszLineBuffer[ m_nBrowseLine ], MAX_CONSOLE_TEXTLEN);

	m_nConsoleTextLen = strlen(m_aszLineBuffer[ m_nBrowseLine ]);
	m_nCursorPosition = m_nConsoleTextLen;
}

void CTextConsole::ReceiveDownArrow()
{
	if (m_nBrowseLine == m_nInputLine)
		return;

	if (++m_nBrowseLine > m_nTotalLines)
		m_nBrowseLine = 0;

	// delete old line
	while (m_nConsoleTextLen--)
	{
		Echo("\b \b");
	}

	if (m_nBrowseLine == m_nInputLine)
	{
		if (m_nSavedConsoleTextLen > 0)
		{
			// Restore current text
			strncpy(m_szConsoleText, m_szSavedConsoleText, m_nSavedConsoleTextLen);
			// No terminator, it's a raw buffer we always know the length of

			Echo(m_szConsoleText, m_nSavedConsoleTextLen);
		}

		m_nConsoleTextLen = m_nSavedConsoleTextLen;
	}
	else
	{
		// copy buffered line
		Echo(m_aszLineBuffer[ m_nBrowseLine ]);
		strncpy(m_szConsoleText, m_aszLineBuffer[ m_nBrowseLine ], MAX_CONSOLE_TEXTLEN);
		m_nConsoleTextLen = strlen(m_aszLineBuffer[ m_nBrowseLine ]);
	}

	m_nCursorPosition = m_nConsoleTextLen;
}

void CTextConsole::ReceiveLeftArrow()
{
	if (m_nCursorPosition == 0)
		return;

	Echo("\b");
	m_nCursorPosition--;
}

void CTextConsole::ReceiveRightArrow()
{
	if (m_nCursorPosition == m_nConsoleTextLen)
		return;

	Echo(m_szConsoleText + m_nCursorPosition, 1);
	m_nCursorPosition++;
}
