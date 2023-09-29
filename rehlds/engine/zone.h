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

#define ZONE_DYNAMIC_SIZE 0x200000

typedef struct memblock_s memblock_t;
typedef struct memzone_s memzone_t;
typedef struct hunk_s hunk_t;
typedef struct cache_user_s cache_user_t;
typedef struct cache_system_s cache_system_t;

extern cvar_t mem_dbgfile;
extern memzone_t *mainzone;
extern byte *hunk_base;
extern int hunk_size;
extern int hunk_low_used;
extern int hunk_high_used;
extern qboolean hunk_tempactive;
extern int hunk_tempmark;
extern cache_system_t cache_head;

void Z_ClearZone(memzone_t *zone, int size);
void Z_Free(void *ptr);
void *Z_Malloc(int size);
void *Z_TagMalloc(int size, int tag);
NOXREF void Z_Print(memzone_t *zone);
void Z_CheckHeap();

void Hunk_Check();
NOXREF void Hunk_Print(qboolean all);
void *Hunk_AllocName(int size, const char *name);
void *Hunk_Alloc(int size);
int Hunk_LowMark();
void Hunk_FreeToLowMark(int mark);
int Hunk_HighMark();
void Hunk_FreeToHighMark(int mark);
void *Hunk_HighAllocName(int size, const char *name);
void *Hunk_TempAlloc(int size);

void Cache_Move(cache_system_t *c);
void Cache_FreeLow(int new_low_hunk);
void Cache_FreeHigh(int new_high_hunk);
void Cache_UnlinkLRU(cache_system_t *cs);
void Cache_MakeLRU(cache_system_t *cs);
cache_system_t *Cache_TryAlloc(int size, qboolean nobottom);
void Cache_Force_Flush();
void Cache_Flush();
NOXREF int CacheSystemCompare(const void *ppcs1, const void *ppcs2);
NOXREF void Cache_Print();
NOXREF int ComparePath1(char *path1, char *path2);
NOXREF char *CommatizeNumber(int num, char *pout);
NOXREF void Cache_Report();
NOXREF void Cache_Compact();
void Cache_Init();
void Cache_Free(cache_user_t *c);
NOXREF int Cache_TotalUsed();
void *Cache_Check(cache_user_t *c);
void *Cache_Alloc(cache_user_t *c, int size, char *name);
void Memory_Init(void *buf, int size);
NOXREF void Cache_Print_Models_And_Totals();
NOXREF void Cache_Print_Sounds_And_Totals();
