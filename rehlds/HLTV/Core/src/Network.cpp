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

Network::Network()
{
	m_NoDNS = false;
	m_IsMultihomed = false;

	m_FakeLoss = 0;
	m_LastStatsUpdateTime = 0;
}

bool Network::Init(IBaseSystem *system, int serial, char *name)
{
	BaseSystemModule::Init(system, serial, name);

	if (!name) {
		SetName(NETWORK_INTERFACE_VERSION);
	}

	m_FakeLoss = 0;
	m_IsMultihomed = false;
	m_NoDNS = false;

	m_Sockets.Init();
	m_System->RegisterCommand("fakeloss", this, CMD_ID_FAKELOSS);

#ifdef _WIN32
	// Startup winock
	WORD version = MAKEWORD(2, 2); // for use Winsock 2.2, version should be 514
	WSADATA wsaData;
	int err = WSAStartup(version, &wsaData);
	if (err != NO_ERROR)
	{
		m_System->Printf("Network Error WSAStartup:%s\n", GetErrorText(GetLastErrorCode()));
		return false;
	}
#endif // _WIN32

	if (m_System->CheckParam("-nodns")) {
		m_NoDNS = true;
	}

	if (ResolveAddress(m_System->CheckParam("-ip"), &m_LocalAddress))
	{
		m_IsMultihomed = true;
		m_System->Printf("Network uses %s as host IP.\n", m_LocalAddress.ToBaseString());
	}
	else
	{
		char hostname[512];
		gethostname(hostname, sizeof(hostname));
		ResolveAddress(hostname, &m_LocalAddress);
	}

	char *portparam = m_System->CheckParam("-port");
	if (!portparam) {
		portparam = "27020";
	}

	m_LocalAddress.m_Port = htons(atoi(portparam));
	m_LastStatsUpdateTime = 0;

	m_State = MODULE_RUNNING;
	m_System->Printf("Network initialized.\n");
	return true;
}

void Network::ExecuteCommand(int commandID, char *commandLine)
{
	if (commandID == CMD_ID_FAKELOSS) {
		CMD_FakeLoss(commandLine);
		return;
	}

	m_System->Printf("ERROR! Network::ExecuteCommand: unknown command ID %i.\n", commandID);
}

void Network::ShutDown()
{
	if (m_State == MODULE_DISCONNECTED) {
		return;
	}

	NetSocket *sock;
	while ((sock = (NetSocket *)m_Sockets.RemoveTail()))
	{
		sock->Close();
		delete sock;
	}

#ifdef _WIN32
	if (WSACleanup() == SOCKET_ERROR) {
		m_System->Printf("Network Error WSACleanup:%s\n", GetErrorText(GetLastErrorCode()));
	}
#endif // _WIN32

	m_System->Printf("Network module shutdown.\n");
	BaseSystemModule::ShutDown();
}

INetSocket *Network::CreateSocket(int port, bool reuse, bool loopback)
{
	NetSocket *socket = GetSocket(port);
	if (socket) {
		return socket;
	}

	socket = new NetSocket;
	if (!socket) {
		m_System->Printf("ERROR! Network::CreateSocket: out of memory.\n");
		return nullptr;
	}

	if (!socket->Create(this, port, reuse, loopback))
	{
		m_System->Printf("ERROR! Network::CreateSocket: %s.\n", GetErrorText(GetLastErrorCode()));

		delete socket;
		return nullptr;
	}

	m_Sockets.Add(socket);
	return socket;
}

bool Network::RemoveSocket(INetSocket *netsocket)
{
	return m_Sockets.Remove(netsocket);
}

char *Network::GetType()
{
	return NETWORK_INTERFACE_VERSION;
}

int Network::ReceiveData()
{
	int countPackets = 0;
	NetSocket *sock = (NetSocket *)m_Sockets.GetFirst();
	while (sock)
	{
		countPackets += sock->DispatchIncoming();
		sock = (NetSocket *)m_Sockets.GetNext();
	}

	return countPackets;
}

int Network::SendData()
{
	int countPackets = 0;
	NetSocket *sock = (NetSocket *)m_Sockets.GetFirst();
	while (sock)
	{
		countPackets += sock->DrainChannels();
		sock = (NetSocket *)m_Sockets.GetNext();
	}

	return countPackets;
}

