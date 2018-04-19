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

// Returns false if any part of the bottom of the entity is off an edge that is not a staircase.
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

	qboolean monsterClip = (ent->v.flags & FL_MONSTERCLIP) ? TRUE : FALSE;

	VectorAdd(ent->v.origin, ent->v.mins, mins);
	VectorAdd(ent->v.origin, ent->v.maxs, maxs);

	// if all of the points under the corners are solid world, don't bother
	// with the tougher checks
	// the corners must be within 16 of the midpoint
	start[2] = mins[2] - 1.0f;

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

	// we got out easy
	return TRUE;

realcheck:

	// check it for real...
	start[2] = mins[2];

	// the midpoint must be within 16 of the bottom
	start[0] = stop[0] = (mins[0] + maxs[0]) * 0.5f;
	start[1] = stop[1] = (mins[1] + maxs[1]) * 0.5f;

	stop[2] = start[2] - 2.0f * sv_stepsize.value;
	trace = SV_Move(start, vec3_origin, vec3_origin, stop, MOVE_NOMONSTERS, ent, monsterClip);

	if (trace.fraction == 1.0f)
		return FALSE;

	mid = bottom = trace.endpos[2];

	// the corners must be within 16 of the midpoint
	for (x = 0; x <= 1; x++)
	{
		for (y = 0; y <= 1; y++)
		{
			start[0] = stop[0] = x ? maxs[0] : mins[0];
			start[1] = stop[1] = y ? maxs[1] : mins[1];

			trace = SV_Move(start, vec3_origin, vec3_origin, stop, MOVE_NOMONSTERS, ent, monsterClip);

			if (trace.fraction != 1.0f && trace.endpos[2] > bottom)
				bottom = trace.endpos[2];

			if (trace.fraction == 1.0f || mid - trace.endpos[2] > sv_stepsize.value)
				return FALSE;
		}
	}

	return TRUE;
}

// Called by monster program code.
// The move will be adjusted for slopes and stairs, but if the move isn't
// possible, no move is done, false is returned, and
// pr_global_struct->trace_normal is set to the normal of the blocking wall
qboolean SV_movetest(edict_t *ent, vec_t *move, qboolean relink)
{
	vec3_t oldorg;
	vec3_t neworg;
	vec3_t end;
	trace_t trace;

	// try the move
	VectorCopy(ent->v.origin, oldorg);
	VectorAdd(ent->v.origin, move, neworg);

	// push down from a step height above the wished position
	neworg[2] += sv_stepsize.value;
	VectorCopy(neworg, end);
	end[2] -= sv_stepsize.value * 2.0f;

	trace = SV_MoveNoEnts(neworg, ent->v.mins, ent->v.maxs, end, MOVE_NORMAL, ent);

	if (trace.allsolid)
		return FALSE;

	if (trace.startsolid)
	{
		neworg[2] -= sv_stepsize.value;
		trace = SV_MoveNoEnts(neworg, ent->v.mins, ent->v.maxs, end, MOVE_NORMAL, ent);

		if (trace.allsolid || trace.startsolid)
			return FALSE;
	}

	if (trace.fraction == 1.0f)
	{
		// if monster had the ground pulled out, go ahead and fall
		if (ent->v.flags & FL_PARTIALGROUND)
		{
			VectorAdd(ent->v.origin, move, ent->v.origin);

			if (relink)
			{
				SV_LinkEdict(ent, TRUE);
			}

			// fall down
			ent->v.flags &= ~FL_ONGROUND;
			return TRUE;
		}

		// walked off an edge
		return FALSE;
	}

	// check point traces down for dangling corners
	VectorCopy(trace.endpos, ent->v.origin);

	if (!SV_CheckBottom(ent))
	{
		if (!(ent->v.flags & FL_PARTIALGROUND))
		{
			VectorCopy(oldorg, ent->v.origin);
			return FALSE;
		}

		// entity had floor mostly pulled out from underneath it
		// and is trying to correct
	}
	else
	{
		if (ent->v.flags & FL_PARTIALGROUND)
		{
			// back on ground
			ent->v.flags &= ~FL_PARTIALGROUND;
		}

		ent->v.groundentity = trace.ent;
	}

	// the move is ok
	if (relink)
	{
		SV_LinkEdict(ent, TRUE);
	}

	return TRUE;
}

