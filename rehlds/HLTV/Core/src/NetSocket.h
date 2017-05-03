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

#include <HLTV/INetSocket.h>

class Network;
class IBaseSytem;

class NetSocket: public INetSocket {
public:
	NetSocket() : m_netSplitSequenceNumber(0) {}
	virtual ~NetSocket() {}

	NetPacket *ReceivePacket();
	void FreePacket(NetPacket *packet);
	bool SendPacket(NetPacket *packet);
	bool SendPacket(NetAddress *packet, const void *data, int length);
	void AddPacket(NetPacket *packet);
	bool AddChannel(INetChannel *channel);
	bool RemoveChannel(INetChannel *channel);
	INetwork *GetNetwork();
	void OutOfBandPrintf(NetAddress *to, const char *format, ...);
	void Flush();
	void GetFlowStats(float *avgInKBSec, float *avgOutKBSec);
	bool LeaveGroup(NetAddress *group);
	bool JoinGroup(NetAddress *group);
	void Close();
	int GetPort();

	bool Create(Network *network, int port, bool reuse, bool loopback);
	void UpdateStats(double time);
	int DrainChannels();
	int DispatchIncoming();

private:
	int ReceivePacketIntern(NetAddress *fromHost);
	int SendLong(const char *pData, int len, int flags, const sockaddr *to, int tolen);
	int SendShort(const char *pData, int len, int flags, const sockaddr *to, int tolen);
	int GetLong(unsigned char *pData, int size);

protected:
	int m_Port;
	unsigned char m_Buffer[MAX_UDP_PACKET];

	ObjectList m_IncomingPackets;
	ObjectList m_Channels;
	SOCKET m_Socket;
	Network *m_Network;
	IBaseSystem *m_System;

	enum { MAX_SPLIT_FRAGMENTS = 5 };
	// Use this to pick apart the network stream, must be packed
	#pragma pack(push, 1)
	typedef struct SPLITPACKET_t
	{
		int netID;
		int sequenceNumber;
		unsigned char packetID;
	} SPLITPACKET;
	#pragma pack(pop)

	// Split long packets. Anything over 1460 is failing on some routers.
	typedef struct LONGPACKET_t
	{
		int currentSequence;
		int splitCount;
		int totalSize;
		// TODO: It should be NET_MAX_MESSAGE, but value differs
		char buffer[MAX_UDP_PACKET];	// This has to be big enough to hold the largest message
	} LONGPACKET;

	LONGPACKET m_NetSplitPacket;

	int m_netSplitSequenceNumber;
	int m_netSplitFlags[MAX_SPLIT_FRAGMENTS];
	int m_BytesIn;
	int m_BytesOut;
	double m_LastUpdateTime;
	float m_AvgBytesIn;
	float m_AvgBytesOut;
};
