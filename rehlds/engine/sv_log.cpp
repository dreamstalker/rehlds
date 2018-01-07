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

LOGLIST_T *firstLog;

cvar_t mp_logecho = { "mp_logecho", "1", 0, 0.0f, NULL };
cvar_t mp_logfile = { "mp_logfile", "1", FCVAR_SERVER, 0.0f, NULL };

void Log_Printf(const char *fmt, ...)
{
	va_list argptr;
	char string[1024];
	time_t ltime;
	tm *today;
	LOGLIST_T *list;

	if (!g_psvs.log.net_log_ && !firstLog && !g_psvs.log.active)
		return;

	time(&ltime);
	today = localtime(&ltime);

	va_start(argptr, fmt);
	Q_snprintf(string,sizeof(string), "L %02i/%02i/%04i - %02i:%02i:%02i: ",
		today->tm_mon + 1,
		today->tm_mday,
		today->tm_year + 1900,
		today->tm_hour,
		today->tm_min,
		today->tm_sec);

	Q_vsnprintf(&string[Q_strlen(string)], sizeof(string) - Q_strlen(string), fmt, argptr);
	va_end(argptr);

#ifdef REHLDS_FLIGHT_REC
	FR_Log("REHLDS_LOG", string);
#endif

	if (g_psvs.log.net_log_ || firstLog != NULL)
	{
		if (g_psvs.log.net_log_)
			Netchan_OutOfBandPrint(NS_SERVER, g_psvs.log.net_address_, "log %s", string);

		for (list = firstLog; list != NULL; list = list->next)
		{
			if (sv_logsecret.value == 0.0f)
				Netchan_OutOfBandPrint(NS_SERVER, list->log.net_address_, "log %s", string);

			else Netchan_OutOfBandPrint(NS_SERVER, list->log.net_address_, "%c%s%s", S2A_LOGKEY, sv_logsecret.string, string);
		}
	}
	if (g_psvs.log.active && (g_psvs.maxclients > 1 || sv_log_singleplayer.value != 0.0f))
	{
		if (mp_logecho.value != 0.0f)
			Con_Printf("%s", string);

		if (g_psvs.log.file)
		{
			if (mp_logfile.value != 0.0f)
				FS_FPrintf((FileHandle_t)g_psvs.log.file, "%s", string);
		}
	}
}

void Log_PrintServerVars(void)
{
	cvar_t *var;
	if (!g_psvs.log.active)
		return;

	Log_Printf("Server cvars start\n");
	for (var = cvar_vars; var != NULL; var = var->next)
	{
		if (var->flags & FCVAR_SERVER)
			Log_Printf("Server cvar \"%s\" = \"%s\"\n", var->name, var->string);
	}
	Log_Printf("Server cvars end\n");
}

void Log_Close(void)
{
	if (g_psvs.log.file)
	{
		Log_Printf("Log file closed\n");
		FS_Close((FileHandle_t)g_psvs.log.file);
	}
	g_psvs.log.file = NULL;
}

