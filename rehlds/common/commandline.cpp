#include "precompiled.h"

class CCommandLine: public ICommandLine {
public:
	CCommandLine();
	virtual ~CCommandLine();

	void CreateCmdLine(const char *commandline);
	void CreateCmdLine(int argc, const char *argv[]);
	const char *GetCmdLine() const;

	// Check whether a particular parameter exists
	const char *CheckParm(const char *psz, char **ppszValue = nullptr) const;
	void RemoveParm(const char *pszParm);
	void AppendParm(const char *pszParm, const char *pszValues);

	void SetParm(const char *pszParm, const char *pszValues);
	void SetParm(const char *pszParm, int iValue);

	// When the commandline contains @name, it reads the parameters from that file
	void LoadParametersFromFile(const char *&pSrc, char *&pDst, int maxDestLen);

private:
	// Copy of actual command line
	char *m_pszCmdLine;
};

CCommandLine g_CmdLine;
ICommandLine *cmdline = &g_CmdLine;

ICommandLine *CommandLine()
{
	return &g_CmdLine;
}

CCommandLine::CCommandLine()
{
	m_pszCmdLine = nullptr;
}

CCommandLine::~CCommandLine()
{
	if (m_pszCmdLine)
	{
		delete [] m_pszCmdLine;
		m_pszCmdLine = nullptr;
	}
}

char *CopyString(const char *src)
{
	if (!src)
		return nullptr;

	char *out = (char *)new char[strlen(src) + 1];
	strcpy(out, src);
	return out;
}

// Creates a command line from the arguments passed in
void CCommandLine::CreateCmdLine(int argc, const char *argv[])
{
	char cmdline[4096] = "";
	const int MAX_CHARS = sizeof(cmdline) - 1;

	for (int i = 0; i < argc; ++i)
	{
		if (strchr(argv[i], ' '))
		{
			strncat(cmdline, "\"", MAX_CHARS);
			strncat(cmdline, argv[i], MAX_CHARS);
			strncat(cmdline, "\"", MAX_CHARS);
		}
		else
		{
			strncat(cmdline, argv[i], MAX_CHARS);
		}

		strncat(cmdline, " ", MAX_CHARS);
	}

	cmdline[strlen(cmdline)] = '\0';
	CreateCmdLine(cmdline);
}

void CCommandLine::LoadParametersFromFile(const char *&pSrc, char *&pDst, int maxDestLen)
{
	// Suck out the file name
	char szFileName[ MAX_PATH ];
	char *pOut;
	char *pDestStart = pDst;

	// Skip the @ sign
	pSrc++;
	pOut = szFileName;

	while (*pSrc && *pSrc != ' ')
	{
		*pOut++ = *pSrc++;
#if 0
		if ((pOut - szFileName) >= (MAX_PATH - 1))
			break;
#endif
	}

	*pOut = '\0';

	// Skip the space after the file name
	if (*pSrc)
		pSrc++;

	// Now read in parameters from file
	FILE *fp = fopen(szFileName, "r");
	if (fp)
	{
		char c;
		c = (char)fgetc(fp);
		while (c != EOF)
		{
			// Turn return characters into spaces
			if (c == '\n')
				c = ' ';

			*pDst++ = c;

#if 0
			// Don't go past the end, and allow for our terminating space character AND a terminating null character.
			if ((pDst - pDestStart) >= (maxDestLen - 2))
				break;
#endif

			// Get the next character, if there are more
			c = (char)fgetc(fp);
		}

		// Add a terminating space character
		*pDst++ = ' ';

		fclose(fp);
	}
	else
	{
		printf("Parameter file '%s' not found, skipping...", szFileName);
	}
}

// Purpose: Create a command line from the passed in string
// Note that if you pass in a @filename, then the routine will read settings from a file instead of the command line
void CCommandLine::CreateCmdLine(const char *commandline)
{
	if (m_pszCmdLine)
	{
		delete[] m_pszCmdLine;
		m_pszCmdLine = nullptr;
	}

	char szFull[4096];

	char *pDst = szFull;
	const char *pSrc = commandline;

	bool allowAtSign = true;

	while (*pSrc)
	{
		if (*pSrc == '@')
		{
			if (allowAtSign)
			{
				LoadParametersFromFile(pSrc, pDst, sizeof(szFull) - (pDst - szFull));
				continue;
			}
		}

		allowAtSign = isspace(*pSrc) != 0;

#if 0
		// Don't go past the end.
		if ((pDst - szFull) >= (sizeof(szFull) - 1))
			break;
#endif
		*pDst++ = *pSrc++;
	}

	*pDst = '\0';

	int len = strlen(szFull) + 1;
	m_pszCmdLine = new char[len];
	memcpy(m_pszCmdLine, szFull, len);
}

