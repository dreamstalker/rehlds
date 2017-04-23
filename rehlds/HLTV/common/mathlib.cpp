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

vec3_t vec3_origin;

float TriangleDiameter(const vec_t *v1, vec_t *v2, vec_t *v3)
{
	vec3_t e1, e2, e3;
	float l1, l2, l3;
	int i;

	l1 = 0;
	l2 = 0;
	l3 = 0;

	VectorSubtract(v2, v1, e1);
	VectorSubtract(v3, v2, e2);
	VectorSubtract(v1, v3, e3);

	for (i = 0; i < 3; i++) {
		l3 = e1[i] * e1[i] + l3;
	}

	for (i = 0; i < 3; i++) {
		l2 = e2[i] * e2[i] + l2;
	}

	for (i = 0; i < 3; i++) {
		l1 = e3[i] * e3[i] + l1;
	}

	if (l3 > l2 && l3 > l1) {
		return sqrt(l3);
	}

	if (l2 > l3 && l2 > l1) {
		return sqrt(l2);
	}

	return sqrt(l1);
}

float VectorNormalize(vec_t *v)
{
	float length = Length(v);
	if (length)
	{
		float ilength = 1 / length;

		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return length;
}

float VectorDistance(const vec_t *v1, const vec_t *v2)
{
	vec3_t t;
	VectorSubtract(v1, v2, t);
	return Length(t);
}

void VectorAngles(const float *forward, float *angles)
{
	float tmp, yaw, pitch;
	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = float(atan2(forward[1], forward[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = float(atan2(forward[2], tmp) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void VectorRAngles(const float *v, float *a)
{
	a[0] = atan2(v[2], sqrt(v[0] * v[0] + v[1] * v[1]));
	a[1] = atan2(v[1], v[0]);
	NormalizeRAngles(a);
}

int BoxOnPlaneSide(vec_t *emins, vec_t *emaxs, mplane_t *p)
{
	// From sources
	float dist1, dist2;
	int sides = 0;

	// general case
	switch (p->signbits)
	{
	case 0:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		break;
	case 1:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		break;
	case 2:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		break;
	case 3:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		break;
	case 4:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		break;
	case 5:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emaxs[2];
		break;
	case 6:
		dist1 = p->normal[0] * emaxs[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emins[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		break;
	case 7:
		dist1 = p->normal[0] * emins[0] + p->normal[1] * emins[1] + p->normal[2] * emins[2];
		dist2 = p->normal[0] * emaxs[0] + p->normal[1] * emaxs[1] + p->normal[2] * emaxs[2];
		break;
	default:
		dist1 = dist2 = 0.0;
		break;
	}

	if (dist1 >= p->dist)
		sides = 1;
	if (dist2 < p->dist)
		sides |= 2;

	return sides;
}

float AngleBetweenVectors(const vec_t *angle1, const vec_t *angle2)
{
	float angle;
	float d1 = Length(angle1);
	float d2 = Length(angle2);

	if (!d1 || !d2) {
		return 0;
	}

	angle = acos(DotProduct(angle1, angle2) / (d1 * d2));
	angle = angle * float(180.0f / M_PI);

	return angle;
}

float RAngleBetweenVectors(const vec_t *angle1, const vec_t *angle2)
{
	float d1, d2, d3;

	d1 = Length(angle1);
	d2 = Length(angle2);

	if (!d1 || !d2) {
		return 0;
	}

	d3 = DotProduct(angle1, angle2) / (d1 * d2);
	if (d3 > 1) {
		return 0;
	}

	if (d3 < -1) {
		return (float)M_PI;
	}

	return acosf(d3);
}

void NormalizeAngles(vec_t *angles)
{
	// Normalize angles
	for (int i = 0; i < 3; ++i)
	{
		if (angles[i] > 180.0)
		{
			angles[i] -= 360.0;
		}
		else if (angles[i] < -180.0)
		{
			angles[i] += 360.0;
		}
	}
}

void NormalizeRAngles(vec_t *a)
{
	if (a[0] > M_PI) {
		a[0] -= float(M_PI * 2);
	}
	else if (a[0] < -M_PI) {
		a[0] += float(M_PI * 2);
	}

	if (a[1] > M_PI) {
		a[1] -= float(M_PI * 2);
	}
	else if (a[1] < -M_PI) {
		a[1] += float(M_PI * 2);
	}

	a[2] = 0;
}

void AngleVectors(const vec_t *angles, vec_t *forward, vec_t *right, vec_t *up)
{
	float sr, sp, sy, cr, cp, cy;

	float angle;
	angle = (float)(angles[YAW] * (M_PI * 2 / 360));
	sy = sin(angle);
	cy = cos(angle);
	angle = (float)(angles[PITCH] * (M_PI * 2 / 360));
	sp = sin(angle);
	cp = cos(angle);
	angle = (float)(angles[ROLL] * (M_PI * 2 / 360));
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;
	}

	if (right)
	{
		right[0] = (-1 * sr * sp * cy + -1 * cr * -sy);
		right[1] = (-1 * sr * sp * sy + -1 * cr * cy);
		right[2] = -1 * sr * cp;
	}

	if (up)
	{
		up[0] = (cr * sp * cy + -sr * -sy);
		up[1] = (cr * sp * sy + -sr * cy);
		up[2] = cr * cp;
	}
}

int AngleLeftOfOther(const vec_t *v1, const vec_t *v2)
{
	float a = v1[1];
	float b = v2[1];

	if (a >= 180)
		return (b >= a) || ((a - 180) > b);
	else
		return (b > a) && ((a + 180) >= b);
}

float RadiusFromBounds(const vec_t *mins, const vec_t *maxs)
{
	vec3_t corner;
	for (int i = 0; i < 3; i++)
	{
		float fmin = fabs(mins[i]);
		float fmax = fabs(mins[i]);

		if (fmin > fmax)
			corner[i] = fmin;
		else
			corner[i] = fmax;
	}

	return Length(corner);
}

bool SolveLSE(vec_t *v0, vec_t *v1, vec_t *v2, vec_t *v3, float *x, float *y, float *z)
{
	float d = v1[2] * (v2[0] * v3[1] - v2[1] * v3[0]) + v1[1] * (v3[0] * v2[2] - v2[0] * v3[2]) + v1[0] * (v2[1] * v3[2] - v3[1] * v2[2]);
	if (!d) {
		return false;
	}

	if (x) *x = ((v2[0] * v3[1] - v2[1] * v3[0]) * v0[2] + (v3[0] * v2[2] - v2[0] * v3[2]) * v0[1] + (v2[1] * v3[2] - v3[1] * v2[2]) * v0[0]) / d;
	if (y) *y = ((v0[0] * v3[1] - v0[1] * v3[0]) * v1[2] + (v3[0] * v0[2] - v0[0] * v3[2]) * v1[1] + (v0[1] * v3[2] - v3[1] * v0[2]) * v1[0]) / d;
	if (z) *z = ((v2[0] * v0[1] - v2[1] * v0[0]) * v1[2] + (v0[0] * v2[2] - v2[0] * v0[2]) * v1[1] + (v2[1] * v0[2] - v0[1] * v2[2]) * v1[0]) / d;

	return true;
}

void NormalizePoints(vec_t *v0, vec_t *v1)
{
	vec3_t a;
	VectorSubtract(v1, v0, a);

	if (Length(a))
	{
		VectorNormalize(a);
		VectorAdd(a, v0, v1);
	}
}

void Normal2Plane(vec_t *v1, vec_t *v2, vec_t *v3)
{
	if (v1[0])
	{
		v2[0] = -v1[1] / v1[0];
		v2[1] = 1;
		v2[2] = 0;

		v3[0] = -v1[2] / v1[0];
		v3[1] = 0;
		v3[2] = 1;
		return;
	}

	if (v1[1])
	{
		v2[0] = 1;
		v2[1] = -v1[0] / v1[1];
		v2[2] = 0;

		v3[0] = 0;
		v3[1] = -v1[2] / v1[1];
		v3[2] = 1;

		return;
	}

	v2[0] = 1;
	v2[1] = 0;
	v2[2] = -v1[0] / v1[2];

	v3[0] = 0;
	v3[1] = 1;
	v3[2] = -v1[1] / v1[2];
}

float Length(const vec_t *v)
{
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float DotProduct(const vec_t *v1, const vec_t *v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
