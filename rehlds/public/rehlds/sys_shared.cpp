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
#include "sys_shared.h"

bool Sys_ChechSSE42Support();

bool g_HasSSE42 = Sys_ChechSSE42Support();


bool Sys_ChechSSE42Support() {
	unsigned int cpuid_data[4];

#if defined(__GNUC__)
	__get_cpuid(0x1, &cpuid_data[0], &cpuid_data[1], &cpuid_data[2], &cpuid_data[3]);
#else //__GNUC__
	__cpuidex((int*)cpuid_data, 1, 0);
#endif //__GNUC__

	return (0 != (cpuid_data[2] & (1 << 20)));
}
