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

// ZONE MEMORY ALLOCATION
//
// There is never any space between memblocks, and there will never be two
// contiguous free memblocks.
//
// The rover can be left pointing at a non-empty block
//
// The zone calls are pretty much only used for small strings and structures,
// all big things are allocated on the hunk.

#define ZONEID 0x001d4a11
const int MINFRAGMENT = 64;

typedef struct memblock_s
{
	int size;
	int tag;
	int id;
	memblock_t *next;
	memblock_t *prev;
	int pad;
} memblock_t;

typedef struct memzone_s
{
	int size;
	memblock_t blocklist;
	memblock_t *rover;
} memzone_t;

cvar_t mem_dbgfile = { "mem_dbgfile", ".\\mem.txt", 0, 0.0f, NULL };

memzone_t *mainzone;

void Z_ClearZone(memzone_t *zone, int size)
{
	memblock_t *block = (memblock_t *)&zone[1];

	zone->blocklist.prev = zone->blocklist.next = zone->rover = block;
	zone->blocklist.size = zone->blocklist.id = 0;
	zone->blocklist.tag = 1;

	block->prev = block->next = &zone->blocklist;
	block->tag = 0;
	block->id = ZONEID;
	block->size = size - sizeof(memzone_t);
}

void Z_Free(void *ptr)
{
	if (!ptr)
	{
		Sys_Error("%s: NULL pointer", __func__);
	}

	memblock_t *block = (memblock_t *)((char *)ptr - sizeof(memblock_t));

	if (block->id != ZONEID)
	{
		Sys_Error("%s: freed a pointer without ZONEID", __func__);
	}

	if (!block->tag)
	{
		Sys_Error("%s: freed a freed pointer", __func__);
	}

	block->tag = 0;

	memblock_t *otherblock = block->prev;

	if (!otherblock->tag)
	{
		otherblock->size += block->size;
		otherblock->next = block->next;
		block->next->prev = otherblock;

		if (block == mainzone->rover)
		{
			mainzone->rover = otherblock;
		}

		block = otherblock;
	}

	otherblock = block->next;

	if (!otherblock->tag)
	{
		block->size += otherblock->size;
		block->next = otherblock->next;
		otherblock->next->prev = block;

		if (otherblock == mainzone->rover)
		{
			mainzone->rover = block;
		}
	}
}

void *Z_Malloc(int size)
{
	Z_CheckHeap();

	void *buf = Z_TagMalloc(size, 1);

	if (!buf)
	{
		Sys_Error("%s: failed on allocation of %i bytes", __func__, size);
	}

	Q_memset(buf, 0, size);
	return buf;
}

void *Z_TagMalloc(int size, int tag)
{
	int extra;
	memblock_t *start, *rover, *newz, *base;

	if (tag == 0)
	{
		Sys_Error("%s: tried to use a 0 tag", __func__);
	}

	size += sizeof(memblock_t);
	size += 4;
	size = (size + 7) & ~7;

	base = rover = mainzone->rover;
	start = base->prev;

	do
	{
		if (rover == start)
		{
			return(NULL);
		}

		if (rover->tag)
		{
			base = rover = rover->next;
		}
		else
		{
			rover = rover->next;
		}
	} while (base->tag || base->size < size);

	extra = base->size - size;

	if (extra > MINFRAGMENT)
	{
		newz = (memblock_t *)((byte *)base + size);
		newz->size = extra;
		newz->tag = 0;
		newz->prev = base;
		newz->id = ZONEID;
		newz->next = base->next;
		newz->next->prev = newz;
		base->next = newz;
		base->size = size;
	}

	base->tag = tag;
	mainzone->rover = base->next;
	base->id = ZONEID;

	// marker for memory trash testing
	*(int *)((byte *)base + base->size - 4) = ZONEID;

	return (void *)((byte *)base + sizeof(memblock_t));
}

