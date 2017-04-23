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

bool NetSocket::AddChannel(INetChannel *channel)
{
	return m_Channels.AddTail(channel);
}

bool NetSocket::RemoveChannel(INetChannel *channel)
{
	return m_Channels.Remove(channel);
}

int NetSocket::DispatchIncoming()
{
	int length = 0;
	int count = 0;

	NetAddress from;
	while ((length = ReceivePacketIntern(&from)))
	{
		if (length == -1)
			break;

		INetChannel *channel = (INetChannel *)m_Channels.GetFirst();
		while (channel)
		{
			if (from.Equal(channel->GetTargetAddress())) {
				channel->ProcessIncoming(m_Buffer, length);
				break;
			}

			channel = (INetChannel *)m_Channels.GetNext();
		}

		// not found an existing channel for this address.
		// create a new channel
		if (!channel)
		{
			// do accept only connectionless (\xFF\xFF\xFF\xFF) incoming packet.
			if (*(int *)m_Buffer == CONNECTIONLESS_HEADER)
			{
				NetPacket *p = new NetPacket;
				p->time = m_System->GetTime();
				p->address.FromNetAddress(&from);
				p->connectionless = true;
				p->seqnr = -1;

				// cut connectionless data
				p->data.Resize(length - 4);
				p->data.WriteBuf(&m_Buffer[4], length - 4);
				p->data.Reset();
				m_IncomingPackets.AddHead(p);
			}
		}

		count++;
	}

	return count;
}

bool NetSocket::SendPacket(NetPacket *packet)
{
	return SendPacket(&packet->address, packet->data.GetData(), packet->data.CurrentSize());
}

bool NetSocket::SendPacket(NetAddress *to, const void *data, int length)
{
	struct sockaddr addr;
	int ret;

	if (m_Network->m_FakeLoss && RandomFloat(0, 1) < m_Network->m_FakeLoss) {
		return true;
	}

	if (length <= 0 || !data || !to) {
		return true;
	}

	to->ToSockadr(&addr);

	if (length > MAX_ROUTEABLE_PACKET)
	{
		ret = SendLong((const char *)data, length, 0, &addr, sizeof(addr));
	}
	else
	{
		if (length < sizeof(addr))
			ret = SendShort((const char *)data, length, 0, &addr, sizeof(addr));
		else
			ret = sendto(m_Socket, (const char *)data, length, 0, &addr, sizeof(addr));
	}

	if (ret == -1)
	{
		int err = m_Network->GetLastErrorCode();

		// wouldblock is silent
		if (err == WSAEWOULDBLOCK)
			return true;

		if (err == WSAECONNREFUSED || err == WSAECONNRESET)
			return true;

		// some PPP links dont allow broadcasts
		if (err == WSAEADDRNOTAVAIL)
			return true;

		m_System->Printf("WARNING! NetSocket::SendPacket: %s\n", m_Network->GetErrorText(err));
		return false;
	}

	m_BytesOut += ret + UDP_HEADER_SIZE;
	return true;
}

INetwork *NetSocket::GetNetwork()
{
	return m_Network;
}

int NetSocket::ReceivePacketIntern(NetAddress *fromHost)
{
	sockaddr from;
	socklen_t fromlen = sizeof(from);
	int size = recvfrom(m_Socket, (char *)m_Buffer, sizeof(m_Buffer), 0, &from, &fromlen);
	if (size == -1)
	{
		int err = m_Network->GetLastErrorCode();
		if (err != WSAEWOULDBLOCK && err != WSAECONNRESET && err != WSAECONNREFUSED)
		{
			if (err == WSAEMSGSIZE)
			{
				m_System->DPrintf("WARNING! NetSocket::ReceivePacket: Ignoring oversized network message\n");
			}
			else
			{
				m_System->DPrintf("WARNING! NetSocket::ReceivePacket: %s %d %i\n", m_Network->GetErrorText(err), err, errno);
				perror("err");
			}
		}

		return false;
	}

	if (size == 0 || RandomFloat(0, 1) < m_Network->m_FakeLoss) {
		return 0;
	}

	if (size > 0 && size < 4) {
		m_System->DPrintf("WARNING! NetSocket::ReceivePacket: Ignoring undersized network message\n");
		return 0;
	}

	if (size >= MAX_UDP_PACKET) {
		m_System->DPrintf("WARNING! NetSocket::ReceivePacket: Oversize packet from %s\n", fromHost->ToString());
		return 0;
	}

	if (*(uint32 *)m_Buffer == NET_HEADER_FLAG_SPLITPACKET)
	{
		if (size < sizeof(SPLITPACKET))
		{
			m_System->Printf("Invalid split packet length %i\n", size);
			return 0;
		}

		size = GetLong(m_Buffer, size);
	}

	fromHost->FromSockadr(&from);
	m_BytesIn += size + UDP_HEADER_SIZE;
	return size;
}

