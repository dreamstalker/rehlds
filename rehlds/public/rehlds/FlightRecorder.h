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

#include "archtypes.h"

class IRehldsFlightRecorder
{
public:
	virtual ~IRehldsFlightRecorder() { }

	virtual uint16 RegisterMessage(const char* module, const char *message, unsigned int version, bool inOut) = 0;
	
	virtual void StartMessage(uint16 msg, bool entrance) = 0;
	virtual void EndMessage(uint16 msg, bool entrance) = 0;
	
	virtual void WriteInt8(int8 v) = 0;
	virtual void WriteUInt8(uint8 v) = 0;
	
	virtual void WriteInt16(int16 v) = 0;
	virtual void WriteUInt16(uint16 v) = 0;

	virtual void WriteInt32(int32 v) = 0;
	virtual void WriteUInt32(uint32 v) = 0;

	virtual void WriteInt64(int64 v) = 0;
	virtual void WriteUInt64(uint64 v) = 0;

	virtual void WriteFloat(float v) = 0;
	virtual void WriteDouble(double v) = 0;

	virtual void WriteString(const char* s) = 0;

	virtual void WriteBuffer(const void* data ,unsigned int len) = 0;

};
