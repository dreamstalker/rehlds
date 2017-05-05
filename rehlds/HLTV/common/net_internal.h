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

#define MAX_VOICEDATA_LEN		4096
#define PROTOCOL_VERSION		48

// all OOB packet start with this sequence
#define CONNECTIONLESS_HEADER		0xFFFFFFFF

// info request
#define S2A_INFO			'C'	// deprecated goldsrc response
#define S2A_INFO_DETAILED		'm'	// New Query protocol, returns dedicated or not, + other performance info.

#define S2A_PROXY_LISTEN		'G'
#define S2A_PROXY_REDIRECT		'L'

// Response details about each player on the server
#define S2A_PLAYERS			'D'

// Response as multi-packeted the rules the server is using
#define S2A_RULES			'E'

/* ------ S2C_* - Server to client ------ */
// Rejected the connection because the password is invalid
#define S2C_REJECT_BADPASSWORD		'8'

// Rejected the connection by explain the reason
#define S2C_REJECT			'9'

// Client connection is initiated by requesting a challenge value
// the server sends this value back
#define S2C_CHALLENGE			'A'	// + challenge value

// Send a userid, client remote address, is this server secure and engine build number
#define S2C_CONNECTION			'B'

// HLMaster rejected a server's connection because the server needs to be updated
#define M2S_REQUESTRESTART		'O'

// Send a log event as key value
#define S2A_LOGSTRING			'R'

// Send a log string
#define S2A_LOGKEY			'S'

// Basic information about the server
#define A2S_INFO			'T'

// Details about each player on the server
#define A2S_PLAYER			'U'

// The rules the server is using
#define A2S_RULES			'V'

/* ------ A2A_* - Another Answer to ------ */
// Another user is requesting a challenge value from this machine
#define A2A_GETCHALLENGE		'W'	// Request challenge # from another machine

// Generic Ping Request
#define	A2A_PING			'i'	// respond with an A2A_ACK

// Generic Ack
#define	A2A_ACK				'j'	// general acknowledgement without info

// Print to client console.
#define	A2A_PRINT			'l'	// print a message on client

// Challenge response from master
#define M2A_CHALLENGE			's'	// + challenge value

// Max size of udp packet payload
#define	MAX_UDP_PACKET	4010		// 9 bytes SPLITHEADER + 4000 payload?

enum svc_commands_e
{
	svc_bad,
	svc_nop,
	svc_disconnect,
	svc_event,
	svc_version,
	svc_setview,
	svc_sound,
	svc_time,
	svc_print,
	svc_stufftext,
	svc_setangle,
	svc_serverinfo,
	svc_lightstyle,
	svc_updateuserinfo,
	svc_deltadescription,
	svc_clientdata,
	svc_stopsound,
	svc_pings,
	svc_particle,
	svc_damage,
	svc_spawnstatic,
	svc_event_reliable,
	svc_spawnbaseline,
	svc_temp_entity,
	svc_setpause,
	svc_signonnum,
	svc_centerprint,
	svc_killedmonster,
	svc_foundsecret,
	svc_spawnstaticsound,
	svc_intermission,
	svc_finale,
	svc_cdtrack,
	svc_restore,
	svc_cutscene,
	svc_weaponanim,
	svc_decalname,
	svc_roomtype,
	svc_addangle,
	svc_newusermsg,
	svc_packetentities,
	svc_deltapacketentities,
	svc_choke,
	svc_resourcelist,
	svc_newmovevars,
	svc_resourcerequest,
	svc_customization,
	svc_crosshairangle,
	svc_soundfade,
	svc_filetxferfailed,
	svc_hltv,
	svc_director,
	svc_voiceinit,
	svc_voicedata,
	svc_sendextrainfo,
	svc_timescale,
	svc_resourcelocation,
	svc_sendcvarvalue,
	svc_sendcvarvalue2,
	svc_startofusermessages = svc_sendcvarvalue2,
	svc_endoflist = 255,
};

enum clc_commands : byte
{
	clc_bad,
	clc_nop,
	clc_move,
	clc_stringcmd,
	clc_delta,
	clc_resourcelist,
	clc_tmove,
	clc_fileconsistency,
	clc_voicedata,
	clc_hltv,
	clc_cvarvalue,
	clc_cvarvalue2,
	clc_endoflist = 255,
};
