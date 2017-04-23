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
//#define World_region
//#define Server_region
//#define Network_region
//#define NetSocket_region
//#define BSPModel_region
//#define Delta_region
//#define ObjectDictionary_region
//#define ObjectList_region
//#define BitBuffer_region
//#define Function_References_region
//#define Data_References_region

FunctionHook g_FunctionHooks[] =
{
	// DO NOT DISABLE, other functions depends on memory allocation routines
#ifndef Mem_region

	HOOK_DEF(0x01D21F5F, malloc_wrapper),
	HOOK_DEF(0x01D21E4E, free_wrapper),
	HOOK_DEF(0x01D30145, strdup_wrapper),
	HOOK_DEF(0x01D21F71, __nh_malloc_wrapper),

	HOOK_DEF(0x01D043C0, Mem_ZeroMalloc),
	//HOOK_DEF(0x0, Mem_Malloc),
	//HOOK_DEF(0x0, Mem_Realloc),
	//HOOK_DEF(0x0, Mem_Calloc),
	//HOOK_DEF(0x0, Mem_Strdup),
	//HOOK_DEF(0x0, Mem_Free),

	//HOOK_DEF(0x0, realloc_wrapper),
	//HOOK_DEF(0x0, calloc_wrapper),

#endif // Mem_region

#ifndef World_region

	// World virtual functions
	HOOK_VIRTUAL_DEF(0x01D14DA0, World::Init),
	HOOK_VIRTUAL_DEF(0x01D15470, World::RunFrame),
	HOOK_VIRTUAL_DEF(0x01D154F0, World::ReceiveSignal),
	HOOK_VIRTUAL_DEF(0x01D15490, World::ExecuteCommand),
	HOOK_VIRTUAL_DEF(0x01D15500, World::RegisterListener),
	HOOK_VIRTUAL_DEF(0x01D15530, World::RemoveListener),
	HOOK_VIRTUAL_DEF(0x01D15570, World::GetSystem),
	HOOK_VIRTUAL_DEF(0x01D15580, World::GetSerial),
	HOOK_VIRTUAL_DEF(0x01D17380, World::GetStatusLine),
	HOOK_VIRTUAL_DEF(0x01D17370, World::GetType),
	HOOK_VIRTUAL_DEF(0x01D15590, World::GetName),
	HOOK_VIRTUAL_DEF(0x01D155C0, World::GetState),
	HOOK_VIRTUAL_DEF(0x01D15620, World::GetVersion),
	HOOK_VIRTUAL_DEF(0x01D14F70, World::ShutDown),
	HOOK_VIRTUAL_DEF(0x01D175D0, World::GetTime),
	HOOK_VIRTUAL_DEF(0x01D186A0, World::GetGameServerAddress),
	HOOK_VIRTUAL_DEF(0x01D186B0, World::GetLevelName),
	HOOK_VIRTUAL_DEF(0x01D186F0, World::GetGameDir),
	HOOK_VIRTUAL_DEF(0x01D16470, World::GetFrameByTime),
	HOOK_VIRTUAL_DEF(0x01D16420, World::GetFrameBySeqNr),
	HOOK_VIRTUAL_DEF(0x01D16450, World::GetLastFrame),
	HOOK_VIRTUAL_DEF(0x01D16460, World::GetFirstFrame),
	HOOK_VIRTUAL_DEF(0x01D18700, World::GetServerCount),
	HOOK_VIRTUAL_DEF(0x01D18670, World::GetSlotNumber),
	HOOK_VIRTUAL_DEF(0x01D18710, World::GetMaxClients),
	HOOK_VIRTUAL_DEF(0x01D15840, World::GetNumPlayers),
	HOOK_VIRTUAL_DEF(0x01D186C0, World::GetWorldModel),
	HOOK_VIRTUAL_DEF(0x01D18350, World::GetServerInfoString),
	HOOK_VIRTUAL_DEF(0x01D18360, World::GetPlayerInfoString),
	HOOK_VIRTUAL_DEF(0x01D15450, World::GetUserMsg),
	HOOK_VIRTUAL_DEF(0x01D15A60, World::GetHostName),
	HOOK_VIRTUAL_DEF(0x01D189A0, World::GetServerInfo),
	HOOK_VIRTUAL_DEF(0x01D15510, World::IsPlayerIndex),
	HOOK_VIRTUAL_DEF(0x01D18660, World::IsVoiceEnabled),
	HOOK_VIRTUAL_DEF(0x01D15870, World::IsActive),
	HOOK_VIRTUAL_DEF(0x01D18190, World::IsPaused),
	HOOK_VIRTUAL_DEF(0x01D181A0, World::IsComplete),
	HOOK_VIRTUAL_DEF(0x01D18D20, World::IsHLTV),
	HOOK_VIRTUAL_DEF(0x01D15010, World::Reset),
	HOOK_VIRTUAL_DEF(0x01D158A0, World::SetServerInfoString),
	HOOK_VIRTUAL_DEF(0x01D18C30, World::UpdateServerInfo),
	HOOK_VIRTUAL_DEF(0x01D18150, World::SetPaused),
	HOOK_VIRTUAL_DEF(0x01D15630, World::SetTime),
	HOOK_VIRTUAL_DEF(0x01D181B0, World::SetBufferSize),
	HOOK_VIRTUAL_DEF(0x01D18690, World::SetVoiceEnabled),
	HOOK_VIRTUAL_DEF(0x01D18130, World::SetMoveVars),
	HOOK_VIRTUAL_DEF(0x01D18320, World::SetCDInfo),
	HOOK_VIRTUAL_DEF(0x01D18D10, World::SetHLTV),
	HOOK_VIRTUAL_DEF(0x01D183B0, World::SetExtraInfo),
	HOOK_VIRTUAL_DEF(0x01D18680, World::SetViewEntity),
	HOOK_VIRTUAL_DEF(0x01D18340, World::SetGameServerAddress),
	HOOK_VIRTUAL_DEF(0x01D15680, World::SetHostName),
	HOOK_VIRTUAL_DEF(0x01D15760, World::NewGame),
	HOOK_VIRTUAL_DEF(0x01D17490, World::FinishGame),
	HOOK_VIRTUAL_DEF(0x01D18720, World::SaveAsDemo),
	HOOK_VIRTUAL_DEF(0x01D174C0, World::StopGame),
	HOOK_VIRTUAL_DEF(0x01D17560, World::FindUserMsgByName),
	HOOK_VIRTUAL_DEF(0x01D181F0, World::ParseDeltaDescription),
	HOOK_VIRTUAL_DEF(0x01D18420, World::ParseBaseline),
	HOOK_VIRTUAL_DEF(0x01D183F0, World::ParseEvent),
	HOOK_VIRTUAL_DEF(0x01D16990, World::ParseClientData),
	HOOK_VIRTUAL_DEF(0x01D18070, World::SetServerInfo, void(int, CRC32_t, unsigned char *, int, int, int, char *, char *, char *)),
	HOOK_VIRTUAL_DEF(0x01D17470, World::SetServerInfo, void(serverinfo_t *)),
	HOOK_VIRTUAL_DEF(0x01D16740, World::GetUncompressedFrame, bool(unsigned int, frame_t *)),
	HOOK_VIRTUAL_DEF(0x01D17A60, World::UncompressEntitiesFromStream, bool(frame_t *, BitBuffer *)),
	HOOK_VIRTUAL_DEF(0x01D175E0, World::UncompressEntitiesFromStream, bool(frame_t *, BitBuffer *, unsigned int)),
	HOOK_VIRTUAL_DEF(0x01D167F0, World::GetClientData, bool(frame_t *, clientdata_t *)),
	HOOK_VIRTUAL_DEF(0x01D167B0, World::GetClientData, bool(unsigned int, clientdata_t *)),
	HOOK_VIRTUAL_DEF(0x01D160A0, World::AddFrame),
	HOOK_VIRTUAL_DEF(0x01D153B0, World::AddResource),
	HOOK_VIRTUAL_DEF(0x01D156E0, World::AddLightStyle),
	HOOK_VIRTUAL_DEF(0x01D17520, World::AddSignonData),
	HOOK_VIRTUAL_DEF(0x01D15130, World::AddUserMessage),
	HOOK_VIRTUAL_DEF(0x01D154A0, World::AddBaselineEntity),
	HOOK_VIRTUAL_DEF(0x01D155D0, World::AddInstancedBaselineEntity),
	HOOK_VIRTUAL_DEF(0x01D151F0, World::UpdatePlayer),
	HOOK_VIRTUAL_DEF(0x01D164C0, World::WriteFrame),
	HOOK_VIRTUAL_DEF(0x01D17CB0, World::WriteNewData),
	HOOK_VIRTUAL_DEF(0x01D16030, World::WriteClientUpdate),
	HOOK_VIRTUAL_DEF(0x01D15D20, World::WriteMovevars),
	HOOK_VIRTUAL_DEF(0x01D174F0, World::WriteSigonData),
	HOOK_VIRTUAL_DEF(0x01D15A20, World::WriteLightStyles),
	HOOK_VIRTUAL_DEF(0x01D17D80, World::RemoveFrames),
	HOOK_VIRTUAL_DEF(0x01D17ED0, World::DuplicateFrames),
	HOOK_VIRTUAL_DEF(0x01D17EF0, World::MoveFrames),
	HOOK_VIRTUAL_DEF(0x01D17F10, World::RevertFrames),

	// World non-virtual functions
	HOOK_DEF(0x01D16BA0, World::CompressFrame),
	HOOK_DEF(0x01D16A80, World::ParseDeltaHeader),
	HOOK_DEF(0x01D150D0, World::SetMaxClients),
	HOOK_DEF(0x01D17D30, World::GetBufferedGameTime),
	HOOK_DEF(0x01D15800, World::ConnectionComplete),
	HOOK_DEF(0x01D15F00, World::WriteResources),
	HOOK_DEF(0x01D15C60, World::WriteDeltaDescriptions),
	HOOK_DEF(0x01D15EB0, World::WriteRegisteredUserMessages),
	HOOK_DEF(0x01D158B0, World::WriteBaseline),
	HOOK_DEF(0x01D15A80, World::WriteServerinfo),
	HOOK_DEF(0x01D170E0, World::WritePacketEntities),
	HOOK_DEF(0x01D189B0, World::WriteDeltaEntities),
	HOOK_DEF(0x01D15790, World::SetState),
	HOOK_DEF(0x01D15100, World::ClearUserMessages),
	HOOK_DEF(0x01D17430, World::ClearServerInfo),
	HOOK_DEF(0x01D15410, World::ClearResources),
	HOOK_DEF(0x01D155A0, World::ClearInstancedBaseline),
	HOOK_DEF(0x01D15540, World::ClearBaseline),
	HOOK_DEF(0x01D156C0, World::ClearLightStyles),
	HOOK_DEF(0x01D15880, World::ClearPlayers),
	HOOK_DEF(0x01D17F30, World::ClearFrames),
	HOOK_DEF(0x01D16E80, World::ClearEntityCache),
	HOOK_DEF(0x01D16F30, World::GetFrameFromCache),
	HOOK_DEF(0x01D17010, World::GetDeltaFromCache),
	HOOK_DEF(0x01D17F90, World::CheckFrameBufferSize),
	HOOK_DEF(0x01D18000, World::ReorderFrameTimes),
	HOOK_DEF(0x01D16D20, World::FindBestBaseline),
	HOOK_DEF(0x01D18A60, World::RearrangeFrame),
	HOOK_DEF(0x01D16880, World::GetUncompressedFrame, bool(frame_t *deltaFrame, frame_t *frame)),

	//HOOK_DEF(0x01D17D00, World::SetName), 	// NOXREF
	//HOOK_DEF(0x0, World::SetDirector), 		// NOXREF
	//HOOK_DEF(0x0, World::SetTimeScale), 		// NOXREF
	//HOOK_DEF(0x0, World::SetGameGroupAddress), 	// NOXREF
	//HOOK_DEF(0x0, World::GetDirector), 		// NOXREF
	//HOOK_DEF(0x0, World::WriteCustomDecals), 	// NOXREF

#endif // World_region

#ifndef Server_region

	// IServer virtual functions
	HOOK_VIRTUAL_DEF(0x01D106B0, Server::Init),
	HOOK_VIRTUAL_DEF(0x01D10C10, Server::RunFrame),
	HOOK_VIRTUAL_DEF(0x01D10A90, Server::ExecuteCommand),
	HOOK_VIRTUAL_DEF(0x01D13870, Server::GetStatusLine),
	HOOK_VIRTUAL_DEF(0x01D137F0, Server::GetType),
	HOOK_VIRTUAL_DEF(0x01D10B80, Server::ShutDown),
	HOOK_VIRTUAL_DEF(0x01D14220, Server::Connect),
	HOOK_VIRTUAL_DEF(0x01D142F0, Server::LoadDemo),
	HOOK_VIRTUAL_DEF(0x01D13E40, Server::Reconnect),
	HOOK_VIRTUAL_DEF(0x01D13EA0, Server::Disconnect),
	HOOK_VIRTUAL_DEF(0x01D14620, Server::Retry),
	HOOK_VIRTUAL_DEF(0x01D14680, Server::StopRetry),
	HOOK_VIRTUAL_DEF(0x01D14430, Server::SendStringCommand),
	HOOK_VIRTUAL_DEF(0x01D14470, Server::SendHLTVCommand),
	HOOK_VIRTUAL_DEF(0x01D140C0, Server::IsConnected),
	HOOK_VIRTUAL_DEF(0x01D140D0, Server::IsDemoFile),
	HOOK_VIRTUAL_DEF(0x01D140E0, Server::IsGameServer),
	HOOK_VIRTUAL_DEF(0x01D140F0, Server::IsRelayProxy),
	HOOK_VIRTUAL_DEF(0x01D11260, Server::IsVoiceBlocking),
	HOOK_VIRTUAL_DEF(0x01D13A70, Server::SetProxy),
	HOOK_VIRTUAL_DEF(0x01D13A90, Server::SetDirector),
	HOOK_VIRTUAL_DEF(0x01D137A0, Server::SetPlayerName),
	HOOK_VIRTUAL_DEF(0x01D146B0, Server::SetDelayReconnect),
	HOOK_VIRTUAL_DEF(0x01D14690, Server::SetAutoRetry),
	HOOK_VIRTUAL_DEF(0x01D11270, Server::SetVoiceBlocking),
	HOOK_VIRTUAL_DEF(0x01D11280, Server::SetRate),
	HOOK_VIRTUAL_DEF(0x01D112D0, Server::SetUpdateRate),
	HOOK_VIRTUAL_DEF(0x01D14390, Server::SetUserInfo),
	HOOK_VIRTUAL_DEF(0x01D13770, Server::SetProtocol),
	HOOK_VIRTUAL_DEF(0x01D109C0, Server::SetGameDirectory),
	HOOK_VIRTUAL_DEF(0x01D13730, Server::GetRate),
	HOOK_VIRTUAL_DEF(0x01D13740, Server::GetUpdateRate),
	HOOK_VIRTUAL_DEF(0x01D13720, Server::GetServerInfoString),
	HOOK_VIRTUAL_DEF(0x01D137D0, Server::GetPlayerName),

	// Server virtual functions
	HOOK_VIRTUAL_DEF(0x01D145A0, Server::GetTime),
	HOOK_VIRTUAL_DEF(0x01D13800, Server::GetWorld),
	HOOK_VIRTUAL_DEF(0x01D13830, Server::GetDemoFileName),
	HOOK_VIRTUAL_DEF(0x01D13810, Server::GetAddress),
	HOOK_VIRTUAL_DEF(0x01D13750, Server::GetAutoRetry),
	HOOK_VIRTUAL_DEF(0x01D13850, Server::GetHostName),
	HOOK_VIRTUAL_DEF(0x01D13860, Server::GetPacketLoss),
	HOOK_VIRTUAL_DEF(0x01D13760, Server::GetProtocol),

	// Server non-virtual functions
	HOOK_DEF(0x01D14190, Server::CheckAutoRetry),
	HOOK_DEF(0x01D145B0, Server::CheckConnection),
	HOOK_DEF(0x01D141F0, Server::ScheduleAutoRetry),
	HOOK_DEF(0x01D11750, Server::AcceptConnection),
	HOOK_DEF(0x01D116C0, Server::AcceptBadPassword),
	HOOK_DEF(0x01D11700, Server::AcceptRejection),
	HOOK_DEF(0x01D117D0, Server::AcceptRedirect),
	HOOK_DEF(0x01D11460, Server::SendConnectPacket),
	HOOK_DEF(0x01D132F0, Server::SendServerCommands),
	HOOK_DEF(0x01D11320, Server::SetState),
	HOOK_DEF(0x01D10AB0, Server::Challenge),
	HOOK_DEF(0x01D146C0, Server::Reset),
	HOOK_DEF(0x01D11170, Server::AcceptChallenge),
	HOOK_DEF(0x01D14050, Server::SendUserVar),
	HOOK_DEF(0x01D10F70, Server::ProcessMessage),
	HOOK_DEF(0x01D147B0, Server::ProcessEntityUpdate),
	HOOK_DEF(0x01D10DA0, Server::ProcessConnectionlessMessage),
	HOOK_DEF(0x01D12EF0, Server::ClearFrame),
	HOOK_DEF(0x01D139E0, Server::ParseHLTV),
	HOOK_DEF(0x01D13AA0, Server::ParseDirector),
	HOOK_DEF(0x01D12CA0, Server::ParseFileTransferFailed),
	HOOK_DEF(0x01D13B80, Server::ParseInfo),
	HOOK_DEF(0x01D136E0, Server::ParseParticle),
	HOOK_DEF(0x01D136B0, Server::ParseRoomType),
	HOOK_DEF(0x01D13610, Server::ParseSpawnStaticSound),
	HOOK_DEF(0x01D13580, Server::ParseEventReliable),
	HOOK_DEF(0x01D13510, Server::ParsePings),
	HOOK_DEF(0x01D12CE0, Server::ParseSignonNum),
	HOOK_DEF(0x01D12FB0, Server::ParseUserMessage),
	HOOK_DEF(0x01D134E0, Server::ParseStopSound),
	HOOK_DEF(0x01D13410, Server::ParseEvent),
	HOOK_DEF(0x01D13340, Server::ParseSound),
	HOOK_DEF(0x01D13240, Server::ParseDeltaPacketEntities),
	HOOK_DEF(0x01D131A0, Server::ParsePacketEntities),
	HOOK_DEF(0x01D12DF0, Server::ParseCustomization),
	HOOK_DEF(0x01D12010, Server::ParseCrosshairAngle),
	HOOK_DEF(0x01D12050, Server::ParseSoundFade),
	HOOK_DEF(0x01D12C10, Server::ParseDisconnect),
	HOOK_DEF(0x01D12C00, Server::ParseChoke),
	HOOK_DEF(0x01D12AC0, Server::ParseSetAngle),
	HOOK_DEF(0x01D12B60, Server::ParseAddAngle),
	HOOK_DEF(0x01D12A40, Server::ParseLightStyle),
	HOOK_DEF(0x01D12A20, Server::ParseTime),
	HOOK_DEF(0x01D129F0, Server::ParseVersion),
	HOOK_DEF(0x01D129D0, Server::ParseBaseline),
	HOOK_DEF(0x01D12860, Server::ParseTempEntity),
	HOOK_DEF(0x01D12650, Server::ParseResourceList),
	HOOK_DEF(0x01D12590, Server::ParseUpdateUserInfo),
	HOOK_DEF(0x01D12150, Server::ParseStuffText),
	HOOK_DEF(0x01D120C0, Server::ParseNewUserMsg),
	HOOK_DEF(0x01D11F80, Server::ParseResourceRequest),
	HOOK_DEF(0x01D11F00, Server::ParseSetView),
	HOOK_DEF(0x01D11E90, Server::ParseCDTrack),
	HOOK_DEF(0x01D11EC0, Server::ParseRestore),
	HOOK_DEF(0x01D11CC0, Server::ParseMoveVars),
	HOOK_DEF(0x01D11C60, Server::ParseDeltaDescription),
	HOOK_DEF(0x01D119F0, Server::ParseServerinfo),
	HOOK_DEF(0x01D119D0, Server::ParseBad),
	HOOK_DEF(0x01D119C0, Server::ParseNop),
	HOOK_DEF(0x01D11850, Server::ParsePrint),
	HOOK_DEF(0x01D11880, Server::ParseVoiceInit),
	HOOK_DEF(0x01D11940, Server::ParseVoiceData),
	HOOK_DEF(0x01D144F0, Server::ParseTimeScale),
	HOOK_DEF(0x01D144C0, Server::ParseSendExtraInfo),
	HOOK_DEF(0x01D14100, Server::ParseCenterPrint),
	HOOK_DEF(0x01D143D0, Server::ParseSetPause),
	HOOK_DEF(0x01D14020, Server::ParseCutscene),
	HOOK_DEF(0x01D12140, Server::ParseWeaponAnim),
	HOOK_DEF(0x01D11C80, Server::ParseDecalName),
	HOOK_DEF(0x01D13FF0, Server::ParseFinale),
	HOOK_DEF(0x01D13FC0, Server::ParseIntermission),
	HOOK_DEF(0x01D13F50, Server::ParseClientData),
	HOOK_DEF(0x01D14570, Server::ParseResourceLocation),
	HOOK_DEF(0x01D14820, Server::ParseSendCvarValue),
	HOOK_DEF(0x01D14830, Server::ParseSendCvarValue2),
	//HOOK_DEF(0x01D14740, Server::GetCmdName), 				// NOXREF

#endif // Server_region

#ifndef Network_region

	// Network virtual functions
	HOOK_VIRTUAL_DEF(0x01D0E040, Network::Init),
	HOOK_VIRTUAL_DEF(0x01D0E480, Network::RunFrame),
	HOOK_VIRTUAL_DEF(0x01D0E5B0, Network::ReceiveSignal),
	HOOK_VIRTUAL_DEF(0x01D0E210, Network::ExecuteCommand),
	HOOK_VIRTUAL_DEF(0x01D0E5C0, Network::RegisterListener),
	HOOK_VIRTUAL_DEF(0x01D0E670, Network::RemoveListener),
	HOOK_VIRTUAL_DEF(0x01D0E680, Network::GetSystem),
	HOOK_VIRTUAL_DEF(0x01D0E690, Network::GetSerial),
	HOOK_VIRTUAL_DEF(0x01D0E4D0, Network::GetStatusLine),
	HOOK_VIRTUAL_DEF(0x01D0E3B0, Network::GetType),
	HOOK_VIRTUAL_DEF(0x01D0E6B0, Network::GetName),
	HOOK_VIRTUAL_DEF(0x01D0E810, Network::GetState),
	HOOK_VIRTUAL_DEF(0x01D0E820, Network::GetVersion),
	HOOK_VIRTUAL_DEF(0x01D0E250, Network::ShutDown),
	HOOK_VIRTUAL_DEF(0x01D0E2E0, Network::CreateSocket),
	HOOK_VIRTUAL_DEF(0x01D0E3A0, Network::RemoveSocket),
	HOOK_VIRTUAL_DEF(0x01D0E4C0, Network::GetLocalAddress),
	HOOK_VIRTUAL_DEF(0x01D0E6C0, Network::ResolveAddress),
	HOOK_VIRTUAL_DEF(0x01D0E530, Network::GetFlowStats),
	HOOK_VIRTUAL_DEF(0x01D0E6A0, Network::GetLastErrorCode),
	HOOK_VIRTUAL_DEF(0x01D0E830, Network::GetErrorText),
	HOOK_VIRTUAL_DEF(0x01D0E2E0, Network::CreateSocket),
	HOOK_VIRTUAL_DEF(0x01D0E3A0, Network::RemoveSocket),
	HOOK_VIRTUAL_DEF(0x01D0E4C0, Network::GetLocalAddress),
	HOOK_VIRTUAL_DEF(0x01D0E6C0, Network::ResolveAddress),
	HOOK_VIRTUAL_DEF(0x01D0E530, Network::GetFlowStats),
	HOOK_VIRTUAL_DEF(0x01D0E6A0, Network::GetLastErrorCode),
	HOOK_VIRTUAL_DEF(0x01D0E830, Network::GetErrorText),

	// Network non-virtual functions
	HOOK_DEF(0x01D0EAE0, Network::UpdateStats),
	HOOK_DEF(0x01D0E440, Network::GetSocket),
	HOOK_DEF(0x01D0E400, Network::SendData),
	HOOK_DEF(0x01D0E3C0, Network::ReceiveData),

#endif // Network_region

#ifndef NetSocket_region

	// Network virtual functions
	HOOK_VIRTUAL_DEF(0x01D0D940, NetSocket::ReceivePacket),
	HOOK_VIRTUAL_DEF(0x01D0D950, NetSocket::FreePacket),
	HOOK_VIRTUAL_DEF(0x01D0D980, NetSocket::AddPacket),
	HOOK_VIRTUAL_DEF(0x01D0D000, NetSocket::AddChannel),
	HOOK_VIRTUAL_DEF(0x01D0D010, NetSocket::RemoveChannel),
	HOOK_VIRTUAL_DEF(0x01D0D310, NetSocket::GetNetwork),
	HOOK_VIRTUAL_DEF(0x01D0D8D0, NetSocket::OutOfBandPrintf),
	HOOK_VIRTUAL_DEF(0x01D0DA30, NetSocket::Flush),
	HOOK_VIRTUAL_DEF(0x01D0D8A0, NetSocket::GetFlowStats),
	HOOK_VIRTUAL_DEF(0x01D0D610, NetSocket::LeaveGroup),
	HOOK_VIRTUAL_DEF(0x01D0D570, NetSocket::JoinGroup),
	HOOK_VIRTUAL_DEF(0x01D0D500, NetSocket::Close),
	HOOK_VIRTUAL_DEF(0x01D0DCB0, NetSocket::GetPort),

	HOOK_VIRTUAL_DEF(0x01D0D160, NetSocket::SendPacket, bool(NetPacket *)),
	HOOK_VIRTUAL_DEF(0x01D0D190, NetSocket::SendPacket, bool(NetAddress *, const void *, int)),

	// Network non-virtual functions
	HOOK_DEF(0x01D0DAC0, NetSocket::Create),
	HOOK_DEF(0x01D0DCC0, NetSocket::UpdateStats),
	HOOK_DEF(0x01D0D6B0, NetSocket::DrainChannels),
	HOOK_DEF(0x01D0D020, NetSocket::DispatchIncoming),
	HOOK_DEF(0x01D0D330, NetSocket::ReceivePacketIntern),
	HOOK_DEF(0x01D0DD50, NetSocket::SendLong),
	HOOK_DEF(0x01D0DE70, NetSocket::SendShort),
	HOOK_DEF(0x01D0D730, NetSocket::GetLong),

#endif // NetSocket_region

#ifndef Delta_region

	HOOK_DEF(0x01D06740, DELTA_Init),
	HOOK_DEF(0x01D06810, DELTA_UpdateDescriptions),
	HOOK_DEF(0x01D06880, DELTA_Shutdown),
	HOOK_DEF(0x01D06890, DELTA_SetTime),
	HOOK_DEF(0x01D068B0, DELTA_SetLargeTimeBufferSize),
	HOOK_DEF(0x01D04AF0, DELTA_FindField),
	HOOK_DEF(0x01D04C10, DELTA_ClearFlags),
	HOOK_DEF(0x01D04C40, DELTA_CountSendFields),
	HOOK_DEF(0x01D04C80, DELTA_MarkSendFields),
	HOOK_DEF(0x01D04E80, DELTA_SetSendFlagBits),
	HOOK_DEF(0x01D04EF0, DELTA_WriteMarkedFields),
	HOOK_DEF(0x01D052A0, DELTA_WriteHeader),
	HOOK_DEF(0x01D053C0, DELTA_WriteDelta),
	HOOK_DEF(0x01D05470, DELTA_ParseDelta),
	HOOK_DEF(0x01D05A30, DELTA_TestDelta),
	HOOK_DEF(0x01D05CA0, DELTA_AddEncoder),
	HOOK_DEF(0x01D05CE0, DELTA_ClearEncoders),
	HOOK_DEF(0x01D05D60, DELTA_CountLinks),
	HOOK_DEF(0x01D05D80, DELTA_ReverseLinks),
	HOOK_DEF(0x01D05DA0, DELTA_ClearLinks),
	HOOK_DEF(0x01D05DD0, DELTA_BuildFromLinks),
	HOOK_DEF(0x01D05E80, DELTA_FindOffset),
	HOOK_DEF(0x01D05EC0, DELTA_ParseType),
	HOOK_DEF(0x01D06050, DELTA_ParseField),
	HOOK_DEF(0x01D06200, DELTA_FreeDescription),
	HOOK_DEF(0x01D06240, DELTA_AddDefinition),
	HOOK_DEF(0x01D062A0, DELTA_ClearDefinitions),
	HOOK_DEF(0x01D062E0, DELTA_FindDefinition),
	HOOK_DEF(0x01D06330, DELTA_SkipDescription),
	HOOK_DEF(0x01D06380, DELTA_ParseOneField),
	HOOK_DEF(0x01D06640, DELTA_RegisterDescription),
	HOOK_DEF(0x01D06670, DELTA_ClearRegistrations),
	HOOK_DEF(0x01D066C0, DELTA_LookupRegistration),

	//HOOK_DEF(0x01D06700, DELTA_ClearStats),		// NOXREF
	//HOOK_DEF(0x01D06430, DELTA_ParseDescription),		// NOXREF
	//HOOK_DEF(0x01D06630, DELTA_Load),			// NOXREF
	//HOOK_DEF(0x01D04B40, DELTA_FindFieldIndex),		// NOXREF
	//HOOK_DEF(0x01D04B90, DELTA_SetField),			// NOXREF
	//HOOK_DEF(0x01D04BB0, DELTA_UnsetField),		// NOXREF
	//HOOK_DEF(0x01D04BD0, DELTA_SetFieldByIndex),		// NOXREF
	//HOOK_DEF(0x01D04BF0, DELTA_UnsetFieldByIndex),	// NOXREF
	//HOOK_DEF(0x01D05270, DELTA_CheckDelta),		// NOXREF
	//HOOK_DEF(0x01D05D20, DELTA_LookupEncoder),		// NOXREF

#endif // Delta_region

#ifndef ObjectDictionary_region

	HOOK_DEF(0x01D0ECA0, MethodThunk<ObjectDictionary>::Constructor),
	HOOK_DEF(0x01D0ECE0, MethodThunk<ObjectDictionary>::Destructor),

	HOOK_VIRTUAL_DEF(0x01D0F220, ObjectDictionary::Init, void()),
	HOOK_VIRTUAL_DEF(0x01D0F290, ObjectDictionary::Add,  bool(void *)),
	HOOK_VIRTUAL_DEF(0x01D0F050, ObjectDictionary::Remove),
	HOOK_VIRTUAL_DEF(0x01D0ED10, ObjectDictionary::Clear),
	HOOK_VIRTUAL_DEF(0x01D0F310, ObjectDictionary::GetFirst),
	HOOK_VIRTUAL_DEF(0x01D0F5B0, ObjectDictionary::GetNext),
	HOOK_VIRTUAL_DEF(0x01D0F2A0, ObjectDictionary::CountElements),
	HOOK_VIRTUAL_DEF(0x01D0F2C0, ObjectDictionary::Contains),
	HOOK_VIRTUAL_DEF(0x01D0F2B0, ObjectDictionary::IsEmpty),

	HOOK_DEF(0x01D0F250, ObjectDictionary::Init, void(int)),		// NOXREF
	HOOK_DEF(0x01D0ED90, ObjectDictionary::Add,  bool(void *, float)),
	HOOK_DEF(0x01D0F4D0, ObjectDictionary::AddToCache, void(ObjectDictionary::entry_t *, float)),
	HOOK_DEF(0x01D0F490, ObjectDictionary::AddToCache, void(ObjectDictionary::entry_t *)),
	HOOK_DEF(0x01D0F340, ObjectDictionary::ChangeKey),
	HOOK_DEF(0x01D0F0D0, ObjectDictionary::RemoveKey),
	HOOK_DEF(0x01D0F590, ObjectDictionary::FindClosestKey),
	HOOK_DEF(0x01D0F5D0, ObjectDictionary::FindExactKey),
	HOOK_DEF(0x01D0F320, ObjectDictionary::GetLast),
	HOOK_DEF(0x01D0F510, ObjectDictionary::FindKeyInCache),
	HOOK_DEF(0x01D0F550, ObjectDictionary::FindObjectInCache),
	HOOK_DEF(0x01D0EF60, ObjectDictionary::ClearCache),
	HOOK_DEF(0x01D0F130, ObjectDictionary::CheckSize),
	HOOK_DEF(0x01D0EF80, ObjectDictionary::RemoveIndex),
	HOOK_DEF(0x01D0F010, ObjectDictionary::RemoveIndexRange),
	HOOK_DEF(0x01D0EE60, ObjectDictionary::FindClosestAsIndex),
	//HOOK_DEF(0x0, ObjectDictionary::RemoveRange),			// NOXREF
	//HOOK_DEF(0x01D0F440, ObjectDictionary::UnsafeChangeKey),	// NOXREF
	//HOOK_DEF(0x01D0F090, ObjectDictionary::RemoveSingle),		// NOXREF

#endif // ObjectDictionary_region

#ifndef ObjectList_region

	HOOK_DEF(0x01D0F610, MethodThunk<ObjectList>::Constructor),
	HOOK_DEF(0x01D0F650, MethodThunk<ObjectList>::Destructor),

	HOOK_VIRTUAL_DEF(0x01D0F8D0, ObjectList::Init),
	HOOK_VIRTUAL_DEF(0x01D0F920, ObjectList::Add),
	HOOK_VIRTUAL_DEF(0x01D0F850, ObjectList::Remove),
	HOOK_VIRTUAL_DEF(0x01D0F800, ObjectList::Clear),
	HOOK_VIRTUAL_DEF(0x01D0F8E0, ObjectList::GetFirst),
	HOOK_VIRTUAL_DEF(0x01D0F900, ObjectList::GetNext),
	HOOK_VIRTUAL_DEF(0x01D0F7C0, ObjectList::CountElements),
	HOOK_VIRTUAL_DEF(0x01D0F7D0, ObjectList::Contains),
	HOOK_VIRTUAL_DEF(0x01D0F7B0, ObjectList::IsEmpty),

	HOOK_DEF(0x01D0F760, ObjectList::RemoveTail),
	HOOK_DEF(0x01D0F6C0, ObjectList::RemoveHead),
	HOOK_DEF(0x01D0F710, ObjectList::AddTail),
	HOOK_DEF(0x01D0F670, ObjectList::AddHead),

#endif // ObjectList_region

#ifndef BitBuffer_region

	HOOK_DEF(0x01D015A0, MethodThunk<BitBuffer>::Destructor),
	HOOK_DEF(0x01D01530, (MethodThunk<BitBuffer>::Constructor), void()),
	HOOK_DEF(0x01D015B0, (MethodThunk<BitBuffer, unsigned int>::Constructor), void(unsigned int)),
	HOOK_DEF(0x01D01570, (MethodThunk<BitBuffer, void *, unsigned int>::Constructor), void(void *, unsigned int)),

	HOOK_DEF(0x01D015E0, BitBuffer::Resize),
	HOOK_DEF(0x01D01630, BitBuffer::Clear),
	HOOK_DEF(0x01D01670, BitBuffer::Reset),
	HOOK_DEF(0x01D01690, BitBuffer::Free),
	HOOK_DEF(0x01D01840, BitBuffer::PeekBits),
	HOOK_DEF(0x01D01EF0, BitBuffer::CurrentSize),
	HOOK_DEF(0x01D02300, BitBuffer::FastClear),
	HOOK_DEF(0x01D02350, BitBuffer::ConcatBuffer),
	HOOK_DEF(0x01D02210, BitBuffer::SkipBytes),
	HOOK_DEF(0x01D01660, BitBuffer::CurrentBit),
	HOOK_DEF(0x01D01F10, BitBuffer::SpaceLeft),
	HOOK_DEF(0x01D01F20, BitBuffer::AlignByte),
	HOOK_DEF(0x01D02090, BitBuffer::StartBitMode),
	HOOK_DEF(0x01D020A0, BitBuffer::EndBitMode),
	HOOK_DEF(0x01D020E0, BitBuffer::SetBuffer),
	HOOK_DEF(0x01D02240, BitBuffer::SkipBits),
	HOOK_DEF(0x01D022D0, BitBuffer::SkipString),

	// Read
	HOOK_DEF(0x01D016D0, BitBuffer::ReadBits),
	HOOK_DEF(0x01D017B0, BitBuffer::ReadBit),
	HOOK_DEF(0x01D01870, BitBuffer::ReadChar),
	HOOK_DEF(0x01D01880, BitBuffer::ReadByte),
	HOOK_DEF(0x01D01890, BitBuffer::ReadShort),
	HOOK_DEF(0x01D018A0, BitBuffer::ReadWord),
	HOOK_DEF(0x01D018B0, BitBuffer::ReadLong),
	HOOK_DEF(0x01D018C0, BitBuffer::ReadFloat),
	HOOK_DEF(0x01D018E0, BitBuffer::ReadBuf),
	HOOK_DEF(0x01D019C0, BitBuffer::ReadString),
	HOOK_DEF(0x01D01A00, BitBuffer::ReadStringLine),		// NOXREF
	HOOK_DEF(0x01D02020, BitBuffer::ReadBitString),
	HOOK_DEF(0x01D020B0, BitBuffer::ReadBitData),
	HOOK_DEF(0x01D02110, BitBuffer::ReadBitVec3Coord),
	HOOK_DEF(0x01D02170, BitBuffer::ReadBitCoord),
	HOOK_DEF(0x01D021F0, BitBuffer::ReadCoord),
	HOOK_DEF(0x01D01A40, BitBuffer::ReadAngle),				// NOXREF
	HOOK_DEF(0x01D01A60, BitBuffer::ReadHiresAngle),		// NOXREF
	HOOK_DEF(0x01D01F40, BitBuffer::ReadSBits),
	HOOK_DEF(0x01D01F70, BitBuffer::ReadBitAngle),

	// Write
	HOOK_DEF(0x01D01D90, BitBuffer::WriteBuf, void(const void *, int)),
	HOOK_DEF(0x01D01990, BitBuffer::WriteBuf, void(BitBuffer *, int)),

	HOOK_DEF(0x01D01A80, BitBuffer::WriteBit),
	HOOK_DEF(0x01D01B50, BitBuffer::WriteBits),
	HOOK_DEF(0x01D01C70, BitBuffer::WriteSBits),
	HOOK_DEF(0x01D01CD0, BitBuffer::WriteChar),
	HOOK_DEF(0x01D01CE0, BitBuffer::WriteByte),
	HOOK_DEF(0x01D01CF0, BitBuffer::WriteShort),
	HOOK_DEF(0x01D01D00, BitBuffer::WriteWord),
	HOOK_DEF(0x01D01D10, BitBuffer::WriteLong),
	HOOK_DEF(0x01D01D20, BitBuffer::WriteFloat),
	HOOK_DEF(0x01D01D50, BitBuffer::WriteString),
	HOOK_DEF(0x01D02370, BitBuffer::WriteCoord),
	HOOK_DEF(0x01D01E60, BitBuffer::WriteBitData),
	HOOK_DEF(0x01D01E90, BitBuffer::WriteAngle),			// NOXREF
	HOOK_DEF(0x01D01EC0, BitBuffer::WriteHiresAngle),		// NOXREF
	HOOK_DEF(0x01D01FB0, BitBuffer::WriteBitAngle),
	HOOK_DEF(0x01D02050, BitBuffer::WriteBitString),

#endif // BitBuffer_region

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

	GLOBALVAR_LINK(0x01D45708, "_ZL6g_defs", pg_defs),
	GLOBALVAR_LINK(0x01D4570C, "_ZL10g_encoders", pg_encoders),
	GLOBALVAR_LINK(0x01D45710, "g_deltaregistry", pg_deltaregistry),
	GLOBALVAR_LINK(0x01D456F0, "g_pplayerdelta", pg_pplayerdelta),
	GLOBALVAR_LINK(0x01D456F4, "g_pentitydelta", pg_pentitydelta),
	GLOBALVAR_LINK(0x01D456F8, "g_pcustomentitydelta", pg_pcustomentitydelta),
	GLOBALVAR_LINK(0x01D456FC, "g_pclientdelta", pg_pclientdelta),
	GLOBALVAR_LINK(0x01D45700, "g_pweapondelta", pg_pweapondelta),
	GLOBALVAR_LINK(0x01D45704, "g_peventdelta", pg_peventdelta),
	GLOBALVAR_LINK(0x01D456E8, "g_delta_Time", pg_delta_Time),
	GLOBALVAR_LINK(0x01D456E0, "g_large_Time_Buffers", pg_large_Time_Buffers),
	GLOBALVAR_LINK(0x01D46870, "g_DownloadURL", pg_DownloadURL),

	GLOBALVAR_LINK(0x01D442C8, "com_token", pcom_token),
	GLOBALVAR_LINK(0x01D456C8, "s_com_token_unget", ps_com_token_unget),

#endif // Data_References_region

	{ NULL, NULL, NULL },
};

#endif // HOOK_HLTV