NetSocket *Network::GetSocket(int port)
{
	NetSocket *sock = (NetSocket *)m_Sockets.GetFirst();
	if (!sock) {
		return nullptr;
	}

	while (sock)
	{
		if (sock->GetPort() == port || !port) {
			return sock;
		}

		sock = (NetSocket *)m_Sockets.GetNext();
	}

	return nullptr;
}

void Network::RunFrame(double time)
{
	BaseSystemModule::RunFrame(time);

	SendData();
	ReceiveData();
	UpdateStats();
}

NetAddress *Network::GetLocalAddress()
{
	return &m_LocalAddress;
}

char *Network::GetStatusLine()
{
	static char string[256];
	float in, out;

	GetFlowStats(&in, &out);
	_snprintf(string, sizeof(string), "Local IP %s, Sockets %i, In %.2f, Out %.2f.\n", m_LocalAddress.ToBaseString(), m_Sockets.CountElements(), in, out);
	return string;
}

void Network::GetFlowStats(float *totalIn, float *totalOut)
{
	*totalIn = 0;
	*totalOut = 0;

	NetSocket *sock = (NetSocket *)m_Sockets.GetFirst();
	while (sock)
	{
		float in, out;
		sock->GetFlowStats(&in, &out);

		*totalIn += in;
		*totalOut += out;

		sock = (NetSocket *)m_Sockets.GetNext();
	}
}

void Network::CMD_FakeLoss(char *cmdLine)
{
	TokenLine params(cmdLine);
	if (params.CountToken() != 2) {
		m_System->Printf("Syntax: fakeloss <float>\n");
		m_System->Printf("Current fakeloss is %.2f\n", m_FakeLoss);
		return;
	}

	m_FakeLoss = atof(params.GetToken(1));
}

int Network::GetLastErrorCode()
{
	return WSAGetLastError();
}

bool Network::ResolveAddress(char *string, NetAddress *address)
{
	sockaddr sadr;
	hostent *h;
	char copy[128];

	address->Clear();

	if (!string || !strlen(string)) {
		return 0;
	}

	memset(&sadr, 0, sizeof(sadr));
	((sockaddr_in *)&sadr)->sin_family = AF_INET;
	((sockaddr_in *)&sadr)->sin_port = 0;

	strcopy(copy, string);

	// Parse port
	char *colon = copy;
	while (*colon != '\0')
	{
		if (*colon == ':')
		{
			*colon = '\0';
			int val = atoi(colon + 1);
			((sockaddr_in *)&sadr)->sin_port = htons(val);
		}
		colon++;
	}

	// Parse address
	// Validate IPv4
	if (copy[0] >= '0' && copy[0] <= '9' && strstr(copy, "."))
	{
		uint32 ret = inet_addr(copy);
		if (ret == INADDR_NONE) {
			return false;
		}

		((sockaddr_in *)&sadr)->sin_addr.s_addr = ret;
	}
	else if (m_NoDNS)
	{
		// do not resolve address via DNS.
		return false;
	}
	// If we allow the use of DNS,
	// let's get the address by the hostname.
	else
	{
		h = gethostbyname(copy);
		if (!h || !h->h_addr) {
			return false;
		}

		((sockaddr_in *)&sadr)->sin_addr.s_addr = *(uint32 *)h->h_addr;
	}

	return address->FromSockadr(&sadr);
}