NOXREF void Z_Print(memzone_t *zone)
{
	NOXREFCHECK;

	memblock_t *block;
	Con_Printf("zone size: %i  location: %p\n", mainzone->size, mainzone);

	for (block = zone->blocklist.next; ; block = block->next)
	{
		Con_Printf("block:%p    size:%7i    tag:%3i\n", block, block->size, block->tag);

		if (block->next == &zone->blocklist)
		{
			break; // all blocks have been hit
		}

		if ((byte *)block + block->size != (byte *)block->next)
		{
			Con_Printf("ERROR: block size does not touch the next block\n");
		}

		if (block->next->prev != block)
		{
			Con_Printf("ERROR: next block doesn't have proper back link\n");
		}

		if (!block->tag && !block->next->tag)
		{
			Con_Printf("ERROR: two consecutive free blocks\n");
		}
	}
}

void Z_CheckHeap()
{
	memblock_t *block;

	for (block = mainzone->blocklist.next; ; block = block->next)
	{
		if (block->next == &mainzone->blocklist)
		{
			break;
		}

		if ((byte *)block + block->size != (byte *)block->next)
		{
			Sys_Error("%s: block size does not touch the next block\n", __func__);
		}

		if (block->next->prev != block)
		{
			Sys_Error("%s: next block doesn't have proper back link\n", __func__);
		}

		if (!block->tag && !block->next->tag)
		{
			Sys_Error("%s: two consecutive free blocks\n", __func__);
		}
	}
}

const int HUNK_NAME_LEN = 64;
#define HUNK_SENTINEL 0x1df001ed

typedef struct hunk_s
{
	int sentinel;
	int size;
	char name[HUNK_NAME_LEN];
} hunk_t;

byte *hunk_base;
int hunk_size;

int hunk_low_used;
int hunk_high_used;

qboolean hunk_tempactive;
int hunk_tempmark;

// Run consistency and sentinel trashing checks
void Hunk_Check()
{
	hunk_t *h;

	for (h = (hunk_t *)hunk_base; (byte *)h != (hunk_base + hunk_low_used); h = (hunk_t *)((byte *)h + h->size))
	{
		if (h->sentinel != HUNK_SENTINEL)
		{
			Sys_Error("%s: trahsed sentinel", __func__);
		}

		if (h->size < 16 || h->size + (byte *)h - hunk_base > hunk_size)
		{
			Sys_Error("%s: bad size", __func__);
		}
	}
}

// If "all" is specified, every single allocation is printed.
// Otherwise, allocations with the same name will be totaled up before printing.
NOXREF void Hunk_Print(qboolean all)
{
	NOXREFCHECK;

	hunk_t	*h, *next, *endlow, *starthigh, *endhigh;
	int		count, sum;
	int		totalblocks;
	char	name[HUNK_NAME_LEN];

	name[HUNK_NAME_LEN - 1] = 0;
	count = 0;
	sum = 0;
	totalblocks = 0;

	h = (hunk_t *)hunk_base;
	endlow = (hunk_t *)(hunk_base + hunk_low_used);
	starthigh = (hunk_t *)(hunk_base + hunk_size - hunk_high_used);
	endhigh = (hunk_t *)(hunk_base + hunk_size);

	Con_Printf("          :%8i total hunk size\n", hunk_size);
	Con_Printf("-------------------------\n");

	while (true)
	{
		// skip to the high hunk if done with low hunk
		if (h == endlow)
		{
			Con_Printf("-------------------------\n");
			Con_Printf("          :%8i REMAINING\n", hunk_size - hunk_low_used - hunk_high_used);
			Con_Printf("-------------------------\n");
			h = starthigh;
		}

		// if totally done, break
		if (h == endhigh)
			break;

		// run consistancy checks
		if (h->sentinel != HUNK_SENTINEL)
			Sys_Error("%s: trahsed sentinal", __func__);
		if (h->size < 16 || h->size + (byte *)h - hunk_base > hunk_size)
			Sys_Error("%s: bad size", __func__);

		next = (hunk_t *)((byte *)h + h->size);
		count++;
		totalblocks++;
		sum += h->size;

		// print the single block
		Q_memcpy(name, h->name, HUNK_NAME_LEN);
		if (all)
			Con_Printf("%8p :%8i %8s\n", h, h->size, name);

		// print the total
		if (next == endlow || next == endhigh ||
			Q_strncmp(h->name, next->name, HUNK_NAME_LEN))
		{
			if (!all)
				Con_Printf("          :%8i %8s (TOTAL)\n", sum, name);
			count = 0;
			sum = 0;
		}

		h = next;
	}

	Con_Printf("-------------------------\n");
	Con_Printf("%8i total blocks\n", totalblocks);
}

