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

#if defined(HLTV_FIXES) || defined(LAUNCHER_FIXES)
#ifdef _WIN32

// Writes process mini dump by exception code and info.
void WriteMiniDump(unsigned int exceptionCode,
                   struct _EXCEPTION_POINTERS *exceptionInfo);

// Determines either debugger attached to a process or not.
bool HasDebuggerPresent();

// Determines either writing mini dumps is enabled or not.  Same as in Source games.
bool IsWriteMiniDumpsEnabled(const char *commandLine);

// Catches and writes out any exception thrown by the specified function.
template <typename F>
int CatchAndWriteMiniDump(F function, const char *commandLine)
{
	// Don't mask exceptions when mini dumps disabled (-nominidumps) or running in the debugger.
	if (!IsWriteMiniDumpsEnabled(commandLine) || HasDebuggerPresent())
	{
		return function();
	}

	__try
	{
		return function();
	} __except (WriteMiniDump(GetExceptionCode(), GetExceptionInformation()),
				EXCEPTION_EXECUTE_HANDLER)
	{
		// Write the minidump from inside the filter.
		return GetExceptionCode();
	}
}

#endif  // _WIN32
#endif  // HLTV_FIXES || LAUNCHER_FIXES
