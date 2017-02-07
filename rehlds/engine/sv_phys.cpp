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

vec3_t* g_moved_from;
edict_t** g_moved_edict;

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

cvar_t sv_maxvelocity = { "sv_maxvelocity", "2000", 0, 0.0f, NULL };
cvar_t sv_gravity = { "sv_gravity", "800", FCVAR_SERVER, 0.0f, NULL };
cvar_t sv_bounce = { "sv_bounce", "1", FCVAR_SERVER, 0.0f, NULL };
cvar_t sv_stepsize = { "sv_stepsize", "18", FCVAR_SERVER, 0.0f, NULL };
cvar_t sv_friction = { "sv_friction", "4", FCVAR_SERVER, 0.0f, NULL };
cvar_t sv_stopspeed = { "sv_stopspeed", "100", FCVAR_SERVER, 0.0f, NULL };

#else // HOOK_ENGINE

cvar_t sv_maxvelocity;
cvar_t sv_gravity;
cvar_t sv_bounce;
cvar_t sv_stepsize;
cvar_t sv_friction;
cvar_t sv_stopspeed;

#endif // HOOK_ENGINE

NOXREF void SV_CheckAllEnts(void)
{
	NOXREFCHECK;
	int e;
	edict_t *check;

	for (e = 1; e < g_psv.num_edicts; e++)
	{
		check = &g_psv.edicts[e];

		if (check->free)
			continue;

		int movetype = check->v.movetype;
		if (check->v.movetype == MOVETYPE_NONE)
			continue;

		if (movetype != MOVETYPE_PUSH
			&& movetype != MOVETYPE_NOCLIP
			&& movetype != MOVETYPE_FOLLOW)
		{
			if (SV_TestEntityPosition(check) != NULL)
				Con_Printf("entity in invalid position\n");
		}
	}
}

void SV_CheckVelocity(edict_t *ent)
{
	for (int i = 0; i < 3; i++)
	{
		if (IS_NAN(ent->v.velocity[i]))
		{
			Con_Printf("Got a NaN velocity on %s\n", &pr_strings[ent->v.classname]);
			ent->v.velocity[i] = 0;
		}
		if (IS_NAN(ent->v.origin[i]))
		{
			Con_Printf("Got a NaN origin on %s\n", &pr_strings[ent->v.classname]);
			ent->v.origin[i] = 0;
		}

		if (ent->v.velocity[i] > sv_maxvelocity.value)
		{
			Con_DPrintf("Got a velocity too high on %s\n", &pr_strings[ent->v.classname]);
			ent->v.velocity[i] = sv_maxvelocity.value;
		}
		else if (ent->v.velocity[i] < -sv_maxvelocity.value)
		{
			Con_DPrintf("Got a velocity too low on %s\n", &pr_strings[ent->v.classname]);
			ent->v.velocity[i] = -sv_maxvelocity.value;
		}
	}
}

// Runs thinking code if time. There is some play in the exact time the think
// function will be called, because it is called before any movement is done
// in a frame. Not used for pushmove objects, because they must be exact.
// Returns false if the entity removed itself.
qboolean SV_RunThink(edict_t *ent)
{
	float thinktime;

	if (!(ent->v.flags & FL_KILLME))
	{
		thinktime = ent->v.nextthink;
		if (thinktime <= 0.0 || thinktime > g_psv.time + host_frametime)
		{
			return TRUE;
		}
		if (thinktime < g_psv.time)
		{
			thinktime = (float) g_psv.time;		// don't let things stay in the past.
										// it is possible to start that way
										// by a trigger with a local time.
		}
		ent->v.nextthink = 0.0;
		gGlobalVariables.time = thinktime;
		gEntityInterface.pfnThink(ent);
	}

	if (ent->v.flags & FL_KILLME)
	{
		ED_Free(ent);
	}

	return !ent->free;
}

void SV_Impact(edict_t *e1, edict_t *e2, trace_t *ptrace)
{
	gGlobalVariables.time = (float) g_psv.time;
	if ((e1->v.flags & FL_KILLME) || (e2->v.flags & FL_KILLME))
		return;

	if (e1->v.groupinfo && e2->v.groupinfo)
	{
		if (g_groupop)
		{
			if (g_groupop == GROUP_OP_NAND && (e1->v.groupinfo & e2->v.groupinfo))
				return;
		}
		else
		{
			if (!(e1->v.groupinfo & e2->v.groupinfo))
				return;
		}
	}

	if (e1->v.solid)
	{
		SV_SetGlobalTrace(ptrace);
		gEntityInterface.pfnTouch(e1, e2);

#ifdef REHLDS_FIXES
		if ((e1->v.flags & FL_KILLME) || (e2->v.flags & FL_KILLME))
			return;
#endif // REHLDS_FIXES
	}
	if (e2->v.solid)
	{
		SV_SetGlobalTrace(ptrace);
		gEntityInterface.pfnTouch(e2, e1);
	}
}

int ClipVelocity(vec_t *in, vec_t *normal, vec_t *out, float overbounce)
{
	int blocked = 0;
	if (normal[2] > 0.0)
		blocked = 1;
	if (normal[2] == 0.0)
		blocked |= 2u;

	float backoff = _DotProduct(in, normal) * overbounce;
	for (int i = 0; i < 3; i++)
	{
		float tmp = normal[i] * backoff;
		float change = in[i] - tmp;
		out[i] = (change > -0.1f && change < 0.1f) ? 0.0f : change;
	}

	return blocked;
}

