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

Status::Status()
{
	m_Proxy = nullptr;

	m_NextUpdateTime = 0;
	m_NumberOfProxies = 0;
	m_NumberOfSlots = 0;
	m_NumberOfSpectators = 0;
	m_MaxNumberOfSpectators = 0;
}

bool Status::Init(IBaseSystem *system, int serial, char *name)
{
	BaseSystemModule::Init(system, serial, name);

	SetName("status");

	m_Proxies.Init();
	m_System->RegisterCommand("proxies", this, CMD_ID_PROXIES);

	Reset();

	m_State = MODULE_RUNNING;
	m_System->DPrintf("Status module initialized.\n");

	return true;
}

void Status::RunFrame(double time)
{
	BaseSystemModule::RunFrame(time);

	if (m_MaxNumberOfSpectators < m_NumberOfSpectators) {
		m_MaxNumberOfSpectators = m_NumberOfSpectators;
	}

	if (!m_Proxy->IsMaster() || !m_Proxy->IsActive())
		return;

	if (m_SystemTime > m_NextUpdateTime)
	{
		GetLocalStats(m_NumberOfProxies, m_NumberOfSlots, m_NumberOfSpectators);
		SendStatusUpdate();

		m_Proxy->GetServer()->SetUserInfo("hspecs", COM_VarArgs("%u", m_NumberOfSpectators));
		m_Proxy->GetServer()->SetUserInfo("hslots", COM_VarArgs("%u", m_NumberOfSlots));

		m_NextUpdateTime = float(m_SystemTime) + 16.f;
	}
}

void Status::ExecuteCommand(int commandID, char *commandLine)
{
	if (commandID == CMD_ID_PROXIES) {
		CMD_Proxies(commandLine);
		return;
	}

	m_System->Printf("ERROR! Status::ExecuteCommand: unknown command ID %i.\n", commandID);
}

void Status::ShutDown()
{
	if (m_State == MODULE_DISCONNECTED)
		return;

	m_Proxies.Clear(true);
	m_System->Printf("Status module shutdown.\n");

	BaseSystemModule::ShutDown();
}

void Status::GetGlobalStats(int &proxies, int &slots, int &spectators)
{
	proxies = m_NumberOfProxies;
	slots = m_NumberOfSlots;
	spectators = m_NumberOfSpectators;
}

void Status::SetProxy(IProxy *proxy)
{
	m_Proxy = proxy;
}

void Status::SendStatusUpdate()
{
	BitBuffer buffer(128);

	buffer.WriteByte(svc_hltv);
	buffer.WriteByte(HLTV_STATUS);

	buffer.WriteWord(m_NumberOfProxies);
	buffer.WriteLong(m_NumberOfSlots);
	buffer.WriteLong(m_NumberOfSpectators);

	m_Proxy->Broadcast(buffer.GetData(), buffer.CurrentSize(), GROUP_PROXY, true);
	buffer.FastClear();

	DirectorCmd cmd;
	cmd.SetStatusData(m_NumberOfSlots, m_NumberOfSpectators, m_NumberOfProxies);
	cmd.WriteToStream(&buffer);

	// group spectators
	m_Proxy->Broadcast(buffer.GetData(), buffer.CurrentSize(), GROUP_CLIENT | GROUP_DEMO | GROUP_UNKNOWN, false);
	buffer.FastClear();
}

void Status::SetName(char *newName)
{
	strcopy(m_Name, newName);
}

char *Status::GetType()
{
	return STATUS_INTERFACE_VERSION;
}

char *Status::GetStatusLine()
{
	static char string[256];
	_snprintf(string, sizeof(string), "Global Status: Proxies %i, Slots %i, Spectators %i (max %i)\n",
		m_NumberOfProxies, m_NumberOfSlots,
		m_NumberOfSpectators, m_MaxNumberOfSpectators);

	return string;
}

void Status::Reset()
{
	m_Proxies.Clear(true);

	m_NumberOfProxies = 0;
	m_NumberOfSlots = 0;
	m_NumberOfSpectators = 0;
	m_MaxNumberOfSpectators = 0;
	m_NextUpdateTime = 0;
}

void Status::ParseStatusMsg(BitBuffer *stream)
{
	if (m_Proxy->IsMaster()) {
		m_System->Printf("WARNING! Status::ParseStatusMsg: unexpected HLTV_STATUS message as Master.\n");
		stream->SkipBytes(10);
		return;
	}

	m_NumberOfProxies = stream->ReadWord();
	m_NumberOfSlots = stream->ReadLong();
	m_NumberOfSpectators = stream->ReadLong();

	ReplyStatusReport();
	SendStatusUpdate();
}

