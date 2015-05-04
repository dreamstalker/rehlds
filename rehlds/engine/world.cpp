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


hull_t box_hull;
hull_t beam_hull;
box_clipnodes_t box_clipnodes;
box_planes_t box_planes;
beam_planes_t beam_planes;
areanode_t sv_areanodes[32];
int sv_numareanodes;


/* <ca50b> ../engine/world.c:48 */
void ClearLink(link_t *l)
{
	l->next = l;
	l->prev = l;
}

/* <ca41e> ../engine/world.c:53 */
void RemoveLink(link_t *l)
{
	l->next->prev = l->prev;
	l->prev->next = l->next;
}

/* <ca452> ../engine/world.c:59 */
void InsertLinkBefore(link_t *l, link_t *before)
{
	l->next = before;
	l->prev = before->prev;
	l->next->prev = l;
	l->prev->next = l;
}

/* <ca56a> ../engine/world.c:66 */
NOXREF void InsertLinkAfter(link_t *l, link_t *after)
{
	l->prev = after;
	l->next = after->next;

	after->next = l;
	l->next->prev = l;
}

/* <ca59d> ../engine/world.c:94 */
void SV_InitBoxHull(void)
{
	box_hull.clipnodes = &box_clipnodes[0];
	box_hull.planes = &box_planes[0];
	box_hull.firstclipnode = 0;
	box_hull.lastclipnode = 5;
	Q_memcpy(&beam_hull, &box_hull, sizeof(beam_hull));
	beam_hull.planes = &beam_planes[0];

	for (int i = 0; i < 6; i++)
	{
		int side = i & 1;
		box_clipnodes[i].planenum = i;
		box_clipnodes[i].children[side] = -1;
		box_clipnodes[i].children[side ^ 1] = (i != 5) ? i + 1 : CONTENTS_SOLID;
		box_planes[i].type = i >> 1;
		beam_planes[i].type = 5;
		box_planes[i].normal[i >> 1] = 1.0f;
	}
}

/* <ca3c6> ../engine/world.c:135 */
hull_t *SV_HullForBox(const vec_t *mins, const vec_t *maxs)
{
	box_planes[0].dist = maxs[0];
	box_planes[1].dist = mins[0];
	box_planes[2].dist = maxs[1];
	box_planes[3].dist = mins[1];
	box_planes[4].dist = maxs[2];
	box_planes[5].dist = mins[2];
	return &box_hull;
}

/* <ca60c> ../engine/world.c:148 */
NOXREF hull_t *SV_HullForBeam(const vec_t *start, const vec_t *end, const vec_t *size)
{
	vec3_t tmp = { 0, 0, 0 };

	beam_planes[0].normal[0] = end[0] - start[0];
	beam_planes[0].normal[1] = end[1] - start[1];
	beam_planes[0].normal[2] = end[2] - start[2];

	VectorNormalize(beam_planes[0].normal);

	beam_planes[1].normal[0] = beam_planes[0].normal[0];
	beam_planes[1].normal[1] = beam_planes[0].normal[1];
	beam_planes[1].normal[2] = beam_planes[0].normal[2];

	beam_planes[0].dist = _DotProduct((vec_t *)end, beam_planes[0].normal);
	beam_planes[1].dist = _DotProduct((vec_t *)start, beam_planes[0].normal);

	if (fabs(beam_planes[0].normal[2]) < 0.9f)
		tmp[2] = 1.0f;
	else
		tmp[0] = 1.0f;

	CrossProduct(beam_planes[0].normal, tmp, beam_planes[2].normal);
	VectorNormalize(beam_planes[2].normal);

	beam_planes[3].normal[0] = beam_planes[2].normal[0];
	beam_planes[3].normal[1] = beam_planes[2].normal[1];
	beam_planes[3].normal[2] = beam_planes[2].normal[2];

	beam_planes[2].dist = (start[0] + beam_planes[2].normal[0]) * beam_planes[2].normal[0] + (start[1] + beam_planes[2].normal[1]) * beam_planes[2].normal[1] + (start[2] + beam_planes[2].normal[2]) * beam_planes[2].normal[2];

	tmp[0] = start[0] - beam_planes[2].normal[0];
	tmp[1] = start[1] - beam_planes[2].normal[1];
	tmp[2] = start[2] - beam_planes[2].normal[2];

	beam_planes[3].dist = _DotProduct(tmp, beam_planes[2].normal);
	CrossProduct(beam_planes[2].normal, beam_planes[0].normal, beam_planes[4].normal);
	VectorNormalize(beam_planes[4].normal);

	beam_planes[5].normal[0] = beam_planes[4].normal[0];
	beam_planes[5].normal[1] = beam_planes[4].normal[1];
	beam_planes[5].normal[2] = beam_planes[4].normal[2];

	beam_planes[4].dist = _DotProduct((vec_t *)start, beam_planes[4].normal);
	beam_planes[5].dist = (start[0] - beam_planes[4].normal[0]) * beam_planes[4].normal[0] + (start[1] - beam_planes[4].normal[1]) * beam_planes[4].normal[1] + (start[2] - beam_planes[4].normal[2]) * beam_planes[4].normal[2];
	beam_planes[0].dist += fabs(beam_planes[0].normal[0] * size[0]) + fabs(beam_planes[0].normal[1] * size[1]) + fabs(beam_planes[0].normal[2] * size[2]);

	beam_planes[1].dist -= (fabs(beam_planes[1].normal[0] * size[0]) + fabs(beam_planes[1].normal[1] * size[1]) + fabs(beam_planes[1].normal[2] * size[2]));
	beam_planes[2].dist += fabs(beam_planes[2].normal[0] * size[0]) + fabs(beam_planes[2].normal[1] * size[1]) + fabs(beam_planes[2].normal[2] * size[2]);
	beam_planes[3].dist -= (fabs(beam_planes[3].normal[0] * size[0]) + fabs(beam_planes[3].normal[1] * size[1]) + fabs(beam_planes[3].normal[2] * size[2]));
	beam_planes[4].dist += fabs(beam_planes[4].normal[0] * size[0]) + fabs(beam_planes[4].normal[1] * size[1]) + fabs(beam_planes[4].normal[2] * size[2]);
	beam_planes[5].dist -= (fabs(beam_planes[4].normal[0] * size[0]) + fabs(beam_planes[4].normal[1] * size[1]) + fabs(beam_planes[4].normal[2] * size[2]));

	return &beam_hull;
}

