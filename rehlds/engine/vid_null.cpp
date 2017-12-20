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

float scr_con_current;

int r_pixbytes = 1;
cvar_t gl_vsync = { "gl_vsync", "1", 0, 0.0f, NULL };

void VID_SetPalette(unsigned char *palette) { }
void VID_ShiftPalette(unsigned char *palette) { }
void VID_WriteBuffer(const char *pFilename) { }
int VID_Init(unsigned short *palette) { return 1; }
void D_FlushCaches() { }
void R_SetStackBase() { }
void SCR_UpdateScreen() { }
void V_Init() { }
void Draw_Init() { }
void SCR_Init() { }
void R_Init() { }
void R_ForceCVars(qboolean multiplayer) { }
void SCR_BeginLoadingPlaque(qboolean reconnect) { }
void SCR_EndLoadingPlaque() { }
void R_InitSky() { }
void R_MarkLeaves()
{
}

void R_InitTextures()
{
	r_notexture_mip = (texture_t *)Hunk_AllocName(sizeof(texture_t) + 16*16 + 8*8 + 4*4 + 2*2, "notexture");
	r_notexture_mip->height = 16;
	r_notexture_mip->width = 16;
	r_notexture_mip->offsets[0] = sizeof(texture_t);
	r_notexture_mip->offsets[1] = r_notexture_mip->offsets[0] + 16*16;
	r_notexture_mip->offsets[2] = r_notexture_mip->offsets[1] + 8*8;
	r_notexture_mip->offsets[3] = r_notexture_mip->offsets[2] + 4*4;

	for (int m = 0; m < 4; m++)
	{
		int texSize = 16 >> m;
		unsigned char* dest = (unsigned char *)r_notexture_mip + r_notexture_mip->offsets[m];

		for (int x = 0; x < texSize; x++)
		{
			for (int y = 0; y < texSize; y++, dest++)
			{
				if (x < (texSize / 2) == y < (texSize / 2))
					*dest = -1;
				else
					*dest = 0;
			}
		}
	}
}

void StartLoadingProgressBar(const char *loadingType, int numProgressPoints) { }
void ContinueLoadingProgressBar(const char *loadingType, int progressPoint, float progressFraction) { }
void SetLoadingProgressBarStatusText(const char *statusText) { }
