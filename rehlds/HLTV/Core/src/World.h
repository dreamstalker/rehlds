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

#include "info.h"				// MAX_INFO_STRING
#include "qlimits.h"			// MAX_PACKET_ENTITIES

#include "Delta.h"
#include "ObjectDictionary.h"
#include "BSPModel.h"

// TODO: move to qlimits.h
#define RESOURCE_INDEX_BITS	12
#define RESOURCE_MAX_COUNT	(1 << RESOURCE_INDEX_BITS)

class World: public IWorld, public BaseSystemModule {
public:
	World() {}
	virtual ~World() {}

	bool Init(IBaseSystem *system, int serial, char *name);
	void RunFrame(double time);
	void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data);
	void ExecuteCommand(int commandID, char *commandLine);
	void RegisterListener(ISystemModule *module);
	void RemoveListener(ISystemModule *module);
	IBaseSystem *GetSystem();
	int GetSerial();
	char *GetStatusLine();
	char *GetType();
	char *GetName();
	int GetState();
	int GetVersion();
	void ShutDown();

	double GetTime();
	NetAddress *GetGameServerAddress();
	char *GetLevelName();
	char *GetGameDir();
	frame_t *GetFrameByTime(double time);
	frame_t *GetFrameBySeqNr(unsigned int seqnr);
	frame_t *GetLastFrame();
	frame_t *GetFirstFrame();
	int GetServerCount();
	int GetSlotNumber();
	int GetMaxClients();
	int GetNumPlayers();
	IBSPModel *GetWorldModel();
	InfoString *GetServerInfoString();
	bool GetPlayerInfoString(int playerNum, InfoString *infoString);
	UserMsg *GetUserMsg(int msgNumber);
	char *GetHostName();
	serverinfo_t *GetServerInfo();

	bool IsPlayerIndex(int index);
	bool IsVoiceEnabled();
	bool IsActive();
	bool IsPaused();
	bool IsComplete();
	bool IsHLTV();
	void Reset();

	void SetServerInfo(int protocol, CRC32_t nserverCRC, unsigned char *nclientdllmd5, int nmaxclients, int nplayernum, int ngametype, char *ngamedir, char *nservername, char *nlevelname);
	void SetServerInfoString(char *infostring);
	void SetServerInfo(serverinfo_t *serverinfo);

	void UpdateServerInfo();
	void SetPaused(bool state);
	void SetTime(double newTime);
	void SetBufferSize(float seconds);
	void SetVoiceEnabled(bool state);
	void SetMoveVars(movevars_t *nmovevars);
	void SetCDInfo(int ncdtrack, int nlooptrack);
	void SetHLTV(bool state);
	void SetExtraInfo(char *nclientfallback, int nallowCheats);
	void SetViewEntity(int nviewentity);
	void SetGameServerAddress(NetAddress *address);
	void SetHostName(char *name);
	void NewGame(int newServerCount);
	void FinishGame();
	bool SaveAsDemo(char *filename, IDirector *director);
	void StopGame();
	int FindUserMsgByName(char *name);
	void ParseDeltaDescription(BitBuffer *stream);
	void ParseBaseline(BitBuffer *stream);
	void ParseEvent(BitBuffer *stream);
	void ParseClientData(BitBuffer *stream, unsigned int deltaSeqNr, BitBuffer *to, clientdata_t *clientData);
	bool GetUncompressedFrame(unsigned int seqNr, frame_t *frame);
	bool UncompressEntitiesFromStream(frame_t *frame, BitBuffer *stream);
	bool UncompressEntitiesFromStream(frame_t *frame, BitBuffer *stream, unsigned int from);
	bool GetClientData(unsigned int SeqNr, clientdata_t *clientData);
	bool GetClientData(frame_t *frame, clientdata_t *clientData);
	int AddFrame(frame_t *newFrame);
	bool AddResource(resource_t *resource);
	void AddLightStyle(int index, char *style);
	bool AddSignonData(unsigned char type, unsigned char *data, int size);
	bool AddUserMessage(int msgNumber, int size, char *name);
	void AddBaselineEntity(int index, entity_state_t *ent);
	void AddInstancedBaselineEntity(int index, entity_state_t *ent);
	void UpdatePlayer(int playerNum, int userId, char *infostring, char *hashedcdkey);

	void WriteFrame(frame_t *frame, unsigned int lastFrameSeqnr, BitBuffer *reliableStream, BitBuffer *unreliableStream, unsigned int deltaSeqNr, unsigned int clientDelta, bool addVoice);
	void WriteNewData(BitBuffer *stream);
	void WriteClientUpdate(BitBuffer *stream, int playerIndex);
	void WriteMovevars(BitBuffer *stream);
	void WriteSigonData(BitBuffer *stream);
	void WriteLightStyles(BitBuffer *stream);

	int RemoveFrames(unsigned int startSeqNr, unsigned int endSeqNr);
	int DuplicateFrames(unsigned int startSeqNr, unsigned int endSeqNr);
	int MoveFrames(unsigned int startSeqNr, unsigned int endSeqNr, double destSeqnr);
	int RevertFrames(unsigned int startSeqNr, unsigned int endSeqNr);

