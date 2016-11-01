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
	m_NumHooks = 0;
}

void AbstractHookChainRegistry::addHook(void* hookFunc, int priority) {
	if (m_NumHooks >= MAX_HOOKS_IN_CHAIN) {
		rehlds_syserror("MAX_HOOKS_IN_CHAIN limit hit");
	}

	for (int i = 0; i < MAX_HOOKS_IN_CHAIN; i++)
	{
		if (m_Hooks[i] && priority <= m_Priorities[i])
			continue;

		auto swapHookFunc = m_Hooks[i];
		auto swapPriority = m_Priorities[i];

		m_Hooks[i] = hookFunc;
		m_Priorities[i] = priority;

		hookFunc = swapHookFunc;
		priority = swapPriority;
	}

	m_NumHooks++;
}

void AbstractHookChainRegistry::removeHook(void* hookFunc, int priority) {

	int bestMatch = -1;

	// Search for matching hook func address and priority or at least address match
	for (int i = 0; i < m_NumHooks; i++)
	{
		if (hookFunc == m_Hooks[i])
		{
			bestMatch = i;
			if (m_Priorities[i] == priority)
				break;
		}
	}

	if (bestMatch >= 0)
	{
		// erase hook
		--m_NumHooks;

		if (m_NumHooks != bestMatch)
		{
			Q_memmove(&m_Hooks[bestMatch], &m_Hooks[bestMatch + 1], (m_NumHooks - bestMatch) * sizeof(m_Hooks[0]));
			Q_memmove(&m_Priorities[bestMatch], &m_Priorities[bestMatch + 1], (m_NumHooks - bestMatch) * sizeof(m_Priorities[0]));

			m_Hooks[m_NumHooks] = NULL;
		}
		else
			m_Hooks[bestMatch] = NULL;
	}
}
