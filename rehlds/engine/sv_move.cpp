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

/*
* Local initialization
*/
#ifndef HOOK_ENGINE

static int c_yes = 0;
static int c_no = 0;

#else // HOOK_ENGINE

int c_yes;
int c_no;

#endif // HOOK_ENGINE

qboolean SV_CheckBottom(edict_t *ent)
{
	vec3_t mins;
	vec3_t maxs;
	vec3_t start;
	vec3_t stop;
	trace_t trace;
	int x;
	int y;
	float mid;
	float bottom;
	qboolean monsterClip = (ent->v.flags & FL_MONSTERCLIP) ? 1 : 0;

	_VectorAdd(ent->v.origin, ent->v.mins, mins);
	_VectorAdd(ent->v.origin, ent->v.maxs, maxs);

	start[2] = mins[2] - 1;
	for (x = 0; x <= 1; x++)
	{
		for (y = 0; y <= 1; y++)
		{
			start[0] = x ? maxs[0] : mins[0];
			start[1] = y ? maxs[1] : mins[1];

			g_groupmask = ent->v.groupinfo;

			if (SV_PointContents(start) != CONTENTS_SOLID)
				goto realcheck;
		}
	}
	++c_yes;
	return 1;

realcheck:
	++c_no;
	start[2] = mins[2];

	start[0] = stop[0] = (mins[0] + maxs[0]) * 0.5f;
	start[1] = stop[1] = (mins[1] + maxs[1]) * 0.5f;
	stop[2] = start[2] - 2 * sv_stepsize.value;
	trace = SV_Move(start, vec3_origin, vec3_origin, stop, 1, ent, monsterClip);

	if (trace.fraction == 1.0f)
		return 0;

	mid = bottom = trace.endpos[2];

	for (x = 0; x <= 1; x++)
	{
		for (y = 0; y <= 1; y++)
		{
			start[0] = stop[0] = x ? maxs[0] : mins[0];
			start[1] = stop[1] = y ? maxs[1] : mins[1];

			trace = SV_Move(start, vec3_origin, vec3_origin, stop, 1, ent, monsterClip);

			if (trace.fraction != 1.0f && trace.endpos[2] > bottom)
				bottom = trace.endpos[2];
			if (trace.fraction == 1.0f || mid - trace.endpos[2] > sv_stepsize.value)
				return 0;
		}
	}

	return 1;
}

qboolean SV_movetest(edict_t *ent, vec_t *move, qboolean relink)
{
	vec3_t oldorg;
	vec3_t neworg;
	vec3_t end;
	trace_t trace;

	oldorg[0] = ent->v.origin[0];
	oldorg[1] = ent->v.origin[1];
	oldorg[2] = ent->v.origin[2];

	neworg[0] = ent->v.origin[0] + move[0];
	neworg[1] = ent->v.origin[1] + move[1];
	neworg[2] = ent->v.origin[2] + move[2];
	end[0] = neworg[0];
	end[1] = neworg[1];

	neworg[2] += sv_stepsize.value;
	end[2] = neworg[2] - (2 * sv_stepsize.value);
	trace = SV_MoveNoEnts(neworg, ent->v.mins, ent->v.maxs, end, 0, ent);
	if (trace.allsolid)
		return 0;

	if (trace.startsolid)
	{
		neworg[2] -= sv_stepsize.value;
		trace = SV_MoveNoEnts(neworg, ent->v.mins, ent->v.maxs, end, 0, ent);
		if (trace.allsolid || trace.startsolid)
			return 0;
	}
	if (trace.fraction == 1.0f)
	{
		if (ent->v.flags & FL_PARTIALGROUND)
		{
			ent->v.origin[0] = *move + ent->v.origin[0];
			ent->v.origin[1] = ent->v.origin[1] + move[1];
			ent->v.origin[2] = ent->v.origin[2] + move[2];
			if (relink)
				SV_LinkEdict(ent, TRUE);

			ent->v.flags &= ~FL_ONGROUND;
			return 1;
		}
		return 0;
	}

	ent->v.origin[0] = trace.endpos[0];
	ent->v.origin[1] = trace.endpos[1];
	ent->v.origin[2] = trace.endpos[2];
	if (SV_CheckBottom(ent) == 0)
	{
		if (!(ent->v.flags & FL_PARTIALGROUND))
		{
			ent->v.origin[0] = oldorg[0];
			ent->v.origin[1] = oldorg[1];
			ent->v.origin[2] = oldorg[2];
			return 0;
		}
	}
	else
	{
		if (ent->v.flags & FL_PARTIALGROUND)
		{
			ent->v.flags &= ~FL_PARTIALGROUND;
		}
		ent->v.groundentity = trace.ent;
	}

	if (relink)
		SV_LinkEdict(ent, TRUE);

	return 1;
}