int SV_FlyMove(edict_t *ent, float time, trace_t *steptrace)
{
	vec_t planes[5][3];
	int numplanes = 0;

	vec3_t primal_velocity;
	primal_velocity[0] = ent->v.velocity[0];
	primal_velocity[1] = ent->v.velocity[1];
	primal_velocity[2] = ent->v.velocity[2];

	vec3_t original_velocity;
	original_velocity[0] = ent->v.velocity[0];
	original_velocity[1] = ent->v.velocity[1];
	original_velocity[2] = ent->v.velocity[2];

	vec3_t new_velocity;

	qboolean monsterClip = (ent->v.flags & FL_MONSTERCLIP) ? 1 : 0;
	int blocked = 0;
	for (int bumpcount = 0; bumpcount < 4; bumpcount++)
	{
		if (ent->v.velocity[0] == 0.0f && ent->v.velocity[1] == 0.0f && ent->v.velocity[2] == 0.0f)
			break;

		vec3_t end;
		for (int i = 0; i < 3; i++)
			VectorMA(ent->v.origin, time, ent->v.velocity, end);

		trace_t trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, 0, ent, monsterClip);
		if (trace.allsolid)
		{
			ent->v.velocity[0] = vec3_origin[0];
			ent->v.velocity[1] = vec3_origin[1];
			ent->v.velocity[2] = vec3_origin[2];
			return 4;
		}

		if (trace.fraction > 0.0f)
		{
			trace_t test = SV_Move(trace.endpos, ent->v.mins, ent->v.maxs, trace.endpos, 0, ent, monsterClip);
			if (test.allsolid != 1)
			{
				ent->v.origin[0] = trace.endpos[0];
				ent->v.origin[1] = trace.endpos[1];
				ent->v.origin[2] = trace.endpos[2];
				primal_velocity[0] = ent->v.velocity[0];
				primal_velocity[1] = ent->v.velocity[1];
				primal_velocity[2] = ent->v.velocity[2];
				numplanes = 0;
			}
		}

		if (trace.fraction == 1.0f)
			break;

		if (!trace.ent)
			Sys_Error("%s: !trace.ent", __FUNCTION__);

		if (trace.plane.normal[2] > 0.7)
		{
			blocked |= 1u;
			if (trace.ent->v.solid == SOLID_BSP
				|| trace.ent->v.movetype == MOVETYPE_PUSHSTEP
				|| trace.ent->v.solid == SOLID_SLIDEBOX
				|| ent->v.flags & FL_CLIENT)
			{
				ent->v.flags |= FL_ONGROUND;
				ent->v.groundentity = trace.ent;
			}
		}

		if (trace.plane.normal[2] == 0.0)
		{
			blocked |= 2u;
			if (steptrace)
				Q_memcpy(steptrace, &trace, 0x38u);
		}

		SV_Impact(ent, trace.ent, &trace);
		if (ent->free)
			break;

		time = time - trace.fraction * time;
		if (numplanes >= 5)
		{
			ent->v.velocity[0] = vec3_origin[0];
			ent->v.velocity[1] = vec3_origin[1];
			ent->v.velocity[2] = vec3_origin[2];
			return blocked;
		}

		planes[numplanes][0] = trace.plane.normal[0];
		planes[numplanes][1] = trace.plane.normal[1];
		planes[numplanes][2] = trace.plane.normal[2];
		++numplanes;
		if (numplanes != 1
			|| ent->v.movetype != MOVETYPE_WALK
			|| ((ent->v.flags & FL_ONGROUND) && ent->v.friction == 1.0f))
		{
			int i = 0;
			for (; i < numplanes; i++)
			{
				ClipVelocity(primal_velocity, planes[i], new_velocity, 1.0);
				int j = 0;
				for (; j < numplanes; j++)
				{
					if (j == i)
						continue;

					if (_DotProduct(new_velocity, planes[j]) < 0.0f)
						break;
				}
				if (j == numplanes)
					break;
			}
			if (i == numplanes)
			{
				if (numplanes != 2)
					return blocked;

				vec3_t dir;
				CrossProduct(planes[0], planes[1], dir);
				float vscale = _DotProduct(dir, ent->v.velocity);
				VectorScale(dir, vscale, ent->v.velocity);
			}
			else
			{
				ent->v.velocity[1] = new_velocity[1];
				ent->v.velocity[2] = new_velocity[2];
				ent->v.velocity[0] = new_velocity[0];
			}

			if (_DotProduct(original_velocity, ent->v.velocity) <= 0.0f)
			{
				ent->v.velocity[0] = vec3_origin[0];
				ent->v.velocity[1] = vec3_origin[1];
				ent->v.velocity[2] = vec3_origin[2];
				return blocked;
			}
		}
		else
		{
			float d;
			if (planes[0][2] <= 0.7)
			{
				d = (1.0f - ent->v.friction) * sv_bounce.value + 1.0f;
			}
			else
			{
				d = 1.0f;
			}
			ClipVelocity(primal_velocity, planes[0], new_velocity, d);
			ent->v.velocity[0] = new_velocity[0];
			ent->v.velocity[1] = new_velocity[1];
			ent->v.velocity[2] = new_velocity[2];
			primal_velocity[0] = new_velocity[0];
			primal_velocity[1] = new_velocity[1];
			primal_velocity[2] = new_velocity[2];
		}
	}

	return blocked;
}

void SV_AddGravity(edict_t *ent)
{
	float ent_gravity;
	if (ent->v.gravity == 0.0)
		ent_gravity = 1.0;
	else
		ent_gravity = ent->v.gravity;
	ent_gravity = (float)(ent->v.velocity[2] - sv_gravity.value * ent_gravity * host_frametime);
	ent->v.velocity[2] = (float)(ent_gravity + ent->v.basevelocity[2] * host_frametime);
	ent->v.basevelocity[2] = 0;
	SV_CheckVelocity(ent);
}

NOXREF void SV_AddCorrectGravity(edict_t *ent)
{
	NOXREFCHECK;
	float ent_gravity = 1.0f;
	if (ent->v.gravity != 0.0f)
		ent_gravity = ent->v.gravity;

	ent->v.velocity[2] -= (ent_gravity * sv_gravity.value * 0.5f * host_frametime);
	ent->v.velocity[2] += ent->v.basevelocity[2] * host_frametime;

	pmove->basevelocity[2] = 0.0f;
	SV_CheckVelocity(ent);
}

NOXREF void SV_FixupGravityVelocity(edict_t *ent)
{
	NOXREFCHECK;
	float ent_gravity = 1.0f;
	if (ent->v.gravity != 0.0f)
		ent_gravity = ent->v.gravity;

	ent->v.velocity[2] -= (ent_gravity * sv_gravity.value * 0.5f * host_frametime);
	SV_CheckVelocity(ent);
}

