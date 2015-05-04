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

#define MAX_HOOKS_IN_CHAIN 64

class AbstractHookChain {

protected:
	void* m_Hooks[MAX_HOOKS_IN_CHAIN];
	void* m_OriginalFunc;
	int m_CurHook;
	int m_NumHooks;

	bool m_bOriginalCalled;

	AbstractHookChain();

	void* nextHook();

public:
	void init(void* origFunc, void* hooks, int numHooks);
};


template<typename t_ret, typename ...t_args>
class IHookChainImpl : public IHookChain<t_ret, t_args...>, public AbstractHookChain {
public:
	typedef t_ret(*hookfunc_t)(IHookChain<t_ret, t_args...>*, t_args...);
	typedef t_ret(*origfunc_t)(t_args...);

private:
	t_ret m_OriginalReturnResult;

public:
	virtual ~IHookChainImpl() { }
	virtual t_ret callNext(t_args... args) {
		void* nextvhook = nextHook();
		if (nextvhook) {
			hookfunc_t nexthook = (hookfunc_t)nextvhook;
			return nexthook(this, args...);
		}

		origfunc_t origfunc = (origfunc_t)m_OriginalFunc;
		m_OriginalReturnResult = origfunc(args...);
		m_bOriginalCalled = true;
		return m_OriginalReturnResult;
	}

	virtual t_ret getOriginalReturnResult() {
		return m_OriginalReturnResult;
	}

	virtual bool isOriginalCalled() {
		return m_bOriginalCalled;
	}
};

template<typename ...t_args>
class IVoidHookChainImpl : public IVoidHookChain<t_args...>, public AbstractHookChain {
public:
	typedef void(*hookfunc_t)(IVoidHookChain<t_args...>*, t_args...);
	typedef void(*origfunc_t)(t_args...);

	virtual ~IVoidHookChainImpl() { }

	virtual void callNext(t_args... args) {
		void* nextvhook = nextHook();
		if (nextvhook) {
			hookfunc_t nexthook = (hookfunc_t)nextvhook;
			nexthook(this, args...);
			return;
		}

		origfunc_t origfunc = (origfunc_t)m_OriginalFunc;
		if (origfunc) {
			origfunc(args...);
		}
		m_bOriginalCalled = true;
	}

	virtual bool isOriginalCalled() {
		return m_bOriginalCalled;
	}
};

class AbstractHookChainRegistry {
protected:
	void* m_Hooks[MAX_HOOKS_IN_CHAIN];
	int m_NumHooks;

protected:
	void addHook(void* hookFunc);

public:
	AbstractHookChainRegistry();
};

template<typename t_ret, typename ...t_args>
class IHookChainRegistryImpl : public IHookChainRegistry < t_ret, t_args...>, public AbstractHookChainRegistry {
public:
	typedef t_ret(*hookfunc_t)(IHookChain<t_ret, t_args...>*, t_args...);
	typedef t_ret(*origfunc_t)(t_args...);

	virtual ~IHookChainRegistryImpl() { }

	t_ret callChain(origfunc_t origFunc, t_args... args) {
		IHookChainImpl<t_ret, t_args...> chain;
		chain.init((void*)origFunc, m_Hooks, m_NumHooks);
		return chain.callNext(args...);
	}

	virtual void registerHook(hookfunc_t hook) {
		addHook((void*)hook);
	}
};

template<typename ...t_args>
class IVoidHookChainRegistryImpl : public IVoidHookChainRegistry <t_args...>, public AbstractHookChainRegistry {
public:
	typedef void(*hookfunc_t)(IVoidHookChain<t_args...>*, t_args...);
	typedef void(*origfunc_t)(t_args...);

	virtual ~IVoidHookChainRegistryImpl() { }

	void callChain(origfunc_t origFunc, t_args... args) {
		IVoidHookChainImpl<t_args...> chain;
		chain.init((void*)origFunc, m_Hooks, m_NumHooks);
		chain.callNext(args...);
	}

	virtual void registerHook(hookfunc_t hook) {
		addHook((void*)hook);
	}
};
