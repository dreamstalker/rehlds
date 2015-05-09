#pragma once
#ifdef _WIN32

#include "osconfig.h"
#include "testsuite.h"
#include "funccalls.h"

class CRecordingEngExtInterceptor;

class CRecorderFuncCall {
	friend class CRecordingEngExtInterceptor;

private:
	IEngExtCall* m_FuncCall;
	bool m_StartWritten;
	CRecorderFuncCall* m_Next;
	CRecorderFuncCall* m_Prev;

public:
	CRecorderFuncCall(IEngExtCall* fcall);
};

class CSteamAppsRecordingWrapper : public ISteamApps
{
private:
	ISteamApps* m_Wrapped;
	CRecordingEngExtInterceptor* m_Recorder;

public:
	CSteamAppsRecordingWrapper(ISteamApps* original, CRecordingEngExtInterceptor* recorder);

	virtual bool BIsSubscribed();
	virtual bool BIsLowViolence();
	virtual bool BIsCybercafe();
	virtual bool BIsVACBanned();
	virtual const char *GetCurrentGameLanguage();
	virtual const char *GetAvailableGameLanguages();

	virtual bool BIsSubscribedApp(AppId_t appID);
	virtual bool BIsDlcInstalled(AppId_t appID);

	virtual uint32 GetEarliestPurchaseUnixTime(AppId_t nAppID);
	virtual bool BIsSubscribedFromFreeWeekend();

	virtual int GetDLCCount();
	virtual bool BGetDLCDataByIndex(int iDLC, AppId_t *pAppID, bool *pbAvailable, char *pchName, int cchNameBufferSize);

	virtual void InstallDLC(AppId_t nAppID);
	virtual void UninstallDLC(AppId_t nAppID);

	virtual void RequestAppProofOfPurchaseKey(AppId_t nAppID);

	virtual bool GetCurrentBetaName(char *pchName, int cchNameBufferSize);
	virtual bool MarkContentCorrupt(bool bMissingFilesOnly);
	virtual uint32 GetInstalledDepots(DepotId_t *pvecDepots, uint32 cMaxDepots);

	virtual uint32 GetAppInstallDir(AppId_t appID, char *pchFolder, uint32 cchFolderBufferSize);
};

class CSteamGameServerRecordingWrapper : public ISteamGameServer
{
private:
	ISteamGameServer* m_Wrapped;
	CRecordingEngExtInterceptor* m_Recorder;

public:
	CSteamGameServerRecordingWrapper(ISteamGameServer* original, CRecordingEngExtInterceptor* recorder);

	virtual bool InitGameServer(uint32 unIP, uint16 usGamePort, uint16 usQueryPort, uint32 unFlags, AppId_t nGameAppId, const char *pchVersionString);
	virtual void SetProduct(const char *pszProduct);
	virtual void SetGameDescription(const char *pszGameDescription);
	virtual void SetModDir(const char *pszModDir);
	virtual void SetDedicatedServer(bool bDedicated);
	virtual void LogOn(const char *pszAccountName, const char *pszPassword);
	virtual void LogOnAnonymous();
	virtual void LogOff();
	virtual bool BLoggedOn();
	virtual bool BSecure();
	virtual CSteamID GetSteamID();
	virtual bool WasRestartRequested();
	virtual void SetMaxPlayerCount(int cPlayersMax);
	virtual void SetBotPlayerCount(int cBotplayers);
	virtual void SetServerName(const char *pszServerName);
	virtual void SetMapName(const char *pszMapName);
	virtual void SetPasswordProtected(bool bPasswordProtected);
	virtual void SetSpectatorPort(uint16 unSpectatorPort);
	virtual void SetSpectatorServerName(const char *pszSpectatorServerName);
	virtual void ClearAllKeyValues();
	virtual void SetKeyValue(const char *pKey, const char *pValue);
	virtual void SetGameTags(const char *pchGameTags);
	virtual void SetGameData(const char *pchGameData);
	virtual void SetRegion(const char *pszRegion);
	virtual bool SendUserConnectAndAuthenticate(uint32 unIPClient, const void *pvAuthBlob, uint32 cubAuthBlobSize, CSteamID *pSteamIDUser);
	virtual CSteamID CreateUnauthenticatedUserConnection();
	virtual void SendUserDisconnect(CSteamID steamIDUser);
	virtual bool BUpdateUserData(CSteamID steamIDUser, const char *pchPlayerName, uint32 uScore);
	virtual HAuthTicket GetAuthSessionTicket(void *pTicket, int cbMaxTicket, uint32 *pcbTicket);
	virtual EBeginAuthSessionResult BeginAuthSession(const void *pAuthTicket, int cbAuthTicket, CSteamID steamID);
	virtual void EndAuthSession(CSteamID steamID);
	virtual void CancelAuthTicket(HAuthTicket hAuthTicket);
	virtual EUserHasLicenseForAppResult UserHasLicenseForApp(CSteamID steamID, AppId_t appID);
	virtual bool RequestUserGroupStatus(CSteamID steamIDUser, CSteamID steamIDGroup);
	virtual void GetGameplayStats();
	virtual SteamAPICall_t GetServerReputation();
	virtual uint32 GetPublicIP();
	virtual bool HandleIncomingPacket(const void *pData, int cbData, uint32 srcIP, uint16 srcPort);
	virtual int GetNextOutgoingPacket(void *pOut, int cbMaxOut, uint32 *pNetAdr, uint16 *pPort);
	virtual void EnableHeartbeats(bool bActive);
	virtual void SetHeartbeatInterval(int iHeartbeatInterval);
	virtual void ForceHeartbeat();
	virtual SteamAPICall_t AssociateWithClan(CSteamID steamIDClan);
	virtual SteamAPICall_t ComputeNewPlayerCompatibility(CSteamID steamIDNewPlayer);
};

