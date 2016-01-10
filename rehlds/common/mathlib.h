/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#ifndef MATHLIB_H
#define MATHLIB_H

/* <42b7f> ../common/mathlib.h:3 */
typedef float vec_t;

/* <42b91> ../common/mathlib.h:6 */
#ifndef DID_VEC3_T_DEFINE
#define DID_VEC3_T_DEFINE
typedef vec_t vec3_t[3];
#endif

/* <80013> ../common/mathlib.h:8 */
typedef vec_t vec4_t[4];

/* <42bac> ../common/mathlib.h:18 */
typedef int fixed16_t; /* size: 4 */

/* <42bb7> ../common/mathlib.h:60 */
typedef union DLONG_u
{
	int i[2];
	double d;
	float f;
} DLONG;

#define M_PI			3.14159265358979323846

#ifdef __cplusplus
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#ifdef clamp
#undef clamp
#endif

template <typename T>
inline T min(T a, T b) {
	return (a < b) ? a : b;
}

template <typename T>
inline T max(T a, T b) {
	return (a < b) ? b : a;
}

template <typename T>
inline T clamp(T a, T min, T max) {
	return (a > max) ? max : (a < min) ? min : a;
}

template <typename T>
inline T bswap(T s) {
	switch (sizeof(T)) {
#ifdef _WIN32
	case 2: {auto res = _byteswap_ushort(*(uint16 *)&s); return *(T *)&res;}
	case 4:	{auto res = _byteswap_ulong(*(uint32 *)(&s)); return *(T *)&res;}
	case 8: {auto res = _byteswap_uint64(*(uint64 *)&s); return *(T *)&res;}
#else
	case 2: {auto res = _bswap16(*(uint16 *)&s); return *(T *)&res;}
	case 4: {auto res = _bswap(*(uint32 *)&s); return *(T *)&res;}
	case 8: {auto res = _bswap64(*(uint64 *)&s); return *(T *)&res;}
#endif
	default: return s;
	}
}
#else // __cplusplus
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
#endif // __cplusplus

#endif // MATHLIB_H
