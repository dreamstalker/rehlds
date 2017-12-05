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

#include "BaseFileSystem.h"
#include "linux_support.h"

class CFileSystem_Stdio: public CBaseFileSystem
{
public:
	CFileSystem_Stdio();
	virtual ~CFileSystem_Stdio();

	// Mount and unmount the filesystem
	virtual void Mount();
	virtual void Unmount();

	virtual void GetLocalCopy(const char *pFileName);

	virtual void LogLevelLoadStarted(const char *name);
	virtual void LogLevelLoadFinished(const char *name);
	virtual int HintResourceNeed(const char *hintlist, int forgetEverything);
	virtual int PauseResourcePreloading();
	virtual int ResumeResourcePreloading();
	virtual int SetVBuf(FileHandle_t stream, char *buffer, int mode, long size);
	virtual void GetInterfaceVersion(char *p, int maxlen);

	// starts waiting for resources to be available
	// returns FILESYSTEM_INVALID_HANDLE if there is nothing to wait on
	virtual WaitForResourcesHandle_t WaitForResources(const char *resourcelist);

	// get progress on waiting for resources; progress is a float [0, 1], complete is true on the waiting being done
	// returns false if no progress is available
	// any calls after complete is true or on an invalid handle will return false, 0.0f, true
	virtual bool GetWaitForResourcesProgress(WaitForResourcesHandle_t handle, float *progress /* out */ , bool *complete /* out */);

	// cancels a progress call
	virtual void CancelWaitForResources(WaitForResourcesHandle_t handle);

	// returns true if the appID has all its caches fully preloaded
	virtual bool IsAppReadyForOfflinePlay(int appID);

protected:
	// implementation of CBaseFileSystem virtual functions
	virtual FILE *FS_fopen(const char *filename, const char *options, bool bFromCache = false);
	virtual void FS_fclose(FILE *fp);
	virtual void FS_fseek(FILE *fp, int64_t pos, FileSystemSeek_t seekType);
	virtual size_t FS_ftell(FILE *fp);
	virtual int FS_feof(FILE *fp);
	virtual size_t FS_fread(void *dest, size_t count, size_t size, FILE *fp);
	virtual size_t FS_fwrite(const void *src, size_t count, size_t size, FILE *fp);
	virtual size_t FS_vfprintf(FILE *fp, const char *fmt, va_list list);
	virtual int FS_ferror(FILE *fp);
	virtual int FS_fflush(FILE *fp);
	virtual char *FS_fgets(char *dest, int destSize, FILE *fp);
	virtual int FS_stat(const char *path, struct _stat *buf);
	virtual HANDLE FS_FindFirstFile(char *findname, WIN32_FIND_DATA *dat);
	virtual bool FS_FindNextFile(HANDLE handle, WIN32_FIND_DATA *dat);
	virtual bool FS_FindClose(HANDLE handle);
	virtual bool IsThreadSafe();
	virtual bool IsFileImmediatelyAvailable(const char *pFileName);

private:
	bool m_bMounted;
};
