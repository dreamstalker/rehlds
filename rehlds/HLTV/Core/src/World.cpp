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

char g_DownloadURL[128];

bool World::Init(IBaseSystem *system, int serial, char *name)
{
	BaseSystemModule::Init(system, serial, name);
	SetState(WORLD_INITIALIZING);

	if (!name) {
		SetName(WORLD_INTERFACE_VERSION);
	}

	m_ClientUserMsgs = nullptr;
	m_ResourcesList = nullptr;
	m_ResourcesNum = 0;
	m_Protocol = PROTOCOL_VERSION;

	m_SignonData.Resize(MAX_BUFFER_SIGNONDATA);
	m_ServerInfo.SetMaxSize(MAX_SERVERINFO_STRING);

	memset(m_HostName, 0, sizeof(m_HostName));
	strcopy(m_ServerName, "Unnamed HLTV");

	m_Frames.Init();
	m_FramesByTime.Init();

	m_WorldModel.Init(system);
	m_Delta.Init(system);

	m_MaxBufferLength = -1.0f;
	m_MaxCacheIndex = MAX_FRAME_CACHE;

	char *maxCacheparam = m_System->CheckParam("-cachesize");
	if (maxCacheparam) {
		m_MaxCacheIndex = atoi(maxCacheparam);
	}

	if (m_MaxCacheIndex <= 0) {
		m_MaxCacheIndex = MAX_FRAME_CACHE;
	}

	m_FrameCache = (frameCache_t *)Mem_ZeroMalloc(sizeof(frameCache_t) * m_MaxCacheIndex);
	m_DeltaCache = (deltaCache_t *)Mem_ZeroMalloc(sizeof(deltaCache_t) * m_MaxCacheIndex);

	if (!m_FrameCache || !m_DeltaCache) {
		m_System->Errorf("World::Init: Not enough memory for caches. Reduce -cachesize.\n");
		return false;
	}

	Reset();
	SetState(WORLD_DISCONNECTED);
	m_State = MODULE_RUNNING;
	m_System->Printf("World module initialized.\n");

	return true;
}

void World::ShutDown()
{
	if (m_State == MODULE_DISCONNECTED) {
		return;
	}

	Reset();
	SetState(WORLD_DISCONNECTED);
	BaseSystemModule::FireSignal(8);

	m_Delta.Shutdown();
	m_Listener.Clear();

	if (m_DeltaCache)
	{
		Mem_Free(m_DeltaCache);
		m_DeltaCache = nullptr;
	}

	if (m_FrameCache)
	{
		Mem_Free(m_FrameCache);
		m_FrameCache = nullptr;
	}

	BaseSystemModule::ShutDown();
	m_System->Printf("World module shutdown.\n");
}

void World::Reset()
{
	ClearUserMessages();
	ClearResources();
	ClearBaseline();
	ClearInstancedBaseline();
	ClearLightStyles();
	ClearPlayers();
	ClearEntityCache();
	ClearServerInfo();
	ClearFrames();

	m_SignonData.Clear();
	m_WorldModel.Clear();

	m_SequenceNr = 0;
	m_ViewEntity = 0;
	m_WorldTime = 0;
	m_StartTime = 0;
	m_VoiceEnabled = false;

	memset(m_ClientFallback, 0, sizeof(m_ClientFallback));

	m_AllowCheats = false;
	m_IsHLTV = false;
	m_IsPaused = false;

	m_GameServerAddress.Clear();
	g_DownloadURL[0] = '\0';
}

bool World::SetMaxClients(int max)
{
	if (max <= 0 || max > MAX_CLIENTS) {
		m_System->Printf("WARNING! World::SetMaxClients: Bad maxclients (%u)\n", max);
		return false;
	}

	m_Maxclients = max;
	return true;
}

void World::ClearUserMessages()
{
	UserMsg *pList, *pNext;
	for (pList = m_ClientUserMsgs; pList; pList = pNext)
	{
		pNext = pList->next;
		Mem_Free(pList);
	}

	m_ClientUserMsgs = nullptr;
}

bool World::AddUserMessage(int msgNumber, int size, char *name)
{
	UserMsg umsg;
	memset(&umsg, 0, sizeof(umsg));

	umsg.iMsg = msgNumber;
	umsg.iSize = size;

	if (size == 255) {
		umsg.iSize = -1;
	}

	strcopy(umsg.szName, name);

	bool bFound = false;
	for (UserMsg *pList = m_ClientUserMsgs; pList; pList = pList->next)
	{
		if (!_stricmp(pList->szName, umsg.szName)) {
			bFound = true;
			pList->iMsg = umsg.iMsg;
			pList->iSize = umsg.iSize;
		}
	}

	if (!bFound)
	{
		UserMsg *pumsg = (UserMsg *)Mem_Malloc(sizeof(UserMsg));
		memcpy(pumsg, &umsg, sizeof(*pumsg));
		pumsg->next = m_ClientUserMsgs;
		m_ClientUserMsgs = pumsg;
		return true;
	}

	return false;
}

void World::UpdatePlayer(int playerNum, int userId, char *infostring, char *hashedcdkey)
{
	InfoString infostr(infostring);
	if (playerNum >= MAX_CLIENTS) {
		m_System->Errorf("World::UpdatePlayer: player number %i >= MAX_CLIENTS\n", playerNum);
		return;
	}

	player_info_t *player = &m_Players[playerNum];
	if (!infostring[0]) {
		return;
	}

	player->active = true;
	player->userid = userId;
	memcpy(player->hashedcdkey, hashedcdkey, sizeof(player->hashedcdkey));

	m_System->DPrintf("Player update(%i:%s)\n", playerNum, player->name);
	infostr.RemovePrefixedKeys('_');

	strcopy(player->userinfo, infostr.GetString());
	strcopy(player->name, infostr.ValueForKey("name"));
	strcopy(player->model, infostr.ValueForKey("model"));

	player->topcolor    = atoi(infostr.ValueForKey("topcolor"));
	player->bottomcolor = atoi(infostr.ValueForKey("bottomcolor"));
	player->spectator   = atoi(infostr.ValueForKey("*hltv"));
	player->trackerID   = atoi(infostr.ValueForKey("*fid"));

	BaseSystemModule::FireSignal(4, &playerNum);
}

bool World::AddResource(resource_t *resource)
{
	resource_t *newresource = (resource_t *)Mem_ZeroMalloc(sizeof(resource_t));
	if (!newresource) {
		return false;
	}

	memcpy(newresource, resource, sizeof(*newresource));

	newresource->data = nullptr;
	newresource->pNext = m_ResourcesList;

	m_ResourcesList = newresource;
	m_ResourcesNum++;

	return true;
}

void World::ClearResources()
{
	resource_t *res, *next;
	for (res = m_ResourcesList; res; res = next) {
		next = res->pNext;
		Mem_Free(res);
	}

	m_ResourcesList = nullptr;
	m_ResourcesNum = 0;
}

UserMsg *World::GetUserMsg(int msgNumber)
{
	for (UserMsg *pList = m_ClientUserMsgs; pList; pList = pList->next)
	{
		if (pList->iMsg == msgNumber) {
			return pList;
		}
	}

	return nullptr;
}

void World::AddBaselineEntity(int index, entity_state_t *ent)
{
	if (index < 0 || index >= MAX_ENTITIES) {
		m_System->Printf("WARNING! World::SetBaselineEntity: index (%i) out of bounds.\n", index);
		return;
	}

	memcpy(&m_BaseLines[index], ent, sizeof(m_BaseLines[index]));
}

bool World::IsPlayerIndex(int index)
{
	if (index <= 0 || index > m_Maxclients) {
		return false;
	}

	return true;
}

void World::ClearBaseline()
{
	memset(m_BaseLines, 0, sizeof(m_BaseLines));

	for (auto& base : m_BaseLines) {
		base.entityType = ENTITY_UNINITIALIZED;
	}
}

void World::ClearInstancedBaseline()
{
	memset(m_Instanced_BaseLines, 0, sizeof(m_Instanced_BaseLines));
}

void World::AddInstancedBaselineEntity(int index, entity_state_t *ent)
{
	if (index < 0 || index >= MAX_INSTANCED_BASELINES) {
		m_System->Printf("WARNING! World::SetInstancedBaselineEntity: index (%i) out of bounds.\n", index);
		return;
	}

	memcpy(&m_Instanced_BaseLines[index], ent, sizeof(m_Instanced_BaseLines[index]));
}

void World::SetTime(double newTime)
{
	m_System->DPrintf("Synchronizing time %.2f ( %.2f)\n", newTime, newTime - m_WorldTime);
	m_WorldTime = newTime;
}

void World::SetHostName(char *name)
{
	if (!name || !name[0]) {
		memset(m_HostName, 0, sizeof(m_HostName));
		return;
	}

	strcopy(m_HostName, name);
}

