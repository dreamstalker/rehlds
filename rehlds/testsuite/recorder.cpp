#include "precompiled.h"

CRecorderFuncCall::CRecorderFuncCall(IEngExtCall* fcall)
{
	m_FuncCall = fcall;
	m_StartWritten = false;
	m_Next = m_Prev = NULL;
}

CSteamCallbackRecordingWrapper::CSteamCallbackRecordingWrapper(CRecordingEngExtInterceptor* recorder, CCallbackBase* cb, int id)
{
	m_Recorder = recorder;
	m_Wrapped = cb;
	m_Id = id;
	m_Size = cb->GetCallbackSizeBytes();
	m_iCallback = cb->GetICallback();
	this->m_nCallbackFlags = cb->GetFlags();
}

void CSteamCallbackRecordingWrapper::Run(void *pvParam)
{
	if (m_Wrapped->GetICallback() != this->GetICallback()) rehlds_syserror("%s: iCallback desync", __func__);
	if (m_Wrapped->GetFlags() != this->GetFlags()) rehlds_syserror("%s: flags desync", __func__);

	CSteamCallbackCall1 fcall(m_Id, pvParam, m_Size, m_Wrapped); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->Run(pvParam);
	fcall.setResult(m_Wrapped);
	this->SetFlags(m_Wrapped->GetFlags());
	this->SetICallback(m_Wrapped->GetICallback());
	m_Recorder->PopFunc(&frec);
}

void CSteamCallbackRecordingWrapper::Run(void *pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall)
{
	if (m_Wrapped->GetICallback() != this->GetICallback()) rehlds_syserror("%s: iCallback desync", __func__);
	if (m_Wrapped->GetFlags() != this->GetFlags()) rehlds_syserror("%s: flags desync", __func__);

	CSteamCallbackCall2 fcall(m_Id, pvParam, m_Size, bIOFailure, hSteamAPICall, m_Wrapped); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->Run(pvParam, bIOFailure, hSteamAPICall);
	fcall.setResult(m_Wrapped);
	this->SetFlags(m_Wrapped->GetFlags());
	this->SetICallback(m_Wrapped->GetICallback());
	m_Recorder->PopFunc(&frec);
}

int CSteamCallbackRecordingWrapper::GetCallbackSizeBytes()
{
	return m_Wrapped->GetCallbackSizeBytes();
}




CSteamAppsRecordingWrapper::CSteamAppsRecordingWrapper(ISteamApps* original, CRecordingEngExtInterceptor* recorder)
{
	m_Wrapped = original;
	m_Recorder = recorder;
}

bool CSteamAppsRecordingWrapper::BIsSubscribed()
{
	rehlds_syserror("%s: not implemented", __func__);
	return false;
}

bool CSteamAppsRecordingWrapper::BIsLowViolence()
{
	rehlds_syserror("%s: not implemented", __func__);
	return false;
}

bool CSteamAppsRecordingWrapper::BIsCybercafe()
{
	rehlds_syserror("%s: not implemented", __func__);
	return false;
}

bool CSteamAppsRecordingWrapper::BIsVACBanned()
{
	rehlds_syserror("%s: not implemented", __func__);
	return false;
}

const char* CSteamAppsRecordingWrapper::GetCurrentGameLanguage()
{
	CSteamAppGetCurrentGameLanguageCall fcall; CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec); 
	const char* res = m_Wrapped->GetCurrentGameLanguage();
	fcall.setResult(res);
	m_Recorder->PopFunc(&frec);
	return res;
}

const char* CSteamAppsRecordingWrapper::GetAvailableGameLanguages()
{
	rehlds_syserror("%s: not implemented", __func__);
	return NULL;
}

bool CSteamAppsRecordingWrapper::BIsSubscribedApp(AppId_t appID)
{
	rehlds_syserror("%s: not implemented", __func__);
	return false;
}

bool CSteamAppsRecordingWrapper::BIsDlcInstalled(AppId_t appID)
{
	rehlds_syserror("%s: not implemented", __func__);
	return false;
}