trace_t SV_PushEntity(edict_t *ent, vec_t *push)
{
	trace_t trace;
	vec3_t end;
	int moveType;

	end[0] = push[0] + ent->v.origin[0];
	end[1] = push[1] + ent->v.origin[1];
	end[2] = push[2] + ent->v.origin[2];

	if (ent->v.movetype == MOVETYPE_FLYMISSILE)
		moveType = 2;
	else
		moveType = (ent->v.solid == SOLID_TRIGGER || ent->v.solid == SOLID_NOT) ? 1 : 0;

	qboolean monsterClip = (ent->v.flags & FL_MONSTERCLIP) ? 1 : 0;
	trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, moveType, ent, monsterClip);
	if (trace.fraction != 0.0)
	{
		ent->v.origin[0] = trace.endpos[0];
		ent->v.origin[1] = trace.endpos[1];
		ent->v.origin[2] = trace.endpos[2];
	}

	SV_LinkEdict(ent, 1);
	if (trace.ent)
		SV_Impact(ent, trace.ent, &trace);

	return trace;
}

void SV_PushMove(edict_t *pusher, float movetime)
{
	vec3_t mins;
	vec3_t maxs;
	vec3_t move;
	vec3_t pushorig;

	if (pusher->v.velocity[0] == 0.0f && pusher->v.velocity[1] == 0.0f && pusher->v.velocity[2] == 0.0f)
	{
		pusher->v.ltime = movetime + pusher->v.ltime;
		return;
	}

	pushorig[0] = pusher->v.origin[0];
	pushorig[1] = pusher->v.origin[1];
	pushorig[2] = pusher->v.origin[2];

	for (int i = 0; i < 3; i++)
	{
		float movedist = movetime * pusher->v.velocity[i];
		move[i] = movedist;
		mins[i] = movedist + pusher->v.absmin[i];
		maxs[i] = movedist + pusher->v.absmax[i];
		pusher->v.origin[i] = movedist + pusher->v.origin[i];
	}

	pusher->v.ltime = movetime + pusher->v.ltime;
	SV_LinkEdict(pusher, 0);

	if (pusher->v.solid == SOLID_NOT)
		return;

	int num_moved = 0;
	for (int i = 1; i < g_psv.num_edicts; i++)
	{
		edict_t* check = &g_psv.edicts[i];

		if (check->free)
			continue;

		if (check->v.movetype == MOVETYPE_PUSH)
			continue;

		if (check->v.movetype == MOVETYPE_NONE || check->v.movetype == MOVETYPE_FOLLOW || check->v.movetype == MOVETYPE_NOCLIP)
			continue;

		if (((check->v.flags & FL_ONGROUND) && check->v.groundentity == pusher)
			|| (check->v.absmin[0] < maxs[0]
			&& check->v.absmin[1] < maxs[1]
			&& check->v.absmin[2] < maxs[2]
			&& check->v.absmax[0] > mins[0]
			&& check->v.absmax[1] > mins[1]
			&& check->v.absmax[2] > mins[2]
			&& SV_TestEntityPosition(check)))
		{
			if (check->v.movetype != MOVETYPE_WALK)
				check->v.flags &= ~FL_ONGROUND;

			vec3_t entorigin;
			entorigin[0] = check->v.origin[0];
			entorigin[1] = check->v.origin[1];
			entorigin[2] = check->v.origin[2];

			pusher->v.solid = SOLID_NOT;
			g_moved_from[num_moved][0] = check->v.origin[0];
			g_moved_from[num_moved][1] = check->v.origin[1];
			g_moved_from[num_moved][2] = check->v.origin[2];
			g_moved_edict[num_moved] = check;
			++num_moved;

			SV_PushEntity(check, move);

			pusher->v.solid = SOLID_BSP;
			if (!SV_TestEntityPosition(check))
				continue;

			if (check->v.mins[0] == check->v.maxs[0])
				continue;

			if (check->v.solid == SOLID_NOT || check->v.solid == SOLID_TRIGGER)
			{
				check->v.mins[1] = 0;
				check->v.mins[0] = 0;
				check->v.maxs[0] = 0;
				check->v.maxs[1] = 0;
				check->v.maxs[2] = check->v.mins[2];
				continue;
			}

			check->v.origin[0] = entorigin[0];
			check->v.origin[1] = entorigin[1];
			check->v.origin[2] = entorigin[2];
			SV_LinkEdict(check, 1);

			pusher->v.origin[0] = pushorig[0];
			pusher->v.origin[1] = pushorig[1];
			pusher->v.origin[2] = pushorig[2];
			SV_LinkEdict(pusher, 0);

			pusher->v.ltime = pusher->v.ltime - movetime;
			gEntityInterface.pfnBlocked(pusher, check);

			for (int e = 0; e < num_moved; e++)
			{
				g_moved_edict[e]->v.origin[0] = g_moved_from[e][0];
				g_moved_edict[e]->v.origin[1] = g_moved_from[e][1];
				g_moved_edict[e]->v.origin[2] = g_moved_from[e][2];

				SV_LinkEdict(g_moved_edict[e], 0);
			}
			return;
		}
	}
}

