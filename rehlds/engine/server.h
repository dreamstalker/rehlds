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

#ifndef SERVER_H
#define SERVER_H
#ifdef _WIN32
#pragma once
#endif

#include "maintypes.h"

// TODO: I think this defines must be in /common/
#define MAX_CLIENTS				32
#define MAX_EDICTS				900
#define MAX_NAME				32
#define MAX_LIGHTSTYLES			64
#define MAX_PACKET_ENTITIES		256

#include "custom_int.h"
#include "crc.h"
#include "cvar.h"
#include "dll_state.h"
#include "consistency.h"
#include "event.h"
#include "entity_state.h"
#include "delta.h"
#include "delta_packet.h"
#include "model.h"
#include "net.h"
#include "progs.h"
#include "filter.h"
#include "server_static.h"
#include "usermsg.h"
#include "userid.h"
#include "pm_defs.h"
#include "inst_baseline.h"
#include "net_ws.h"

#define DEFAULT_SOUND_PACKET_VOLUME			255
#define DEFAULT_SOUND_PACKET_ATTENUATION	1.0f
#define DEFAULT_SOUND_PACKET_PITCH			100

// Sound flags
#define SND_FL_VOLUME			BIT(0)	// send volume
#define SND_FL_ATTENUATION		BIT(1)	// send attenuation
#define SND_FL_LARGE_INDEX		BIT(2)	// send sound number as short instead of byte
#define SND_FL_PITCH			BIT(3)	// send pitch
#define SND_FL_SENTENCE			BIT(4)	// set if sound num is actually a sentence num
#define SND_FL_STOP				BIT(5)	// stop the sound
#define SND_FL_CHANGE_VOL		BIT(6)	// change sound vol
#define SND_FL_CHANGE_PITCH		BIT(7)	// change sound pitch
#define SND_FL_SPAWNING			BIT(8)	// we're spawning, used in some cases for ambients (not sent across network)

// Message send destination flags
#define MSG_FL_NONE				0		// No flags
#define MSG_FL_BROADCAST		BIT(0)	// Broadcast?
#define MSG_FL_PVS				BIT(1)	// Send to PVS
#define MSG_FL_PAS				BIT(2)	// Send to PAS
#define MSG_FL_ONE				BIT(7)	// Send to single client


#define HL_SOUND_MAX 512
#define HL_SOUND_HASHLOOKUP_SIZE (HL_SOUND_MAX * 2 - 1)

#define HL_MODEL_MAX 512
#define HL_GENERIC_MAX 512
#define HL_EVENT_MAX 256

/* <87d6f> ../engine/server.h:43 */
typedef enum redirect_e
{
	RD_NONE = 0,
	RD_CLIENT = 1,
	RD_PACKET = 2,
} redirect_t;

/* <7588> ../engine/server.h:65 */
typedef enum server_state_e
{
	ss_dead = 0,
	ss_loading = 1,
	ss_active = 2,
} server_state_t;

/* <75f3> ../engine/server.h:75 */
typedef struct server_s
{
	qboolean active;
	qboolean paused;
	qboolean loadgame;
	double time;
	double oldtime;
	int lastcheck;
	double lastchecktime;
	char name[64];
	char oldname[64];
	char startspot[64];
	char modelname[64];
	struct model_s *worldmodel;
	CRC32_t worldmapCRC;
	unsigned char clientdllmd5[16];
	resource_t resourcelist[1280];
	int num_resources;
	consistency_t consistency_list[512];
	int num_consistency;
	const char *model_precache[HL_MODEL_MAX];
	struct model_s *models[HL_MODEL_MAX];
	unsigned char model_precache_flags[HL_MODEL_MAX];
	struct event_s event_precache[HL_EVENT_MAX];
	const char *sound_precache[HL_SOUND_MAX];
	short int sound_precache_hashedlookup[HL_SOUND_HASHLOOKUP_SIZE];
	qboolean sound_precache_hashedlookup_built;
	const char *generic_precache[HL_GENERIC_MAX];
	char generic_precache_names[HL_GENERIC_MAX][64];
	int num_generic_names;
	char *lightstyles[MAX_LIGHTSTYLES];
	int num_edicts;
	int max_edicts;
	edict_t *edicts;
	struct entity_state_s *baselines;
	extra_baselines_t *instance_baselines;
	server_state_t state;
	sizebuf_t datagram;
	unsigned char datagram_buf[MAX_DATAGRAM];
	sizebuf_t reliable_datagram;
	unsigned char reliable_datagram_buf[MAX_DATAGRAM];
	sizebuf_t multicast;
	unsigned char multicast_buf[1024];
	sizebuf_t spectator;
	unsigned char spectator_buf[1024];
	sizebuf_t signon;
	unsigned char signon_data[32768];
} server_t;