uint32 CSteamAppsRecordingWrapper::GetEarliestPurchaseUnixTime(AppId_t nAppID)
{
	rehlds_syserror("%s: not implemented", __func__);
	return 0;
}

bool CSteamAppsRecordingWrapper::BIsSubscribedFromFreeWeekend()
{
	rehlds_syserror("%s: not implemented", __func__);
	return false;
}

int CSteamAppsRecordingWrapper::GetDLCCount()
{
	rehlds_syserror("%s: not implemented", __func__);
	return 0;
}

bool CSteamAppsRecordingWrapper::BGetDLCDataByIndex(int iDLC, AppId_t *pAppID, bool *pbAvailable, char *pchName, int cchNameBufferSize)
{
	rehlds_syserror("%s: not implemented", __func__);
	return false;
}

void CSteamAppsRecordingWrapper::InstallDLC(AppId_t nAppID)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamAppsRecordingWrapper::UninstallDLC(AppId_t nAppID)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamAppsRecordingWrapper::RequestAppProofOfPurchaseKey(AppId_t nAppID)
{
	rehlds_syserror("%s: not implemented", __func__);
}

bool CSteamAppsRecordingWrapper::GetCurrentBetaName(char *pchName, int cchNameBufferSize)
{
	rehlds_syserror("%s: not implemented", __func__);
	return false;
}

bool CSteamAppsRecordingWrapper::MarkContentCorrupt(bool bMissingFilesOnly)
{
	rehlds_syserror("%s: not implemented", __func__);
	return false;
}

uint32 CSteamAppsRecordingWrapper::GetInstalledDepots(DepotId_t *pvecDepots, uint32 cMaxDepots)
{
	rehlds_syserror("%s: not implemented", __func__);
	return 0;
}

uint32 CSteamAppsRecordingWrapper::GetAppInstallDir(AppId_t appID, char *pchFolder, uint32 cchFolderBufferSize)
{
	rehlds_syserror("%s: not implemented", __func__);
	return 0;
}


CSteamGameServerRecordingWrapper::CSteamGameServerRecordingWrapper(ISteamGameServer* original, CRecordingEngExtInterceptor* recorder)
{
	m_Wrapped = original;
	m_Recorder = recorder;
}

bool CSteamGameServerRecordingWrapper::InitGameServer(uint32 unIP, uint16 usGamePort, uint16 usQueryPort, uint32 unFlags, AppId_t nGameAppId, const char *pchVersionString)
{
	rehlds_syserror("%s: not implemented", __func__);
	return false;
}

