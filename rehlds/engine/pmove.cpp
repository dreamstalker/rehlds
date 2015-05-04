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




playermove_t *pmove;
movevars_t movevars;

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

cvar_t pm_showclip = { "pm_showclip", "0", 0, 0.0f, NULL };
vec_t player_mins[4][3] = {
		{ -16.0f, -16.0f, -36.0f, },
		{ -16.0f, -16.0f, -18.0f, },
		{ 0.0f, 0.0f, 0.0f, },
		{ -32.0f, -32.0f, -32.0f, }
};
vec_t player_maxs[4][3] = {
		{ 16.0f, 16.0f, 36.0f, },
		{ 16.0f, 16.0f, 18.0f, },
		{ 0.0f, 0.0f, 0.0f, },
		{ 32.0f, 32.0f, 32.0f, }
};
#else //HOOK_ENGINE

cvar_t pm_showclip;
vec_t player_mins[4][3]; // vec_t player_mins[4][3];
vec_t player_maxs[4][3]; // vec_t player_maxs[4][3];

#endif //HOOK_ENGINE

/* <69bcf> ../engine/pmove.c:36 */
qboolean PM_AddToTouched(pmtrace_t tr, vec_t *impactvelocity)
{
	int i;

	for (i = 0; i < pmove->numtouch; i++)
	{
		if (pmove->touchindex[i].ent == tr.ent)
		{
			return FALSE;
		}
	}

	if (pmove->numtouch >= MAX_PHYSENTS)
	{
		pmove->Con_DPrintf("Too many entities were touched!\n");
#ifdef REHLDS_FIXES
		return FALSE;	// FIXED: added for preventing buffer overrun
#endif
	}

	tr.deltavelocity[0] = impactvelocity[0];
	tr.deltavelocity[1] = impactvelocity[1];
	tr.deltavelocity[2] = impactvelocity[2];

	pmove->touchindex[pmove->numtouch++] = tr;

	return TRUE;
}

/* <69b96> ../engine/pmove.c:63 */
void PM_StuckTouch(int hitent, pmtrace_t *ptraceresult)
{
#ifdef REHLDS_CHECKS
	if (hitent >= MAX_PHYSENTS)	// FIXED: added for preventing buffer overrun
	{
		return;
	}
#endif

	if (pmove->server)
	{
		int n = pmove->physents[hitent].info;
		edict_t *info = EDICT_NUM(n);	// looks like just entity index check
		PM_AddToTouched(*ptraceresult, pmove->velocity);
	}
}

