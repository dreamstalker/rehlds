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

typedef struct qpic_s
{
	int width;
	int height;
	byte data[4];
} qpic_t;

typedef struct wadinfo_s
{
	char identification[4];
	int numlumps;
	int infotableofs;
} wadinfo_t;

typedef struct lumpinfo_s
{
	int filepos;
	int disksize;
	int size;
	char type;
	char compression;
	char pad1;
	char pad2;
	char name[16];
} lumpinfo_t;

typedef struct wadlist_s
{
	qboolean loaded;
	char wadname[32];
	int wad_numlumps;
	lumpinfo_t * wad_lumps;
	byte * wad_base;
} wadlist_t;

typedef struct wadlist_s wadlist_t;
typedef struct lumpinfo_s lumpinfo_t;

const int NUM_WADS = 2;

extern wadlist_t wads[NUM_WADS];

void W_CleanupName(char *in, char *out);
int W_LoadWadFile(char *filename);
lumpinfo_t *W_GetLumpinfo(int wad, char *name, qboolean doerror);
void *W_GetLumpName(int wad, char *name);
NOXREF void *W_GetLumpNum(int wad, int num);
void W_Shutdown(void);
void SwapPic(qpic_t *pic);
