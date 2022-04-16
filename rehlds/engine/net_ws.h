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

#include "maintypes.h"
#include "enums.h"
#include "common.h"
#include "netadr.h"

#ifndef _WIN32

#define WSAEWOULDBLOCK		EWOULDBLOCK	// Operation would block EAGAIN (11)
#define WSAEMSGSIZE		EMSGSIZE	// Message too long (90)
#define WSAEADDRNOTAVAIL	EADDRNOTAVAIL	// Cannot assign requested address (99)
#define WSAEAFNOSUPPORT		EAFNOSUPPORT	// Address family not supported by protocol (97)
#define WSAECONNRESET		ECONNRESET	// Connection reset by peer (104)
#define WSAECONNREFUSED		ECONNREFUSED	// Connection refused (111)
#define WSAEADDRINUSE		EADDRINUSE	// Address already in use (98)
#define WSAENOBUFS		ENOBUFS		// No buffer space available (105)

#endif // _WIN32

const int MAX_ROUTEABLE_PACKET = 1400;

#define SPLIT_SIZE				(MAX_ROUTEABLE_PACKET - sizeof(SPLITPACKET))

// Create general message queues
const int NUM_MSG_QUEUES = 40;
const int MSG_QUEUE_SIZE = 1536;

const int NET_HEADER_FLAG_SPLITPACKET = -2;

typedef struct loopmsg_s
{
	unsigned char data[NET_MAX_MESSAGE];
	int datalen;
} loopmsg_t;

const int MAX_LOOPBACK = 4;

typedef struct loopback_s
{
	loopmsg_t msgs[MAX_LOOPBACK];
	int get;
	int send;
} loopback_t;

typedef struct packetlag_s
{
	unsigned char *pPacketData;	// Raw stream data is stored.
	int nSize;
	netadr_t net_from_;
	float receivedTime;
	struct packetlag_s *pNext;
	struct packetlag_s *pPrev;
} packetlag_t;

typedef struct net_messages_s
{
	struct net_messages_s *next;
	qboolean preallocated;
	unsigned char *buffer;
	netadr_t from;
	int buffersize;
} net_messages_t;

// Split long packets. Anything over 1460 is failing on some routers.
typedef struct LONGPACKET_t
{
	int currentSequence;
	int splitCount;
	int totalSize;
	// TODO: It should be NET_MAX_MESSAGE, but value differs
	char buffer[MAX_UDP_PACKET];	// This has to be big enough to hold the largest message
} LONGPACKET;

// Use this to pick apart the network stream, must be packed
#pragma pack(push, 1)
typedef struct SPLITPACKET_t
{
	int netID;
	int sequenceNumber;
	unsigned char packetID;
} SPLITPACKET;
#pragma pack(pop)

const int NET_WS_MAX_FRAGMENTS = 5;

extern qboolean net_thread_initialized;
extern cvar_t net_address;
extern cvar_t ipname;
extern cvar_t defport;
extern cvar_t ip_clientport;
extern cvar_t clientport;
extern int net_sleepforever;
extern loopback_t loopbacks[2];
extern packetlag_t g_pLagData[3];
extern float gFakeLag;
extern int net_configured;
#ifdef _WIN32
extern netadr_t net_local_ipx_adr;
#endif // _WIN32
extern netadr_t net_local_adr;
extern netadr_t net_from;
extern qboolean noip;
#ifdef _WIN32
extern qboolean noipx;
#endif // _WIN32
extern sizebuf_t net_message;
extern cvar_t clockwindow;
extern qboolean use_thread;
extern cvar_t iphostport;
extern cvar_t hostport;
#ifdef _WIN32
extern cvar_t ipx_hostport;
extern cvar_t ipx_clientport;
#endif // _WIN32
extern cvar_t multicastport;
extern cvar_t fakelag;
extern cvar_t fakeloss;
extern cvar_t net_graph;
extern cvar_t net_graphwidth;
extern cvar_t net_scale;
extern cvar_t net_graphpos;
extern unsigned char net_message_buffer[NET_MAX_PAYLOAD];
extern unsigned char in_message_buf[NET_MAX_PAYLOAD];
extern sizebuf_t in_message;
extern netadr_t in_from;
extern SOCKET ip_sockets[3];
#ifdef _WIN32
extern SOCKET ipx_sockets[3];
#endif // _WIN32
extern LONGPACKET gNetSplit;
extern net_messages_t *messages[3];
extern net_messages_t *normalqueue;


