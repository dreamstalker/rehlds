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

// Hooks stuff
#include "hookers/hooker.cpp"

/*
	Requred hooks for runing hooker with rehlds API enabled:
	Whole regions:
		Common_MSG_region
		Common_SZ_region
		Sv_Steam3_region

	SysDll_region:
		Sys_FloatTime, Sys_InitHardwareTimer

	Pr_Cmds_region:
		SeedRandomNumberGenerator, ran1, fran1, RandomFloat, RandomLong, PF_GetPlayerAuthId

	Sv_Main_region:
		SV_RejectConnection, SV_FilterUser, SV_CheckChallenge, SV_CheckIPRestrictions, SV_CheckIPConnectionReuse, SV_FinishCertificateCheck, SV_CheckKeyInfo
		SV_CheckForDuplicateSteamID, SV_CheckForDuplicateNames, SV_CheckUserInfo, SV_ConnectClient, SVC_GetChallenge, SVC_ServiceChallenge,
		SV_SetMaxclients, SV_CompareUserID, SV_GetIDString, SV_GetClientIDString
*/

//#define Common_MSG_region
//#define Common_SZ_region
#define Common_COM_region
#define Delta_region
#define Main_region
#define Zone_region
#define FileSystem_Internal_region
#define FileSystem_region
#define Unicode_StrTools_region
#define Cmd_region
#define Cvar_region
#define Info_region
//#define SysDll_region
#define Sys_Dll2_region
#define CModel_region
#define Model_region
#define Sv_Log_region
#define Cl_Null_region
#define Snd_Null_region
//#define Sv_Steam3_region
#define Host_region
#define Host_Cmd_region
#define Pmove_region
#define Pmovetst_region
#define Pr_Edict_region
//#define Pr_Cmds_region
#define Mathlib_region
#define World_region
#define Sv_Phys_region
#define Sv_Move_region
#define Sv_pmove_region
//#define Sv_Main_region
#define R_Studio_region
#define TraceInit_Region
#define Net_ws_region
#define Net_chan_region
#define Hashpak_region
#define Sv_user_region
#define Wad_region
#define Textures_region
#define Tmessage_region
#define Vid_null_region
#define L_studio_region
#define Crc_region
#define Sv_RemoteAccess_region
#define Com_custom
#define Sv_upld
#define Decals
#define IpratelimitWrapper_region
#define Sys_engine
#define Sys_linuxwind
//#define SystemWrapper_region

//#define Function_References_region
//#define Data_References_region

