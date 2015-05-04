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


HIDDEN Module g_EngineModule = { NULL, NULL, NULL, NULL };

extern const size_t g_BaseOffset;
extern FunctionHook g_FunctionHooks[];
extern AddressRef g_FunctionRefs[];
extern AddressRef g_DataRefs[];

void *GetOriginalFuncAddrOrDie(const char *funcName)
{
	for (FunctionHook *cfh = &g_FunctionHooks[0]; cfh->symbolName; cfh++)
	{
		if (!strcmp(cfh->symbolName, funcName))
			return (void*) cfh->originalAddress;
	}

	rehlds_syserror("%s: Could not find function '%s'", __FUNCTION__, funcName);
	return NULL;
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

	rehlds_syserror("%s: Could not find function '%s'", __FUNCTION__, funcName);
	return NULL;
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

int HookEngine(size_t addr)
{
	if (addr == NULL || !FindModuleByAddress(addr, &g_EngineModule))
	{
		return (FALSE);
	}

	// Find all addresses
	bool success = true;

	AddressRef *refData = g_DataRefs;
	while (refData->symbolName != NULL)
	{
		if (!GetAddress(&g_EngineModule, (Address*)refData, g_BaseOffset))
		{
#if _DEBUG
			printf("%s: symbol not found \"%s\", symbol index: %i\n", __FUNCTION__, refData->symbolName, refData->symbolIndex);
			success = false;
#endif
		}
		refData++;
	}

	AddressRef *refFunc = g_FunctionRefs;
	while (refFunc->symbolName != NULL)
	{
		if (!GetAddress(&g_EngineModule, (Address*)refFunc, g_BaseOffset))
		{
#if _DEBUG
			printf("%s: symbol not found \"%s\", symbol index: %i\n", __FUNCTION__, refData->symbolName, refData->symbolIndex);
			success = false;
#endif
		}
		refFunc++;
	}

	FunctionHook *hookFunc = g_FunctionHooks;
	while (hookFunc->handlerFunc != NULL)
	{
		if (!GetAddress(&g_EngineModule, (Address*)hookFunc, g_BaseOffset))
		{
#if _DEBUG
			printf("%s: symbol not found \"%s\", symbol index: %i\n", __FUNCTION__, refData->symbolName, refData->symbolIndex);
			success = false;
#endif
		}
		hookFunc++;
	}

	if (!success)
	{
#if _DEBUG
		printf("%s: failed to hook engine!\n", __FUNCTION__);
#endif
		return (FALSE);
	}

#ifdef _WIN32
	Module hlds_exe;
	if (!FindModuleByName("hlds.exe", &hlds_exe))
		return (FALSE);

	TestSuite_Init(&g_EngineModule, &hlds_exe, g_FunctionRefs);
#endif


	refData = g_DataRefs;
	while (refData->addressRef != NULL)
	{
		if (!FindDataRef(&g_EngineModule, refData))
			return (FALSE);
		refData++;
	}

	refFunc = g_FunctionRefs;
	while (refFunc->addressRef != NULL)
	{
		if (!FindDataRef(&g_EngineModule, refFunc))
			return (FALSE);
		refFunc++;
	}

	// Actually hook all things
	if (!g_RehldsRuntimeConfig.disableAllHooks)
	{
		hookFunc = g_FunctionHooks;
		while (hookFunc->handlerFunc != NULL)
		{
			if (!HookFunction(&g_EngineModule, hookFunc))
				return (FALSE);
			hookFunc++;
		}
	}

#ifdef _WIN32
	Rehlds_Debug_Init(&g_EngineModule);
#endif

	return (TRUE);
}