qboolean SV_movestep(edict_t *ent, vec_t *move, qboolean relink)
{
	trace_t trace;
	vec3_t end;
	vec3_t oldorg;
	float dz;
	qboolean monsterClipBrush;
	vec3_t start;

	oldorg[0] = ent->v.origin[0];
	oldorg[1] = ent->v.origin[1];
	oldorg[2] = ent->v.origin[2];

	start[0] = ent->v.origin[0] + move[0];
	start[1] = ent->v.origin[1] + move[1];
	start[2] = ent->v.origin[2] + move[2];
	monsterClipBrush = (ent->v.flags & FL_MONSTERCLIP) != 0;
	if (ent->v.flags & (FL_FLY | FL_SWIM))
	{
		int i = 0;
		while (i < 2)
		{
			start[0] = ent->v.origin[0] + move[0];
			start[1] = ent->v.origin[1] + move[1];
			start[2] = ent->v.origin[2] + move[2];
			edict_t* enemy = ent->v.enemy;


			if (i == 0 && enemy)
			{
				dz = ent->v.origin[2] - enemy->v.origin[2];
				if (dz > 40.0)
					start[2] = start[2] - 8.0;
				if (dz < 30.0)
					start[2] = start[2] + 8.0;
			}
			trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, start, 0, ent, monsterClipBrush);
			if (trace.fraction == 1.0f)
				break;

			if (!enemy)
				return 0;

			if (i == 1)
				return 0;

			i++;
		}

		g_groupmask = ent->v.groupinfo;
		if ((ent->v.flags & FL_SWIM) && SV_PointContents(trace.endpos) == -1)
			return 0;

		ent->v.origin[0] = trace.endpos[0];
		ent->v.origin[1] = trace.endpos[1];
		ent->v.origin[2] = trace.endpos[2];
		if (relink)
			SV_LinkEdict(ent, TRUE);

		return 1;
	}

	start[2] += sv_stepsize.value;
	end[0] = start[0];
	end[1] = start[1];
	end[2] = start[2] - (2 * sv_stepsize.value);
	trace = SV_Move(start, ent->v.mins, ent->v.maxs, end, 0, ent, (ent->v.flags & FL_MONSTERCLIP) != 0);
	if (trace.allsolid)
		return 0;

	if (trace.startsolid)
	{
		start[2] = start[2] - sv_stepsize.value;
		trace = SV_Move(start, ent->v.mins, ent->v.maxs, end, 0, ent, monsterClipBrush);
		if (trace.allsolid || trace.startsolid)
			return 0;
	}

	if (trace.fraction != 1.0f)
	{
		ent->v.origin[0] = trace.endpos[0];
		ent->v.origin[1] = trace.endpos[1];
		ent->v.origin[2] = trace.endpos[2];
		if (SV_CheckBottom(ent) == 0)
		{
			if (!(ent->v.flags & FL_PARTIALGROUND))
			{
				ent->v.origin[0] = oldorg[0];
				ent->v.origin[1] = oldorg[1];
				ent->v.origin[2] = oldorg[2];
				return 0;
			}
		}
		else
		{
			if (ent->v.flags & FL_PARTIALGROUND)
				ent->v.flags &= ~FL_PARTIALGROUND;

			ent->v.groundentity = trace.ent;
		}

		if (relink)
			SV_LinkEdict(ent, TRUE);

		return 1;
	}

	if (!(ent->v.flags & FL_PARTIALGROUND))
		return 0;

	ent->v.origin[0] += move[0];
	ent->v.origin[1] += move[1];
	ent->v.origin[2] += move[2];
	if (relink)
		SV_LinkEdict(ent, TRUE);

	ent->v.flags &= ~FL_ONGROUND;
	return 1;
}

