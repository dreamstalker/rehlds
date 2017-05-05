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

#include "ISystemModule.h"

class IWorld;
class IProxy;
class BitBuffer;
class DirectorCmd;
class IObjectContainer;

class IDirector: virtual public ISystemModule {
public:
	virtual ~IDirector() {}

	virtual void NewGame(IWorld *world, IProxy *proxy) = 0;
	virtual char *GetModName() = 0;
	virtual void WriteCommands(BitBuffer *stream, float startTime, float endTime) = 0;
	virtual int AddCommand(DirectorCmd *cmd) = 0;
	virtual bool RemoveCommand(int index) = 0;
	virtual DirectorCmd *GetLastCommand() = 0;
	virtual IObjectContainer *GetCommands() = 0;
};

#define DIRECTOR_INTERFACE_VERSION "director001"
