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

#include "event_args.h"
#include "cdll_int.h"

class IBaseSystem;
class IEngineWrapper {
public:
	virtual ~IEngineWrapper() {}

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

	virtual bool GetViewOrigin(float *origin) = 0;
	virtual bool GetViewAngles(float *angles) = 0;
	virtual int GetTraceEntity() = 0;
	virtual float GetCvarFloat(char *szName) = 0;
	virtual char *GetCvarString(char *szName) = 0;
	virtual void SetCvar(char *szName, char *szValue) = 0;
	virtual void Cbuf_AddText(char *text) = 0;
	virtual void DemoUpdateClientData(client_data_t *cdat) = 0;
	virtual void CL_QueueEvent(int flags, int index, float delay, event_args_t *pargs) = 0;
	virtual void HudWeaponAnim(int iAnim, int body) = 0;
	virtual void CL_DemoPlaySound(int channel, char* sample, float attenuation, float volume, int flags, int pitch) = 0;
	virtual void ClientDLL_ReadDemoBuffer(int size, unsigned char *buffer) = 0;
};

#define ENGINEWRAPPER_INTERFACE_VERSION "enginewrapper001"
