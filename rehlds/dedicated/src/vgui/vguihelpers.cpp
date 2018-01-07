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
		MessageBox(nullptr, "Fatal Error: Could not initialize vgui.", "Steam - Fatal Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	filesystem()->AddSearchPath("platform", "PLATFORM");

	// find our configuration directory
	char szConfigDir[512];
	const char *steamPath = getenv("SteamInstallPath");
	if (steamPath)
	{
		// put the config dir directly under steam
		Q_snprintf(szConfigDir, sizeof(szConfigDir), "%s/config", steamPath);
	}
	else
	{
		// we're not running steam, so just put the config dir under the platform
		Q_strlcpy(szConfigDir, "platform/config");
	}

	mkdir(szConfigDir);
	filesystem()->AddSearchPath(szConfigDir, "CONFIG");

	vgui::system()->SetUserConfigFile("DialogConfig.vdf", "CONFIG");

	// Init the surface
	g_pMainPanel = new CMainPanel();
	g_pMainPanel->SetVisible(true);

	vgui::surface()->SetEmbeddedPanel(g_pMainPanel->GetVPanel());

	// load the scheme
	vgui::scheme()->LoadSchemeFromFile("Resource/TrackerScheme.res", nullptr);

	// localization
	vgui::localize()->AddFile("Resource/platform_%language%.txt");
	vgui::localize()->AddFile("Resource/vgui_%language%.txt");
	vgui::localize()->AddFile("Admin/server_%language%.txt");

	// Start vgui
	vgui::ivgui()->Start();

	// load the module
	g_pFullFileSystem->GetLocalCopy("Platform/Admin/AdminServer.dll");
	g_hAdminServerModule = Sys_LoadModule("Platform/Admin/AdminServer.dll");
	Assert(g_hAdminServerModule != nullptr);
	CreateInterfaceFn adminFactory = nullptr;

	if (!g_hAdminServerModule)
	{
		vgui::ivgui()->DPrintf2("Admin Error: module version (Admin/AdminServer.dll, %s) invalid, not loading\n", IMANAGESERVER_INTERFACE_VERSION);
	}
	else
	{
		// make sure we get the right version
		adminFactory = Sys_GetFactory(g_hAdminServerModule);
		g_pAdminServer = (IAdminServer *)adminFactory(ADMINSERVER_INTERFACE_VERSION, nullptr);
		g_pAdminVGuiModule = (IVGuiModule *)adminFactory(VGUIMODULE_INTERFACE_VERSION, nullptr);
		Assert(g_pAdminServer != nullptr);
		Assert(g_pAdminVGuiModule != nullptr);
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
