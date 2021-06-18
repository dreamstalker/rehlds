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

class IEngine
{
public:
	enum
	{
		QUIT_NOTQUITTING = 0,
		QUIT_TODESKTOP,
		QUIT_RESTART
	};

	virtual ~IEngine() {}

	virtual bool Load(bool dedicated, const char *basedir, const char *cmdline) = 0;
	virtual void Unload() = 0;
	virtual void SetState(int iState) = 0;
	virtual int GetState() = 0;
	virtual void SetSubState(int iSubState) = 0;
	virtual int GetSubState() = 0;

	virtual int Frame() = 0;
	virtual double GetFrameTime() = 0;
	virtual double GetCurTime() = 0;

	virtual void TrapKey_Event(int key, bool down) = 0;
	virtual void TrapMouse_Event(int buttons, bool down) = 0;

	virtual void StartTrapMode() = 0;
	virtual bool IsTrapping() = 0;
	virtual bool CheckDoneTrapping(int &buttons, int &key) = 0;

	virtual int GetQuitting() = 0;
	virtual void SetQuitting(int quittype) = 0;
};

extern IEngine *eng;
