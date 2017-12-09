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

#include "maintypes.h"
#include "model.h"

#ifndef REHLDS_FIXES
// NOTE: In some cases we need high precision of floating-point,
// so use double instead of float, otherwise unittest will fail
typedef double real_t;
#else
typedef float real_t;
#endif

typedef real_t real3_t[3];

enum
{
	PITCH = 0,	// up / down
	YAW,		// left / right
	ROLL		// fall over
};

#define RAD2DEG(x)	((float)(x) * (float)(180.f / M_PI))
#define DEG2RAD(x)	((float)(x) * (float)(M_PI / 180.f))

#ifdef HOOK_ENGINE
#define vec3_origin (*pvec3_origin)
#endif // HOOK_ENGINE

#define BOX_ON_PLANE_SIDE(emins, emaxs, p)      \
	(((p)->type < 3) ?                          \
	(                                           \
		((p)->dist <= (emins)[(p)->type]) ?     \
			1                                   \
		:                                       \
		(                                       \
			((p)->dist >= (emaxs)[(p)->type]) ? \
				2                               \
			:                                   \
				3                               \
		)                                       \
	)                                           \
	:                                           \
		BoxOnPlaneSide((emins), (emaxs), (p)))

extern vec3_t vec3_origin;

static const int nanmask = 0x7F800000;

#define IS_NAN(fvar) ((*reinterpret_cast<int*>(&(fvar)) & nanmask) == nanmask)

inline float M_sqrt(float value) {
	return _mm_cvtss_f32(_mm_sqrt_ss(_mm_load_ss(&value)));
}

inline double M_sqrt(double value) {
	auto v = _mm_load_sd(&value);
	return _mm_cvtsd_f64(_mm_sqrt_sd(v, v));
}

template<typename T>
inline double M_sqrt(T value) {
	return sqrt(value);
}

inline float M_min(float a, float b) {
	return _mm_cvtss_f32(_mm_min_ss(_mm_load_ss(&a), _mm_load_ss(&b)));
}

inline double M_min(double a, double b) {
	return _mm_cvtsd_f64(_mm_min_sd(_mm_load_sd(&a), _mm_load_sd(&b)));
}

template<typename T>
inline T M_min(T a, T b) {
	return min(a, b);
}

inline float M_max(float a, float b) {
	return _mm_cvtss_f32(_mm_max_ss(_mm_load_ss(&a), _mm_load_ss(&b)));
}

inline double M_max(double a, double b) {
	return _mm_cvtsd_f64(_mm_max_sd(_mm_load_sd(&a), _mm_load_sd(&b)));
}

template<typename T>
inline T M_max(T a, T b) {
	return max(a, b);
}

inline float M_clamp(float a, float min, float max) {
	return _mm_cvtss_f32(_mm_min_ss(_mm_max_ss(_mm_load_ss(&a), _mm_load_ss(&min)), _mm_load_ss(&max)));
}

inline double M_clamp(double a, double min, double max) {
	return _mm_cvtsd_f64(_mm_min_sd(_mm_max_sd(_mm_load_sd(&a), _mm_load_sd(&min)), _mm_load_sd(&max)));
}

template<typename T>
inline T M_clamp(T a, T min, T max) {
	return clamp(a, min, max);
}

inline void VectorAdd(const vec_t *veca, const vec_t *vecb, vec_t *out)
{
	out[0] = veca[0] + vecb[0];
	out[1] = veca[1] + vecb[1];
	out[2] = veca[2] + vecb[2];
}

template <typename T = vec_t>
inline void VectorSubtract(const vec_t *veca, const vec_t *vecb, T *out)
{
	out[0] = veca[0] - vecb[0];
	out[1] = veca[1] - vecb[1];
	out[2] = veca[2] - vecb[2];
}

#ifndef REHLDS_FIXES
template <typename T = vec_t>
inline void VectorMA(const vec_t *veca, float scale, const T *vecm, vec_t *out)
{
	out[0] = scale * vecm[0] + veca[0];
	out[1] = scale * vecm[1] + veca[1];
	out[2] = scale * vecm[2] + veca[2];
}
#endif

inline void VectorScale(const vec_t *in, float scale, vec_t *out)
{
	out[0] = scale * in[0];
	out[1] = scale * in[1];
	out[2] = scale * in[2];
}

inline void VectorClear(vec_t *in)
{
	in[0] = 0.0f;
	in[1] = 0.0f;
	in[2] = 0.0f;
}

inline void VectorCopy(const vec_t *in, vec_t *out)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

inline void VectorNegate(const vec_t *in, vec_t *out)
{
	out[0] = -in[0];
	out[1] = -in[1];
	out[2] = -in[2];
}

inline void VectorAverage(const vec_t *veca, const vec_t *vecb, vec_t *out)
{
	out[0] = (veca[0] + vecb[0]) * 0.5f;
	out[1] = (veca[1] + vecb[1]) * 0.5f;
	out[2] = (veca[2] + vecb[2]) * 0.5f;
}

inline bool VectorIsZero(const vec_t *in)
{
	return (in[0] == 0.0f && in[1] == 0.0f && in[2] == 0.0f);
}

float anglemod(float a);
void BOPS_Error(void);

int BoxOnPlaneSide(vec_t *emins, vec_t *emaxs, mplane_t *p);
NOBODY int InvertMatrix(const float *m, float *out);
void AngleVectors_ext(const vec_t *angles, vec_t *forward, vec_t *right, vec_t *up);
void AngleVectors(const vec_t *angles, vec_t *forward, vec_t *right, vec_t *up);
void AngleVectorsTranspose(const vec_t *angles, vec_t *forward, vec_t *right, vec_t *up);
void AngleMatrix(const vec_t *angles, float(*matrix)[4]);
NOBODY void AngleIMatrix(const vec_t *angles, float *matrix);
NOBODY void NormalizeAngles(float *angles);
NOBODY void InterpolateAngles(float *start, float *end, float *output, float frac);
void VectorTransform(const vec_t *in1, float (*in2)[4], vec_t *out);
int VectorCompare(const vec_t *v1, const vec_t *v2);
void VectorMA(const vec_t *veca, float scale, const vec_t *vecm, vec_t *out);
real_t _DotProduct(const vec_t *v1, const vec_t *v2);
NOBODY void _VectorSubtract(vec_t *veca, vec_t *vecb, vec_t *out);
void _VectorAdd(vec_t *veca, vec_t *vecb, vec_t *out);
NOBODY void _VectorCopy(vec_t *in, vec_t *out);
void CrossProduct(const vec_t *v1, const vec_t *v2, vec_t *cross);
float Length(const vec_t *v);
float Length2D(const vec_t *v);
float VectorNormalize(vec_t *v);
NOBODY void VectorInverse(vec_t *v);
void VectorScale(const vec_t *in, float scale, vec_t *out);
NOBODY int Q_log2(int val);
NOBODY void VectorMatrix(vec_t *forward, vec_t *right, vec_t *up);
void VectorAngles(const vec_t *forward, vec_t *angles);
NOBODY void R_ConcatRotations(float *in1, float *in2, float *out);
void R_ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]);
NOBODY void FloorDivMod(double numer, double denom, int *quotient, int *rem);
NOBODY int GreatestCommonDivisor(int i1, int i2);
NOBODY fixed16_t Invert24To16(fixed16_t val);
