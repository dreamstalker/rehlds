#include "precompiled.h"

cvar_t sv_rehlds_movecmdrate_max_avg = { "sv_rehlds_movecmdrate_max_avg", "750", 0, 750.0f, NULL };
cvar_t sv_rehlds_movecmdrate_max_burst = { "sv_rehlds_movecmdrate_max_burst", "3500", 0, 3500.0f, NULL };

CMoveCommandRateLimiter g_MoveCommandRateLimiter;

CMoveCommandRateLimiter::CMoveCommandRateLimiter() {
	memset(m_AverageMoveCmdRate, 0, sizeof(m_AverageMoveCmdRate));
	memset(m_CurrentMoveCmds, 0, sizeof(m_CurrentMoveCmds));
	m_LastCheckTime = 0;
}

void CMoveCommandRateLimiter::UpdateAverageRates(double currentTime) {
	double dt = currentTime - m_LastCheckTime;
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

	UpdateAverageRates(currentTime);
	m_LastCheckTime = currentTime;
}

void CMoveCommandRateLimiter::ClientConnected(unsigned int clientId) {
	if (clientId >= (unsigned)g_psvs.maxclients) {
		rehlds_syserror(__FUNCTION__": Invalid clientId %u", clientId);
	}

	m_CurrentMoveCmds[clientId] = 0;
	m_AverageMoveCmdRate[clientId] = 0.0f;
}

void CMoveCommandRateLimiter::MoveCommandsIssued(unsigned int clientId, unsigned int numCmds) {
	if (clientId >= (unsigned)g_psvs.maxclients) {
		rehlds_syserror(__FUNCTION__": Invalid clientId %u", clientId);
	}

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
		Cbuf_AddText(va("addip %i %s\n", 5, NET_BaseAdrToString(cl->netchan.remote_address)));
		SV_DropClient(cl, false, "Banned for move commands flooding (burst)");
	}
}

void CMoveCommandRateLimiter::CheckAverageRate(unsigned int clientId) {
	client_t* cl = &g_psvs.clients[clientId];
	if (!cl->active || sv_rehlds_movecmdrate_max_burst.value <= 0.0f) {
		return;
	}

	if (m_AverageMoveCmdRate[clientId] > sv_rehlds_movecmdrate_max_avg.value) {
		Cbuf_AddText(va("addip %i %s\n", 5, NET_BaseAdrToString(cl->netchan.remote_address)));
		SV_DropClient(cl, false, "Banned for move commands flooding (Avg)");
	}
}

void Rehlds_Security_Init() {
#ifdef REHLDS_FIXES
	Cvar_RegisterVariable(&sv_rehlds_movecmdrate_max_avg);
	Cvar_RegisterVariable(&sv_rehlds_movecmdrate_max_burst);
#endif
}

void Rehlds_Security_Shutdown() {
}

void Rehlds_Security_Frame() {
#ifdef REHLDS_FIXES
	g_MoveCommandRateLimiter.Frame();
#endif
}

void Rehlds_Security_ClientConnected(unsigned int clientId) {
#ifdef REHLDS_FIXES
	g_MoveCommandRateLimiter.ClientConnected(clientId);
#endif	
}