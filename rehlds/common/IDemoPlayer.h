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

#include "ref_params.h"

class IWorld;
class IProxy;
class DirectorCmd;
class IBaseSystem;
class ISystemModule;
class IObjectContainer;

class IDemoPlayer {
public:
	virtual ~IDemoPlayer() {}

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

	virtual void NewGame(IWorld *world, IProxy *proxy = nullptr) = 0;
	virtual char *GetModName() = 0;
	virtual void WriteCommands(BitBuffer *stream, float startTime, float endTime) = 0;
	virtual int AddCommand(DirectorCmd *cmd) = 0;
	virtual bool RemoveCommand(int index) = 0;
	virtual DirectorCmd *GetLastCommand() = 0;
	virtual IObjectContainer *GetCommands() = 0;
	virtual void SetWorldTime(double time, bool relative) = 0;
	virtual void SetTimeScale(float scale) = 0;
	virtual void SetPaused(bool state) = 0;
	virtual void SetEditMode(bool state) = 0;
	virtual void SetMasterMode(bool state) = 0;
	virtual bool IsPaused() = 0;
	virtual bool IsLoading() = 0;
	virtual bool IsActive() = 0;
	virtual bool IsEditMode() = 0;
	virtual bool IsMasterMode() = 0;
	virtual void RemoveFrames(double starttime, double endtime) = 0;
	virtual void ExecuteDirectorCmd(DirectorCmd *cmd) = 0;
	virtual double GetWorldTime() = 0;
	virtual double GetStartTime() = 0;
	virtual double GetEndTime() = 0;
	virtual float GetTimeScale() = 0;
	virtual IWorld *GetWorld() = 0;
	virtual char *GetFileName() = 0;
	virtual bool SaveGame(char *filename) = 0;
	virtual bool LoadGame(char *filename) = 0;
	virtual void Stop() = 0;
	virtual void ForceHLTV(bool state) = 0;
	virtual void GetDemoViewInfo(ref_params_t *rp, float *view, int *viewmodel) = 0;
	virtual int ReadDemoMessage(unsigned char *buffer, int size) = 0;
	virtual void ReadNetchanState(int *incoming_sequence, int *incoming_acknowledged, int *incoming_reliable_acknowledged, int *incoming_reliable_sequence, int *outgoing_sequence, int *reliable_sequence, int *last_reliable_sequence) = 0;
};

#define DEMOPLAYER_INTERFACE_VERSION "demoplayer001"
