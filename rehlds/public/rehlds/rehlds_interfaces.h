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

class INetChan;
class IGameClient;

#include "archtypes.h"
#include "const.h"
#include "netadr.h"

#include "common_rehlds.h"
#include "userid_rehlds.h"

#ifdef REHLDS_SELF
#include "server.h"
#endif

class INetChan;
class IGameClient;

class IGameClient {
public:
	virtual int GetId() = 0;

	virtual bool IsActive() = 0;
	virtual void SetActive(bool active) = 0;

	virtual bool IsSpawned() = 0;
	virtual void SetSpawned(bool spawned) = 0;

	virtual INetChan* GetNetChan() = 0;

	virtual sizebuf_t* GetDatagram() = 0;

	virtual edict_t* GetEdict() = 0;

	virtual USERID_t* GetNetworkUserID() = 0;

	virtual const char* GetName() = 0;

	virtual bool IsConnected() = 0;
	virtual void SetConnected(bool connected) = 0;

	virtual uint32 GetVoiceStream(int stream_id) = 0;
	virtual void SetLastVoiceTime(double time) = 0;
	virtual double GetLastVoiceTime() = 0;
	virtual bool GetLoopback() = 0;
	virtual struct usercmd_s *GetLastCmd() = 0;

	// this must be the last virtual function in class
#ifdef REHLDS_SELF
	virtual client_t* GetClient() = 0;
#endif
};

class INetChan {
public:
	virtual const netadr_t* GetRemoteAdr() = 0;
	virtual sizebuf_t* GetMessageBuf() = 0;


	// this must be the last virtual function in class
#ifdef REHLDS_SELF
	virtual netchan_t* GetChan() = 0;
#endif
};

#ifndef REHLDS_SELF
struct client_t;
#endif

class IRehldsServerStatic {
public:
	virtual ~IRehldsServerStatic() { }

	virtual int GetMaxClients() = 0;
	virtual bool IsLogActive() = 0;
	virtual IGameClient* GetClient(int id) = 0;
	virtual client_t* GetClient_t(int id) = 0;
	virtual int GetIndexOfClient_t(client_t* client) = 0;
};

class IRehldsServerData {
public:
	virtual ~IRehldsServerData() { }

	virtual const char* GetModelName() = 0;
	virtual const char* GetName() = 0;
	virtual uint32 GetWorldmapCrc() = 0;
	virtual uint8* GetClientDllMd5() = 0;
	virtual sizebuf_t* GetDatagram() = 0;
	virtual sizebuf_t* GetReliableDatagram() = 0;

	virtual void SetModelName(const char* modelname) = 0;
	virtual void SetConsistencyNum(int num) = 0;
	virtual int GetConsistencyNum() = 0;
	virtual int GetResourcesNum() = 0;
	virtual int GetDecalNameNum() = 0;

	virtual double GetTime() = 0;
	virtual void SetResourcesNum(int num) = 0;
	virtual struct resource_s *GetResource(int index) = 0;
	virtual void SetName(const char* name) = 0;
	virtual class ISteamGameServer *GetSteamGameServer() = 0;
	virtual struct netadr_s *GetNetFrom() = 0;
};
