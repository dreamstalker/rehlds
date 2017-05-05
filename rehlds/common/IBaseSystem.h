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

#if defined(_WIN32)
	#define LIBRARY_PREFIX "dll"
#elif defined(OSX)
	#define LIBRARY_PREFIX "dylib"
#else
	#define LIBRARY_PREFIX "so"
#endif

#include "ISystemModule.h"
#include "IVGuiModule.h"

class Panel;
class ObjectList;
class IFileSystem;

class IBaseSystem: virtual public ISystemModule {
public:
	virtual ~IBaseSystem() {}

	virtual double GetTime() = 0;
	virtual unsigned int GetTick() = 0;
	virtual void SetFPS(float fps) = 0;

	virtual void Printf(char *fmt, ...) = 0;
	virtual void DPrintf(char *fmt, ...) = 0;

	virtual void RedirectOutput(char *buffer = nullptr, int maxSize = 0) = 0;

	virtual IFileSystem *GetFileSystem() = 0;
	virtual unsigned char *LoadFile(const char *name, int *length = nullptr) = 0;
	virtual void FreeFile(unsigned char *fileHandle) = 0;

	virtual void SetTitle(char *text) = 0;
	virtual void SetStatusLine(char *text) = 0;

	virtual void ShowConsole(bool visible) = 0;
	virtual void LogConsole(char *filename) = 0;

	virtual bool InitVGUI(IVGuiModule *module) = 0;

#ifdef _WIN32
	virtual Panel *GetPanel() = 0;
#endif // _WIN32

	virtual bool RegisterCommand(char *name, ISystemModule *module, int commandID) = 0;
	virtual void GetCommandMatches(char *string, ObjectList *pMatchList) = 0;
	virtual void ExecuteString(char *commands) = 0;
	virtual void ExecuteFile(char *filename) = 0;
	virtual void Errorf(char *fmt, ...) = 0;

	virtual char *CheckParam(char *param) = 0;

	virtual bool AddModule(ISystemModule *module, char *name) = 0;
	virtual ISystemModule *GetModule(char *interfacename, char *library, char *instancename = nullptr) = 0;
	virtual bool RemoveModule(ISystemModule *module) = 0;

	virtual void Stop() = 0;
	virtual char *GetBaseDir() = 0;
};

#define BASESYSTEM_INTERFACE_VERSION "basesystem002"