void *Hunk_AllocName(int size, const char *name)
{
	if (size < 0)
	{
		Sys_Error("%s: bad size: %i", __func__, size);
	}

	int totalsize = ((size + 15) & ~15) + sizeof(hunk_t);

	if (hunk_size - hunk_high_used - hunk_low_used < totalsize)
	{
		Sys_Error("%s: failed on %i bytes", __func__, totalsize);
	}

	hunk_t *h = (hunk_t *)(hunk_base + hunk_low_used);

	hunk_low_used += totalsize;
	Cache_FreeLow(hunk_low_used);

	Q_memset(h, 0, totalsize);
	h->size = totalsize;
	h->sentinel = HUNK_SENTINEL;
	Q_strncpy(h->name, name, HUNK_NAME_LEN - 1);
	h->name[HUNK_NAME_LEN - 1] = 0;

	return (void *)(h + 1);
}

void *Hunk_Alloc(int size)
{
	return Hunk_AllocName(size, "unknown");
}

int Hunk_LowMark()
{
	return hunk_low_used;
}

void Hunk_FreeToLowMark(int mark)
{
	if (mark < 0 || mark > hunk_low_used)
	{
		Sys_Error("%s: bad mark %i", __func__, mark);
	}

	hunk_low_used = mark;
}

int Hunk_HighMark()
{
	if (hunk_tempactive)
	{
		hunk_tempactive = FALSE;
		Hunk_FreeToHighMark(hunk_tempmark);
	}

	return hunk_high_used;
}

void Hunk_FreeToHighMark(int mark)
{
	if (hunk_tempactive)
	{
		hunk_tempactive = FALSE;
		Hunk_FreeToHighMark(hunk_tempmark);
	}

	if (mark < 0 || mark > hunk_high_used)
	{
		Sys_Error("%s: bad mark %i", __func__, mark);
	}

	hunk_high_used = mark;
}

void *Hunk_HighAllocName(int size, const char *name)
{
	hunk_t *h;
	if (size < 0)
	{
		Sys_Error("%s: bad size: %i", __func__, size);
	}

	if (hunk_tempactive)
	{
		Hunk_FreeToHighMark(hunk_tempmark);
		hunk_tempactive = FALSE;
	}

	size = ((size + 15) & ~15) + sizeof(hunk_t);

	if (hunk_size - hunk_high_used - hunk_low_used < size)
	{
		Con_Printf("%s: failed on %i bytes\n", __func__, size);
		return 0;
	}

	hunk_high_used += size;
	Cache_FreeHigh(hunk_high_used);

	h = (hunk_t *)(hunk_base + hunk_size - hunk_high_used);
	Q_memset(h, 0, size);

	h->size = size;
	h->sentinel = HUNK_SENTINEL;
	Q_strncpy(h->name, name, HUNK_NAME_LEN - 1);
	h->name[HUNK_NAME_LEN - 1] = 0;

	return (void *)(h + 1);
}

// Return space from the top of the hunk
void *Hunk_TempAlloc(int size)
{
	void *buf;

	if (hunk_tempactive)
	{
		Hunk_FreeToHighMark(hunk_tempmark);
		hunk_tempactive = 0;
	}

	hunk_tempmark = Hunk_HighMark();
	buf = Hunk_HighAllocName((size + 15) & ~15, "temp");
	hunk_tempactive = 1;

	return buf;
}

// CACHE MEMORY
const int CACHE_NAME_LEN = 64;

typedef struct cache_system_s
{
	int size;
	cache_user_t *user;
	char name[CACHE_NAME_LEN];
	cache_system_t *prev;
	cache_system_t *next;
	cache_system_t *lru_prev;
	cache_system_t *lru_next;
} cache_system_t;

cache_system_t cache_head;

void Cache_Move(cache_system_t *c)
{
	cache_system_t *newmem = Cache_TryAlloc(c->size, 1);

	if (!newmem)
	{
		Cache_Free(c->user);
	}
	else
	{
		Q_memcpy(newmem + 1, c + 1, c->size - sizeof(cache_system_t));
		newmem->user = c->user;
		Q_memcpy(newmem->name, c->name, sizeof(newmem->name));
		Cache_Free(c->user);
		newmem->user->data = (void *)(newmem + 1);
	}
}

