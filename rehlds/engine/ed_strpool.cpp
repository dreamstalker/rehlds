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
		Sys_Error("%s: Too long string allocated: %s", __FUNCTION__, origStr);
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
