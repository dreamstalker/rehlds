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

IFileSystem *g_pFileSystem;


/* <2605f> ../engine/filesystem_internal.cpp:18 */
NOXREF void FS_RemoveAllSearchPaths(void)
{
	//NOXREFCHECK;	// We call it

	g_pFileSystem->RemoveAllSearchPaths();
}

/* <26073> ../engine/filesystem_internal.cpp:24 */
void FS_AddSearchPath(const char *pPath, const char *pathID)
{
	g_pFileSystem->AddSearchPath(pPath, pathID);
}

/* <260a8> ../engine/filesystem_internal.cpp:30 */
NOXREF int FS_RemoveSearchPath(const char *pPath)
{
	NOXREFCHECK;

	return g_pFileSystem->RemoveSearchPath(pPath);
}

/* <260d3> ../engine/filesystem_internal.cpp:36 */
void FS_RemoveFile(const char *pRelativePath, const char *pathID)
{
	g_pFileSystem->RemoveFile(pRelativePath, pathID);
}

/* <26108> ../engine/filesystem_internal.cpp:42 */
void FS_CreateDirHierarchy(const char *path, const char *pathID)
{
	g_pFileSystem->CreateDirHierarchy(path, pathID);
}

/* <2613d> ../engine/filesystem_internal.cpp:48 */
int FS_FileExists(const char *pFileName)
{
	return g_pFileSystem->FileExists(pFileName);
}

/* <26168> ../engine/filesystem_internal.cpp:54 */
NOXREF int FS_IsDirectory(const char *pFileName)
{
	NOXREFCHECK;

	return g_pFileSystem->IsDirectory(pFileName);
}

/* <26193> ../engine/filesystem_internal.cpp:60 */
FileHandle_t FS_Open(const char *pFileName, const char *pOptions)
{
	return g_pFileSystem->Open(pFileName, pOptions, 0);
}

/* <261cc> ../engine/filesystem_internal.cpp:66 */
FileHandle_t FS_OpenPathID(const char *pFileName, const char *pOptions, const char *pathID)
{
	return g_pFileSystem->Open(pFileName, pOptions, pathID);
}

/* <26213> ../engine/filesystem_internal.cpp:72 */
void FS_Close(FileHandle_t file)
{
	g_pFileSystem->Close(file);
}

/* <2623a> ../engine/filesystem_internal.cpp:78 */
void FS_Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType)
{
	g_pFileSystem->Seek(file, pos, seekType);
}

/* <2627d> ../engine/filesystem_internal.cpp:84 */
unsigned int FS_Tell(FileHandle_t file)
{
	return g_pFileSystem->Tell(file);
}

/* <262a8> ../engine/filesystem_internal.cpp:90 */
unsigned int FS_Size(FileHandle_t file)
{
	return g_pFileSystem->Size(file);
}

/* <262d3> ../engine/filesystem_internal.cpp:96 */
unsigned int FS_FileSize(const char *pFileName)
{
	return g_pFileSystem->Size(pFileName);
}

/* <262fe> ../engine/filesystem_internal.cpp:101 */
int32_t FS_GetFileTime(const char *pFileName)
{
	return g_pFileSystem->GetFileTime(pFileName);
}

/* <26329> ../engine/filesystem_internal.cpp:107 */
NOXREF void FS_FileTimeToString(char *pStrip, int maxCharsIncludingTerminator, int32_t fileTime)
{
	NOXREFCHECK;

	g_pFileSystem->FileTimeToString(pStrip, maxCharsIncludingTerminator, fileTime);
}

/* <2636c> ../engine/filesystem_internal.cpp:113 */
int FS_IsOk(FileHandle_t file)
{
	return g_pFileSystem->IsOk(file);
}

/* <26397> ../engine/filesystem_internal.cpp:119 */
void FS_Flush(FileHandle_t file)
{
	g_pFileSystem->Flush(file);
}

/* <263be> ../engine/filesystem_internal.cpp:125 */
int FS_EndOfFile(FileHandle_t file)
{
	return g_pFileSystem->EndOfFile(file);
}

/* <25fa9> ../engine/filesystem_internal.cpp:131 */
int FS_Read(void *pOutput, int size, int count, FileHandle_t file)
{
	return g_pFileSystem->Read(pOutput, size, file);
}

/* <2641e> ../engine/filesystem_internal.cpp:138 */
int FS_Write(const void *pInput, int size, int count, FileHandle_t file)
{
	return g_pFileSystem->Write(pInput, size, file);
}

/* <26479> ../engine/filesystem_internal.cpp:145 */
char *FS_ReadLine(char *pOutput, int maxChars, FileHandle_t file)
{
	return g_pFileSystem->ReadLine(pOutput, maxChars, file);
}

/* <264c0> ../engine/filesystem_internal.cpp:151 */
int FS_FPrintf(FileHandle_t file, char *pFormat, ...)
{
	char data[8192];
	va_list va;

	va_start(va, pFormat);
	vsprintf(data, pFormat, va);
	va_end(va);

	return g_pFileSystem->FPrintf(file, "%s", data);
}

/* <26534> ../engine/filesystem_internal.cpp:164 */
const char *FS_FindFirst(const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID)
{
	return g_pFileSystem->FindFirst(pWildCard, pHandle, pathID);
}

/* <26581> ../engine/filesystem_internal.cpp:170 */
const char *FS_FindNext(FileFindHandle_t handle)
{
	return g_pFileSystem->FindNext(handle);
}