FunctionHook g_FunctionHooks[] =
{
	// DO NOT DISABLE, other functions depends on memory allocation routines
#ifndef Mem_region

	HOOK_DEF(0x01D50AB0, Mem_Malloc),
	HOOK_DEF(0x01D50AD0, Mem_ZeroMalloc),
	HOOK_DEF(0x01D50B00, Mem_Realloc),
	HOOK_DEF(0x01D50B20, Mem_Calloc),
	HOOK_DEF(0x01D50B40, Mem_Strdup),
	HOOK_DEF(0x01D50B60, Mem_Free),

#ifdef _WIN32
	HOOK_SYMBOLDEF(0x01DFE945, "_malloca", malloc_wrapper),
	HOOK_SYMBOLDEF(0x01DFEA7F, "realloc", realloc_wrapper),
	HOOK_SYMBOLDEF(0x01DFE35D, "_frea", free_wrapper),
	HOOK_SYMBOLDEF(0x01DFF016, "calloc", calloc_wrapper),
	HOOK_SYMBOLDEF(0x01DFE957, "__nh_malloc", __nh_malloc_wrapper),
#endif //_WIN32

#endif // Mem_region

#ifndef Common_MSG_region

#ifdef Q_functions

	//HOOK_DEF(0x, Q_memset),
	//HOOK_DEF(0x, Q_memcpy),
	//HOOK_DEF(0x, Q_memcmp),
	HOOK_DEF(0x01D28AF0, Q_strcpy),
	//HOOK_DEF(0x, Q_strncpy),
	HOOK_DEF(0x01D28B50, Q_strlen),
	//HOOK_DEF(0x, Q_strrchr),
	//HOOK_DEF(0x, Q_strcat),
	//HOOK_DEF(0x, Q_strcmp),
	//HOOK_DEF(0x, Q_strncmp),
	//HOOK_DEF(0x, Q_strncasecmp),
	//HOOK_DEF(0x, Q_strcasecmp),
	//HOOK_DEF(0x, Q_stricmp),
	//HOOK_DEF(0x, Q_strnicmp),
	//HOOK_DEF(0x, Q_atoi),
	//HOOK_DEF(0x, Q_atof),
	//HOOK_DEF(0x, Q_strlwr),
	//HOOK_DEF(0x, Q_FileNameCmp),
	//HOOK_DEF(0x, Q_strstr),
	//HOOK_DEF(0x, Q_strtoull),

#endif // Q_functions

	HOOK_DEF(0x01D29290, MSG_WriteChar),
	HOOK_DEF(0x01D292B0, MSG_WriteByte),
	HOOK_DEF(0x01D292D0, MSG_WriteShort),
	HOOK_DEF(0x01D292F0, MSG_WriteWord),
	HOOK_DEF(0x01D29310, MSG_WriteLong),
	HOOK_DEF(0x01D29340, MSG_WriteFloat),
	HOOK_DEF(0x01D29370, MSG_WriteString),
	HOOK_DEF(0x01D293B0, MSG_WriteBuf),
	HOOK_DEF(0x01D293D0, MSG_WriteAngle),
	HOOK_DEF(0x01D29420, MSG_WriteHiresAngle),
	HOOK_DEF(0x01D29470, MSG_WriteUsercmd),
	HOOK_DEF(0x01D294D0, MSG_WriteOneBit),
	HOOK_DEF(0x01D29550, MSG_StartBitWriting),
	HOOK_DEF(0x01D29590, MSG_EndBitWriting),
	HOOK_DEF(0x01D295E0, MSG_WriteBits),
	HOOK_DEF(0x01D29700, MSG_WriteSBits),
	HOOK_DEF(0x01D29750, MSG_WriteBitString),
	HOOK_DEF(0x01D29790, MSG_WriteBitData),
	HOOK_DEF(0x01D297C0, MSG_WriteBitAngle),
	HOOK_DEF(0x01D29850, MSG_ReadBitAngle),
	HOOK_DEF(0x01D298A0, MSG_CurrentBit),
	HOOK_DEF(0x01D298E0, MSG_StartBitReading),
	HOOK_DEF(0x01D29930, MSG_EndBitReading),
	HOOK_DEF(0x01D29970, MSG_ReadOneBit),
	HOOK_DEF(0x01D29A00, MSG_ReadBits),
	HOOK_DEF(0x01D29B40, MSG_ReadSBits),
	HOOK_DEF(0x01D29BB0, MSG_ReadBitData),
	HOOK_DEF(0x01D29C70, MSG_WriteBitCoord),
	HOOK_DEF(0x01D29D50, MSG_WriteBitVec3Coord),
	HOOK_DEF(0x01D29E60, MSG_WriteCoord),
	HOOK_DEF(0x01D29F00, MSG_BeginReading),
	HOOK_DEF(0x01D29F10, MSG_ReadChar),
	HOOK_DEF(0x01D29F50, MSG_ReadByte),
	HOOK_DEF(0x01D29F90, MSG_ReadShort),
	HOOK_DEF(0x01D2A030, MSG_ReadLong),
	HOOK_DEF(0x01D2A090, MSG_ReadFloat),
	HOOK_DEF(0x01D2A0E0, MSG_ReadBuf),
	HOOK_DEF(0x01D2A130, MSG_ReadString),
	HOOK_DEF(0x01D2A170, MSG_ReadStringLine),
	HOOK_DEF(0x01D2A210, MSG_ReadUsercmd),
	HOOK_DEF(0x01D29B70, MSG_ReadBitString),	// NOXREF
	HOOK_DEF(0x01D29BE0, MSG_ReadBitCoord),		// NOXREF
	HOOK_DEF(0x01D29D00, MSG_ReadBitVec3Coord),	// NOXREF
	HOOK_DEF(0x01D29E30, MSG_ReadCoord),		// NOXREF
	HOOK_DEF(0x01D29E80, MSG_ReadVec3Coord),	// NOXREF
	HOOK_DEF(0x01D29EC0, MSG_WriteVec3Coord),	// NOXREF
	HOOK_DEF(0x01D29FE0, MSG_ReadWord),			// NOXREF
	HOOK_DEF(0x01D29580, MSG_IsBitWriting),		// NOXREF
	HOOK_DEF(0x01D298D0, MSG_IsBitReading),		// NOXREF
	HOOK_DEF(0x01D29B00, MSG_PeekBits),			// NOXREF
	HOOK_DEF(0x01D2A1B0, MSG_ReadAngle),		// NOXREF
	HOOK_DEF(0x01D2A1E0, MSG_ReadHiresAngle),	// NOXREF

#endif // Common_MSG_region

#ifndef Common_SZ_region

	HOOK_DEF(0x01D2A260, SZ_Alloc),
	HOOK_DEF(0x01D2A2A0, SZ_Clear),
	HOOK_DEF(0x01D2A2C0, SZ_GetSpace),
	HOOK_DEF(0x01D2A380, SZ_Write),
	HOOK_DEF(0x01D2A3B0, SZ_Print),

#endif // Common_SZ_region

#ifndef Common_COM_region

	HOOK_DEF(0x01D08E50, build_number),
	HOOK_DEF(0x01D289E0, Info_Serverinfo),
	HOOK_DEF(0x01D28FF0, COM_Nibble),
	HOOK_DEF(0x01D29030, COM_HexConvert),
	HOOK_DEF(0x01D29100, COM_ExplainDisconnection),
	HOOK_DEF(0x01D2A440, COM_StripExtension),
	HOOK_DEF(0x01D2A4A0, COM_FileExtension),
	HOOK_DEF(0x01D2A4F0, COM_FileBase),
	HOOK_DEF(0x01D2A580, COM_DefaultExtension),
	HOOK_DEF(0x01D2A5D0, COM_UngetToken),
	HOOK_DEF(0x01D2A5E0, COM_Parse),
	HOOK_DEF(0x01D2A730, COM_ParseLine),
	HOOK_DEF(0x01D2A830, COM_CheckParm),
	HOOK_DEF(0x01D2A880, COM_InitArgv),
	HOOK_DEF(0x01D2A960, COM_Init),
	HOOK_DEF(0x01D2AA00, va),
	HOOK_DEF(0x01D2AB70, COM_FixSlashes),
	HOOK_DEF(0x01D2AB90, COM_CreatePath),
	HOOK_DEF(0x01D2ACD0, COM_FileSize),
	HOOK_DEF(0x01D2AD10, COM_LoadFile),
	HOOK_DEF(0x01D2AE70, COM_FreeFile),
	HOOK_DEF(0x01D2AEA0, COM_CopyFileChunk),
	HOOK_DEF(0x01D2B020, COM_LoadHunkFile),
	HOOK_DEF(0x01D2B040, COM_LoadTempFile),
	HOOK_DEF(0x01D2B060, COM_LoadCacheFile),
	HOOK_DEF(0x01D2B0B0, COM_Shutdown),
	HOOK_DEF(0x01D2B0E0, COM_AddDefaultDir),
	HOOK_DEF(0x01D2B100, COM_StripTrailingSlash),
	HOOK_DEF(0x01D2B130, COM_ParseDirectoryFromCmd),
	HOOK_DEF(0x01D2B1C0, COM_SetupDirectories),
	//HOOK_DEF(0x01D2B3D0, COM_CheckPrintMap),	// Totally not exists on Linux (it is inlined), but present and used on Windows, anyway just commented it out
	HOOK_DEF(0x01D2B250, COM_ListMaps),
	HOOK_DEF(0x01D2B410, COM_Log),
	HOOK_DEF(0x01D2B480, COM_LoadFileForMe),
	HOOK_DEF(0x01D2B4A0, COM_CompareFileTime),
	HOOK_DEF(0x01D2B500, COM_GetGameDir),
	HOOK_DEF(0x01D2B530, COM_EntsForPlayerSlots),
	HOOK_DEF(0x01D2B590, COM_NormalizeAngles),
	HOOK_DEF(0x01D2B610, COM_Munge),
	HOOK_DEF(0x01D2B6A0, COM_UnMunge),
	HOOK_DEF(0x01D2B730, COM_Munge2),
	HOOK_DEF(0x01D2B7C0, COM_UnMunge2),
	HOOK_DEF(0x01D2B850, COM_Munge3),
	HOOK_DEF(0x01D2B970, COM_GetApproxWavePlayLength),
	HOOK_DEF(0x01D2B8E0, COM_UnMunge3),							// NOXREF
	HOOK_DEF(0x01D2B080, COM_LoadStackFile),					// NOXREF
	HOOK_DEF(0x01D2B0C0, COM_AddAppDirectory),					// NOXREF
	HOOK_DEF(0x01D2ABD0, COM_CopyFile),							// NOXREF
	HOOK_DEF(0x01D2AC70, COM_ExpandFilename),					// NOXREF
	HOOK_DEF(0x01D2AAD0, COM_WriteFile),						// NOXREF
	HOOK_DEF(0x01D2A7A0, COM_TokenWaiting),						// NOXREF
	HOOK_DEF(0x01D29170, COM_ExtendedExplainDisconnection),		// NOXREF
	HOOK_DEF(0x01D2A410, COM_SkipPath),							// NOXREF
	HOOK_DEF(0x01D2AA40, vstr),									// NOXREF
	HOOK_DEF(0x01D2AAA0, memsearch),							// NOXREF
	//HOOK_DEF(0x01D29080, COM_BinPrintf),						// NOXREF
	//HOOK_DEF(0x01D2AF40, COM_LoadFileLimit),					// NOXREF

#endif // Common_COM_region

#ifndef Delta_region

	HOOK_DEF(0x01D351D0, DELTA_FindField),
	HOOK_DEF(0x01D35240, DELTA_FindFieldIndex),
	HOOK_DEF(0x01D352A0, DELTA_SetField),
	HOOK_DEF(0x01D352C0, DELTA_UnsetField),
	HOOK_DEF(0x01D352E0, DELTA_SetFieldByIndex),
	HOOK_DEF(0x01D35300, DELTA_UnsetFieldByIndex),
	HOOK_DEF(0x01D35320, DELTA_ClearFlags),
	HOOK_DEF(0x01D35340, DELTA_TestDelta),
	HOOK_DEF(0x01D355B0, DELTA_CountSendFields),
	HOOK_DEF(0x01D355E0, DELTA_MarkSendFields),
	HOOK_DEF(0x01D35830, DELTA_SetSendFlagBits),
	HOOK_DEF(0x01D358B0, DELTA_WriteMarkedFields),
	HOOK_DEF(0x01D35C10, DELTA_CheckDelta),
	HOOK_DEF(0x01D35C40, DELTA_WriteDelta),
	HOOK_DEF(0x01D35C80, _DELTA_WriteDelta),
	HOOK_DEF(0x01D35D00, DELTA_ParseDelta),

	HOOK_DEF(0x01D36420, DELTA_AddEncoder),
	HOOK_DEF(0x01D36460, DELTA_ClearEncoders),
	HOOK_DEF(0x01D364A0, DELTA_LookupEncoder),
	HOOK_DEF(0x01D364E0, DELTA_CountLinks),
	HOOK_DEF(0x01D36500, DELTA_ReverseLinks),
	HOOK_DEF(0x01D36520, DELTA_ClearLinks),
	HOOK_DEF(0x01D36550, DELTA_BuildFromLinks),
	HOOK_DEF(0x01D365F0, DELTA_FindOffset),
	HOOK_DEF(0x01D36640, DELTA_ParseType),
	HOOK_DEF(0x01D36800, DELTA_ParseField),
	HOOK_DEF(0x01D36A10, DELTA_FreeDescription),
	HOOK_DEF(0x01D36A50, DELTA_AddDefinition),
	HOOK_DEF(0x01D36AB0, DELTA_ClearDefinitions),
	HOOK_DEF(0x01D36AF0, DELTA_FindDefinition),
	HOOK_DEF(0x01D36B40, DELTA_SkipDescription),
	HOOK_DEF(0x01D36BA0, DELTA_ParseOneField),
	HOOK_DEF(0x01D36C50, DELTA_ParseDescription),
	HOOK_DEF(0x01D36EA0, DELTA_Load),
	HOOK_DEF(0x01D36EF0, DELTA_RegisterDescription),
	HOOK_DEF(0x01D36F30, DELTA_ClearRegistrations),
	HOOK_DEF(0x01D36FC0, DELTA_ClearStats),
	HOOK_DEF(0x01D36FF0, DELTA_ClearStats_f),
	HOOK_DEF(0x01D36F80, DELTA_LookupRegistration),
	HOOK_DEF(0x01D37020, DELTA_PrintStats),
	HOOK_DEF(0x01D37090, DELTA_DumpStats_f),
	HOOK_DEF(0x01D370C0, DELTA_Init),
	HOOK_DEF(0x01D37160, DELTA_Shutdown),

#endif // Delta_region

#ifndef Sv_Main_region

	//HOOK_DEF(0x01D87E40, SV_LookupDelta),
	//HOOK_DEF(0x01D87F70, SV_GatherStatistics),
	//HOOK_DEF(0x01D88190, SV_DeallocateDynamicData),
	//HOOK_DEF(0x01D881D0, SV_ReallocateDynamicData),
	//HOOK_DEF(0x01D88250, SV_AllocClientFrames),
	//HOOK_DEF(0x01D882B0, SV_IsPlayerIndex_wrapped),
	//HOOK_DEF(0x01D882D0, SV_ClearPacketEntities),
	//HOOK_DEF(0x01D88310, SV_AllocPacketEntities),
	//HOOK_DEF(0x01D88370, SV_ClearFrames),
	//HOOK_DEF(0x01D883E0, SV_Serverinfo_f),
	//HOOK_DEF(0x01D884C0, SV_Localinfo_f),
	//HOOK_DEF(0x01D88550, SV_User_f),
	//HOOK_DEF(0x01D88640, SV_Users_f),
	//HOOK_DEF(0x01D88700, SV_CountPlayers),
	//HOOK_DEF(0x01D88740, SV_CountProxies),
	//HOOK_DEF(0x01D88790, SV_FindModelNumbers),
	//HOOK_DEF(0x01D887D0, SV_StartParticle),
	//HOOK_DEF(0x01D88880, SV_StartSound),
	//HOOK_DEF(0x01D88960, SV_BuildSoundMsg),
	//HOOK_DEF(0x01D88BD0, SV_HashString),
	//HOOK_DEF(0x01D88C10, SV_LookupSoundIndex),
	//HOOK_DEF(0x01D88CD0, SV_BuildHashedSoundLookupTable),
	//HOOK_DEF(0x01D88D20, SV_AddSampleToHashedLookupTable),
	//HOOK_DEF(0x01D88DA0, SV_ValidClientMulticast),
	//HOOK_DEF(0x01D88E40, SV_Multicast),
	//HOOK_DEF(0x01D88FA0, SV_WriteMovevarsToClient),
	//HOOK_DEF(0x01D89110, SV_WriteDeltaDescriptionsToClient),
	//HOOK_DEF(0x01D891B0, SV_SetMoveVars),
	//HOOK_DEF(0x01D892F0, SV_QueryMovevarsChanged),
	//HOOK_DEF(0x01D89590, SV_SendServerinfo),
	//HOOK_DEF(0x01D897C0, SV_SendResources),
	//HOOK_DEF(0x01D89920, SV_WriteClientdataToMessage),
	//HOOK_DEF(0x01D89BF0, SV_WriteSpawn),
	//HOOK_DEF(0x01D89F10, SV_SendUserReg),
	//HOOK_DEF(0x01D89F80, SV_New_f),
	//HOOK_DEF(0x01D8A210, SV_SendRes_f),
	//HOOK_DEF(0x01D8A2C0, SV_Spawn_f),
	//HOOK_DEF(0x01D8A3F0, SV_CheckUpdateRate),
	HOOK_DEF(0x01D8A510, SV_RejectConnection),
	//HOOK_DEF(0x01D8A5A0, SV_RejectConnectionForPassword),
	//HOOK_DEF(0x01D8A610, SV_GetFragmentSize),
	HOOK_DEF(0x01D8A680, SV_FilterUser),
	//HOOK_DEF(0x01D8A760, SV_CheckProtocol),
	HOOK_DEF(0x01D8A7E0, SV_CheckChallenge),
	HOOK_DEF(0x01D8A8D0, SV_CheckIPRestrictions),
	HOOK_DEF(0x01D8A980, SV_CheckIPConnectionReuse),
	HOOK_DEF(0x01D8AA40, SV_FinishCertificateCheck),
	HOOK_DEF(0x01D8AB10, SV_CheckKeyInfo),
	HOOK_DEF(0x01D8AC30, SV_CheckForDuplicateSteamID),
	HOOK_DEF(0x01D8ACE0, SV_CheckForDuplicateNames),
	HOOK_DEF(0x01D8AE10, SV_CheckUserInfo),
	//HOOK_DEF(0x01D8B080, SV_FindEmptySlot),
	HOOK_DEF(0x01D8B100, SV_ConnectClient),
	//HOOK_DEF(0x01D8B8E0, SVC_Ping),
	HOOK_DEF(0x01D8B930, SVC_GetChallenge),
	HOOK_DEF(0x01D8BB20, SVC_ServiceChallenge),
	//HOOK_DEF(0x01D8C260, SVC_InfoString),					// NOXREF
	//HOOK_DEF(0x01D8C720, SVC_Info),						// NOXREF
	//HOOK_DEF(0x01D8CA40, SVC_PlayerInfo),					// NOXREF
	//HOOK_DEF(0x01D8CBA0, SVC_RuleInfo),					// NOXREF
	//HOOK_DEF(0x01D8CCC0, SVC_GameDllQuery),
	//HOOK_DEF(0x01D8BCB0, SV_ResetModInfo),
	//HOOK_DEF(0x01D8CD70, SV_FlushRedirect),
	//HOOK_DEF(0x01D8CE70, SV_BeginRedirect),
	//HOOK_DEF(0x01D8CEA0, SV_EndRedirect),
	//HOOK_DEF(0x01D8CEB0, SV_ResetRcon_f),
	//HOOK_DEF(0x01D8CED0, SV_AddFailedRcon),
	//HOOK_DEF(0x01D8D1F0, SV_CheckRconFailure),
	//HOOK_DEF(0x01D8D250, SV_Rcon_Validate),
	//HOOK_DEF(0x01D8D370, SV_Rcon),
	//HOOK_DEF(0x01D8D560, SV_ConnectionlessPacket),
	//HOOK_DEF(0x01D8D750, SV_CheckRate),
	//HOOK_DEF(0x01D8D810, SV_ProcessFile),
	//HOOK_DEF(0x01D8D960, SV_FilterPacket),
	//HOOK_DEF(0x01D8DA30, SV_SendBan),
	//HOOK_DEF(0x01D8DAB0, SV_ReadPackets),
	//HOOK_DEF(0x, ntohl),
	//HOOK_DEF(0x, htons),
	//HOOK_DEF(0x01D8DCC0, SV_CheckTimeouts),
	//HOOK_DEF(0x01D8DD50, SV_CalcPing),
	//HOOK_DEF(0x01D8DE20, SV_FullClientUpdate),
	//HOOK_DEF(0x01D8DEF0, SV_EmitEvents),
	//HOOK_DEF(0x01D8E0F0, SV_AddToFatPVS),
	//HOOK_DEF(0x01D8E1B0, SV_FatPVS),
	//HOOK_DEF(0x01D8E200, SV_AddToFatPAS),
	//HOOK_DEF(0x01D8E2D0, SV_FatPAS),
	//HOOK_DEF(0x01D8E320, SV_PointLeafnum),
	//HOOK_DEF(0x01D8E370, SV_SetCallback),
	//HOOK_DEF(0x01D8E3C0, SV_SetNewInfo),
	//HOOK_DEF(0x01D8E3E0, SV_WriteDeltaHeader),
	//HOOK_DEF(0x01D8E4E0, SV_InvokeCallback),
	//HOOK_DEF(0x01D8E520, SV_FindBestBaseline),
	//HOOK_DEF(0x01D8E650, SV_CreatePacketEntities),
	//HOOK_DEF(0x01D8E9A0, SV_EmitPacketEntities),
	//HOOK_DEF(0x01D8E9E0, SV_ShouldUpdatePing),
	//HOOK_DEF(0x01D8EA70, SV_GetNetInfo),
	//HOOK_DEF(0x01D8EB00, SV_CheckVisibility),
	//HOOK_DEF(0x01D8EBF0, SV_EmitPings),
	//HOOK_DEF(0x01D8EC90, SV_WriteEntitiesToClient),
	//HOOK_DEF(0x01D8EE90, SV_CleanupEnts),
	//HOOK_DEF(0x01D8EEC0, SV_SendClientDatagram),
	//HOOK_DEF(0x01D8EFC0, SV_UpdateToReliableMessages),
	//HOOK_DEF(0x01D8F230, SV_SkipUpdates),
	//HOOK_DEF(0x01D8F280, SV_SendClientMessages),
	//HOOK_DEF(0x01D8F470, SV_ExtractFromUserinfo),
	//HOOK_DEF(0x01D8F870, SV_ModelIndex),
	//HOOK_DEF(0x01D8F8E0, SV_AddResource),
	//HOOK_DEF(0x01D8F950, SV_CreateGenericResources),
	//HOOK_DEF(0x01D8FC30, SV_CreateResourceList),
	//HOOK_DEF(0x01D8FDC0, SV_ClearCaches),
	//HOOK_DEF(0x01D8FE00, SV_PropagateCustomizations),
	//HOOK_DEF(0x01D8FF00, SV_WriteVoiceCodec),
	//HOOK_DEF(0x01D8FF30, SV_CreateBaseline),
	//HOOK_DEF(0x01D90170, SV_BroadcastCommand),
	//HOOK_DEF(0x01D90260, SV_BuildReconnect),
	//HOOK_DEF(0x01D903D0, SetCStrikeFlags),
	//HOOK_DEF(0x01D904E0, SV_ActivateServer),
	//HOOK_DEF(0x01D90790, SV_ServerShutdown),
	//HOOK_DEF(0x01D907C0, SV_SpawnServer),
	//HOOK_DEF(0x01D90E70, SV_LoadEntities),
	//HOOK_DEF(0x01D90E90, SV_ClearEntities),
	//HOOK_DEF(0x01D90ED0, RegUserMsg),
	//HOOK_DEF(0x01D90F80, StringToFilter),
	//HOOK_DEF(0x01D91020, SV_StringToUserID),
	//HOOK_DEF(0x01D910C0, SV_BanId_f),
	//HOOK_DEF(0x01D915C0, Host_Kick_f),
	//HOOK_DEF(0x01D91990, SV_RemoveId_f),
	//HOOK_DEF(0x01D91B90, SV_WriteId_f),
	//HOOK_DEF(0x01D91C60, SV_ListId_f),
	//HOOK_DEF(0x01D91D00, SV_AddIP_f),
	//HOOK_DEF(0x01D91F00, SV_RemoveIP_f),
	//HOOK_DEF(0x01D91FD0, SV_ListIP_f),
	//HOOK_DEF(0x01D920B0, SV_WriteIP_f),
	//HOOK_DEF(0x01D92190, SV_KickPlayer),
	//HOOK_DEF(0x01D92300, SV_InactivateClients),
	//HOOK_DEF(0x01D923A0, SV_FailDownload),
	//HOOK_DEF(0x01D8EA40, SV_HasEventsInQueue),			// NOXREF
	//HOOK_DEF(0x01D90280, SV_ReconnectAllClients),			// NOXREF
	//HOOK_DEF(0x01D8BE40, SV_GetFakeClientCount),			// NOXREF
	//HOOK_DEF(0x01D8BE70, SV_GetModInfo),					// NOXREF
	//HOOK_DEF(0x01D87E90, SV_DownloadingModules),			// NOXREF
	//HOOK_DEF(0x01D923E0, Q_stristr),
	//HOOK_DEF(0x01D92480, IsSafeFileToDownload),
	//HOOK_DEF(0x01D92710, SV_BeginFileDownload_f),
	HOOK_DEF(0x01D92940, SV_SetMaxclients),
	//HOOK_DEF(0x01D92B00, SV_HandleRconPacket),
	//HOOK_DEF(0x01D92B70, SV_CheckCmdTimes),
	//HOOK_DEF(0x01D92C60, SV_CheckForRcon),
	//HOOK_DEF(0x01D92CC0, SV_IsSimulating),
	//HOOK_DEF(0x01D92D00, SV_CheckMapDifferences),
	//HOOK_DEF(0x01D92D80, SV_Frame),
	//HOOK_DEF(0x01D92E00, SV_Drop_f),
	//HOOK_DEF(0x01D92E40, SV_RegisterDelta),
	//HOOK_DEF(0x01D92EC0, SV_InitDeltas),
	//HOOK_DEF(0x01D93010, SV_InitEncoders),
	//HOOK_DEF(0x01D93050, SV_Init),
	//HOOK_DEF(0x01D93600, SV_Shutdown),
	HOOK_DEF(0x01D93650, SV_CompareUserID),
	HOOK_DEF(0x01D936D0, SV_GetIDString),
	HOOK_DEF(0x01D938E0, SV_GetClientIDString),
	//HOOK_DEF(0x01D93950, GetGameAppID),
	//HOOK_DEF(0x01D939C0, IsGameSubscribed),
	//HOOK_DEF(0x, TRACE_DELTA),							// NOXREF

	//HOOK_DEF(0x01D93A10, BIsValveGame),					// NOXREF
	//HOOK_DEF(0x01D8BF40, RequireValidChallenge),			// NOXREF
	//HOOK_DEF(0x01D8BF60, ValidInfoChallenge),				// NOXREF
	//HOOK_DEF(0x01D8BFB0, GetChallengeNr),					// NOXREF
	//HOOK_DEF(0x01D8C0C0, CheckChallengeNr),				// NOXREF
	//HOOK_DEF(0x01D8C180, ReplyServerChallenge),			// NOXREF
	//HOOK_DEF(0x01D8C200, ValidChallenge),					// NOXREF

#endif // Sv_Main_region

#ifndef Zone_region

	HOOK_DEF(0x01DBB120, Z_ClearZone),
	HOOK_DEF(0x01DBB170, Z_Free),
	HOOK_DEF(0x01DBB220, Z_Malloc),
	HOOK_DEF(0x01DBB260, Z_TagMalloc),
	HOOK_DEF(0x01DBB310, Z_Print),				// NOXREF
	HOOK_DEF(0x01DBB3C0, Z_CheckHeap),

	HOOK_DEF(0x01DBB440, Hunk_Check),
	HOOK_DEF(0x01DBB4B0, Hunk_Print),			// NOXREF
	HOOK_DEF(0x01DBB6B0, Hunk_AllocName),
	HOOK_DEF(0x01DBB750, Hunk_Alloc),
	HOOK_DEF(0x01DBB770, Hunk_LowMark),
	HOOK_DEF(0x01DBB780, Hunk_FreeToLowMark),
	HOOK_DEF(0x01DBB7B0, Hunk_HighMark),
	HOOK_DEF(0x01DBB7E0, Hunk_FreeToHighMark),
	HOOK_DEF(0x01DBB830, Hunk_HighAllocName),
	HOOK_DEF(0x01DBB900, Hunk_TempAlloc),

	HOOK_DEF(0x01DBB960, Cache_Move),
	HOOK_DEF(0x01DBB9D0, Cache_FreeLow),
	HOOK_DEF(0x01DBBA00, Cache_FreeHigh),
	HOOK_DEF(0x01DBBA60, Cache_UnlinkLRU),
	HOOK_DEF(0x01DBBAA0, Cache_MakeLRU),
	HOOK_DEF(0x01DBBAF0, Cache_TryAlloc),
	HOOK_DEF(0x01DBBC30, Cache_Force_Flush),
	HOOK_DEF(0x01DBBC60, Cache_Flush),
	HOOK_DEF(0x01DBBC90, Cache_Print),			// NOXREF
	//HOOK_DEF(0x, ComparePath1),				// NOXREF		// not yet located on windows
	//HOOK_DEF(0x, CommatizeNumber),			// NOXREF	// not yet located on windows
	//HOOK_DEF(0x, Cache_Report),				// NOXREF		// not yet located on windows
	//HOOK_DEF(0x, Cache_Compact),				// NOXREF		// not yet located on windows
	HOOK_DEF(0x01DBBED0, Cache_Init),
	HOOK_DEF(0x01DBBF00, Cache_Free),
	HOOK_DEF(0x01DBBF70, Cache_Check),
	HOOK_DEF(0x01DBBFA0, Cache_Alloc),
	HOOK_DEF(0x01DBC040, Memory_Init),
	HOOK_DEF(0x01DBBD60, CacheSystemCompare),
	HOOK_DEF(0x01DBBF50, Cache_TotalUsed),	// NOXREF		// NOXREF
	//HOOK_DEF(0x01DBC0E0, Cache_Print_Models_And_Totals),	// NOXREF
	//HOOK_DEF(0x01DBC1F0, Cache_Print_Sounds_And_Totals),	// NOXREF

#endif // Zone_region

#ifndef FileSystem_Internal_region

	HOOK_DEF(0x01D3E2E0, FS_RemoveAllSearchPaths),	// NOXREF
	HOOK_DEF(0x01D3E2F0, FS_AddSearchPath),
	HOOK_DEF(0x01D3E310, FS_RemoveSearchPath),		// NOXREF
	HOOK_DEF(0x01D3E330, FS_RemoveFile),
	HOOK_DEF(0x01D3E350, FS_CreateDirHierarchy),
	HOOK_DEF(0x01D3E370, FS_FileExists),
	HOOK_DEF(0x01D3E390, FS_IsDirectory),			// NOXREF
	HOOK_DEF(0x01D3E3B0, FS_Open),
	HOOK_DEF(0x01D3E3D0, FS_OpenPathID),
	HOOK_DEF(0x01D3E3F0, FS_Close),
	HOOK_DEF(0x01D3E410, FS_Seek),
	HOOK_DEF(0x01D3E430, FS_Tell),
	HOOK_DEF(0x01D3E450, FS_Size),
	HOOK_DEF(0x01D3E470, FS_FileSize),
	HOOK_DEF(0x01D3E490, FS_GetFileTime),
	HOOK_DEF(0x01D3E4B0, FS_FileTimeToString),		// NOXREF
	HOOK_DEF(0x01D3E4D0, FS_IsOk),
	HOOK_DEF(0x01D3E4F0, FS_Flush),
	HOOK_DEF(0x01D3E510, FS_EndOfFile),
	HOOK_DEF(0x01D3E530, FS_Read),
	HOOK_DEF(0x01D3E550, FS_Write),
	HOOK_DEF(0x01D3E570, FS_ReadLine),
	HOOK_DEF(0x01D3E590, FS_FPrintf),
	HOOK_DEF(0x01D3E5E0, FS_FindFirst),
	HOOK_DEF(0x01D3E600, FS_FindNext),
	HOOK_DEF(0x01D3E620, FS_FindIsDirectory),			// NOXREF
	HOOK_DEF(0x01D3E640, FS_FindClose),
	HOOK_DEF(0x01D3E660, FS_GetLocalCopy),
	HOOK_DEF(0x01D3E680, FS_GetLocalPath),
	HOOK_DEF(0x01D3E6A0, FS_ParseFile),					// NOXREF
	HOOK_DEF(0x01D3E6E0, FS_FullPathToRelativePath),	// NOXREF
	HOOK_DEF(0x01D3E700, FS_GetCurrentDirectory),		// NOXREF
	HOOK_DEF(0x01D3E720, FS_PrintOpenedFiles),			// NOXREF
	HOOK_DEF(0x01D3E730, FS_SetWarningFunc),			// NOXREF
	HOOK_DEF(0x01D3E750, FS_SetWarningLevel),			// NOXREF
	HOOK_DEF(0x01D3E770, FS_GetCharacter),				// NOXREF
	HOOK_DEF(0x01D3E790, FS_LogLevelLoadStarted),
	HOOK_DEF(0x01D3E7B0, FS_LogLevelLoadFinished),
	HOOK_DEF(0x01D3E7D0, FS_SetVBuf),
	HOOK_DEF(0x01D3E800, FS_GetInterfaceVersion),
	HOOK_DEF(0x01D3E820, FS_GetReadBuffer),
	HOOK_DEF(0x01D3E840, FS_ReleaseReadBuffer),
	HOOK_DEF(0x01D3E860, FS_Unlink),
	HOOK_DEF(0x01D3E8A0, FS_Rename),
	HOOK_DEF(0x01D3E940, FS_LoadLibrary),

#endif // FileSystem_Internal_region

#ifndef FileSystem_region

	HOOK_DEF(0x01D3D340, GetBaseDirectory),
	//HOOK_DEF(0x01D3E250, FileSystem_LoadDLL),		// Totally not exists on Linux (it is inlined), but present and used on Windows, anyway just commented it out
	//HOOK_DEF(0x01D3E2C0, FileSystem_UnloadDLL),	// Totally not exists on Linux (it is inlined), but present and used on Windows, anyway just commented it out
	HOOK_DEF(0x01D3D360, BEnabledHDAddon),
	HOOK_DEF(0x01D3D390, BEnableAddonsFolder),

	HOOK_DEF(0x01D3D3C0, Host_SetHDModels_f),
	HOOK_DEF(0x01D3D440, Host_SetAddonsFolder_f),
	HOOK_DEF(0x01D3D4C0, Host_SetVideoLevel_f),
	HOOK_DEF(0x01D3D510, Host_GetVideoLevel),

	HOOK_DEF(0x01D3DA60, FileSystem_SetGameDirectory),
	HOOK_DEF(0x01D3E130, FileSystem_AddFallbackGameDir),

	HOOK_SYMBOLDEF(0x01D3D355, "_Z20GetFileSystemFactoryv", GetFileSystemFactory),						// NOXREF
	HOOK_SYMBOLDEF(0x01D3D530, "_Z26CheckLiblistForFallbackDirPKcbS0_b", CheckLiblistForFallbackDir),
	HOOK_SYMBOLDEF(0x01D3E200, "_Z15FileSystem_InitPcPv", FileSystem_Init),
	HOOK_SYMBOLDEF(0x01D3E2B0, "_Z19FileSystem_Shutdownv", FileSystem_Shutdown),

#endif // FileSystem_region

#ifndef Unicode_StrTools_region

	HOOK_SYMBOLDEF(0x01DA8F00, "_Z16Q_IsValidUChar32w", Q_IsValidUChar32),
	HOOK_SYMBOLDEF(0x01DA97D0, "_Z15Q_UTF8ToUChar32PKcRwRb", Q_UTF8ToUChar32),

	HOOK_DEF(0x01DA9910, Q_UnicodeValidate),
	HOOK_DEF(0x01DA99A0, Q_UnicodeAdvance),
	HOOK_DEF(0x01DA9EA0, Q_UnicodeRepair),
	HOOK_DEF(0x01DA9E70, V_UTF8ToUChar32),

#endif // Unicode_StrTools_region

#ifndef Cmd_region

	HOOK_DEF(0x01D26CD0, Cmd_Wait_f),
	HOOK_DEF(0x01D26CE0, Cbuf_Init),
	HOOK_DEF(0x01D26D00, Cbuf_AddText),
	HOOK_DEF(0x01D26D50, Cbuf_InsertText),
	HOOK_DEF(0x01D26DE0, Cbuf_InsertTextLines),
	HOOK_DEF(0x01D26E80, Cbuf_Execute),
	HOOK_DEF(0x01D26F60, Cmd_StuffCmds_f),
	HOOK_DEF(0x01D270F0, Cmd_Exec_f),
	HOOK_DEF(0x01D273A0, Cmd_Echo_f),
	HOOK_DEF(0x01D273E0, CopyString),
	HOOK_DEF(0x01D27410, Cmd_Alias_f),
	HOOK_DEF(0x01D276A0, Cmd_GetFirstCmd),
	HOOK_DEF(0x01D276B0, Cmd_Init),
	HOOK_DEF(0x01D27720, Cmd_Shutdown),
	HOOK_DEF(0x01D27750, Cmd_Argc),
	HOOK_DEF(0x01D27760, Cmd_Argv),
	HOOK_DEF(0x01D27790, Cmd_Args),
	HOOK_DEF(0x01D277A0, Cmd_TokenizeString),

	HOOK_DEF(0x01D27900, Cmd_AddCommand),
	HOOK_DEF(0x01D27A10, Cmd_AddMallocCommand),
	HOOK_DEF(0x01D27AF0, Cmd_AddGameCommand),
	HOOK_DEF(0x01D27B10, Cmd_RemoveMallocedCmds),
	HOOK_DEF(0x01D27B60, Cmd_RemoveGameCmds),
	HOOK_DEF(0x01D27B70, Cmd_RemoveWrapperCmds),
	HOOK_DEF(0x01D27B80, Cmd_Exists),
	HOOK_DEF(0x01D27D10, Cmd_ExecuteString),
	HOOK_DEF(0x01D27DF0, Cmd_ForwardToServerInternal),
	HOOK_DEF(0x01D27F40, Cmd_ForwardToServer),
	HOOK_DEF(0x01D27F90, Cmd_ForwardToServerUnreliable),
	HOOK_DEF(0x01D28000, Cmd_CmdList_f),
	HOOK_DEF(0x01D27AB0, Cmd_AddHUDCommand),			// NOXREF
	HOOK_DEF(0x01D27AD0, Cmd_AddWrapperCommand),		// NOXREF
	HOOK_DEF(0x01D27B50, Cmd_RemoveHudCmds),			// NOXREF
	HOOK_DEF(0x01D27BC0, Cmd_CompleteCommand),			// NOXREF
	HOOK_DEF(0x01D27FA0, Cmd_CheckParm),				// NOXREF
	HOOK_DEF(0x01D27880, Cmd_FindCmd),					// NOXREF
	HOOK_DEF(0x01D278C0, Cmd_FindCmdPrev),				// NOXREF
#endif // Cmd_region

#ifndef Cvar_region

	HOOK_DEF(0x01D2D760, Cvar_Init),
	HOOK_DEF(0x01D2D770, Cvar_Shutdown),
	HOOK_DEF(0x01D2D780, Cvar_FindVar),
	HOOK_DEF(0x01D2D800, Cvar_VariableValue),
	HOOK_DEF(0x01D2D860, Cvar_VariableString),
	HOOK_DEF(0x01D2D9C0, Cvar_DirectSet),
	HOOK_DEF(0x01D2DC30, Cvar_Set),
	HOOK_DEF(0x01D2DC70, Cvar_SetValue),
	HOOK_DEF(0x01D2DD20, Cvar_RegisterVariable),
	HOOK_DEF(0x01D2DE70, Cvar_IsMultipleTokens),
	HOOK_DEF(0x01D2DED0, Cvar_Command),
	HOOK_DEF(0x01D2E350, Cvar_UnlinkExternals),
	HOOK_DEF(0x01D2E380, Cvar_CmdInit),
	HOOK_DEF(0x01D2D7C0, Cvar_FindPrevVar),				// NOXREF
	HOOK_DEF(0x01D2D830, Cvar_VariableInt),				// NOXREF
	HOOK_DEF(0x01D2D880, Cvar_CompleteVariable),		// NOXREF
	HOOK_DEF(0x01D2DE10, Cvar_RemoveHudCvars),			// NOXREF
	HOOK_DEF(0x01D2DFA0, Cvar_WriteVariables),			// NOXREF
	HOOK_DEF(0x01D2E330, Cvar_CountServerVariables),	// NOXREF
	HOOK_DEF(0x01D2DFE0, Cmd_CvarListPrintCvar),
	HOOK_DEF(0x01D2E0F0, Cmd_CvarList_f),

#endif // Cvar_region

#ifndef Info_region

	HOOK_DEF(0x01D4B610, Info_ValueForKey),
	HOOK_DEF(0x01D4B6E0, Info_RemoveKey),
	HOOK_DEF(0x01D4B7D0, Info_RemovePrefixedKeys),
	HOOK_DEF(0x01D4B860, Info_IsKeyImportant),
	HOOK_DEF(0x01D4B980, Info_FindLargestKey),
	HOOK_DEF(0x01D4BA80, Info_SetValueForStarKey),
	HOOK_DEF(0x01D4BCF0, Info_SetValueForKey),
	HOOK_DEF(0x01D4BD30, Info_Print),
	HOOK_DEF(0x01D4BE10, Info_IsValid),

#endif // Info_region

#ifndef SysDll_region

	//HOOK_DEF(0x, Sys_PageIn),
	//HOOK_DEF(0x01D9ECF0, Sys_FindFirst),
	//HOOK_DEF(0x01D9ED50, Sys_FindFirstPathID),
	//HOOK_DEF(0x01D9ED90, Sys_FindNext),
	//HOOK_DEF(0x01D9EDC0, Sys_FindClose),
	//HOOK_DEF(0x01D9EDE0, glob_match_after_star),
	//HOOK_DEF(0x01D9EF50, glob_match),
	//HOOK_DEF(0x01D9EFD0, Sys_MakeCodeWriteable),
	//HOOK_DEF(0x, Sys_SetFPCW),
	//HOOK_DEF(0x, Sys_PushFPCW_SetHigh),
	//HOOK_DEF(0x, Sys_PopFPCW),
	//HOOK_DEF(0x, MaskExceptions),
	//HOOK_DEF(0x, Sys_Init),	// NOXREF
	//HOOK_DEF(0x, Sys_Sleep),
	//HOOK_DEF(0x, Sys_DebugOutStraight),
	//HOOK_DEF(0x01D9F0E0, Sys_Error),
	//HOOK_DEF(0x01D9F1F0, Sys_Warning),
	//HOOK_DEF(0x01D9F230, Sys_Printf),
	//HOOK_DEF(0x01D9F4A0, Sys_Quit),
	HOOK_DEF(0x01D9F2A0, Sys_FloatTime),
	//HOOK_DEF(0x01D9F460, Dispatch_Substate),
	//HOOK_DEF(0x01D9F470, GameSetSubState),
	//HOOK_DEF(0x01D9F4A0, GameSetState),
	//HOOK_DEF(0x, GameSetBackground),
	//HOOK_DEF(0x01D9F7B0, Voice_GetClientListening),
	//HOOK_DEF(0x01D9F810, Voice_SetClientListening),
	//HOOK_DEF(0x01D9F890, GetDispatch),
	//HOOK_DEF(0x01D9F8D0, FindAddressInTable),
	//HOOK_DEF(0x01D9F910, FindNameInTable),
	//HOOK_DEF(0x, ConvertNameToLocalPlatform),
	//HOOK_DEF(0x01D9FAA0, FunctionFromName),
	//HOOK_DEF(0x01D9FB00, NameForFunction),
	//HOOK_DEF(0x01D9FB50, GetEntityInit),
	//HOOK_DEF(0x01D9FB70, GetIOFunction),
	//HOOK_DEF(0x01D9FB90, DLL_SetModKey),
	//HOOK_DEF(0x01D9FE50, LoadEntityDLLs),
	//HOOK_DEF(0x01DA02D0, LoadThisDll),
	//HOOK_DEF(0x01DA0390, ReleaseEntityDlls),
	//HOOK_DEF(0x01DA0410, EngineFprintf),
	//HOOK_DEF(0x01DA0420, AlertMessage),
	//HOOK_DEF(0x01DA0640, Sys_SplitPath),
	//HOOK_DEF(0x01D2BD90, Con_Debug_f),
	//HOOK_DEF(0x01D2BDD0, Con_Init),
	//HOOK_DEF(0x01D2BFC0, Con_DebugLog),
	//HOOK_DEF(0x01D2C020, Con_Printf),
	//HOOK_DEF(0x01D2C1E0, Con_SafePrintf),
	//HOOK_DEF(0x01D2C140, Con_DPrintf),

#ifdef _WIN32
	HOOK_DEF(0x01DA0A70, Sys_InitHardwareTimer),
#endif //_WIN32

#endif // SysDll_region

#ifndef Sys_Dll2_region

	// virtual functions
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA16B0, "_ZN19CDedicatedServerAPI4InitEPcS0_PFP14IBaseInterfacePKcPiES7_", CDedicatedServerAPI::Init),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1800, "_ZN19CDedicatedServerAPI8ShutdownEv", CDedicatedServerAPI::Shutdown),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1860, "_ZN19CDedicatedServerAPI8RunFrameEv", CDedicatedServerAPI::RunFrame),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1880, "_ZN19CDedicatedServerAPI14AddConsoleTextEPc", CDedicatedServerAPI::AddConsoleText),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA18A0, "_ZN19CDedicatedServerAPI12UpdateStatusEPfPiS1_Pc", CDedicatedServerAPI::UpdateStatus),

	HOOK_DEF(0x01DA0670, GetCurrentSteamAppName),
	HOOK_DEF(0x01DA07A0, F),
	HOOK_DEF(0x01DA0820, Sys_GetCDKey),
	HOOK_DEF(0x01DA0D50, Sys_ShutdownMemory),

