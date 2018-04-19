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

#pragma once

#include <vgui/VGUI.h>
#include "interface.h"

// Purpose: Standard interface to loading vgui modules
class IVGuiModule: public IBaseInterface
{
public:
	// called first to setup the module with the vgui
	// returns true on success, false on failure
	virtual bool Initialize(CreateInterfaceFn *vguiFactories, int factoryCount) = 0;

	// called after all the modules have been initialized
	// modules should use this time to link to all the other module interfaces
	virtual bool PostInitialize(CreateInterfaceFn *modules = nullptr, int factoryCount = 0) = 0;

	// called when the module is selected from the menu or otherwise activated
	virtual bool Activate() = 0;

	// returns true if the module is successfully initialized and available
	virtual bool IsValid() = 0;

	// requests that the UI is temporarily disabled and all data files saved
	virtual void Deactivate() = 0;

	// restart from a Deactivate()
	virtual void Reactivate() = 0;

	// called when the module is about to be shutdown
	virtual void Shutdown() = 0;

	// returns a handle to the main module panel
	virtual vgui2::VPANEL GetPanel() = 0;

	// sets the parent of the main module panel
	virtual void SetParent(vgui2::VPANEL parent) = 0;

	// messages sent through through the panel returned by GetPanel():
	//
	//  "ConnectedToGame"	"ip" "port" "gamedir"
	//	"DisconnectedFromGame"
	//  "ActiveGameName"	"name"
	//	"LoadingStarted"	"type"	"name"
	//  "LoadingFinished"	"type"	"name"
};

#define VGUIMODULE_INTERFACE_VERSION "VGuiModuleAdminServer001"