void World::ClearLightStyles()
{
	memset(m_Lightstyles, 0, sizeof(m_Lightstyles));
}

void World::AddLightStyle(int index, char *style)
{
	if (index < 0 || index >= MAX_LIGHTSTYLES) {
		m_System->Printf("WARNING! World::SetLightStyle: index (%i) out of bounds.\n", index);
		return;
	}

	int length = strlen(style);
	if (length >= sizeof(m_Lightstyles[0])) {
		m_System->Printf("WARNING! World::SetLightStyle: style too long (%i).\n", length);
	}

	strcopy(m_Lightstyles[index], style);
}

void World::NewGame(int newServerCount)
{
	BaseSystemModule::FireSignal(1);

	Reset();
	m_ServerCount = newServerCount;
	SetState(WORLD_CONNECTING);
}

void World::SetState(WorldState newState)
{
	if (newState == m_WorldState)
		return;

	bool stateError = false;
	switch (newState)
	{
	case WORLD_INITIALIZING:
	case WORLD_DISCONNECTED:
		break;
	case WORLD_CONNECTING:
	{
		if (m_WorldState != WORLD_DISCONNECTED
			&& m_WorldState != WORLD_RUNNING
			&& m_WorldState != WORLD_COMPLETE) {
			stateError = true;
		}
		break;
	}
	case WORLD_RUNNING:
	{
		if (m_WorldState != WORLD_CONNECTING) {
			stateError = true;
		}
		break;
	}
	case WORLD_COMPLETE:
	{
		if (m_WorldState != WORLD_RUNNING) {
			stateError = true;
		}
		break;
	}
	default:
		stateError = true;
		break;
	}

	if (stateError)
	{
		m_System->Errorf("World::SetState: not valid m_WorldState (%i -> %i).\n", m_WorldState, newState);
		return;
	}

	m_WorldState = newState;
}

void World::ConnectionComplete()
{
	WriteBaseline(&m_SignonData);
	m_StartTime = m_SystemTime;

	SetState(WORLD_RUNNING);
	BaseSystemModule::FireSignal(2);
}

int World::GetNumPlayers()
{
	int count = 0;
	for (int i = 0; i < m_Maxclients; i++)
	{
		if (m_Players[i].active) {
			count++;
		}
	}

	return count;
}

bool World::IsActive()
{
	return (m_WorldState == WORLD_RUNNING
		|| m_WorldState == WORLD_COMPLETE);
}

void World::ClearPlayers()
{
	memset(m_Players, 0, sizeof(m_Players));
}

void World::SetServerInfoString(char *infostring)
{
	m_ServerInfo.SetString(infostring);
}

void World::WriteBaseline(BitBuffer *stream)
{
	if (!IsDeltaEncoder()) {
		m_System->Errorf("World::WriteBaseline: delta encoder not found.\n");
		return;
	}

	stream->WriteByte(svc_spawnbaseline);
	stream->StartBitMode();

	bool custom = false;
	int entnum = 0;
	entity_state_t nullstate;
	memset(&nullstate, 0, sizeof(entity_state_t));

	for (auto& base : m_BaseLines)
	{
		// ignore uninitialized entity, not passed parse process World::ParseBaseline
		if (base.entityType != ENTITY_UNINITIALIZED)
		{
			stream->WriteBits(entnum, 11);
			stream->WriteBits(m_BaseLines[entnum].entityType, 2);

			custom = ~m_BaseLines[entnum].entityType & ENTITY_NORMAL;

			delta_t *delta = GetDeltaEncoder(entnum, custom);
			m_Delta.WriteDelta(stream, (byte *)&nullstate, (byte *)&base, true, delta);
		}

		entnum++;
	}

	stream->WriteBits(0xFFFF, 16);
	stream->WriteBits(m_MaxInstanced_BaseLine, 6);

	for (entnum = 0; entnum < m_MaxInstanced_BaseLine; entnum++) {
		m_Delta.WriteDelta(stream, (byte *)&nullstate, (byte *)&m_Instanced_BaseLines[entnum], true, GetEntityDelta());
	}

	stream->EndBitMode();
}

void World::WriteLightStyles(BitBuffer *stream)
{
	for (int i = 0; i < MAX_LIGHTSTYLES; i++)
	{
		stream->WriteByte(svc_lightstyle);
		stream->WriteByte(i);
		stream->WriteString(m_Lightstyles[i]);
	}
}

char *World::GetHostName()
{
	if (m_HostName[0]) {
		return m_HostName;
	}

	return m_ServerName;
}

void World::WriteServerinfo(BitBuffer *stream)
{
	char message[2048];
	_snprintf(message, sizeof(message), "Protocol Version %i, Spawn count %i %s\n", m_Protocol, m_ServerCount, m_IsHLTV ? "(HLTV)" : "");

	stream->WriteByte(svc_print);
	stream->WriteString(message);

	if (m_IsHLTV)
	{
		stream->WriteByte(svc_hltv);
		stream->WriteByte(HLTV_ACTIVE);
	}

	stream->WriteByte(svc_serverinfo);
	stream->WriteLong(m_Protocol);
	stream->WriteLong(m_ServerCount);

	CRC32_t mungebuffer = m_ServerCRC;
	COM_Munge3((byte *)&mungebuffer, sizeof(mungebuffer), (-1 - m_PlayerNum) & 0xFF);

	stream->WriteLong(mungebuffer);
	stream->WriteBuf(m_ClientdllMD5, sizeof(m_ClientdllMD5));
	stream->WriteByte(m_Maxclients);
	stream->WriteByte(m_PlayerNum);
	stream->WriteByte(m_GameType);			// deathmatch, coop etc
	stream->WriteString(m_GameDir);
	stream->WriteString(GetHostName());
	stream->WriteString(m_LevelName);
	stream->WriteString("mapcycle failure");
	stream->WriteByte(0);

	stream->WriteByte(svc_sendextrainfo);
	stream->WriteString(m_ClientFallback);
	stream->WriteByte(m_AllowCheats);

	WriteDeltaDescriptions(stream);
	WriteMovevars(stream);

	stream->WriteByte(svc_cdtrack);
	stream->WriteByte(m_CDTrack);
	stream->WriteByte(m_CDTrack);

	stream->WriteByte(svc_setview);
	stream->WriteShort(m_ViewEntity);
}

void World::WriteDeltaDescriptions(BitBuffer *stream)
{
	int i, c;

	delta_description_t nulldesc;
	memset(&nulldesc, 0, sizeof(nulldesc));

	for (auto p = m_Delta.GetRegistry(); p; p = p->next)
	{
		stream->WriteByte(svc_deltadescription);
		stream->WriteString(p->name);

		if (!p->pdesc)
		{
			m_System->Errorf("World::WriteDeltaDescriptions : missing delta for %s\n", p->name);
			return;
		}

		c = p->pdesc->fieldCount;
		stream->StartBitMode();
		stream->WriteBits(c, 16);

		for (i = 0; i < c; i++)
		{
			delta_description_t *from = &nulldesc;
			delta_description_t *to = &p->pdesc->pdd[i];
			m_Delta.WriteDelta(stream, (byte *)from, (byte *)to, true, (delta_t *)&Delta::m_MetaDelta);
		}

		stream->EndBitMode();
	}
}

void World::WriteMovevars(BitBuffer *stream)
{
	stream->WriteByte (svc_newmovevars);
	stream->WriteFloat(m_MoveVars.gravity);
	stream->WriteFloat(m_MoveVars.stopspeed);
	stream->WriteFloat(m_MoveVars.maxspeed);
	stream->WriteFloat(m_MoveVars.spectatormaxspeed);
	stream->WriteFloat(m_MoveVars.accelerate);
	stream->WriteFloat(m_MoveVars.airaccelerate);
	stream->WriteFloat(m_MoveVars.wateraccelerate);
	stream->WriteFloat(m_MoveVars.friction);
	stream->WriteFloat(m_MoveVars.edgefriction);
	stream->WriteFloat(m_MoveVars.waterfriction);
	stream->WriteFloat(m_MoveVars.entgravity);
	stream->WriteFloat(m_MoveVars.bounce);
	stream->WriteFloat(m_MoveVars.stepsize);
	stream->WriteFloat(m_MoveVars.maxvelocity);
	stream->WriteFloat(m_MoveVars.zmax);
	stream->WriteFloat(m_MoveVars.waveHeight);
	stream->WriteByte (m_MoveVars.footsteps != 0);
	stream->WriteFloat(m_MoveVars.rollangle);
	stream->WriteFloat(m_MoveVars.rollspeed);
	stream->WriteFloat(m_MoveVars.skycolor_r);
	stream->WriteFloat(m_MoveVars.skycolor_g);
	stream->WriteFloat(m_MoveVars.skycolor_b);
	stream->WriteFloat(m_MoveVars.skyvec_x);
	stream->WriteFloat(m_MoveVars.skyvec_y);
	stream->WriteFloat(m_MoveVars.skyvec_z);
	stream->WriteString(m_MoveVars.skyName);
}