private:
	int CompressFrame(frame_t *from, BitBuffer *stream);
	int ParseDeltaHeader(BitBuffer *stream, bool &remove, bool &custom, int &numbase, bool &newbl, int &newblindex, bool full, int &offset);
	void SetDirector(IDirector *director);
	void SetTimeScale(float scale);
	void SetGameGroupAddress(NetAddress *addr);
	bool SetMaxClients(int max);
	void SetName(char *newName);
	IDirector *GetDirector();
	float GetBufferedGameTime();
	void ConnectionComplete();

	void WriteResources(BitBuffer *stream);
	void WriteDeltaDescriptions(BitBuffer *stream);
	void WriteRegisteredUserMessages(BitBuffer *stream);
	void WriteBaseline(BitBuffer *stream);
	void WriteServerinfo(BitBuffer *stream);
	void WriteCustomDecals(BitBuffer *stream);
	void WritePacketEntities(BitBuffer *stream, frame_t *frame, frame_t *deltaframe);
	bool WriteDeltaEntities(BitBuffer *stream, frame_t *fullFrame, unsigned int deltaSeqNr, unsigned int clientDelta);

	enum WorldState {
		WORLD_UNDEFINED,
		WORLD_INITIALIZING,
		WORLD_DISCONNECTED,
		WORLD_CONNECTING,
		WORLD_RUNNING,
		WORLD_COMPLETE
	};

	void SetState(WorldState newState);
	void ClearUserMessages();
	void ClearServerInfo();
	void ClearResources();
	void ClearInstancedBaseline();
	void ClearBaseline();
	void ClearLightStyles();
	void ClearPlayers();
	void ClearFrames();
	void ClearEntityCache();
	bool GetFrameFromCache(unsigned int seqNr, entity_state_t **entities);
	bool GetUncompressedFrame(frame_t *deltaFrame, frame_t *frame);
	bool GetDeltaFromCache(unsigned int seqNr, unsigned int deltaNr, BitBuffer **buffer);
	void CheckFrameBufferSize();
	void ReorderFrameTimes(float newLastTime);
	int FindBestBaseline(int index, entity_state_t **baseline, entity_state_t *to, int num, bool custom);
	void RearrangeFrame(frame_t *frame, int seqNrOffset, float timeOffset);

	delta_t *GetEventDelta() const;
	delta_t *GetClientDelta() const;
	delta_t *GetEntityDelta() const;
	delta_t *GetWeaponDelta() const;
	delta_t *GetDeltaEncoder(int index, bool custom);
	bool IsDeltaEncoder() const;

protected:
	bool m_IsPaused;
	int m_WorldState;
	int m_Protocol;
	int m_ServerCount;
	CRC32_t m_ServerCRC;
	unsigned char m_ClientdllMD5[16];
	int m_Maxclients;
	int m_PlayerNum;
	int m_GameType;
	char m_GameDir[MAX_PATH];
	char m_LevelName[40];
	char m_ServerName[255];

	int m_ViewEntity;
	serverinfo_t m_DetailedServerInfo;

	enum {
		MAX_ENTITIES            = 1380,
		MAX_INSTANCED_BASELINES = 64,
		MAX_FRAME_CACHE         = 32,
		MAX_SCOREBOARDNAME      = 32,

		MAX_SERVERINFO_STRING   = 512,
		MAX_BUFFER_SIGNONDATA   = 32768
	};

	// Defined in client.h differently
	typedef struct player_info_s
	{
		int userid;							// User id on server

		// User info string
		char userinfo[MAX_INFO_STRING];		// User info string
		char name[MAX_SCOREBOARDNAME];		// Name
		int spectator;						// Spectator or not, unused
		int ping;
		int packet_loss;

		// skin information
		char model[MAX_PATH];
		int topcolor;
		int bottomcolor;

		bool active;
		char hashedcdkey[16];

		int trackerID;

	} player_info_t;

	player_info_t m_Players[MAX_CLIENTS];
	unsigned char m_EntityBuffer[MAX_PACKET_ENTITIES * sizeof(entity_state_t)];

	entity_state_t m_BaseLines[MAX_ENTITIES];
	int m_MaxBaseLines;
	entity_state_t m_Instanced_BaseLines[MAX_INSTANCED_BASELINES];

	int m_MaxInstanced_BaseLine;
	char m_Lightstyles[MAX_LIGHTSTYLES][65];

	movevars_t m_MoveVars;
	BSPModel m_WorldModel;
	InfoString m_ServerInfo;
	UserMsg *m_ClientUserMsgs;
	resource_t *m_ResourcesList;
	int m_ResourcesNum;

	BitBuffer m_SignonData;
	int m_CDTrack;
	int m_LoopTrack;

	ObjectDictionary m_Frames;
	ObjectDictionary m_FramesByTime;
	ObjectDictionary m_CamCommands;

	unsigned int m_SequenceNr;
	float m_MaxBufferLength;

	typedef struct frameCache_s {
		unsigned int seqNr;
		entity_state_t entities[MAX_PACKET_ENTITIES];
	} frameCache_t;

	typedef struct deltaCache_s {
		unsigned int seqNr;
		unsigned int deltaNr;
		BitBuffer buffer;
	} deltaCache_t;

	int m_MaxCacheIndex;
	frameCache_t *m_FrameCache;
	deltaCache_t *m_DeltaCache;
	int m_CacheHits;
	int m_CacheFaults;

	double m_WorldTime;
	double m_StartTime;

	bool m_VoiceEnabled;
	char m_ClientFallback[MAX_PATH];
	bool m_AllowCheats;
	bool m_IsHLTV;
	char m_HostName[255];

	NetAddress m_GameServerAddress;

#ifdef HOOK_HLTV
	static DeltaWrapper m_Delta;
#else
	Delta m_Delta;
#endif // HOOK_HLTV

};

extern char g_DownloadURL[128];
