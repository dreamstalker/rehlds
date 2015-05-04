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

CGameClient** g_GameClients;

CGameClient::CGameClient(int id, client_t* cl)
	: m_NetChan(&cl->netchan)
{
	m_Id = id;
	m_pClient = cl;
}

int CGameClient::GetId()
{
	return m_Id;
}

bool CGameClient::IsActive()
{
	return m_pClient->active != 0;
}

void CGameClient::SetActive(bool active)
{
	m_pClient->active = active ? 1 : 0;
}

bool CGameClient::IsSpawned()
{
	return m_pClient->spawned != 0;
}

void CGameClient::SetSpawned(bool spawned)
{
	m_pClient->spawned = spawned ? 1 : 0;
}

bool CGameClient::IsConnected() {
	return m_pClient->connected != 0;;
}

void CGameClient::SetConnected(bool connected) {
	m_pClient->connected = connected ? 1 : 0;
}

INetChan* CGameClient::GetNetChan()
{
	return &m_NetChan;
}

sizebuf_t* CGameClient::GetDatagram()
{
	return &m_pClient->datagram;
}

edict_t* CGameClient::GetEdict()
{
	return m_pClient->edict;
}

USERID_t* CGameClient::GetNetworkUserID()
{
	return &m_pClient->network_userid;
}

const char* CGameClient::GetName()
{
	return m_pClient->name;
}

client_t* CGameClient::GetClient()
{
	return m_pClient;
}


CNetChan::CNetChan(netchan_t* chan)
{
	m_pNetChan = chan;
}

const netadr_t* CNetChan::GetRemoteAdr()
{
	return &m_pNetChan->remote_address;
}

netchan_t* CNetChan::GetChan()
{
	return m_pNetChan;
}



int CRehldsServerStatic::GetMaxClients()
{
	return g_psvs.maxclients;
}

bool CRehldsServerStatic::IsLogActive()
{
	return g_psvs.log.active ? true : false;
}

IGameClient* CRehldsServerStatic::GetClient(int id)
{
	if (id < 0 || id >= g_psvs.maxclients)
		Sys_Error(__FUNCTION__": invalid id provided: %d", id);

	return g_GameClients[id];
}



const char* CRehldsServerData::GetModelName() {
	return g_psv.modelname;
}

const char* CRehldsServerData::GetName() {
	return g_psv.name;
}

uint32_t CRehldsServerData::GetWorldmapCrc() {
	return g_psv.worldmapCRC;
}

uint8_t* CRehldsServerData::GetClientDllMd5() {
	return g_psv.clientdllmd5;
}

void Rehlds_Interfaces_FreeClients() 
{
	if (g_GameClients == NULL)
		return;

	for (int i = 0; i < g_psvs.maxclientslimit; i++)
	{
		delete g_GameClients[i];
	}

	free(g_GameClients);
	g_GameClients = NULL;
}

void Rehlds_Interfaces_InitClients() 
{
	Rehlds_Interfaces_FreeClients();

	g_GameClients = (CGameClient**)malloc(sizeof(CGameClient*) * g_psvs.maxclientslimit);
	for (int i = 0; i < g_psvs.maxclientslimit; i++)
	{
		g_GameClients[i] = new CGameClient(i, &g_psvs.clients[i]);
	}
}

IGameClient* GetRehldsApiClient(client_t* cl)
{
	int idx = cl - g_psvs.clients;
	if (idx < 0 || idx >= g_psvs.maxclients)
	{
		rehlds_syserror(__FUNCTION__": Invalid client index %d", idx);
	}

	return g_GameClients[idx];
}