// Called by monster program code.
// The move will be adjusted for slopes and stairs, but if the move isn't
// possible, no move is done, false is returned, and
// pr_global_struct->trace_normal is set to the normal of the blocking wall
qboolean SV_movestep(edict_t *ent, vec_t *move, qboolean relink)
{
	trace_t trace;
	vec3_t neworg, oldorg, end;

	// try the move
	VectorCopy(ent->v.origin, oldorg);
	VectorAdd(ent->v.origin, move, neworg);

	qboolean monsterClipBrush = (ent->v.flags & FL_MONSTERCLIP) ? TRUE : FALSE;

	// flying monsters don't step up
	if (ent->v.flags & (FL_FLY | FL_SWIM))
	{
		// try one move with vertical motion, then one without
		for (int i = 0; i < 2; i++)
		{
			VectorAdd(ent->v.origin, move, neworg);

			edict_t *enemy = ent->v.enemy;
			if (i == 0 && enemy)
			{
				float dz = ent->v.origin[2] - enemy->v.origin[2];

				if (dz > 40.0f)
					neworg[2] -= 8.0f;
				else if (dz < 30.0f)
					neworg[2] += 8.0f;
			}

			trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, neworg, MOVE_NORMAL, ent, monsterClipBrush);

			if (trace.fraction == 1.0f)
			{
				g_groupmask = ent->v.groupinfo;

				if ((ent->v.flags & FL_SWIM) && SV_PointContents(trace.endpos) == CONTENTS_EMPTY)
				{
					// swim monster left water
					return FALSE;
				}

				VectorCopy(trace.endpos, ent->v.origin);

				if (relink)
				{
					SV_LinkEdict(ent, TRUE);
				}

				return TRUE;
			}

			if (!enemy)
			{
				break;
			}
		}

		return FALSE;
	}

	// push down from a step height above the wished position
	neworg[2] += sv_stepsize.value;
	VectorCopy(neworg, end);
	end[2] -= sv_stepsize.value * 2.0f;

	trace = SV_Move(neworg, ent->v.mins, ent->v.maxs, end, MOVE_NORMAL, ent, monsterClipBrush);

	if (trace.allsolid)
		return FALSE;

	if (trace.startsolid)
	{
		neworg[2] = neworg[2] - sv_stepsize.value;
		trace = SV_Move(neworg, ent->v.mins, ent->v.maxs, end, MOVE_NORMAL, ent, monsterClipBrush);

		if (trace.allsolid || trace.startsolid)
			return FALSE;
	}

	if (trace.fraction == 1.0f)
	{
		// if monster had the ground pulled out, go ahead and fall
		if (ent->v.flags & FL_PARTIALGROUND)
		{
			VectorAdd(ent->v.origin, move, ent->v.origin);

			if (relink)
			{
				SV_LinkEdict(ent, TRUE);
			}

			// fall down
			ent->v.flags &= ~FL_ONGROUND;
			return TRUE;
		}

		// walked off an edge
		return FALSE;
	}

	// check point traces down for dangling corners
	VectorCopy(trace.endpos, ent->v.origin);

	if (!SV_CheckBottom(ent))
	{
		if (!(ent->v.flags & FL_PARTIALGROUND))
		{
			VectorCopy(oldorg, ent->v.origin);
			return FALSE;
		}

		// entity had floor mostly pulled out from underneath it
		// and is trying to correct
	}
	else
	{
		if (ent->v.flags & FL_PARTIALGROUND)
		{
			// back on ground
			ent->v.flags &= ~FL_PARTIALGROUND;
		}

		ent->v.groundentity = trace.ent;
	}

	// the move is ok
	if (relink)
	{
		SV_LinkEdict(ent, TRUE);
	}

	return TRUE;
}

// Turns to the movement direction, and walks the current distance if facing it.
qboolean SV_StepDirection(edict_t *ent, float yaw, float dist)
{
	vec3_t move;

	yaw = yaw * (M_PI * 2.0f) / 360.0f;

	move[0] = cos(yaw) * dist;
	move[1] = sin(yaw) * dist;
	move[2] = 0.0f;

	if (SV_movestep(ent, move, FALSE))
	{
		SV_LinkEdict(ent, TRUE);
		return TRUE;
	}

	SV_LinkEdict(ent, TRUE);
	return FALSE;
}

qboolean SV_FlyDirection(edict_t *ent, vec_t *direction)
{
	if (SV_movestep(ent, direction, FALSE))
	{
		SV_LinkEdict(ent, TRUE);
		return TRUE;
	}
	else
	{
		SV_LinkEdict(ent, TRUE);
		return FALSE;
	}
}

