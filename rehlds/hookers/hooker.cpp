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

	hookFunc = g_FunctionHooks;
	while (hookFunc->handlerFunc != NULL)
	{
		if (!HookFunction(&g_Module, hookFunc))
			return (FALSE);
		hookFunc++;
	}

	return (TRUE);
}

#ifdef _WIN32

void *malloc_wrapper(size_t size) {
	return malloc(size);
}

void *realloc_wrapper(void *orig, size_t newSize) {
	return realloc(orig, newSize);
}

void free_wrapper(void *mem) {
	free(mem);
}

void *calloc_wrapper(size_t count, size_t size) {
	return calloc(count, size);
}

void *__nh_malloc_wrapper(size_t sz, int unk) {
	return malloc(sz);
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
