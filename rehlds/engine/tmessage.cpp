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

char gNetworkTextMessageBuffer[MAX_NETMESSAGE][512];
client_textmessage_t gMessageParms;

client_textmessage_t *gMessageTable = NULL;
int gMessageTableCount = 0;

const char *gNetworkMessageNames[MAX_NETMESSAGE] =
{
	NETWORK_MESSAGE1,
	NETWORK_MESSAGE2,
	NETWORK_MESSAGE3,
	NETWORK_MESSAGE4
};

client_textmessage_t gNetworkTextMessage[MAX_NETMESSAGE] =
{
	{
		0, // effect
		255, 255, 255, 255,
		255, 255, 255, 255,
		-1.0f, // x
		-1.0f, // y
		0.0f, // fadein
		0.0f, // fadeout
		0.0f, // holdtime
		0.0f, // fxTime,
		NETWORK_MESSAGE1,// pName message name.
		gNetworkTextMessageBuffer[0] // pMessage
	}
};

char* EXT_FUNC memfgets(unsigned char *pMemFile, int fileSize, int *pFilePos, char *pBuffer, int bufferSize)
{
	int filePos;
	int last;
	int i;
	int stop;
	int size;

	filePos = *pFilePos;
	if (!pMemFile || !pBuffer || filePos >= fileSize)
		return NULL;

	last = fileSize;
	i = *pFilePos;
	if (fileSize - filePos > bufferSize - 1)
		last = filePos + bufferSize - 1;
	stop = 0;

	if (filePos >= last)
		return NULL;

	do
	{
		if (stop)
			break;
		if (pMemFile[i] == 10)
			stop = 1;
		++i;
	} while (i < last);

	if (i == filePos)
		return NULL;

	size = i - filePos;
	Q_memcpy(pBuffer, &pMemFile[filePos], i - filePos);
	if (size < bufferSize)
		pBuffer[size] = 0;
	*pFilePos = i;
	return pBuffer;
}

int IsComment(char *pText)
{
	if (!pText)
		return TRUE;

#ifdef REHLDS_FIXES
	if ((pText[0] == '/' && pText[1] == '/') || !pText[0])
		return TRUE;
#else
	int length = Q_strlen(pText);
	if ((length >= 2 && pText[0] == '/' && pText[1] == '/') || length <= 0)
	{
		return TRUE;
	}
#endif

	return FALSE;
}

int IsStartOfText(char *pText)
{
	return pText && pText[0] == '{';
}

int IsEndOfText(char *pText)
{
	return pText && pText[0] == '}';
}

int IsWhiteSpace(char space)
{
#ifdef REHLDS_FIXES
	return isspace(space);
#else
	return space == ' ' || space == '\t' || space == '\r' || space == '\n';
#endif
}

NOXREF const char *SkipSpace(const char *pText)
{
	NOXREFCHECK;
	if (pText)
	{
		int pos = 0;
		while (pText[pos] && IsWhiteSpace(pText[pos]))
			pos++;

		return pText + pos;
	}
	return NULL;
}

NOXREF const char *SkipText(const char *pText)
{
	NOXREFCHECK;
	if (pText)
	{
		int pos = 0;
		while (pText[pos] && !IsWhiteSpace(pText[pos]))
			pos++;
		return pText + pos;
	}
	return NULL;
}

NOXREF int ParseFloats(const char *pText, float *pFloat, int count)
{
	NOXREFCHECK;
	const char *pTemp = pText;
	int index = 0;

	while (pTemp && count > 0)
	{
		pTemp = SkipText(pTemp);
		pTemp = SkipSpace(pTemp);

		if (pTemp)
		{
			pFloat[index] = (float)Q_atof(pTemp);
			count--;
			index++;
		}
	}
	if (count == 0)
		return 1;

	return 0;
}

