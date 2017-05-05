#pragma once

#ifdef HOOK_HLTV

#include "hookers/memory.h"
#include "hookers/helper.h"
#include "hookers/hooker.h"

#define rehlds_syserror HLTV_SysError

#define com_token (*pcom_token)
#define s_com_token_unget (*ps_com_token_unget)
#define g_DownloadURL (*pg_DownloadURL)

extern char com_token[COM_TOKEN_LEN];
extern qboolean s_com_token_unget;

#endif // HOOK_HLTV