int SV_PushRotate(edict_t *pusher, float movetime)
{
	vec3_t amove;
	vec3_t pushorig;
	vec3_t forward;
	vec3_t right;
	vec3_t up;
	vec3_t forwardNow;
	vec3_t rightNow;
	vec3_t upNow;

	if (pusher->v.avelocity[0] == 0.0 && pusher->v.avelocity[1] == 0.0 && pusher->v.avelocity[2] == 0.0)
	{
		pusher->v.ltime = movetime + pusher->v.ltime;
		return 1;
	}

	for (int i = 0; i < 3; i++)
		amove[i] = movetime * pusher->v.avelocity[i];

	AngleVectors(pusher->v.angles, forward, right, up);

	pushorig[0] = pusher->v.angles[0];
	pushorig[1] = pusher->v.angles[1];
	pushorig[2] = pusher->v.angles[2];

	pusher->v.angles[0] = amove[0] + pusher->v.angles[0];
	pusher->v.angles[1] = amove[1] + pusher->v.angles[1];
	pusher->v.angles[2] = amove[2] + pusher->v.angles[2];

	AngleVectorsTranspose(pusher->v.angles, forwardNow, rightNow, upNow);
	pusher->v.ltime = movetime + pusher->v.ltime;

	SV_LinkEdict(pusher, 0);
	if (pusher->v.solid == SOLID_NOT)
		return 1;

	int num_moved = 0;
	for (int i = 1; i < g_psv.num_edicts; i++)
	{
		edict_t* check = &g_psv.edicts[i];
		if (check->free)
			continue;

		if (check->v.movetype == MOVETYPE_PUSH)
			continue;

		if (check->v.movetype == MOVETYPE_NONE || check->v.movetype == MOVETYPE_FOLLOW || check->v.movetype == MOVETYPE_NOCLIP)
			continue;

		if (((check->v.flags & FL_ONGROUND) && check->v.groundentity == pusher)
			|| (check->v.absmin[0] < pusher->v.absmax[0]
			&& check->v.absmin[1] < pusher->v.absmax[1]
			&& check->v.absmin[2] < pusher->v.absmax[2]
			&& check->v.absmax[0] > pusher->v.absmin[0]
			&& check->v.absmax[1] > pusher->v.absmin[1]
			&& check->v.absmax[2] > pusher->v.absmin[2]
			&& SV_TestEntityPosition(check)))
		{
			if (check->v.movetype != MOVETYPE_WALK)
				check->v.flags &= ~FL_ONGROUND;

			vec3_t entorig;
			entorig[0] = check->v.origin[0];
			entorig[1] = check->v.origin[1];
			entorig[2] = check->v.origin[2];

			g_moved_from[num_moved][0] = check->v.origin[0];
			g_moved_from[num_moved][1] = check->v.origin[1];
			g_moved_from[num_moved][2] = check->v.origin[2];
			g_moved_edict[num_moved] = check;
			++num_moved;

			if (num_moved >= g_psv.max_edicts)
				Sys_Error("%s: Out of edicts in simulator!\n", __FUNCTION__);

			vec3_t start, end, push, move;

			if (check->v.movetype == MOVETYPE_PUSHSTEP)
			{
				vec3_t org;
				org[0] = (check->v.absmax[0] + check->v.absmin[0]) * 0.5f;
				org[1] = (check->v.absmax[1] + check->v.absmin[1]) * 0.5f;
				org[2] = (check->v.absmax[2] + check->v.absmin[2]) * 0.5f;
				start[0] = org[0] - pusher->v.origin[0];
				start[1] = org[1] - pusher->v.origin[1];
				start[2] = org[2] - pusher->v.origin[2];
			}
			else
			{
				start[0] = check->v.origin[0] - pusher->v.origin[0];
				start[1] = check->v.origin[1] - pusher->v.origin[1];
				start[2] = check->v.origin[2] - pusher->v.origin[2];
			}

			pusher->v.solid = SOLID_NOT;

			move[0] = _DotProduct(forward, start);
			move[1] = -_DotProduct(right, start);
			move[2] = _DotProduct(up, start);
			end[0] = _DotProduct(forwardNow, move);
			end[1] = _DotProduct(rightNow, move);
			end[2] = _DotProduct(upNow, move);
			push[0] = end[0] - start[0];
			push[1] = end[1] - start[1];
			push[2] = end[2] - start[2];
			SV_PushEntity(check, push);

			pusher->v.solid = SOLID_BSP;
			if (check->v.movetype != MOVETYPE_PUSHSTEP)
			{
				if (check->v.flags & FL_CLIENT)
				{
					check->v.fixangle = 2;
					check->v.avelocity[1] = amove[1] + check->v.avelocity[1];
				}
				else
				{
					//check->v.angles[0] = check->v.angles[0] + 0.0f; //TODO: The 'check->v.angles[0]' variable is assigned to itself.
					check->v.angles[1] = amove[1] + check->v.angles[1];
					//check->v.angles[2] = check->v.angles[2] + 0.0f; //TODO: The 'check->v.angles[2]' variable is assigned to itself.
				}
			}

			if (!SV_TestEntityPosition(check) || check->v.mins[0] == check->v.maxs[0])
				continue;

			if (check->v.solid == SOLID_NOT || check->v.solid == SOLID_TRIGGER)
			{
				check->v.mins[1] = 0.0f;
				check->v.mins[0] = 0.0f;
				check->v.maxs[0] = 0.0f;
				check->v.maxs[1] = 0.0f;
				check->v.maxs[2] = check->v.mins[2];
				continue;
			}

			check->v.origin[0] = entorig[0];
			check->v.origin[1] = entorig[1];
			check->v.origin[2] = entorig[2];
			SV_LinkEdict(check, 1);

			pusher->v.angles[0] = pushorig[0];
			pusher->v.angles[1] = pushorig[1];
			pusher->v.angles[2] = pushorig[2];
			SV_LinkEdict(pusher, 0);

			pusher->v.ltime = pusher->v.ltime - movetime;
			gEntityInterface.pfnBlocked(pusher, check);
			for (int e = 0; e < num_moved; e++)
			{
				edict_t* movedEnt = g_moved_edict[e];
				movedEnt->v.origin[0] = g_moved_from[e][0];
				movedEnt->v.origin[1] = g_moved_from[e][1];
				movedEnt->v.origin[2] = g_moved_from[e][2];
				if (movedEnt->v.flags & FL_CLIENT)
				{
					movedEnt->v.avelocity[1] = 0.0f;
				}
				else
				{
					if (movedEnt->v.movetype != MOVETYPE_PUSHSTEP)
					{
						//movedEnt->v.angles[0] = movedEnt->v.angles[0]; //TODO: V570 The 'movedEnt->v.angles[0]' variable is assigned to itself.
						movedEnt->v.angles[1] = movedEnt->v.angles[1] - amove[1];
						//movedEnt->v.angles[2] = movedEnt->v.angles[2]; //TODO: V570 The 'movedEnt->v.angles[2]' variable is assigned to itself.
					}
				}
				SV_LinkEdict(movedEnt, 0);
			}

			return 0;
		}
	}

	return 1;
}

