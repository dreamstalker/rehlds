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

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

cvar_t cl_name = { "name", "0", FCVAR_ARCHIVE | FCVAR_USERINFO, 0.0f, NULL };
cvar_t rate_ = { "rate", "30000", FCVAR_USERINFO, 0.0f, NULL };
cvar_t console = { "console", "1.0", FCVAR_ARCHIVE, 0.0f, NULL };

#else //HOOK_ENGINE

cvar_t cl_name;
cvar_t rate_;
cvar_t console;

#endif //HOOK_ENGINE

/* <1c38b> ../engine/cl_null.c:18 */
void CL_RecordHUDCommand(char *cmdname) { }

/* <1c3b2> ../engine/cl_null.c:19 */
void R_DecalRemoveAll(int textureIndex) { }

/* <1c3d9> ../engine/cl_null.c:20 */
void CL_CheckForResend(void) { }

/* <1c3ed> ../engine/cl_null.c:21 */
qboolean CL_CheckFile(sizebuf_t *msg, char *filename) { return 1; }

/* <1c42c> ../engine/cl_null.c:22 */
void CL_ClearClientState(void) { }

/* <1c440> ../engine/cl_null.c:23 */
void CL_Connect_f(void) { }

/* <1c454> ../engine/cl_null.c:24 */
void CL_DecayLights(void) { }

/* <1c468> ../engine/cl_null.c:25 */
void CL_Disconnect(void) { }

/* <1c47c> ../engine/cl_null.c:26 */
void CL_Disconnect_f(void) { }

/* <1c490> ../engine/cl_null.c:27 */
void CL_EmitEntities(void) { }

/* <1c4a4> ../engine/cl_null.c:28 */
void CL_InitClosest(void) { }

/* <1c4b7> ../engine/cl_null.c:29 */
void CL_Init(void) { }

/* <1c4cb> ../engine/cl_null.c:30 */
void CL_Particle(vec_t *origin, int color, float life, int zpos, int zvel) { }

/* <1c530> ../engine/cl_null.c:31 */
void CL_PredictMove(qboolean repredicting) { }

/* <1c557> ../engine/cl_null.c:32 */
void CL_PrintLogos(void) { }

/* <1c56b> ../engine/cl_null.c:33 */
void CL_ReadPackets(void) { }

/* <1c57f> ../engine/cl_null.c:34 */
qboolean CL_RequestMissingResources(void) { return 0; }

/* <1c597> ../engine/cl_null.c:35 */
void CL_Move(void) { }

/* <1c5ab> ../engine/cl_null.c:36 */
void CL_SendConnectPacket(void) { }

/* <1c5bf> ../engine/cl_null.c:38 */
void CL_StopPlayback(void) { }

/* <1c5d3> ../engine/cl_null.c:39 */
void CL_UpdateSoundFade(void) { }

/* <1c5e7> ../engine/cl_null.c:40 */
void CL_AdjustClock(void) { }

void CL_HudMessage(const char *pMessage) { }
int Key_CountBindings(void) { return 0; }

/* <1c5fb> ../engine/cl_null.c:43 */
void Key_WriteBindings(FileHandle_t f) { }

/* <1c620> ../engine/cl_null.c:44 */
extern "C" void ClientDLL_UpdateClientData(void) { }

/* <1c634> ../engine/cl_null.c:45 */
extern "C" void ClientDLL_HudVidInit(void) { }

/* <1c648> ../engine/cl_null.c:46 */
void Chase_Init(void) { }

/* <1c65c> ../engine/cl_null.c:47 */
void Key_Init(void) { }

/* <1c670> ../engine/cl_null.c:48 */
extern "C" void ClientDLL_Init(void) { }
extern "C" void ClientDLL_Shutdown(void) { }

/* <1c684> ../engine/cl_null.c:49 */
void Con_Shutdown(void) { }

/* <1c698> ../engine/cl_null.c:50 */
int DispatchDirectUserMsg(const char *pszName, int iSize, void *pBuf) { return 0; }

/* <1c6df> ../engine/cl_null.c:51 */
void CL_ShutDownUsrMessages(void) { }

/* <1c6f3> ../engine/cl_null.c:52 */
void CL_ShutDownClientStatic(void) { }

/* <1c707> ../engine/cl_null.c:54 */
void ClientDLL_MoveClient(struct playermove_s *pmove) { }

void ClientDLL_DeactivateMouse(void) { }
void ClientDLL_MouseEvent(int mstate) { }

/* <1c73a> ../engine/cl_null.c:56 */
void CL_Shutdown(void) { }

/* <1c74e> ../engine/cl_null.c:58 */
extern "C" void ClientDLL_Frame(double time) { }

/* <1c775> ../engine/cl_null.c:59 */
extern "C" void ClientDLL_CAM_Think(void) { }

/* <1c788> ../engine/cl_null.c:60 */
void CL_InitEventSystem(void) { }

/* <1c79b> ../engine/cl_null.c:61 */
void CL_CheckClientState(void) { }

/* <1c7af> ../engine/cl_null.c:62 */
void CL_RedoPrediction(void) { }

/* <1c7c3> ../engine/cl_null.c:63 */
void CL_SetLastUpdate(void) { }

/* <1c7d7> ../engine/cl_null.c:65 */
void Con_NPrintf(int idx, const char *fmt, ...) { }
void Sequence_OnLevelLoad( const char* mapName ) { }

/* <1c80d> ../engine/cl_null.c:69 */
void CL_WriteMessageHistory(int starting_count, int cmd) { }

/* <1c842> ../engine/cl_null.c:71 */
void CL_MoveSpectatorCamera(void) { }

/* <1c856> ../engine/cl_null.c:72 */
void CL_AddVoiceToDatagram(qboolean bFinal) { }

/* <1c87d> ../engine/cl_null.c:73 */
void CL_VoiceIdle(void) { }

/* <1c891> ../engine/cl_null.c:75 */
void PollDInputDevices(void) { }

/* <1c8a4> ../engine/cl_null.c:77 */
void CL_KeepConnectionActive(void) { }

/* <1c8b8> ../engine/cl_null.c:79 */
void CL_UpdateModuleC(void) { }

/* <1c8cc> ../engine/cl_null.c:81 */
int VGuiWrap2_IsInCareerMatch(void) { return 0; }

/* <1c8e3> ../engine/cl_null.c:83 */
void VguiWrap2_GetCareerUI(void) { }

/* <1c8f6> ../engine/cl_null.c:85 */
int VGuiWrap2_GetLocalizedStringLength(const char *label) { return 0; }
void VGuiWrap2_LoadingStarted(const char *resourceType, const char *resourceName) {}

/* <1c921> ../engine/cl_null.c:87 */
void ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength) { }

/* <1c95c> ../engine/cl_null.c:88 */
void ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength) { }

/* <1c991> ../engine/cl_null.c:89 */
int GetTimesTutorMessageShown(int id) { return -1; }

/* <1c9bb> ../engine/cl_null.c:90 */
void RegisterTutorMessageShown(int mid) { }

/* <1c9e2> ../engine/cl_null.c:91 */
void ResetTutorMessageDecayData(void) { }

/* <1c9f5> ../engine/cl_null.c:92 */
void SetCareerAudioState(int state) { }