void NetSocket::Close()
{
	INetChannel *channel;
	while ((channel = (INetChannel *)m_Channels.RemoveHead())) {
		channel->Close();
	}

	Flush();
	m_Network->RemoveSocket(this);

	shutdown(m_Socket, 2);
	SOCKET_CLOSE(m_Socket);
}

bool NetSocket::JoinGroup(NetAddress *group)
{
	ip_mreq mreq;
	SIN_SET_ADDR(&mreq.imr_multiaddr, *(unsigned int *)&group->m_IP[0]);
	SIN_SET_ADDR(&mreq.imr_interface, 0);

	if (setsockopt(m_Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) == SOCKET_ERROR)
	{
		int err = m_Network->GetLastErrorCode();
		if (err != WSAEAFNOSUPPORT) {
			m_System->DPrintf("WARNING! NetSocket::JoinGroup: IP_ADD_MEMBERSHIP: %s\n", m_Network->GetErrorText(err));
		}

		return false;
	}

	return true;
}

bool NetSocket::LeaveGroup(NetAddress *group)
{
	ip_mreq mreq;
	SIN_SET_ADDR(&mreq.imr_multiaddr, *(unsigned int *)&group->m_IP[0]);
	SIN_SET_ADDR(&mreq.imr_interface, 0);

	if (setsockopt(m_Socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) == SOCKET_ERROR)
	{
		int err = m_Network->GetLastErrorCode();
		if (err != WSAEAFNOSUPPORT) {
			m_System->DPrintf("WARNING! NetSocket::LeaveGroup: IP_DROP_MEMBERSHIP: %s\n", m_Network->GetErrorText(err));
		}

		return false;
	}

	return true;
}

int NetSocket::DrainChannels()
{
	int count = 0;
	INetChannel *channel = (INetChannel *)m_Channels.GetFirst();
	while (channel)
	{
		if (channel->IsReadyToSend()
			&& channel->KeepAlive()
			&& channel->IsConnected())
		{
			count++;
			channel->TransmitOutgoing();
		}

		channel = (INetChannel *)m_Channels.GetNext();
	}

	return count;
}