// Throw things out until the hunk can be expanded to the given point
void Cache_FreeLow(int new_low_hunk)
{
	cache_system_t	*c;

	while (true)
	{
		c = cache_head.next;
		if (c == &cache_head)
			return;		// nothing in cache at all
		if ((byte *)c >= hunk_base + new_low_hunk)
			return;		// there is space to grow the hunk
		Cache_Move(c);	// reclaim the space
	}
}

// Throw things out until the hunk can be expanded to the given point
void Cache_FreeHigh(int new_high_hunk)
{
	cache_system_t	*c, *prev;

	prev = NULL;
	while (true)
	{
		c = cache_head.prev;
		if (c == &cache_head)
			return;		// nothing in cache at all
		if ((byte *)c + c->size <= hunk_base + hunk_size - new_high_hunk)
			return;		// there is space to grow the hunk
		if (c == prev)
			Cache_Free(c->user);	// didn't move out of the way
		else
		{
			Cache_Move(c);	// try to move it
			prev = c;
		}
	}
}

void Cache_UnlinkLRU(cache_system_t *cs)
{
	if (!cs->lru_next || !cs->lru_prev)
	{
		Sys_Error("%s: NULL link", __func__);
	}

	cs->lru_next->lru_prev = cs->lru_prev;
	cs->lru_prev->lru_next = cs->lru_next;
	cs->lru_next = cs->lru_prev = 0;
}

void Cache_MakeLRU(cache_system_t *cs)
{
	if (cs->lru_next || cs->lru_prev)
	{
		Sys_Error("%s: active link", __func__);
	}

	cache_head.lru_next->lru_prev = cs;
	cs->lru_next = cache_head.lru_next;
	cs->lru_prev = &cache_head;
	cache_head.lru_next = cs;
}

// Looks for a free block of memory between the high and low hunk marks
// Size should already include the header and padding
cache_system_t *Cache_TryAlloc(int size, qboolean nobottom)
{
	cache_system_t *cs;
	cache_system_t *newmem;

	if (!nobottom && cache_head.prev == &cache_head)
	{
		if (hunk_size - hunk_low_used - hunk_high_used < size)
		{
			Sys_Error("%s: %i is greater then free hunk", __func__, size);
		}

		newmem = (cache_system_t *)(hunk_base + hunk_low_used);
		Q_memset(newmem, 0, sizeof(cache_system_t));
		newmem->size = size;
		cache_head.next = cache_head.prev = newmem;
		newmem->next = newmem->prev = &cache_head;

		Cache_MakeLRU(newmem);
		return newmem;
	}

	cs = cache_head.next;
	newmem = (cache_system_t *)(hunk_base + hunk_low_used);

	do
	{
		if ((!nobottom || cs != cache_head.next) && (signed int)((char *)cs - (char *)newmem) >= size)
		{
			Q_memset(newmem, 0, sizeof(cache_system_t));

			newmem->size = size;
			newmem->next = cs;
			newmem->prev = cs->prev;

			cs->prev->next = newmem;
			cs->prev = newmem;

			Cache_MakeLRU(newmem);
			return newmem;
		}

		newmem = (cache_system_t *)((char *)cs + cs->size);
		cs = cs->next;
	} while (cs != &cache_head);

	if ((int)(hunk_size + hunk_base - hunk_high_used - (byte *)newmem) < size)
	{
		return 0;
	}

	Q_memset(newmem, 0, sizeof(cache_system_t));

	newmem->size = size;
	newmem->next = &cache_head;
	newmem->prev = cache_head.prev;

	cache_head.prev->next = newmem;
	cache_head.prev = newmem;

	Cache_MakeLRU(newmem);
	return newmem;
}

// Throw everything out, so new data will be demand cached
void Cache_Force_Flush()
{
	cache_system_t *i;

	for (i = cache_head.next; cache_head.next != &cache_head; i = cache_head.next)
	{
		Cache_Free(i->user);
	}
}

void Cache_Flush()
{
	if (g_pcl.maxclients <= 1 || allow_cheats)
	{
		Cache_Force_Flush();
	}
	else
	{
		Con_Printf("Server must enable sv_cheats to activate the flush command in multiplayer games.\n");
	}
}

