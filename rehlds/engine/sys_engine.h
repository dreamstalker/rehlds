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
#include "igame.h"
#include "iengine.h"

// sleep time when not focus
#define NOT_FOCUS_SLEEP	50
#define MINIMIZED_SLEEP 20

#ifdef HOOK_ENGINE
#define game (*pgame)
#define eng (*peng)
#endif // HOOK_ENGINE

extern IGame* game;
extern IEngine* eng;


/* <95955> ../engine/sys_engine.cpp:92 */
class CEngine : public IEngine
{
private:

	int                        m_nQuitting;          /*     4     4 */
	int                        m_nDLLState;          /*     8     4 */
	int                        m_nSubState;          /*    12     4 */
	double                     m_fCurTime;           /*    16     8 */
	double                     m_fFrameTime;         /*    24     8 */
	double                     m_fOldTime;           /*    32     8 */
	bool                       m_bTrapMode;          /*    40     1 */
	bool                       m_bDoneTrapping;      /*    41     1 */
	int                        m_nTrapKey;           /*    44     4 */
	int                        m_nTrapButtons;       /*    48     4 */

public:

	CEngine();
	virtual ~CEngine();

	virtual bool Load(bool dedicated, char *rootDir, char *cmdLine);
	bool Load_noVirt(bool dedicated, char *rootDir, char *cmdLine);

	virtual void Unload();
	void Unload_noVirt();

	virtual void SetState(int iState);
	void SetState_noVirt(int iState);

	virtual int GetState();
	int GetState_noVirt();

	virtual void SetSubState(int iSubstate);
	void SetSubState_noVirt(int iSubstate);

	virtual int GetSubState();
	int GetSubState_noVirt();

	virtual int Frame();
	int Frame_noVirt();

	virtual double GetFrameTime();
	double GetFrameTime_noVirt();

	virtual double GetCurTime();
	double GetCurTime_noVirt();

	virtual void TrapKey_Event(int key, bool down);
	void TrapKey_Event_noVirt(int key, bool down);

	virtual void TrapMouse_Event(int buttons, bool down);
	void TrapMouse_Event_noVirt(int buttons, bool down);

	virtual void StartTrapMode();
	void StartTrapMode_noVirt();

	virtual bool IsTrapping();
	bool IsTrapping_noVirt();

	virtual bool CheckDoneTrapping(int& buttons, int& keys);
	bool CheckDoneTrapping_noVirt(int& buttons, int& keys);

	virtual int GetQuitting();
	int GetQuitting_noVirt();

	virtual void SetQuitting(int quittype);
	void SetQuitting_noVirt(int quittype);

	/* vtable has 16 entries: {
	[2] = Load(_ZN7CEngine4LoadEbPcS0_),
	[3] = Unload(_ZN7CEngine6UnloadEv),
	[4] = SetState(_ZN7CEngine8SetStateEi),
	[5] = GetState(_ZN7CEngine8GetStateEv),
	[6] = SetSubState(_ZN7CEngine11SetSubStateEi),
	[7] = GetSubState(_ZN7CEngine11GetSubStateEv),
	[8] = Frame(_ZN7CEngine5FrameEv),
	[9] = GetFrameTime(_ZN7CEngine12GetFrameTimeEv),
	[10] = GetCurTime(_ZN7CEngine10GetCurTimeEv),
	[11] = TrapKey_Event(_ZN7CEngine13TrapKey_EventEib),
	[12] = TrapMouse_Event(_ZN7CEngine15TrapMouse_EventEib),
	[13] = StartTrapMode(_ZN7CEngine13StartTrapModeEv),
	[14] = IsTrapping(_ZN7CEngine10IsTrappingEv),
	[15] = CheckDoneTrapping(_ZN7CEngine17CheckDoneTrappingERiS0_),
	[16] = GetQuitting(_ZN7CEngine11GetQuittingEv),
	[17] = SetQuitting(_ZN7CEngine11SetQuittingEi),
	} */
	/* size: 52, cachelines: 1, members: 11 */
	/* last cacheline: 52 bytes */

	/* BRAIN FART ALERT! 52 != 50 + 0(holes), diff = 2 */

}; /* size: 52 */
