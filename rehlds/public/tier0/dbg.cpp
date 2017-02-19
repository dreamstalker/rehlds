//=========== (C) Copyright 1999 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// $Header: $
// $NoKeywords: $
//
// The main debug library implementation
//=============================================================================

#include "precompiled.h"

//-----------------------------------------------------------------------------
// internal structures
//-----------------------------------------------------------------------------

enum
{
	MAX_GROUP_NAME_LENGTH = 48
};

struct SpewGroup_t
{
	char	m_GroupName[MAX_GROUP_NAME_LENGTH];
	int		m_Level;
};


//-----------------------------------------------------------------------------
// Templates to assist in validating pointers:

void _AssertValidReadPtr(void* ptr, int count/* = 1*/)
{
#ifdef _WIN32
	Assert(!IsBadReadPtr(ptr, count));
#else
	Assert(ptr);
#endif

}

void _AssertValidWritePtr(void* ptr, int count/* = 1*/)
{
#ifdef _WIN32
	Assert(!IsBadWritePtr(ptr, count));
#else
	Assert(ptr);
#endif
}

void _AssertValidReadWritePtr(void* ptr, int count/* = 1*/)
{
#ifdef _WIN32
	Assert(!(IsBadWritePtr(ptr, count) || IsBadReadPtr(ptr, count)));
#else
	Assert(ptr);
#endif
}

void AssertValidStringPtr(const char* ptr, int maxchar/* = 0xFFFFFF */)
{
#ifdef _WIN32
	Assert(!IsBadStringPtr(ptr, maxchar));
#else
	Assert(ptr);
#endif
}


//-----------------------------------------------------------------------------
// globals
//-----------------------------------------------------------------------------

SpewRetval_t DefaultSpewFunc(SpewType_t type, char const *pMsg)
{
	printf("%s", pMsg);
	if (type == SPEW_ASSERT)
		return SPEW_DEBUGGER;
	else if (type == SPEW_ERROR)
		return SPEW_ABORT;
	else
		return SPEW_CONTINUE;
}

static SpewOutputFunc_t   s_SpewOutputFunc = DefaultSpewFunc;

static char const*	s_pFileName;
static int			s_Line;
static SpewType_t	s_SpewType;

static SpewGroup_t* s_pSpewGroups = 0;
static int			s_GroupCount = 0;
static int			s_DefaultLevel = 0;



//-----------------------------------------------------------------------------
// Spew output management.
//-----------------------------------------------------------------------------

void  SpewOutputFunc(SpewOutputFunc_t func)
{
	s_SpewOutputFunc = func ? func : DefaultSpewFunc;
}

SpewOutputFunc_t GetSpewOutputFunc(void)
{
	if (s_SpewOutputFunc)
	{
		return s_SpewOutputFunc;
	}
	else
	{
		return DefaultSpewFunc;
	}
}

//-----------------------------------------------------------------------------
// Spew functions
//-----------------------------------------------------------------------------

void  _SpewInfo(SpewType_t type, char const* pFile, int line)
{
	// Only grab the file name. Ignore the path.
	char const* pSlash = strrchr(pFile, '\\');
	char const* pSlash2 = strrchr(pFile, '/');
	if (pSlash < pSlash2) pSlash = pSlash2;

	s_pFileName = pSlash ? pSlash + 1 : pFile;
	s_Line = line;
	s_SpewType = type;
}

