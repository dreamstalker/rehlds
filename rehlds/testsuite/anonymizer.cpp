#include "precompiled.h"

CSteamCallbackAnonymizingWrapper::CSteamCallbackAnonymizingWrapper(CAnonymizingEngExtInterceptor* anonymizer, CCallbackBase* cb, int id)
{
	m_Anonymizer = anonymizer;
	m_Wrapped = cb;
	m_Id = id;
	m_Size = cb->GetCallbackSizeBytes();
	m_iCallback = cb->GetICallback();
	this->m_nCallbackFlags = cb->GetFlags();
}

void* CSteamCallbackAnonymizingWrapper::Anonymize(void* data) {
	switch (m_iCallback) {
	case GSClientApprove_t::k_iCallback:
	{
		static GSClientApprove_t cbdata;
		cbdata = *(GSClientApprove_t*)data;
		cbdata.m_SteamID = m_Anonymizer->Real2FakeSteamId(cbdata.m_SteamID, __func__);
		return &cbdata;
	}

	case GSClientDeny_t::k_iCallback:
	{
		static GSClientDeny_t cbdata;
		cbdata = *(GSClientDeny_t*)data;
		cbdata.m_SteamID = m_Anonymizer->Real2FakeSteamId(cbdata.m_SteamID, __func__);
		return &cbdata;
	}

	case GSClientKick_t::k_iCallback:
	{
		static GSClientKick_t cbdata;
		cbdata = *(GSClientKick_t*)data;
		cbdata.m_SteamID = m_Anonymizer->Real2FakeSteamId(cbdata.m_SteamID, __func__);
		return &cbdata;
	}

	case GSPolicyResponse_t::k_iCallback:
	case SteamServersConnected_t::k_iCallback:
	case SteamServerConnectFailure_t::k_iCallback:
		return data;

	default:
		rehlds_syserror("%s: unsupported callback %u", __func__, m_iCallback);
	}
}

void* CSteamCallbackAnonymizingWrapper::Anonymize(void* data, bool bIOFailure, SteamAPICall_t hSteamAPICall) {
	rehlds_syserror("%s: not implemented", __func__);
	//return NULL;
}


void CSteamCallbackAnonymizingWrapper::Run(void *pvParam)
{
	if (m_Wrapped->GetICallback() != this->GetICallback()) rehlds_syserror("%s: iCallback desync", __func__);
	if (m_Wrapped->GetFlags() != this->GetFlags()) rehlds_syserror("%s: flags desync", __func__);

	m_Wrapped->Run(Anonymize(pvParam));
	this->SetFlags(m_Wrapped->GetFlags());
	this->SetICallback(m_Wrapped->GetICallback());
}

void CSteamCallbackAnonymizingWrapper::Run(void *pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall)
{
	if (m_Wrapped->GetICallback() != this->GetICallback()) rehlds_syserror("%s: iCallback desync", __func__);
	if (m_Wrapped->GetFlags() != this->GetFlags()) rehlds_syserror("%s: flags desync", __func__);

	m_Wrapped->Run(Anonymize(pvParam, bIOFailure, hSteamAPICall), bIOFailure, hSteamAPICall);
	this->SetFlags(m_Wrapped->GetFlags());
	this->SetICallback(m_Wrapped->GetICallback());
}

int CSteamCallbackAnonymizingWrapper::GetCallbackSizeBytes()
{
	return m_Wrapped->GetCallbackSizeBytes();
}





CSteamAppsAnonymizingWrapper::CSteamAppsAnonymizingWrapper(ISteamApps* original, CAnonymizingEngExtInterceptor* anonymizer)
{
	m_Wrapped = original;
	m_Anonymizer = anonymizer;
}