// Compares the names of two cache_system_t structs.
// Used with qsort()
NOXREF int CacheSystemCompare(const void *ppcs1, const void *ppcs2)
{
	NOXREFCHECK;

	cache_system_t *pcs1 = (cache_system_t *)ppcs1;
	cache_system_t *pcs2 = (cache_system_t *)ppcs2;

	return Q_stricmp(pcs1->name, pcs2->name);
}

NOXREF void Cache_Print()
{
	NOXREFCHECK;

	cache_system_t *cd;

	for (cd = cache_head.next; cd != &cache_head; cd = cd->next)
	{
		Con_Printf("%8i : %s\n", cd->size, cd->name);
	}
}

// compares the first directory of two paths...
// (so  "foo/bar" will match "foo/fred"
NOXREF int ComparePath1(char *path1, char *path2)
{
	NOXREFCHECK;
	while (*path1 != '/' && *path1 != '\\' && *path1)
	{
		if (*path1 != *path2)
			return 0;
		else
		{
			path1++;
			path2++;
		}
	}
	return 1;
}

// Takes a number, and creates a string of that with commas in the appropriate places.
NOXREF char *CommatizeNumber(int num, char *pout)
{
	NOXREFCHECK;

	// this is probably more complex than it needs to be.
	int len = 0;
	int i;
	char outbuf[50];
	Q_memset(outbuf, 0, 50);
	while (num)
	{
		char tempbuf[50];
		int temp = num % 1000;
		num = num / 1000;
		Q_strcpy(tempbuf, outbuf);

		Q_snprintf(outbuf, sizeof(outbuf), ",%03i%s", temp, tempbuf);
	}

	len = Q_strlen(outbuf);

	for (i = 0; i < len; i++)				// find first significant digit
		if (outbuf[i] != '0' && outbuf[i] != ',')
			break;

	if (i == len)
		Q_strcpy(pout, "0");
	else
		Q_strcpy(pout, &outbuf[i]);	// copy from i to get rid of the first comma and leading zeros

	return pout;
}

NOXREF void Cache_Report()
{
	NOXREFCHECK;
	Con_DPrintf("%4.1f megabyte data cache\n", (hunk_size - hunk_low_used - hunk_high_used) / (float)(1024 * 1024));
}

NOXREF void Cache_Compact()
{
	NOXREFCHECK;
}

void Cache_Init()
{
	cache_head.next = cache_head.prev = &cache_head;
	cache_head.lru_next = cache_head.lru_prev = &cache_head;

	Cmd_AddCommand("flush", Cache_Flush);
}

// Frees the memory and removes it from the LRU list
void Cache_Free(cache_user_t *c)
{
	if (!c->data)
	{
		Sys_Error("%s: not allocated", __func__);
	}

	cache_system_t *cs = ((cache_system_t *)c->data - 1);

	cs->prev->next = cs->next;
	cs->next->prev = cs->prev;
	c->data = cs->prev = cs->next = 0;

	Cache_UnlinkLRU(cs);
}

NOXREF int Cache_TotalUsed()
{
	NOXREFCHECK;

	cache_system_t	*cd;
	int Total = 0;
	for (cd = cache_head.next; cd != &cache_head; cd = cd->next)
		Total += cd->size;

	return Total;
}

void* EXT_FUNC Cache_Check(cache_user_t *c)
{
	cache_system_t *cs;

	if (c->data)
	{
		cs = (cache_system_t *)((char *)c->data - sizeof(cache_system_t));

		Cache_UnlinkLRU((cache_system_t *)c->data - 1);
		Cache_MakeLRU(cs);
	}

	return c->data;
}

void *Cache_Alloc(cache_user_t *c, int size, char *name)
{
	cache_system_t *cs;

	if (c->data)
	{
		Sys_Error("%s: already allocated", __func__);
	}

	if (size <= 0)
	{
		Sys_Error("%s: size %i", __func__, size);
	}

	while (true)
	{
		cs = Cache_TryAlloc((size + sizeof(cache_system_t) + 15) & ~15, 0);

		if (cs)
		{
			Q_strncpy(cs->name, name, CACHE_NAME_LEN - 1);
			cs->name[CACHE_NAME_LEN - 1] = 0;
			c->data = cs + 1;
			cs->user = c;

			break;
		}

		if (cache_head.lru_prev == &cache_head)
		{
			Sys_Error("%s: out of memory", __func__);
		}

		Cache_Free(cache_head.lru_prev->user);
	}

	return Cache_Check(c);
}

