#include "precompiled.h"

#if defined(HOOK_HLTV)
#define ORIGINAL_CORE_DLL_NAME "core2.dll"

CSysModule *g_pOriginalCoreModule = NULL;
CreateInterfaceFn g_OriginalCoreFactory = NULL;
ISystemModule *g_pOriginalServer = NULL;
ISystemModule *g_pOriginalWorld = NULL;
ISystemModule *g_pOriginalNetwork = NULL;

IBaseInterface *CreateCoreInterface()
{
	if (g_pOriginalServer) {
		return g_pOriginalServer;
	}

	if (g_pOriginalCoreModule)
	{
		g_OriginalCoreFactory = Sys_GetFactory(g_pOriginalCoreModule);
		if (g_OriginalCoreFactory)
		{
			int returnCode = 0;
			g_pOriginalServer = reinterpret_cast<ISystemModule *>(g_OriginalCoreFactory(SERVER_INTERFACE_VERSION, &returnCode));
			return g_pOriginalServer;
		}
	}

	return NULL;
}

IBaseInterface *CreateWorldInterface()
{
	if (g_pOriginalWorld) {
		return g_pOriginalWorld;
	}

	if (g_pOriginalCoreModule)
	{
		g_OriginalCoreFactory = Sys_GetFactory(g_pOriginalCoreModule);
		if (g_OriginalCoreFactory)
		{
			int returnCode = 0;
			g_pOriginalWorld = reinterpret_cast<ISystemModule *>(g_OriginalCoreFactory(WORLD_INTERFACE_VERSION, &returnCode));
			return g_pOriginalWorld;
		}
	}

	return NULL;
}

IBaseInterface *CreateNetworkInterface()
{
	if (g_pOriginalNetwork) {
		return g_pOriginalNetwork;
	}

	if (g_pOriginalCoreModule)
	{
		g_OriginalCoreFactory = Sys_GetFactory(g_pOriginalCoreModule);
		if (g_OriginalCoreFactory)
		{
			int returnCode = 0;
			g_pOriginalNetwork = reinterpret_cast<ISystemModule *>(g_OriginalCoreFactory(NETWORK_INTERFACE_VERSION, &returnCode));
			return g_pOriginalNetwork;
		}
	}

	return NULL;
}

InterfaceReg iface_Server = InterfaceReg(CreateCoreInterface,    SERVER_INTERFACE_VERSION);
InterfaceReg iface_World  = InterfaceReg(CreateWorldInterface,   WORLD_INTERFACE_VERSION);
InterfaceReg iface_Network= InterfaceReg(CreateNetworkInterface, NETWORK_INTERFACE_VERSION);

// DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		g_pOriginalCoreModule = Sys_LoadModule(ORIGINAL_CORE_DLL_NAME);
		size_t addr = (size_t)Sys_GetProcAddress(ORIGINAL_CORE_DLL_NAME, CREATEINTERFACE_PROCNAME);
		HookModule("hltv.exe", addr);
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		if (g_pOriginalCoreModule)
		{
			Sys_UnloadModule(g_pOriginalCoreModule);
			g_pOriginalCoreModule = NULL;
			g_OriginalCoreFactory = NULL;

			g_pOriginalServer = NULL;
			g_pOriginalWorld = NULL;
			g_pOriginalNetwork = NULL;
		}
	}

	return TRUE;
}

#endif // #if defined(HOOK_HLTV)