/* <69c7e> ../engine/pmove.c:79 */
void PM_Init(playermove_t *ppm)
{
	PM_InitBoxHull();
	for (int i = 0; i < 4; i++)
	{
		ppm->_player_mins[i][0] = player_mins[i][0];
		ppm->_player_mins[i][1] = player_mins[i][1];
		ppm->_player_mins[i][2] = player_mins[i][2];
		ppm->_player_maxs[i][0] = player_maxs[i][0];
		ppm->_player_maxs[i][1] = player_maxs[i][1];
		ppm->_player_maxs[i][2] = player_maxs[i][2];
	}

	ppm->_movevars = &movevars;

#ifdef HOOK_ENGINE
	*(size_t*)&ppm->PM_Info_ValueForKey = (size_t)GetOriginalFuncAddrOrDefault("Info_ValueForKey", (void *)Info_ValueForKey);
	*(size_t*)&ppm->PM_Particle = (size_t)GetOriginalFuncAddrOrDefault("CL_Particle", (void *)CL_Particle);
	*(size_t*)&ppm->PM_TestPlayerPosition = (size_t)GetOriginalFuncAddrOrDefault("PM_TestPlayerPosition", (void *)PM_TestPlayerPosition);
	*(size_t*)&ppm->Con_NPrintf = (size_t)GetFuncRefAddrOrDefault("Con_NPrintf", (void *)Con_NPrintf);
	*(size_t*)&ppm->Con_DPrintf = (size_t)GetFuncRefAddrOrDefault("Con_DPrintf", (void *)Con_DPrintf);
	*(size_t*)&ppm->Con_Printf = (size_t)GetFuncRefAddrOrDefault("Con_Printf", (void *)Con_Printf);
	*(size_t*)&ppm->Sys_FloatTime = (size_t)GetOriginalFuncAddrOrDefault("Sys_FloatTime", (void *)Sys_FloatTime);
	*(size_t*)&ppm->PM_StuckTouch = (size_t)GetOriginalFuncAddrOrDefault("PM_StuckTouch", (void *)PM_StuckTouch);
	*(size_t*)&ppm->PM_PointContents = (size_t)GetOriginalFuncAddrOrDefault("PM_PointContents", (void *)PM_PointContents);
	*(size_t*)&ppm->PM_TruePointContents = (size_t)GetOriginalFuncAddrOrDefault("PM_TruePointContents", (void *)PM_TruePointContents);
	*(size_t*)&ppm->PM_HullPointContents = (size_t)GetOriginalFuncAddrOrDefault("PM_HullPointContents", (void *)PM_HullPointContents);
	*(size_t*)&ppm->PM_PlayerTrace = (size_t)GetOriginalFuncAddrOrDefault("PM_PlayerTrace", (void *)PM_PlayerTrace);
	*(size_t*)&ppm->PM_TraceLine = (size_t)GetOriginalFuncAddrOrDefault("PM_TraceLine", (void *)PM_TraceLine);
	*(size_t*)&ppm->PM_TraceModel = (size_t)GetOriginalFuncAddrOrDefault("PM_TraceModel", (void *)PM_TraceModel);
	*(size_t*)&ppm->RandomLong = (size_t)GetOriginalFuncAddrOrDefault("RandomLong", (void *)RandomLong);
	*(size_t*)&ppm->RandomFloat = (size_t)GetOriginalFuncAddrOrDefault("RandomFloat", (void *)RandomFloat);
	*(size_t*)&ppm->PM_GetModelType = (size_t)GetOriginalFuncAddrOrDefault("PM_GetModelType", (void *)PM_GetModelType);
	*(size_t*)&ppm->PM_HullForBsp = (size_t)GetOriginalFuncAddrOrDefault("PM_HullForBsp", (void *)PM_HullForBsp);
	*(size_t*)&ppm->PM_GetModelBounds = (size_t)GetOriginalFuncAddrOrDefault("PM_GetModelBounds", (void *)PM_GetModelBounds);
	*(size_t*)&ppm->COM_FileSize = (size_t)GetOriginalFuncAddrOrDefault("COM_FileSize", (void *)COM_FileSize);
	*(size_t*)&ppm->COM_LoadFile = (size_t)GetOriginalFuncAddrOrDefault("COM_LoadFile", (void *)COM_LoadFile);
	*(size_t*)&ppm->COM_FreeFile = (size_t)GetOriginalFuncAddrOrDefault("COM_FreeFile", (void *)COM_FreeFile);
	*(size_t*)&ppm->memfgets = (size_t)GetOriginalFuncAddrOrDefault("memfgets", (void *)memfgets);
	*(size_t*)&ppm->PM_PlayerTraceEx = (size_t)GetOriginalFuncAddrOrDefault("PM_PlayerTraceEx", (void *)PM_PlayerTraceEx);
	*(size_t*)&ppm->PM_TestPlayerPositionEx = (size_t)GetOriginalFuncAddrOrDefault("PM_TestPlayerPositionEx", (void *)PM_TestPlayerPositionEx);
	*(size_t*)&ppm->PM_TraceLineEx = (size_t)GetOriginalFuncAddrOrDefault("PM_TraceLineEx", (void *)PM_TraceLineEx);
#else
	ppm->PM_Info_ValueForKey = Info_ValueForKey;
	ppm->PM_Particle = CL_Particle;
	ppm->PM_TestPlayerPosition = PM_TestPlayerPosition;
	ppm->Con_NPrintf = (void(*)(int idx, char *fmt, ...))Con_NPrintf;
	ppm->Con_DPrintf = (void(*)(char *fmt, ...))Con_DPrintf;
	ppm->Con_Printf = (void (*)(char *fmt, ...))Con_Printf;
	ppm->Sys_FloatTime = Sys_FloatTime;
	ppm->PM_StuckTouch = PM_StuckTouch;
	ppm->PM_PointContents = PM_PointContents;
	ppm->PM_TruePointContents = PM_TruePointContents;
	ppm->PM_HullPointContents = PM_HullPointContents;
	ppm->PM_PlayerTrace = PM_PlayerTrace;
	ppm->PM_TraceLine = PM_TraceLine;
	ppm->PM_TraceModel = PM_TraceModel;
	ppm->RandomLong = RandomLong;
	ppm->RandomFloat = RandomFloat;
	ppm->PM_GetModelType = PM_GetModelType;
	ppm->PM_HullForBsp = (void *(__cdecl *)(physent_t *, float *))PM_HullForBsp;
	ppm->PM_GetModelBounds = PM_GetModelBounds;
	ppm->COM_FileSize = COM_FileSize;
	ppm->COM_LoadFile = COM_LoadFile;
	ppm->COM_FreeFile = COM_FreeFile;
	ppm->memfgets = memfgets;
	ppm->PM_PlayerTraceEx = PM_PlayerTraceEx;
	ppm->PM_TestPlayerPositionEx = PM_TestPlayerPositionEx;
	ppm->PM_TraceLineEx = PM_TraceLineEx;
#endif
}
