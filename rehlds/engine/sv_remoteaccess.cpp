#include "precompiled.h"

class CServerRemoteAccess g_ServerRemoteAccess;

void CServerRemoteAccess::WriteDataRequest(const void *buffer, int bufferSize)
{
	WriteDataRequest_noVirt(buffer, bufferSize);
}

int CServerRemoteAccess::ReadDataResponse(void *data, int len)
{
	return ReadDataResponse_noVirt(data, len);
}

void CServerRemoteAccess::WriteDataRequest_noVirt(const void *buffer, int bufferSize)
{
	int requestID;
	int requestType;
	char value[256];
	char command[256];
	char variable[256];

	CUtlBuffer cmd(buffer, bufferSize, false);

	this->m_iBytesReceived += bufferSize;
	requestID = cmd.GetInt();
	requestType = cmd.GetInt();

	switch (requestType)
	{
	case 0:
		cmd.GetString(variable);
		RequestValue(requestID, variable);
		break;

	case 1:
		cmd.GetString(variable);
		cmd.GetString(value);
		SetValue(variable, value);
		break;

	case 2:
		cmd.GetString(command);
		ExecCommand(command);
	}
}

int CServerRemoteAccess::ReadDataResponse_noVirt(void *data, int len)
{
	int i = m_ResponsePackets.Head();
	if (!m_ResponsePackets.IsValidIndex(i))
		return 0;

	CUtlBuffer &response = m_ResponsePackets.Element(i).packet;
	int bytesToCopy = response.TellPut();
	if (bytesToCopy > len)
		bytesToCopy = 0;

	if (bytesToCopy)
		Q_memcpy(data, response.Base(), bytesToCopy);

	m_iBytesSent += bytesToCopy;

	m_ResponsePackets.Remove(i);

	return bytesToCopy;
}

void CServerRemoteAccess::SendMessageToAdminUI(const char *message)
{
	DataResponse_t &resp = m_ResponsePackets.Element(m_ResponsePackets.AddToTail());
	resp.packet.PutInt(0);
	resp.packet.PutInt(1);
	resp.packet.PutString(message);
}

const char* CServerRemoteAccess::LookupStringValue(const char *variable)
{
	static char s_ReturnBuf[32];

	cvar_t *var = Cvar_FindVar(variable);
	if (var)
		return var->string;

	if (!Q_stricmp(variable, "map"))
		return g_psv.name;

	if (!Q_stricmp(variable, "playercount"))
	{
		int count = 0;
		for (int i = 0; i < g_psvs.maxclients; i++)
		{
			if (g_psvs.clients[i].active || g_psvs.clients[i].spawned || g_psvs.clients[i].connected)
				count++;
		}

		_snprintf(s_ReturnBuf, sizeof(s_ReturnBuf) - 1, "%d", count);
		return s_ReturnBuf;
	}

	if (!Q_stricmp(variable, "maxplayers"))
	{
		_snprintf(s_ReturnBuf, sizeof(s_ReturnBuf) - 1, "%d", g_psvs.maxclients);
		return s_ReturnBuf;
	}

	if (!Q_stricmp(variable, "gamedescription"))
		return gEntityInterface.pfnGetGameDescription();

	return NULL;
}

void CServerRemoteAccess::GetUserBanList(CUtlBuffer &value)
{
	for (int i = 0; i < numuserfilters; i++)
	{
		value.Printf("%i %s : %.3f min\n", i + 1, SV_GetIDString(&userfilters[i].userid), userfilters[i].banTime);
	}

	for (int i = 0; i < numuserfilters; i++)
	{
		ipfilter_t* f = &ipfilters[i];
		value.Printf("%i %i.%i.%i.%i : %.3f min\n", numuserfilters + i + 1, f->compare.octets[0], f->compare.octets[1], f->compare.octets[2], f->compare.octets[3], f->banTime);
	}

	value.PutChar(0);
}

void CServerRemoteAccess::GetPlayerList(CUtlBuffer &value)
{
	for (int i = 0; i < g_psvs.maxclients; ++i)
	{
		client_t* cli = &g_psvs.clients[i];
		if (!cli->active || Q_strlen(cli->name) < 1)
			continue;

		value.Printf("\"%s\" %s %s %d %d %d %d\n", cli->name, SV_GetIDString(&cli->network_userid), NET_AdrToString(cli->netchan.remote_address),
			int(cli->latency * 1000.0), (int)cli->packet_loss, (int)cli->edict->v.frags, int(realtime - cli->netchan.connect_time));

	}

	value.PutChar(0);
}

