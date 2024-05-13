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
#include "hookchains.h"

const int MAX_HOOKS_IN_CHAIN = 19;

// Implementation for chains in modules
template<typename t_ret, typename ...t_args>
class IHookChainImpl : public IHookChain<t_ret, t_args...> {
public:
	typedef t_ret(*hookfunc_t)(IHookChain<t_ret, t_args...>*, t_args...);
	typedef t_ret(*origfunc_t)(t_args...);

	IHookChainImpl(void** hooks, origfunc_t orig) : m_Hooks(hooks), m_OriginalFunc(orig)
	{
		if (orig == NULL)
			Sys_Error("%s: Non-void HookChain without original function.", __func__);
	}

	virtual ~IHookChainImpl() {}

	EXT_FUNC virtual t_ret callNext(t_args... args) {
		hookfunc_t nexthook = (hookfunc_t)m_Hooks[0];

		if (nexthook)
		{
			IHookChainImpl nextChain(m_Hooks + 1, m_OriginalFunc);
			return nexthook(&nextChain, args...);
		}

		return m_OriginalFunc(args...);
	}

	EXT_FUNC virtual t_ret callOriginal(t_args... args) {
		return m_OriginalFunc(args...);
	}

private:
	void** m_Hooks;
	origfunc_t m_OriginalFunc;
};

// Implementation for void chains in modules
template<typename ...t_args>
class IVoidHookChainImpl : public IVoidHookChain<t_args...> {
public:
	typedef void(*hookfunc_t)(IVoidHookChain<t_args...>*, t_args...);
	typedef void(*origfunc_t)(t_args...);

	IVoidHookChainImpl(void** hooks, origfunc_t orig) : m_Hooks(hooks), m_OriginalFunc(orig) {}
	virtual ~IVoidHookChainImpl() {}

	EXT_FUNC virtual void callNext(t_args... args) {
		hookfunc_t nexthook = (hookfunc_t)m_Hooks[0];

		if (nexthook)
		{
			IVoidHookChainImpl nextChain(m_Hooks + 1, m_OriginalFunc);
			nexthook(&nextChain, args...);
		}
		else
		{
			if (m_OriginalFunc)
				m_OriginalFunc(args...);
		}
	}

	EXT_FUNC virtual void callOriginal(t_args... args) {
		if (m_OriginalFunc)
			m_OriginalFunc(args...);
	}

private:
	void** m_Hooks;
	origfunc_t m_OriginalFunc;
};

class AbstractHookChainRegistry {
protected:
	void* m_Hooks[MAX_HOOKS_IN_CHAIN + 1]; // +1 for null
	int m_Priorities[MAX_HOOKS_IN_CHAIN + 1];
	int m_NumHooks;

protected:
	void addHook(void* hookFunc, int priority);
	void removeHook(void* hookFunc);

public:
	int  getCount() const;
	bool findHook(void* hookFunc) const;
	AbstractHookChainRegistry();
};

template<typename t_ret, typename ...t_args>
class IHookChainRegistryImpl : public IHookChainRegistry < t_ret, t_args...>, public AbstractHookChainRegistry {
public:
	typedef t_ret(*hookfunc_t)(IHookChain<t_ret, t_args...>*, t_args...);
	typedef t_ret(*origfunc_t)(t_args...);

	virtual ~IHookChainRegistryImpl() { }

	t_ret callChain(origfunc_t origFunc, t_args... args) {
		IHookChainImpl<t_ret, t_args...> chain(m_Hooks, origFunc);
		return chain.callNext(args...);
	}

	EXT_FUNC virtual void registerHook(hookfunc_t hook, int priority) {
		addHook((void*)hook, priority);
	}

	EXT_FUNC virtual void unregisterHook(hookfunc_t hook) {
		removeHook((void*)hook);
	}

	bool isEmpty() const {
		return getCount() == 0;
	}
};

template<typename ...t_args>
class IVoidHookChainRegistryImpl : public IVoidHookChainRegistry <t_args...>, public AbstractHookChainRegistry {
public:
	typedef void(*hookfunc_t)(IVoidHookChain<t_args...>*, t_args...);
	typedef void(*origfunc_t)(t_args...);

	virtual ~IVoidHookChainRegistryImpl() { }

	void callChain(origfunc_t origFunc, t_args... args) {
		IVoidHookChainImpl<t_args...> chain(m_Hooks, origFunc);
		chain.callNext(args...);
	}

	EXT_FUNC virtual void registerHook(hookfunc_t hook, int priority) {
		addHook((void*)hook, priority);
	}

	EXT_FUNC virtual void unregisterHook(hookfunc_t hook) {
		removeHook((void*)hook);
	}

	bool isEmpty() const {
		return getCount() == 0;
	}
};
