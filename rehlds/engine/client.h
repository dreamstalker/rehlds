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

#include "maintypes.h"
#include "common.h"
#include "custom.h"
#include "cl_entity.h"
#include "consistency.h"
#include "delta_packet.h"
#include "dlight.h"
#include "entity_state.h"
#include "event.h"
#include "info.h"
#include "net.h"
#include "keys.h"
#include "sound.h"
#include "screenfade.h"
#include "usercmd.h"
#include "model.h"
#include "kbutton.h"

const int MAX_SCOREBOARDNAME = 32;
const int MAX_DEMOS = 32;

typedef enum cactive_e
{
	ca_dedicated,
	ca_disconnected,
	ca_connecting,
	ca_connected,
	ca_uninitialized,
	ca_active,
} cactive_t;

typedef struct cmd_s
{
	usercmd_t cmd;
	float senttime;
	float receivedtime;
	float frame_lerp;
	qboolean processedfuncs;
	qboolean heldback;
	int sendsize;
} cmd_t;

typedef struct frame_s
{
	double receivedtime;
	double latency;
	qboolean invalid;
	qboolean choked;
	entity_state_t playerstate[32];
	double time;
	clientdata_t clientdata;
	weapon_data_t weapondata[64];
	packet_entities_t packet_entities;
	uint16 clientbytes;
	uint16 playerinfobytes;
	uint16 packetentitybytes;
	uint16 tentitybytes;
	uint16 soundbytes;
	uint16 eventbytes;
	uint16 usrbytes;
	uint16 voicebytes;
	uint16 msgbytes;
} frame_t;

typedef struct player_info_s
{
	int userid;
	char userinfo[MAX_INFO_STRING];
	char name[MAX_SCOREBOARDNAME];
	int spectator;
	int ping;
	int packet_loss;
	char model[MAX_QPATH];
	int topcolor;
	int bottomcolor;
	int renderframe;
	int gaitsequence;
	float gaitframe;
	float gaityaw;
	vec3_t prevgaitorigin;
	customization_t customdata;
	char hashedcdkey[16];
	uint64 m_nSteamID;
} player_info_t;

typedef struct soundfade_s
{
	int nStartPercent;
	int nClientSoundFadePercent;
	double soundFadeStartTime;
	int soundFadeOutTime;
	int soundFadeHoldTime;
	int soundFadeInTime;
} soundfade_t;

typedef struct client_static_s
{
	cactive_t state;
	netchan_t netchan;
	sizebuf_t datagram;
	byte datagram_buf[MAX_DATAGRAM];
	double connect_time;
	int connect_retry;
	int challenge;
	byte authprotocol;
	int userid;
	char trueaddress[32];
	float slist_time;
	int signon;
	char servername[MAX_PATH];
	char mapstring[64];
	char spawnparms[2048];
	char userinfo[256];
	float nextcmdtime;
	int lastoutgoingcommand;
	int demonum;
	char demos[MAX_DEMOS][16];
	qboolean demorecording;
	qboolean demoplayback;
	qboolean timedemo;
	float demostarttime;
	int demostartframe;
	int forcetrack;
	FileHandle_t demofile;
	FileHandle_t demoheader;
	qboolean demowaiting;
	qboolean demoappending;
	char demofilename[MAX_PATH];
	int demoframecount;
	int td_lastframe;
	int td_startframe;
	float td_starttime;
	incomingtransfer_t dl;
	float packet_loss;
	double packet_loss_recalc_time;
	int playerbits;
	soundfade_t soundfade;
	char physinfo[MAX_PHYSINFO_STRING];
	unsigned char md5_clientdll[16];
	netadr_t game_stream;
	netadr_t connect_stream;
	qboolean passive;
	qboolean spectator;
	qboolean director;
	qboolean fSecureClient;
	qboolean isVAC2Secure;
	uint64 GameServerSteamID;
	int build_num;
} client_static_t;

typedef struct client_state_s
{
	int max_edicts;
	resource_t resourcesonhand;
	resource_t resourcesneeded;
	resource_t resourcelist[MAX_RESOURCE_LIST];
	int num_resources;
	qboolean need_force_consistency_response;
	char serverinfo[512];
	int servercount;
	int validsequence;
	int parsecount;
	int parsecountmod;
	int stats[32];
	int weapons;
	usercmd_t cmd;
	vec3_t viewangles;
	vec3_t punchangle;
	vec3_t crosshairangle;
	vec3_t simorg;
	vec3_t simvel;
	vec3_t simangles;
	vec3_t predicted_origins[64];
	vec3_t prediction_error;
	float idealpitch;
	vec3_t viewheight;
	screenfade_t sf;
	qboolean paused;
	int onground;
	int moving;
	int waterlevel;
	int usehull;
	float maxspeed;
	int pushmsec;
	int light_level;
	int intermission;
	double mtime[2];
	double time;
	double oldtime;
	frame_t frames[64];
	cmd_t commands[64];
	local_state_t predicted_frames[64];
	int delta_sequence;
	int playernum;
	event_t event_precache[MAX_EVENTS];
	model_t *model_precache[MAX_MODELS];
	int model_precache_count;
	sfx_s *sound_precache[MAX_SOUNDS];
	consistency_t consistency_list[MAX_CONSISTENCY_LIST];
	int num_consistency;
	int highentity;
	char levelname[40];
	int maxclients;
	int gametype;
	int viewentity;
	model_t *worldmodel;
	efrag_t *free_efrags;
	int num_entities;
	int num_statics;
	cl_entity_t viewent;
	int cdtrack;
	int looptrack;
	CRC32_t serverCRC;
	unsigned char clientdllmd5[16];
	float weaponstarttime;
	int weaponsequence;
	int fPrecaching;
	dlight_t *pLight;
	player_info_t players[32];
	entity_state_t instanced_baseline[64];
	int instanced_baseline_number;
	CRC32_t mapCRC;
	event_state_t events;
	char downloadUrl[128];
} client_state_t;

