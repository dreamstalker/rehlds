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

#include "ISystemModule.h"

class IWorld;
class IProxy;
class IDirector;
class INetSocket;
class ISystemModule;
class IBaseSystem;

class NetAddress;
class InfoString;
class BitBuffer;

class IServer {
public:
	virtual ~IServer() {}

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

	virtual bool Connect(IWorld *world, NetAddress *adr, INetSocket *socket) = 0;
	virtual bool LoadDemo(IWorld *world, char *filename, bool forceHLTV, bool continuous) = 0;
	virtual void Reconnect() = 0;
	virtual void Disconnect() = 0;
	virtual void Retry() = 0;
	virtual void StopRetry() = 0;
	virtual void SendStringCommand(char *command) = 0;
	virtual void SendHLTVCommand(BitBuffer *msg) = 0;
	virtual bool IsConnected() = 0;
	virtual bool IsDemoFile() = 0;
	virtual bool IsGameServer() = 0;
	virtual bool IsRelayProxy() = 0;
	virtual bool IsVoiceBlocking() = 0;
	virtual void SetProxy(IProxy *proxy) = 0;
	virtual void SetDirector(IDirector *director) = 0;
	virtual void SetPlayerName(char *newName) = 0;
	virtual void SetDelayReconnect(bool state) = 0;
	virtual void SetAutoRetry(bool state) = 0;
	virtual void SetVoiceBlocking(bool state) = 0;
	virtual void SetRate(int rate) = 0;
	virtual void SetUpdateRate(int updaterate) = 0;
	virtual void SetUserInfo(char *key, char *value) = 0;
	virtual bool SetProtocol(int version) = 0;
	virtual void SetGameDirectory(const char *defaultDir, const char *gameDir = nullptr) = 0;
	virtual int GetRate() = 0;
	virtual int GetUpdateRate() = 0;
	virtual InfoString *GetServerInfoString() = 0;
	virtual char *GetPlayerName() = 0;
	virtual float GetTime() = 0;
	virtual IWorld *GetWorld() = 0;
	virtual char *GetDemoFileName() = 0;
	virtual NetAddress *GetAddress() = 0;
	virtual char *GetHostName() = 0;
	virtual bool GetAutoRetry() = 0;
	virtual float GetPacketLoss() = 0;
	virtual int GetProtocol() = 0;
};

#define SERVER_INTERFACE_VERSION "server001"