void World::WriteRegisteredUserMessages(BitBuffer *stream)
{
	for (UserMsg *pMsg = m_ClientUserMsgs; pMsg; pMsg = pMsg->next)
	{
		stream->WriteByte(svc_newusermsg);
		stream->WriteByte(pMsg->iMsg);
		stream->WriteByte(pMsg->iSize);
		stream->WriteBuf(pMsg->szName, sizeof(pMsg->szName));
	}
}

void World::WriteResources(BitBuffer *stream)
{
	unsigned char nullbuffer[32];
	memset(nullbuffer, 0, sizeof(nullbuffer));

	if (g_DownloadURL[0])
	{
		stream->WriteByte(svc_resourcelocation);
		stream->WriteString(g_DownloadURL);
	}

	stream->WriteByte(svc_resourcelist);
	stream->StartBitMode();
	stream->WriteBits(m_ResourcesNum, RESOURCE_INDEX_BITS);

	for (resource_t *resource = m_ResourcesList; resource; resource = resource->pNext)
	{
		stream->WriteBits(resource->type, 4);
		stream->WriteBitString(resource->szFileName);
		stream->WriteBits(resource->nIndex, RESOURCE_INDEX_BITS);
		stream->WriteBits(resource->nDownloadSize, 24);
		stream->WriteBits(resource->ucFlags & (RES_WASMISSING | RES_FATALIFMISSING), 3);

		if (resource->ucFlags & RES_CUSTOM)
		{
			stream->WriteBitData(resource->rgucMD5_hash, sizeof(resource->rgucMD5_hash));
		}

		if (!memcmp(resource->rguc_reserved, nullbuffer, sizeof(resource->rguc_reserved)))
		{
			stream->WriteBit(0);
		}
		else
		{
			stream->WriteBit(1);
			stream->WriteBitData(resource->rguc_reserved, sizeof(resource->rguc_reserved));
		}
	}

	stream->WriteBit(0);
	stream->EndBitMode();
}

void World::WriteClientUpdate(BitBuffer *stream, int playerIndex)
{
	if (m_Players[playerIndex].active)
	{
		stream->WriteByte(svc_updateuserinfo);
		stream->WriteByte(playerIndex);
		stream->WriteLong(m_Players[playerIndex].userid);
		stream->WriteString(m_Players[playerIndex].userinfo);
		stream->WriteBuf(m_Players[playerIndex].hashedcdkey, sizeof(m_Players[playerIndex].hashedcdkey));
	}
}

int World::AddFrame(frame_t *newFrame)
{
	if (!newFrame)
	{
		m_System->Errorf("World::AddFrame: newFrame == NULL.\n");
		return 0;
	}

	m_SequenceNr++;

	frame_t *currentFrame;
	currentFrame = (frame_t *)Mem_ZeroMalloc(sizeof(frame_t));
	currentFrame->seqnr = m_SequenceNr;
	currentFrame->time = newFrame->time;

	if (newFrame->time < m_WorldTime)
	{
		m_System->DPrintf("Fixing frame time, delta %.3f\n", m_WorldTime - currentFrame->time);
		ReorderFrameTimes(currentFrame->time - 0.05f);
	}

	m_WorldTime = currentFrame->time;

	if (m_WorldState == WORLD_CONNECTING) {
		ConnectionComplete();
	}

	int compressedEntitiesSize = 0;
	if (newFrame->entitiesSize)
	{
		if (newFrame->delta)
		{
			m_System->Errorf("World::AddFrame: only uncompressed frames accepted.\n");
			return 0;
		}

		m_Delta.SetLargeTimeBufferSize(true);
		BitBuffer tempStream(m_EntityBuffer, sizeof(m_EntityBuffer));
		memset(m_EntityBuffer, 0, sizeof(m_EntityBuffer));

		compressedEntitiesSize = CompressFrame(newFrame, &tempStream);
		m_Delta.SetLargeTimeBufferSize(false);

		if (tempStream.IsOverflowed())
		{
			m_System->Printf("WARNING! World::AddFrame: couldn't compress frame entities.\n");
			return 0;
		}
	}

	unsigned char *pdata;
	int maxFrameSize = newFrame->demoDataSize
		+ newFrame->userMessagesSize
		+ newFrame->voiceDataSize
		+ newFrame->unreliableDataSize
		+ newFrame->reliableDataSize
		+ newFrame->clientDataSize
		+ newFrame->eventsSize
		+ compressedEntitiesSize
		+ 4;

	if (newFrame->demoInfo)
	{
		maxFrameSize += sizeof(demo_info_t);
	}

	pdata = (unsigned char *)Mem_ZeroMalloc(maxFrameSize);

	currentFrame->data = pdata;
	currentFrame->delta = -1;

	if (newFrame->entitiesSize)
	{
		memcpy(pdata, m_EntityBuffer, compressedEntitiesSize);
		currentFrame->entities = pdata;
		currentFrame->entitiesSize = compressedEntitiesSize;
		currentFrame->entitynum = newFrame->entitynum;
		pdata += compressedEntitiesSize;
	}

	if (newFrame->clientDataSize)
	{
		memcpy(pdata, newFrame->clientData, newFrame->clientDataSize);
		currentFrame->clientData = pdata;
		currentFrame->clientDataSize = newFrame->clientDataSize;
		pdata += currentFrame->clientDataSize;
	}

	if (newFrame->eventsSize)
	{
		memcpy(pdata, newFrame->events, newFrame->eventsSize);
		currentFrame->events = pdata;
		currentFrame->eventsSize = newFrame->eventsSize;
		currentFrame->eventnum = newFrame->eventnum;
		pdata += currentFrame->eventsSize;
	}

	if (newFrame->reliableDataSize)
	{
		memcpy(pdata, newFrame->reliableData, newFrame->reliableDataSize);
		currentFrame->reliableData = pdata;
		currentFrame->reliableDataSize = newFrame->reliableDataSize;
		pdata += currentFrame->reliableDataSize;
	}

	if (newFrame->unreliableDataSize)
	{
		memcpy(pdata, newFrame->unreliableData, newFrame->unreliableDataSize);
		currentFrame->unreliableData = pdata;
		currentFrame->unreliableDataSize = newFrame->unreliableDataSize;
		pdata += currentFrame->unreliableDataSize;
	}

	if (newFrame->voiceDataSize)
	{
		memcpy(pdata, newFrame->voiceData, newFrame->voiceDataSize);
		currentFrame->voiceData = pdata;
		currentFrame->voiceDataSize = newFrame->voiceDataSize;
		pdata += currentFrame->voiceDataSize;
	}

	if (newFrame->userMessagesSize)
	{
		memcpy(pdata, newFrame->userMessages, newFrame->userMessagesSize);
		currentFrame->userMessages = pdata;
		currentFrame->userMessagesSize = newFrame->userMessagesSize;
		pdata += currentFrame->userMessagesSize;
	}

	if (newFrame->demoDataSize)
	{
		memcpy(pdata, newFrame->demoData, newFrame->demoDataSize);
		currentFrame->demoData = pdata;
		currentFrame->demoDataSize = newFrame->demoDataSize;
		pdata += currentFrame->demoDataSize;
	}

	if (newFrame->demoInfo)
	{
		memcpy(pdata, newFrame->demoInfo, sizeof(demo_info_t));
		currentFrame->demoInfo = pdata;
	}

	m_Frames.Add(currentFrame, currentFrame->seqnr);
	m_FramesByTime.Add(currentFrame, m_WorldTime);

	CheckFrameBufferSize();
	BaseSystemModule::FireSignal(3, &m_SequenceNr);

	return m_SequenceNr;
}

frame_t *World::GetFrameBySeqNr(unsigned int seqnr)
{
	return (frame_t *)m_Frames.FindExactKey(seqnr);
}

frame_t *World::GetLastFrame()
{
	return (frame_t *)m_Frames.GetLast();
}

frame_t *World::GetFirstFrame()
{
	return (frame_t *)m_Frames.GetFirst();
}

frame_t *World::GetFrameByTime(double time)
{
	frame_t *lastFrame = (frame_t *)m_FramesByTime.FindClosestKey(time);
	if (!lastFrame) {
		return nullptr;
	}

	if (lastFrame->time < time)
	{
		frame_t *nextFrame = (frame_t *)m_FramesByTime.GetNext();
		if (nextFrame) {
			lastFrame = nextFrame;
		}
	}

	return lastFrame;
}

