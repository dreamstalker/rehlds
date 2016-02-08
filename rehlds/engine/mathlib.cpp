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

// Intrisics guide: https://software.intel.com/sites/landingpage/IntrinsicsGuide/
// Shufps calculator: http://wurstcaptures.untergrund.net/assembler_tricks.html

vec3_t vec3_origin;
//int nanmask;
//short int new_cw;
//short int old_cw;
//DLONG dlong;

// aligned vec4_t
typedef ALIGN16 vec4_t avec4_t;
typedef ALIGN16 int aivec4_t[4];

// conversion multiplier
const avec4_t deg2rad =
{
	M_PI / 180.f,
	M_PI / 180.f,
	M_PI / 180.f,
	M_PI / 180.f
};

const aivec4_t negmask[4] =
{
	0x80000000,
	0x80000000,
	0x80000000,
	0x80000000
};

const aivec4_t negmask_1001 =
{
	0x80000000,
	0,
	0,
	0x80000000
};

const aivec4_t negmask_0010 =
{
	0,
	0,
	0x80000000,
	0
};

// save 4d xmm to 3d vector. we can't optimize many simple vector3 functions because saving back to 3d is slow.
static inline void xmm2vec(vec_t *v, const __m128 m)
{
	_mm_storel_pi((__m64*)v, m);
	_mm_store_ss(v + 2, _mm_shuffle_ps(m, m, 0x02));
}

/* <46ebf> ../engine/mathlib.c:14 */
float anglemod(float a)
{
	return (360.0 / 65536) * ((int)(a*(65536 / 360.0)) & 65535);
}

/* <46eea> ../engine/mathlib.c:33 */
void BOPS_Error(void)
{
	Sys_Error("BoxOnPlaneSide:  Bad signbits");
}

#ifdef REHLDS_OPT_PEDANTIC
int BoxOnPlaneSide(vec_t *emins, vec_t *emaxs, mplane_t *p)
{
	double	dist1, dist2;
	int		sides = 0;

	__m128 emin = _mm_loadu_ps(emins);
	__m128 emax = _mm_loadu_ps(emaxs);;
	avec4_t d1, d2;

	// general case
	switch (p->signbits)
	{
	case 0:
		_mm_store_ps(d1, emax);
		_mm_store_ps(d2, emin);
		break;
	case 1:
		_mm_store_ps(d1, emax);
		_mm_store_ps(d2, emin);
		d1[0] = emins[0];
		d2[0] = emaxs[0];
		break;
	case 2:
		_mm_store_ps(d1, emax);
		_mm_store_ps(d2, emin);
		d1[1] = emins[1];
		d2[1] = emaxs[1];
		break;
	case 3:
		_mm_store_ps(d1, emin);
		_mm_store_ps(d2, emax);
		d1[2] = emaxs[2];
		d2[2] = emins[2];
		break;
	case 4:
		_mm_store_ps(d1, emax);
		_mm_store_ps(d2, emin);
		d1[2] = emins[2];
		d2[2] = emaxs[2];
		break;
	case 5:
		_mm_store_ps(d1, emin);
		_mm_store_ps(d2, emax);
		d1[1] = emaxs[1];
		d2[1] = emins[1];
		break;
	case 6:
		_mm_store_ps(d1, emin);
		_mm_store_ps(d2, emax);
		d1[0] = emaxs[0];
		d2[0] = emins[0];
		break;
	case 7:
		_mm_store_ps(d1, emin);
		_mm_store_ps(d2, emax);
		break;
	default:
		BOPS_Error();
		dist1 = dist2 = 0.0;
		break;
	}

	dist1 = _DotProduct(p->normal, d1);
	dist2 = _DotProduct(p->normal, d2);

	if (dist1 >= p->dist)
		sides = 1;
	if (dist2 < p->dist)
		sides |= 2;

	return sides;
}
#else
/* <46f05> ../engine/mathlib.c:48 */
int BoxOnPlaneSide(vec_t *emins, vec_t *emaxs, mplane_t *p)
{
#if (1)
	// Engine actual types
	double	dist1, dist2;
#else
	// From sources
	float	dist1, dist2;
#endif
	int		sides = 0;

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
		BOPS_Error();
		dist1 = dist2 = 0.0;
		break;
	}

	if (dist1 >= p->dist)
		sides = 1;
	if (dist2 < p->dist)
		sides |= 2;

	return sides;
}
#endif