#ifdef _WIN32
	HOOK_SYMBOLDEF(0x01DA09E0, "_Z18Sys_CheckOSVersionv", Sys_CheckOSVersion),
#else
	//HOOK_SYMBOLDEF(0x0, "_Z19Sys_SetupLegacyAPIsv", Sys_SetupLegacyAPIs),
#endif

	HOOK_SYMBOLDEF(0x01DA0940, "_Z17Legacy_Sys_PrintfPcz", Legacy_Sys_Printf),
	HOOK_SYMBOLDEF(0x01DA0B70, "_Z12Sys_InitArgvPc", Sys_InitArgv),
	HOOK_SYMBOLDEF(0x01DA0C20, "_Z14Sys_InitMemoryv", Sys_InitMemory),
	HOOK_SYMBOLDEF(0x01DA0D70, "_Z25Sys_InitLauncherInterfacev", Sys_InitLauncherInterface),
	HOOK_SYMBOLDEF(0x01DA0DA0, "_Z22Sys_InitAuthenticationv", Sys_InitAuthentication),
	HOOK_SYMBOLDEF(0x01DA0DC0, "_Z21Sys_ShowProgressTicksPc", Sys_ShowProgressTicks),
	HOOK_SYMBOLDEF(0x01DA0ED0, "_Z12Sys_InitGamePcS_Pvi", Sys_InitGame),
	HOOK_SYMBOLDEF(0x01DA0FE0, "_Z16Sys_ShutdownGamev", Sys_ShutdownGame),
	HOOK_SYMBOLDEF(0x01DA1060, "_Z13ClearIOStatesv", ClearIOStates),

	//HOOK_DEF(0x01DA0930, "_Z19Legacy_ErrorMessageiPKc", Legacy_ErrorMessage),							// NOXREF
	//HOOK_DEF(0x01DA09C0, "_Z11Sys_IsWin95v", Sys_IsWin95),											// NOXREF
	//HOOK_DEF(0x01DA09D0, "_Z11Sys_IsWin98v", Sys_IsWin98),											// NOXREF
	//HOOK_DEF(0x01DA0980, "_Z30Legacy_MP3subsys_Suspend_Audiov", Legacy_MP3subsys_Suspend_Audio),		// NOXREF
	//HOOK_DEF(0x01DA0990, "_Z29Legacy_MP3subsys_Resume_Audiov", Legacy_MP3subsys_Resume_Audio),		// NOXREF
	//HOOK_DEF(0x01DA13E0, "_Z22BuildMapCycleListHintsPPc", BuildMapCycleListHints),					// NOXREF

	//HOOK_DEF(0x, Sys_Init),							// NOXREF
	//HOOK_DEF(0x, Sys_Shutdown),						// NOXREF
	//HOOK_DEF(0x, Sys_ShutdownArgv),					// NOXREF
	//HOOK_DEF(0x, Sys_ShutdownLauncherInterface),		// NOXREF
	//HOOK_DEF(0x, Sys_ShutdownAuthentication),			// NOXREF
	//HOOK_DEF(0x01DA0760, SetRateRegistrySetting),		// NOXREF
	//HOOK_DEF(0x01DA0780, GetRateRegistrySetting),		// NOXREF

#endif // Sys_Dll2_region

#ifndef CModel_region

	HOOK_DEF(0x01D281B0, Mod_Init),
	HOOK_DEF(0x01D281D0, Mod_DecompressVis),
	HOOK_DEF(0x01D28210, Mod_LeafPVS),
	HOOK_DEF(0x01D28270, CM_DecompressPVS),
	HOOK_DEF(0x01D282E0, CM_LeafPVS),
	HOOK_DEF(0x01D28310, CM_LeafPAS),
	HOOK_DEF(0x01D28340, CM_FreePAS),
	HOOK_DEF(0x01D28380, CM_CalcPAS),
	HOOK_DEF(0x01D28580, CM_HeadnodeVisible),

#endif // CModel_region

#ifndef Model_region

	HOOK_DEF(0x01D50B80, SW_Mod_Init),
	HOOK_DEF(0x01D50B90, Mod_Extradata),
	HOOK_DEF(0x01D50BF0, Mod_PointInLeaf),
	HOOK_DEF(0x01D50C80, Mod_ClearAll),
	HOOK_DEF(0x01D50CC0, Mod_FillInCRCInfo),
	HOOK_DEF(0x01D50CE0, Mod_FindName),
	HOOK_DEF(0x01D50E90, Mod_LoadModel),
	HOOK_DEF(0x01D51110, Mod_ForName),
	HOOK_DEF(0x01D51150, Mod_AdInit),
	HOOK_DEF(0x01D511F0, Mod_AdSwap),
	HOOK_DEF(0x01D51280, Mod_LoadTextures),
	HOOK_DEF(0x01D517C0, Mod_LoadLighting),
	HOOK_DEF(0x01D51810, Mod_LoadVisibility),
	HOOK_DEF(0x01D51860, Mod_LoadEntities),
	HOOK_DEF(0x01D51930, Mod_LoadVertexes),
	HOOK_DEF(0x01D519E0, Mod_LoadSubmodels),
	HOOK_DEF(0x01D51B10, Mod_LoadEdges),
	HOOK_DEF(0x01D51BA0, Mod_LoadTexinfo),
	HOOK_DEF(0x01D51D60, CalcSurfaceExtents),
	HOOK_DEF(0x01D51F10, Mod_LoadFaces),
	HOOK_DEF(0x01D52140, Mod_SetParent),
	HOOK_DEF(0x01D52180, Mod_LoadNodes),
	HOOK_DEF(0x01D52330, Mod_LoadLeafs),
	HOOK_DEF(0x01D52490, Mod_LoadClipnodes),
	HOOK_DEF(0x01D52630, Mod_MakeHull0),
	HOOK_DEF(0x01D52720, Mod_LoadMarksurfaces),
	HOOK_DEF(0x01D527E0, Mod_LoadSurfedges),
	HOOK_DEF(0x01D52860, Mod_LoadPlanes),
	HOOK_DEF(0x01D52970, RadiusFromBounds),
	HOOK_DEF(0x01D52A00, Mod_LoadBrushModel),
	HOOK_DEF(0x01D53410, Mod_LoadSpriteFrame),
	HOOK_DEF(0x01D53510, Mod_LoadSpriteGroup),
	HOOK_DEF(0x01D535E0, Mod_LoadSpriteModel),
	HOOK_DEF(0x01D53820, Mod_Print),
	//HOOK_DEF(0x01D52C30, Mod_LoadAliasFrame),
	//HOOK_DEF(0x01D52CE0, Mod_LoadAliasGroup),
	//HOOK_DEF(0x01D52DF0, Mod_LoadAliasSkin),
	//HOOK_DEF(0x01D52E60, Mod_LoadAliasSkinGroup),
	//HOOK_DEF(0x01D52F40, Mod_LoadAliasModel),
	//HOOK_DEF(0x01D53800, Mod_UnloadSpriteTextures),
	//HOOK_DEF(0x01D538A0, Mod_ChangeGame),
	//HOOK_DEF(0x01D51100, Mod_MarkClient),		// NOXREF
	//HOOK_DEF(0x01D50DF0, Mod_ValidateCRC),		// NOXREF
	//HOOK_DEF(0x01D50E50, Mod_NeedCRC),			// NOXREF

#endif // Model_region

#ifndef Sv_Log_region

	HOOK_DEF(0x01D873D0, Log_Printf),
	HOOK_DEF(0x01D875A0, Log_PrintServerVars),
	HOOK_DEF(0x01D87600, Log_Close),
	HOOK_DEF(0x01D87630, Log_Open),
	HOOK_DEF(0x01D87850, SV_SetLogAddress_f),
	HOOK_DEF(0x01D87990, SV_AddLogAddress_f),
	HOOK_DEF(0x01D87BC0, SV_DelLogAddress_f),

#endif // Sv_Log_region

#ifndef Cl_Null_region

	//HOOK_DEF(0x, CL_RecordHUDCommand),
	//HOOK_DEF(0x, R_DecalRemoveAll),
	//HOOK_DEF(0x, CL_CheckForResend),
	//HOOK_DEF(0x, CL_CheckFile),
	//HOOK_DEF(0x01D17350, CL_ClearClientState),
	//HOOK_DEF(0x, CL_Connect_f),
	//HOOK_DEF(0x, CL_DecayLights),
	//HOOK_DEF(0x01D17490, CL_Disconnect),
	//HOOK_DEF(0x01D17660, CL_Disconnect_f),
	//HOOK_DEF(0x, CL_EmitEntities),
	//HOOK_DEF(0x, CL_InitClosest),
	//HOOK_DEF(0x, CL_Init),
	HOOK_DEF(0x01D131E0, CL_Particle),
	//HOOK_DEF(0x, CL_PredictMove),
	//HOOK_DEF(0x, CL_PrintLogos),
	//HOOK_DEF(0x, CL_ReadPackets),
	//HOOK_DEF(0x, CL_RequestMissingResources),
	//HOOK_DEF(0x, CL_Move),
	//HOOK_DEF(0x, CL_SendConnectPacket),
	//HOOK_DEF(0x01D0FABD, CL_StopPlayback),
	//HOOK_DEF(0x, CL_UpdateSoundFade),
	//HOOK_DEF(0x, CL_AdjustClock),
	//HOOK_DEF(0x01D48910, CL_Save),
	//HOOK_DEF(0x01D19650, CL_HudMessage),
	//HOOK_DEF(0x, Key_WriteBindings),
	//HOOK_DEF(0x, ClientDLL_UpdateClientData),
	//HOOK_DEF(0x, ClientDLL_HudVidInit),
	//HOOK_DEF(0x, Chase_Init),
	//HOOK_DEF(0x, Key_Init),
	//HOOK_DEF(0x, ClientDLL_Init),
	//HOOK_DEF(0x, Con_Shutdown),
	//HOOK_DEF(0x, DispatchDirectUserMsg),
	//HOOK_DEF(0x, CL_ShutDownUsrMessages),
	//HOOK_DEF(0x, CL_ShutDownClientStatic),
	//HOOK_DEF(0x, ClientDLL_MoveClient),
	//HOOK_DEF(0x, CL_Shutdown),
	//HOOK_DEF(0x, ClientDLL_Frame),
	//HOOK_DEF(0x, ClientDLL_CAM_Think),
	//HOOK_DEF(0x, CL_InitEventSystem),
	//HOOK_DEF(0x, CL_CheckClientState),
	//HOOK_DEF(0x, CL_RedoPrediction),
	//HOOK_DEF(0x, CL_SetLastUpdate),
	//HOOK_DEF(0x, Con_NPrintf),
	//HOOK_DEF(0x01D805A0, Sequence_OnLevelLoad),
	//HOOK_DEF(0x01D1CDD0, CL_WriteMessageHistory),
	//HOOK_DEF(0x, CL_MoveSpectatorCamera),
	//HOOK_DEF(0x, CL_AddVoiceToDatagram),
	//HOOK_DEF(0x, CL_VoiceIdle),
	//HOOK_DEF(0x, PollDInputDevices),
	//HOOK_DEF(0x, CL_KeepConnectionActive),
	//HOOK_DEF(0x, CL_UpdateModuleC),
	//HOOK_DEF(0x, VGuiWrap2_IsInCareerMatch),
	//HOOK_DEF(0x, VguiWrap2_GetCareerUI),
	//HOOK_DEF(0x, VGuiWrap2_GetLocalizedStringLength),
	//HOOK_DEF(0x01D07630, VGuiWrap2_LoadingStarted),
	//HOOK_DEF(0x, ConstructTutorMessageDecayBuffer),
	//HOOK_DEF(0x, ProcessTutorMessageDecayBuffer),
	//HOOK_DEF(0x, GetTimesTutorMessageShown),
	//HOOK_DEF(0x, RegisterTutorMessageShown),
	//HOOK_DEF(0x, ResetTutorMessageDecayData),
	//HOOK_DEF(0x01D83340, SetCareerAudioState),

#endif // Cl_Null_region

#ifndef Snd_Null_region

	//HOOK_DEF(0x0, S_Init),
	//HOOK_DEF(0x0, S_AmbientOff),
	//HOOK_DEF(0x0, S_AmbientOn),
	//HOOK_DEF(0x0, S_Shutdown),
	//HOOK_DEF(0x0, S_TouchSound),
	//HOOK_DEF(0x0, S_ClearBuffer),
	//HOOK_DEF(0x0, S_StartStaticSound),
	//HOOK_DEF(0x0, S_StartDynamicSound),
	//HOOK_DEF(0x0, S_StopSound),
	//HOOK_DEF(0x0, S_PrecacheSound),
	//HOOK_DEF(0x0, S_ClearPrecache),
	//HOOK_DEF(0x0, S_Update),
	//HOOK_DEF(0x0, S_StopAllSounds),
	//HOOK_DEF(0x0, S_BeginPrecaching),
	//HOOK_DEF(0x0, S_EndPrecaching),
	//HOOK_DEF(0x0, S_ExtraUpdate),
	//HOOK_DEF(0x0, S_LocalSound),
	//HOOK_DEF(0x0, S_BlockSound),
	//HOOK_DEF(0x0, S_PrintStats),
	//HOOK_DEF(0x0, Voice_RecordStart),
	//HOOK_DEF(0x0, Voice_IsRecording),
	//HOOK_DEF(0x0, Voice_RegisterCvars),
	//HOOK_DEF(0x01DB65D0, Voice_Deinit),
	//HOOK_DEF(0x0, Voice_Idle),
	//HOOK_DEF(0x0, Voice_RecordStop),

#endif // Snd_Null_region

#ifndef Sv_Steam3_region

	HOOK_SYMBOLDEF(0x01D994C0, "_ZN13CSteam3Server19NotifyClientConnectEP8client_sPKvj", CSteam3Server::NotifyClientConnect),
	HOOK_SYMBOLDEF(0x01D98B20, "_ZN13CSteam3Server14OnLogonFailureEP27SteamServerConnectFailure_t", CSteam3Server::OnLogonFailure),
	HOOK_SYMBOLDEF(0x01D99A10, "_ZN13CSteam3Server24SendUpdatedServerDetailsEv", CSteam3Server::SendUpdatedServerDetails),
	HOOK_SYMBOLDEF(0x01D98A70, "_ZN13CSteam3Server14OnLogonSuccessEP23SteamServersConnected_t", CSteam3Server::OnLogonSuccess),
	HOOK_SYMBOLDEF(0x01D98A40, "_ZN13CSteam3Server18OnGSPolicyResponseEP18GSPolicyResponse_t", CSteam3Server::OnGSPolicyResponse),
	HOOK_SYMBOLDEF(0x01D98AE0, "_ZN13CSteam3Server10GetSteamIDEv", CSteam3Server::GetSteamID),
	HOOK_SYMBOLDEF(0x01D98F10, "_ZN13CSteam3Server21ClientFindFromSteamIDER8CSteamID", CSteam3Server::ClientFindFromSteamID),
	HOOK_SYMBOLDEF(0x01D98DF0, "_ZN13CSteam3Server17OnGSClientApproveEP17GSClientApprove_t", CSteam3Server::OnGSClientApprove),
	HOOK_SYMBOLDEF(0x01D99660, "_ZN13CSteam3Server22NotifyClientDisconnectEP8client_s", CSteam3Server::NotifyClientDisconnect),
	HOOK_SYMBOLDEF(0x01D98BC0, "_ZN13CSteam3Server20OnGSClientDenyHelperEP8client_s11EDenyReasonPKc", CSteam3Server::OnGSClientDenyHelper),
	HOOK_SYMBOLDEF(0x01D98B90, "_ZN13CSteam3Server14OnGSClientDenyEP14GSClientDeny_t", CSteam3Server::OnGSClientDeny),
	HOOK_SYMBOLDEF(0x01D98DC0, "_ZN13CSteam3Server14OnGSClientKickEP14GSClientKick_t", CSteam3Server::OnGSClientKick),
	HOOK_SYMBOLDEF(0x01D996B0, "_ZN13CSteam3Server19NotifyOfLevelChangeEb", CSteam3Server::NotifyOfLevelChange),
	HOOK_SYMBOLDEF(0x01D99110, "_ZN13CSteam3Server8ActivateEv", CSteam3Server::Activate),
	HOOK_SYMBOLDEF(0x01D99770, "_ZN13CSteam3Server8RunFrameEv", CSteam3Server::RunFrame),
	HOOK_SYMBOLDEF(0x01D99600, "_ZN13CSteam3Server16NotifyBotConnectEP8client_s", CSteam3Server::NotifyBotConnect),

	HOOK_DEF(0x01D99550, ISteamGameServer_CreateUnauthenticatedUserConnection),
	HOOK_DEF(0x01D99590, ISteamGameServer_BUpdateUserData),
	HOOK_DEF(0x01D995D0, ISteamApps_BIsSubscribedApp),
	HOOK_DEF(0x01D99AB0, Steam_GetCommunityName),
	HOOK_DEF(0x01D99CA0, Steam_NotifyClientConnect),
	HOOK_DEF(0x01D99CD0, Steam_NotifyBotConnect),
	HOOK_DEF(0x01D99D00, Steam_NotifyClientDisconnect),
	HOOK_DEF(0x01D99D20, Steam_NotifyOfLevelChange),
	HOOK_DEF(0x01D99D40, Steam_Shutdown),
	HOOK_DEF(0x01D99D70, Steam_Activate),
	HOOK_DEF(0x01D99DC0, Steam_RunFrame),
	HOOK_DEF(0x01D99DE0, Steam_SetCVar),
	HOOK_DEF(0x01D99E10, Steam_ClientRunFrame),
	HOOK_DEF(0x01D99E20, Steam_InitClient),
	HOOK_DEF(0x01D99E30, Steam_GSInitiateGameConnection),
	HOOK_DEF(0x01D99E70, Steam_GSTerminateGameConnection),
	HOOK_DEF(0x01D99E90, Steam_ShutdownClient),
	HOOK_DEF(0x01D99EA0, Steam_GSGetSteamID),
	HOOK_DEF(0x01D99EB0, Steam_GSBSecure),
	HOOK_DEF(0x01D99ED0, Steam_GSBLoggedOn),
	HOOK_DEF(0x01D99F00, Steam_GSBSecurePreference),
	HOOK_DEF(0x01D99F10, Steam_Steam3IDtoSteam2),
	HOOK_DEF(0x01D99F50, Steam_StringToSteamID),
	HOOK_DEF(0x01D99FD0, Steam_GetGSUniverse),
	HOOK_DEF(0x01D9A020, Steam3Server),
	HOOK_DEF(0x01D9A1D0, Steam3Client),
	HOOK_DEF(0x01D9A1E0, Master_SetMaster_f),
	HOOK_DEF(0x01D9A2D0, Steam_HandleIncomingPacket),

