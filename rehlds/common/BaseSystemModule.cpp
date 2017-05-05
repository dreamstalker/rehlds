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

char *BaseSystemModule::GetName()
{
	return m_Name;
}

char *BaseSystemModule::GetType()
{
	return "GenericModule";
}

char *BaseSystemModule::GetStatusLine()
{
	return "No status available.\n";
}

void BaseSystemModule::ExecuteCommand(int commandID, char *commandLine)
{
	m_System->DPrintf("WARNING! Undeclared ExecuteCommand().\n");
}

extern int COM_BuildNumber();

int BaseSystemModule::GetVersion()
{
	return COM_BuildNumber();
}

int BaseSystemModule::GetSerial()
{
	return m_Serial;
}

IBaseSystem *BaseSystemModule::GetSystem()
{
	return m_System;
}

bool BaseSystemModule::Init(IBaseSystem *system, int serial, char *name)
{
	if (!system)
		return false;

	m_State = MODULE_INITIALIZING;
	m_System = system;
	m_Serial = serial;
	m_SystemTime = 0;

	if (name) {
		strcopy(m_Name, name);
	}

	return true;
}

void BaseSystemModule::RunFrame(double time)
{
	m_SystemTime = time;
}

void BaseSystemModule::ShutDown()
{
	if (m_State == MODULE_DISCONNECTED)
		return;

	m_Listener.Clear();
	m_State = MODULE_DISCONNECTED;

	if (!m_System->RemoveModule(this))
	{
		m_System->DPrintf("ERROR! BaseSystemModule::ShutDown: faild to remove module %s.\n", m_Name);
	}
}

void BaseSystemModule::RegisterListener(ISystemModule *module)
{
	ISystemModule *listener = (ISystemModule *)m_Listener.GetFirst();
	while (listener)
	{
		if (listener->GetSerial() == module->GetSerial())
		{
			m_System->DPrintf("WARNING! BaseSystemModule::RegisterListener: module %s already added.\n", module->GetName());
			return;
		}

		listener = (ISystemModule *)m_Listener.GetNext();
	}

	m_Listener.Add(module);
}

void BaseSystemModule::RemoveListener(ISystemModule *module)
{
	ISystemModule *listener = (ISystemModule *)m_Listener.GetFirst();
	while (listener)
	{
		if (listener->GetSerial() == module->GetSerial())
		{
			m_Listener.Remove(module);
			return;
		}

		listener = (ISystemModule *)m_Listener.GetNext();
	}
}

void BaseSystemModule::FireSignal(unsigned int signal, void *data)
{
	ISystemModule *listener = (ISystemModule *)m_Listener.GetFirst();
	while (listener)
	{
		listener->ReceiveSignal(this, signal, data);
		listener = (ISystemModule *)m_Listener.GetNext();
	}
}

void BaseSystemModule::ReceiveSignal(ISystemModule *module, unsigned int signal, void *data)
{
	m_System->DPrintf("WARNING! Unhandled signal (%i) from module %s.\n", signal, module->GetName());
}

int BaseSystemModule::GetState()
{
	return m_State;
}
