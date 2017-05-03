#include "precompiled.h"

#if defined(HOOK_HLTV)
#define ORIGINAL_PROXY_DLL_NAME "proxy2.dll"

CSysModule *g_pOriginalProxyModule = NULL;
CreateInterfaceFn g_OriginalProxyFactory = NULL;
ISystemModule *g_pOriginalProxy = NULL;

IBaseInterface *CreateProxyInterface()
{
	if (g_pOriginalProxy) {
		return g_pOriginalProxy;
	}

	if (g_pOriginalProxyModule)
	{
		g_OriginalProxyFactory = Sys_GetFactory(g_pOriginalProxyModule);
		if (g_OriginalProxyFactory)
		{
			int returnCode = 0;
			g_pOriginalProxy = reinterpret_cast<ISystemModule *>(g_OriginalProxyFactory(PROXY_INTERFACE_VERSION, &returnCode));
			return g_pOriginalProxy;
		}
	}

	return NULL;
}

InterfaceReg iface = InterfaceReg(CreateProxyInterface, PROXY_INTERFACE_VERSION);

// DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		g_pOriginalProxyModule = Sys_LoadModule(ORIGINAL_PROXY_DLL_NAME);
		size_t addr = (size_t)Sys_GetProcAddress(ORIGINAL_PROXY_DLL_NAME, CREATEINTERFACE_PROCNAME);
		HookModule("hltv.exe", addr);
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		if (g_pOriginalProxyModule)
		{
			Sys_UnloadModule(g_pOriginalProxyModule);
			g_pOriginalProxyModule = NULL;
			g_OriginalProxyFactory = NULL;
			g_pOriginalProxy = NULL;
		}
	}

	return TRUE;
}

#endif // #if defined(HOOK_HLTV)
