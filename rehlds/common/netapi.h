#ifndef NETAPI_H
#define NETAPI_H
#ifdef _WIN32
#pragma once
#endif

#include "netadr.h"

class INetAPI {
public:
	virtual void NetAdrToSockAddr(netadr_t *a, struct sockaddr *s) = 0;		// Convert a netadr_t to sockaddr
	virtual void SockAddrToNetAdr(struct sockaddr *s, netadr_t *a) = 0;		// Convert a sockaddr to netadr_t

	virtual char *AdrToString(netadr_t *a) = 0;					// Convert a netadr_t to a string
 	virtual bool StringToAdr(const char *s, netadr_t *a) = 0;			// Convert a string address to a netadr_t, doing DNS if needed
	virtual void GetSocketAddress(int socket, netadr_t *a) = 0;			// Look up IP address for socket
	virtual bool CompareAdr(netadr_t *a, netadr_t *b) = 0;

	// return the IP of the local host
	virtual void GetLocalIP(netadr_t *a) = 0;
};

extern INetAPI *net;

#endif // NETAPI_H
