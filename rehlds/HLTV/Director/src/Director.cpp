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

#ifdef DIRECTOR_MODULE
EXPOSE_SINGLE_INTERFACE(Director, IDirector, DIRECTOR_INTERFACE_VERSION);
#endif // DIRECTOR_MODULE

bool Director::Init(IBaseSystem *system, int serial, char *name)
{
	BaseSystemModule::Init(system, serial, name);
	m_System->RegisterCommand("slowmotion", this, CMD_ID_SLOWMOTION);

	m_historyLength = MAX_WORLD_HISTORY;
	m_history = (worldHistory_t *)malloc(sizeof(worldHistory_t) * m_historyLength);
	if (!m_history) {
		m_System->Printf("ERROR!Director::Init: not enough memory for world history.\n");
		return false;
	}

	m_LastExecTime = 0;
	m_currentTime = 0;
	m_slowMotion = 0.5f;

	for (int i = 0; i < MAX_WORLD_HISTORY; i++) {
		m_gaussFilter[i] = 1.0f / exp((i * i) / 10000.0f);
	}

	m_World = nullptr;
	m_Active = false;
	m_System->ExecuteFile("director.cfg");
	m_System->Printf("Director module initialized.\n");
	return true;
}

float WeightedAngle(vec_t *vec1, vec_t *vec2)
{
	float a = AngleBetweenVectors(vec1, vec2);
	if (a < 11.25f) {
		return 1;
	}
	else if (a < 22.5f) {
		return 0.5f;
	}
	else if (a < 45.0f) {
		return 0.25f;
	}
	else if (a < 90.0f) {
		return 0.125f;
	}

	return 0.05f;
}

void Director::WriteProxyStatus(BitBuffer *stream)
{
	;
}

void Director::ShutDown()
{
	if (m_State == MODULE_DISCONNECTED) {
		return;
	}

	if (m_history) {
		free(m_history);
	}

	if (m_World) {
		m_World->RemoveListener(this);
	}

	ClearDirectorCommands();
	BaseSystemModule::ShutDown();

	m_System->Printf("Director module shutdown.\n");
}

void Director::RunFrame(double time)
{
	BaseSystemModule::RunFrame(time);

	if (!m_World || !m_World->IsActive()) {
		return;
	}

	if (m_nextCutTime && m_Active)
	{
		ExecuteDirectorCommands();
		if ((m_currentTime - m_nextCutTime) > 10)
		{
			float duration = AddBestMODCut();
			if (!duration) {
				duration = AddBestGenericCut();
			}

			m_nextCutTime += duration;

			while (m_nextCutTime > m_history[m_nextCutSeqnr % m_historyLength].time) {
				m_nextCutSeqnr++;
			}
		}
	}
}

void Director::ReceiveSignal(ISystemModule *module, unsigned int signal, void *data)
{
	int from = module->GetSerial();
	if (m_World->GetSerial() != from) {
		return;
	}

	if (signal != 3) {
		return;
	}

	if (!m_World) {
		m_System->Printf("Director::ReceiveSignal: world == NULL\n");
		return;
	}

	frame_t frame;
	m_World->GetUncompressedFrame(m_World->GetLastFrame()->seqnr, &frame);
	AnalyseFrame(&frame);
}

char *Director::GetModName()
{
	return "valve";
}

void Director::NewGame(IWorld *world, IProxy *proxy)
{
	m_World = world;
	m_Proxy = proxy;

	m_World->RegisterListener(this);
	m_WorldModel = m_World->GetWorldModel();

	memset(m_history, 0, sizeof(*m_history) * m_historyLength);
	memset(&m_frameEvent, 0, sizeof(m_frameEvent));

	ClearDirectorCommands();

	m_lastCut = nullptr;
	m_LastExecTime = 0;
	m_nextCutTime = 0;
	m_currentTime = 0;
	m_nextCutSeqnr = 0;
	m_maxRank = 0;

	WriteSignonData();
	m_Active = true;
}

