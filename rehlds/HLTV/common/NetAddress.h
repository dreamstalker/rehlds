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

#include "netadr.h"
#include "BitBuffer.h"

class NetAddress {
public:
	NetAddress();

	bool IsValid();
	bool IsSubAdress(NetAddress *adr);

	void Clear();

	bool FromOldNetAdr(netadr_t *adr);
	netadr_t *ToOldNetAdr();

	void FromNetAddress(NetAddress *adr);
	char *ToBaseString();
	char *ToString();

	bool EqualBase(NetAddress *a);
	bool Equal(NetAddress *a);
	bool FromSockadr(struct sockaddr *s);
	void ToSockadr(struct sockaddr *s);
	void SetPort(int16 port);
	void ToStream(BitBuffer *stream);
	void FromStream(BitBuffer *stream);

	virtual ~NetAddress() {}

public:
	unsigned char m_IP[4];
	short unsigned int m_Port;
	char m_String[32];
};

class NetPacket {
public:
	NetAddress address;
	BitBuffer data;
	double time;
	bool connectionless;
	unsigned int seqnr;
	bool hasReliableData;
};
