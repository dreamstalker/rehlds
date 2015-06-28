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
*/
#pragma once
#include "osconfig.h"
#include "FlightRecorderImpl.h"

extern CRehldsFlightRecorder* g_FlightRecorder;

extern void FR_Init();
extern void FR_Shutdown();

#ifdef REHLDS_FLIGHT_REC

extern cvar_t rehlds_flrec_frame;
extern cvar_t rehlds_flrec_pvdata;

extern uint16 g_FRMsg_Frame;
extern uint16 g_FRMsg_FreeEntPrivateData;
extern uint16 g_FRMsg_AllocEntPrivateData;

extern void FR_Dump_f();
extern void FR_Rehlds_Init();


extern void FR_StartFrame(long frameCounter);
extern void FR_EndFrame(long frameCounter);
extern void FR_FreeEntPrivateData(void* data);
extern void FR_AllocEntPrivateData(void* res, int size);
extern void FR_Log(const char* prefix, const char* msg);

#endif //REHLDS_FLIGHT_REC
