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

#include <HLTV/IWorld.h>
#include <HLTV/IServer.h>

#include "usercmd.h"
#include "ref_params.h"
#include "event_args.h"
#include "common/ServerInfo.h"

#include "vmodes.h"
#include "cdll_int.h"

const int MAX_DEMO_ENTRY = 1024;

const int DEMO_PROTOCOL  = 5;

const int DEMO_STARTUP   = 0;	// this lump contains startup info needed to spawn into the server
const int DEMO_NORMAL    = 1;	// this lump contains playback info of messages, etc., needed during playback.

enum class DemoCmd : unsigned char {
	Unknown = 0,
	NoRewind,					// startup message
	StartTime,
	StringCmd,
	ClientData,
	Read,
	Event,
	WeaponAnim,
	PlaySound,
	PayLoad
};

typedef struct demoheader_s {
	char szFileStamp[8];
	int nDemoProtocol;			// should be DEMO_PROTOCOL
	int nNetProtocolVersion;	// should be PROTOCOL_VERSION
	char szMapName[260];		// name of map
	char szDllDir[260];			// name of game directory
	CRC32_t mapCRC;
	int nDirectoryOffset;		// offset of Entry Directory.

} demoheader_t;

typedef struct demoentry_s {
	int nEntryType;				// DEMO_STARTUP or DEMO_NORMAL
	char szDescription[64];
	int nFlags;
	int nCDTrack;
	float fTrackTime;			// time of track
	int nFrames;				// # of frames in track
	int nOffset;				// file offset of track data
	int nFileLength;			// length of track
} demoentry_t;

typedef struct demo_info_s {
	float timestamp;
	ref_params_t rp;
	usercmd_t cmd;
	movevars_t movevars;
	vec3_t view;
	int viewmodel;
} demo_info_t;

class NetChannel;
class DemoFile {
public:
	DemoFile();
	virtual ~DemoFile() {}

	void Init(IWorld *world, IServer *server, NetChannel *channel);
	bool LoadDemo(char *demoname);
	void StopPlayBack();
	bool StartRecording(char *newName);
	void CloseFile();
	void Reset();
	void SetContinuous(bool state);
	bool IsContinuous();
	bool IsPlaying();
	bool IsRecording();
	char *GetFileName();
	void ReadDemoPacket(BitBuffer *demoData, demo_info_t *demoInfo);
	void WriteDemoMessage(BitBuffer *unreliableData, BitBuffer *reliableData);
	void WriteUpdateClientData(client_data_t *cdata);
	float GetDemoTime();
	void ReadSequenceInfo();
	void ReadDemoInfo(demo_info_t *demoInfo);
	void WriteDemoStartup(BitBuffer *buffer);
	void WriteSequenceInfo();
	void WriteDemoInfo(demo_info_t *demoInfo);
	void WriteSignonData();

	serverinfo_t m_ServerInfo;

private:
	char m_FileName[MAX_PATH];

	enum DemoState {
		DEMO_IDLE,
		DEMO_PLAYING,
		DEMO_RECORDING
	};

	int m_DemoState;
	unsigned int m_frameCount;
	FileHandle_t m_FileHandle;
	demoheader_t m_demoHeader;
	demoentry_t m_loadEntry;
	demoentry_t m_gameEntry;
	demo_info_t m_zeroDemoInfo;

	float m_startTime;
	float m_nextReadTime;

	NetChannel *m_DemoChannel;
	int m_StartPos;
	int m_EntryNumber;
	int m_CurrentEntry;
	demoentry_t *m_Entries;
	bool m_Continuous;

	IBaseSystem *m_System;
	IWorld *m_World;
	IServer *m_Server;
	IFileSystem *m_FileSystem;
};
