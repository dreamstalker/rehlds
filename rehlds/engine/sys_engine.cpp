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

#include "precompiled.h"

CEngine g_Engine;
IEngine *eng = &g_Engine;

CEngine::CEngine()
{
	m_fFrameTime = 0.0;
	m_nSubState = 0;
	m_nDLLState = DLL_INACTIVE;
	m_fOldTime = 0.0;
	m_bTrapMode = false;
	m_bDoneTrapping = false;
	m_nTrapKey = 0;
	m_nTrapButtons = 0;
	m_nQuitting = QUIT_NOTQUITTING;

#ifdef REHLDS_FIXES
	m_fCurTime = 0.0;
#endif
}

CEngine::~CEngine()
{
}

void CEngine::Unload()
{
	Sys_ShutdownGame();
	m_nDLLState = DLL_INACTIVE;
}

void ForceReloadProfile()
{
	Cbuf_AddText("exec config.cfg\n");
	Cbuf_AddText("+mlook\n");
	Cbuf_Execute();
	if (COM_CheckParm("-nomousegrab"))
		Cvar_Set("cl_mousegrab", "0");

	//Key_SetBinding(126, "toggleconsole");
	//Key_SetBinding(96, "toggleconsole");
	//Key_SetBinding(27, "cancelselect");
	//SDL_GL_SetSwapInterval((gl_vsync.value <= 0.0) - 1);
	if (g_pcls.state != ca_dedicated)
	{
		Sys_Error("%s: Only dedicated mode is supported", __func__);
		/*
		v0 = GetRateRegistrySetting(rate.string);
		Q_strncpy(szRate, v0, 0x20u);
		Cvar_DirectSet(&rate, szRate);
		*/
	}
}

bool CEngine::Load(bool dedicated, char *basedir, char *cmdline)
{
	bool success = false;
	SetState(DLL_ACTIVE);
	if (Sys_InitGame(cmdline, basedir, game->GetMainWindowAddress(), dedicated))
	{
		success = true;
#ifdef _WIN32
		ForceReloadProfile();
#endif // _WIN32
	}
	return success;
}

int CEngine::Frame()
{
#ifndef SWDS
	(*(void(**)(void))(*(_DWORD *)cdaudio + 24))();
#endif // SWDS

	if (!game->IsActiveApp())
		game->SleepUntilInput(m_nDLLState != DLL_PAUSED ? MINIMIZED_SLEEP : NOT_FOCUS_SLEEP);

	m_fCurTime = Sys_FloatTime();
	m_fFrameTime = m_fCurTime - m_fOldTime;
	m_fOldTime = m_fCurTime;
	if (m_fFrameTime < 0.0)
	{
		m_fFrameTime = 0.001;
	}

	if (m_nDLLState == DLL_INACTIVE)
		return m_nDLLState;

	int dummy;
	int iState = Host_Frame(m_fFrameTime, m_nDLLState, &dummy);
	if (iState == m_nDLLState)
		return m_nDLLState;

	SetState(iState);
	if (m_nDLLState == DLL_CLOSE)
	{
		SetQuitting(QUIT_TODESKTOP);
	}
	else if (m_nDLLState == DLL_RESTART)
	{
		SetQuitting(QUIT_RESTART);
	}

	return m_nDLLState;
}

void CEngine::SetSubState(int iSubState)
{
	if (iSubState != 1)
		GameSetSubState(iSubState);
}

void CEngine::SetState(int iState)
{
	m_nDLLState = iState;
	GameSetState(iState);
}

int CEngine::GetState()
{
	return m_nDLLState;
}

int CEngine::GetSubState()
{
	return m_nSubState;
}

double CEngine::GetFrameTime()
{
	return m_fFrameTime;
}

double CEngine::GetCurTime()
{
	return m_fCurTime;
}

void CEngine::TrapKey_Event(int key, bool down)
{
}

void CEngine::TrapMouse_Event(int buttons, bool down)
{
	if (m_bTrapMode && buttons && !down)
	{
		m_bTrapMode = false;
		m_bDoneTrapping = true;
		m_nTrapKey = 0;
		m_nTrapButtons = buttons;
	}
	else
	{
		ClientDLL_MouseEvent(buttons);
	}
}

void CEngine::StartTrapMode()
{
	if (!m_bTrapMode)
	{
		m_bDoneTrapping = false;
		m_bTrapMode = true;
	}
}

bool CEngine::IsTrapping()
{
	return m_bTrapMode;
}

bool CEngine::CheckDoneTrapping(int & buttons, int & key)
{
	if (m_bTrapMode)
	{
		return false;
	}
	else if (m_bDoneTrapping)
	{
		m_bDoneTrapping = false;
		key = m_nTrapKey;
		buttons = m_nTrapButtons;
		return true;
	}
	else
	{
		return false;
	}
}

void CEngine::SetQuitting(int quittype)
{
	m_nQuitting = quittype;
}

int CEngine::GetQuitting()
{
	return m_nQuitting;
}
