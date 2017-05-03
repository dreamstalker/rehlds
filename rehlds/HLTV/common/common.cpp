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

static char *date = __DATE__;
static char *mon[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static char mond[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

int COM_BuildNumber()
{
	int m = 0;
	int d = 0;
	int y = 0;
	static int b = 0;

	if (b != 0)
		return b;

	for (m = 0; m < 11; m++)
	{
		if (_strnicmp(&date[0], mon[m], 3) == 0)
			break;
		d += mond[m];
	}

	d += atoi(&date[4]) - 1;
	y = atoi(&date[7]) - 1900;
	b = d + (int)((y - 1) * 365.25);

	if (((y % 4) == 0) && m > 1)
	{
		b += 1;
	}

	// return days since initial commit on Apr 12 2014 (Happy Cosmonautics Day!)
	b -= 41374;
	return b;
}

char *COM_TimeString()
{
	static char timedate[16];
	time_t ltime = time(nullptr);
	tm *now = localtime(&ltime);

	strftime(timedate, sizeof(timedate), "%y%m%d%H%M", now);
	return timedate;
}

char *COM_SkipPath(char *pathname)
{
	char *last = pathname;
	while (*pathname)
	{
		if (*pathname == '/' || *pathname == '\\') {
			last = pathname + 1;
		}

		pathname++;
	}

	return last;
}

#ifdef _WIN32
char *COM_GetBaseDir()
{
	static char basedir[MAX_PATH];
	basedir[0] = '\0';

	if (GetModuleFileName((HMODULE)nullptr, basedir, sizeof(basedir)))
	{
		char *pBuffer = strrchr(basedir, '\\');

		if (pBuffer && *pBuffer)
			pBuffer[1] = '\0';

		int j = strlen(basedir);
		if (j > 0 && (basedir[j - 1] == '\\' || basedir[j - 1] == '/'))
			basedir[j - 1] = '\0';
	}

	return basedir;
}

#else // _WIN32

char g_szEXEName[256];
char *COM_GetBaseDir()
{
	static char basedir[MAX_PATH];
	basedir[0] = '\0';

	strcpy(basedir, g_szEXEName);
	char *pBuffer = strrchr(basedir, '/');

	if (pBuffer && *pBuffer)
		pBuffer[1] = '\0';

	int j = strlen(basedir);
	if (j > 0 && (basedir[j - 1] == '\\' || basedir[j - 1] == '/'))
		basedir[j - 1] = '\0';

	return basedir;
}

#endif // _WIN32

void COM_FixSlashes(char *pname)
{
	while (*pname)
	{
#ifdef _WIN32
		if (*pname == '/')
		{
			*pname = '\\';
		}
#else
		if (*pname == '\\')
		{
			*pname = '/';
		}
#endif

		pname++;
	}
}

// Fills "out" with the file name without path and extension.
void COM_FileBase(char *in, char *out)
{
	*out = '\0';

	int len = strlen(in);
	if (len <= 0) {
		return;
	}

	const char *start = in + len - 1;
	const char *end = in + len;

	while (start >= in && *start != '/' && *start != '\\')
	{
		if (*start == '.') {
			end = start;
		}

		start--;
	}
	start++;

	len = end - start;
	strncpy(out, start, len);
	out[len] = '\0';
}

char com_token[COM_TOKEN_LEN];
qboolean s_com_token_unget = FALSE;

void COM_UngetToken()
{
	s_com_token_unget = TRUE;
}

char *COM_Parse(char *data)
{
	int c;
	int len;

	if (s_com_token_unget)
	{
		s_com_token_unget = FALSE;
		return data;
	}

	len = 0;
	com_token[0] = '\0';

	if (!data) {
		return nullptr;
	}

skipwhite:
	while (*data <= 32)
	{
		if (!*data) {
			return nullptr;
		}

		data++;
	}

	c = *data;

	// skip // comments till the next line
	if (c == '/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;

		// start over new line
		goto skipwhite;
	}

	// handle quoted strings specially: copy till the end or another quote
	if (c == '\"')
	{
		// skip starting quote
		data++;
		while (true)
		{
			c = *data++;	// get char and advance
			if (c == '\"')	// closing quote
			{
				com_token[len] = '\0';
				return data;
			}

			if (!c || len == COM_TOKEN_LEN - 1) // check if buffer is full
			{
				com_token[len] = 0;
				return data;
			}

			com_token[len++] = c;
		}
	}
	else
	{
		// parse single characters
		if (c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',')
		{
			com_token[len++] = c;
			com_token[len] = '\0';
			return data + 1;
		}

		// parse a regular word
		do
		{
			com_token[len] = c;
			data++;
			len++;
			c = *data;
			if (c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',')
				break;
		}
		while (len < COM_TOKEN_LEN - 1 && (c < 0 || c > 32));
	}

	com_token[len] = '\0';
	return data;
}

char *COM_VarArgs(char *format, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, format);
	_vsnprintf(string, sizeof(string), format, argptr);
	va_end(argptr);

	return string;
}

unsigned char COM_Nibble(char c)
{
	if (c >= '0' && c <= '9')
	{
		return (unsigned char)(c - '0');
	}

	if (c >= 'A' && c <= 'F')
	{
		return (unsigned char)(c - 'A' + 0x0A);
	}

	if (c >= 'a' && c <= 'f')
	{
		return (unsigned char)(c - 'a' + 0x0A);
	}

	return '0';
}

void COM_HexConvert(const char *pszInput, int nInputLength, unsigned char *pOutput)
{
	const char *pIn;
	unsigned char *p = pOutput;
	for (int i = 0; i < nInputLength - 1; i += 2)
	{
		pIn = &pszInput[i];
		if (!pIn[0] || !pIn[1])
			break;

		*p++ = ((COM_Nibble(pIn[0]) << 4) | COM_Nibble(pIn[1]));
	}
}

char *COM_BinPrintf(unsigned char *buf, int length)
{
	char szChunk[10];
	static char szReturn[4096];
	memset(szReturn, 0, sizeof(szReturn));

	for (int i = 0; i < length; i++)
	{
		_snprintf(szChunk, sizeof(szChunk), "%02x", buf[i]);
		strncat(szReturn, szChunk, sizeof(szReturn) - strlen(szReturn) - 1);
	}

	return szReturn;
}

char *COM_FormatTime(float seconds)
{

	static char time[32];
	int hours = (int)seconds / 3600;
	if (hours > 0)
	{
		_snprintf(time, sizeof(time), "%02i:%2i:%02i", hours, (int)seconds / 60, (int)seconds % 60);
	}
	else
	{
		_snprintf(time, sizeof(time), "%02i:%02i", (int)seconds / 60, (int)seconds % 60);
	}

	return time;
}

void COM_RemoveEvilChars(char *string)
{
	char *c = string;
	if (!c) {
		return;
	}

	while (*c)
	{
		if (*c < ' ' || *c > '~' || *c == '%' || *c == ';') {
			*c = ' ';
		}

		c++;
	}
}

int COM_FileNameCmp(const char *file1, const char *file2)
{
	while (*file1 && *file2)
	{
		if ((*file1 != '/' || *file2 != '\\') && (*file2 != '/' || *file1 != '\\'))
		{
			if (tolower(*file1) != tolower(*file2))
				return -1;

			if (!*file1)
				return 0;
		}

		file1++;
		file2++;
	}

	return 0;
}

bool COM_IsWhiteSpace(char space)
{
	switch (space) {
	case '\t':
	case '\r':
	case '\n':
	case ' ':
		return true;
	}

	return false;
}

void COM_TrimSpace(const char *source, char *dest)
{
	int start = 0;
	while (source[start] && COM_IsWhiteSpace(source[start])) {
		start++;
	}

	int end = strlen(source) - 1;
	while (end > 0 && COM_IsWhiteSpace(source[end])) {
		end--;
	}

	end++;

	int length = end - start;
	if (length > 0) {
		strncpy(dest, &source[start], length);
	}
	else
		length = 0;

	dest[length] = '\0';
}

void COM_UnpackRGB(unsigned char &r, unsigned char &g, unsigned char &b, unsigned int color)
{
	r = (color >> 16) & 0xFF;
	g = (color >> 8) & 0xFF;
	b = (color) & 0xFF;
}

unsigned int COM_PackRGB(unsigned char r, unsigned char g, unsigned char b)
{
	return (r << 16) | (g << 8) | b;
}

unsigned int COM_PackRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return (a << 24) | (r << 16) | (g << 8) | b;
}

void NORETURN HLTV_SysError(const char *fmt, ...)
{
	va_list argptr;
	static char string[8192];

	va_start(argptr, fmt);
	vsnprintf(string, sizeof(string), fmt, argptr);
	va_end(argptr);

	printf("%s\n", string);

	FILE* fl = fopen("hltv_error.txt", "w");
	fprintf(fl, "%s\n", string);
	fclose(fl);

	int *null = 0;
	*null = 0;
	exit(-1);
}
