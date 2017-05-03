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
#include "hookers/memory.cpp"
#include "hookers/hooker.cpp"

//#define Mem_region
//#define Function_References_region
//#define Data_References_region

FunctionHook g_FunctionHooks[] =
{
	// DO NOT DISABLE, other functions depends on memory allocation routines
#ifndef Mem_region

	HOOK_DEF(0x01D07C0F, malloc_wrapper),
	HOOK_DEF(0x01D07AFE, free_wrapper),
	HOOK_DEF(0x01D14F4D, strdup_wrapper),
	HOOK_DEF(0x01D07C21, __nh_malloc_wrapper),

	HOOK_DEF(0x01D02510, Mem_ZeroMalloc),
	//HOOK_DEF(0x0, Mem_Malloc),
	//HOOK_DEF(0x0, Mem_Realloc),
	//HOOK_DEF(0x0, Mem_Calloc),
	//HOOK_DEF(0x0, Mem_Strdup),
	//HOOK_DEF(0x0, Mem_Free),

	//HOOK_DEF(0x0, realloc_wrapper),
	//HOOK_DEF(0x0, calloc_wrapper),

#endif // Mem_region

#ifndef DemoPlayer_Region

	// BaseSystemModule virtual function
	//HOOK_VIRTUAL_DEF(0x01D032A0, DemoPlayer::RegisterListener),
	//HOOK_VIRTUAL_DEF(0x01D03360, DemoPlayer::RemoveListener),
	//HOOK_VIRTUAL_DEF(0x1D033700, DemoPlayer::GetSystem),
	//HOOK_VIRTUAL_DEF(0x01D03380, DemoPlayer::GetSerial),
	//HOOK_VIRTUAL_DEF(0x01D03390, DemoPlayer::GetName),
	//HOOK_VIRTUAL_DEF(0x01D033A0, DemoPlayer::GetState),
	//HOOK_VIRTUAL_DEF(0x01D033F0, DemoPlayer::GetVersion),

	// DemoPlayer virtual function
	HOOK_VIRTUAL_DEF(0x01D03670, DemoPlayer::Init),
	HOOK_VIRTUAL_DEF(0x01D04180, DemoPlayer::RunFrame),
	HOOK_VIRTUAL_DEF(0x01D042E0, DemoPlayer::ReceiveSignal),
	HOOK_VIRTUAL_DEF(0x01D02FA0, DemoPlayer::ExecuteCommand),
	HOOK_VIRTUAL_DEF(0x01D02F70, DemoPlayer::GetStatusLine),
	HOOK_VIRTUAL_DEF(0x01D02F60, DemoPlayer::GetType),
	HOOK_VIRTUAL_DEF(0x01D04490, DemoPlayer::ShutDown),
	HOOK_VIRTUAL_DEF(0x01D04400, DemoPlayer::NewGame),
	HOOK_VIRTUAL_DEF(0x01D03410, DemoPlayer::GetModName),
	HOOK_VIRTUAL_DEF(0x01D03420, DemoPlayer::WriteCommands),
	HOOK_VIRTUAL_DEF(0x01D03570, DemoPlayer::AddCommand),
	HOOK_VIRTUAL_DEF(0x01D035F0, DemoPlayer::RemoveCommand),
	HOOK_VIRTUAL_DEF(0x01D04640, DemoPlayer::GetLastCommand),
	HOOK_VIRTUAL_DEF(0x01D03660, DemoPlayer::GetCommands),
	HOOK_VIRTUAL_DEF(0x01D03900, DemoPlayer::SetWorldTime),
	HOOK_VIRTUAL_DEF(0x01D03940, DemoPlayer::SetTimeScale),
	HOOK_VIRTUAL_DEF(0x01D039B0, DemoPlayer::SetPaused),
	HOOK_VIRTUAL_DEF(0x01D039C0, DemoPlayer::SetEditMode),
	HOOK_VIRTUAL_DEF(0x01D03A20, DemoPlayer::SetMasterMode),
	HOOK_VIRTUAL_DEF(0x01D03A40, DemoPlayer::IsPaused),
	HOOK_VIRTUAL_DEF(0x01D02F30, DemoPlayer::IsLoading),
	HOOK_VIRTUAL_DEF(0x01D02F50, DemoPlayer::IsActive),
	HOOK_VIRTUAL_DEF(0x01D039D0, DemoPlayer::IsEditMode),
	HOOK_VIRTUAL_DEF(0x01D03A30, DemoPlayer::IsMasterMode),
	HOOK_VIRTUAL_DEF(0x01D04560, DemoPlayer::RemoveFrames),
	HOOK_VIRTUAL_DEF(0x01D04570, DemoPlayer::ExecuteDirectorCmd),
	HOOK_VIRTUAL_DEF(0x01D02ED0, DemoPlayer::GetWorldTime),
	HOOK_VIRTUAL_DEF(0x01D02EE0, DemoPlayer::GetStartTime),
	HOOK_VIRTUAL_DEF(0x01D02F00, DemoPlayer::GetEndTime),
	HOOK_VIRTUAL_DEF(0x01D03400, DemoPlayer::GetTimeScale),
	HOOK_VIRTUAL_DEF(0x01D02F80, DemoPlayer::GetWorld),
	HOOK_VIRTUAL_DEF(0x01D04630, DemoPlayer::GetFileName),
	HOOK_VIRTUAL_DEF(0x01D02E60, DemoPlayer::SaveGame),
	HOOK_VIRTUAL_DEF(0x01D04650, DemoPlayer::LoadGame),
	HOOK_VIRTUAL_DEF(0x01D039E0, DemoPlayer::Stop),
	HOOK_VIRTUAL_DEF(0x01D03A10, DemoPlayer::ForceHLTV),
	HOOK_VIRTUAL_DEF(0x01D03A50, DemoPlayer::GetDemoViewInfo),
	HOOK_VIRTUAL_DEF(0x01D03F90, DemoPlayer::ReadDemoMessage),
	HOOK_VIRTUAL_DEF(0x01D04130, DemoPlayer::ReadNetchanState),
	HOOK_VIRTUAL_DEF(0x01D02F90, DemoPlayer::GetDirector),		// NOXREF

	// DemoPlayer non-virtual function
	//HOOK_DEF(0x01D02F20, DemoPlayer::GetPlayerTime),		// NOXREF
	//HOOK_DEF(0x01D04500, DemoPlayer::FormatTime),			// NOXREF
	HOOK_DEF(0x01D041B0, DemoPlayer::RunClocks),
	HOOK_DEF(0x01D046F0, DemoPlayer::WriteDatagram),
	HOOK_DEF(0x01D03D30, DemoPlayer::WriteSpawn),
	HOOK_DEF(0x01D03F50, DemoPlayer::ReindexCommands),
	HOOK_DEF(0x01D03DC0, DemoPlayer::WriteCameraPath),
	HOOK_DEF(0x01D04840, DemoPlayer::ExecuteDemoFileCommands),
	HOOK_DEF(0x01D03030, DemoPlayer::CMD_Jump),
	HOOK_DEF(0x01D030E0, DemoPlayer::CMD_ForceHLTV),
	HOOK_DEF(0x01D031F0, DemoPlayer::CMD_Pause),
	HOOK_DEF(0x01D032B0, DemoPlayer::CMD_Speed),
	HOOK_DEF(0x01D033B0, DemoPlayer::CMD_Start),
	HOOK_DEF(0x01D03170, DemoPlayer::CMD_Save),

#endif // DemoPlayer_Region

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
