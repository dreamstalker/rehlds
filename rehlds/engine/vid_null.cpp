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

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

int r_pixbytes = 1;
cvar_t gl_vsync = { "gl_vsync", "1", 0, 0.0f, NULL };

#else //HOOK_ENGINE

int r_pixbytes;
cvar_t gl_vsync;

#endif //HOOK_ENGINE

/* <d6867> ../engine/vid_null.c:28 */
void VID_SetPalette(unsigned char *palette) { }

/* <d6894> ../engine/vid_null.c:29 */
void VID_ShiftPalette(unsigned char *palette) { }

/* <d68bb> ../engine/vid_null.c:30 */
void VID_WriteBuffer(const char *pFilename) { }

/* <d68e2> ../engine/vid_null.c:32 */
int VID_Init(short unsigned int *palette) { return 1; }

/* <d690d> ../engine/vid_null.c:51 */
void D_FlushCaches(void) { }

/* <d6921> ../engine/vid_null.c:52 */
void R_SetStackBase(void) { }

/* <d6935> ../engine/vid_null.c:53 */
void SCR_UpdateScreen(void) { }

/* <d6949> ../engine/vid_null.c:54 */
void V_Init(void) { }

/* <d695d> ../engine/vid_null.c:56 */
void Draw_Init(void) { }

/* <d6971> ../engine/vid_null.c:57 */
void SCR_Init(void) { }

/* <d6985> ../engine/vid_null.c:58 */
void R_Init(void) { }

void R_ForceCVars(qboolean multiplayer) { }

/* <d6999> ../engine/vid_null.c:59 */
void SCR_BeginLoadingPlaque(qboolean reconnect) { }

/* <d69c0> ../engine/vid_null.c:60 */
void SCR_EndLoadingPlaque(void) { }

/* <d69d4> ../engine/vid_null.c:62 */
void R_InitSky(void) { }

/* <d69e8> ../engine/vid_null.c:63 */
void R_MarkLeaves(void)
{
}

/* <d69fc> ../engine/vid_null.c:78 */
void R_InitTextures(void)
{
	r_notexture_mip = (texture_t *)Hunk_AllocName(404, "notexture");
	r_notexture_mip->height = 16;
	r_notexture_mip->width = 16;
	r_notexture_mip->offsets[0] = 64;
	r_notexture_mip->offsets[1] = 320;
	r_notexture_mip->offsets[2] = 384;
	r_notexture_mip->offsets[3] = 400;

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

/* <d6a48> ../engine/vid_null.c:110 */
void StartLoadingProgressBar(const char *loadingType, int numProgressPoints) { }

/* <d6a7d> ../engine/vid_null.c:114 */
void ContinueLoadingProgressBar(const char *loadingType, int progressPoint, float progressFraction) { }

/* <d6ac0> ../engine/vid_null.c:118 */
void SetLoadingProgressBarStatusText(const char *statusText) { }
