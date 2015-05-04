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

#ifndef SOUND_H
#define SOUND_H
#ifdef _WIN32
#pragma once
#endif

#include "quakedef.h"

// max number of sentences in game. NOTE: this must match CVOXFILESENTENCEMAX in dlls\util.h!!!
#define CVOXFILESENTENCEMAX		1536

/* <2e874> ../engine/sound.h:26 */
typedef struct sfx_s
{
	char name[64];
	cache_user_t cache;
	int servercount;
} sfx_t;

void S_Init(void);
void S_AmbientOff(void);
void S_AmbientOn(void);
void S_Shutdown(void);
void S_TouchSound(char *sample);
void S_ClearBuffer(void);
void S_StartStaticSound(int entnum, int entchannel, sfx_t *sfx, vec_t *origin, float vol, float attenuation, int flags, int pitch);
void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, vec_t *origin, float fvol, float attenuation, int flags, int pitch);
void S_StopSound(int entnum, int entchannel);
sfx_t *S_PrecacheSound(char *sample);
void S_ClearPrecache(void);
void S_Update(vec_t * origin, vec_t * v_forward, vec_t * v_right, vec_t * v_up);
void S_StopAllSounds(qboolean clear);
void S_BeginPrecaching(void);
void S_EndPrecaching(void);
void S_ExtraUpdate(void);
void S_LocalSound(char * s);
void S_BlockSound(void);
void S_PrintStats(void);
qboolean Voice_RecordStart(const char  * pUncompressedFile, const char  * pDecompressedFile, const char  * pMicInputFile);
qboolean Voice_IsRecording(void);
void Voice_RegisterCvars(void);
void Voice_Deinit(void);
void Voice_Idle(float frametime);
qboolean Voice_RecordStop(void);

#endif // SOUND_H