/* <265ac> ../engine/filesystem_internal.cpp:176 */
NOXREF int FS_FindIsDirectory(FileFindHandle_t handle)
{
	NOXREFCHECK;

	return g_pFileSystem->FindIsDirectory(handle);
}

/* <265d7> ../engine/filesystem_internal.cpp:182 */
void FS_FindClose(FileFindHandle_t handle)
{
	return g_pFileSystem->FindClose(handle);
}

/* <26046> ../engine/filesystem_internal.cpp:188 */
void FS_GetLocalCopy(const char *pFileName)
{
	g_pFileSystem->GetLocalCopy(pFileName);
}

/* <25fe7> ../engine/filesystem_internal.cpp:194 */
const char *FS_GetLocalPath(const char *pFileName, char *pLocalPath, int localPathBufferSize)
{
	return g_pFileSystem->GetLocalPath(pFileName, pLocalPath, localPathBufferSize);
}

/* <2664a> ../engine/filesystem_internal.cpp:200 */
NOXREF char *FS_ParseFile(char *pFileBytes, char *pToken, int *pWasQuoted)
{
	NOXREFCHECK;

	bool wasquoted;
	char *result = g_pFileSystem->ParseFile(pFileBytes, pToken, &wasquoted);

	if (pWasQuoted)
	{
		*pWasQuoted = wasquoted;
	}

	return result;
}

/* <266ba> ../engine/filesystem_internal.cpp:214 */
NOXREF int FS_FullPathToRelativePath(const char *pFullpath, char *pRelative)
{
	NOXREFCHECK;

	return g_pFileSystem->FullPathToRelativePath(pFullpath, pRelative);
}

/* <266f3> ../engine/filesystem_internal.cpp:220 */
NOXREF int FS_GetCurrentDirectory(char *pDirectory, int maxlen)
{
	NOXREFCHECK;

	return g_pFileSystem->GetCurrentDirectory(pDirectory, maxlen);
}

/* <2672c> ../engine/filesystem_internal.cpp:226 */
NOXREF void FS_PrintOpenedFiles(void)
{
	NOXREFCHECK;

	g_pFileSystem->PrintOpenedFiles();
}

/* <26740> ../engine/filesystem_internal.cpp:232 */
NOXREF void FS_SetWarningFunc(void (*pfnWarning)(const char *, ...))
{
	NOXREFCHECK;

	g_pFileSystem->SetWarningFunc(pfnWarning);
}

/* <26779> ../engine/filesystem_internal.cpp:238 */
NOXREF void FS_SetWarningLevel(FileWarningLevel_t level)
{
	NOXREFCHECK;

	g_pFileSystem->SetWarningLevel(level);
}

/* <267a0> ../engine/filesystem_internal.cpp:245 */
NOXREF unsigned char FS_GetCharacter(FileHandle_t f)
{
	NOXREFCHECK;

	uint8_t retval;
	g_pFileSystem->Read(&retval, 1, f);
	return retval;
}

/* <2680e> ../engine/filesystem_internal.cpp:252 */
void FS_LogLevelLoadStarted(const char *name)
{
	g_pFileSystem->LogLevelLoadStarted(name);
}

/* <26835> ../engine/filesystem_internal.cpp:257 */
void FS_LogLevelLoadFinished(const char *name)
{
	g_pFileSystem->LogLevelLoadFinished(name);
}

/* <2685e> ../engine/filesystem_internal.cpp:262 */
int FS_SetVBuf(FileHandle_t stream, char *buffer, int mode, size_t size)
{
	return g_pFileSystem->SetVBuf(stream, buffer, mode, size);
}

/* <268b8> ../engine/filesystem_internal.cpp:267 */
void FS_GetInterfaceVersion(char *p, int maxlen)
{
	g_pFileSystem->GetInterfaceVersion(p, maxlen);
}

/* <268ee> ../engine/filesystem_internal.cpp:272 */
void *FS_GetReadBuffer(FileHandle_t file, int *outBufSize)
{
	return g_pFileSystem->GetReadBuffer(file, outBufSize, 1);
}

/* <2692a> ../engine/filesystem_internal.cpp:277 */
void FS_ReleaseReadBuffer(FileHandle_t file, void *buffer)
{
	g_pFileSystem->ReleaseReadBuffer(file, buffer);
}

/* <26962> ../engine/filesystem_internal.cpp:282 */
void FS_Unlink(const char *filename)
{
	char localPath[512];
	FS_GetLocalPath(filename, localPath, 512);
	_unlink(localPath);
}

/* <269e8> ../engine/filesystem_internal.cpp:292 */
void FS_Rename(const char *originalName, const char *newName)
{
	char *cut;
	char localPath[512];
	char newPath[512];

	if (FS_GetLocalPath(originalName, localPath, 512))
	{
		strcpy(newPath, localPath);
		cut = strstr(newPath, originalName);

		if (cut)
		{
			*cut = 0;
			strcat(newPath, newName);
			rename(localPath, newPath);
		}
	}
}

/* <26abf> ../engine/filesystem_internal.cpp:320 */
void *FS_LoadLibrary(const char *dllName)
{
	void *result = NULL;

	if (dllName)
	{
		FS_GetLocalCopy(dllName);
#ifdef _WIN32
		result = LoadLibraryA(dllName);
#else
		result = dlopen(dllName, 2);
#endif
	}

	return result;
}

