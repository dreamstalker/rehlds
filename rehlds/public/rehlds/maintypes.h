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

#ifndef MAINTYPES_H
#define MAINTYPES_H
#ifdef _WIN32
#pragma once
#endif

#include "osconfig.h"
#include "mathlib.h"

// Has no references on server side.
#define NOXREF
// Function body is not implemented.
#define NOBODY
// Function is not tested at all.
#define UNTESTED

#define CONST_INTEGER_AS_STRING(x) #x //Wraps the integer in quotes, allowing us to form constant strings with it
#define __HACK_LINE_AS_STRING__(x) CONST_INTEGER_AS_STRING(x) //__LINE__ can only be converted to an actual number by going through this, otherwise the output is literally "__LINE__"
#define __LINE__AS_STRING __HACK_LINE_AS_STRING__(__LINE__) //Gives you the line number in constant string form

#if defined _MSC_VER || defined __INTEL_COMPILER
#define NOXREFCHECK			int __retAddr; __asm { __asm mov eax, [ebp + 4] __asm mov __retAddr, eax }; Sys_Error("[NOXREFCHECK]: %s: (" __FILE__ ":" __LINE__AS_STRING ") NOXREF, but called from 0x%.08x", __func__, __retAddr)
#else
// For EBP based stack (older gcc) (uncomment version apropriate for your compiler)
//#define NOXREFCHECK			int __retAddr; __asm__ __volatile__("movl 4(%%ebp), %%eax;" "movl %%eax, %0":"=r"(__retAddr)::"%eax"); Sys_Error("[NOXREFCHECK]: %s: (" __FILE__ ":" __LINE__AS_STRING ") NOXREF, but called from 0x%.08x", __func__, __retAddr);
// For ESP based stack (newer gcc) (uncomment version apropriate for your compiler)
#define NOXREFCHECK			int __retAddr; __asm__ __volatile__("movl 16(%%esp), %%eax;" "movl %%eax, %0":"=r"(__retAddr)::"%eax"); Sys_Error("[NOXREFCHECK]: %s: (" __FILE__ ":" __LINE__AS_STRING ") NOXREF, but called from 0x%.08x", __func__, __retAddr);
#endif

#define BIT(n) (1<<(n))

// From engine/pr_comp.h;
typedef unsigned int string_t;

// From engine/server.h
typedef enum sv_delta_s
{
	sv_packet_nodelta,
	sv_packet_delta,
} sv_delta_t;

#endif // MAINTYPES_H
