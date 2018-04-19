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

#ifdef HOOK_HLTV

// Hooks stuff
#include "hookers/memory.cpp"
#include "hookers/hooker.cpp"

//#define Mem_region
//#define Proxy_region
//#define BaseClient_region
//#define ProxyClient_region
//#define FakeClient_region
//#define DemoFile_region
//#define DemoClient_region
//#define Director_region
//#define DirectorCmd_region
//#define Status_region
//#define InfoString_region
//#define BitBuffer_region
//#define NetChannel_region
//#define Master_region
//#define Function_References_region
//#define Data_References_region

FunctionHook g_FunctionHooks[] =
{
	// DO NOT DISABLE, other functions depends on memory allocation routines
#ifndef Mem_region

	HOOK_DEF(0x01D20F3F, malloc_wrapper),
	HOOK_DEF(0x01D20E2E, free_wrapper),
	HOOK_DEF(0x01D3078C, strdup_wrapper),
	HOOK_DEF(0x01D20F51, __nh_malloc_wrapper),

	HOOK_DEF(0x01D03EF0, Mem_ZeroMalloc),
	//HOOK_DEF(0x0, Mem_Malloc),
	//HOOK_DEF(0x0, Mem_Realloc),
	//HOOK_DEF(0x0, Mem_Calloc),
	//HOOK_DEF(0x0, Mem_Strdup),
	//HOOK_DEF(0x0, Mem_Free),

	//HOOK_DEF(0x0, realloc_wrapper),
	//HOOK_DEF(0x0, calloc_wrapper),

#endif // Mem_region

#ifndef Proxy_region

	// virtual functions - BaseSystemModule
	HOOK_VIRTUAL_DEF(0x01D16010, Proxy::Init),
	HOOK_VIRTUAL_DEF(0x01D16020, Proxy::RunFrame),
	HOOK_VIRTUAL_DEF(0x01D16030, Proxy::ReceiveSignal),
	HOOK_VIRTUAL_DEF(0x01D16040, Proxy::ExecuteCommand),
	HOOK_VIRTUAL_DEF(0x01D16090, Proxy::GetStatusLine),
	HOOK_VIRTUAL_DEF(0x01D160A0, Proxy::GetType),
	HOOK_VIRTUAL_DEF(0x01D160E0, Proxy::ShutDown),

	// virtual functions - Proxy
	HOOK_VIRTUAL_DEF(0x01D115D0, Proxy::Reset),
	HOOK_VIRTUAL_DEF(0x01D11620, Proxy::Broadcast),
	HOOK_VIRTUAL_DEF(0x01D14FE0, Proxy::IncreaseCheering),
	HOOK_VIRTUAL_DEF(0x01D11600, Proxy::ParseStatusMsg),
	HOOK_VIRTUAL_DEF(0x01D11610, Proxy::ParseStatusReport),
	HOOK_VIRTUAL_DEF(0x01D12FF0, Proxy::ProcessConnectionlessMessage),
	HOOK_VIRTUAL_DEF(0x01D14780, Proxy::ChatCommentator),
	HOOK_VIRTUAL_DEF(0x01D137B0, Proxy::ChatSpectator),
	HOOK_VIRTUAL_DEF(0x01D11370, Proxy::CountLocalClients),
	HOOK_VIRTUAL_DEF(0x01D143B0, Proxy::AddResource),
	HOOK_VIRTUAL_DEF(0x01D11570, Proxy::IsLanOnly),
	HOOK_VIRTUAL_DEF(0x01D13A00, Proxy::IsMaster),
	HOOK_VIRTUAL_DEF(0x01D11580, Proxy::IsActive),
	HOOK_VIRTUAL_DEF(0x01D114F0, Proxy::IsPublicGame),
	HOOK_VIRTUAL_DEF(0x01D11500, Proxy::IsPasswordProtected),
	HOOK_VIRTUAL_DEF(0x01D11510, Proxy::IsStressed),
	HOOK_VIRTUAL_DEF(0x01D154A0, Proxy::SetDelay),
	HOOK_VIRTUAL_DEF(0x01D15540, Proxy::SetClientTime),
	HOOK_VIRTUAL_DEF(0x01D15580, Proxy::SetClientTimeScale),
	HOOK_VIRTUAL_DEF(0x01D15430, Proxy::SetMaxRate),
	HOOK_VIRTUAL_DEF(0x01D118D0, Proxy::SetMaxLoss),
	HOOK_VIRTUAL_DEF(0x01D15470, Proxy::SetMaxUpdateRate),
	HOOK_VIRTUAL_DEF(0x01D11870, Proxy::SetMaxClients),
	HOOK_VIRTUAL_DEF(0x01D11930, Proxy::SetRegion),
	HOOK_VIRTUAL_DEF(0x01D14FA0, Proxy::GetDelay),
	HOOK_VIRTUAL_DEF(0x01D14FC0, Proxy::GetSpectatorTime),
	HOOK_VIRTUAL_DEF(0x01D14FD0, Proxy::GetProxyTime),
	HOOK_VIRTUAL_DEF(0x01D11920, Proxy::GetMaxClients),
	HOOK_VIRTUAL_DEF(0x01D10EF0, Proxy::GetWorld),
	HOOK_VIRTUAL_DEF(0x01D10F00, Proxy::GetServer),
	HOOK_VIRTUAL_DEF(0x01D10F10, Proxy::GetDirector),
	HOOK_VIRTUAL_DEF(0x01D10F20, Proxy::GetSocket),
	HOOK_VIRTUAL_DEF(0x01D10F30, Proxy::GetChatMode),
	HOOK_VIRTUAL_DEF(0x01D113C0, Proxy::GetStatistics),
	HOOK_VIRTUAL_DEF(0x01D113D0, Proxy::GetMaxRate),
	HOOK_VIRTUAL_DEF(0x01D113E0, Proxy::GetMaxUpdateRate),
	HOOK_VIRTUAL_DEF(0x01D14420, Proxy::GetResource),
	HOOK_VIRTUAL_DEF(0x01D11550, Proxy::GetDispatchMode),
	HOOK_VIRTUAL_DEF(0x01D115A0, Proxy::GetRegion),
	HOOK_VIRTUAL_DEF(0x01D10EE0, Proxy::GetClients),
	HOOK_VIRTUAL_DEF(0x01D113F0, Proxy::WriteSignonData),

	// non-virtual functions - Proxy
	HOOK_DEF(0x01D12BD0, Proxy::ReplyServiceChallenge),
	HOOK_DEF(0x01D115B0, Proxy::ReplyListen),
	HOOK_DEF(0x01D10900, Proxy::ReplyConnect),
	HOOK_DEF(0x01D10890, Proxy::ReplyRules),
	HOOK_DEF(0x01D10820, Proxy::ReplyPlayers),
	HOOK_DEF(0x01D10770, Proxy::ReplyInfo),
	HOOK_DEF(0x01D10720, Proxy::ReplyInfoString),
	HOOK_DEF(0x01D106D0, Proxy::ReplyChallenge),
	HOOK_DEF(0x01D105F0, Proxy::ReplyPing),
	HOOK_DEF(0x01D12F50, Proxy::ExecuteRcon),
	HOOK_DEF(0x01D13B70, Proxy::ReconnectClients),
	HOOK_DEF(0x01D129A0, Proxy::SendRcon),
	HOOK_DEF(0x01D10DD0, Proxy::RejectConnection),
	HOOK_DEF(0x01D10F40, Proxy::UpdateStatusLine),
	HOOK_DEF(0x01D14220, Proxy::DispatchClient),
	HOOK_DEF(0x01D13FA0, Proxy::IsValidPassword),
	HOOK_DEF(0x01D13E50, Proxy::WriteHUDMsg),
	HOOK_DEF(0x01D150D0, Proxy::ExecuteLoopCommands),
	HOOK_DEF(0x01D12A30, Proxy::GetChallengeNumber),
	HOOK_DEF(0x01D12B60, Proxy::CheckChallenge),
	HOOK_DEF(0x01D15150, Proxy::CreateServerInfoString),
	HOOK_DEF(0x01D13A10, Proxy::CheckDirectorModule),
	HOOK_DEF(0x01D158B0, Proxy::RunClocks),
	HOOK_DEF(0x01D156B0, Proxy::NewGameStarted),
	HOOK_DEF(0x01D12CC0, Proxy::NewServerConnection),
	HOOK_DEF(0x01D15620, Proxy::BroadcastPaused),
	HOOK_DEF(0x01D12E80, Proxy::BroadcastRetryMessage),
	HOOK_DEF(0x01D12EE0, Proxy::StopBroadcast),
	HOOK_DEF(0x01D15A10, Proxy::DisconnectClients),
	HOOK_DEF(0x01D14350, Proxy::FreeResource),
	HOOK_DEF(0x01D14380, Proxy::ClearResources),
	HOOK_DEF(0x01D142C0, Proxy::LoadResourceFromFile),
	HOOK_DEF(0x01D14800, Proxy::IsBanned),
	HOOK_DEF(0x01D15A60, Proxy::UpdateInfoMessages),

	HOOK_DEF(0x01D12860, Proxy::CMD_Rcon),
	HOOK_DEF(0x01D11EF0, Proxy::CMD_ServerCmd),
	HOOK_DEF(0x01D11F90, Proxy::CMD_ClientCmd),
	HOOK_DEF(0x01D127D0, Proxy::CMD_RconPassword),
	HOOK_DEF(0x01D12700, Proxy::CMD_RconAddress),
	HOOK_DEF(0x01D11700, Proxy::CMD_Say),
	HOOK_DEF(0x01D11C10, Proxy::CMD_Msg),
	HOOK_DEF(0x01D10D40, Proxy::CMD_Clients),
	HOOK_DEF(0x01D13920, Proxy::CMD_Kick),
	HOOK_DEF(0x01D13650, Proxy::CMD_ChatMode),
	HOOK_DEF(0x01D13DA0, Proxy::CMD_PublicGame),
	HOOK_DEF(0x01D13BB0, Proxy::CMD_OffLineText),
	HOOK_DEF(0x01D14550, Proxy::CMD_AdminPassword),
	HOOK_DEF(0x01D14470, Proxy::CMD_SignOnCommands),
	HOOK_DEF(0x01D13EE0, Proxy::CMD_SpectatorPassword),
	HOOK_DEF(0x01D140B0, Proxy::CMD_DispatchMode),
	HOOK_DEF(0x01D14F00, Proxy::CMD_CheeringThreshold),
	HOOK_DEF(0x01D10610, Proxy::CMD_Ping),
	HOOK_DEF(0x01D12C00, Proxy::CMD_ProxyPassword),
	HOOK_DEF(0x01D11E50, Proxy::CMD_MaxRate),
	HOOK_DEF(0x01D12570, Proxy::CMD_LoopCmd),
	HOOK_DEF(0x01D117C0, Proxy::CMD_MaxClients),
	HOOK_DEF(0x01D14610, Proxy::CMD_LocalMsg),
	HOOK_DEF(0x01D11A50, Proxy::CMD_Connect),
	HOOK_DEF(0x01D139F0, Proxy::CMD_Disconnect),
	HOOK_DEF(0x01D14180, Proxy::CMD_PlayDemo),
	HOOK_DEF(0x01D11940, Proxy::CMD_Delay),
	HOOK_DEF(0x01D119E0, Proxy::CMD_Stop),
	HOOK_DEF(0x01D12480, Proxy::CMD_Record),
	HOOK_DEF(0x01D12530, Proxy::CMD_StopRecording),
	HOOK_DEF(0x01D123C0, Proxy::CMD_BlockVoice),
	HOOK_DEF(0x01D11B20, Proxy::CMD_Name),
	HOOK_DEF(0x01D120B0, Proxy::CMD_Rate),
	HOOK_DEF(0x01D12300, Proxy::CMD_Updaterate),
	HOOK_DEF(0x01D12220, Proxy::CMD_HostName),
	HOOK_DEF(0x01D13CC0, Proxy::CMD_AddResource),
	HOOK_DEF(0x01D13C60, Proxy::CMD_Resources),
	HOOK_DEF(0x01D14DD0, Proxy::CMD_BannerFile),
	HOOK_DEF(0x01D14AF0, Proxy::CMD_Bann),
	HOOK_DEF(0x01D14850, Proxy::CMD_AddFakeClients),
	HOOK_DEF(0x01D14CD0, Proxy::CMD_Retry),
	HOOK_DEF(0x01D14D00, Proxy::CMD_AutoRetry),
	HOOK_DEF(0x01D14C40, Proxy::CMD_ServerPassword),
	HOOK_DEF(0x01D11040, Proxy::CMD_Status),
	HOOK_DEF(0x01D13710, Proxy::CMD_MaxQueries),
	HOOK_DEF(0x01D12170, Proxy::CMD_Players),
	HOOK_DEF(0x01D14C00, Proxy::CMD_ClearBanns),
	HOOK_DEF(0x01D149B0, Proxy::CMD_MaxLoss),
	HOOK_DEF(0x01D11D80, Proxy::CMD_Protocol),
	HOOK_DEF(0x01D14A50, Proxy::CMD_Region),

	//HOOK_DEF(0x01D12E70, Proxy::GetModVersion),		// NOXREF
	//HOOK_DEF(0x0, Proxy::CMD_InformPlayers),			// NOXREF
	//HOOK_DEF(0x0, Proxy::CMD_MaxUpdateRate),			// NOXREF

#endif // Proxy_region

#ifndef ProxyClient_region

	// BaseSystemModule
	HOOK_VIRTUAL_DEF(0x01D16C50, ProxyClient::Init),
	HOOK_VIRTUAL_DEF(0x01D16D20, ProxyClient::ShutDown),

	// BaseClient
	HOOK_VIRTUAL_DEF(0x01D16A40, ProxyClient::HasChatEnabled),
	HOOK_VIRTUAL_DEF(0x01D16B30, ProxyClient::DownloadFile),
	HOOK_VIRTUAL_DEF(0x01D16920, ProxyClient::SendDatagram),
	HOOK_VIRTUAL_DEF(0x01D168E0, ProxyClient::ReplySpawn),
	HOOK_VIRTUAL_DEF(0x01D16770, ProxyClient::UpdateUserInfo),
	HOOK_VIRTUAL_DEF(0x01D16A50, ProxyClient::ParseVoiceData),
	HOOK_VIRTUAL_DEF(0x01D16310, ProxyClient::ProcessStringCmd),
	HOOK_VIRTUAL_DEF(0x01D16850, ProxyClient::ParseHLTV),

#endif // ProxyClient_region

#ifndef FakeClient_region

	// BaseSystemModule
	HOOK_VIRTUAL_DEF(0x01D09300, FakeClient::Init),
	HOOK_VIRTUAL_DEF(0x01D09310, FakeClient::RunFrame),
	HOOK_VIRTUAL_DEF(0x01D09320, FakeClient::ReceiveSignal),
	HOOK_VIRTUAL_DEF(0x01D09380, FakeClient::GetStatusLine),
	HOOK_VIRTUAL_DEF(0x01D09390, FakeClient::GetType),
	HOOK_VIRTUAL_DEF(0x01D093D0, FakeClient::ShutDown),

	// FakeClient
	HOOK_DEF(0x01D08EA0, FakeClient::SetRate),
	HOOK_DEF(0x01D08EB0, FakeClient::Connect),
	HOOK_DEF(0x01D08EE0, FakeClient::Retry),			// NOXREF
	HOOK_DEF(0x01D08EF0, FakeClient::Say),				// NOXREF
	HOOK_DEF(0x01D08F50, FakeClient::Disconnect),		// NOXREF

#endif // FakeClient_region

#ifndef BaseClient_region

	// IClient
	HOOK_VIRTUAL_DEF(0x01D01670, BaseClient::Connect),
	HOOK_VIRTUAL_DEF(0x01D02790, BaseClient::Send),
	HOOK_VIRTUAL_DEF(0x01D01830, BaseClient::Disconnect),
	HOOK_VIRTUAL_DEF(0x01D023F0, BaseClient::Reconnect),
	HOOK_VIRTUAL_DEF(0x01D023C0, BaseClient::SetWorld),
	HOOK_VIRTUAL_DEF(0x01D027D0, BaseClient::GetClientType),
	HOOK_VIRTUAL_DEF(0x01D01810, BaseClient::GetClientName),
	HOOK_VIRTUAL_DEF(0x01D01820, BaseClient::GetUserInfo),
	HOOK_VIRTUAL_DEF(0x01D027C0, BaseClient::GetAddress),
	HOOK_VIRTUAL_DEF(0x01D02420, BaseClient::IsActive),
	HOOK_VIRTUAL_DEF(0x01D026F0, BaseClient::IsHearingVoices),
	HOOK_VIRTUAL_DEF(0x01D02730, BaseClient::HasChatEnabled),
	HOOK_VIRTUAL_DEF(0x01D02760, BaseClient::DownloadFailed),
	HOOK_VIRTUAL_DEF(0x01D02740, BaseClient::DownloadFile),
	HOOK_VIRTUAL_DEF(0x01D025C0, BaseClient::UpdateVoiceMask),
	HOOK_VIRTUAL_DEF(0x01D02580, BaseClient::QueryVoiceEnabled),
	HOOK_VIRTUAL_DEF(0x01D02470, BaseClient::SetName),
	HOOK_VIRTUAL_DEF(0x01D02100, BaseClient::WriteSpawn),
	HOOK_VIRTUAL_DEF(0x01D02180, BaseClient::WriteDatagram),
	HOOK_VIRTUAL_DEF(0x01D027F0, BaseClient::SendDatagram),
	HOOK_VIRTUAL_DEF(0x01D02820, BaseClient::Reset),
	HOOK_VIRTUAL_DEF(0x01D02020, BaseClient::SetState),
	HOOK_VIRTUAL_DEF(0x01D01F20, BaseClient::ReplyNew),
	HOOK_VIRTUAL_DEF(0x01D01E50, BaseClient::ReplySpawn),
	HOOK_VIRTUAL_DEF(0x01D01F90, BaseClient::ReplyFullUpdate),
	HOOK_VIRTUAL_DEF(0x01D01E10, BaseClient::PrintfToClient),
	HOOK_VIRTUAL_DEF(0x01D01D00, BaseClient::UpdateUserInfo),
	HOOK_VIRTUAL_DEF(0x01D01900, BaseClient::ParseStringCmd),
	HOOK_VIRTUAL_DEF(0x01D018C0, BaseClient::ParseNop),
	HOOK_VIRTUAL_DEF(0x01D018D0, BaseClient::ParseBad),
	HOOK_VIRTUAL_DEF(0x01D02450, BaseClient::ParseMove),
	HOOK_VIRTUAL_DEF(0x01D02430, BaseClient::ParseVoiceData),
	HOOK_VIRTUAL_DEF(0x01D02530, BaseClient::ParseHLTV),
	HOOK_VIRTUAL_DEF(0x01D022E0, BaseClient::ParseDelta),
	HOOK_VIRTUAL_DEF(0x01D02540, BaseClient::ParseCvarValue),
	HOOK_VIRTUAL_DEF(0x01D02550, BaseClient::ParseCvarValue2),
	HOOK_VIRTUAL_DEF(0x01D01740, BaseClient::ProcessMessage),
	HOOK_VIRTUAL_DEF(0x01D01920, BaseClient::ProcessStringCmd),

	// BaseSystemModule
	HOOK_VIRTUAL_DEF(0x01D02950, BaseClient::Init),
	HOOK_VIRTUAL_DEF(0x01D02960, BaseClient::RunFrame),
	HOOK_VIRTUAL_DEF(0x01D02330, BaseClient::GetStatusLine),
	HOOK_VIRTUAL_DEF(0x01D16CE0, BaseClient::GetType),
	HOOK_VIRTUAL_DEF(0x01D02A20, BaseClient::ShutDown),

#endif // BaseClient_region

#ifndef BitBuffer_region

	HOOK_DEF(0x01D02F80, MethodThunk<BitBuffer>::Destructor),
	HOOK_DEF(0x01D02F10, (MethodThunk<BitBuffer>::Constructor), void()),
	HOOK_DEF(0x01D02F90, (MethodThunk<BitBuffer, unsigned int>::Constructor), void(unsigned int)),
	HOOK_DEF(0x01D02F50, (MethodThunk<BitBuffer, void *, unsigned int>::Constructor), void(void *, unsigned int)),

	HOOK_DEF(0x01D02FC0, BitBuffer::Resize),
	HOOK_DEF(0x01D03010, BitBuffer::Clear),
	HOOK_DEF(0x01D03050, BitBuffer::Reset),
	HOOK_DEF(0x01D03070, BitBuffer::Free),
	HOOK_DEF(0x01D03220, BitBuffer::PeekBits),
	HOOK_DEF(0x01D038D0, BitBuffer::CurrentSize),
	HOOK_DEF(0x01D03CE0, BitBuffer::FastClear),
	HOOK_DEF(0x01D03D30, BitBuffer::ConcatBuffer),
	HOOK_DEF(0x01D03BF0, BitBuffer::SkipBytes),
	HOOK_DEF(0x01D03040, BitBuffer::CurrentBit),		// NOXREF
	HOOK_DEF(0x01D038F0, BitBuffer::SpaceLeft),			// NOXREF
	HOOK_DEF(0x01D03900, BitBuffer::AlignByte),			// NOXREF
	HOOK_DEF(0x01D03A70, BitBuffer::StartBitMode),		// NOXREF
	HOOK_DEF(0x01D03A80, BitBuffer::EndBitMode),		// NOXREF
	HOOK_DEF(0x01D03AC0, BitBuffer::SetBuffer),			// NOXREF
	HOOK_DEF(0x01D03C20, BitBuffer::SkipBits),			// NOXREF
	HOOK_DEF(0x01D03CB0, BitBuffer::SkipString),		// NOXREF

	// Read
	HOOK_DEF(0x01D030B0, BitBuffer::ReadBits),
	HOOK_DEF(0x01D03190, BitBuffer::ReadBit),
	HOOK_DEF(0x01D03250, BitBuffer::ReadChar),
	HOOK_DEF(0x01D03260, BitBuffer::ReadByte),
	HOOK_DEF(0x01D03270, BitBuffer::ReadShort),
	HOOK_DEF(0x01D03280, BitBuffer::ReadWord),
	HOOK_DEF(0x01D03290, BitBuffer::ReadLong),
	HOOK_DEF(0x01D032A0, BitBuffer::ReadFloat),
	HOOK_DEF(0x01D032C0, BitBuffer::ReadBuf),
	HOOK_DEF(0x01D033A0, BitBuffer::ReadString),
	HOOK_DEF(0x01D033E0, BitBuffer::ReadStringLine),
	HOOK_DEF(0x01D03A00, BitBuffer::ReadBitString),
	HOOK_DEF(0x01D03A90, BitBuffer::ReadBitData),
	HOOK_DEF(0x01D03AF0, BitBuffer::ReadBitVec3Coord),
	HOOK_DEF(0x01D03B50, BitBuffer::ReadBitCoord),
	HOOK_DEF(0x01D03BD0, BitBuffer::ReadCoord),
	HOOK_DEF(0x01D03420, BitBuffer::ReadAngle),			// NOXREF
	HOOK_DEF(0x01D03440, BitBuffer::ReadHiresAngle),	// NOXREF
	HOOK_DEF(0x01D03920, BitBuffer::ReadSBits),			// NOXREF
	HOOK_DEF(0x01D03950, BitBuffer::ReadBitAngle),		// NOXREF

	// Write
	HOOK_DEF(0x01D03770, BitBuffer::WriteBuf, void(const void *, int)),
	HOOK_DEF(0x01D03370, BitBuffer::WriteBuf, void(BitBuffer *, int)),

	HOOK_DEF(0x01D03460, BitBuffer::WriteBit),
	HOOK_DEF(0x01D03530, BitBuffer::WriteBits),
	HOOK_DEF(0x01D03650, BitBuffer::WriteSBits),		// NOXREF
	HOOK_DEF(0x01D036B0, BitBuffer::WriteChar),
	HOOK_DEF(0x01D036C0, BitBuffer::WriteByte),
	HOOK_DEF(0x01D036D0, BitBuffer::WriteShort),
	HOOK_DEF(0x01D036E0, BitBuffer::WriteWord),
	HOOK_DEF(0x01D036F0, BitBuffer::WriteLong),
	HOOK_DEF(0x01D03700, BitBuffer::WriteFloat),
	HOOK_DEF(0x01D03730, BitBuffer::WriteString),
	HOOK_DEF(0x01D03D50, BitBuffer::WriteCoord),
	HOOK_DEF(0x01D03840, BitBuffer::WriteBitData),		// NOXREF
	HOOK_DEF(0x01D03870, BitBuffer::WriteAngle),		// NOXREF
	HOOK_DEF(0x01D038A0, BitBuffer::WriteHiresAngle),	// NOXREF
	HOOK_DEF(0x01D03990, BitBuffer::WriteBitAngle),		// NOXREF
	HOOK_DEF(0x01D03A30, BitBuffer::WriteBitString),	// NOXREF

#endif // BitBuffer_region

#ifndef NetChannel_region

	// virtual functions
	HOOK_VIRTUAL_DEF(0x01D0C7E0, NetChannel::Create),
	HOOK_VIRTUAL_DEF(0x01D0DC20, NetChannel::GetTargetAddress),
	HOOK_VIRTUAL_DEF(0x01D0C8C0, NetChannel::Close),
	HOOK_VIRTUAL_DEF(0x01D0C6E0, NetChannel::Clear),
	HOOK_VIRTUAL_DEF(0x01D0C650, NetChannel::Reset),
	HOOK_VIRTUAL_DEF(0x01D0DC40, NetChannel::IsConnected),
	HOOK_VIRTUAL_DEF(0x01D0C8F0, NetChannel::IsReadyToSend),
	HOOK_VIRTUAL_DEF(0x01D0DD60, NetChannel::IsCrashed),
	HOOK_VIRTUAL_DEF(0x01D0DC70, NetChannel::IsTimedOut),
	HOOK_VIRTUAL_DEF(0x01D0DC30, NetChannel::IsFakeChannel),
	HOOK_VIRTUAL_DEF(0x01D0DC10, NetChannel::KeepAlive),
	HOOK_VIRTUAL_DEF(0x01D0DB90, NetChannel::SetRate),

	HOOK_VIRTUAL_DEF(0x01D0D9F0, NetChannel::SetUpdateRate),
	HOOK_VIRTUAL_DEF(0x01D0DC50, NetChannel::SetTimeOut),
	HOOK_VIRTUAL_DEF(0x01D0DC00, NetChannel::SetKeepAlive),
	HOOK_VIRTUAL_DEF(0x01D0DCB0, NetChannel::GetIdleTime),
	HOOK_VIRTUAL_DEF(0x01D0DCC0, NetChannel::GetRate),
	HOOK_VIRTUAL_DEF(0x01D0DCD0, NetChannel::GetUpdateRate),
	HOOK_VIRTUAL_DEF(0x01D0DCE0, NetChannel::GetLoss),

	HOOK_VIRTUAL_DEF(0x01D0C9E0, NetChannel::TransmitOutgoing),
	HOOK_VIRTUAL_DEF(0x01D0D130, NetChannel::ProcessIncoming),
	HOOK_VIRTUAL_DEF(0x01D0DCF0, NetChannel::FakeAcknowledgement),
	HOOK_VIRTUAL_DEF(0x01D0C480, NetChannel::OutOfBandPrintf),

	// non-virtual functions
	HOOK_DEF(0x01D0C400, NetChannel::UnlinkFragment),
	HOOK_DEF(0x01D0C530, NetChannel::ClearFragbufs),
	HOOK_DEF(0x01D0C560, NetChannel::ClearFragments),
	HOOK_DEF(0x01D0C5D0, NetChannel::FlushIncoming),
	HOOK_DEF(0x01D0C910, NetChannel::UpdateFlow),
	HOOK_DEF(0x01D0D030, NetChannel::FindBufferById),
	HOOK_DEF(0x01D0D090, NetChannel::CheckForCompletion),
	HOOK_DEF(0x01D0D620, NetChannel::FragSend),
	HOOK_DEF(0x01D0D670, NetChannel::AddBufferToList),
	HOOK_DEF(0x01D0D6C0, NetChannel::CreateFragmentsFromBuffer),
	HOOK_DEF(0x01D0DD20, NetChannel::CreateFragmentsFromFile),
	HOOK_DEF(0x01D0D970, NetChannel::AddFragbufToTail),
	HOOK_DEF(0x01D0D9B0, NetChannel::GetPacket),
	HOOK_DEF(0x01D0D9C0, NetChannel::FreePacket),
	HOOK_DEF(0x01D0DA30, NetChannel::CopyNormalFragments),
	HOOK_DEF(0x01D0DBD0, NetChannel::GetFlowStats),
	HOOK_DEF(0x01D0DBC0, NetChannel::SetConnected),			// NOXREF
	HOOK_DEF(0x01D0DD70, NetChannel::CopyFileFragments),	// NOXREF

#endif // NetChannel_region

#ifndef Master_region

	// virtual functions
	HOOK_VIRTUAL_DEF(0x01D0A480, Master::Init),
	HOOK_VIRTUAL_DEF(0x01D0A490, Master::RunFrame),
	HOOK_VIRTUAL_DEF(0x01D0A4B0, Master::ExecuteCommand),
	HOOK_VIRTUAL_DEF(0x01D0A500, Master::GetStatusLine),
	HOOK_VIRTUAL_DEF(0x01D0A510, Master::GetType),
	HOOK_VIRTUAL_DEF(0x01D0A550, Master::ShutDown),

	//HOOK_DEF(0x0, Master::InitializeSteam),			// NOXREF
	HOOK_DEF(0x01D0A230, Master::CMD_Heartbeat),
	HOOK_DEF(0x01D0A260, Master::CMD_NoMaster),
	HOOK_DEF(0x01D0A350, Master::CMD_ListMaster),
	HOOK_DEF(0x01D0A3E0, Master::SendShutdown),

#endif // Master_region

#ifndef DemoFile_region

	HOOK_DEF(0x01D04F60, MethodThunk<DemoFile>::Destructor),
	HOOK_DEF(0x01D04F10, MethodThunk<DemoFile>::Constructor),

	HOOK_DEF(0x01D06380, DemoFile::Init),
	HOOK_DEF(0x01D059D0, DemoFile::LoadDemo),
	HOOK_DEF(0x01D05D30, DemoFile::StopPlayBack),
	HOOK_DEF(0x01D056B0, DemoFile::StartRecording),
	HOOK_DEF(0x01D04FC0, DemoFile::CloseFile),
	HOOK_DEF(0x01D04F70, DemoFile::Reset),
	HOOK_DEF(0x01D06370, DemoFile::SetContinuous),		// NOXREF
	HOOK_DEF(0x01D06360, DemoFile::IsContinuous),		// NOXREF
	HOOK_DEF(0x01D059C0, DemoFile::IsPlaying),
	HOOK_DEF(0x01D04FB0, DemoFile::IsRecording),
	HOOK_DEF(0x01D06350, DemoFile::GetFileName),
	HOOK_DEF(0x01D05D60, DemoFile::ReadDemoPacket),
	HOOK_DEF(0x01D053B0, DemoFile::WriteDemoMessage),
	HOOK_DEF(0x01D05500, DemoFile::WriteUpdateClientData),
	HOOK_DEF(0x01D055B0, DemoFile::GetDemoTime),
	HOOK_DEF(0x01D06260, DemoFile::ReadSequenceInfo),
	HOOK_DEF(0x01D06220, DemoFile::ReadDemoInfo),
	HOOK_DEF(0x01D052B0, DemoFile::WriteDemoStartup),
	HOOK_DEF(0x01D051C0, DemoFile::WriteSequenceInfo),
	HOOK_DEF(0x01D05190, DemoFile::WriteDemoInfo),
	HOOK_DEF(0x01D055D0, DemoFile::WriteSignonData),

#endif // DemoFile_region

#ifndef DemoClient_region

	// BaseSystemModule
	HOOK_VIRTUAL_DEF(0x01D04E30, DemoClient::Init),
	HOOK_VIRTUAL_DEF(0x01D04E40, DemoClient::RunFrame),
	HOOK_VIRTUAL_DEF(0x01D04EB0, DemoClient::GetStatusLine),
	HOOK_VIRTUAL_DEF(0x01D04EC0, DemoClient::GetType),
	HOOK_VIRTUAL_DEF(0x01D04F00, DemoClient::ShutDown),

	// IClient
	HOOK_VIRTUAL_DEF(0x01D04810, DemoClient::Connect),
	HOOK_VIRTUAL_DEF(0x01D04D30, DemoClient::Send),
	HOOK_VIRTUAL_DEF(0x01D04BE0, DemoClient::Disconnect),
	HOOK_VIRTUAL_DEF(0x01D04800, DemoClient::Reconnect),
	HOOK_VIRTUAL_DEF(0x01D04D90, DemoClient::SetWorld),
	HOOK_VIRTUAL_DEF(0x01D047F0, DemoClient::GetClientType),
	HOOK_VIRTUAL_DEF(0x01D04D60, DemoClient::GetClientName),
	HOOK_VIRTUAL_DEF(0x01D04D70, DemoClient::GetUserInfo),
	HOOK_VIRTUAL_DEF(0x01D04CE0, DemoClient::GetAddress),
	HOOK_VIRTUAL_DEF(0x01D04BD0, DemoClient::IsActive),
	HOOK_VIRTUAL_DEF(0x01D047C0, DemoClient::IsHearingVoices),
	HOOK_VIRTUAL_DEF(0x01D047D0, DemoClient::HasChatEnabled),

	HOOK_DEF(0x01D04990, DemoClient::SendDatagram),
	HOOK_DEF(0x01D04A30, DemoClient::WriteDatagram),
	HOOK_DEF(0x01D04C70, DemoClient::FinishDemo),
	HOOK_DEF(0x01D04D80, DemoClient::SetProxy),
	HOOK_DEF(0x01D04DA0, DemoClient::SetFileName),
	HOOK_DEF(0x01D04DD0, DemoClient::GetDemoFile),

#endif // DemoClient_region

#ifndef Director_region

	// BaseSystemModule
	HOOK_VIRTUAL_DEF(0x01D07BC0, Director::Init),
	HOOK_VIRTUAL_DEF(0x01D07BD0, Director::RunFrame),
	HOOK_VIRTUAL_DEF(0x01D07BE0, Director::ReceiveSignal),
	HOOK_VIRTUAL_DEF(0x01D07BF0, Director::ExecuteCommand),
	HOOK_VIRTUAL_DEF(0x01D07C40, Director::GetStatusLine),
	HOOK_VIRTUAL_DEF(0x01D07C50, Director::GetType),
	HOOK_VIRTUAL_DEF(0x01D07C90, Director::ShutDown),

	// IDirector
	HOOK_VIRTUAL_DEF(0x01D068C0, Director::NewGame),
	HOOK_VIRTUAL_DEF(0x01D068B0, Director::GetModName),
	HOOK_VIRTUAL_DEF(0x01D06E10, Director::WriteCommands),
	HOOK_VIRTUAL_DEF(0x01D07A60, Director::AddCommand),
	HOOK_VIRTUAL_DEF(0x01D07B50, Director::RemoveCommand),
	HOOK_VIRTUAL_DEF(0x01D07B60, Director::GetLastCommand),
	HOOK_VIRTUAL_DEF(0x01D07B40, Director::GetCommands),

	// Director
	HOOK_DEF(0x01D07910, Director::FindBestEvent),
	HOOK_DEF(0x01D07690, Director::ExecuteDirectorCommands),
	HOOK_DEF(0x01D07420, Director::RandomizeCommand),
	HOOK_DEF(0x01D07280, Director::GetClosestPlayer),
	HOOK_DEF(0x01D06EC0, Director::AddEvent),
	HOOK_DEF(0x01D06B50, Director::SmoothRank),
	HOOK_DEF(0x01D06C40, Director::AnalysePlayer),
	HOOK_DEF(0x01D06980, Director::AnalyseFrame),
	HOOK_DEF(0x01D073E0, Director::ClearDirectorCommands),
	HOOK_DEF(0x01D07110, Director::AddBestMODCut),
	HOOK_DEF(0x01D06F00, Director::AddBestGenericCut),
	HOOK_DEF(0x01D06EA0, Director::WriteSignonData),
	//HOOK_DEF(0x0, Director::WriteProxyStatus),			// NOXREF
	HOOK_DEF(0x01D07830, Director::CMD_SlowMotion),

#endif // Director_region

#ifndef DirectorCmd_region

	HOOK_DEF(0x01D07D00, DirectorCmd::GetEventData),
	HOOK_DEF(0x01D07D50, DirectorCmd::GetModeData),
	HOOK_DEF(0x01D07D80, DirectorCmd::GetChaseData),
	HOOK_DEF(0x01D07DD0, DirectorCmd::GetInEyeData),
	HOOK_DEF(0x01D07E00, DirectorCmd::GetMapData),
	HOOK_DEF(0x01D07E50, DirectorCmd::GetCameraData),
	HOOK_DEF(0x01D07EE0, DirectorCmd::GetCamPathData),
	HOOK_DEF(0x01D07F70, DirectorCmd::GetSoundData),
	HOOK_DEF(0x01D07FB0, DirectorCmd::GetTime),				// NOXREF
	HOOK_DEF(0x01D07FC0, DirectorCmd::GetType),
	HOOK_DEF(0x01D07FD0, DirectorCmd::GetName),				// NOXREF
	HOOK_DEF(0x01D07FE0, DirectorCmd::GetTimeScaleData),
	HOOK_DEF(0x01D08010, DirectorCmd::GetWayPointsData),
	HOOK_DEF(0x01D08040, DirectorCmd::GetMessageData),
	HOOK_DEF(0x01D080E0, DirectorCmd::GetStatusData),
	HOOK_DEF(0x01D08130, DirectorCmd::GetBannerData),
	HOOK_DEF(0x01D08170, DirectorCmd::GetStuffTextData),
	HOOK_DEF(0x01D081B0, DirectorCmd::SetEventData),
	HOOK_DEF(0x01D081F0, DirectorCmd::SetChaseData),		// NOXREF
	HOOK_DEF(0x01D08240, DirectorCmd::SetInEyeData),		// NOXREF
	HOOK_DEF(0x01D08270, DirectorCmd::SetMapData),			// NOXREF
	HOOK_DEF(0x01D082B0, DirectorCmd::SetStartData),		// NOXREF
	HOOK_DEF(0x01D082C0, DirectorCmd::SetModeData),			// NOXREF
	HOOK_DEF(0x01D082F0, DirectorCmd::SetCameraData),		// NOXREF
	HOOK_DEF(0x01D08370, DirectorCmd::SetCamPathData),		// NOXREF
	HOOK_DEF(0x01D083F0, DirectorCmd::SetSoundData),
	HOOK_DEF(0x01D08440, DirectorCmd::SetTimeScaleData),
	HOOK_DEF(0x01D08470, DirectorCmd::SetTime),
	HOOK_DEF(0x01D08480, DirectorCmd::SetMessageData),
	HOOK_DEF(0x01D08520, DirectorCmd::Copy),				// NOXREF
	HOOK_DEF(0x01D08570, DirectorCmd::SetStatusData),
	HOOK_DEF(0x01D085B0, DirectorCmd::SetBannerData),
	HOOK_DEF(0x01D085F0, DirectorCmd::SetStuffTextData),	// NOXREF
	HOOK_DEF(0x01D08630, DirectorCmd::SetWayPoints),		// NOXREF
	HOOK_DEF(0x01D08660, DirectorCmd::ReadFromStream),		// NOXREF
	HOOK_DEF(0x01D088C0, DirectorCmd::WriteToStream),
	HOOK_DEF(0x01D08920, DirectorCmd::ToString),			// NOXREF
	HOOK_DEF(0x01D08D50, DirectorCmd::FromString),			// NOXREF
	HOOK_DEF(0x01D08D60, DirectorCmd::Clear),
	HOOK_DEF(0x01D08D70, DirectorCmd::Resize),

#endif // DirectorCmd_region

#ifndef Status_region

	// BaseSystemModule
	HOOK_VIRTUAL_DEF(0x01D178A0, Status::Init),
	HOOK_VIRTUAL_DEF(0x01D178B0, Status::RunFrame),
	HOOK_VIRTUAL_DEF(0x01D178D0, Status::ExecuteCommand),
	HOOK_VIRTUAL_DEF(0x01D17920, Status::GetStatusLine),
	HOOK_VIRTUAL_DEF(0x01D17930, Status::GetType),
	HOOK_VIRTUAL_DEF(0x01D17970, Status::ShutDown),

#endif // Status_region

#ifndef InfoString_region

	HOOK_DEF(0x01D09590, MethodThunk<InfoString>::Destructor),
	HOOK_DEF(0x01D094F0, (MethodThunk<InfoString>::Constructor), void()),
	HOOK_DEF(0x01D09510, (MethodThunk<InfoString, unsigned int>::Constructor), void(unsigned int)),
	HOOK_DEF(0x01D09540, (MethodThunk<InfoString, char *>::Constructor), void(char *)),
	HOOK_DEF(0x01D09480, (MethodThunk<InfoString, char *, unsigned int>::Constructor), void(char *, unsigned int)),

	HOOK_DEF(0x01D095B0, InfoString::SetString),
	HOOK_DEF(0x01D09600, InfoString::SetMaxSize),
	HOOK_DEF(0x01D09660, InfoString::GetMaxSize),
	HOOK_DEF(0x01D09670, InfoString::GetCurrentSize),
	HOOK_DEF(0x01D09690, InfoString::Clear),
	HOOK_DEF(0x01D096B0, InfoString::GetString),
	HOOK_DEF(0x01D096C0, InfoString::ValueForKey),
	HOOK_DEF(0x01D097B0, InfoString::RemoveKey),
	HOOK_DEF(0x01D09880, InfoString::RemovePrefixedKeys),
	HOOK_DEF(0x01D09900, InfoString::SetValueForStarKey),
	HOOK_DEF(0x01D09AB0, InfoString::SetValueForKey),

#endif // InfoString_region

	{ NULL, NULL, NULL },
};

AddressRef g_FunctionRefs[] =
{
#ifndef Function_References_region

#endif // Function_References_region

	{ NULL, NULL, NULL },
};

AddressRef g_DataRefs[] =
{
#ifndef Data_References_region

#endif // Data_References_region

	{ NULL, NULL, NULL },
};

#endif // HOOK_HLTV
