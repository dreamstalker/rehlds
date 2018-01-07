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

client_static_t g_pcls;
client_state_t g_pcl;
keydest_t key_dest;

playermove_t g_clmove;
qboolean cl_inmovie;

cvar_t cl_name = { "name", "0", FCVAR_ARCHIVE | FCVAR_USERINFO, 0.0f, NULL };
cvar_t rate_ = { "rate", "30000", FCVAR_USERINFO, 0.0f, NULL };
cvar_t console = { "console", "1.0", FCVAR_ARCHIVE, 0.0f, NULL };

void CL_RecordHUDCommand(const char *cmdname) { }
void R_DecalRemoveAll(int textureIndex) { }
void CL_CheckForResend(void) { }
qboolean CL_CheckFile(sizebuf_t *msg, char *filename) { return 1; }
void CL_ClearClientState(void) { }
void CL_Connect_f(void) { }
void CL_DecayLights(void) { }
void CL_Disconnect(void) { }
void CL_Disconnect_f(void) { }
void CL_EmitEntities(void) { }
void CL_InitClosest(void) { }
void CL_Init(void) { }
void EXT_FUNC CL_Particle(vec_t *origin, int color, float life, int zpos, int zvel) { }
void CL_PredictMove(qboolean repredicting) { }
void CL_PrintLogos(void) { }
void CL_ReadPackets(void) { }
qboolean CL_RequestMissingResources(void) { return 0; }
void CL_Move(void) { }
void CL_SendConnectPacket(void) { }
void CL_StopPlayback(void) { }
void CL_UpdateSoundFade(void) { }
void CL_AdjustClock(void) { }
void CL_HudMessage(const char *pMessage) { }
int Key_CountBindings(void) { return 0; }
void Key_WriteBindings(FileHandle_t f) { }
extern "C" void ClientDLL_UpdateClientData(void) { }
extern "C" void ClientDLL_HudVidInit(void) { }
void Chase_Init(void) { }
void Key_Init(void) { }
extern "C" void ClientDLL_Init(void) { }
extern "C" void ClientDLL_Shutdown(void) { }
void Con_Shutdown(void) { }
int DispatchDirectUserMsg(const char *pszName, int iSize, void *pBuf) { return 0; }
void CL_ShutDownUsrMessages(void) { }
void CL_ShutDownClientStatic(void) { }
void ClientDLL_MoveClient(struct playermove_s *pmove) { }
void ClientDLL_DeactivateMouse(void) { }
void ClientDLL_MouseEvent(int mstate) { }
void CL_Shutdown(void) { }
extern "C" void ClientDLL_Frame(double time) { }
extern "C" void ClientDLL_CAM_Think(void) { }
void CL_InitEventSystem(void) { }
void CL_CheckClientState(void) { }
void CL_RedoPrediction(void) { }
void CL_SetLastUpdate(void) { }
void EXT_FUNC Con_NPrintf(int idx, const char *fmt, ...) { }
void Sequence_OnLevelLoad( const char* mapName ) { }
void CL_WriteMessageHistory(int starting_count, int cmd) { }
void CL_MoveSpectatorCamera(void) { }
void CL_AddVoiceToDatagram(qboolean bFinal) { }
void CL_VoiceIdle(void) { }
void PollDInputDevices(void) { }
void CL_KeepConnectionActive(void) { }
void CL_UpdateModuleC(void) { }
void EXT_FUNC ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength) { }
void EXT_FUNC ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength) { }
int EXT_FUNC GetTimesTutorMessageShown(int id) { return -1; }
void EXT_FUNC RegisterTutorMessageShown(int mid) { }
void EXT_FUNC ResetTutorMessageDecayData(void) { }
void SetCareerAudioState(int state) { }

void *VguiWrap2_GetCareerUI() { return NULL; }
void VguiWrap2_GetMouseDelta(int *x, int *y) {}
int EXT_FUNC VGuiWrap2_GetLocalizedStringLength(const char *label) { return 0; }
int EXT_FUNC VGuiWrap2_IsInCareerMatch() { return 0; }
int VGuiWrap2_IsConsoleVisible() { return 0; }
int VGuiWrap2_Key_Event(int down, int keynum, const char *pszCurrentBinding) { return 0; }
int VGuiWrap2_GameUIKeyPressed() { return 0; }
int VGuiWrap2_IsGameUIVisible() { return 0; }
int VGuiWrap2_CallEngineSurfaceAppHandler(void *event, void *userData) { return 0; }
int VGuiWrap2_UseVGUI1() { return 0; }
void *VGuiWrap2_GetPanel() { return NULL; }
void VGuiWrap2_NotifyOfServerConnect(const char *game, int IP, int port) {}
void VGuiWrap2_LoadingFinished(const char *resourceType, const char *resourceName) {}
void VGuiWrap2_LoadingStarted(const char *resourceType, const char *resourceName) {}
void VGuiWrap2_ConDPrintf(const char *msg) {}
void VGuiWrap2_Startup() {}
void VGuiWrap2_ConPrintf(const char *msg) {}
void VGuiWrap2_ClearConsole() {}
void VGuiWrap2_HideConsole() {}
void VGuiWrap2_ShowDemoPlayer() {}
void VGuiWrap2_ShowConsole() {}
void VGuiWrap2_HideGameUI() {}
void VGuiWrap2_NotifyOfServerDisconnect() {}
void VGuiWrap2_Paint() {}
void VGuiWrap2_SetVisible(int state) {}
void VGuiWrap2_GetMouse() {}
void VGuiWrap2_ReleaseMouse() {}
void VGuiWrap2_Shutdown() {}