/* <ca662> ../engine/world.c:201 */
struct hull_s *SV_HullForBsp(edict_t *ent, const vec_t *mins, const vec_t *maxs, vec_t *offset)
{
	model_t *model;
	hull_t *hull;

	model = g_psv.models[ent->v.modelindex];
	if (!model)
		Sys_Error("Hit a %s with no model (%s)", &pr_strings[ent->v.classname], &pr_strings[ent->v.model]);

	if (model->type)
		Sys_Error("Hit a %s with no model (%s)", &pr_strings[ent->v.classname], &pr_strings[ent->v.model]);

	float xSize = maxs[0] - mins[0];
	if (xSize > 8.0f)
	{
		if (xSize > 36.0f)
		{
			hull = &model->hulls[2];
		}
		else
		{
			float zSize = maxs[2] - mins[2];
			if (zSize > 36.0f)
				hull = &model->hulls[1];
			else
				hull = &model->hulls[3];
		}
		offset[0] = hull->clip_mins[0] - mins[0];
		offset[1] = hull->clip_mins[1] - mins[1];
		offset[2] = hull->clip_mins[2] - mins[2];
	}
	else
	{
		hull = &model->hulls[0];
		offset[0] = model->hulls[0].clip_mins[0];
		offset[1] = model->hulls[0].clip_mins[1];
		offset[2] = model->hulls[0].clip_mins[2];
	}

	offset[0] = ent->v.origin[0] + offset[0];
	offset[1] = ent->v.origin[1] + offset[1];
	offset[2] = ent->v.origin[2] + offset[2];
	return hull;
}

/* <ca6f1> ../engine/world.c:251 */
hull_t *SV_HullForEntity(edict_t *ent, const vec_t *mins, const vec_t *maxs, vec_t *offset)
{
	vec3_t hullmins;
	vec3_t hullmaxs;

	if (ent->v.solid == SOLID_BSP)
	{
		if (ent->v.movetype != MOVETYPE_PUSH && ent->v.movetype != MOVETYPE_PUSHSTEP)
			Sys_Error("SOLID_BSP without MOVETYPE_PUSH");

		return SV_HullForBsp(ent, mins, maxs, offset);
	}

	hullmins[0] = ent->v.mins[0] - maxs[0];
	hullmins[1] = ent->v.mins[1] - maxs[1];
	hullmins[2] = ent->v.mins[2] - maxs[2];
	hullmaxs[0] = ent->v.maxs[0] - mins[0];
	hullmaxs[1] = ent->v.maxs[1] - mins[1];
	hullmaxs[2] = ent->v.maxs[2] - mins[2];
	offset[0] = ent->v.origin[0];
	offset[1] = ent->v.origin[1];
	offset[2] = ent->v.origin[2];
	return SV_HullForBox(hullmins, hullmaxs);
}

/* <ca793> ../engine/world.c:308 */
areanode_t *SV_CreateAreaNode(int depth, vec_t *mins, vec_t *maxs)
{
	areanode_t *anode;
	vec3_t mins1;
	vec3_t maxs2;
	vec3_t size;
	vec3_t maxs1;
	vec3_t mins2;
	float fmid;

	anode = &sv_areanodes[sv_numareanodes++];
	ClearLink(&anode->trigger_edicts);
	ClearLink(&anode->solid_edicts);
	if (depth == 4)
	{
		anode->axis = -1;
		anode->children[0] = NULL;
		anode->children[1] = NULL;
		return anode;
	}

	size[0] = maxs[0] - mins[0];
	size[1] = maxs[1] - mins[1];
	anode->axis = (size[0] <= size[1]) ? 1 : 0;
	mins1[0] = mins[0];
	mins1[1] = mins[1];
	mins1[2] = mins[2];

	mins2[0] = mins[0];
	mins2[1] = mins[1];
	mins2[2] = mins[2];

	maxs1[0] = maxs[0];
	maxs1[1] = maxs[1];
	maxs1[2] = maxs[2];

	maxs2[0] = maxs[0];
	maxs2[1] = maxs[1];
	maxs2[2] = maxs[2];

	fmid = 0.5f * (mins[anode->axis] + maxs[anode->axis]);
	mins2[anode->axis] = fmid;
	maxs1[anode->axis] = fmid;

	anode->dist = fmid;
	anode->children[0] = SV_CreateAreaNode(depth + 1, mins2, maxs2);
	anode->children[1] = SV_CreateAreaNode(depth + 1, mins1, maxs1);

	return anode;

}