char *Network::GetErrorText(int code)
{
	switch (code)
	{
	case WSAEINTR:           return "WSAEINTR";
	case WSAEBADF:           return "WSAEBADF";
	case WSAEACCES:          return "WSAEACCES";
	case WSAEFAULT:          return "WSAEFAULT";
	case WSAEINVAL:          return "WSAEINVAL";
	case WSAEMFILE:          return "WSAEMFILE";
	case WSAEWOULDBLOCK:     return "WSAEWOULDBLOCK";
	case WSAEINPROGRESS:     return "WSAEINPROGRESS";
	case WSAEALREADY:        return "WSAEALREADY";
	case WSAENOTSOCK:        return "WSAENOTSOCK";
	case WSAEDESTADDRREQ:    return "WSAEDESTADDRREQ";
	case WSAEMSGSIZE:        return "WSAEMSGSIZE";
	case WSAEPROTOTYPE:      return "WSAEPROTOTYPE";
	case WSAENOPROTOOPT:     return "WSAENOPROTOOPT";
	case WSAEPROTONOSUPPORT: return "WSAEPROTONOSUPPORT";
	case WSAESOCKTNOSUPPORT: return "WSAESOCKTNOSUPPORT";
	case WSAEOPNOTSUPP:      return "WSAEOPNOTSUPP";
	case WSAEPFNOSUPPORT:    return "WSAEPFNOSUPPORT";
	case WSAEAFNOSUPPORT:    return "WSAEAFNOSUPPORT";
	case WSAEADDRINUSE:      return "WSAEADDRINUSE";
	case WSAEADDRNOTAVAIL:   return "WSAEADDRNOTAVAIL";
	case WSAENETDOWN:        return "WSAENETDOWN";
	case WSAENETUNREACH:     return "WSAENETUNREACH";
	case WSAENETRESET:       return "WSAENETRESET";
	case WSAECONNABORTED:    return "WSWSAECONNABORTEDAEINTR";
	case WSAECONNRESET:      return "WSAECONNRESET";
	case WSAENOBUFS:         return "WSAENOBUFS";
	case WSAEISCONN:         return "WSAEISCONN";
	case WSAENOTCONN:        return "WSAENOTCONN";
	case WSAESHUTDOWN:       return "WSAESHUTDOWN";
	case WSAETOOMANYREFS:    return "WSAETOOMANYREFS";
	case WSAETIMEDOUT:       return "WSAETIMEDOUT";
	case WSAECONNREFUSED:    return "WSAECONNREFUSED";
	case WSAELOOP:           return "WSAELOOP";
	case WSAENAMETOOLONG:    return "WSAENAMETOOLONG";
	case WSAEHOSTDOWN:       return "WSAEHOSTDOWN";
	default:                 return "UNKNOWN ERROR";

#ifdef _WIN32
	case WSASYSNOTREADY:     return "WSASYSNOTREADY";
	case WSAVERNOTSUPPORTED: return "WSAVERNOTSUPPORTED";
	case WSANOTINITIALISED:  return "WSANOTINITIALISED";
	case WSAEDISCON:         return "WSAEDISCON";
	case WSAHOST_NOT_FOUND:  return "WSAHOST_NOT_FOUND";
	case WSATRY_AGAIN:       return "WSATRY_AGAIN";
	case WSANO_RECOVERY:     return "WSANO_RECOVERY";
	case WSANO_DATA:         return "WSANO_DATA";
#endif // _WIN32
	}
}

void Network::SetName(char *newName)
{
	strcopy(m_Name, newName);
}

void Network::UpdateStats()
{
	if (m_LastStatsUpdateTime + 0.5 > m_SystemTime) {
		return;
	}

	NetSocket *sock = (NetSocket *)m_Sockets.GetFirst();
	while (sock)
	{
		sock->UpdateStats(m_SystemTime);
		sock = (NetSocket *)m_Sockets.GetNext();
	}

	m_LastStatsUpdateTime = m_SystemTime;
}

void Network::ReceiveSignal(ISystemModule *module, unsigned int signal, void *data)
{
	BaseSystemModule::ReceiveSignal(module, signal, data);
}

void Network::RegisterListener(ISystemModule *module)
{
	BaseSystemModule::RegisterListener(module);
}

void Network::RemoveListener(ISystemModule *module)
{
	BaseSystemModule::RemoveListener(module);
}

IBaseSystem *Network::GetSystem()
{
	return BaseSystemModule::GetSystem();
}

int Network::GetSerial()
{
	return BaseSystemModule::GetSerial();
}

char *Network::GetName()
{
	return BaseSystemModule::GetName();
}

int Network::GetState()
{
	return BaseSystemModule::GetState();
}

int Network::GetVersion()
{
	return BaseSystemModule::GetVersion();
}

IBaseInterface *CreateNetwork()
{
	INetwork *pNetwork = new Network;
	return (IBaseInterface *)pNetwork;
}

#ifndef HOOK_HLTV
EXPOSE_INTERFACE_FN(CreateNetwork, Network, NETWORK_INTERFACE_VERSION);
#endif // HOOK_HLTV
