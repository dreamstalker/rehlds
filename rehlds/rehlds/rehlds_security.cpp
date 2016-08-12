#include "precompiled.h"

cvar_t sv_rehlds_movecmdrate_max_avg = { "sv_rehlds_movecmdrate_max_avg", "1800", 0, 1800.0f, NULL };
cvar_t sv_rehlds_movecmdrate_max_burst = { "sv_rehlds_movecmdrate_max_burst", "5500", 0, 5500.0f, NULL };
cvar_t sv_rehlds_stringcmdrate_max_avg = { "sv_rehlds_stringcmdrate_max_avg", "250", 0, 250.0f, NULL };
cvar_t sv_rehlds_stringcmdrate_max_burst = { "sv_rehlds_stringcmdrate_max_burst", "500", 0, 500.0f, NULL };

cvar_t sv_rehlds_movecmdrate_avg_punish = { "sv_rehlds_movecmdrate_avg_punish", "5", 0, 5.0f, NULL };
cvar_t sv_rehlds_movecmdrate_burst_punish = { "sv_rehlds_movecmdrate_burst_punish", "5", 0, 5.0f, NULL };
cvar_t sv_rehlds_stringcmdrate_avg_punish = { "sv_rehlds_stringcmdrate_avg_punish", "5", 0, 5.0f, NULL };
cvar_t sv_rehlds_stringcmdrate_burst_punish = { "sv_rehlds_stringcmdrate_burst_punish", "5", 0, 5.0f, NULL };

CMoveCommandRateLimiter g_MoveCommandRateLimiter;
CStringCommandsRateLimiter g_StringCommandsRateLimiter;

CMoveCommandRateLimiter::CMoveCommandRateLimiter() {
	Q_memset(m_AverageMoveCmdRate, 0, sizeof(m_AverageMoveCmdRate));
	Q_memset(m_CurrentMoveCmds, 0, sizeof(m_CurrentMoveCmds));
	m_LastCheckTime = 0.0;
}

void CMoveCommandRateLimiter::UpdateAverageRates(double dt) {
	for (unsigned int i = 0; i < MAX_CLIENTS; i++) {
		m_AverageMoveCmdRate[i] = (2.0 * m_AverageMoveCmdRate[i] / 3.0) + m_CurrentMoveCmds[i] / dt / 3.0;
		m_CurrentMoveCmds[i] = 0;

		CheckAverageRate(i);
	}
}

void CMoveCommandRateLimiter::Frame() {
	double currentTime = realtime;
	double dt = currentTime - m_LastCheckTime;

	if (dt < 0.5) { //refresh avg. rate every 0.5 sec
		return;
	}

	UpdateAverageRates(dt);
	m_LastCheckTime = currentTime;
}

void CMoveCommandRateLimiter::ClientConnected(unsigned int clientId) {
	m_CurrentMoveCmds[clientId] = 0;
	m_AverageMoveCmdRate[clientId] = 0.0f;
}

void CMoveCommandRateLimiter::MoveCommandsIssued(unsigned int clientId, unsigned int numCmds) {
	m_CurrentMoveCmds[clientId] += numCmds;
	CheckBurstRate(clientId);
}

void CMoveCommandRateLimiter::CheckBurstRate(unsigned int clientId) {
	client_t* cl = &g_psvs.clients[clientId];
	if (!cl->active || sv_rehlds_movecmdrate_max_burst.value <= 0.0f) {
		return;
	}

	double dt = realtime - m_LastCheckTime;
	if (dt < 0.2) {
		dt = 0.2; //small intervals may give too high rates
	}
	if ((m_CurrentMoveCmds[clientId] / dt) > sv_rehlds_movecmdrate_max_burst.value) {
		if(sv_rehlds_movecmdrate_burst_punish.value < 0) {
			Con_DPrintf("%s Kicked for move commands flooding (burst) (%.1f)\n", cl->name, (m_CurrentMoveCmds[clientId] / dt));
			SV_DropClient(cl, false, "Kicked for move commands flooding (burst)");
		}
		else
		{
			Con_DPrintf("%s Banned for move commands flooding (burst) (%.1f)\n", cl->name, (m_CurrentMoveCmds[clientId] / dt));
			Cbuf_AddText(va("addip %.1f %s\n", sv_rehlds_movecmdrate_burst_punish.value, NET_BaseAdrToString(cl->netchan.remote_address)));
			SV_DropClient(cl, false, "Banned for move commands flooding (burst)");
		}
	}
}

void CMoveCommandRateLimiter::CheckAverageRate(unsigned int clientId) {
	client_t* cl = &g_psvs.clients[clientId];
	if (!cl->active || sv_rehlds_movecmdrate_max_burst.value <= 0.0f) {
		return;
	}
	
	if (m_AverageMoveCmdRate[clientId] > sv_rehlds_movecmdrate_max_avg.value) {
		if(sv_rehlds_movecmdrate_avg_punish.value < 0) {
			Con_DPrintf("%s Kicked for move commands flooding (Avg) (%.1f)\n", cl->name, m_AverageMoveCmdRate[clientId]);
			SV_DropClient(cl, false, "Kicked for move commands flooding (Avg)");
		}
		else
		{
			Con_DPrintf("%s Banned for move commands flooding (Avg) (%.1f)\n", cl->name, m_AverageMoveCmdRate[clientId]);
			Cbuf_AddText(va("addip %.1f %s\n", sv_rehlds_movecmdrate_avg_punish.value, NET_BaseAdrToString(cl->netchan.remote_address)));
			SV_DropClient(cl, false, "Banned for move commands flooding (Avg)");
		}
	}
}

