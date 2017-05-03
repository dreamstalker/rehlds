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

Module g_Module = { NULL, NULL, NULL, NULL };

// Offset where module assumed be loaded to ajust hooks offsets. NULL for the Linux to trigger symbols searching.
#ifdef _WIN32
const size_t g_BaseOffset = 0x01D00000;
#else
const size_t g_BaseOffset = NULL;
#endif

void *GetOriginalFuncAddrOrDie(const char *funcName)
{
	for (FunctionHook *cfh = &g_FunctionHooks[0]; cfh->symbolName; cfh++)
	{
		if (!strcmp(cfh->symbolName, funcName))
			return (void*) cfh->originalAddress;
	}

	rehlds_syserror("%s: Could not find function '%s'", __func__, funcName);
}

void *GetOriginalFuncAddrOrDefault(const char *funcName, void *def)
{
	for (FunctionHook *cfh = &g_FunctionHooks[0]; cfh->symbolName; cfh++)
	{
		if (!strcmp(cfh->symbolName, funcName))
			return (void*)cfh->originalAddress;
	}

	return def;
}

void *GetFuncRefAddrOrDie(const char *funcName)
{
	for (AddressRef *cfh = &g_FunctionRefs[0]; cfh->symbolName; cfh++)
	{
		if (!strcmp(cfh->symbolName, funcName))
			return (void*)cfh->originalAddress;
	}

	rehlds_syserror("%s: Could not find function '%s'", __func__, funcName);
}

void *GetFuncRefAddrOrDefault(const char *funcName, void *def)
{
	for (AddressRef *cfh = &g_FunctionRefs[0]; cfh->symbolName; cfh++)
	{
		if (!strcmp(cfh->symbolName, funcName))
			return (void*)cfh->originalAddress;
	}

	return def;
}

int HookModule(const char *pszAppName, size_t addr)
{
	if (addr == NULL || !FindModuleByAddress(addr, &g_Module)) {
		return (FALSE);
	}

	// Find all addresses
	bool success = true;

	AddressRef *refData = g_DataRefs;
	while (refData->symbolName != NULL)
	{
		if (!GetAddress(&g_Module, (Address*)refData, g_BaseOffset))
		{
#if _DEBUG
			printf("%s: symbol not found \"%s\", symbol index: %i\n", __func__, refData->symbolName, refData->symbolIndex);
			success = false;
#endif
		}
		refData++;
	}

	AddressRef *refFunc = g_FunctionRefs;
	while (refFunc->symbolName != NULL)
	{
		if (!GetAddress(&g_Module, (Address*)refFunc, g_BaseOffset))
		{
#if _DEBUG
			printf("%s: symbol not found \"%s\", symbol index: %i\n", __func__, refData->symbolName, refData->symbolIndex);
			success = false;
#endif
		}
		refFunc++;
	}

	FunctionHook *hookFunc = g_FunctionHooks;
	while (hookFunc->handlerFunc != NULL)
	{
		if (!GetAddress(&g_Module, (Address*)hookFunc, g_BaseOffset))
		{
#if _DEBUG
			printf("%s: symbol not found \"%s\", symbol index: %i\n", __func__, refData->symbolName, refData->symbolIndex);
			success = false;
#endif
		}
		hookFunc++;
	}

	if (!success)
	{
#if _DEBUG
		printf("%s: failed to hook module!\n", __func__);
#endif
		return (FALSE);
	}

#if defined(_WIN32) && defined(HOOK_ENGINE)
	Module application;
	if (!FindModuleByName(pszAppName, &application))
		printf("%s: launcher is not %s, tests playing/recording is disabled!\n", __func__, pszAppName);
	else
		TestSuite_Init(&g_Module, &application, g_FunctionRefs);
#endif

	refData = g_DataRefs;
	while (refData->addressRef != NULL)
	{
		if (!FindDataRef(&g_Module, refData))
			return (FALSE);
		refData++;
	}

	refFunc = g_FunctionRefs;
	while (refFunc->addressRef != NULL)
	{
		if (!FindDataRef(&g_Module, refFunc))
			return (FALSE);
		refFunc++;
	}

#ifdef HOOK_ENGINE
	// Actually hook all things
	if (!g_RehldsRuntimeConfig.disableAllHooks)
#endif // SWDS
	{
		hookFunc = g_FunctionHooks;
		while (hookFunc->handlerFunc != NULL)
		{
			if (!HookFunction(&g_Module, hookFunc))
				return (FALSE);
			hookFunc++;
		}
	}

#if defined(_WIN32) && defined(HOOK_ENGINE)
	Rehlds_Debug_Init(&g_Module);
#endif // _WIN32

	return (TRUE);
}

#ifdef _WIN32

void *malloc_wrapper(size_t size) {
	void *res = malloc(size);
#ifdef HOOK_ENGINE
	//Rehlds_Debug_logAlloc(size, res);
#endif // HOOK_ENGINE
	return res;
}

void *realloc_wrapper(void *orig, size_t newSize) {
	void *res = realloc(orig, newSize);
#ifdef HOOK_ENGINE
	//Rehlds_Debug_logRealloc(newSize, orig, res);
#endif // HOOK_ENGINE
	return res;
}

void free_wrapper(void *mem) {
#ifdef HOOK_ENGINE
	//Rehlds_Debug_logFree(mem);
#endif // HOOK_ENGINE
	free(mem);
}

void *calloc_wrapper(size_t count, size_t size) {
	void *res = calloc(count, size);
#ifdef HOOK_ENGINE
	//Rehlds_Debug_logAlloc(size * count, res);
#endif // HOOK_ENGINE
	return res;
}

void *__nh_malloc_wrapper(size_t sz, int unk) {
	void *res = malloc(sz);
#ifdef HOOK_ENGINE
	//Rehlds_Debug_logAlloc(sz, res);
#endif // HOOK_ENGINE
	return res;
}

char *strdup_wrapper(const char *s) {
	return _strdup(s);
}

#endif // _WIN32

void logf(const char *fmt, ...)
{
	va_list argptr;
	static char string[8192];

	va_start(argptr, fmt);
	_vsnprintf(string, sizeof(string), fmt, argptr);
	string[sizeof(string) - 1] = 0;

	FILE *fl = fopen("hooker.log", "a");
	fprintf(fl, "%s\n", string);
	fclose(fl);
}
