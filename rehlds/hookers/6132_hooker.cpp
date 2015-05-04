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

template<typename MFUNC>
size_t mfunc_ptr_cast(MFUNC f) {
	auto x = f;
	size_t addr = (size_t &)x;
	return addr;
}

// Offset where engine assumed be loaded to ajust hooks offsets. NULL for the Linux to trigger symbols searching.
#ifdef _WIN32
extern const size_t g_BaseOffset = 0x01D00000;
#else
extern const size_t g_BaseOffset = NULL;
#endif


//#define Common_MSG_region
//#define Common_SZ_region
//#define Common_COM_region
//#define Delta_region
//#define Main_region
//#define Zone_region
//#define FileSystem_Internal_region
//#define FileSystem_region
//#define Unicode_StrTools_region
//#define Cmd_region
//#define Cvar_region
//#define Info_region
//#define SysDll_region
//#define Sys_Dll2_region
//#define CModel_region
//#define Model_region
//#define Sv_Log_region
//#define Cl_Null_region
//#define Snd_Null_region
//#define Sv_Steam3_region
//#define Host_region
//#define Host_Cmd_region
//#define Pmove_region
//#define Pmovetst_region
//#define Pr_Edict_region
//#define Pr_Cmds_region
//#define Mathlib_region
//#define World_region
//#define Sv_Phys_region
//#define Sv_Move_region
//#define Sv_pmove_region
//#define R_Studio_region
//#define Net_ws_region
//#define Net_chan_region
//#define Hashpak_region
//#define Sv_user_region
//#define Wad_region
//#define Textures_region
//#define Tmessage_region
//#define Vid_null_region
//#define L_studio_region
//#define Crc_region
//#define Sv_RemoteAccess_region
//#define Com_custom
//#define Sv_upld
//#define Decals
//#define IpratelimitWrapper_region
//#define Sys_engine
//#define Sys_linuxwind

//#define Function_References_region
//#define Data_References_region


void* malloc_wrapper(size_t size) {
	void* res = malloc(size);
	//Rehlds_Debug_logAlloc(size, res);
	return res;
}

void* realloc_wrapper(void* orig, size_t newSize) {
	void* res = realloc(orig, newSize);
	//Rehlds_Debug_logRealloc(newSize, orig, res);
	return res;
}

void free_wrapper(void* mem) {
	//Rehlds_Debug_logFree(mem);
	free(mem);
}

void* calloc_wrapper(size_t count, size_t size) {
	void* res = calloc(count, size);
	//Rehlds_Debug_logAlloc(size * count, res);
	return res;
}

void* __nh_malloc_wrapper(size_t sz, int unk) {
	void* res = malloc(sz);
	//Rehlds_Debug_logAlloc(sz, res);
	return res;
}


