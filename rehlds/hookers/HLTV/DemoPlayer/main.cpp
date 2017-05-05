#include "precompiled.h"

#if defined(HOOK_HLTV)
#define ORIGINAL_DEMOPLAYER_DLL_NAME "DemoPlayer2.dll"

CSysModule *g_pOriginalDemoPlayerModule = NULL;
CreateInterfaceFn g_OriginalDemoPlayerFactory = NULL;
ISystemModule *g_pOriginalDemoPlayer = NULL;

IBaseInterface *CreateDemoPlayerInterface()
{
	if (g_pOriginalDemoPlayer) {
		return g_pOriginalDemoPlayer;
	}

	if (g_pOriginalDemoPlayerModule)
	{
		g_OriginalDemoPlayerFactory = Sys_GetFactory(g_pOriginalDemoPlayerModule);
		if (g_OriginalDemoPlayerFactory)
		{
			int returnCode = 0;
			g_pOriginalDemoPlayer = reinterpret_cast<ISystemModule *>(g_OriginalDemoPlayerFactory(DEMOPLAYER_INTERFACE_VERSION, &returnCode));
			return g_pOriginalDemoPlayer;
		}
	}

	return NULL;
}

InterfaceReg iface = InterfaceReg(CreateDemoPlayerInterface, DEMOPLAYER_INTERFACE_VERSION);

// DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		g_pOriginalDemoPlayerModule = Sys_LoadModule(ORIGINAL_DEMOPLAYER_DLL_NAME);
		size_t addr = (size_t)Sys_GetProcAddress(ORIGINAL_DEMOPLAYER_DLL_NAME, CREATEINTERFACE_PROCNAME);
		HookModule("hltv.exe", addr);
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		if (g_pOriginalDemoPlayerModule)
		{
			Sys_UnloadModule(g_pOriginalDemoPlayerModule);
			g_pOriginalDemoPlayerModule = NULL;
			g_OriginalDemoPlayerFactory = NULL;
			g_pOriginalDemoPlayer = NULL;
		}
	}

	return TRUE;
}

#endif // #if defined(HOOK_HLTV)