#endif // Sv_Steam3_region

#ifndef Host_region

	HOOK_DEF(0x01D43A00, Host_EndGame),
	HOOK_DEF(0x01D43AC0, Host_Error),
	HOOK_DEF(0x01D43BA0, Host_InitLocal),
	HOOK_DEF(0x01D43C90, Info_WriteVars),				// NOXREF
	HOOK_DEF(0x01D43D70, Host_WriteConfiguration),		// NOXREF
	HOOK_DEF(0x01D43FA0, Host_WriteCustomConfig),
	HOOK_DEF(0x01D44190, SV_ClientPrintf),
	HOOK_DEF(0x01D441F0, SV_BroadcastPrintf),
	HOOK_DEF(0x01D442A0, Host_ClientCommands),
	HOOK_DEF(0x01D44310, SV_DropClient),
	HOOK_DEF(0x01D44510, Host_ClearClients),
	HOOK_DEF(0x01D446B0, Host_ShutdownServer),
	HOOK_DEF(0x01D447F0, SV_ClearClientStates),
	HOOK_DEF(0x01D44830, Host_CheckDyanmicStructures),
	HOOK_DEF(0x01D44880, Host_ClearMemory),
	HOOK_DEF(0x01D44900, Host_FilterTime),
	HOOK_DEF(0x01D44B50, Master_IsLanGame),
	HOOK_DEF(0x01D44B70, Master_Heartbeat_f),
	HOOK_DEF(0x01D44B80, Host_ComputeFPS),
	HOOK_DEF(0x01D44BB0, Host_GetHostInfo),
	HOOK_DEF(0x01D44C80, Host_Speeds),
	HOOK_DEF(0x01D44DC0, Host_UpdateScreen),
	HOOK_DEF(0x01D44EB0, Host_UpdateSounds),
	HOOK_DEF(0x01D44F30, Host_CheckConnectionFailure),
	HOOK_DEF(0x01D44F90, _Host_Frame),
	HOOK_DEF(0x01D45150, Host_Frame),
	HOOK_DEF(0x01D45290, CheckGore),
	HOOK_DEF(0x01D45450, Host_IsSinglePlayerGame),
	HOOK_DEF(0x01D45470, Host_IsServerActive),
	HOOK_DEF(0x01D45480, Host_Version),
	HOOK_DEF(0x01D456A0, Host_Init),
	HOOK_DEF(0x01D459A0, Host_Shutdown),

#endif // Host_region

#ifndef Host_Cmd_region

	HOOK_DEF(0x01D45AF0, SV_GetPlayerHulls),
	HOOK_DEF(0x01D45B20, Host_InitializeGameDLL),
	HOOK_DEF(0x01D45BA0, Host_Motd_f),
	HOOK_DEF(0x01D45D00, Host_Motd_Write_f),
	HOOK_DEF(0x01D45E70, Host_GetStartTime),
	HOOK_DEF(0x01D45E80, Host_UpdateStats),				// UNTESTED Linux
	HOOK_DEF(0x01D45FB0, GetStatsString),
	HOOK_DEF(0x01D460B0, Host_Stats_f),
	HOOK_DEF(0x01D460F0, Host_Quit_f),
	HOOK_DEF(0x01D46140, Host_Quit_Restart_f),
	HOOK_DEF(0x01D46250, Host_Status_Printf),
	HOOK_DEF(0x01D462F0, Host_Status_f),
	HOOK_DEF(0x01D467B0, Host_Status_Formatted_f),
	HOOK_DEF(0x01D46EC0, Host_Ping_f),
	HOOK_DEF(0x01D46F30, Host_Map),
	HOOK_DEF(0x01D470D0, Host_Map_f),
	HOOK_DEF(0x01D473B0, Host_Career_f),
	HOOK_DEF(0x01D473D0, Host_Maps_f),
	HOOK_DEF(0x01D47410, Host_Changelevel_f),
	HOOK_DEF(0x01D47510, Host_FindRecentSave),
	HOOK_DEF(0x01D47600, Host_Restart_f),
	HOOK_DEF(0x01D47690, Host_Reload_f),
	HOOK_DEF(0x01D47710, Host_Reconnect_f),
	HOOK_DEF(0x01D477E0, Host_SaveGameDirectory),
	HOOK_DEF(0x01D47810, Host_SavegameComment),
	HOOK_DEF(0x01D478B0, Host_SaveAgeList),
	HOOK_DEF(0x01D479B0, Host_ValidSave),
	HOOK_DEF(0x01D47A70, SaveInit),
	HOOK_DEF(0x01D47B80, SaveExit),
	HOOK_DEF(0x01D47BC0, SaveGameSlot),
	HOOK_DEF(0x01D47E70, Host_Savegame_f),
	HOOK_DEF(0x01D47F60, Host_AutoSave_f),
	HOOK_DEF(0x01D47FF0, SaveGame),
	HOOK_DEF(0x01D48020, SaveReadHeader),
	HOOK_DEF(0x01D481A0, SaveReadComment),				// NOXREF
	HOOK_DEF(0x01D481D0, Host_Loadgame_f),
	HOOK_DEF(0x01D48220, LoadGame),
	HOOK_DEF(0x01D48250, Host_Load),
	HOOK_DEF(0x01D484C0, SaveGamestate),
	HOOK_DEF(0x01D48A00, EntityInit),
	HOOK_DEF(0x01D48A50, LoadSaveData),
	HOOK_DEF(0x01D48C70, ParseSaveTables),
	HOOK_DEF(0x01D48DC0, EntityPatchWrite),
	HOOK_DEF(0x01D48EB0, EntityPatchRead),
	HOOK_DEF(0x01D48F60, LoadGamestate),
	HOOK_DEF(0x01D491D0, EntryInTable),
	HOOK_DEF(0x01D49220, LandmarkOrigin),
	HOOK_DEF(0x01D49290, EntityInSolid),
	HOOK_DEF(0x01D49320, CreateEntityList),
	HOOK_DEF(0x01D49570, LoadAdjacentEntities),
	HOOK_DEF(0x01D49730, FileSize),
	HOOK_DEF(0x01D49750, FileCopy),
	HOOK_DEF(0x01D497B0, DirectoryCopy),
	HOOK_DEF(0x01D49880, DirectoryExtract),
	HOOK_DEF(0x01D49930, DirectoryCount),
	HOOK_DEF(0x01D49970, Host_ClearSaveDirectory),
	HOOK_DEF(0x01D49AB0, Host_ClearGameState),
	HOOK_DEF(0x01D49AD0, Host_Changelevel2_f),
	HOOK_DEF(0x01D49CB0, Host_Version_f),
	HOOK_DEF(0x01D49CE0, Host_FullInfo_f),
	HOOK_DEF(0x01D49E00, Host_KillVoice_f),				// NOXREF
	HOOK_DEF(0x01D49E10, Host_SetInfo_f),
	HOOK_DEF(0x01D49EB0, Host_Say),
	HOOK_DEF(0x01D4A060, Host_Say_f),
	HOOK_DEF(0x01D4A070, Host_Say_Team_f),
	HOOK_DEF(0x01D4A080, Host_Tell_f),
	HOOK_DEF(0x01D4A230, Host_Kill_f),
	HOOK_DEF(0x01D4A280, Host_TogglePause_f),
	HOOK_DEF(0x01D4A320, Host_Pause_f),
	HOOK_DEF(0x01D4A380, Host_Unpause_f),
	HOOK_DEF(0x01D4A620, Host_Interp_f),
	HOOK_DEF(0x01D4A650, Host_NextDemo),
	HOOK_DEF(0x01D4A700, Host_Startdemos_f),
	HOOK_DEF(0x01D4A7C0, Host_Demos_f),
	HOOK_DEF(0x01D4A7F0, Host_Stopdemo_f),
	HOOK_DEF(0x01D4A810, Host_EndSection),				// NOXREF
	HOOK_DEF(0x01D4A910, Host_Soundfade_f),
	HOOK_DEF(0x01D4AA10, Host_KillServer_f),
	HOOK_DEF(0x01D4AA50, Host_VoiceRecordStart_f),
	HOOK_DEF(0x01D4AAA0, Host_VoiceRecordStop_f),
	HOOK_DEF(0x01D4AC90, Host_Crash_f),					// NOXREF
	HOOK_DEF(0x01D4ACA0, Host_InitCommands),
	HOOK_DEF(0x01D4B060, SV_CheckBlendingInterface),
	HOOK_DEF(0x01D4B0D0, SV_CheckSaveGameCommentInterface),

#endif // Host_Cmd_region

#ifndef Pmove_region

	HOOK_DEF(0x01D5B490, PM_AddToTouched),
	HOOK_DEF(0x01D5B530, PM_StuckTouch),
	HOOK_DEF(0x01D5B590, PM_Init),

#endif // Pmove_region

#ifndef Pmovetst_region

	HOOK_DEF(0x01D5B8F0, PM_TraceModel),
	HOOK_DEF(0x01D5B990, PM_GetModelBounds),
	HOOK_DEF(0x01D5B9C0, PM_GetModelType),
	HOOK_DEF(0x01D5B9D0, PM_InitBoxHull),
	HOOK_DEF(0x01D5BA60, PM_HullForBox),
	HOOK_DEF(0x01D5BAB0, PM_HullPointContents),
	HOOK_DEF(0x01D5BB50, PM_LinkContents),
	HOOK_DEF(0x01D5BC10, PM_PointContents),
	HOOK_DEF(0x01D5BCA0, PM_WaterEntity),
	HOOK_DEF(0x01D5BD10, PM_TruePointContents),
	HOOK_DEF(0x01D5BD40, PM_HullForStudioModel),
	HOOK_DEF(0x01D5BDF0, PM_HullForBsp),
	HOOK_DEF(0x01D5BEA0, _PM_TestPlayerPosition),
	HOOK_DEF(0x01D5C190, PM_TestPlayerPosition),
	HOOK_DEF(0x01D5C1B0, PM_TestPlayerPositionEx),
	HOOK_DEF(0x01D5C1D0, _PM_PlayerTrace),
	HOOK_DEF(0x01D5C890, PM_PlayerTrace),
	HOOK_DEF(0x01D5C8F0, PM_PlayerTraceEx),
	HOOK_DEF(0x01D5C9C8, PM_TraceLine),
	HOOK_DEF(0x01D5CA60, PM_TraceLineEx),
	HOOK_DEF(0x01D5CB20, PM_RecursiveHullCheck),

#endif // Pmovetst_region

#ifndef Pr_Edict_region

	HOOK_DEF(0x01D61510, ED_ClearEdict),
	HOOK_DEF(0x01D61550, ED_Alloc),
	HOOK_DEF(0x01D61610, ED_Free),
	HOOK_DEF(0x01D616D0, ED_Count),				// NOXREF
	HOOK_DEF(0x01D61770, ED_NewString),
	HOOK_DEF(0x01D617D0, ED_ParseEdict),
	HOOK_DEF(0x01D61A70, ED_LoadFromFile),
	HOOK_DEF(0x01D61B80, PR_Init),				// NOXREF
	HOOK_DEF(0x01D61B90, EDICT_NUM),
	HOOK_DEF(0x01D61BD0, NUM_FOR_EDICT),
	HOOK_DEF(0x01D61C10, SuckOutClassname),
	HOOK_DEF(0x01D61CD0, ReleaseEntityDLLFields),
	HOOK_DEF(0x01D61CF0, InitEntityDLLFields),
	HOOK_DEF(0x01D61D00, PvAllocEntPrivateData),
	HOOK_DEF(0x01D61D30, PvEntPrivateData),
	HOOK_DEF(0x01D61D50, FreeEntPrivateData),
	HOOK_DEF(0x01D61D90, FreeAllEntPrivateData),
	HOOK_DEF(0x01D61DD0, PEntityOfEntOffset),
	HOOK_DEF(0x01D61DE0, EntOffsetOfPEntity),
	HOOK_DEF(0x01D61DF0, IndexOfEdict),
	HOOK_DEF(0x01D61E40, PEntityOfEntIndex),
	HOOK_DEF(0x01D61E80, SzFromIndex),
	HOOK_DEF(0x01D61E90, GetVarsOfEnt),
	HOOK_DEF(0x01D61EA0, FindEntityByVars),
	HOOK_DEF(0x01D61EE0, CVarGetFloat),
	HOOK_DEF(0x01D61F00, CVarGetString),
	HOOK_DEF(0x01D61F20, CVarGetPointer),
	HOOK_DEF(0x01D61F40, CVarSetFloat),
	HOOK_DEF(0x01D61F60, CVarSetString),
	HOOK_DEF(0x01D61F80, CVarRegister),
	HOOK_DEF(0x01D61FA0, AllocEngineString),
	HOOK_DEF(0x01D61FC0, SaveSpawnParms),
	HOOK_DEF(0x01D61FF0, GetModelPtr),

#endif // Pr_Edict_region

#ifndef Pr_Cmds_region

	//HOOK_DEF(0x01D5CF00, PF_makevectors_I),
	//HOOK_DEF(0x01D5CF20, PF_Time),
	//HOOK_DEF(0x01D5CF40, PF_setorigin_I),
	//HOOK_DEF(0x01D5CF80, SetMinMaxSize),
	//HOOK_DEF(0x01D5D030, PF_setsize_I),
	//HOOK_DEF(0x01D5D050, PF_setmodel_I),
	//HOOK_DEF(0x01D5D0F0, PF_modelindex),
	//HOOK_DEF(0x01D5D110, ModelFrames),
	//HOOK_DEF(0x01D5D150, PF_bprint),
	//HOOK_DEF(0x01D5D170, PF_sprint),
	//HOOK_DEF(0x01D5D1D0, ServerPrint),
	//HOOK_DEF(0x01D5D1F0, ClientPrintf),
	//HOOK_DEF(0x01D5D2A0, PF_vectoyaw_I),
	//HOOK_DEF(0x01D5D330, PF_vectoangles_I),
	//HOOK_DEF(0x01D5D350, PF_particle_I),
	//HOOK_DEF(0x01D5D380, PF_ambientsound_I),
	//HOOK_DEF(0x01D5D4C0, PF_sound_I),
	//HOOK_DEF(0x01D5D5A0, PF_traceline_Shared),
	//HOOK_DEF(0x01D5D600, PF_traceline_DLL),
	//HOOK_DEF(0x01D5D6C0, TraceHull),
	//HOOK_DEF(0x01D5D770, TraceSphere),
	//HOOK_DEF(0x01D5D780, TraceModel),
	//HOOK_DEF(0x01D5D890, SurfaceAtPoint),
	//HOOK_DEF(0x01D5DA90, TraceTexture),
	//HOOK_DEF(0x01D5DCA0, PF_TraceToss_Shared),
	//HOOK_DEF(0x01D5DCE0, SV_SetGlobalTrace),
	//HOOK_DEF(0x01D5DD80, PF_TraceToss_DLL),
	//HOOK_DEF(0x01D5DE40, TraceMonsterHull),
	//HOOK_DEF(0x01D5DF10, PF_newcheckclient),
	//HOOK_DEF(0x01D5DFF0, PF_checkclient_I),
	//HOOK_DEF(0x01D5E110, PVSNode),
	//HOOK_DEF(0x01D5E1D0, PVSMark),
	//HOOK_DEF(0x01D5E250, PVSFindEntities),
	//HOOK_DEF(0x01D5E360, ValidCmd),
	//HOOK_DEF(0x01D5E390, PF_stuffcmd_I),
	//HOOK_DEF(0x01D5E450, PF_localcmd_I),
	//HOOK_DEF(0x01D5E480, PF_localexec_I),
	//HOOK_DEF(0x01D5E490, FindEntityInSphere),
	//HOOK_DEF(0x01D5E5C0, PF_Spawn_I),
	//HOOK_DEF(0x01D5E5D0, CreateNamedEntity),
	//HOOK_DEF(0x01D5E640, PF_Remove_I),
	//HOOK_DEF(0x01D5E920, PF_find_Shared),
	//HOOK_DEF(0x01D5E660, iGetIndex),
	//HOOK_DEF(0x01D5E8C0, FindEntityByString),
	//HOOK_DEF(0x01D5E9B0, GetEntityIllum),
	//HOOK_DEF(0x01D5EA50, PR_IsEmptyString),
	//HOOK_DEF(0x01D5EA60, PF_precache_sound_I),
	//HOOK_DEF(0x01D5EB50, EV_Precache),
	//HOOK_DEF(0x01D5ECD0, EV_PlayReliableEvent),
	//HOOK_DEF(0x01D5EE10, EV_Playback),
	//HOOK_DEF(0x01D5F190, EV_SV_Playback),
	//HOOK_DEF(0x01D5F210, PF_precache_model_I),
	//HOOK_DEF(0x01D5F310, PF_precache_generic_I),
	//HOOK_DEF(0x01D5F3E0, PF_IsMapValid_I),
	//HOOK_DEF(0x01D5F430, PF_NumberOfEntities_I),
	//HOOK_DEF(0x01D5F460, PF_GetInfoKeyBuffer_I),
	//HOOK_DEF(0x01D5F4B0, PF_InfoKeyValue_I),
	//HOOK_DEF(0x01D5F4D0, PF_SetKeyValue_I),
	//HOOK_DEF(0x01D5F530, PF_RemoveKey_I),
	//HOOK_DEF(0x01D5F550, PF_SetClientKeyValue_I),
	//HOOK_DEF(0x01D5F5E0, PF_walkmove_I),
	//HOOK_DEF(0x01D5F690, PF_droptofloor_I),
	//HOOK_DEF(0x01D5F780, PF_DecalIndex),
	//HOOK_DEF(0x01D5F7D0, PF_lightstyle_I),
	//HOOK_DEF(0x01D5F860, PF_checkbottom_I),
	//HOOK_DEF(0x01D5F880, PF_pointcontents_I),
	//HOOK_DEF(0x01D5F8A0, PF_aim_I),
	//HOOK_DEF(0x01D5FB60, PF_changeyaw_I),
	//HOOK_DEF(0x01D5FC50, PF_changepitch_I),
	//HOOK_DEF(0x01D5FD40, PF_setview_I),
	//HOOK_DEF(0x01D5FDC0, PF_crosshairangle_I),
	//HOOK_DEF(0x01D5FEC0, PF_CreateFakeClient_I),
	//HOOK_DEF(0x01D60070, PF_RunPlayerMove_I),
	//HOOK_DEF(0x01D60180, WriteDest_Parm),
	//HOOK_DEF(0x01D60260, PF_MessageBegin_I),
	//HOOK_DEF(0x01D60350, PF_MessageEnd_I),
	//HOOK_DEF(0x01D605E0, PF_WriteByte_I),
	//HOOK_DEF(0x01D60610, PF_WriteChar_I),
	//HOOK_DEF(0x01D60640, PF_WriteShort_I),
	//HOOK_DEF(0x01D60670, PF_WriteLong_I),
	//HOOK_DEF(0x01D606A0, PF_WriteAngle_I),
	//HOOK_DEF(0x01D606D0, PF_WriteCoord_I),
	//HOOK_DEF(0x01D60710, PF_WriteString_I),
	//HOOK_DEF(0x01D60740, PF_WriteEntity_I),
	//HOOK_DEF(0x01D60770, PF_makestatic_I),
	//HOOK_DEF(0x01D608C0, PF_StaticDecal),
	//HOOK_DEF(0x01D60940, PF_setspawnparms_I),
	//HOOK_DEF(0x01D60970, PF_changelevel_I),
	HOOK_DEF(0x01D609D0, SeedRandomNumberGenerator),
	HOOK_DEF(0x01D60A10, ran1),
	HOOK_DEF(0x01D60AE0, fran1),
	HOOK_DEF(0x01D60B30, RandomFloat),
	HOOK_DEF(0x01D60B60, RandomLong),
	//HOOK_DEF(0x01D60BC0, PF_FadeVolume),
	//HOOK_DEF(0x01D60C60, PF_SetClientMaxspeed),
	//HOOK_DEF(0x01D60CA0, PF_GetPlayerUserId),
	//HOOK_DEF(0x01D60CF0, PF_GetPlayerWONId),
	HOOK_DEF(0x01D60D00, PF_GetPlayerAuthId),
	//HOOK_DEF(0x01D60E00, PF_BuildSoundMsg_I),
	//HOOK_DEF(0x01D60E50, PF_IsDedicatedServer),
	//HOOK_DEF(0x01D60E60, PF_GetPhysicsInfoString),
	//HOOK_DEF(0x01D60EB0, PF_GetPhysicsKeyValue),
	//HOOK_DEF(0x01D60F10, PF_SetPhysicsKeyValue),
	//HOOK_DEF(0x01D60F70, PF_GetCurrentPlayer),
	//HOOK_DEF(0x01D60FB0, PF_CanSkipPlayer),
	//HOOK_DEF(0x01D61000, PF_SetGroupMask),
	//HOOK_DEF(0x01D61020, PF_CreateInstancedBaseline),
	//HOOK_DEF(0x01D61070, PF_Cvar_DirectSet),
	//HOOK_DEF(0x01D61090, PF_ForceUnmodified),
	//HOOK_DEF(0x01D611B0, PF_GetPlayerStats),
	//HOOK_DEF(0x01D61230, QueryClientCvarValueCmd), 	// NOXREF
	//HOOK_DEF(0x01D612D0, QueryClientCvarValueCmd2), 	// NOXREF
	//HOOK_DEF(0x01D61390, QueryClientCvarValue),
	//HOOK_DEF(0x01D61410, QueryClientCvarValue2),
	//HOOK_DEF(0x01D614A0, hudCheckParm),
	//HOOK_DEF(0x01D614F0, EngCheckParm),

#endif // Pr_Cmds_region