void World::WriteFrame(frame_t *frame, unsigned int lastFrameSeqnr, BitBuffer *reliableStream, BitBuffer *unreliableStream, unsigned int deltaSeqNr, unsigned int clientDelta, bool addVoice)
{
	frame_t fullFrame;
	if (!GetUncompressedFrame(frame->seqnr, &fullFrame)) {
		m_System->DPrintf("World::WriteFrame: couldn't uncompress frame.\n");
		return;
	}

	if (m_IsHLTV)
	{
		unreliableStream->WriteByte(svc_clientdata);
	}
	else
	{
		clientdata_t clientData;
		if (GetClientData(frame, &clientData))
		{
			clientdata_t nullClientData;
			memset(&nullClientData, 0, sizeof(nullClientData));

			unreliableStream->WriteByte(svc_clientdata);
			unreliableStream->StartBitMode();
			unreliableStream->WriteBit(0);

			m_Delta.WriteDelta(unreliableStream, (byte *)&nullClientData, (byte *)&clientData, true, GetClientDelta());

			unreliableStream->WriteBit(0);
			unreliableStream->EndBitMode();
		}
	}

	bool validEntities = false;
	if (deltaSeqNr && WriteDeltaEntities(unreliableStream, &fullFrame, deltaSeqNr, clientDelta)) {
		validEntities = true;
	}
	else
	{
		unreliableStream->WriteByte(svc_packetentities);
		unreliableStream->WriteShort(frame->entitynum);

		if (CompressFrame(&fullFrame, unreliableStream) > 0) {
			validEntities = true;
		}
		else {
			m_System->DPrintf("WARNING! World::WriteFrame: could write entities!\n");
		}
	}

	if (frame->eventsSize && validEntities)
	{
		unreliableStream->WriteByte(svc_event);
		unreliableStream->WriteBuf(frame->events, frame->eventsSize);
	}

	frame_t *lastFrame = (frame_t *)m_Frames.FindExactKey(lastFrameSeqnr + 1);
	while (lastFrame)
	{
		if (lastFrame->seqnr > frame->seqnr
			|| reliableStream->IsOverflowed()) {
			break;
		}

		if (lastFrame->reliableDataSize && (unsigned)reliableStream->SpaceLeft() > lastFrame->reliableDataSize) {
			reliableStream->WriteBuf(lastFrame->reliableData, lastFrame->reliableDataSize);
		}

		if (lastFrame->userMessagesSize && (unsigned)reliableStream->SpaceLeft() > lastFrame->userMessagesSize) {
			reliableStream->WriteBuf(lastFrame->userMessages, lastFrame->userMessagesSize);
		}

		if (lastFrame->seqnr + 8 > frame->seqnr)
		{
			if (lastFrame->unreliableDataSize && (unsigned)unreliableStream->SpaceLeft() > lastFrame->unreliableDataSize) {
				unreliableStream->WriteBuf(lastFrame->unreliableData, lastFrame->unreliableDataSize);
			}

			if (lastFrame->voiceDataSize)
			{
				if ((unsigned)unreliableStream->SpaceLeft() > lastFrame->voiceDataSize) {
					unreliableStream->WriteBuf(lastFrame->voiceData, lastFrame->voiceDataSize);
				}
			}
		}

		lastFrame = (frame_t *)m_Frames.GetNext();
	}
}

bool World::GetUncompressedFrame(unsigned int seqNr, frame_t *frame)
{
	frame_t *deltaFrame = (frame_t *)m_Frames.FindExactKey(seqNr);
	if (!deltaFrame) {
		return false;
	}

	if (deltaFrame->seqnr != seqNr) {
		m_System->DPrintf("WARNING! World::GetUncompressedFrame: frame not found %i.\n", seqNr);
		return false;
	}

	return GetUncompressedFrame(deltaFrame, frame);
}

bool World::GetClientData(unsigned int SeqNr, clientdata_t *clientData)
{
	frame_t *frame = (frame_t *)m_Frames.FindExactKey(SeqNr);
	return GetClientData(frame, clientData);
}

bool World::GetClientData(frame_t *frame, clientdata_t *clientData)
{
	if (!frame || !frame->clientData || !frame->clientDataSize) {
		return false;
	}

	clientdata_t BaseLineClientData;
	memset(&BaseLineClientData, 0, sizeof(BaseLineClientData));

	BitBuffer stream(frame->clientData, frame->clientDataSize);
	stream.StartBitMode();
	m_Delta.ParseDelta(&stream, (byte *)&BaseLineClientData, (byte *)clientData, GetClientDelta());

	return true;
}

bool World::GetUncompressedFrame(frame_t *deltaFrame, frame_t *frame)
{
	BitBuffer frameStream;
	if (!deltaFrame) {
		return false;
	}

	if (deltaFrame->delta != -1) {
		m_System->DPrintf("World::GetUncompressedFrame: unexpected non baseline delta frame.\n");
		return false;
	}

	memcpy(frame, deltaFrame, sizeof(*frame));

	frame->delta = 0;
	frame->entitiesSize = deltaFrame->entitynum * sizeof(entity_state_t);

	if (frame->entitynum > MAX_PACKET_ENTITIES) {
		m_System->Errorf("World::GetUncompressedFrame: entnum >= MAX_PACKET_ENTITIES.\n");
	}

	if (frame->entitynum)
	{
		if (!GetFrameFromCache(deltaFrame->seqnr, (entity_state_t **)&frame->entities))
		{
			m_Delta.SetLargeTimeBufferSize(true);
			frameStream.SetBuffer(deltaFrame->entities, deltaFrame->entitiesSize);
			UncompressEntitiesFromStream(frame, &frameStream);
			m_Delta.SetLargeTimeBufferSize(false);
		}
	}

	return true;
}

void World::ParseClientData(BitBuffer *stream, unsigned int deltaSeqNr, BitBuffer *to, clientdata_t *clientData)
{
	weapon_data_t nullWeaponData;
	clientdata_t fromClientData, nullClientData;

	memset(&nullWeaponData, 0, sizeof(nullWeaponData));
	memset(&fromClientData, 0, sizeof(fromClientData));
	memset(&nullClientData, 0, sizeof(nullClientData));
	memset(clientData, 0, sizeof(*clientData));

	if (deltaSeqNr && !GetClientData(deltaSeqNr, &fromClientData)) {
		m_System->Printf("WARNING! World::ParseClientData: couldn't uncompress delta frame %i\n", deltaSeqNr);
	}

	m_Delta.ParseDelta(stream, (byte *)&fromClientData, (byte *)clientData, GetClientDelta());
	to->StartBitMode();
	m_Delta.WriteDelta(to, (byte *)&nullClientData, (byte *)clientData, true, GetClientDelta());
	to->EndBitMode();

	while (stream->ReadBit())
	{
		stream->ReadBits(6);
		m_Delta.ParseDelta(stream, (byte *)&nullWeaponData, (byte *)&nullWeaponData, GetWeaponDelta());
	}
}

int World::ParseDeltaHeader(BitBuffer *stream, bool &remove, bool &custom, int &numbase, bool &newbl, int &newblindex, bool full, int &offset)
{
	int num;
	bool isdelta, isnext;

	offset = 0;
	custom = false;
	newbl = false;

	if (full)
	{
		isdelta = stream->ReadBit() ? true : false;
		remove = false;
	}
	else
	{
		isdelta = false;
		remove = stream->ReadBit() ? true : false;
	}

	if (isdelta)
	{
		num = numbase + 1;
	}
	else
	{
		if (stream->ReadBit())
		{
			num = stream->ReadBits(11);
		}
		else
		{
			int delta = stream->ReadBits(6);
			num = delta + numbase;
		}
	}

	numbase = num;

	if (!remove)
	{
		custom = stream->ReadBit() ? true : false;

		if (m_MaxInstanced_BaseLine)
		{
			isnext = stream->ReadBit() ? true : false;
			if (isnext)
			{
				newbl = true;
				newblindex = stream->ReadBits(6);
			}
		}

		if (full && !newbl)
		{
			isnext = stream->ReadBit() ? true : false;
			if (isnext)
			{
				offset = stream->ReadBits(6);
			}
		}
	}

	return num;
}

