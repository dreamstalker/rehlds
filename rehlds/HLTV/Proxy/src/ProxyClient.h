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

#include "common/BaseClient.h"

class IProxy;
class NetPacket;
class IBaseSystem;

class ProxyClient: public BaseClient {
public:
	ProxyClient(IProxy *proxy = nullptr);
	virtual ~ProxyClient() {}

	bool Init(IBaseSystem *system, int serial, char *name);
	void ShutDown();
	bool HasChatEnabled();
	bool ProcessStringCmd(char *string);
	void ReplySpawn(int spawncount, int crcMap);
	void UpdateUserInfo(char *userinfostring = nullptr);
	void ParseVoiceData(NetPacket *packet);
	void DownloadFile(char *fileName);
	void SendDatagram();
	void ParseHLTV(NetPacket *packet);

private:
	enum LocalCommandIDs {
		CMD_ID_CHEER = 1,
		CMD_ID_SAY,
		CMD_ID_JOINGAME,
		CMD_ID_STATUS,
		CMD_ID_IGNOREMSG
	};

	void CMD_Cheer(TokenLine *cmd);
	void CMD_Say(TokenLine *cmd);
	void CMD_JoinGame(TokenLine *cmd);
	void CMD_Status(TokenLine *cmd);
	void CMD_IgnoreMsg(TokenLine *cmd);

	struct LocalCommandID_s {
		char *name;
		LocalCommandIDs id;
		void (ProxyClient::*pfnCmd)(TokenLine *cmd);
	};
	static LocalCommandID_s m_LocalCmdReg[];

protected:
	IProxy *m_Proxy;

	float m_LastChatTime;
	float m_LastCheerTime;
	float m_NextDecalTime;
	bool m_ChatEnabled;
};