/* <46f8d> ../engine/mathlib.c:157 */
NOBODY int InvertMatrix(const float *m, float *out);
//{
//	float wtmp;                                                   //   159
//	float m0;                                                     //   160
//	float m1;                                                     //   160
//	float m2;                                                     //   160
//	float m3;                                                     //   160
//	float s;                                                      //   160
//	float *r0;                                                   //   161
//	float *r1;                                                   //   161
//	float *r2;                                                   //   161
//	float *r3;                                                   //   161
//}

void EXT_FUNC AngleVectors_ext(const vec_t *angles, vec_t *forward, vec_t *right, vec_t *up) {
	AngleVectors(angles, forward, right, up);
}

#ifdef REHLDS_FIXES
// parallel SSE version
void AngleVectors(const vec_t *angles, vec_t *forward, vec_t *right, vec_t *up)
{
#ifndef SWDS
	g_engdstAddrs.pfnAngleVectors(&angles, &forward, &right, &up);
#endif // SWDS

	__m128 s, c;
	sincos_ps(_mm_mul_ps(_mm_loadu_ps(angles), _mm_load_ps(deg2rad)), &s, &c);

	__m128 m1 = _mm_shuffle_ps(c, s, 0x90); // [cp][cp][sy][sr]
	__m128 m2 = _mm_shuffle_ps(c, c, 0x09); // [cy][cr][cp][cp]
	__m128 cp_mults = _mm_mul_ps(m1, m2); // [cp * cy][cp * cr][cp * sy][cp * sr];

	m1 = _mm_shuffle_ps(c, s, 0x15); // [cy][cy][sy][sp]
	m2 = _mm_shuffle_ps(s, c, 0xA0); // [sp][sp][cr][cr]
	m1 = _mm_shuffle_ps(m1, m1, 0xC8); // [cy][sy][cy][sp]

	__m128 m3 = _mm_shuffle_ps(s, s, 0x4A); // [sr][sr][sp][sy];
	m3 = _mm_mul_ps(m3, _mm_mul_ps(m1, m2)); // [sp*cy*sr][sp*sy*sr][cr*cy*sp][cr*sp*sy]

	m2 = _mm_shuffle_ps(s, c, 0x65); // [sy][sy][cr][cy]
	m1 = _mm_shuffle_ps(c, s, 0xA6); // [cr][cy][sr][sr]
	m2 = _mm_shuffle_ps(m2, m2, 0xD8); // [sy][cr][sy][cy]
	m1 = _mm_xor_ps(m1, _mm_load_ps((float *)&negmask_1001)); // [-cr][cy][sr][-sr]
	m1 = _mm_mul_ps(m1, m2); // [-cr*sy][cy*cr][sr*sy][-sr*cy]

	m3 = _mm_add_ps(m3, m1);

	if (forward)
	{
		_mm_storel_pi((__m64 *)forward, _mm_shuffle_ps(cp_mults, cp_mults, 0x08));
		forward[2] = -_mm_cvtss_f32(s);
	}
	if (right)
	{
		__m128 r = _mm_shuffle_ps(m3, cp_mults, 0xF4); // [m3(0)][m3(1)][cp(3)][cp(3)]
		xmm2vec(right, _mm_xor_ps(r, _mm_load_ps((float *)&negmask)));
	}
	if (up)
	{
		_mm_storel_pi((__m64 *)up, _mm_shuffle_ps(m3, m3, 0x0E));
		up[2] = _mm_cvtss_f32(_mm_shuffle_ps(cp_mults, cp_mults, 0x01));
	}
}
#else // REHLDS_FIXES
/* <47067> ../engine/mathlib.c:267 */
void AngleVectors(const vec_t *angles, vec_t *forward, vec_t *right, vec_t *up)
{
	float		sr, sp, sy, cr, cp, cy;

#ifndef SWDS
	g_engdstAddrs.pfnAngleVectors(&angles, &forward, &right, &up);
#endif // SWDS

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
		forward[0] = cp*cy;
		forward[1] = cp*sy;
		forward[2] = -sp;
	}
	if (right)
	{
		right[0] = (-1 * sr*sp*cy + -1 * cr*-sy);
		right[1] = (-1 * sr*sp*sy + -1 * cr*cy);
		right[2] = -1 * sr*cp;
	}
	if (up)
	{
		up[0] = (cr*sp*cy + -sr*-sy);
		up[1] = (cr*sp*sy + -sr*cy);
		up[2] = cr*cp;
	}
}
#endif // REHLDS_FIXES

