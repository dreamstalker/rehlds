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
*/

#include "precompiled.h"
#include "hookchains_impl.h"

AbstractHookChainRegistry::AbstractHookChainRegistry()
{
	Q_memset(m_Hooks, 0, sizeof(m_Hooks));
	Q_memset(m_Priorities, 0, sizeof(m_Priorities));

	m_NumHooks = 0;
}

void AbstractHookChainRegistry::addHook(void* hookFunc, int priority) {
	for (int i = 0; i < MAX_HOOKS_IN_CHAIN; i++)
	{
		if (m_Hooks[i] == hookFunc)
			return;

		if (m_Hooks[i] && priority <= m_Priorities[i])
			continue;

		auto swapHookFunc = m_Hooks[i];
		auto swapPriority = m_Priorities[i];

		m_Hooks[i] = hookFunc;
		m_Priorities[i] = priority;

		hookFunc = swapHookFunc;
		priority = swapPriority;
	}

	if (m_NumHooks >= MAX_HOOKS_IN_CHAIN) {
		rehlds_syserror("MAX_HOOKS_IN_CHAIN limit hit");
	}

	m_NumHooks++;
}

void AbstractHookChainRegistry::removeHook(void* hookFunc) {

	// erase hook
	for (int i = 0; i < m_NumHooks; i++)
	{
		--m_NumHooks;
		if (hookFunc == m_Hooks[i])
		{
			if (m_NumHooks != i)
			{
				Q_memmove(&m_Hooks[i], &m_Hooks[i + 1], (m_NumHooks - i) * sizeof(m_Hooks[0]));
				Q_memmove(&m_Priorities[i], &m_Priorities[i + 1], (m_NumHooks - i) * sizeof(m_Priorities[0]));
				m_Hooks[m_NumHooks] = NULL;
			}
			else
				m_Hooks[i] = NULL;

			return;
		}
	}
}