/* <ca877> ../engine/world.c:353 */
void SV_ClearWorld(void)
{
	SV_InitBoxHull();
	Q_memset(sv_areanodes, 0, sizeof(sv_areanodes));
	sv_numareanodes = 0;
	SV_CreateAreaNode(0, g_psv.worldmodel->mins, g_psv.worldmodel->maxs);
}

/* <ca8bb> ../engine/world.c:369 */
void SV_UnlinkEdict(edict_t *ent)
{
	if (ent->area.prev)
	{
		RemoveLink(&ent->area);
		ent->area.next = NULL;
		ent->area.prev = NULL;
	}
}

/* <ca9f5> ../engine/world.c:383 */
void SV_TouchLinks(edict_t *ent, areanode_t *node)
{
	edict_t *touch;
	model_t *pModel;
	link_t *next;

	for (link_t *l = node->trigger_edicts.next; l != &node->trigger_edicts; l = next)
	{
		next = l->next;
		touch = (edict_t *)((char *)l - offsetof(edict_t, area));
		if (touch == ent)
			continue;

		if (ent->v.groupinfo != 0 && touch->v.groupinfo != 0)
		{
			if (g_groupop)
			{
				if (g_groupop == GROUP_OP_NAND && (ent->v.groupinfo & touch->v.groupinfo))
					continue;
			}
			else
			{
				if (!(ent->v.groupinfo & touch->v.groupinfo))
					continue;
			}
		}

		if (touch->v.solid == SOLID_TRIGGER
			&& ent->v.absmin[0] <= touch->v.absmax[0]
			&& ent->v.absmin[1] <= touch->v.absmax[1]
			&& ent->v.absmin[2] <= touch->v.absmax[2]
			&& ent->v.absmax[0] >= touch->v.absmin[0]
			&& ent->v.absmax[1] >= touch->v.absmin[1]
			&& ent->v.absmax[2] >= touch->v.absmin[2])
		{
			pModel = g_psv.models[touch->v.modelindex];

			if (pModel && pModel->type == mod_brush)
			{
				vec3_t offset;
				hull_t *hull = SV_HullForBsp(touch, ent->v.mins, ent->v.maxs, offset);

				vec3_t localPosition;
				localPosition[0] = ent->v.origin[0] - offset[0];
				localPosition[1] = ent->v.origin[1] - offset[1];
				localPosition[2] = ent->v.origin[2] - offset[2];

				int contents = SV_HullPointContents(hull, hull->firstclipnode, localPosition);
				if (contents != CONTENTS_SOLID)
					continue;
			}

			gGlobalVariables.time = (float)g_psv.time;
			gEntityInterface.pfnTouch(touch, ent);
		}
	}

	if (node->axis != -1)
	{
		if (ent->v.absmax[node->axis] > node->dist)
			SV_TouchLinks(ent, node->children[0]);

		if (node->dist > ent->v.absmin[node->axis])
			SV_TouchLinks(ent, node->children[1]);
	}
}

/* <ca8f2> ../engine/world.c:467 */
void SV_FindTouchedLeafs(edict_t *ent, mnode_t *node, int *topnode)
{
	mplane_t *splitplane;
	int sides;

	if (node->contents == CONTENTS_SOLID)
		return;

	if (node->contents < 0)
	{
		if (ent->num_leafs < MAX_ENT_LEAFS)
		{
			mleaf_t *leaf = (mleaf_t *)node;
			int leafnum = leaf - g_psv.worldmodel->leafs - 1;
			ent->leafnums[ent->num_leafs] = leafnum;
			ent->num_leafs++;
		}
		else
		{
			ent->num_leafs = MAX_ENT_LEAFS + 1;
		}
		return;
	}

	splitplane = node->plane;
	if (splitplane->type >= 3)
	{
		sides = BoxOnPlaneSide(ent->v.absmin, ent->v.absmax, splitplane);
	}
	else
	{
		if (splitplane->dist > ent->v.absmin[splitplane->type])
		{
			if (splitplane->dist < ent->v.absmax[splitplane->type])
			{
				sides = 3;
			}
			else
			{
				sides = 2;
			}
		}
		else
		{
			sides = 1;
		}
	}

	if (sides == 3)
	{
		if (*topnode == -1)
			*topnode = node - g_psv.worldmodel->nodes;
	}

	if (sides & 1)
		SV_FindTouchedLeafs(ent, node->children[0], topnode);

	if (sides & 2)
		SV_FindTouchedLeafs(ent, node->children[1], topnode);
}