int World::CompressFrame(frame_t *from, BitBuffer *stream)
{
	deltacallback_t header;
	header.instanced_baseline = (m_MaxInstanced_BaseLine > 0) ? true : false;
	header.num = 0;
	header.offset = 0;
	header.numbase = 0;
	header.newblindex = 0;
	header.full = true;
	header.newbl = false;
	header.remove = false;
	header.custom = false;

	if (from->delta)
	{
		m_System->Errorf("World::CompressFrame: frame was not uncompressed.\n");
		return 0;
	}

	m_Delta.SetTime(from->time);
	stream->StartBitMode();

	entity_state_t *entities = (entity_state_t *)from->entities;
	unsigned char *start = stream->CurrentByte();
	for (auto entnum = 0u; entnum < from->entitynum; entnum++)
	{
		header.num = entities[entnum].number;
		header.custom = (entities[entnum].entityType & ENTITY_BEAM) == ENTITY_BEAM;
		header.newblindex = 0;
		header.newbl = false;

		entity_state_t *baseline = &m_BaseLines[header.num];
		header.offset = FindBestBaseline(entnum, &baseline, entities, header.num, header.custom);

		delta_t *delta = GetDeltaEncoder(header.num, header.custom);
		m_Delta.WriteDelta(stream, (byte *)baseline, (byte *)&entities[entnum], true, delta, &header);
	}

	stream->WriteBits(0, 16);
	stream->EndBitMode();

	return stream->CurrentByte() - start;
}

int World::FindBestBaseline(int index, entity_state_t **baseline, entity_state_t *to, int num, bool custom)
{
	int bestbitnumber;
	delta_t *delta;

	delta = GetDeltaEncoder(num, custom);
	bestbitnumber = m_Delta.TestDelta((byte *)*baseline, (byte *)&to[index], delta);
	bestbitnumber -= 6;

	int i = 0;
	int bitnumber = 0;
	int bestfound = index;

	for (i = index - 1; bestbitnumber > 0 && i >= 0 && (index - i) <= 64; i--)
	{
		if (to[index].modelindex == to[i].modelindex)
		{
			bitnumber = m_Delta.TestDelta((byte *)&to[i], (byte *)&to[index], delta);

			if (bitnumber < bestbitnumber)
			{
				bestbitnumber = bitnumber;
				bestfound = i;
			}
		}
	}

	if (index != bestfound) {
		*baseline = &to[bestfound];
	}

	return index - bestfound;
}

void World::ClearEntityCache()
{
	if (m_DeltaCache)
	{
		for (int i = 0; i < m_MaxCacheIndex; i++) {
			m_DeltaCache[i].buffer.Free();
		}

		memset(m_DeltaCache, 0, sizeof(deltaCache_t) * m_MaxCacheIndex);
	}

	if (m_FrameCache) {
		memset(m_FrameCache, 0, sizeof(frameCache_t) * m_MaxCacheIndex);
	}

	m_CacheHits = 1;
	m_CacheFaults = 1;
}

bool World::GetFrameFromCache(unsigned int seqNr, entity_state_t **entities)
{
	for (int i = 0; i < m_MaxCacheIndex; i++)
	{
		if (m_FrameCache[i].seqNr == seqNr)
		{
			*entities = m_FrameCache[i].entities;
			m_CacheHits++;
			return true;
		}
	}

	static int next = 0;
	m_FrameCache[next].seqNr = seqNr;
	*entities = m_FrameCache[next].entities;

	if (++next == m_MaxCacheIndex) {
		next = 0;
	}

	m_CacheFaults++;
	return false;
}

bool World::GetDeltaFromCache(unsigned int seqNr, unsigned int deltaNr, BitBuffer **buffer)
{
	for (int i = 0; i < m_MaxCacheIndex; i++)
	{
		if (m_DeltaCache[i].seqNr == seqNr && m_DeltaCache[i].deltaNr == deltaNr)
		{
			*buffer = &m_DeltaCache[i].buffer;
			m_CacheHits++;
			return true;
		}
	}

	static int next = 0;
	m_DeltaCache[next].seqNr = seqNr;
	m_DeltaCache[next].deltaNr = deltaNr;
	*buffer = &m_DeltaCache[next].buffer;

	if (++next == m_MaxCacheIndex) {
		next = 0;
	}

	m_CacheFaults++;
	return 0;
}

void World::WritePacketEntities(BitBuffer *stream, frame_t *frame, frame_t *deltaframe)
{
	int oldmax;
	int oldindex;
	int newindex;
	int newnum;
	int oldnum;
	entity_state_t *frameEntities;
	entity_state_t *deltaEntities;

	deltacallback_t header;
	header.instanced_baseline = (m_MaxInstanced_BaseLine > 0) ? true : false;
	header.num = 0;
	header.offset = 0;
	header.numbase = 0;
	header.newblindex = 0;
	header.full = false;
	header.newbl = false;
	header.remove = false;
	header.custom = false;

	if (frame->delta || deltaframe->delta) {
		m_System->Errorf("World::WritePacketEntities: frame and delta frame must be uncompressed.\n");
		return;
	}

	m_Delta.SetTime(frame->time);

	oldmax = deltaframe->entitynum;
	newnum = 0; // index in frame->entities
	oldnum = 0; // index in deltaframe->entities

	frameEntities = (entity_state_t *)frame->entities;
	deltaEntities = (entity_state_t *)deltaframe->entities;

	stream->StartBitMode();
	while (true)
	{
		if ((unsigned)newnum < frame->entitynum)
		{
			newindex = frameEntities[newnum].number;
		}
		else
		{
			if (oldnum >= oldmax)
				break;

			// TODO: Unreachable code
			if ((unsigned)newnum < frame->entitynum)
				newindex = frameEntities[newnum].number;
			else
				newindex = 9999;
		}

		if (oldnum < oldmax)
			oldindex = deltaEntities[oldnum].number;
		else
			oldindex = 9999;

		if (newindex == oldindex)
		{
			header.custom = (frameEntities[newnum].entityType & ENTITY_BEAM) == ENTITY_BEAM;
			header.num = newindex;
			header.newblindex = 0;
			header.newbl = false;
			header.remove = false;

			delta_t *delta = GetDeltaEncoder(newindex, header.custom);
			m_Delta.WriteDelta(stream, (byte *)&deltaEntities[oldnum], (byte *)&frameEntities[newnum], true, delta, &header);

			oldnum++;
			newnum++;
			continue;
		}

		if (newindex >= oldindex)
		{
			if (newindex > oldindex)
			{
				header.num = oldindex;
				header.remove = true;
				header.newbl = false;
				header.newblindex = 0;

				m_Delta.WriteHeader(stream, &header);
				++oldnum;
			}
			continue;
		}

		header.custom = (frameEntities[newnum].entityType & ENTITY_BEAM) == ENTITY_BEAM;
		header.newblindex = 0;
		header.num = newindex;
		header.remove = false;
		header.newbl = false;

		delta_t *delta = GetDeltaEncoder(newindex, header.custom);
		m_Delta.WriteDelta(stream, (byte *)&m_BaseLines[oldnum], (byte *)&frameEntities[newnum], true, delta, &header);
		newnum++;
	}

	stream->WriteBits(0, 16);
	stream->EndBitMode();
}

char *World::GetType()
{
	return WORLD_INTERFACE_VERSION;
}

char *World::GetStatusLine()
{
	static char string[256];
	if (IsActive())
	{
		_snprintf(string, sizeof(string),
			"Game \"%s\", Map \"%s\", Time %s, Players %i\nFrame cache use %.1f, Buffered time %.0f .\n",
			m_GameDir,
			m_LevelName,
			COM_FormatTime(m_WorldTime),
			GetNumPlayers(),
			m_CacheHits / float(m_CacheHits + m_CacheFaults),
			GetBufferedGameTime());
	}
	else
	{
		_snprintf(string, sizeof(string), "World not active.\n");
	}

	return string;
}

void World::ClearServerInfo()
{
	m_ServerInfo.Clear();

	memset(&m_DetailedServerInfo, 0, sizeof(m_DetailedServerInfo));
	m_DetailedServerInfo.type = '?';
	m_DetailedServerInfo.os   = '?';
	m_DetailedServerInfo.pw   = '?';
}

void World::SetServerInfo(serverinfo_t *serverinfo)
{
	memcpy(&m_DetailedServerInfo, serverinfo, sizeof(m_DetailedServerInfo));
}

void World::FinishGame()
{
	if (m_WorldState != WORLD_RUNNING) {
		return;
	}

	SetState(WORLD_COMPLETE);
	BaseSystemModule::FireSignal(7);
}

void World::StopGame()
{
	BaseSystemModule::FireSignal(1);
	Reset();
	SetState(WORLD_DISCONNECTED);
}

void World::WriteSigonData(BitBuffer *stream)
{
	stream->WriteBuf(m_SignonData.GetData(), m_SignonData.CurrentSize());
}

bool World::AddSignonData(unsigned char type, unsigned char *data, int size)
{
	m_SignonData.WriteByte(type);
	m_SignonData.WriteBuf(data, size);

	return m_SignonData.IsOverflowed();
}

int World::FindUserMsgByName(char *name)
{
	for (UserMsg *pList = m_ClientUserMsgs; pList; pList = pList->next)
	{
		if (!strcmp(pList->szName, name)) {
			return pList->iMsg;
		}
	}

	return 0;
}