FunctionHook g_FunctionHooks[] =
{
	// DO NOT DISABLE, other functions depends on memory allocation routines
#ifndef Mem_region

	{ 0x01D50AB0, "Mem_Malloc", (size_t)&Mem_Malloc },
	{ 0x01D50AD0, "Mem_ZeroMalloc", (size_t)&Mem_ZeroMalloc },
	{ 0x01D50B00, "Mem_Realloc", (size_t)&Mem_Realloc },
	{ 0x01D50B20, "Mem_Calloc", (size_t)&Mem_Calloc },
	{ 0x01D50B40, "Mem_Strdup", (size_t)&Mem_Strdup },
	{ 0x01D50B60, "Mem_Free", (size_t)&Mem_Free },

#ifdef _WIN32
	{ 0x01DFE945, "_malloca", (size_t)&malloc_wrapper },
	{ 0x01DFEA7F, "realloc", (size_t)&realloc_wrapper },
	{ 0x01DFE35D, "_frea", (size_t)&free_wrapper },
	{ 0x01DFF016, "calloc", (size_t)&calloc_wrapper },
	{ 0x01DFE957, "__nh_malloc", (size_t)&__nh_malloc_wrapper },

#endif //_WIN32

#endif // Mem_region

#ifndef Common_MSG_region

#ifdef Q_functions

	//{ 0x, "Q_memset", (size_t)&Q_memset },
	//{ 0x, "Q_memcpy", (size_t)&Q_memcpy },
	//{ 0x, "Q_memcmp", (size_t)&Q_memcmp },
	{ 0x01D28AF0, "Q_strcpy", (size_t)&Q_strcpy },
	//{ 0x, "Q_strncpy", (size_t)&Q_strncpy },
	{ 0x01D28B50, "Q_strlen", (size_t)&Q_strlen },
	//{ 0x, "Q_strrchr", (size_t)&Q_strrchr },
	//{ 0x, "Q_strcat", (size_t)&Q_strcat },
	//{ 0x, "Q_strcmp", (size_t)&Q_strcmp },
	//{ 0x, "Q_strncmp", (size_t)&Q_strncmp },
	//{ 0x, "Q_strncasecmp", (size_t)&Q_strncasecmp },
	//{ 0x, "Q_strcasecmp", (size_t)&Q_strcasecmp },
	//{ 0x, "Q_stricmp", (size_t)&Q_stricmp },
	//{ 0x, "Q_strnicmp", (size_t)&Q_strnicmp },
	//{ 0x, "Q_atoi", (size_t)&Q_atoi },
	//{ 0x, "Q_atof", (size_t)&Q_atof },
	//{ 0x, "Q_strlwr", (size_t)&Q_strlwr },
	//{ 0x, "Q_FileNameCmp", (size_t)&Q_FileNameCmp },
	//{ 0x, "Q_strstr", (size_t)&Q_strstr },
	//{ 0x, "Q_strtoull", (size_t)&Q_strtoull },

#endif // Q_functions

	{ 0x01D29290, "MSG_WriteChar", (size_t)&MSG_WriteChar },
	{ 0x01D292B0, "MSG_WriteByte", (size_t)&MSG_WriteByte },
	{ 0x01D292D0, "MSG_WriteShort", (size_t)&MSG_WriteShort },
	{ 0x01D292F0, "MSG_WriteWord", (size_t)&MSG_WriteWord },
	{ 0x01D29310, "MSG_WriteLong", (size_t)&MSG_WriteLong },
	{ 0x01D29340, "MSG_WriteFloat", (size_t)&MSG_WriteFloat },
	{ 0x01D29370, "MSG_WriteString", (size_t)&MSG_WriteString },
	{ 0x01D293B0, "MSG_WriteBuf", (size_t)&MSG_WriteBuf },
	{ 0x01D293D0, "MSG_WriteAngle", (size_t)&MSG_WriteAngle },
	{ 0x01D29420, "MSG_WriteHiresAngle", (size_t)&MSG_WriteHiresAngle },
	{ 0x01D29470, "MSG_WriteUsercmd", (size_t)&MSG_WriteUsercmd },
	{ 0x01D294D0, "MSG_WriteOneBit", (size_t)&MSG_WriteOneBit },
	{ 0x01D29550, "MSG_StartBitWriting", (size_t)&MSG_StartBitWriting },
	{ 0x01D29580, "MSG_IsBitWriting", (size_t)&MSG_IsBitWriting },	// NOXREF
	{ 0x01D29590, "MSG_EndBitWriting", (size_t)&MSG_EndBitWriting },
	{ 0x01D295E0, "MSG_WriteBits", (size_t)&MSG_WriteBits },
	{ 0x01D29700, "MSG_WriteSBits", (size_t)&MSG_WriteSBits },
	{ 0x01D29750, "MSG_WriteBitString", (size_t)&MSG_WriteBitString },
	{ 0x01D29790, "MSG_WriteBitData", (size_t)&MSG_WriteBitData },
	{ 0x01D297C0, "MSG_WriteBitAngle", (size_t)&MSG_WriteBitAngle },
	{ 0x01D29850, "MSG_ReadBitAngle", (size_t)&MSG_ReadBitAngle },
	{ 0x01D298A0, "MSG_CurrentBit", (size_t)&MSG_CurrentBit },
	{ 0x01D298D0, "MSG_IsBitReading", (size_t)&MSG_IsBitReading },	// NOXREF
	{ 0x01D298E0, "MSG_StartBitReading", (size_t)&MSG_StartBitReading },
	{ 0x01D29930, "MSG_EndBitReading", (size_t)&MSG_EndBitReading },
	{ 0x01D29970, "MSG_ReadOneBit", (size_t)&MSG_ReadOneBit },
	{ 0x01D29A00, "MSG_ReadBits", (size_t)&MSG_ReadBits },
	{ 0x01D29B00, "MSG_PeekBits", (size_t)&MSG_PeekBits },	// NOXREF
	{ 0x01D29B40, "MSG_ReadSBits", (size_t)&MSG_ReadSBits },
	{ 0x01D29B70, "MSG_ReadBitString", (size_t)&MSG_ReadBitString },	// NOXREF
	{ 0x01D29BB0, "MSG_ReadBitData", (size_t)&MSG_ReadBitData },
	{ 0x01D29BE0, "MSG_ReadBitCoord", (size_t)&MSG_ReadBitCoord },	// NOXREF
	{ 0x01D29C70, "MSG_WriteBitCoord", (size_t)&MSG_WriteBitCoord },
	{ 0x01D29D00, "MSG_ReadBitVec3Coord", (size_t)&MSG_ReadBitVec3Coord },	// NOXREF
	{ 0x01D29D50, "MSG_WriteBitVec3Coord", (size_t)&MSG_WriteBitVec3Coord },
	{ 0x01D29E30, "MSG_ReadCoord", (size_t)&MSG_ReadCoord },	// NOXREF
	{ 0x01D29E60, "MSG_WriteCoord", (size_t)&MSG_WriteCoord },
	{ 0x01D29E80, "MSG_ReadVec3Coord", (size_t)&MSG_ReadVec3Coord },	// NOXREF
	{ 0x01D29EC0, "MSG_WriteVec3Coord", (size_t)&MSG_WriteVec3Coord },	// NOXREF
	{ 0x01D29F00, "MSG_BeginReading", (size_t)&MSG_BeginReading },
	{ 0x01D29F10, "MSG_ReadChar", (size_t)&MSG_ReadChar },
	{ 0x01D29F50, "MSG_ReadByte", (size_t)&MSG_ReadByte },
	{ 0x01D29F90, "MSG_ReadShort", (size_t)&MSG_ReadShort },
	{ 0x01D29FE0, "MSG_ReadWord", (size_t)&MSG_ReadWord },	// NOXREF
	{ 0x01D2A030, "MSG_ReadLong", (size_t)&MSG_ReadLong },
	{ 0x01D2A090, "MSG_ReadFloat", (size_t)&MSG_ReadFloat },
	{ 0x01D2A0E0, "MSG_ReadBuf", (size_t)&MSG_ReadBuf },
	{ 0x01D2A130, "MSG_ReadString", (size_t)&MSG_ReadString },
	{ 0x01D2A170, "MSG_ReadStringLine", (size_t)&MSG_ReadStringLine },
	{ 0x01D2A1B0, "MSG_ReadAngle", (size_t)&MSG_ReadAngle },	// NOXREF
	{ 0x01D2A1E0, "MSG_ReadHiresAngle", (size_t)&MSG_ReadHiresAngle },	// NOXREF
	{ 0x01D2A210, "MSG_ReadUsercmd", (size_t)&MSG_ReadUsercmd },

#endif // Common_MSG_region

#ifndef Common_SZ_region

	{ 0x01D2A260, "SZ_Alloc", (size_t)&SZ_Alloc },
	{ 0x01D2A2A0, "SZ_Clear", (size_t)&SZ_Clear },
	{ 0x01D2A2C0, "SZ_GetSpace", (size_t)&SZ_GetSpace },
	{ 0x01D2A380, "SZ_Write", (size_t)&SZ_Write },
	{ 0x01D2A3B0, "SZ_Print", (size_t)&SZ_Print },

#endif // Common_SZ_region

#ifndef Common_COM_region

	{ 0x01D08E50, "build_number", (size_t)&build_number },
	{ 0x01D289E0, "Info_Serverinfo", (size_t)&Info_Serverinfo },
	{ 0x01D28FF0, "COM_Nibble", (size_t)&COM_Nibble },
	{ 0x01D29030, "COM_HexConvert", (size_t)&COM_HexConvert },
	//{ 0x01D29080, "COM_BinPrintf", (size_t)&COM_BinPrintf },	// NOXREF
	{ 0x01D29100, "COM_ExplainDisconnection", (size_t)&COM_ExplainDisconnection },
	{ 0x01D29170, "COM_ExtendedExplainDisconnection", (size_t)&COM_ExtendedExplainDisconnection },	// NOXREF
	{ 0x01D2A410, "COM_SkipPath", (size_t)&COM_SkipPath },	// NOXREF
	{ 0x01D2A440, "COM_StripExtension", (size_t)&COM_StripExtension },
	{ 0x01D2A4A0, "COM_FileExtension", (size_t)&COM_FileExtension },
	{ 0x01D2A4F0, "COM_FileBase", (size_t)&COM_FileBase },
	{ 0x01D2A580, "COM_DefaultExtension", (size_t)&COM_DefaultExtension },
	{ 0x01D2A5D0, "COM_UngetToken", (size_t)&COM_UngetToken },
	{ 0x01D2A5E0, "COM_Parse", (size_t)&COM_Parse },
	{ 0x01D2A730, "COM_ParseLine", (size_t)&COM_ParseLine },
	{ 0x01D2A7A0, "COM_TokenWaiting", (size_t)&COM_TokenWaiting },	// NOXREF
	{ 0x01D2A830, "COM_CheckParm", (size_t)&COM_CheckParm },
	{ 0x01D2A880, "COM_InitArgv", (size_t)&COM_InitArgv },
	{ 0x01D2A960, "COM_Init", (size_t)&COM_Init },
	{ 0x01D2AA00, "va", (size_t)&va },
	{ 0x01D2AA40, "vstr", (size_t)&vstr }, // NOXREF
	{ 0x01D2AAA0, "memsearch", (size_t)&memsearch }, // NOXREF
	{ 0x01D2AAD0, "COM_WriteFile", (size_t)&COM_WriteFile }, // NOXREF
	{ 0x01D2AB70, "COM_FixSlashes", (size_t)&COM_FixSlashes },
	{ 0x01D2AB90, "COM_CreatePath", (size_t)&COM_CreatePath },
	{ 0x01D2ABD0, "COM_CopyFile", (size_t)&COM_CopyFile }, // NOXREF
	{ 0x01D2AC70, "COM_ExpandFilename", (size_t)&COM_ExpandFilename }, // NOXREF
	{ 0x01D2ACD0, "COM_FileSize", (size_t)&COM_FileSize },
	{ 0x01D2AD10, "COM_LoadFile", (size_t)&COM_LoadFile },
	{ 0x01D2AE70, "COM_FreeFile", (size_t)&COM_FreeFile },
	{ 0x01D2AEA0, "COM_CopyFileChunk", (size_t)&COM_CopyFileChunk },
	//{ 0x01D2AF40, "COM_LoadFileLimit", (size_t)&COM_LoadFileLimit },	// NOXREF
	{ 0x01D2B020, "COM_LoadHunkFile", (size_t)&COM_LoadHunkFile },
	{ 0x01D2B040, "COM_LoadTempFile", (size_t)&COM_LoadTempFile },
	{ 0x01D2B060, "COM_LoadCacheFile", (size_t)&COM_LoadCacheFile },
	{ 0x01D2B080, "COM_LoadStackFile", (size_t)&COM_LoadStackFile },	// NOXREF
	{ 0x01D2B0B0, "COM_Shutdown", (size_t)&COM_Shutdown },
	{ 0x01D2B0C0, "COM_AddAppDirectory", (size_t)&COM_AddAppDirectory },	// NOXREF
	{ 0x01D2B0E0, "COM_AddDefaultDir", (size_t)&COM_AddDefaultDir },
	{ 0x01D2B100, "COM_StripTrailingSlash", (size_t)&COM_StripTrailingSlash },
	{ 0x01D2B130, "COM_ParseDirectoryFromCmd", (size_t)&COM_ParseDirectoryFromCmd },
	{ 0x01D2B1C0, "COM_SetupDirectories", (size_t)&COM_SetupDirectories },
	//{ 0x01D2B3D0, "COM_CheckPrintMap", (size_t)&COM_CheckPrintMap },	// Totally not exists on Linux (it is inlined), but present and used on Windows, anyway just commented it out
	{ 0x01D2B250, "COM_ListMaps", (size_t)&COM_ListMaps },
	{ 0x01D2B410, "COM_Log", (size_t)&COM_Log },
	{ 0x01D2B480, "COM_LoadFileForMe", (size_t)&COM_LoadFileForMe },
	{ 0x01D2B4A0, "COM_CompareFileTime", (size_t)&COM_CompareFileTime },
	{ 0x01D2B500, "COM_GetGameDir", (size_t)&COM_GetGameDir },
	{ 0x01D2B530, "COM_EntsForPlayerSlots", (size_t)&COM_EntsForPlayerSlots },
	{ 0x01D2B590, "COM_NormalizeAngles", (size_t)&COM_NormalizeAngles },
	{ 0x01D2B610, "COM_Munge", (size_t)&COM_Munge },
	{ 0x01D2B6A0, "COM_UnMunge", (size_t)&COM_UnMunge },
	{ 0x01D2B730, "COM_Munge2", (size_t)&COM_Munge2 },
	{ 0x01D2B7C0, "COM_UnMunge2", (size_t)&COM_UnMunge2 },
	{ 0x01D2B850, "COM_Munge3", (size_t)&COM_Munge3 },
	{ 0x01D2B8E0, "COM_UnMunge3", (size_t)&COM_UnMunge3 },	// NOXREF
	{ 0x01D2B970, "COM_GetApproxWavePlayLength", (size_t)&COM_GetApproxWavePlayLength },

#endif // Common_COM_region

#ifndef Delta_region

	{ 0x01D351D0, "DELTA_FindField", (size_t)&DELTA_FindField },
	{ 0x01D35240, "DELTA_FindFieldIndex", (size_t)&DELTA_FindFieldIndex },
	{ 0x01D352A0, "DELTA_SetField", (size_t)&DELTA_SetField },
	{ 0x01D352C0, "DELTA_UnsetField", (size_t)&DELTA_UnsetField },
	{ 0x01D352E0, "DELTA_SetFieldByIndex", (size_t)&DELTA_SetFieldByIndex },
	{ 0x01D35300, "DELTA_UnsetFieldByIndex", (size_t)&DELTA_UnsetFieldByIndex },
	{ 0x01D35320, "DELTA_ClearFlags", (size_t)&DELTA_ClearFlags },
	{ 0x01D35340, "DELTA_TestDelta", (size_t)&DELTA_TestDelta },
	{ 0x01D355B0, "DELTA_CountSendFields", (size_t)&DELTA_CountSendFields },
	{ 0x01D355E0, "DELTA_MarkSendFields", (size_t)&DELTA_MarkSendFields },
	{ 0x01D35830, "DELTA_SetSendFlagBits", (size_t)&DELTA_SetSendFlagBits },
	{ 0x01D358B0, "DELTA_WriteMarkedFields", (size_t)&DELTA_WriteMarkedFields },
	{ 0x01D35C10, "DELTA_CheckDelta", (size_t)&DELTA_CheckDelta },
	{ 0x01D35C40, "DELTA_WriteDelta", (size_t)&DELTA_WriteDelta },
	{ 0x01D35C80, "_DELTA_WriteDelta", (size_t)&_DELTA_WriteDelta },
	{ 0x01D35D00, "DELTA_ParseDelta", (size_t)&DELTA_ParseDelta },

	{ 0x01D36420, "DELTA_AddEncoder", (size_t)&DELTA_AddEncoder },
	{ 0x01D36460, "DELTA_ClearEncoders", (size_t)&DELTA_ClearEncoders },
	{ 0x01D364A0, "DELTA_LookupEncoder", (size_t)&DELTA_LookupEncoder },
	{ 0x01D364E0, "DELTA_CountLinks", (size_t)&DELTA_CountLinks },
	{ 0x01D36500, "DELTA_ReverseLinks", (size_t)&DELTA_ReverseLinks },
	{ 0x01D36520, "DELTA_ClearLinks", (size_t)&DELTA_ClearLinks },
	{ 0x01D36550, "DELTA_BuildFromLinks", (size_t)&DELTA_BuildFromLinks },
	{ 0x01D365F0, "DELTA_FindOffset", (size_t)&DELTA_FindOffset },
	{ 0x01D36640, "DELTA_ParseType", (size_t)&DELTA_ParseType },
	{ 0x01D36800, "DELTA_ParseField", (size_t)&DELTA_ParseField },
	{ 0x01D36A10, "DELTA_FreeDescription", (size_t)&DELTA_FreeDescription },
	{ 0x01D36A50, "DELTA_AddDefinition", (size_t)&DELTA_AddDefinition },
	{ 0x01D36AB0, "DELTA_ClearDefinitions", (size_t)&DELTA_ClearDefinitions },
	{ 0x01D36AF0, "DELTA_FindDefinition", (size_t)&DELTA_FindDefinition },
	{ 0x01D36B40, "DELTA_SkipDescription", (size_t)&DELTA_SkipDescription },
	{ 0x01D36BA0, "DELTA_ParseOneField", (size_t)&DELTA_ParseOneField },
	{ 0x01D36C50, "DELTA_ParseDescription", (size_t)&DELTA_ParseDescription },
	{ 0x01D36EA0, "DELTA_Load", (size_t)&DELTA_Load },
	{ 0x01D36EF0, "DELTA_RegisterDescription", (size_t)&DELTA_RegisterDescription },
	{ 0x01D36F30, "DELTA_ClearRegistrations", (size_t)&DELTA_ClearRegistrations },
	{ 0x01D36FC0, "DELTA_ClearStats", (size_t)&DELTA_ClearStats },
	{ 0x01D36FF0, "DELTA_ClearStats_f", (size_t)&DELTA_ClearStats_f },
	{ 0x01D36F80, "DELTA_LookupRegistration", (size_t)&DELTA_LookupRegistration },
	{ 0x01D37020, "DELTA_PrintStats", (size_t)&DELTA_PrintStats },
	{ 0x01D37090, "DELTA_DumpStats_f", (size_t)&DELTA_DumpStats_f },
	{ 0x01D370C0, "DELTA_Init", (size_t)&DELTA_Init },
	{ 0x01D37160, "DELTA_Shutdown", (size_t)&DELTA_Shutdown },

#endif // Delta_region

#ifndef Sv_Main_region

	{ 0x01D87E40, "SV_LookupDelta", (size_t)&SV_LookupDelta },
	{ 0x01D87E90, "SV_DownloadingModules", (size_t)&SV_DownloadingModules }, 	// NOXREF
	{ 0x01D87F70, "SV_GatherStatistics", (size_t)&SV_GatherStatistics },
	{ 0x01D88190, "SV_DeallocateDynamicData", (size_t)&SV_DeallocateDynamicData },
	{ 0x01D881D0, "SV_ReallocateDynamicData", (size_t)&SV_ReallocateDynamicData },
	{ 0x01D88250, "SV_AllocClientFrames", (size_t)&SV_AllocClientFrames },
	{ 0x01D882B0, "SV_IsPlayerIndex", (size_t)&SV_IsPlayerIndex_wrapped },
	{ 0x01D882D0, "SV_ClearPacketEntities", (size_t)&SV_ClearPacketEntities },
	{ 0x01D88310, "SV_AllocPacketEntities", (size_t)&SV_AllocPacketEntities },
	{ 0x01D88370, "SV_ClearFrames", (size_t)&SV_ClearFrames },
	{ 0x01D883E0, "SV_Serverinfo_f", (size_t)&SV_Serverinfo_f },
	{ 0x01D884C0, "SV_Localinfo_f", (size_t)&SV_Localinfo_f },
	{ 0x01D88550, "SV_User_f", (size_t)&SV_User_f },
	{ 0x01D88640, "SV_Users_f", (size_t)&SV_Users_f },
	{ 0x01D88700, "SV_CountPlayers", (size_t)&SV_CountPlayers },
	{ 0x01D88740, "SV_CountProxies", (size_t)&SV_CountProxies },
	{ 0x01D88790, "SV_FindModelNumbers", (size_t)&SV_FindModelNumbers },
	{ 0x01D887D0, "SV_StartParticle", (size_t)&SV_StartParticle },
	{ 0x01D88880, "SV_StartSound", (size_t)&SV_StartSound },
	{ 0x01D88960, "SV_BuildSoundMsg", (size_t)&SV_BuildSoundMsg },
	{ 0x01D88BD0, "SV_HashString", (size_t)&SV_HashString },
	{ 0x01D88C10, "SV_LookupSoundIndex", (size_t)&SV_LookupSoundIndex },
	{ 0x01D88CD0, "SV_BuildHashedSoundLookupTable", (size_t)&SV_BuildHashedSoundLookupTable },
	{ 0x01D88D20, "SV_AddSampleToHashedLookupTable", (size_t)&SV_AddSampleToHashedLookupTable },
	{ 0x01D88DA0, "SV_ValidClientMulticast", (size_t)&SV_ValidClientMulticast },
	{ 0x01D88E40, "SV_Multicast", (size_t)&SV_Multicast },
	{ 0x01D88FA0, "SV_WriteMovevarsToClient", (size_t)&SV_WriteMovevarsToClient },
	{ 0x01D89110, "SV_WriteDeltaDescriptionsToClient", (size_t)&SV_WriteDeltaDescriptionsToClient },
	{ 0x01D891B0, "SV_SetMoveVars", (size_t)&SV_SetMoveVars },
	{ 0x01D892F0, "SV_QueryMovevarsChanged", (size_t)&SV_QueryMovevarsChanged },
	{ 0x01D89590, "SV_SendServerinfo", (size_t)&SV_SendServerinfo },
	{ 0x01D897C0, "SV_SendResources", (size_t)&SV_SendResources },
	{ 0x01D89920, "SV_WriteClientdataToMessage", (size_t)&SV_WriteClientdataToMessage },
	{ 0x01D89BF0, "SV_WriteSpawn", (size_t)&SV_WriteSpawn },
	{ 0x01D89F10, "SV_SendUserReg", (size_t)&SV_SendUserReg },
	{ 0x01D89F80, "SV_New_f", (size_t)&SV_New_f },
	{ 0x01D8A210, "SV_SendRes_f", (size_t)&SV_SendRes_f },
	{ 0x01D8A2C0, "SV_Spawn_f", (size_t)&SV_Spawn_f },
	{ 0x01D8A3F0, "SV_CheckUpdateRate", (size_t)&SV_CheckUpdateRate },
	{ 0x01D8A510, "SV_RejectConnection", (size_t)&SV_RejectConnection },
	{ 0x01D8A5A0, "SV_RejectConnectionForPassword", (size_t)&SV_RejectConnectionForPassword },
	{ 0x01D8A610, "SV_GetFragmentSize", (size_t)&SV_GetFragmentSize },
	{ 0x01D8A680, "SV_FilterUser", (size_t)&SV_FilterUser },
	{ 0x01D8A760, "SV_CheckProtocol", (size_t)&SV_CheckProtocol },
	{ 0x01D8A7E0, "SV_CheckChallenge", (size_t)&SV_CheckChallenge },
	{ 0x01D8A8D0, "SV_CheckIPRestrictions", (size_t)&SV_CheckIPRestrictions },
	{ 0x01D8A980, "SV_CheckIPConnectionReuse", (size_t)&SV_CheckIPConnectionReuse },
	{ 0x01D8AA40, "SV_FinishCertificateCheck", (size_t)&SV_FinishCertificateCheck },
	{ 0x01D8AB10, "SV_CheckKeyInfo", (size_t)&SV_CheckKeyInfo },
	{ 0x01D8AC30, "SV_CheckForDuplicateSteamID", (size_t)&SV_CheckForDuplicateSteamID },
	{ 0x01D8ACE0, "SV_CheckForDuplicateNames", (size_t)&SV_CheckForDuplicateNames },
	{ 0x01D8AE10, "SV_CheckUserInfo", (size_t)&SV_CheckUserInfo },
	{ 0x01D8B080, "SV_FindEmptySlot", (size_t)&SV_FindEmptySlot },
	{ 0x01D8B100, "SV_ConnectClient", (size_t)&SV_ConnectClient },
	{ 0x01D8B8E0, "SVC_Ping", (size_t)&SVC_Ping },
	{ 0x01D8B930, "SVC_GetChallenge", (size_t)&SVC_GetChallenge },
	{ 0x01D8BB20, "SVC_ServiceChallenge", (size_t)&SVC_ServiceChallenge },
	{ 0x01D8BCB0, "SV_ResetModInfo", (size_t)&SV_ResetModInfo },
	//{ 0x01D8BE40, "SV_GetFakeClientCount", (size_t)&SV_GetFakeClientCount },//NOXREF
	//{ 0x01D8BE70, "SV_GetModInfo", (size_t)&SV_GetModInfo }, //NOXREF
	//{ 0x01D8BF40, "RequireValidChallenge", (size_t)&RequireValidChallenge }, //NOXREF
	//{ 0x01D8BF60, "ValidInfoChallenge", (size_t)&ValidInfoChallenge }, //NOXREF
	//{ 0x01D8BFB0, "GetChallengeNr", (size_t)&GetChallengeNr }, //NOXREF
	//{ 0x01D8C0C0, "CheckChallengeNr", (size_t)&CheckChallengeNr }, //NOXREF
	//{ 0x01D8C180, "ReplyServerChallenge", (size_t)&ReplyServerChallenge }, //NOXREF
	//{ 0x01D8C200, "ValidChallenge", (size_t)&ValidChallenge }, //NOXREF
	//{ 0x01D8C260, "SVC_InfoString", (size_t)&SVC_InfoString }, //NOXREF
	//{ 0x01D8C720, "SVC_Info", (size_t)&SVC_Info }, //NOXREF
	//{ 0x01D8CA40, "SVC_PlayerInfo", (size_t)&SVC_PlayerInfo }, //NOXREF
	//{ 0x01D8CBA0, "SVC_RuleInfo", (size_t)&SVC_RuleInfo }, //NOXREF
	{ 0x01D8CCC0, "SVC_GameDllQuery", (size_t)&SVC_GameDllQuery },
	{ 0x01D8CD70, "SV_FlushRedirect", (size_t)&SV_FlushRedirect },
	{ 0x01D8CE70, "SV_BeginRedirect", (size_t)&SV_BeginRedirect },
	{ 0x01D8CEA0, "SV_EndRedirect", (size_t)&SV_EndRedirect },
	{ 0x01D8CEB0, "SV_ResetRcon_f", (size_t)&SV_ResetRcon_f },
	{ 0x01D8CED0, "SV_AddFailedRcon", (size_t)&SV_AddFailedRcon },
	{ 0x01D8D1F0, "SV_CheckRconFailure", (size_t)&SV_CheckRconFailure },
	{ 0x01D8D250, "SV_Rcon_Validate", (size_t)&SV_Rcon_Validate },
	{ 0x01D8D370, "SV_Rcon", (size_t)&SV_Rcon },
	{ 0x01D8D560, "SV_ConnectionlessPacket", (size_t)&SV_ConnectionlessPacket },
	{ 0x01D8D750, "SV_CheckRate", (size_t)&SV_CheckRate },
	{ 0x01D8D810, "SV_ProcessFile", (size_t)&SV_ProcessFile },
	{ 0x01D8D960, "SV_FilterPacket", (size_t)&SV_FilterPacket },
	{ 0x01D8DA30, "SV_SendBan", (size_t)&SV_SendBan },
	{ 0x01D8DAB0, "SV_ReadPackets", (size_t)&SV_ReadPackets },
	//{ 0x, "ntohl", (size_t)&ntohl },
	//{ 0x, "htons", (size_t)&htons },
	{ 0x01D8DCC0, "SV_CheckTimeouts", (size_t)&SV_CheckTimeouts },
	{ 0x01D8DD50, "SV_CalcPing", (size_t)&SV_CalcPing },
	{ 0x01D8DE20, "SV_FullClientUpdate", (size_t)&SV_FullClientUpdate },
	{ 0x01D8DEF0, "SV_EmitEvents", (size_t)&SV_EmitEvents },
	{ 0x01D8E0F0, "SV_AddToFatPVS", (size_t)&SV_AddToFatPVS },
	{ 0x01D8E1B0, "SV_FatPVS", (size_t)&SV_FatPVS },
	{ 0x01D8E200, "SV_AddToFatPAS", (size_t)&SV_AddToFatPAS },
	{ 0x01D8E2D0, "SV_FatPAS", (size_t)&SV_FatPAS },
	{ 0x01D8E320, "SV_PointLeafnum", (size_t)&SV_PointLeafnum },
	//{ 0x, "TRACE_DELTA", (size_t)&TRACE_DELTA }, //NOXREF
	{ 0x01D8E370, "SV_SetCallback", (size_t)&SV_SetCallback },
	{ 0x01D8E3C0, "SV_SetNewInfo", (size_t)&SV_SetNewInfo },
	{ 0x01D8E3E0, "SV_WriteDeltaHeader", (size_t)&SV_WriteDeltaHeader },
	{ 0x01D8E4E0, "SV_InvokeCallback", (size_t)&SV_InvokeCallback },
	{ 0x01D8E520, "SV_FindBestBaseline", (size_t)&SV_FindBestBaseline },
	{ 0x01D8E650, "SV_CreatePacketEntities", (size_t)&SV_CreatePacketEntities },
	{ 0x01D8E9A0, "SV_EmitPacketEntities", (size_t)&SV_EmitPacketEntities },
	{ 0x01D8E9E0, "SV_ShouldUpdatePing", (size_t)&SV_ShouldUpdatePing },
	//{ 0x01D8EA40, "SV_HasEventsInQueue", (size_t)&SV_HasEventsInQueue }, //NOXREF
	{ 0x01D8EA70, "SV_GetNetInfo", (size_t)&SV_GetNetInfo },
	{ 0x01D8EB00, "SV_CheckVisibility", (size_t)&SV_CheckVisibility },
	{ 0x01D8EBF0, "SV_EmitPings", (size_t)&SV_EmitPings },
	{ 0x01D8EC90, "SV_WriteEntitiesToClient", (size_t)&SV_WriteEntitiesToClient },
	{ 0x01D8EE90, "SV_CleanupEnts", (size_t)&SV_CleanupEnts },
	{ 0x01D8EEC0, "SV_SendClientDatagram", (size_t)&SV_SendClientDatagram },
	{ 0x01D8EFC0, "SV_UpdateToReliableMessages", (size_t)&SV_UpdateToReliableMessages },
	{ 0x01D8F230, "SV_SkipUpdates", (size_t)&SV_SkipUpdates },
	{ 0x01D8F280, "SV_SendClientMessages", (size_t)&SV_SendClientMessages },
	{ 0x01D8F470, "SV_ExtractFromUserinfo", (size_t)&SV_ExtractFromUserinfo },
	{ 0x01D8F870, "SV_ModelIndex", (size_t)&SV_ModelIndex },
	{ 0x01D8F8E0, "SV_AddResource", (size_t)&SV_AddResource },
	{ 0x01D8F950, "SV_CreateGenericResources", (size_t)&SV_CreateGenericResources },
	{ 0x01D8FC30, "SV_CreateResourceList", (size_t)&SV_CreateResourceList },
	{ 0x01D8FDC0, "SV_ClearCaches", (size_t)&SV_ClearCaches },
	{ 0x01D8FE00, "SV_PropagateCustomizations", (size_t)&SV_PropagateCustomizations },
	{ 0x01D8FF00, "SV_WriteVoiceCodec", (size_t)&SV_WriteVoiceCodec },
	{ 0x01D8FF30, "SV_CreateBaseline", (size_t)&SV_CreateBaseline },
	{ 0x01D90170, "SV_BroadcastCommand", (size_t)&SV_BroadcastCommand },
	{ 0x01D90260, "SV_BuildReconnect", (size_t)&SV_BuildReconnect },
	//{ 0x01D90280, "SV_ReconnectAllClients", (size_t)&SV_ReconnectAllClients }, //NOXREF
	{ 0x01D903D0, "SetCStrikeFlags", (size_t)&SetCStrikeFlags },
	{ 0x01D904E0, "SV_ActivateServer", (size_t)&SV_ActivateServer },
	{ 0x01D90790, "SV_ServerShutdown", (size_t)&SV_ServerShutdown },
	{ 0x01D907C0, "SV_SpawnServer", (size_t)&SV_SpawnServer },
	{ 0x01D90E70, "SV_LoadEntities", (size_t)&SV_LoadEntities },
	{ 0x01D90E90, "SV_ClearEntities", (size_t)&SV_ClearEntities },
	{ 0x01D90ED0, "RegUserMsg", (size_t)&RegUserMsg },
	{ 0x01D90F80, "StringToFilter", (size_t)&StringToFilter },
	{ 0x01D91020, "SV_StringToUserID", (size_t)&SV_StringToUserID },
	{ 0x01D910C0, "SV_BanId_f", (size_t)&SV_BanId_f },
	{ 0x01D915C0, "Host_Kick_f", (size_t)&Host_Kick_f },
	{ 0x01D91990, "SV_RemoveId_f", (size_t)&SV_RemoveId_f },
	{ 0x01D91B90, "SV_WriteId_f", (size_t)&SV_WriteId_f },
	{ 0x01D91C60, "SV_ListId_f", (size_t)&SV_ListId_f },
	{ 0x01D91D00, "SV_AddIP_f", (size_t)&SV_AddIP_f },
	{ 0x01D91F00, "SV_RemoveIP_f", (size_t)&SV_RemoveIP_f },
	{ 0x01D91FD0, "SV_ListIP_f", (size_t)&SV_ListIP_f },
	{ 0x01D920B0, "SV_WriteIP_f", (size_t)&SV_WriteIP_f },
	{ 0x01D92190, "SV_KickPlayer", (size_t)&SV_KickPlayer },
	{ 0x01D92300, "SV_InactivateClients", (size_t)&SV_InactivateClients },
	{ 0x01D923A0, "SV_FailDownload", (size_t)&SV_FailDownload },
	{ 0x01D923E0, "Q_stristr", (size_t)&Q_stristr },
	{ 0x01D92480, "IsSafeFileToDownload", (size_t)&IsSafeFileToDownload },
	{ 0x01D92710, "SV_BeginFileDownload_f", (size_t)&SV_BeginFileDownload_f },
	{ 0x01D92940, "SV_SetMaxclients", (size_t)&SV_SetMaxclients },
	{ 0x01D92B00, "SV_HandleRconPacket", (size_t)&SV_HandleRconPacket },
	{ 0x01D92B70, "SV_CheckCmdTimes", (size_t)&SV_CheckCmdTimes },
	{ 0x01D92C60, "SV_CheckForRcon", (size_t)&SV_CheckForRcon },
	{ 0x01D92CC0, "SV_IsSimulating", (size_t)&SV_IsSimulating },
	{ 0x01D92D00, "SV_CheckMapDifferences", (size_t)&SV_CheckMapDifferences },
	{ 0x01D92D80, "SV_Frame", (size_t)&SV_Frame },
	{ 0x01D92E00, "SV_Drop_f", (size_t)&SV_Drop_f },
	{ 0x01D92E40, "SV_RegisterDelta", (size_t)&SV_RegisterDelta },
	{ 0x01D92EC0, "SV_InitDeltas", (size_t)&SV_InitDeltas },
	{ 0x01D93010, "SV_InitEncoders", (size_t)&SV_InitEncoders },
	{ 0x01D93050, "SV_Init", (size_t)&SV_Init },
	{ 0x01D93600, "SV_Shutdown", (size_t)&SV_Shutdown },
	{ 0x01D93650, "SV_CompareUserID", (size_t)&SV_CompareUserID },
	{ 0x01D936D0, "SV_GetIDString", (size_t)&SV_GetIDString },
	{ 0x01D938E0, "SV_GetClientIDString", (size_t)&SV_GetClientIDString },
	{ 0x01D93950, "GetGameAppID", (size_t)&GetGameAppID },
	{ 0x01D939C0, "IsGameSubscribed", (size_t)&IsGameSubscribed },
	{ 0x01D93A10, "BIsValveGame", (size_t)&BIsValveGame }, //NOXREF

#endif // Sv_Main_region

#ifndef Zone_region

	{ 0x01DBB120, "Z_ClearZone", (size_t)&Z_ClearZone },
	{ 0x01DBB170, "Z_Free", (size_t)&Z_Free },
	{ 0x01DBB220, "Z_Malloc", (size_t)&Z_Malloc },
	{ 0x01DBB260, "Z_TagMalloc", (size_t)&Z_TagMalloc },
	{ 0x01DBB310, "Z_Print", (size_t)&Z_Print },	// NOXREF
	{ 0x01DBB3C0, "Z_CheckHeap", (size_t)&Z_CheckHeap },

	{ 0x01DBB440, "Hunk_Check", (size_t)&Hunk_Check },
	{ 0x01DBB4B0, "Hunk_Print", (size_t)&Hunk_Print },	// NOXREF
	{ 0x01DBB6B0, "Hunk_AllocName", (size_t)&Hunk_AllocName },
	{ 0x01DBB750, "Hunk_Alloc", (size_t)&Hunk_Alloc },
	{ 0x01DBB770, "Hunk_LowMark", (size_t)&Hunk_LowMark },
	{ 0x01DBB780, "Hunk_FreeToLowMark", (size_t)&Hunk_FreeToLowMark },
	{ 0x01DBB7B0, "Hunk_HighMark", (size_t)&Hunk_HighMark },
	{ 0x01DBB7E0, "Hunk_FreeToHighMark", (size_t)&Hunk_FreeToHighMark },
	{ 0x01DBB830, "Hunk_HighAllocName", (size_t)&Hunk_HighAllocName },
	{ 0x01DBB900, "Hunk_TempAlloc", (size_t)&Hunk_TempAlloc },

	{ 0x01DBB960, "Cache_Move", (size_t)&Cache_Move },
	{ 0x01DBB9D0, "Cache_FreeLow", (size_t)&Cache_FreeLow },
	{ 0x01DBBA00, "Cache_FreeHigh", (size_t)&Cache_FreeHigh },
	{ 0x01DBBA60, "Cache_UnlinkLRU", (size_t)&Cache_UnlinkLRU },
	{ 0x01DBBAA0, "Cache_MakeLRU", (size_t)&Cache_MakeLRU },
	{ 0x01DBBAF0, "Cache_TryAlloc", (size_t)&Cache_TryAlloc },
	{ 0x01DBBC30, "Cache_Force_Flush", (size_t)&Cache_Force_Flush },
	{ 0x01DBBC60, "Cache_Flush", (size_t)&Cache_Flush },
	{ 0x01DBBD60, "CacheSystemCompare", (size_t)&CacheSystemCompare },	// NOXREF
	{ 0x01DBBC90, "Cache_Print", (size_t)&Cache_Print },	// NOXREF
	//{ 0x, "ComparePath1", (size_t)&ComparePath1 },	// NOXREF		// not yet located on windows
	//{ 0x, "CommatizeNumber", (size_t)&CommatizeNumber },	// NOXREF	// not yet located on windows
	//{ 0x, "Cache_Report", (size_t)&Cache_Report },	// NOXREF		// not yet located on windows
	//{ 0x, "Cache_Compact", (size_t)&Cache_Compact },	// NOXREF		// not yet located on windows
	{ 0x01DBBED0, "Cache_Init", (size_t)&Cache_Init },
	{ 0x01DBBF00, "Cache_Free", (size_t)&Cache_Free },
	{ 0x01DBBF50, "Cache_TotalUsed", (size_t)&Cache_TotalUsed },	// NOXREF
	{ 0x01DBBF70, "Cache_Check", (size_t)&Cache_Check },
	{ 0x01DBBFA0, "Cache_Alloc", (size_t)&Cache_Alloc },
	{ 0x01DBC040, "Memory_Init", (size_t)&Memory_Init },
	//{ 0x01DBC0E0, "Cache_Print_Sounds_And_Totals", (size_t)&Cache_Print_Models_And_Totals },	// NOXREF
	//{ 0x01DBC1F0, "Cache_Print_Sounds_And_Totals", (size_t)&Cache_Print_Sounds_And_Totals },	// NOXREF

#endif // Zone_region

#ifndef FileSystem_Internal_region

	{ 0x01D3E2E0, "FS_RemoveAllSearchPaths", (size_t)&FS_RemoveAllSearchPaths },	// NOXREF
	{ 0x01D3E2F0, "FS_AddSearchPath", (size_t)&FS_AddSearchPath },
	{ 0x01D3E310, "FS_RemoveSearchPath", (size_t)&FS_RemoveSearchPath },	// NOXREF
	{ 0x01D3E330, "FS_RemoveFile", (size_t)&FS_RemoveFile },
	{ 0x01D3E350, "FS_CreateDirHierarchy", (size_t)&FS_CreateDirHierarchy },
	{ 0x01D3E370, "FS_FileExists", (size_t)&FS_FileExists },
	{ 0x01D3E390, "FS_IsDirectory", (size_t)&FS_IsDirectory },	// NOXREF
	{ 0x01D3E3B0, "FS_Open", (size_t)&FS_Open },
	{ 0x01D3E3D0, "FS_OpenPathID", (size_t)&FS_OpenPathID },
	{ 0x01D3E3F0, "FS_Close", (size_t)&FS_Close },
	{ 0x01D3E410, "FS_Seek", (size_t)&FS_Seek },
	{ 0x01D3E430, "FS_Tell", (size_t)&FS_Tell },
	{ 0x01D3E450, "FS_Size", (size_t)&FS_Size },
	{ 0x01D3E470, "FS_FileSize", (size_t)&FS_FileSize },
	{ 0x01D3E490, "FS_GetFileTime", (size_t)&FS_GetFileTime },
	{ 0x01D3E4B0, "FS_FileTimeToString", (size_t)&FS_FileTimeToString },	// NOXREF
	{ 0x01D3E4D0, "FS_IsOk", (size_t)&FS_IsOk },
	{ 0x01D3E4F0, "FS_Flush", (size_t)&FS_Flush },
	{ 0x01D3E510, "FS_EndOfFile", (size_t)&FS_EndOfFile },
	{ 0x01D3E530, "FS_Read", (size_t)&FS_Read },
	{ 0x01D3E550, "FS_Write", (size_t)&FS_Write },
	{ 0x01D3E570, "FS_ReadLine", (size_t)&FS_ReadLine },
	{ 0x01D3E590, "FS_FPrintf", (size_t)&FS_FPrintf },
	{ 0x01D3E5E0, "FS_FindFirst", (size_t)&FS_FindFirst },
	{ 0x01D3E600, "FS_FindNext", (size_t)&FS_FindNext },
	{ 0x01D3E620, "FS_FindIsDirectory", (size_t)&FS_FindIsDirectory },	// NOXREF
	{ 0x01D3E640, "FS_FindClose", (size_t)&FS_FindClose },
	{ 0x01D3E660, "FS_GetLocalCopy", (size_t)&FS_GetLocalCopy },
	{ 0x01D3E680, "FS_GetLocalPath", (size_t)&FS_GetLocalPath },
	{ 0x01D3E6A0, "FS_ParseFile", (size_t)&FS_ParseFile },	// NOXREF
	{ 0x01D3E6E0, "FS_FullPathToRelativePath", (size_t)&FS_FullPathToRelativePath },	// NOXREF
	{ 0x01D3E700, "FS_GetCurrentDirectory", (size_t)&FS_GetCurrentDirectory },	// NOXREF
	{ 0x01D3E720, "FS_PrintOpenedFiles", (size_t)&FS_PrintOpenedFiles },	// NOXREF
	{ 0x01D3E730, "FS_SetWarningFunc", (size_t)&FS_SetWarningFunc },	// NOXREF
	{ 0x01D3E750, "FS_SetWarningLevel", (size_t)&FS_SetWarningLevel },	// NOXREF
	{ 0x01D3E770, "FS_GetCharacter", (size_t)&FS_GetCharacter },	// NOXREF
	{ 0x01D3E790, "FS_LogLevelLoadStarted", (size_t)&FS_LogLevelLoadStarted },
	{ 0x01D3E7B0, "FS_LogLevelLoadFinished", (size_t)&FS_LogLevelLoadFinished },
	{ 0x01D3E7D0, "FS_SetVBuf", (size_t)&FS_SetVBuf },
	{ 0x01D3E800, "FS_GetInterfaceVersion", (size_t)&FS_GetInterfaceVersion },
	{ 0x01D3E820, "FS_GetReadBuffer", (size_t)&FS_GetReadBuffer },
	{ 0x01D3E840, "FS_ReleaseReadBuffer", (size_t)&FS_ReleaseReadBuffer },
	{ 0x01D3E860, "FS_Unlink", (size_t)&FS_Unlink },
	{ 0x01D3E8A0, "FS_Rename", (size_t)&FS_Rename },
	{ 0x01D3E940, "FS_LoadLibrary", (size_t)&FS_LoadLibrary },

#endif // FileSystem_Internal_region

#ifndef FileSystem_region

	{ 0x01D3D340, "GetBaseDirectory", (size_t)&GetBaseDirectory },
	{ 0x01D3D355, "_Z20GetFileSystemFactoryv", (size_t)&GetFileSystemFactory },	// NOXREF
	//{ 0x01D3E250, "FileSystem_LoadDLL", (size_t)&FileSystem_LoadDLL },		// Totally not exists on Linux (it is inlined), but present and used on Windows, anyway just commented it out
	//{ 0x01D3E2C0, "FileSystem_UnloadDLL", (size_t)&FileSystem_UnloadDLL },	// Totally not exists on Linux (it is inlined), but present and used on Windows, anyway just commented it out
	{ 0x01D3D360, "BEnabledHDAddon", (size_t)&BEnabledHDAddon },
	{ 0x01D3D390, "BEnableAddonsFolder", (size_t)&BEnableAddonsFolder },

	{ 0x01D3D3C0, "Host_SetHDModels_f", (size_t)&Host_SetHDModels_f },
	{ 0x01D3D440, "Host_SetAddonsFolder_f", (size_t)&Host_SetAddonsFolder_f },
	{ 0x01D3D4C0, "Host_SetVideoLevel_f", (size_t)&Host_SetVideoLevel_f },
	{ 0x01D3D510, "Host_GetVideoLevel", (size_t)&Host_GetVideoLevel },

	{ 0x01D3D530, "_Z26CheckLiblistForFallbackDirPKcbS0_b", (size_t)&CheckLiblistForFallbackDir },
	{ 0x01D3DA60, "FileSystem_SetGameDirectory", (size_t)&FileSystem_SetGameDirectory },
	{ 0x01D3E130, "FileSystem_AddFallbackGameDir", (size_t)&FileSystem_AddFallbackGameDir },
	{ 0x01D3E200, "_Z15FileSystem_InitPcPv", (size_t)&FileSystem_Init },
	{ 0x01D3E2B0, "_Z19FileSystem_Shutdownv", (size_t)&FileSystem_Shutdown },

#endif // FileSystem_region

#ifndef Unicode_StrTools_region

	{ 0x01DA8F00, "_Z16Q_IsValidUChar32w", (size_t)&Q_IsValidUChar32 },
	{ 0x01DA97D0, "_Z15Q_UTF8ToUChar32PKcRwRb", (size_t)&Q_UTF8ToUChar32 },
	{ 0x01DA9910, "Q_UnicodeValidate", (size_t)&Q_UnicodeValidate },
	{ 0x01DA99A0, "Q_UnicodeAdvance", (size_t)&Q_UnicodeAdvance },
	{ 0x01DA9EA0, "Q_UnicodeRepair", (size_t)&Q_UnicodeRepair },
	{ 0x01DA9E70, "V_UTF8ToUChar32", (size_t)&V_UTF8ToUChar32 },

#endif // Unicode_StrTools_region

#ifndef Cmd_region

	{ 0x01D26CD0, "Cmd_Wait_f", (size_t)&Cmd_Wait_f },
	{ 0x01D26CE0, "Cbuf_Init", (size_t)&Cbuf_Init },
	{ 0x01D26D00, "Cbuf_AddText", (size_t)&Cbuf_AddText },
	{ 0x01D26D50, "Cbuf_InsertText", (size_t)&Cbuf_InsertText },
	{ 0x01D26DE0, "Cbuf_InsertTextLines", (size_t)&Cbuf_InsertTextLines },
	{ 0x01D26E80, "Cbuf_Execute", (size_t)&Cbuf_Execute },
	{ 0x01D26F60, "Cmd_StuffCmds_f", (size_t)&Cmd_StuffCmds_f },
	{ 0x01D270F0, "Cmd_Exec_f", (size_t)&Cmd_Exec_f },
	{ 0x01D273A0, "Cmd_Echo_f", (size_t)&Cmd_Echo_f },
	{ 0x01D273E0, "CopyString", (size_t)&CopyString },
	{ 0x01D27410, "Cmd_Alias_f", (size_t)&Cmd_Alias_f },
	{ 0x01D276A0, "Cmd_GetFirstCmd", (size_t)&Cmd_GetFirstCmd },
	{ 0x01D276B0, "Cmd_Init", (size_t)&Cmd_Init },
	{ 0x01D27720, "Cmd_Shutdown", (size_t)&Cmd_Shutdown },
	{ 0x01D27750, "Cmd_Argc", (size_t)&Cmd_Argc },
	{ 0x01D27760, "Cmd_Argv", (size_t)&Cmd_Argv },
	{ 0x01D27790, "Cmd_Args", (size_t)&Cmd_Args },
	{ 0x01D277A0, "Cmd_TokenizeString", (size_t)&Cmd_TokenizeString },
	{ 0x01D27880, "Cmd_FindCmd", (size_t)&Cmd_FindCmd },	// NOXREF
	{ 0x01D278C0, "Cmd_FindCmdPrev", (size_t)&Cmd_FindCmdPrev },	// NOXREF
	{ 0x01D27900, "Cmd_AddCommand", (size_t)&Cmd_AddCommand },
	{ 0x01D27A10, "Cmd_AddMallocCommand", (size_t)&Cmd_AddMallocCommand },
	{ 0x01D27AB0, "Cmd_AddHUDCommand", (size_t)&Cmd_AddHUDCommand },	// NOXREF
	{ 0x01D27AD0, "Cmd_AddWrapperCommand", (size_t)&Cmd_AddWrapperCommand },	// NOXREF
	{ 0x01D27AF0, "Cmd_AddGameCommand", (size_t)&Cmd_AddGameCommand },
	{ 0x01D27B10, "Cmd_RemoveMallocedCmds", (size_t)&Cmd_RemoveMallocedCmds },
	{ 0x01D27B50, "Cmd_RemoveHudCmds", (size_t)&Cmd_RemoveHudCmds },	// NOXREF
	{ 0x01D27B60, "Cmd_RemoveGameCmds", (size_t)&Cmd_RemoveGameCmds },
	{ 0x01D27B70, "Cmd_RemoveWrapperCmds", (size_t)&Cmd_RemoveWrapperCmds },
	{ 0x01D27B80, "Cmd_Exists", (size_t)&Cmd_Exists },
	{ 0x01D27BC0, "Cmd_CompleteCommand", (size_t)&Cmd_CompleteCommand },	// NOXREF
	{ 0x01D27D10, "Cmd_ExecuteString", (size_t)&Cmd_ExecuteString },
	{ 0x01D27DF0, "Cmd_ForwardToServerInternal", (size_t)&Cmd_ForwardToServerInternal },
	{ 0x01D27F40, "Cmd_ForwardToServer", (size_t)&Cmd_ForwardToServer },
	{ 0x01D27F90, "Cmd_ForwardToServerUnreliable", (size_t)&Cmd_ForwardToServerUnreliable },
	{ 0x01D27FA0, "Cmd_CheckParm", (size_t)&Cmd_CheckParm },	// NOXREF
	{ 0x01D28000, "Cmd_CmdList_f", (size_t)&Cmd_CmdList_f },

#endif // Cmd_region

#ifndef Cvar_region

	{ 0x01D2D760, "Cvar_Init", (size_t)&Cvar_Init },
	{ 0x01D2D770, "Cvar_Shutdown", (size_t)&Cvar_Shutdown },
	{ 0x01D2D780, "Cvar_FindVar", (size_t)&Cvar_FindVar },
	{ 0x01D2D7C0, "Cvar_FindPrevVar", (size_t)&Cvar_FindPrevVar },	// NOXREF
	{ 0x01D2D800, "Cvar_VariableValue", (size_t)&Cvar_VariableValue },
	{ 0x01D2D830, "Cvar_VariableInt", (size_t)&Cvar_VariableInt },	// NOXREF
	{ 0x01D2D860, "Cvar_VariableString", (size_t)&Cvar_VariableString },
	{ 0x01D2D880, "Cvar_CompleteVariable", (size_t)&Cvar_CompleteVariable },	// NOXREF
	{ 0x01D2D9C0, "Cvar_DirectSet", (size_t)&Cvar_DirectSet },
	{ 0x01D2DC30, "Cvar_Set", (size_t)&Cvar_Set },
	{ 0x01D2DC70, "Cvar_SetValue", (size_t)&Cvar_SetValue },
	{ 0x01D2DD20, "Cvar_RegisterVariable", (size_t)&Cvar_RegisterVariable },
	{ 0x01D2DE10, "Cvar_RemoveHudCvars", (size_t)&Cvar_RemoveHudCvars },	// NOXREF
	{ 0x01D2DE70, "Cvar_IsMultipleTokens", (size_t)&Cvar_IsMultipleTokens },
	{ 0x01D2DED0, "Cvar_Command", (size_t)&Cvar_Command },
	{ 0x01D2DFA0, "Cvar_WriteVariables", (size_t)&Cvar_WriteVariables },	// NOXREF
	{ 0x01D2DFE0, "Cmd_CvarListPrintCvar", (size_t)&Cmd_CvarListPrintCvar },
	{ 0x01D2E0F0, "Cmd_CvarList_f", (size_t)&Cmd_CvarList_f },
	{ 0x01D2E330, "Cvar_CountServerVariables", (size_t)&Cvar_CountServerVariables },	// NOXREF
	{ 0x01D2E350, "Cvar_UnlinkExternals", (size_t)&Cvar_UnlinkExternals },
	{ 0x01D2E380, "Cvar_CmdInit", (size_t)&Cvar_CmdInit },

#endif // Cvar_region

#ifndef Info_region

	{ 0x01D4B610, "Info_ValueForKey", (size_t)&Info_ValueForKey },
	{ 0x01D4B6E0, "Info_RemoveKey", (size_t)&Info_RemoveKey },
	{ 0x01D4B7D0, "Info_RemovePrefixedKeys", (size_t)&Info_RemovePrefixedKeys },
	{ 0x01D4B860, "Info_IsKeyImportant", (size_t)&Info_IsKeyImportant },
	{ 0x01D4B980, "Info_FindLargestKey", (size_t)&Info_FindLargestKey },
	{ 0x01D4BA80, "Info_SetValueForStarKey", (size_t)&Info_SetValueForStarKey },
	{ 0x01D4BCF0, "Info_SetValueForKey", (size_t)&Info_SetValueForKey },
	{ 0x01D4BD30, "Info_Print", (size_t)&Info_Print },
	{ 0x01D4BE10, "Info_IsValid", (size_t)&Info_IsValid },

#endif // Info_region

#ifndef SysDll_region

	//{ 0x, "Sys_PageIn", (size_t)&Sys_PageIn },
	{ 0x01D9ECF0, "Sys_FindFirst", (size_t)&Sys_FindFirst },
	{ 0x01D9ED50, "Sys_FindFirstPathID", (size_t)&Sys_FindFirstPathID },
	{ 0x01D9ED90, "Sys_FindNext", (size_t)&Sys_FindNext },
	{ 0x01D9EDC0, "Sys_FindClose", (size_t)&Sys_FindClose },
	//{ 0x01D9EDE0, "glob_match_after_star", (size_t)&glob_match_after_star },
	//{ 0x01D9EF50, "glob_match", (size_t)&glob_match },
	//{ 0x01D9EFD0, "Sys_MakeCodeWriteable", (size_t)&Sys_MakeCodeWriteable },
	//{ 0x, "Sys_SetFPCW", (size_t)&Sys_SetFPCW },
	//{ 0x, "Sys_PushFPCW_SetHigh", (size_t)&Sys_PushFPCW_SetHigh },
	//{ 0x, "Sys_PopFPCW", (size_t)&Sys_PopFPCW },
	//{ 0x, "MaskExceptions", (size_t)&MaskExceptions },
	//{ 0x, "Sys_Init", (size_t)&Sys_Init },	// NOXREF
	//{ 0x, "Sys_Sleep", (size_t)&Sys_Sleep },
	//{ 0x, "Sys_DebugOutStraight", (size_t)&Sys_DebugOutStraight },
	//{ 0x01D9F0E0, "Sys_Error", (size_t)&Sys_Error },
	//{ 0x01D9F1F0, "Sys_Warning", (size_t)&Sys_Warning },
	{ 0x01D9F230, "Sys_Printf", (size_t)&Sys_Printf },
	{ 0x01D9F4A0, "Sys_Quit", (size_t)&Sys_Quit },
	{ 0x01D9F2A0, "Sys_FloatTime", (size_t)&Sys_FloatTime },
	{ 0x01D9F460, "Dispatch_Substate", (size_t)&Dispatch_Substate },
	{ 0x01D9F470, "GameSetSubState", (size_t)&GameSetSubState },
	{ 0x01D9F4A0, "GameSetState", (size_t)&GameSetState },
	//{ 0x, "GameSetBackground", (size_t)&GameSetBackground },
	{ 0x01D9F7B0, "Voice_GetClientListening", (size_t)&Voice_GetClientListening },
	{ 0x01D9F810, "Voice_SetClientListening", (size_t)&Voice_SetClientListening },
	{ 0x01D9F890, "GetDispatch", (size_t)&GetDispatch },
	{ 0x01D9F8D0, "FindAddressInTable", (size_t)&FindAddressInTable },
	{ 0x01D9F910, "FindNameInTable", (size_t)&FindNameInTable },
	//{ 0x, "ConvertNameToLocalPlatform", (size_t)&ConvertNameToLocalPlatform },
	{ 0x01D9FAA0, "FunctionFromName", (size_t)&FunctionFromName },
	{ 0x01D9FB00, "NameForFunction", (size_t)&NameForFunction },
	{ 0x01D9FB50, "GetEntityInit", (size_t)&GetEntityInit },
	{ 0x01D9FB70, "GetIOFunction", (size_t)&GetIOFunction },
	{ 0x01D9FB90, "DLL_SetModKey", (size_t)&DLL_SetModKey },
	{ 0x01D9FE50, "LoadEntityDLLs", (size_t)&LoadEntityDLLs },
	{ 0x01DA02D0, "LoadThisDll", (size_t)&LoadThisDll },
	{ 0x01DA0390, "ReleaseEntityDlls", (size_t)&ReleaseEntityDlls },
	//{ 0x01DA0410, "EngineFprintf", (size_t)&EngineFprintf },
	{ 0x01DA0420, "AlertMessage", (size_t)&AlertMessage },
	//{ 0x01DA0640, "Sys_SplitPath", (size_t)&Sys_SplitPath },
	{ 0x01D2BD90, "Con_Debug_f", (size_t)&Con_Debug_f },
	{ 0x01D2BDD0, "Con_Init", (size_t)&Con_Init },
	{ 0x01D2BFC0, "Con_DebugLog", (size_t)&Con_DebugLog },
	{ 0x01D2C020, "Con_Printf", (size_t)&Con_Printf },
	{ 0x01D2C1E0, "Con_SafePrintf", (size_t)&Con_SafePrintf },
	{ 0x01D2C140, "Con_DPrintf", (size_t)&Con_DPrintf },

#ifdef _WIN32
	{ 0x01DA0A70, "Sys_InitHardwareTimer", (size_t)&Sys_InitHardwareTimer },
#endif //_WIN32

#endif // SysDll_region

#ifndef Sys_Dll2_region

	{ 0x01DA0670, "GetCurrentSteamAppName", (size_t)&GetCurrentSteamAppName },
	//{ 0x01DA0760, "SetRateRegistrySetting", (size_t)&SetRateRegistrySetting },	// NOXREF
	//{ 0x01DA0780, "GetRateRegistrySetting", (size_t)&GetRateRegistrySetting },	// NOXREF
	{ 0x01DA07A0, "F", (size_t)&F },
	{ 0x01DA0820, "Sys_GetCDKey", (size_t)&Sys_GetCDKey },
	//{ 0x01DA0930, "_Z19Legacy_ErrorMessageiPKc", (size_t)&Legacy_ErrorMessage },	// NOXREF
	{ 0x01DA0940, "_Z17Legacy_Sys_PrintfPcz", (size_t)&Legacy_Sys_Printf },
	//{ 0x01DA0980, "_Z30Legacy_MP3subsys_Suspend_Audiov", (size_t)&Legacy_MP3subsys_Suspend_Audio },	// NOXREF
	//{ 0x01DA0990, "_Z29Legacy_MP3subsys_Resume_Audiov", (size_t)&Legacy_MP3subsys_Resume_Audio },	// NOXREF
#ifndef _WIN32
	{ 0x0, "_Z19Sys_SetupLegacyAPIsv", (size_t)&Sys_SetupLegacyAPIs },
#endif
	//{ 0x01DA09C0, "_Z11Sys_IsWin95v", (size_t)&Sys_IsWin95 },	// NOXREF
	//{ 0x01DA09D0, "_Z11Sys_IsWin98v", (size_t)&Sys_IsWin98 },	// NOXREF
#ifdef _WIN32
	//{ 0x, "_Z18Sys_CheckOSVersionv", (size_t)&Sys_CheckOSVersion },	// NOXREF
#endif
	//{ 0x, "Sys_Init", (size_t)&Sys_Init },			// NOXREF
	//{ 0x, "Sys_Shutdown", (size_t)&Sys_Shutdown },	// NOXREF
	{ 0x01DA0B70, "_Z12Sys_InitArgvPc", (size_t)&Sys_InitArgv },
	//{ 0x, "Sys_ShutdownArgv", (size_t)&Sys_ShutdownArgv },	// NOXREF
	{ 0x01DA0C20, "_Z14Sys_InitMemoryv", (size_t)&Sys_InitMemory },
	{ 0x01DA0D50, "Sys_ShutdownMemory", (size_t)&Sys_ShutdownMemory },
	{ 0x01DA0D70, "_Z25Sys_InitLauncherInterfacev", (size_t)&Sys_InitLauncherInterface },
	//{ 0x, "Sys_ShutdownLauncherInterface", (size_t)&Sys_ShutdownLauncherInterface },	// NOXREF
	{ 0x01DA0DA0, "_Z22Sys_InitAuthenticationv", (size_t)&Sys_InitAuthentication },
	//{ 0x, "Sys_ShutdownAuthentication", (size_t)&Sys_ShutdownAuthentication },	// NOXREF
	{ 0x01DA0DC0, "_Z21Sys_ShowProgressTicksPc", (size_t)&Sys_ShowProgressTicks },
	{ 0x01DA0ED0, "_Z12Sys_InitGamePcS_Pvi", (size_t)&Sys_InitGame },
	{ 0x01DA0FE0, "_Z16Sys_ShutdownGamev", (size_t)&Sys_ShutdownGame },
	{ 0x01DA1060, "_Z13ClearIOStatesv", (size_t)&ClearIOStates },
	//{ 0x01DA13E0, "_Z22BuildMapCycleListHintsPPc", (size_t)&BuildMapCycleListHints },	// NOXREF

	{ 0x01DA16B0, "_ZN19CDedicatedServerAPI4InitEPcS0_PFP14IBaseInterfacePKcPiES7_", mfunc_ptr_cast(&CDedicatedServerAPI::Init_noVirt) },
	{ 0x01DA1800, "_ZN19CDedicatedServerAPI8ShutdownEv", mfunc_ptr_cast(&CDedicatedServerAPI::Shutdown_noVirt) },
	{ 0x01DA1860, "_ZN19CDedicatedServerAPI8RunFrameEv", mfunc_ptr_cast(&CDedicatedServerAPI::RunFrame_noVirt) },
	{ 0x01DA1880, "_ZN19CDedicatedServerAPI14AddConsoleTextEPc", mfunc_ptr_cast(&CDedicatedServerAPI::AddConsoleText_noVirt) },
	{ 0x01DA18A0, "_ZN19CDedicatedServerAPI12UpdateStatusEPfPiS1_Pc", mfunc_ptr_cast(&CDedicatedServerAPI::UpdateStatus_noVirt) },

#endif // Sys_Dll2_region

#ifndef CModel_region

	{ 0x01D281B0, "Mod_Init", (size_t)&Mod_Init },
	{ 0x01D281D0, "Mod_DecompressVis", (size_t)&Mod_DecompressVis },
	{ 0x01D28210, "Mod_LeafPVS", (size_t)&Mod_LeafPVS },
	{ 0x01D28270, "CM_DecompressPVS", (size_t)&CM_DecompressPVS },
	{ 0x01D282E0, "CM_LeafPVS", (size_t)&CM_LeafPVS },
	{ 0x01D28310, "CM_LeafPAS", (size_t)&CM_LeafPAS },
	{ 0x01D28340, "CM_FreePAS", (size_t)&CM_FreePAS },
	{ 0x01D28380, "CM_CalcPAS", (size_t)&CM_CalcPAS },
	{ 0x01D28580, "CM_HeadnodeVisible", (size_t)&CM_HeadnodeVisible },

#endif // CModel_region

#ifndef Model_region

	{ 0x01D50B80, "SW_Mod_Init", (size_t)&SW_Mod_Init },
	{ 0x01D50B90, "Mod_Extradata", (size_t)&Mod_Extradata },
	{ 0x01D50BF0, "Mod_PointInLeaf", (size_t)&Mod_PointInLeaf },
	{ 0x01D50C80, "Mod_ClearAll", (size_t)&Mod_ClearAll },
	{ 0x01D50CC0, "Mod_FillInCRCInfo", (size_t)&Mod_FillInCRCInfo },
	{ 0x01D50CE0, "Mod_FindName", (size_t)&Mod_FindName },
	//{ 0x01D50DF0, "Mod_ValidateCRC", (size_t)&Mod_ValidateCRC },	// NOXREF
	//{ 0x01D50E50, "Mod_NeedCRC", (size_t)&Mod_NeedCRC },	// NOXREF
	{ 0x01D50E90, "Mod_LoadModel", (size_t)&Mod_LoadModel },
	//{ 0x01D51100, "Mod_MarkClient", (size_t)&Mod_MarkClient },	// NOXREF
	{ 0x01D51110, "Mod_ForName", (size_t)&Mod_ForName },
	{ 0x01D51150, "Mod_AdInit", (size_t)&Mod_AdInit },
	{ 0x01D511F0, "Mod_AdSwap", (size_t)&Mod_AdSwap },
	{ 0x01D51280, "Mod_LoadTextures", (size_t)&Mod_LoadTextures },
	{ 0x01D517C0, "Mod_LoadLighting", (size_t)&Mod_LoadLighting },
	{ 0x01D51810, "Mod_LoadVisibility", (size_t)&Mod_LoadVisibility },
	{ 0x01D51860, "Mod_LoadEntities", (size_t)&Mod_LoadEntities },
	{ 0x01D51930, "Mod_LoadVertexes", (size_t)&Mod_LoadVertexes },
	{ 0x01D519E0, "Mod_LoadSubmodels", (size_t)&Mod_LoadSubmodels },
	{ 0x01D51B10, "Mod_LoadEdges", (size_t)&Mod_LoadEdges },
	{ 0x01D51BA0, "Mod_LoadTexinfo", (size_t)&Mod_LoadTexinfo },
	{ 0x01D51D60, "CalcSurfaceExtents", (size_t)&CalcSurfaceExtents },
	{ 0x01D51F10, "Mod_LoadFaces", (size_t)&Mod_LoadFaces },
	{ 0x01D52140, "Mod_SetParent", (size_t)&Mod_SetParent },
	{ 0x01D52180, "Mod_LoadNodes", (size_t)&Mod_LoadNodes },
	{ 0x01D52330, "Mod_LoadLeafs", (size_t)&Mod_LoadLeafs },
	{ 0x01D52490, "Mod_LoadClipnodes", (size_t)&Mod_LoadClipnodes },
	{ 0x01D52630, "Mod_MakeHull0", (size_t)&Mod_MakeHull0 },
	{ 0x01D52720, "Mod_LoadMarksurfaces", (size_t)&Mod_LoadMarksurfaces },
	{ 0x01D527E0, "Mod_LoadSurfedges", (size_t)&Mod_LoadSurfedges },
	{ 0x01D52860, "Mod_LoadPlanes", (size_t)&Mod_LoadPlanes },
	{ 0x01D52970, "RadiusFromBounds", (size_t)&RadiusFromBounds },
	{ 0x01D52A00, "Mod_LoadBrushModel", (size_t)&Mod_LoadBrushModel },
	//{ 0x01D52C30, "Mod_LoadAliasFrame", (size_t)&Mod_LoadAliasFrame },
	//{ 0x01D52CE0, "Mod_LoadAliasGroup", (size_t)&Mod_LoadAliasGroup },
	//{ 0x01D52DF0, "Mod_LoadAliasSkin", (size_t)&Mod_LoadAliasSkin },
	//{ 0x01D52E60, "Mod_LoadAliasSkinGroup", (size_t)&Mod_LoadAliasSkinGroup },
	//{ 0x01D52F40, "Mod_LoadAliasModel", (size_t)&Mod_LoadAliasModel },
	{ 0x01D53410, "Mod_LoadSpriteFrame", (size_t)&Mod_LoadSpriteFrame },
	{ 0x01D53510, "Mod_LoadSpriteGroup", (size_t)&Mod_LoadSpriteGroup },
	{ 0x01D535E0, "Mod_LoadSpriteModel", (size_t)&Mod_LoadSpriteModel },
	//{ 0x01D53800, "Mod_UnloadSpriteTextures", (size_t)&Mod_UnloadSpriteTextures },
	{ 0x01D53820, "Mod_Print", (size_t)&Mod_Print },
	//{ 0x01D538A0, "Mod_ChangeGame", (size_t)&Mod_ChangeGame },

#endif // Model_region

#ifndef Sv_Log_region

	{ 0x01D873D0, "Log_Printf", (size_t)&Log_Printf },
	{ 0x01D875A0, "Log_PrintServerVars", (size_t)&Log_PrintServerVars },
	{ 0x01D87600, "Log_Close", (size_t)&Log_Close },
	{ 0x01D87630, "Log_Open", (size_t)&Log_Open },
	{ 0x01D87850, "SV_SetLogAddress_f", (size_t)&SV_SetLogAddress_f },
	{ 0x01D87990, "SV_AddLogAddress_f", (size_t)&SV_AddLogAddress_f },
	{ 0x01D87BC0, "SV_DelLogAddress_f", (size_t)&SV_DelLogAddress_f },

#endif // Sv_Log_region

#ifndef Cl_Null_region

	//{ 0x, "CL_RecordHUDCommand", (size_t)&CL_RecordHUDCommand },
	//{ 0x, "R_DecalRemoveAll", (size_t)&R_DecalRemoveAll },
	//{ 0x, "CL_CheckForResend", (size_t)&CL_CheckForResend },
	//{ 0x, "CL_CheckFile", (size_t)&CL_CheckFile },
	//{ 0x01D17350, "CL_ClearClientState", (size_t)&CL_ClearClientState },
	//{ 0x, "CL_Connect_f", (size_t)&CL_Connect_f },
	//{ 0x, "CL_DecayLights", (size_t)&CL_DecayLights },
	//{ 0x01D17490, "CL_Disconnect", (size_t)&CL_Disconnect },
	//{ 0x01D17660, "CL_Disconnect_f", (size_t)&CL_Disconnect_f },
	//{ 0x, "CL_EmitEntities", (size_t)&CL_EmitEntities },
	//{ 0x, "CL_InitClosest", (size_t)&CL_InitClosest },
	//{ 0x, "CL_Init", (size_t)&CL_Init },
	{ 0x01D131E0, "CL_Particle", (size_t)&CL_Particle },
	//{ 0x, "CL_PredictMove", (size_t)&CL_PredictMove },
	//{ 0x, "CL_PrintLogos", (size_t)&CL_PrintLogos },
	//{ 0x, "CL_ReadPackets", (size_t)&CL_ReadPackets },
	//{ 0x, "CL_RequestMissingResources", (size_t)&CL_RequestMissingResources },
	//{ 0x, "CL_Move", (size_t)&CL_Move },
	//{ 0x, "CL_SendConnectPacket", (size_t)&CL_SendConnectPacket },
	//{ 0x01D0FABD, "CL_StopPlayback", (size_t)&CL_StopPlayback },
	//{ 0x, "CL_UpdateSoundFade", (size_t)&CL_UpdateSoundFade },
	//{ 0x, "CL_AdjustClock", (size_t)&CL_AdjustClock },
	//{ 0x01D48910, "CL_Save", (size_t)&CL_Save },
	//{ 0x01D19650, "CL_HudMessage", (size_t)&CL_HudMessage },

	//{ 0x, "Key_WriteBindings", (size_t)&Key_WriteBindings },
	//{ 0x, "ClientDLL_UpdateClientData", (size_t)&ClientDLL_UpdateClientData },
	//{ 0x, "ClientDLL_HudVidInit", (size_t)&ClientDLL_HudVidInit },
	//{ 0x, "Chase_Init", (size_t)&Chase_Init },
	//{ 0x, "Key_Init", (size_t)&Key_Init },
	//{ 0x, "ClientDLL_Init", (size_t)&ClientDLL_Init },
	//{ 0x, "Con_Shutdown", (size_t)&Con_Shutdown },
	//{ 0x, "DispatchDirectUserMsg", (size_t)&DispatchDirectUserMsg },
	//{ 0x, "CL_ShutDownUsrMessages", (size_t)&CL_ShutDownUsrMessages },
	//{ 0x, "CL_ShutDownClientStatic", (size_t)&CL_ShutDownClientStatic },

	//{ 0x, "ClientDLL_MoveClient", (size_t)&ClientDLL_MoveClient },

	//{ 0x, "CL_Shutdown", (size_t)&CL_Shutdown },

	//{ 0x, "ClientDLL_Frame", (size_t)&ClientDLL_Frame },
	//{ 0x, "ClientDLL_CAM_Think", (size_t)&ClientDLL_CAM_Think },
	//{ 0x, "CL_InitEventSystem", (size_t)&CL_InitEventSystem },
	//{ 0x, "CL_CheckClientState", (size_t)&CL_CheckClientState },
	//{ 0x, "CL_RedoPrediction", (size_t)&CL_RedoPrediction },
	//{ 0x, "CL_SetLastUpdate", (size_t)&CL_SetLastUpdate },

	//{ 0x, "Con_NPrintf", (size_t)&Con_NPrintf },
	//{ 0x01D805A0, "Sequence_OnLevelLoad", (size_t)&Sequence_OnLevelLoad },
	//{ 0x01D1CDD0, "CL_WriteMessageHistory", (size_t)&CL_WriteMessageHistory },
	//{ 0x, "CL_MoveSpectatorCamera", (size_t)&CL_MoveSpectatorCamera },
	//{ 0x, "CL_AddVoiceToDatagram", (size_t)&CL_AddVoiceToDatagram },
	//{ 0x, "CL_VoiceIdle", (size_t)&CL_VoiceIdle },
	//{ 0x, "PollDInputDevices", (size_t)&PollDInputDevices },
	//{ 0x, "CL_KeepConnectionActive", (size_t)&CL_KeepConnectionActive },
	//{ 0x, "CL_UpdateModuleC", (size_t)&CL_UpdateModuleC },
	//{ 0x, "VGuiWrap2_IsInCareerMatch", (size_t)&VGuiWrap2_IsInCareerMatch },
	//{ 0x, "VguiWrap2_GetCareerUI", (size_t)&VguiWrap2_GetCareerUI },
	//{ 0x, "VGuiWrap2_GetLocalizedStringLength", (size_t)&VGuiWrap2_GetLocalizedStringLength },
	//{ 0x01D07630, "VGuiWrap2_LoadingStarted", (size_t)&VGuiWrap2_LoadingStarted },

	//{ 0x, "ConstructTutorMessageDecayBuffer", (size_t)&ConstructTutorMessageDecayBuffer },
	//{ 0x, "ProcessTutorMessageDecayBuffer", (size_t)&ProcessTutorMessageDecayBuffer },
	//{ 0x, "GetTimesTutorMessageShown", (size_t)&GetTimesTutorMessageShown },
	//{ 0x, "RegisterTutorMessageShown", (size_t)&RegisterTutorMessageShown },
	//{ 0x, "ResetTutorMessageDecayData", (size_t)&ResetTutorMessageDecayData },
	//{ 0x01D83340, "SetCareerAudioState", (size_t)&SetCareerAudioState },

#endif // Cl_Null_region

#ifndef Snd_Null_region

	//{ 0x0, "S_Init", (size_t)&S_Init },
	//{ 0x0, "S_AmbientOff", (size_t)&S_AmbientOff },
	//{ 0x0, "S_AmbientOn", (size_t)&S_AmbientOn },
	//{ 0x0, "S_Shutdown", (size_t)&S_Shutdown },
	//{ 0x0, "S_TouchSound", (size_t)&S_TouchSound },
	//{ 0x0, "S_ClearBuffer", (size_t)&S_ClearBuffer },
	//{ 0x0, "S_StartStaticSound", (size_t)&S_StartStaticSound },
	//{ 0x0, "S_StartDynamicSound", (size_t)&S_StartDynamicSound },
	//{ 0x0, "S_StopSound", (size_t)&S_StopSound },
	//{ 0x0, "S_PrecacheSound", (size_t)&S_PrecacheSound },
	//{ 0x0, "S_ClearPrecache", (size_t)&S_ClearPrecache },
	//{ 0x0, "S_Update", (size_t)&S_Update },
	//{ 0x0, "S_StopAllSounds", (size_t)&S_StopAllSounds },
	//{ 0x0, "S_BeginPrecaching", (size_t)&S_BeginPrecaching },
	//{ 0x0, "S_EndPrecaching", (size_t)&S_EndPrecaching },
	//{ 0x0, "S_ExtraUpdate", (size_t)&S_ExtraUpdate },
	//{ 0x0, "S_LocalSound", (size_t)&S_LocalSound },
	//{ 0x0, "S_BlockSound", (size_t)&S_BlockSound },
	//{ 0x0, "S_PrintStats", (size_t)&S_PrintStats },
	//{ 0x0, "Voice_RecordStart", (size_t)&Voice_RecordStart },
	//{ 0x0, "Voice_IsRecording", (size_t)&Voice_IsRecording },
	//{ 0x0, "Voice_RegisterCvars", (size_t)&Voice_RegisterCvars },
	//{ 0x01DB65D0, "Voice_Deinit", (size_t)&Voice_Deinit },
	//{ 0x0, "Voice_Idle", (size_t)&Voice_Idle },
	//{ 0x0, "Voice_RecordStop", (size_t)&Voice_RecordStop },

#endif // Snd_Null_region

#ifndef Sv_Steam3_region

	{ 0x01D994C0, "_ZN13CSteam3Server19NotifyClientConnectEP8client_sPKvj", (size_t)*(void **)&pNotifyClientConnect },
	{ 0x01D98B20, "_ZN13CSteam3Server14OnLogonFailureEP27SteamServerConnectFailure_t", mfunc_ptr_cast(&CSteam3Server::OnLogonFailure) },
	{ 0x01D99A10, "_ZN13CSteam3Server24SendUpdatedServerDetailsEv", mfunc_ptr_cast(&CSteam3Server::SendUpdatedServerDetails) },
	{ 0x01D98A70, "_ZN13CSteam3Server14OnLogonSuccessEP23SteamServersConnected_t", mfunc_ptr_cast(&CSteam3Server::OnLogonSuccess) },
	{ 0x01D98A40, "_ZN13CSteam3Server18OnGSPolicyResponseEP18GSPolicyResponse_t", mfunc_ptr_cast(&CSteam3Server::OnGSPolicyResponse) },
	{ 0x01D98AE0, "_ZN13CSteam3Server10GetSteamIDEv", mfunc_ptr_cast(&CSteam3Server::GetSteamID) },
	{ 0x01D98F10, "_ZN13CSteam3Server21ClientFindFromSteamIDER8CSteamID", mfunc_ptr_cast(&CSteam3Server::ClientFindFromSteamID) },
	{ 0x01D98DF0, "_ZN13CSteam3Server17OnGSClientApproveEP17GSClientApprove_t", mfunc_ptr_cast(&CSteam3Server::OnGSClientApprove) },
	{ 0x01D99660, "_ZN13CSteam3Server22NotifyClientDisconnectEP8client_s", mfunc_ptr_cast(&CSteam3Server::NotifyClientDisconnect) },
	{ 0x01D98BC0, "_ZN13CSteam3Server20OnGSClientDenyHelperEP8client_s11EDenyReasonPKc", mfunc_ptr_cast(&CSteam3Server::OnGSClientDenyHelper) },
	{ 0x01D98B90, "_ZN13CSteam3Server14OnGSClientDenyEP14GSClientDeny_t", mfunc_ptr_cast(&CSteam3Server::OnGSClientDeny) },
	{ 0x01D98DC0, "_ZN13CSteam3Server14OnGSClientKickEP14GSClientKick_t", mfunc_ptr_cast(&CSteam3Server::OnGSClientKick) },
	{ 0x01D996B0, "_ZN13CSteam3Server19NotifyOfLevelChangeEb", mfunc_ptr_cast(&CSteam3Server::NotifyOfLevelChange) },
	{ 0x01D99110, "_ZN13CSteam3Server8ActivateEv", mfunc_ptr_cast(&CSteam3Server::Activate) },
	{ 0x01D99770, "_ZN13CSteam3Server8RunFrameEv", mfunc_ptr_cast(&CSteam3Server::RunFrame) },
	{ 0x01D99600, "_ZN13CSteam3Server16NotifyBotConnectEP8client_s", mfunc_ptr_cast(&CSteam3Server::NotifyBotConnect) },

	{ 0x01D99550, "ISteamGameServer_CreateUnauthenticatedUserConnection", (size_t)&ISteamGameServer_CreateUnauthenticatedUserConnection },
	{ 0x01D99590, "ISteamGameServer_BUpdateUserData", (size_t)&ISteamGameServer_BUpdateUserData },
	{ 0x01D995D0, "ISteamApps_BIsSubscribedApp", (size_t)&ISteamApps_BIsSubscribedApp },
	{ 0x01D99AB0, "Steam_GetCommunityName", (size_t)&Steam_GetCommunityName },
	{ 0x01D99CA0, "Steam_NotifyClientConnect", (size_t)&Steam_NotifyClientConnect },
	{ 0x01D99CD0, "Steam_NotifyBotConnect", (size_t)&Steam_NotifyBotConnect },
	{ 0x01D99D00, "Steam_NotifyClientDisconnect", (size_t)&Steam_NotifyClientDisconnect },
	{ 0x01D99D20, "Steam_NotifyOfLevelChange", (size_t)&Steam_NotifyOfLevelChange },
	{ 0x01D99D40, "Steam_Shutdown", (size_t)&Steam_Shutdown },
	{ 0x01D99D70, "Steam_Activate", (size_t)&Steam_Activate },
	{ 0x01D99DC0, "Steam_RunFrame", (size_t)&Steam_RunFrame },
	{ 0x01D99DE0, "Steam_SetCVar", (size_t)&Steam_SetCVar },
	{ 0x01D99E10, "Steam_ClientRunFrame", (size_t)&Steam_ClientRunFrame },
	{ 0x01D99E20, "Steam_InitClient", (size_t)&Steam_InitClient },
	{ 0x01D99E30, "Steam_GSInitiateGameConnection", (size_t)&Steam_GSInitiateGameConnection },
	{ 0x01D99E70, "Steam_GSTerminateGameConnection", (size_t)&Steam_GSTerminateGameConnection },
	{ 0x01D99E90, "Steam_ShutdownClient", (size_t)&Steam_ShutdownClient },
	{ 0x01D99EA0, "Steam_GSGetSteamID", (size_t)&Steam_GSGetSteamID },
	{ 0x01D99EB0, "Steam_GSBSecure", (size_t)&Steam_GSBSecure },
	{ 0x01D99ED0, "Steam_GSBLoggedOn", (size_t)&Steam_GSBLoggedOn },
	{ 0x01D99F00, "Steam_GSBSecurePreference", (size_t)&Steam_GSBSecurePreference },
	{ 0x01D99F10, "Steam_Steam3IDtoSteam2", (size_t)&Steam_Steam3IDtoSteam2 },
	{ 0x01D99F50, "Steam_StringToSteamID", (size_t)&Steam_StringToSteamID },
	{ 0x01D99FD0, "Steam_GetGSUniverse", (size_t)&Steam_GetGSUniverse },
	{ 0x01D9A020, "Steam3Server", (size_t)&Steam3Server },
	{ 0x01D9A1D0, "Steam3Client", (size_t)&Steam3Client },
	{ 0x01D9A1E0, "Master_SetMaster_f", (size_t)&Master_SetMaster_f },
	{ 0x01D9A2D0, "Steam_HandleIncomingPacket", (size_t)&Steam_HandleIncomingPacket },

#endif // Sv_Steam3_region

#ifndef Host_region

	{ 0x01D43A00, "Host_EndGame", (size_t)&Host_EndGame },
	{ 0x01D43AC0, "Host_Error", (size_t)&Host_Error },
	{ 0x01D43BA0, "Host_InitLocal", (size_t)&Host_InitLocal },
	{ 0x01D43C90, "Info_WriteVars", (size_t)&Info_WriteVars }, // NOXREF
	{ 0x01D43D70, "Host_WriteConfiguration", (size_t)&Host_WriteConfiguration }, // NOXREF
	{ 0x01D43FA0, "Host_WriteCustomConfig", (size_t)&Host_WriteCustomConfig },
	{ 0x01D44190, "SV_ClientPrintf", (size_t)&SV_ClientPrintf },
	{ 0x01D441F0, "SV_BroadcastPrintf", (size_t)&SV_BroadcastPrintf },
	{ 0x01D442A0, "Host_ClientCommands", (size_t)&Host_ClientCommands },
	{ 0x01D44310, "SV_DropClient", (size_t)&SV_DropClient },
	{ 0x01D44510, "Host_ClearClients", (size_t)&Host_ClearClients },
	{ 0x01D446B0, "Host_ShutdownServer", (size_t)&Host_ShutdownServer },
	{ 0x01D447F0, "SV_ClearClientStates", (size_t)&SV_ClearClientStates },
	{ 0x01D44830, "Host_CheckDyanmicStructures", (size_t)&Host_CheckDyanmicStructures },
	{ 0x01D44880, "Host_ClearMemory", (size_t)&Host_ClearMemory },
	{ 0x01D44900, "Host_FilterTime", (size_t)&Host_FilterTime },
	{ 0x01D44B50, "Master_IsLanGame", (size_t)&Master_IsLanGame },
	{ 0x01D44B70, "Master_Heartbeat_f", (size_t)&Master_Heartbeat_f },
	{ 0x01D44B80, "Host_ComputeFPS", (size_t)&Host_ComputeFPS },
	{ 0x01D44BB0, "Host_GetHostInfo", (size_t)&Host_GetHostInfo },
	{ 0x01D44C80, "Host_Speeds", (size_t)&Host_Speeds },
	{ 0x01D44DC0, "Host_UpdateScreen", (size_t)&Host_UpdateScreen },
	{ 0x01D44EB0, "Host_UpdateSounds", (size_t)&Host_UpdateSounds },
	{ 0x01D44F30, "Host_CheckConnectionFailure", (size_t)&Host_CheckConnectionFailure },
	{ 0x01D44F90, "_Host_Frame", (size_t)&_Host_Frame },
	{ 0x01D45150, "Host_Frame", (size_t)&Host_Frame },
	{ 0x01D45290, "CheckGore", (size_t)&CheckGore },
	{ 0x01D45450, "Host_IsSinglePlayerGame", (size_t)&Host_IsSinglePlayerGame },
	{ 0x01D45470, "Host_IsServerActive", (size_t)&Host_IsServerActive },
	{ 0x01D45480, "Host_Version", (size_t)&Host_Version },
	{ 0x01D456A0, "Host_Init", (size_t)&Host_Init },
	{ 0x01D459A0, "Host_Shutdown", (size_t)&Host_Shutdown },

#endif // Host_region

#ifndef Host_Cmd_region

	{ 0x01D45AF0, "SV_GetPlayerHulls", (size_t)&SV_GetPlayerHulls },
	{ 0x01D45B20, "Host_InitializeGameDLL", (size_t)&Host_InitializeGameDLL },
	{ 0x01D45BA0, "Host_Motd_f", (size_t)&Host_Motd_f },
	{ 0x01D45D00, "Host_Motd_Write_f", (size_t)&Host_Motd_Write_f },
	{ 0x01D45E70, "Host_GetStartTime", (size_t)&Host_GetStartTime },
	{ 0x01D45E80, "Host_UpdateStats", (size_t)&Host_UpdateStats },	// UNTESTED Linux
	{ 0x01D45FB0, "GetStatsString", (size_t)&GetStatsString },
	{ 0x01D460B0, "Host_Stats_f", (size_t)&Host_Stats_f },
	{ 0x01D460F0, "Host_Quit_f", (size_t)&Host_Quit_f },
	{ 0x01D46140, "Host_Quit_Restart_f", (size_t)&Host_Quit_Restart_f },
	{ 0x01D46250, "Host_Status_Printf", (size_t)&Host_Status_Printf },
	{ 0x01D462F0, "Host_Status_f", (size_t)&Host_Status_f },
	{ 0x01D467B0, "Host_Status_Formatted_f", (size_t)&Host_Status_Formatted_f },
	{ 0x01D46EC0, "Host_Ping_f", (size_t)&Host_Ping_f },
	{ 0x01D46F30, "Host_Map", (size_t)&Host_Map },
	{ 0x01D470D0, "Host_Map_f", (size_t)&Host_Map_f },
	{ 0x01D473B0, "Host_Career_f", (size_t)&Host_Career_f },
	{ 0x01D473D0, "Host_Maps_f", (size_t)&Host_Maps_f },
	{ 0x01D47410, "Host_Changelevel_f", (size_t)&Host_Changelevel_f },
	{ 0x01D47510, "Host_FindRecentSave", (size_t)&Host_FindRecentSave },
	{ 0x01D47600, "Host_Restart_f", (size_t)&Host_Restart_f },
	{ 0x01D47690, "Host_Reload_f", (size_t)&Host_Reload_f },
	{ 0x01D47710, "Host_Reconnect_f", (size_t)&Host_Reconnect_f },
	{ 0x01D477E0, "Host_SaveGameDirectory", (size_t)&Host_SaveGameDirectory },
	{ 0x01D47810, "Host_SavegameComment", (size_t)&Host_SavegameComment },
	{ 0x01D478B0, "Host_SaveAgeList", (size_t)&Host_SaveAgeList },
	{ 0x01D479B0, "Host_ValidSave", (size_t)&Host_ValidSave },
	{ 0x01D47A70, "SaveInit", (size_t)&SaveInit },
	{ 0x01D47B80, "SaveExit", (size_t)&SaveExit },
	{ 0x01D47BC0, "SaveGameSlot", (size_t)&SaveGameSlot },
	{ 0x01D47E70, "Host_Savegame_f", (size_t)&Host_Savegame_f },
	{ 0x01D47F60, "Host_AutoSave_f", (size_t)&Host_AutoSave_f },
	{ 0x01D47FF0, "SaveGame", (size_t)&SaveGame },
	{ 0x01D48020, "SaveReadHeader", (size_t)&SaveReadHeader },
	{ 0x01D481A0, "SaveReadComment", (size_t)&SaveReadComment }, // NOXREF
	{ 0x01D481D0, "Host_Loadgame_f", (size_t)&Host_Loadgame_f },
	{ 0x01D48220, "LoadGame", (size_t)&LoadGame },
	{ 0x01D48250, "Host_Load", (size_t)&Host_Load },
	{ 0x01D484C0, "SaveGamestate", (size_t)&SaveGamestate },
	{ 0x01D48A00, "EntityInit", (size_t)&EntityInit },
	{ 0x01D48A50, "LoadSaveData", (size_t)&LoadSaveData },
	{ 0x01D48C70, "ParseSaveTables", (size_t)&ParseSaveTables },
	{ 0x01D48DC0, "EntityPatchWrite", (size_t)&EntityPatchWrite },
	{ 0x01D48EB0, "EntityPatchRead", (size_t)&EntityPatchRead },
	{ 0x01D48F60, "LoadGamestate", (size_t)&LoadGamestate },
	{ 0x01D491D0, "EntryInTable", (size_t)&EntryInTable },
	{ 0x01D49220, "LandmarkOrigin", (size_t)&LandmarkOrigin },
	{ 0x01D49290, "EntityInSolid", (size_t)&EntityInSolid },
	{ 0x01D49320, "CreateEntityList", (size_t)&CreateEntityList },
	{ 0x01D49570, "LoadAdjacentEntities", (size_t)&LoadAdjacentEntities },
	{ 0x01D49730, "FileSize", (size_t)&FileSize },
	{ 0x01D49750, "FileCopy", (size_t)&FileCopy },
	{ 0x01D497B0, "DirectoryCopy", (size_t)&DirectoryCopy },
	{ 0x01D49880, "DirectoryExtract", (size_t)&DirectoryExtract },
	{ 0x01D49930, "DirectoryCount", (size_t)&DirectoryCount },
	{ 0x01D49970, "Host_ClearSaveDirectory", (size_t)&Host_ClearSaveDirectory },
	{ 0x01D49AB0, "Host_ClearGameState", (size_t)&Host_ClearGameState },
	{ 0x01D49AD0, "Host_Changelevel2_f", (size_t)&Host_Changelevel2_f },
	{ 0x01D49CB0, "Host_Version_f", (size_t)&Host_Version_f },
	{ 0x01D49CE0, "Host_FullInfo_f", (size_t)&Host_FullInfo_f },
	{ 0x01D49E00, "Host_KillVoice_f", (size_t)&Host_KillVoice_f }, // NOXREF
	{ 0x01D49E10, "Host_SetInfo_f", (size_t)&Host_SetInfo_f },
	{ 0x01D49EB0, "Host_Say", (size_t)&Host_Say },
	{ 0x01D4A060, "Host_Say_f", (size_t)&Host_Say_f },
	{ 0x01D4A070, "Host_Say_Team_f", (size_t)&Host_Say_Team_f },
	{ 0x01D4A080, "Host_Tell_f", (size_t)&Host_Tell_f },
	{ 0x01D4A230, "Host_Kill_f", (size_t)&Host_Kill_f },
	{ 0x01D4A280, "Host_TogglePause_f", (size_t)&Host_TogglePause_f },
	{ 0x01D4A320, "Host_Pause_f", (size_t)&Host_Pause_f },
	{ 0x01D4A380, "Host_Unpause_f", (size_t)&Host_Unpause_f },
	{ 0x01D4A620, "Host_Interp_f", (size_t)&Host_Interp_f },
	{ 0x01D4A650, "Host_NextDemo", (size_t)&Host_NextDemo },
	{ 0x01D4A700, "Host_Startdemos_f", (size_t)&Host_Startdemos_f },
	{ 0x01D4A7C0, "Host_Demos_f", (size_t)&Host_Demos_f },
	{ 0x01D4A7F0, "Host_Stopdemo_f", (size_t)&Host_Stopdemo_f },
	{ 0x01D4A810, "Host_EndSection", (size_t)&Host_EndSection },	// NOXREF
	{ 0x01D4A910, "Host_Soundfade_f", (size_t)&Host_Soundfade_f },
	{ 0x01D4AA10, "Host_KillServer_f", (size_t)&Host_KillServer_f },
	{ 0x01D4AA50, "Host_VoiceRecordStart_f", (size_t)&Host_VoiceRecordStart_f },
	{ 0x01D4AAA0, "Host_VoiceRecordStop_f", (size_t)&Host_VoiceRecordStop_f },
	{ 0x01D4AC90, "Host_Crash_f", (size_t)&Host_Crash_f },	// NOXREF
	{ 0x01D4ACA0, "Host_InitCommands", (size_t)&Host_InitCommands },
	{ 0x01D4B060, "SV_CheckBlendingInterface", (size_t)&SV_CheckBlendingInterface },
	{ 0x01D4B0D0, "SV_CheckSaveGameCommentInterface", (size_t)&SV_CheckSaveGameCommentInterface },

#endif // Host_Cmd_region

#ifndef Pmove_region

	{ 0x01D5B490, "PM_AddToTouched", (size_t)&PM_AddToTouched },
	{ 0x01D5B530, "PM_StuckTouch", (size_t)&PM_StuckTouch },
	{ 0x01D5B590, "PM_Init", (size_t)&PM_Init },

#endif // Pmove_region

#ifndef Pmovetst_region

	{ 0x01D5B8F0, "PM_TraceModel", (size_t)&PM_TraceModel },
	{ 0x01D5B990, "PM_GetModelBounds", (size_t)&PM_GetModelBounds },
	{ 0x01D5B9C0, "PM_GetModelType", (size_t)&PM_GetModelType },
	{ 0x01D5B9D0, "PM_InitBoxHull", (size_t)&PM_InitBoxHull },
	{ 0x01D5BA60, "PM_HullForBox", (size_t)&PM_HullForBox },
	{ 0x01D5BAB0, "PM_HullPointContents", (size_t)&PM_HullPointContents },
	{ 0x01D5BB50, "PM_LinkContents", (size_t)&PM_LinkContents },
	{ 0x01D5BC10, "PM_PointContents", (size_t)&PM_PointContents },
	{ 0x01D5BCA0, "PM_WaterEntity", (size_t)&PM_WaterEntity },
	{ 0x01D5BD10, "PM_TruePointContents", (size_t)&PM_TruePointContents },
	{ 0x01D5BD40, "PM_HullForStudioModel", (size_t)&PM_HullForStudioModel },
	{ 0x01D5BDF0, "PM_HullForBsp", (size_t)&PM_HullForBsp },
	{ 0x01D5BEA0, "_PM_TestPlayerPosition", (size_t)&_PM_TestPlayerPosition },
	{ 0x01D5C190, "PM_TestPlayerPosition", (size_t)&PM_TestPlayerPosition },
	{ 0x01D5C1B0, "PM_TestPlayerPositionEx", (size_t)&PM_TestPlayerPositionEx },
	{ 0x01D5C1D0, "_PM_PlayerTrace", (size_t)&_PM_PlayerTrace },
	{ 0x01D5C890, "PM_PlayerTrace", (size_t)&PM_PlayerTrace },
	{ 0x01D5C8F0, "PM_PlayerTraceEx", (size_t)&PM_PlayerTraceEx },
	{ 0x01D5C9C8, "PM_TraceLine", (size_t)&PM_TraceLine },
	{ 0x01D5CA60, "PM_TraceLineEx", (size_t)&PM_TraceLineEx },
	{ 0x01D5CB20, "PM_RecursiveHullCheck", (size_t)&PM_RecursiveHullCheck },

#endif // Pmovetst_region

#ifndef Pr_Edict_region

	{ 0x01D61510, "ED_ClearEdict", (size_t)&ED_ClearEdict },
	{ 0x01D61550, "ED_Alloc", (size_t)&ED_Alloc },
	{ 0x01D61610, "ED_Free", (size_t)&ED_Free },
	{ 0x01D616D0, "ED_Count", (size_t)&ED_Count },	// NOXREF
	{ 0x01D61770, "ED_NewString", (size_t)&ED_NewString },
	{ 0x01D617D0, "ED_ParseEdict", (size_t)&ED_ParseEdict },
	{ 0x01D61A70, "ED_LoadFromFile", (size_t)&ED_LoadFromFile },
	{ 0x01D61B80, "PR_Init", (size_t)&PR_Init },	// NOXREF
	{ 0x01D61B90, "EDICT_NUM", (size_t)&EDICT_NUM },
	{ 0x01D61BD0, "NUM_FOR_EDICT", (size_t)&NUM_FOR_EDICT },
	{ 0x01D61C10, "SuckOutClassname", (size_t)&SuckOutClassname },
	{ 0x01D61CD0, "ReleaseEntityDLLFields", (size_t)&ReleaseEntityDLLFields },
	{ 0x01D61CF0, "InitEntityDLLFields", (size_t)&InitEntityDLLFields },
	{ 0x01D61D00, "PvAllocEntPrivateData", (size_t)&PvAllocEntPrivateData },
	{ 0x01D61D30, "PvEntPrivateData", (size_t)&PvEntPrivateData },
	{ 0x01D61D50, "FreeEntPrivateData", (size_t)&FreeEntPrivateData },
	{ 0x01D61D90, "FreeAllEntPrivateData", (size_t)&FreeAllEntPrivateData },
	{ 0x01D61DD0, "PEntityOfEntOffset", (size_t)&PEntityOfEntOffset },
	{ 0x01D61DE0, "EntOffsetOfPEntity", (size_t)&EntOffsetOfPEntity },
	{ 0x01D61DF0, "IndexOfEdict", (size_t)&IndexOfEdict },
	{ 0x01D61E40, "PEntityOfEntIndex", (size_t)&PEntityOfEntIndex },
	{ 0x01D61E80, "SzFromIndex", (size_t)&SzFromIndex },
	{ 0x01D61E90, "GetVarsOfEnt", (size_t)&GetVarsOfEnt },
	{ 0x01D61EA0, "FindEntityByVars", (size_t)&FindEntityByVars },
	{ 0x01D61EE0, "CVarGetFloat", (size_t)&CVarGetFloat },
	{ 0x01D61F00, "CVarGetString", (size_t)&CVarGetString },
	{ 0x01D61F20, "CVarGetPointer", (size_t)&CVarGetPointer },
	{ 0x01D61F40, "CVarSetFloat", (size_t)&CVarSetFloat },
	{ 0x01D61F60, "CVarSetString", (size_t)&CVarSetString },
	{ 0x01D61F80, "CVarRegister", (size_t)&CVarRegister },
	{ 0x01D61FA0, "AllocEngineString", (size_t)&AllocEngineString },
	{ 0x01D61FC0, "SaveSpawnParms", (size_t)&SaveSpawnParms },
	{ 0x01D61FF0, "GetModelPtr", (size_t)&GetModelPtr },

#endif // Pr_Edict_region

#ifndef Pr_Cmds_region

	{ 0x01D5CF00, "PF_makevectors_I", (size_t)&PF_makevectors_I },
	{ 0x01D5CF20, "PF_Time", (size_t)&PF_Time },
	{ 0x01D5CF40, "PF_setorigin_I", (size_t)&PF_setorigin_I },
	{ 0x01D5CF80, "SetMinMaxSize", (size_t)&SetMinMaxSize },
	{ 0x01D5D030, "PF_setsize_I", (size_t)&PF_setsize_I },
	{ 0x01D5D050, "PF_setmodel_I", (size_t)&PF_setmodel_I },
	{ 0x01D5D0F0, "PF_modelindex", (size_t)&PF_modelindex },
	{ 0x01D5D110, "ModelFrames", (size_t)&ModelFrames },
	{ 0x01D5D150, "PF_bprint", (size_t)&PF_bprint },
	{ 0x01D5D170, "PF_sprint", (size_t)&PF_sprint },
	{ 0x01D5D1D0, "ServerPrint", (size_t)&ServerPrint },
	{ 0x01D5D1F0, "ClientPrintf", (size_t)&ClientPrintf },
	{ 0x01D5D2A0, "PF_vectoyaw_I", (size_t)&PF_vectoyaw_I },
	{ 0x01D5D330, "PF_vectoangles_I", (size_t)&PF_vectoangles_I },
	{ 0x01D5D350, "PF_particle_I", (size_t)&PF_particle_I },
	{ 0x01D5D380, "PF_ambientsound_I", (size_t)&PF_ambientsound_I },
	{ 0x01D5D4C0, "PF_sound_I", (size_t)&PF_sound_I },
	{ 0x01D5D5A0, "PF_traceline_Shared", (size_t)&PF_traceline_Shared },
	{ 0x01D5D600, "PF_traceline_DLL", (size_t)&PF_traceline_DLL },
	{ 0x01D5D6C0, "TraceHull", (size_t)&TraceHull },
	{ 0x01D5D770, "TraceSphere", (size_t)&TraceSphere },
	{ 0x01D5D780, "TraceModel", (size_t)&TraceModel },
	{ 0x01D5D890, "SurfaceAtPoint", (size_t)&SurfaceAtPoint },
	{ 0x01D5DA90, "TraceTexture", (size_t)&TraceTexture },
	{ 0x01D5DCA0, "PF_TraceToss_Shared", (size_t)&PF_TraceToss_Shared },
	{ 0x01D5DCE0, "SV_SetGlobalTrace", (size_t)&SV_SetGlobalTrace },
	{ 0x01D5DD80, "PF_TraceToss_DLL", (size_t)&PF_TraceToss_DLL },
	{ 0x01D5DE40, "TraceMonsterHull", (size_t)&TraceMonsterHull },
	{ 0x01D5DF10, "PF_newcheckclient", (size_t)&PF_newcheckclient },
	{ 0x01D5DFF0, "PF_checkclient_I", (size_t)&PF_checkclient_I },
	{ 0x01D5E110, "PVSNode", (size_t)&PVSNode },
	{ 0x01D5E1D0, "PVSMark", (size_t)&PVSMark },
	{ 0x01D5E250, "PVSFindEntities", (size_t)&PVSFindEntities },
	{ 0x01D5E360, "ValidCmd", (size_t)&ValidCmd },
	{ 0x01D5E390, "PF_stuffcmd_I", (size_t)&PF_stuffcmd_I },
	{ 0x01D5E450, "PF_localcmd_I", (size_t)&PF_localcmd_I },
	{ 0x01D5E480, "PF_localexec_I", (size_t)&PF_localexec_I },
	{ 0x01D5E490, "FindEntityInSphere", (size_t)&FindEntityInSphere },
	{ 0x01D5E5C0, "PF_Spawn_I", (size_t)&PF_Spawn_I },
	{ 0x01D5E5D0, "CreateNamedEntity", (size_t)&CreateNamedEntity },
	{ 0x01D5E640, "PF_Remove_I", (size_t)&PF_Remove_I },
	{ 0x01D5E920, "PF_find_Shared", (size_t)&PF_find_Shared },
	{ 0x01D5E660, "iGetIndex", (size_t)&iGetIndex },
	{ 0x01D5E8C0, "FindEntityByString", (size_t)&FindEntityByString },
	{ 0x01D5E9B0, "GetEntityIllum", (size_t)&GetEntityIllum },
	{ 0x01D5EA50, "PR_IsEmptyString", (size_t)&PR_IsEmptyString },
	{ 0x01D5EA60, "PF_precache_sound_I", (size_t)&PF_precache_sound_I },
	{ 0x01D5EB50, "EV_Precache", (size_t)&EV_Precache },
	{ 0x01D5ECD0, "EV_PlayReliableEvent", (size_t)&EV_PlayReliableEvent },
	{ 0x01D5EE10, "EV_Playback", (size_t)&EV_Playback },
	{ 0x01D5F190, "EV_SV_Playback", (size_t)&EV_SV_Playback },
	{ 0x01D5F210, "PF_precache_model_I", (size_t)&PF_precache_model_I },
	{ 0x01D5F310, "PF_precache_generic_I", (size_t)&PF_precache_generic_I },
	{ 0x01D5F3E0, "PF_IsMapValid_I", (size_t)&PF_IsMapValid_I },
	{ 0x01D5F430, "PF_NumberOfEntities_I", (size_t)&PF_NumberOfEntities_I },
	{ 0x01D5F460, "PF_GetInfoKeyBuffer_I", (size_t)&PF_GetInfoKeyBuffer_I },
	{ 0x01D5F4B0, "PF_InfoKeyValue_I", (size_t)&PF_InfoKeyValue_I },
	{ 0x01D5F4D0, "PF_SetKeyValue_I", (size_t)&PF_SetKeyValue_I },
	{ 0x01D5F530, "PF_RemoveKey_I", (size_t)&PF_RemoveKey_I },
	{ 0x01D5F550, "PF_SetClientKeyValue_I", (size_t)&PF_SetClientKeyValue_I },
	{ 0x01D5F5E0, "PF_walkmove_I", (size_t)&PF_walkmove_I },
	{ 0x01D5F690, "PF_droptofloor_I", (size_t)&PF_droptofloor_I },
	{ 0x01D5F780, "PF_DecalIndex", (size_t)&PF_DecalIndex },
	{ 0x01D5F7D0, "PF_lightstyle_I", (size_t)&PF_lightstyle_I },
	{ 0x01D5F860, "PF_checkbottom_I", (size_t)&PF_checkbottom_I },
	{ 0x01D5F880, "PF_pointcontents_I", (size_t)&PF_pointcontents_I },
	{ 0x01D5F8A0, "PF_aim_I", (size_t)&PF_aim_I },
	{ 0x01D5FB60, "PF_changeyaw_I", (size_t)&PF_changeyaw_I },
	{ 0x01D5FC50, "PF_changepitch_I", (size_t)&PF_changepitch_I },
	{ 0x01D5FD40, "PF_setview_I", (size_t)&PF_setview_I },
	{ 0x01D5FDC0, "PF_crosshairangle_I", (size_t)&PF_crosshairangle_I },
	{ 0x01D5FEC0, "PF_CreateFakeClient_I", (size_t)&PF_CreateFakeClient_I },
	{ 0x01D60070, "PF_RunPlayerMove_I", (size_t)&PF_RunPlayerMove_I },
	{ 0x01D60180, "WriteDest_Parm", (size_t)&WriteDest_Parm },
	{ 0x01D60260, "PF_MessageBegin_I", (size_t)&PF_MessageBegin_I },
	{ 0x01D60350, "PF_MessageEnd_I", (size_t)&PF_MessageEnd_I },
	{ 0x01D605E0, "PF_WriteByte_I", (size_t)&PF_WriteByte_I },
	{ 0x01D60610, "PF_WriteChar_I", (size_t)&PF_WriteChar_I },
	{ 0x01D60640, "PF_WriteShort_I", (size_t)&PF_WriteShort_I },
	{ 0x01D60670, "PF_WriteLong_I", (size_t)&PF_WriteLong_I },
	{ 0x01D606A0, "PF_WriteAngle_I", (size_t)&PF_WriteAngle_I },
	{ 0x01D606D0, "PF_WriteCoord_I", (size_t)&PF_WriteCoord_I },
	{ 0x01D60710, "PF_WriteString_I", (size_t)&PF_WriteString_I },
	{ 0x01D60740, "PF_WriteEntity_I", (size_t)&PF_WriteEntity_I },
	{ 0x01D60770, "PF_makestatic_I", (size_t)&PF_makestatic_I },
	{ 0x01D608C0, "PF_StaticDecal", (size_t)&PF_StaticDecal },
	{ 0x01D60940, "PF_setspawnparms_I", (size_t)&PF_setspawnparms_I },
	{ 0x01D60970, "PF_changelevel_I", (size_t)&PF_changelevel_I },
	{ 0x01D609D0, "SeedRandomNumberGenerator", (size_t)&SeedRandomNumberGenerator },
	{ 0x01D60A10, "ran1", (size_t)&ran1 },
	{ 0x01D60AE0, "fran1", (size_t)&fran1 },
	{ 0x01D60B30, "RandomFloat", (size_t)&RandomFloat },
	{ 0x01D60B60, "RandomLong", (size_t)&RandomLong },
	{ 0x01D60BC0, "PF_FadeVolume", (size_t)&PF_FadeVolume },
	{ 0x01D60C60, "PF_SetClientMaxspeed", (size_t)&PF_SetClientMaxspeed },
	{ 0x01D60CA0, "PF_GetPlayerUserId", (size_t)&PF_GetPlayerUserId },
	{ 0x01D60CF0, "PF_GetPlayerWONId", (size_t)&PF_GetPlayerWONId },
	{ 0x01D60D00, "PF_GetPlayerAuthId", (size_t)&PF_GetPlayerAuthId },
	{ 0x01D60E00, "PF_BuildSoundMsg_I", (size_t)&PF_BuildSoundMsg_I },
	{ 0x01D60E50, "PF_IsDedicatedServer", (size_t)&PF_IsDedicatedServer },
	{ 0x01D60E60, "PF_GetPhysicsInfoString", (size_t)&PF_GetPhysicsInfoString },
	{ 0x01D60EB0, "PF_GetPhysicsKeyValue", (size_t)&PF_GetPhysicsKeyValue },
	{ 0x01D60F10, "PF_SetPhysicsKeyValue", (size_t)&PF_SetPhysicsKeyValue },
	{ 0x01D60F70, "PF_GetCurrentPlayer", (size_t)&PF_GetCurrentPlayer },
	{ 0x01D60FB0, "PF_CanSkipPlayer", (size_t)&PF_CanSkipPlayer },
	{ 0x01D61000, "PF_SetGroupMask", (size_t)&PF_SetGroupMask },
	{ 0x01D61020, "PF_CreateInstancedBaseline", (size_t)&PF_CreateInstancedBaseline },
	{ 0x01D61070, "PF_Cvar_DirectSet", (size_t)&PF_Cvar_DirectSet },
	{ 0x01D61090, "PF_ForceUnmodified", (size_t)&PF_ForceUnmodified },
	{ 0x01D611B0, "PF_GetPlayerStats", (size_t)&PF_GetPlayerStats },
	//{ 0x01D61230, "QueryClientCvarValueCmd", (size_t)&QueryClientCvarValueCmd }, //NOXREF
	//{ 0x01D612D0, "QueryClientCvarValueCmd2", (size_t)&QueryClientCvarValueCmd2 }, //NOXREF
	{ 0x01D61390, "QueryClientCvarValue", (size_t)&QueryClientCvarValue },
	{ 0x01D61410, "QueryClientCvarValue2", (size_t)&QueryClientCvarValue2 },
	{ 0x01D614A0, "hudCheckParm", (size_t)&hudCheckParm },
	{ 0x01D614F0, "EngCheckParm", (size_t)&EngCheckParm },

#endif // Pr_Cmds_region

#ifndef Mathlib_region

	{ 0x01D4F570, "anglemod", (size_t)&anglemod },
	//{ 0x, "BOPS_Error", (size_t)&BOPS_Error },
	{ 0x01DBEE44, "BoxOnPlaneSide", (size_t)&BoxOnPlaneSide },
	//{ 0x, "InvertMatrix", (size_t)&InvertMatrix },
	{ 0x01D4FD10, "AngleVectors", (size_t)&AngleVectors },
	{ 0x01D4FE70, "AngleVectorsTranspose", (size_t)&AngleVectorsTranspose },
	{ 0x01D4FF90, "AngleMatrix", (size_t)&AngleMatrix },
	//{ 0x, "AngleIMatrix", (size_t)&AngleIMatrix },
	//{ 0x, "NormalizeAngles", (size_t)&NormalizeAngles },
	//{ 0x, "InterpolateAngles", (size_t)&InterpolateAngles },
	{ 0x01D502C0, "VectorTransform", (size_t)&VectorTransform },
	{ 0x01D50320, "VectorCompare", (size_t)&VectorCompare },
	{ 0x01D50350, "VectorMA", (size_t)&VectorMA },
	//{ 0x, "_DotProduct", (size_t)&_DotProduct },
	//{ 0x, "_VectorSubtract", (size_t)&_VectorSubtract },
	//{ 0x, "_VectorAdd", (size_t)&_VectorAdd },
	//{ 0x, "_VectorCopy", (size_t)&_VectorCopy },
	{ 0x01D50420, "CrossProduct", (size_t)&CrossProduct },
	{ 0x01D50460, "Length", (size_t)&Length },
	{ 0x01D504A0, "VectorNormalize", (size_t)&VectorNormalize },
	//{ 0x, "VectorInverse", (size_t)&VectorInverse },
	{ 0x01D50550, "VectorScale", (size_t)&VectorScale },
	//{ 0x, "Q_log2", (size_t)&Q_log2 },
	//{ 0x, "VectorMatrix", (size_t)&VectorMatrix },
	{ 0x01D50640, "VectorAngles", (size_t)&VectorAngles },
	//{ 0x, "R_ConcatRotations", (size_t)&R_ConcatRotations },
	{ 0x01D50850, "R_ConcatTransforms", (size_t)&R_ConcatTransforms },
	//{ 0x, "FloorDivMod", (size_t)&FloorDivMod },
	//{ 0x, "GreatestCommonDivisor", (size_t)&GreatestCommonDivisor },
	//{ 0x, "Invert24To16", (size_t)&Invert24To16 },

#endif // Mathlib_region

#ifndef World_region

	{ 0x01DB9050, "ClearLink", (size_t)&ClearLink },
	{ 0x01DB9060, "RemoveLink", (size_t)&RemoveLink },
	{ 0x01DB9080, "InsertLinkBefore", (size_t)&InsertLinkBefore },
	//{ 0x01DB90A0, "InsertLinkAfter", (size_t)&InsertLinkAfter },	//NOXREF
	{ 0x01DB90C0, "SV_InitBoxHull", (size_t)&SV_InitBoxHull },
	{ 0x01DB9180, "SV_HullForBox", (size_t)&SV_HullForBox },
	//{ 0x01DB91D0, "SV_HullForBeam", (size_t)&SV_HullForBeam },	//NOXREF
	{ 0x01DB9640, "SV_HullForBsp", (size_t)&SV_HullForBsp },
	{ 0x01DB9780, "SV_HullForEntity", (size_t)&SV_HullForEntity },
	{ 0x01DB9850, "SV_CreateAreaNode", (size_t)&SV_CreateAreaNode },
	{ 0x01DB9970, "SV_ClearWorld", (size_t)&SV_ClearWorld },
	{ 0x01DB99B0, "SV_UnlinkEdict", (size_t)&SV_UnlinkEdict },
	{ 0x01DB99E0, "SV_TouchLinks", (size_t)&SV_TouchLinks },
	{ 0x01DB9BC0, "SV_FindTouchedLeafs", (size_t)&SV_FindTouchedLeafs },
	{ 0x01DB9CF0, "SV_LinkEdict", (size_t)&SV_LinkEdict },
	{ 0x01DC0614, "SV_HullPointContents", (size_t)&SV_HullPointContents },
	{ 0x01DB9EB0, "SV_LinkContents", (size_t)&SV_LinkContents },
	{ 0x01DBA0A0, "SV_PointContents", (size_t)&SV_PointContents },
	{ 0x01DBA0F0, "SV_TestEntityPosition", (size_t)&SV_TestEntityPosition },
	{ 0x01DBA160, "SV_RecursiveHullCheck", (size_t)&SV_RecursiveHullCheck },
	{ 0x01DBA550, "SV_SingleClipMoveToEntity", (size_t)&SV_SingleClipMoveToEntity },
	{ 0x01DBA950, "SV_ClipMoveToEntity", (size_t)&SV_ClipMoveToEntity },
	{ 0x01DBA990, "SV_ClipToLinks", (size_t)&SV_ClipToLinks },
	{ 0x01DBAC60, "SV_ClipToWorldbrush", (size_t)&SV_ClipToWorldbrush },
	{ 0x01DBAE00, "SV_MoveBounds", (size_t)&SV_MoveBounds },
	{ 0x01DBAE80, "SV_MoveNoEnts", (size_t)&SV_MoveNoEnts },
	{ 0x01DBAFC0, "SV_Move", (size_t)&SV_Move },

#endif // World_region

#ifndef Sv_Phys_region

	//{ 0x01D94A90, "SV_CheckAllEnts", (size_t)&SV_CheckAllEnts },	//NOXREF
	{ 0x01D94B00, "SV_CheckVelocity", (size_t)&SV_CheckVelocity },
	{ 0x01D94C00, "SV_RunThink", (size_t)&SV_RunThink },
	{ 0x01D94CB0, "SV_Impact", (size_t)&SV_Impact },
	{ 0x01D94D50, "ClipVelocity", (size_t)&ClipVelocity },
	{ 0x01D94E10, "SV_FlyMove", (size_t)&SV_FlyMove },
	{ 0x01D952F0, "SV_AddGravity", (size_t)&SV_AddGravity },
	//{ 0x01D95370, "SV_AddCorrectGravity", (size_t)&SV_AddCorrectGravity },	//NOXREF
	//{ 0x01D953F0, "SV_FixupGravityVelocity", (size_t)&SV_FixupGravityVelocity },	//NOXREF
	{ 0x01D95450, "SV_PushEntity", (size_t)&SV_PushEntity },
	{ 0x01D95550, "SV_PushMove", (size_t)&SV_PushMove },
	{ 0x01D958F0, "SV_PushRotate", (size_t)&SV_PushRotate },
	{ 0x01D95F00, "SV_Physics_Pusher", (size_t)&SV_Physics_Pusher },
	{ 0x01D960F0, "SV_CheckWater", (size_t)&SV_CheckWater },
	{ 0x01D96290, "SV_RecursiveWaterLevel", (size_t)&SV_RecursiveWaterLevel },
	{ 0x01D96310, "SV_Submerged", (size_t)&SV_Submerged },
	{ 0x01D96410, "SV_Physics_None", (size_t)&SV_Physics_None },
	{ 0x01D96430, "SV_Physics_Follow", (size_t)&SV_Physics_Follow },
	{ 0x01D964F0, "SV_Physics_Noclip", (size_t)&SV_Physics_Noclip },
	{ 0x01D96560, "SV_CheckWaterTransition", (size_t)&SV_CheckWaterTransition },
	{ 0x01D96740, "SV_Physics_Toss", (size_t)&SV_Physics_Toss },
	{ 0x01D96B20, "PF_WaterMove", (size_t)&PF_WaterMove },
	{ 0x01D96E70, "SV_Physics_Step", (size_t)&SV_Physics_Step },
	{ 0x01D97240, "SV_Physics", (size_t)&SV_Physics },
	{ 0x01D97480, "SV_Trace_Toss", (size_t)&SV_Trace_Toss },

#endif // Sv_Phys_region

#ifndef Sv_Move_region

	{ 0x01D93A40, "SV_CheckBottom", (size_t)&SV_CheckBottom },
	{ 0x01D93CB0, "SV_movetest", (size_t)&SV_movetest },
	{ 0x01D93ED0, "SV_movestep", (size_t)&SV_movestep },
	{ 0x01D94250, "SV_StepDirection", (size_t)&SV_StepDirection },
	{ 0x01D942D0, "SV_FlyDirection", (size_t)&SV_FlyDirection },
	{ 0x01D94310, "SV_FixCheckBottom", (size_t)&SV_FixCheckBottom },
	//{ 0x01D94330, "SV_NewChaseDir", (size_t)&SV_NewChaseDir }, //NOXREF
	//{ 0x01D94620, "SV_CloseEnough", (size_t)&SV_CloseEnough }, //NOXREF
	//{ 0x01D94680, "SV_ReachedGoal", (size_t)&SV_ReachedGoal }, //NOXREF
	{ 0x01D946D0, "SV_NewChaseDir2", (size_t)&SV_NewChaseDir2 },
	{ 0x01D949C0, "SV_MoveToOrigin_I", (size_t)&SV_MoveToOrigin_I },

#endif // Sv_Move_region

#ifndef Sv_pmove_region

	{ 0x01D975F0, "PM_SV_PlaybackEventFull", (size_t)&PM_SV_PlaybackEventFull },
	{ 0x01D97630, "PM_SV_PlaySound", (size_t)&PM_SV_PlaySound },
	{ 0x01D97670, "PM_SV_TraceTexture", (size_t)&PM_SV_TraceTexture },

#endif // Sv_pmove_region

#ifndef R_Studio_region

	{ 0x01D6F260, "SV_InitStudioHull", (size_t)&SV_InitStudioHull },
	{ 0x01D6F2F0, "R_CheckStudioCache", (size_t)&R_CheckStudioCache },
	//{ 0x01D6F3C0, "R_AddToStudioCache", (size_t)&R_AddToStudioCache },	// NOXREF
	{ 0x01D6F500, "AngleQuaternion", (size_t)&AngleQuaternion },
	{ 0x01D6F600, "QuaternionSlerp", (size_t)&QuaternionSlerp },
	{ 0x01D6F810, "QuaternionMatrix", (size_t)&QuaternionMatrix },
	{ 0x01D6F900, "R_StudioCalcBoneAdj", (size_t)&R_StudioCalcBoneAdj },
	{ 0x01D6FB60, "R_StudioCalcBoneQuaterion", (size_t)&R_StudioCalcBoneQuaterion },
	{ 0x01D6FD20, "R_StudioCalcBonePosition", (size_t)&R_StudioCalcBonePosition },
	{ 0x01D6FE70, "R_StudioSlerpBones", (size_t)&R_StudioSlerpBones },
	{ 0x01D6FF80, "R_GetAnim", (size_t)&R_GetAnim },
	{ 0x01D70020, "SV_StudioSetupBones", (size_t)&SV_StudioSetupBones },
	{ 0x01D70390, "SV_SetStudioHullPlane", (size_t)&SV_SetStudioHullPlane },
	{ 0x01D70400, "R_StudioHull", (size_t)&R_StudioHull },
	{ 0x01D70A40, "SV_HitgroupForStudioHull", (size_t)&SV_HitgroupForStudioHull },
	//{ 0x01D70A50, "R_InitStudioCache", (size_t)&R_InitStudioCache },	// NOXREF
	//{ 0x01D70A90, "R_FlushStudioCache", (size_t)&R_FlushStudioCache },		// NOXREF
	{ 0x01D70AA0, "R_StudioBodyVariations", (size_t)&R_StudioBodyVariations },
	{ 0x01D70AF0, "R_StudioPlayerBlend", (size_t)&R_StudioPlayerBlend },
	{ 0x01D70BD0, "SV_HullForStudioModel", (size_t)&SV_HullForStudioModel },
	{ 0x01D70E10, "DoesSphereIntersect", (size_t)&DoesSphereIntersect },
	{ 0x01D70ED0, "SV_CheckSphereIntersection", (size_t)&SV_CheckSphereIntersection },
	{ 0x01D70FF0, "AnimationAutomove", (size_t)&AnimationAutomove },
	{ 0x01D71000, "GetBonePosition", (size_t)&GetBonePosition },
	{ 0x01D710A0, "GetAttachment", (size_t)&GetAttachment },
	{ 0x01D71170, "ModelFrameCount", (size_t)&ModelFrameCount },
	{ 0x01D76260, "R_StudioBoundVertex", (size_t)&R_StudioBoundVertex },
	{ 0x01D762F0, "R_StudioBoundBone", (size_t)&R_StudioBoundBone },
	{ 0x01D76380, "R_StudioAccumulateBoneVerts", (size_t)&R_StudioAccumulateBoneVerts },
	{ 0x01D76420, "R_StudioComputeBounds", (size_t)&R_StudioComputeBounds },
	{ 0x01D76600, "R_GetStudioBounds", (size_t)&R_GetStudioBounds },
	{ 0x01D76C40, "R_ResetSvBlending", (size_t)&R_ResetSvBlending },

#endif // R_Studio_region

#ifndef Net_ws_region

	{ 0x01D57050, "NET_ThreadLock", (size_t)&NET_ThreadLock },
	{ 0x01D57070, "NET_ThreadUnlock", (size_t)&NET_ThreadUnlock },
	{ 0x01D57090, "Q_ntohs", (size_t)&Q_ntohs },
	{ 0x01D570A0, "NetadrToSockadr", (size_t)&NetadrToSockadr },
	{ 0x01D57160, "SockadrToNetadr", (size_t)&SockadrToNetadr },
	{ 0x01D571D0, "NET_HostToNetShort", (size_t)&NET_HostToNetShort },	// NOXREF
	{ 0x01D571E0, "NET_CompareAdr", (size_t)&NET_CompareAdr },
	{ 0x01D57270, "NET_CompareClassBAdr", (size_t)&NET_CompareClassBAdr },
	{ 0x01D572C0, "NET_IsReservedAdr", (size_t)&NET_IsReservedAdr },
	{ 0x01D57310, "NET_CompareBaseAdr", (size_t)&NET_CompareBaseAdr },
	{ 0x01D57380, "NET_AdrToString", (size_t)&NET_AdrToString },
	{ 0x01D574A0, "NET_BaseAdrToString", (size_t)&NET_BaseAdrToString },
	{ 0x01D575A0, "NET_StringToSockaddr", (size_t)&NET_StringToSockaddr },
	{ 0x01D57820, "NET_StringToAdr", (size_t)&NET_StringToAdr },
	{ 0x01D57890, "NET_IsLocalAddress", (size_t)&NET_IsLocalAddress },
	{ 0x01D578A0, "NET_ErrorString", (size_t)&NET_ErrorString },
	{ 0x01D57B20, "NET_TransferRawData", (size_t)&NET_TransferRawData },
	{ 0x01D57B50, "NET_GetLoopPacket", (size_t)&NET_GetLoopPacket },
	{ 0x01D57C00, "NET_SendLoopPacket", (size_t)&NET_SendLoopPacket },
	{ 0x01D57C80, "NET_RemoveFromPacketList", (size_t)&NET_RemoveFromPacketList },
	{ 0x01D57CA0, "NET_CountLaggedList", (size_t)&NET_CountLaggedList },
	{ 0x01D57CC0, "NET_ClearLaggedList", (size_t)&NET_ClearLaggedList },
	{ 0x01D57D10, "NET_AddToLagged", (size_t)&NET_AddToLagged },
	{ 0x01D57D80, "NET_AdjustLag", (size_t)&NET_AdjustLag },
	{ 0x01D57EC0, "NET_LagPacket", (size_t)&NET_LagPacket },
	{ 0x01D58090, "NET_FlushSocket", (size_t)&NET_FlushSocket },
	{ 0x01D580E0, "NET_GetLong", (size_t)&NET_GetLong },
	{ 0x01D582F0, "NET_QueuePacket", (size_t)&NET_QueuePacket },
#ifdef __linux__
	{ 0x0, "NET_Sleep_Timeout", (size_t)&NET_Sleep_Timeout },
#endif // __linux__
	{ 0x01D584A0, "NET_Sleep", (size_t)&NET_Sleep },
	{ 0x01D58630, "NET_StartThread", (size_t)&NET_StartThread },
	{ 0x01D586B0, "NET_StopThread", (size_t)&NET_StopThread },
	{ 0x01D586F0, "net_malloc", (size_t)&net_malloc },
	{ 0x01D58710, "NET_AllocMsg", (size_t)&NET_AllocMsg },
	{ 0x01D58760, "NET_FreeMsg", (size_t)&NET_FreeMsg },
	{ 0x01D587A0, "NET_GetPacket", (size_t)&NET_GetPacket },
	{ 0x01D588B0, "NET_AllocateQueues", (size_t)&NET_AllocateQueues },
	{ 0x01D588F0, "NET_FlushQueues", (size_t)&NET_FlushQueues },
	{ 0x01D58960, "NET_SendLong", (size_t)&NET_SendLong },
	{ 0x01D58AE0, "NET_SendPacket", (size_t)&NET_SendPacket },
	{ 0x01D58C80, "NET_IPSocket", (size_t)&NET_IPSocket },
	{ 0x01D58E80, "NET_OpenIP", (size_t)&NET_OpenIP },
#ifdef _WIN32
	{ 0x01D59000, "NET_IPXSocket", (size_t)&NET_IPXSocket },
	{ 0x01D59170, "NET_OpenIPX", (size_t)&NET_OpenIPX },
#endif //_WIN32
	{ 0x01D59240, "NET_GetLocalAddress", (size_t)&NET_GetLocalAddress },
	{ 0x01D59410, "NET_IsConfigured", (size_t)&NET_IsConfigured },
	{ 0x01D59420, "NET_Config", (size_t)&NET_Config },
	{ 0x01D596F0, "MaxPlayers_f", (size_t)&MaxPlayers_f },
	{ 0x01D594D0, "NET_Init", (size_t)&NET_Init },
	{ 0x01D59790, "NET_ClearLagData", (size_t)&NET_ClearLagData },
	{ 0x01D597E0, "NET_Shutdown", (size_t)&NET_Shutdown },
	{ 0x01D59810, "NET_JoinGroup", (size_t)&NET_JoinGroup },
	{ 0x01D59870, "NET_LeaveGroup", (size_t)&NET_LeaveGroup },

#endif // Net_ws_region

#ifndef Net_chan_region

	{ 0x01D54DB0, "Netchan_UnlinkFragment", (size_t)&Netchan_UnlinkFragment },
	{ 0x01D54E20, "Netchan_OutOfBand", (size_t)&Netchan_OutOfBand },
	{ 0x01D54EA0, "Netchan_OutOfBandPrint", (size_t)&Netchan_OutOfBandPrint },
	{ 0x01D54F00, "Netchan_ClearFragbufs", (size_t)&Netchan_ClearFragbufs },
	{ 0x01D54F30, "Netchan_ClearFragments", (size_t)&Netchan_ClearFragments },
	{ 0x01D54FA0, "Netchan_Clear", (size_t)&Netchan_Clear },
	{ 0x01D55040, "Netchan_Setup", (size_t)&Netchan_Setup },
	{ 0x01D550D0, "Netchan_CanPacket", (size_t)&Netchan_CanPacket },
	{ 0x01D55130, "Netchan_UpdateFlow", (size_t)&Netchan_UpdateFlow },
	{ 0x01D55240, "Netchan_Transmit", (size_t)&Netchan_Transmit },
	{ 0x01D558E0, "Netchan_FindBufferById", (size_t)&Netchan_FindBufferById },
	{ 0x01D55950, "Netchan_CheckForCompletion", (size_t)&Netchan_CheckForCompletion },
	{ 0x01D55A00, "Netchan_Validate", (size_t)&Netchan_Validate },
	{ 0x01D55A90, "Netchan_Process", (size_t)&Netchan_Process },
	{ 0x01D55F40, "Netchan_FragSend", (size_t)&Netchan_FragSend },
	{ 0x01D55F90, "Netchan_AddBufferToList", (size_t)&Netchan_AddBufferToList },
	{ 0x01D55FE0, "Netchan_AllocFragbuf", (size_t)&Netchan_AllocFragbuf },
	{ 0x01D56010, "Netchan_AddFragbufToTail", (size_t)&Netchan_AddFragbufToTail },
	{ 0x01D56050, "Netchan_CreateFragments_", (size_t)&Netchan_CreateFragments_ },
	{ 0x01D56210, "Netchan_CreateFragments", (size_t)&Netchan_CreateFragments },
	{ 0x01D56250, "Netchan_CreateFileFragmentsFromBuffer", (size_t)&Netchan_CreateFileFragmentsFromBuffer },
	{ 0x01D56450, "Netchan_CreateFileFragments", (size_t)&Netchan_CreateFileFragments },
	{ 0x01D56850, "Netchan_FlushIncoming", (size_t)&Netchan_FlushIncoming },
	{ 0x01D568C0, "Netchan_CopyNormalFragments", (size_t)&Netchan_CopyNormalFragments },
	{ 0x01D569D0, "Netchan_CopyFileFragments", (size_t)&Netchan_CopyFileFragments },
	//{ 0x01D56DB0, "Netchan_IsSending", (size_t)&Netchan_IsSending },
	//{ 0x01D56DE0, "Netchan_IsReceiving", (size_t)&Netchan_IsReceiving },
	{ 0x01D56E10, "Netchan_IncomingReady", (size_t)&Netchan_IncomingReady },
	//{ 0x01D56E40, "Netchan_UpdateProgress", (size_t)&Netchan_UpdateProgress },
	{ 0x01D56FE0, "Netchan_Init", (size_t)&Netchan_Init },
	//{ 0x01D57030, "Netchan_CompressPacket", (size_t)&Netchan_CompressPacket }, //NOXREF
	//{ 0x01D57040, "Netchan_DecompressPacket", (size_t)&Netchan_DecompressPacket }, //NOXREF

#endif // Net_chan_region

#ifndef Hashpak_region

	{ 0x01D3E970, "HPAK_GetDataPointer", (size_t)&HPAK_GetDataPointer },
	{ 0x01D3EC40, "HPAK_FindResource", (size_t)&HPAK_FindResource },
	{ 0x01D3ECB0, "HPAK_AddToQueue", (size_t)&HPAK_AddToQueue },
	{ 0x01D3EDC0, "HPAK_FlushHostQueue", (size_t)&HPAK_FlushHostQueue },
	{ 0x01D3EE20, "HPAK_AddLump", (size_t)&HPAK_AddLump },
	{ 0x01D3F3D0, "HPAK_RemoveLump", (size_t)&HPAK_RemoveLump },
	{ 0x01D3F850, "HPAK_ResourceForIndex", (size_t)&HPAK_ResourceForIndex },
	{ 0x01D3F9E0, "HPAK_ResourceForHash", (size_t)&HPAK_ResourceForHash },
	{ 0x01D3FBA0, "HPAK_List_f", (size_t)&HPAK_List_f },
	{ 0x01D3FE30, "HPAK_CreatePak", (size_t)&HPAK_CreatePak },
	{ 0x01D40140, "HPAK_Remove_f", (size_t)&HPAK_Remove_f },
	{ 0x01D401F0, "HPAK_Validate_f", (size_t)&HPAK_Validate_f },
	{ 0x01D405B0, "HPAK_Extract_f", (size_t)&HPAK_Extract_f },
	{ 0x01D409A0, "HPAK_Init", (size_t)&HPAK_Init },
	{ 0x01D409F1, "HPAK_GetItem", (size_t)&HPAK_GetItem },	// NOXREF
	{ 0x01D40B90, "HPAK_CheckSize", (size_t)&HPAK_CheckSize },
	{ 0x01D40CE0, "HPAK_ValidatePak", (size_t)&HPAK_ValidatePak },
	{ 0x01D40F80, "HPAK_CheckIntegrity", (size_t)&HPAK_CheckIntegrity },

#endif // Hashpak_region

#ifndef Wad_region

	{ 0x01DB8D30, "W_CleanupName", (size_t)&W_CleanupName },
	{ 0x01DB8D90, "W_LoadWadFile", (size_t)&W_LoadWadFile },
	{ 0x01DB8EF0, "W_GetLumpinfo", (size_t)&W_GetLumpinfo },
	{ 0x01DB8F70, "W_GetLumpName", (size_t)&W_GetLumpName },
	//{ 0x, "W_GetLumpNum", (size_t)&W_GetLumpNum },	// NOXREF
	{ 0x01DB8FF0, "W_Shutdown", (size_t)&W_Shutdown },
	{ 0x01DB9020, "SwapPic", (size_t)&SwapPic },

#endif // Wad_region

#ifndef Textures_region

	{ 0x01DA7200, "SafeRead", (size_t)&SafeRead },
	{ 0x01DA7230, "CleanupName", (size_t)&CleanupName },
	{ 0x01DA7290, "lump_sorter", (size_t)&lump_sorter },
	{ 0x01DA72E0, "ForwardSlashes", (size_t)&ForwardSlashes },
	{ 0x01DA7300, "TEX_InitFromWad", (size_t)&TEX_InitFromWad },
	{ 0x01DA75D0, "TEX_CleanupWadInfo", (size_t)&TEX_CleanupWadInfo },
	{ 0x01DA7630, "TEX_LoadLump", (size_t)&TEX_LoadLump },
	{ 0x01DA76D4, "FindMiptex", (size_t)&FindMiptex },
	{ 0x01DA7740, "TEX_AddAnimatingTextures", (size_t)&TEX_AddAnimatingTextures },

#endif // Textures_region

#ifndef Sv_user_region

	{ 0x01D9B3B0, "SV_ParseConsistencyResponse", (size_t)&SV_ParseConsistencyResponse },
	{ 0x01D9B790, "SV_FileInConsistencyList", (size_t)&SV_FileInConsistencyList },
	{ 0x01D9B7F0, "SV_TransferConsistencyInfo", (size_t)&SV_TransferConsistencyInfo },
	{ 0x01D9B960, "SV_SendConsistencyList", (size_t)&SV_SendConsistencyList },
	{ 0x01D9BA50, "SV_PreRunCmd", (size_t)&SV_PreRunCmd },
	{ 0x01D9BA60, "SV_CopyEdictToPhysent", (size_t)&SV_CopyEdictToPhysent },
	{ 0x01D9BD70, "SV_AddLinksToPM_", (size_t)&SV_AddLinksToPM_ },
	{ 0x01D9C070, "SV_AddLinksToPM", (size_t)&SV_AddLinksToPM },
	{ 0x01D9C1A0, "SV_PlayerRunPreThink", (size_t)&SV_PlayerRunPreThink },
	{ 0x01D9C1C0, "SV_PlayerRunThink", (size_t)&SV_PlayerRunThink },
	{ 0x01D9C260, "SV_CheckMovingGround", (size_t)&SV_CheckMovingGround },
	{ 0x01D9C340, "SV_ConvertPMTrace", (size_t)&SV_ConvertPMTrace },
	{ 0x01D9C3A0, "SV_ForceFullClientsUpdate", (size_t)&SV_ForceFullClientsUpdate },
	{ 0x01D9C470, "SV_RunCmd", (size_t)&SV_RunCmd },
	{ 0x01D9D69F, "SV_ValidateClientCommand", (size_t)&SV_ValidateClientCommand },
	{ 0x01D9D6EF, "SV_CalcClientTime", (size_t)&SV_CalcClientTime },
	{ 0x01D9D86F, "SV_ComputeLatency", (size_t)&SV_ComputeLatency },
	{ 0x01D9D88F, "SV_UnlagCheckTeleport", (size_t)&SV_UnlagCheckTeleport },
	{ 0x01D9D8DF, "SV_GetTrueOrigin", (size_t)&SV_GetTrueOrigin },
	{ 0x01D9D96F, "SV_GetTrueMinMax", (size_t)&SV_GetTrueMinMax },
	{ 0x01D9D9EF, "SV_FindEntInPack", (size_t)&SV_FindEntInPack },
	{ 0x01D9DA2F, "SV_SetupMove", (size_t)&SV_SetupMove },
	{ 0x01D9E01F, "SV_RestoreMove", (size_t)&SV_RestoreMove },
	{ 0x01D9E17F, "SV_ParseStringCommand", (size_t)&SV_ParseStringCommand },
	{ 0x01D9E1DF, "SV_ParseDelta", (size_t)&SV_ParseDelta },
	{ 0x01D9E1FF, "SV_EstablishTimeBase", (size_t)&SV_EstablishTimeBase },
	{ 0x01D9E2EF, "SV_ParseMove", (size_t)&SV_ParseMove },
	{ 0x01D9E76F, "SV_ParseVoiceData", (size_t)&SV_ParseVoiceData },
	{ 0x01D9E8DF, "SV_IgnoreHLTV", (size_t)&SV_IgnoreHLTV },
	{ 0x01D9E8EF, "SV_ParseCvarValue", (size_t)&SV_ParseCvarValue },
	{ 0x01D9E92F, "SV_ParseCvarValue2", (size_t)&SV_ParseCvarValue2 },
	{ 0x01D9E9AF, "SV_ExecuteClientMessage", (size_t)&SV_ExecuteClientMessage },
	{ 0x01D9EACF, "SV_SetPlayer", (size_t)&SV_SetPlayer },
	{ 0x01D9EB2F, "SV_ShowServerinfo_f", (size_t)&SV_ShowServerinfo_f },
	{ 0x01D9EB4F, "SV_SendEnts_f", (size_t)&SV_SendEnts_f },
	{ 0x01D9EB7F, "SV_FullUpdate_f", (size_t)&SV_FullUpdate_f },

#endif // Sv_user_region

#ifndef Tmessage_region

	{ 0x01DA7B00, "memfgets", (size_t)&memfgets },
	{ 0x01DA7B90, "IsComment", (size_t)&IsComment },
	{ 0x01DA7BD0, "IsStartOfText", (size_t)&IsStartOfText },
	{ 0x01DA7BF0, "IsEndOfText", (size_t)&IsEndOfText },
	{ 0x01DA7C10, "IsWhiteSpace", (size_t)&IsWhiteSpace },
	//{ 0x01DA7C40, "SkipSpace", (size_t)&SkipSpace },	// NOXREF
	//{ 0x01DA7C80, "SkipText", (size_t)&SkipText },	// NOXREF
	//{ 0x01DA7CC0, "ParseFloats", (size_t)&ParseFloats },		// NOXREF
	{ 0x01DA7D10, "TrimSpace", (size_t)&TrimSpace },
	//{ 0x01DA7D90, "IsToken", (size_t)&IsToken },	// NOXREF
	//{ 0x01DA7DD0, "ParseDirective", (size_t)&ParseDirective },	// NOXREF
	//{ 0x01DA80A0, "TextMessageParse", (size_t)&TextMessageParse },	// NOXREF
	//{ 0x01DA8030, "TextMessageShutdown", (size_t)&TextMessageShutdown },	// NOXREF
	//{ 0x01DA8050, "TextMessageInit", (size_t)&TextMessageInit },	// NOXREF
	//{ 0x01DA8410, "TextMessageGet", (size_t)&TextMessageGet },	// NOXREF

#endif // Tmessage_region

#ifndef TraceInit_Region

	{ 0x01DA8CA0, "_ZN12CInitTracker4InitEPKcS1_i", mfunc_ptr_cast(&CInitTracker::Init) },
	{ 0x01DA8D80, "_ZN12CInitTracker8ShutdownEPKci", mfunc_ptr_cast(&CInitTracker::Shutdown) },
	//{ 0x01DA8B60, "_ZN12CInitTrackerC2Ev", mfunc_ptr_cast(&CInitTracker::CInitTracker) },
	//{ 0x01DA8BD0, "_ZN12CInitTrackerD2Ev", mfunc_ptr_cast(&CInitTracker::~CInitTracker) },
	{ 0x01DA8E60, "_Z9TraceInitPKcS0_i", (size_t)&TraceInit },
	{ 0x01DA8E80, "_Z13TraceShutdownPKci", (size_t)&TraceShutdown },

#endif // TraceInit_Region

#ifndef Vid_null_region

	//{ 0x, "VID_SetPalette", (size_t)&VID_SetPalette },
	//{ 0x, "VID_ShiftPalette", (size_t)&VID_ShiftPalette },
	//{ 0x, "VID_WriteBuffer", (size_t)&VID_WriteBuffer },
	//{ 0x01DB4BD0, "VID_Init", (size_t)&VID_Init },
	//{ 0x, "D_FlushCaches", (size_t)&D_FlushCaches },
	//{ 0x, "R_SetStackBase", (size_t)&R_SetStackBase },
	//{ 0x01D7E3D0, "SCR_UpdateScreen", (size_t)&SCR_UpdateScreen },
	//{ 0x, "V_Init", (size_t)&V_Init },
	//{ 0x, "Draw_Init", (size_t)&Draw_Init },
	//{ 0x, "SCR_Init", (size_t)&SCR_Init },
	//{ 0x01D65630, "R_Init", (size_t)&R_Init },
	//{ 0x01D67B20, "R_ForceCVars", (size_t)&R_ForceCVars },
	//{ 0x01D7E320, "SCR_BeginLoadingPlaque", (size_t)&SCR_BeginLoadingPlaque },
	//{ 0x01D7E3A0, "SCR_EndLoadingPlaque", (size_t)&SCR_EndLoadingPlaque },
	//{ 0x, "R_InitSky", (size_t)&R_InitSky },
	//{ 0x01D65F70, "R_MarkLeaves", (size_t)&R_MarkLeaves },
	{ 0x01D65570, "R_InitTextures", (size_t)&R_InitTextures },
	//{ 0x, "StartLoadingProgressBar", (size_t)&StartLoadingProgressBar },
	//{ 0x01D07670, "ContinueLoadingProgressBar", (size_t)&ContinueLoadingProgressBar },
	//{ 0x01D07700, "SetLoadingProgressBarStatusText", (size_t)&SetLoadingProgressBarStatusText },

#endif // Vid_null_region

#ifndef L_studio_region

	{ 0x01D4F0E0, "Mod_LoadStudioModel", (size_t)&Mod_LoadStudioModel },

#endif // L_studio_region

#ifndef Crc_region

	{ 0x01D2C610, "CRC32_Init", (size_t)&CRC32_Init },
	{ 0x01D2C620, "CRC32_Final", (size_t)&CRC32_Final },
	{ 0x01D2C630, "CRC32_ProcessByte", (size_t)&CRC32_ProcessByte },
	{ 0x01D2C660, "CRC32_ProcessBuffer", (size_t)&CRC32_ProcessBuffer },
	{ 0x01D2C8D0, "COM_BlockSequenceCRCByte", (size_t)&COM_BlockSequenceCRCByte },
	{ 0x01D2C970, "CRC_File", (size_t)&CRC_File },	// NOXREF
	{ 0x01D2CA40, "CRC_MapFile", (size_t)&CRC_MapFile },
	{ 0x01D2CBB0, "MD5Init", (size_t)&MD5Init },
	{ 0x01D2CBE0, "MD5Update", (size_t)&MD5Update },
	{ 0x01D2CCB0, "MD5Final", (size_t)&MD5Final },
	{ 0x01D2CD40, "MD5Transform", (size_t)&MD5Transform },
	{ 0x01D2D5B0, "MD5_Hash_File", (size_t)&MD5_Hash_File },
	{ 0x01D2D6F0, "MD5_Print", (size_t)&MD5_Print },

#endif // Crc_region

#ifndef Sv_RemoteAccess_region

	{ 0x01D984E0, "_ZN19CServerRemoteAccess20SendMessageToAdminUIEPKc", mfunc_ptr_cast(&CServerRemoteAccess::SendMessageToAdminUI) },
	{ 0x01D985E0, "NotifyDedicatedServerUI", (size_t)&NotifyDedicatedServerUI },
	{ 0x01D98090, "_ZN19CServerRemoteAccess17LookupStringValueEPKc", mfunc_ptr_cast(&CServerRemoteAccess::LookupStringValue) },
	{ 0x01D97D40, "_ZN19CServerRemoteAccess11LookupValueEPKcR10CUtlBuffer", mfunc_ptr_cast(&CServerRemoteAccess::LookupValue) },

	{ 0x01D98190, "_ZN19CServerRemoteAccess14GetUserBanListER10CUtlBuffer", mfunc_ptr_cast(&CServerRemoteAccess::GetUserBanList) },
	{ 0x01D98280, "_ZN19CServerRemoteAccess13GetPlayerListER10CUtlBuffer", mfunc_ptr_cast(&CServerRemoteAccess::GetPlayerList) },
	{ 0x01D983B0, "_ZN19CServerRemoteAccess10GetMapListER10CUtlBuffer", mfunc_ptr_cast(&CServerRemoteAccess::GetMapList) },

	{ 0x01D97AD0, "_ZN19CServerRemoteAccess12RequestValueEiPKc", mfunc_ptr_cast(&CServerRemoteAccess::RequestValue) },
	{ 0x01D97C20, "_ZN19CServerRemoteAccess8SetValueEPKcS1_", mfunc_ptr_cast(&CServerRemoteAccess::SetValue) },
	{ 0x01D97D10, "_ZN19CServerRemoteAccess11ExecCommandEPKc", mfunc_ptr_cast(&CServerRemoteAccess::ExecCommand) },

	{ 0x01D97850, "_ZN19CServerRemoteAccess16WriteDataRequestEPKvi", mfunc_ptr_cast(&CServerRemoteAccess::WriteDataRequest_noVirt) },
	{ 0x01D979A0, "_ZN19CServerRemoteAccess16ReadDataResponseEPvi", mfunc_ptr_cast(&CServerRemoteAccess::ReadDataResponse_noVirt) },

#endif // Sv_RemoteAccess_region

#ifndef IpratelimitWrapper_region

	//{ 0x01D4D990, "CheckIP", (size_t)&CheckIP },

#endif // IpratelimitWrapper_region

#ifndef Com_custom

	{ 0x01D28610, "COM_ClearCustomizationList", (size_t)&COM_ClearCustomizationList },
	{ 0x01D28740, "COM_CreateCustomization", (size_t)&COM_CreateCustomization },
	{ 0x01D28980, "COM_SizeofResourceList", (size_t)&COM_SizeofResourceList },

#endif // Com_custom

#ifndef Sv_upld

	{ 0x01D9A980, "SV_CheckFile", (size_t)&SV_CheckFile },
	{ 0x01D9AA50, "SV_ClearResourceLists", (size_t)&SV_ClearResourceLists },
	{ 0x01D9AA90, "SV_CreateCustomizationList", (size_t)&SV_CreateCustomizationList },
	{ 0x01D9ABB0, "SV_Customization", (size_t)&SV_Customization },
	{ 0x01D9AD20, "SV_RegisterResources", (size_t)&SV_RegisterResources },
	{ 0x01D9AD70, "SV_MoveToOnHandList", (size_t)&SV_MoveToOnHandList },
	{ 0x01D9ADB0, "SV_AddToResourceList", (size_t)&SV_AddToResourceList },
	{ 0x01D9AE00, "SV_ClearResourceList", (size_t)&SV_ClearResourceList },
	{ 0x01D9AE50, "SV_RemoveFromResourceList", (size_t)&SV_RemoveFromResourceList },
	{ 0x01D9AE80, "SV_EstimateNeededResources", (size_t)&SV_EstimateNeededResources },
	{ 0x01D9AEF0, "SV_RequestMissingResourcesFromClients", (size_t)&SV_RequestMissingResourcesFromClients },
	{ 0x01D9AF40, "SV_UploadComplete", (size_t)&SV_UploadComplete },
	{ 0x01D9AFA0, "SV_BatchUploadRequest", (size_t)&SV_BatchUploadRequest },
	{ 0x01D9B040, "SV_RequestMissingResources", (size_t)&SV_RequestMissingResources },
	{ 0x01D9B070, "SV_ParseResourceList", (size_t)&SV_ParseResourceList },

#endif // Sv_upld

#ifndef Decals

	{ 0x01D33EF0, "Draw_Shutdown", (size_t)&Draw_Shutdown },
	{ 0x01D33F30, "Draw_DecalShutdown", (size_t)&Draw_DecalShutdown },
	{ 0x01D33F60, "Draw_AllocateCacheSpace", (size_t)&Draw_AllocateCacheSpace },
	{ 0x01D33F90, "Draw_CacheWadInitFromFile", (size_t)&Draw_CacheWadInitFromFile },
	{ 0x01D34070, "Draw_CacheWadInit", (size_t)&Draw_CacheWadInit },
	{ 0x01D340C0, "Draw_CustomCacheWadInit", (size_t)&Draw_CustomCacheWadInit },
	{ 0x01D3A830, "Draw_MiptexTexture", (size_t)&Draw_MiptexTexture },
	{ 0x01D34290, "Draw_CacheWadHandler", (size_t)&Draw_CacheWadHandler },
	{ 0x01D342B0, "Draw_FreeWad", (size_t)&Draw_FreeWad },
	{ 0x01D34380, "Draw_DecalSetName", (size_t)&Draw_DecalSetName },			// NOXREF
	{ 0x01D343C0, "Draw_DecalIndex", (size_t)&Draw_DecalIndex },				// NOXREF
	{ 0x01D34450, "Draw_DecalCount", (size_t)&Draw_DecalCount },
	{ 0x01D34460, "Draw_DecalSize", (size_t)&Draw_DecalSize },
	{ 0x01D34490, "Draw_DecalName", (size_t)&Draw_DecalName },
	{ 0x01D344C0, "Draw_DecalTexture", (size_t)&Draw_DecalTexture },			// NOXREF
	{ 0x01D34560, "Draw_CacheByIndex", (size_t)&Draw_CacheByIndex },
	{ 0x01D34600, "Draw_DecalIndexFromName", (size_t)&Draw_DecalIndexFromName },		// NOXREF
	{ 0x01D34670, "Decal_ReplaceOrAppendLump", (size_t)&Decal_ReplaceOrAppendLump },
	{ 0x01D34710, "Decal_CountLumps", (size_t)&Decal_CountLumps },
	{ 0x01D34730, "Decal_SizeLumps", (size_t)&Decal_SizeLumps },
	{ 0x01D34750, "Decal_MergeInDecals", (size_t)&Decal_MergeInDecals },
	{ 0x01D349A0, "Decal_Init", (size_t)&Decal_Init },
	{ 0x01D34B00, "CustomDecal_Validate", (size_t)&CustomDecal_Validate },
	{ 0x01D34B60, "CustomDecal_Init", (size_t)&CustomDecal_Init },
	{ 0x01D34BD0, "Draw_CacheGet", (size_t)&Draw_CacheGet },				// NOXREF
	{ 0x01D34CC0, "Draw_CustomCacheGet", (size_t)&Draw_CustomCacheGet },
	{ 0x01D34D60, "Draw_CacheReload", (size_t)&Draw_CacheReload },				// NOXREF
	{ 0x01D34E40, "Draw_ValidateCustomLogo", (size_t)&Draw_ValidateCustomLogo },
	{ 0x01D35000, "Draw_CacheLoadFromCustom", (size_t)&Draw_CacheLoadFromCustom },
	{ 0x01D35110, "Draw_CacheIndex", (size_t)&Draw_CacheIndex },				// NOXREF
	{ 0x01D351A9, "Draw_CacheFindIndex", (size_t)&Draw_CacheFindIndex },			// NOXREF

#endif // Decals

#ifndef Sys_engine

	//{ 0x, "CEngine::CEngine", (size_t)&CEngine::CEngine },
	//{ 0x, "CEngine::~CEngine", (size_t)&CEngine::~CEngine },
	{ 0x01DA1C80, "_ZN7CEngine6UnloadEv", mfunc_ptr_cast(&CEngine::Unload_noVirt) },
	{ 0x01DA1CA0, "_ZN7CEngine4LoadEbPcS0_", mfunc_ptr_cast(&CEngine::Load_noVirt) },
	{ 0x01DA1CF0, "_ZN7CEngine5FrameEv", mfunc_ptr_cast(&CEngine::Frame_noVirt) },
	{ 0x01DA1DD0, "_ZN7CEngine11SetSubStateEi", mfunc_ptr_cast(&CEngine::SetSubState_noVirt) },
	{ 0x01DA1DF0, "_ZN7CEngine8SetStateEi", mfunc_ptr_cast(&CEngine::SetState_noVirt) },
	{ 0x01DA1E10, "_ZN7CEngine8GetStateEv", mfunc_ptr_cast(&CEngine::GetState_noVirt) },
	{ 0x01DA1E20, "_ZN7CEngine11GetSubStateEv", mfunc_ptr_cast(&CEngine::GetSubState_noVirt) },
	{ 0x01DA1E30, "_ZN7CEngine12GetFrameTimeEv", mfunc_ptr_cast(&CEngine::GetFrameTime_noVirt) },
	{ 0x01DA1E40, "_ZN7CEngine10GetCurTimeEv", mfunc_ptr_cast(&CEngine::GetCurTime_noVirt) },
	{ 0x01DA1E50, "_ZN7CEngine13TrapKey_EventEib", mfunc_ptr_cast(&CEngine::TrapKey_Event_noVirt) },
	{ 0x01DA1E90, "_ZN7CEngine15TrapMouse_EventEib", mfunc_ptr_cast(&CEngine::TrapMouse_Event_noVirt) },
	{ 0x01DA1ED0, "_ZN7CEngine13StartTrapModeEv", mfunc_ptr_cast(&CEngine::StartTrapMode_noVirt) },
	{ 0x01DA1EE0, "_ZN7CEngine10IsTrappingEv", mfunc_ptr_cast(&CEngine::IsTrapping_noVirt) },
	{ 0x01DA1EF0, "_ZN7CEngine17CheckDoneTrappingERiS0_", mfunc_ptr_cast(&CEngine::CheckDoneTrapping_noVirt) },
	{ 0x01DA1F30, "_ZN7CEngine11SetQuittingEi", mfunc_ptr_cast(&CEngine::SetQuitting_noVirt) },
	{ 0x01DA1F40, "_ZN7CEngine11GetQuittingEv", mfunc_ptr_cast(&CEngine::GetQuitting_noVirt) },

#endif // Sys_engine

#ifndef Sys_linuxwind
	// Disable/enable all at once!
	
	{ 0x01DA5A10, "_ZN5CGame4InitEPv", mfunc_ptr_cast(&CGame::Init_noVirt) },
	{ 0x01DA5A20, "_ZN5CGame8ShutdownEv", mfunc_ptr_cast(&CGame::Shutdown_noVirt) },
	{ 0x01DA5760, "_ZN5CGame16CreateGameWindowEv", mfunc_ptr_cast(&CGame::CreateGameWindow_noVirt) },
	{ 0x01DA4B20, "_ZN5CGame15SleepUntilInputEi", mfunc_ptr_cast(&CGame::SleepUntilInput_noVirt) },
	{ 0x01DA5A40, "_ZN5CGame13GetMainWindowEv", mfunc_ptr_cast(&CGame::GetMainWindow_noVirt) },
	{ 0x01DA5A50, "_ZN5CGame20GetMainWindowAddressEv", mfunc_ptr_cast(&CGame::GetMainWindowAddress_noVirt) },
	{ 0x01DA5A60, "_ZN5CGame11SetWindowXYEii", mfunc_ptr_cast(&CGame::SetWindowXY_noVirt) },
	{ 0x01DA5A80, "_ZN5CGame13SetWindowSizeEii", mfunc_ptr_cast(&CGame::SetWindowSize_noVirt) },
	{ 0x01DA5B00, "_ZN5CGame13GetWindowRectEPiS0_S0_S0_", mfunc_ptr_cast(&CGame::GetWindowRect_noVirt) },
	{ 0x01DA5B40, "_ZN5CGame11IsActiveAppEv", mfunc_ptr_cast(&CGame::IsActiveApp_noVirt) },
	{ 0x01DA5B60, "_ZN5CGame13IsMultiplayerEv", mfunc_ptr_cast(&CGame::IsMultiplayer_noVirt) },
	{ 0x01DA5BB0, "_ZN5CGame17PlayStartupVideosEv", mfunc_ptr_cast(&CGame::PlayStartupVideos_noVirt) },
	{ 0x01DA5BC0, "_ZN5CGame14PlayAVIAndWaitEPKc", mfunc_ptr_cast(&CGame::PlayAVIAndWait_noVirt) },
	{ 0x01DA5B70, "_ZN5CGame16SetCursorVisibleEb", mfunc_ptr_cast(&CGame::SetCursorVisible_noVirt) },

#endif //Sys_linuxwind

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

	{ 0x0229AE80, "pr_strings", (size_t)&ppr_strings },
	{ 0x01E45C04, "gNullString", (size_t)&pgNullString },

	{ 0x01E46480, "SV_UPDATE_BACKUP", (size_t)&pSV_UPDATE_BACKUP },
	{ 0x01E46484, "SV_UPDATE_MASK", (size_t)&pSV_UPDATE_MASK },


	{ 0x021D3E80, "gGlobalVariables", (size_t)&pgGlobalVariables },
	{ 0x021D3DE0, "svs", (size_t)&psvs },
	{ 0x021D4960, "sv", (size_t)&psv },
	{ 0x0208F788, "gPacketSuppressed", (size_t)&pgPacketSuppressed },
	{ 0x01E45C08, "giNextUserMsg", (size_t)&pgiNextUserMsg },
	{ 0x0208F7A4, "hashstrings_collisions", (size_t)&phashstrings_collisions },

	{ 0x0208F76C, "g_pplayerdelta", (size_t)&pg_pplayerdelta },
	{ 0x0208F770, "g_pentitydelta", (size_t)&pg_pentitydelta },
	{ 0x0208F774, "g_pcustomentitydelta", (size_t)&pg_pcustomentitydelta },
	{ 0x0208F778, "g_pclientdelta", (size_t)&pg_pclientdelta },
	{ 0x0208F77C, "g_pweapondelta", (size_t)&pg_pweapondelta },

	{ 0x021C2B60, "localinfo", (size_t)&plocalinfo },
	{ 0x021D3F40, "localmodels", (size_t)&plocalmodels },

	{ 0x0221AD80, "ipfilters", (size_t)&pipfilters },
	{ 0x021D4940, "numipfilters", (size_t)&pnumipfilters },
	{ 0x0229B6C0, "userfilters", (size_t)&puserfilters },
	{ 0x0208F798, "numuserfilters", (size_t)&pnumuserfilters },

	{ 0x01E45F84, "sv_lan", (size_t)&psv_lan },
	{ 0x01E45FAC, "sv_lan_rate", (size_t)&psv_lan_rate },
	{ 0x01E45E8C, "sv_aim", (size_t)&psv_aim },
	
	{ 0x021D1BC0, "sv_decalnames", (size_t)&psv_decalnames },
	{ 0x0208F78C, "sv_decalnamecount", (size_t)&psv_decalnamecount },

	{ 0x01E46274, "sv_skycolor_r", (size_t)&psv_skycolor_r },
	{ 0x01E46298, "sv_skycolor_g", (size_t)&psv_skycolor_g },
	{ 0x01E462BC, "sv_skycolor_b", (size_t)&psv_skycolor_b },
	{ 0x01E462E0, "sv_skyvec_x", (size_t)&psv_skyvec_x },
	{ 0x01E46304, "sv_skyvec_y", (size_t)&psv_skyvec_y },
	{ 0x01E46328, "sv_skyvec_z", (size_t)&psv_skyvec_z },

	{ 0x01E48F64, "sv_spectatormaxspeed", (size_t)&psv_spectatormaxspeed },
	{ 0x01E48F90, "sv_airaccelerate", (size_t)&psv_airaccelerate },
	{ 0x01E48FBC, "sv_wateraccelerate", (size_t)&psv_wateraccelerate },
	{ 0x01E48FE4, "sv_waterfriction", (size_t)&psv_waterfriction },
	{ 0x01E49008, "sv_zmax", (size_t)&psv_zmax },
	{ 0x01E4902C, "sv_wateramp", (size_t)&psv_wateramp },
	{ 0x01E49054, "sv_skyname", (size_t)&psv_skyname },
	{ 0x01E46034, "mapcyclefile", (size_t)&pmapcyclefile },
	{ 0x01E46060, "motdfile", (size_t)&pmotdfile },
	{ 0x01E46090, "servercfgfile", (size_t)&pservercfgfile },
	{ 0x01E460F0, "lservercfgfile", (size_t)&plservercfgfile },
	{ 0x01E46114, "logsdir", (size_t)&plogsdir },
	{ 0x01E46144, "bannedcfgfile", (size_t)&pbannedcfgfile },


	{ 0x0208F790, "sv_gpNewUserMsgs", (size_t)&psv_gpNewUserMsgs },
	{ 0x0208F794, "sv_gpUserMsgs", (size_t)&psv_gpUserMsgs },

	{ 0x0239BCE0, "g_svmove", (size_t)&pg_svmove },

	{ 0x0208F760, "sv_lastnum", (size_t)&psv_lastnum },
	{ 0x02089220, "g_sv_instance_baselines", (size_t)&pg_sv_instance_baselines },
	{ 0x0208F764, "g_bOutOfDateRestart", (size_t)&pg_bOutOfDateRestart },
	{ 0x01E45C00, "g_userid", (size_t)&pg_userid },

	{ 0x0208F768, "g_sv_delta", (size_t)&pg_sv_delta },
	{ 0x0208F780, "g_peventdelta", (size_t)&pg_peventdelta },

	{ 0x01E45C1C, "rcon_password", (size_t)&prcon_password },
	{ 0x01E45C48, "sv_enableoldqueries", (size_t)&psv_enableoldqueries },
	
	{ 0x01E45C74, "sv_instancedbaseline", (size_t)&psv_instancedbaseline },
	{ 0x01E45C94, "sv_contact", (size_t)&psv_contact },
	{ 0x01E45CC4, "sv_maxupdaterate", (size_t)&psv_maxupdaterate },
	{ 0x01E45CF4, "sv_minupdaterate", (size_t)&psv_minupdaterate },
	{ 0x01E45D18, "sv_filterban", (size_t)&psv_filterban },
	{ 0x01E45D3C, "sv_minrate", (size_t)&psv_minrate },
	{ 0x01E45D60, "sv_maxrate", (size_t)&psv_maxrate },
	{ 0x01E45D84, "sv_logrelay", (size_t)&psv_logrelay },
	
	{ 0x01E45DD0, "violence_hblood", (size_t)&pviolence_hblood },
	{ 0x01E45DF8, "violence_ablood", (size_t)&pviolence_ablood },
	{ 0x01E45E20, "violence_hgibs", (size_t)&pviolence_hgibs },
	{ 0x01E45E48, "violence_agibs", (size_t)&pviolence_agibs },
	{ 0x01E45E6C, "sv_newunit", (size_t)&psv_newunit },

	{ 0x01E45ED8, "sv_clienttrace", (size_t)&psv_clienttrace },
	{ 0x01E45EFC, "sv_timeout", (size_t)&psv_timeout },
	{ 0x01E45F24, "sv_failuretime", (size_t)&psv_failuretime },

	{ 0x01E45F44, "sv_cheats", (size_t)&psv_cheats },
	{ 0x01E45F64, "sv_password", (size_t)&psv_password },
	{ 0x01E45FD0, "sv_proxies", (size_t)&psv_proxies },
	{ 0x01E46000, "sv_outofdatetime", (size_t)&psv_outofdatetime },
	
	{ 0x01E460B8, "mapchangecfgfile", (size_t)&pmapchangecfgfile },

	{ 0x021D1BA0, "allow_cheats", (size_t)&pallow_cheats },

	{ 0x01E455A8, "mp_logecho", (size_t)&pmp_logecho },
	{ 0x01E45584, "mp_logfile", (size_t)&pmp_logfile },

	{ 0x01E4616C, "sv_allow_download", (size_t)&psv_allow_download },
	{ 0x01E46190, "sv_send_logos", (size_t)&psv_send_logos },
	{ 0x01E461B8, "sv_send_resources", (size_t)&psv_send_resources },

	{ 0x01E455FC, "sv_log_singleplayer", (size_t)&psv_log_singleplayer },
	{ 0x01E45620, "sv_logsecret", (size_t)&psv_logsecret },
	{ 0x01E455D0, "sv_log_onefile", (size_t)&psv_log_onefile },
	{ 0x01E461DC, "sv_logbans", (size_t)&psv_logbans },
	{ 0x01E46204, "sv_allow_upload", (size_t)&psv_allow_upload },
	{ 0x01E4622C, "sv_max_upload", (size_t)&psv_max_upload },
	{ 0x01E46250, "hpk_maxsize", (size_t)&phpk_maxsize },
	{ 0x01E46358, "sv_visiblemaxplayers", (size_t)&psv_visiblemaxplayers },

	{ 0x01E46380, "max_queries_sec", (size_t)&pmax_queries_sec },
	{ 0x01E463B0, "max_queries_sec_global", (size_t)&pmax_queries_sec_global },
	{ 0x01E463DC, "max_queries_window", (size_t)&pmax_queries_window },

	{ 0x01E46400, "sv_logblocks", (size_t)&psv_logblocks },
	{ 0x01E46424, "sv_downloadurl", (size_t)&psv_downloadurl },
	{ 0x01E4644C, "sv_allow_dlfile", (size_t)&psv_allow_dlfile },
	{ 0x01E4646C, "sv_version", (size_t)&psv_version },
	
	{ 0x0229B2A0, "sv_playermodel", (size_t)&psv_playermodel },

	{ 0x026EB5E0, "cls", (size_t)&pcls },
	{ 0x026F1DE0, "cl", (size_t)&pcl },
	{ 0x01FED50C, "key_dest", (size_t)&pkey_dest },

	{ 0x0269BB00, "g_clmove", (size_t)&pg_clmove },
	{ 0x028A1454, "cl_inmovie", (size_t)&pcl_inmovie },

	{ 0x01E2F0F8, "cl_name", (size_t)&pcl_name },
	{ 0x01E2F1B4, "rate", (size_t)&prate },
	{ 0x01E33D6C, "console", (size_t)&pconsole },

	{ 0x01E39FF4, "host_name", (size_t)&phost_name },
	{ 0x01E3A040, "host_speeds", (size_t)&phost_speeds },
	{ 0x01E3A064, "host_profile", (size_t)&phost_profile },
	{ 0x01E3A084, "developer", (size_t)&pdeveloper },
	{ 0x01E3A0AC, "host_limitlocal", (size_t)&phost_limitlocal },
	{ 0x01E3A0C8, "skill", (size_t)&pskill },
	{ 0x01E3A0EC, "deathmatch", (size_t)&pdeathmatch },
	{ 0x01E3A108, "coop", (size_t)&pcoop },
	{ 0x0266AFA0, "realtime", (size_t)&prealtime },
	{ 0x01FED0C0, "rolling_fps", (size_t)&prolling_fps },

	{ 0x0266AF20, "host_parms", (size_t)&phost_parms },
	{ 0x01FED0B4, "host_initialized", (size_t)&phost_initialized },
	{ 0x0266AEC8, "host_frametime", (size_t)&phost_frametime },

	{ 0x0266AED0, "host_framecount", (size_t)&phost_framecount },

	{ 0x0266AEDC, "host_client", (size_t)&phost_client },
	{ 0x01FED0B8, "gfNoMasterServer", (size_t)&pgfNoMasterServer },

	{ 0x0266AF38, "oldrealtime", (size_t)&poldrealtime },
	{ 0x0266AEC4, "host_hunklevel", (size_t)&phost_hunklevel },
	{ 0x0266AF60, "host_abortserver", (size_t)&phost_abortserver },
	{ 0x0266AEE0, "host_enddemo", (size_t)&phost_enddemo },

	{ 0x0266AED4, "host_basepal", (size_t)&phost_basepal },

	{ 0x01FD9778, "bfread", (size_t)&pbfread },
	{ 0x01FD9790, "bfwrite", (size_t)&pbfwrite },
	{ 0x02699D48, "msg_readcount", (size_t)&pmsg_readcount },
	{ 0x02699D4C, "msg_badread", (size_t)&pmsg_badread },

	{ 0x01FEB260, "decal_wad", (size_t)&pdecal_wad },
	{ 0x01FEB264, "menu_wad", (size_t)&pmenu_wad },
	{ 0x0266D440, "szCustName", (size_t)&pszCustName },
	{ 0x0266B440, "decal_names", (size_t)&pdecal_names },
	{ 0x01FEB268, "m_bDrawInitialized", (size_t)&pm_bDrawInitialized },
	{ 0x01FEB25C, "gfCustomBuild", (size_t)&pgfCustomBuild },

	{ 0x01FEB26C, "g_defs", (size_t)&pg_defs },
	{ 0x01FEB270, "g_encoders", (size_t)&pg_encoders },
	{ 0x01FEB274, "g_deltaregistry", (size_t)&pg_deltaregistry },

	{ 0x021B97E0, "gNetworkTextMessageBuffer", (size_t)&pgNetworkTextMessageBuffer },
	{ 0x021B9FE0, "gMessageParms", (size_t)&pgMessageParms },
	{ 0x02095C8C, "gMessageTable", (size_t)&pgMessageTable },
	{ 0x02095C90, "gMessageTableCount", (size_t)&pgMessageTableCount },
	
	{ 0x01E4CE10, "gNetworkMessageNames", (size_t)&pgNetworkMessageNames },
	{ 0x01E4CE30, "gNetworkTextMessage", (size_t)&pgNetworkTextMessage },

	{ 0x021B61C4, "hunk_base", (size_t)&phunk_base },
	{ 0x021B61C8, "hunk_tempactive", (size_t)&phunk_tempactive },
	{ 0x021B61D0, "hunk_low_used", (size_t)&phunk_low_used },
	{ 0x021B61D4, "hunk_size", (size_t)&phunk_size },
	{ 0x021B61D8, "hunk_high_used", (size_t)&phunk_high_used },
	{ 0x021B61E0, "cache_head", (size_t)&pcache_head },
	{ 0x021B6238, "hunk_tempmark", (size_t)&phunk_tempmark },
	{ 0x01E4DEC0, "mem_dbgfile", (size_t)&pmem_dbgfile },
	{ 0x021B61CC, "mainzone", (size_t)&pmainzone },
	{ 0x0266AEC0, "current_skill", (size_t)&pcurrent_skill },
	{ 0x01FED1E0, "g_careerState", (size_t)&pg_careerState },
	{ 0x01FED1D8, "gHostSpawnCount", (size_t)&pgHostSpawnCount },
	{ 0x01E3A960, "g_pSaveGameCommentFunc", (size_t)&pg_pSaveGameCommentFunc },
	{ 0x01E5C834, "g_LastScreenUpdateTime", (size_t)&pg_LastScreenUpdateTime },
	{ 0x023FECB4, "scr_skipupdate", (size_t)&pscr_skipupdate },
	{ 0x023FF0C0, "scr_centertime_off", (size_t)&pscr_centertime_off },

	{ 0x02699460, "serverinfo", (size_t)&pserverinfo },
	{ 0x026995EC, "com_argc", (size_t)&pcom_argc },
	{ 0x02699708, "com_argv", (size_t)&pcom_argv },
	{ 0x02699840, "com_token", (size_t)&pcom_token },
	{ 0x01FE1FA0, "com_ignorecolons", (size_t)&pcom_ignorecolons },
	{ 0x01FE1FA4, "s_com_token_unget", (size_t)&ps_com_token_unget },
	{ 0x02699600, "com_clientfallback", (size_t)&pcom_clientfallback },
	{ 0x02699720, "com_gamedir", (size_t)&pcom_gamedir },
	{ 0x02699C40, "com_cmdline", (size_t)&pcom_cmdline },

	{ 0x02699580, "gpszVersionString", (size_t)&pgpszVersionString },
	{ 0x026995C0, "gpszProductString", (size_t)&pgpszProductString },

	{ 0x02699D44, "bigendien", (size_t)&pbigendien },
	{ 0x026995E8, "BigShort", (size_t)&pBigShort },
	{ 0x026995E0, "LittleShort", (size_t)&pLittleShort },
	{ 0x0269970C, "BigLong", (size_t)&pBigLong },
	{ 0x02699710, "LittleLong", (size_t)&pLittleLong },
	{ 0x026995A0, "BigFloat", (size_t)&pBigFloat },
	{ 0x02699560, "LittleFloat", (size_t)&pLittleFloat },

	{ 0x01FD3420, "cmd_argc", (size_t)&pcmd_argc },
	{ 0x01FD3428, "cmd_argv", (size_t)&pcmd_argv },
	{ 0x01FD366C, "cmd_args", (size_t)&pcmd_args },
	{ 0x02699D7C, "cmd_source", (size_t)&pcmd_source },
	{ 0x02699D74, "cmd_wait", (size_t)&pcmd_wait },
	{ 0x01FD3670, "cmd_functions", (size_t)&pcmd_functions },
	{ 0x02699D60, "cmd_text", (size_t)&pcmd_text },
	{ 0x02699D84, "cmd_alias", (size_t)&pcmd_alias },

	{ 0x01FE501C, "cvar_vars", (size_t)&pcvar_vars },

	{ 0x026995E4, "loadcache", (size_t)&ploadcache },
	{ 0x02699704, "loadbuf", (size_t)&ploadbuf },
	{ 0x02699D40, "loadsize", (size_t)&ploadsize },

	{ 0x01FEC4C8, "g_fallbackLocalizationFiles", (size_t)&pg_fallbackLocalizationFiles },
	{ 0x01FEC4E0, "s_pBaseDir", (size_t)&ps_pBaseDir },

	{ 0x01FEC6E8, "bLowViolenceBuild", (size_t)&pbLowViolenceBuild },
	
	{ 0x01FEC6EC, "g_pFileSystem", (size_t)&pg_pFileSystem },
	{ 0x01FEC6E0, "g_pFileSystemModule", (size_t)&pg_pFileSystemModule },
	{ 0x01FEC6E4, "g_FileSystemFactory", (size_t)&pg_FileSystemFactory },

	{ 0x01E4AAC0, "g_hfind", (size_t)&pg_hfind },
	{ 0x01E4AAC8, "g_engfuncsExportedToDlls", (size_t)&pg_engfuncsExportedToDlls },

	{ 0x021C2260, "gszDisconnectReason", (size_t)&pgszDisconnectReason },
	{ 0x021C27C0, "gszExtendedDisconnectReason", (size_t)&pgszExtendedDisconnectReason },
	{ 0x02090C64, "gfExtendedError", (size_t)&pgfExtendedError },
	{ 0x021C28F8, "g_bIsDedicatedServer", (size_t)&pg_bIsDedicatedServer },

	{ 0x021C2248, "giSubState", (size_t)&pgiSubState },
	{ 0x021C2448, "giActive", (size_t)&pgiActive },
	{ 0x02090C60, "giStateInfo", (size_t)&pgiStateInfo },

	{ 0x021C2380, "gEntityInterface", (size_t)&pgEntityInterface },
	{ 0x021C28E0, "gNewDLLFunctions", (size_t)&pgNewDLLFunctions },
	{ 0x01FF0638, "g_modfuncs", (size_t)&pg_modfuncs },
	
	{ 0x021C2480, "g_rgextdll", (size_t)&pg_rgextdll },
	{ 0x02090C94, "g_iextdllMac", (size_t)&pg_iextdllMac },
	{ 0x021C2900, "gmodinfo", (size_t)&pgmodinfo },
	{ 0x02090C90, "gfBackground", (size_t)&pgfBackground },
	

	{ 0x02699454, "con_debuglog", (size_t)&pcon_debuglog },
	{ 0x02090C78, "g_bPrintingKeepAliveDots", (size_t)&pg_bPrintingKeepAliveDots },
	{ 0x02090C80, "Launcher_ConsolePrintf", (size_t)&pLauncher_ConsolePrintf },

	{ 0x01E42EC4, "registry", (size_t)&pregistry },

	{ 0x0239B740, "outputbuf", (size_t)&poutputbuf },
	{ 0x021CAB94, "sv_redirected", (size_t)&psv_redirected },
	{ 0x021CAB80, "sv_redirectto", (size_t)&psv_redirectto },

	{ 0x01E464A0, "sv_rcon_minfailures", (size_t)&psv_rcon_minfailures },
	{ 0x01E464CC, "sv_rcon_maxfailures", (size_t)&psv_rcon_maxfailures },
	{ 0x01E464FC, "sv_rcon_minfailuretime", (size_t)&psv_rcon_minfailuretime },
	{ 0x01E46528, "sv_rcon_banpenalty", (size_t)&psv_rcon_banpenalty },
	{ 0x0208E828, "g_rgRconFailures", (size_t)&pg_rgRconFailures },

	{ 0x01E42FF8, "scr_downloading", (size_t)&pscr_downloading },

	{ 0x0208F7B8, "g_bCS_CZ_Flags_Initialized", (size_t)&pg_bCS_CZ_Flags_Initialized },
	{ 0x0208F7AC, "g_bIsCZero", (size_t)&pg_bIsCZero },
	{ 0x0208F7B4, "g_bIsCZeroRitual", (size_t)&pg_bIsCZeroRitual },
	{ 0x0208F7B0, "g_bIsTerrorStrike", (size_t)&pg_bIsTerrorStrike },
	{ 0x0208F7BC, "g_bIsTFC", (size_t)&pg_bIsTFC },
	{ 0x0208F7C0, "g_bIsHL1", (size_t)&pg_bIsHL1 },
	{ 0x0208F7A8, "g_bIsCStrike", (size_t)&pg_bIsCStrike },

	{ 0x01FD3E80, "gPAS", (size_t)&pgPAS },
	{ 0x01FD3E84, "gPVS", (size_t)&pgPVS },
	{ 0x01FD3A78, "gPVSRowBytes", (size_t)&pgPVSRowBytes },
	{ 0x01FD3A80, "mod_novis", (size_t)&pmod_novis },

	{ 0x0229AE84, "fatbytes", (size_t)&pfatbytes },
	{ 0x0229B2C0, "fatpvs", (size_t)&pfatpvs },
	{ 0x0229B2A4, "fatpasbytes", (size_t)&pfatpasbytes },
	{ 0x0229AEA0, "fatpas", (size_t)&pfatpas },

	{ 0x0208FAB8, "s_Steam3Server", (size_t)&ps_Steam3Server },
	{ 0x0208FA58, "s_Steam3Client", (size_t)&ps_Steam3Client },

	{ 0x01E39F14, "sys_ticrate", (size_t)&psys_ticrate },
	{ 0x01E39F3C, "sys_timescale", (size_t)&psys_timescale },
	{ 0x01E39F60, "fps_max", (size_t)&pfps_max },
	{ 0x01E39F84, "host_killtime", (size_t)&phost_killtime },
	{ 0x01E39FA4, "sv_stats", (size_t)&psv_stats },
	{ 0x01E39FC8, "fps_override", (size_t)&pfps_override },
	{ 0x01E3A01C, "host_framerate", (size_t)&phost_framerate },
	{ 0x01E3A128, "pausable", (size_t)&ppausable },

	{ 0x01E43F6C, "suitvolume", (size_t)&psuitvolume },

	{ 0x0208FAC0, "truepositions", (size_t)&ptruepositions },
	{ 0x021C2B44, "sv_player", (size_t)&psv_player },
	{ 0x01E4A328, "clcommands", (size_t)&pclcommands },
	{ 0x0209064C, "g_balreadymoved", (size_t)&pg_balreadymoved },
	{ 0x01E4A658, "sv_clcfuncs", (size_t)&psv_clcfuncs },
	{ 0x020905C0, "s_LastFullUpdate", (size_t)&ps_LastFullUpdate },
	
	{ 0x01E4A3E0, "sv_edgefriction", (size_t)&psv_edgefriction },
	{ 0x01E4A404, "sv_maxspeed", (size_t)&psv_maxspeed },
	{ 0x01E4A42C, "sv_accelerate", (size_t)&psv_accelerate },
	{ 0x01E4A3BC, "sv_footsteps", (size_t)&psv_footsteps },
	{ 0x01E4A454, "sv_rollspeed", (size_t)&psv_rollspeed },
	{ 0x01E4A47C, "sv_rollangle", (size_t)&psv_rollangle },

	{ 0x01E4A49C, "sv_unlag", (size_t)&psv_unlag },
	{ 0x01E4A4C0, "sv_maxunlag", (size_t)&psv_maxunlag },
	{ 0x01E4A4E8, "sv_unlagpush", (size_t)&psv_unlagpush },
	{ 0x01E4A510, "sv_unlagsamples", (size_t)&psv_unlagsamples },
	{ 0x01E4A398, "mp_consistency", (size_t)&pmp_consistency },
	{ 0x01E4A568, "sv_voiceenable", (size_t)&psv_voiceenable },

	{ 0x02090648, "nofind", (size_t)&pnofind },

	{ 0x01E3F7F4, "pm_showclip", (size_t)&ppm_showclip },
	{ 0x01E3F808, "player_mins", (size_t)&pplayer_mins },
	{ 0x01E3F838, "player_maxs", (size_t)&pplayer_maxs },
	{ 0x02004308, "pmove", (size_t)&ppmove },
	{ 0x025E3F00, "movevars", (size_t)&pmovevars },

	{ 0x01FED51C, "vec3_origin", (size_t)&pvec3_origin },

	{ 0x02004980, "gMsgData", (size_t)&pgMsgData },
	{ 0x01E3F9AC, "gMsgBuffer", (size_t)&pgMsgBuffer },
	{ 0x02004B80, "gMsgEntity", (size_t)&pgMsgEntity },
	{ 0x02004B84, "gMsgDest", (size_t)&pgMsgDest },
	{ 0x02004B88, "gMsgType", (size_t)&pgMsgType },
	{ 0x02004B8C, "gMsgStarted", (size_t)&pgMsgStarted },
	{ 0x020043A8, "gMsgOrigin", (size_t)&pgMsgOrigin },
	{ 0x02004B90, "idum", (size_t)&pidum },
	{ 0x02004B9C, "g_groupop", (size_t)&pg_groupop },
	{ 0x02004B98, "g_groupmask", (size_t)&pg_groupmask },
	{ 0x025E39E0, "checkpvs", (size_t)&pcheckpvs },
	{ 0x025E3DE0, "c_invis", (size_t)&pc_invis },
	{ 0x025E39CC, "c_notvis", (size_t)&pc_notvis },

	{ 0x0208F7E0, "vec_origin", (size_t)&pvec_origin },

	{ 0x025DFD94, "r_visframecount", (size_t)&pr_visframecount },

	{ 0x02052A48, "cache_hull_hitgroup", (size_t)&pcache_hull_hitgroup },
	{ 0x0206FB60, "cache_hull", (size_t)&pcache_hull },
	{ 0x0201CDC8, "cache_planes", (size_t)&pcache_planes },

	{ 0x0206FB50, "pstudiohdr", (size_t)&ppstudiohdr },
	{ 0x0200A398, "studio_hull", (size_t)&pstudio_hull },
	{ 0x02052C48, "studio_hull_hitgroup", (size_t)&pstudio_hull_hitgroup },
	{ 0x0200C998, "studio_planes", (size_t)&pstudio_planes },
	{ 0x02052E48, "studio_clipnodes", (size_t)&pstudio_clipnodes },
	{ 0x0206E908, "rgStudioCache", (size_t)&prgStudioCache },
	{ 0x0206FB54, "r_cachecurrent", (size_t)&pr_cachecurrent },
	{ 0x02016DC4, "nCurrentHull", (size_t)&pnCurrentHull },
	{ 0x0200A384, "nCurrentPlane", (size_t)&pnCurrentPlane },
	{ 0x025DD0E0, "bonetransform", (size_t)&pbonetransform },
	{ 0x01E42284, "r_cachestudio", (size_t)&pr_cachestudio },
	{ 0x01E424E8, "g_pSvBlendingAPI", (size_t)&pg_pSvBlendingAPI },
	{ 0x01E424E0, "svBlending", (size_t)&psvBlending },
	{ 0x01E424D0, "server_studio_api", (size_t)&pserver_studio_api },
	{ 0x025DEEE0, "rotationmatrix", (size_t)&protationmatrix },

	{ 0x021B1568, "box_hull", (size_t)&pbox_hull, 1 },
	{ 0x021B1540, "beam_hull", (size_t)&pbeam_hull },
	{ 0x021B1590, "box_clipnodes", (size_t)&pbox_clipnodes, 1 },
	{ 0x021B15C0, "box_planes", (size_t)&pbox_planes, 1 },
	{ 0x021B14C8, "beam_planes", (size_t)&pbeam_planes },
	{ 0x021B6240, "sv_areanodes", (size_t)&psv_areanodes },
	{ 0x021B6640, "sv_numareanodes", (size_t)&psv_numareanodes },

	
	{ 0x02004310, "g_contentsresult", (size_t)&pg_contentsresult },
	{ 0x025E3E00, "box_hull", (size_t)&pbox_hull_0, 2 },
	{ 0x025E3E40, "box_clipnodes", (size_t)&pbox_clipnodes_0, 2 },
	{ 0x025E3E80, "box_planes", (size_t)&pbox_planes_0, 2 },

	{ 0x01E48EF0, "sv_maxvelocity", (size_t)&psv_maxvelocity },
	{ 0x01E48EC4, "sv_gravity", (size_t)&psv_gravity },
	{ 0x01E48F34, "sv_bounce", (size_t)&psv_bounce },
	{ 0x01E48F14, "sv_stepsize", (size_t)&psv_stepsize },
	{ 0x01E48E78, "sv_friction", (size_t)&psv_friction },
	{ 0x01E48EA0, "sv_stopspeed", (size_t)&psv_stopspeed },

	{ 0x021C2B48, "g_moved_from", (size_t)&pg_moved_from },
	{ 0x021C2B4C, "sv_numareanodes", (size_t)&pg_moved_edict },

	{ 0x021C2B50, "c_yes", (size_t)&pc_yes },
	{ 0x021C2B54, "c_no", (size_t)&pc_no },

	{ 0x020042AC, "net_thread_initialized", (size_t)&pnet_thread_initialized },
	{ 0x01E3E8DC, "net_address", (size_t)&pnet_address },
	{ 0x01E3E900, "ipname", (size_t)&pipname },
	{ 0x01E3E968, "defport", (size_t)&pdefport },
	{ 0x01E3E98C, "ip_clientport", (size_t)&pip_clientport },
	{ 0x01E3E9B4, "clientport", (size_t)&pclientport },
	{ 0x01E3EB34, "net_sleepforever", (size_t)&pnet_sleepforever },
	{ 0x01FF2B60, "loopbacks", (size_t)&ploopbacks },
	{ 0x01FF2AE8, "g_pLagData", (size_t)&pg_pLagData },
	{ 0x020042D8, "gFakeLag", (size_t)&pgFakeLag },
	{ 0x020042FC, "net_configured", (size_t)&pnet_configured },
	{ 0x025F4020, "net_message", (size_t)&pnet_message },
	{ 0x025F4040, "net_local_adr", (size_t)&pnet_local_adr },
	{ 0x02604060, "net_from", (size_t)&pnet_from },
	{ 0x020042BC, "noip", (size_t)&pnoip },
	{ 0x01E3E9D8, "clockwindow", (size_t)&pclockwindow },

	{ 0x020042B0, "use_thread", (size_t)&puse_thread },

	{ 0x025F4000, "in_message", (size_t)&pin_message },
	{ 0x025F3FC0, "in_from", (size_t)&pin_from },

	{ 0x020042C0, "ip_sockets", (size_t)&pip_sockets },
	{ 0x01E3E924, "iphostport", (size_t)&piphostport },
	{ 0x01E3E944, "hostport", (size_t)&phostport },
	{ 0x01E3EA04, "multicastport", (size_t)&pmulticastport },
	{ 0x01E3EA70, "fakelag", (size_t)&pfakelag },
	{ 0x01E3EA94, "fakeloss", (size_t)&pfakeloss },

	{ 0x01E3EAB4, "net_graph", (size_t)&pnet_graph },
	{ 0x01E3EADC, "net_graphwidth", (size_t)&pnet_graphwidth },
	{ 0x01E3EAFC, "net_scale", (size_t)&pnet_scale },
	{ 0x01E3EB20, "net_graphpos", (size_t)&pnet_graphpos },
	{ 0x025F4060, "net_message_buffer", (size_t)&pnet_message_buffer },
	{ 0x025E3FC0, "in_message_buf", (size_t)&pin_message_buf },

#ifdef _WIN32
	{ 0x025F3FE0, "net_local_ipx_adr", (size_t)&pnet_local_ipx_adr },
	{ 0x020042B8, "noipx", (size_t)&pnoipx },
	{ 0x01E3EA28, "ipx_hostport", (size_t)&pipx_hostport },
	{ 0x01E3EA50, "ipx_clientport", (size_t)&pipx_clientport },
	{ 0x020042CC, "ipx_sockets", (size_t)&pipx_sockets },
#endif // _WIN32
	{ 0x02604080, "gNetSplit", (size_t)&pgNetSplit },
	{ 0x020042E8, "messages", (size_t)&pmessages },
	{ 0x020042F4, "normalqueue", (size_t)&pnormalqueue },

	{ 0x02605040, "gDownloadFile", (size_t)&pgDownloadFile },

	{ 0x02605140, "net_drop", (size_t)&pnet_drop },
	{ 0x01E3E00C, "net_log", (size_t)&pnet_log },
	{ 0x01E3E034, "net_showpackets", (size_t)&pnet_showpackets },
	{ 0x01E3E058, "net_showdrop", (size_t)&pnet_showdrop },
	{ 0x01E3E080, "net_drawslider", (size_t)&pnet_drawslider },
	{ 0x01E3E0A4, "net_chokeloopback", (size_t)&pnet_chokeloopback },
	{ 0x01E3E0D8, "sv_filetransfercompression", (size_t)&psv_filetransfercompression },
	{ 0x01E3E110, "sv_filetransfermaxsize", (size_t)&psv_filetransfermaxsize },

#ifdef _WIN32
	{ 0x02090C88, "g_PerfCounterInitialized", (size_t)&pg_PerfCounterInitialized },
	{ 0x021C2460, "g_PerfCounterMutex", (size_t)&pg_PerfCounterMutex },

	{ 0x021C2454, "g_PerfCounterShiftRightAmount", (size_t)&pg_PerfCounterShiftRightAmount },
	{ 0x021C28C0, "g_PerfCounterSlice", (size_t)&pg_PerfCounterSlice },
	{ 0x02090C68, "g_CurrentTime", (size_t)&pg_CurrentTime },
	{ 0x02090C70, "g_StartTime", (size_t)&pg_StartTime },

	{ 0x01E4E98C, "g_FPUCW_Mask_Prec_64Bit", (size_t)&pg_FPUCW_Mask_Prec_64Bit },
	{ 0x01E4E990, "g_FPUCW_Mask_Prec_64Bit_2", (size_t)&pg_FPUCW_Mask_Prec_64Bit_2 },
	{ 0x01E4E988, "g_FPUCW_Mask_ZeroDiv_OFlow", (size_t)&pg_FPUCW_Mask_Round_Trunc },
	{ 0x01E4E984, "g_FPUCW_Mask_OFlow", (size_t)&pg_FPUCW_Mask_Round_Up },

	{ 0x021C2244, "g_WinNTOrHigher", (size_t)&pg_WinNTOrHigher },
	{ 0x020914E4, "g_bIsWin95", (size_t)&pg_bIsWin95 },
	{ 0x020914E8, "g_bIsWin98", (size_t)&pg_bIsWin98 },
#endif // _WIN32

	{ 0x02605180, "loadmodel", (size_t)&ploadmodel },
	{ 0x02605160, "loadname", (size_t)&ploadname },
	{ 0x026051A0, "mod_known", (size_t)&pmod_known },
	{ 0x02605144, "mod_numknown", (size_t)&pmod_numknown },
	{ 0x026671A0, "mod_base", (size_t)&pmod_base },
	{ 0x01FF062C, "wadpath", (size_t)&pwadpath },
	{ 0x01FF0630, "tested", (size_t)&ptested },
	{ 0x01FF0634, "ad_enabled", (size_t)&pad_enabled },
	{ 0x026671C0, "ad_wad", (size_t)&pad_wad },
	{ 0x01FED528, "mod_known_info", (size_t)&pmod_known_info },

	{ 0x02095B48, "lumpinfo", (size_t)&plumpinfo },
	{ 0x02095B4C, "nTexLumps", (size_t)&pnTexLumps },
	{ 0x021C2040, "texfiles", (size_t)&ptexfiles },
	{ 0x02095B44, "nTexFiles", (size_t)&pnTexFiles },
	{ 0x021B7660, "texgammatable", (size_t)&ptexgammatable },
	{ 0x025DFCE4, "r_notexture_mip", (size_t)&pr_notexture_mip },
	{ 0x021C2020, "nummiptex", (size_t)&pnummiptex },
	{ 0x021BA020, "miptex", (size_t)&pmiptex },

	{ 0x01E41A30, "r_wadtextures", (size_t)&pr_wadtextures },

	{ 0x01E42270, "r_dointerp", (size_t)&pr_dointerp },
	{ 0x025DFE40, "r_origin", (size_t)&pr_origin },

	{ 0x01E415BC, "r_pixbytes", (size_t)&pr_pixbytes },
	{ 0x01E43090, "gl_vsync", (size_t)&pgl_vsync },
	{ 0x023FF0D0, "scr_con_current", (size_t)&pscr_con_current },
	

	{ 0x0208F7EC, "g_ServerRemoteAccess", (size_t)&pg_ServerRemoteAccess },

	{ 0x01FED1E8, "cpuPercent", (size_t)&pcpuPercent },
	{ 0x01FED1D4, "startTime", (size_t)&pstartTime },
	{ 0x01FED1E4, "g_bMajorMapChange", (size_t)&pg_bMajorMapChange },
	{ 0x01E3A97C, "voice_recordtofile", (size_t)&pvoice_recordtofile },
	{ 0x01E3A9A8, "voice_inputfromfile", (size_t)&pvoice_inputfromfile },
	{ 0x01E3B250, "gTitleComments", (size_t)&pgTitleComments },
	{ 0x01E3A9D8, "gGameHeaderDescription", (size_t)&pgGameHeaderDescription },
	{ 0x01E3AAA0, "gSaveHeaderDescription", (size_t)&pgSaveHeaderDescription },
	{ 0x01E3ABB0, "gAdjacencyDescription", (size_t)&pgAdjacencyDescription },
	{ 0x01E3AC20, "gEntityTableDescription", (size_t)&pgEntityTableDescription },
	{ 0x01E3AC80, "gLightstyleDescription", (size_t)&pgLightstyleDescription },
	{ 0x01E3ACB0, "gHostMap", (size_t)&pgHostMap },
	{ 0x01FED208, "g_iQuitCommandIssued", (size_t)&pg_iQuitCommandIssued },
	{ 0x01FED20C, "g_pPostRestartCmdLineArgs", (size_t)&pg_pPostRestartCmdLineArgs },

	{ 0x021CABA0, "g_rg_sv_challenges", (size_t)&pg_rg_sv_challenges },

	{ 0x0239BCC0, "g_svdeltacallback", (size_t)&pg_svdeltacallback },

	{ 0x01FED4D0, "_ZL11rateChecker", (size_t)&prateChecker },

	{ 0x01FEC7F4, "gp_hpak_queue", (size_t)&pgp_hpak_queue },
	{ 0x01FEC7F8, "hash_pack_dir", (size_t)&phash_pack_dir },
	{ 0x0266AFB0, "hash_pack_header", (size_t)&phash_pack_header },

	{ 0x020891D8, "firstLog", (size_t)&pfirstLog },

	{ 0x01E4C6C4, "game", (size_t)&pgame },
	{ 0x01E4BB44, "eng", (size_t)&peng },
	{ 0x021B1468, "wads", (size_t)&pwads },
	{ 0x01FF06B0, "g_module", (size_t)&pg_module },

	{ 0x020914E0, "dedicated", (size_t)&pdedicated },
	{ 0x020914E4, "g_bIsWin95", (size_t)&pg_bIsWin95 },
	{ 0x020914E8, "g_bIsWin98", (size_t)&pg_bIsWin98 },
	{ 0x020914F8, "g_flLastSteamProgressUpdateTime", (size_t)&pg_flLastSteamProgressUpdateTime },
	{ 0x01E4B3E0, "szCommonPreloads", (size_t)&pszCommonPreloads },
	{ 0x01E4B3F0, "szReslistsBaseDir", (size_t)&pszReslistsBaseDir },
	{ 0x01E4B3FC, "szReslistsExt", (size_t)&pszReslistsExt },

	{ 0x02095C98, "g_InitTracker", (size_t)&pg_InitTracker },

#ifndef _WIN32
	{ 0x0, "gHasMMXTechnology", (size_t)&pgHasMMXTechnology },
#endif

#endif // Data_References_region

	{ NULL, NULL, NULL },
};
