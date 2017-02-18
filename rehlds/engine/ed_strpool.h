#pragma once

#include "osconfig.h"
#include "static_map.h"

extern char* Ed_StrPool_Alloc(const char* str);
extern void Ed_StrPool_Reset();
extern void Ed_StrPool_Init();