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

ProxyClient::ProxyClient(IProxy *proxy)
{
	m_Proxy = proxy;

	m_ChatEnabled = false;
	m_LastChatTime = 0;
	m_LastCheerTime = 0;
	m_NextDecalTime = 0;
}

void ProxyClient::ShutDown()
{
	char *clientTypeString[] = { "Spectator", "Relay Proxy", "Director", "Commentator", "Fake Client" };
	if (m_State == MODULE_DISCONNECTED) {
		return;
	}

	m_System->Printf("%s disconnected (%s)\n", clientTypeString[m_ClientType], m_ClientChannel.m_remote_address.ToString());
	m_Proxy->GetClients()->Remove(this);

	BaseClient::ShutDown();
	delete this;
}

bool ProxyClient::Init(IBaseSystem *system, int serial, char *name)
{
	BaseClient::Init(system, serial, name);

	m_ClientType = TYPE_CLIENT;
	m_ChatEnabled = true;

	m_LastChatTime = 0;
	m_LastCheerTime = 0;
	m_NextDecalTime = 0;

	return true;
}

ProxyClient::LocalCommandID_s ProxyClient::m_LocalCmdReg[] = {
	{ "cheer",     CMD_ID_CHEER,     &ProxyClient::CMD_Cheer },
	{ "say",       CMD_ID_SAY,       &ProxyClient::CMD_Say },
	{ "joingame",  CMD_ID_JOINGAME,  &ProxyClient::CMD_JoinGame },
	{ "status",    CMD_ID_STATUS,    &ProxyClient::CMD_Status },
	{ "ignoremsg", CMD_ID_IGNOREMSG, &ProxyClient::CMD_IgnoreMsg },
};

bool ProxyClient::ProcessStringCmd(char *string)
{
	if (BaseClient::ProcessStringCmd(string)) {
		return true;
	}

	TokenLine cmdLine;
	if (!cmdLine.SetLine(string)) {
		m_System->Printf("WARNING! ProxyClient::ProcessStringCmd: string command too long.\n");
		return true;
	}

	char *cmd = cmdLine.GetToken(0);
	for (auto& local_cmd : m_LocalCmdReg)
	{
		if (!_stricmp(local_cmd.name, cmd)) {
			(this->*local_cmd.pfnCmd)(&cmdLine);
			return true;
		}
	}

	if (m_ClientType < TYPE_COMMENTATOR) {
		m_System->DPrintf("Unkown client command: \"%s\"\n", cmd);
	}

	m_System->DPrintf("Unkown director command: \"%s\"\n", cmd);
	return false;
}

void ProxyClient::CMD_Cheer(TokenLine *cmd)
{
	if (m_SystemTime <= m_LastCheerTime + 6) {
		return;
	}

	m_LastCheerTime = float(m_SystemTime);
	m_Proxy->IncreaseCheering(1);
}

void ProxyClient::CMD_Say(TokenLine *cmd)
{
	char *chatText = (cmd->CountToken() > 2) ? cmd->GetRestOfLine(1) : cmd->GetToken(1);
	if (m_ClientType == TYPE_COMMENTATOR) {
		m_Proxy->ChatCommentator(nullptr, chatText);
		return;
	}

	if (m_ClientType == TYPE_PROXY && m_Proxy->GetChatMode() == CHAT_GLOBAL) {
		m_Proxy->ChatSpectator("Unknown", chatText);
		return;
	}

	if (m_SystemTime >= m_LastChatTime + 6) {
		m_Proxy->ChatSpectator(m_ClientName, chatText);
		m_LastChatTime = float(m_SystemTime);
		return;
	}
}

void ProxyClient::CMD_JoinGame(TokenLine *cmd)
{
	if (m_Proxy->GetServer()->IsDemoFile()) {
		PrintfToClient("Proxy is replaying demo.\n");
		return;
	}

	if (m_Proxy->IsPublicGame())
	{
		char string[64];
		_snprintf(string, sizeof(string), "connect %s\n", m_World->GetGameServerAddress()->ToString());

		m_ClientChannel.m_reliableStream.WriteByte(svc_stufftext);
		m_ClientChannel.m_reliableStream.WriteString(string);
	}

	PrintfToClient("Joining game is not allowed.\n");
}

void ProxyClient::CMD_Status(TokenLine *cmd)
{
	PrintfToClient("--- HLTV Status ---\n");

	if (m_Proxy->GetServer()->IsDemoFile()) {
		PrintfToClient("Replay demo file %s\n", m_Proxy->GetServer()->GetDemoFileName());
	}
	else if (m_Proxy->IsPublicGame()) {
		PrintfToClient("Game Server: %s\n", m_World->GetGameServerAddress()->ToString());
	}
	else {
		PrintfToClient("Private game server\n");
	}

	int proxies, spectators, slots;
	m_Proxy->GetStatistics(proxies, slots, spectators);
	PrintfToClient("Global HLTV stats: spectators %i, slots %i, proxies %i\n", spectators, slots, proxies);
}

