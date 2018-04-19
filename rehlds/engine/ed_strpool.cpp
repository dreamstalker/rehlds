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

class CStringPoolMap : public CStaticMap<const char*, char*, 8, 2048> {
protected:
	virtual uint32 hash(const char* const &val) {
		unsigned int len = Q_strlen(val);
		return crc32c((const uint8*)val, len);
	}

	virtual bool equals(const char* const &val1, const char* const &val2) {
		return !Q_strcmp(val1, val2);
	}

public:
	CStringPoolMap() {
	}
};

CStringPoolMap g_EdStringPool;
sizebuf_t g_EdStringPool_Hunk;

void Ed_StrPool_Init() {
	Q_memset(&g_EdStringPool_Hunk, 0, sizeof(g_EdStringPool_Hunk));

	g_EdStringPool_Hunk.maxsize = 128 * 1024;
	g_EdStringPool_Hunk.data = (byte*) Hunk_AllocName(g_EdStringPool_Hunk.maxsize, "Ed_StrPool");
	g_EdStringPool_Hunk.cursize = 0;
	g_EdStringPool_Hunk.buffername = "Ed_StrPool";
	g_EdStringPool_Hunk.flags = SIZEBUF_ALLOW_OVERFLOW;
}

void Ed_StrPool_Reset() {
	g_EdStringPool_Hunk.cursize = 0;
	g_EdStringPool_Hunk.flags = SIZEBUF_ALLOW_OVERFLOW;
	g_EdStringPool.clear();
}

char* Ed_StrPool_Alloc(const char* origStr) {
	char str[2048];
	unsigned int len = Q_strlen(origStr) + 1;

	if (len >= ARRAYSIZE(str)) {
		Sys_Error("%s: Too long string allocated: %s", __func__, origStr);
	}

	Q_strcpy(str, origStr);
	char* new_p = str;
	for (unsigned int i = 0; i < len; i++, new_p++)
	{
		if (str[i] == '\\' && str[i + 1] == 'n')
		{
			// Replace "\\n" with "\n"
			*new_p = '\n';
			i++;
		}
		else
		{
			*new_p = str[i];
		}
	}

	*new_p = 0;
	len = Q_strlen(str) + 1;

	auto node = g_EdStringPool.get(str);
	if (node) {
		return node->val;
	}

	char* val = NULL;
	
	//try to alloc string from shared hunk
	if (!(g_EdStringPool_Hunk.flags & SIZEBUF_OVERFLOWED)) {
		val = (char*)g_EdStringPool_Hunk.data + g_EdStringPool_Hunk.cursize;
		MSG_WriteString(&g_EdStringPool_Hunk, str);
		if (g_EdStringPool_Hunk.flags & SIZEBUF_OVERFLOWED) {
			val = NULL;
		}
	}

	if (!val) {
		val = (char*) Hunk_Alloc(len);
		Q_memcpy(val, str, len);
	}

	g_EdStringPool.put(val, val);

	return val;
}