void Director::AnalyseFrame(frame_t *frame)
{
	unsigned int i;
	unsigned int num;
	float ranks[MAX_CLIENTS];
	unsigned int maxclients = m_World->GetMaxClients();
	unsigned int seqnr = frame->seqnr;
	worldHistory_t *now = &m_history[seqnr % m_historyLength];

	for (i = 0; i < MAX_CLIENTS; i++) {
		ranks[i] = now->players[i].rank;
	}

	memset(now, 0, sizeof(*now));
	for (i = 0; i < MAX_CLIENTS; i++) {
		now->players[i].rank = ranks[i];
	}

	if (frame->delta || !m_Active) {
		m_System->DPrintf("Director::AnalyseFrame: frame must be uncompressed.\n");
		return;
	}

	if (!m_currentTime)
	{
		m_nextCutTime = frame->time;
		m_nextCutSeqnr = seqnr;
	}

	m_currentSeqnr = seqnr;
	now->seqNr = seqnr;

	m_currentTime = frame->time;
	now->time = frame->time;

	unsigned int count = min(maxclients, frame->entitynum);
	for (num = 0; num < count; num++)
	{
		entity_state_t *ent = &((entity_state_t *)frame->entities)[num];
		unsigned int index = ent->number - 1;
		if (index >= maxclients) {
			continue;
		}

		playerData_t *player = &now->players[index];
		player->active = (ent->solid != SOLID_NOT);
		if (player->active)
		{
			player->origin[0] = ent->origin[0];
			player->origin[1] = ent->origin[1];
			player->origin[2] = ent->origin[2];

			vec3_t angles;
			AngleVectors(ent->angles, angles);
			VectorCopy(angles, player->angles);
		}
	}

	for (i = 0; i < MAX_CLIENTS; i++) {
		AnalysePlayer(i);
	}

	now->event.entity1 = m_frameEvent.entity1;
	now->event.entity2 = m_frameEvent.entity2;
	now->event.flags = m_frameEvent.flags;

	memset(&m_frameEvent, 0, sizeof(m_frameEvent));
}

void Director::SmoothRank(int playerNum, float rank)
{
	const unsigned int smoothRange = 40;
	m_history[(m_currentSeqnr + smoothRange) % m_historyLength].players[playerNum].rank = 0;

	unsigned int steps;
	for (unsigned int i = 0; i < smoothRange; i++)
	{
		float addition = rank * m_gaussFilter[i * (MAX_WORLD_HISTORY / smoothRange)];

		steps = (m_currentSeqnr - i) % m_historyLength;
		m_history[steps].players[playerNum].rank += addition;

		steps = (i + m_currentSeqnr) % m_historyLength;
		m_history[steps].players[playerNum].rank += addition;
	}
}

void Director::AnalysePlayer(int playerNum)
{
	int secondTarget = 0;
	float rank = RandomFloat(0.0f, 0.01f) + 1;
	float targetRank, bestTargetRank = 0;

	worldHistory_t *now = &m_history[m_currentSeqnr % m_historyLength];
	playerData_t *player = &now->players[playerNum];

	if (player->active)
	{
		if (m_WorldModel->IsValid())
		{
			m_WorldModel->SetPVS(player->origin);
			for (int i = 0; i < MAX_CLIENTS; i++)
			{
				playerData_t *target = &now->players[i];
				if (!target->active || !m_WorldModel->InPVS(target->origin)) {
					continue;
				}

				vec3_t offset;
				VectorSubtract(target->origin, player->origin, offset);

				float distance = Length(offset);
				if (distance >= 1.0f)
				{
					targetRank = WeightedAngle(target->angles, offset) + 1 / distance;

					// inverted
					VectorScale(offset, -1.0f, offset);

					targetRank = WeightedAngle(target->angles, offset) * targetRank;
					rank += targetRank;

					// remember closest player
					if (targetRank > bestTargetRank)
					{
						bestTargetRank = targetRank;
						secondTarget = i + 1;
					}
				}
			}
		}
	}
	else
	{
		rank = 0;
	}

	player->target = secondTarget;
	player->rank = player->rank + rank;
	SmoothRank(playerNum, rank);
}

void Director::WriteCommands(BitBuffer *stream, float startTime, float endTime)
{
	DirectorCmd *cmd = (DirectorCmd *)m_Commands.FindClosestKey(startTime);
	while (cmd && cmd->GetTime() <= endTime)
	{
		if (cmd->GetTime() > startTime)
		{
			if (cmd->GetType() == DRC_CMD_TIMESCALE)
			{
				float timescale;
				cmd->GetTimeScaleData(timescale);

				stream->WriteByte(svc_timescale);
				stream->WriteFloat(timescale);
			}

			cmd->WriteToStream(stream);
		}

		cmd = (DirectorCmd *)m_Commands.GetNext();
	}
}

