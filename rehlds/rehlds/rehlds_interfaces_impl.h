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
#include "rehlds_interfaces.h"
#include "server.h"

class CNetChan : public INetChan
{
private:
	netchan_t* m_pNetChan;

public:
	CNetChan(netchan_t* chan);

	virtual const netadr_t* GetRemoteAdr();
	virtual sizebuf_t* GetMessageBuf();

	virtual netchan_t* GetChan();
};


class CGameClient : public IGameClient
{
private:
	int m_Id;
	client_t* m_pClient;
	CNetChan m_NetChan;

public:
	CGameClient(int id, client_t* cl);

	virtual int GetId();

	virtual bool IsActive();
	virtual void SetActive(bool active);

	virtual bool IsSpawned();
	virtual void SetSpawned(bool spawned);

	virtual INetChan* GetNetChan();

	virtual sizebuf_t* GetDatagram();

	virtual edict_t* GetEdict();

	virtual USERID_t* GetNetworkUserID();

	virtual const char* GetName();

	virtual bool IsConnected();
	virtual void SetConnected(bool connected);


	virtual client_t* GetClient();
};

class CRehldsServerStatic : public IRehldsServerStatic {
public:
	virtual int GetMaxClients();
	virtual bool IsLogActive();
	virtual IGameClient* GetClient(int id);
	virtual client_t* GetClient_t(int id);
	virtual int GetIndexOfClient_t(client_t* client);
};

class CRehldsServerData : public IRehldsServerData {
public:
	virtual const char* GetModelName();
	virtual const char* GetName();
	virtual uint32 GetWorldmapCrc();
	virtual uint8* GetClientDllMd5();
	virtual sizebuf_t* GetDatagram();
	virtual sizebuf_t* GetReliableDatagram();

	virtual void SetModelName(const char* modelname);
	virtual void SetConsistencyNum(int num);
	virtual int GetConsistencyNum();
	virtual int GetResourcesNum();
	virtual int GetDecalNameNum();
};

extern CGameClient** g_GameClients;

extern void Rehlds_Interfaces_InitClients();
extern void Rehlds_Interfaces_FreeClients();

extern IGameClient* GetRehldsApiClient(client_t* cl);