/* <caab0> ../engine/world.c:517 */
void SV_LinkEdict(edict_t *ent, qboolean touch_triggers)
{
	static int iTouchLinkSemaphore = 0;
	areanode_t *node;
	int topnode;

	if (ent->area.prev)
		SV_UnlinkEdict(ent);

	if (ent == &g_psv.edicts[0] || ent->free)
		return;

	gEntityInterface.pfnSetAbsBox(ent);
	if (ent->v.movetype == MOVETYPE_FOLLOW && ent->v.aiment)
	{
		ent->headnode = ent->v.aiment->headnode;
		ent->num_leafs = ent->v.aiment->num_leafs;
		Q_memcpy(ent->leafnums, ent->v.aiment->leafnums, sizeof(ent->leafnums));
	}
	else
	{
		ent->num_leafs = 0;
		ent->headnode = -1;
		topnode = -1;
		if (ent->v.modelindex)
			SV_FindTouchedLeafs(ent, g_psv.worldmodel->nodes, &topnode);

		if (ent->num_leafs > MAX_ENT_LEAFS)
		{
			ent->num_leafs = 0;
			ent->headnode = (int)topnode;
			Q_memset(ent->leafnums, -1, sizeof(ent->leafnums));
		}
	}

	if (ent->v.solid == SOLID_NOT && ent->v.skin >= -1)
		return;

	if (ent->v.solid != SOLID_BSP || g_psv.models[ent->v.modelindex] || Q_strlen(&pr_strings[ent->v.model]))
	{
		node = sv_areanodes;
		while (1)
		{
			if (node->axis == -1)
				break;

			if (ent->v.absmin[node->axis] <= node->dist)
			{
				if (ent->v.absmax[node->axis] >= node->dist)
					break;
				node = node->children[1];
			}
			else
			{
				node = node->children[0];
			}
		}

		InsertLinkBefore(&ent->area, (ent->v.solid == SOLID_TRIGGER) ? &node->trigger_edicts : &node->solid_edicts);
		if (touch_triggers)
		{
			if (!iTouchLinkSemaphore)
			{
				iTouchLinkSemaphore = 1;
				SV_TouchLinks(ent, sv_areanodes);
				iTouchLinkSemaphore = 0;
			}
		}
	}
	else
	{
		Con_DPrintf("Inserted %s with no model\n", &pr_strings[ent->v.classname]);
	}
}

/* <ca97c> ../engine/world.c:630 */
int SV_HullPointContents(hull_t *hull, int num, const vec_t *p)
{
	dclipnode_t *node;
	mplane_t *plane;
	float d;

	int i = num;
	while (i >= 0)
	{
		if (hull->firstclipnode > i || hull->lastclipnode < i)
			Sys_Error(__FUNCTION__ ": bad node number");
		node = &hull->clipnodes[i];
		plane = &hull->planes[node->planenum];
		if (plane->type > 2)
			d = plane->normal[0] * *p + plane->normal[1] * p[1] + plane->normal[2] * p[2] - plane->dist;
		else
			d = p[plane->type] - plane->dist;
		i = node->children[(d >= 0.0f) ? 0 : 1];
	}

	return i;
}

/* <cabb7> ../engine/world.c:663 */
int SV_LinkContents(areanode_t *node, const vec_t *pos)
{
	link_t *l;
	link_t *next;
	edict_t *touch;
	model_t *pModel;
	hull_t *hull;
	vec3_t localPosition;
	vec3_t offset;

	for (l = node->solid_edicts.next; l != &node->solid_edicts; l = next)
	{
		next = l->next;
		touch = (edict_t *)((char *)l - offsetof(edict_t, area));
		if (!touch->v.solid)
		{
			if (touch->v.groupinfo)
			{
				if (g_groupop)
				{
					if (g_groupop == GROUP_OP_NAND && (touch->v.groupinfo & g_groupmask))
						continue;
				}
				else
				{
					if (!(touch->v.groupinfo & g_groupmask))
						continue;
				}
			}
			pModel = g_psv.models[touch->v.modelindex];
			if (pModel
				&& !pModel->type
				&& pos[0] <= (double)touch->v.absmax[0]
				&& pos[1] <= (double)touch->v.absmax[1]
				&& pos[2] <= (double)touch->v.absmax[2]
				&& pos[0] >= (double)touch->v.absmin[0]
				&& pos[1] >= (double)touch->v.absmin[1]
				&& pos[2] >= (double)touch->v.absmin[2])
			{
				int contents = touch->v.skin;
				if (contents < -100 || contents > 100)
					Con_DPrintf("Invalid contents on trigger field: %s\n", &pr_strings[touch->v.classname]);
				hull = SV_HullForBsp(touch, vec3_origin, vec3_origin, offset);
				localPosition[0] = pos[0] - offset[0];
				localPosition[1] = pos[1] - offset[1];
				localPosition[2] = pos[2] - offset[2];
				if (SV_HullPointContents(hull, hull->firstclipnode, localPosition) != -1)
					return contents;
			}
		}
	}


	if (node->axis == -1)
		return -1;

	if (pos[node->axis] > node->dist)
		return SV_LinkContents(node->children[0], pos);

	if (pos[node->axis] < node->dist)
		return SV_LinkContents(node->children[1], pos);

	return -1;
}

