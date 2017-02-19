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
#include "netadr.h"

typedef struct server_log_s
{
	qboolean active;
	qboolean net_log_;
	netadr_t net_address_;
	void *file;
} server_log_t;

typedef struct server_stats_s
{
	int num_samples;
	int at_capacity;
	int at_empty;
	float capacity_percent;
	float empty_percent;
	int minusers;
	int maxusers;
	float cumulative_occupancy;
	float occupancy;
	int num_sessions;
	float cumulative_sessiontime;
	float average_session_len;
	float cumulative_latency;
	float average_latency;
} server_stats_t;

typedef struct server_static_s
{
	qboolean dll_initialized;
	struct client_s *clients;
	int maxclients;
	int maxclientslimit;
	int spawncount;
	int serverflags;
	server_log_t log;
	double next_cleartime;
	double next_sampletime;
	server_stats_t stats;
	qboolean isSecure;
} server_static_t;
