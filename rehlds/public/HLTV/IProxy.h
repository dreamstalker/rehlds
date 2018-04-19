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
#include "custom.h"

class IWorld;
class IServer;
class IDirector;
class INetSocket;
class BitBuffer;
class NetAddress;
class IObjectContainer;

#define MAX_PROXY_CLIENTS 255

#define GROUP_CLIENT      0x00001 // Broadcast to client
#define GROUP_PROXY       0x00002 // Broadcast to proxy
#define GROUP_DEMO        0x00004 // Broadcast to demo file
#define GROUP_MULTICAST   0x00008 // Broadcast to multicast (obsolete)
#define GROUP_VOICE       0x00010 // Broadcast to voice enabled clients
#define GROUP_CHAT        0x00020 // Broadcast to chat enabled clients

#define GROUP_CLIENT_ALL  GROUP_CLIENT | GROUP_PROXY | GROUP_DEMO | GROUP_MULTICAST

enum ChatMode_e : int
{
	CHAT_OFF,         // Spectators can't chat.
	CHAT_LOCAL,       // Only spectators connected to the same proxy can see their chat messages.
	CHAT_GLOBAL,      // All spectators can chat between each other (then Master and all Relay proxies must have set chatmode 2).
};

class IProxy: virtual public ISystemModule {
public:
	virtual ~IProxy() {}

	virtual void Reset() = 0;
	virtual void Broadcast(byte *data, int length, int groupType, bool isReliable) = 0;
	virtual void IncreaseCheering(int votes) = 0;
	virtual void ParseStatusMsg(BitBuffer *stream) = 0;
	virtual void ParseStatusReport(NetAddress *from, BitBuffer *stream) = 0;
	virtual bool ProcessConnectionlessMessage(NetAddress *from, BitBuffer *stream) = 0;
	virtual void ChatCommentator(char *nick, char *text) = 0;
	virtual void ChatSpectator(char *nick, char *text) = 0;
	virtual void CountLocalClients(int &spectators, int &proxies) = 0;
	virtual struct resource_s *AddResource(char *fileName, resourcetype_t type, char *asFileName = nullptr) = 0;
	virtual bool IsLanOnly() = 0;
	virtual bool IsMaster() = 0;
	virtual bool IsActive() = 0;
	virtual bool IsPublicGame() = 0;
	virtual bool IsPasswordProtected() = 0;
	virtual bool IsStressed() = 0;
	virtual void SetDelay(float seconds) = 0;
	virtual void SetClientTime(double time, bool relative) = 0;
	virtual void SetClientTimeScale(float scale) = 0;
	virtual void SetMaxRate(int rate) = 0;
	virtual void SetMaxLoss(float maxloss) = 0;
	virtual void SetMaxUpdateRate(int updaterate) = 0;
	virtual bool SetMaxClients(int number) = 0;
	virtual void SetRegion(unsigned char region) = 0;
	virtual float GetDelay() = 0;
	virtual double GetSpectatorTime() = 0;
	virtual double GetProxyTime() = 0;
	virtual int GetMaxClients() = 0;
	virtual IWorld *GetWorld() = 0;
	virtual IServer *GetServer() = 0;
	virtual IDirector *GetDirector() = 0;
	virtual INetSocket *GetSocket() = 0;
	virtual ChatMode_e GetChatMode() = 0;
	virtual void GetStatistics(int &proxies, int &slots, int &spectators) = 0;
	virtual int GetMaxRate() = 0;
	virtual int GetMaxUpdateRate() = 0;
	virtual struct resource_s *GetResource(char *fileName) = 0;
	virtual int GetDispatchMode() = 0;
	virtual unsigned char GetRegion() = 0;
	virtual IObjectContainer *GetClients() = 0;
	virtual bool WriteSignonData(int type, BitBuffer *stream) = 0;
};

#define PROXY_INTERFACE_VERSION "proxy001"