int NetSocket::GetLong(unsigned char *pData, int size)
{
	unsigned int packetNumber;
	unsigned int packetCount;
	int sequenceNumber;
	unsigned char packetID;
	static int netSplitFlags[MAX_SPLIT_FRAGMENTS] = { -1, -1, -1, -1, -1 };
	SPLITPACKET *pHeader = (SPLITPACKET *)pData;

	sequenceNumber = pHeader->sequenceNumber;
	packetID = pHeader->packetID;
	packetCount = packetID & 0xF;
	packetNumber = (unsigned int)packetID >> 4;

	if (packetNumber >= MAX_SPLIT_FRAGMENTS || packetCount > MAX_SPLIT_FRAGMENTS)
	{
		m_System->Printf("Malformed packet number (%i)\n", packetNumber);
		return 0;
	}

	if (m_NetSplitPacket.currentSequence == -1 || sequenceNumber != m_NetSplitPacket.currentSequence)
	{
		m_NetSplitPacket.currentSequence = sequenceNumber;
		m_NetSplitPacket.splitCount = packetCount;
	}

	unsigned int packetPayloadSize = size - sizeof(SPLITPACKET);
	if (netSplitFlags[packetNumber] == sequenceNumber)
	{
		m_System->DPrintf("NetSocket::GetLong: Ignoring duplicated split packet %i of %i ( %i bytes )\n", packetNumber + 1, packetCount, packetPayloadSize);
	}
	else
	{
		if (packetNumber == packetCount - 1) {
			m_NetSplitPacket.totalSize = packetPayloadSize + SPLIT_SIZE * (packetCount - 1);
		}

		--m_NetSplitPacket.splitCount;
		netSplitFlags[packetNumber] = sequenceNumber;
	}

	memcpy(&m_NetSplitPacket.buffer[SPLIT_SIZE * packetNumber], pHeader + 1, packetPayloadSize);

	if (m_NetSplitPacket.splitCount > 0) {
		return 0;
	}

	m_NetSplitPacket.currentSequence = -1;
	if (m_NetSplitPacket.totalSize > MAX_UDP_PACKET)
	{
		m_System->DPrintf("WARNING! NetSocket::GetLong: Split packet too large! %d bytes\n", m_NetSplitPacket.totalSize);
		return -1;

	}

	memcpy(pData, m_NetSplitPacket.buffer, m_NetSplitPacket.totalSize);
	return m_NetSplitPacket.totalSize;
}

void NetSocket::OutOfBandPrintf(NetAddress *to, const char *format, ...)
{
	va_list argptr;
	char string[NET_MAX_MESSAGE];

	*(int *)string = CONNECTIONLESS_HEADER;

	va_start(argptr, format);
	_vsnprintf(&string[4], sizeof(string) - 4, format, argptr);
	va_end(argptr);

	SendPacket(to, string, strlen(string) + 1);
}

void NetSocket::GetFlowStats(float *avgInKBSec, float *avgOutKBSec)
{
	*avgInKBSec = m_AvgBytesIn / 1024;
	*avgOutKBSec = m_AvgBytesOut / 1024;
}

NetPacket *NetSocket::ReceivePacket()
{
	return (NetPacket *)m_IncomingPackets.RemoveTail();
}

void NetSocket::FreePacket(NetPacket *packet)
{
	if (packet) {
		delete packet;
	}
}

void NetSocket::AddPacket(NetPacket *packet)
{
	if (!packet->connectionless) {
		m_System->DPrintf("WARNING! NetSocket::AddPacket: only connectionless packets accepted.\n");
		return;
	}

	NetPacket *newpacket = new NetPacket;

	newpacket->time = packet->time;
	newpacket->address.FromNetAddress(&packet->address);
	newpacket->connectionless = true;
	newpacket->seqnr = -1;

	newpacket->data.Resize(packet->data.GetMaxSize());
	newpacket->data.WriteBuf(packet->data.GetData(), packet->data.GetMaxSize());
	newpacket->data.Reset();

	m_IncomingPackets.AddHead(newpacket);
}

void NetSocket::Flush()
{
	NetPacket *in;
	while ((in = (NetPacket *)m_IncomingPackets.RemoveTail())) {
		FreePacket(in);
	}

	if (m_Socket != INVALID_SOCKET)
	{
		sockaddr from;
		socklen_t fromlen = sizeof(from);
		while (recvfrom(m_Socket, (char *)m_Buffer, sizeof(m_Buffer), 0, &from, &fromlen) > 0)
			;
	}
}

