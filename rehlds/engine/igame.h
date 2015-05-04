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

//Restored  params names from hl2beta https://github.com/RaisingTheDerp/raisingthebar/
/* <8f8da> ../engine/igame.h:26 */
class IGame {
public:

	/* <8f8f7> ../engine/igame.h:29 */
	virtual ~IGame() { }

	/* <8f91a> ../engine/igame.h:31 */
	virtual bool Init(void *pvInstance) = 0;

	/* <8f947> ../engine/igame.h:32 */
	virtual bool Shutdown() = 0;

	/* <8f96f> ../engine/igame.h:34 */
	virtual bool CreateGameWindow() = 0;

	/* <8f997> ../engine/igame.h:36 */
	virtual void SleepUntilInput(int time) = 0;

	/* <8f9c0> ../engine/igame.h:38 */
	virtual HWND GetMainWindow() = 0;

	/* <8f9e8> ../engine/igame.h:39 */
	virtual HWND * GetMainWindowAddress() = 0;

	/* <8fa10> ../engine/igame.h:41 */
	virtual void SetWindowXY(int x, int y) = 0;

	/* <8fa3e> ../engine/igame.h:42 */
	virtual void SetWindowSize(int w, int h) = 0;

	/* <8fa6c> ../engine/igame.h:44 */
	virtual void GetWindowRect(int *x, int *y, int *w, int *h) = 0;

	/* <8faa4> ../engine/igame.h:47 */
	virtual bool IsActiveApp() = 0;

	/* <8facc> ../engine/igame.h:49 */
	virtual bool IsMultiplayer() = 0;

	/* <8faf4> ../engine/igame.h:51 */
	virtual void PlayStartupVideos() = 0;

	/* <8fb18> ../engine/igame.h:52 */
	virtual void PlayAVIAndWait(const char *aviFile) = 0;

	/* <8fb41> ../engine/igame.h:54 */
	virtual void SetCursorVisible(bool vis) = 0;

	/* vtable has 14 entries: {
	[2] = Init(_ZN5IGame4InitEPv),
	[3] = Shutdown(_ZN5IGame8ShutdownEv),
	[4] = CreateGameWindow(_ZN5IGame16CreateGameWindowEv),
	[5] = SleepUntilInput(_ZN5IGame15SleepUntilInputEi),
	[6] = GetMainWindow(_ZN5IGame13GetMainWindowEv),
	[7] = GetMainWindowAddress(_ZN5IGame20GetMainWindowAddressEv),
	[8] = SetWindowXY(_ZN5IGame11SetWindowXYEii),
	[9] = SetWindowSize(_ZN5IGame13SetWindowSizeEii),
	[10] = GetWindowRect(_ZN5IGame13GetWindowRectEPiS0_S0_S0_),
	[11] = IsActiveApp(_ZN5IGame11IsActiveAppEv),
	[12] = IsMultiplayer(_ZN5IGame13IsMultiplayerEv),
	[13] = PlayStartupVideos(_ZN5IGame17PlayStartupVideosEv),
	[14] = PlayAVIAndWait(_ZN5IGame14PlayAVIAndWaitEPKc),
	[15] = SetCursorVisible(_ZN5IGame16SetCursorVisibleEb),
	} */
	/* size: 4, cachelines: 1, members: 1 */
	/* last cacheline: 4 bytes */
}; /* size: 4 */
