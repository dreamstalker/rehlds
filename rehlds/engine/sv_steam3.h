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

#ifndef SV_STEAM3_H
#define SV_STEAM3_H
#ifdef _WIN32
#pragma once
#endif

#include "maintypes.h"
#include "common.h"
#include "common/SteamCommon.h"
#include "public/steam/steam_api.h"
#include "public/steam/steam_gameserver.h"
#include "public/steam/steamclientpublic.h"
#include "server.h"


/* <edae6> ../engine/sv_steam3.h:21 */
class CSteam3
{
public:		// TODO: Make protected after all (it is set public for testing purpouses)

	bool m_bLoggedOn;
	bool m_bLogOnResult;
	HSteamPipe m_hSteamPipe;

protected:
	/* <edb4c> ../engine/sv_steam3.h:24 */
	CSteam3(void) {
		m_bLoggedOn = false;
		m_bLogOnResult = false;
		m_hSteamPipe = 0;
	}

	/* <edb64> ../engine/sv_steam3.h:32 */
	virtual ~CSteam3(void) { }

	/* <edb87> ../engine/sv_steam3.h:38 */
	virtual void Shutdown(void) = 0; /* linkage=_ZN7CSteam38ShutdownEv */

	/* <edbab> ../engine/sv_steam3.h:40 */
	void GSSendUserStatusResponse(class CSteamID &, int, int); /* linkage=_ZN7CSteam324GSSendUserStatusResponseER8CSteamIDii */

	/* <edbd6> ../engine/sv_steam3.h:43 */
	bool InitModule(void); /* linkage=_ZN7CSteam310InitModuleEv */
};

/* <ee151> ../engine/sv_steam3.h:52 */
class CSteam3Server : public CSteam3
{
public:

	STEAM_GAMESERVER_CALLBACK(CSteam3Server, OnGSClientApprove, GSClientApprove_t, m_CallbackGSClientApprove);
	STEAM_GAMESERVER_CALLBACK(CSteam3Server, OnGSClientDeny, GSClientDeny_t, m_CallbackGSClientDeny);
	STEAM_GAMESERVER_CALLBACK(CSteam3Server, OnGSClientKick, GSClientKick_t, m_CallbackGSClientKick);
	STEAM_GAMESERVER_CALLBACK(CSteam3Server, OnGSPolicyResponse, GSPolicyResponse_t, m_CallbackGSPolicyResponse);
	STEAM_GAMESERVER_CALLBACK(CSteam3Server, OnLogonSuccess, SteamServersConnected_t, m_CallbackLogonSuccess);
	STEAM_GAMESERVER_CALLBACK(CSteam3Server, OnLogonFailure, SteamServerConnectFailure_t, m_CallbackLogonFailure);

public:		// TODO: Make protected after all (it is set public for testing purpouses)
	bool m_bHasActivePlayers;
	bool m_bWantToBeSecure;
	bool m_bLanOnly;
	CSteamID m_SteamIDGS;

public:

	/* <ee26e> ../engine/sv_steam3.h:58 */
	NOBODY void SetServerType(void); /* linkage=_ZN13CSteam3Server13SetServerTypeEv */

	/* <ee36f> ../engine/sv_steam3.h:68 */
	NOBODY void SetSpawnCount(int count); /* linkage=_ZN13CSteam3Server13SetSpawnCountEi */

	/* <ee3b0> ../engine/sv_steam3.h:72 */
	NOBODY bool BSecure(void); /* linkage=_ZNK13CSteam3Server7BSecureEv */

	/* <ee3d0> ../engine/sv_steam3.h:73 */
	NOBODY bool BLanOnly(void); /* linkage=_ZNK13CSteam3Server8BLanOnlyEv */

	/* <ee3f0> ../engine/sv_steam3.h:74 */
	bool BWantsSecure(void)
	{
		return m_bWantToBeSecure;
	}

	/* <ee410> ../engine/sv_steam3.h:75 */
	bool BLoggedOn(void)
	{
		return m_bLoggedOn;
	}

	/* <ee390> ../engine/sv_steam3.cpp:128 */
	uint64_t GetSteamID(void); /* linkage=_ZN13CSteam3Server10GetSteamIDEv */

	/* <ee4f8> ../engine/sv_steam3.cpp:190 */
	void OnGSClientDenyHelper(client_t *cl, EDenyReason eDenyReason, const char *pchOptionalText); /* linkage=_ZN13CSteam3Server20OnGSClientDenyHelperEP8client_s11EDenyReasonPKc */

	/* <ee2af> ../engine/sv_steam3.cpp:333 */
	client_t *ClientFindFromSteamID(class CSteamID &steamIDFind); /* linkage=_ZN13CSteam3Server21ClientFindFromSteamIDER8CSteamID */

	/* <ee21b> ../engine/sv_steam3.cpp:356 */
	CSteam3Server(void);

	/* <ee234> ../engine/sv_steam3.cpp:375 */
	void Activate(void); /* linkage=_ZN13CSteam3Server8ActivateEv */

	/* <ee28a> ../engine/sv_steam3.cpp:506 */
	virtual void Shutdown(void); /* linkage=_ZN13CSteam3Server8ShutdownEv */

	/* <ee2d5> ../engine/sv_steam3.cpp:537 */
	bool NotifyClientConnect(client_t *client, const void *pvSteam2Key, uint32_t ucbSteam2Key); /* linkage=_ZN13CSteam3Server19NotifyClientConnectEP8client_sPKvj */

	/* <ee327> ../engine/sv_steam3.cpp:578 */
	bool NotifyBotConnect(client_t *client); /* linkage=_ZN13CSteam3Server16NotifyBotConnectEP8client_s */

