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

#include "ObjectList.h"
#include "TokenLine.h"
#include "BaseSystemModule.h"
#include "IEngineWrapper.h"

class EngineWrapper: public IEngineWrapper, public BaseSystemModule {
public:
	bool Init(IBaseSystem *system, int serial, char *name) { return BaseSystemModule::Init(system, serial, name); }

	void RunFrame(double time)                                                 { BaseSystemModule::RunFrame(time); }
	void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data) { BaseSystemModule::ReceiveSignal(module, signal, data); }
	void ExecuteCommand(int commandID, char *commandLine)                      { BaseSystemModule::ExecuteCommand(commandID, commandLine); }
	void RegisterListener(ISystemModule *module)                               { BaseSystemModule::RegisterListener(module); }
	void RemoveListener(ISystemModule *module)                                 { BaseSystemModule::RemoveListener(module); }

	IBaseSystem *GetSystem() { return BaseSystemModule::GetSystem(); }
	int GetSerial()          { return BaseSystemModule::GetSerial(); }
	char *GetStatusLine();
	char *GetType();
	char *GetName()          { return BaseSystemModule::GetName(); }
	int GetState()           { return BaseSystemModule::GetState(); }
	int GetVersion()         { return BaseSystemModule::GetVersion(); }
	void ShutDown()          { BaseSystemModule::ShutDown(); }

	bool GetViewOrigin(float *origin);
	bool GetViewAngles(float *angles);
	int GetTraceEntity();
	float GetCvarFloat(char *szName);
	char *GetCvarString(char *szName);
	void SetCvar(char *szName, char *szValue);
	void Cbuf_AddText(char *text);
	void DemoUpdateClientData(client_data_t *cdat);
	void CL_QueueEvent(int flags, int index, float delay, event_args_t *pargs);
	void HudWeaponAnim(int iAnim, int body);
	void CL_DemoPlaySound(int channel, char* sample, float attenuation, float volume, int flags, int pitch);
	void ClientDLL_ReadDemoBuffer(int size, unsigned char *buffer);
};

class Panel;
class SystemWrapper: public IBaseSystem, public BaseSystemModule {
public:
	EXT_FUNC bool Init(IBaseSystem *system, int serial, char *name);
	EXT_FUNC void RunFrame(double time);
	EXT_FUNC void ExecuteCommand(int commandID, char *commandLine);
	EXT_FUNC char *GetStatusLine();
	EXT_FUNC char *GetType();
	EXT_FUNC void ShutDown();

	EXT_FUNC double GetTime();
	EXT_FUNC unsigned int GetTick();
	EXT_FUNC void SetFPS(float fps) {}
	EXT_FUNC void Printf(char *fmt, ...);
	EXT_FUNC void DPrintf(char *fmt, ...);
	EXT_FUNC void RedirectOutput(char *buffer, int maxSize);
	EXT_FUNC IFileSystem *GetFileSystem();
	EXT_FUNC unsigned char *LoadFile(const char *name, int *length);
	EXT_FUNC void FreeFile(unsigned char *fileHandle);
	EXT_FUNC void SetTitle(char *pszTitle);
	EXT_FUNC void SetStatusLine(char *pszStatus);
	EXT_FUNC void ShowConsole(bool visible) {}
	EXT_FUNC void LogConsole(char *filename);
	EXT_FUNC bool InitVGUI(IVGuiModule *module);
	EXT_FUNC Panel *GetPanel();
	EXT_FUNC bool RegisterCommand(char *name, ISystemModule *module, int commandID);
	EXT_FUNC void GetCommandMatches(char *string, ObjectList *pMatchList);
	EXT_FUNC void ExecuteString(const char *commands);
	EXT_FUNC void ExecuteFile(char *filename);
	EXT_FUNC void Errorf(char *fmt, ...);
	EXT_FUNC char *CheckParam(char *param);
	EXT_FUNC bool AddModule(ISystemModule *module, char *name);
	EXT_FUNC ISystemModule *GetModule(char *interfacename, char *library, char *instancename = nullptr);
	EXT_FUNC bool RemoveModule(ISystemModule *module);
	EXT_FUNC void Stop();
	EXT_FUNC char *GetBaseDir() { return BaseSystemModule::GetBaseDir(); }

protected:
	struct command_t {
		char name[32];
		int commandID;
		ISystemModule *module;
	};

	struct library_t {
		char name[MAX_PATH];
		CSysModule *handle;
		CreateInterfaceFn createInterfaceFn;
	};

	library_t *GetLibrary(char *name);
	ISystemModule *FindModule(char *type, char *name = nullptr);
	bool DispatchCommand(char *command);

	enum LocalCommandIDs {
		CMD_ID_MODULES = 1,
		CMD_ID_LOADMODULE,
		CMD_ID_UNLOADMODULE,
	};

	void CMD_Modules(char *cmdLine);
	void CMD_UnloadModule(char *cmdLine);
	void CMD_LoadModule(char *cmdLine);

private:
	ObjectList m_Modules;
	ObjectList m_Libraries;
	ObjectList m_Commands;
	unsigned int m_SerialCounter;
	unsigned int m_Tick;
	double m_LastTime;
	IEngineWrapper *m_EngineWrapper;
};

extern SystemWrapper gSystemWrapper;

void SystemWrapper_Init();
void SystemWrapper_ShutDown();
void SystemWrapper_RunFrame(double time);
BOOL SystemWrapper_LoadModule(char *interfacename, char *library, char *instancename = nullptr);
void SystemWrapper_ExecuteString(const char *command);
void SystemWrapper_CommandForwarder();

int COM_BuildNumber();
void COM_RemoveEvilChars(char *string);