void NET_ThreadLock();
void NET_ThreadUnlock();
unsigned short Q_ntohs(unsigned short netshort);
void NetadrToSockadr(const netadr_t *a, struct sockaddr *s);
void SockadrToNetadr(const struct sockaddr *s, netadr_t *a);
NOXREF unsigned short NET_HostToNetShort(unsigned short us_in);
qboolean NET_CompareAdr(netadr_t& a, netadr_t& b);
qboolean NET_CompareClassBAdr(netadr_t& a, netadr_t& b);
qboolean NET_IsReservedAdr(netadr_t& a);
qboolean NET_CompareBaseAdr(const netadr_t& a, const netadr_t& b);
char *NET_AdrToString(const netadr_t& a);
char *NET_BaseAdrToString(netadr_t& a);
qboolean NET_StringToSockaddr(const char *s, struct sockaddr *sadr);
qboolean NET_StringToAdr(const char *s, netadr_t *a);
qboolean NET_IsLocalAddress(const netadr_t& adr);
char *NET_ErrorString(int code);
void NET_TransferRawData(sizebuf_t *msg, unsigned char *pStart, int nSize);
qboolean NET_GetLoopPacket(netsrc_t sock, netadr_t *in_from_, sizebuf_t *msg);
void NET_SendLoopPacket(netsrc_t sock, int length, void *data, const netadr_t& to);
void NET_RemoveFromPacketList(packetlag_t *pPacket);
NOXREF int NET_CountLaggedList(packetlag_t *pList);
void NET_ClearLaggedList(packetlag_t *pList);
void NET_AddToLagged(netsrc_t sock, packetlag_t *pList, packetlag_t *pPacket, netadr_t *net_from_, sizebuf_t messagedata, float timestamp);
void NET_AdjustLag();
qboolean NET_LagPacket(qboolean newdata, netsrc_t sock, netadr_t *from, sizebuf_t *data);
void NET_FlushSocket(netsrc_t sock);
qboolean NET_GetLong(unsigned char *pData, int size, int *outSize);
qboolean NET_QueuePacket(netsrc_t sock);
int NET_Sleep();
void NET_StartThread();
void NET_StopThread();
void *net_malloc(size_t size);
net_messages_t *NET_AllocMsg(int size);
void NET_FreeMsg(net_messages_t *pmsg);
qboolean NET_GetPacket(netsrc_t sock);
void NET_AllocateQueues();
void NET_FlushQueues();
int NET_SendLong(netsrc_t sock, SOCKET s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen);
void NET_SendPacket_api(unsigned int length, void *data, const netadr_t &to);
void NET_SendPacket(netsrc_t sock, int length, void *data, const netadr_t& to);
SOCKET NET_IPSocket(char *net_interface, int port, qboolean multicast);
void NET_OpenIP();
SOCKET NET_IPXSocket(int hostshort);
void NET_OpenIPX();
void NET_GetLocalAddress();
int NET_IsConfigured();
bool NET_CheckPort(int port);
void NET_Config(qboolean multiplayer);
void MaxPlayers_f();
void NET_Init();
void NET_ClearLagData(qboolean bClient, qboolean bServer);
void NET_Shutdown();
NOXREF qboolean NET_JoinGroup(netsrc_t sock, netadr_t& addr);
NOXREF qboolean NET_LeaveGroup(netsrc_t sock, netadr_t& addr);
