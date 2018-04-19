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

void FR_Init() {
	g_FlightRecorder = new CRehldsFlightRecorder();
}

void FR_Shutdown() {
	if (g_FlightRecorder) {
		delete g_FlightRecorder;
		g_FlightRecorder = NULL;
	}
}

#ifdef REHLDS_FLIGHT_REC

uint16 g_FRMsg_Frame;
uint16 g_FRMsg_FreeEntPrivateData;
uint16 g_FRMsg_AllocEntPrivateData;
uint16 g_FRMsg_Log;

cvar_t rehlds_flrec_frame = { "rehlds_flrec_frame", "1", 0, 1.0f, NULL };
cvar_t rehlds_flrec_pvdata = { "rehlds_flrec_privdata", "1", 0, 1.0f, NULL };

void FR_Dump_f() {
	const char* fname = "rehlds_flightrec.bin";
	if (Cmd_Argc() == 1) {
		fname = Cmd_Argv(0);
	} else if (Cmd_Argc() > 1) {
		Con_Printf("usage:  rehlds_flrec_dump < filename >\n");
	}

	g_FlightRecorder->dump(fname);
}

void FR_Rehlds_Init() {
	Cvar_RegisterVariable(&rehlds_flrec_frame);
	Cvar_RegisterVariable(&rehlds_flrec_pvdata);
	Cmd_AddCommand("rehlds_flrec_dump", &FR_Dump_f);

	g_FRMsg_Frame = g_FlightRecorder->RegisterMessage("rehlds", "Frame", 2, true);
	g_FRMsg_FreeEntPrivateData = g_FlightRecorder->RegisterMessage("rehlds", "FreeEntPrivateData", 1, false);
	g_FRMsg_AllocEntPrivateData = g_FlightRecorder->RegisterMessage("rehlds", "AllocEntPrivateData", 2, false);
	g_FRMsg_Log = g_FlightRecorder->RegisterMessage("rehlds", "Log", 1, false);
}

void FR_StartFrame(long frameCounter) {
	g_FlightRecorder->StartMessage(g_FRMsg_Frame, true);
	g_FlightRecorder->WriteInt64(frameCounter);
	g_FlightRecorder->WriteDouble(realtime);
	g_FlightRecorder->EndMessage(g_FRMsg_Frame, true);
}

void FR_EndFrame(long frameCounter) {
	g_FlightRecorder->StartMessage(g_FRMsg_Frame, false);
	g_FlightRecorder->WriteInt64(frameCounter);
	g_FlightRecorder->EndMessage(g_FRMsg_Frame, false);
}

void FR_AllocEntPrivateData(void* res, int size) {
	g_FlightRecorder->StartMessage(g_FRMsg_AllocEntPrivateData, true);
	g_FlightRecorder->WriteUInt32((size_t)res);
	g_FlightRecorder->WriteInt32(size);
	g_FlightRecorder->EndMessage(g_FRMsg_AllocEntPrivateData, true);
}

void FR_FreeEntPrivateData(void* data) {
	g_FlightRecorder->StartMessage(g_FRMsg_FreeEntPrivateData, true);
	g_FlightRecorder->WriteUInt32((size_t)data);
	g_FlightRecorder->EndMessage(g_FRMsg_FreeEntPrivateData, true);
}

void FR_Log(const char* prefix, const char* msg) {
	if (g_FlightRecorder == NULL) { 
		return; //During server initialization some messages could be written in console/log before flightrecorder is initialized
	}
	g_FlightRecorder->StartMessage(g_FRMsg_Log, true);
	g_FlightRecorder->WriteString(prefix);
	g_FlightRecorder->WriteString(msg);
	g_FlightRecorder->EndMessage(g_FRMsg_Log, true);
}

#endif //REHLDS_FLIGHT_REC