double World::GetTime()
{
	return m_WorldTime;
}

bool World::UncompressEntitiesFromStream(frame_t *frame, BitBuffer *stream, unsigned int from)
{
	int newnum, oldnum;
	int oldindex, newindex;

	bool remove, custom, newbl;
	int newblindex, numbase, offset;

	frame_t deltaFrame;
	if (!GetUncompressedFrame(from, &deltaFrame)) {
		m_System->DPrintf("WARNING! World::UncompressEntitiesFromStream: delta frame too old.\n");
		stream->m_Overflowed = true;
		return false;
	}

	oldindex = 0;
	newindex = 0;

	remove = false;
	custom = false;
	newbl = false;
	newblindex = 0;
	numbase = 0;

	m_Delta.SetTime(frame->time);
	stream->StartBitMode();

	entity_state_t *entity = (entity_state_t *)frame->entities;
	entity_state_t *deltaEntity = (entity_state_t *)deltaFrame.entities;

	while (stream->PeekBits(16))
	{
		newnum = ParseDeltaHeader(stream, remove, custom, numbase, newbl, newblindex, false, offset);

		if ((unsigned)oldindex < deltaFrame.entitynum)
			oldnum = deltaEntity[oldindex].number;
		else
			oldnum = 9999;

		while (newnum > oldnum)
		{
			if (newindex >= MAX_PACKET_ENTITIES)
			{
				m_System->DPrintf("WARNING!World::UncompressEntitiesFromStream: newindex >= MAX_PACKET_ENTITIES.\n");
				stream->m_Overflowed = true;
			}

			memcpy(&entity[newindex], &deltaEntity[oldindex], sizeof(entity[newindex]));

			newindex++;
			oldindex++;

			if ((unsigned)oldindex < deltaFrame.entitynum)
				oldnum = deltaEntity[oldindex].number;
			else
				oldnum = 9999;
		}

		if (newnum >= oldnum)
		{
			if (newnum == oldnum)
			{
				if (remove)
				{
					++oldindex;
				}
				else
				{
					entity[newindex].entityType = custom ? ENTITY_BEAM : ENTITY_NORMAL;

					delta_t *delta = GetDeltaEncoder(newnum, custom);
					m_Delta.ParseDelta(stream, (byte *)&deltaEntity[oldindex], (byte *)&entity[newindex], delta);

					entity[newindex].number = newnum;
					++newindex;
					++oldindex;
				}
			}
		}
		else if (!remove)
		{
			if (newindex >= MAX_PACKET_ENTITIES)
			{
				m_System->DPrintf("World::UncompressEntitiesFromStream: newindex >= MAX_PACKET_ENTITIES.\n");
				stream->m_Overflowed = true;
			}

			entity_state_t *baseline;
			if (newbl)
			{
				baseline = &m_Instanced_BaseLines[newblindex];
			}
			else if (offset)
			{
				baseline = &entity[newindex - offset];
			}
			else
			{
				baseline = &m_BaseLines[newnum];
			}

			entity[newindex].entityType = custom ? ENTITY_BEAM : ENTITY_NORMAL;

			delta_t *delta = GetDeltaEncoder(newnum, custom);
			m_Delta.ParseDelta(stream, (byte *)baseline, (byte *)&entity[newindex], delta);
			entity[newindex].number = newnum;
			newindex++;
		}
	}

	if (stream->ReadShort())
	{
		m_System->DPrintf("WARNING! World::UncompressEntitiesFromStream: missing end tag.\n");
		return false;
	}

	stream->EndBitMode();

	while ((unsigned)oldindex < deltaFrame.entitynum)
	{
		if (newindex >= MAX_PACKET_ENTITIES)
		{
			m_System->DPrintf("WARNING! World::UncompressEntitiesFromStream: newindex >= MAX_PACKET_ENTITIES.\n");
			stream->m_Overflowed = true;
		}

		memcpy(&entity[newindex], &deltaEntity[oldindex], sizeof(entity[newindex]));
		newindex++;
		oldindex++;
	}

	if (newindex != frame->entitynum) {
		m_System->DPrintf("WARNING! World::UncompressEntitiesFromStream: newindex != frame->entitynum.\n");
	}

	return true;
}

bool World::UncompressEntitiesFromStream(frame_t *frame, BitBuffer *stream)
{
	int num;
	int newindex = 0;
	int entnum = frame->entitynum;
	entity_state_t *baseline;
	bool remove, custom, newbl;
	int newblindex, numbase, offset;

	newblindex = 0;
	numbase = 0;

	remove = false;
	custom = false;
	newbl = false;

	entity_state_t *entities = (entity_state_t *)frame->entities;
	m_Delta.SetTime(frame->time);
	stream->StartBitMode();
	while (true)
	{
		if (stream->IsOverflowed()) {
			m_System->Printf("WARNING! World::UncompressEntitiesFromStream: incoming entities are corrupt.\n");
			break;
		}

		if (!stream->PeekBits(16)) {
			break;
		}

		num = ParseDeltaHeader(stream, remove, custom, numbase, newbl, newblindex, true, offset);
		if (num >= MAX_ENTITIES)
		{
			m_System->Errorf("World::GetUncompressedFrame: entity number %i >= MAX_ENTITIES\n", num);
			memset(frame, 0, sizeof(*frame));
			return false;
		}

		if (remove)
		{
			m_System->Errorf("World::GetUncompressedFrame: remove invalid on non-delta compressed frames\n");
			memset(frame, 0, sizeof(*frame));
			return false;
		}

		if (newbl)
		{
			baseline = &m_Instanced_BaseLines[newblindex];
		}
		else if (offset)
		{
			baseline = &entities[newindex - offset];
		}
		else
		{
			baseline = &m_BaseLines[num];
		}

		entities[newindex].entityType = custom ? ENTITY_BEAM : ENTITY_NORMAL;

		delta_t *delta = GetDeltaEncoder(num, custom);
		m_Delta.ParseDelta(stream, (byte *)baseline, (byte *)&entities[newindex], delta);
		entities[newindex].number = num;
		newindex++;
	}

	if (stream->ReadShort()) {
		m_System->Printf("WARNING! World::UncompressEntitiesFromStream: missing end tag.\n");
		return false;
	}

	stream->EndBitMode();

	if (newindex != entnum) {
		m_System->DPrintf("WARNING! World::UncompressEntitiesFromStream: newindex != entnum.\n");
		return false;
	}

	return true;
}

void World::WriteNewData(BitBuffer *stream)
{
	WriteServerinfo(stream);
	WriteRegisteredUserMessages(stream);

	stream->WriteByte(svc_stufftext);
	stream->WriteString(COM_VarArgs("fullserverinfo \"%s\"\n", m_ServerInfo.GetString()));

	WriteResources(stream);
}

void World::SetName(char *newName)
{
	strcopy(m_Name, newName);
}

float World::GetBufferedGameTime()
{
	if (!IsActive() || m_Frames.IsEmpty()) {
		return 0;
	}

	frame_t *firstFrame = (frame_t *)m_Frames.GetFirst();
	frame_t *lastFrame = (frame_t *)m_Frames.GetLast();

	return lastFrame->time - firstFrame->time;
}

int World::RemoveFrames(unsigned int startSeqNr, unsigned int endSeqNr)
{
	if (startSeqNr > endSeqNr) {
		return 0;
	}

	frame_t *startFrame = (frame_t *)m_Frames.FindExactKey(startSeqNr);
	frame_t *endFrame = (frame_t *)m_Frames.FindExactKey(endSeqNr);
	if (!startFrame || !endFrame) {
		return 0;
	}

	unsigned int nextseqnr = startFrame->seqnr;
	unsigned int lastseqnr = endFrame->seqnr;

	int seqNrOffset = lastseqnr - nextseqnr + 1;
	float timeOffset = endFrame->time - startFrame->time;

	frame_t *frame = startFrame;
	while (frame)
	{
		if (frame->seqnr > lastseqnr) {
			break;
		}

		m_Frames.Remove(frame);
		m_FramesByTime.Remove(frame);

		if (frame->data) {
			Mem_Free(frame->data);
		}

		Mem_Free(frame);
		frame = (frame_t *)m_Frames.FindExactKey(++nextseqnr);
	}

	if (frame != m_Frames.GetFirst())
	{
		ClearEntityCache();
		while (frame)
		{
			RearrangeFrame(frame, seqNrOffset, timeOffset);
			frame = (frame_t *)m_Frames.FindExactKey(++nextseqnr);
		}
		ClearEntityCache();
	}

	return seqNrOffset;
}

int World::DuplicateFrames(unsigned int startSeqNr, unsigned int endSeqNr)
{
	m_System->Printf("TODO World::DuplicateFrames\n");
	return 0;
}