#ifdef REHLDS_FIXES
// parallel SSE version
void AngleVectorsTranspose(const vec_t *angles, vec_t *forward, vec_t *right, vec_t *up)
{
	__m128 s, c;
	sincos_ps(_mm_mul_ps(_mm_loadu_ps(angles), _mm_load_ps(deg2rad)), &s, &c);

	__m128 m1 = _mm_shuffle_ps(c, s, 0x90); // [cp][cp][sy][sr]
	__m128 m2 = _mm_shuffle_ps(c, c, 0x09); // [cy][cr][cp][cp]
	__m128 cp_mults = _mm_mul_ps(m1, m2); // [cp * cy][cp * cr][cp * sy][cp * sr];

	m1 = _mm_shuffle_ps(s, s, 0x50); // [sp][sp][sy][sy]
	m2 = _mm_shuffle_ps(c, s, 0x05); // [cy][cy][sp][sp]

	__m128 m3 = _mm_shuffle_ps(s, c, 0xAA); // [sr][sr][cr][cr]
	m1 = _mm_mul_ps(m1, m2);
	m3 = _mm_shuffle_ps(m3, m3, 0xD8); // [sr][cr][sr][cr]
	m3 = _mm_mul_ps(m3, m1); // [sp*cy*sr][sp*cy*cr][sy*sp*sr][sy*sp*cr]

	m2 = _mm_shuffle_ps(c, s, 0xA6); // [cr][cy][sr][sr]
	m1 = _mm_shuffle_ps(s, c, 0x65); // [sy][sy][cr][cy]
	m2 = _mm_shuffle_ps(m2, m2, 0xD8); // [cr][sr][cy][sr]
	m1 = _mm_xor_ps(m1, _mm_load_ps((float *)&negmask_1001)); // [-cr][cy][sr][-sr]
	m1 = _mm_mul_ps(m1, m2); // [-cr*sy][sr*sy][cy*cr][-sr*cy]

	m3 = _mm_add_ps(m3, m1);

	if (forward)
	{
		forward[0] = _mm_cvtss_f32(cp_mults);
		_mm_storel_pi((__m64*)(forward + 1), m3); // (sr*sp*cy + cr*-sy);
	}
	if (right)
	{
		right[0] = _mm_cvtss_f32(_mm_shuffle_ps(cp_mults, cp_mults, 0x02));
		_mm_storel_pi((__m64*)(right + 1), _mm_shuffle_ps(m3, m3, 0x0E));
	}
	if (up)
	{
		up[0] = -_mm_cvtss_f32(s);
		_mm_storel_pi((__m64 *)&up[1], _mm_shuffle_ps(cp_mults, cp_mults, 0x07));
	}
}
#else // REHLDS_FIXES
/* <4712e> ../engine/mathlib.c:304 */
void AngleVectorsTranspose(const vec_t *angles, vec_t *forward, vec_t *right, vec_t *up)
{
	float		sr, sp, sy, cr, cp, cy;

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
		forward[0] = cp*cy;
		forward[1] = (sr*sp*cy + cr*-sy);
		forward[2] = (cr*sp*cy + -sr*-sy);
	}
	if (right)
	{
		right[0] = cp*sy;
		right[1] = (sr*sp*sy + cr*cy);
		right[2] = (cr*sp*sy + -sr*cy);
	}
	if (up)
	{
		up[0] = -sp;
		up[1] = sr*cp;
		up[2] = cr*cp;
	}
}
#endif

