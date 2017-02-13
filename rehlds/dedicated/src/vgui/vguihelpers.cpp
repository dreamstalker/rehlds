#include "precompiled.h"

CSysModule *g_pVGUIModule = nullptr;

bool InitializeVGui(CreateInterfaceFn *factorylist, int factorycount)
{
#ifdef _WIN32
	// TODO: finish me!
	/*if (vgui::VGuiControls_Init("DEDICATED", factoryList, numFactories))
	{
		filesystem()->AddSearchPath(".", "MAIN");
		return true;
	}*/
#endif

	return false;
}

// Starts up the VGUI system and loads the base panel
int StartVGUI()
{
	// TODO: finish me!
	/*g_pVGUIModule = Sys_LoadModule("vgui2.dll");

	CreateInterfaceFn ifaceFactory = Sys_GetFactoryThis();
	CreateInterfaceFn adminFactory = Sys_GetFactory(g_pVGUIModule);
	CreateInterfaceFn filesystemFactory = Sys_GetFactory(g_pFileSystemModule);
	CreateInterfaceFn dedicatedFactory = Sys_GetFactory(g_pEngineModule);

	const int numFactories = 4;
	if (!InitializeVGui(&ifaceFactory, numFactories))
	{
		MessageBox(NULL, "Fatal Error: Could not initialize vgui.", "Steam - Fatal Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	filesystem()->AddSearchPath("platform", "PLATFORM");

	// find our configuration directory
	char szConfigDir[512];
	const char *steamPath = getenv("SteamInstallPath");
	if (steamPath)
	{
		// put the config dir directly under steam
		_snprintf(szConfigDir, sizeof(szConfigDir), "%s/config", steamPath);
	}
	else
	{
		// we're not running steam, so just put the config dir under the platform
		strncpy(szConfigDir, "platform/config", sizeof(szConfigDir));
	}

	mkdir(szConfigDir);
	filesystem()->AddSearchPath(szConfigDir, "CONFIG");

	vgui::system()->SetUserConfigFile("DialogConfig.vdf", "CONFIG");

	// Init the surface
	g_pMainPanel = new CMainPanel();
	g_pMainPanel->SetVisible(true);

	vgui::surface()->SetEmbeddedPanel(g_pMainPanel->GetVPanel());

	// load the scheme
	vgui::scheme()->LoadSchemeFromFile("Resource/TrackerScheme.res", NULL);

	// localization
	vgui::localize()->AddFile("Resource/platform_%language%.txt");
	vgui::localize()->AddFile("Resource/vgui_%language%.txt");
	vgui::localize()->AddFile("Admin/server_%language%.txt");

	// Start vgui
	vgui::ivgui()->Start();

	// load the module
	g_pFullFileSystem->GetLocalCopy("Platform/Admin/AdminServer.dll");
	g_hAdminServerModule = Sys_LoadModule("Platform/Admin/AdminServer.dll");
	Assert(g_hAdminServerModule != NULL);
	CreateInterfaceFn adminFactory = NULL;

	if (!g_hAdminServerModule)
	{
		vgui::ivgui()->DPrintf2("Admin Error: module version (Admin/AdminServer.dll, %s) invalid, not loading\n", IMANAGESERVER_INTERFACE_VERSION);
	}
	else
	{
		// make sure we get the right version
		adminFactory = Sys_GetFactory(g_hAdminServerModule);
		g_pAdminServer = (IAdminServer *)adminFactory(ADMINSERVER_INTERFACE_VERSION, NULL);
		g_pAdminVGuiModule = (IVGuiModule *)adminFactory("VGuiModuleAdminServer001", NULL);
		Assert(g_pAdminServer != NULL);
		Assert(g_pAdminVGuiModule != NULL);
		if (!g_pAdminServer || !g_pAdminVGuiModule)
		{
			vgui::ivgui()->DPrintf2("Admin Error: module version (Admin/AdminServer.dll, %s) invalid, not loading\n", IMANAGESERVER_INTERFACE_VERSION);
		}
	}

	// finish initializing admin module
	g_pAdminVGuiModule->Initialize(&dedicatedFactory, 1);
	g_pAdminVGuiModule->PostInitialize(&adminFactory, 1);
	g_pAdminVGuiModule->SetParent(g_pMainPanel->GetVPanel());

	// finish setting up main panel
	g_pMainPanel->Initialize();
	g_pMainPanel->Open();
*/
	return 0;
}

// Run a single VGUI frame
void StopVGUI()
{
}

void RunVGUIFrame()
{
	// TODO: finish me!
	//vgui::ivgui()->RunFrame();
}

bool VGUIIsRunning()
{
	//return vgui::ivgui()->IsRunning();
	return false;
}

bool VGUIIsStopping()
{
	//return g_pMainPanel->Stopping();
	return false;
}

bool VGUIIsInConfig()
{
	//return g_pMainPanel->IsInConfig();
	return false;
}

void VGUIFinishedConfig()
{
	/*Assert(g_pMainPanel);

	// engine is loaded, pass the message on
	if (g_pMainPanel)
	{
		SetEvent(g_pMainPanel->GetShutdownHandle());
	}*/
}

void VGUIPrintf(const char *msg)
{
	/*if (g_pMainPanel)
	{
		g_pMainPanel->AddConsoleText(msg);
	}*/
}