void Memory_Init(void *buf, int size)
{
	int zonesize = ZONE_DYNAMIC_SIZE;

	hunk_base = (byte *)buf;
	hunk_size = size;
	hunk_low_used = 0;
	hunk_high_used = 0;

	Cache_Init();

	int p = COM_CheckParm("-zone");

	if (p)
	{
		if (p < com_argc - 1)
		{
			zonesize = Q_atoi(com_argv[p + 1]) * 1024;
		}
		else
		{
			Sys_Error("%s: you must specify a size in KB after -zone", __func__);
		}
	}

	mainzone = ((memzone_t *)Hunk_AllocName(zonesize, "zone"));
	Z_ClearZone(mainzone, zonesize);
}

NOXREF void Cache_Print_Models_And_Totals()
{
	NOXREFCHECK;
	char buf[50];
	cache_system_t *cd;
	cache_system_t *sortarray[512];
	int32 i = 0;
	int32 j = 0;
	int32 totalbytes = 0;
	FileHandle_t file;

	file = FS_Open(mem_dbgfile.string, "a");
	if (!file)
		return;

	Q_memset(sortarray, 0, sizeof(cache_system_t *) * 512);

	// pack names into the array.
	for (cd = cache_head.next; cd != &cache_head; cd = cd->next)
	{
		if (Q_strstr(cd->name,".mdl"))
			sortarray[i++] = cd;
	}

	qsort(sortarray, i, sizeof(cache_system_t *), CacheSystemCompare);
	FS_FPrintf(file, "\nCACHED MODELS:\n");

	// now process the sorted list.
	for (j = 0; j < i; j++)
	{
		FS_FPrintf(file, "\t%16.16s : %s\n", CommatizeNumber(sortarray[j]->size, buf), sortarray[j]->name);
		totalbytes += sortarray[j]->size;
	}

	FS_FPrintf(file,"Total bytes in cache used by models:  %s\n", CommatizeNumber(totalbytes, buf));
	FS_Close(file);
}

const int MAX_SFX = 1024;

NOXREF void Cache_Print_Sounds_And_Totals()
{
	NOXREFCHECK;
	char buf[50];
	cache_system_t *cd;
	cache_system_t *sortarray[1024];
	int32 i = 0;
	int32 j = 0;
	int32 totalsndbytes = 0;
	FileHandle_t file;
	int subtot = 0;

	file = FS_Open(mem_dbgfile.string, "a");
	if (!file)
		return;

	Q_memset(sortarray, 0, sizeof(cache_system_t *) * MAX_SFX);

	// pack names into the array.
	for (cd = cache_head.next; cd != &cache_head; cd = cd->next)
	{
		if (Q_strstr(cd->name,".wav"))
			sortarray[i++] = cd;
	}

	qsort(sortarray, i, sizeof(cache_system_t *), CacheSystemCompare);
	FS_FPrintf(file, "\nCACHED SOUNDS:\n");

	// now process the sorted list.  (totals by directory)
	for (j = 0; j < i; j++)
	{
		FS_FPrintf(file, "\t%16.16s : %s\n", CommatizeNumber(sortarray[j]->size, buf), sortarray[j]->name);
		totalsndbytes += sortarray[j]->size;

		if ((j + 1) == i || ComparePath1(sortarray[j]->name, sortarray[j + 1]->name) == 0)
		{
			char pathbuf[512];
			Sys_SplitPath(sortarray[j]->name, NULL, pathbuf, NULL, NULL);
			FS_FPrintf(file, "\tTotal Bytes used in \"%s\": %s\n", pathbuf, CommatizeNumber(totalsndbytes - subtot, buf));
			subtot = totalsndbytes;
		}
	}
	FS_FPrintf(file, "Total bytes in cache used by sound:  %s\n", CommatizeNumber(totalsndbytes, buf));
	FS_Close(file);
}