struct rehlds_server_t {

	//map for sv.model_precache (for faster resolving of model index by its name)
#if defined(REHLDS_FIXES)
	CStringKeyStaticMap<int, 7, HL_MODEL_MAX * 2> modelsMap; //case-sensitive keys for better performance
#elif defined(REHLDS_OPT_PEDANTIC)
	CICaseStringKeyStaticMap<int, 7, HL_MODEL_MAX * 2> modelsMap; //use case-insensitive keys to conform original engine's behavior
#endif

};


/* <3b30a> ../engine/server.h:163 */
typedef struct client_frame_s
{
	double senttime;
	float ping_time;
	clientdata_t clientdata;
	weapon_data_t weapondata[64];
	packet_entities_t entities;
} client_frame_t;

/* <2eb23> ../engine/server.h:177 */
typedef struct client_s
{
	qboolean active;
	qboolean spawned;
	qboolean fully_connected;
	qboolean connected;
	qboolean uploading;
	qboolean hasusrmsgs;
	qboolean has_force_unmodified;
	netchan_t netchan;
	int chokecount;
	int delta_sequence;
	qboolean fakeclient;
	qboolean proxy;
	usercmd_t lastcmd;
	double connecttime;
	double cmdtime;
	double ignorecmdtime;
	float latency;
	float packet_loss;
	double localtime;
	double nextping;
	double svtimebase;
	sizebuf_t datagram;
	byte datagram_buf[MAX_DATAGRAM];
	double connection_started;
	double next_messagetime;
	double next_messageinterval;
	qboolean send_message;
	qboolean skip_message;
	client_frame_t *frames;
	event_state_t events;
	edict_t *edict;
	const edict_t *pViewEntity;
	int userid;
	USERID_t network_userid;
	char userinfo[MAX_INFO_STRING];
	qboolean sendinfo;
	float sendinfo_time;
	char hashedcdkey[64];
	char name[32];
	int topcolor;
	int bottomcolor;
	int entityId;
	resource_t resourcesonhand;
	resource_t resourcesneeded;
	FileHandle_t upload;
	qboolean uploaddoneregistering;
	customization_t customdata;
	int crcValue;
	int lw;
	int lc;
	char physinfo[MAX_INFO_STRING];
	qboolean m_bLoopback;
	uint32 m_VoiceStreams[2];
	double m_lastvoicetime;
	int m_sendrescount;
} client_t;

/* <9e1af> ../engine/server.h:276 */
typedef enum sv_delta_s
{
	sv_packet_nodelta,
	sv_packet_delta,
} sv_delta_t;

/* <9e1d1> ../engine/server.h:336 */
enum
{
	SND_ANYPLAYER,
	SND_NOTHOST,
};

typedef struct rcon_failure_s rcon_failure_t;
typedef struct challenge_s challenge_t;

/* <a5b0c> ../engine/sv_main.c:5335 */
typedef struct deltacallback_s
{
	int *numbase;
	int num;
	qboolean remove;
	qboolean custom;
	qboolean newbl;
	int newblindex;
	qboolean full;
	int offset;
} deltacallback_t;