typedef enum CareerStateType_e
{
	CAREER_NONE = 0,
	CAREER_LOADING = 1,
	CAREER_PLAYING = 2,
} CareerStateType;

extern keydest_t key_dest;
extern client_static_t g_pcls;
extern client_state_t g_pcl;

extern playermove_t g_clmove;
extern qboolean cl_inmovie;

extern cvar_t cl_name;
extern cvar_t rate_;
extern cvar_t console;

void CL_RecordHUDCommand(const char *cmdname);
void R_DecalRemoveAll(int textureIndex);
void CL_CheckForResend();
qboolean CL_CheckFile(sizebuf_t *msg, char *filename);
void CL_ClearClientState();
void CL_Connect_f();
void CL_DecayLights();
void CL_Disconnect();
void CL_Disconnect_f();
void CL_EmitEntities();
void CL_InitClosest();
void CL_Init();
void CL_Particle(vec_t *origin, int color, float life, int zpos, int zvel);
void CL_PredictMove(qboolean repredicting);
void CL_PrintLogos();
void CL_ReadPackets();
qboolean CL_RequestMissingResources();
void CL_Move();
void CL_SendConnectPacket();
void CL_StopPlayback();
void CL_UpdateSoundFade();
void CL_AdjustClock();
void CL_Save(const char *name);
void CL_HudMessage(const char *pMessage);

int Key_CountBindings();
void Key_WriteBindings(FileHandle_t f);
extern "C" void ClientDLL_UpdateClientData();
extern "C" void ClientDLL_HudVidInit();
void Chase_Init();
void Key_Init();
extern "C" void ClientDLL_Init();
void Con_Shutdown();
int DispatchDirectUserMsg(const char *pszName, int iSize, void *pBuf);
void CL_ShutDownUsrMessages();
void CL_ShutDownClientStatic();
extern "C" void ClientDLL_MoveClient(struct playermove_s *ppmove);
void CL_Shutdown();
extern "C" void ClientDLL_Frame(double time);
extern "C" void ClientDLL_CAM_Think();
void CL_InitEventSystem();
void CL_CheckClientState();
void CL_RedoPrediction();
void CL_SetLastUpdate();
void Con_NPrintf(int idx, const char *fmt, ...);
void CL_WriteMessageHistory(int starting_count, int cmd);
void CL_MoveSpectatorCamera();
void CL_AddVoiceToDatagram(qboolean bFinal);
void CL_VoiceIdle();
void PollDInputDevices();
void CL_KeepConnectionActive();
void CL_UpdateModuleC();
void ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength);
void ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength);
int GetTimesTutorMessageShown(int id);
void RegisterTutorMessageShown(int mid);
void ResetTutorMessageDecayData();
void SetCareerAudioState(int state);

int EXT_FUNC VGuiWrap2_IsInCareerMatch();
int EXT_FUNC VGuiWrap2_GetLocalizedStringLength(const char *label);
void VGuiWrap2_LoadingStarted(const char *resourceType, const char *resourceName);
void *VguiWrap2_GetCareerUI();
void VguiWrap2_GetMouseDelta(int *x, int *y);
int EXT_FUNC VGuiWrap2_GetLocalizedStringLength(const char *label);
int EXT_FUNC VGuiWrap2_IsInCareerMatch();
int VGuiWrap2_IsConsoleVisible();
int VGuiWrap2_Key_Event(int down, int keynum, const char *pszCurrentBinding);
int VGuiWrap2_GameUIKeyPressed();
int VGuiWrap2_IsGameUIVisible();
int VGuiWrap2_CallEngineSurfaceAppHandler(void *event, void *userData);
int VGuiWrap2_UseVGUI1();
void *VGuiWrap2_GetPanel();
void VGuiWrap2_NotifyOfServerConnect(const char *game, int IP, int port);
void VGuiWrap2_LoadingFinished(const char *resourceType, const char *resourceName);
void VGuiWrap2_LoadingStarted(const char *resourceType, const char *resourceName);
void VGuiWrap2_ConDPrintf(const char *msg);
void VGuiWrap2_Startup();
void VGuiWrap2_ConPrintf(const char *msg);
void VGuiWrap2_ClearConsole();
void VGuiWrap2_HideConsole();
void VGuiWrap2_ShowDemoPlayer();
void VGuiWrap2_ShowConsole();
void VGuiWrap2_HideGameUI();
void VGuiWrap2_NotifyOfServerDisconnect();
void VGuiWrap2_Paint();
void VGuiWrap2_SetVisible(int state);
void VGuiWrap2_GetMouse();
void VGuiWrap2_ReleaseMouse();
void VGuiWrap2_Shutdown();
