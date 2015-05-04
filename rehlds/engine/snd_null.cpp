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

/* <84fa2> ../engine/snd_null.c:16 */
void S_Init(void) { }

/* <84fb6> ../engine/snd_null.c:18 */
void S_AmbientOff(void) { }

/* <84fca> ../engine/snd_null.c:20 */
void S_AmbientOn(void) { }

/* <84fde> ../engine/snd_null.c:22 */
void S_Shutdown(void) { }

/* <84ff2> ../engine/snd_null.c:24 */
void S_TouchSound(char *sample) { }

/* <85019> ../engine/snd_null.c:26 */
void S_ClearBuffer(void) { }

/* <8502d> ../engine/snd_null.c:28 */
void S_StartStaticSound(int entnum, int entchannel, sfx_t *sfx, vec_t *origin, float vol, float attenuation, int flags, int pitch) { }

/* <850bc> ../engine/snd_null.c:30 */
void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, vec_t *origin, float fvol, float attenuation, int flags, int pitch) { }

/* <85145> ../engine/snd_null.c:32 */
void S_StopSound(int entnum, int entchannel) { }

/* <8517a> ../engine/snd_null.c:34 */
sfx_t *S_PrecacheSound(char *sample) { return NULL; }

/* <851a5> ../engine/snd_null.c:36 */
void S_ClearPrecache(void) { }

/* <851b9> ../engine/snd_null.c:38 */
void S_Update(vec_t *origin, vec_t *v_forward, vec_t *v_right, vec_t *v_up) { }

/* <8520a> ../engine/snd_null.c:40 */
void S_StopAllSounds(qboolean clear) { }

/* <85231> ../engine/snd_null.c:42 */
void S_BeginPrecaching(void) { }

/* <85245> ../engine/snd_null.c:44 */
void S_EndPrecaching(void) { }

/* <85259> ../engine/snd_null.c:46 */
void S_ExtraUpdate(void) { }

/* <8526d> ../engine/snd_null.c:48 */
void S_LocalSound(char *s) { }

/* <85292> ../engine/snd_null.c:50 */
void S_BlockSound(void) { }

/* <852a6> ../engine/snd_null.c:52 */
void S_PrintStats(void) { }

/* <852ba> ../engine/snd_null.c:55 */
qboolean Voice_RecordStart(const char *pUncompressedFile, const char *pDecompressedFile, const char *pMicInputFile) { return FALSE; }

/* <85301> ../engine/snd_null.c:56 */
qboolean Voice_IsRecording(void) { return FALSE; }

/* <85319> ../engine/snd_null.c:57 */
void Voice_RegisterCvars(void) { }

/* <8532d> ../engine/snd_null.c:58 */
void Voice_Deinit(void) { }

/* <85341> ../engine/snd_null.c:59 */
void Voice_Idle(float frametime) { }

/* <85368> ../engine/snd_null.c:60 */
qboolean Voice_RecordStop(void) { return TRUE; }
