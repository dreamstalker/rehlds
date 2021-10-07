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

#include <HLTV/IWorld.h>
#include <HLTV/IServer.h>
#include <HLTV/IDirector.h>

#include "vmodes.h"
#include "cdll_int.h"
#include "IDemoPlayer.h"
#include "IEngineWrapperEx.h"

#include "BaseSystemModule.h"
#include "ObjectDictionary.h"

class IProxy;
class DirectorCmd;
class IBaseSystem;
class IObjectContainer;

class DemoPlayer: public IDemoPlayer, public BaseSystemModule, public IDirector {
public:
	DemoPlayer();
	virtual ~DemoPlayer() {}

	EXT_FUNC bool Init(IBaseSystem *system, int serial, char *name);
	EXT_FUNC void RunFrame(double time);
	EXT_FUNC void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data);
	EXT_FUNC void ExecuteCommand(int commandID, char *commandLine);
	EXT_FUNC void RegisterListener(ISystemModule *module);
	EXT_FUNC void RemoveListener(ISystemModule *module);
	EXT_FUNC IBaseSystem *GetSystem();
	EXT_FUNC int GetSerial();
	EXT_FUNC char *GetStatusLine();
	EXT_FUNC char *GetType();
	EXT_FUNC char *GetName();
	EXT_FUNC int GetState();
	EXT_FUNC int GetVersion();
	EXT_FUNC void ShutDown();

	EXT_FUNC void NewGame(IWorld *world, IProxy *proxy = nullptr);
	EXT_FUNC char *GetModName();
	EXT_FUNC void WriteCommands(BitBuffer *stream, float startTime, float endTime);
	EXT_FUNC int AddCommand(DirectorCmd *cmd);
	EXT_FUNC bool RemoveCommand(int index);
	EXT_FUNC DirectorCmd *GetLastCommand();
	EXT_FUNC IObjectContainer *GetCommands();
	IDirector *GetDirector();
	EXT_FUNC void SetWorldTime(double time, bool relative);
	EXT_FUNC void SetTimeScale(float scale);
	EXT_FUNC void SetPaused(bool state);
	EXT_FUNC void SetEditMode(bool state);
	EXT_FUNC void SetMasterMode(bool state);
	EXT_FUNC bool IsPaused();
	EXT_FUNC bool IsLoading();
	EXT_FUNC bool IsActive();
	EXT_FUNC bool IsEditMode();
	EXT_FUNC bool IsMasterMode();
	EXT_FUNC void RemoveFrames(double starttime, double endtime);
	EXT_FUNC void ExecuteDirectorCmd(DirectorCmd *cmd);
	EXT_FUNC double GetWorldTime();
	EXT_FUNC double GetStartTime();
	EXT_FUNC double GetEndTime();
	EXT_FUNC float GetTimeScale();
	EXT_FUNC IWorld *GetWorld();
	EXT_FUNC char *GetFileName();
	EXT_FUNC bool SaveGame(char *filename);
	EXT_FUNC bool LoadGame(char *filename);
	EXT_FUNC void Stop();
	EXT_FUNC void ForceHLTV(bool state);
	EXT_FUNC void GetDemoViewInfo(ref_params_t *rp, float *view, int *viewmodel);
	EXT_FUNC int ReadDemoMessage(unsigned char *buffer, int size);
	EXT_FUNC void ReadNetchanState(int *incoming_sequence, int *incoming_acknowledged, int *incoming_reliable_acknowledged, int *incoming_reliable_sequence, int *outgoing_sequence, int *reliable_sequence, int *last_reliable_sequence);
	void SetName(char *newName);

private:
	enum LocalCommandIDs {
		CMD_ID_JUMP = 1,
		CMD_ID_FORCEHLTV,
		CMD_ID_PAUSE,
		CMD_ID_SPEED,
		CMD_ID_START,
		CMD_ID_SAVE,
	};

	void CMD_Jump(char *cmdLine);
	void CMD_ForceHLTV(char *cmdLine);
	void CMD_Pause(char *cmdLine);
	void CMD_Speed(char *cmdLine);
	void CMD_Start(char *cmdLine);
	void CMD_Save(char *cmdLine);

	struct LocalCommandID_s {
		char *name;
		LocalCommandIDs id;
		void (DemoPlayer::*pfnCmd)(char *cmdLine);
	};
	static LocalCommandID_s m_LocalCmdReg[];

protected:
	double GetPlayerTime();
	char *FormatTime(float time);

	void RunClocks();
	void WriteDatagram(BitBuffer *stream);
	void WriteSpawn(BitBuffer *stream);
	void ReindexCommands();
	void WriteCameraPath(DirectorCmd *, BitBuffer *stream);
	void ExecuteDemoFileCommands(BitBuffer *stream);

private:
	IEngineWrapper *m_Engine;
	IWorld *m_World;
	IServer *m_Server;
	ObjectDictionary m_Commands;
	DirectorCmd *m_LastCmd;
	unsigned int m_CommandCounter;
	char m_DemoFileName[MAX_PATH];

	enum PlayerState {
		DEMOPLAYER_UNDEFINED,
		DEMOPLAYER_INITIALIZING,
		DEMOPLAYER_CONNECTING,
		DEMOPLAYER_CONNECTED,
		DEMOPLAYER_RUNNING
	};

	int m_PlayerState;
	BitBuffer m_DemoStream;
	bool m_EditorMode;
	bool m_MasterMode;
	bool m_ForceHLTV;
	bool m_IsSaving;
	float m_TimeScale;
	double m_WorldTime;
	double m_PlayerTime;
	double m_StartTime;

	double m_LastFrameTime;
	bool m_IsPaused;
	double m_LastClockUpdateTime;

	unsigned int m_LastFrameSeqNr;
	unsigned int m_DeltaFrameSeqNr;
	unsigned int m_Outgoing_sequence;
};

// Use this to expose a singleton interface. This creates the global variable for you automatically.
#define EXPOSE_SINGLE_INTERFACE2(className, interfaceName, versionName) \
	static className __g_##className##_singleton;\
	static IBaseInterface *__Create##className##interfaceName##_interface() {return (IBaseInterface *)((interfaceName *)&__g_##className##_singleton);}\
	static InterfaceReg __g_Create##className##interfaceName##_reg(__Create##className##interfaceName##_interface, versionName);

EXPOSE_SINGLE_INTERFACE2(DemoPlayer, IDemoPlayer, DEMOPLAYER_INTERFACE_VERSION);