/* <cac70> ../engine/world.c:738 */
int SV_PointContents(const vec_t *p)
{
	int cont;
	int entityContents;

	cont = SV_HullPointContents(g_psv.worldmodel->hulls, 0, p);
	if (cont <= CONTENTS_CURRENT_0 && cont >= CONTENTS_CURRENT_DOWN)
	{
		cont = CONTENTS_WATER;
	}
	else
	{
		if (cont == CONTENTS_SOLID)
			return CONTENTS_SOLID;
	}

	entityContents = SV_LinkContents(&sv_areanodes[0], p);
	return (entityContents != -1) ? entityContents : cont;
}

/* <cb289> ../engine/world.c:764 */
edict_t *SV_TestEntityPosition(edict_t *ent)
{
	trace_t trace;
	qboolean monsterClip;

	monsterClip = (ent->v.flags & FL_MONSTERCLIP) ? TRUE : FALSE;
	trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, ent->v.origin, 0, ent, monsterClip);
	if (trace.startsolid)
	{
		SV_SetGlobalTrace(&trace);
		return trace.ent;
	}

	return NULL;
}

/* <cacbc> ../engine/world.c:804 */
qboolean SV_RecursiveHullCheck(hull_t *hull, int num, float p1f, float p2f, vec_t *p1, vec_t *p2, trace_t *trace)
{
	dclipnode_t *node;
	mplane_t *plane;
	float t2;
	vec3_t mid;
	float frac;
	float t1;
	signed int side;
	float midf;
	float pdif = p2f - p1f;

	if (num >= 0)
	{
		if (num < hull->firstclipnode || num > hull->lastclipnode || !hull->planes)
			Sys_Error(__FUNCTION__ ": bad node number");

		node = &hull->clipnodes[num];
		plane = &hull->planes[hull->clipnodes[num].planenum];
		if (plane->type >= 3)
		{
			t1 = p1[1] * plane->normal[1] + p1[2] * plane->normal[2] + p1[0] * plane->normal[0] - plane->dist;
			t2 = p2[1] * plane->normal[1] + p2[2] * plane->normal[2] + plane->normal[0] * p2[0] - plane->dist;
		}
		else
		{
			t1 = p1[plane->type] - plane->dist;
			t2 = p2[plane->type] - plane->dist;
		}
		if (t1 >= 0.0f && t2 >= 0.0f)
			return SV_RecursiveHullCheck(hull, node->children[0], p1f, p2f, p1, p2, trace);

		if (t1 >= 0.0f)
		{
			midf = t1 - 0.03125f;
		}
		else
		{
			if (t2 < 0.0f)
				return SV_RecursiveHullCheck(hull, node->children[1], p1f, p2f, p1, p2, trace);

			midf = t1 + 0.03125f;
		}

		midf = midf / (t1 - t2);
		if (midf >= 0.0f)
		{
			if (midf > 1.0f)
				midf = 1.0f;
		}
		else
		{
			midf = 0.0f;
		}
		if (((*reinterpret_cast<int*>(&midf)) & nanmask) != nanmask)
		{
			frac = pdif * midf + p1f;
			mid[0] = (p2[0] - p1[0]) * midf + p1[0];
			mid[1] = (p2[1] - p1[1]) * midf + p1[1];
			mid[2] = (p2[2] - p1[2]) * midf + p1[2];
			side = (t1 < 0.0f) ? 1 : 0;
			if (SV_RecursiveHullCheck(hull, node->children[side], p1f, frac, p1, mid, trace))
			{
				if (SV_HullPointContents(hull, node->children[side ^ 1], mid) != CONTENTS_SOLID)
					return SV_RecursiveHullCheck(hull, node->children[side ^ 1], frac, p2f, mid, p2, trace);

				if (!trace->allsolid)
				{
					if (side)
					{
						trace->plane.normal[0] = vec3_origin[0] - plane->normal[0];
						trace->plane.normal[1] = vec3_origin[1] - plane->normal[1];
						trace->plane.normal[2] = vec3_origin[2] - plane->normal[2];
						trace->plane.dist = -plane->dist;
					}
					else
					{
						trace->plane.normal[0] = plane->normal[0];
						trace->plane.normal[1] = plane->normal[1];
						trace->plane.normal[2] = plane->normal[2];
						trace->plane.dist = plane->dist;
					}

					while (1)
					{
						if (SV_HullPointContents(hull, hull->firstclipnode, mid) != CONTENTS_SOLID)
						{
							trace->fraction = frac;
							trace->endpos[0] = mid[0];
							trace->endpos[1] = mid[1];
							trace->endpos[2] = mid[2];
							return FALSE;
						}
						midf -= 0.1f;
						if (midf < 0.0f)
							break;
						frac = pdif * midf + p1f;
						mid[0] = (p2[0] - p1[1]) * midf + p1[0];
						mid[1] = (p2[1] - p1[1]) * midf + p1[1];
						mid[2] = (p2[2] - p1[2]) * midf + p1[2];
					}
					trace->fraction = frac;
					trace->endpos[0] = mid[0];
					trace->endpos[1] = mid[1];
					trace->endpos[2] = mid[2];
					Con_DPrintf("backup past 0\n");
					return FALSE;
				}
			}
		}
		return FALSE;
	}

	if (num == CONTENTS_SOLID)
	{
		trace->startsolid = TRUE;
	}
	else
	{
		trace->allsolid = FALSE;
		if (num == CONTENTS_EMPTY)
		{
			trace->inopen = TRUE;
			return TRUE;
		}
		if (num != CONTENTS_TRANSLUCENT)
		{
			trace->inwater = TRUE;
			return TRUE;
		}
	}
	return TRUE;
}