int World::MoveFrames(unsigned int startSeqNr, unsigned int endSeqNr, double destSeqnr)
{
	m_System->Printf("TODO World::MoveFrames\n");
	return 0;
}

int World::RevertFrames(unsigned int startSeqNr, unsigned int endSeqNr)
{
	m_System->Printf("TODO World::RevertFrames\n");
	return 0;
}

void World::ClearFrames()
{
	frame_t *frame = (frame_t *)m_Frames.GetFirst();
	while (frame)
	{
		if (frame->data) {
			Mem_Free(frame->data);
		}

		Mem_Free(frame);
		frame = (frame_t *)m_Frames.GetNext();
	}

	m_Frames.Clear();
	m_FramesByTime.Clear();
}

void World::CheckFrameBufferSize()
{
	if (m_MaxBufferLength <= 0) {
		return;
	}

	frame_t *frame = (frame_t *)m_Frames.GetLast();
	if (!frame) {
		return;
	}

	frame_t *firstFrame = (frame_t *)m_Frames.GetFirst();
	if (!firstFrame) {
		return;
	}

	frame_t *newfirstFrame = (frame_t *)m_FramesByTime.FindClosestKey(frame->time - m_MaxBufferLength);
	if (newfirstFrame) {
		RemoveFrames(firstFrame->seqnr, newfirstFrame->seqnr - 1);
	}
}

void World::ReorderFrameTimes(float newLastTime)
{
	frame_t *fprev = (frame_t *)m_Frames.GetLast();
	if (!fprev) {
		return;
	}

	frame_t *f;
	float offset = newLastTime;
	int fseqnr = fprev->seqnr - 1;
	while ((f = (frame_t *)m_Frames.FindExactKey(fseqnr)))
	{
		float timediff = fprev->time - f->time;

		fseqnr--;
		fprev->time = offset;
		fprev = f;

		offset -= timediff;
		f = (frame_t *)m_Frames.FindExactKey(fseqnr);
	}

	fprev->time = offset;
}

void World::SetServerInfo(int protocol, CRC32_t nserverCRC, unsigned char *nclientdllmd5, int nmaxclients, int nplayernum, int ngametype, char *ngamedir, char *nservername, char *nlevelname)
{
	m_Protocol = protocol;
	m_ServerCRC = nserverCRC;
	memcpy(m_ClientdllMD5, nclientdllmd5, sizeof(m_ClientdllMD5));

	m_Maxclients = nmaxclients;
	m_PlayerNum = nplayernum;
	m_GameType = ngametype;

	strcopy(m_GameDir, ngamedir);
	strcopy(m_LevelName, nlevelname);

	_snprintf(m_ServerName, sizeof(m_ServerName), "%s:%i", nservername, m_PlayerNum);
}

void World::SetMoveVars(movevars_t *nmovevars)
{
	memcpy(&m_MoveVars, nmovevars, sizeof(m_MoveVars));
}

void World::SetPaused(bool state)
{
	if (m_IsPaused == state) {
		return;
	}

	m_IsPaused = state;
	BaseSystemModule::FireSignal(m_IsPaused ? 5 : 6);
}

bool World::IsPaused()
{
	return m_IsPaused;
}

bool World::IsComplete()
{
	return m_WorldState == WORLD_COMPLETE;
}

void World::SetBufferSize(float seconds)
{
	if (seconds <= 0) {
		m_MaxBufferLength = -1;
		return;
	}

	m_MaxBufferLength = seconds + 10;
}

void World::ParseDeltaDescription(BitBuffer *stream)
{
	delta_description_t *pdesc;
	delta_description_t nulldesc;
	memset(&nulldesc, 0, sizeof(nulldesc));

	char *s = stream->ReadString();
	if (!s || !s[0]) {
		m_System->Errorf("ParseDeltaDescription: Illegible description name\n");
		return;
	}

	char szDesc[256];
	strcopy(szDesc, s);
	m_System->DPrintf("Reading delta description for: %s.\n", s);

	delta_t **ppdelta = m_Delta.LookupRegistration(szDesc);
	if (ppdelta && *ppdelta) {
		m_Delta.FreeDescription(ppdelta);
	}

	stream->StartBitMode();

	int c = stream->ReadShort();
	*ppdelta = (delta_t *)Mem_ZeroMalloc(sizeof(delta_t));
	pdesc = (delta_description_t *)Mem_ZeroMalloc(sizeof(delta_description_t) * c);

	(*ppdelta)->dynamic = TRUE;
	(*ppdelta)->fieldCount = c;

	for (int i = 0; i < c; i++) {
		m_Delta.ParseDelta(stream, (byte *)&nulldesc, (byte *)&pdesc[i], Delta::m_MetaDelta);
	}

	(*ppdelta)->pdd = pdesc;
	stream->EndBitMode();
	m_Delta.UpdateDescriptions();
}

void World::SetCDInfo(int ncdtrack, int nlooptrack)
{
	m_CDTrack = ncdtrack;
	m_LoopTrack = nlooptrack;
}

void World::SetGameServerAddress(NetAddress *address)
{
	m_GameServerAddress.FromNetAddress(address);
}

InfoString *World::GetServerInfoString()
{
	return &m_ServerInfo;
}

bool World::GetPlayerInfoString(int playerNum, InfoString *infoString)
{
	if (!infoString || playerNum >= MAX_CLIENTS || playerNum < 0) {
		return false;
	}

	if (!m_Players[playerNum].userinfo[0]
		|| !m_Players[playerNum].active) {
		return false;
	}

	return infoString->SetString(m_Players[playerNum].userinfo);
}

void World::SetExtraInfo(char *nclientfallback, int nallowCheats)
{
	strcopy(m_ClientFallback, nclientfallback);
	m_AllowCheats = nallowCheats ? true : false;
}

void World::ParseEvent(BitBuffer *stream)
{
	event_args_t nullargs;
	memset(&nullargs, 0, sizeof(nullargs));
	m_Delta.ParseDelta(stream, (byte *)&nullargs, (byte *)&nullargs, GetEventDelta());
}

void World::ParseBaseline(BitBuffer *stream)
{
	bool custom;
	unsigned char type;
	int count = 0;

	entity_state_t nullstate;
	entity_state_t entstate;
	delta_t **ppentity, **ppcustom, **ppplayer;

	ppentity = m_Delta.LookupRegistration("entity_state_t");
	ppplayer = m_Delta.LookupRegistration("entity_state_player_t");
	ppcustom = m_Delta.LookupRegistration("custom_entity_state_t");

	if (!(ppentity && *ppentity && ppcustom && *ppcustom && ppplayer && *ppplayer)) {
		m_System->Errorf("Server::ParseBaseline: needed delta encoder missing.\n");
		return;
	}

	memset(&nullstate, 0, sizeof(nullstate));
	stream->StartBitMode();
	m_MaxBaseLines = 0;
	ClearBaseline();

	while (stream->PeekBits(16) != 0xFFFF)
	{
		int index = stream->ReadBits(11);
		m_MaxBaseLines = max(index, m_MaxBaseLines);

		memset(&entstate, 0, sizeof(nullstate));

		type = stream->ReadBits(2);
		custom = (type & ENTITY_BEAM) == ENTITY_BEAM;

		delta_t *delta;
		if (custom)
		{
			delta = *ppcustom;
		}
		else
		{
			delta = IsPlayerIndex(index) ? *ppplayer : *ppentity;
		}

		m_Delta.ParseDelta(stream, (byte *)&nullstate, (byte *)&entstate, delta);

		entstate.entityType = type;
		AddBaselineEntity(index, &entstate);
		count++;
	}

	m_System->Printf("Received baseline with %i entities.\n", count);

	stream->ReadBits(16);
	ClearInstancedBaseline();
	m_MaxInstanced_BaseLine = stream->ReadBits(6);
	for (int i = 0; i < m_MaxInstanced_BaseLine; i++)
	{
		memset(&entstate, 0, sizeof(entstate));
		m_Delta.ParseDelta(stream, (byte *)&nullstate, (byte *)&entstate, *ppentity);
		AddInstancedBaselineEntity(i, &entstate);
	}

	if (m_MaxInstanced_BaseLine) {
		m_System->Printf("Received instanced baseline with %i entities.\n", m_MaxInstanced_BaseLine);
	}

	stream->EndBitMode();
}

bool World::IsVoiceEnabled()
{
	return m_VoiceEnabled;
}

int World::GetSlotNumber()
{
	return m_PlayerNum;
}

void World::SetViewEntity(int nviewentity)
{
	m_ViewEntity = nviewentity;
}

void World::SetVoiceEnabled(bool state)
{
	m_VoiceEnabled = state;
}

NetAddress *World::GetGameServerAddress()
{
	return &m_GameServerAddress;
}

char *World::GetLevelName()
{
	return m_LevelName;
}

