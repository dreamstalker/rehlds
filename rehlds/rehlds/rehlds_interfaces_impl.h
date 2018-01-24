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

#ifdef REHLDS_FIXES
	uint8_t m_messageBuffer[NET_MAX_PAYLOAD];
#endif
public:
	CNetChan(netchan_t* chan);

	virtual const netadr_t* GetRemoteAdr();
	virtual sizebuf_t* GetMessageBuf();

	virtual netchan_t* GetChan();

public:
#ifdef REHLDS_FIXES
	uint8_t* GetExtendedMessageBuffer() { return m_messageBuffer; };
#endif
};


class CGameClient : public IGameClient
{
private:
	int m_Id;
	client_t* m_pClient;
	CNetChan m_NetChan;

	// added this field to handle a bug with hanging clients in scoreboard after a map change.
	// we would also use the field client_t:connected, but actually can't because there is a bug in CS client when server sends TeamScore
	// and client is not yet initialized ScoreBoard which leads to memory corruption in the client.
	bool m_bSpawnedOnce;

#ifdef REHLDS_FIXES
	double m_localGameTimeBase;
#endif
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

	virtual uint32 GetVoiceStream(int stream_id);
	virtual void SetLastVoiceTime(double time);
	virtual double GetLastVoiceTime();
	virtual bool GetLoopback();
	virtual struct usercmd_s *GetLastCmd();

	virtual client_t* GetClient();

public:
	bool GetSpawnedOnce() const { return m_bSpawnedOnce; }
	void SetSpawnedOnce(bool spawned) { m_bSpawnedOnce = spawned; }
#ifdef REHLDS_FIXES
	uint8_t* GetExtendedMessageBuffer() { return m_NetChan.GetExtendedMessageBuffer(); };
#endif

#ifdef REHLDS_FIXES
	void SetupLocalGameTime() { m_localGameTimeBase = g_psv.time; }
	double GetLocalGameTime() const { return g_psv.time - m_localGameTimeBase; }
	double GetLocalGameTimeBase() const { return m_localGameTimeBase; }
#endif
};

class CRehldsServerStatic : public IRehldsServerStatic {
public:
	virtual int GetMaxClients();
	virtual bool IsLogActive();
	virtual IGameClient* GetClient(int id);
	virtual client_t* GetClient_t(int id);
	virtual int GetIndexOfClient_t(client_t* client);
	virtual int GetMaxClientsLimit();
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

	virtual double GetTime();
	virtual void SetResourcesNum(int num);
	virtual struct resource_s *GetResource(int index);
	virtual void SetName(const char* name);
	virtual ISteamGameServer *GetSteamGameServer();
	virtual netadr_t *GetNetFrom();
};

extern CGameClient** g_GameClients;

extern void Rehlds_Interfaces_InitClients();
extern void Rehlds_Interfaces_FreeClients();

extern IGameClient* GetRehldsApiClient(client_t* cl);