qboolean SV_StepDirection(edict_t *ent, float yaw, float dist)
{
	vec3_t move;

	move[0] = cos(yaw * (2 * M_PI) / 360.0) * dist;
	move[1] = sin(yaw * (2 * M_PI) / 360.0) * dist;
	move[2] = 0;
	if (SV_movestep(ent, move, 0))
	{
		SV_LinkEdict(ent, TRUE);
		return 1;
	}
	else
	{
		SV_LinkEdict(ent, TRUE);
		return 0;
	}
}

qboolean SV_FlyDirection(edict_t *ent, vec_t *direction)
{
	if (SV_movestep(ent, direction, 0))
	{
		SV_LinkEdict(ent, TRUE);
		return 1;
	}
	else
	{
		SV_LinkEdict(ent, TRUE);
		return 0;
	}
}

void SV_FixCheckBottom(edict_t *ent)
{
	ent->v.flags |= FL_PARTIALGROUND;
}

NOXREF void SV_NewChaseDir(edict_t *actor, edict_t *enemy, float dist)
{
	NOXREFCHECK;
	float deltax;
	float deltay;
	float d[3];
	float tdir;
	float olddir;
	float turnaround;

	olddir = anglemod(45.0 * (int)(actor->v.ideal_yaw / 45.0));
	turnaround = anglemod(olddir - 180.0);

	deltax = enemy->v.origin[0] - actor->v.origin[0];
	deltay = enemy->v.origin[1] - actor->v.origin[1];

	if (deltax > 10.0)
		d[1] = 0.0;
	else if (deltax <- 10.0)
		d[1]= 180.0;
	else
		d[1]= DI_NODIR;
	if (deltay < -10.0)
		d[2] = 270.0;
	else if (deltay > 10.0)
		d[2] = 90.0;
	else
		d[2]= DI_NODIR;

	if (d[1] != DI_NODIR && d[2] != DI_NODIR)
	{
		if (d[1] == 0.0)
			tdir = d[2] == 90.0 ? 45.0 : 315.0;
		else
			tdir = d[2] == 90.0 ? 135.0 : 215.0;

		if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
			return;
	}

	if (RandomLong(0, 1) ||  abs(deltay) > abs(deltax))
	{
		tdir = d[1];
		d[1] = d[2];
		d[2] = tdir;
	}

	if (d[1] != DI_NODIR && d[1] != turnaround && SV_StepDirection(actor, d[1], dist))
		return;

	if (d[2] != DI_NODIR && d[2] != turnaround && SV_StepDirection(actor, d[2], dist))
		return;

	if (olddir != DI_NODIR && SV_StepDirection(actor, olddir, dist))
		return;

	if (RandomLong(0, 1))
	{
		for (tdir = 0.0; tdir <= 315.0; tdir += 45.0)
			if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
					return;
	}
	else
	{
		for (tdir = 315.0 ; tdir >= 0.0; tdir -= 45.0)
			if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
					return;
	}

	if (turnaround != DI_NODIR && SV_StepDirection(actor, turnaround, dist))
		return;

	actor->v.ideal_yaw = olddir;

	if (!SV_CheckBottom(actor))
		SV_FixCheckBottom(actor);
}