	/* <ee305> ../engine/sv_steam3.cpp:596 */
	void NotifyClientDisconnect(client_t *cl); /* linkage=_ZN13CSteam3Server22NotifyClientDisconnectEP8client_s */

	/* <ee34d> ../engine/sv_steam3.cpp:616 */
	void NotifyOfLevelChange(bool bForce); /* linkage=_ZN13CSteam3Server19NotifyOfLevelChangeEb */

	/* <ee251> ../engine/sv_steam3.cpp:664 */
	void RunFrame(void); /* linkage=_ZN13CSteam3Server8RunFrameEv */

	/* <ee524> ../engine/sv_steam3.cpp:777 */
	void SendUpdatedServerDetails(void); /* linkage=_ZN13CSteam3Server24SendUpdatedServerDetailsEv */
};

/* <ed925> ../engine/sv_steam3.h:101 */
class CSteam3Client : public CSteam3
{
public:
	STEAM_CALLBACK(CSteam3Client, OnClientGameServerDeny, ClientGameServerDeny_t, m_CallbackClientGameServerDeny);
	STEAM_CALLBACK(CSteam3Client, OnGameServerChangeRequested, GameServerChangeRequested_t, m_CallbackGameServerChangeRequested);
	STEAM_CALLBACK(CSteam3Client, OnGameOverlayActivated, GameOverlayActivated_t, m_CallbackGameOverlayActivated);

	/* <ed984> ../engine/sv_steam3.h:104 */
	CSteam3Client(void) :
		m_CallbackClientGameServerDeny(this, &CSteam3Client::OnClientGameServerDeny),
		m_CallbackGameServerChangeRequested(this, &CSteam3Client::OnGameServerChangeRequested),
		m_CallbackGameOverlayActivated(this, &CSteam3Client::OnGameOverlayActivated)
	{ }

	/* <ed9d6> ../engine/sv_steam3.cpp:523 */
	virtual void Shutdown(void); /* linkage=_ZN13CSteam3Client8ShutdownEv */

	/* <ed9fb> ../engine/sv_steam3.cpp:816 */
	int InitiateGameConnection(void *pData, int cbMaxData, uint64 steamID, uint32 unIPServer, uint16 usPortServer, bool bSecure); /* linkage=_ZN13CSteam3Client22InitiateGameConnectionEPviyjtb */

	/* <eda3a> ../engine/sv_steam3.cpp:822 */
	void TerminateConnection(uint32, uint16); /* linkage=_ZN13CSteam3Client19TerminateConnectionEjt */

	/* <ed99c> ../engine/sv_steam3.cpp:827 */
	void InitClient(void); /* linkage=_ZN13CSteam3Client10InitClientEv */

	/* <ed9b9> ../engine/sv_steam3.cpp:905 */
	void RunFrame(void); /* linkage=_ZN13CSteam3Client8RunFrameEv */
};


#ifdef HOOK_ENGINE
#define s_Steam3Server (*ps_Steam3Server)
#define s_Steam3Client (*ps_Steam3Client)
#endif // HOOK_ENGINE


extern CSteam3Server *s_Steam3Server;
extern CSteam3Client s_Steam3Client;


extern bool (CSteam3Server::*pNotifyClientConnect)(client_t *client, const void *pvSteam2Key, uint32_t ucbSteam2Key);


uint64_t ISteamGameServer_CreateUnauthenticatedUserConnection(void);
bool ISteamGameServer_BUpdateUserData(uint64_t steamid, const char *netname, uint32_t score);
bool ISteamApps_BIsSubscribedApp(uint32_t appid);
const char *Steam_GetCommunityName(void);
qboolean Steam_NotifyClientConnect(client_t *cl, const void *pvSteam2Key, unsigned int ucbSteam2Key);
qboolean Steam_NotifyClientConnect_internal(client_t *cl, const void *pvSteam2Key, unsigned int ucbSteam2Key);
qboolean Steam_NotifyBotConnect(client_t *cl);
qboolean Steam_NotifyBotConnect_internal(client_t *cl);
qboolean Steam_NotifyBotConnect_api(IGameClient* cl);
void Steam_NotifyClientDisconnect(client_t *cl);
void Steam_NotifyClientDisconnect_internal(client_t *cl);
void Steam_NotifyOfLevelChange(void);
void Steam_Shutdown(void);
void Steam_Activate(void);
void Steam_RunFrame(void);
void Steam_SetCVar(const char *pchKey, const char *pchValue);
void Steam_ClientRunFrame(void);
void Steam_InitClient(void);
int Steam_GSInitiateGameConnection(void *pData, int cbMaxData, uint64_t steamID, uint32_t unIPServer, uint16_t usPortServer, qboolean bSecure);
void Steam_GSTerminateGameConnection(uint32_t unIPServer, uint16_t usPortServer);
void Steam_ShutdownClient(void);
uint64_t Steam_GSGetSteamID(void);
qboolean Steam_GSBSecure(void);
qboolean Steam_GSBLoggedOn(void);
qboolean Steam_GSBSecurePreference(void);
TSteamGlobalUserID Steam_Steam3IDtoSteam2(uint64_t unSteamID);
uint64_t Steam_StringToSteamID(const char *pStr);
const char *Steam_GetGSUniverse(void);
CSteam3Server *Steam3Server(void);
CSteam3Client *Steam3Client(void);
void Master_SetMaster_f(void);
void Steam_HandleIncomingPacket(byte *data, int len, int fromip, uint16_t port);

#endif // SV_STEAM3_H
