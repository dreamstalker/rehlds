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

#include "maintypes.h"
#include "iregistry.h"
#include "utlvector.h"

extern CUtlVector<char *> g_fallbackLocalizationFiles;
extern char s_pBaseDir[512];
extern bool bLowViolenceBuild;

extern CSysModule *g_pFileSystemModule;
extern CreateInterfaceFn g_FileSystemFactory;

const char *GetBaseDirectory(void);
NOXREF void *GetFileSystemFactory(void);
bool FileSystem_LoadDLL(CreateInterfaceFn filesystemFactory);
void FileSystem_UnloadDLL(void);
bool BEnabledHDAddon(void);
bool BEnableAddonsFolder(void);
void Host_SetHDModels_f(void);
void Host_SetAddonsFolder_f(void);
void Host_SetVideoLevel_f(void);
int Host_GetVideoLevel(void);
void CheckLiblistForFallbackDir(const char *pGameDir, bool bLanguage, const char *pLanguage, bool bLowViolenceBuild_);
int FileSystem_SetGameDirectory(const char *pDefaultDir, const char *pGameDir);
int FileSystem_AddFallbackGameDir(const char *pGameDir);
int FileSystem_Init(char *basedir, void *voidfilesystemFactory);
void FileSystem_Shutdown(void);
