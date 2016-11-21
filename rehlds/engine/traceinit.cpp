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

CInitTracker g_InitTracker;

CInitTracker::CInitTracker(void)
{
	for (int l = 0; l < NUM_LISTS; l++)
	{
		m_Funcs[l].RemoveAll();
		m_nNumFuncs[l] = 0;
	}
}

CInitTracker::~CInitTracker(void)
{
	for (int l = 0; l < NUM_LISTS; l++)
	{
		for (int i = 0; i < m_nNumFuncs[l]; i++)
		{
			InitFunc *f = m_Funcs[l][i];
			if (f->referencecount)
			{
				Sys_Printf("Missing shutdown function for %s : %s\n", f->initname, f->shutdownname);
			}
			delete f;
		}
		m_Funcs[l].RemoveAll();
		m_nNumFuncs[l] = 0;
	}
}

void CInitTracker::Init(const char *init, const char *shutdown, int listnum)
{
	InitFunc *f = new InitFunc;

	f->initname = init;
	f->shutdownname = shutdown;
	f->inittime = 0.0f;
	f->referencecount = 1;
	f->shutdowntime = 0.0f;
	f->sequence = m_nNumFuncs[listnum];
	f->warningprinted = false;

	m_Funcs[listnum].AddToHead(f);
	m_nNumFuncs[listnum]++;
}

void CInitTracker::Shutdown(const char *shutdown, int listnum)
{
	int i = 0;
	InitFunc *f = NULL;

	if (!m_nNumFuncs[listnum])
	{
		Sys_Printf("Mismatched shutdown function %s\n", shutdown);
		return;
	}
	for (i = 0; i < m_nNumFuncs[listnum]; i++)
	{
		f = m_Funcs[listnum][i];
		if (f->referencecount)
			break;
	}
	if (f && f->referencecount && Q_stricmp(f->shutdownname, shutdown))
	{
		if (!f->warningprinted)
			f->warningprinted = true;
	}
	for (i = 0; i < m_nNumFuncs[listnum]; i++)
	{
		InitFunc *f = m_Funcs[listnum][i];
		if (!Q_stricmp(f->shutdownname, shutdown))
		{
			f->referencecount--;
			return;
		}
	}
	Sys_Printf("Shutdown function %s not in list!!!\n", shutdown);
}

void TraceInit(const char *i, const char *s, int listnum)
{
	g_InitTracker.Init(i, s, listnum);
}

void TraceShutdown(const char *s, int listnum)
{
	g_InitTracker.Shutdown(s, listnum);
}