void TrimSpace(const char *source, char *dest)
{
	int start, end, length;

	length = Q_strlen(source);

	for (start = 0; start < length; start++)
	{
		if (!IsWhiteSpace(source[start]))
			break;
	}

	for (end = length - 1; end > start; end--)
	{
		if (!IsWhiteSpace(source[end]))
			break;
	}

	length = end - start + 1;

	if (length <= 0)
	{
		dest[0] = '\0';
	}
	else
	{
		Q_memmove(dest, &source[start], length);
		dest[length] = '\0';
	}
}

NOXREF int IsToken(const char *pText, const char *pTokenName)
{
	NOXREFCHECK;
	if (!pText || !pTokenName)
		return 0;

	if (!Q_strnicmp(pText + 1, pTokenName, Q_strlen(pTokenName)))
		return 1;

	return 0;
}

NOXREF int ParseDirective(const char *pText)
{
	NOXREFCHECK;
	if (pText && pText[0] == '$')
	{
		float tempFloat[8];
		if (IsToken(pText, "position"))
		{
			if (ParseFloats(pText, tempFloat, 2))
			{
				gMessageParms.x = tempFloat[0];
				gMessageParms.y = tempFloat[1];
			}
		}
		else if (IsToken(pText, "effect"))
		{
			if (ParseFloats(pText, tempFloat, 1))
			{
				gMessageParms.effect = (int)tempFloat[0];
			}
		}
		else if (IsToken(pText, "fxtime"))
		{
			if (ParseFloats(pText, tempFloat, 1))
			{
				gMessageParms.fxtime = tempFloat[0];
			}
		}
		else if (IsToken(pText, "color2"))
		{
			if (ParseFloats(pText, tempFloat, 3))
			{
				gMessageParms.r2 = (int)tempFloat[0];
				gMessageParms.g2 = (int)tempFloat[1];
				gMessageParms.b2 = (int)tempFloat[2];
			}
		}
		else if (IsToken(pText, "color"))
		{
			if (ParseFloats(pText, tempFloat, 3))
			{
				gMessageParms.r1 = (int)tempFloat[0];
				gMessageParms.g1 = (int)tempFloat[1];
				gMessageParms.b1 = (int)tempFloat[2];
			}
		}
		else if (IsToken(pText, "fadein"))
		{
			if (ParseFloats(pText, tempFloat, 1))
			{
				gMessageParms.fadein = tempFloat[0];
			}
		}
		else if (IsToken(pText, "fadeout"))
		{
			if (ParseFloats(pText, tempFloat, 3))
			{
				gMessageParms.fadeout = tempFloat[0];
			}
		}
		else if (IsToken(pText, "holdtime"))
		{
			if (ParseFloats(pText, tempFloat, 3))
			{
				gMessageParms.holdtime = tempFloat[0];
			}
		}
		else
		{
			Con_DPrintf("Unknown token: %s\n", pText);
		}
		return 1;
	}
	return 0;
}

