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

#include <quakedef.h>

// MD5 Hash
typedef struct
{
	unsigned int buf[4];
	unsigned int bits[2];
	unsigned char in[64];
} MD5Context_t;

void MD5Init(MD5Context_t *ctx);
void MD5Update(MD5Context_t *ctx, const unsigned char *buf, unsigned int len);
void MD5Final(unsigned char digest[16], MD5Context_t *ctx);
void MD5Transform(unsigned int buf[4], const unsigned int in[16]);

BOOL MD5_Hash_File(unsigned char digest[16], char *pszFileName, BOOL bUsefopen, BOOL bSeed, unsigned int seed[4]);
char *MD5_Print(unsigned char hash[16]);