CStringCommandsRateLimiter::CStringCommandsRateLimiter() {
	Q_memset(m_AverageStringCmdRate, 0, sizeof(m_AverageStringCmdRate));
	Q_memset(m_CurrentStringCmds, 0, sizeof(m_CurrentStringCmds));
	m_LastCheckTime = 0.0;
}

void CStringCommandsRateLimiter::UpdateAverageRates(double dt) {
	for (unsigned int i = 0; i < MAX_CLIENTS; i++) {
		m_AverageStringCmdRate[i] = (2.0 * m_AverageStringCmdRate[i] / 3.0) + m_CurrentStringCmds[i] / dt / 3.0;
		m_CurrentStringCmds[i] = 0;

		CheckAverageRate(i);
	}
}

void CStringCommandsRateLimiter::Frame() {
	double currentTime = realtime;
	double dt = currentTime - m_LastCheckTime;

	if (dt < 0.5) { //refresh avg. rate every 0.5 sec
		return;
	}

	UpdateAverageRates(dt);
	m_LastCheckTime = currentTime;
}

void CStringCommandsRateLimiter::ClientConnected(unsigned int clientId) {
	m_CurrentStringCmds[clientId] = 0;
	m_AverageStringCmdRate[clientId] = 0.0f;
}

void CStringCommandsRateLimiter::StringCommandIssued(unsigned int clientId) {
	m_CurrentStringCmds[clientId]++;
	CheckBurstRate(clientId);
}

void CStringCommandsRateLimiter::CheckBurstRate(unsigned int clientId) {
	client_t* cl = &g_psvs.clients[clientId];
	if (!cl->active || sv_rehlds_stringcmdrate_max_burst.value <= 0.0f) {
		return;
	}

	double dt = realtime - m_LastCheckTime;
	if (dt < 0.2) {
		dt = 0.2; //small intervals may give too high rates
	}
	if ((m_CurrentStringCmds[clientId] / dt) > sv_rehlds_stringcmdrate_max_burst.value) {
		if(sv_rehlds_stringcmdrate_burst_punish.value < 0) {
			Con_DPrintf("%s Kicked for string commands flooding (burst) (%.1f)\n", cl->name, (m_CurrentStringCmds[clientId] / dt));
			SV_DropClient(cl, false, "Kicked for string commands flooding (burst)");
		}
		else
		{
			Con_DPrintf("%s Banned for string commands flooding (burst) (%.1f)\n", cl->name, (m_CurrentStringCmds[clientId] / dt));
			Cbuf_AddText(va("addip %.1f %s\n", sv_rehlds_stringcmdrate_burst_punish.value, NET_BaseAdrToString(cl->netchan.remote_address)));
			SV_DropClient(cl, false, "Banned for string commands flooding (burst)");
		}
	}
}

void CStringCommandsRateLimiter::CheckAverageRate(unsigned int clientId) {
	client_t* cl = &g_psvs.clients[clientId];
	if (!cl->active || sv_rehlds_stringcmdrate_max_burst.value <= 0.0f) {
		return;
	}

	if (m_AverageStringCmdRate[clientId] > sv_rehlds_stringcmdrate_max_avg.value) {
		if(sv_rehlds_stringcmdrate_avg_punish.value < 0) {
			Con_DPrintf("%s Kicked for string commands flooding (Avg) (%.1f)\n", cl->name, m_AverageStringCmdRate[clientId]);
			SV_DropClient(cl, false, "Kicked for string commands flooding (Avg)");
		}
		else
		{
			Con_DPrintf("%s Banned for string commands flooding (Avg) (%.1f)\n", cl->name, m_AverageStringCmdRate[clientId]);
			Cbuf_AddText(va("addip %.1f %s\n", sv_rehlds_stringcmdrate_avg_punish.value, NET_BaseAdrToString(cl->netchan.remote_address)));
			SV_DropClient(cl, false, "Banned for string commands flooding (Avg)");
		}
	}
}

void Rehlds_Security_Init() {
#ifdef REHLDS_FIXES
	Cvar_RegisterVariable(&sv_rehlds_movecmdrate_max_avg);
	Cvar_RegisterVariable(&sv_rehlds_movecmdrate_max_burst);
	Cvar_RegisterVariable(&sv_rehlds_stringcmdrate_max_avg);
	Cvar_RegisterVariable(&sv_rehlds_stringcmdrate_max_burst);

	Cvar_RegisterVariable(&sv_rehlds_movecmdrate_avg_punish);
	Cvar_RegisterVariable(&sv_rehlds_movecmdrate_burst_punish);
	Cvar_RegisterVariable(&sv_rehlds_stringcmdrate_avg_punish);
	Cvar_RegisterVariable(&sv_rehlds_stringcmdrate_burst_punish);
#endif
}

void Rehlds_Security_Shutdown() {
}

void Rehlds_Security_Frame() {
#ifdef REHLDS_FIXES
	g_MoveCommandRateLimiter.Frame();
	g_StringCommandsRateLimiter.Frame();
#endif
}

void Rehlds_Security_ClientConnected(unsigned int clientId) {
#ifdef REHLDS_FIXES
	g_MoveCommandRateLimiter.ClientConnected(clientId);
	g_StringCommandsRateLimiter.ClientConnected(clientId);
#endif	
}
