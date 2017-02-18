#pragma once
#ifndef REHLDS_ED_STRPOOL_H
#define REHLDS_ED_STRPOOL_H

#include "osconfig.h"
#include "static_map.h"

extern char* Ed_StrPool_Alloc(const char* str);
extern void Ed_StrPool_Reset();
extern void Ed_StrPool_Init();

#endif // REHLDS_ED_STRPOOL_H