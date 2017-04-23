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

#include "BaseSystemModule.h"
#include "common/NetAddress.h"

#include "ObjectList.h"
#include "DemoClient.h"
#include "Status.h"
#include "Master.h"
#include "Director.h"
#include "DirectorNull.h"

class IWorld;
class IServer;
class INetwork;
class IDirector;
class IBaseSystem;

#define MAX_NAME				32
#define MAX_PROXY_RATE			20000
#define MAX_PROXY_UPDATERATE	40

#define PROXY_CHALLENGE_LIFE	40.0f
#define PROXY_PRIVATE			0x8000
#define PROXY_VERSION			"1.1.2.0"
#define PROXY_DEFAULT_PORT		"27020"

// TODO: it is necessary to unify with rehlsdk
typedef struct textmessage_s {
	int effect;
	byte r1, g1, b1, a1; // 2 colors for effects
	byte r2, g2, b2, a2;
	float x;
	float y;
	float fadein;
	float fadeout;
	float holdtime;
	float fxtime;
	char text[80];
} textmessage_t;

class Proxy: public IProxy, public BaseSystemModule {
public:
	Proxy() {}
	virtual ~Proxy() {}

	bool Init(IBaseSystem *system, int serial, char *name);
	void RunFrame(double time);
	void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data);
	void ExecuteCommand(int commandID, char *commandLine);
	char *GetStatusLine();
	char *GetType();
	void ShutDown();

	void Reset();
	void Broadcast(byte *data, int length, int groupType, bool isReliable);
	void IncreaseCheering(int votes);
	void ChatCommentator(char *nick, char *text);
	void ChatSpectator(char *nick, char *text);
	void CountLocalClients(int &spectators, int &proxies);
	void ParseStatusMsg(BitBuffer *stream);
	void ParseStatusReport(NetAddress *from, BitBuffer *stream);
	bool ProcessConnectionlessMessage(NetAddress *from, BitBuffer *stream);
	resource_t *AddResource(char *fileName, resourcetype_t type, char *asFileName = nullptr);
	bool IsMaster();
	bool IsLanOnly();
	bool IsActive();
	bool IsPublicGame();
	bool IsPasswordProtected();
	bool IsStressed();
	void SetDelay(float seconds);
	void SetClientTime(double time, bool relative);
	void SetClientTimeScale(float scale);
	void SetMaxRate(int rate);
	void SetMaxLoss(float maxloss);
	void SetMaxUpdateRate(int updaterate);
	bool SetMaxClients(int number);
	void SetRegion(unsigned char region);
	float GetDelay();
	double GetSpectatorTime();
	double GetProxyTime();
	IObjectContainer *GetClients();
	IWorld *GetWorld();
	IServer *GetServer();
	IDirector *GetDirector();
	INetSocket *GetSocket();
	ChatMode_e GetChatMode();
	void GetStatistics(int &proxies, int &slots, int &spectators);
	int GetMaxRate();
	int GetMaxClients();
	int GetMaxUpdateRate();
	resource_t *GetResource(char *fileName);
	int GetDispatchMode();
	unsigned char GetRegion();
	bool WriteSignonData(int type, BitBuffer *stream);

	void ReconnectClients();
	void ExecuteRcon(NetAddress *from, char *command);
	void SendRcon(NetAddress *to, unsigned int challenge);
	void ReplyServiceChallenge(NetAddress *to, char *type);
	void ReplyListen(NetAddress *to);
	void RejectConnection(NetAddress *adr, bool badPassword, const char *fmt, ...);
	void ReplyConnect(NetAddress *to, int protocol, int challenge, char *protinfo, char *userinfo);
	void ReplyRules(NetAddress *to);
	void ReplyPlayers(NetAddress *to);
	void ReplyInfo(NetAddress *to, bool detailed);
	void ReplyInfoString(NetAddress *to);
	void ReplyChallenge(NetAddress *to);
	void ReplyPing(NetAddress *to);

	void UpdateStatusLine();
	void DispatchClient(NetAddress *client, NetAddress *proxy);
	bool IsValidPassword(int type, char *pw);
	void WriteHUDMsg(textmessage_t *msg, BitBuffer *stream);

	typedef struct loopcmd_s {
		int id;
		float interval;
		float lastTime;
		char command[255];
	} loopcmd_t;

	void ExecuteLoopCommands();
	unsigned int GetChallengeNumber(NetAddress *host);
	bool CheckChallenge(NetAddress *from, unsigned int challengeNumber);
	void CreateServerInfoString(InfoString *info);
	bool CheckDirectorModule();
	void RunClocks();
	void NewGameStarted();
	void NewServerConnection();
	void BroadcastPaused(bool paused);
	void BroadcastRetryMessage();
	void StopBroadcast(const char *message);
	char *GetModVersion(char *gamedir);
	void DisconnectClients(const char *reason);
	void FreeResource(resource_t *resource);
	void ClearResources();
	resource_t *LoadResourceFromFile(char *fileName, resourcetype_t type);
	bool IsBanned(NetAddress *adr);
	void UpdateInfoMessages();
	void SetName(char *newName);