void ProxyClient::CMD_IgnoreMsg(TokenLine *cmd)
{
	if (cmd->CountToken() != 2) {
		return;
	}

	m_ChatEnabled = !atoi(cmd->GetToken(1));
	PrintfToClient("Global HLTV stats: spectators %i, slots %i, proxies %i\n", m_ChatEnabled ? "Spectator chat enabled.\n" : "Spectator chat disabled.\n");
}

void ProxyClient::UpdateUserInfo(char *userinfostring)
{
	BaseClient::UpdateUserInfo(userinfostring);

	if (m_ClientType > TYPE_DEMO) {
		m_System->DPrintf("WARNING! Client::UpdateUserInfo: invalid client ype %i\n", m_ClientType);
		m_ClientType = TYPE_CLIENT;
	}

	if (m_ClientType) {
		return;
	}

	// clamp rate
	if (m_ClientChannel.GetRate() > m_Proxy->GetMaxRate()) {
		m_ClientChannel.SetRate(m_Proxy->GetMaxRate());
	}

	if (m_ClientType == TYPE_CLIENT)
	{
		if (m_ClientChannel.GetUpdateRate() > m_Proxy->GetMaxUpdateRate()) {
			m_ClientChannel.SetUpdateRate(m_Proxy->GetMaxUpdateRate());
		}
	}
}

void ProxyClient::ParseHLTV(NetPacket *packet)
{
	unsigned char cmd = packet->data.ReadByte();
	if (cmd != HLTV_STATUS) {
		m_System->Printf("WARNING! unknown HLTV client msg %i\n", cmd);
		return;
	}

	if (m_ClientType != TYPE_PROXY) {
		m_System->DPrintf("WARNING! HLTV status data from spectator client\n");
		packet->data.SkipBytes(12);
		return;
	}

	m_Proxy->ParseStatusReport(GetAddress(), &packet->data);
}

void ProxyClient::ReplySpawn(int spawncount, int crcMap)
{
	BaseClient::ReplySpawn(spawncount, crcMap);

	if (m_ClientType == TYPE_PROXY) {
		m_VoiceQuery = false;
	}

	m_Proxy->WriteSignonData(m_ClientType, &m_ClientChannel.m_reliableStream);
}

void ProxyClient::SendDatagram()
{
	if (m_Proxy->GetDelay() > 0)
	{
		double worldTime = m_Proxy->GetSpectatorTime();
		double proxyTime = m_Proxy->GetProxyTime();

		frame_t *frame = m_World->GetFrameByTime(worldTime);
		if (!frame) {
			return;
		}

		if (m_ClientChannel.GetIdleTime() > 2) {
			m_ClientChannel.m_unreliableStream.WriteByte(svc_centerprint);
			m_ClientChannel.m_unreliableStream.WriteString((frame->seqnr > 1) ? "Game pending..." : "Buffering game...");
		}

		double time = proxyTime - (worldTime - frame->time);
		WriteDatagram(time, frame);
		return;
	}

	frame_t *frame = m_World->GetLastFrame();
	if (frame)
	{
		if (m_ClientChannel.GetIdleTime() > 2) {
			m_ClientChannel.m_unreliableStream.WriteByte(svc_centerprint);
			m_ClientChannel.m_unreliableStream.WriteString("Game pending...");
		}

		WriteDatagram(frame->time, frame);
	}
}

bool ProxyClient::HasChatEnabled()
{
	return m_ChatEnabled;
}

void ProxyClient::ParseVoiceData(NetPacket *packet)
{
	int nDataLength = packet->data.ReadShort();
	BitBuffer voiceMsg(nDataLength + 8);

	if (m_ClientType != TYPE_COMMENTATOR) {
		packet->data.SkipBytes(nDataLength);
		return;
	}

	if (nDataLength > MAX_VOICEDATA_LEN) {
		Disconnect("Invalid voice message.\n");
		return;
	}

	voiceMsg.WriteByte(svc_voicedata);
	voiceMsg.WriteByte(m_World->GetSlotNumber());
	voiceMsg.WriteShort(nDataLength);
	voiceMsg.WriteBuf(packet->data.CurrentByte(), nDataLength);

	m_Proxy->Broadcast(voiceMsg.GetData(), voiceMsg.CurrentSize(), GROUP_PROXY | GROUP_DEMO | GROUP_UNKNOWN | GROUP_VOICE, false);
	packet->data.SkipBytes(nDataLength);
}

void ProxyClient::DownloadFile(char *fileName)
{
	if (!fileName || !fileName[0])
		return;

	const char szMD5[] = "!MD5";
	if (strstr(fileName, "..") ||
		// ignore customization's
		(strlen(fileName) == 36 && !_strnicmp(fileName, szMD5, sizeof(szMD5) - 1)))
	{
		DownloadFailed(fileName);
		return;
	}

	resource_t *resource = m_Proxy->GetResource(fileName);
	if (resource) {
		m_ClientChannel.CreateFragmentsFromBuffer(resource->data, resource->nDownloadSize, FRAG_FILE_STREAM, fileName);
		return;
	}

	if (m_ClientChannel.CreateFragmentsFromFile(fileName)) {
		m_ClientChannel.FragSend();
		return;
	}

	DownloadFailed(fileName);
}
