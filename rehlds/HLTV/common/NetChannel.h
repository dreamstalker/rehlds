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
#include <HLTV/INetChannel.h>

#include "bzip2/bzlib.h"
#include "ObjectList.h"

// 0 == regular, 1 == file stream
enum
{
	FRAG_NORMAL_STREAM = 0,
	FRAG_FILE_STREAM,

	MAX_STREAMS
};

enum
{
	FLOW_OUTGOING = 0,
	FLOW_INCOMING,

	MAX_FLOWS
};

#define MAX_LATENT				32
#define FRAGMENT_MAX_SIZE		1400		// Size of fragmentation buffer internal buffers

#define UDP_HEADER_SIZE			28
#define MAX_RELIABLE_PAYLOAD	1200

#define MAKE_FRAGID(id, count)	(((id & 0xffff) << 16) | (count & 0xffff))
#define FRAG_GETID(fragid)		((fragid >> 16) & 0xffff)
#define FRAG_GETCOUNT(fragid)	(fragid & 0xffff)

// Max length of a reliable message
#define	MAX_MSGLEN				3990		// 10 reserved for fragheader?
#define MAX_POSSIBLE_MSG		65536

#define MAX_ROUTEABLE_PACKET	1400
#define MIN_ROUTEABLE_PACKET	16

#define SPLIT_SIZE				(MAX_ROUTEABLE_PACKET - sizeof(SPLITPACKET))

// Pad this to next higher 16 byte boundary
// This is the largest packet that can come in/out over the wire, before processing the header
// bytes will be stripped by the networking channel layer
// #define NET_MAX_MESSAGE PAD_NUMBER( ( MAX_MSGLEN + HEADER_BYTES ), 16 )
// This is currently used value in the engine. TODO: define above gives 4016, check it why.
#define NET_MAX_MESSAGE 4037
#define NET_HEADER_FLAG_SPLITPACKET -2

class IBaseSystem;

// Network Connection Channel
class NetChannel: public INetChannel {
public:
	NetChannel();
	virtual ~NetChannel() {}

	virtual bool Create(IBaseSystem *system, INetSocket *netsocket = nullptr, NetAddress *adr = nullptr);
	virtual bool IsConnected();
	virtual bool IsReadyToSend();
	virtual bool IsCrashed();
	virtual bool IsTimedOut();
	virtual bool IsFakeChannel();
	virtual bool KeepAlive();
	virtual NetAddress *GetTargetAddress();
	virtual void Close();
	virtual void Clear();
	virtual void Reset();
	virtual void TransmitOutgoing();
	virtual void ProcessIncoming(unsigned char *data, int size);
	virtual void OutOfBandPrintf(const char *format, ...);
	virtual void FakeAcknowledgement();
	virtual void SetUpdateRate(int newupdaterate);
	virtual void SetRate(int newRate);
	virtual void SetKeepAlive(bool flag);
	virtual void SetTimeOut(float time);
	virtual float GetIdleTime();
	virtual int GetRate();
	virtual int GetUpdateRate();
	virtual float GetLoss();

	enum { FRAGMENT_C2S_MIN_SIZE = 16, FRAGMENT_S2C_MIN_SIZE = 256, FRAGMENT_S2C_MAX_SIZE = 1024 };

	// Message data
	typedef struct flowstats_s
	{
		int size;		// Size of message sent/received
		double time;		// Time that message was sent/received
	} flowstats_t;

	typedef struct flow_s
	{
		flowstats_t stats[MAX_LATENT];		// Data for last MAX_LATENT messages
		int current;				// Current message position
		double nextcompute;			// Time when we should recompute k/sec data

		// Average data
		float kbytespersec;
		float avgkbytespersec;
	} flow_t;

	// Generic fragment structure
	typedef struct fragbuf_s
	{
		struct fragbuf_s *next;			// Next buffer in chain
		int bufferId;				// Id of this buffer
		byte data[FRAGMENT_MAX_SIZE];		// The actual data sits here

		int size;				// Size of data to read at that offset
		bool isfile;				// Is this a file buffer?
		bool isbuffer;				// Is this file buffer from memory ( custom decal, etc. ).
		char fileName[MAX_PATH];		// Name of the file to save out on remote host
		int fOffset;				// Offset in file from which to read data
	} fragbuf_t;