void Director::WriteSignonData()
{
	static unsigned char cmd[] = {
		1,				// command length in bytes
		DRC_CMD_START	// tell them for start director
	};

	m_World->AddSignonData(svc_director, cmd, sizeof(cmd));
}

void Director::AddEvent(int entity1, int entity2, unsigned int flags)
{
	if ((flags & DRC_FLAG_PRIO_MASK) > (m_frameEvent.flags & DRC_FLAG_PRIO_MASK))
	{
		m_frameEvent.flags = flags;
		m_frameEvent.entity1 = entity1;
		m_frameEvent.entity2 = entity2;
	}
}

float Director::AddBestGenericCut()
{
	int seqNrMod = m_nextCutSeqnr % m_historyLength;

	float sumTarget2Rank[MAX_CLIENTS];
	float bestTarget2Rank, bestRank = 0;
	float targetRankSum = 0;
	int newTarget, newTarget2;
	int bestTarget2;

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		memset(sumTarget2Rank, 0, sizeof(sumTarget2Rank));

		float tillTime = m_nextCutTime + 4;
		while (tillTime > m_history[seqNrMod].time)
		{
			playerData_t *player = &m_history[seqNrMod].players[i];

			targetRankSum += player->rank;
			if (player->target) {
				sumTarget2Rank[player->target - 1] += player->rank;
			}

			if (++seqNrMod == m_historyLength) {
				seqNrMod = 0;
			}
		}

		bestTarget2 = 0;
		bestTarget2Rank = 0;

		for (int j = 0; j < MAX_CLIENTS; j++)
		{
			if (sumTarget2Rank[j] > bestTarget2Rank)
			{
				bestTarget2 = j + 1;
				bestTarget2Rank = sumTarget2Rank[j];
			}
		}

		if (targetRankSum > bestRank)
		{
			bestRank = targetRankSum;
			newTarget = i + 1;
			newTarget2 = bestTarget2;
		}
	}

	if (bestRank > m_maxRank) {
		m_maxRank = bestRank;
	}

	float duration = 1.0f;
	if (newTarget > 0)
	{
		if (m_lastCut && m_lastCut->GetType() == TYPE_DIRECTOR)
		{
			int lastTarget1, lastTarget2,flags;
			m_lastCut->GetEventData(lastTarget1, lastTarget2, flags);

			if (newTarget == lastTarget1 && newTarget2 == lastTarget2) {
				return 1.0f;
			}

			if (newTarget == lastTarget1) {
				duration = 2.0f;
			}
		}

		DirectorCmd *cmd = new DirectorCmd;
		cmd->SetEventData(newTarget, newTarget2, 0);
		cmd->SetTime(m_nextCutTime);

		RandomizeCommand(cmd);
		m_Commands.Add(cmd, m_nextCutTime);
		m_lastCut = cmd;
	}

	return duration;
}

float Director::AddBestMODCut()
{
	worldHistory_t *timepoint = FindBestEvent();
	if (!timepoint) {
		return 0;
	}

	DirectorCmd *cmd = new DirectorCmd;
	cmd->SetEventData(timepoint->event.entity1, timepoint->event.entity2, timepoint->event.flags);
	cmd->SetTime(m_nextCutTime);

	RandomizeCommand(cmd);
	m_Commands.Add(cmd, cmd->GetTime());
	m_lastCut = cmd;

	int lastTarget1, lastTarget2, lastFlags;
	cmd->GetEventData(lastTarget1, lastTarget2, lastFlags);

	float trailTime = 2.0f;
	if ((lastFlags & DRC_FLAG_SLOWMOTION) && m_slowMotion)
	{
		trailTime = 1.5f;

		cmd = new DirectorCmd;
		cmd->SetTimeScaleData(m_slowMotion);
		cmd->SetTime(timepoint->time - 0.25f);
		m_Commands.Add(cmd, cmd->GetTime());

		cmd = new DirectorCmd;
		cmd->SetTimeScaleData(1.0);
		cmd->SetTime(timepoint->time + trailTime);
		m_Commands.Add(cmd, cmd->GetTime());
	}

	return trailTime + timepoint->time - m_nextCutTime;
}

