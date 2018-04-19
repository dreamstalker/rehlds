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

// NOTE: This file contains a lot of fixes that are not covered by REHLDS_FIXES define.
// TODO: Most of the Info_ functions can be speedup via removing unneded copy of key and values.

// Searches the string for the given
// key and returns the associated value, or an empty string.
const char* EXT_FUNC Info_ValueForKey(const char *s, const char *key)
{
	// use few (two?) buffers so compares work without stomping on each other
	static char value[INFO_MAX_BUFFER_VALUES][MAX_KV_LEN];
	static int valueindex;
	char pkey[MAX_KV_LEN];
	char *c;
	int nCount;

	while (*s)
	{
		if (*s == '\\')
		{
			s++;	// skip the slash
		}

		// Copy a key
		nCount = 0;
		c = pkey;
		while (*s != '\\')
		{
			if (!*s)
			{
				return "";		// key should end with a \, not a NULL, but suppose its value as absent
			}
			if (nCount >= MAX_KV_LEN)
			{
				s++;
				continue;	// skip oversized key chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;
		s++;	// skip the slash

		// Copy a value
		nCount = 0;
		c = value[valueindex];
		while (*s != '\\')
		{
			if (!*s)
			{
				break;		// allow value to be ended with NULL
			}
			if (nCount >= MAX_KV_LEN)
			{
				s++;
				continue;	// skip oversized value chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;

		if (!Q_strcmp(key, pkey))
		{
			c = value[valueindex];
			valueindex = (valueindex + 1) % INFO_MAX_BUFFER_VALUES;
			return c;
		}
	}

	return "";
}

void Info_RemoveKey(char *s, const char *key)
{
	char pkey[MAX_KV_LEN];
	char value[MAX_KV_LEN];
	char *start;
	char *c;
	int cmpsize;
	int nCount;

	if (Q_strstr(key, "\\"))
	{
		Con_Printf("Can't use a key with a \\\n");
		return;
	}

	cmpsize = Q_strlen(key);
	if (cmpsize > MAX_KV_LEN - 1)
		cmpsize = MAX_KV_LEN - 1;

	while (*s)
	{
		start = s;

		if (*s == '\\')
		{
			s++;	// skip the slash
		}

		// Copy a key
		nCount = 0;
		c = pkey;
		while (*s != '\\')
		{
			if (!*s)
			{
				break;		// key should end with a \, not a NULL, but allow to remove it
			}
			if (nCount >= MAX_KV_LEN)
			{
				s++;
				continue;	// skip oversized key chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;
		if (*s)
			s++;	// skip the slash

		// Copy a value
		nCount = 0;
		c = value;
		while (*s != '\\')
		{
			if (!*s)
			{
				break;		// allow value to be ended with NULL
			}
			if (nCount >= MAX_KV_LEN)
			{
				s++;
				continue;	// skip oversized value chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;

		// Compare keys
		if (!Q_strncmp(key, pkey, cmpsize))
		{
			Q_strcpy_s(start, s);	// remove this part
			s = start;	// continue searching
		}
	}
}

void Info_RemovePrefixedKeys(char *s, const char prefix)
{
	char pkey[MAX_KV_LEN];
	char value[MAX_KV_LEN];
	char *start;
	char *c;
	int nCount;

	while (*s)
	{
		start = s;

		if (*s == '\\')
		{
			s++;	// skip the slash
		}

		// Copy a key
		nCount = 0;
		c = pkey;
		while (*s != '\\')
		{
			if (!*s)
			{
				break;		// key should end with a \, not a NULL, but allow to remove it
			}
			if (nCount >= MAX_KV_LEN)
			{
				s++;
				continue;	// skip oversized key chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;
		if (*s)
			s++;	// skip the slash

		// Copy a value
		nCount = 0;
		c = value;
		while (*s != '\\')
		{
			if (!*s)
			{
				break;		// allow value to be ended with NULL
			}
			if (nCount >= MAX_KV_LEN)
			{
				s++;
				continue;	// skip oversized value chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;

		if (pkey[0] == prefix)
		{
			Q_strcpy_s(start, s);	// remove this part
			s = start;	// continue searching
		}
	}
}

qboolean Info_IsKeyImportant(const char *key)
{
	if (key[0] == '*')
		return true;
	if (!Q_strcmp(key, "name"))
		return true;
	if (!Q_strcmp(key, "model"))
		return true;
	if (!Q_strcmp(key, "rate"))
		return true;
	if (!Q_strcmp(key, "topcolor"))
		return true;
	if (!Q_strcmp(key, "bottomcolor"))
		return true;
	if (!Q_strcmp(key, "cl_updaterate"))
		return true;
	if (!Q_strcmp(key, "cl_lw"))
		return true;
	if (!Q_strcmp(key, "cl_lc"))
		return true;
#ifndef REHLDS_FIXES
	// keys starts from '*' already checked
	if (!Q_strcmp(key, "*hltv"))
		return true;
	if (!Q_strcmp(key, "*sid"))
		return true;
#endif

	return false;
}

char *Info_FindLargestKey(char *s, int maxsize)
{
	static char largest_key[MAX_KV_LEN];
	char key[MAX_KV_LEN];
	char value[MAX_KV_LEN];
	char *c;
	int nCount;
	int largest_size = 0;

	largest_key[0] = 0;

	while (*s)
	{
		if (*s == '\\')
		{
			s++;	// skip the slash
		}

		// Copy a key
		nCount = 0;
		c = key;
		while (*s != '\\')
		{
			if (!*s)		// key should end with a \, not a NULL, return this key, so it will be deleted as wrong
			{
				*c = 0;
				Q_strcpy(largest_key, key);
				return largest_key;
			}
			if (nCount >= MAX_KV_LEN)	// oversized key, return this key, so it will be deleted as wrong
			{
				*c = 0;
				Q_strcpy(largest_key, key);
				return largest_key;
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;
		s++;	// skip the slash

		// Get length
		int size = c - key;

		// Copy a value
		nCount = 0;
		c = value;
		while (*s != '\\')
		{
			if (!*s)
			{
				break;				// allow value to be ended with NULL
			}
			if (nCount >= MAX_KV_LEN)	// oversized value, return this key, so it will be deleted as wrong
			{
				*c = 0;
				Q_strcpy(largest_key, key);
				return largest_key;
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;

		// Add length
		size += c - value;

		if (size > largest_size && !Info_IsKeyImportant(key))
		{
			largest_size = size;
			Q_strcpy(largest_key, key);
		}
	}

	return largest_key;
}

void Info_SetValueForStarKey(char *s, const char *key, const char *value, int maxsize)
{
	char newArray[MAX_INFO_STRING];
	char *v;
	int c;

	if (!key || !value)
	{
		Con_Printf("Keys and values can't be null\n");
		return;
	}

	if (key[0] == 0)
	{
		Con_Printf("Keys can't be an empty string\n");
		return;
	}

	if (Q_strstr(key, "\\") || Q_strstr(value, "\\"))
	{
		Con_Printf("Can't use keys or values with a \\\n");
		return;
	}

	if (Q_strstr(key, "..") || Q_strstr(value, ".."))
	{
		// TODO: Why silently return?
		//Con_Printf("Can't use keys or values with a ..\n");
		return;
	}

	if (Q_strstr(key, "\"") || Q_strstr(value, "\""))
	{
		Con_Printf("Can't use keys or values with a \"\n");
		return;
	}

	int keyLen = Q_strlen(key);
	int valueLan = Q_strlen(value);

	if (keyLen >= MAX_KV_LEN || valueLan >= MAX_KV_LEN)
	{
		Con_Printf("Keys and values must be < %i characters\n", MAX_KV_LEN);
		return;
	}

	if (!Q_UnicodeValidate(key) || !Q_UnicodeValidate(value))
	{
		Con_Printf("Keys and values must be valid utf8 text\n");
		return;
	}

	// Remove current key/value and return if we doesn't specified to set a value
	Info_RemoveKey(s, key);
	if (value[0] == 0)
	{
		return;
	}

	// Create key/value pair
	Q_snprintf(newArray, MAX_INFO_STRING - 1, "\\%s\\%s", key, value);
	newArray[MAX_INFO_STRING - 1] = 0;

	int neededLength = Q_strlen(newArray);
	if ((int)Q_strlen(s) + neededLength >= maxsize)
	{
		// no more room in the buffer to add key/value
		if (!Info_IsKeyImportant(key))
		{
			// no room to add setting
			Con_Printf("Info string length exceeded\n");
			return;
		}

		// keep removing the largest key/values until we have a room
		char *largekey;
		do
		{
			largekey = Info_FindLargestKey(s, maxsize);
			if (largekey[0] == 0)
			{
				// no room to add setting
				Con_Printf("Info string length exceeded\n");
				return;
			}
			Info_RemoveKey(s, largekey);
		} while ((int)Q_strlen(s) + neededLength >= maxsize);
	}

	// auto lowercase team
	bool lowerCaseValue = Q_stricmp(key, "team") == 0;
	s += Q_strlen(s);
	v = newArray;
	while (*v)
	{
		c = (unsigned char)*v++;
		if (lowerCaseValue)
		{
			c = tolower(c);
		}
		*s++ = c;
	}
	*s = 0;
}

void Info_SetValueForKey(char *s, const char *key, const char *value, int maxsize)
{
	if (key[0] == '*')
	{
		Con_Printf("Can't set * keys\n");
		return;
	}

	Info_SetValueForStarKey(s, key, value, maxsize);
}

void Info_Print(const char *s)
{
	char key[MAX_KV_LEN];
	char value[MAX_KV_LEN];
	char *c;
	int l;
	int nCount;

	while (*s)
	{
		if (*s == '\\')
		{
			s++;	// skip the slash
		}

		// Copy a key
		nCount = 0;
		c = key;
		while (*s != '\\')
		{
			if (!*s)
			{
				break;		// key should end with a \, not a NULL, but allow to print it
			}
			if (nCount >= MAX_KV_LEN)
			{
				s++;
				continue;	// skip oversized key chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;
		if (*s)
			s++;	// skip the slash

		// Pad and print a key
		l = c - key;
		if (l < 20)
		{
			Q_memset(c, ' ', 20 - l);
			key[20] = 0;
		}
		Con_Printf("%s", key);

		if (!*s)
		{
			Con_Printf("MISSING VALUE\n");
			return;
		}

		// Copy a value
		nCount = 0;
		c = value;
		while (*s != '\\')
		{
			if (!*s)
			{
				break;		// allow value to be ended with NULL
			}
			if (nCount >= MAX_KV_LEN)
			{
				s++;
				continue;	// skip oversized value chars till the slash or EOL
			}
			*c++ = *s++;
			nCount++;
		}
		*c = 0;

		Con_Printf("%s\n", value);
	}
}

qboolean Info_IsValid(const char *s)
{
	while (*s)
	{
		if (*s == '\\')
		{
			s++;	// skip the slash
		}

		// Returns character count
		// -1 - error
		//  0 - string size is zero
		enum class AllowNull {
			Yes,
			No,
		};
		auto validate = [&s](AllowNull allowNull) -> int
		{
			int nCount = 0;

			for(; *s != '\\'; nCount++, s++)
			{
				if (!*s)
				{
					return (allowNull == AllowNull::Yes) ? nCount : -1;
				}

				if (nCount >= MAX_KV_LEN)
				{
					return -1;		// string length should be less then MAX_KV_LEN
				}

#ifdef REHLDS_FIXES
				if (*s == '\"')
				{
					return -1; // string should not contain "
				}
#endif
			}
			return nCount;
		};

		if (validate(AllowNull::No) == -1)
		{
			return FALSE;
		}
		s++; // Skip slash

		if (validate(AllowNull::Yes) <= 0)
		{
			return FALSE;
		}

		if (!*s)
		{
			return TRUE;	// EOL, info is valid
		}
	}

	return FALSE;
}

#ifdef REHLDS_FIXES
void Info_CollectFields(char *destInfo, const char *srcInfo, const char *collectedKeysOfFields)
{
	char keys[MAX_INFO_STRING];
	Q_strcpy(keys, collectedKeysOfFields);

	size_t userInfoLength = 0;
	for (const char *key = strtok(keys, "\\"); key; key = strtok(nullptr, "\\"))
	{
		const char *value = Info_ValueForKey(srcInfo, key);

		if (value[0] == '\0')
			continue;

		// Integer fields
		if (!Q_strcmp(key, "*hltv")
		 || !Q_strcmp(key, "bottomcolor")
		 || !Q_strcmp(key, "topcolor"))
		{
			int intValue = Q_atoi(value);

			if (!intValue)
				continue;

			destInfo[userInfoLength++] = '\\';
			Q_strcpy(&destInfo[userInfoLength], key);
			userInfoLength += Q_strlen(key);

			destInfo[userInfoLength++] = '\\';
			userInfoLength += Q_sprintf(&destInfo[userInfoLength], "%d", intValue);
		}
		// String fields
		else
		{
			destInfo[userInfoLength++] = '\\';
			Q_strcpy(&destInfo[userInfoLength], key);
			userInfoLength += Q_strlen(key);

			destInfo[userInfoLength++] = '\\';
			Q_strcpy(&destInfo[userInfoLength], value);
			userInfoLength += Q_strlen(value);
		}

	}
	destInfo[userInfoLength] = '\0';
}
#endif // REHLDS_FIXES