#ifdef HOOK_ENGINE
#define pr_strings (*ppr_strings)
#define gNullString (*pgNullString)
#define scr_skipupdate (*pscr_skipupdate)
#define scr_centertime_off (*pscr_centertime_off)
#define g_LastScreenUpdateTime (*pg_LastScreenUpdateTime)

#define SV_UPDATE_BACKUP (*pSV_UPDATE_BACKUP)
#define SV_UPDATE_MASK (*pSV_UPDATE_MASK)

#define gGlobalVariables (*pgGlobalVariables)
#define g_psvs (*psvs)
#define g_psv (*psv)

#define sv_lan (*psv_lan)
#define sv_lan_rate (*psv_lan_rate)
#define sv_aim (*psv_aim)

#define sv_skycolor_r (*psv_skycolor_r)
#define sv_skycolor_g (*psv_skycolor_g)
#define sv_skycolor_b (*psv_skycolor_b)
#define sv_skyvec_x (*psv_skyvec_x)
#define sv_skyvec_y (*psv_skyvec_y)
#define sv_skyvec_z (*psv_skyvec_z)
#define sv_skyname (*psv_skyname)

#define sv_spectatormaxspeed (*psv_spectatormaxspeed)
#define sv_airaccelerate (*psv_airaccelerate)
#define sv_wateraccelerate (*psv_wateraccelerate)
#define sv_waterfriction (*psv_waterfriction)
#define sv_zmax (*psv_zmax)
#define sv_wateramp (*psv_wateramp)
#define mapcyclefile (*pmapcyclefile)
#define motdfile (*pmotdfile)
#define servercfgfile (*pservercfgfile)
#define lservercfgfile (*plservercfgfile)
#define logsdir (*plogsdir)
#define bannedcfgfile (*pbannedcfgfile)

#define sv_decalnames (*psv_decalnames)
#define sv_decalnamecount (*psv_decalnamecount)

#define sv_gpNewUserMsgs (*psv_gpNewUserMsgs)
#define sv_gpUserMsgs (*psv_gpUserMsgs)

#define g_svmove (*pg_svmove)

#define sv_lastnum (*psv_lastnum)
#define g_sv_instance_baselines (*pg_sv_instance_baselines)
#define g_bOutOfDateRestart (*pg_bOutOfDateRestart)
#define g_userid (*pg_userid)

#define g_sv_delta (*pg_sv_delta)
#define g_peventdelta (*pg_peventdelta)

#define rcon_password (*prcon_password)
#define sv_enableoldqueries (*psv_enableoldqueries)
#define sv_instancedbaseline (*psv_instancedbaseline)
#define sv_contact (*psv_contact)
#define sv_maxupdaterate (*psv_maxupdaterate)
#define sv_minupdaterate (*psv_minupdaterate)
#define sv_filterban (*psv_filterban)
#define sv_minrate (*psv_minrate)
#define sv_maxrate (*psv_maxrate)
#define sv_logrelay (*psv_logrelay)
#define violence_hblood (*pviolence_hblood)
#define violence_ablood (*pviolence_ablood)
#define violence_hgibs (*pviolence_hgibs)
#define violence_agibs (*pviolence_agibs)
#define sv_newunit (*psv_newunit)
#define sv_clienttrace (*psv_clienttrace)
#define sv_timeout (*psv_timeout)
#define sv_failuretime (*psv_failuretime)

#define sv_cheats (*psv_cheats)
#define sv_password (*psv_password)
#define sv_proxies (*psv_proxies)
#define sv_outofdatetime (*psv_outofdatetime)
#define mapchangecfgfile (*pmapchangecfgfile)

#define allow_cheats (*pallow_cheats)
#define mp_logecho (*pmp_logecho)
#define mp_logfile (*pmp_logfile)