void CSteamGameServerRecordingWrapper::SetProduct(const char *pszProduct)
{
	CGameServerSetProductCall fcall(pszProduct); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->SetProduct(pszProduct);
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::SetGameDescription(const char *pszGameDescription)
{
	CGameServerSetGameDescCall fcall(pszGameDescription); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->SetGameDescription(pszGameDescription);
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::SetModDir(const char *pszModDir)
{
	CGameServerSetModDirCall fcall(pszModDir); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->SetModDir(pszModDir);
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::SetDedicatedServer(bool bDedicated)
{
	CGameServerSetDedicatedServerCall fcall(bDedicated); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->SetDedicatedServer(bDedicated);
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::LogOn(const char *pszAccountName, const char *pszPassword)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerRecordingWrapper::LogOnAnonymous()
{
	CGameServerLogOnAnonymousCall fcall; CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->LogOnAnonymous();
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::LogOff()
{
	CGameServerLogOffCall fcall; CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->LogOff();
	m_Recorder->PopFunc(&frec);
}

bool CSteamGameServerRecordingWrapper::BLoggedOn()
{
	CGameServerBLoggedOnCall fcall; CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	bool res = m_Wrapped->BLoggedOn();
	fcall.setResult(res);
	m_Recorder->PopFunc(&frec);
	return res;
}

bool CSteamGameServerRecordingWrapper::BSecure()
{
	CGameServerBSecureCall fcall; CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	bool res = m_Wrapped->BSecure();
	fcall.setResult(res);
	m_Recorder->PopFunc(&frec);
	return res;
}

CSteamID CSteamGameServerRecordingWrapper::GetSteamID()
{
	CGameServerGetSteamIdCall fcall; CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	CSteamID res = m_Wrapped->GetSteamID();
	fcall.setResult(res);
	m_Recorder->PopFunc(&frec);
	return res;
}

bool CSteamGameServerRecordingWrapper::WasRestartRequested()
{
	CGameServerWasRestartRequestedCall fcall; CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	bool res = m_Wrapped->WasRestartRequested();
	fcall.setResult(res);
	m_Recorder->PopFunc(&frec);
	return res;
}

void CSteamGameServerRecordingWrapper::SetMaxPlayerCount(int cPlayersMax)
{
	CGameServerSetMaxPlayersCall fcall(cPlayersMax); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->SetMaxPlayerCount(cPlayersMax);
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::SetBotPlayerCount(int cBotplayers)
{
	CGameServerSetBotCountCall fcall(cBotplayers); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->SetBotPlayerCount(cBotplayers);
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::SetServerName(const char *pszServerName)
{
	CGameServerSetServerNameCall fcall(pszServerName); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->SetServerName(pszServerName);
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::SetMapName(const char *pszMapName)
{
	CGameServerSetMapNameCall fcall(pszMapName); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->SetMapName(pszMapName);
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::SetPasswordProtected(bool bPasswordProtected)
{
	CGameServerSetPasswordProtectedCall fcall(bPasswordProtected); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->SetPasswordProtected(bPasswordProtected);
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::SetSpectatorPort(uint16 unSpectatorPort)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerRecordingWrapper::SetSpectatorServerName(const char *pszSpectatorServerName)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerRecordingWrapper::ClearAllKeyValues()
{
	CGameServerClearAllKVsCall fcall; CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->ClearAllKeyValues();
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::SetKeyValue(const char *pKey, const char *pValue)
{
	CGameServerSetKeyValueCall fcall(pKey, pValue); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->SetKeyValue(pKey, pValue);
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::SetGameTags(const char *pchGameTags)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerRecordingWrapper::SetGameData(const char *pchGameData)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerRecordingWrapper::SetRegion(const char *pszRegion)
{
	rehlds_syserror("%s: not implemented", __func__);
}

bool CSteamGameServerRecordingWrapper::SendUserConnectAndAuthenticate(uint32 unIPClient, const void *pvAuthBlob, uint32 cubAuthBlobSize, CSteamID *pSteamIDUser)
{
	CGameServerSendUserConnectAndAuthenticateCall fcall(unIPClient, pvAuthBlob, cubAuthBlobSize); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	bool res = m_Wrapped->SendUserConnectAndAuthenticate(unIPClient, pvAuthBlob, cubAuthBlobSize, pSteamIDUser);
	fcall.setResult(*pSteamIDUser, res);
	m_Recorder->PopFunc(&frec);
	return res;
}

CSteamID CSteamGameServerRecordingWrapper::CreateUnauthenticatedUserConnection()
{
	CGameServerCreateUnauthUserConnectionCall fcall; CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	CSteamID res = m_Wrapped->CreateUnauthenticatedUserConnection();
	fcall.setResult(res);
	m_Recorder->PopFunc(&frec);
	return res;
}

void CSteamGameServerRecordingWrapper::SendUserDisconnect(CSteamID steamIDUser)
{
	CGameServerSendUserDisconnectCall fcall(steamIDUser); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->SendUserDisconnect(steamIDUser);
	m_Recorder->PopFunc(&frec);
}

bool CSteamGameServerRecordingWrapper::BUpdateUserData(CSteamID steamIDUser, const char *pchPlayerName, uint32 uScore)
{
	CGameServerBUpdateUserDataCall fcall(steamIDUser, pchPlayerName, uScore); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	bool res = m_Wrapped->BUpdateUserData(steamIDUser, pchPlayerName, uScore);
	fcall.setResult(res);
	m_Recorder->PopFunc(&frec);
	return res;
}

HAuthTicket CSteamGameServerRecordingWrapper::GetAuthSessionTicket(void *pTicket, int cbMaxTicket, uint32 *pcbTicket)
{
	rehlds_syserror("%s: not implemented", __func__);
	return k_HAuthTicketInvalid;
}

EBeginAuthSessionResult CSteamGameServerRecordingWrapper::BeginAuthSession(const void *pAuthTicket, int cbAuthTicket, CSteamID steamID)
{
	rehlds_syserror("%s: not implemented", __func__);
	return k_EBeginAuthSessionResultInvalidTicket;
}

void CSteamGameServerRecordingWrapper::EndAuthSession(CSteamID steamID)
{
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerRecordingWrapper::CancelAuthTicket(HAuthTicket hAuthTicket)
{
	rehlds_syserror("%s: not implemented", __func__);
}

EUserHasLicenseForAppResult CSteamGameServerRecordingWrapper::UserHasLicenseForApp(CSteamID steamID, AppId_t appID)
{
	rehlds_syserror("%s: not implemented", __func__);
	return k_EUserHasLicenseResultDoesNotHaveLicense;
}

bool CSteamGameServerRecordingWrapper::RequestUserGroupStatus(CSteamID steamIDUser, CSteamID steamIDGroup)
{
	rehlds_syserror("%s: not implemented", __func__);
	return false;
}

void CSteamGameServerRecordingWrapper::GetGameplayStats()
{
	rehlds_syserror("%s: not implemented", __func__);
}

SteamAPICall_t CSteamGameServerRecordingWrapper::GetServerReputation()
{
	rehlds_syserror("%s: not implemented", __func__);
	return k_uAPICallInvalid;
}

uint32 CSteamGameServerRecordingWrapper::GetPublicIP()
{
	rehlds_syserror("%s: not implemented", __func__);
	return 0;
}

bool CSteamGameServerRecordingWrapper::HandleIncomingPacket(const void *pData, int cbData, uint32 srcIP, uint16 srcPort)
{
	CGameServerHandleIncomingPacketCall fcall(pData, cbData, srcIP, srcPort); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	bool res = m_Wrapped->HandleIncomingPacket(pData, cbData, srcIP, srcPort);
	fcall.setResult(res);
	m_Recorder->PopFunc(&frec);
	return res;
}

int CSteamGameServerRecordingWrapper::GetNextOutgoingPacket(void *pOut, int cbMaxOut, uint32 *pNetAdr, uint16 *pPort)
{
	CGameServerGetNextOutgoingPacketCall fcall(cbMaxOut); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	int res = m_Wrapped->GetNextOutgoingPacket(pOut, cbMaxOut, pNetAdr, pPort);
	fcall.setResult(pOut, res, pNetAdr, pPort);
	m_Recorder->PopFunc(&frec);
	return res;
}

void CSteamGameServerRecordingWrapper::EnableHeartbeats(bool bActive)
{
	CGameServerEnableHeartbeatsCall fcall(bActive); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->EnableHeartbeats(bActive);
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::SetHeartbeatInterval(int iHeartbeatInterval)
{
	CGameServerSetHeartbeatIntervalCall fcall(iHeartbeatInterval); CRecorderFuncCall frec(&fcall);
	m_Recorder->PushFunc(&frec);
	m_Wrapped->SetHeartbeatInterval(iHeartbeatInterval);
	m_Recorder->PopFunc(&frec);
}

void CSteamGameServerRecordingWrapper::ForceHeartbeat()
{
	rehlds_syserror("%s: not implemented", __func__);
}

SteamAPICall_t CSteamGameServerRecordingWrapper::AssociateWithClan(CSteamID steamIDClan)
{
	rehlds_syserror("%s: not implemented", __func__);
	return k_uAPICallInvalid;
}

SteamAPICall_t CSteamGameServerRecordingWrapper::ComputeNewPlayerCompatibility(CSteamID steamIDNewPlayer)
{
	rehlds_syserror("%s: not implemented", __func__);
	return k_uAPICallInvalid;
}

CRecordingEngExtInterceptor::CRecordingEngExtInterceptor(const char* fname, IReHLDSPlatform* basePlatform)
{
	m_OutStream.exceptions(std::ios::badbit | std::ios::failbit);
	m_OutStream.open(fname, std::ios::out | std::ios::binary);
	
	m_ServerSocket = INVALID_SOCKET;
	m_SteamCallbacksCounter = 0;
	m_SteamAppsWrapper = NULL;
	m_GameServerWrapper = NULL;
	m_SteamBreakpadContext = NULL;
	m_RootFunc = m_LastFunc = NULL;
	m_BasePlatform = basePlatform;

	uint16 versionMajor = TESTSUITE_PROTOCOL_VERSION_MAJOR;
	uint16 versionMinor = TESTSUITE_PROTOCOL_VERSION_MINOR;
	m_OutStream.write((char*)&versionMinor, 2).write((char*)&versionMajor, 2);

	const char* cmdLine = GetCommandLineA();
	int cmdLineLength = strlen(cmdLine) + 1;

	m_OutStream.write((char*)&cmdLineLength, 4);
	m_OutStream.write(cmdLine, cmdLineLength);
}

void CRecordingEngExtInterceptor::writeCall(bool start, bool end, IEngExtCall* fcall)
{
	uint16 opc = fcall->getOpcode();
	if (start)
		opc |= (1 << 15);

	if (end)
		opc |= (1 << 14);

	m_OutStream.write((char*)&opc, 2);
	if (start)
		fcall->writePrologue(m_OutStream);

	if (end)
		fcall->writeEpilogue(m_OutStream);

	m_OutStream.flush();
}

void CRecordingEngExtInterceptor::PushFunc(CRecorderFuncCall* func)
{
	CRecorderFuncCall* cc = m_LastFunc;
	while (cc != NULL) {
		if (!cc->m_StartWritten) {
			writeCall(true, false, cc->m_FuncCall);
			cc->m_StartWritten = true;
		}
		cc = cc->m_Prev;
	}

	if (m_LastFunc != NULL) {
		m_LastFunc->m_Next = func;
		func->m_Next = NULL;
		func->m_Prev = m_LastFunc;
		m_LastFunc = func;
	}
	else
	{
		m_LastFunc = m_RootFunc = func;
		func->m_Next = func->m_Prev = NULL;
	}

}

void CRecordingEngExtInterceptor::PopFunc(CRecorderFuncCall* func)
{
	if (func != m_LastFunc)
		rehlds_syserror("%s: stack corrupted", __func__);

	writeCall(!func->m_StartWritten, true, func->m_FuncCall);
	if (m_LastFunc->m_Prev == NULL) {
		m_LastFunc = m_RootFunc = NULL;
	}
	else
	{
		m_LastFunc = m_LastFunc->m_Prev;
		m_LastFunc->m_Next = NULL;
	}
}

uint32 CRecordingEngExtInterceptor::time(uint32* pTime)
{
	CStdTimeCall fcall(pTime); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	uint32 res = m_BasePlatform->time(pTime);
	fcall.setResult(res);
	PopFunc(&frec);

	return res;
}

struct tm* CRecordingEngExtInterceptor::localtime(uint32 time)
{
	CStdLocalTimeCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	struct tm* res = m_BasePlatform->localtime(time);
	fcall.setResult(res);
	PopFunc(&frec);

	return res;
}

void CRecordingEngExtInterceptor::srand(uint32 seed)
{
	CStdSrandCall fcall(seed); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	m_BasePlatform->srand(seed);
	PopFunc(&frec);
}

int CRecordingEngExtInterceptor::rand()
{
	CStdRandCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	int res = m_BasePlatform->rand();
	fcall.setResult(res);
	PopFunc(&frec);

	return res;
}

void CRecordingEngExtInterceptor::Sleep(DWORD msec)
{
	CSleepExtCall fcall(msec); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	m_BasePlatform->Sleep(msec);
	PopFunc(&frec);
}

BOOL CRecordingEngExtInterceptor::QueryPerfCounter(LARGE_INTEGER* counter)
{
	CQueryPerfCounterCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	BOOL res = m_BasePlatform->QueryPerfCounter(counter);
	fcall.SetResult(*counter, res);
	PopFunc(&frec);
	return res;
}

BOOL CRecordingEngExtInterceptor::QueryPerfFreq(LARGE_INTEGER* counter)
{
	CQueryPerfFreqCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	BOOL res = m_BasePlatform->QueryPerfFreq(counter);
	fcall.SetResult(*counter, res);
	PopFunc(&frec);
	return res;
}

DWORD CRecordingEngExtInterceptor::GetTickCount()
{
	CGetTickCountCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	DWORD res = m_BasePlatform->GetTickCount();
	fcall.SetResult(res);
	PopFunc(&frec);
	return res;
}

void CRecordingEngExtInterceptor::GetLocalTime(LPSYSTEMTIME time)
{
	CGetLocalTimeCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	m_BasePlatform->GetLocalTime(time);
	fcall.SetResult(time);
	PopFunc(&frec);
}

void CRecordingEngExtInterceptor::GetSystemTime(LPSYSTEMTIME time)
{
	CGetSystemTimeCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	m_BasePlatform->GetSystemTime(time);
	fcall.SetResult(time);
	PopFunc(&frec);
}

void CRecordingEngExtInterceptor::GetTimeZoneInfo(LPTIME_ZONE_INFORMATION zinfo)
{
	CGetTimeZoneInfoCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	m_BasePlatform->GetTimeZoneInfo(zinfo);
	fcall.SetResult(zinfo);
	PopFunc(&frec);
}

BOOL CRecordingEngExtInterceptor::GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime)
{
	CGetProcessTimesCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	BOOL res = m_BasePlatform->GetProcessTimes(hProcess, lpCreationTime, lpExitTime, lpKernelTime, lpUserTime);
	fcall.setResult(res, lpCreationTime, lpExitTime, lpKernelTime, lpUserTime);
	PopFunc(&frec);
	return res;
}

void CRecordingEngExtInterceptor::GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime)
{
	CGetSystemTimeAsFileTimeCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	m_BasePlatform->GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
	fcall.setResult(lpSystemTimeAsFileTime);
	PopFunc(&frec);
}

SOCKET CRecordingEngExtInterceptor::socket(int af, int type, int protocol)
{
	CSocketCall fcall(af, type, protocol); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	SOCKET s = m_BasePlatform->socket(af, type, protocol);
	fcall.setResult(s);
	PopFunc(&frec);
	return s;
}

int CRecordingEngExtInterceptor::ioctlsocket(SOCKET s, long cmd, u_long *argp)
{
	CIoCtlSocketCall fcall(s, cmd, *argp); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	int res = m_BasePlatform->ioctlsocket(s, cmd, argp);
	fcall.setResult(*argp, res);
	PopFunc(&frec);
	return res;
}

int CRecordingEngExtInterceptor::setsockopt(SOCKET s, int level, int optname, const char* optval, int optlen)
{
	CSetSockOptCall fcall(s, level, optname, optval, optlen); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	int res = m_BasePlatform->setsockopt(s, level, optname, optval, optlen);
	fcall.setResult(res);
	PopFunc(&frec);
	return res;
}

int CRecordingEngExtInterceptor::closesocket(SOCKET s)
{
	CCloseSocketCall fcall(s); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	int res = m_BasePlatform->closesocket(s);
	fcall.setResult(s);
	PopFunc(&frec);
	return res;
}

int CRecordingEngExtInterceptor::recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, socklen_t *fromlen)
{
	CRecvFromCall fcall(s, len, flags, *fromlen); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	int res = m_BasePlatform->recvfrom(s, buf, len, flags, from, fromlen);
	fcall.setResult(buf, from, *fromlen, res);
	PopFunc(&frec);
	return res;
}

int CRecordingEngExtInterceptor::sendto(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen)
{
	CSendToCall fcall(s, buf, len, flags, to, tolen); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	int res = m_BasePlatform->sendto(s, buf, len, flags, to, tolen);
	fcall.setResult(res);
	PopFunc(&frec);
	return res;
}

int CRecordingEngExtInterceptor::bind(SOCKET s, const struct sockaddr* addr, int namelen)
{
	CBindCall fcall(s, addr, namelen); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	int res = m_BasePlatform->bind(s, addr, namelen);
	fcall.setResult(res);
	PopFunc(&frec);
	return res;
}

int CRecordingEngExtInterceptor::getsockname(SOCKET s, struct sockaddr* name, socklen_t* namelen)
{
	CGetSockNameCall fcall(s, *namelen); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	int res = m_BasePlatform->getsockname(s, name, namelen);
	fcall.setResult(name, *namelen, res);
	PopFunc(&frec);
	return res;
}

int CRecordingEngExtInterceptor::WSAGetLastError()
{
	CWSAGetLastErrorCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	int res = m_BasePlatform->WSAGetLastError();
	fcall.setResult(res);
	PopFunc(&frec);
	return res;
}

struct hostent* CRecordingEngExtInterceptor::gethostbyname(const char *name)
{
	CGetHostByNameCall fcall(name); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	struct hostent* res = m_BasePlatform->gethostbyname(name);
	fcall.setResult(res);
	PopFunc(&frec);
	return res;
}

int CRecordingEngExtInterceptor::gethostname(char *name, int namelen)
{
	CGetHostNameCall fcall(namelen); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	int res = m_BasePlatform->gethostname(name, namelen);
	fcall.setResult(name, res);
	PopFunc(&frec);
	return res;
}


CSteamCallbackRecordingWrapper* CRecordingEngExtInterceptor::getOrCreateCallbackWrapper(CCallbackBase *pCallback)
{
	auto itr = m_SteamCallbacks.find(pCallback);
	if (itr == m_SteamCallbacks.end())
	{
		CSteamCallbackRecordingWrapper* wrappee = new CSteamCallbackRecordingWrapper(this, pCallback, m_SteamCallbacksCounter++);
		m_SteamCallbacks[pCallback] = wrappee;
		return wrappee;
	}
	else
	{
		return (*itr).second;
	}
}

void CRecordingEngExtInterceptor::SteamAPI_RegisterCallback(CCallbackBase *pCallback, int iCallback)
{
	CSteamCallbackRecordingWrapper* wrappee = getOrCreateCallbackWrapper(pCallback);

	if (wrappee->GetFlags() != pCallback->GetFlags()) rehlds_syserror("%s: flags desync", __func__);
	//if (wrappee->GetICallback() != pCallback->GetICallback()) rehlds_syserror("%s: flags desync", __func__);

	CSteamApiRegisterCallbackCall fcall(wrappee->getRehldsCallbackId(), iCallback, wrappee); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	m_BasePlatform->SteamAPI_RegisterCallback(wrappee, iCallback);
	
	fcall.setResult(wrappee);
	pCallback->SetFlags(wrappee->GetFlags());
	pCallback->SetICallback(wrappee->GetICallback());

	PopFunc(&frec);
}

bool CRecordingEngExtInterceptor::SteamAPI_Init()
{
	CSteamApiInitCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	bool res = m_BasePlatform->SteamAPI_Init();
	fcall.setResult(res);
	PopFunc(&frec);
	return res;
}

void CRecordingEngExtInterceptor::SteamAPI_UnregisterCallResult(class CCallbackBase *pCallback, SteamAPICall_t hAPICall)
{
	CSteamCallbackRecordingWrapper* wrappee = getOrCreateCallbackWrapper(pCallback);

	if (wrappee->GetFlags() != pCallback->GetFlags()) rehlds_syserror("%s: flags desync", __func__);
	if (wrappee->GetICallback() != pCallback->GetICallback()) rehlds_syserror("%s: flags desync", __func__);

	CSteamApiUnrigestierCallResultCall fcall(wrappee->getRehldsCallbackId(), hAPICall, wrappee); CRecorderFuncCall frec(&fcall);
	
	PushFunc(&frec);
	
	m_BasePlatform->SteamAPI_UnregisterCallResult(wrappee, hAPICall);
	
	fcall.setResult(wrappee);
	pCallback->SetFlags(wrappee->GetFlags());
	pCallback->SetICallback(wrappee->GetICallback());

	PopFunc(&frec);
}

ISteamApps* CRecordingEngExtInterceptor::SteamApps()
{
	CSteamAppsCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	if (m_SteamAppsWrapper == NULL)
	{
		ISteamApps* orig = m_BasePlatform->SteamApps();
		fcall.setReturnNull(orig == NULL);
		if (orig != NULL)
			m_SteamAppsWrapper = new CSteamAppsRecordingWrapper(orig, this);
	}
	else
	{
		m_BasePlatform->SteamApps();
		fcall.setReturnNull(false);
	}

	PopFunc(&frec);
	return m_SteamAppsWrapper;
}

bool CRecordingEngExtInterceptor::SteamGameServer_Init(uint32 unIP, uint16 usSteamPort, uint16 usGamePort, uint16 usQueryPort, EServerMode eServerMode, const char *pchVersionString)
{
	CSteamGameServerInitCall fcall(unIP, usSteamPort, usGamePort, usQueryPort, eServerMode, pchVersionString); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	bool res = m_BasePlatform->SteamGameServer_Init(unIP, usSteamPort, usGamePort, usQueryPort, eServerMode, pchVersionString);
	fcall.setResult(res);
	PopFunc(&frec);
	return res;
}

ISteamGameServer* CRecordingEngExtInterceptor::SteamGameServer()
{
	CSteamGameServerCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);

	if (m_GameServerWrapper == NULL)
	{
		ISteamGameServer* orig = m_BasePlatform->SteamGameServer();
		fcall.setReturnNull(orig == NULL);
		if (orig != NULL)
			m_GameServerWrapper = new CSteamGameServerRecordingWrapper(orig, this);
	}
	else
	{
		fcall.setReturnNull(false);
		m_BasePlatform->SteamGameServer();
	}

	PopFunc(&frec);
	return m_GameServerWrapper;
}

void CRecordingEngExtInterceptor::SteamAPI_SetBreakpadAppID(uint32 unAppID)
{
	CSteamApiSetBreakpadAppIdCall fcall(unAppID); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	m_BasePlatform->SteamAPI_SetBreakpadAppID(unAppID);
	PopFunc(&frec);
}

void CRecordingEngExtInterceptor::SteamAPI_UseBreakpadCrashHandler(char const *pchVersion, char const *pchDate, char const *pchTime, bool bFullMemoryDumps, void *pvContext, PFNPreMinidumpCallback m_pfnPreMinidumpCallback)
{
	m_BasePlatform->SteamAPI_UseBreakpadCrashHandler(pchVersion, pchDate, pchTime, bFullMemoryDumps, pvContext, m_pfnPreMinidumpCallback);
}

void CRecordingEngExtInterceptor::SteamGameServer_RunCallbacks()
{
	CSteamGameServerRunCallbacksCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	m_BasePlatform->SteamGameServer_RunCallbacks();
	PopFunc(&frec);
}

void CRecordingEngExtInterceptor::SteamAPI_RunCallbacks()
{
	CSteamApiRunCallbacksCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	m_BasePlatform->SteamAPI_RunCallbacks();
	PopFunc(&frec);
}

void CRecordingEngExtInterceptor::SteamGameServer_Shutdown()
{
	CSteamGameServerShutdownCall fcall; CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	m_BasePlatform->SteamGameServer_Shutdown();
	m_GameServerWrapper = NULL;
	PopFunc(&frec);
}

void CRecordingEngExtInterceptor::SteamAPI_UnregisterCallback(CCallbackBase *pCallback)
{
	CSteamCallbackRecordingWrapper* wrappee = getOrCreateCallbackWrapper(pCallback);

	if (wrappee->GetFlags() != pCallback->GetFlags()) rehlds_syserror("%s: flags desync", __func__);
	if (wrappee->GetICallback() != pCallback->GetICallback()) rehlds_syserror("%s: flags desync", __func__);

	CSteamApiUnregisterCallbackCall fcall(wrappee->getRehldsCallbackId(), wrappee); CRecorderFuncCall frec(&fcall);
	PushFunc(&frec);
	m_BasePlatform->SteamAPI_UnregisterCallback(wrappee);

	fcall.setResult(wrappee);
	pCallback->SetFlags(wrappee->GetFlags());
	pCallback->SetICallback(wrappee->GetICallback());

	PopFunc(&frec);
}