private:
	enum LocalCommandIDs {
		CMD_ID_RCON = 1,
		CMD_ID_RCONPASSWORD,
		CMD_ID_RCONADDRESS,
		CMD_ID_SAY,
		CMD_ID_MSG,
		CMD_ID_CLIENTS,
		CMD_ID_KICK,
		CMD_ID_CHATMODE,
		CMD_ID_PUBLICGAME,
		CMD_ID_OFFLINETEXT,
		CMD_ID_ADMINPASSWORD,
		CMD_ID_SIGNONCOMMANDS,
		CMD_ID_SPECTATORPASSWORD,
		CMD_ID_DISPATCHMODE,
		CMD_ID_CHEERINGTHRESHOLD,
		CMD_ID_INFORMPLAYERS,
		CMD_ID_PING,
		CMD_ID_PROXYPASSWORD,
		CMD_ID_MAXRATE,
		CMD_ID_LOOPCMD,
		CMD_ID_MAXCLIENTS,
		CMD_ID_DISCONNECT,
		CMD_ID_LOCALMSG,
		CMD_ID_CONNECT,
		CMD_ID_PLAYDEMO,
		CMD_ID_DELAY,
		CMD_ID_STOP,
		CMD_ID_RECORD,
		CMD_ID_STOPRECORDING,
		CMD_ID_SERVERCMD,
		CMD_ID_CLIENTCMD,
		CMD_ID_BLOCKVOICE,
		CMD_ID_NAME,
		CMD_ID_UPDATERATE,
		CMD_ID_RATE,
		CMD_ID_ADDRESOURCE,
		CMD_ID_RESOURCES,
		CMD_ID_BANNERFILE,
		CMD_ID_BANN,
		CMD_ID_ADDFAKECLIENTS,
		CMD_ID_RETRY,
		CMD_ID_PLAYERS,
		CMD_ID_AUTORETRY,
		CMD_ID_SERVERPASSWORD,
		CMD_ID_STATUS,
		CMD_ID_HOSTNAME,
		CMD_ID_MAXQUERIES,
		CMD_ID_CLEARBANNS,
		CMD_ID_MAXLOSS,
		CMD_ID_PROTOCOL,
		CMD_ID_REGION
	};

	void CMD_Rcon(char *cmdLine);
	void CMD_ServerCmd(char *cmdLine);
	void CMD_ClientCmd(char *cmdLine);
	void CMD_RconPassword(char *cmdLine);
	void CMD_RconAddress(char *cmdLine);
	void CMD_Say(char *cmdLine);
	void CMD_Msg(char *cmdLine);
	void CMD_Clients(char *cmdLine);
	void CMD_Kick(char *cmdLine);
	void CMD_ChatMode(char *cmdLine);
	void CMD_PublicGame(char *cmdLine);
	void CMD_OffLineText(char *cmdLine);
	void CMD_AdminPassword(char *cmdLine);
	void CMD_SignOnCommands(char *cmdLine);
	void CMD_SpectatorPassword(char *cmdLine);
	void CMD_DispatchMode(char *cmdLine);
	void CMD_CheeringThreshold(char *cmdLine);
	void CMD_InformPlayers(char *cmdLine);
	void CMD_Ping(char *cmdLine);
	void CMD_ProxyPassword(char *cmdLine);
	void CMD_MaxRate(char *cmdLine);
	void CMD_MaxUpdateRate(char *cmdLine);
	void CMD_LoopCmd(char *cmdLine);
	void CMD_MaxClients(char *cmdLine);
	void CMD_LocalMsg(char *cmdLine);
	void CMD_Connect(char *cmdLine);
	void CMD_Disconnect(char *cmdLine);
	void CMD_PlayDemo(char *cmdLine);
	void CMD_Delay(char *cmdLine);
	void CMD_Stop(char *cmdLine);
	void CMD_Record(char *cmdLine);
	void CMD_StopRecording(char *cmdLine);
	void CMD_BlockVoice(char *cmdLine);
	void CMD_Name(char *cmdLine);
	void CMD_Rate(char *cmdLine);
	void CMD_Updaterate(char *cmdLine);
	void CMD_HostName(char *cmdLine);
	void CMD_AddResource(char *cmdLine);
	void CMD_Resources(char *cmdLine);
	void CMD_BannerFile(char *cmdLine);
	void CMD_Bann(char *cmdLine);
	void CMD_AddFakeClients(char *cmdLine);
	void CMD_Retry(char *cmdLine);
	void CMD_AutoRetry(char *cmdLine);
	void CMD_ServerPassword(char *cmdLine);
	void CMD_Status(char *cmdLine);
	void CMD_MaxQueries(char *cmdLine);
	void CMD_Players(char *cmdLine);
	void CMD_ClearBanns(char *cmdLine);
	void CMD_MaxLoss(char *cmdLine);
	void CMD_Protocol(char *cmdLine);
	void CMD_Region(char *cmdLine);

	struct LocalCommandID_s {
		char *name;
		LocalCommandIDs id;
		void (Proxy::*pfnCmd)(char *cmdLine);
	};
	static LocalCommandID_s m_LocalCmdReg[];
	const char *GetDescription();