class CSteamCallbackRecordingWrapper : public CCallbackBase
{
public:
	CSteamCallbackRecordingWrapper(CRecordingEngExtInterceptor* recorder, CCallbackBase* cb, int id);
	virtual void Run(void *pvParam);
	virtual void Run(void *pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall);
	virtual int GetCallbackSizeBytes();

	int getRehldsCallbackId() { return m_Id; }

protected:
	CRecordingEngExtInterceptor* m_Recorder;
	CCallbackBase* m_Wrapped;
	int m_Id;
	int m_Size;
};

class CRecordingEngExtInterceptor : public IReHLDSPlatform {
	friend class CSteamAppsRecordingWrapper;
	friend class CSteamGameServerRecordingWrapper;

private:
	std::ofstream m_OutStream;

	SOCKET m_ServerSocket;

	int m_SteamCallbacksCounter;
	std::unordered_map<CCallbackBase*, CSteamCallbackRecordingWrapper*> m_SteamCallbacks;
	CSteamAppsRecordingWrapper* m_SteamAppsWrapper;

	CSteamCallbackRecordingWrapper* getOrCreateCallbackWrapper(CCallbackBase *pCallback);

	CSteamGameServerRecordingWrapper* m_GameServerWrapper;
	void* m_SteamBreakpadContext;

	CRecorderFuncCall* m_RootFunc;
	CRecorderFuncCall* m_LastFunc;

	void writeCall(bool start, bool end, IEngExtCall* fcall);
	IReHLDSPlatform* m_BasePlatform;

public:
	CRecordingEngExtInterceptor(const char* fname, IReHLDSPlatform* basePlatform);
	void PushFunc(CRecorderFuncCall* func);
	void PopFunc(CRecorderFuncCall* func);

	virtual uint32 time(uint32* pTime);
	virtual struct tm* localtime(uint32 time);
	virtual void srand(uint32 seed);
	virtual int rand();

	virtual void Sleep(DWORD msec);
	virtual BOOL QueryPerfCounter(LARGE_INTEGER* counter);
	virtual BOOL QueryPerfFreq(LARGE_INTEGER* freq);
	virtual DWORD GetTickCount();
	virtual void GetLocalTime(LPSYSTEMTIME time);
	virtual void GetSystemTime(LPSYSTEMTIME time);
	virtual void GetTimeZoneInfo(LPTIME_ZONE_INFORMATION zinfo);
	virtual BOOL GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime);
	virtual void GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime);

	virtual SOCKET socket(int af, int type, int protocol);
	virtual int ioctlsocket(SOCKET s, long cmd, u_long *argp);
	virtual int setsockopt(SOCKET s, int level, int optname, const char* optval, int optlen);
	virtual int closesocket(SOCKET s);
	virtual int recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, socklen_t *fromlen);
	virtual int sendto(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen);
	virtual int bind(SOCKET s, const struct sockaddr* addr, int namelen);
	virtual int getsockname(SOCKET s, struct sockaddr* name, socklen_t* namelen);
	virtual int WSAGetLastError();
	virtual struct hostent* gethostbyname(const char *name);
	virtual int gethostname(char *name, int namelen);

	virtual void SteamAPI_SetBreakpadAppID(uint32 unAppID);
	virtual void SteamAPI_UseBreakpadCrashHandler(char const *pchVersion, char const *pchDate, char const *pchTime, bool bFullMemoryDumps, void *pvContext, PFNPreMinidumpCallback m_pfnPreMinidumpCallback);
	virtual void SteamAPI_RegisterCallback(CCallbackBase *pCallback, int iCallback);
	virtual bool SteamAPI_Init();
	virtual void SteamAPI_UnregisterCallResult(class CCallbackBase *pCallback, SteamAPICall_t hAPICall);
	virtual ISteamApps* SteamApps();
	virtual bool SteamGameServer_Init(uint32 unIP, uint16 usSteamPort, uint16 usGamePort, uint16 usQueryPort, EServerMode eServerMode, const char *pchVersionString);
	virtual ISteamGameServer* SteamGameServer();
	virtual void SteamGameServer_RunCallbacks();
	virtual void SteamAPI_RunCallbacks();
	virtual void SteamGameServer_Shutdown();
	virtual void SteamAPI_UnregisterCallback(CCallbackBase *pCallback);
};
#endif //_WIN32