void SV_Physics_Pusher(edict_t *ent)
{
	float thinktime = ent->v.nextthink;
	float oldltime = ent->v.ltime;
	float movetime;

	if (oldltime + host_frametime <= thinktime)
	{
		movetime = (float) host_frametime;
	}
	else
	{
		movetime = thinktime - ent->v.ltime;
		if (movetime < 0.0)
			movetime = 0;
	}
	if (movetime != 0.0)
	{
		if (ent->v.avelocity[0] != 0.0 || ent->v.avelocity[1] != 0.0 || ent->v.avelocity[2] != 0.0)
		{
			if (ent->v.velocity[0] != 0.0 || ent->v.velocity[1] != 0.0 || ent->v.velocity[2] != 0.0)
			{
				if (SV_PushRotate(ent, movetime))
				{
					float savetime = ent->v.ltime;
					ent->v.ltime = oldltime;
					SV_PushMove(ent, movetime);
					if (ent->v.ltime < savetime)
						ent->v.ltime = savetime;
				}
			}
			else
			{
				SV_PushRotate(ent, movetime);
			}
		}
		else
		{
			SV_PushMove(ent, movetime);
		}
	}

	for (int i = 0; i < 3; i++)
	{
		if (ent->v.angles[i] < -3600.0f || ent->v.angles[i] > 3600.0f)
			ent->v.angles[i] = fmod(ent->v.angles[i], 3600.0f);
	}

	if (thinktime > oldltime && ((ent->v.flags & FL_ALWAYSTHINK) || thinktime <= ent->v.ltime)
#ifdef REHLDS_FIXES
	 && !(ent->v.flags & FL_KILLME)
#endif // REHLDS_FIXES
	)
	{
		ent->v.nextthink = 0;
		gGlobalVariables.time = (float) g_psv.time;
		gEntityInterface.pfnThink(ent);
	}
}

qboolean SV_CheckWater(edict_t *ent)
{
	vec3_t point;

	ent->v.waterlevel = 0;
	ent->v.watertype = -1;
	g_groupmask = ent->v.groupinfo;
	point[0] = (ent->v.absmax[0] + ent->v.absmin[0]) * 0.5f;
	point[1] = (ent->v.absmax[1] + ent->v.absmin[1]) * 0.5f;
	point[2] = ent->v.absmin[2] + 1.0f;
	int cont = SV_PointContents(point);
	if (cont > CONTENTS_WATER || cont <= CONTENTS_TRANSLUCENT)
		return 0;

	ent->v.watertype = cont;
	ent->v.waterlevel = 1;
	if (ent->v.absmin[2] == ent->v.absmax[2])
	{
		ent->v.waterlevel = 3;
	}
	else
	{
		g_groupmask = ent->v.groupinfo;
		point[2] = (ent->v.absmax[2] + ent->v.absmin[2]) * 0.5f;
		int truecont = SV_PointContents(point);
		if (truecont <= CONTENTS_WATER && truecont > CONTENTS_TRANSLUCENT)
		{
			ent->v.waterlevel = 2;
			g_groupmask = ent->v.groupinfo;
			point[0] = point[0] + ent->v.view_ofs[0];
			point[1] = point[1] + ent->v.view_ofs[1];
			point[2] = point[2] + ent->v.view_ofs[2];
			truecont = SV_PointContents(point);
			if (truecont <= CONTENTS_WATER && truecont > CONTENTS_TRANSLUCENT)
				ent->v.waterlevel = 3;
		}
	}

	if (cont <= CONTENTS_CURRENT_0 && cont >= CONTENTS_CURRENT_DOWN)
	{
		static vec_t current_table[6][3] =
		{
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ -1.0f, 0.0f, 0.0f },
			{ 0.0f, -1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, -1.0f }
		};

		VectorMA(ent->v.basevelocity, ent->v.waterlevel * 50.0f, current_table[-(cont - CONTENTS_CURRENT_0)], ent->v.basevelocity);
	}

	return (ent->v.waterlevel > 1) ? 1 : 0;
}

float SV_RecursiveWaterLevel(vec_t *center, float out, float in, int count)
{
	float offset = (float)((out - in) * 0.5 + in);
	if (count + 1 < 6)
	{
		vec3_t test;
		test[0] = center[0];
		test[1] = center[1];
		test[2] = offset + center[2];
		if (SV_PointContents(test) == CONTENTS_WATER)
			return SV_RecursiveWaterLevel(center, out, offset, count + 1);
		else
			return SV_RecursiveWaterLevel(center, offset, in, count + 1);
	}
	else
	{
		return offset;
	}
}

float SV_Submerged(edict_t *ent)
{
	float end;
	vec3_t center;

	center[0] = (ent->v.absmax[0] + ent->v.absmin[0]) * 0.5f;
	center[1] = (ent->v.absmax[1] + ent->v.absmin[1]) * 0.5f;
	center[2] = (ent->v.absmin[2] + ent->v.absmax[2]) * 0.5f;
	float start = ent->v.absmin[2] - center[2];
	if (ent->v.waterlevel == 1)
	{
		return SV_RecursiveWaterLevel(center, 0.0, start, 0) - start;
	}

	if (ent->v.waterlevel == 2)
	{
		end = ent->v.absmax[2] - center[2];
		return SV_RecursiveWaterLevel(center, end, 0.0, 0) - start;
	}

	if (ent->v.waterlevel == 3)
	{
		vec3_t point;
		point[0] = center[0];
		point[1] = center[1];
		point[2] = ent->v.absmax[2];
		g_groupmask = ent->v.groupinfo;
		if (SV_PointContents(point) == -3)
			return ent->v.maxs[2] - ent->v.mins[2];

		end = ent->v.absmax[2] - center[2];
		return SV_RecursiveWaterLevel(center, end, 0.0, 0) - start;
	}

	return 0;
}

// Non moving objects can only think.
void SV_Physics_None(edict_t *ent)
{
	SV_RunThink(ent);
}

void SV_Physics_Follow(edict_t *ent)
{
	if (SV_RunThink(ent))
	{
		if (ent->v.aiment)
		{
			ent->v.angles[0] = ent->v.aiment->v.angles[0];
			ent->v.angles[1] = ent->v.aiment->v.angles[1];
			ent->v.angles[2] = ent->v.aiment->v.angles[2];
			ent->v.origin[0] = ent->v.aiment->v.origin[0] + ent->v.v_angle[0];
			ent->v.origin[1] = ent->v.aiment->v.origin[1] + ent->v.v_angle[1];
			ent->v.origin[2] = ent->v.aiment->v.origin[2] + ent->v.v_angle[2];
			SV_LinkEdict(ent, 1);
		}
		else
		{
			Con_DPrintf("%s movetype FOLLOW with NULL aiment\n", &pr_strings[ent->v.classname]);
			ent->v.movetype = 0;
		}
	}
}

