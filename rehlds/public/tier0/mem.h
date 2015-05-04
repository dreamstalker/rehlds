//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: Memory allocation!
//
// $NoKeywords: $
//=============================================================================

#ifndef TIER0_MEM_H
#define TIER0_MEM_H

#ifdef _WIN32
#pragma once
#endif

#include "osconfig.h"
#include <stddef.h>
#include "tier0/platform.h"

#ifdef TIER0_DLL_EXPORT
#  define MEM_INTERFACE DLL_EXPORT
#else
#  define MEM_INTERFACE DLL_IMPORT
#endif


//-----------------------------------------------------------------------------
// DLL-exported methods for particular kinds of memory
//-----------------------------------------------------------------------------
MEM_INTERFACE void *MemAllocScratch(int nMemSize);
MEM_INTERFACE void MemFreeScratch();

#ifdef __linux__
MEM_INTERFACE void ZeroMemory(void *mem, size_t length);
#endif


#endif /* TIER0_MEM_H */
