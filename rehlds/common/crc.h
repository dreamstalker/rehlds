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
/* crc.h */
#pragma once

#include "quakedef.h"

// MD5 Hash
typedef struct
{
	unsigned int buf[4];
	unsigned int bits[2];
	unsigned char in[64];
} MD5Context_t;

typedef unsigned int CRC32_t;


#ifdef __cplusplus
extern "C"
{
#endif

void CRC32_Init(CRC32_t *pulCRC);
CRC32_t CRC32_Final(CRC32_t pulCRC);
void CRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch);
void CRC32_ProcessBuffer(CRC32_t *pulCRC, void *pBuffer, int nBuffer);
BOOL CRC_File(CRC32_t *crcvalue, char *pszFileName);

#ifdef __cplusplus
}
#endif

byte COM_BlockSequenceCRCByte(byte *base, int length, int sequence);
int CRC_MapFile(CRC32_t *crcvalue, char *pszFileName);

void MD5Init(MD5Context_t *ctx);
void MD5Update(MD5Context_t *ctx, const unsigned char *buf, unsigned int len);
void MD5Final(unsigned char digest[16], MD5Context_t *ctx);
void MD5Transform(unsigned int buf[4], const unsigned int in[16]);

BOOL MD5_Hash_File(unsigned char digest[16], char *pszFileName, BOOL bUsefopen, BOOL bSeed, unsigned int seed[4]);
char *MD5_Print(unsigned char hash[16]);
