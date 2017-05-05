#pragma once

#include "osconfig.h"

extern int HookModule(const char *pszAppName, size_t addr);
extern void *GetOriginalFuncAddrOrDie(const char *funcName);
extern void *GetOriginalFuncAddrOrDefault(const char *funcName, void *def);
extern void *GetFuncRefAddrOrDie(const char *funcName);
extern void *GetFuncRefAddrOrDefault(const char *funcName, void *def);
extern void logf(const char *fmt, ...);

extern FunctionHook g_FunctionHooks[];
extern AddressRef g_FunctionRefs[];
extern AddressRef g_DataRefs[];

#ifdef _WIN32

void *malloc_wrapper(size_t size);
void *realloc_wrapper(void *orig, size_t newSize);
void free_wrapper(void *mem);
void *calloc_wrapper(size_t count, size_t size);
void *__nh_malloc_wrapper(size_t sz, int unk);
char *strdup_wrapper(const char *s);

#endif // _WIN32
