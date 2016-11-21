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

void S_Init() { }
void S_AmbientOff() { }
void S_AmbientOn() { }
void S_Shutdown() { }
void S_TouchSound(char *sample) { }
void S_ClearBuffer() { }
void S_StartStaticSound(int entnum, int entchannel, sfx_t *sfx, vec_t *origin, float vol, float attenuation, int flags, int pitch) { }
void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, vec_t *origin, float fvol, float attenuation, int flags, int pitch) { }
void S_StopSound(int entnum, int entchannel) { }
sfx_t *S_PrecacheSound(char *sample) { return NULL; }
void S_ClearPrecache() { }
void S_Update(vec_t *origin, vec_t *v_forward, vec_t *v_right, vec_t *v_up) { }
void S_StopAllSounds(qboolean clear) { }
void S_BeginPrecaching() { }
void S_EndPrecaching() { }
void S_ExtraUpdate() { }
void S_LocalSound(char *s) { }
void S_BlockSound() { }
void S_PrintStats() { }
qboolean Voice_RecordStart(const char *pUncompressedFile, const char *pDecompressedFile, const char *pMicInputFile) { return FALSE; }
qboolean Voice_IsRecording() { return FALSE; }
void Voice_RegisterCvars() { }
void Voice_Deinit() { }
void Voice_Idle(float frametime) { }
qboolean Voice_RecordStop() { return TRUE; }