#define sv_allow_download (*psv_allow_download)
#define sv_send_logos (*psv_send_logos)
#define sv_send_resources (*psv_send_resources)
#define sv_log_singleplayer (*psv_log_singleplayer)
#define sv_logsecret (*psv_logsecret)
#define sv_log_onefile (*psv_log_onefile)
#define sv_logbans (*psv_logbans)
#define sv_allow_upload (*psv_allow_upload)
#define sv_max_upload (*psv_max_upload)
#define hpk_maxsize (*phpk_maxsize)
#define sv_visiblemaxplayers (*psv_visiblemaxplayers)
#define max_queries_sec (*pmax_queries_sec)
#define max_queries_sec_global (*pmax_queries_sec_global)
#define max_queries_window (*pmax_queries_window)
#define sv_logblocks (*psv_logblocks)
#define sv_downloadurl (*psv_downloadurl)
#define sv_allow_dlfile (*psv_allow_dlfile)
#define sv_version (*psv_version)
#define sv_playermodel (*psv_playermodel)

#define outputbuf (*poutputbuf)
#define sv_redirected (*psv_redirected)
#define sv_redirectto (*psv_redirectto)

#define sv_rcon_minfailures (*psv_rcon_minfailures)
#define sv_rcon_maxfailures (*psv_rcon_maxfailures)
#define sv_rcon_minfailuretime (*psv_rcon_minfailuretime)
#define sv_rcon_banpenalty (*psv_rcon_banpenalty)

#define scr_downloading (*pscr_downloading)

#define g_bCS_CZ_Flags_Initialized (*pg_bCS_CZ_Flags_Initialized)
#define g_bIsCZero (*pg_bIsCZero)
#define g_bIsCZeroRitual (*pg_bIsCZeroRitual)
#define g_bIsTerrorStrike (*pg_bIsTerrorStrike)
#define g_bIsTFC (*pg_bIsTFC)
#define g_bIsHL1 (*pg_bIsHL1)
#define g_bIsCStrike (*pg_bIsCStrike)

#define fatbytes (*pfatbytes)
#define fatpvs (*pfatpvs)
#define fatpasbytes (*pfatpasbytes)
#define fatpas (*pfatpas)
#define gPacketSuppressed (*pgPacketSuppressed)
#define giNextUserMsg (*pgiNextUserMsg)
#define hashstrings_collisions (*phashstrings_collisions)

#define g_pplayerdelta (*pg_pplayerdelta)
#define g_pentitydelta (*pg_pentitydelta)
#define g_pcustomentitydelta (*pg_pcustomentitydelta)
#define g_pclientdelta (*pg_pclientdelta)
#define g_pweapondelta (*pg_pweapondelta)

#define localinfo (*plocalinfo)
#define localmodels (*plocalmodels)

#define ipfilters (*pipfilters)
#define numipfilters (*pnumipfilters)
#define userfilters (*puserfilters)
#define numuserfilters (*pnumuserfilters)

#define g_rg_sv_challenges (*pg_rg_sv_challenges)

#define g_svdeltacallback (*pg_svdeltacallback)

#endif // HOOK_ENGINE


extern char *pr_strings;
extern char *gNullString;
extern qboolean scr_skipupdate;
extern float scr_centertime_off;
extern float g_LastScreenUpdateTime;

extern int SV_UPDATE_BACKUP;
extern int SV_UPDATE_MASK;

extern globalvars_t gGlobalVariables;
extern server_static_t g_psvs;
extern server_t g_psv;

extern rehlds_server_t g_rehlds_sv;

extern cvar_t sv_lan;
extern cvar_t sv_lan_rate;
extern cvar_t sv_aim;

extern cvar_t sv_skycolor_r;
extern cvar_t sv_skycolor_g;
extern cvar_t sv_skycolor_b;
extern cvar_t sv_skyvec_x;
extern cvar_t sv_skyvec_y;
extern cvar_t sv_skyvec_z;
extern cvar_t sv_skyname;