#ifdef REHLDS_FIXES
// parallel SSE version
void AngleMatrix(const vec_t *angles, float(*matrix)[4])
{
	__m128 s, c;
	sincos_ps(_mm_mul_ps(_mm_loadu_ps(angles), _mm_load_ps(deg2rad)), &s, &c);

	/*
	matrix[0][1] = sr * sp * cy - cr * sy;
	matrix[1][1] = sr * sp * sy + cr * cy;
	matrix[0][2] = cr * sp * cy + sr * sy;
	matrix[1][2] = cr * sp * sy - sr * cy;
	*/
	__m128 m1;
	__m128 m2 = _mm_shuffle_ps(s, c, 0x00); // [sp][sp][cp][cp]
	__m128 m3 = _mm_shuffle_ps(c, s, 0x55); // [cy][cy][sy][sy]

	m1 = _mm_shuffle_ps(s, c, 0xAA);   // [sr][sr][cr][cr]
	m2 = _mm_shuffle_ps(m2, m2, 0x00); // [sp][sp][sp][sp]
	m3 = _mm_shuffle_ps(m3, m3, 0xD8); // [cy][sy][cy][sy]

	m2 = _mm_mul_ps(m2, _mm_mul_ps(m1, m3)); // m1*m2*m3

	m1 = _mm_shuffle_ps(m1, m1, 0x1B); // [cr][cr][sr][sr]
	m3 = _mm_shuffle_ps(m3, m3, 0xB1); // [sy][cy][sy][cy]
	m3 = _mm_xor_ps(m3, _mm_load_ps((float *)&negmask_1001));
	m3 = _mm_mul_ps(m3, m1);

	m2 = _mm_add_ps(m2, m3);

	/*
	matrix[0][0] = cp * cy;
	matrix[1][0] = cp * sy;
	matrix[2][1] = sr * cp;
	matrix[2][2] = cr * cp;
	*/
	m1 = _mm_shuffle_ps(s, c, 0x29); // [sy][sr][cr][cp]
	c = _mm_shuffle_ps(c, c, 0x40);  // [cp][cp][cp][cy]
	m1 = _mm_mul_ps(m1, c);

	// matrix[0]
	m3 = _mm_shuffle_ps(m2, m2, 0xE1);
	_mm_storeu_ps(&matrix[0][0], m3);
	matrix[0][0] = _mm_cvtss_f32(_mm_shuffle_ps(m1, m1, 0x03));
	*(int *)&matrix[0][3] = 0;

	// matrix[1]
	m2 = _mm_shuffle_ps(m2, m2, 0xB4);
	_mm_storeu_ps(&matrix[1][0], m2);
	matrix[1][0] = _mm_cvtss_f32(m1);
	*(int *)&matrix[1][3] = 0;

	// matrix[2]
	_mm_storeu_ps(&matrix[2][0], m1);
	matrix[2][0] = -_mm_cvtss_f32(s);
	*(int *)&matrix[2][3] = 0;
}
#else // REHLDS_FIXES
/* <471e9> ../engine/mathlib.c:340 */
void AngleMatrix(const vec_t *angles, float(*matrix)[4])
{
	float		sr, sp, sy, cr, cp, cy;

	float angle;
	angle = (float)(angles[ROLL] * (M_PI * 2 / 360));
	sy = sin(angle);
	cy = cos(angle);
	angle = (float)(angles[YAW] * (M_PI * 2 / 360));
	sp = sin(angle);
	cp = cos(angle);
	angle = (float)(angles[PITCH] * (M_PI * 2 / 360));
	sr = sin(angle);
	cr = cos(angle);

	matrix[0][0] = cr * cp;
	matrix[0][1] = sy * sr * cp - cy * sp;
	matrix[0][2] = cy * sr * cp + sy * sp;
	matrix[0][3] = 0.0f;

	matrix[1][0] = cr * sp;
	matrix[1][1] = sy * sr * sp + cy * cp;
	matrix[1][2] = cy * sr * sp - sy * cp;
	matrix[1][3] = 0.0f;

	matrix[2][0] = -sr;
	matrix[2][1] = sy * cr;
	matrix[2][2] = cy * cr;
	matrix[2][3] = 0.0f;
}
#endif // REHLDS_FIXES

/* <4729e> ../engine/mathlib.c:370 */
NOBODY void AngleIMatrix(const vec_t *angles, float *matrix);
//{
//	float angle;                                                  //   372
//	float sr;                                                     //   373
//	float sp;                                                     //   373
//	float sy;                                                     //   373
//	float cr;                                                     //   373
//	float cp;                                                     //   373
//	float cy;                                                     //   373
//}

/* <4733d> ../engine/mathlib.c:400 */
NOBODY void NormalizeAngles(float *angles);
//{
//	int i;                                                        //   402
//}

/* <47389> ../engine/mathlib.c:426 */
NOBODY void InterpolateAngles(float *start, float *end, float *output, float frac);
//{
//	int i;                                                        //   428
//	float ang1;                                                   //   429
//	float ang2;                                                   //   429
//	float d;                                                      //   430
//	NormalizeAngles(float *angles); /* size=0, low_pc=0 */ //   432
//	NormalizeAngles(float *angles); /* size=0, low_pc=0 */ //   433
//	NormalizeAngles(float *angles); /* size=0, low_pc=0 */ //   453
//}

