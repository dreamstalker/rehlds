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

#include "interface.h"

// handle to a game window
typedef unsigned int ManageServerUIHandle_t;
class IManageServer;

// Purpose: Interface to server administration functions
class IAdminServer: public IBaseInterface
{
public:
	// opens a manage server dialog for a local server
	virtual ManageServerUIHandle_t OpenManageServerDialog(const char *serverName, const char *gameDir) = 0;

	// opens a manage server dialog to a remote server
	virtual ManageServerUIHandle_t OpenManageServerDialog(unsigned int gameIP, unsigned int gamePort, const char *password) = 0;

	// forces the game info dialog closed
	virtual void CloseManageServerDialog(ManageServerUIHandle_t gameDialog) = 0;

	// Gets a handle to the interface
	virtual IManageServer *GetManageServerInterface(ManageServerUIHandle_t handle) = 0;
};

#define ADMINSERVER_INTERFACE_VERSION "AdminServer002"
