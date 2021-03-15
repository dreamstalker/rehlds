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

// wordbreak parsing set
characterset_t g_BreakSet, g_BreakSetIncludingColons;

void InitializeCharacterSets()
{
	static bool s_CharacterSetInitialized = false;
	if (!s_CharacterSetInitialized)
	{
		CharacterSetBuild(&g_BreakSet, "{}()'");
		CharacterSetBuild(&g_BreakSetIncludingColons, "{}()':");
		s_CharacterSetInitialized = true;
	}
}

const char *ParseFile(const char *pFileBytes, char *pToken, bool *pWasQuoted, characterset_t *pCharSet)
{
	if (pWasQuoted)
		*pWasQuoted = false;

	if (!pFileBytes)
		return nullptr;

	InitializeCharacterSets();

	// YWB: Ignore colons as token separators in COM_Parse
	static const bool com_ignorecolons = false;
	characterset_t &breaks = pCharSet ? *pCharSet : (com_ignorecolons ? g_BreakSet : g_BreakSetIncludingColons);

	int c;
	int len = 0;
	pToken[0] = '\0';

skipwhite:
	// skip whitespace
	while ((c = *pFileBytes) <= ' ')
	{
		if (c == 0)
		{
			// end of file;
			return nullptr;
		}

		pFileBytes++;
	}

	// skip // comments till the next line
	if (c == '/' && pFileBytes[1] == '/')
	{
		while (*pFileBytes && *pFileBytes != '\n')
			pFileBytes++;
		goto skipwhite; // start over new line
	}

	// skip c-style comments
	if (c == '/' && pFileBytes[1] == '*')
	{
		// Skip "/*"
		pFileBytes += 2;

		while (*pFileBytes)
		{
			if (*pFileBytes == '*' && pFileBytes[1] == '/')
			{
				pFileBytes += 2;
				break;
			}

			pFileBytes++;
		}

		goto skipwhite;
	}

	// handle quoted strings specially: copy till the end or another quote
	if (c == '\"')
	{
		if (pWasQuoted)
			*pWasQuoted = true;

		pFileBytes++; // skip starting quote
		while (true)
		{
			c = *pFileBytes++; // get char and advance

			if (!c) // EOL
			{
				pToken[len] = '\0';
				return pFileBytes - 1; // we are done with that, but return data to show that token is present
			}

			if (c == '\"') // closing quote
			{
				pToken[len] = '\0';
				return pFileBytes;
			}

			pToken[len++] = c;
		}
	}

	// parse single characters
	if (IN_CHARACTERSET(breaks, c))
	{
		pToken[len++] = c;
		pToken[len] = '\0';
		return pFileBytes + 1;
	}

	// parse a regular word
	do
	{
		pToken[len++] = c;
		pFileBytes++;

		c = *pFileBytes;
		if (IN_CHARACTERSET(breaks, c))
			break;
	}
	while (c > 32);

	pToken[len] = '\0';
	return pFileBytes;
}

char *ParseFile(char *pFileBytes, char *pToken, bool *pWasQuoted)
{
	return const_cast<char *>(ParseFile(static_cast<const char *>(pFileBytes), pToken, pWasQuoted));
}

void NORETURN FileSystem_SysError(const char *fmt, ...)
{
	va_list argptr;
	static char string[8192];

	va_start(argptr, fmt);
	vsnprintf(string, sizeof(string), fmt, argptr);
	va_end(argptr);

	printf("%s\n", string);

	FILE* fl = fopen("filesystem_error.txt", "w");
	fprintf(fl, "%s\n", string);
	fclose(fl);

	volatile int *null = 0;
	*null = 0;
	exit(-1);
}
