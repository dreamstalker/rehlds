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

#include <HLTV/IClient.h>
#include <HLTV/IWorld.h>
#include <game_shared/voice_common.h>

#include "InfoString.h"
#include "NetChannel.h"
#include "BaseSystemModule.h"

class TokenLine;
class BaseClient: public IClient, public BaseSystemModule {
public:
	BaseClient() {}
	virtual ~BaseClient() {}

	virtual bool Init(IBaseSystem *system, int serial, char *name);
	virtual void RunFrame(double time);
	virtual char *GetStatusLine();
	virtual char *GetType();
	virtual void ShutDown();
	virtual bool Connect(INetSocket *socket, NetAddress *adr, char *userinfo);
	virtual void Disconnect(const char *reason = nullptr);
	virtual void Reconnect();
	virtual void SetWorld(IWorld *world);
	virtual bool IsHearingVoices();
	virtual bool HasChatEnabled();
	virtual NetAddress *GetAddress();
	virtual InfoString *GetUserInfo();
	virtual int GetClientType();
	virtual char *GetClientName();
	virtual bool IsActive();
	virtual void Send(unsigned char *data, int length, bool isReliable);
	virtual void DownloadFailed(char *fileName);
	virtual void DownloadFile(char *fileName);
	virtual void UpdateVoiceMask(BitBuffer *stream);
	virtual void QueryVoiceEnabled(BitBuffer *stream);
	virtual void SetName(char *newName);
	virtual void WriteSpawn(BitBuffer *stream);
	virtual void WriteDatagram(double time, frame_t *frame);
	virtual void SendDatagram();
	virtual void Reset();

	enum ClientState {
		CLIENT_UNDEFINED = 0,
		CLIENT_INITIALIZING,
		CLIENT_CONNECTING,
		CLIENT_RUNNING,
		CLIENT_DISCONNECTED
	};

	virtual void SetState(ClientState newState);
	virtual void ReplyNew();
	virtual void ReplySpawn(int spawncount, int crcMap);
	virtual void ReplyFullUpdate();
	virtual void PrintfToClient(char *fmt, ...);
	virtual void UpdateUserInfo(char *userinfostring = nullptr);
	virtual void ParseStringCmd(NetPacket *packet);
	virtual void ParseNop(NetPacket *packet);
	virtual void ParseBad(NetPacket *packet);
	virtual void ParseMove(NetPacket *packet);
	virtual void ParseVoiceData(NetPacket *packet);
	virtual void ParseHLTV(NetPacket *packet);
	virtual void ParseDelta(NetPacket *packet);
	virtual void ParseCvarValue(NetPacket *packet);
	virtual void ParseCvarValue2(NetPacket *packet);
	virtual void ProcessMessage(NetPacket *packet);
	virtual bool ProcessStringCmd(char *string);

private:
	enum LocalCommandIDs {
		CMD_ID_NAME = 1,
		CMD_ID_SPAWN,
		CMD_ID_NEW,
		CMD_ID_FULLUPDATE,
		CMD_ID_DROPCLIENT,
		CMD_ID_DLFILE,
		CMD_ID_SETINFO,
		CMD_ID_SHOWINFO,
		CMD_ID_SENDENTS,
		CMD_ID_VMODENABLE,
		CMD_ID_VBAN,
		CMD_ID_SENDERS,
		CMD_ID_PING,
		CMD_ID_SPECTATE,
		CMD_ID_SPK,
		CMD_ID_PAUSE,
		CMD_ID_UNPAUSE,
		CMD_ID_SETPAUSE
	};

	void CMD_Name(TokenLine *cmd);
	void CMD_Spawn(TokenLine *cmd);
	void CMD_New(TokenLine *cmd);
	void CMD_FullUpdate(TokenLine *cmd);
	void CMD_DropClient(TokenLine *cmd);
	void CMD_DownloadFile(TokenLine *cmd);
	void CMD_SetInfo(TokenLine *cmd);
	void CMD_ShowInfo(TokenLine *cmd);
	void CMD_SendEntities(TokenLine *cmd);
	void CMD_VoiceModEnable(TokenLine *cmd);
	void CMD_VoiceBan(TokenLine *cmd);
	void CMD_SendResources(TokenLine *cmd);
	void CMD_RequestPing(TokenLine *cmd);
	void CMD_Spectate(TokenLine *cmd);
	void CMD_Spk(TokenLine *cmd);
	void CMD_Pause(TokenLine *cmd);
	void CMD_UnPause(TokenLine *cmd);
	void CMD_SetPause(TokenLine *cmd);

	struct LocalCommandID_s {
		char *name;
		LocalCommandIDs id;
		void (BaseClient::*pfnCmd)(TokenLine *cmd);
	};
	static LocalCommandID_s m_LocalCmdReg[];

protected:
	struct clc_func_s {
		clc_commands opcode;
		char *pszname;
		void (BaseClient::*func)(NetPacket *packet);
	};
	static clc_func_s m_ClientFuncs[];

	enum { MAX_USER_INFO = 256, MAX_SCOREBOARD_NAME = 32 };

	IWorld *m_World;
	INetSocket *m_Socket;
	NetChannel m_ClientChannel;

	int m_ClientType;
	int m_ClientState;
	char m_ClientName[MAX_SCOREBOARD_NAME];

	InfoString m_Userinfo;
	unsigned int m_LastFrameSeqNr;
	unsigned int m_DeltaFrameSeqNr;
	unsigned int m_ClientDelta;
	unsigned int m_SeqNrMap[256];

	int m_CRC_Value;
	bool m_VoiceEnabled;
	bool m_VoiceQuery;

	CPlayerBitVec m_SentGameRulesMask;
	CPlayerBitVec m_SentBanMask;
	CPlayerBitVec m_BanMask;
};