bool CSteamAppsAnonymizingWrapper::BIsSubscribed()
{
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

bool CSteamAppsAnonymizingWrapper::BIsLowViolence()
{
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

bool CSteamAppsAnonymizingWrapper::BIsCybercafe()
{
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

bool CSteamAppsAnonymizingWrapper::BIsVACBanned()
{
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

const char* CSteamAppsAnonymizingWrapper::GetCurrentGameLanguage()
{
	const char* res = m_Wrapped->GetCurrentGameLanguage();
	return res;
}

const char* CSteamAppsAnonymizingWrapper::GetAvailableGameLanguages()
{
	rehlds_syserror("%s: not implemented", __func__);
	//return NULL;
}

bool CSteamAppsAnonymizingWrapper::BIsSubscribedApp(AppId_t appID)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

bool CSteamAppsAnonymizingWrapper::BIsDlcInstalled(AppId_t appID)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

uint32 CSteamAppsAnonymizingWrapper::GetEarliestPurchaseUnixTime(AppId_t nAppID)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return 0;
}

bool CSteamAppsAnonymizingWrapper::BIsSubscribedFromFreeWeekend()
{
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

int CSteamAppsAnonymizingWrapper::GetDLCCount()
{
	rehlds_syserror("%s: not implemented", __func__);
	//return 0;
}

bool CSteamAppsAnonymizingWrapper::BGetDLCDataByIndex(int iDLC, AppId_t *pAppID, bool *pbAvailable, char *pchName, int cchNameBufferSize)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

void CSteamAppsAnonymizingWrapper::InstallDLC(AppId_t nAppID)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamAppsAnonymizingWrapper::UninstallDLC(AppId_t nAppID)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamAppsAnonymizingWrapper::RequestAppProofOfPurchaseKey(AppId_t nAppID)
{
	rehlds_syserror("%s: not implemented", __func__);
}

bool CSteamAppsAnonymizingWrapper::GetCurrentBetaName(char *pchName, int cchNameBufferSize)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

bool CSteamAppsAnonymizingWrapper::MarkContentCorrupt(bool bMissingFilesOnly)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

uint32 CSteamAppsAnonymizingWrapper::GetInstalledDepots(DepotId_t *pvecDepots, uint32 cMaxDepots)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return 0;
}

uint32 CSteamAppsAnonymizingWrapper::GetAppInstallDir(AppId_t appID, char *pchFolder, uint32 cchFolderBufferSize)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return 0;
}






CSteamGameServerAnonymizingWrapper::CSteamGameServerAnonymizingWrapper(ISteamGameServer* original, CAnonymizingEngExtInterceptor* anonymizer)
{
	m_Wrapped = original;
	m_Anonymizer = anonymizer;
}

bool CSteamGameServerAnonymizingWrapper::InitGameServer(uint32 unIP, uint16 usGamePort, uint16 usQueryPort, uint32 unFlags, AppId_t nGameAppId, const char *pchVersionString)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

void CSteamGameServerAnonymizingWrapper::SetProduct(const char *pszProduct)
{
	m_Wrapped->SetProduct(pszProduct);
}

void CSteamGameServerAnonymizingWrapper::SetGameDescription(const char *pszGameDescription)
{
	m_Wrapped->SetGameDescription(pszGameDescription);
}

void CSteamGameServerAnonymizingWrapper::SetModDir(const char *pszModDir)
{
	m_Wrapped->SetModDir(pszModDir);
}

void CSteamGameServerAnonymizingWrapper::SetDedicatedServer(bool bDedicated)
{
	m_Wrapped->SetDedicatedServer(bDedicated);
}

void CSteamGameServerAnonymizingWrapper::LogOn(const char *pszAccountName, const char *pszPassword)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerAnonymizingWrapper::LogOnAnonymous()
{
	m_Wrapped->LogOnAnonymous();
}

void CSteamGameServerAnonymizingWrapper::LogOff()
{
	m_Wrapped->LogOff();
}

bool CSteamGameServerAnonymizingWrapper::BLoggedOn()
{
	bool res = m_Wrapped->BLoggedOn();
	return res;
}

bool CSteamGameServerAnonymizingWrapper::BSecure()
{
	bool res = m_Wrapped->BSecure();
	return res;
}

CSteamID CSteamGameServerAnonymizingWrapper::GetSteamID()
{
	CSteamID res = m_Wrapped->GetSteamID();
	return CSteamID(666, 1, k_EUniversePublic, k_EAccountTypeAnonGameServer);
}

bool CSteamGameServerAnonymizingWrapper::WasRestartRequested()
{
	bool res = m_Wrapped->WasRestartRequested();
	return res;
}

void CSteamGameServerAnonymizingWrapper::SetMaxPlayerCount(int cPlayersMax)
{
	m_Wrapped->SetMaxPlayerCount(cPlayersMax);
}

void CSteamGameServerAnonymizingWrapper::SetBotPlayerCount(int cBotplayers)
{
	m_Wrapped->SetBotPlayerCount(cBotplayers);
}

void CSteamGameServerAnonymizingWrapper::SetServerName(const char *pszServerName)
{
	m_Wrapped->SetServerName(pszServerName);
}

void CSteamGameServerAnonymizingWrapper::SetMapName(const char *pszMapName)
{
	m_Wrapped->SetMapName(pszMapName);
}

void CSteamGameServerAnonymizingWrapper::SetPasswordProtected(bool bPasswordProtected)
{
	m_Wrapped->SetPasswordProtected(bPasswordProtected);
}

void CSteamGameServerAnonymizingWrapper::SetSpectatorPort(uint16 unSpectatorPort)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerAnonymizingWrapper::SetSpectatorServerName(const char *pszSpectatorServerName)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerAnonymizingWrapper::ClearAllKeyValues()
{
	m_Wrapped->ClearAllKeyValues();
}

void CSteamGameServerAnonymizingWrapper::SetKeyValue(const char *pKey, const char *pValue)
{
	m_Wrapped->SetKeyValue(pKey, pValue);
}

void CSteamGameServerAnonymizingWrapper::SetGameTags(const char *pchGameTags)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerAnonymizingWrapper::SetGameData(const char *pchGameData)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerAnonymizingWrapper::SetRegion(const char *pszRegion)
{
	rehlds_syserror("%s: not implemented", __func__);
}

bool CSteamGameServerAnonymizingWrapper::SendUserConnectAndAuthenticate(uint32 unIPClient, const void *pvAuthBlob, uint32 cubAuthBlobSize, CSteamID *pSteamIDUser)
{
	uint32 realIp = m_Anonymizer->Fake2RealIp(ntohl(unIPClient), __func__);
	bool res = m_Wrapped->SendUserConnectAndAuthenticate(htonl(realIp), pvAuthBlob, cubAuthBlobSize, pSteamIDUser);
	if (res) {
		*pSteamIDUser = m_Anonymizer->Real2FakeSteamId(*pSteamIDUser, __func__);
	}
	return res;
}

CSteamID CSteamGameServerAnonymizingWrapper::CreateUnauthenticatedUserConnection()
{
	CSteamID res = m_Wrapped->CreateUnauthenticatedUserConnection();
	return res;
}

void CSteamGameServerAnonymizingWrapper::SendUserDisconnect(CSteamID steamIDUser)
{
	CSteamID real = m_Anonymizer->Fake2RealSteamId(steamIDUser, __func__);
	m_Wrapped->SendUserDisconnect(real);
}

bool CSteamGameServerAnonymizingWrapper::BUpdateUserData(CSteamID steamIDUser, const char *pchPlayerName, uint32 uScore)
{
	CSteamID real = steamIDUser.BAnonAccount() ? steamIDUser : m_Anonymizer->Fake2RealSteamId(steamIDUser, __func__);
	std::string realName = m_Anonymizer->Fake2RealName(pchPlayerName, __func__);

	bool res = m_Wrapped->BUpdateUserData(real, realName.c_str(), uScore);
	return res;
}

HAuthTicket CSteamGameServerAnonymizingWrapper::GetAuthSessionTicket(void *pTicket, int cbMaxTicket, uint32 *pcbTicket)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return k_HAuthTicketInvalid;
}

EBeginAuthSessionResult CSteamGameServerAnonymizingWrapper::BeginAuthSession(const void *pAuthTicket, int cbAuthTicket, CSteamID steamID)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return k_EBeginAuthSessionResultInvalidTicket;
}

