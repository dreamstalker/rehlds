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


/* <f2fc1> ../engine/unicode_strtools.cpp:23 */
//-----------------------------------------------------------------------------
// Purpose: determine if a uchar32 represents a valid Unicode code point
//-----------------------------------------------------------------------------
qboolean Q_IsValidUChar32(uchar32 uVal)
{
	// Values > 0x10FFFF are explicitly invalid; ditto for UTF-16 surrogate halves,
	// values ending in FFFE or FFFF, or values in the 0x00FDD0-0x00FDEF reserved range
	return (uVal < 0x110000u) && ((uVal - 0x00D800u) > 0x7FFu) && ((uVal & 0xFFFFu) < 0xFFFEu) && ((uVal - 0x00FDD0u) > 0x1Fu);
}

/* <f4251> ../engine/unicode_strtools.cpp:346 */
// Decode one character from a UTF-8 encoded string. Treats 6-byte CESU-8 sequences
// as a single character, as if they were a correctly-encoded 4-byte UTF-8 sequence.
int Q_UTF8ToUChar32(const char *pUTF8_, uchar32 &uValueOut, bool &bErrorOut)
{
	const uint8 *pUTF8 = (const uint8 *)pUTF8_;

	int nBytes = 1;
	uint32 uValue = pUTF8[0];
	uint32 uMinValue = 0;

	// 0....... single byte
	if (uValue < 0x80)
		goto decodeFinishedNoCheck;

	// Expecting at least a two-byte sequence with 0xC0 <= first <= 0xF7 (110...... and 11110...)
	if ((uValue - 0xC0u) > 0x37u || (pUTF8[1] & 0xC0) != 0x80)
		goto decodeError;

	uValue = (uValue << 6) - (0xC0 << 6) + pUTF8[1] - 0x80;
	nBytes = 2;
	uMinValue = 0x80;

	// 110..... two-byte lead byte
	if (!(uValue & (0x20 << 6)))
		goto decodeFinished;

	// Expecting at least a three-byte sequence
	if ((pUTF8[2] & 0xC0) != 0x80)
		goto decodeError;

	uValue = (uValue << 6) - (0x20 << 12) + pUTF8[2] - 0x80;
	nBytes = 3;
	uMinValue = 0x800;

	// 1110.... three-byte lead byte
	if (!(uValue & (0x10 << 12)))
		goto decodeFinishedMaybeCESU8;

	// Expecting a four-byte sequence, longest permissible in UTF-8
	if ((pUTF8[3] & 0xC0) != 0x80)
		goto decodeError;

	uValue = (uValue << 6) - (0x10 << 18) + pUTF8[3] - 0x80;
	nBytes = 4;
	uMinValue = 0x10000;

	// 11110... four-byte lead byte. fall through to finished.

decodeFinished:
	if (uValue >= uMinValue && Q_IsValidUChar32(uValue))
	{
decodeFinishedNoCheck:
		uValueOut = uValue;
		bErrorOut = false;
		return nBytes;
	}
decodeError:
	uValueOut = '?';
	bErrorOut = true;
	return nBytes;

decodeFinishedMaybeCESU8:
	// Do we have a full UTF-16 surrogate pair that's been UTF-8 encoded afterwards?
	// That is, do we have 0xD800-0xDBFF followed by 0xDC00-0xDFFF? If so, decode it all.
	if ((uValue - 0xD800u) < 0x400u && pUTF8[3] == 0xED && (uint8)(pUTF8[4] - 0xB0) < 0x10 && (pUTF8[5] & 0xC0) == 0x80)
	{
		uValue = 0x10000 + ((uValue - 0xD800u) << 10) + ((uint8)(pUTF8[4] - 0xB0) << 6) + pUTF8[5] - 0x80;
		nBytes = 6;
		uMinValue = 0x10000;
	}
	goto decodeFinished;
}

