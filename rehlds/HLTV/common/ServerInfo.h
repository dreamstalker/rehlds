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

typedef struct serverinfo_s
{
	char address[64];
	char name[80];
	char map[16];
	char gamedir[256];
	char description[256];

	int activePlayers;
	int maxPlayers;

	char type; // HLServerType
	char os;
	char pw;
	bool mod;

	char url_info[256];
	char url_dl[256];
	char hlversion[256];

	int ver;
	int size;

	bool svonly;
	bool cldll;

	unsigned char protocol;
} serverinfo_t;

enum HLServerType {
	HLST_Dedicated,
	HLST_NonDedicated,
	HLST_TV,
};

inline const char *GetServerType(HLServerType type)
{
	switch (type)
	{
	case HLST_Dedicated:    return "d";
	case HLST_NonDedicated: return "l";
	case HLST_TV:           return "p";
	}

	return "";
}

inline const char *GetServerOS()
{
#ifdef _WIN32
	return "w";
#else
	return "l";
#endif
}
