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

class INetSocket;
class IBaseSystem;
class INetChannel {
public:
	virtual ~INetChannel() {}

	virtual bool Create(IBaseSystem *system, INetSocket *netsocket = nullptr, NetAddress *adr = nullptr) = 0;
	virtual NetAddress *GetTargetAddress() = 0;
	virtual void Close() = 0;
	virtual void Clear() = 0;
	virtual void Reset() = 0;
	virtual bool IsConnected() = 0;
	virtual bool IsReadyToSend() = 0;
	virtual bool IsCrashed() = 0;
	virtual bool IsTimedOut() = 0;
	virtual bool IsFakeChannel() = 0;
	virtual bool KeepAlive() = 0;
	virtual void SetRate(int newRate) = 0;
	virtual void SetUpdateRate(int newupdaterate) = 0;
	virtual void SetTimeOut(float time) = 0;
	virtual void SetKeepAlive(bool flag) = 0;
	virtual float GetIdleTime() = 0;
	virtual int GetRate() = 0;
	virtual int GetUpdateRate() = 0;
	virtual float GetLoss() = 0;
	virtual void TransmitOutgoing() = 0;
	virtual void ProcessIncoming(unsigned char *data, int size) = 0;
	virtual void OutOfBandPrintf(const char *format, ...) = 0;
	virtual void FakeAcknowledgement() = 0;
};