int Director::GetClosestPlayer(frame_t *frame, int entityIndex)
{
	unsigned int i;
	unsigned int bestPlayer = 0;
	unsigned int maxclients = m_World->GetMaxClients();

	if (!m_WorldModel->IsValid()) {
		return 0;
	}

	vec3_t origin;
	for (i = 0; i < frame->entitynum; i++)
	{
		entity_state_t *ent = &((entity_state_t *)frame->entities)[i];
		if (ent->number == entityIndex) {
			VectorCopy(origin, ent->origin);
			break;
		}
	}

	// entityIndex not found
	if (i == frame->entitynum) {
		return 0;
	}

	m_WorldModel->SetPVS(origin);

	float minDistance = 32000.0f;
	unsigned int count = min(maxclients, frame->entitynum);

	for (i = 0; i < count; i++)
	{
		entity_state_t *ent = &((entity_state_t *)frame->entities)[i];
		if ((unsigned)(ent->number - 1) < maxclients && m_WorldModel->InPVS(ent->origin))
		{
			vec3_t offset;
			VectorSubtract(ent->origin, origin, offset);

			float distance = Length(offset);
			if (distance >= 1 && distance < minDistance)
			{
				minDistance = distance;
				bestPlayer = ent->number;
			}
		}
	}

	return bestPlayer;
}

void Director::ClearDirectorCommands()
{
	DirectorCmd *cmd = (DirectorCmd *)m_Commands.GetFirst();
	while (cmd)
	{
		delete cmd;
		cmd = (DirectorCmd *)m_Commands.GetNext();
	}

	m_Commands.Clear();
}

void Director::RandomizeCommand(DirectorCmd *cmd)
{
	if ((cmd->GetType() & DRC_FLAG_NO_RANDOM) || cmd->GetType() != DRC_CMD_EVENT) {
		return;
	}

	int target1, target2, flags;
	cmd->GetEventData(target1, target2, flags);
	if ((flags & DRC_FLAG_SLOWMOTION) && RandomFloat(0, 1) > 0.25f)
	{
		flags &= ~DRC_FLAG_SLOWMOTION;
	}
	// check DRC_CMD_SOUND, DRC_CMD_STATUS, DRC_CMD_BANNER
	else if ((flags & DRC_FLAG_PRIO_MASK) > DRC_CMD_MESSAGE
		&& (flags & DRC_FLAG_PRIO_MASK) < DRC_CMD_STUFFTEXT && RandomFloat(0, 1) < 0.15f)
	{
		flags |= DRC_FLAG_SLOWMOTION;
	}

	// toggle flag DRC_FLAG_DRAMATIC
	if (RandomFloat(0, 1) < 0.33f) {
		flags ^= DRC_FLAG_DRAMATIC;
	}

	if (target1 && target2 && !(flags & DRC_FLAG_SLOWMOTION) && RandomFloat(0, 1) < 0.33f)
	{
		int swap = target1;
		target1 = target2;
		target2 = swap;
	}

	worldHistory_t *now = &m_history[m_nextCutSeqnr % m_historyLength];

	vec3_t v1, v2;
	VectorSubtract(now->players[target2 % MAX_CLIENTS].origin, now->players[target1 % MAX_CLIENTS].origin, v1);

	v1[2] = 0;
	VectorAngles(v1, v1);

	VectorCopy(now->players[target1 % MAX_CLIENTS].angles, v2);
	if (AngleLeftOfOther(v1, v2))
		flags &= ~DRC_FLAG_SIDE;
	else
		flags |= DRC_FLAG_SIDE;

	if (target1 == target2) {
		target2 = 0;
	}

	cmd->SetEventData(target1, target2, flags);
}

void Director::ExecuteCommand(int commandID, char *commandLine)
{
	if (commandID == CMD_ID_SLOWMOTION) {
		CMD_SlowMotion(commandLine);
		return;
	}

	m_System->Printf("ERROR! Director::ExecuteCommand: unknown command ID %i.\n", commandID);
}

char *Director::GetStatusLine()
{
	return "Default Director";
}

char *Director::GetType()
{
	return DIRECTOR_INTERFACE_VERSION;
}