IBSPModel *World::GetWorldModel()
{
	if (!m_WorldModel.IsValid()) {
		m_WorldModel.Load(m_LevelName, true);
	}

	return &m_WorldModel;
}

char *World::GetGameDir()
{
	return m_GameDir;
}

int World::GetServerCount()
{
	return m_ServerCount;
}

int World::GetMaxClients()
{
	return m_Maxclients;
}

bool World::SaveAsDemo(char *filename, IDirector *director)
{
	DemoFile demoFile;
	NetChannel demoChannel;

	client_data_t cdata;
	double lastFrameTime = 0;
	double originalWorldTime = m_WorldTime;
	unsigned int clientDelta = 0;
	unsigned int lastFrameSeqNr = 0;

	memset(&cdata, 0, sizeof(cdata));

	frame_t *frame = (frame_t *)m_Frames.GetFirst();
	if (!frame) {
		return false;
	}

	m_WorldTime = frame->time;
	demoChannel.Create(m_System);
	demoFile.Init(this, nullptr, &demoChannel);

	if (!demoFile.StartRecording(filename)) {
		return false;
	}

	m_System->Printf("Saving game to \"%s\". This can take some time...\n", filename);

	while (frame)
	{
		m_WorldTime = frame->time;

		if (!lastFrameSeqNr || lastFrameSeqNr > frame->seqnr) {
			lastFrameSeqNr = frame->seqnr - 1;
		}

		if (lastFrameSeqNr < frame->seqnr)
		{
			demoChannel.m_unreliableStream.WriteByte(svc_time);
			demoChannel.m_unreliableStream.WriteByte(frame->time);

			WriteFrame(frame, lastFrameSeqNr, &demoChannel.m_reliableStream, &demoChannel.m_unreliableStream, lastFrameSeqNr, clientDelta, true);

			if (director) {
				director->WriteCommands(&demoChannel.m_reliableStream, lastFrameTime, frame->time);
			}

			lastFrameSeqNr = frame->seqnr;
			clientDelta = (demoChannel.m_outgoing_sequence & 0xFF);

			if (demoChannel.m_reliableStream.IsOverflowed()) {
				m_System->Printf("WARNING! World::SaveAsDemo: reliable data overflow.\n");
				return false;
			}

			if (demoChannel.m_unreliableStream.IsOverflowed()) {
				demoChannel.m_unreliableStream.Clear();
			}

			demoFile.WriteDemoMessage(&demoChannel.m_unreliableStream, &demoChannel.m_reliableStream);
			demoChannel.TransmitOutgoing();
			demoFile.WriteUpdateClientData(&cdata);

			lastFrameTime = frame->time;
			frame = (frame_t *)m_Frames.FindExactKey(lastFrameSeqNr + 1);
		}
	}

	demoFile.CloseFile();
	demoChannel.Clear();
	return true;
}

serverinfo_t *World::GetServerInfo()
{
	return &m_DetailedServerInfo;
}

bool World::WriteDeltaEntities(BitBuffer *stream, frame_t *fullFrame, unsigned int deltaSeqNr, unsigned int clientDelta)
{
	BitBuffer *buffer;
	if (!GetDeltaFromCache(fullFrame->seqnr, deltaSeqNr, &buffer))
	{
		frame_t deltaFrame;
		if (!GetUncompressedFrame(deltaSeqNr, &deltaFrame))
		{
			m_System->DPrintf("WARNING! World::WriteFrame: delta frame too old (%i).\n", deltaSeqNr);
			return false;
		}

		buffer->Resize(fullFrame->entitiesSize);
		WritePacketEntities(buffer, fullFrame, &deltaFrame);
	}

	stream->WriteByte(svc_deltapacketentities);
	stream->WriteShort(fullFrame->entitynum);
	stream->WriteByte(clientDelta);
	stream->ConcatBuffer(buffer);
	return true;
}

void World::RearrangeFrame(frame_t *frame, int seqNrOffset, float timeOffset)
{
	frame_t fullFrame;
	if (!GetUncompressedFrame(frame, &fullFrame)) {
		return;
	}

	m_FramesByTime.ChangeKey(frame, frame->time - timeOffset);
	m_Frames.ChangeKey(frame, frame->seqnr - seqNrOffset);

	frame->time -= timeOffset;
	frame->seqnr -= seqNrOffset;

	m_Delta.SetLargeTimeBufferSize(true);

	if (frame->entitynum)
	{
		for (unsigned int i = 0; i < fullFrame.entitynum; i++)
		{
			entity_state_t *entity = &((entity_state_t *)fullFrame.entities)[i];
			if (entity->animtime != m_BaseLines[entity->number].animtime) {
				entity->animtime -= timeOffset;
			}

			if (entity->impacttime != m_BaseLines[entity->number].impacttime) {
				entity->impacttime -= timeOffset;
			}

			if (entity->starttime != m_BaseLines[entity->number].starttime) {
				entity->starttime -= timeOffset;
			}
		}

		BitBuffer tempStream(frame->entities, frame->entitiesSize);
		memset(frame->entities, 0, frame->entitiesSize);

		int newsize = CompressFrame(&fullFrame, &tempStream);
		if ((unsigned)newsize > frame->entitiesSize || tempStream.IsOverflowed()) {
			m_System->Printf("WARNING! World::RearrangeFrame: wrong entities size (%i != %i).\n", frame->entitiesSize, newsize);
			return;
		}

		frame->entitiesSize = newsize;
	}

	m_Delta.SetLargeTimeBufferSize(false);
}

void World::UpdateServerInfo()
{
	strcopy(m_DetailedServerInfo.address, "");
	strcopy(m_DetailedServerInfo.name, GetHostName());

	COM_FileBase(m_LevelName, m_DetailedServerInfo.map);
	m_DetailedServerInfo.map[sizeof(m_DetailedServerInfo.map) - 1] = '\0';

	strcopy(m_DetailedServerInfo.gamedir, m_GameDir);
	strcopy(m_DetailedServerInfo.description, "HLTV");

	m_DetailedServerInfo.activePlayers = m_PlayerNum;
	m_DetailedServerInfo.maxPlayers = m_Maxclients;
	m_DetailedServerInfo.protocol = m_Protocol;
	m_DetailedServerInfo.type = GetServerType(HLST_Dedicated)[0];
	m_DetailedServerInfo.os = GetServerOS()[0];
	m_DetailedServerInfo.pw = PROTOCOL_VERSION;
	m_DetailedServerInfo.mod = false;
}

void World::SetHLTV(bool state)
{
	m_IsHLTV = state;
}

bool World::IsHLTV()
{
	return m_IsHLTV;
}

void World::RunFrame(double time)
{
	BaseSystemModule::RunFrame(time);
}

void World::ReceiveSignal(ISystemModule *module, unsigned int signal, void *data)
{
	BaseSystemModule::ReceiveSignal(module, signal, data);
}

void World::ExecuteCommand(int commandID, char *commandLine)
{
	BaseSystemModule::ExecuteCommand(commandID, commandLine);
}

void World::RegisterListener(ISystemModule *module)
{
	BaseSystemModule::RegisterListener(module);
}

void World::RemoveListener(ISystemModule *module)
{
	BaseSystemModule::RemoveListener(module);
}

IBaseSystem *World::GetSystem()
{
	return BaseSystemModule::GetSystem();
}

int World::GetSerial()
{
	return BaseSystemModule::GetSerial();
}

char *World::GetName()
{
	return BaseSystemModule::GetName();
}

int World::GetState()
{
	return BaseSystemModule::GetState();
}

int World::GetVersion()
{
	return BaseSystemModule::GetVersion();
}

IBaseInterface *CreateWorld()
{
	IWorld *pWorld = new World;
	return (IBaseInterface *)pWorld;
}

#ifndef HOOK_HLTV
bool World::IsDeltaEncoder() const
{
	if (Delta::m_CustomentityDelta
		&& Delta::m_EntityDelta
		&& Delta::m_PlayerDelta) {
		return true;
	}

	return false;
}

delta_t *World::GetDeltaEncoder(int index, bool custom)
{
	if (custom) {
		return Delta::m_CustomentityDelta;
	}
	else if (IsPlayerIndex(index)) {
		return Delta::m_PlayerDelta;
	}

	return Delta::m_EntityDelta;
}

delta_t *World::GetEventDelta() const {
	return Delta::m_EventDelta;
}

delta_t *World::GetClientDelta() const {
	return Delta::m_ClientDelta;
}

delta_t *World::GetEntityDelta() const {
	return Delta::m_EntityDelta;
}

delta_t *World::GetWeaponDelta() const {
	return Delta::m_WeaponDelta;
}

EXPOSE_INTERFACE_FN(CreateWorld, World, WORLD_INTERFACE_VERSION);
#endif // HOOK_HLTV