void SV_Physics_Noclip(edict_t *ent)
{
	if (SV_RunThink(ent))
	{
		VectorMA(ent->v.angles, (float)host_frametime, ent->v.avelocity, ent->v.angles);
		VectorMA(ent->v.origin, (float)host_frametime, ent->v.velocity, ent->v.origin);
		SV_LinkEdict(ent, 0);
	}
}

void SV_CheckWaterTransition(edict_t *ent)
{
	vec3_t point;

	g_groupmask = ent->v.groupinfo;
	point[0] = (ent->v.absmax[0] + ent->v.absmin[0]) * 0.5f;
	point[1] = (ent->v.absmax[1] + ent->v.absmin[1]) * 0.5f;
	point[2] = ent->v.absmin[2] + 1.0f;
	int cont = SV_PointContents(point);

	if (ent->v.watertype == 0)
	{
		ent->v.watertype = cont;
		ent->v.waterlevel = 1;
		return;
	}

	if (cont > CONTENTS_WATER || cont <= CONTENTS_TRANSLUCENT)
	{
		if (ent->v.watertype != -1)
			SV_StartSound(0, ent, 0, "player/pl_wade2.wav", 255, 1.0, 0, 100);
		ent->v.watertype = -1;
		ent->v.waterlevel = 0;
		return;
	}

	if (ent->v.watertype == -1)
	{
		SV_StartSound(0, ent, 0, "player/pl_wade1.wav", 255, 1.0, 0, 100);
		ent->v.velocity[2] = ent->v.velocity[2] * 0.5f;
	}

	ent->v.watertype = cont;
	ent->v.waterlevel = 1;
	if (ent->v.absmin[2] == ent->v.absmax[2])
	{
		ent->v.waterlevel = 3;
		return;
	}

	g_groupmask = ent->v.groupinfo;
	point[2] = (ent->v.absmin[2] + ent->v.absmax[2]) * 0.5f;
	cont = SV_PointContents(point);
	if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
	{
		ent->v.waterlevel = 2;
		g_groupmask = ent->v.groupinfo;
		point[0] = point[0] + ent->v.view_ofs[0];
		point[1] = point[1] + ent->v.view_ofs[1];
		point[2] = point[2] + ent->v.view_ofs[2];
		cont = SV_PointContents(point);
		if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
		{
			ent->v.waterlevel = 3;
		}
	}
}

void SV_Physics_Toss(edict_t *ent)
{
	SV_CheckWater(ent);
	if (!SV_RunThink(ent))
		return;

	if (ent->v.velocity[2] > 0.0 || ent->v.groundentity == NULL || ent->v.groundentity->v.flags & (FL_MONSTER | FL_CLIENT))
		ent->v.flags &= ~FL_ONGROUND;

	if ((ent->v.flags & FL_ONGROUND) && VectorCompare(ent->v.velocity, vec_origin)) {
		ent->v.avelocity[0] = vec3_origin[0];
		ent->v.avelocity[1] = vec3_origin[1];
		ent->v.avelocity[2] = vec3_origin[2];
		if (VectorCompare(ent->v.basevelocity, vec_origin))
			return;
	}

	SV_CheckVelocity(ent);
	if (ent->v.movetype != MOVETYPE_FLY && ent->v.movetype != MOVETYPE_BOUNCEMISSILE && ent->v.movetype != MOVETYPE_FLYMISSILE)
		SV_AddGravity(ent);

	VectorMA(ent->v.angles, (float)host_frametime, ent->v.avelocity, ent->v.angles);
	ent->v.velocity[0] = ent->v.basevelocity[0] + ent->v.velocity[0];
	ent->v.velocity[1] = ent->v.basevelocity[1] + ent->v.velocity[1];
	ent->v.velocity[2] = ent->v.basevelocity[2] + ent->v.velocity[2];
	SV_CheckVelocity(ent);

	vec3_t move;
	VectorScale(ent->v.velocity, (float)host_frametime, move);
	ent->v.velocity[0] = ent->v.velocity[0] - ent->v.basevelocity[0];
	ent->v.velocity[1] = ent->v.velocity[1] - ent->v.basevelocity[1];
	ent->v.velocity[2] = ent->v.velocity[2] - ent->v.basevelocity[2];

	trace_t trace = SV_PushEntity(ent, move);
	SV_CheckVelocity(ent);

	if (trace.allsolid)
	{
		ent->v.velocity[0] = vec3_origin[0];
		ent->v.velocity[1] = vec3_origin[1];
		ent->v.velocity[2] = vec3_origin[2];
		ent->v.avelocity[0] = vec3_origin[0];
		ent->v.avelocity[1] = vec3_origin[1];
		ent->v.avelocity[2] = vec3_origin[2];
		return;
	}

	if (trace.fraction == 1.0f)
	{
		SV_CheckWaterTransition(ent);
		return;
	}

	if (ent->free)
		return;

	float backoff;
	if (ent->v.movetype == MOVETYPE_BOUNCE)
	{
		backoff = 2.0f - ent->v.friction;
	}
	else
	{
		backoff = 2.0f;
		if (ent->v.movetype != MOVETYPE_BOUNCEMISSILE)
			backoff = 1.0f;
	}
	ClipVelocity(ent->v.velocity, trace.plane.normal, ent->v.velocity, backoff);

	if (trace.plane.normal[2] <= 0.7)
	{
		SV_CheckWaterTransition(ent);
		return;
	}

	move[0] = ent->v.basevelocity[0] + ent->v.velocity[0];
	move[1] = ent->v.basevelocity[1] + ent->v.velocity[1];
	move[2] = ent->v.basevelocity[2] + ent->v.velocity[2];
	if (sv_gravity.value * host_frametime > move[2])
	{
		ent->v.flags |= FL_ONGROUND;
		ent->v.velocity[2]= 0;
		ent->v.groundentity = trace.ent;
	}

	if (_DotProduct(move, move) >= 900.0f)
	{
		if (ent->v.movetype == MOVETYPE_BOUNCE || ent->v.movetype == MOVETYPE_BOUNCEMISSILE)
		{
			float scale = (1.0 - trace.fraction) * host_frametime * 0.9f;
			VectorScale(ent->v.velocity, scale, move);
			VectorMA(move, scale, ent->v.basevelocity, move);
			SV_PushEntity(ent, move);
			SV_CheckWaterTransition(ent);
			return;
		}
	}

	ent->v.flags |= FL_ONGROUND;
	ent->v.groundentity = trace.ent;
	ent->v.velocity[0] = vec3_origin[0];
	ent->v.velocity[1] = vec3_origin[1];
	ent->v.velocity[2] = vec3_origin[2];
	ent->v.avelocity[0] = vec3_origin[0];
	ent->v.avelocity[1] = vec3_origin[1];
	ent->v.avelocity[2] = vec3_origin[2];
	SV_CheckWaterTransition(ent);
}

