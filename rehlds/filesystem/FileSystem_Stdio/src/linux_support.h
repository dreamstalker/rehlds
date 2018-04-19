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

#ifndef _WIN32

#include <ctype.h>		// tolower()
#include <limits.h>		// PATH_MAX define
#include <string.h>		// strcmp, strcpy
#include <sys/stat.h>	// stat()
#include <unistd.h>
#include <dirent.h>		// scandir()
#include <stdlib.h>
#include <stdio.h>

#define FILE_ATTRIBUTE_DIRECTORY S_IFDIR

typedef struct
{
	// public data
	int dwFileAttributes;
	char cFileName[MAX_PATH]; // the file name returned from the call

	int numMatches;
	struct dirent **namelist;
	char cDir[MAX_PATH];
} FIND_DATA;

#define WIN32_FIND_DATA FIND_DATA

int FindFirstFile(char *findName, FIND_DATA *dat);
bool FindNextFile(int handle, FIND_DATA *dat);
bool FindClose(int handle);
const char *findFileInDirCaseInsensitive(const char *file);

#endif // _WIN32
