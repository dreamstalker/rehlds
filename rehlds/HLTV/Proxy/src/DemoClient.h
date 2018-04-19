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

#include "common/InfoString.h"

class DemoClient: public IClient, public BaseSystemModule {
public:
	DemoClient();
	virtual ~DemoClient() {}

	bool Init(IBaseSystem *system, int serial, char *name);
	void RunFrame(double time);
	char *GetStatusLine();
	char *GetType();
	void ShutDown();

	bool Connect(INetSocket *socket = nullptr, NetAddress *adr = nullptr, char *userinfo = "");
	void Send(unsigned char *data, int length, bool isReliable);
	void Disconnect(const char *reason);
	void Reconnect();
	void SetWorld(IWorld *world);
	int GetClientType();
	char *GetClientName();
	InfoString *GetUserInfo();
	NetAddress *GetAddress();
	bool IsActive();
	bool IsHearingVoices();
	bool HasChatEnabled();

	void SetProxy(IProxy *proxy);
	void SetFileName(char *fileName);
	DemoFile *GetDemoFile();
	void FinishDemo();
	void SendDatagram();
	void WriteDatagram(double time, frame_t *frame);

protected:
	IProxy *m_Proxy;
	IWorld *m_World;
	bool m_IsActive;
	NetChannel m_DemoChannel;

	enum { MAX_DEMO_INFO = 256 };
	DemoFile m_DemoFile;
	char m_BaseFileName[MAX_PATH];

	InfoString m_DemoInfo;
	unsigned int m_LastFrameSeqNr;
	unsigned int m_ClientDelta;
};

#define DEMOCLIENT_INTERFACE_VERSION "democlient000"