void PF_WaterMove(edict_t *pSelf)
{
	int flags;
	int waterlevel;
	int watertype;
	float drownlevel;

	if (pSelf->v.movetype == MOVETYPE_NOCLIP)
	{
		pSelf->v.air_finished = (float)(g_psv.time + 12.0f);
		return;
	}

	if (pSelf->v.health < 0.0)
		return;

	drownlevel = pSelf->v.deadflag ? 1.0f : 3.0f;
	waterlevel = pSelf->v.waterlevel;
	watertype = pSelf->v.watertype;
	flags = pSelf->v.flags;
	if (!(flags & (FL_IMMUNE_WATER | FL_GODMODE)))
	{
		if (flags & FL_SWIM && (waterlevel < drownlevel)
			|| (waterlevel >= drownlevel))
		{
			if (pSelf->v.air_finished < g_psv.time && pSelf->v.pain_finished < g_psv.time)
			{
				pSelf->v.dmg += 2.0;
				if (pSelf->v.dmg > 15.0)
					pSelf->v.dmg = 10.0f;
				pSelf->v.pain_finished = (float)(g_psv.time + 1.0f);
			}
		}
		else
		{
			pSelf->v.dmg = 2.0f;
			pSelf->v.air_finished = (float)(g_psv.time + 12.0);
		}
	}
	if (!waterlevel)
	{
		if (flags & FL_INWATER)
		{
			switch (RandomLong(0, 3))
			{
			case 0:
				SV_StartSound(0, pSelf, 4, "player/pl_wade1.wav", 255, 0.8f, 0, 100);
				break;
			case 1:
				SV_StartSound(0, pSelf, 4, "player/pl_wade2.wav", 255, 0.8f, 0, 100);
				break;
			case 2:
				SV_StartSound(0, pSelf, 4, "player/pl_wade3.wav", 255, 0.8f, 0, 100);
				break;
			case 3:
				SV_StartSound(0, pSelf, 4, "player/pl_wade4.wav", 255, 0.8f, 0, 100);
				break;
			default:
				break;
			}
			pSelf->v.flags &= ~FL_INWATER;
		}
		pSelf->v.air_finished = (float)(g_psv.time + 12.0);
		return;
	}

	if (watertype == -5)
	{
		if (!(flags & (FL_IMMUNE_LAVA | FL_GODMODE)) && pSelf->v.dmgtime < g_psv.time)
		{
			if (g_psv.time <= pSelf->v.radsuit_finished)
				pSelf->v.dmgtime = (float)(g_psv.time + 1.0);
			else
				pSelf->v.dmgtime = (float)(g_psv.time + 0.2);
		}
	}
	else
	{
		if (watertype == -4
			&& !(flags & (FL_IMMUNE_SLIME | FL_GODMODE))
			&& pSelf->v.dmgtime < g_psv.time
			&& pSelf->v.radsuit_finished < g_psv.time)
		{
			pSelf->v.dmgtime = (float)(g_psv.time + 1.0);
		}
	}

	if (!(flags & FL_INWATER))
	{
		if (watertype == -3)
		{
			switch (RandomLong(0, 3))
			{
			case 0:
				SV_StartSound(0, pSelf, 4, "player/pl_wade1.wav", 255, 0.8f, 0, 100);
				break;
			case 1:
				SV_StartSound(0, pSelf, 4, "player/pl_wade2.wav", 255, 0.8f, 0, 100);
				break;
			case 2:
				SV_StartSound(0, pSelf, 4, "player/pl_wade3.wav", 255, 0.8f, 0, 100);
				break;
			case 3:
				SV_StartSound(0, pSelf, 4, "player/pl_wade4.wav", 255, 0.8f, 0, 100);
				break;
			default:
				break;
			}
		}
		pSelf->v.dmgtime = 0;
		pSelf->v.flags = flags | FL_INWATER;
	}

	if (!(flags & FL_WATERJUMP))
	{
		VectorMA(pSelf->v.velocity, (float)(pSelf->v.waterlevel * host_frametime * -0.8), pSelf->v.velocity, pSelf->v.velocity);
	}
}


