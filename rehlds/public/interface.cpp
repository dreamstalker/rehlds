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

#include "interface.h"

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include "windows.h"
#endif // _WIN32

// InterfaceReg
InterfaceReg *InterfaceReg::s_pInterfaceRegs = nullptr;

InterfaceReg::InterfaceReg(InstantiateInterfaceFn fn, const char *pName) : m_pName(pName)
{
	m_CreateFn = fn;
	m_pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}

// This is the primary exported function by a dll, referenced by name via dynamic binding
// that exposes an opqaue function pointer to the interface.
//
// We have the Internal variant so Sys_GetFactoryThis() returns the correct internal
// symbol under GCC/Linux/Mac as CreateInterface is DLL_EXPORT so its global so the loaders
// on those OS's pick exactly 1 of the CreateInterface symbols to be the one that is process wide and
// all Sys_GetFactoryThis() calls find that one, which doesn't work. Using the internal walkthrough here
// makes sure Sys_GetFactoryThis() has the dll specific symbol and GetProcAddress() returns the module specific
// function for CreateInterface again getting the dll specific symbol we need.
EXPORT_FUNCTION IBaseInterface *CreateInterface(const char *pName, int *pReturnCode)
{
	InterfaceReg *pCur;
	for (pCur = InterfaceReg::s_pInterfaceRegs; pCur; pCur = pCur->m_pNext)
	{
		if (strcmp(pCur->m_pName, pName) == 0)
		{
			if (pReturnCode)
			{
				*pReturnCode = IFACE_OK;
			}

			return pCur->m_CreateFn();
		}
	}

	if (pReturnCode)
	{
		*pReturnCode = IFACE_FAILED;
	}

	return nullptr;
}

#ifndef _WIN32
// Linux doesn't have this function so this emulates its functionality
void *GetModuleHandle(const char *name)
{
	void *handle;
	if (name == nullptr)
	{
		// hmm, how can this be handled under linux....
		// is it even needed?
		return nullptr;
	}

	if ((handle = dlopen(name, RTLD_NOW)) == nullptr)
	{
		//printf("Error:%s\n",dlerror());
		// couldn't open this file
		return nullptr;
	}

	// read "man dlopen" for details
	// in short dlopen() inc a ref count
	// so dec the ref count by performing the close
	dlclose(handle);
	return handle;
}
#endif // _WIN32

// Purpose: returns a pointer to a function, given a module
// Input  : pModuleName - module name
//			*pName - proc name
//static hlds_run wants to use this function
void *Sys_GetProcAddress(const char *pModuleName, const char *pName)
{
	return GetProcAddress(GetModuleHandle(pModuleName), pName);
}

// Purpose: returns a pointer to a function, given a module
// Input  : pModuleName - module name
//			*pName - proc name
// hlds_run wants to use this function
void *Sys_GetProcAddress(void *pModuleHandle, const char *pName)
{
	return GetProcAddress((HMODULE)pModuleHandle, pName);
}

// Purpose: Loads a DLL/component from disk and returns a handle to it
// Input  : *pModuleName - filename of the component
// Output : opaque handle to the module (hides system dependency)
CSysModule *Sys_LoadModule(const char *pModuleName)
{
#ifdef _WIN32
	HMODULE hDLL = LoadLibrary(pModuleName);
#else
	HMODULE hDLL  = nullptr;
	char szAbsoluteModuleName[2048];
	if (pModuleName[0] != '/')
	{
		char szCwd[1024];
		getcwd(szCwd, sizeof(szCwd));
		if (szCwd[strlen(szCwd) - 1] == '/')
			szCwd[strlen(szCwd) - 1] = '\0';

		_snprintf(szAbsoluteModuleName, sizeof(szAbsoluteModuleName), "%s/%s", szCwd, pModuleName);
		hDLL = dlopen(szAbsoluteModuleName, RTLD_NOW);
	}
	else
	{
		_snprintf(szAbsoluteModuleName, sizeof(szAbsoluteModuleName), "%s", pModuleName);
		hDLL = dlopen(pModuleName, RTLD_NOW);
	}
#endif // _WIN32

	if (!hDLL)
	{
		char str[2048+6]; // room for extension string

#if defined(_WIN32)
		_snprintf(str, sizeof(str), "%s.dll", pModuleName);
		hDLL = LoadLibrary(str);
#elif defined(OSX)
		printf("Error: %s\n", dlerror());
		_snprintf(str, sizeof(str), "%s.dylib", szAbsoluteModuleName);
		hDLL = dlopen(str, RTLD_NOW);
#else
		printf("Error: %s\n", dlerror());
		_snprintf(str, sizeof(str), "%s.so", szAbsoluteModuleName);
		hDLL = dlopen(str, RTLD_NOW);
#endif
	}

	return reinterpret_cast<CSysModule *>(hDLL);
}

// Purpose: Unloads a DLL/component from
// Input  : *pModuleName - filename of the component
// Output : opaque handle to the module (hides system dependency)
void Sys_UnloadModule(CSysModule *pModule)
{
	if (!pModule)
		return;

	HMODULE hDLL = reinterpret_cast<HMODULE>(pModule);

#ifdef _WIN32
	FreeLibrary(hDLL);
#else
	dlclose(hDLL);
#endif // _WIN32
}

// Purpose: returns a pointer to a function, given a module
// Input  : module - windows HMODULE from Sys_LoadModule()
//			*pName - proc name
// Output : factory for this module
CreateInterfaceFn Sys_GetFactory(CSysModule *pModule)
{
	if (!pModule)
		return nullptr;

	return reinterpret_cast<CreateInterfaceFn>(Sys_GetProcAddress(pModule, CREATEINTERFACE_PROCNAME));
}

// Purpose: returns the instance of this module
// Output : CreateInterfaceFn
CreateInterfaceFn Sys_GetFactoryThis()
{
	return CreateInterface;
}

// Purpose: returns the instance of the named module
// Input  : *pModuleName - name of the module
// Output : CreateInterfaceFn - instance of that module
CreateInterfaceFn Sys_GetFactory(const char *pModuleName)
{
	return reinterpret_cast<CreateInterfaceFn>(Sys_GetProcAddress(pModuleName, CREATEINTERFACE_PROCNAME));
}

// Purpose: finds a particular interface in the factory set
void *InitializeInterface(char const *interfaceName, CreateInterfaceFn *factoryList, int numFactories)
{
	void *retval;

	for (int i = 0; i < numFactories; i++)
	{
		CreateInterfaceFn factory = factoryList[ i ];
		if (!factory)
			continue;

		retval = factory(interfaceName, nullptr);
		if (retval)
			return retval;
	}

	// No provider for requested interface!!!
	// assert(!"No provider for requested interface!!!");

	return nullptr;
}
