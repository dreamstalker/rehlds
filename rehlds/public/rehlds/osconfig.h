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

#ifndef _OSCONFIG_H
#define _OSCONFIG_H

#ifdef _WIN32 // WINDOWS
	#pragma warning(disable : 4005)
#endif // _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <setjmp.h>
#include <assert.h>

#include <algorithm>
#include <deque>
#include <functional>

#ifdef _WIN32 // WINDOWS
	#include <windows.h>
	#include <winsock.h>
	#include <wsipx.h> // for support IPX
	#define PSAPI_VERSION 1
	#include <psapi.h>
	#include <nmmintrin.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <io.h>
#else // _WIN32
	#include <arpa/inet.h>
	#include <ctype.h>
	//#include <dirent.h>
	#include <dlfcn.h>
	#include <elf.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <limits.h>
	#include <link.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <pthread.h>
	#include <sys/ioctl.h>
	#include <sys/mman.h>
	#include <sys/socket.h>
	#include <sys/stat.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/sysinfo.h>
	#include <unistd.h>

	// Deail with stupid macro in kernel.h
	#undef __FUNCTION__
#endif // _WIN32

#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <smmintrin.h>
#include <xmmintrin.h>

#ifdef _WIN32 // WINDOWS
	#define _CRT_SECURE_NO_WARNINGS
	#define WIN32_LEAN_AND_MEAN

	#ifndef CDECL
		#define CDECL __cdecl
	#endif
	#define STDCALL __stdcall
	#define HIDDEN
	#define NOINLINE __declspec(noinline)
	#define ALIGN16 __declspec(align(16))
	#define NORETURN __declspec(noreturn)
	#define FORCE_STACK_ALIGN

	//inline bool SOCKET_FIONBIO(SOCKET s, int m) { return (ioctlsocket(s, FIONBIO, (u_long*)&m) == 0); }
	//inline int SOCKET_MSGLEN(SOCKET s, u_long& r) { return ioctlsocket(s, FIONREAD, (u_long*)&r); }
	typedef int socklen_t;
	#define SOCKET_FIONBIO(s, m) ioctlsocket(s, FIONBIO, (u_long*)&m)
	#define SOCKET_MSGLEN(s, r) ioctlsocket(s, FIONREAD, (u_long*)&r)
	#define SIN_GET_ADDR(saddr, r) r = (saddr)->S_un.S_addr
	#define SIN_SET_ADDR(saddr, r) (saddr)->S_un.S_addr = (r)
	#define SOCKET_CLOSE(s) closesocket(s)
	#define SOCKET_AGAIN() (WSAGetLastError() == WSAEWOULDBLOCK)

	inline void* sys_allocmem(unsigned int size) {
		return VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
	}

	inline void sys_freemem(void* ptr, unsigned int size) {
		VirtualFree(ptr, 0, MEM_RELEASE);
	}
#else // _WIN32
	#ifdef __FUNCTION__
		#undef __FUNCTION__
	#endif
	#define __FUNCTION__ __func__

	#ifndef PAGESIZE
		#define PAGESIZE 4096
	#endif
	#define ALIGN(addr) (size_t)((size_t)addr & ~(PAGESIZE-1))
	#define ARRAYSIZE(p) (sizeof(p)/sizeof(p[0]))

	#define _MAX_FNAME NAME_MAX
	#define MAX_PATH 260

	typedef void *HWND;

	typedef unsigned long DWORD;
	typedef unsigned short WORD;
	typedef unsigned int UNINT32;

	#define CDECL __attribute__ ((cdecl))
	#define STDCALL __attribute__ ((stdcall))
	#define HIDDEN __attribute__((visibility("hidden")))
	#define NOINLINE __attribute__((noinline))
	#define ALIGN16 __attribute__((aligned(16)))
	#define NORETURN __attribute__((noreturn))
	#define FORCE_STACK_ALIGN __attribute__((force_align_arg_pointer))

	//inline bool SOCKET_FIONBIO(SOCKET s, int m) { return (ioctl(s, FIONBIO, (int*)&m) == 0); }
	//inline int SOCKET_MSGLEN(SOCKET s, u_long& r) { return ioctl(s, FIONREAD, (int*)&r); }
	typedef int SOCKET;
	#define INVALID_SOCKET (SOCKET)(~0)
	#define SOCKET_FIONBIO(s, m) ioctl(s, FIONBIO, (char*)&m)
	#define SOCKET_MSGLEN(s, r) ioctl(s, FIONREAD, (char*)&r)
	#define SIN_GET_ADDR(saddr, r) r = (saddr)->s_addr
	#define SIN_SET_ADDR(saddr, r) (saddr)->s_addr = (r)
	#define SOCKET_CLOSE(s) close(s)
	#define SOCKET_AGAIN() (errno == EAGAIN)
	#define SOCKET_ERROR -1

	inline void* sys_allocmem(unsigned int size) {
		return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	}
	inline void sys_freemem(void* ptr, unsigned int size) {
		munmap(ptr, size);
	}

	#define WSAENOPROTOOPT ENOPROTOOPT

	#ifndef FALSE
	#define FALSE	0
	#endif
	#ifndef TRUE
	#define TRUE	1
	#endif
#endif // _WIN32

#ifdef _WIN32
	static const bool __isWindows = true;
	static const bool __isLinux = false;
#else
	static const bool __isWindows = false;
	static const bool __isLinux = true;
#endif

#define EXT_FUNC FORCE_STACK_ALIGN

#endif // _OSCONFIG_H
