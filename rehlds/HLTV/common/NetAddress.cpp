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

NetAddress::NetAddress() :
	m_Port(0), m_IP(), m_String()
{
}

void NetAddress::SetPort(int16 port)
{
	m_Port = htons(port);
}

void NetAddress::ToSockadr(sockaddr *s)
{
	s->sa_family = AF_INET;
	((struct sockaddr_in *)s)->sin_addr.s_addr = *(int *)&m_IP;
	((struct sockaddr_in *)s)->sin_port = m_Port;
}

bool NetAddress::FromSockadr(sockaddr *s)
{
	if (s->sa_family != AF_INET) {
		return false;
	}

	*(int *)&m_IP = ((struct sockaddr_in *)s)->sin_addr.s_addr;
	m_Port = ((struct sockaddr_in *)s)->sin_port;

	return true;
}

bool NetAddress::Equal(NetAddress *a)
{
	if (*(int *)&m_IP == *(int *)&a->m_IP && m_Port == a->m_Port) {
		return true;
	}

	return false;
}

bool NetAddress::EqualBase(NetAddress *a)
{
	if (*(int *)&m_IP == *(int *)&a->m_IP) {
		return true;
	}

	return false;
}

void NetAddress::ToStream(BitBuffer *stream)
{
	stream->WriteByte(m_IP[0]);
	stream->WriteByte(m_IP[1]);
	stream->WriteByte(m_IP[2]);
	stream->WriteByte(m_IP[3]);
	stream->WriteShort(m_Port);
}

void NetAddress::FromStream(BitBuffer *stream)
{
	m_IP[0] = stream->ReadByte();
	m_IP[1] = stream->ReadByte();
	m_IP[2] = stream->ReadByte();
	m_IP[3] = stream->ReadByte();
	m_Port  = stream->ReadShort();
}

char *NetAddress::ToString()
{
	_snprintf(m_String, sizeof(m_String), "%i.%i.%i.%i:%i", m_IP[0], m_IP[1], m_IP[2], m_IP[3], ntohs(m_Port));
	return m_String;
}

char *NetAddress::ToBaseString()
{
	_snprintf(m_String, sizeof(m_String), "%i.%i.%i.%i", m_IP[0], m_IP[1], m_IP[2], m_IP[3]);
	return m_String;
}

void NetAddress::FromNetAddress(NetAddress *adr)
{
	if (!adr) {
		memset(m_IP, 0, sizeof(m_IP));
		m_Port = 0;
		return;
	}

	*(int *)&m_IP[0] = *(int *)&adr->m_IP[0];
	m_Port = adr->m_Port;
}

netadr_t *NetAddress::ToOldNetAdr()
{
	static netadr_t adr;

	adr.type = NA_IP;
	adr.port = m_Port;
	*(int *)&adr.ip[0] = *(int *)&m_IP[0];

	return &adr;
}

bool NetAddress::FromOldNetAdr(netadr_t *adr)
{
	if (adr->type != NA_IP) {
		return false;
	}

	*(int *)&m_IP[0] = *(int *)&adr->ip[0];
	m_Port = adr->port;
	return true;
}

bool NetAddress::IsValid()
{
	return m_Port && (m_IP[0] || m_IP[1] || m_IP[2] || m_IP[3]);
}

void NetAddress::Clear()
{
	m_IP[3] = 0;
	m_IP[2] = 0;
	m_IP[1] = 0;
	m_IP[0] = 0;
	m_Port = 0;

	memset(m_String, 0, sizeof(m_String));
}

bool NetAddress::IsSubAdress(NetAddress *adr)
{
	if ((!m_IP[0] || m_IP[0] == adr->m_IP[0])
		&& (!m_IP[1] || m_IP[1] == adr->m_IP[1])
		&& (!m_IP[2] || m_IP[2] == adr->m_IP[2])
		&& (!m_IP[3] || m_IP[3] == adr->m_IP[3])
		&& (m_IP[0] || m_IP[1] || m_IP[2] || m_IP[3]))
	{
		return true;
	}

	return false;
}