NOXREF qboolean SV_CloseEnough(edict_t *ent, edict_t *goal, float dist)
{
	NOXREFCHECK;
	int i;
	for (i = 0; i < 3; i++)
	{
		if (goal->v.absmin[i] > ent->v.absmax[i] + dist)
			return FALSE;
		if (goal->v.absmax[i] < ent->v.absmin[i] - dist)
			return FALSE;
	}
	return TRUE;
}

NOXREF qboolean SV_ReachedGoal(edict_t *ent, vec_t *vecGoal, float flDist)
{
	NOXREFCHECK;
	int i;
	for (i = 0; i < 3; i++)
	{
		if (vecGoal[i] > ent->v.absmax[i] + flDist)
			return FALSE;
		if (vecGoal[i] < ent->v.absmin[i] - flDist)
			return FALSE;
	}
	return TRUE;
}

void SV_NewChaseDir2(edict_t *actor, vec_t *vecGoal, float dist)
{
	float deltax;
	float deltay;
	float d_1, d_2;
	float tdir;
	float olddir;
	float turnaround;

	olddir = anglemod(45 * (int)(actor->v.ideal_yaw / 45.0));
	turnaround = anglemod(olddir - 180.0);
	deltax = vecGoal[0] - actor->v.origin[0];
	deltay = vecGoal[1] - actor->v.origin[1];

	if (deltax > 10)
		d_1 = 0;
	else if (deltax < -10)
		d_1 = 180;
	else
		d_1 = DI_NODIR;

	if (deltay < -10)
		d_2 = 270;
	else if (deltay > 10)
		d_2 = 90;
	else
		d_2 = DI_NODIR;

	if (d_1 != DI_NODIR && d_2 != DI_NODIR)
	{
		if (d_1 == 0.0)
			tdir = d_2 == 90 ? 45 : 315;
		else
			tdir = d_2 == 90 ? 135 : 215;

		if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
			return;
	}

	if (RandomLong(0, 1) || abs(deltay) > abs(deltax))
	{
		tdir = d_1;
		d_1 = d_2;
		d_2 = tdir;
	}

	if (d_1 != DI_NODIR && d_1 != turnaround
		&& SV_StepDirection(actor, d_1, dist))
		return;

	if (d_2 != DI_NODIR && d_2 != turnaround
		&& SV_StepDirection(actor, d_2, dist))
		return;

	if (olddir != DI_NODIR && SV_StepDirection(actor, olddir, dist))
		return;

	if (RandomLong(0, 1))
	{
		for (tdir = 0; tdir <= 315; tdir += 45)
		{
			if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
				return;
		}
	}
	else
	{
		for (tdir = 315; tdir >= 0; tdir -= 45)
		{
			if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
				return;
		}
	}

	if (turnaround == DI_NODIR || !SV_StepDirection(actor, turnaround, dist))
	{
		actor->v.ideal_yaw = olddir;
		if (!SV_CheckBottom(actor))
			SV_FixCheckBottom(actor);
	}
}

void EXT_FUNC SV_MoveToOrigin_I(edict_t *ent, const float *pflGoal, float dist, int iStrafe)
{
	vec3_t vecGoal;
	vec3_t vecDir;

	vecGoal[0] = pflGoal[0];
	vecGoal[1] = pflGoal[1];
	vecGoal[2] = pflGoal[2];

	if (ent->v.flags & (FL_ONGROUND | FL_SWIM | FL_FLY))
	{
		if (iStrafe)
		{
			vecDir[0] = vecGoal[0] - ent->v.origin[0];
			vecDir[1] = vecGoal[1] - ent->v.origin[1];
			vecDir[2] = vecGoal[2] - ent->v.origin[2];
			if (!(ent->v.flags & (FL_SWIM | FL_FLY)))
				vecDir[2] = 0;

			VectorNormalize(vecDir);
			VectorScale(vecDir, dist, vecDir);
			SV_FlyDirection(ent, vecDir);
		}
		else
		{
			if (!SV_StepDirection(ent, ent->v.ideal_yaw, dist))
				SV_NewChaseDir2(ent, vecGoal, dist);
		}
	}
}
