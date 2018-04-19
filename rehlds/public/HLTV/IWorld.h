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

#include "IBSPModel.h"
#include "IDirector.h"
#include "ISystemModule.h"
#include "common/ServerInfo.h"

#include "pm_movevars.h"
#include "usermsg.h"
#include "entity_state.h"

typedef struct frame_s
{
	float time;
	unsigned int seqnr;
	unsigned char *data;
	void *entities;
	unsigned int entitiesSize;
	unsigned int entitynum;
	void *clientData;
	unsigned int clientDataSize;
	unsigned char *events;
	unsigned int eventsSize;
	unsigned int eventnum;
	unsigned char *reliableData;
	unsigned int reliableDataSize;
	unsigned char *unreliableData;
	unsigned int unreliableDataSize;
	unsigned char *userMessages;
	unsigned int userMessagesSize;
	unsigned char *voiceData;
	unsigned int voiceDataSize;
	unsigned char *demoData;
	unsigned int demoDataSize;
	void *demoInfo;
	unsigned int delta;
} frame_t;

class InfoString;
class NetAddress;

class IWorld {
public:
	virtual ~IWorld() {}

	virtual bool Init(IBaseSystem *system, int serial, char *name) = 0;
	virtual void RunFrame(double time) = 0;
	virtual void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data) = 0;
	virtual void ExecuteCommand(int commandID, char *commandLine) = 0;
	virtual void RegisterListener(ISystemModule *module) = 0;
	virtual void RemoveListener(ISystemModule *module) = 0;
	virtual IBaseSystem *GetSystem() = 0;
	virtual int GetSerial() = 0;
	virtual char *GetStatusLine() = 0;
	virtual char *GetType() = 0;
	virtual char *GetName() = 0;
	virtual int GetState() = 0;
	virtual int GetVersion() = 0;
	virtual void ShutDown() = 0;

	virtual double GetTime() = 0;
	virtual NetAddress *GetGameServerAddress() = 0;
	virtual char *GetLevelName() = 0;
	virtual char *GetGameDir() = 0;
	virtual frame_t *GetFrameByTime(double time) = 0;
	virtual frame_t *GetFrameBySeqNr(unsigned int seqnr) = 0;
	virtual frame_t *GetLastFrame() = 0;
	virtual frame_t *GetFirstFrame() = 0;
	virtual int GetServerCount() = 0;
	virtual int GetSlotNumber() = 0;
	virtual int GetMaxClients() = 0;
	virtual int GetNumPlayers() = 0;
	virtual IBSPModel *GetWorldModel() = 0;
	virtual InfoString *GetServerInfoString() = 0;
	virtual bool GetPlayerInfoString(int playerNum, InfoString *infoString) = 0;
	virtual UserMsg *GetUserMsg(int msgNumber) = 0;
	virtual char *GetHostName() = 0;
	virtual serverinfo_t *GetServerInfo() = 0;
	virtual bool IsPlayerIndex(int index) = 0;
	virtual bool IsVoiceEnabled() = 0;
	virtual bool IsActive() = 0;
	virtual bool IsPaused() = 0;
	virtual bool IsComplete() = 0;
	virtual bool IsHLTV() = 0;
	virtual void Reset() = 0;
	virtual void SetServerInfo(int protocol, CRC32_t nserverCRC, byte *nclientdllmd5, int nmaxclients, int nplayernum, int ngametype, char *ngamedir, char *nservername, char *nlevelname) = 0;
	virtual void SetServerInfoString(char *infostring) = 0;
	virtual void SetServerInfo(serverinfo_t *serverinfo) = 0;
	virtual void UpdateServerInfo() = 0;
	virtual void SetPaused(bool state) = 0;
	virtual void SetTime(double newTime) = 0;
	virtual void SetBufferSize(float seconds) = 0;
	virtual void SetVoiceEnabled(bool state) = 0;
	virtual void SetMoveVars(movevars_t *nmovevars) = 0;
	virtual void SetCDInfo(int ncdtrack, int nlooptrack) = 0;
	virtual void SetHLTV(bool state) = 0;
	virtual void SetExtraInfo(char *nclientfallback, int nallowCheats) = 0;
	virtual void SetViewEntity(int nviewentity) = 0;
	virtual void SetGameServerAddress(NetAddress *address) = 0;
	virtual void SetHostName(char *name) = 0;
	virtual void NewGame(int newServerCount) = 0;
	virtual void FinishGame() = 0;
	virtual bool SaveAsDemo(char *filename, IDirector *director) = 0;
	virtual void StopGame() = 0;
	virtual int FindUserMsgByName(char *name) = 0;
	virtual void ParseDeltaDescription(BitBuffer *stream) = 0;
	virtual void ParseBaseline(BitBuffer *stream) = 0;
	virtual void ParseEvent(BitBuffer *stream) = 0;
	virtual void ParseClientData(BitBuffer *stream, unsigned int deltaSeqNr, BitBuffer *to, clientdata_t *clientData) = 0;
	virtual bool GetUncompressedFrame(unsigned int seqNr, frame_t *frame) = 0;
	virtual bool UncompressEntitiesFromStream(frame_t *frame, BitBuffer *stream) = 0;
	virtual bool UncompressEntitiesFromStream(frame_t *frame, BitBuffer *stream, unsigned int from) = 0;
	virtual bool GetClientData(unsigned int SeqNr, clientdata_t *clientData) = 0;
	virtual bool GetClientData(frame_t *frame, clientdata_t *clientData) = 0;
	virtual int AddFrame(frame_t *newFrame) = 0;
	virtual bool AddResource(resource_t *resource) = 0;
	virtual void AddLightStyle(int index, char *style) = 0;
	virtual bool AddSignonData(unsigned char type, unsigned char *data, int size) = 0;
	virtual bool AddUserMessage(int msgNumber, int size, char *name) = 0;
	virtual void AddBaselineEntity(int index, entity_state_t *ent) = 0;
	virtual void AddInstancedBaselineEntity(int index, entity_state_t *ent) = 0;
	virtual void UpdatePlayer(int playerNum, int userId, char *infostring, char *hashedcdkey) = 0;
	virtual void WriteFrame(frame_t *frame, unsigned int lastFrameSeqnr, BitBuffer *reliableStream, BitBuffer *unreliableStream, unsigned int deltaSeqNr, unsigned int clientDelta, bool addVoice) = 0;
	virtual void WriteNewData(BitBuffer *stream) = 0;
	virtual void WriteClientUpdate(BitBuffer *stream, int playerIndex) = 0;
	virtual void WriteMovevars(BitBuffer *stream) = 0;
	virtual void WriteSigonData(BitBuffer *stream) = 0;
	virtual void WriteLightStyles(BitBuffer *stream) = 0;
	virtual int RemoveFrames(unsigned int startSeqNr, unsigned int endSeqNr) = 0;
	virtual int DuplicateFrames(unsigned int startSeqNr, unsigned int endSeqNr) = 0;
	virtual int MoveFrames(unsigned int startSeqNr, unsigned int endSeqNr, double destSeqnr) = 0;
	virtual int RevertFrames(unsigned int startSeqNr, unsigned int endSeqNr) = 0;
};

#define WORLD_INTERFACE_VERSION "world001"