/* <cadd3> ../engine/world.c:948 */
void SV_SingleClipMoveToEntity(edict_t *ent, const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, trace_t *trace)
{
	hull_t *hull;
	trace_t testtrace;
	vec3_t offset;
	int rotated;
	int closest;
	vec3_t end_l;
	vec3_t start_l;
	int numhulls;

	Q_memset(trace, 0, sizeof(trace_t));
	trace->fraction = 1.0f;
	trace->allsolid = TRUE;
	trace->endpos[0] = end[0];
	trace->endpos[1] = end[1];
	trace->endpos[2] = end[2];
	if (g_psv.models[ent->v.modelindex]->type == mod_studio)
	{
		hull = SV_HullForStudioModel(ent, mins, maxs, offset, &numhulls);
	}
	else
	{
		hull = SV_HullForEntity(ent, mins, maxs, offset);
		numhulls = 1;
	}
	start_l[0] = start[0] - offset[0];
	start_l[1] = start[1] - offset[1];
	start_l[2] = start[2] - offset[2];
	end_l[0] = end[0] - offset[0];
	end_l[1] = end[1] - offset[1];
	end_l[2] = end[2] - offset[2];
	if (ent->v.solid == SOLID_BSP && (ent->v.angles[0] != 0.0f || ent->v.angles[1] != 0.0f || ent->v.angles[2] != 0.0f))
	{
		vec3_t right;
		vec3_t forward;
		vec3_t up;
		vec3_t temp;

		AngleVectors(ent->v.angles, forward, right, up);

		temp[0] = start_l[0]; temp[1] = start_l[1]; temp[2] = start_l[2];
		start_l[0] = forward[2] * temp[2] + forward[1] * temp[1] + forward[0] * temp[0];
		start_l[1] = -(right[0] * temp[0] + right[2] * temp[2] + right[1] * temp[1]);
		start_l[2] = up[1] * temp[1] + up[0] * temp[0] + up[2] * temp[2];

		temp[0] = end_l[0]; temp[1] = end_l[1]; temp[2] = end_l[2];
		end_l[0] = forward[2] * temp[2] + forward[1] * temp[1] + forward[0] * temp[0];
		end_l[1] = -(right[0] * temp[0] + right[2] * temp[2] + right[1] * temp[1]);
		end_l[2] = up[1] * temp[1] + up[0] * temp[0] + up[2] * temp[2];

		rotated = 1;
	}
	else
	{
		rotated = 0;
	}

	if (numhulls == 1)
	{
		SV_RecursiveHullCheck(hull, hull->firstclipnode, 0.0f, 1.0f, start_l, end_l, trace);
	}
	else
	{
		closest = 0;
		for (int i = 0; i < numhulls; i++)
		{
			Q_memset(&testtrace, 0, sizeof(trace_t));
			testtrace.endpos[0] = end[0];
			testtrace.endpos[1] = end[1];
			testtrace.endpos[2] = end[2];
			testtrace.fraction = 1.0f;
			testtrace.allsolid = TRUE;
			SV_RecursiveHullCheck(&hull[i], hull[i].firstclipnode, 0.0f, 1.0f, start_l, end_l, &testtrace);
			if (i == 0 || testtrace.allsolid || testtrace.startsolid || testtrace.fraction < trace->fraction)
			{
				int isSolid = trace->startsolid;
				memcpy(trace, &testtrace, sizeof(trace_t));
				if (isSolid)
					trace->startsolid = TRUE;
				closest = i;
			}
		}

		trace->hitgroup = SV_HitgroupForStudioHull(closest);
	}

	if (trace->fraction != 1.0f)
	{
		if (rotated)
		{
			vec3_t right;
			vec3_t forward;
			vec3_t up;
			vec3_t temp;

			AngleVectorsTranspose(ent->v.angles, up, right, forward);
			temp[0] = trace->plane.normal[0];
			temp[1] = trace->plane.normal[1];
			temp[2] = trace->plane.normal[2];

			trace->plane.normal[0] = up[2] * temp[2] + up[1] * temp[1] + up[0] * temp[0];
			trace->plane.normal[1] = right[2] * temp[2] + right[1] * temp[1] + right[0] * temp[0];
			trace->plane.normal[2] = forward[2] * temp[2] + forward[1] * temp[1] + forward[0] * temp[0];
		}

		trace->endpos[0] = (end[0] - start[0]) * trace->fraction + start[0];
		trace->endpos[1] = (end[1] - start[1]) * trace->fraction + start[1];
		trace->endpos[2] = (end[2] - start[2]) * trace->fraction + start[2];
	}

	if (trace->fraction < 1.0f || trace->startsolid)
		trace->ent = ent;
}