void Log_Open(void)
{
	time_t ltime;
	struct tm *today;
	char szFileBase[MAX_PATH];
	char szTestFile[MAX_PATH];
	int i;
	FileHandle_t fp;
	char *temp;

	if (!g_psvs.log.active || (sv_log_onefile.value != 0.0f && g_psvs.log.file))
		return;

	if (mp_logfile.value == 0.0f)
		Con_Printf("Server logging data to console.\n");
	else
	{
		Log_Close();
		time(&ltime);
		today = localtime(&ltime);

		temp = Cvar_VariableString("logsdir");

		if (!temp || Q_strlen(temp) <= 0 || Q_strstr(temp, ":") || Q_strstr(temp, ".."))
			Q_snprintf(szFileBase, sizeof(szFileBase), "logs/L%02i%02i", today->tm_mon + 1, today->tm_mday);

		else Q_snprintf(szFileBase, sizeof(szFileBase), "%s/L%02i%02i", temp, today->tm_mon + 1, today->tm_mday);

		for (i = 0; i < 1000; i++)
		{
			Q_snprintf(szTestFile, sizeof(szTestFile), "%s%03i.log", szFileBase, i);

			COM_FixSlashes(szTestFile);
			COM_CreatePath(szTestFile);

			fp = FS_OpenPathID(szTestFile, "r", "GAMECONFIG");
			if (!fp)
			{
				COM_CreatePath(szTestFile);
				fp = FS_OpenPathID(szTestFile, "wt", "GAMECONFIG");
				if (fp)
				{
					g_psvs.log.file = (void *)fp;
					Con_Printf("Server logging data to file %s\n", szTestFile);
					Log_Printf("Log file started (file \"%s\") (game \"%s\") (version \"%i/%s/%d\")\n", szTestFile, Info_ValueForKey(Info_Serverinfo(), "*gamedir"), PROTOCOL_VERSION, gpszVersionString, build_number());
				}
				return;
			}
			FS_Close(fp);
		}
		Con_Printf("Unable to open logfiles under %s\nLogging disabled\n", szFileBase);
		g_psvs.log.active = FALSE;
	}
}

void SV_SetLogAddress_f(void)
{
	const char *s;
	int nPort;
	char szAdr[MAX_PATH];
	netadr_t adr;

	if (Cmd_Argc() != 3)
	{
		Con_Printf("logaddress:  usage\nlogaddress ip port\n");
		if (g_psvs.log.active)
			Con_Printf("current:  %s\n", NET_AdrToString(g_psvs.log.net_address_));
		return;
	}

	nPort = Q_atoi(Cmd_Argv(2));
	if (!nPort)
	{
		Con_Printf("logaddress:  must specify a valid port\n");
		return;
	}

	s = Cmd_Argv(1);
	if (!s || *s == '\0')
	{
		Con_Printf("logaddress:  unparseable address\n");
		return;
	}

	Q_snprintf(szAdr, sizeof(szAdr), "%s:%i", s, nPort);

	if (!NET_StringToAdr(szAdr, &adr))
	{
		Con_Printf("logaddress:  unable to resolve %s\n", szAdr);
		return;
	}

	g_psvs.log.net_log_ = TRUE;
	Q_memcpy(&g_psvs.log.net_address_, &adr, sizeof(netadr_t));
	Con_Printf("logaddress:  %s\n", NET_AdrToString(adr));
}

void SV_AddLogAddress_f(void)
{
	const char *s;
	int nPort;
	char szAdr[MAX_PATH];
	netadr_t adr;
	LOGLIST_T *list;
	qboolean found = FALSE;
	LOGLIST_T *tmp;

	if (Cmd_Argc() != 3)
	{
		Con_Printf("logaddress_add:  usage\nlogaddress_add ip port\n");
		for (list = firstLog; list != NULL; list = list->next)
			Con_Printf("current:  %s\n", NET_AdrToString(list->log.net_address_));
		return;
	}

	nPort = Q_atoi(Cmd_Argv(2));
	if (!nPort)
	{
		Con_Printf("logaddress_add:  must specify a valid port\n");
		return;
	}

	s = Cmd_Argv(1);
	if (!s || *s == '\0')
	{
		Con_Printf("logaddress_add:  unparseable address\n");
		return;
	}
	Q_snprintf(szAdr, sizeof(szAdr), "%s:%i", s, nPort);

	if (!NET_StringToAdr(szAdr, &adr))
	{
		Con_Printf("logaddress_add:  unable to resolve %s\n", szAdr);
		return;
	}

	if (firstLog)
	{
		for (list = firstLog; list != NULL; list = list->next)
		{
#ifdef REHLDS_FIXES
			//for IPX support
			if (NET_CompareAdr(adr, list->log.net_address_))
#else
			if (Q_memcmp(adr.ip, list->log.net_address_.ip, 4) == 0 && adr.port == list->log.net_address_.port)
#endif // REHLDS_FIXES
			{
				found = TRUE;
				break;
			}
		}
		if (found)
		{
			Con_Printf("logaddress_add:  address already in list\n");
			return;
		}
		tmp = (LOGLIST_T *)Mem_Malloc(sizeof(LOGLIST_T));
		if (!tmp)
		{
			Con_Printf("logaddress_add:  error allocating new node\n");
			return;
		}

		tmp->next = NULL;
		Q_memcpy(&tmp->log.net_address_, &adr, sizeof(netadr_t));

		list = firstLog;

		while (list->next)
			list = list->next;

		list->next = tmp;
	}
	else
	{
		firstLog = (LOGLIST_T *)Mem_Malloc(sizeof(LOGLIST_T));
		if (!firstLog)
		{
			Con_Printf("logaddress_add:  error allocating new node\n");
			return;
		}
		firstLog->next = NULL;
		Q_memcpy(&firstLog->log.net_address_, &adr, sizeof(netadr_t));
	}

	Con_Printf("logaddress_add:  %s\n", NET_AdrToString(adr));
}