void CSteamGameServerAnonymizingWrapper::EndAuthSession(CSteamID steamID)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerAnonymizingWrapper::CancelAuthTicket(HAuthTicket hAuthTicket)
{
	rehlds_syserror("%s: not implemented", __func__);
}

EUserHasLicenseForAppResult CSteamGameServerAnonymizingWrapper::UserHasLicenseForApp(CSteamID steamID, AppId_t appID)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return k_EUserHasLicenseResultDoesNotHaveLicense;
}

bool CSteamGameServerAnonymizingWrapper::RequestUserGroupStatus(CSteamID steamIDUser, CSteamID steamIDGroup)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

void CSteamGameServerAnonymizingWrapper::GetGameplayStats()
{
	rehlds_syserror("%s: not implemented", __func__);
}

SteamAPICall_t CSteamGameServerAnonymizingWrapper::GetServerReputation()
{
	rehlds_syserror("%s: not implemented", __func__);
	//return k_uAPICallInvalid;
}

uint32 CSteamGameServerAnonymizingWrapper::GetPublicIP()
{
	rehlds_syserror("%s: not implemented", __func__);
	//return 0;
}

bool CSteamGameServerAnonymizingWrapper::HandleIncomingPacket(const void *pData, int cbData, uint32 srcIP, uint16 srcPort)
{
	uint32 realIp = m_Anonymizer->Fake2RealIp(htonl(srcIP), __func__);
	
	bool res;
	if (m_Anonymizer->m_OriginalConnectPacketLen) {
		res = m_Wrapped->HandleIncomingPacket(m_Anonymizer->m_OriginalConnectPacketData, m_Anonymizer->m_OriginalConnectPacketLen, ntohl(realIp), srcPort);
		m_Anonymizer->m_OriginalConnectPacketLen = 0;
	} else {
		res = m_Wrapped->HandleIncomingPacket(pData, cbData, ntohl(realIp), srcPort);
	}

	return res;
}