/* <caf84> ../engine/world.c:1082 */
trace_t SV_ClipMoveToEntity(edict_t *ent, const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end)
{
	trace_t goodtrace;
	SV_SingleClipMoveToEntity(ent, start, mins, maxs, end, &goodtrace);

	return goodtrace;
}

/* <cb027> ../engine/world.c:1148 */
void SV_ClipToLinks(areanode_t *node, moveclip_t *clip)
{
	link_t *l;
	link_t *next;

	for (l = node->solid_edicts.next; l != &node->solid_edicts; l = next)
	{
		next = l->next;
		edict_t *touch = (edict_t *)((char *)l - offsetof(edict_t, area));
		if (touch->v.groupinfo && clip->passedict && clip->passedict->v.groupinfo)
		{
			if (g_groupop)
			{
				if (g_groupop == GROUP_OP_NAND && (clip->passedict->v.groupinfo & touch->v.groupinfo))
					continue;
			}
			else
			{
				if (!(clip->passedict->v.groupinfo & touch->v.groupinfo))
					continue;
			}
		}

		if (touch->v.solid == SOLID_NOT)
			continue;

		if (touch == clip->passedict)
			continue;

		if (touch->v.solid == SOLID_TRIGGER)
			Sys_Error("Trigger in clipping list");

		if (gNewDLLFunctions.pfnShouldCollide && !gNewDLLFunctions.pfnShouldCollide(touch, clip->passedict))
			return;

		if (touch->v.solid == SOLID_BSP)
		{
			if ((touch->v.flags & FL_MONSTERCLIP) && !clip->monsterClipBrush)
				continue;
		}
		else
		{
			if (clip->type == 1 && touch->v.movetype != MOVETYPE_PUSHSTEP)
				continue;
		}

		if ((!clip->ignoretrans || !touch->v.rendermode || (touch->v.flags & FL_WORLDBRUSH))
			&& clip->boxmins[0] <= touch->v.absmax[0]
			&& clip->boxmins[1] <= touch->v.absmax[1]
			&& clip->boxmins[2] <= touch->v.absmax[2]
			&& clip->boxmaxs[0] >= touch->v.absmin[0]
			&& clip->boxmaxs[1] >= touch->v.absmin[1]
			&& clip->boxmaxs[2] >= touch->v.absmin[2]
			&& (touch->v.solid == SOLID_SLIDEBOX || SV_CheckSphereIntersection(touch, clip->start, clip->end))
			&& (!clip->passedict || clip->passedict->v.size[0] == 0.0f || touch->v.size[0] != 0.0f))
		{
			if (clip->trace.allsolid)
				return;

			if (clip->passedict && (touch->v.owner == clip->passedict || clip->passedict->v.owner == touch))
				continue;

			trace_t trace;
			if (touch->v.flags & FL_MONSTER)
				trace = SV_ClipMoveToEntity(touch, clip->start, clip->mins2, clip->maxs2, clip->end);
			else
				trace = SV_ClipMoveToEntity(touch, clip->start, clip->mins, clip->maxs, clip->end);

			if (trace.allsolid || trace.startsolid || trace.fraction < clip->trace.fraction)
			{
				int oldStartSolid = clip->trace.startsolid;
				trace.ent = touch;
				clip->trace = trace;
				if (oldStartSolid)
					clip->trace.startsolid = TRUE;
			}
		}
	}

	if (node->axis != -1)
	{
		if (clip->boxmaxs[node->axis] > node->dist)
			SV_ClipToLinks(node->children[0], clip);

		if (node->dist > clip->boxmins[node->axis])
			SV_ClipToLinks(node->children[1], clip);
	}
}

