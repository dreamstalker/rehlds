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
#include "wad.h"

const int MAX_MAP_TEXTURES = 512;

typedef struct texlumpinfo_s
{
	lumpinfo_t lump;
	int iTexFile;
} texlumpinfo_t;

extern texlumpinfo_t* lumpinfo;
extern int nTexLumps;
extern FILE* texfiles[128];
extern int nTexFiles;

extern unsigned char texgammatable[256];
extern texture_t * r_notexture_mip;

extern int nummiptex;
extern char miptex[512][64];

extern cvar_t r_wadtextures;

void SafeRead(FileHandle_t f, void *buffer, int count);
void CleanupName(char *in, char *out);
int lump_sorter(const void *lump1, const void *lump2);
void ForwardSlashes(char *pname);
qboolean TEX_InitFromWad(char *path);
void TEX_CleanupWadInfo(void);
int TEX_LoadLump(char *name, unsigned char *dest);
int FindMiptex(char *name);
void TEX_AddAnimatingTextures(void);
