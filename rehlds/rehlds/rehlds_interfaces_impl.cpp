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
#ifdef REHLDS_FIXES
	, m_localGameTimeBase(0)
#endif
{
	m_Id = id;
	m_pClient = cl;
	m_bSpawnedOnce = false;
}

int EXT_FUNC CGameClient::GetId()
{
	return m_Id;
}

struct usercmd_s* EXT_FUNC CGameClient::GetLastCmd()
{
	return &m_pClient->lastcmd;
}

bool EXT_FUNC CGameClient::IsActive()
{
	return m_pClient->active != 0;
}

void EXT_FUNC CGameClient::SetActive(bool active)
{
	m_pClient->active = active ? 1 : 0;
}

bool EXT_FUNC CGameClient::IsSpawned()
{
	return m_pClient->spawned != 0;
}

void EXT_FUNC CGameClient::SetSpawned(bool spawned)
{
	m_pClient->spawned = spawned ? 1 : 0;
}

bool EXT_FUNC CGameClient::IsConnected() {
	return m_pClient->connected != 0;
}

void EXT_FUNC CGameClient::SetConnected(bool connected) {
	m_pClient->connected = connected ? 1 : 0;
}

uint32 EXT_FUNC CGameClient::GetVoiceStream(int stream_id) {
	if (stream_id >= 0 && stream_id < ARRAYSIZE(m_pClient->m_VoiceStreams))
		return m_pClient->m_VoiceStreams[stream_id];
	return 0;
}

void EXT_FUNC CGameClient::SetLastVoiceTime(double time) {
	m_pClient->m_lastvoicetime = time;
}

double EXT_FUNC CGameClient::GetLastVoiceTime() {
	return m_pClient->m_lastvoicetime;
}

bool EXT_FUNC CGameClient::GetLoopback() {
	return m_pClient->m_bLoopback != 0;
}

INetChan* EXT_FUNC CGameClient::GetNetChan()
{
	return &m_NetChan;
}

sizebuf_t* EXT_FUNC CGameClient::GetDatagram()
{
	return &m_pClient->datagram;
}

edict_t* EXT_FUNC CGameClient::GetEdict()
{
	return m_pClient->edict;
}

USERID_t* EXT_FUNC CGameClient::GetNetworkUserID()
{
	return &m_pClient->network_userid;
}

const char* EXT_FUNC CGameClient::GetName()
{
	return m_pClient->name;
}

client_t* EXT_FUNC CGameClient::GetClient()
{
	return m_pClient;
}


CNetChan::CNetChan(netchan_t* chan)
{
	m_pNetChan = chan;
}

const netadr_t* EXT_FUNC CNetChan::GetRemoteAdr()
{
	return &m_pNetChan->remote_address;
}

sizebuf_t* EXT_FUNC CNetChan::GetMessageBuf()
{
	return &m_pNetChan->message;
}

netchan_t* EXT_FUNC CNetChan::GetChan()
{
	return m_pNetChan;
}



int EXT_FUNC CRehldsServerStatic::GetMaxClients()
{
	return g_psvs.maxclients;
}

int EXT_FUNC CRehldsServerStatic::GetMaxClientsLimit()
{
	return g_psvs.maxclientslimit;
}

bool EXT_FUNC CRehldsServerStatic::IsLogActive()
{
	return g_psvs.log.active ? true : false;
}

IGameClient* EXT_FUNC CRehldsServerStatic::GetClient(int id)
{
	if (id < 0 || id >= g_psvs.maxclientslimit)
		Sys_Error("%s: invalid id provided: %d", __func__, id);

	return g_GameClients[id];
}

client_t* EXT_FUNC CRehldsServerStatic::GetClient_t(int id)
{
	if (id < 0 || id >= g_psvs.maxclientslimit)
		Sys_Error("%s: invalid id provided: %d", __func__, id);

	return &g_psvs.clients[id];
}

int EXT_FUNC CRehldsServerStatic::GetIndexOfClient_t(client_t* client)
{
	if (client < g_psvs.clients || client >= &g_psvs.clients[g_psvs.maxclientslimit])
		return -1;

	if (((size_t)client - (size_t)g_psvs.clients) % sizeof(client_t))
		return -1;

	return int(client - g_psvs.clients);
}



const char* EXT_FUNC CRehldsServerData::GetModelName() {
	return g_psv.modelname;
}

void EXT_FUNC CRehldsServerData::SetModelName(const char* modelname) {
	Q_strncpy(g_psv.modelname, modelname, ARRAYSIZE(g_psv.modelname) - 1);
	g_psv.modelname[ARRAYSIZE(g_psv.modelname) - 1] = '\0';
}

const char* EXT_FUNC CRehldsServerData::GetName() {
	return g_psv.name;
}

void EXT_FUNC CRehldsServerData::SetName(const char* name) {
	Q_strncpy(g_psv.name, name, ARRAYSIZE(g_psv.name) - 1);
	g_psv.name[ARRAYSIZE(g_psv.name) - 1] = '\0';
}

uint32 EXT_FUNC CRehldsServerData::GetWorldmapCrc() {
	return g_psv.worldmapCRC;
}

uint8* EXT_FUNC CRehldsServerData::GetClientDllMd5() {
	return g_psv.clientdllmd5;
}

sizebuf_t* EXT_FUNC CRehldsServerData::GetDatagram() {
	return &g_psv.datagram;
}

sizebuf_t* EXT_FUNC CRehldsServerData::GetReliableDatagram() {
	return &g_psv.reliable_datagram;
}

void EXT_FUNC CRehldsServerData::SetConsistencyNum(int num) {
	g_psv.num_consistency = num;
}

int EXT_FUNC CRehldsServerData::GetConsistencyNum() {
	return g_psv.num_consistency;
}

int EXT_FUNC CRehldsServerData::GetResourcesNum() {
	return g_psv.num_resources;
}

int EXT_FUNC CRehldsServerData::GetDecalNameNum() {
	return sv_decalnamecount;
}

double EXT_FUNC CRehldsServerData::GetTime() {
	return g_psv.time;
}

void EXT_FUNC CRehldsServerData::SetResourcesNum(int num) {
	g_psv.num_resources = num;
}

struct resource_s *EXT_FUNC CRehldsServerData::GetResource(int index) {
#ifdef REHLDS_FIXES
	return &g_rehlds_sv.resources[index];
#else // REHLDS_FIXES
	return &g_psv.resourcelist[index];
#endif // REHLDS_FIXES
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
	if (cl == NULL)
		return NULL; //I think it's logical.

	int idx = cl - g_psvs.clients;
	if (idx < 0 || idx >= g_psvs.maxclientslimit)
	{
		Sys_Error("%s: Invalid client index %d", __func__, idx);
	}

	return g_GameClients[idx];
}

ISteamGameServer* EXT_FUNC CRehldsServerData::GetSteamGameServer() {
	return ::SteamGameServer();
}

netadr_t* EXT_FUNC CRehldsServerData::GetNetFrom() {
	return &net_from;
}
