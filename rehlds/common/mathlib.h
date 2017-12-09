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

#ifndef MATHLIB_H
#define MATHLIB_H
#ifdef _WIN32
#pragma once
#endif

typedef float vec_t;

#if !defined DID_VEC3_T_DEFINE && !defined vec3_t
#define DID_VEC3_T_DEFINE
typedef vec_t vec3_t[3];
#endif

typedef vec_t vec4_t[4];
typedef int fixed16_t;

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
inline T min(T a, T b)
{
	return (a < b) ? a : b;
}

template <typename T>
inline T max(T a, T b)
{
	return (a < b) ? b : a;
}

template <typename T>
inline T clamp(T a, T min, T max)
{
	return (a > max) ? max : (a < min) ? min : a;
}

template<typename T>
inline T M_min(T a, T b)
{
	return min(a, b);
}

template<typename T>
inline T M_max(T a, T b)
{
	return max(a, b);
}

template<typename T>
inline T M_clamp(T a, T min, T max)
{
	return clamp(a, min, max);
}

template<typename T>
inline double M_sqrt(T value)
{
	return sqrt(value);
}

template <typename T>
inline T bswap(T s)
{
	switch (sizeof(T))
	{
	case 2: {auto res = __builtin_bswap16(*(uint16 *)&s); return *(T *)&res; }
	case 4: {auto res = __builtin_bswap32(*(uint32 *)&s); return *(T *)&res; }
	case 8: {auto res = __builtin_bswap64(*(uint64 *)&s); return *(T *)&res; }
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
