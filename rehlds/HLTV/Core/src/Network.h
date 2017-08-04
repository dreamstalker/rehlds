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

#include <HLTV/INetwork.h>
#include "BaseSystemModule.h"

#ifndef _WIN32
#define WSAEWOULDBLOCK          EWOULDBLOCK      // Operation would block EAGAIN (11)
#define WSAEMSGSIZE             EMSGSIZE         // Message too long (90)
#define WSAEADDRNOTAVAIL        EADDRNOTAVAIL    // Cannot assign requested address (99)
#define WSAEAFNOSUPPORT         EAFNOSUPPORT     // Address family not supported by protocol (97)
#define WSAECONNRESET           ECONNRESET       // Connection reset by peer (104)
#define WSAECONNREFUSED         ECONNREFUSED     // Connection refused (111)
#define WSAEADDRINUSE           EADDRINUSE       // Address already in use (98)
#define WSAENOBUFS              ENOBUFS          // No buffer space available (105)

#define WSAEINTR                EINTR            // Interrupted system call
#define WSAEBADF                EBADF            // Bad file number
#define WSAEACCES               EACCES           // Permission denied
#define WSAEFAULT               EFAULT           // Bad address
#define WSAEINVAL               EINVAL           // Invalid argument
#define WSAEMFILE               EMFILE           // Too many open files
#define WSAEWOULDBLOCK          EWOULDBLOCK      // Operation would block
#define WSAEINPROGRESS          EINPROGRESS      // Operation now in progress
#define WSAEALREADY             EALREADY         // Operation already in progress
#define WSAENOTSOCK             ENOTSOCK         // Socket operation on non-socket
#define WSAEDESTADDRREQ         EDESTADDRREQ     // Destination address required
#define WSAEMSGSIZE             EMSGSIZE         // Message too long
#define WSAEPROTOTYPE           EPROTOTYPE       // Protocol wrong type for socket
#define WSAENOPROTOOPT          ENOPROTOOPT      // Protocol not available
#define WSAEPROTONOSUPPORT      EPROTONOSUPPORT  // Protocol not supported
#define WSAESOCKTNOSUPPORT      ESOCKTNOSUPPORT  // Socket type not supported
#define WSAEOPNOTSUPP           EOPNOTSUPP       // Operation not supported on transport endpoint
#define WSAEPFNOSUPPORT         EPFNOSUPPORT     // Protocol family not supported
#define WSAEAFNOSUPPORT         EAFNOSUPPORT     // Address family not supported by protocol
#define WSAEADDRINUSE           EADDRINUSE       // Address already in use
#define WSAEADDRNOTAVAIL        EADDRNOTAVAIL    // Cannot assign requested address
#define WSAENETDOWN             ENETDOWN         // Network is down
#define WSAENETUNREACH          ENETUNREACH      // Network is unreachable
#define WSAENETRESET            ENETRESET        // Network dropped connection because of reset
#define WSAECONNABORTED         ECONNABORTED     // Software caused connection abort
#define WSAECONNRESET           ECONNRESET       // Connection reset by peer
#define WSAENOBUFS              ENOBUFS          // No buffer space available
#define WSAEISCONN              EISCONN          // Transport endpoint is already connected
#define WSAENOTCONN             ENOTCONN         // Transport endpoint is not connected
#define WSAESHUTDOWN            ESHUTDOWN        // Cannot send after transport endpoint shutdown
#define WSAETOOMANYREFS         ETOOMANYREFS     // Too many references: cannot splice
#define WSAETIMEDOUT            ETIMEDOUT        // Connection timed out
#define WSAECONNREFUSED         ECONNREFUSED     // Connection refused
#define WSAELOOP                ELOOP            // Too many symbolic links encountered
#define WSAENAMETOOLONG         ENAMETOOLONG     // File name too long
#define WSAEHOSTDOWN            EHOSTDOWN        // Host is down
#endif // _WIN32

class NetSocket;
class Network: public BaseSystemModule, public INetwork {
public:
	Network();
	virtual ~Network() {}

	EXT_FUNC bool Init(IBaseSystem *system, int serial, char *name);
	EXT_FUNC void RunFrame(double time);
	EXT_FUNC void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data);
	EXT_FUNC void ExecuteCommand(int commandID, char *commandLine);
	EXT_FUNC void RegisterListener(ISystemModule *module);
	EXT_FUNC void RemoveListener(ISystemModule *module);
	EXT_FUNC IBaseSystem *GetSystem();
	EXT_FUNC int GetSerial();
	EXT_FUNC char *GetStatusLine();
	EXT_FUNC char *GetType();
	EXT_FUNC char *GetName();
	EXT_FUNC int GetState();
	EXT_FUNC int GetVersion();
	EXT_FUNC void ShutDown();

	EXT_FUNC INetSocket *CreateSocket(int port, bool reuse, bool loopback);
	EXT_FUNC bool RemoveSocket(INetSocket *netsocket);
	EXT_FUNC NetAddress *GetLocalAddress();
	EXT_FUNC bool ResolveAddress(char *string, NetAddress *address);
	EXT_FUNC void GetFlowStats(float *totalIn, float *totalOut);
	EXT_FUNC int GetLastErrorCode();
	EXT_FUNC char *GetErrorText(int code);

protected:
	enum LocalCommandIDs { CMD_ID_FAKELOSS = 1 };
	void CMD_FakeLoss(char *cmdLine);

	void SetName(char *newName);
	void UpdateStats();
	NetSocket *GetSocket(int port);
	int SendData();
	int ReceiveData();

protected:
	friend class NetSocket;

	float m_FakeLoss;
	NetAddress m_LocalAddress;
	bool m_IsMultihomed;
	bool m_NoDNS;
	double m_LastStatsUpdateTime;
	ObjectList m_Sockets;
};