#ifndef Mathlib_region

	HOOK_DEF(0x01D4F570, anglemod),
	//HOOK_DEF(0x, BOPS_Error),
	HOOK_DEF(0x01DBEE44, BoxOnPlaneSide),
	//HOOK_DEF(0x, InvertMatrix),
	HOOK_DEF(0x01D4FD10, AngleVectors),
	HOOK_DEF(0x01D4FE70, AngleVectorsTranspose),
	HOOK_DEF(0x01D4FF90, AngleMatrix),
	//HOOK_DEF(0x, AngleIMatrix),
	//HOOK_DEF(0x, NormalizeAngles),
	//HOOK_DEF(0x, InterpolateAngles),
	HOOK_DEF(0x01D502C0, VectorTransform),
	HOOK_DEF(0x01D50320, VectorCompare),
	HOOK_DEF(0x01D50350, VectorMA),
	//HOOK_DEF(0x, _DotProduct),
	//HOOK_DEF(0x, _VectorSubtract),
	//HOOK_DEF(0x, _VectorAdd),
	//HOOK_DEF(0x, _VectorCopy),
	HOOK_DEF(0x01D50420, CrossProduct),
	HOOK_DEF(0x01D50460, Length),
	HOOK_DEF(0x01D504A0, VectorNormalize),
	//HOOK_DEF(0x, VectorInverse),
	HOOK_DEF(0x01D50550, VectorScale),
	//HOOK_DEF(0x, Q_log2),
	//HOOK_DEF(0x, VectorMatrix),
	HOOK_DEF(0x01D50640, VectorAngles),
	//HOOK_DEF(0x, R_ConcatRotations),
	HOOK_DEF(0x01D50850, R_ConcatTransforms),
	//HOOK_DEF(0x, FloorDivMod),
	//HOOK_DEF(0x, GreatestCommonDivisor),
	//HOOK_DEF(0x, Invert24To16),

#endif // Mathlib_region

#ifndef World_region

	HOOK_DEF(0x01DB9050, ClearLink),
	HOOK_DEF(0x01DB9060, RemoveLink),
	HOOK_DEF(0x01DB9080, InsertLinkBefore),
	//HOOK_DEF(0x01DB90A0, InsertLinkAfter),			// NOXREF
	HOOK_DEF(0x01DB90C0, SV_InitBoxHull),
	HOOK_DEF(0x01DB9180, SV_HullForBox),
	//HOOK_DEF(0x01DB91D0, SV_HullForBeam),				// NOXREF
	HOOK_DEF(0x01DB9640, SV_HullForBsp),
	HOOK_DEF(0x01DB9780, SV_HullForEntity),
	HOOK_DEF(0x01DB9850, SV_CreateAreaNode),
	HOOK_DEF(0x01DB9970, SV_ClearWorld),
	HOOK_DEF(0x01DB99B0, SV_UnlinkEdict),
	HOOK_DEF(0x01DB99E0, SV_TouchLinks),
	HOOK_DEF(0x01DB9BC0, SV_FindTouchedLeafs),
	HOOK_DEF(0x01DB9CF0, SV_LinkEdict),
	HOOK_DEF(0x01DC0614, SV_HullPointContents),
	HOOK_DEF(0x01DB9EB0, SV_LinkContents),
	HOOK_DEF(0x01DBA0A0, SV_PointContents),
	HOOK_DEF(0x01DBA0F0, SV_TestEntityPosition),
	HOOK_DEF(0x01DBA160, SV_RecursiveHullCheck),
	HOOK_DEF(0x01DBA550, SV_SingleClipMoveToEntity),
	HOOK_DEF(0x01DBA950, SV_ClipMoveToEntity),
	HOOK_DEF(0x01DBA990, SV_ClipToLinks),
	HOOK_DEF(0x01DBAC60, SV_ClipToWorldbrush),
	HOOK_DEF(0x01DBAE00, SV_MoveBounds),
	HOOK_DEF(0x01DBAE80, SV_MoveNoEnts),
	HOOK_DEF(0x01DBAFC0, SV_Move),

#endif // World_region

#ifndef Sv_Phys_region

	//HOOK_DEF(0x01D94A90, SV_CheckAllEnts),			// NOXREF
	HOOK_DEF(0x01D94B00, SV_CheckVelocity),
	HOOK_DEF(0x01D94C00, SV_RunThink),
	HOOK_DEF(0x01D94CB0, SV_Impact),
	HOOK_DEF(0x01D94D50, ClipVelocity),
	HOOK_DEF(0x01D94E10, SV_FlyMove),
	HOOK_DEF(0x01D952F0, SV_AddGravity),
	//HOOK_DEF(0x01D95370, SV_AddCorrectGravity),		// NOXREF
	//HOOK_DEF(0x01D953F0, SV_FixupGravityVelocity),	// NOXREF
	HOOK_DEF(0x01D95450, SV_PushEntity),
	HOOK_DEF(0x01D95550, SV_PushMove),
	HOOK_DEF(0x01D958F0, SV_PushRotate),
	HOOK_DEF(0x01D95F00, SV_Physics_Pusher),
	HOOK_DEF(0x01D960F0, SV_CheckWater),

	HOOK_DEF(0x01D96290, SV_RecursiveWaterLevel),
	HOOK_DEF(0x01D96310, SV_Submerged),
	HOOK_DEF(0x01D96410, SV_Physics_None),
	HOOK_DEF(0x01D96430, SV_Physics_Follow),
	HOOK_DEF(0x01D964F0, SV_Physics_Noclip),

	HOOK_DEF(0x01D96560, SV_CheckWaterTransition),
	HOOK_DEF(0x01D96740, SV_Physics_Toss),
	HOOK_DEF(0x01D96B20, PF_WaterMove),

	HOOK_DEF(0x01D96E70, SV_Physics_Step),
	HOOK_DEF(0x01D97240, SV_Physics),
	HOOK_DEF(0x01D97480, SV_Trace_Toss),

#endif // Sv_Phys_region

#ifndef Sv_Move_region

	HOOK_DEF(0x01D93A40, SV_CheckBottom),
	HOOK_DEF(0x01D93CB0, SV_movetest),
	HOOK_DEF(0x01D93ED0, SV_movestep),
	HOOK_DEF(0x01D94250, SV_StepDirection),
	HOOK_DEF(0x01D942D0, SV_FlyDirection),
	HOOK_DEF(0x01D94310, SV_FixCheckBottom),
	//HOOK_DEF(0x01D94330, SV_NewChaseDir), // NOXREF
	//HOOK_DEF(0x01D94620, SV_CloseEnough), // NOXREF
	//HOOK_DEF(0x01D94680, SV_ReachedGoal), // NOXREF
	HOOK_DEF(0x01D946D0, SV_NewChaseDir2),
	HOOK_DEF(0x01D949C0, SV_MoveToOrigin_I),

#endif // Sv_Move_region

#ifndef Sv_pmove_region

	HOOK_DEF(0x01D975F0, PM_SV_PlaybackEventFull),
	HOOK_DEF(0x01D97630, PM_SV_PlaySound),
	HOOK_DEF(0x01D97670, PM_SV_TraceTexture),

#endif // Sv_pmove_region

#ifndef R_Studio_region

	HOOK_DEF(0x01D6F260, SV_InitStudioHull),
	HOOK_DEF(0x01D6F2F0, R_CheckStudioCache),
	//HOOK_DEF(0x01D6F3C0, R_AddToStudioCache),			// NOXREF
	HOOK_DEF(0x01D6F500, AngleQuaternion),
	HOOK_DEF(0x01D6F600, QuaternionSlerp),
	HOOK_DEF(0x01D6F810, QuaternionMatrix),
	HOOK_DEF(0x01D6F900, R_StudioCalcBoneAdj),
	HOOK_DEF(0x01D6FB60, R_StudioCalcBoneQuaterion),
	HOOK_DEF(0x01D6FD20, R_StudioCalcBonePosition),
	HOOK_DEF(0x01D6FE70, R_StudioSlerpBones),
	HOOK_DEF(0x01D6FF80, R_GetAnim),
	HOOK_DEF(0x01D70020, SV_StudioSetupBones),
	HOOK_DEF(0x01D70390, SV_SetStudioHullPlane),
	HOOK_DEF(0x01D70400, R_StudioHull),
	HOOK_DEF(0x01D70A40, SV_HitgroupForStudioHull),
	//HOOK_DEF(0x01D70A50, R_InitStudioCache),			// NOXREF
	//HOOK_DEF(0x01D70A90, R_FlushStudioCache),			// NOXREF
	HOOK_DEF(0x01D70AA0, R_StudioBodyVariations),
	HOOK_DEF(0x01D70AF0, R_StudioPlayerBlend),
	HOOK_DEF(0x01D70BD0, SV_HullForStudioModel),
	HOOK_DEF(0x01D70E10, DoesSphereIntersect),
	HOOK_DEF(0x01D70ED0, SV_CheckSphereIntersection),
	HOOK_DEF(0x01D70FF0, AnimationAutomove),
	HOOK_DEF(0x01D71000, GetBonePosition),
	HOOK_DEF(0x01D710A0, GetAttachment),
	HOOK_DEF(0x01D71170, ModelFrameCount),
	HOOK_DEF(0x01D76260, R_StudioBoundVertex),
	HOOK_DEF(0x01D762F0, R_StudioBoundBone),
	HOOK_DEF(0x01D76380, R_StudioAccumulateBoneVerts),
	HOOK_DEF(0x01D76420, R_StudioComputeBounds),
	HOOK_DEF(0x01D76600, R_GetStudioBounds),
	HOOK_DEF(0x01D76C40, R_ResetSvBlending),

#endif // R_Studio_region

#ifndef Net_ws_region

	HOOK_DEF(0x01D57050, NET_ThreadLock),
	HOOK_DEF(0x01D57070, NET_ThreadUnlock),
	HOOK_DEF(0x01D57090, Q_ntohs),
	HOOK_DEF(0x01D570A0, NetadrToSockadr),
	HOOK_DEF(0x01D57160, SockadrToNetadr),
	HOOK_DEF(0x01D571D0, NET_HostToNetShort),	// NOXREF
	HOOK_DEF(0x01D571E0, NET_CompareAdr),
	HOOK_DEF(0x01D57270, NET_CompareClassBAdr),
	HOOK_DEF(0x01D572C0, NET_IsReservedAdr),
	HOOK_DEF(0x01D57310, NET_CompareBaseAdr),
	HOOK_DEF(0x01D57380, NET_AdrToString),
	HOOK_DEF(0x01D574A0, NET_BaseAdrToString),
	HOOK_DEF(0x01D575A0, NET_StringToSockaddr),
	HOOK_DEF(0x01D57820, NET_StringToAdr),
	HOOK_DEF(0x01D57890, NET_IsLocalAddress),
	HOOK_DEF(0x01D578A0, NET_ErrorString),
	HOOK_DEF(0x01D57B20, NET_TransferRawData),
	HOOK_DEF(0x01D57B50, NET_GetLoopPacket),
	HOOK_DEF(0x01D57C00, NET_SendLoopPacket),
	HOOK_DEF(0x01D57C80, NET_RemoveFromPacketList),
	HOOK_DEF(0x01D57CA0, NET_CountLaggedList),
	HOOK_DEF(0x01D57CC0, NET_ClearLaggedList),
	HOOK_DEF(0x01D57D10, NET_AddToLagged),
	HOOK_DEF(0x01D57D80, NET_AdjustLag),
	HOOK_DEF(0x01D57EC0, NET_LagPacket),
	HOOK_DEF(0x01D58090, NET_FlushSocket),
	HOOK_DEF(0x01D580E0, NET_GetLong),
	HOOK_DEF(0x01D582F0, NET_QueuePacket),

#ifdef __linux__
	//HOOK_DEF(0x0, NET_Sleep_Timeout),
#endif // __linux__

	HOOK_DEF(0x01D584A0, NET_Sleep),
	HOOK_DEF(0x01D58630, NET_StartThread),
	HOOK_DEF(0x01D586B0, NET_StopThread),
	HOOK_DEF(0x01D586F0, net_malloc),
	HOOK_DEF(0x01D58710, NET_AllocMsg),
	HOOK_DEF(0x01D58760, NET_FreeMsg),
	HOOK_DEF(0x01D587A0, NET_GetPacket),
	HOOK_DEF(0x01D588B0, NET_AllocateQueues),
	HOOK_DEF(0x01D588F0, NET_FlushQueues),
	HOOK_DEF(0x01D58960, NET_SendLong),
	HOOK_DEF(0x01D58AE0, NET_SendPacket),
	HOOK_DEF(0x01D58C80, NET_IPSocket),
	HOOK_DEF(0x01D58E80, NET_OpenIP),
#ifdef _WIN32
	HOOK_DEF(0x01D59000, NET_IPXSocket),
	HOOK_DEF(0x01D59170, NET_OpenIPX),
#endif //_WIN32
	HOOK_DEF(0x01D59240, NET_GetLocalAddress),
	HOOK_DEF(0x01D59410, NET_IsConfigured),
	HOOK_DEF(0x01D59420, NET_Config),
	HOOK_DEF(0x01D596F0, MaxPlayers_f),
	HOOK_DEF(0x01D594D0, NET_Init),
	HOOK_DEF(0x01D59790, NET_ClearLagData),
	HOOK_DEF(0x01D597E0, NET_Shutdown),
	HOOK_DEF(0x01D59810, NET_JoinGroup),
	HOOK_DEF(0x01D59870, NET_LeaveGroup),

#endif // Net_ws_region

#ifndef Net_chan_region

	HOOK_DEF(0x01D54DB0, Netchan_UnlinkFragment),
	HOOK_DEF(0x01D54E20, Netchan_OutOfBand),
	HOOK_DEF(0x01D54EA0, Netchan_OutOfBandPrint),
	HOOK_DEF(0x01D54F00, Netchan_ClearFragbufs),
	HOOK_DEF(0x01D54F30, Netchan_ClearFragments),
	HOOK_DEF(0x01D54FA0, Netchan_Clear),
	HOOK_DEF(0x01D55040, Netchan_Setup),
	HOOK_DEF(0x01D550D0, Netchan_CanPacket),
	HOOK_DEF(0x01D55130, Netchan_UpdateFlow),
	HOOK_DEF(0x01D55240, Netchan_Transmit),
	HOOK_DEF(0x01D558E0, Netchan_FindBufferById),
	HOOK_DEF(0x01D55950, Netchan_CheckForCompletion),
	HOOK_DEF(0x01D55A00, Netchan_Validate),
	HOOK_DEF(0x01D55A90, Netchan_Process),
	HOOK_DEF(0x01D55F40, Netchan_FragSend),
	HOOK_DEF(0x01D55F90, Netchan_AddBufferToList),
	HOOK_DEF(0x01D55FE0, Netchan_AllocFragbuf),
	HOOK_DEF(0x01D56010, Netchan_AddFragbufToTail),
	HOOK_DEF(0x01D56050, Netchan_CreateFragments_),
	HOOK_DEF(0x01D56210, Netchan_CreateFragments),
	HOOK_DEF(0x01D56250, Netchan_CreateFileFragmentsFromBuffer),
	HOOK_DEF(0x01D56450, Netchan_CreateFileFragments),
	HOOK_DEF(0x01D56850, Netchan_FlushIncoming),
	HOOK_DEF(0x01D568C0, Netchan_CopyNormalFragments),
	HOOK_DEF(0x01D569D0, Netchan_CopyFileFragments),
	//HOOK_DEF(0x01D56DB0, Netchan_IsSending),
	//HOOK_DEF(0x01D56DE0, Netchan_IsReceiving),
	HOOK_DEF(0x01D56E10, Netchan_IncomingReady),
	//HOOK_DEF(0x01D56E40, Netchan_UpdateProgress),
	HOOK_DEF(0x01D56FE0, Netchan_Init),
	//HOOK_DEF(0x01D57030, Netchan_CompressPacket),		// NOXREF
	//HOOK_DEF(0x01D57040, Netchan_DecompressPacket),	// NOXREF

#endif // Net_chan_region

#ifndef Hashpak_region

	HOOK_DEF(0x01D3E970, HPAK_GetDataPointer),
	HOOK_DEF(0x01D3EC40, HPAK_FindResource),
	HOOK_DEF(0x01D3ECB0, HPAK_AddToQueue),
	HOOK_DEF(0x01D3EDC0, HPAK_FlushHostQueue),
	HOOK_DEF(0x01D3EE20, HPAK_AddLump),
	HOOK_DEF(0x01D3F3D0, HPAK_RemoveLump),
	HOOK_DEF(0x01D3F850, HPAK_ResourceForIndex),
	HOOK_DEF(0x01D3F9E0, HPAK_ResourceForHash),
	HOOK_DEF(0x01D3FBA0, HPAK_List_f),
	HOOK_DEF(0x01D3FE30, HPAK_CreatePak),
	HOOK_DEF(0x01D40140, HPAK_Remove_f),
	HOOK_DEF(0x01D401F0, HPAK_Validate_f),
	HOOK_DEF(0x01D405B0, HPAK_Extract_f),
	HOOK_DEF(0x01D409A0, HPAK_Init),
	HOOK_DEF(0x01D409F1, HPAK_GetItem),					// NOXREF
	HOOK_DEF(0x01D40B90, HPAK_CheckSize),
	HOOK_DEF(0x01D40CE0, HPAK_ValidatePak),
	HOOK_DEF(0x01D40F80, HPAK_CheckIntegrity),

#endif // Hashpak_region

#ifndef Wad_region

	HOOK_DEF(0x01DB8D30, W_CleanupName),
	HOOK_DEF(0x01DB8D90, W_LoadWadFile),
	HOOK_DEF(0x01DB8EF0, W_GetLumpinfo),
	HOOK_DEF(0x01DB8F70, W_GetLumpName),
	//HOOK_DEF(0x, W_GetLumpNum),						// NOXREF
	HOOK_DEF(0x01DB8FF0, W_Shutdown),
	HOOK_DEF(0x01DB9020, SwapPic),

#endif // Wad_region

#ifndef Textures_region

	HOOK_DEF(0x01DA7200, SafeRead),
	HOOK_DEF(0x01DA7230, CleanupName),
	HOOK_DEF(0x01DA7290, lump_sorter),
	HOOK_DEF(0x01DA72E0, ForwardSlashes),
	HOOK_DEF(0x01DA7300, TEX_InitFromWad),
	HOOK_DEF(0x01DA75D0, TEX_CleanupWadInfo),
	HOOK_DEF(0x01DA7630, TEX_LoadLump),
	HOOK_DEF(0x01DA76D4, FindMiptex),
	HOOK_DEF(0x01DA7740, TEX_AddAnimatingTextures),

#endif // Textures_region

#ifndef Sv_user_region

	HOOK_DEF(0x01D9B3B0, SV_ParseConsistencyResponse),
	HOOK_DEF(0x01D9B790, SV_FileInConsistencyList),
	HOOK_DEF(0x01D9B7F0, SV_TransferConsistencyInfo),
	HOOK_DEF(0x01D9B960, SV_SendConsistencyList),
	HOOK_DEF(0x01D9BA50, SV_PreRunCmd),
	HOOK_DEF(0x01D9BA60, SV_CopyEdictToPhysent),
	HOOK_DEF(0x01D9BD70, SV_AddLinksToPM_),
	HOOK_DEF(0x01D9C070, SV_AddLinksToPM),
	HOOK_DEF(0x01D9C1A0, SV_PlayerRunPreThink),
	HOOK_DEF(0x01D9C1C0, SV_PlayerRunThink),
	HOOK_DEF(0x01D9C260, SV_CheckMovingGround),
	HOOK_DEF(0x01D9C340, SV_ConvertPMTrace),
	HOOK_DEF(0x01D9C3A0, SV_ForceFullClientsUpdate),
	HOOK_DEF(0x01D9C470, SV_RunCmd),
	HOOK_DEF(0x01D9D69F, SV_ValidateClientCommand),
	HOOK_DEF(0x01D9D6EF, SV_CalcClientTime),
	HOOK_DEF(0x01D9D86F, SV_ComputeLatency),
	HOOK_DEF(0x01D9D88F, SV_UnlagCheckTeleport),
	HOOK_DEF(0x01D9D8DF, SV_GetTrueOrigin),
	HOOK_DEF(0x01D9D96F, SV_GetTrueMinMax),
	HOOK_DEF(0x01D9D9EF, SV_FindEntInPack),
	HOOK_DEF(0x01D9DA2F, SV_SetupMove),
	HOOK_DEF(0x01D9E01F, SV_RestoreMove),
	HOOK_DEF(0x01D9E17F, SV_ParseStringCommand),
	HOOK_DEF(0x01D9E1DF, SV_ParseDelta),
	HOOK_DEF(0x01D9E1FF, SV_EstablishTimeBase),
	HOOK_DEF(0x01D9E2EF, SV_ParseMove),
	HOOK_DEF(0x01D9E76F, SV_ParseVoiceData),
	HOOK_DEF(0x01D9E8DF, SV_IgnoreHLTV),
	HOOK_DEF(0x01D9E8EF, SV_ParseCvarValue),
	HOOK_DEF(0x01D9E92F, SV_ParseCvarValue2),
	HOOK_DEF(0x01D9E9AF, SV_ExecuteClientMessage),
	HOOK_DEF(0x01D9EACF, SV_SetPlayer),
	HOOK_DEF(0x01D9EB2F, SV_ShowServerinfo_f),
	HOOK_DEF(0x01D9EB4F, SV_SendEnts_f),
	HOOK_DEF(0x01D9EB7F, SV_FullUpdate_f),

#endif // Sv_user_region

#ifndef Tmessage_region

	HOOK_DEF(0x01DA7B00, memfgets),
	HOOK_DEF(0x01DA7B90, IsComment),
	HOOK_DEF(0x01DA7BD0, IsStartOfText),
	HOOK_DEF(0x01DA7BF0, IsEndOfText),
	HOOK_DEF(0x01DA7C10, IsWhiteSpace),
	//HOOK_DEF(0x01DA7C40, SkipSpace),					// NOXREF
	//HOOK_DEF(0x01DA7C80, SkipText),					// NOXREF
	//HOOK_DEF(0x01DA7CC0, ParseFloats),				// NOXREF
	HOOK_DEF(0x01DA7D10, TrimSpace),
	//HOOK_DEF(0x01DA7D90, IsToken),					// NOXREF
	//HOOK_DEF(0x01DA7DD0, ParseDirective),				// NOXREF
	//HOOK_DEF(0x01DA80A0, TextMessageParse),			// NOXREF
	//HOOK_DEF(0x01DA8030, TextMessageShutdown),		// NOXREF
	//HOOK_DEF(0x01DA8050, TextMessageInit),			// NOXREF
	//HOOK_DEF(0x01DA8410, TextMessageGet),				// NOXREF

