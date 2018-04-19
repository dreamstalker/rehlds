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

#include "common/NetAddress.h"
#include "ObjectDictionary.h"
#include "hltv.h"

typedef struct proxyInfo_s {
	NetAddress address;
	int slots;
	int proxies;
	int spectators;
	int isPrivate;
	float time;
} proxyInfo_t;

class IProxy;
class Status: public BaseSystemModule {
public:
	Status();
	virtual ~Status() {}

	bool Init(IBaseSystem *system, int serial, char *name);
	void RunFrame(double time);
	void ExecuteCommand(int commandID, char *commandLine);
	char *GetStatusLine();
	char *GetType();
	void ShutDown();

	void GetGlobalStats(int &proxies, int &slots, int &spectators);
	void GetLocalStats(int &proxies, int &slots, int &spectators);
	void SetProxy(IProxy *proxy);
	float GetBestRelayProxy(NetAddress *addr);
	void ParseStatusMsg(BitBuffer *stream);
	void ParseStatusReport(NetAddress *from, BitBuffer *stream);
	void Reset();
	int GetMaxSpectatorNumber();
	void RemoveProxy(NetAddress *addr);
	void ClearProxyList();
	void ReplyStatusReport();
	void SendStatusUpdate();
	void SetName(char *newName);

protected:
	enum LocalCommandIDs { CMD_ID_PROXIES = 1 };
	void CMD_Proxies(char *cmdLine);

private:
	IProxy *m_Proxy;
	float m_NextUpdateTime;
	ObjectDictionary m_Proxies;
	int m_NumberOfProxies;
	int m_NumberOfSlots;
	int m_NumberOfSpectators;
	int m_MaxNumberOfSpectators;
};

#define STATUS_INTERFACE_VERSION "status000"