protected:
	friend class DemoClient;
	friend class Status;
	friend class Master;
	friend class Director;

	INetwork *m_Network;
	IWorld *m_World;
	IDirector *m_Director;
	IServer *m_Server;
	ObjectList m_Clients;

	DemoClient m_DemoClient;
	Status m_Status;
	Master m_Master;
	Director m_DefaultDirector;
	DirectorNull m_NullDirector;
	INetSocket *m_Socket;

	typedef struct challenge_s {
		NetAddress adr;
		unsigned int challenge;
		float time;
	} challenge_t;

	// MAX_CHALLENGES is made large to prevent a denial
	// of service attack that could cycle all of them
	// out before legitimate users connected
	enum { MAX_CHALLENGES = 1024 };
	challenge_t m_Challenges[MAX_CHALLENGES];

	bool m_IsMaster;
	bool m_IsFinishingBroadcast;
	bool m_IsReconnectRequested;

	int m_MaxRate;
	int m_MaxUpdateRate;

	ChatMode_e m_ChatMode;
	int m_MaxClients;
	float m_MaxQueries;
	int m_MaxFrameQueries;
	int m_MaxSeenClients;

	NetAddress m_RconAddress;
	char m_RconPassword[128];
	char m_AdminPassword[128];
	char m_ProxyPassword[128];
	char m_SpectatorPassword[128];
	char m_LastRconCommand[1024];
	char m_OffLineText[128];
	char m_SignonCommands[256];

	ObjectList m_LoopCommands;
	bool m_PublicGame;

	enum DispatchMode_e : int {
		DISPATCH_OFF,       // Won't redirect any clients.
		DISPATCH_BALANCE,   // Will redirect connecting clients to other proxies balancing work load between all proxies.
		DISPATCH_ALL,       // Any spectator clients will be redirected, so this proxy serves only as dispatcher.
	};
	DispatchMode_e m_DispatchMode;

	float m_ClientDelay;
	float m_ClientTimeScale;
	double m_ClientWorldTime;
	double m_ClientProxyTime;

	double m_LastClockUpdateTime;
	float m_CheeringThreshold;
	float m_LastCheeringUpdate;
	int m_CheeringPlayers;
	resource_t *m_BannerTGA;
	ObjectList m_Resources;
	double m_NextStatusUpdateTime;
	float m_FPS;
	float m_MaxLoss;
	float m_CurrentLoss;
	ObjectList m_BannList;
	unsigned char m_Region;

	textmessage_t m_LocalMessage;
	textmessage_t m_CommentatorMessage;

	double m_NextInfoMessagesUpdate;
	BitBuffer m_InfoRules;
	BitBuffer m_InfoPlayers;
	BitBuffer m_InfoDetails;
	BitBuffer m_InfoInfo;
	BitBuffer m_InfoString;
};