void SV_DelLogAddress_f(void)
{
	const char *s;
	int nPort;
	char szAdr[MAX_PATH];
	netadr_t adr;
	LOGLIST_T *list;
	LOGLIST_T *prev;
	qboolean found = FALSE;

	if (Cmd_Argc() != 3)
	{
		Con_Printf("logaddress_del:  usage\nlogaddress_del ip port\n");
		for (list = firstLog; list != NULL; list = list->next)
			Con_Printf("current:  %s\n", NET_AdrToString(list->log.net_address_));
		return;
	}
	nPort = Q_atoi(Cmd_Argv(2));
	if (!nPort)
	{
		Con_Printf("logaddress_del:  must specify a valid port\n");
		return;
	}

	s = Cmd_Argv(1);
	if (!s || *s == '\0')
	{
		Con_Printf("logaddress_del:  unparseable address\n");
		return;
	}
	Q_snprintf(szAdr, sizeof(szAdr), "%s:%i", s, nPort);
	if (!NET_StringToAdr(szAdr,&adr))
	{
		Con_Printf("logaddress_del:  unable to resolve %s\n", szAdr);
		return;
	}
	if (!firstLog)
	{
		Con_Printf("logaddress_del:  No addresses added yet\n");
		return;
	}
	for(list = firstLog, prev = firstLog; list != NULL; list = list->next)
	{
#ifdef REHLDS_FIXES
		//for IPX
		if (NET_CompareAdr(adr,list->log.net_address_))
#else
		if (Q_memcmp(adr.ip, list->log.net_address_.ip, 4) == 0 && adr.port == list->log.net_address_.port)
#endif // REHLDS_FIXES
		{
			found = TRUE;
			if (list == prev)
			{
				firstLog = prev->next;
				Mem_Free(prev);
			}
			else
			{
				prev->next = list->next;
				Mem_Free(list);
			}
			break;
		}
		prev = list;
	}
	if (!found)
	{
		Con_Printf("logaddress_del:  Couldn't find address in list\n");
		return;
	}
	Con_Printf("deleting:  %s\n", NET_AdrToString(adr));
}

void SV_ServerLog_f(void)
{
	if (Cmd_Argc() != 2)
	{
		Con_Printf("usage:  log < on | off >\n");

		if (g_psvs.log.active)
			Con_Printf("currently logging\n");

		else Con_Printf("not currently logging\n");
		return;
	}

	const char *s = Cmd_Argv(1);
	if (Q_stricmp(s, "off"))
	{
		if (Q_stricmp(s, "on"))
			Con_Printf("log:  unknown parameter %s, 'on' and 'off' are valid\n", s);
		else
		{
			g_psvs.log.active = TRUE;
			Log_Open();
		}
	}
	else if (g_psvs.log.active)
	{
		Log_Close();
		Con_Printf("Server logging disabled.\n");
		g_psvs.log.active = FALSE;
	}
}