void SV_FixCheckBottom(edict_t *ent)
{
	ent->v.flags |= FL_PARTIALGROUND;
}

const int DI_NODIR = -1;

NOXREF void SV_NewChaseDir(edict_t *actor, edict_t *enemy, float dist)
{
	NOXREFCHECK;

	SV_NewChaseDir2(actor, enemy->v.origin, dist);
}

NOXREF qboolean SV_CloseEnough(edict_t *ent, edict_t *goal, float dist)
{
	NOXREFCHECK;

	for (int i = 0; i < 3; i++)
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

	for (int i = 0; i < 3; i++)
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
	vec3_t d;
	float deltax, deltay;
	float tempdir, olddir, turnaround;

	olddir = anglemod(45.0f * (int)(actor->v.ideal_yaw / 45.0f));
	turnaround = anglemod(olddir - 180.0f);

	deltax = vecGoal[0] - actor->v.origin[0];
	deltay = vecGoal[1] - actor->v.origin[1];

	if (deltax > 10.0f)
		d[1] = 0.0f;
	else if (deltax < -10.0f)
		d[1] = 180.0f;
	else
		d[1] = DI_NODIR;

	if (deltay < -10.0f)
		d[2] = 270.0f;
	else if (deltay > 10.0f)
		d[2] = 90.0f;
	else
		d[2] = DI_NODIR;

	// try direct route
	if (d[1] != DI_NODIR && d[2] != DI_NODIR)
	{
		if (d[1] == 0.0f)
			tempdir = d[2] == 90.0f ? 45.0f : 315.0f;
		else
			tempdir = d[2] == 90.0f ? 135.0f : 215.0f;

		if (tempdir != turnaround && SV_StepDirection(actor, tempdir, dist))
			return;
	}

	// try other directions
	if (RandomLong(0, 1) || abs(deltay) > abs(deltax))
	{
		tempdir = d[1];
		d[1] = d[2];
		d[2] = tempdir;
	}

	if (d[1] != DI_NODIR && d[1] != turnaround && SV_StepDirection(actor, d[1], dist))
		return;

	if (d[2] != DI_NODIR && d[2] != turnaround && SV_StepDirection(actor, d[2], dist))
		return;

	// there is no direct path to the player, so pick another direction
	if (olddir != DI_NODIR && SV_StepDirection(actor, olddir, dist))
		return;

	// randomly determine direction of search
	if (RandomLong(0, 1))
	{
		for (tempdir = 0.0f; tempdir <= 315.0f; tempdir += 45.0f)
		{
			if (tempdir != turnaround && SV_StepDirection(actor, tempdir, dist))
				return;
		}
	}
	else
	{
		for (tempdir = 315.0f; tempdir >= 0.0f; tempdir -= 45.0f)
		{
			if (tempdir != turnaround && SV_StepDirection(actor, tempdir, dist))
				return;
		}
	}

	// we tried. run backwards. that ought to work...
	if (turnaround != DI_NODIR && SV_StepDirection(actor, turnaround, dist))
		return;

	// can't move, we're stuck somehow
	actor->v.ideal_yaw = olddir;

	// if a bridge was pulled out from underneath a monster, it may not have
	// a valid standing position at all
	if (!SV_CheckBottom(actor))
	{
		SV_FixCheckBottom(actor);
	}
}

void EXT_FUNC SV_MoveToOrigin_I(edict_t *ent, const float *pflGoal, float dist, int iMoveType)
{
	vec3_t vecGoal;
	VectorCopy(pflGoal, vecGoal);

	if (ent->v.flags & (FL_FLY | FL_SWIM | FL_ONGROUND))
	{
		if (iMoveType == MOVE_NORMAL)
		{
			if (!SV_StepDirection(ent, ent->v.ideal_yaw, dist))
			{
				SV_NewChaseDir2(ent, vecGoal, dist);
			}
		}
		else
		{
			vec3_t vecDir;
			VectorSubtract(vecGoal, ent->v.origin, vecDir);

			if (!(ent->v.flags & (FL_SWIM | FL_FLY)))
				vecDir[2] = 0.0f;

			VectorNormalize(vecDir);
			VectorScale(vecDir, dist, vecDir);
			SV_FlyDirection(ent, vecDir);
		}
	}
}