void Status::ReplyStatusReport()
{
	BitBuffer buf(128);

	int slots, proxies, spectators;
	GetLocalStats(proxies, slots, spectators);

	if (!m_Proxy->IsPasswordProtected())
	{
		if (!m_Proxy->GetDispatchMode()) {
			slots = m_Proxy->GetMaxClients();
		}
		else if (m_Proxy->IsStressed()) {
			proxies |= PROXY_PRIVATE;
		}
	}

	buf.WriteByte(1);
	buf.WriteWord(proxies);
	buf.WriteLong(slots);
	buf.WriteLong(spectators);

	m_Proxy->GetServer()->SendHLTVCommand(&buf);
}

int Status::GetMaxSpectatorNumber()
{
	return m_MaxNumberOfSpectators;
}

void Status::ParseStatusReport(NetAddress *from, BitBuffer *stream)
{
	int proxies = stream->ReadWord();
	int slots = stream->ReadLong();
	int spectators = stream->ReadLong();

	bool isPrivate = false;
	if (proxies & PROXY_PRIVATE) {
		proxies &= ~PROXY_PRIVATE;
		isPrivate = true;
	}

	proxyInfo_t *proxy = (proxyInfo_t *)m_Proxies.GetFirst();
	while (proxy)
	{
		if (from->Equal(&proxy->address)) {
			break;
		}

		proxy = (proxyInfo_t *)m_Proxies.GetNext();
	}

	// couldn't find it?
	// allocate for
	if (!proxy)
	{
		proxy = (proxyInfo_t *)Mem_ZeroMalloc(sizeof(proxyInfo_t));
		if (!proxy) {
			m_System->Printf("WARNING! Status::ParseStatusReport: not enough memory to increase proxy list.\n");
			return;
		}

		proxy->address.FromNetAddress(from);
		m_Proxies.Add(proxy);
	}

	proxy->slots = slots;
	proxy->spectators = spectators;
	proxy->proxies = proxies;
	proxy->time = float(m_SystemTime);
	proxy->isPrivate = isPrivate;

	float ratio = 1;
	if (slots > 0 && !isPrivate) {
		ratio = float(spectators) / float(slots);
	}

	m_Proxies.ChangeKey(proxy, ratio);
}

void Status::GetLocalStats(int &proxies, int &slots, int &spectators)
{
	slots = 0;
	spectators = 0;
	proxies = 0;

	proxyInfo_t *proxy = (proxyInfo_t *)m_Proxies.GetFirst();
	while (proxy)
	{
		if (m_SystemTime > proxy->time + 64)
		{
			m_Proxies.Remove(proxy);
			Mem_Free(proxy);
		}
		else
		{
			proxies += proxy->proxies;
			slots += proxy->slots;
			spectators += proxy->spectators;
		}

		proxy = (proxyInfo_t *)m_Proxies.GetNext();
	}

	int mySpecs, myProxies;
	m_Proxy->CountLocalClients(mySpecs, myProxies);

	spectators += mySpecs;
	slots += m_Proxy->GetMaxClients();

	proxies++;
}

void Status::CMD_Proxies(char *cmdLine)
{
	int nCount = 0;
	proxyInfo_t *proxy = (proxyInfo_t *)m_Proxies.GetFirst();
	while (proxy)
	{
		nCount++;
		m_System->Printf("IP %s, Clients %i, MaxClients %i%s\n", proxy->address.ToString(), proxy->spectators, proxy->slots, proxy->isPrivate ? " (excluded)" : "");

		proxy = (proxyInfo_t *)m_Proxies.GetNext();
	}

	m_System->Printf("--- Total %i relay proxies ---\n", nCount);
}

float Status::GetBestRelayProxy(NetAddress *addr)
{
	proxyInfo_t *proxy = (proxyInfo_t *)m_Proxies.GetFirst();
	while (proxy)
	{
		if (proxy->slots > 0 && proxy->slots >= proxy->spectators && !proxy->isPrivate)
		{
			float ratio = float(++proxy->spectators) / float(proxy->slots);

			m_Proxies.ChangeKey(proxy, ratio);
			addr->FromNetAddress(&proxy->address);
			return ratio;
		}

		m_Proxies.ChangeKey(proxy, 1);
		proxy = (proxyInfo_t *)m_Proxies.GetNext();
	}

	addr->Clear();
	return -1;
}
