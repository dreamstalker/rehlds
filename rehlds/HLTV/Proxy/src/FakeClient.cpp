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

FakeClient::FakeClient()
{
	m_Network = nullptr;
	m_World = nullptr;
	m_Server = nullptr;
	m_Socket = nullptr;
}

bool FakeClient::Init(IBaseSystem *system, int serial, char *name)
{
	BaseSystemModule::Init(system, serial, name);

	_snprintf(m_Name, sizeof(m_Name), "fakeclient%i", serial);

	m_Network = dynamic_cast<INetwork *>(m_System->GetModule(NETWORK_INTERFACE_VERSION, "core"));
	if (!m_Network)
	{
		m_System->Errorf("FakeClient::Init: couldn't load network module.\n");
		return false;
	}

	if (!(m_Socket = m_Network->CreateSocket(serial + 2048)))
	{
		m_System->Errorf("FakeClient::Init: Could not create port %i.\n", serial + 2048);
		return false;
	}

	char temp[256];
	sprintf(temp, "fakeserver%i", serial);

	m_Server = dynamic_cast<IServer *>(m_System->GetModule(SERVER_INTERFACE_VERSION, "core", temp));
	if (!m_Server)
	{
		m_System->Errorf("FakeClient::Init: couldn't load server module.\n");
		return false;
	}

	m_Server->RegisterListener(this);
	m_Server->SetUserInfo("hspecs", "0");
	m_Server->SetUserInfo("hslots", "0");
	m_Server->SetDelayReconnect(false);
	m_Server->SetPlayerName(m_Name);
	m_Server->SetAutoRetry(false);

	sprintf(temp, "fakeworld%i", serial);

	m_World = dynamic_cast<IWorld *>(m_System->GetModule(WORLD_INTERFACE_VERSION, "core", temp));
	if (!m_World)
	{
		m_System->Errorf("FakeClient::Init: couldn't load world module.\n");
		return false;
	}

	m_World->RegisterListener(this);

	m_State = MODULE_RUNNING;
	m_System->Printf("Fake client module initialized (%i).\n", serial);
	return true;
}

void FakeClient::RunFrame(double time)
{
	BaseSystemModule::RunFrame(time);

	NetPacket *packet;
	while ((packet = m_Socket->ReceivePacket()))
	{
		m_System->DPrintf("FakeClient: WARNING! Packet from %s with invalid sequence number.\n", packet->address.ToString());
		m_Socket->FreePacket(packet);
	}
}

void FakeClient::SetRate(int rate)
{
	m_Server->SetRate(rate);
}

void FakeClient::Connect(NetAddress *adr)
{
	m_Server->Connect(m_World, adr, m_Socket);
}

void FakeClient::Retry()
{
	m_Server->Retry();
}

void FakeClient::Say(char *text)
{
	if (!m_Server->IsConnected())
		return;

	char string[1024];
	_snprintf(string, sizeof(string), "say \"%s\"", text);
	m_Server->SendStringCommand(text);
}

void FakeClient::Disconnect()
{
	m_Server->Disconnect();
}

void FakeClient::ShutDown()
{
	if (m_State == MODULE_DISCONNECTED)
		return;

	if (m_World) {
		m_World->RemoveListener(this);
	}

	BaseSystemModule::ShutDown();
	m_System->Printf("FakeClient module shutdown.\n");
}

void FakeClient::ReceiveSignal(ISystemModule *module, unsigned int signal, void *data)
{
	if (module->GetSerial() != m_World->GetSerial())
		return;

	if (signal == 8 /* signal to shutdown*/) {
		ShutDown();
	}
}

char *FakeClient::GetType()
{
	return FAKECLIENT_INTERFACE_VERSION;
}

char *FakeClient::GetStatusLine()
{
	return "I am a fake client.\n";
}