NOXREF void TextMessageParse(unsigned char *pMemFile, int fileSize)
{
	NOXREFCHECK;
	char buf[512];
	char trim[512];
	char *pCurrentText;
	char *pNameHeap;
	char currentName[512];
	char nameHeap[NAME_HEAP_SIZE];
	int lastNamePos;
	int mode;
	int lineNumber;
	int filePos;
	int lastLinePos;
	int messageCount;
	client_textmessage_t textMessages[MAX_MESSAGES];
	int i;
	int nameHeapSize;
	int textHeapSize;
	int messageSize;
	int nameOffset;

	lastNamePos = 0;
	lineNumber = 0;
	filePos = 0;
	lastLinePos = 0;
	messageCount = 0;
	mode = MSGFILE_NAME;

	while (memfgets(pMemFile, fileSize, &filePos, buf, 512) != NULL)
	{
		if(messageCount >= MAX_MESSAGES)
			Sys_Error("%s: messageCount >= MAX_MESSAGES", __func__);

		TrimSpace(buf, trim);
		switch (mode)
		{
			case MSGFILE_NAME:
			{
				if (IsComment(trim))
					break;

				if (ParseDirective(trim))
					break;

				if (IsStartOfText(trim))
				{
					mode = MSGFILE_TEXT;
					pCurrentText = (char *)(pMemFile + filePos);
					break;
				}
				if (IsEndOfText(trim))
				{
					Con_DPrintf("Unexpected '}' found, line %d\n", lineNumber);
					return;
				}
				Q_strncpy(currentName, trim, 511);
				currentName[511] = 0;

				break;
			}
			case MSGFILE_TEXT:
			{
				if (IsEndOfText(trim))
				{
					int length = Q_strlen(currentName);
					if (lastNamePos + length > sizeof(nameHeap))
					{
						Con_DPrintf("Error parsing file!  length > %i bytes\n", sizeof(nameHeap));
						return;
					}

					Q_strcpy(nameHeap + lastNamePos, currentName);

					pMemFile[lastLinePos - 1] = 0;

					textMessages[messageCount] = gMessageParms;
					textMessages[messageCount].pName = nameHeap + lastNamePos;
					lastNamePos += Q_strlen(currentName) + 1;
					textMessages[messageCount].pMessage = pCurrentText;
					messageCount++;

					mode = MSGFILE_NAME;
					break;
				}
				if (IsStartOfText(trim))
				{
					Con_DPrintf("Unexpected '{' found, line %d\n", lineNumber);
					return;
				}
				break;
			}
		}

		lineNumber++;
		lastLinePos = filePos;
	}

	Con_DPrintf("Parsed %d text messages\n", messageCount);
	nameHeapSize = lastNamePos;
	textHeapSize = 0;

	for (i = 0; i < messageCount; i++)
		textHeapSize += Q_strlen(textMessages[i].pMessage) + 1;

	messageSize = (messageCount * sizeof(client_textmessage_t));

	gMessageTable = (client_textmessage_t *)Mem_Malloc(textHeapSize + nameHeapSize + messageSize);

	Q_memcpy(gMessageTable, textMessages, messageSize);

	pNameHeap = ((char *)gMessageTable) + messageSize;
	Q_memcpy(pNameHeap, nameHeap, nameHeapSize);
	nameOffset = pNameHeap - gMessageTable[0].pName;

	pCurrentText = pNameHeap + nameHeapSize;
	for (i = 0; i < messageCount; i++)
	{
		gMessageTable[i].pName += nameOffset;
		Q_strcpy(pCurrentText, gMessageTable[i].pMessage);
		gMessageTable[i].pMessage = pCurrentText;
		pCurrentText += Q_strlen(pCurrentText) + 1;
	}

	gMessageTableCount = messageCount;
}

NOXREF void TextMessageShutdown(void)
{
	NOXREFCHECK;
	if (gMessageTable)
	{
		Mem_Free(gMessageTable);
		gMessageTable = NULL;
	}
}

NOXREF void TextMessageInit(void)
{
	NOXREFCHECK;
	int fileSize;
	unsigned char *pMemFile;

	if (gMessageTable)
	{
		Mem_Free(gMessageTable);
		gMessageTable = NULL;
	}

	pMemFile = COM_LoadTempFile("titles.txt", &fileSize);

	if (pMemFile)
		TextMessageParse(pMemFile, fileSize);
}

NOXREF client_textmessage_t *TextMessageGet(const char *pName)
{
	NOXREFCHECK;
	int i;

#ifndef SWDS

	g_engdstAddrs->pfnTextMessageGet(&pName);

	if (!Q_stricmp(pName, DEMO_MESSAGE))
		return &tm_demomessage;
#endif // SWDS

	if (!Q_stricmp(pName, NETWORK_MESSAGE1))
		return gNetworkTextMessage;

	else if (!Q_stricmp(pName, NETWORK_MESSAGE2))
		return gNetworkTextMessage + 1;

	else if (!Q_stricmp(pName, NETWORK_MESSAGE3))
		return gNetworkTextMessage + 2;

	else if (!Q_stricmp(pName, NETWORK_MESSAGE4))
		return gNetworkTextMessage + 3;

	for (i = 0; i < gMessageTableCount; i++)
	{
		if (!Q_strcmp(pName, gMessageTable[i].pName))
			return &gMessageTable[i];
	}

	return NULL;
}
