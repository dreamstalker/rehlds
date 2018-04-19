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

#pragma once

#include "model.h"

#define	PITCH   0 // up / down
#define	YAW     1 // left / right
#define	ROLL    2 // fall over

extern vec3_t vec3_origin;

static const int nanmask = 0x7F800000;
#define IS_NAN(fvar) ((*reinterpret_cast<int*>(&(fvar)) & nanmask) == nanmask)

#define VectorScale(a,b,c) {(c)[0]=(b)*(a)[0];(c)[1]=(b)*(a)[1];(c)[2]=(b)*(a)[2];}
#define VectorSubtract(a,b,c) {(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];(c)[2]=(a)[2]-(b)[2];}
#define VectorAdd(a,b,c) {(c)[0]=(a)[0]+(b)[0];(c)[1]=(a)[1]+(b)[1];(c)[2]=(a)[2]+(b)[2];}
#define VectorCopy(a,b) {(b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];}
#define VectorClear(a) {(a)[0]=0.0;(a)[1]=0.0;(a)[2]=0.0;}

float DotProduct(const vec_t *v1, const vec_t *v2);
float TriangleDiameter(const vec_t *v1, vec_t *v2, vec_t *v3);
float VectorNormalize(vec_t *v);
float VectorDistance(const vec_t *v1, const vec_t *v2);
void VectorAngles(const float *forward, float *angles);
void VectorRAngles(const float *v, float *a);
int BoxOnPlaneSide(vec_t *emins, vec_t *emaxs, struct mplane_s *p);
float AngleBetweenVectors(const vec_t *angle1, const vec_t *angle2);
float RAngleBetweenVectors(const vec_t *angle1, const vec_t *angle2);
void NormalizeAngles(vec_t *angles);
void NormalizeRAngles(vec_t *a);
void AngleVectors(const vec_t *angles, vec_t *forward, vec_t *right = nullptr, vec_t *up = nullptr);
int AngleLeftOfOther(const vec_t *v1, const vec_t *v2);
float RadiusFromBounds(const vec_t *mins, const vec_t *maxs);
bool SolveLSE(vec_t *v0, vec_t *v1, vec_t *v2, vec_t *v3, float *x, float *y, float *z);
void NormalizePoints(vec_t *v0, vec_t *v1);
void Normal2Plane(vec_t *v1, vec_t *v2, vec_t *v3);
float Length(const vec_t *v);
