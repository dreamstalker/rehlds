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

#ifndef IENGINE_H
#define IENGINE_H
#ifdef _WIN32
#pragma once
#endif

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

	virtual			~IEngine(void) { }

	virtual bool	Load(bool dedicated, char *basedir, char *cmdline) = 0;
	virtual void	Unload(void) = 0;
	virtual void	SetState(int iState) = 0;
	virtual int		GetState(void) = 0;
	virtual void	SetSubState(int iSubState) = 0;
	virtual int		GetSubState(void) = 0;

	virtual int		Frame(void) = 0;
	virtual double	GetFrameTime(void) = 0;
	virtual double	GetCurTime(void) = 0;

	virtual void	TrapKey_Event(int key, bool down) = 0;
	virtual void	TrapMouse_Event(int buttons, bool down) = 0;

	virtual void	StartTrapMode(void) = 0;
	virtual bool	IsTrapping(void) = 0;
	virtual bool	CheckDoneTrapping(int& buttons, int& key) = 0;

	virtual int		GetQuitting(void) = 0;
	virtual void	SetQuitting(int quittype) = 0;
};

extern IEngine *eng;

///* <8f12e> ../engine/iengine.h:39 */
//virtual void ~IEngine(class IEngine *, int)
//{
//} /* size: 165463000 */
//
///* <8f151> ../engine/iengine.h:41 */
//virtual bool Load(class IEngine *, bool, char *, char *) /* linkage=_ZN7IEngine4LoadEbPcS0_ */
//{
//} /* size: 1 */
//
///* <8f188> ../engine/iengine.h:42 */
//virtual void Unload(class IEngine *) /* linkage=_ZN7IEngine6UnloadEv */
//{
//} /* size: 0 */
//
///* <8f1ac> ../engine/iengine.h:43 */
//virtual void SetState(class IEngine *, int) /* linkage=_ZN7IEngine8SetStateEi */
//{
//} /* size: 165479164 */
//
///* <8f1d5> ../engine/iengine.h:44 */
//virtual int GetState(class IEngine *) /* linkage=_ZN7IEngine8GetStateEv */
//{
//} /* size: 4 */
//
///* <8f1fd> ../engine/iengine.h:45 */
//virtual void SetSubState(class IEngine *, int) /* linkage=_ZN7IEngine11SetSubStateEi */
//{
//} /* size: 165463956 */
//
///* <8f226> ../engine/iengine.h:46 */
//virtual int GetSubState(class IEngine *) /* linkage=_ZN7IEngine11GetSubStateEv */
//{
//} /* size: 4 */
//
///* <8f24e> ../engine/iengine.h:47 */
//virtual int Frame(class IEngine *) /* linkage=_ZN7IEngine5FrameEv */
//{
//} /* size: 4 */
//
///* <8f276> ../engine/iengine.h:49 */
//virtual double GetFrameTime(class IEngine *) /* linkage=_ZN7IEngine12GetFrameTimeEv */
//{
//} /* size: 8 */
//
///* <8f29e> ../engine/iengine.h:50 */
//virtual double GetCurTime(class IEngine *) /* linkage=_ZN7IEngine10GetCurTimeEv */
//{
//} /* size: 8 */
//
///* <8f2c6> ../engine/iengine.h:52 */
//virtual void TrapKey_Event(class IEngine *, int, bool) /* linkage=_ZN7IEngine13TrapKey_EventEib */
//{
//} /* size: 0 */
//
///* <8f2f4> ../engine/iengine.h:53 */
//virtual void TrapMouse_Event(class IEngine *, int, bool) /* linkage=_ZN7IEngine15TrapMouse_EventEib */
//{
//} /* size: 773 */
//
///* <8f322> ../engine/iengine.h:55 */
//virtual void StartTrapMode(class IEngine *) /* linkage=_ZN7IEngine13StartTrapModeEv */
//{
//} /* size: 2865 */
//
///* <8f346> ../engine/iengine.h:56 */
//virtual bool IsTrapping(class IEngine *) /* linkage=_ZN7IEngine10IsTrappingEv */
//{
//} /* size: 1 */
//
///* <8f36e> ../engine/iengine.h:57 */
//virtual bool CheckDoneTrapping(class IEngine *, int &, int &) /* linkage=_ZN7IEngine17CheckDoneTrappingERiS0_ */
//{
//} /* size: 1 */
//
///* <8f3a0> ../engine/iengine.h:59 */
//virtual int GetQuitting(class IEngine *) /* linkage=_ZN7IEngine11GetQuittingEv */
//{
//} /* size: 4 */
//
///* <8f3c8> ../engine/iengine.h:60 */
//virtual void SetQuitting(class IEngine *, int) /* linkage=_ZN7IEngine11SetQuittingEi */
//{
//} /* size: 165481496 */
//
///* <961fe> ../engine/iengine.h:29 */
//inline void IEngine(class IEngine *const this)
//{
//} /* size: 167122024 */
//
///* <8fcbf> ../engine/iengine.h:39 */
//inline void ~IEngine(class IEngine *const this, intconst __in_chrg)
//{
//} /* size: 768 */
//
///* <96256> ../engine/iengine.h:39 */
//void ~IEngine(class IEngine *const this)
//{
//} /* size: 167122528 */
//
///* <965c9> ../engine/iengine.h:39 */
//void ~IEngine(class IEngine *const this)
//{
//	~IEngine(class IEngine *const this,
//			 intconst __in_chrg); /* size=167350372, low_pc=0 */ //    39
//} /* size: 167350468 */

#endif // IENGINE_H
