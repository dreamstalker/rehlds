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

DemoClient::DemoClient()
{
	m_Proxy = nullptr;
	m_World = nullptr;

	m_ClientDelta = 0;
	m_LastFrameSeqNr = 0;
	m_IsActive = false;

	memset(m_BaseFileName, 0, sizeof(m_BaseFileName));
}

bool DemoClient::Init(IBaseSystem *system, int serial, char *name)
{
	BaseSystemModule::Init(system, serial, name);

	m_Proxy = nullptr;
	m_LastFrameSeqNr = 0;
	m_ClientDelta = 0;
	m_IsActive = false;

	memset(m_BaseFileName, 0, sizeof(m_BaseFileName));
	m_DemoInfo.SetMaxSize(MAX_DEMO_INFO);

	m_State = MODULE_RUNNING;
	m_System->Printf("Demo client initialized.\n");

	return true;
}

char *DemoClient::GetStatusLine()
{
	static char string[256];
	_snprintf(string, sizeof(string), "Recording to %s (%.1f seconds)\n", m_DemoFile.GetFileName(), m_DemoFile.GetDemoTime());
	return string;
}

bool DemoClient::IsHearingVoices()
{
	return true;
}

bool DemoClient::HasChatEnabled()
{
	return false;
}

char *DemoClient::GetType()
{
	return DEMOCLIENT_INTERFACE_VERSION;
}

int DemoClient::GetClientType()
{
	return TYPE_DEMO;
}

void DemoClient::Reconnect()
{
	FinishDemo();
}

bool DemoClient::Connect(INetSocket *socket, NetAddress *adr, char *userinfo)
{
	if (IsActive())
	{
		if (m_DemoFile.IsRecording()) {
			m_System->Printf("Already recording to %s.\n", m_DemoFile.GetFileName());
		} else {
			m_System->Printf("Already recording initialized for %s.\n", m_BaseFileName);
		}

		return false;
	}

	m_DemoChannel.Create(m_System);
	m_DemoChannel.SetUpdateRate(40);
	m_DemoChannel.SetRate(20000);

	m_LastFrameSeqNr = 0;
	m_ClientDelta = 0;

	m_DemoChannel.SetKeepAlive(false);
	m_DemoFile.Init(m_World, nullptr, &m_DemoChannel);

	m_IsActive = true;
	m_System->Printf("Recording initialized.\n");

	return true;
}

void DemoClient::RunFrame(double time)
{
	BaseSystemModule::RunFrame(time);

	NetPacket *packet = m_DemoChannel.GetPacket();
	while (packet)
	{
		m_System->DPrintf("WARNING! Incoming data in demo game channel.Ignored.\n");
		m_DemoChannel.FreePacket(packet);

		packet = m_DemoChannel.GetPacket();
	}

	if (IsActive() && m_World->IsActive() && m_DemoChannel.IsReadyToSend()) {
		SendDatagram();
	}
}

void DemoClient::SendDatagram()
{
	if (m_Proxy->GetDelay() > 0)
	{
		double worldTime = m_Proxy->GetSpectatorTime();
		double proxyTime = m_Proxy->GetProxyTime();

		frame_t *frame = m_World->GetFrameByTime(worldTime);
		if (!frame) {
			return;
		}

		if (frame->seqnr > 1) {
			WriteDatagram(proxyTime - (worldTime - frame->time), frame);
		}

		return;
	}

	frame_t *frame = m_World->GetLastFrame();
	if (frame) {
		WriteDatagram(frame->time, frame);
	}
}

