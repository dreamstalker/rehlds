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
#include "precompiled.h"

char* GetClientFallback_api() {
	return com_clientfallback;
}

int* GetAllowCheats_api() {
	return &allow_cheats;
}

bool GSBSecure_api() {
	return Steam_GSBSecure() != 0;
}

int GetBuildNumber_api() {
	return build_number();
}

double GetRealTime_api() {
	return realtime;
}

int* GetMsgBadRead_api() {
	return &msg_badread;
}

cmd_source_t* GetCmdSource_api() {
	return &cmd_source;
}

void Log_api(const char* prefix, const char* msg) {
#ifdef REHLDS_FLIGHT_REC
	FR_Log(prefix, msg);
#endif
}

CRehldsServerStatic g_RehldsServerStatic;
CRehldsServerData g_RehldsServerData;
CRehldsHookchains g_RehldsHookchains;
RehldsFuncs_t g_RehldsApiFuncs = 
{
	&SV_DropClient_api,
	&SV_RejectConnection,
	&Steam_NotifyBotConnect_api,
	&GetNetMessage_api,
	&GetHostClient_api,
	&GetMsgReadCount_api,
	&SV_FilterUser,
	&NET_SendPacket_api,
	&Cmd_TokenizeString,
	&SV_CheckChallenge_api,
	&SV_SendUserReg,
	&SV_WriteDeltaDescriptionsToClient,
	&SV_SetMoveVars,
	&SV_WriteMovevarsToClient,
	&GetClientFallback_api,
	&GetAllowCheats_api,
	&GSBSecure_api,
	&GetBuildNumber_api,
	&GetRealTime_api,
	&GetMsgBadRead_api,
	&GetCmdSource_api,
	&Log_api
};

sizebuf_t* GetNetMessage_api()
{
	return &net_message;
}

IGameClient* GetHostClient_api()
{
	if (host_client == NULL)
		return NULL;

	return GetRehldsApiClient(host_client);
}

extern int* GetMsgReadCount_api()
{
	return &msg_readcount;
}

IRehldsHookRegistry_Steam_NotifyClientConnect* CRehldsHookchains::Steam_NotifyClientConnect()
{
	return &m_Steam_NotifyClientConnect;
}

IRehldsHookRegistry_SV_ConnectClient* CRehldsHookchains::SV_ConnectClient()
{
	return &m_SV_ConnectClient;
}

IRehldsHookRegistry_SV_GetIDString* CRehldsHookchains::SV_GetIDString()
{
	return &m_SV_GetIDString;
}

IRehldsHookRegistry_SV_SendServerinfo* CRehldsHookchains::SV_SendServerinfo()
{
	return &m_SV_SendServerinfo;
}

IRehldsHookRegistry_SV_CheckProtocol* CRehldsHookchains::SV_CheckProtocol()
{
	return &m_SV_CheckProtocol;
}

IRehldsHookRegistry_SVC_GetChallenge_mod* CRehldsHookchains::SVC_GetChallenge_mod()
{
	return &m_SVC_GetChallenge_mod;
}

IRehldsHookRegistry_SV_CheckKeyInfo* CRehldsHookchains::SV_CheckKeyInfo()
{
	return &m_SV_CheckKeyInfo;
}

IRehldsHookRegistry_SV_CheckIPRestrictions* CRehldsHookchains::SV_CheckIPRestrictions()
{
	return &m_SV_CheckIPRestrictions;
}

IRehldsHookRegistry_SV_FinishCertificateCheck* CRehldsHookchains::SV_FinishCertificateCheck()
{
	return &m_SV_FinishCertificateCheck;
}

IRehldsHookRegistry_Steam_NotifyBotConnect* CRehldsHookchains::Steam_NotifyBotConnect()
{
	return &m_Steam_NotifyBotConnect;
}

IRehldsHookRegistry_SerializeSteamId* CRehldsHookchains::SerializeSteamId()
{
	return &m_SerializeSteamId;
}

IRehldsHookRegistry_SV_CompareUserID* CRehldsHookchains::SV_CompareUserID()
{
	return &m_SV_CompareUserID;
}

IRehldsHookRegistry_Steam_NotifyClientDisconnect* CRehldsHookchains::Steam_NotifyClientDisconnect()
{
	return &m_Steam_NotifyClientDisconnect;
}

IRehldsHookRegistry_PreprocessPacket* CRehldsHookchains::PreprocessPacket() {
	return &m_PreprocessPacket;
}

IRehldsHookRegistry_ValidateCommand* CRehldsHookchains::ValidateCommand() {
	return &m_ValidateCommand;
}

IRehldsHookRegistry_ClientConnected* CRehldsHookchains::ClientConnected() {
	return &m_ClientConnected;
}

IRehldsHookRegistry_HandleNetCommand* CRehldsHookchains::HandleNetCommand() {
	return &m_HandleNetCommand;
}

IRehldsHookRegistry_Mod_LoadBrushModel* CRehldsHookchains::Mod_LoadBrushModel() {
	return &m_Mod_LoadBrushModel;
}

IRehldsHookRegistry_Mod_LoadStudioModel* CRehldsHookchains::Mod_LoadStudioModel() {
	return &m_Mod_LoadStudioModel;
}

IRehldsHookRegistry_ExecuteServerStringCmd* CRehldsHookchains::ExecuteServerStringCmd() {
	return &m_ExecuteServerStringCmd;
}

int CRehldsApi::GetMajorVersion()
{
	return REHLDS_API_VERSION_MAJOR;
}

int CRehldsApi::GetMinorVersion()
{
	return REHLDS_API_VERSION_MINOR;
}

const RehldsFuncs_t* CRehldsApi::GetFuncs()
{
	return &g_RehldsApiFuncs;
}

IRehldsHookchains* CRehldsApi::GetHookchains()
{
	return &g_RehldsHookchains;
}

IRehldsServerStatic* CRehldsApi::GetServerStatic() {
	return &g_RehldsServerStatic;
}

IRehldsServerData* CRehldsApi::GetServerData() {
	return &g_RehldsServerData;
}

IRehldsFlightRecorder* CRehldsApi::GetFlightRecorder() {
	return g_FlightRecorder;
}

EXPOSE_SINGLE_INTERFACE(CRehldsApi, IRehldsApi, VREHLDS_HLDS_API_VERSION);