int CSteamGameServerAnonymizingWrapper::GetNextOutgoingPacket(void *pOut, int cbMaxOut, uint32 *pNetAdr, uint16 *pPort)
{
	int res = m_Wrapped->GetNextOutgoingPacket(pOut, cbMaxOut, pNetAdr, pPort);
	if (res > 0) {
		uint32 fakeIp = m_Anonymizer->Real2FakeIp(ntohl(*pNetAdr), __func__);
		*pNetAdr = htonl(fakeIp);

		//Clear players list
		if (res > 6 && *(uint32*)pOut == 0xFFFFFFFF && ((uint8*)pOut)[4] == 0x44) {
			memset((uint8*)pOut + 6, 0, res - 6);
		}

		//Clear serverinfo
		if (res > 6 && *(uint32*)pOut == 0xFFFFFFFF && ((uint8*)pOut)[4] == 0x49) {
			memset((uint8*)pOut + 6, 0, res - 6);
		}
	}
	return res;
}

void CSteamGameServerAnonymizingWrapper::EnableHeartbeats(bool bActive)
{
	m_Wrapped->EnableHeartbeats(bActive);
}

void CSteamGameServerAnonymizingWrapper::SetHeartbeatInterval(int iHeartbeatInterval)
{
	m_Wrapped->SetHeartbeatInterval(iHeartbeatInterval);
}

void CSteamGameServerAnonymizingWrapper::ForceHeartbeat()
{
	rehlds_syserror("%s: not implemented", __func__);
}

SteamAPICall_t CSteamGameServerAnonymizingWrapper::AssociateWithClan(CSteamID steamIDClan)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return k_uAPICallInvalid;
}

SteamAPICall_t CSteamGameServerAnonymizingWrapper::ComputeNewPlayerCompatibility(CSteamID steamIDNewPlayer)
{
	rehlds_syserror("%s: not implemented", __func__);
	//return k_uAPICallInvalid;
}







CAnonymizingEngExtInterceptor::CAnonymizingEngExtInterceptor(IReHLDSPlatform* basePlatform)
{
	m_ServerSocket = INVALID_SOCKET;
	m_SteamCallbacksCounter = 0;
	m_SteamAppsWrapper = NULL;
	m_GameServerWrapper = NULL;
	m_SteamBreakpadContext = NULL;
	m_BasePlatform = basePlatform;
	m_OriginalConnectPacketLen = 0;
}

uint32 CAnonymizingEngExtInterceptor::time(uint32* pTime)
{
	uint32 res = m_BasePlatform->time(pTime);
	return res;
}

struct tm* CAnonymizingEngExtInterceptor::localtime(uint32 time)
{
	struct tm* res = m_BasePlatform->localtime(time);
	return res;
}

void CAnonymizingEngExtInterceptor::srand(uint32 seed)
{
	m_BasePlatform->srand(seed);
}

int CAnonymizingEngExtInterceptor::rand()
{
	int res = m_BasePlatform->rand();
	return res;
}

void CAnonymizingEngExtInterceptor::Sleep(DWORD msec)
{
	m_BasePlatform->Sleep(msec);
}

BOOL CAnonymizingEngExtInterceptor::QueryPerfCounter(LARGE_INTEGER* counter)
{
	BOOL res = m_BasePlatform->QueryPerfCounter(counter);
	return res;
}

BOOL CAnonymizingEngExtInterceptor::QueryPerfFreq(LARGE_INTEGER* counter)
{
	BOOL res = m_BasePlatform->QueryPerfFreq(counter);
	return res;
}

DWORD CAnonymizingEngExtInterceptor::GetTickCount()
{
	DWORD res = m_BasePlatform->GetTickCount();
	return res;
}

void CAnonymizingEngExtInterceptor::GetLocalTime(LPSYSTEMTIME time)
{
	m_BasePlatform->GetLocalTime(time);
}

void CAnonymizingEngExtInterceptor::GetSystemTime(LPSYSTEMTIME time)
{
	m_BasePlatform->GetSystemTime(time);
}

void CAnonymizingEngExtInterceptor::GetTimeZoneInfo(LPTIME_ZONE_INFORMATION zinfo)
{
	m_BasePlatform->GetTimeZoneInfo(zinfo);
}

BOOL CAnonymizingEngExtInterceptor::GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime)
{
	BOOL res = m_BasePlatform->GetProcessTimes(hProcess, lpCreationTime, lpExitTime, lpKernelTime, lpUserTime);
	return res;
}

void CAnonymizingEngExtInterceptor::GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime)
{
	m_BasePlatform->GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
}

SOCKET CAnonymizingEngExtInterceptor::socket(int af, int type, int protocol)
{
	SOCKET s = m_BasePlatform->socket(af, type, protocol);
	return s;
}

int CAnonymizingEngExtInterceptor::ioctlsocket(SOCKET s, long cmd, u_long *argp)
{
	int res = m_BasePlatform->ioctlsocket(s, cmd, argp);
	return res;
}