void DemoClient::WriteDatagram(double time, frame_t *frame)
{
	unsigned int deltaFrameSeqNr = m_LastFrameSeqNr;
	if (deltaFrameSeqNr > 0)
	{
		if (deltaFrameSeqNr == frame->seqnr)
			return;

		if (deltaFrameSeqNr > frame->seqnr)
			m_LastFrameSeqNr = frame->seqnr - 1;
	}
	else
	{
		char mapname[MAX_PATH];
		COM_FileBase(m_World->GetLevelName(), mapname);

		char fileName[MAX_PATH];
		_snprintf(fileName, sizeof(fileName), "%s-%s-%s.dem", m_BaseFileName, COM_TimeString(), mapname);

		m_DemoFile.StartRecording(fileName);
		m_Proxy->WriteSignonData(TYPE_DEMO, &m_DemoChannel.m_reliableStream);
	}

	m_DemoChannel.m_unreliableStream.WriteByte(svc_time);
	m_DemoChannel.m_unreliableStream.WriteFloat((float)time);

	m_World->WriteFrame(frame, m_LastFrameSeqNr, &m_DemoChannel.m_reliableStream, &m_DemoChannel.m_unreliableStream, deltaFrameSeqNr, m_ClientDelta, true);
	m_LastFrameSeqNr = frame->seqnr;
	m_ClientDelta = (m_DemoChannel.m_outgoing_sequence & 0xFF);

	// reliable stream has overflowed
	if (m_DemoChannel.m_reliableStream.IsOverflowed())
	{
		m_System->Printf("WARNING! DemoClient::WriteDatagram: reliable data overflow.\n");
		Disconnect("reliable data overflow");
		return;
	}

	if (m_DemoChannel.m_unreliableStream.IsOverflowed()) {
		m_DemoChannel.m_unreliableStream.Clear();
	}

	m_DemoFile.WriteDemoMessage(&m_DemoChannel.m_unreliableStream, &m_DemoChannel.m_reliableStream);
	m_DemoChannel.TransmitOutgoing();

	client_data_t cdata;
	memset(&cdata, 0, sizeof(cdata));
	m_DemoFile.WriteUpdateClientData(&cdata);
}

bool DemoClient::IsActive()
{
	return m_IsActive;
}

void DemoClient::Disconnect(const char *reason)
{
	if (!IsActive())
		return;

	if (reason)
	{
		m_DemoChannel.m_reliableStream.WriteByte(svc_print);
		m_DemoChannel.m_reliableStream.WriteString(reason);
	}

	FinishDemo();
	m_IsActive = false;
}

void DemoClient::ShutDown()
{
	if (m_State == MODULE_DISCONNECTED)
		return;

	FinishDemo();
	m_IsActive = false;

	BaseSystemModule::ShutDown();
	m_System->Printf("Demo module shutdown.\n");
}

void DemoClient::FinishDemo()
{
	if (!IsActive())
		return;

	m_DemoChannel.m_reliableStream.WriteByte(svc_print);
	m_DemoChannel.m_reliableStream.WriteString("HLTV Demo finished.\n");

	m_DemoFile.WriteDemoMessage(&m_DemoChannel.m_unreliableStream, &m_DemoChannel.m_reliableStream);
	m_DemoFile.CloseFile();
	m_DemoChannel.Clear();

	m_LastFrameSeqNr = 0;
	m_ClientDelta = 0;
}

NetAddress *DemoClient::GetAddress()
{
	static NetAddress fakeaddress;
	fakeaddress.Clear();
	return &fakeaddress;
}

void DemoClient::Send(unsigned char *data, int length, bool isReliable)
{
	if (isReliable)
		m_DemoChannel.m_reliableStream.WriteBuf(data, length);
	else
		m_DemoChannel.m_unreliableStream.WriteBuf(data, length);
}

void DemoClient::SetProxy(IProxy *proxy)
{
	m_Proxy = proxy;
}

InfoString *DemoClient::GetUserInfo()
{
	return &m_DemoInfo;
}

char *DemoClient::GetClientName()
{
	return m_Name;
}

void DemoClient::SetWorld(IWorld *world)
{
	m_World = world;
}

void DemoClient::SetFileName(char *fileName)
{
	strcopy(m_BaseFileName, fileName);
}

DemoFile *DemoClient::GetDemoFile()
{
	return &m_DemoFile;
}