extern cvar_t sv_spectatormaxspeed;
extern cvar_t sv_airaccelerate;
extern cvar_t sv_wateraccelerate;
extern cvar_t sv_waterfriction;
extern cvar_t sv_zmax;
extern cvar_t sv_wateramp;
extern cvar_t mapcyclefile;
extern cvar_t motdfile;
extern cvar_t servercfgfile;
extern cvar_t lservercfgfile;
extern cvar_t logsdir;
extern cvar_t bannedcfgfile;

extern decalname_t sv_decalnames[512];
extern int sv_decalnamecount;

extern UserMsg *sv_gpNewUserMsgs;
extern UserMsg *sv_gpUserMsgs;
extern playermove_t g_svmove;

extern int sv_lastnum;
extern extra_baselines_t g_sv_instance_baselines;
extern qboolean g_bOutOfDateRestart;
extern int g_userid;

extern delta_info_t *g_sv_delta;
extern delta_t *g_peventdelta;

extern cvar_t rcon_password;
extern cvar_t sv_enableoldqueries;
extern cvar_t sv_instancedbaseline;
extern cvar_t sv_contact;
extern cvar_t sv_maxupdaterate;
extern cvar_t sv_minupdaterate;
extern cvar_t sv_filterban;
extern cvar_t sv_minrate;
extern cvar_t sv_maxrate;
extern cvar_t sv_logrelay;
extern cvar_t violence_hblood;
extern cvar_t violence_ablood;
extern cvar_t violence_hgibs;
extern cvar_t violence_agibs;
extern cvar_t sv_newunit;
extern cvar_t sv_clienttrace;
extern cvar_t sv_timeout;
extern cvar_t sv_failuretime;

extern cvar_t sv_cheats;
extern cvar_t sv_password;
extern cvar_t sv_proxies;
extern cvar_t sv_outofdatetime;
extern cvar_t mapchangecfgfile;

extern qboolean allow_cheats;
extern cvar_t mp_logecho;
extern cvar_t mp_logfile;
extern cvar_t sv_allow_download;
extern cvar_t sv_send_logos;
extern cvar_t sv_send_resources;
extern cvar_t sv_log_singleplayer;
extern cvar_t sv_logsecret;
extern cvar_t sv_log_onefile;
extern cvar_t sv_logbans;
extern cvar_t sv_allow_upload;
extern cvar_t sv_max_upload;
extern cvar_t hpk_maxsize;
extern cvar_t sv_visiblemaxplayers;
extern cvar_t max_queries_sec;
extern cvar_t max_queries_sec_global;
extern cvar_t max_queries_window;
extern cvar_t sv_logblocks;
extern cvar_t sv_downloadurl;
extern cvar_t sv_allow_dlfile;
extern cvar_t sv_version;
extern int sv_playermodel;

extern char outputbuf[MAX_ROUTEABLE_PACKET];
extern redirect_t sv_redirected;
extern netadr_t sv_redirectto;

extern cvar_t sv_rcon_minfailures;
extern cvar_t sv_rcon_maxfailures;
extern cvar_t sv_rcon_minfailuretime;
extern cvar_t sv_rcon_banpenalty;

extern cvar_t scr_downloading;

extern int g_bCS_CZ_Flags_Initialized;
extern int g_bIsCZero;
extern int g_bIsCZeroRitual;
extern int g_bIsTerrorStrike;
extern int g_bIsTFC;
extern int g_bIsHL1;
extern int g_bIsCStrike;

extern int fatbytes;
extern int giNextUserMsg;
extern int hashstrings_collisions;

extern delta_t *g_pplayerdelta;
extern delta_t *g_pentitydelta;
extern delta_t *g_pcustomentitydelta;
extern delta_t *g_pclientdelta;
extern delta_t *g_pweapondelta;
#ifdef REHLDS_OPT_PEDANTIC
extern delta_t *g_pusercmddelta;
#endif


