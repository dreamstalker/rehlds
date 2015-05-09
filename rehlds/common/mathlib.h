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

#pragma once


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

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
