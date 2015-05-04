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

#ifndef CLIENT_H
#define CLIENT_H
#ifdef _WIN32
#pragma once
#endif

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

#define MAX_SCOREBOARDNAME	32
#define MAX_DEMOS		32

typedef enum cactive_e
{
	ca_dedicated,
	ca_disconnected,
	ca_connecting,
	ca_connected,
	ca_uninitialized,
	ca_active,
} cactive_t;

/* <1523b> ../engine/client.h:70 */
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

/* <152b1> ../engine/client.h:85 */
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
	uint16_t clientbytes;
	uint16_t playerinfobytes;
	uint16_t packetentitybytes;
	uint16_t tentitybytes;
	uint16_t soundbytes;
	uint16_t eventbytes;
	uint16_t usrbytes;
	uint16_t voicebytes;
	uint16_t msgbytes;
} frame_t;

/* <153e9> ../engine/client.h:127 */
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
	uint64_t m_nSteamID;
} player_info_t;

/* <277f5> ../engine/client.h:208 */
typedef struct soundfade_s
{
	int nStartPercent;
	int nClientSoundFadePercent;
	double soundFadeStartTime;
	int soundFadeOutTime;
	int soundFadeHoldTime;
	int soundFadeInTime;
} soundfade_t;

/* <1f23> ../engine/client.h:223 */
typedef struct client_static_s
{
	cactive_t state;
	netchan_t netchan;
	sizebuf_t datagram;
	byte datagram_buf[4000];
	double connect_time;
	int connect_retry;
	int challenge;
	byte authprotocol;
	int userid;
	char trueaddress[32];
	float slist_time;
	int signon;
	char servername[260];
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
	char demofilename[260];
	int demoframecount;
	int td_lastframe;
	int td_startframe;
	float td_starttime;
	incomingtransfer_t dl;
	float packet_loss;
	double packet_loss_recalc_time;
	int playerbits;
	soundfade_t soundfade;
	char physinfo[256];
	unsigned char md5_clientdll[16];
	netadr_t game_stream;
	netadr_t connect_stream;
	qboolean passive;
	qboolean spectator;
	qboolean director;
	qboolean fSecureClient;
	qboolean isVAC2Secure;
	uint64_t GameServerSteamID;
	int build_num;
} client_static_t;

/* <1bda4> ../engine/client.h:320 */
typedef struct client_state_s
{
	int max_edicts;
	resource_t resourcesonhand;
	resource_t resourcesneeded;
	resource_t resourcelist[1280];
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
	vec_t predicted_origins[64][3];
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
	event_t event_precache[256];
	model_t *model_precache[512];
	int model_precache_count;
	sfx_s *sound_precache[512];
	consistency_t consistency_list[512];
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

/* <3a9c7> ../engine/client.h:645 */
typedef enum CareerStateType_e
{
	CAREER_NONE = 0,
	CAREER_LOADING = 1,
	CAREER_PLAYING = 2,
} CareerStateType;


#ifdef HOOK_ENGINE
#define g_pcls (*pcls)
#define g_pcl (*pcl)
#define key_dest (*pkey_dest)

#define g_clmove (*pg_clmove)
#define cl_inmovie (*pcl_inmovie)

#define cl_name (*pcl_name)
#define rate_ (*prate)
#define console (*pconsole)
#endif // HOOK_ENGINE

extern keydest_t key_dest;
extern client_static_t g_pcls;
extern client_state_t g_pcl;

extern playermove_t g_clmove;
extern qboolean cl_inmovie;

extern cvar_t cl_name;
extern cvar_t rate_;
extern cvar_t console;

void CL_RecordHUDCommand(char *cmdname);
void R_DecalRemoveAll(int textureIndex);
void CL_CheckForResend(void);
qboolean CL_CheckFile(sizebuf_t *msg, char *filename);
void CL_ClearClientState(void);
void CL_Connect_f(void);
void CL_DecayLights(void);
void CL_Disconnect(void);
void CL_Disconnect_f(void);
void CL_EmitEntities(void);
void CL_InitClosest(void);
void CL_Init(void);
void CL_Particle(vec_t *origin, int color, float life, int zpos, int zvel);
void CL_PredictMove(qboolean repredicting);
void CL_PrintLogos(void);
void CL_ReadPackets(void);
qboolean CL_RequestMissingResources(void);
void CL_Move(void);
void CL_SendConnectPacket(void);
void CL_StopPlayback(void);
void CL_UpdateSoundFade(void);
void CL_AdjustClock(void);
void CL_Save(const char *name);
void CL_HudMessage(const char *pMessage);

int Key_CountBindings(void);
void Key_WriteBindings(FileHandle_t f);
extern "C" void ClientDLL_UpdateClientData(void);
extern "C" void ClientDLL_HudVidInit(void);
void Chase_Init(void);
void Key_Init(void);
extern "C" void ClientDLL_Init(void);
void Con_Shutdown(void);
int DispatchDirectUserMsg(const char *pszName, int iSize, void *pBuf);
void CL_ShutDownUsrMessages(void);
void CL_ShutDownClientStatic(void);

extern "C" void ClientDLL_MoveClient(struct playermove_s *ppmove);

void CL_Shutdown(void);

extern "C" void ClientDLL_Frame(double time);
extern "C" void ClientDLL_CAM_Think(void);
void CL_InitEventSystem(void);
void CL_CheckClientState(void);
void CL_RedoPrediction(void);
void CL_SetLastUpdate(void);

void Con_NPrintf(int idx, const char *fmt, ...);


void CL_WriteMessageHistory(int starting_count, int cmd);

void CL_MoveSpectatorCamera(void);
void CL_AddVoiceToDatagram(qboolean bFinal);
void CL_VoiceIdle(void);

void PollDInputDevices(void);

void CL_KeepConnectionActive(void);

void CL_UpdateModuleC(void);

int VGuiWrap2_IsInCareerMatch(void);

void VguiWrap2_GetCareerUI(void);

int VGuiWrap2_GetLocalizedStringLength(const char *label);
void VGuiWrap2_LoadingStarted(const char *resourceType, const char *resourceName);

void ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength);
void ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength);
int GetTimesTutorMessageShown(int id);
void RegisterTutorMessageShown(int mid);
void ResetTutorMessageDecayData(void);
void SetCareerAudioState(int state);

#endif // CLIENT_H