/* <cb142> ../engine/world.c:1267 */
void SV_ClipToWorldbrush(areanode_t *node, moveclip_t *clip)
{
	link_t *l;
	link_t *next;

	for (l = node->solid_edicts.next; l != &node->solid_edicts; l = next)
	{
		next = l->next;
		edict_t *touch = (edict_t *)((char *)l - offsetof(edict_t, area));

		if (touch->v.solid != SOLID_BSP)
			continue;

		if (!(touch->v.flags & FL_WORLDBRUSH))
			continue;

		if (clip->boxmins[0] <= touch->v.absmax[0]
			&& clip->boxmins[1] <= touch->v.absmax[1]
			&& clip->boxmins[2] <= touch->v.absmax[2]
			&& clip->boxmaxs[0] >= touch->v.absmin[0]
			&& clip->boxmaxs[1] >= touch->v.absmin[1]
			&& clip->boxmaxs[2] >= touch->v.absmin[2])
		{
			if (clip->trace.allsolid)
				return;

			trace_t trace = SV_ClipMoveToEntity(touch, clip->start, clip->mins, clip->maxs, clip->end);
			if (trace.allsolid || trace.startsolid || trace.fraction < clip->trace.fraction)
			{
				int oldSolid = clip->trace.startsolid;
				trace.ent = touch;
				clip->trace = trace;
				if (oldSolid)
					clip->trace.startsolid = TRUE;
			}
		}
	}

	if (node->axis != -1)
	{
		if (clip->boxmaxs[node->axis] > node->dist)
			SV_ClipToWorldbrush(node->children[0], clip);

		if (node->dist > clip->boxmins[node->axis])
			SV_ClipToWorldbrush(node->children[1], clip);
	}
}

/* <cb206> ../engine/world.c:1330 */
void SV_MoveBounds(const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, vec_t *boxmins, vec_t *boxmaxs)
{
	for (int i = 0; i < 3; i++)
	{
		if (end[i] > start[i])
		{
			boxmins[i] = start[i] + mins[i] - 1.0f;
			boxmaxs[i] = end[i] + maxs[i] + 1.0f;
		}
		else
		{
			boxmins[i] = end[i] + mins[i] - 1.0f;
			boxmaxs[i] = start[i] + maxs[i] + 1.0f;
		}
	}
}

/* <cb391> ../engine/world.c:1364 */
trace_t SV_MoveNoEnts(const vec_t *start, vec_t *mins, vec_t *maxs, const vec_t *end, int type, edict_t *passedict)
{
	moveclip_t clip;
	vec3_t worldEndPoint;
	float worldFraction;

	Q_memset(&clip, 0, sizeof(clip));
	clip.trace = SV_ClipMoveToEntity(g_psv.edicts, start, mins, maxs, end);
	if (clip.trace.fraction != 0.0f)
	{
		worldEndPoint[0] = clip.trace.endpos[0];
		worldEndPoint[2] = clip.trace.endpos[2];
		worldEndPoint[1] = clip.trace.endpos[1];
		clip.end = worldEndPoint;

		clip.ignoretrans = type >> 8;
		worldFraction = clip.trace.fraction;
		clip.type = type;
		clip.passedict = passedict;

		clip.mins2[0] = mins[0];
		clip.mins2[1] = mins[1];
		clip.mins2[2] = mins[2];
		clip.maxs2[0] = maxs[0];
		clip.maxs2[1] = maxs[1];
		clip.maxs2[2] = maxs[2];

		clip.trace.fraction = 1.0f;
		clip.start = start;
		clip.mins = mins;
		clip.maxs = maxs;
		clip.monsterClipBrush = FALSE;

		SV_MoveBounds(start, clip.mins2, clip.maxs2, worldEndPoint, clip.boxmins, clip.boxmaxs);
		SV_ClipToWorldbrush(sv_areanodes, &clip);

		gGlobalVariables.trace_ent = clip.trace.ent;
		clip.trace.fraction = worldFraction * clip.trace.fraction;
	}

	return clip.trace;
}

/* <cb47e> ../engine/world.c:1415 */
trace_t SV_Move(const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, int type, edict_t *passedict, qboolean monsterClipBrush)
{
	moveclip_t clip;
	vec3_t worldEndPoint;
	float worldFraction;

	Q_memset(&clip, 0, sizeof(clip));
	clip.trace = SV_ClipMoveToEntity(g_psv.edicts, start, mins, maxs, end);
	if (clip.trace.fraction != 0.0f)
	{
		worldEndPoint[0] = clip.trace.endpos[0];
		worldEndPoint[1] = clip.trace.endpos[1];
		worldEndPoint[2] = clip.trace.endpos[2];
		clip.end = worldEndPoint;
		worldFraction = clip.trace.fraction;

		clip.type = (unsigned char)type;
		clip.ignoretrans = type >> 8;
		clip.trace.fraction = 1.0f;
		clip.start = start;
		clip.mins = mins;
		clip.maxs = maxs;
		clip.passedict = passedict;
		clip.monsterClipBrush = monsterClipBrush;
		if (type == 2)
		{
			for (int i = 0; i < 3; i++)
			{
				clip.mins2[i] = -15.0f;
				clip.maxs2[i] = +15.0f;
			}
		}
		else
		{
			clip.mins2[0] = mins[0];
			clip.mins2[1] = mins[1];
			clip.mins2[2] = mins[2];
			clip.maxs2[0] = maxs[0];
			clip.maxs2[1] = maxs[1];
			clip.maxs2[2] = maxs[2];
		}
		SV_MoveBounds(start, clip.mins2, clip.maxs2, worldEndPoint, clip.boxmins, clip.boxmaxs);
		SV_ClipToLinks(sv_areanodes, &clip);
		gGlobalVariables.trace_ent = clip.trace.ent;
		clip.trace.fraction = worldFraction * clip.trace.fraction;
	}

	return clip.trace;
}