	// Waiting list of fragbuf chains
	typedef struct fragbufwaiting_s
	{
		struct fragbufwaiting_s *next;		// Next chain in waiting list
		int fragbufcount;			// Number of buffers in this chain
		fragbuf_t *fragbufs;			// The actual buffers
	} fragbufwaiting_t;

	bool CreateFragmentsFromFile(char *fileName);
	bool CopyFileFragments();
	void GetFlowStats(float *avgInKBSec, float *avgOutKBSec);
	void SetConnected(bool flag);
	void FlushOutgoing();
	void CopyNormalFragments();
	void UpdateFlow(int stream);
	void FlushIncoming(int stream);
	void ClearFragments();
	void ClearFragbufs(fragbuf_t **ppbuf);
	void UnlinkFragment(fragbuf_t *buf, int stream);
	NetPacket *GetPacket();
	void FreePacket(NetPacket *packet);
	void AddFragbufToTail(fragbufwaiting_t *wait, fragbuf_t *buf);
	bool CreateFragmentsFromBuffer(void *buffer, int size, int streamtype, char *filename = nullptr);
	void AddBufferToList(fragbuf_t ** pplist, fragbuf_t *pbuf);
	void FragSend();
	bool CheckForCompletion(int stream, int intotalbuffers);
	fragbuf_t *FindBufferById(fragbuf_t **pplist, int id, bool allocate);

public:
	IBaseSystem *m_System;
	INetSocket *m_Socket;

	NetAddress m_remote_address;		// Address this channel is talking to.
	double m_last_received;
	double m_last_send;
	double m_connect_time;			// Time when channel was connected.
	float m_timeout;
	int m_max_bandwidth_rate;
	double m_send_interval;
	int m_updaterate;			// Bandwidth choke, bytes per second
	double m_cleartime;			// If realtime > cleartime, free to send next packet

	bool m_keep_alive;
	bool m_crashed;
	bool m_connected;

	// Sequencing variables
	int m_incoming_sequence;		// Increasing count of sequence numbers
	int m_incoming_acknowledged;		// # of last outgoing message that has been ack'd.
	int m_incoming_reliable_acknowledged;	// Toggles T/F as reliable messages are received.
	int m_incoming_reliable_sequence;	// single bit, maintained local
	int m_outgoing_sequence;		// Message we are sending to remote
	int m_reliable_sequence;		// Whether the message contains reliable payload, single bit
	int m_last_reliable_sequence;		// Outgoing sequence number of last send that had reliable data

	void *m_connection_status;

	int m_blocksize;
	BitBuffer m_reliableStream;
	BitBuffer m_unreliableStream;
	ObjectList m_incomingPackets;

	// Reliable message buffer.
	// We keep adding to it until reliable is acknowledged. Then we clear it.
	int m_reliableOutSize;
	unsigned char m_reliableOutBuffer[MAX_MSGLEN + 20];

	fragbufwaiting_t *m_waitlist[MAX_STREAMS];		// Waiting list of buffered fragments to go onto queue. Multiple outgoing buffers can be queued in succession.

	int m_reliable_fragment[MAX_STREAMS];			// Is reliable waiting buf a fragment?
	size_t m_reliable_fragid[MAX_STREAMS];			// Buffer id for each waiting fragment

	fragbuf_t *m_fragbufs[MAX_STREAMS];			// The current fragment being set
	int m_fragbufcount[MAX_STREAMS];			// The total number of fragments in this stream

	int16 m_frag_startpos[MAX_STREAMS];			// Position in outgoing buffer where frag data starts
	int16 m_frag_length[MAX_STREAMS];			// Length of frag data in the buffer

	fragbuf_t *m_incomingbufs[MAX_STREAMS];			// Incoming fragments are stored here

	// Only referenced by the FRAG_FILE_STREAM component
	// Name of file being downloaded
	char m_incomingfilename[MAX_PATH];

	void *m_tempBuffer;
	int m_tempBufferSize;

	// Incoming and outgoing flow metrics
	flow_t m_flow[MAX_FLOWS];
	float m_loss;
};