void SV_Physics_Step(edict_t *ent)
{
	vec3_t maxs;
	vec3_t mins;
	vec3_t point;

	PF_WaterMove(ent);
	SV_CheckVelocity(ent);
	qboolean wasonground = (ent->v.flags & FL_ONGROUND) ? 1 : 0;
	qboolean inwater = SV_CheckWater(ent);
	if ((ent->v.flags & FL_FLOAT) && ent->v.waterlevel > 0)// FL_FLOAT
	{
		float waterLevel = (float)(SV_Submerged(ent) * ent->v.skin * host_frametime);
		SV_AddGravity(ent);
		ent->v.velocity[2] += waterLevel;
	}
	if (!wasonground)
	{
		if (!(ent->v.flags & FL_FLY))
		{
			if (!(ent->v.flags & FL_SWIM) || ent->v.waterlevel <= 0)
			{
				if (!inwater)
					SV_AddGravity(ent);
			}
		}
	}
	if (VectorCompare(ent->v.velocity, vec_origin) && VectorCompare(ent->v.basevelocity, vec_origin))
	{
		if (gGlobalVariables.force_retouch != 0.0)
		{
			trace_t trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, ent->v.origin,	0, ent,	(ent->v.flags & FL_MONSTERCLIP) ? 1 : 0);
			if (trace.fraction < 1.0 || trace.startsolid)
			{
				if (trace.ent)
					SV_Impact(ent, trace.ent, &trace);
			}
		}
	}
	else
	{
		ent->v.flags &= ~FL_ONGROUND;
		if (wasonground && (ent->v.health > 0.0 || SV_CheckBottom(ent)))
		{
			float speed = (float)Q_sqrt((long double)(ent->v.velocity[0] * ent->v.velocity[0] + ent->v.velocity[1] * ent->v.velocity[1]));
			if (speed  != 0.0)
			{
				float friction = sv_friction.value * ent->v.friction;
				ent->v.friction = 1.0f;
				float control = (speed >= sv_stopspeed.value) ? speed : sv_stopspeed.value;

				float newspeed = (float)(speed - control * friction * host_frametime);
				if (newspeed < 0.0)
					newspeed = 0.0;
				newspeed = newspeed / speed;

				ent->v.velocity[0] *= newspeed;
				ent->v.velocity[1] *= newspeed;
			}
		}
		ent->v.velocity[0] += ent->v.basevelocity[0];
		ent->v.velocity[1] += ent->v.basevelocity[1];
		ent->v.velocity[2] += ent->v.basevelocity[2];
		SV_CheckVelocity(ent);
		SV_FlyMove(ent, (float)host_frametime, 0);
		SV_CheckVelocity(ent);
		ent->v.velocity[0] -= ent->v.basevelocity[0];
		ent->v.velocity[1] -= ent->v.basevelocity[1];
		ent->v.velocity[2] -= ent->v.basevelocity[2];
		SV_CheckVelocity(ent);
		mins[0] = ent->v.mins[0] + ent->v.origin[0];
		mins[1] = ent->v.mins[1] + ent->v.origin[1];
		mins[2] = ent->v.mins[2] + ent->v.origin[2];
		maxs[0] = ent->v.maxs[0] + ent->v.origin[0];
		maxs[1] = ent->v.maxs[1] + ent->v.origin[1];
		point[2] = mins[2] - 1.0f;

		for (int x = 0; x <= 1; x++)
		{
			for (int y = 0; y <= 1; y++)
			{
				point[0] = x ? (maxs[0]) : (mins[0]);
				point[1] = y ? (maxs[1]) : (mins[1]);
				g_groupmask = ent->v.groupinfo;
				if (SV_PointContents(point) == -2)
				{
					ent->v.flags |= FL_ONGROUND;
					break;
				}
			}
		};
		SV_LinkEdict(ent, 1);
	}
	SV_RunThink(ent);
	SV_CheckWaterTransition(ent);
}

void SV_Physics(void)
{
	gGlobalVariables.time = (float)g_psv.time;
	gEntityInterface.pfnStartFrame();
	for (int i = 0; i < g_psv.num_edicts; i++)
	{
		edict_t* ent = &g_psv.edicts[i];
		if (ent->free)
			continue;

		if (gGlobalVariables.force_retouch != 0.0)
			SV_LinkEdict(ent, 1);

		if (i > 0 && i <= g_psvs.maxclients)
			continue;

		if (ent->v.flags & FL_ONGROUND)
		{
			edict_t* groundentity = ent->v.groundentity;
			if (groundentity)
			{
				if (groundentity->v.flags & FL_CONVEYOR)
				{
					if (ent->v.flags & FL_BASEVELOCITY)
						VectorMA(ent->v.basevelocity, groundentity->v.speed, groundentity->v.movedir, ent->v.basevelocity);
					else
						VectorScale(groundentity->v.movedir, groundentity->v.speed, ent->v.basevelocity);
					ent->v.flags |= FL_BASEVELOCITY;
				}
			}
		}
		if (!(ent->v.flags & FL_BASEVELOCITY))
		{
			VectorMA(ent->v.velocity, (float)(host_frametime * 0.5 + 1.0), ent->v.basevelocity, ent->v.velocity);
			ent->v.basevelocity[0] = vec3_origin[0];
			ent->v.basevelocity[1] = vec3_origin[1];
			ent->v.basevelocity[2] = vec3_origin[2];
		}
		ent->v.flags &= ~FL_BASEVELOCITY;

		switch (ent->v.movetype)
		{
		case MOVETYPE_NONE:
			SV_Physics_None(ent);
			break;

		case MOVETYPE_PUSH:
			SV_Physics_Pusher(ent);
			break;

		case MOVETYPE_FOLLOW:
			SV_Physics_Follow(ent);
			break;

		case MOVETYPE_NOCLIP:
			SV_Physics_Noclip(ent);
			break;

		case MOVETYPE_STEP:
		case MOVETYPE_PUSHSTEP:
			SV_Physics_Step(ent);
			break;

		case MOVETYPE_TOSS:
		case MOVETYPE_BOUNCE:
		case MOVETYPE_BOUNCEMISSILE:
		case MOVETYPE_FLY:
		case MOVETYPE_FLYMISSILE:
			SV_Physics_Toss(ent);
			break;

		default:
			Sys_Error("%s: %s bad movetype %d", __FUNCTION__, &pr_strings[ent->v.classname], ent->v.movetype);
		}

		if (ent->v.flags & FL_KILLME)
			ED_Free(ent);
	}

	if (gGlobalVariables.force_retouch != 0.0f)
		gGlobalVariables.force_retouch = gGlobalVariables.force_retouch - 1.0f;
}

trace_t SV_Trace_Toss(edict_t *ent, edict_t *ignore)
{
	edict_t tempent;
	trace_t trace;
	vec3_t move;
	vec3_t end;
	double save_frametime;

	save_frametime = host_frametime;
	host_frametime = 5.0;
	Q_memcpy(&tempent, ent, sizeof(tempent));
	do
	{
		do
		{
			SV_CheckVelocity(&tempent);
			SV_AddGravity(&tempent);
			VectorMA(tempent.v.angles, (float)host_frametime, tempent.v.avelocity, tempent.v.angles);
			VectorScale(tempent.v.velocity, (float)host_frametime, move);
			end[0] = tempent.v.origin[0] + move[0];
			end[1] = tempent.v.origin[1] + move[1];
			end[2] = tempent.v.origin[2] + move[2];
			trace = SV_Move(tempent.v.origin, tempent.v.mins, tempent.v.maxs, end, 0, &tempent, 0);
			tempent.v.origin[1] = trace.endpos[1];
			tempent.v.origin[0] = trace.endpos[0];
			tempent.v.origin[2] = trace.endpos[2];
		}
		while (!trace.ent);
	}
	while (trace.ent == ignore);

	host_frametime = save_frametime;
	return trace;
}