SpewRetval_t  _SpewMessage(SpewType_t spewType, char const* pMsgFormat, va_list args)
{
	char pTempBuffer[1024];

	/* Printf the file and line for warning + assert only... */
	int len = 0;
	if ((spewType == SPEW_ASSERT))
	{
		len = sprintf(pTempBuffer, "%s (%d) : ", s_pFileName, s_Line);
	}

	/* Create the message.... */
	len += vsprintf(&pTempBuffer[len], pMsgFormat, args);

	// Add \n for warning and assert
	if ((spewType == SPEW_ASSERT))
	{
		len += sprintf(&pTempBuffer[len], "\n");
	}

	assert(len < 1024); /* use normal assert here; to avoid recursion. */
	assert(s_SpewOutputFunc);

	/* direct it to the appropriate target(s) */
	SpewRetval_t ret = s_SpewOutputFunc(spewType, pTempBuffer);
	switch (ret)
	{
		// Put the break into the macro so it would occur in the right place
		//	case SPEW_DEBUGGER:
		//		DebuggerBreak();
		//		break;

	case SPEW_ABORT:
		//		MessageBox(NULL,"Error in _SpewMessage","Error",MB_OK);
		exit(0);
	default:
		break;
	}

	return ret;
}

SpewRetval_t  _SpewMessage(char const* pMsgFormat, ...)
{
	va_list args;
	va_start(args, pMsgFormat);
	SpewRetval_t ret = _SpewMessage(s_SpewType, pMsgFormat, args);
	va_end(args);
	return ret;
}

SpewRetval_t _DSpewMessage(char const *pGroupName, int level, char const* pMsgFormat, ...)
{
	if (!IsSpewActive(pGroupName, level))
		return SPEW_CONTINUE;

	va_list args;
	va_start(args, pMsgFormat);
	SpewRetval_t ret = _SpewMessage(s_SpewType, pMsgFormat, args);
	va_end(args);
	return ret;
}

void Msg(char const* pMsgFormat, ...)
{
	va_list args;
	va_start(args, pMsgFormat);
	_SpewMessage(SPEW_MESSAGE, pMsgFormat, args);
	va_end(args);
}

void DMsg(char const *pGroupName, int level, char const *pMsgFormat, ...)
{
	if (!IsSpewActive(pGroupName, level))
		return;

	va_list args;
	va_start(args, pMsgFormat);
	_SpewMessage(SPEW_MESSAGE, pMsgFormat, args);
	va_end(args);
}

void Warning(char const *pMsgFormat, ...)
{
	va_list args;
	va_start(args, pMsgFormat);
	_SpewMessage(SPEW_WARNING, pMsgFormat, args);
	va_end(args);
}

void DWarning(char const *pGroupName, int level, char const *pMsgFormat, ...)
{
	if (!IsSpewActive(pGroupName, level))
		return;

	va_list args;
	va_start(args, pMsgFormat);
	_SpewMessage(SPEW_WARNING, pMsgFormat, args);
	va_end(args);
}

void Log(char const *pMsgFormat, ...)
{
	va_list args;
	va_start(args, pMsgFormat);
	_SpewMessage(SPEW_LOG, pMsgFormat, args);
	va_end(args);
}

void DLog(char const *pGroupName, int level, char const *pMsgFormat, ...)
{
	if (!IsSpewActive(pGroupName, level))
		return;

	va_list args;
	va_start(args, pMsgFormat);
	_SpewMessage(SPEW_LOG, pMsgFormat, args);
	va_end(args);
}

void Error(char const *pMsgFormat, ...)
{
	va_list args;
	va_start(args, pMsgFormat);
	_SpewMessage(SPEW_ERROR, pMsgFormat, args);
	va_end(args);
}


//-----------------------------------------------------------------------------
// A couple of super-common dynamic spew messages, here for convenience 
// These looked at the "developer" group, print if it's level 1 or higher 
//-----------------------------------------------------------------------------

void DevMsg(int level, char const* pMsgFormat, ...)
{
	if (!IsSpewActive("developer", level))
		return;

	va_list args;
	va_start(args, pMsgFormat);
	_SpewMessage(SPEW_MESSAGE, pMsgFormat, args);
	va_end(args);
}

void DevWarning(int level, char const *pMsgFormat, ...)
{
	if (!IsSpewActive("developer", level))
		return;

	va_list args;
	va_start(args, pMsgFormat);
	_SpewMessage(SPEW_WARNING, pMsgFormat, args);
	va_end(args);
}