#endif // Tmessage_region

#ifndef TraceInit_Region

	//HOOK_SYMBOLDEF(0x01DA8B60, "_ZN12CInitTrackerC2Ev", MethodThunk<CInitTracker>::Constructor),
	//HOOK_SYMBOLDEF(0x01DA8BD0, "_ZN12CInitTrackerD2Ev", MethodThunk<CInitTracker>::Destructor),

	HOOK_SYMBOLDEF(0x01DA8CA0, "_ZN12CInitTracker4InitEPKcS1_i", CInitTracker::Init),
	HOOK_SYMBOLDEF(0x01DA8D80, "_ZN12CInitTracker8ShutdownEPKci", CInitTracker::Shutdown),
	HOOK_SYMBOLDEF(0x01DA8E60, "_Z9TraceInitPKcS0_i", TraceInit),
	HOOK_SYMBOLDEF(0x01DA8E80, "_Z13TraceShutdownPKci", TraceShutdown),

#endif // TraceInit_Region

#ifndef Vid_null_region

	//HOOK_DEF(0x, VID_SetPalette),
	//HOOK_DEF(0x, VID_ShiftPalette),
	//HOOK_DEF(0x, VID_WriteBuffer),
	//HOOK_DEF(0x01DB4BD0, VID_Init),
	//HOOK_DEF(0x, D_FlushCaches),
	//HOOK_DEF(0x, R_SetStackBase),
	//HOOK_DEF(0x01D7E3D0, SCR_UpdateScreen),
	//HOOK_DEF(0x, V_Init),
	//HOOK_DEF(0x, Draw_Init),
	//HOOK_DEF(0x, SCR_Init),
	//HOOK_DEF(0x01D65630, R_Init),
	//HOOK_DEF(0x01D67B20, R_ForceCVars),
	//HOOK_DEF(0x01D7E320, SCR_BeginLoadingPlaque),
	//HOOK_DEF(0x01D7E3A0, SCR_EndLoadingPlaque),
	//HOOK_DEF(0x, R_InitSky),
	//HOOK_DEF(0x01D65F70, R_MarkLeaves),
	HOOK_DEF(0x01D65570, R_InitTextures),
	//HOOK_DEF(0x, StartLoadingProgressBar),
	//HOOK_DEF(0x01D07670, ContinueLoadingProgressBar),
	//HOOK_DEF(0x01D07700, SetLoadingProgressBarStatusText),

#endif // Vid_null_region

#ifndef L_studio_region

	HOOK_DEF(0x01D4F0E0, Mod_LoadStudioModel),

#endif // L_studio_region

#ifndef Crc_region

	HOOK_DEF(0x01D2C610, CRC32_Init),
	HOOK_DEF(0x01D2C620, CRC32_Final),
	HOOK_DEF(0x01D2C630, CRC32_ProcessByte),
	HOOK_DEF(0x01D2C660, CRC32_ProcessBuffer),
	HOOK_DEF(0x01D2C8D0, COM_BlockSequenceCRCByte),
	HOOK_DEF(0x01D2C970, CRC_File),	// NOXREF
	HOOK_DEF(0x01D2CA40, CRC_MapFile),
	HOOK_DEF(0x01D2CBB0, MD5Init),
	HOOK_DEF(0x01D2CBE0, MD5Update),
	HOOK_DEF(0x01D2CCB0, MD5Final),
	HOOK_DEF(0x01D2CD40, MD5Transform),
	HOOK_DEF(0x01D2D5B0, MD5_Hash_File),
	HOOK_DEF(0x01D2D6F0, MD5_Print),

#endif // Crc_region

#ifndef Sv_RemoteAccess_region

	// virtual functions
	HOOK_SYMBOL_VIRTUAL_DEF(0x01D97850, "_ZN19CServerRemoteAccess16WriteDataRequestEPKvi", CServerRemoteAccess::WriteDataRequest),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01D979A0, "_ZN19CServerRemoteAccess16ReadDataResponseEPvi", CServerRemoteAccess::ReadDataResponse),

	HOOK_SYMBOLDEF(0x01D984E0, "_ZN19CServerRemoteAccess20SendMessageToAdminUIEPKc", CServerRemoteAccess::SendMessageToAdminUI),
	HOOK_SYMBOLDEF(0x01D98090, "_ZN19CServerRemoteAccess17LookupStringValueEPKc", CServerRemoteAccess::LookupStringValue),
	HOOK_SYMBOLDEF(0x01D97D40, "_ZN19CServerRemoteAccess11LookupValueEPKcR10CUtlBuffer", CServerRemoteAccess::LookupValue),
	HOOK_SYMBOLDEF(0x01D98190, "_ZN19CServerRemoteAccess14GetUserBanListER10CUtlBuffer", CServerRemoteAccess::GetUserBanList),
	HOOK_SYMBOLDEF(0x01D98280, "_ZN19CServerRemoteAccess13GetPlayerListER10CUtlBuffer", CServerRemoteAccess::GetPlayerList),
	HOOK_SYMBOLDEF(0x01D983B0, "_ZN19CServerRemoteAccess10GetMapListER10CUtlBuffer", CServerRemoteAccess::GetMapList),
	HOOK_SYMBOLDEF(0x01D97AD0, "_ZN19CServerRemoteAccess12RequestValueEiPKc", CServerRemoteAccess::RequestValue),
	HOOK_SYMBOLDEF(0x01D97C20, "_ZN19CServerRemoteAccess8SetValueEPKcS1_", CServerRemoteAccess::SetValue),
	HOOK_SYMBOLDEF(0x01D97D10, "_ZN19CServerRemoteAccess11ExecCommandEPKc", CServerRemoteAccess::ExecCommand),

	HOOK_DEF(0x01D985E0, NotifyDedicatedServerUI),

#endif // Sv_RemoteAccess_region

#ifndef IpratelimitWrapper_region

	//HOOK_DEF(0x01D4D990, CheckIP),

#endif // IpratelimitWrapper_region

#ifndef Com_custom

	HOOK_DEF(0x01D28610, COM_ClearCustomizationList),
	HOOK_DEF(0x01D28740, COM_CreateCustomization),
	HOOK_DEF(0x01D28980, COM_SizeofResourceList),

#endif // Com_custom

#ifndef Sv_upld

	HOOK_DEF(0x01D9A980, SV_CheckFile),
	HOOK_DEF(0x01D9AA50, SV_ClearResourceLists),
	HOOK_DEF(0x01D9AA90, SV_CreateCustomizationList),
	HOOK_DEF(0x01D9ABB0, SV_Customization),
	HOOK_DEF(0x01D9AD20, SV_RegisterResources),
	HOOK_DEF(0x01D9AD70, SV_MoveToOnHandList),
	HOOK_DEF(0x01D9ADB0, SV_AddToResourceList),
	HOOK_DEF(0x01D9AE00, SV_ClearResourceList),
	HOOK_DEF(0x01D9AE50, SV_RemoveFromResourceList),
	HOOK_DEF(0x01D9AE80, SV_EstimateNeededResources),
	HOOK_DEF(0x01D9AEF0, SV_RequestMissingResourcesFromClients),
	HOOK_DEF(0x01D9AF40, SV_UploadComplete),
	HOOK_DEF(0x01D9AFA0, SV_BatchUploadRequest),
	HOOK_DEF(0x01D9B040, SV_RequestMissingResources),
	HOOK_DEF(0x01D9B070, SV_ParseResourceList),

#endif // Sv_upld

#ifndef Decals

	HOOK_DEF(0x01D33EF0, Draw_Shutdown),
	HOOK_DEF(0x01D33F30, Draw_DecalShutdown),
	HOOK_DEF(0x01D33F60, Draw_AllocateCacheSpace),
	HOOK_DEF(0x01D33F90, Draw_CacheWadInitFromFile),
	HOOK_DEF(0x01D34070, Draw_CacheWadInit),
	HOOK_DEF(0x01D340C0, Draw_CustomCacheWadInit),
	HOOK_DEF(0x01D3A830, Draw_MiptexTexture),
	HOOK_DEF(0x01D34290, Draw_CacheWadHandler),
	HOOK_DEF(0x01D342B0, Draw_FreeWad),
	HOOK_DEF(0x01D34380, Draw_DecalSetName),			// NOXREF
	HOOK_DEF(0x01D343C0, Draw_DecalIndex),				// NOXREF
	HOOK_DEF(0x01D34450, Draw_DecalCount),
	HOOK_DEF(0x01D34460, Draw_DecalSize),
	HOOK_DEF(0x01D34490, Draw_DecalName),
	HOOK_DEF(0x01D344C0, Draw_DecalTexture),			// NOXREF
	HOOK_DEF(0x01D34560, Draw_CacheByIndex),
	HOOK_DEF(0x01D34600, Draw_DecalIndexFromName),		// NOXREF
	HOOK_DEF(0x01D34670, Decal_ReplaceOrAppendLump),
	HOOK_DEF(0x01D34710, Decal_CountLumps),
	HOOK_DEF(0x01D34730, Decal_SizeLumps),
	HOOK_DEF(0x01D34750, Decal_MergeInDecals),
	HOOK_DEF(0x01D349A0, Decal_Init),
	HOOK_DEF(0x01D34B00, CustomDecal_Validate),
	HOOK_DEF(0x01D34B60, CustomDecal_Init),
	HOOK_DEF(0x01D34BD0, Draw_CacheGet),				// NOXREF
	HOOK_DEF(0x01D34CC0, Draw_CustomCacheGet),
	HOOK_DEF(0x01D34D60, Draw_CacheReload),				// NOXREF
	HOOK_DEF(0x01D34E40, Draw_ValidateCustomLogo),
	HOOK_DEF(0x01D35000, Draw_CacheLoadFromCustom),
	HOOK_DEF(0x01D35110, Draw_CacheIndex),				// NOXREF
	HOOK_DEF(0x01D351A9, Draw_CacheFindIndex),			// NOXREF

#endif // Decals

#ifndef Sys_engine

	//HOOK_SYMBOLDEF(0x, "_ZN7CEngineC2Ev", MethodThunk<CEngine>::Constructor),
	//HOOK_SYMBOLDEF(0x, "_ZN7CEngineD0Ev", MethodThunk<CEngine>::Destructor),

	// virtual functions
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1C80, "_ZN7CEngine6UnloadEv", CEngine::Unload),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1CA0, "_ZN7CEngine4LoadEbPcS0_", CEngine::Load),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1CF0, "_ZN7CEngine5FrameEv", CEngine::Frame),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1DD0, "_ZN7CEngine11SetSubStateEi", CEngine::SetSubState),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1DF0, "_ZN7CEngine8SetStateEi", CEngine::SetState),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1E10, "_ZN7CEngine8GetStateEv", CEngine::GetState),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1E20, "_ZN7CEngine11GetSubStateEv", CEngine::GetSubState),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1E30, "_ZN7CEngine12GetFrameTimeEv", CEngine::GetFrameTime),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1E40, "_ZN7CEngine10GetCurTimeEv", CEngine::GetCurTime),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1E50, "_ZN7CEngine13TrapKey_EventEib", CEngine::TrapKey_Event),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1E90, "_ZN7CEngine15TrapMouse_EventEib", CEngine::TrapMouse_Event),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1ED0, "_ZN7CEngine13StartTrapModeEv", CEngine::StartTrapMode),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1EE0, "_ZN7CEngine10IsTrappingEv", CEngine::IsTrapping),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1EF0, "_ZN7CEngine17CheckDoneTrappingERiS0_", CEngine::CheckDoneTrapping),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1F30, "_ZN7CEngine11SetQuittingEi", CEngine::SetQuitting),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA1F40, "_ZN7CEngine11GetQuittingEv", CEngine::GetQuitting),

#endif // Sys_engine

#ifndef Sys_linuxwind

	// Disable/enable all at once!
	// virtual functions
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5A10, "_ZN5CGame4InitEPv", CGame::Init),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5A20, "_ZN5CGame8ShutdownEv", CGame::Shutdown),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5760, "_ZN5CGame16CreateGameWindowEv", CGame::CreateGameWindow),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA4B20, "_ZN5CGame15SleepUntilInputEi", CGame::SleepUntilInput),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5A40, "_ZN5CGame13GetMainWindowEv", CGame::GetMainWindow),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5A50, "_ZN5CGame20GetMainWindowAddressEv", CGame::GetMainWindowAddress),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5A60, "_ZN5CGame11SetWindowXYEii", CGame::SetWindowXY),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5A80, "_ZN5CGame13SetWindowSizeEii", CGame::SetWindowSize),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5B00, "_ZN5CGame13GetWindowRectEPiS0_S0_S0_", CGame::GetWindowRect),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5B40, "_ZN5CGame11IsActiveAppEv", CGame::IsActiveApp),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5B60, "_ZN5CGame13IsMultiplayerEv", CGame::IsMultiplayer),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5BB0, "_ZN5CGame17PlayStartupVideosEv", CGame::PlayStartupVideos),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5BC0, "_ZN5CGame14PlayAVIAndWaitEPKc", CGame::PlayAVIAndWait),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5B70, "_ZN5CGame16SetCursorVisibleEb", CGame::SetCursorVisible),

#endif // Sys_linuxwind

#ifndef SystemWrapper_region

	HOOK_DEF(0x01DA5CB0, SystemWrapper_Init),
	HOOK_DEF(0x01DA5CD0, SystemWrapper_RunFrame),
	HOOK_DEF(0x01DA5CF0, SystemWrapper_ShutDown),
	HOOK_DEF(0x01DA5D00, SystemWrapper_LoadModule),		// NOXREF
	HOOK_DEF(0x01DA5D30, SystemWrapper_ExecuteString),
	HOOK_DEF(0x01DA5D50, SystemWrapper_CommandForwarder),

	// virtual functions
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA7120, "_ZN13SystemWrapper4InitEP11IBaseSystemiPc", SystemWrapper::Init),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA7130, "_ZN13SystemWrapper8RunFrameEd", SystemWrapper::RunFrame),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA7150, "_ZN13SystemWrapper14ExecuteCommandEiPc", SystemWrapper::ExecuteCommand),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA71A0, "_ZN13SystemWrapper13GetStatusLineEv", SystemWrapper::GetStatusLine),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA71B0, "_ZN13SystemWrapper7GetTypeEv", SystemWrapper::GetType),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA71E0, "_ZN13SystemWrapper8ShutDownEv", SystemWrapper::ShutDown),

	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6730, "_ZN13SystemWrapper7GetTimeEv", SystemWrapper::GetTime),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6950, "_ZN13SystemWrapper7GetTickEv", SystemWrapper::GetTick),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6FA0, "_ZN13SystemWrapper6SetFPSEf", SystemWrapper::SetFPS),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6750, "_ZN13SystemWrapper6PrintfEPcz", SystemWrapper::Printf),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6820, "_ZN13SystemWrapper7DPrintfEPcz", SystemWrapper::DPrintf),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6740, "_ZN13SystemWrapper14RedirectOutputEPci", SystemWrapper::RedirectOutput),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6900, "_ZN13SystemWrapper13GetFileSystemEv", SystemWrapper::GetFileSystem),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6910, "_ZN13SystemWrapper8LoadFileEPKcPi", SystemWrapper::LoadFile),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6930, "_ZN13SystemWrapper8FreeFileEPh", SystemWrapper::FreeFile),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6710, "_ZN13SystemWrapper8SetTitleEPc", SystemWrapper::SetTitle),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6720, "_ZN13SystemWrapper13SetStatusLineEPc", SystemWrapper::SetStatusLine),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6FB0, "_ZN13SystemWrapper11ShowConsoleEb", SystemWrapper::ShowConsole),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6A90, "_ZN13SystemWrapper10LogConsoleEPc", SystemWrapper::LogConsole),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6AD0, "_ZN13SystemWrapper8InitVGUIEP11IVGuiModule", SystemWrapper::InitVGUI),
#ifdef _WIN32
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6AC0, "_ZN13SystemWrapper8GetPanelEv", SystemWrapper::GetPanel),
#endif // _WIN32
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6850, "_ZN13SystemWrapper15RegisterCommandEPcP13ISystemModulei", SystemWrapper::RegisterCommand),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA69F0, "_ZN13SystemWrapper17GetCommandMatchesEPcP10ObjectList", SystemWrapper::GetCommandMatches),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6CA0, "_ZN13SystemWrapper13ExecuteStringEPc", SystemWrapper::ExecuteString),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA67E0, "_ZN13SystemWrapper11ExecuteFileEPc", SystemWrapper::ExecuteFile),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6200, "_ZN13SystemWrapper6ErrorfEPcz", SystemWrapper::Errorf),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA61C0, "_ZN13SystemWrapper10CheckParamEPc", SystemWrapper::CheckParam),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6AE0, "_ZN13SystemWrapper9AddModuleEP13ISystemModulePc", SystemWrapper::AddModule),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6B50, "_ZN13SystemWrapper9GetModuleEPcS0_S0_", SystemWrapper::GetModule),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6BD0, "_ZN13SystemWrapper12RemoveModuleEP13ISystemModule", SystemWrapper::RemoveModule),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6300, "_ZN13SystemWrapper4StopEv", SystemWrapper::Stop),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6FC0, "_ZN13SystemWrapper14COM_GetBaseDirEv", SystemWrapper::GetBaseDir),

	HOOK_SYMBOLDEF(0x01DA6D60, "_ZN13SystemWrapper15DispatchCommandEPc", SystemWrapper::DispatchCommand),
	HOOK_SYMBOLDEF(0x01DA65C0, "_ZN13SystemWrapper10GetLibraryEPc", SystemWrapper::GetLibrary),
	HOOK_SYMBOLDEF(0x01DA6A10, "_ZN13SystemWrapper10FindModuleEPcS0_", SystemWrapper::FindModule),
	HOOK_SYMBOLDEF(0x01DA6960, "_ZN13SystemWrapper11CMD_ModulesEPc", SystemWrapper::CMD_Modules),
	HOOK_SYMBOLDEF(0x01DA6420, "_ZN13SystemWrapper14CMD_LoadModuleEPc", SystemWrapper::CMD_LoadModule),
	HOOK_SYMBOLDEF(0x01DA64E0, "_ZN13SystemWrapper16CMD_UnloadModuleEPc", SystemWrapper::CMD_UnloadModule),

	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6E40, "_ZN13EngineWrapper4InitEP11IBaseSystemiPc", EngineWrapper::Init),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6E70, "_ZN13EngineWrapper8RunFrameEd", EngineWrapper::RunFrame),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6EF0, "_ZN13EngineWrapper13ReceiveSignalEP13ISystemModulejPv", EngineWrapper::ReceiveSignal),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6E90, "_ZN13EngineWrapper14ExecuteCommandEiPc", EngineWrapper::ExecuteCommand),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6F10, "_ZN13EngineWrapper16RegisterListenerEP13ISystemModule", EngineWrapper::RegisterListener),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6F30, "_ZN13EngineWrapper14RemoveListenerEP13ISystemModule", EngineWrapper::RemoveListener),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6F50, "_ZN13EngineWrapper9GetSystemEv", EngineWrapper::GetSystem),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6F60, "_ZN13EngineWrapper9GetSerialEv", EngineWrapper::GetSerial),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5EC0, "_ZN13EngineWrapper13GetStatusLineEv", EngineWrapper::GetStatusLine),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5EB0, "_ZN13EngineWrapper7GetTypeEv", EngineWrapper::GetType),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6F70, "_ZN13EngineWrapper7GetNameEv", EngineWrapper::GetName),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6F80, "_ZN13EngineWrapper8GetStateEv", EngineWrapper::GetState),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6F90, "_ZN13EngineWrapper10GetVersionEv", EngineWrapper::GetVersion),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6E60, "_ZN13EngineWrapper8ShutDownEv", EngineWrapper::ShutDown),

	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5ED0, "_ZN13EngineWrapper13GetViewOriginEPf", EngineWrapper::GetViewOrigin),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5F00, "_ZN13EngineWrapper13GetViewAnglesEPf", EngineWrapper::GetViewAngles),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6070, "_ZN13EngineWrapper14GetTraceEntityEv", EngineWrapper::GetTraceEntity),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5F30, "_ZN13EngineWrapper12GetCvarFloatEPc", EngineWrapper::GetCvarFloat),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5F60, "_ZN13EngineWrapper13GetCvarStringEPc", EngineWrapper::GetCvarString),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5F80, "_ZN13EngineWrapper7SetCvarEPcS0_", EngineWrapper::SetCvar),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5FA0, "_ZN13EngineWrapper12Cbuf_AddTextEPc", EngineWrapper::Cbuf_AddText),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5FC0, "_ZN13EngineWrapper20DemoUpdateClientDataEP13client_data_s", EngineWrapper::DemoUpdateClientData),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA5FE0, "_ZN13EngineWrapper13CL_QueueEventEiifP12event_args_s", EngineWrapper::CL_QueueEvent),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6000, "_ZN13EngineWrapper13HudWeaponAnimEii", EngineWrapper::HudWeaponAnim),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6020, "_ZN13EngineWrapper16CL_DemoPlaySoundEiPcffii", EngineWrapper::CL_DemoPlaySound),
	HOOK_SYMBOL_VIRTUAL_DEF(0x01DA6050, "_ZN13EngineWrapper24ClientDLL_ReadDemoBufferEiPh", EngineWrapper::ClientDLL_ReadDemoBuffer),

#endif // SystemWrapper_region

	{ NULL, NULL, NULL },
};


AddressRef g_FunctionRefs[] =
{
#ifndef Function_References_region

	{ 0x01D4D990, "CheckIP", (size_t)&pCheckIP },

	// Find C STD functions
	{ 0x01E000AE, "time", 0 },
	{ 0x01E0008C, "rand", 0 },
	{ 0x01E0007F, "srand", 0 },
	{ 0x01E00AF1, "localtime", 0 },

#endif // Function_References_region

	{ NULL, NULL, NULL },
};