int CAnonymizingEngExtInterceptor::setsockopt(SOCKET s, int level, int optname, const char* optval, int optlen)
{
	int res = m_BasePlatform->setsockopt(s, level, optname, optval, optlen);
	return res;
}

int CAnonymizingEngExtInterceptor::closesocket(SOCKET s)
{
	int res = m_BasePlatform->closesocket(s);
	return res;
}

int CAnonymizingEngExtInterceptor::recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, socklen_t *fromlen)
{
	int res = m_BasePlatform->recvfrom(s, buf, len, flags, from, fromlen);
	if (res > 0) {
		Real2FakeSockaddr(from, __func__);
		if (res > 4 && (*(uint32*)buf) == 0xFFFFFFFF) {
			unsigned int localLen = res;
			ProcessConnectionlessPacket((uint8*)buf, &localLen);
			res = localLen;
		}
	}
	return res;
}

int CAnonymizingEngExtInterceptor::sendto(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen)
{

	sockaddr saddr;
	memcpy(&saddr, to, sizeof(sockaddr_in));
	Fake2RealSockaddr(&saddr, __func__);

	int res = m_BasePlatform->sendto(s, buf, len, flags, &saddr, tolen);
	return res;
}

int CAnonymizingEngExtInterceptor::bind(SOCKET s, const struct sockaddr* addr, int namelen)
{
	int res = m_BasePlatform->bind(s, addr, namelen);
	return res;
}

int CAnonymizingEngExtInterceptor::getsockname(SOCKET s, struct sockaddr* name, socklen_t* namelen)
{
	int res = m_BasePlatform->getsockname(s, name, namelen);
	return res;
}

int CAnonymizingEngExtInterceptor::WSAGetLastError()
{
	int res = m_BasePlatform->WSAGetLastError();
	return res;
}

struct hostent* CAnonymizingEngExtInterceptor::gethostbyname(const char *name)
{
	auto s = Fake2RealHost(name, __func__);
	struct hostent* res = m_BasePlatform->gethostbyname(s.c_str());
	return res;
}

int CAnonymizingEngExtInterceptor::gethostname(char *name, int namelen)
{
	int res = m_BasePlatform->gethostname(name, namelen);
	if (res == 0) {
		auto s = Real2FakeHost(name, __func__);
		strncpy(name, s.c_str(), namelen);
		name[namelen - 1] = 0;
	}
	return res;
}


CSteamCallbackAnonymizingWrapper* CAnonymizingEngExtInterceptor::getOrCreateCallbackWrapper(CCallbackBase *pCallback)
{
	auto itr = m_SteamCallbacks.find(pCallback);
	if (itr == m_SteamCallbacks.end())
	{
		CSteamCallbackAnonymizingWrapper* wrappee = new CSteamCallbackAnonymizingWrapper(this, pCallback, m_SteamCallbacksCounter++);
		m_SteamCallbacks[pCallback] = wrappee;
		return wrappee;
	}
	else
	{
		return (*itr).second;
	}
}

void CAnonymizingEngExtInterceptor::SteamAPI_RegisterCallback(CCallbackBase *pCallback, int iCallback)
{
	CSteamCallbackAnonymizingWrapper* wrappee = getOrCreateCallbackWrapper(pCallback);

	if (wrappee->GetFlags() != pCallback->GetFlags()) rehlds_syserror("%s: flags desync", __func__);
	//if (wrappee->GetICallback() != pCallback->GetICallback()) rehlds_syserror("%s: flags desync", __func__);

	m_BasePlatform->SteamAPI_RegisterCallback(wrappee, iCallback);

	pCallback->SetFlags(wrappee->GetFlags());
	pCallback->SetICallback(wrappee->GetICallback());

}

bool CAnonymizingEngExtInterceptor::SteamAPI_Init()
{
	bool res = m_BasePlatform->SteamAPI_Init();
	return res;
}

void CAnonymizingEngExtInterceptor::SteamAPI_UnregisterCallResult(class CCallbackBase *pCallback, SteamAPICall_t hAPICall)
{
	CSteamCallbackAnonymizingWrapper* wrappee = getOrCreateCallbackWrapper(pCallback);

	if (wrappee->GetFlags() != pCallback->GetFlags()) rehlds_syserror("%s: flags desync", __func__);
	if (wrappee->GetICallback() != pCallback->GetICallback()) rehlds_syserror("%s: flags desync", __func__);

	m_BasePlatform->SteamAPI_UnregisterCallResult(wrappee, hAPICall);

	pCallback->SetFlags(wrappee->GetFlags());
	pCallback->SetICallback(wrappee->GetICallback());
}