// Purpose: Remove specified string ( and any args attached to it ) from command line
void CCommandLine::RemoveParm(const char *pszParm)
{
	if (!m_pszCmdLine)
		return;

	if (!pszParm || *pszParm == '\0')
		return;

	// Search for first occurrence of pszParm
	char *p, *found;
	char *pnextparam;
	int n;
	int curlen;

	p = m_pszCmdLine;
	while (*p)
	{
		curlen = strlen(p);
		found = strstr(p, pszParm);

		if (!found)
			break;

		pnextparam = found + 1;
		while (pnextparam && *pnextparam && (*pnextparam != '-') && (*pnextparam != '+'))
			pnextparam++;

		if (pnextparam && *pnextparam)
		{
			// We are either at the end of the string, or at the next param. Just chop out the current param.
			// # of characters after this param.
			n = curlen - (pnextparam - p);

			memcpy(found, pnextparam, n);
			found[n] = '\0';
		}
		else
		{
			// Clear out rest of string.
			n = pnextparam - found;
			memset(found, 0, n);
		}
	}

	// Strip and trailing ' ' characters left over.
	while (1)
	{
		int curpos = strlen(m_pszCmdLine);
		if (curpos == 0 || m_pszCmdLine[ curpos - 1 ] != ' ')
			break;

		m_pszCmdLine[curpos - 1] = '\0';
	}
}

// Purpose: Append parameter and argument values to command line
void CCommandLine::AppendParm(const char *pszParm, const char *pszValues)
{
	int nNewLength = 0;
	char *pCmdString;

	// Parameter.
	nNewLength = strlen(pszParm);

	// Values + leading space character.
	if (pszValues)
		nNewLength += strlen(pszValues) + 1;

	// Terminal 0;
	nNewLength++;

	if (!m_pszCmdLine)
	{
		m_pszCmdLine = new char[ nNewLength ];
		strcpy(m_pszCmdLine, pszParm);
		if (pszValues)
		{
			strcat(m_pszCmdLine, " ");
			strcat(m_pszCmdLine, pszValues);
		}

		return;
	}

	// Remove any remnants from the current Cmd Line.
	RemoveParm(pszParm);

	nNewLength += strlen(m_pszCmdLine) + 1 + 1;

	pCmdString = new char[ nNewLength ];
	memset(pCmdString, 0, nNewLength);

	strcpy(pCmdString, m_pszCmdLine);	// Copy old command line.
	strcat(pCmdString, " ");		// Put in a space
	strcat(pCmdString, pszParm);

	if (pszValues)
	{
		strcat(pCmdString, " ");
		strcat(pCmdString, pszValues);
	}

	// Kill off the old one
	delete[] m_pszCmdLine;

	// Point at the new command line.
	m_pszCmdLine = pCmdString;
}

void CCommandLine::SetParm(const char *pszParm, const char *pszValues)
{
	RemoveParm(pszParm);
	AppendParm(pszParm, pszValues);
}

void CCommandLine::SetParm(const char *pszParm, int iValue)
{
	char buf[64];
	_snprintf(buf, sizeof(buf), "%d", iValue);
	SetParm(pszParm, buf);
}

// Purpose: Search for the parameter in the current commandline
const char *CCommandLine::CheckParm(const char *psz, char **ppszValue) const
{
	static char sz[128] = "";

	if (!m_pszCmdLine)
		return nullptr;

	char *pret = strstr(m_pszCmdLine, psz);
	if (!pret || !ppszValue)
		return pret;

	*ppszValue = nullptr;

	// find the next whitespace
	char *p1 = pret;
	do {
		++p1;
	} while (*p1 != ' ' && *p1);

	int i = 0;
	char *p2 = p1 + 1;

	do {
		if (p2[i] == '\0' || p2[i] == ' ')
			break;

		sz[i++] = p2[i];
	} while (i < sizeof(sz));

	sz[i] = '\0';
	*ppszValue = sz;
	
	return pret;
}

const char *CCommandLine::GetCmdLine() const
{
	return m_pszCmdLine;
}
