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

#include "BaseSystemModule.h"
#include "TokenLine.h"

#define MASTER_VERSION		"1.1.2.6"

enum GameType_e
{
	GT_CSTRIKE,
	GT_TFC,
	GT_DOD,
	GT_DMC,
	GT_GEARBOX,
	GT_RICOCHET,
	GT_VALVE,
	GT_CZERO,
	GT_CZEROR,
	GT_BSHIFT,
	GT_CSTRIKE_BETA,
};

class Proxy;
class INetSocket;

class Master: public BaseSystemModule {
public:
	Master();
	virtual ~Master() {}

	bool Init(IBaseSystem *system, int serial, char *name);
	void ExecuteCommand(int commandID, char *commandLine);
	void RunFrame(double time);
	char *GetStatusLine();
	char *GetType();
	void ShutDown();

	void InitializeSteam();
	void SendShutdown();
	void SetProxy(IProxy *proxy);

private:
	enum LocalCommandIDs {
		CMD_ID_HEARTBEAT = 1,
		CMD_ID_NOMASTER,
		CMD_ID_LISTMASTER,
	};

	void CMD_Heartbeat(char *cmdLine);
	void CMD_NoMaster(char *cmdLine);
	void CMD_ListMaster(char *cmdLine);

	enum {
		GAME_APPID_CSTRIKE      = 10,
		GAME_APPID_TFC          = 20,
		GAME_APPID_DOD          = 30,
		GAME_APPID_DMC          = 40,
		GAME_APPID_GEARBOX      = 50,
		GAME_APPID_RICOCHET     = 60,
		GAME_APPID_VALVE        = 70,
		GAME_APPID_CZERO        = 80,
		GAME_APPID_CZEROR       = 100,
		GAME_APPID_BSHIFT       = 130,
		GAME_APPID_CSTRIKE_BETA = 150,
	};

	typedef struct GameToAppIDMapItem_s
	{
		size_t appID;
		const char *dir;
	} GameToAppIDMapItem_t;

	static GameToAppIDMapItem_t m_GameToAppIDMap[];
	int GetGameAppID(const char *gamedir) const;

protected:
	friend class Proxy;

	bool m_NoMaster;
	bool m_bMasterLoaded;
	INetSocket *m_MasterSocket;
	IProxy *m_Proxy;
	double m_flMasterUpdateTime;
	bool m_bSteamInitialized;
};

#define MASTER_INTERFACE_VERSION "master000"
