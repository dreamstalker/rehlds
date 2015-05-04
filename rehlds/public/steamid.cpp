#include "precompiled.h"

bool CSteamID::SetFromSteam2String(const char *pchSteam2ID, EUniverse eUniverse)
{
	Assert(pchSteam2ID);

	// Convert the Steam2 ID string to a Steam2 ID structure
	TSteamGlobalUserID steam2ID;
	steam2ID.m_SteamInstanceID = 0;
	steam2ID.m_SteamLocalUserID.Split.High32bits = 0;
	steam2ID.m_SteamLocalUserID.Split.Low32bits = 0;

	const char *pchTSteam2ID = pchSteam2ID;

	// Customer support is fond of entering steam IDs in the following form:  STEAM_n:x:y
	char *pchOptionalLeadString = "STEAM_";
	if (Q_strnicmp(pchSteam2ID, pchOptionalLeadString, Q_strlen(pchOptionalLeadString)) == 0)
		pchTSteam2ID = pchSteam2ID + Q_strlen(pchOptionalLeadString);

	char cExtraCharCheck = 0;

	int cFieldConverted = sscanf(pchTSteam2ID, "%hu:%u:%u%c", &steam2ID.m_SteamInstanceID,
		&steam2ID.m_SteamLocalUserID.Split.High32bits, &steam2ID.m_SteamLocalUserID.Split.Low32bits, &cExtraCharCheck);

	// Validate the conversion ... a special case is steam2 instance ID 1 which is reserved for special DoD handling
	if (cExtraCharCheck != 0 || cFieldConverted == EOF || cFieldConverted < 2 || (cFieldConverted < 3 && steam2ID.m_SteamInstanceID != 1))
		return false;

	// Now convert to steam ID from the Steam2 ID structure
	SetFromSteam2(&steam2ID, eUniverse);
	return true;
}