/* <47495> ../engine/mathlib.c:457 */
void VectorTransform(const vec_t *in1, float *in2, vec_t *out)
{
	out[0] = _DotProduct(in1, in2 + 0) + in2[3];
	out[1] = _DotProduct(in1, in2 + 4) + in2[7];
	out[2] = _DotProduct(in1, in2 + 8) + in2[11];
}

/* <474dc> ../engine/mathlib.c:465 */
int VectorCompare(const vec_t *v1, const vec_t *v2)
{
#ifdef REHLDS_OPT_PEDANTIC
	__m128 cmp = _mm_cmpneq_ps(_mm_loadu_ps(v1), _mm_loadu_ps(v2));
	return !(_mm_movemask_epi8(*(__m128i *)&cmp) & 0xFFF);
#else // REHLDS_OPT_PEDANTIC
	for (int i = 0; i < 3; i++)
	{
		if (v1[i] != v2[i]) return 0;
	}

	return 1;
#endif // REHLDS_OPT_PEDANTIC
}

#ifdef REHLDS_FIXES
void VectorMA(const vec_t *veca, float scale, const vec_t *vecm, vec_t *out)
{
	xmm2vec(out, _mm_add_ps(_mm_mul_ps(_mm_set_ps1(scale), _mm_loadu_ps(vecm)), _mm_loadu_ps(veca)));
}
#else
/* <47524> ../engine/mathlib.c:476 */
void VectorMA(const vec_t *veca, float scale, const vec_t *vecm, vec_t *out)
{
	out[0] = scale * vecm[0] + veca[0];
	out[1] = scale * vecm[1] + veca[1];
	out[2] = scale * vecm[2] + veca[2];
}
#endif

#ifndef REHLDS_FIXES
/* <4757a> ../engine/mathlib.c:484 */
long double _DotProduct(const vec_t *v1, const vec_t *v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
#else // REHLDS_FIXES
float _DotProduct(const vec_t *v1, const vec_t *v2)
{
	// _mm_loadu_ps - load xmm from unaligned address
	// _mm_cvtss_f32 - return low float value of xmm
	// _mm_dp_ps - dot product
	// 0x71 = 0b01110001 - mask for multiplying operands and result
	// dpps isn't binary compatible with separate sse2 instructions (max difference is about 0.0002f, but usually < 0.00001f)
	if (cpuinfo.sse4_1)
		return _mm_cvtss_f32(_mm_dp_ps(_mm_loadu_ps(v1), _mm_loadu_ps(v2), 0x71));

	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
#endif // REHLDS_FIXES

/* <475b4> ../engine/mathlib.c:489 */
NOBODY void _VectorSubtract(vec_t *veca, vec_t *vecb, vec_t *out);
//{
//}

/* <475fb> ../engine/mathlib.c:496 */
void _VectorAdd(vec_t *veca, vec_t *vecb, vec_t *out)
{
	out[0] = veca[0] + vecb[0];
	out[1] = veca[1] + vecb[1];
	out[2] = veca[2] + vecb[2];
}

/* <47642> ../engine/mathlib.c:503 */
NOBODY void _VectorCopy(vec_t *in, vec_t *out);
//{
//}

/* <47679> ../engine/mathlib.c:510 */
void CrossProduct(const vec_t *v1, const vec_t *v2, vec_t *cross)
{
#ifdef REHLDS_FIXES
	__m128 a = _mm_loadu_ps(v1), b = _mm_loadu_ps(v2);
	__m128 tmp1 = _mm_mul_ps(a, _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1)));
	__m128 tmp2 = _mm_mul_ps(b, _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)));
	__m128 m = _mm_sub_ps(tmp1, tmp2);

	xmm2vec(cross, _mm_shuffle_ps(m, m, _MM_SHUFFLE(3, 0, 2, 1)));
#else // REHLDS_FIXES
	cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
	cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
	cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
#endif // REHLDS_FIXES
}

/* <476d8> ../engine/mathlib.c:519 */
float Length(const vec_t *v)
{
#ifdef REHLDS_FIXES
	// based on dot product
	if (cpuinfo.sse4_1)
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(_mm_loadu_ps(v), _mm_loadu_ps(v), 0x71)));
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
#endif // REHLDS_FIXES

	float length;

	length = 0.0f;
	for (int i = 0; i < 3; i++)
	{
		length = v[i] * v[i] + length;
	}
	return sqrt(length);
}

