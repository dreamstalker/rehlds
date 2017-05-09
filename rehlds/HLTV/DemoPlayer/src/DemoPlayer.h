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
#include "IEngineWrapper.h"

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

	void NewGame(IWorld *world, IProxy *proxy = nullptr);
	char *GetModName();
	void WriteCommands(BitBuffer *stream, float startTime, float endTime);
	int AddCommand(DirectorCmd *cmd);
	bool RemoveCommand(int index);
	DirectorCmd *GetLastCommand();
	IObjectContainer *GetCommands();
	IDirector *GetDirector();
	void SetWorldTime(double time, bool relative);
	void SetTimeScale(float scale);
	void SetPaused(bool state);
	void SetEditMode(bool state);
	void SetMasterMode(bool state);
	bool IsPaused();
	bool IsLoading();
	bool IsActive();
	bool IsEditMode();
	bool IsMasterMode();
	void RemoveFrames(double starttime, double endtime);
	void ExecuteDirectorCmd(DirectorCmd *cmd);
	double GetWorldTime();
	double GetStartTime();
	double GetEndTime();
	float GetTimeScale();
	IWorld *GetWorld();
	char *GetFileName();
	bool SaveGame(char *filename);
	bool LoadGame(char *filename);
	void Stop();
	void ForceHLTV(bool state);
	void GetDemoViewInfo(ref_params_t *rp, float *view, int *viewmodel);
	int ReadDemoMessage(unsigned char *buffer, int size);
	void ReadNetchanState(int *incoming_sequence, int *incoming_acknowledged, int *incoming_reliable_acknowledged, int *incoming_reliable_sequence, int *outgoing_sequence, int *reliable_sequence, int *last_reliable_sequence);
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

#ifndef HOOK_HLTV
// Use this to expose a singleton interface. This creates the global variable for you automatically.
#define EXPOSE_SINGLE_INTERFACE2(className, interfaceName, versionName) \
	static className __g_##className##_singleton;\
	static IBaseInterface *__Create##className##interfaceName##_interface() {return (IBaseInterface *)((interfaceName *)&__g_##className##_singleton);}\
	static InterfaceReg __g_Create##className##interfaceName##_reg(__Create##className##interfaceName##_interface, versionName);

EXPOSE_SINGLE_INTERFACE2(DemoPlayer, IDemoPlayer, DEMOPLAYER_INTERFACE_VERSION);
#endif // HOOK_HLTV