void DevLog(int level, char const *pMsgFormat, ...)
{
	if (!IsSpewActive("developer", level))
		return;

	va_list args;
	va_start(args, pMsgFormat);
	_SpewMessage(SPEW_LOG, pMsgFormat, args);
	va_end(args);
}

void DevMsg(char const *pMsgFormat, ...)
{
	if (!IsSpewActive("developer", 1))
		return;

	va_list args;
	va_start(args, pMsgFormat);
	_SpewMessage(SPEW_MESSAGE, pMsgFormat, args);
	va_end(args);
}

void DevWarning(char const *pMsgFormat, ...)
{
	va_list args;
	va_start(args, pMsgFormat);
	_SpewMessage(SPEW_WARNING, pMsgFormat, args);
	va_end(args);
}

void DevLog(char const *pMsgFormat, ...)
{
	va_list args;
	va_start(args, pMsgFormat);
	_SpewMessage(SPEW_LOG, pMsgFormat, args);
	va_end(args);
}

//-----------------------------------------------------------------------------
// Find a group, return true if found, false if not. Return in ind the
// index of the found group, or the index of the group right before where the
// group should be inserted into the list to maintain sorted order.
//-----------------------------------------------------------------------------

bool FindSpewGroup(char const* pGroupName, int* pInd)
{
	int s = 0;
	if (s_GroupCount)
	{
		int e = (int)(s_GroupCount - 1);
		while (s <= e)
		{
			int m = (s + e) >> 1;
			int cmp = Q_stricmp(pGroupName, s_pSpewGroups[m].m_GroupName);
			if (!cmp)
			{
				*pInd = m;
				return true;
			}
			if (cmp < 0)
				e = m - 1;
			else
				s = m + 1;
		}
	}
	*pInd = s;
	return false;
}


//-----------------------------------------------------------------------------
// Sets the priority level for a spew group
//-----------------------------------------------------------------------------

void SpewActivate(char const* pGroupName, int level)
{
	Assert(pGroupName);

	// check for the default group first...
	if ((pGroupName[0] == '*') && (pGroupName[1] == '\0'))
	{
		s_DefaultLevel = level;
		return;
	}

	// Normal case, search in group list using binary search.
	// If not found, grow the list of groups and insert it into the
	// right place to maintain sorted order. Then set the level.
	int ind;
	if (!FindSpewGroup(pGroupName, &ind))
	{
		// not defined yet, insert an entry.
		++s_GroupCount;
		if (s_pSpewGroups)
		{
			s_pSpewGroups = (SpewGroup_t*)realloc(s_pSpewGroups,
				s_GroupCount * sizeof(SpewGroup_t));

			// shift elements down to preserve order
			int numToMove = s_GroupCount - ind - 1;
			memmove(&s_pSpewGroups[ind + 1], &s_pSpewGroups[ind],
				numToMove * sizeof(SpewGroup_t));
		}
		else
			s_pSpewGroups = (SpewGroup_t*)malloc(s_GroupCount * sizeof(SpewGroup_t));

		Assert(strlen(pGroupName) < MAX_GROUP_NAME_LENGTH);
		strcpy(s_pSpewGroups[ind].m_GroupName, pGroupName);
	}
	s_pSpewGroups[ind].m_Level = level;
}


//-----------------------------------------------------------------------------
// Tests to see if a particular spew is active
//-----------------------------------------------------------------------------

bool IsSpewActive(char const* pGroupName, int level)
{
	// If we don't find the spew group, use the default level.
	int ind;
	if (FindSpewGroup(pGroupName, &ind))
		return s_pSpewGroups[ind].m_Level >= level;
	else
		return s_DefaultLevel >= level;
}


// If we don't have a function from math.h, then it doesn't link certain floating-point
// functions in and printfs with %f cause runtime errors in the C libraries.
float CrackSmokingCompiler(float a)
{
	return fabs(a);
}

void* Plat_SimpleLog(const char* file, int line)
{
	FILE* f = fopen("simple.log", "at+");
	fprintf(f, "%s:%i\n", file, line);
	fclose(f);

	return NULL;
}
