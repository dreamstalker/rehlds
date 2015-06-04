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
#include "precompiled.h"

CRehldsFlightRecorder* g_FlightRecorder;

uint16 g_FRMsg_Frame;
uint16 g_FRMsg_FreeEntPrivateData;
uint16 g_FRMsg_AllocEntPrivateData;

void FR_CheckInit() {
#ifdef HOOK_ENGINE
	if (!g_FlightRecorder)
		FR_Init();
#endif
}

void FR_Init() {
	g_FlightRecorder = new CRehldsFlightRecorder();

	g_FRMsg_Frame = g_FlightRecorder->RegisterMessage("rehlds", "Frame", 1, true);
	g_FRMsg_FreeEntPrivateData = g_FlightRecorder->RegisterMessage("rehlds", "FreeEntPrivateData", 1, false);
	g_FRMsg_AllocEntPrivateData = g_FlightRecorder->RegisterMessage("rehlds", "AllocEntPrivateData", 1, false);
}

void FR_StartFrame() {
	FR_CheckInit();
	g_FlightRecorder->StartMessage(g_FRMsg_Frame, true);
	g_FlightRecorder->WriteDouble(realtime);
	g_FlightRecorder->EndMessage(g_FRMsg_Frame, true);
}

void FR_EndFrame() {
	FR_CheckInit();
	g_FlightRecorder->StartMessage(g_FRMsg_Frame, false);
	g_FlightRecorder->EndMessage(g_FRMsg_Frame, false);
}

void FR_AllocEntPrivateData(void* res) {
	FR_CheckInit();
	g_FlightRecorder->StartMessage(g_FRMsg_AllocEntPrivateData, true);
	g_FlightRecorder->WriteUInt32((size_t)res);
	g_FlightRecorder->EndMessage(g_FRMsg_AllocEntPrivateData, true);
}

void FR_FreeEntPrivateData(void* data) {
	FR_CheckInit();
	g_FlightRecorder->StartMessage(g_FRMsg_FreeEntPrivateData, true);
	g_FlightRecorder->WriteUInt32((size_t)data);
	g_FlightRecorder->EndMessage(g_FRMsg_FreeEntPrivateData, true);
}