ISteamApps* CAnonymizingEngExtInterceptor::SteamApps()
{
	if (m_SteamAppsWrapper == NULL)
	{
		ISteamApps* orig = m_BasePlatform->SteamApps();
		if (orig != NULL)
			m_SteamAppsWrapper = new CSteamAppsAnonymizingWrapper(orig, this);
	} else {
		m_BasePlatform->SteamApps();
	}
	
	return m_SteamAppsWrapper;
}

bool CAnonymizingEngExtInterceptor::SteamGameServer_Init(uint32 unIP, uint16 usSteamPort, uint16 usGamePort, uint16 usQueryPort, EServerMode eServerMode, const char *pchVersionString)
{
	bool res = m_BasePlatform->SteamGameServer_Init(unIP, usSteamPort, usGamePort, usQueryPort, eServerMode, pchVersionString);
	return res;
}

ISteamGameServer* CAnonymizingEngExtInterceptor::SteamGameServer()
{
	if (m_GameServerWrapper == NULL)
	{
		ISteamGameServer* orig = m_BasePlatform->SteamGameServer();
		if (orig != NULL)
			m_GameServerWrapper = new CSteamGameServerAnonymizingWrapper(orig, this);
	} else {
		m_BasePlatform->SteamGameServer();
	}

	return m_GameServerWrapper;
}

void CAnonymizingEngExtInterceptor::SteamAPI_SetBreakpadAppID(uint32 unAppID)
{
	m_BasePlatform->SteamAPI_SetBreakpadAppID(unAppID);
}

void CAnonymizingEngExtInterceptor::SteamAPI_UseBreakpadCrashHandler(char const *pchVersion, char const *pchDate, char const *pchTime, bool bFullMemoryDumps, void *pvContext, PFNPreMinidumpCallback m_pfnPreMinidumpCallback)
{
	m_BasePlatform->SteamAPI_UseBreakpadCrashHandler(pchVersion, pchDate, pchTime, bFullMemoryDumps, pvContext, m_pfnPreMinidumpCallback);
}

void CAnonymizingEngExtInterceptor::SteamGameServer_RunCallbacks()
{
	m_BasePlatform->SteamGameServer_RunCallbacks();
}

void CAnonymizingEngExtInterceptor::SteamAPI_RunCallbacks()
{
	m_BasePlatform->SteamAPI_RunCallbacks();
}

void CAnonymizingEngExtInterceptor::SteamGameServer_Shutdown()
{
	m_BasePlatform->SteamGameServer_Shutdown();
	m_GameServerWrapper = NULL;
}

void CAnonymizingEngExtInterceptor::SteamAPI_UnregisterCallback(CCallbackBase *pCallback)
{
	CSteamCallbackAnonymizingWrapper* wrappee = getOrCreateCallbackWrapper(pCallback);

	if (wrappee->GetFlags() != pCallback->GetFlags()) rehlds_syserror("%s: flags desync", __func__);
	if (wrappee->GetICallback() != pCallback->GetICallback()) rehlds_syserror("%s: flags desync", __func__);

	m_BasePlatform->SteamAPI_UnregisterCallback(wrappee);

	pCallback->SetFlags(wrappee->GetFlags());
	pCallback->SetICallback(wrappee->GetICallback());
}

void CAnonymizingEngExtInterceptor::AnonymizeAddr(const char* real, const char* fake) {
	netadr_t realAdr;
	netadr_t fakeAdr;

	if (!NET_StringToAdr(real, &realAdr)) {
		rehlds_syserror("%s: Invalid address %s", __func__, realAdr);
	}

	if (!NET_StringToAdr(fake, &fakeAdr)) {
		rehlds_syserror("%s: Invalid address %s", __func__, realAdr);
	}

	AnonymizeAddr(realAdr, fakeAdr);
}

void CAnonymizingEngExtInterceptor::AnonymizeAddr(const netadr_t& real, const netadr_t& fake) {
	uint32 realIp = *(uint32*)(&real.ip[0]);
	uint32 fakeIp = *(uint32*)(&fake.ip[0]);

	m_Fake2RealIpMap[fakeIp] = realIp;
	m_Real2FakeIpMap[realIp] = fakeIp;
}

uint32 CAnonymizingEngExtInterceptor::Fake2RealIp(uint32 fakeIp, const char* callsite) {
	auto itr = m_Fake2RealIpMap.find(fakeIp);
	if (itr == m_Fake2RealIpMap.end()) {
		rehlds_syserror("%s: Unmapped fake addr %s", callsite, IpToString(fakeIp));
	}

	return itr->second;
}

uint32 CAnonymizingEngExtInterceptor::Real2FakeIp(uint32 realIp, const char* callsite) {
	auto itr = m_Real2FakeIpMap.find(realIp);
	if (itr == m_Real2FakeIpMap.end()) {
		Con_Printf("%s: Unmapped real addr %s\n", callsite, IpToString(realIp));
		m_Real2FakeIpMap[realIp] = realIp;
		m_Fake2RealIpMap[realIp] = realIp;
		return realIp;
	}

	return itr->second;
}