AddressRef g_DataRefs[] =
{
#ifndef Data_References_region


	GLOBALVAR_LINK(0x0229AE80, "pr_strings", ppr_strings),
	GLOBALVAR_LINK(0x01E45C04, "gNullString", pgNullString),
	GLOBALVAR_LINK(0x01E46480, "SV_UPDATE_BACKUP", pSV_UPDATE_BACKUP),
	GLOBALVAR_LINK(0x01E46484, "SV_UPDATE_MASK", pSV_UPDATE_MASK),
	GLOBALVAR_LINK(0x021D3E80, "gGlobalVariables", pgGlobalVariables),
	GLOBALVAR_LINK(0x021D3DE0, "svs", psvs),
	GLOBALVAR_LINK(0x021D4960, "sv", psv),
	GLOBALVAR_LINK(0x0208F788, "gPacketSuppressed", pgPacketSuppressed),
	GLOBALVAR_LINK(0x01E45C08, "giNextUserMsg", pgiNextUserMsg),
	GLOBALVAR_LINK(0x0208F7A4, "hashstrings_collisions", phashstrings_collisions),
	GLOBALVAR_LINK(0x0208F76C, "g_pplayerdelta", pg_pplayerdelta),
	GLOBALVAR_LINK(0x0208F770, "g_pentitydelta", pg_pentitydelta),
	GLOBALVAR_LINK(0x0208F774, "g_pcustomentitydelta", pg_pcustomentitydelta),
	GLOBALVAR_LINK(0x0208F778, "g_pclientdelta", pg_pclientdelta),
	GLOBALVAR_LINK(0x0208F77C, "g_pweapondelta", pg_pweapondelta),
	GLOBALVAR_LINK(0x021C2B60, "localinfo", plocalinfo),
	GLOBALVAR_LINK(0x021D3F40, "localmodels", plocalmodels),
	GLOBALVAR_LINK(0x0221AD80, "ipfilters", pipfilters),
	GLOBALVAR_LINK(0x021D4940, "numipfilters", pnumipfilters),
	GLOBALVAR_LINK(0x0229B6C0, "userfilters", puserfilters),
	GLOBALVAR_LINK(0x0208F798, "numuserfilters", pnumuserfilters),
	GLOBALVAR_LINK(0x01E45F84, "sv_lan", psv_lan),
	GLOBALVAR_LINK(0x01E45FAC, "sv_lan_rate", psv_lan_rate),
	GLOBALVAR_LINK(0x01E45E8C, "sv_aim", psv_aim),
	GLOBALVAR_LINK(0x021D1BC0, "sv_decalnames", psv_decalnames),
	GLOBALVAR_LINK(0x0208F78C, "sv_decalnamecount", psv_decalnamecount),
	GLOBALVAR_LINK(0x01E46274, "sv_skycolor_r", psv_skycolor_r),
	GLOBALVAR_LINK(0x01E46298, "sv_skycolor_g", psv_skycolor_g),
	GLOBALVAR_LINK(0x01E462BC, "sv_skycolor_b", psv_skycolor_b),
	GLOBALVAR_LINK(0x01E462E0, "sv_skyvec_x", psv_skyvec_x),
	GLOBALVAR_LINK(0x01E46304, "sv_skyvec_y", psv_skyvec_y),
	GLOBALVAR_LINK(0x01E46328, "sv_skyvec_z", psv_skyvec_z),
	GLOBALVAR_LINK(0x01E48F64, "sv_spectatormaxspeed", psv_spectatormaxspeed),
	GLOBALVAR_LINK(0x01E48F90, "sv_airaccelerate", psv_airaccelerate),
	GLOBALVAR_LINK(0x01E48FBC, "sv_wateraccelerate", psv_wateraccelerate),
	GLOBALVAR_LINK(0x01E48FE4, "sv_waterfriction", psv_waterfriction),
	GLOBALVAR_LINK(0x01E49008, "sv_zmax", psv_zmax),
	GLOBALVAR_LINK(0x01E4902C, "sv_wateramp", psv_wateramp),
	GLOBALVAR_LINK(0x01E49054, "sv_skyname", psv_skyname),
	GLOBALVAR_LINK(0x01E46034, "mapcyclefile", pmapcyclefile),
	GLOBALVAR_LINK(0x01E46060, "motdfile", pmotdfile),
	GLOBALVAR_LINK(0x01E46090, "servercfgfile", pservercfgfile),
	GLOBALVAR_LINK(0x01E460F0, "lservercfgfile", plservercfgfile),
	GLOBALVAR_LINK(0x01E46114, "logsdir", plogsdir),
	GLOBALVAR_LINK(0x01E46144, "bannedcfgfile", pbannedcfgfile),
	GLOBALVAR_LINK(0x0208F790, "sv_gpNewUserMsgs", psv_gpNewUserMsgs),
	GLOBALVAR_LINK(0x0208F794, "sv_gpUserMsgs", psv_gpUserMsgs),
	GLOBALVAR_LINK(0x0239BCE0, "g_svmove", pg_svmove),
	GLOBALVAR_LINK(0x0208F760, "sv_lastnum", psv_lastnum),
	GLOBALVAR_LINK(0x02089220, "g_sv_instance_baselines", pg_sv_instance_baselines),
	GLOBALVAR_LINK(0x0208F764, "g_bOutOfDateRestart", pg_bOutOfDateRestart),
	GLOBALVAR_LINK(0x01E45C00, "g_userid", pg_userid),
	GLOBALVAR_LINK(0x0208F768, "g_sv_delta", pg_sv_delta),
	GLOBALVAR_LINK(0x0208F780, "g_peventdelta", pg_peventdelta),
	GLOBALVAR_LINK(0x01E45C1C, "rcon_password", prcon_password),
	GLOBALVAR_LINK(0x01E45C48, "sv_enableoldqueries", psv_enableoldqueries),
	GLOBALVAR_LINK(0x01E45C74, "sv_instancedbaseline", psv_instancedbaseline),
	GLOBALVAR_LINK(0x01E45C94, "sv_contact", psv_contact),
	GLOBALVAR_LINK(0x01E45CC4, "sv_maxupdaterate", psv_maxupdaterate),
	GLOBALVAR_LINK(0x01E45CF4, "sv_minupdaterate", psv_minupdaterate),
	GLOBALVAR_LINK(0x01E45D18, "sv_filterban", psv_filterban),
	GLOBALVAR_LINK(0x01E45D3C, "sv_minrate", psv_minrate),
	GLOBALVAR_LINK(0x01E45D60, "sv_maxrate", psv_maxrate),
	GLOBALVAR_LINK(0x01E45D84, "sv_logrelay", psv_logrelay),
	GLOBALVAR_LINK(0x01E45DD0, "violence_hblood", pviolence_hblood),
	GLOBALVAR_LINK(0x01E45DF8, "violence_ablood", pviolence_ablood),
	GLOBALVAR_LINK(0x01E45E20, "violence_hgibs", pviolence_hgibs),
	GLOBALVAR_LINK(0x01E45E48, "violence_agibs", pviolence_agibs),
	GLOBALVAR_LINK(0x01E45E6C, "sv_newunit", psv_newunit),
	GLOBALVAR_LINK(0x01E45ED8, "sv_clienttrace", psv_clienttrace),
	GLOBALVAR_LINK(0x01E45EFC, "sv_timeout", psv_timeout),
	GLOBALVAR_LINK(0x01E45F24, "sv_failuretime", psv_failuretime),
	GLOBALVAR_LINK(0x01E45F44, "sv_cheats", psv_cheats),
	GLOBALVAR_LINK(0x01E45F64, "sv_password", psv_password),
	GLOBALVAR_LINK(0x01E45FD0, "sv_proxies", psv_proxies),
	GLOBALVAR_LINK(0x01E46000, "sv_outofdatetime", psv_outofdatetime),
	GLOBALVAR_LINK(0x01E460B8, "mapchangecfgfile", pmapchangecfgfile),
	GLOBALVAR_LINK(0x021D1BA0, "allow_cheats", pallow_cheats),
	GLOBALVAR_LINK(0x01E455A8, "mp_logecho", pmp_logecho),
	GLOBALVAR_LINK(0x01E45584, "mp_logfile", pmp_logfile),
	GLOBALVAR_LINK(0x01E4616C, "sv_allow_download", psv_allow_download),
	GLOBALVAR_LINK(0x01E46190, "sv_send_logos", psv_send_logos),
	GLOBALVAR_LINK(0x01E461B8, "sv_send_resources", psv_send_resources),
	GLOBALVAR_LINK(0x01E455FC, "sv_log_singleplayer", psv_log_singleplayer),
	GLOBALVAR_LINK(0x01E45620, "sv_logsecret", psv_logsecret),
	GLOBALVAR_LINK(0x01E455D0, "sv_log_onefile", psv_log_onefile),
	GLOBALVAR_LINK(0x01E461DC, "sv_logbans", psv_logbans),
	GLOBALVAR_LINK(0x01E46204, "sv_allow_upload", psv_allow_upload),
	GLOBALVAR_LINK(0x01E4622C, "sv_max_upload", psv_max_upload),
	GLOBALVAR_LINK(0x01E46250, "hpk_maxsize", phpk_maxsize),
	GLOBALVAR_LINK(0x01E46358, "sv_visiblemaxplayers", psv_visiblemaxplayers),
	GLOBALVAR_LINK(0x01E46380, "max_queries_sec", pmax_queries_sec),
	GLOBALVAR_LINK(0x01E463B0, "max_queries_sec_global", pmax_queries_sec_global),
	GLOBALVAR_LINK(0x01E463DC, "max_queries_window", pmax_queries_window),
	GLOBALVAR_LINK(0x01E46400, "sv_logblocks", psv_logblocks),
	GLOBALVAR_LINK(0x01E46424, "sv_downloadurl", psv_downloadurl),
	GLOBALVAR_LINK(0x01E4644C, "sv_allow_dlfile", psv_allow_dlfile),
	GLOBALVAR_LINK(0x01E4646C, "sv_version", psv_version),
	GLOBALVAR_LINK(0x0229B2A0, "sv_playermodel", psv_playermodel),
	GLOBALVAR_LINK(0x026EB5E0, "cls", pcls),
	GLOBALVAR_LINK(0x026F1DE0, "cl", pcl),
	GLOBALVAR_LINK(0x01FED50C, "key_dest", pkey_dest),
	GLOBALVAR_LINK(0x0269BB00, "g_clmove", pg_clmove),
	GLOBALVAR_LINK(0x028A1454, "cl_inmovie", pcl_inmovie),
	GLOBALVAR_LINK(0x01E2F0F8, "cl_name", pcl_name),
	GLOBALVAR_LINK(0x01E2F1B4, "rate", prate),
	GLOBALVAR_LINK(0x01E33D6C, "console", pconsole),
	GLOBALVAR_LINK(0x01E39FF4, "host_name", phost_name),
	GLOBALVAR_LINK(0x01E3A040, "host_speeds", phost_speeds),
	GLOBALVAR_LINK(0x01E3A064, "host_profile", phost_profile),
	GLOBALVAR_LINK(0x01E3A084, "developer", pdeveloper),
	GLOBALVAR_LINK(0x01E3A0AC, "host_limitlocal", phost_limitlocal),
	GLOBALVAR_LINK(0x01E3A0C8, "skill", pskill),
	GLOBALVAR_LINK(0x01E3A0EC, "deathmatch", pdeathmatch),
	GLOBALVAR_LINK(0x01E3A108, "coop", pcoop),
	GLOBALVAR_LINK(0x0266AFA0, "realtime", prealtime),
	GLOBALVAR_LINK(0x01FED0C0, "rolling_fps", prolling_fps),
	GLOBALVAR_LINK(0x0266AF20, "host_parms", phost_parms),
	GLOBALVAR_LINK(0x01FED0B4, "host_initialized", phost_initialized),
	GLOBALVAR_LINK(0x0266AEC8, "host_frametime", phost_frametime),
	GLOBALVAR_LINK(0x0266AED0, "host_framecount", phost_framecount),
	GLOBALVAR_LINK(0x0266AEDC, "host_client", phost_client),
	GLOBALVAR_LINK(0x01FED0B8, "gfNoMasterServer", pgfNoMasterServer),
	GLOBALVAR_LINK(0x0266AF38, "oldrealtime", poldrealtime),
	GLOBALVAR_LINK(0x0266AEC4, "host_hunklevel", phost_hunklevel),
	GLOBALVAR_LINK(0x0266AF60, "host_abortserver", phost_abortserver),
	GLOBALVAR_LINK(0x0266AEE0, "host_enddemo", phost_enddemo),
	GLOBALVAR_LINK(0x0266AED4, "host_basepal", phost_basepal),
	GLOBALVAR_LINK(0x01FD9778, "bfread", pbfread),
	GLOBALVAR_LINK(0x01FD9790, "bfwrite", pbfwrite),
	GLOBALVAR_LINK(0x02699D48, "msg_readcount", pmsg_readcount),
	GLOBALVAR_LINK(0x02699D4C, "msg_badread", pmsg_badread),
	GLOBALVAR_LINK(0x01FEB260, "decal_wad", pdecal_wad),
	GLOBALVAR_LINK(0x01FEB264, "menu_wad", pmenu_wad),
	GLOBALVAR_LINK(0x0266D440, "szCustName", pszCustName),
	GLOBALVAR_LINK(0x0266B440, "decal_names", pdecal_names),
	GLOBALVAR_LINK(0x01FEB268, "m_bDrawInitialized", pm_bDrawInitialized),
	GLOBALVAR_LINK(0x01FEB25C, "gfCustomBuild", pgfCustomBuild),
	GLOBALVAR_LINK(0x01FEB26C, "g_defs", pg_defs),
	GLOBALVAR_LINK(0x01FEB270, "g_encoders", pg_encoders),
	GLOBALVAR_LINK(0x01FEB274, "g_deltaregistry", pg_deltaregistry),
	GLOBALVAR_LINK(0x021B97E0, "gNetworkTextMessageBuffer", pgNetworkTextMessageBuffer),
	GLOBALVAR_LINK(0x021B9FE0, "gMessageParms", pgMessageParms),
	GLOBALVAR_LINK(0x02095C8C, "gMessageTable", pgMessageTable),
	GLOBALVAR_LINK(0x02095C90, "gMessageTableCount", pgMessageTableCount),
	GLOBALVAR_LINK(0x01E4CE10, "gNetworkMessageNames", pgNetworkMessageNames),
	GLOBALVAR_LINK(0x01E4CE30, "gNetworkTextMessage", pgNetworkTextMessage),
	GLOBALVAR_LINK(0x021B61C4, "hunk_base", phunk_base),
	GLOBALVAR_LINK(0x021B61C8, "hunk_tempactive", phunk_tempactive),
	GLOBALVAR_LINK(0x021B61D0, "hunk_low_used", phunk_low_used),
	GLOBALVAR_LINK(0x021B61D4, "hunk_size", phunk_size),
	GLOBALVAR_LINK(0x021B61D8, "hunk_high_used", phunk_high_used),
	GLOBALVAR_LINK(0x021B61E0, "cache_head", pcache_head),
	GLOBALVAR_LINK(0x021B6238, "hunk_tempmark", phunk_tempmark),
	GLOBALVAR_LINK(0x01E4DEC0, "mem_dbgfile", pmem_dbgfile),
	GLOBALVAR_LINK(0x021B61CC, "mainzone", pmainzone),
	GLOBALVAR_LINK(0x0266AEC0, "current_skill", pcurrent_skill),
	GLOBALVAR_LINK(0x01FED1E0, "g_careerState", pg_careerState),
	GLOBALVAR_LINK(0x01FED1D8, "gHostSpawnCount", pgHostSpawnCount),
	GLOBALVAR_LINK(0x01E3A960, "g_pSaveGameCommentFunc", pg_pSaveGameCommentFunc),
	GLOBALVAR_LINK(0x01E5C834, "g_LastScreenUpdateTime", pg_LastScreenUpdateTime),
	GLOBALVAR_LINK(0x023FECB4, "scr_skipupdate", pscr_skipupdate),
	GLOBALVAR_LINK(0x023FF0C0, "scr_centertime_off", pscr_centertime_off),
	GLOBALVAR_LINK(0x02699460, "serverinfo", pserverinfo),
	GLOBALVAR_LINK(0x026995EC, "com_argc", pcom_argc),
	GLOBALVAR_LINK(0x02699708, "com_argv", pcom_argv),
	GLOBALVAR_LINK(0x02699840, "com_token", pcom_token),
	GLOBALVAR_LINK(0x01FE1FA0, "com_ignorecolons", pcom_ignorecolons),
	GLOBALVAR_LINK(0x01FE1FA4, "s_com_token_unget", ps_com_token_unget),
	GLOBALVAR_LINK(0x02699600, "com_clientfallback", pcom_clientfallback),
	GLOBALVAR_LINK(0x02699720, "com_gamedir", pcom_gamedir),
	GLOBALVAR_LINK(0x02699C40, "com_cmdline", pcom_cmdline),
	GLOBALVAR_LINK(0x02699580, "gpszVersionString", pgpszVersionString),
	GLOBALVAR_LINK(0x026995C0, "gpszProductString", pgpszProductString),
	GLOBALVAR_LINK(0x02699D44, "bigendien", pbigendien),
	GLOBALVAR_LINK(0x026995E8, "BigShort", pBigShort),
	GLOBALVAR_LINK(0x026995E0, "LittleShort", pLittleShort),
	GLOBALVAR_LINK(0x0269970C, "BigLong", pBigLong),
	GLOBALVAR_LINK(0x02699710, "LittleLong", pLittleLong),
	GLOBALVAR_LINK(0x026995A0, "BigFloat", pBigFloat),
	GLOBALVAR_LINK(0x02699560, "LittleFloat", pLittleFloat),
	GLOBALVAR_LINK(0x01FD3420, "cmd_argc", pcmd_argc),
	GLOBALVAR_LINK(0x01FD3428, "cmd_argv", pcmd_argv),
	GLOBALVAR_LINK(0x01FD366C, "cmd_args", pcmd_args),
	GLOBALVAR_LINK(0x02699D7C, "cmd_source", pcmd_source),
	GLOBALVAR_LINK(0x02699D74, "cmd_wait", pcmd_wait),
	GLOBALVAR_LINK(0x01FD3670, "cmd_functions", pcmd_functions),
	GLOBALVAR_LINK(0x02699D60, "cmd_text", pcmd_text),
	GLOBALVAR_LINK(0x02699D84, "cmd_alias", pcmd_alias),
	GLOBALVAR_LINK(0x01FE501C, "cvar_vars", pcvar_vars),
	GLOBALVAR_LINK(0x026995E4, "loadcache", ploadcache),
	GLOBALVAR_LINK(0x02699704, "loadbuf", ploadbuf),
	GLOBALVAR_LINK(0x02699D40, "loadsize", ploadsize),
	GLOBALVAR_LINK(0x01FEC4C8, "g_fallbackLocalizationFiles", pg_fallbackLocalizationFiles),
	GLOBALVAR_LINK(0x01FEC4E0, "s_pBaseDir", ps_pBaseDir),
	GLOBALVAR_LINK(0x01FEC6E8, "bLowViolenceBuild", pbLowViolenceBuild),
	GLOBALVAR_LINK(0x01FEC6EC, "g_pFileSystem", pg_pFileSystem),
	GLOBALVAR_LINK(0x01FEC6E0, "g_pFileSystemModule", pg_pFileSystemModule),
	GLOBALVAR_LINK(0x01FEC6E4, "g_FileSystemFactory", pg_FileSystemFactory),
	GLOBALVAR_LINK(0x01E4AAC0, "g_hfind", pg_hfind),
	GLOBALVAR_LINK(0x01E4AAC8, "g_engfuncsExportedToDlls", pg_engfuncsExportedToDlls),
	GLOBALVAR_LINK(0x021C2260, "gszDisconnectReason", pgszDisconnectReason),
	GLOBALVAR_LINK(0x021C27C0, "gszExtendedDisconnectReason", pgszExtendedDisconnectReason),
	GLOBALVAR_LINK(0x02090C64, "gfExtendedError", pgfExtendedError),
	GLOBALVAR_LINK(0x021C28F8, "g_bIsDedicatedServer", pg_bIsDedicatedServer),
	GLOBALVAR_LINK(0x021C2248, "giSubState", pgiSubState),
	GLOBALVAR_LINK(0x021C2448, "giActive", pgiActive),
	GLOBALVAR_LINK(0x02090C60, "giStateInfo", pgiStateInfo),
	GLOBALVAR_LINK(0x021C2380, "gEntityInterface", pgEntityInterface),
	GLOBALVAR_LINK(0x021C28E0, "gNewDLLFunctions", pgNewDLLFunctions),
	GLOBALVAR_LINK(0x01FF0638, "g_modfuncs", pg_modfuncs),
	GLOBALVAR_LINK(0x021C2480, "g_rgextdll", pg_rgextdll),
	GLOBALVAR_LINK(0x02090C94, "g_iextdllMac", pg_iextdllMac),
	GLOBALVAR_LINK(0x021C2900, "gmodinfo", pgmodinfo),
	GLOBALVAR_LINK(0x02090C90, "gfBackground", pgfBackground),
	GLOBALVAR_LINK(0x02699454, "con_debuglog", pcon_debuglog),
	GLOBALVAR_LINK(0x02090C78, "g_bPrintingKeepAliveDots", pg_bPrintingKeepAliveDots),
	GLOBALVAR_LINK(0x02090C80, "Launcher_ConsolePrintf", pLauncher_ConsolePrintf),
	GLOBALVAR_LINK(0x01E42EC4, "registry", pregistry),
	GLOBALVAR_LINK(0x0239B740, "outputbuf", poutputbuf),
	GLOBALVAR_LINK(0x021CAB94, "sv_redirected", psv_redirected),
	GLOBALVAR_LINK(0x021CAB80, "sv_redirectto", psv_redirectto),
	GLOBALVAR_LINK(0x01E464A0, "sv_rcon_minfailures", psv_rcon_minfailures),
	GLOBALVAR_LINK(0x01E464CC, "sv_rcon_maxfailures", psv_rcon_maxfailures),
	GLOBALVAR_LINK(0x01E464FC, "sv_rcon_minfailuretime", psv_rcon_minfailuretime),
	GLOBALVAR_LINK(0x01E46528, "sv_rcon_banpenalty", psv_rcon_banpenalty),
	GLOBALVAR_LINK(0x0208E828, "g_rgRconFailures", pg_rgRconFailures),
	GLOBALVAR_LINK(0x01E42FF8, "scr_downloading", pscr_downloading),

#ifndef REHLDS_FIXES
	GLOBALVAR_LINK(0x0208F7B8, "g_bCS_CZ_Flags_Initialized", pg_bCS_CZ_Flags_Initialized),
	GLOBALVAR_LINK(0x0208F7AC, "g_bIsCZero", pg_bIsCZero),
	GLOBALVAR_LINK(0x0208F7B4, "g_bIsCZeroRitual", pg_bIsCZeroRitual),
	GLOBALVAR_LINK(0x0208F7B0, "g_bIsTerrorStrike", pg_bIsTerrorStrike),
	GLOBALVAR_LINK(0x0208F7BC, "g_bIsTFC", pg_bIsTFC),
	GLOBALVAR_LINK(0x0208F7C0, "g_bIsHL1", pg_bIsHL1),
	GLOBALVAR_LINK(0x0208F7A8, "g_bIsCStrike", pg_bIsCStrike),
#endif

	GLOBALVAR_LINK(0x01FD3E80, "gPAS", pgPAS),
	GLOBALVAR_LINK(0x01FD3E84, "gPVS", pgPVS),
	GLOBALVAR_LINK(0x01FD3A78, "gPVSRowBytes", pgPVSRowBytes),
	GLOBALVAR_LINK(0x01FD3A80, "mod_novis", pmod_novis),
	GLOBALVAR_LINK(0x0229AE84, "fatbytes", pfatbytes),
	GLOBALVAR_LINK(0x0229B2C0, "fatpvs", pfatpvs),
	GLOBALVAR_LINK(0x0229B2A4, "fatpasbytes", pfatpasbytes),
	GLOBALVAR_LINK(0x0229AEA0, "fatpas", pfatpas),
	GLOBALVAR_LINK(0x0208FAB8, "s_Steam3Server", ps_Steam3Server),
	GLOBALVAR_LINK(0x0208FA58, "s_Steam3Client", ps_Steam3Client),
	GLOBALVAR_LINK(0x01E39F14, "sys_ticrate", psys_ticrate),
	GLOBALVAR_LINK(0x01E39F3C, "sys_timescale", psys_timescale),
	GLOBALVAR_LINK(0x01E39F60, "fps_max", pfps_max),
	GLOBALVAR_LINK(0x01E39F84, "host_killtime", phost_killtime),
	GLOBALVAR_LINK(0x01E39FA4, "sv_stats", psv_stats),
	GLOBALVAR_LINK(0x01E39FC8, "fps_override", pfps_override),
	GLOBALVAR_LINK(0x01E3A01C, "host_framerate", phost_framerate),
	GLOBALVAR_LINK(0x01E3A128, "pausable", ppausable),
	GLOBALVAR_LINK(0x01E43F6C, "suitvolume", psuitvolume),
	GLOBALVAR_LINK(0x0208FAC0, "truepositions", ptruepositions),
	GLOBALVAR_LINK(0x021C2B44, "sv_player", psv_player),
	GLOBALVAR_LINK(0x01E4A328, "clcommands", pclcommands),
	GLOBALVAR_LINK(0x0209064C, "g_balreadymoved", pg_balreadymoved),
	GLOBALVAR_LINK(0x01E4A658, "sv_clcfuncs", psv_clcfuncs),
	GLOBALVAR_LINK(0x020905C0, "s_LastFullUpdate", ps_LastFullUpdate),
	GLOBALVAR_LINK(0x01E4A3E0, "sv_edgefriction", psv_edgefriction),
	GLOBALVAR_LINK(0x01E4A404, "sv_maxspeed", psv_maxspeed),
	GLOBALVAR_LINK(0x01E4A42C, "sv_accelerate", psv_accelerate),
	GLOBALVAR_LINK(0x01E4A3BC, "sv_footsteps", psv_footsteps),
	GLOBALVAR_LINK(0x01E4A454, "sv_rollspeed", psv_rollspeed),
	GLOBALVAR_LINK(0x01E4A47C, "sv_rollangle", psv_rollangle),
	GLOBALVAR_LINK(0x01E4A49C, "sv_unlag", psv_unlag),
	GLOBALVAR_LINK(0x01E4A4C0, "sv_maxunlag", psv_maxunlag),
	GLOBALVAR_LINK(0x01E4A4E8, "sv_unlagpush", psv_unlagpush),
	GLOBALVAR_LINK(0x01E4A510, "sv_unlagsamples", psv_unlagsamples),
	GLOBALVAR_LINK(0x01E4A398, "mp_consistency", pmp_consistency),
	GLOBALVAR_LINK(0x01E4A568, "sv_voiceenable", psv_voiceenable),
	GLOBALVAR_LINK(0x02090648, "nofind", pnofind),
	GLOBALVAR_LINK(0x01E3F7F4, "pm_showclip", ppm_showclip),
	GLOBALVAR_LINK(0x01E3F808, "player_mins", pplayer_mins),
	GLOBALVAR_LINK(0x01E3F838, "player_maxs", pplayer_maxs),
	GLOBALVAR_LINK(0x02004308, "pmove", ppmove),
	GLOBALVAR_LINK(0x025E3F00, "movevars", pmovevars),
	GLOBALVAR_LINK(0x01FED51C, "vec3_origin", pvec3_origin),
	GLOBALVAR_LINK(0x02004980, "gMsgData", pgMsgData),
	GLOBALVAR_LINK(0x01E3F9AC, "gMsgBuffer", pgMsgBuffer),
	GLOBALVAR_LINK(0x02004B80, "gMsgEntity", pgMsgEntity),
	GLOBALVAR_LINK(0x02004B84, "gMsgDest", pgMsgDest),
	GLOBALVAR_LINK(0x02004B88, "gMsgType", pgMsgType),
	GLOBALVAR_LINK(0x02004B8C, "gMsgStarted", pgMsgStarted),
	GLOBALVAR_LINK(0x020043A8, "gMsgOrigin", pgMsgOrigin),
	GLOBALVAR_LINK(0x02004B90, "idum", pidum),
	GLOBALVAR_LINK(0x02004B9C, "g_groupop", pg_groupop),
	GLOBALVAR_LINK(0x02004B98, "g_groupmask", pg_groupmask),
	GLOBALVAR_LINK(0x025E39E0, "checkpvs", pcheckpvs),
	GLOBALVAR_LINK(0x025E3DE0, "c_invis", pc_invis),
	GLOBALVAR_LINK(0x025E39CC, "c_notvis", pc_notvis),
	GLOBALVAR_LINK(0x0208F7E0, "vec_origin", pvec_origin),
	GLOBALVAR_LINK(0x025DFD94, "r_visframecount", pr_visframecount),
	GLOBALVAR_LINK(0x02052A48, "cache_hull_hitgroup", pcache_hull_hitgroup),
	GLOBALVAR_LINK(0x0206FB60, "cache_hull", pcache_hull),
	GLOBALVAR_LINK(0x0201CDC8, "cache_planes", pcache_planes),
	GLOBALVAR_LINK(0x0206FB50, "pstudiohdr", ppstudiohdr),
	GLOBALVAR_LINK(0x0200A398, "studio_hull", pstudio_hull),
	GLOBALVAR_LINK(0x02052C48, "studio_hull_hitgroup", pstudio_hull_hitgroup),
	GLOBALVAR_LINK(0x0200C998, "studio_planes", pstudio_planes),
	GLOBALVAR_LINK(0x02052E48, "studio_clipnodes", pstudio_clipnodes),
	GLOBALVAR_LINK(0x0206E908, "rgStudioCache", prgStudioCache),
	GLOBALVAR_LINK(0x0206FB54, "r_cachecurrent", pr_cachecurrent),
	GLOBALVAR_LINK(0x02016DC4, "nCurrentHull", pnCurrentHull),
	GLOBALVAR_LINK(0x0200A384, "nCurrentPlane", pnCurrentPlane),
	GLOBALVAR_LINK(0x025DD0E0, "bonetransform", pbonetransform),
	GLOBALVAR_LINK(0x01E42284, "r_cachestudio", pr_cachestudio),
	GLOBALVAR_LINK(0x01E424E8, "g_pSvBlendingAPI", pg_pSvBlendingAPI),
	GLOBALVAR_LINK(0x01E424E0, "svBlending", psvBlending),
	GLOBALVAR_LINK(0x01E424D0, "server_studio_api", pserver_studio_api),
	GLOBALVAR_LINK(0x025DEEE0, "rotationmatrix", protationmatrix),
	GLOBALVAR_LINK(0x021B1568, "box_hull", pbox_hull, 1),
	GLOBALVAR_LINK(0x021B1540, "beam_hull", pbeam_hull),
	GLOBALVAR_LINK(0x021B1590, "box_clipnodes", pbox_clipnodes, 1),
	GLOBALVAR_LINK(0x021B15C0, "box_planes", pbox_planes, 1),
	GLOBALVAR_LINK(0x021B14C8, "beam_planes", pbeam_planes),
	GLOBALVAR_LINK(0x021B6240, "sv_areanodes", psv_areanodes),
	GLOBALVAR_LINK(0x021B6640, "sv_numareanodes", psv_numareanodes),
	GLOBALVAR_LINK(0x02004310, "g_contentsresult", pg_contentsresult),
	GLOBALVAR_LINK(0x025E3E00, "box_hull", pbox_hull_0, 2),
	GLOBALVAR_LINK(0x025E3E40, "box_clipnodes", pbox_clipnodes_0, 2),
	GLOBALVAR_LINK(0x025E3E80, "box_planes", pbox_planes_0, 2),
	GLOBALVAR_LINK(0x01E48EF0, "sv_maxvelocity", psv_maxvelocity),
	GLOBALVAR_LINK(0x01E48EC4, "sv_gravity", psv_gravity),
	GLOBALVAR_LINK(0x01E48F34, "sv_bounce", psv_bounce),
	GLOBALVAR_LINK(0x01E48F14, "sv_stepsize", psv_stepsize),
	GLOBALVAR_LINK(0x01E48E78, "sv_friction", psv_friction),
	GLOBALVAR_LINK(0x01E48EA0, "sv_stopspeed", psv_stopspeed),
	GLOBALVAR_LINK(0x021C2B48, "g_moved_from", pg_moved_from),
	GLOBALVAR_LINK(0x021C2B4C, "sv_numareanodes", pg_moved_edict),
	GLOBALVAR_LINK(0x021C2B50, "c_yes", pc_yes),
	GLOBALVAR_LINK(0x021C2B54, "c_no", pc_no),
	GLOBALVAR_LINK(0x020042AC, "net_thread_initialized", pnet_thread_initialized),
	GLOBALVAR_LINK(0x01E3E8DC, "net_address", pnet_address),
	GLOBALVAR_LINK(0x01E3E900, "ipname", pipname),
	GLOBALVAR_LINK(0x01E3E968, "defport", pdefport),
	GLOBALVAR_LINK(0x01E3E98C, "ip_clientport", pip_clientport),
	GLOBALVAR_LINK(0x01E3E9B4, "clientport", pclientport),
	GLOBALVAR_LINK(0x01E3EB34, "net_sleepforever", pnet_sleepforever),
	GLOBALVAR_LINK(0x01FF2B60, "loopbacks", ploopbacks),
	GLOBALVAR_LINK(0x01FF2AE8, "g_pLagData", pg_pLagData),
	GLOBALVAR_LINK(0x020042D8, "gFakeLag", pgFakeLag),
	GLOBALVAR_LINK(0x020042FC, "net_configured", pnet_configured),
	GLOBALVAR_LINK(0x025F4020, "net_message", pnet_message),
	GLOBALVAR_LINK(0x025F4040, "net_local_adr", pnet_local_adr),
	GLOBALVAR_LINK(0x02604060, "net_from", pnet_from),
	GLOBALVAR_LINK(0x020042BC, "noip", pnoip),
	GLOBALVAR_LINK(0x01E3E9D8, "clockwindow", pclockwindow),
	GLOBALVAR_LINK(0x020042B0, "use_thread", puse_thread),
	GLOBALVAR_LINK(0x025F4000, "in_message", pin_message),
	GLOBALVAR_LINK(0x025F3FC0, "in_from", pin_from),
	GLOBALVAR_LINK(0x020042C0, "ip_sockets", pip_sockets),
	GLOBALVAR_LINK(0x01E3E924, "iphostport", piphostport),
	GLOBALVAR_LINK(0x01E3E944, "hostport", phostport),
	GLOBALVAR_LINK(0x01E3EA04, "multicastport", pmulticastport),
	GLOBALVAR_LINK(0x01E3EA70, "fakelag", pfakelag),
	GLOBALVAR_LINK(0x01E3EA94, "fakeloss", pfakeloss),
	GLOBALVAR_LINK(0x01E3EAB4, "net_graph", pnet_graph),
	GLOBALVAR_LINK(0x01E3EADC, "net_graphwidth", pnet_graphwidth),
	GLOBALVAR_LINK(0x01E3EAFC, "net_scale", pnet_scale),
	GLOBALVAR_LINK(0x01E3EB20, "net_graphpos", pnet_graphpos),
	GLOBALVAR_LINK(0x025F4060, "net_message_buffer", pnet_message_buffer),
	GLOBALVAR_LINK(0x025E3FC0, "in_message_buf", pin_message_buf),

#ifdef _WIN32
	GLOBALVAR_LINK(0x025F3FE0, "net_local_ipx_adr", pnet_local_ipx_adr),
	GLOBALVAR_LINK(0x020042B8, "noipx", pnoipx),
	GLOBALVAR_LINK(0x01E3EA28, "ipx_hostport", pipx_hostport),
	GLOBALVAR_LINK(0x01E3EA50, "ipx_clientport", pipx_clientport),
	GLOBALVAR_LINK(0x020042CC, "ipx_sockets", pipx_sockets),
#endif // _WIN32

	GLOBALVAR_LINK(0x02604080, "gNetSplit", pgNetSplit),
	GLOBALVAR_LINK(0x020042E8, "messages", pmessages),
	GLOBALVAR_LINK(0x020042F4, "normalqueue", pnormalqueue),
	GLOBALVAR_LINK(0x02605040, "gDownloadFile", pgDownloadFile),
	GLOBALVAR_LINK(0x02605140, "net_drop", pnet_drop),
	GLOBALVAR_LINK(0x01E3E00C, "net_log", pnet_log),
	GLOBALVAR_LINK(0x01E3E034, "net_showpackets", pnet_showpackets),
	GLOBALVAR_LINK(0x01E3E058, "net_showdrop", pnet_showdrop),
	GLOBALVAR_LINK(0x01E3E080, "net_drawslider", pnet_drawslider),
	GLOBALVAR_LINK(0x01E3E0A4, "net_chokeloopback", pnet_chokeloopback),
	GLOBALVAR_LINK(0x01E3E0D8, "sv_filetransfercompression", psv_filetransfercompression),
	GLOBALVAR_LINK(0x01E3E110, "sv_filetransfermaxsize", psv_filetransfermaxsize),

#ifdef _WIN32
	GLOBALVAR_LINK(0x02090C88, "g_PerfCounterInitialized", pg_PerfCounterInitialized),
	GLOBALVAR_LINK(0x021C2460, "g_PerfCounterMutex", pg_PerfCounterMutex),
	GLOBALVAR_LINK(0x021C2454, "g_PerfCounterShiftRightAmount", pg_PerfCounterShiftRightAmount),
	GLOBALVAR_LINK(0x021C28C0, "g_PerfCounterSlice", pg_PerfCounterSlice),
	GLOBALVAR_LINK(0x02090C68, "g_CurrentTime", pg_CurrentTime),
	GLOBALVAR_LINK(0x02090C70, "g_StartTime", pg_StartTime),
	GLOBALVAR_LINK(0x01E4E98C, "g_FPUCW_Mask_Prec_64Bit", pg_FPUCW_Mask_Prec_64Bit),
	GLOBALVAR_LINK(0x01E4E990, "g_FPUCW_Mask_Prec_64Bit_2", pg_FPUCW_Mask_Prec_64Bit_2),
	GLOBALVAR_LINK(0x01E4E988, "g_FPUCW_Mask_ZeroDiv_OFlow", pg_FPUCW_Mask_Round_Trunc),
	GLOBALVAR_LINK(0x01E4E984, "g_FPUCW_Mask_OFlow", pg_FPUCW_Mask_Round_Up),
	GLOBALVAR_LINK(0x021C2244, "g_WinNTOrHigher", pg_WinNTOrHigher),
	GLOBALVAR_LINK(0x020914E4, "g_bIsWin95", pg_bIsWin95),
	GLOBALVAR_LINK(0x020914E8, "g_bIsWin98", pg_bIsWin98),
#endif // _WIN32

	GLOBALVAR_LINK(0x02605180, "loadmodel", ploadmodel),
	GLOBALVAR_LINK(0x02605160, "loadname", ploadname),
	GLOBALVAR_LINK(0x026051A0, "mod_known", pmod_known),
	GLOBALVAR_LINK(0x02605144, "mod_numknown", pmod_numknown),
	GLOBALVAR_LINK(0x026671A0, "mod_base", pmod_base),
	GLOBALVAR_LINK(0x01FF062C, "wadpath", pwadpath),
	GLOBALVAR_LINK(0x01FF0630, "tested", ptested),
	GLOBALVAR_LINK(0x01FF0634, "ad_enabled", pad_enabled),
	GLOBALVAR_LINK(0x026671C0, "ad_wad", pad_wad),
	GLOBALVAR_LINK(0x01FED528, "mod_known_info", pmod_known_info),
	GLOBALVAR_LINK(0x02095B48, "lumpinfo", plumpinfo),
	GLOBALVAR_LINK(0x02095B4C, "nTexLumps", pnTexLumps),
	GLOBALVAR_LINK(0x021C2040, "texfiles", ptexfiles),
	GLOBALVAR_LINK(0x02095B44, "nTexFiles", pnTexFiles),
	GLOBALVAR_LINK(0x021B7660, "texgammatable", ptexgammatable),
	GLOBALVAR_LINK(0x025DFCE4, "r_notexture_mip", pr_notexture_mip),
	GLOBALVAR_LINK(0x021C2020, "nummiptex", pnummiptex),
	GLOBALVAR_LINK(0x021BA020, "miptex", pmiptex),
	GLOBALVAR_LINK(0x01E41A30, "r_wadtextures", pr_wadtextures),
	GLOBALVAR_LINK(0x01E42270, "r_dointerp", pr_dointerp),
	GLOBALVAR_LINK(0x025DFE40, "r_origin", pr_origin),
	GLOBALVAR_LINK(0x01E415BC, "r_pixbytes", pr_pixbytes),
	GLOBALVAR_LINK(0x01E43090, "gl_vsync", pgl_vsync),
	GLOBALVAR_LINK(0x023FF0D0, "scr_con_current", pscr_con_current),
	GLOBALVAR_LINK(0x0208F7EC, "g_ServerRemoteAccess", pg_ServerRemoteAccess),
	GLOBALVAR_LINK(0x01FED1E8, "cpuPercent", pcpuPercent),
	GLOBALVAR_LINK(0x01FED1D4, "startTime", pstartTime),
	GLOBALVAR_LINK(0x01FED1E4, "g_bMajorMapChange", pg_bMajorMapChange),
	GLOBALVAR_LINK(0x01E3A97C, "voice_recordtofile", pvoice_recordtofile),
	GLOBALVAR_LINK(0x01E3A9A8, "voice_inputfromfile", pvoice_inputfromfile),
	GLOBALVAR_LINK(0x01E3B250, "gTitleComments", pgTitleComments),
	GLOBALVAR_LINK(0x01E3A9D8, "gGameHeaderDescription", pgGameHeaderDescription),
	GLOBALVAR_LINK(0x01E3AAA0, "gSaveHeaderDescription", pgSaveHeaderDescription),
	GLOBALVAR_LINK(0x01E3ABB0, "gAdjacencyDescription", pgAdjacencyDescription),
	GLOBALVAR_LINK(0x01E3AC20, "gEntityTableDescription", pgEntityTableDescription),
	GLOBALVAR_LINK(0x01E3AC80, "gLightstyleDescription", pgLightstyleDescription),
	GLOBALVAR_LINK(0x01E3ACB0, "gHostMap", pgHostMap),
	GLOBALVAR_LINK(0x01FED208, "g_iQuitCommandIssued", pg_iQuitCommandIssued),
	GLOBALVAR_LINK(0x01FED20C, "g_pPostRestartCmdLineArgs", pg_pPostRestartCmdLineArgs),
	GLOBALVAR_LINK(0x021CABA0, "g_rg_sv_challenges", pg_rg_sv_challenges),
	GLOBALVAR_LINK(0x0239BCC0, "g_svdeltacallback", pg_svdeltacallback),
	GLOBALVAR_LINK(0x01FED4D0, "_ZL11rateChecker", prateChecker),
	GLOBALVAR_LINK(0x01FEC7F4, "gp_hpak_queue", pgp_hpak_queue),
	GLOBALVAR_LINK(0x01FEC7F8, "hash_pack_dir", phash_pack_dir),
	GLOBALVAR_LINK(0x0266AFB0, "hash_pack_header", phash_pack_header),
	GLOBALVAR_LINK(0x020891D8, "firstLog", pfirstLog),
	GLOBALVAR_LINK(0x01E4C6C4, "game", pgame),
	GLOBALVAR_LINK(0x01E4BB44, "eng", peng),
	GLOBALVAR_LINK(0x021B1468, "wads", pwads),
	GLOBALVAR_LINK(0x01FF06B0, "g_module", pg_module),
	GLOBALVAR_LINK(0x020914E0, "dedicated", pdedicated),
	GLOBALVAR_LINK(0x020914E4, "g_bIsWin95", pg_bIsWin95),
	GLOBALVAR_LINK(0x020914E8, "g_bIsWin98", pg_bIsWin98),
	GLOBALVAR_LINK(0x020914F8, "g_flLastSteamProgressUpdateTime", pg_flLastSteamProgressUpdateTime),
	GLOBALVAR_LINK(0x01E4B3E0, "szCommonPreloads", pszCommonPreloads),
	GLOBALVAR_LINK(0x01E4B3F0, "szReslistsBaseDir", pszReslistsBaseDir),
	GLOBALVAR_LINK(0x01E4B3FC, "szReslistsExt", pszReslistsExt),
	GLOBALVAR_LINK(0x02095C98, "g_InitTracker", pg_InitTracker),
	GLOBALVAR_LINK(0x020935A0, "gSystemWrapper", pgSystemWrapper),

#ifndef _WIN32
	//GLOBALVAR_LINK(0x0, "gHasMMXTechnology", pgHasMMXTechnology),
#endif

#endif // Data_References_region

	{ NULL, NULL, NULL },
};
