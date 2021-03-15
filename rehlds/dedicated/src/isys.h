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

class ISys {
public:
	virtual ~ISys() {}

	virtual	void Sleep(int msec) = 0;
	virtual bool GetExecutableName(char *out) = 0;
	virtual void ErrorMessage(int level, const char *msg) = 0;

	virtual void WriteStatusText(const char *szText) = 0;
	virtual void UpdateStatus(int force) = 0;

	virtual long LoadLibrary(const char *lib) = 0;
	virtual void FreeLibrary(long library) = 0;

	virtual bool CreateConsoleWindow(void) = 0;
	virtual void DestroyConsoleWindow(void) = 0;

	virtual void ConsoleOutput(const char *string) = 0;
	virtual const char *ConsoleInput(void) = 0;
	virtual void Printf(const char *fmt, ...) = 0;
};

extern ISys *sys;