extern unsigned char fatpvs[1024];
extern int fatpasbytes;
extern unsigned char fatpas[1024];

extern int gPacketSuppressed;

extern char localinfo[MAX_INFO_STRING * 128];
extern char localmodels[512][5];

extern ipfilter_t ipfilters[32768];
extern int numipfilters;
extern userfilter_t userfilters[32768];
extern int numuserfilters;

extern challenge_t g_rg_sv_challenges[1024];


#ifdef HOOK_ENGINE
#define g_rgRconFailures (*pg_rgRconFailures)
#endif // HOOK_ENGINE

extern rcon_failure_t g_rgRconFailures[32];
extern deltacallback_t g_svdeltacallback;

delta_t *SV_LookupDelta(char *name);
NOXREF void SV_DownloadingModules(void);
void SV_GatherStatistics(void);
void SV_DeallocateDynamicData(void);
void SV_ReallocateDynamicData(void);
void SV_AllocClientFrames(void);
qboolean SV_IsPlayerIndex(int index);
qboolean SV_IsPlayerIndex_wrapped(int index);
void SV_ClearPacketEntities(client_frame_t *frame);
void SV_AllocPacketEntities(client_frame_t *frame, int numents);
void SV_ClearFrames(client_frame_t ** frames);
void SV_Serverinfo_f(void);
void SV_Localinfo_f(void);
void SV_User_f(void);
void SV_Users_f(void);
void SV_CountPlayers(int *clients);
void SV_CountProxies(int *proxies);
void SV_FindModelNumbers(void);
void SV_StartParticle(const vec_t *org, const vec_t *dir, int color, int count);
void SV_StartSound(int recipients, edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch);
void SV_StartSound_internal(int recipients, edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch);
qboolean SV_BuildSoundMsg(edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch, const float *origin, sizebuf_t *buffer);
int SV_HashString(const char *string, int iBounds);
int SV_LookupSoundIndex(const char *sample);
void SV_BuildHashedSoundLookupTable(void);
void SV_AddSampleToHashedLookupTable(const char *pszSample, int iSampleIndex);
qboolean SV_ValidClientMulticast(client_t *client, int soundLeaf, int to);
void SV_Multicast(edict_t *ent, vec_t *origin, int to, qboolean reliable);
void SV_WriteMovevarsToClient(sizebuf_t *message);
void SV_WriteDeltaDescriptionsToClient(sizebuf_t *msg);
void SV_SetMoveVars(void);
void SV_QueryMovevarsChanged(void);
void SV_SendServerinfo(sizebuf_t *msg, client_t *client);
void SV_SendServerinfo_internal(sizebuf_t *msg, client_t *client);
void SV_SendResources(sizebuf_t *msg);
void SV_WriteClientdataToMessage(client_t *client, sizebuf_t *msg);
void SV_WriteSpawn(sizebuf_t *msg);
void SV_SendUserReg(sizebuf_t *msg);
void SV_New_f(void);
void SV_SendRes_f(void);
void SV_Spawn_f(void);
void SV_CheckUpdateRate(double *rate);
void SV_RejectConnection(netadr_t *adr, char *fmt, ...);
void SV_RejectConnectionForPassword(netadr_t *adr);
int SV_GetFragmentSize(void *state);
qboolean SV_FilterUser(USERID_t *userid);
int SV_CheckProtocol(netadr_t *adr, int nProtocol);
int SV_CheckProtocol_internal(netadr_t *adr, int nProtocol);
bool SV_CheckChallenge_api(const netadr_t &adr, int nChallengeValue);
int SV_CheckChallenge(netadr_t *adr, int nChallengeValue);
int SV_CheckIPRestrictions(netadr_t *adr, int nAuthProtocol);
int SV_CheckIPRestrictions_internal(netadr_t *adr, int nAuthProtocol);
int SV_CheckIPConnectionReuse(netadr_t *adr);
int SV_FinishCertificateCheck(netadr_t *adr, int nAuthProtocol, char *szRawCertificate, char *userinfo);
int SV_FinishCertificateCheck_internal(netadr_t *adr, int nAuthProtocol, char *szRawCertificate, char *userinfo);
int SV_CheckKeyInfo(netadr_t *adr, char *protinfo, short unsigned int *port, int *pAuthProtocol, char *pszRaw, char *cdkey);
int SV_CheckKeyInfo_internal(netadr_t *adr, char *protinfo, short unsigned int *port, int *pAuthProtocol, char *pszRaw, char *cdkey);
int SV_CheckForDuplicateSteamID(client_t *client);
int SV_CheckForDuplicateNames(char *userinfo, qboolean bIsReconnecting, int nExcludeSlot);
int SV_CheckUserInfo(netadr_t *adr, char *userinfo, qboolean bIsReconnecting, int nReconnectSlot, char *name);
int SV_FindEmptySlot(netadr_t *adr, int *pslot, client_t ** ppClient);
void SV_ConnectClient(void);
void SV_ConnectClient_internal(void);
void SVC_Ping(void);
void SVC_GetChallenge(void);
void SVC_ServiceChallenge(void);
void SV_ResetModInfo(void);
int SV_GetFakeClientCount(void);
NOXREF qboolean SV_GetModInfo(char *pszInfo, char *pszDL, int *version, int *size, qboolean *svonly, qboolean *cldll, char *pszHLVersion);
NOXREF qboolean RequireValidChallenge(netadr_t *adr);
NOXREF qboolean ValidInfoChallenge(netadr_t *adr, const char *nugget);
NOXREF int GetChallengeNr(netadr_t *adr);
NOXREF qboolean CheckChallengeNr(netadr_t *adr, int nChallengeValue);
NOXREF void ReplyServerChallenge(netadr_t *adr);
NOXREF qboolean ValidChallenge(netadr_t *adr, int challengeNr);
NOXREF void SVC_InfoString(void);
NOXREF void SVC_Info(qboolean bDetailed);
NOXREF void SVC_PlayerInfo(void);
NOXREF void SVC_RuleInfo(void);
int SVC_GameDllQuery(const char *s);
void SV_FlushRedirect(void);
void SV_EndRedirect(void);
void SV_BeginRedirect(redirect_t rd, netadr_t *addr);
void SV_ResetRcon_f(void);
void SV_AddFailedRcon(netadr_t *adr);
qboolean SV_CheckRconFailure(netadr_t *adr);
int SV_Rcon_Validate(void);
void SV_Rcon(netadr_t *net_from_);
void SV_ConnectionlessPacket(void);
void SV_CheckRate(client_t *cl);
void SV_ProcessFile(client_t *cl, char *filename);
qboolean SV_FilterPacket(void);
void SV_SendBan(void);
void SV_ReadPackets(void);
//NOBODY int ntohl(void);
//NOBODY int htons(void);
void SV_CheckTimeouts(void);
int SV_CalcPing(client_t *cl);
void SV_FullClientUpdate(client_t *cl, sizebuf_t *sb);
void SV_EmitEvents(client_t *cl, packet_entities_t *pack, sizebuf_t *msg);
void SV_EmitEvents_internal(client_t *cl, packet_entities_t *pack, sizebuf_t *msg);
void SV_AddToFatPVS(vec_t *org, mnode_t *node);
unsigned char *SV_FatPVS(float *org);
void SV_AddToFatPAS(vec_t *org, mnode_t *node);
unsigned char *SV_FatPAS(float *org);
int SV_PointLeafnum(vec_t *p);
void TRACE_DELTA(char *fmt, ...);
void SV_SetCallback(int num, qboolean remove, qboolean custom, int *numbase, qboolean full, int offset);
void SV_SetNewInfo(int newblindex);
void SV_WriteDeltaHeader(int num, qboolean remove, qboolean custom, int *numbase, qboolean newbl, int newblindex, qboolean full, int offset);
void SV_InvokeCallback(void);
int SV_FindBestBaseline(int index, entity_state_t ** baseline, entity_state_t *to, int num, qboolean custom);
int SV_CreatePacketEntities(sv_delta_t type, client_t *client, packet_entities_t *to, sizebuf_t *msg);
void SV_EmitPacketEntities(client_t *client, packet_entities_t *to, sizebuf_t *msg);
qboolean SV_ShouldUpdatePing(client_t *client);
NOXREF qboolean SV_HasEventsInQueue(client_t *client);
void SV_GetNetInfo(client_t *client, int *ping, int *packet_loss);
int SV_CheckVisibility(edict_t *entity, unsigned char *pset);
void SV_EmitPings(client_t *client, sizebuf_t *msg);
void SV_WriteEntitiesToClient(client_t *client, sizebuf_t *msg);
void SV_CleanupEnts(void);
qboolean SV_SendClientDatagram(client_t *client);
void SV_UpdateToReliableMessages(void);
void SV_SkipUpdates(void);
void SV_SendClientMessages(void);
void SV_ExtractFromUserinfo(client_t *cl);
int SV_ModelIndex(const char *name);
void SV_AddResource(resourcetype_t type, const char *name, int size, unsigned char flags, int index);
void SV_CreateGenericResources(void);
void SV_CreateResourceList(void);
void SV_ClearCaches(void);
void SV_PropagateCustomizations(void);
void SV_WriteVoiceCodec(sizebuf_t *pBuf);
void SV_CreateBaseline(void);
void SV_BroadcastCommand(char *fmt, ...);
void SV_BuildReconnect(sizebuf_t *msg);
NOXREF void SV_ReconnectAllClients(void);
void SetCStrikeFlags(void);
void SV_ActivateServer(int runPhysics);
void SV_ServerShutdown(void);
int SV_SpawnServer(qboolean bIsDemo, char *server, char *startspot);
void SV_LoadEntities(void);
void SV_ClearEntities(void);
int RegUserMsg(const char *pszName, int iSize);
qboolean StringToFilter(const char *s, ipfilter_t *f);
USERID_t *SV_StringToUserID(const char *str);
void SV_BanId_f(void);
void Host_Kick_f(void);
void SV_RemoveId_f(void);
void SV_WriteId_f(void);
void SV_ListId_f(void);
void SV_AddIP_f(void);
void SV_RemoveIP_f(void);
void SV_ListIP_f(void);
void SV_WriteIP_f(void);
void SV_KickPlayer(int nPlayerSlot, int nReason);
void SV_InactivateClients(void);
void SV_FailDownload(const char *filename);
const char *Q_stristr(const char *pStr, const char *pSearch);
qboolean IsSafeFileToDownload(const char *filename);
void SV_BeginFileDownload_f(void);
void SV_SetMaxclients(void);
void SV_HandleRconPacket(void);
void SV_CheckCmdTimes(void);
void SV_CheckForRcon(void);
qboolean SV_IsSimulating(void);
void SV_CheckMapDifferences(void);
void SV_Frame(void);
void SV_Drop_f(void);
void SV_RegisterDelta(char *name, char *loadfile);
void SV_InitDeltas(void);
void SV_InitEncoders(void);
void SV_Init(void);
void SV_Shutdown(void);
qboolean SV_CompareUserID(USERID_t *id1, USERID_t *id2);
qboolean SV_CompareUserID_internal(USERID_t *id1, USERID_t *id2);
char *SV_GetIDString(USERID_t *id);
char *SV_GetIDString_internal(USERID_t *id);
char *SV_GetClientIDString(client_t *client);
int GetGameAppID(void);
qboolean IsGameSubscribed(const char *gameName);
NOXREF qboolean BIsValveGame(void);

#endif // SERVER_H