int __cdecl Q_IsUnprintableW(uchar16 c)
{
	switch (c)
	{
	case 0x202A:
	case 0x202B:
	case 0x202C:
	case 0x202D:
	case 0x202E:
	case 0x206A:
	case 0x206B:
	case 0x206C:
	case 0x206D:
	case 0x206E:
	case 0x206F:
		return 1;

	default:
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: returns true if a wide character is a "mean" space; that is,
//			if it is technically a space or punctuation, but causes disruptive
//			behavior when used in names, web pages, chat windows, etc.
//
//			characters in this set are removed from the beginning and/or end of strings
//			by Q_AggressiveStripPrecedingAndTrailingWhitespaceW() 
//-----------------------------------------------------------------------------
bool Q_IsMeanSpaceW(uchar16 wch)
{
	bool bIsMean = false;

	switch (wch)
	{
	case 0x0082:	  // BREAK PERMITTED HERE
	case 0x0083:	  // NO BREAK PERMITTED HERE
	case 0x00A0:	  // NO-BREAK SPACE
	case 0x034F:   // COMBINING GRAPHEME JOINER
	case 0x2000:   // EN QUAD
	case 0x2001:   // EM QUAD
	case 0x2002:   // EN SPACE
	case 0x2003:   // EM SPACE
	case 0x2004:   // THICK SPACE
	case 0x2005:   // MID SPACE
	case 0x2006:   // SIX SPACE
	case 0x2007:   // figure space
	case 0x2008:   // PUNCTUATION SPACE
	case 0x2009:   // THIN SPACE
	case 0x200A:   // HAIR SPACE
	case 0x200B:   // ZERO-WIDTH SPACE
	case 0x200C:   // ZERO-WIDTH NON-JOINER
	case 0x200D:   // ZERO WIDTH JOINER
	case 0x2028:   // LINE SEPARATOR
	case 0x2029:   // PARAGRAPH SEPARATOR
	case 0x202F:   // NARROW NO-BREAK SPACE
	case 0x2060:   // word joiner
	case 0xFEFF:   // ZERO-WIDTH NO BREAK SPACE
	case 0xFFFC:   // OBJECT REPLACEMENT CHARACTER
		bIsMean = true;
		break;
	}

	return bIsMean;
}

//-----------------------------------------------------------------------------
// Purpose: strips trailing whitespace; returns pointer inside string just past
// any leading whitespace.
//
// bAggresive = true causes this function to also check for "mean" spaces,
// which we don't want in persona names or chat strings as they're disruptive
// to the user experience.
//-----------------------------------------------------------------------------
static uchar16 *StripWhitespaceWorker(uchar16 *pwch, int cchLength, bool *pbStrippedWhitespace)
{
	// walk backwards from the end of the string, killing any whitespace
	*pbStrippedWhitespace = false;

	uchar16 *pwchEnd = pwch + cchLength;
	while (--pwchEnd >= pwch)
	{
		if (!iswspace(*pwchEnd) && !Q_IsMeanSpaceW(*pwchEnd))
			break;

		*pwchEnd = 0;
		*pbStrippedWhitespace = true;
	}

	// walk forward in the string
	while (pwch < pwchEnd)
	{
		if (!iswspace(*pwch))
			break;

		*pbStrippedWhitespace = true;
		pwch++;
	}

	return pwch;
}

uchar16 *__cdecl StripUnprintableWorker(uchar16 *pwch, bool *pStripped)
{
	uchar16* rPos = pwch;
	uchar16* wPos = pwch;
	*pStripped = 0;

	while(*rPos)
	{
		uchar16 cc = *rPos;
		if (*rPos >= 0x20u && !Q_IsUnprintableW(cc) && cc != 0x2026)
		{
			*wPos = cc;
			++wPos;
		}
		++rPos;
	}
	
	*wPos = 0;
	*pStripped = rPos != wPos;
	return pwch;
}

/* <f45fd> ../engine/unicode_strtools.cpp:423 */
//-----------------------------------------------------------------------------
// Purpose: Returns false if UTF-8 string contains invalid sequences.
//-----------------------------------------------------------------------------
qboolean Q_UnicodeValidate(const char *pUTF8)
{
	bool bError = false;
	while (*pUTF8)
	{
		uchar32 uVal;
		// Our UTF-8 decoder silently fixes up 6-byte CESU-8 (improperly re-encoded UTF-16) sequences.
		// However, these are technically not valid UTF-8. So if we eat 6 bytes at once, it's an error.
		int nCharSize = Q_UTF8ToUChar32(pUTF8, uVal, bError);
		if (bError || nCharSize == 6)
			return false;
		pUTF8 += nCharSize;
	}
	return true;
}

/* <f46d1> ../engine/unicode_strtools.cpp:459 */
char *Q_UnicodeAdvance(char *pUTF8, int nChars)
{
	uchar32 uVal = 0;
	bool bError = false;

	while (nChars > 0 && *pUTF8)
	{
		pUTF8 += Q_UTF8ToUChar32(pUTF8, uVal, bError);
		--nChars;
	}

	return pUTF8;
}



/* <f4a0d> ../engine/unicode_strtools.cpp:717 */
qboolean V_UTF8ToUChar32(const char *pUTF8_, uchar32 *uValueOut)
{
	bool bError = false;
	Q_UTF8ToUChar32(pUTF8_, *uValueOut, bError);
	return bError;
}

/* <f2fe5> ../engine/unicode_strtools.cpp:137 */
int Q_UChar32ToUTF8Len(uchar32 uVal) {
	if (uVal <= 0x7F)
		return 1;

	if (uVal > 0x7FF)
		return (uVal > 0xFFFF) + 3;
	else
		return 2;
}

/* <f3192> ../engine/unicode_strtools.cpp:180 */
int Q_UChar32ToUTF8(uchar32 uVal, char * pUTF8Out) {
	if (uVal <= 0x7F)
	{
		*pUTF8Out = uVal;
		return 1;
	} 
	else if (uVal <= 0x7FF)
			{
		*pUTF8Out = (uVal >> 6) | 0xC0;
		pUTF8Out[1] = uVal & 0x3F | 0x80;
		return 2;
	}
	else if (uVal <= 0xFFFF)
	{
		*pUTF8Out = (uVal >> 12) | 0xE0;
		pUTF8Out[2] = uVal & 0x3F | 0x80;
		pUTF8Out[1] = (uVal >> 6) & 0x3F | 0x80;
		return 3;
	}
	else
	{
		*pUTF8Out = (uVal >> 18) & 7 | 0xF0;
		pUTF8Out[1] = (uVal >> 12) & 0x3F | 0x80;
		pUTF8Out[3] = uVal & 0x3F | 0x80;
		pUTF8Out[2] = (uVal >> 6) & 0x3F | 0x80;
		return 4;
	}
}

int Q_UChar32ToUTF16Len(uchar32 uVal)
{
	return (uVal > 0xFFFF) ? 2 : 1;
}

int __cdecl Q_UChar32ToUTF16(uchar32 uVal, uchar16 *pUTF16Out)
{

	if (uVal <= 0xFFFF)
	{
		pUTF16Out[0] = uVal;
		return 1;
	}
	else
	{
		pUTF16Out[1] = uVal & 0x3FF | 0xDC00;
		pUTF16Out[0] = ((uVal - 0x10000) >> 10) | 0xD800;
		return 2;
	}
}

template<
		typename T_IN, 
		typename T_OUT,
		bool UNK,
		qboolean(*IN_TO_UCHAR32)(const T_IN *pUTF8, uchar32 &uValueOut, bool &bErrorOut),
		int(UCHAR32_TO_OUT_LEN)(uchar32 uVal),
		int(UCHAR32_TO_OUT)(uchar32 uVal, T_OUT *pUTF8Out)
>
int Q_UnicodeConvertT(const T_IN* pIn, T_OUT *pOut, int nOutBytes, enum EStringConvertErrorPolicy ePolicy)
{
	int nOut = 0;
	if (pOut)
	{
		int nMaxOut = nOutBytes / sizeof(T_OUT) - 1;
		if (nMaxOut <= 0)
			return 0;

		while (*pIn)
		{
			bool bErr;
			uchar32 uVal;
			pIn += IN_TO_UCHAR32(pIn, uVal, bErr);
			int nOutElems = UCHAR32_TO_OUT_LEN(uVal);
			if (nOutElems + nOut > nMaxOut)
				break;
			nOut += UCHAR32_TO_OUT(uVal, &pOut[nOut]);
			if (bErr)
			{
				if (ePolicy & STRINGCONVERT_SKIP)
				{
					nOut -= nOutElems;
				}
				else if (ePolicy & STRINGCONVERT_FAIL)
				{
					pOut[0] = 0;
					return 0;
				}
				
			}
		};
		
		pOut[nOut] = 0;
	}
	else
	{
		while (*pIn)
		{
			bool bErr;
			uchar32 uVal;
			pIn += IN_TO_UCHAR32(pIn, uVal, bErr);
			int nOutElems = UCHAR32_TO_OUT_LEN(uVal);
			if (bErr)
			{
				if (ePolicy & STRINGCONVERT_SKIP)
				{
					nOut -= nOutElems;
				} 
				else if (ePolicy & STRINGCONVERT_FAIL)
				{
#ifndef REHLDS_FIXES
					pOut[0] = 0; //FIXME: pOut is always null there
					//TODO: V522 Dereferencing of the null pointer 'pOut' might take place.
#endif // REHLDS_FIXES
					return 0;
				}
				
			}
		}
	}
	return (nOut + 1) * sizeof(T_OUT);
}

int __cdecl Q_UTF16ToUChar32(const uchar16 *pUTF16, uchar32 &uValueOut, bool &bErrorOut)
{
	if (Q_IsValidUChar32(pUTF16[0]))
	{
		uValueOut = pUTF16[0];
		bErrorOut = false;
		return 1;
	}
	else if (pUTF16[0] - 55296 >= 0x400 || (pUTF16[1] - 56320) >= 0x400)
	{
		uValueOut = 63;
		bErrorOut = true;
		return 1;
	}
	else
	{
		uValueOut = pUTF16[1] + ((uchar32)(pUTF16[0] - 55287) << 10);
		if (Q_IsValidUChar32(uValueOut))
		{
			bErrorOut = false;
		}
		else
		{
			uValueOut = 63;
			bErrorOut = true;
		}
		return 2;
	}
}

/* <f4468> ../engine/unicode_strtools.cpp:246 */
int Q_UTF8ToUTF16(const char *pUTF8, uchar16 *pUTF16, int cubDestSizeInBytes, enum EStringConvertErrorPolicy ePolicy)
{
	return Q_UnicodeConvertT<char, uchar16, true, Q_UTF8ToUChar32, Q_UChar32ToUTF16Len, Q_UChar32ToUTF16>(pUTF8, pUTF16, cubDestSizeInBytes, ePolicy);
}

int Q_UTF16ToUTF8(const uchar16 *pUTF16, char *pUTF8, int cubDestSizeInBytes, enum EStringConvertErrorPolicy ePolicy) /* linkage=_Z13Q_UTF16ToUTF8PKtPci25EStringConvertErrorPolicy */
{
	return Q_UnicodeConvertT<uchar16, char, true, Q_UTF16ToUChar32, Q_UChar32ToUTF8Len, Q_UChar32ToUTF8>(pUTF16, pUTF8, cubDestSizeInBytes, ePolicy);
}


/* <f4a63> ../engine/unicode_strtools.cpp:724 */
int Q_UnicodeRepair(char *pUTF8)
{
	return Q_UnicodeConvertT<char, char, true, Q_UTF8ToUChar32, Q_UChar32ToUTF8Len, Q_UChar32ToUTF8>(pUTF8, pUTF8, 65535, STRINGCONVERT_SKIP);
}

/* <f4800> ../engine/unicode_strtools.cpp:691 */
qboolean Q_StripUnprintableAndSpace(char *pch)
{
	bool bStrippedAny;
	bool bStrippedWhitespace;

	int cch = Q_strlen(pch);
	int cubDest = (cch + 1) * sizeof(uchar16);
	uchar16 *pwch_alloced = (uchar16*)malloc(cubDest);
	bStrippedAny = false;
	bStrippedWhitespace = false;
	int cwch = (unsigned int)Q_UTF8ToUTF16(pch, (uchar16 *)pwch_alloced, cubDest, _STRINGCONVERTFLAG_ASSERT) >> 1;
	uchar16 * pwch = StripUnprintableWorker(pwch_alloced, &bStrippedAny);
	pwch = StripWhitespaceWorker(pwch, cwch - 1, &bStrippedWhitespace);
	if (bStrippedWhitespace || bStrippedAny)
		Q_UTF16ToUTF8(pwch, pch, cch, STRINGCONVERT_ASSERT_REPLACE);

	free(pwch_alloced);
	return bStrippedAny;
}
