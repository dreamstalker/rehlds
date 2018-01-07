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

#if defined(HOOK_FILESYSTEM)

const char *ORIGINAL_FILESYSTEM_DLL_NAME = "filesystem_stdio2.dll";

CSysModule *g_pOriginalFileSystemModule = nullptr;
CreateInterfaceFn g_OriginalFileSystemFactory = nullptr;
IFileSystem *g_pOriginalFileSystem = nullptr;

IBaseInterface *CreateFileSystemInterface()
{
	if (g_pOriginalFileSystem) {
		return g_pOriginalFileSystem;
	}

	if (g_pOriginalFileSystemModule)
	{
		g_OriginalFileSystemFactory = (CreateInterfaceFn)Sys_GetFactory(g_pOriginalFileSystemModule);

		if (g_OriginalFileSystemFactory)
		{
			int returnCode = 0;
			g_pOriginalFileSystem = (IFileSystem *)g_OriginalFileSystemFactory(FILESYSTEM_INTERFACE_VERSION, &returnCode);
			return g_pOriginalFileSystem;
		}
	}

	return nullptr;
}

EXPOSE_INTERFACE_FN(CreateFileSystemInterface, IFileSystem, FILESYSTEM_INTERFACE_VERSION);

// DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		g_pOriginalFileSystemModule = (CSysModule *)LoadLibrary(ORIGINAL_FILESYSTEM_DLL_NAME);
		size_t addr = (size_t)Sys_GetProcAddress(ORIGINAL_FILESYSTEM_DLL_NAME, CREATEINTERFACE_PROCNAME);
		HookModule("hlds.exe", addr);
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		if (g_pOriginalFileSystemModule)
		{
			Sys_UnloadModule(g_pOriginalFileSystemModule);
			g_pOriginalFileSystemModule = nullptr;

			g_OriginalFileSystemFactory = nullptr;
			g_pOriginalFileSystem = nullptr;
		}
	}

	return TRUE;
}

#endif // #if defined(HOOK_FILESYSTEM)