void Director::ExecuteDirectorCommands()
{
	unsigned char bufferdata[4096];
	BitBuffer buffer(bufferdata, sizeof(bufferdata));
	buffer.Clear();

	float newTime = (float)m_Proxy->GetSpectatorTime();
	DirectorCmd *cmd = (DirectorCmd *)m_Commands.FindClosestKey(m_LastExecTime);
	if (!cmd) {
		return;
	}

	while (cmd && cmd->GetTime() <= newTime)
	{
		if (cmd->GetTime() > m_LastExecTime)
		{
			if (cmd->GetType() == DRC_CMD_TIMESCALE)
			{
				float timescale;
				cmd->GetTimeScaleData(timescale);
				m_Proxy->SetClientTimeScale(timescale);

				if (timescale < 1.0)
				{
					vec3_t pos = { 0.02f, 0.75f, 0.f };

					DirectorCmd slowmo;
					slowmo.SetMessageData(0, COM_PackRGBA(255, 160, 0, 255), pos, 0.3f, 0.1f, 2, 0, "Slow Motion");
					slowmo.WriteToStream(&buffer);
				}
			}
			else
			{
				cmd->WriteToStream(&buffer);
			}
		}

		cmd = (DirectorCmd *)m_Commands.GetNext();
	}

	if (buffer.IsOverflowed())
	{
		m_System->Printf("Director::ExecuteDirectorCommands: command overflow.\n");
	}
	else
	{
		m_Proxy->Broadcast(buffer.GetData(), buffer.CurrentSize(), GROUP_CLIENT_ALL, true);
	}

	m_LastExecTime = newTime;
}

void Director::CMD_SlowMotion(char *cmdLine)
{
	TokenLine params(cmdLine);
	if (params.CountToken() != 2) {
		m_System->Printf("Syntax: slowmotion <factor>\n");
		m_System->Printf("Currently slowmotion factor is %.2f\n", m_slowMotion);
		return;
	}

	float val = (float)atof(params.GetToken(1));
	m_slowMotion = clamp(val, 0.05f, 4.0f);
}

Director::worldHistory_t *Director::FindBestEvent()
{
	const int MAX_BEST_EVENT = 4;

	int i;
	int nseqMod = m_nextCutSeqnr % m_historyLength;
	int bestEvent[MAX_BEST_EVENT] = { 0, 0, 0, 0 };
	int bestEventPrio[MAX_BEST_EVENT] = { 0, 0, 0, 0 };

	for (i = 0; i < MAX_BEST_EVENT; i++)
	{
		bestEventPrio[i] = 0;
		bestEvent[i] = 0;

		float tillTime = (i + 1) * 2 + m_nextCutTime;
		while (tillTime > m_history[nseqMod].time)
		{
			if ((m_history[nseqMod].event.flags & DRC_FLAG_PRIO_MASK) > (unsigned int)bestEventPrio[i]) {
				bestEventPrio[i] = (m_history[nseqMod].event.flags & DRC_FLAG_PRIO_MASK);
				bestEvent[i] = nseqMod;
			}

			if (++nseqMod == m_historyLength) {
				nseqMod = 0;
			}
		}
	}

	if (bestEventPrio[0] || bestEventPrio[1] || bestEventPrio[2])
	{
		if (bestEventPrio[1] >= bestEventPrio[0]
			&& bestEventPrio[1] >= bestEventPrio[2]
			&& bestEventPrio[1] >= bestEventPrio[3]) {
			return &m_history[ bestEvent[1] ];
		}

		else if (bestEventPrio[0] > bestEventPrio[1]
			&& bestEventPrio[0] > bestEventPrio[2]) {
			return &m_history[ bestEvent[0] ];
		}

		else if (bestEventPrio[2] > bestEventPrio[3]) {
			return &m_history[ bestEvent[2] ];
		}

		if (bestEventPrio[0]) {
			return &m_history[ bestEvent[0] ];
		}
	}

	return nullptr;
}

int Director::AddCommand(DirectorCmd *cmd)
{
	if (cmd->GetType() > DRC_CMD_LAST) {
		return 0;
	}

	if (cmd->GetType() == DRC_CMD_EVENT)
	{
		int target1, target2, flags;
		cmd->GetEventData(target1, target2, flags);
		AddEvent(target1, target2, flags);
		return 0;
	}

	DirectorCmd *newcmd = new DirectorCmd;
	newcmd->Copy(cmd);

	m_Commands.Add(newcmd, newcmd->GetTime());
	return 1;
}

IObjectContainer *Director::GetCommands()
{
	return &m_Commands;
}

bool Director::RemoveCommand(int index)
{
	return false;
}

DirectorCmd *Director::GetLastCommand()
{
	return nullptr;
}