void CServerRemoteAccess::GetMapList(CUtlBuffer &value)
{
	const char *findfn;
	char *extension;
	char curDir[MAX_PATH];
	char mapName[MAX_PATH];
	char mapwild[64];

	Q_strcpy(mapwild, "maps/*.bsp");
	for (findfn = Sys_FindFirst(mapwild, 0); findfn; findfn = Sys_FindNext(0))
	{
		_snprintf(curDir, MAX_PATH, "maps/%s", findfn);
		FS_GetLocalPath(curDir, curDir, MAX_PATH);
		if (Q_strstr(curDir, com_gamedir))
		{
			Q_strcpy(mapName, findfn);
			extension = Q_strstr(mapName, ".bsp");
			if (extension)
				*extension = 0;

			value.PutString(mapName);
			value.PutString("\n");
		}
	}
	Sys_FindClose();

	value.PutChar(0);
}

bool CServerRemoteAccess::LookupValue(const char *variable, CUtlBuffer &value)
{
	const char* strval = LookupStringValue(variable);
	if (strval)
	{
		value.PutString(strval);
		value.PutChar(0);
		return true;
	}

	if (!Q_stricmp(variable, "stats"))
	{
		char stats[512];
		GetStatsString(stats, sizeof(stats));
		value.PutString(stats);
		value.PutChar(0);
		return true;
	}

	if (!Q_stricmp(variable, "banlist"))
	{
		GetUserBanList(value);
		return true;
	}

	if (!Q_stricmp(variable, "playerlist"))
	{
		GetPlayerList(value);
		return true;
	}

	if (!Q_stricmp(variable, "maplist"))
	{
		GetMapList(value);
		return true;
	}

	if (!Q_stricmp(variable, "uptime"))
	{
		value.PutInt(int(Sys_FloatTime() - Host_GetStartTime()));
		value.PutChar(0);
		return true;
	}

	if (!Q_stricmp(variable, "ipaddress"))
	{
		value.PutString(NET_AdrToString(net_local_adr));
		value.PutChar(0);
		return true;
	}

	if (!Q_stricmp(variable, "mapcycle"))
	{
		int len;
		void* mapcyclelist = COM_LoadFileForMe(mapcyclefile.string, &len);
		if (mapcyclelist && len)
		{
			value.PutString((char*)mapcyclelist);
			value.PutChar(0);
			COM_FreeFile(mapcyclelist);
		}
		return true;
	}

	value.PutChar(0);
	return false;
}

void CServerRemoteAccess::RequestValue(int requestID, const char *variable)
{
	CUtlBuffer value(0, 0x100, true);
	LookupValue(variable, value);

	int i = m_ResponsePackets.AddToTail();
	DataResponse_t& resp = m_ResponsePackets.Element(i);

	resp.packet.PutInt(requestID);
	resp.packet.PutInt(0);
	resp.packet.PutString(variable);
	resp.packet.PutInt(value.TellPut());

	if (value.TellPut())
		resp.packet.Put(value.Base(), value.TellPut());

}

void CServerRemoteAccess::SetValue(const char *variable, const char *value)
{
	FileHandle_t f;
	struct cvar_s *var;

	if (!Q_stricmp(variable, "map"))
	{
		Cbuf_AddText("changelevel ");
		Cbuf_AddText((char*)value);
		Cbuf_AddText("\n");
		Cbuf_Execute();
	}
	else if (!Q_stricmp(variable, "mapcycle"))
	{
		f = FS_Open(mapcyclefile.string, "wt");
		if (!f)
		{
			Con_Printf("Couldn't write to read-only file %s, using file _dev_mapcycle.txt instead.\n", mapcyclefile.string);
			Cvar_DirectSet(&mapcyclefile, "_temp_mapcycle.txt");
			f = FS_Open(mapcyclefile.string, "wt");
		}

		if (f)
		{
			FS_Write(value, Q_strlen(value) + 1, 1, f);
			FS_Close(f);
		}
	}
	else
	{
		var = Cvar_FindVar(variable);
		if (var)
			Cvar_DirectSet(var, value);
	}
}

void CServerRemoteAccess::ExecCommand(const char *cmdString)
{
	Cbuf_AddText((char*)cmdString);
	Cbuf_AddText("\n");
	Cbuf_Execute();
}

void NotifyDedicatedServerUI(const char *message)
{
	g_ServerRemoteAccess.SendMessageToAdminUI(message);
}


#ifndef HOOK_ENGINE

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CServerRemoteAccess, IGameServerData, GAMESERVERDATA_INTERFACE_VERSION, g_ServerRemoteAccess)

#endif //HOOK_ENGINE