float Length2D(const vec_t *v)
{
#ifdef REHLDS_FIXES
	// based on dot product
	if (cpuinfo.sse4_1)
		return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(_mm_loadu_ps(v), _mm_loadu_ps(v), 0x31))); // 0b00110001
#endif // REHLDS_FIXES

	return sqrt(v[0] * v[0] + v[1] * v[1]);
}

/* <47722> ../engine/mathlib.c:532 */
float VectorNormalize(vec3_t v)
{
	float	length, ilength;

#ifdef REHLDS_FIXES
	length = Length(v); // rsqrt is very inaccurate :(
#else // REHLDS_FIXES
	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	length = sqrt(length);
#endif // REHLDS_FIXES

	if (length)
	{
		ilength = 1 / length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return length;
}

/* <47788> ../engine/mathlib.c:551 */
NOBODY void VectorInverse(vec_t *v);
//{
//}

/* <477af> ../engine/mathlib.c:558 */
void VectorScale(const vec_t *in, float scale, vec_t *out)
{
	out[0] = scale * in[0];
	out[1] = scale * in[1];
	out[2] = scale * in[2];
}

/* <477f5> ../engine/mathlib.c:566 */
NOBODY int Q_log2(int val);
//{
//	int answer;                                                   //   568
//}

/* <47833> ../engine/mathlib.c:574 */
NOBODY void VectorMatrix(vec_t *forward, vec_t *right, vec_t *up);
//{
//	vec3_t tmp;                                                   //   576
//	CrossProduct(const vec_t *v1,
//		const vec_t *v2,
//		vec_t *cross); /* size=0, low_pc=0 */ //   590
//	VectorNormalize(vec_t *v); /* size=0, low_pc=0 */ //   591
//	CrossProduct(const vec_t *v1,
//		const vec_t *v2,
//		vec_t *cross); /* size=0, low_pc=0 */ //   592
//	VectorNormalize(vec_t *v); /* size=0, low_pc=0 */ //   593
//}

/* <4794e> ../engine/mathlib.c:597 */
void VectorAngles(const vec_t *forward, vec_t *angles)
{
	float	length, yaw, pitch;

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
		yaw = (atan2((double)forward[1], (double)forward[0]) * 180.0 / M_PI);
		if (yaw < 0)
			yaw += 360;

#ifdef REHLDS_FIXES
		length = Length2D(forward);
#else // REHLDS_FIXES
		length = sqrt((double)(forward[0] * forward[0] + forward[1] * forward[1]));
#endif // REHLDS_FIXES

		pitch = atan2((double)forward[2], (double)length) * 180.0 / M_PI;
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

/* <479b7> ../engine/mathlib.c:632 */
NOBODY void R_ConcatRotations(float *in1, float *in2, float *out);
//{
//
//}

/* <47a04> ../engine/mathlib.c:660 */
#ifdef REHLDS_FIXES
void R_ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4])
{
	for (size_t i = 0; i < 3; i++)
	{
		__m128 a1 = _mm_mul_ps(_mm_set_ps1(in1[i][0]), _mm_loadu_ps(in2[0]));
		__m128 a2 = _mm_mul_ps(_mm_set_ps1(in1[i][1]), _mm_loadu_ps(in2[1]));
		__m128 a3 = _mm_mul_ps(_mm_set_ps1(in1[i][2]), _mm_loadu_ps(in2[2]));
		_mm_storeu_ps(out[i], _mm_add_ps(a1, _mm_add_ps(a2, a3)));
		out[i][3] += in1[i][3];
	}
}
#else // REHLDS_FIXES
void R_ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
}
#endif // REHLDS_FIXES

/* <47a4b> ../engine/mathlib.c:699 */
NOBODY void FloorDivMod(double numer, double denom, int *quotient, int *rem);
//{
//	int q;                                                        //   702
//	int r;                                                        //   702
//	double x;                                                     //   703
//	floor(double __x); /* size=0, low_pc=0 */ //   726
//	floor(double __x); /* size=0, low_pc=0 */ //   717
//}

/* <47b4c> ../engine/mathlib.c:746 */
NOBODY int GreatestCommonDivisor(int i1, int i2);
//{
//}

/* <47b87> ../engine/mathlib.c:775 */
NOBODY fixed16_t Invert24To16(fixed16_t val);
//{
//}