bool NetSocket::Create(Network *network, int port, bool reuse, bool loopback)
{
	sockaddr_in address;
	char _true = 1;
	uint32 i = 1;

	m_Network = network;
	m_System = network->GetSystem();

	m_Channels.Init();

	memset(m_Buffer, 0, sizeof(m_Buffer));
	memset(&m_NetSplitPacket, 0, sizeof(m_NetSplitPacket));

	if ((m_Socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {
		return false;
	}

	if (SOCKET_FIONBIO(m_Socket, i) == SOCKET_ERROR) {
		return false;
	}

	if (setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i)) == SOCKET_ERROR) {
		return false;
	}

	// resue
	if (reuse && setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, &_true, sizeof(_true)) == SOCKET_ERROR) {
		return false;
	}

	if (m_Network->m_IsMultihomed)
	{
		m_Network->GetLocalAddress()->ToSockadr((sockaddr *)&address);
	}
	else
	{
		address.sin_addr.s_addr = 0;
	}

	m_Port = port;
	address.sin_port = htons(port);
	address.sin_family = AF_INET;

	if (bind(m_Socket, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
		SOCKET_CLOSE(m_Socket);
		return false;
	}

	// Time-To-Live (TTL) for Multicast Packets
	//	As the values of the TTL field increase, routers will expand the number of hops they will forward a multicast packet.
	//	To provide meaningful scope control, multicast routers enforce the following
	//	"thresholds" on forwarding based on the TTL field:
	//
	//	0:   restricted to the same host
	//	1:   restricted to the same subnet
	//	32:  restricted to the same site
	//	64:  restricted to the same region
	//	128: restricted to the same continent
	//	255: unrestricted

	uint32 ttl = 32;
	char *ttlparam = m_System->CheckParam("-multicastttl");
	if (ttlparam) {
		ttl = atoi(ttlparam);
	}

	if (setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl)) == SOCKET_ERROR) {
		return false;
	}

	m_BytesOut = 0;
	m_BytesIn = 0;
	m_LastUpdateTime = 0;

	m_AvgBytesOut = 0;
	m_AvgBytesIn = 0;

	return true;
}

int NetSocket::GetPort()
{
	return m_Port;
}

void NetSocket::UpdateStats(double time)
{
	float timeDiff = time - m_LastUpdateTime;
	if (timeDiff > 0)
	{
		m_AvgBytesIn = m_BytesIn / timeDiff * 0.3 + m_AvgBytesIn * 0.6;
		m_AvgBytesOut = m_BytesOut / timeDiff * 0.3 + m_AvgBytesOut * 0.6;

		m_BytesIn = 0;
		m_BytesOut = 0;
		m_LastUpdateTime = time;
	}
}

int NetSocket::SendLong(const char *pData, int len, int flags, const sockaddr *to, int tolen)
{
	char packet[MAX_ROUTEABLE_PACKET];
	int totalSent, ret, size, packetCount, packetNumber;
	SPLITPACKET *pPacket;

	if (++m_netSplitSequenceNumber < 0) {
		m_netSplitSequenceNumber = 1;
	}

	pPacket = (SPLITPACKET *)packet;
	pPacket->netID = -2;
	pPacket->sequenceNumber = m_netSplitSequenceNumber;
	packetNumber = 0;
	totalSent = 0;
	packetCount = (len + SPLIT_SIZE - 1) / SPLIT_SIZE;

	while (len > 0)
	{
		size = min(SPLIT_SIZE, (unsigned)len);
		pPacket->packetID = (packetNumber << 4) + packetCount;
		memcpy(packet + sizeof(SPLITPACKET), pData + (packetNumber * SPLIT_SIZE), size);

		ret = sendto(m_Socket, packet, size + sizeof(SPLITPACKET), flags, to, tolen);
		if (ret < 0) {
			return ret;
		}

		if (ret >= size) {
			totalSent += size;
		}

		len -= size;
		packetNumber++;
	}

	return totalSent;
}

int NetSocket::SendShort(const char *pData, int len, int flags, const sockaddr *to, int tolen)
{
	if (len > MIN_ROUTEABLE_PACKET) {
		m_System->DPrintf("WARNING! NetSocket::SendShort: length > MIN_ROUTEABLE_PACKET.\n");
		return 0;
	}

	char packet[MAX_ROUTEABLE_PACKET];
	memcpy(packet, pData, len);
	memset(&packet[len], 0, sizeof(packet) - len);

	return sendto(m_Socket, packet, sizeof(packet), flags, to, tolen);
}
