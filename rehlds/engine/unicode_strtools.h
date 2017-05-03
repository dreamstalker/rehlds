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

#include "maintypes.h"


#ifdef _WIN32
typedef wchar_t uchar16;
typedef unsigned int uchar32;
#else
typedef unsigned short uchar16;
typedef wchar_t uchar32;
#endif

enum EStringConvertErrorPolicy {
	_STRINGCONVERTFLAG_SKIP = 1,
	_STRINGCONVERTFLAG_FAIL = 2,
	_STRINGCONVERTFLAG_ASSERT = 4,
	STRINGCONVERT_REPLACE = 0,
	STRINGCONVERT_SKIP = 1,
	STRINGCONVERT_FAIL = 2,
	STRINGCONVERT_ASSERT_REPLACE = 4,
	STRINGCONVERT_ASSERT_SKIP = 5,
	STRINGCONVERT_ASSERT_FAIL = 6,
};

qboolean Q_IsValidUChar32(uchar32 uVal);
int Q_UTF8ToUChar32(const char *pUTF8_, uchar32 &uValueOut, bool &bErrorOut);
int Q_UChar32ToUTF8(uchar32 uVal, char * pUTF8Out);
int Q_UChar32ToUTF8Len(uchar32 uVal);
qboolean Q_UnicodeValidate(const char *pUTF8);
char *Q_UnicodeAdvance(char *pUTF8, int nChars);
qboolean Q_StripUnprintableAndSpace(char *pch);
qboolean V_UTF8ToUChar32(const char *pUTF8_, uchar32 *uValueOut);
int Q_UnicodeRepair(char *pUTF8);