void CAnonymizingEngExtInterceptor::Real2FakeSockaddr(sockaddr* saddr, const char* callsite) {
	sockaddr_in* inaddr = (sockaddr_in*)saddr;
	uint32 realIp = *(uint32*)(&inaddr->sin_addr);
	uint32 fakeIp = Real2FakeIp(realIp, callsite);

	*(uint32*)(&inaddr->sin_addr) = fakeIp;
}

void CAnonymizingEngExtInterceptor::Fake2RealSockaddr(sockaddr* saddr, const char* callsite) {
	sockaddr_in* inaddr = (sockaddr_in*)saddr;
	uint32 fakeIp = *(uint32*)(&inaddr->sin_addr);
	uint32 realIp = Fake2RealIp(fakeIp, callsite);

	*(uint32*)(&inaddr->sin_addr) = realIp;
}

char* CAnonymizingEngExtInterceptor::IpToString(uint32 ip) {
	static char buf[64];
	sprintf(buf, "%u.%u.%u.%u", ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
	return buf;
}

CSteamID CAnonymizingEngExtInterceptor::Real2FakeSteamId(CSteamID realId, const char* callsite) {
	auto itr = m_Real2FakeSteamId.find(realId.ConvertToUint64());
	if (itr == m_Real2FakeSteamId.end()) {
		rehlds_syserror("%s: Unmapped real steamid %s", callsite, SteamIdToString(realId));
	}

	return CSteamID(itr->second);
}

CSteamID CAnonymizingEngExtInterceptor::Fake2RealSteamId(CSteamID fakeId, const char* callsite) {
	auto itr = m_Fake2RealSteamId.find(fakeId.ConvertToUint64());
	if (itr == m_Fake2RealSteamId.end()) {
		rehlds_syserror("%s: Unmapped fake steamid %s", callsite, SteamIdToString(fakeId));
	}

	return CSteamID(itr->second);
}

char* CAnonymizingEngExtInterceptor::SteamIdToString(const CSteamID& steamid) {
	USERID_t userid;
	userid.clientip = 0;
	userid.idtype = AUTH_IDTYPE_STEAM;
	userid.m_SteamID = steamid.ConvertToUint64();
	return SV_GetIDString_internal(&userid);
}

void CAnonymizingEngExtInterceptor::AnonymizeSteamId(const CSteamID& real, const CSteamID& fake) {
	m_Real2FakeSteamId[real.ConvertToUint64()] = fake.ConvertToUint64();
	m_Fake2RealSteamId[fake.ConvertToUint64()] = real.ConvertToUint64();
}

void CAnonymizingEngExtInterceptor::AnonymizeSteamId(const char* real, const char* fake) {
	CSteamID realId;
	realId.SetFromSteam2String(real, k_EUniversePublic);

	CSteamID fakeId;
	fakeId.SetFromSteam2String(fake, k_EUniversePublic);

	AnonymizeSteamId(realId, fakeId);
}

void CAnonymizingEngExtInterceptor::ProcessConnectionlessPacket(uint8* data, unsigned int *len) {
	memcpy(net_message.data, data, *len);
	net_message.cursize = *len;
	MSG_BeginReading();
	MSG_ReadLong(); // 0xFFFFFFFF

	char* args = MSG_ReadStringLine();
	Cmd_TokenizeString(args);

	const char* cmd = Cmd_Argv(0);
	if (!strcmp(cmd, "connect")) {
		ProcessConnectPacket(data, len);
	}
}

std::string CAnonymizingEngExtInterceptor::Real2FakeName(const char* realName, const char* callsite) {
	std::string r(realName);
	auto itr = m_Real2FakeName.find(r);
	if (itr == m_Real2FakeName.end()) {
		rehlds_syserror("%s: no fake mapping for real name '%s'", callsite, realName);
	}

	return itr->second;
}

std::string CAnonymizingEngExtInterceptor::Fake2RealName(const char* fakeName, const char* callsite) {
	std::string r(fakeName);
	auto itr = m_Fake2RealName.find(r);
	if (itr == m_Fake2RealName.end()) {
		rehlds_syserror("%s: no real mapping for fake name '%s'", callsite, fakeName);
	}

	return itr->second;
}

void CopyInfoKey(char* from, char* to, const char* key) {
	const char *v = Info_ValueForKey(from, key);
	if (v[0]) {
		Info_SetValueForStarKey(to, key, v, MAX_INFO_STRING);
	}
}

void CAnonymizingEngExtInterceptor::ProcessConnectPacket(uint8* data, unsigned int *len) {
	char origuserinfo[1024];
	char userinfo[1024];
	char protinfo[1024];

	memcpy(m_OriginalConnectPacketData, data, *len);
	m_OriginalConnectPacketLen = *len;

	int protocol = Q_atoi(Cmd_Argv(1));
	int challenge = Q_atoi(Cmd_Argv(2));
	
	strncpy(protinfo, Cmd_Argv(3), ARRAYSIZE(protinfo));
	protinfo[ARRAYSIZE(protinfo) - 1] = 0;

	strncpy(origuserinfo, Cmd_Argv(4), ARRAYSIZE(origuserinfo));
	origuserinfo[ARRAYSIZE(origuserinfo) - 1] = 0;

	bool isSteam = (3 == atoi(Info_ValueForKey(protinfo, "prot")));

	std::string newName = Real2FakeName(Info_ValueForKey(origuserinfo, "name"), __func__);
	Info_SetValueForKey(origuserinfo, "name", newName.c_str(), MAX_INFO_STRING);

	userinfo[0] = 0;
	CopyInfoKey(origuserinfo, userinfo, "name");
	CopyInfoKey(origuserinfo, userinfo, "rate");
	CopyInfoKey(origuserinfo, userinfo, "hslots");
	CopyInfoKey(origuserinfo, userinfo, "hdelay");
	CopyInfoKey(origuserinfo, userinfo, "hspecs");
	CopyInfoKey(origuserinfo, userinfo, "cl_updaterate");
	CopyInfoKey(origuserinfo, userinfo, "*hltv");
	CopyInfoKey(origuserinfo, userinfo, "*hid");
	CopyInfoKey(origuserinfo, userinfo, "cl_autowepswitch");
	CopyInfoKey(origuserinfo, userinfo, "_cl_autowepswitch");
	CopyInfoKey(origuserinfo, userinfo, "_vgui_menus");
	CopyInfoKey(origuserinfo, userinfo, "_ah");
	CopyInfoKey(origuserinfo, userinfo, "cl_dlmax");
	CopyInfoKey(origuserinfo, userinfo, "model");
	CopyInfoKey(origuserinfo, userinfo, "topcolor");
	CopyInfoKey(origuserinfo, userinfo, "bottomcolor");
	CopyInfoKey(origuserinfo, userinfo, "cl_lw");
	CopyInfoKey(origuserinfo, userinfo, "cl_lc");
	CopyInfoKey(origuserinfo, userinfo, "hud_classautokill");

	int infoLenDiff = strlen(origuserinfo) - strlen(userinfo);
	if (infoLenDiff > 8) {
		char localBuf[256];
		for (int i = 0; i < (infoLenDiff - 8); i++) {
			localBuf[i] = 'a' + (i % 26);
			localBuf[i + 1] = 0;
		}
		Info_SetValueForStarKey(userinfo, "_wtf", localBuf, MAX_INFO_STRING);
	}


	uint8 ticket[1024];
	unsigned int ticketLen = *len - msg_readcount;

	if (ticketLen > 0) {
		MSG_ReadBuf(ticketLen, ticket);
	}

	if (isSteam) {
		// clean the ticket
		memset(ticket, 0, sizeof(ticket));
	}

	char agg[2048];
	sprintf(agg, "connect %u %u \"%s\" \"%s\"\n", protocol, challenge, protinfo, userinfo);

	memcpy(data + 4, agg, strlen(agg));

	if (ticketLen > 0) {
		memcpy(data + 4 + strlen(agg), ticket, ticketLen);
	}

	*len = 4 + strlen(agg) + ticketLen;
}

void CAnonymizingEngExtInterceptor::AnonymizeName(const char* real, const char* fake) {
	m_Real2FakeName[std::string(real)] = std::string(fake);
	m_Fake2RealName[std::string(fake)] = std::string(real);
}

std::string CAnonymizingEngExtInterceptor::Real2FakeHost(const char* realHost, const char* callsite) {
	std::string r(realHost);
	auto itr = m_Real2FakeHost.find(r);
	if (itr == m_Real2FakeHost.end()) {
		rehlds_syserror("%s: no fake mapping for real host '%s'", callsite, realHost);
	}

	return itr->second;
}

std::string CAnonymizingEngExtInterceptor::Fake2RealHost(const char* fakeHost, const char* callsite) {
	std::string f(fakeHost);
	auto itr = m_Fake2RealHost.find(f);
	if (itr == m_Fake2RealHost.end()) {
		rehlds_syserror("%s: no fake mapping for fake host '%s'", callsite, fakeHost);
	}

	return itr->second;
}

void CAnonymizingEngExtInterceptor::AnonymizeHost(const char* real, const char* fake) {
	m_Real2FakeHost[std::string(real)] = std::string(fake);
	m_Fake2RealHost[std::string(fake)] = std::string(real);
}
