#include "precompiled.h"

CPlayingEngExtInterceptor::CPlayingEngExtInterceptor(const char* fname, bool strictChecks)
{
	for (int i = 0; i < TESTPLAYER_FUNCTREE_DEPTH; i++)
	{
		m_FuncCalls[i] = &m_FuncCallBuffer[i * TESTPLAYER_FUNCCALL_MAXSIZE];
		m_FuncCallsFree[i] = true;
	}

	m_InStream.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
	m_InStream.open(fname, std::ios::in | std::ios::binary);

	m_InStream.seekg(0, std::ios_base::end);
	m_inStreamSize = m_InStream.tellg();
	m_InStream.seekg(0, std::ios_base::beg);
	m_bLastRead = false;

	m_bStrictChecks = strictChecks;

	m_ServerSocket = INVALID_SOCKET;
	m_SteamCallbacksCounter = 0;
	m_SteamAppsWrapper = NULL;
	m_GameServerWrapper = NULL;
	m_SteamBreakpadContext = NULL;

	m_HeartBeatInterval = 10000000;
	m_PrevHeartBeat = 0;

	uint32 cmdlineLen = 0;
	char cmdLine[2048];

	uint16 versionMajor = 0;
	uint16 versionMinor = 0;

	m_InStream.read((char*)&versionMinor, 2).read((char*)&versionMajor, 2);

	if (versionMajor != TESTSUITE_PROTOCOL_VERSION_MAJOR) {
		rehlds_syserror("%s: protocol major version mismatch; need %d, got %d", __func__, TESTSUITE_PROTOCOL_VERSION_MAJOR, versionMajor);
	}

	if (versionMinor > TESTSUITE_PROTOCOL_VERSION_MINOR) {
		rehlds_syserror("%s: protocol minor version mismatch; need <= %d, got %d", __func__, TESTSUITE_PROTOCOL_VERSION_MINOR, versionMinor);
	}

	m_InStream.read((char*)&cmdlineLen, 4);
	if (cmdlineLen > sizeof(cmdLine)) {
		rehlds_syserror("%s: too long cmdline", __func__);
	}

	m_InStream.read(cmdLine, cmdlineLen);
	printf("Playing testsuite\nrecorders's cmdline: %s\n", cmdLine);

	m_StartTick = ::GetTickCount();
	m_NumFrames = 0;
}

void* CPlayingEngExtInterceptor::allocFuncCall()
{
	for (int i = 0; i < TESTPLAYER_FUNCTREE_DEPTH; i++)
	{
		if (m_FuncCallsFree[i])
		{
			m_FuncCallsFree[i] = false;
			return m_FuncCalls[i];
		}
	}

	rehlds_syserror("%s: running out of free slots", __func__);
}

void CPlayingEngExtInterceptor::freeFuncCall(void* fcall)
{
	for (int i = 0; i < TESTPLAYER_FUNCTREE_DEPTH; i++)
	{
		if (m_FuncCalls[i] == fcall)
		{
			m_FuncCallsFree[i] = true;
			return;
		}
	}

	rehlds_syserror("%s: invalid pointer provided: %p", __func__, fcall);
}

bool CPlayingEngExtInterceptor::readFuncCall() {
	if (m_InStream.tellg() >= m_inStreamSize)
	{
		if (m_bLastRead) return false;

		m_bLastRead = true;
		IEngExtCall* callFunc = new(allocFuncCall()) CEndRecordCall();
		m_CommandsQueue.push(callFunc);
		return true;
	}

	uint16 opc;
	m_InStream.read((char*)&opc, 2);

	bool startFlag = (opc & (1 << 15)) != 0;
	bool endFlag = (opc & (1 << 14)) != 0;
	opc &= 0x3FFF;

	if (startFlag)
	{
		IEngExtCall* callFunc = IEngExtCallFactory::createByOpcode((ExtCallFuncs)opc, allocFuncCall(), TESTPLAYER_FUNCCALL_MAXSIZE);
		callFunc->readPrologue(m_InStream);
		m_CommandsQueue.push(callFunc);
		callFunc->m_Start = true;
	}

	if (endFlag)
	{
		IEngExtCall* callFunc = IEngExtCallFactory::createByOpcode((ExtCallFuncs)opc, allocFuncCall(), TESTPLAYER_FUNCCALL_MAXSIZE);
		callFunc->readEpilogue(m_InStream);
		m_CommandsQueue.push(callFunc);
		callFunc->m_End = true;
	}

	return true;
}

IEngExtCall* CPlayingEngExtInterceptor::getNextCallInternal(bool peek) {
	if (m_CommandsQueue.empty()) {
		readFuncCall();
	}

	if (m_CommandsQueue.empty()) {
		rehlds_syserror("%s: command queue is empty!", __func__);
	}

	IEngExtCall* next = m_CommandsQueue.front();
	if (!peek) {
		m_CommandsQueue.pop();
	}

	return next;
}

IEngExtCall* CPlayingEngExtInterceptor::getNextCall(bool peek, bool processCallbacks, ExtCallFuncs expectedOpcode, bool needStart, const char* callSource) {
	int size = (int)m_InStream.tellg();
	int sizeLeft = m_inStreamSize - size;
	maybeHeartBeat(size);
	IEngExtCall* cmd = getNextCallInternal(peek);
	if (peek) {
		return cmd;
	}

	if (cmd->getOpcode() == ECF_NONE) {
		DWORD endTick = ::GetTickCount();
		FILE* fl = fopen("rehlds_demo_stats.xml", "w");
		if (fl) {
			fprintf(fl, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
			fprintf(fl, "<DemoStats>\n");
			fprintf(fl, "  <Duration>%u</Duration>\n", (endTick - m_StartTick));
			fprintf(fl, "  <NumFrames>%u</NumFrames>\n", m_NumFrames);
			fprintf(fl, "</DemoStats>\n");
			fclose(fl);
		}
		TerminateProcess(GetCurrentProcess(), 777);
	}

	IEngCallbackCall* callback = dynamic_cast<IEngCallbackCall*>(cmd);
	if (callback != NULL && callback->m_Start) {
		if (!processCallbacks) {
			rehlds_syserror("%s: read a callback, but it's not allowed here", __func__);
		}

		while (callback != NULL && callback->m_Start) {
			playCallback(callback);
			cmd = getNextCallInternal(false);
			callback = (IEngCallbackCall*)cmd;
		}
	}

	if (cmd->getOpcode() != expectedOpcode) {
		rehlds_syserror("%s: bad opcode; expected %d got %d; size left: %d", __func__, expectedOpcode, cmd->getOpcode(), sizeLeft);
	}
	if (needStart) {
		if (!cmd->m_Start) rehlds_syserror("%s: bad fcall %d; expected start flag", __func__, cmd->getOpcode());
	}
	else {
		if (!cmd->m_End) rehlds_syserror("%s: bad fcall %d; expected end flag", __func__, cmd->getOpcode());
	}

	return cmd;
}

void CPlayingEngExtInterceptor::playCallback(IEngCallbackCall* cb) {
	switch (cb->getOpcode()) {

	case ECF_STEAM_CALLBACK_CALL_1:
		playSteamCallback1((CSteamCallbackCall1*)cb);
		return;

	case ECF_STEAM_CALLBACK_CALL_2:
		playSteamCallback2((CSteamCallbackCall2*)cb);
		return;

	default:
		rehlds_syserror("%s: unknown callback", __func__);
	}
}

void CPlayingEngExtInterceptor::playSteamCallback1(CSteamCallbackCall1* cb) {
	auto itr = m_SteamCallbacks.find(cb->m_CallbackId);
	if (itr == m_SteamCallbacks.end()) rehlds_syserror("%s: callback %d not found", __func__, cb->m_CallbackId);
	CCallbackBase* steamCallback = (*itr).second;

	if (steamCallback->GetFlags() != cb->m_InState.m_nCallbackFlags) rehlds_syserror("%s: PRE flags desync", __func__);
	if (steamCallback->GetICallback() != cb->m_InState.m_iCallback) rehlds_syserror("%s: PRE flags desync", __func__);

	steamCallback->Run(cb->m_Data);

	CSteamCallbackCall1* endCallback = (CSteamCallbackCall1*)getNextCall(false, true, cb->getOpcode(), false, __func__);

	if (steamCallback->GetFlags() != endCallback->m_OutState.m_nCallbackFlags) rehlds_syserror("%s: POST flags desync", __func__);
	if (steamCallback->GetICallback() != endCallback->m_OutState.m_iCallback) rehlds_syserror("%s: POST flags desync", __func__);
	freeFuncCall(cb);
	freeFuncCall(endCallback);
}

void CPlayingEngExtInterceptor::playSteamCallback2(CSteamCallbackCall2* cb) {
	auto itr = m_SteamCallbacks.find(cb->m_CallbackId);
	if (itr == m_SteamCallbacks.end()) rehlds_syserror("%s: callback %d not found", __func__, cb->m_CallbackId);
	CCallbackBase* steamCallback = (*itr).second;

	if (steamCallback->GetFlags() != cb->m_InState.m_nCallbackFlags) rehlds_syserror("%s: PRE flags desync", __func__);
	if (steamCallback->GetICallback() != cb->m_InState.m_iCallback) rehlds_syserror("%s: PRE flags desync", __func__);

	steamCallback->Run(cb->m_Data, cb->m_bIOFailure, cb->m_SteamAPICall);

	CSteamCallbackCall2* endCallback = (CSteamCallbackCall2*)getNextCall(false, true, cb->getOpcode(), false, __func__);

	if (steamCallback->GetFlags() != endCallback->m_OutState.m_nCallbackFlags) rehlds_syserror("%s: POST flags desync", __func__);
	if (steamCallback->GetICallback() != endCallback->m_OutState.m_iCallback) rehlds_syserror("%s: POST flags desync", __func__);
	freeFuncCall(cb);
	freeFuncCall(endCallback);
}

int CPlayingEngExtInterceptor::getOrRegisterSteamCallback(CCallbackBase* cb) {
	auto itr = m_SteamCallbacksReverse.find(cb);
	if (itr != m_SteamCallbacksReverse.end()) {
		return (*itr).second;
	}

	int id = m_SteamCallbacksCounter++;
	m_SteamCallbacksReverse[cb] = id;
	m_SteamCallbacks[id] = cb;

	return id;
}

void CPlayingEngExtInterceptor::maybeHeartBeat(int readPos) {
	if (m_PrevHeartBeat + m_HeartBeatInterval <= readPos) {
		m_PrevHeartBeat = readPos;
		Con_Printf("%s: readPos=%u\n", __func__, readPos);
	}
}

uint32 CPlayingEngExtInterceptor::time(uint32* pTime)
{
	CStdTimeCall* playCall = dynamic_cast<CStdTimeCall*>(getNextCall(false, false, ECF_CSTD_TIME, true, __func__));
	CStdTimeCall(pTime).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CStdTimeCall* playEndCall = dynamic_cast<CStdTimeCall*>(getNextCall(false, true, ECF_CSTD_TIME, false, __func__));

	uint32 res = playEndCall->m_Res;
	if (pTime != NULL) *pTime = res;

	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

struct tm* CPlayingEngExtInterceptor::localtime(uint32 time)
{
	CStdLocalTimeCall* playCall = dynamic_cast<CStdLocalTimeCall*>(getNextCall(false, false, ECF_CSTD_LOCALTIME, true, __func__));
	CStdLocalTimeCall(time).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CStdLocalTimeCall* playEndCall = dynamic_cast<CStdLocalTimeCall*>(getNextCall(false, true, ECF_CSTD_LOCALTIME, false, __func__));

	setCurrentTm(&playEndCall->m_Res);

	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return &m_CurrentTm;
}

void CPlayingEngExtInterceptor::srand(uint32 seed)
{
	CStdSrandCall* playCall = dynamic_cast<CStdSrandCall*>(getNextCall(false, false, ECF_CSTD_SRAND_CALL, true, __func__));
	CStdSrandCall(seed).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CStdSrandCall* playEndCall = dynamic_cast<CStdSrandCall*>(getNextCall(false, true, ECF_CSTD_SRAND_CALL, false, __func__));

	freeFuncCall(playCall); freeFuncCall(playEndCall);
}

int CPlayingEngExtInterceptor::rand()
{
	CStdRandCall* playCall = dynamic_cast<CStdRandCall*>(getNextCall(false, false, ECF_CSTD_RAND_CALL, true, __func__));
	CStdRandCall* playEndCall = dynamic_cast<CStdRandCall*>(getNextCall(false, true, ECF_CSTD_RAND_CALL, false, __func__));

	int res = playEndCall->m_Res;

	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

void CPlayingEngExtInterceptor::Sleep(DWORD msec) {
	CSleepExtCall* playCall = dynamic_cast<CSleepExtCall*>(getNextCall(false, false, ECF_SLEEP, true, __func__));
	CSleepExtCall(msec).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CSleepExtCall* playEndCall = dynamic_cast<CSleepExtCall*>(getNextCall(false, true, ECF_SLEEP, false, __func__));

	freeFuncCall(playCall); freeFuncCall(playEndCall);
}

BOOL CPlayingEngExtInterceptor::QueryPerfCounter(LARGE_INTEGER* counter) {
	CQueryPerfCounterCall* playCall = dynamic_cast<CQueryPerfCounterCall*>(getNextCall(false, false, ECF_QUERY_PERF_COUNTER, true, __func__));
	CQueryPerfCounterCall* playEndCall = dynamic_cast<CQueryPerfCounterCall*>(getNextCall(false, true, ECF_QUERY_PERF_COUNTER, false, __func__));
	
	counter->QuadPart = playEndCall->m_Counter;
	BOOL res = playEndCall->m_Res;
	freeFuncCall(playCall); freeFuncCall(playEndCall);
	
	return res;
}

BOOL CPlayingEngExtInterceptor::QueryPerfFreq(LARGE_INTEGER* freq) {
	CQueryPerfFreqCall* playCall = dynamic_cast<CQueryPerfFreqCall*>(getNextCall(false, false, ECF_QUERY_PERF_FREQ, true, __func__));
	CQueryPerfFreqCall* playEndCall = dynamic_cast<CQueryPerfFreqCall*>(getNextCall(false, true, ECF_QUERY_PERF_FREQ, false, __func__));

	freq->QuadPart = playEndCall->m_Freq;
	BOOL res = playEndCall->m_Res;
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

DWORD CPlayingEngExtInterceptor::GetTickCount() {
	CGetTickCountCall* playCall = dynamic_cast<CGetTickCountCall*>(getNextCall(false, false, ECF_GET_TICK_COUNT, true, __func__));
	CGetTickCountCall* playEndCall = dynamic_cast<CGetTickCountCall*>(getNextCall(false, true, ECF_GET_TICK_COUNT, false, __func__));

	DWORD res = playEndCall->m_Res;
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

void CPlayingEngExtInterceptor::GetLocalTime(LPSYSTEMTIME time) {
	CGetLocalTimeCall* playCall = dynamic_cast<CGetLocalTimeCall*>(getNextCall(false, false, ECF_GET_LOCAL_TIME, true, __func__));
	CGetLocalTimeCall* playEndCall = dynamic_cast<CGetLocalTimeCall*>(getNextCall(false, true, ECF_GET_LOCAL_TIME, false, __func__));

	memcpy(time, &playEndCall->m_Res, sizeof(SYSTEMTIME));
	freeFuncCall(playCall); freeFuncCall(playEndCall);
}

void CPlayingEngExtInterceptor::GetSystemTime(LPSYSTEMTIME time) {
	CGetSystemTimeCall* playCall = dynamic_cast<CGetSystemTimeCall*>(getNextCall(false, false, ECF_GET_SYSTEM_TIME, true, __func__));
	CGetSystemTimeCall* playEndCall = dynamic_cast<CGetSystemTimeCall*>(getNextCall(false, true, ECF_GET_SYSTEM_TIME, false, __func__));

	memcpy(time, &playEndCall->m_Res, sizeof(SYSTEMTIME));
	freeFuncCall(playCall); freeFuncCall(playEndCall);
}

void CPlayingEngExtInterceptor::GetTimeZoneInfo(LPTIME_ZONE_INFORMATION zinfo) {
	CGetTimeZoneInfoCall* playCall = dynamic_cast<CGetTimeZoneInfoCall*>(getNextCall(false, false, ECF_GET_TIMEZONE_INFO, true, __func__));
	CGetTimeZoneInfoCall* playEndCall = dynamic_cast<CGetTimeZoneInfoCall*>(getNextCall(false, true, ECF_GET_TIMEZONE_INFO, false, __func__));

	memcpy(zinfo, &playEndCall->m_Res, sizeof(TIME_ZONE_INFORMATION));
	freeFuncCall(playCall); freeFuncCall(playEndCall);
}

BOOL CPlayingEngExtInterceptor::GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime)
{
	CGetProcessTimesCall* playCall = dynamic_cast<CGetProcessTimesCall*>(getNextCall(false, false, ECF_GET_PROCESS_TIMES, true, __func__));
	CGetProcessTimesCall* playEndCall = dynamic_cast<CGetProcessTimesCall*>(getNextCall(false, true, ECF_GET_PROCESS_TIMES, false, __func__));

	BOOL res = playEndCall->m_Res;
	memcpy(lpCreationTime, &playEndCall->m_CreationTime, sizeof(FILETIME));
	memcpy(lpExitTime, &playEndCall->m_ExitTime, sizeof(FILETIME));
	memcpy(lpKernelTime, &playEndCall->m_KernelTime, sizeof(FILETIME));
	memcpy(lpUserTime, &playEndCall->m_UserTime, sizeof(FILETIME));
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

void CPlayingEngExtInterceptor::GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime)
{
	CGetSystemTimeAsFileTimeCall* playCall = dynamic_cast<CGetSystemTimeAsFileTimeCall*>(getNextCall(false, false, ECF_GET_SYSTEM_TIME_AS_FILE_TIME, true, __func__));
	CGetSystemTimeAsFileTimeCall* playEndCall = dynamic_cast<CGetSystemTimeAsFileTimeCall*>(getNextCall(false, true, ECF_GET_SYSTEM_TIME_AS_FILE_TIME, false, __func__));

	memcpy(lpSystemTimeAsFileTime, &playEndCall->m_SystemTime, sizeof(FILETIME));
	freeFuncCall(playCall); freeFuncCall(playEndCall);
}


SOCKET CPlayingEngExtInterceptor::socket(int af, int type, int protocol) {
	CSocketCall* playCall = dynamic_cast<CSocketCall*>(getNextCall(false, false, ECF_SOCKET, true, __func__));
	CSocketCall(af, type, protocol).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CSocketCall* playEndCall = dynamic_cast<CSocketCall*>(getNextCall(false, true, ECF_SOCKET, false, __func__));

	SOCKET res = playEndCall->m_Res;
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

int CPlayingEngExtInterceptor::ioctlsocket(SOCKET s, long cmd, u_long *argp) {
	CIoCtlSocketCall* playCall = dynamic_cast<CIoCtlSocketCall*>(getNextCall(false, false, ECF_IOCTL_SOCKET, true, __func__));
	CIoCtlSocketCall(s, cmd, *argp).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CIoCtlSocketCall* playEndCall = dynamic_cast<CIoCtlSocketCall*>(getNextCall(false, true, ECF_IOCTL_SOCKET, false, __func__));

	int res = playEndCall->m_Res;
	*argp = playEndCall->m_OutValue;
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

int CPlayingEngExtInterceptor::setsockopt(SOCKET s, int level, int optname, const char* optval, int optlen) {
	CSetSockOptCall* playCall = dynamic_cast<CSetSockOptCall*>(getNextCall(false, false, ECF_SET_SOCK_OPT, true, __func__));
	CSetSockOptCall(s, level, optname, optval, optlen).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CSetSockOptCall* playEndCall = dynamic_cast<CSetSockOptCall*>(getNextCall(false, true, ECF_SET_SOCK_OPT, false, __func__));

	int res = playEndCall->m_Res;
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

int CPlayingEngExtInterceptor::closesocket(SOCKET s) {
	CCloseSocketCall* playCall = dynamic_cast<CCloseSocketCall*>(getNextCall(false, false, ECF_CLOSE_SOCKET, true, __func__));
	CCloseSocketCall(s).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CCloseSocketCall* playEndCall = dynamic_cast<CCloseSocketCall*>(getNextCall(false, true, ECF_CLOSE_SOCKET, false, __func__));

	int res = playEndCall->m_Res;
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

int CPlayingEngExtInterceptor::recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, socklen_t *fromlen) {
	CRecvFromCall* playCall = dynamic_cast<CRecvFromCall*>(getNextCall(false, false, ECF_RECVFROM, true, __func__));
	CRecvFromCall(s, len, flags, *fromlen).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CRecvFromCall* playEndCall = dynamic_cast<CRecvFromCall*>(getNextCall(false, true, ECF_RECVFROM, false, __func__));

	int res = playEndCall->m_Res;
	*fromlen = playEndCall->m_FromLenOut;
	if (res >= 0) {
		memcpy(buf, playEndCall->m_Data, res);
		memcpy(from, playEndCall->m_From, playEndCall->m_FromLenOut);
	}

	freeFuncCall(playCall); freeFuncCall(playEndCall);

	if (res == -1) {
		m_NumFrames++;
	}

	return res;
}

int CPlayingEngExtInterceptor::sendto(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen) {
	CSendToCall* playCall = dynamic_cast<CSendToCall*>(getNextCall(false, false, ECF_SENDTO, true, __func__));
	CSendToCall(s, buf, len, flags, to, tolen).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CSendToCall* playEndCall = dynamic_cast<CSendToCall*>(getNextCall(false, true, ECF_SENDTO, false, __func__));

	int res = playEndCall->m_Res;
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

int CPlayingEngExtInterceptor::bind(SOCKET s, const struct sockaddr* addr, int namelen) {
	CBindCall* playCall = dynamic_cast<CBindCall*>(getNextCall(false, false, ECF_BIND, true, __func__));
	CBindCall(s, addr, namelen).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CBindCall* playEndCall = dynamic_cast<CBindCall*>(getNextCall(false, true, ECF_BIND, false, __func__));

	int res = playEndCall->m_Res;
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

int CPlayingEngExtInterceptor::getsockname(SOCKET s, struct sockaddr* name, socklen_t* namelen) {
	CGetSockNameCall* playCall = dynamic_cast<CGetSockNameCall*>(getNextCall(false, false, ECF_GET_SOCK_NAME, true, __func__));
	CGetSockNameCall(s, *namelen).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGetSockNameCall* playEndCall = dynamic_cast<CGetSockNameCall*>(getNextCall(false, true, ECF_GET_SOCK_NAME, false, __func__));

	int res = playEndCall->m_Res;
	*namelen = playEndCall->m_AddrLenOut;
	if (res >= 0) {
		memcpy(name, playEndCall->m_Addr, playEndCall->m_AddrLenOut);
	}

	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

int CPlayingEngExtInterceptor::WSAGetLastError() {
	CWSAGetLastErrorCall* playCall = dynamic_cast<CWSAGetLastErrorCall*>(getNextCall(false, false, ECF_WSA_GET_LAST_ERROR, true, __func__));
	CWSAGetLastErrorCall* playEndCall = dynamic_cast<CWSAGetLastErrorCall*>(getNextCall(false, true, ECF_WSA_GET_LAST_ERROR, false, __func__));

	int res = playEndCall->m_Res;
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

struct hostent* CPlayingEngExtInterceptor::gethostbyname(const char *name) {
	CGetHostByNameCall* playCall = dynamic_cast<CGetHostByNameCall*>(getNextCall(false, false, ECF_GET_HOST_BY_NAME, true, __func__));
	CGetHostByNameCall(name).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGetHostByNameCall* playEndCall = dynamic_cast<CGetHostByNameCall*>(getNextCall(false, true, ECF_GET_HOST_BY_NAME, false, __func__));

	setCurrentHostent(&playEndCall->m_HostentData);

	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return &m_CurrentHostent;
}

int CPlayingEngExtInterceptor::gethostname(char *name, int namelen) {
	CGetHostNameCall* playCall = dynamic_cast<CGetHostNameCall*>(getNextCall(false, false, ECF_GET_HOST_NAME, true, __func__));
	CGetHostNameCall(namelen).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGetHostNameCall* playEndCall = dynamic_cast<CGetHostNameCall*>(getNextCall(false, true, ECF_GET_HOST_NAME, false, __func__));

	int res = playEndCall->m_Res;
	strcpy(name, playEndCall->m_Name);

	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

void CPlayingEngExtInterceptor::setCurrentHostent(hostent_data_t* data) {
	memcpy(&m_CurrentHostentData, data, sizeof(m_CurrentHostentData));
	for (int i = 0; i < m_CurrentHostentData.numAliases; i++) {
		m_CurrentHostentData.preparedAliases[i] = m_CurrentHostentData.aliases[i];
		m_CurrentHostentData.preparedAliases[i + 1] = NULL;
	}

	for (int i = 0; i < m_CurrentHostentData.numAddrs; i++) {
		m_CurrentHostentData.preparedAddrs[i] = m_CurrentHostentData.addrs[i];
		m_CurrentHostentData.preparedAddrs[i + 1] = NULL;
	}

	m_CurrentHostent.h_addr_list = m_CurrentHostentData.preparedAddrs;
	m_CurrentHostent.h_addrtype = m_CurrentHostentData.addrtype;
	m_CurrentHostent.h_aliases = m_CurrentHostentData.preparedAliases;
	m_CurrentHostent.h_length = m_CurrentHostentData.addrLen;
	m_CurrentHostent.h_name = m_CurrentHostentData.hostName;
}

void CPlayingEngExtInterceptor::setCurrentTm(struct tm* t) {
	memcpy(&m_CurrentTm, t, sizeof(m_CurrentTm));
}

void CPlayingEngExtInterceptor::SteamAPI_SetBreakpadAppID(uint32 unAppID) {
	CSteamApiSetBreakpadAppIdCall* playCall = dynamic_cast<CSteamApiSetBreakpadAppIdCall*>(getNextCall(false, false, ECF_STEAM_API_SET_BREAKPAD_APP_ID, true, __func__));
	CSteamApiSetBreakpadAppIdCall(unAppID).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CSteamApiSetBreakpadAppIdCall* playEndCall = dynamic_cast<CSteamApiSetBreakpadAppIdCall*>(getNextCall(false, true, ECF_STEAM_API_SET_BREAKPAD_APP_ID, false, __func__));

	freeFuncCall(playCall); freeFuncCall(playEndCall);
}

void CPlayingEngExtInterceptor::SteamAPI_UseBreakpadCrashHandler(char const *pchVersion, char const *pchDate, char const *pchTime, bool bFullMemoryDumps, void *pvContext, PFNPreMinidumpCallback m_pfnPreMinidumpCallback) {
	//do nothing for now
}

void CPlayingEngExtInterceptor::SteamAPI_RegisterCallback(CCallbackBase *pCallback, int iCallback) {
	int rehldsId = getOrRegisterSteamCallback(pCallback);

	CSteamApiRegisterCallbackCall* playCall = dynamic_cast<CSteamApiRegisterCallbackCall*>(getNextCall(false, false, ECF_STEAM_API_REGISTER_CALLBACK, true, __func__));
	CSteamApiRegisterCallbackCall(rehldsId, iCallback, pCallback).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CSteamApiRegisterCallbackCall* playEndCall = dynamic_cast<CSteamApiRegisterCallbackCall*>(getNextCall(false, true, ECF_STEAM_API_REGISTER_CALLBACK, false, __func__));

	pCallback->SetFlags(playEndCall->m_OutState.m_nCallbackFlags);
	pCallback->SetICallback(playEndCall->m_OutState.m_iCallback);
	freeFuncCall(playCall); freeFuncCall(playEndCall);
}

bool CPlayingEngExtInterceptor::SteamAPI_Init() {
	CSteamApiInitCall* playCall = dynamic_cast<CSteamApiInitCall*>(getNextCall(false, false, ECF_STEAM_API_INIT, true, __func__));
	CSteamApiInitCall* playEndCall = dynamic_cast<CSteamApiInitCall*>(getNextCall(false, true, ECF_STEAM_API_INIT, false, __func__));

	bool res = playEndCall->m_Res;
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

void CPlayingEngExtInterceptor::SteamAPI_UnregisterCallResult(class CCallbackBase *pCallback, SteamAPICall_t hAPICall) {
	int rehldsId = getOrRegisterSteamCallback(pCallback);

	CSteamApiUnrigestierCallResultCall* playCall = dynamic_cast<CSteamApiUnrigestierCallResultCall*>(getNextCall(false, false, ECF_STEAM_API_UNREGISTER_CALL_RESULT, true, __func__));
	CSteamApiUnrigestierCallResultCall(rehldsId, hAPICall, pCallback).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CSteamApiUnrigestierCallResultCall* playEndCall = dynamic_cast<CSteamApiUnrigestierCallResultCall*>(getNextCall(false, true, ECF_STEAM_API_UNREGISTER_CALL_RESULT, false, __func__));

	pCallback->SetFlags(playEndCall->m_OutState.m_nCallbackFlags);
	pCallback->SetICallback(playEndCall->m_OutState.m_iCallback);
	freeFuncCall(playCall); freeFuncCall(playEndCall);
}

ISteamApps* CPlayingEngExtInterceptor::SteamApps() {
	CSteamAppsCall* playCall = (CSteamAppsCall*)getNextCall(false, false, ECF_STEAMAPPS, true, __func__);
	CSteamAppsCall* playEndCall = (CSteamAppsCall*)getNextCall(false, true, ECF_STEAMAPPS, false, __func__);

	ISteamApps* res = NULL;
	if (!playEndCall->m_ReturnNull) {
		if (m_SteamAppsWrapper == NULL) m_SteamAppsWrapper = new CSteamAppsPlayingWrapper(this);
		res = m_SteamAppsWrapper;
	}
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

bool CPlayingEngExtInterceptor::SteamGameServer_Init(uint32 unIP, uint16 usSteamPort, uint16 usGamePort, uint16 usQueryPort, EServerMode eServerMode, const char *pchVersionString) {
	CSteamGameServerInitCall* playCall = dynamic_cast<CSteamGameServerInitCall*>(getNextCall(false, false, ECF_STEAMGAMESERVER_INIT, true, __func__));
	CSteamGameServerInitCall(unIP, usSteamPort, usGamePort, usQueryPort, eServerMode, pchVersionString).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CSteamGameServerInitCall* playEndCall = dynamic_cast<CSteamGameServerInitCall*>(getNextCall(false, true, ECF_STEAMGAMESERVER_INIT, false, __func__));

	bool res = playEndCall->m_Res;
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

ISteamGameServer* CPlayingEngExtInterceptor::SteamGameServer() {
	CSteamGameServerCall* playCall = dynamic_cast<CSteamGameServerCall*>(getNextCall(false, false, ECF_STEAMGAMESERVER, true, __func__));
	CSteamGameServerCall* playEndCall = dynamic_cast<CSteamGameServerCall*>(getNextCall(false, true, ECF_STEAMGAMESERVER, false, __func__));

	ISteamGameServer* res = NULL;
	if (!playEndCall->m_ReturnNull) {
		if (m_GameServerWrapper == NULL) m_GameServerWrapper = new CSteamGameServerPlayingWrapper(this);
		res = m_GameServerWrapper;
	}
	freeFuncCall(playCall); freeFuncCall(playEndCall);

	return res;
}

void CPlayingEngExtInterceptor::SteamGameServer_RunCallbacks() {
	CSteamGameServerRunCallbacksCall* playCall = dynamic_cast<CSteamGameServerRunCallbacksCall*>(getNextCall(false, false, ECF_STEAMGAMESERVER_RUN_CALLBACKS, true, __func__));
	CSteamGameServerRunCallbacksCall* playEndCall = dynamic_cast<CSteamGameServerRunCallbacksCall*>(getNextCall(false, true, ECF_STEAMGAMESERVER_RUN_CALLBACKS, false, __func__));

	freeFuncCall(playCall); freeFuncCall(playEndCall);
}

void CPlayingEngExtInterceptor::SteamAPI_RunCallbacks() {
	CSteamApiRunCallbacksCall* playCall = dynamic_cast<CSteamApiRunCallbacksCall*>(getNextCall(false, false, ECF_STEAM_API_RUN_CALLBACKS, true, __func__));
	CSteamApiRunCallbacksCall* playEndCall = dynamic_cast<CSteamApiRunCallbacksCall*>(getNextCall(false, true, ECF_STEAM_API_RUN_CALLBACKS, false, __func__));

	freeFuncCall(playCall); freeFuncCall(playEndCall);
}

void CPlayingEngExtInterceptor::SteamGameServer_Shutdown()
{
	CSteamGameServerShutdownCall* playCall = dynamic_cast<CSteamGameServerShutdownCall*>(getNextCall(false, false, ECF_STEAMGAMESERVER_SHUTDOWN, true, __func__));
	CSteamGameServerShutdownCall* playEndCall = dynamic_cast<CSteamGameServerShutdownCall*>(getNextCall(false, true, ECF_STEAMGAMESERVER_SHUTDOWN, false, __func__));

	freeFuncCall(playCall); freeFuncCall(playEndCall);
}


void CPlayingEngExtInterceptor::SteamAPI_UnregisterCallback(CCallbackBase *pCallback) {
	int rehldsId = getOrRegisterSteamCallback(pCallback);

	CSteamApiUnregisterCallbackCall* playCall = dynamic_cast<CSteamApiUnregisterCallbackCall*>(getNextCall(false, false, ECF_STEAM_API_UNREGISTER_CALLBACK, true, __func__));
	CSteamApiUnregisterCallbackCall(rehldsId, pCallback).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CSteamApiUnregisterCallbackCall* playEndCall = dynamic_cast<CSteamApiUnregisterCallbackCall*>(getNextCall(false, true, ECF_STEAM_API_UNREGISTER_CALLBACK, false, __func__));

	pCallback->SetFlags(playEndCall->m_OutState.m_nCallbackFlags);
	pCallback->SetICallback(playEndCall->m_OutState.m_iCallback);
	freeFuncCall(playCall); freeFuncCall(playEndCall);
}





CSteamGameServerPlayingWrapper::CSteamGameServerPlayingWrapper(CPlayingEngExtInterceptor* player) {
	m_Player = player;
	m_bStrictChecks = player->m_bStrictChecks;
}

bool CSteamGameServerPlayingWrapper::InitGameServer(uint32 unIP, uint16 usGamePort, uint16 usQueryPort, uint32 unFlags, AppId_t nGameAppId, const char *pchVersionString) {
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

void CSteamGameServerPlayingWrapper::SetProduct(const char *pszProduct) {
	CGameServerSetProductCall* playCall = dynamic_cast<CGameServerSetProductCall*>(m_Player->getNextCall(false, false, ECF_GS_SET_PRODUCT, true, __func__));
	CGameServerSetProductCall(pszProduct).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerSetProductCall* playEndCall = dynamic_cast<CGameServerSetProductCall*>(m_Player->getNextCall(false, true, ECF_GS_SET_PRODUCT, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::SetGameDescription(const char *pszGameDescription) {
	CGameServerSetGameDescCall* playCall = dynamic_cast<CGameServerSetGameDescCall*>(m_Player->getNextCall(false, false, ECF_GS_SET_GAME_DESC, true, __func__));
	CGameServerSetGameDescCall(pszGameDescription).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerSetGameDescCall* playEndCall = dynamic_cast<CGameServerSetGameDescCall*>(m_Player->getNextCall(false, true, ECF_GS_SET_GAME_DESC, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::SetModDir(const char *pszModDir) {
	CGameServerSetModDirCall* playCall = dynamic_cast<CGameServerSetModDirCall*>(m_Player->getNextCall(false, false, ECF_GS_SET_GAME_DIR, true, __func__));
	CGameServerSetModDirCall(pszModDir).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerSetModDirCall* playEndCall = dynamic_cast<CGameServerSetModDirCall*>(m_Player->getNextCall(false, true, ECF_GS_SET_GAME_DIR, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::SetDedicatedServer(bool bDedicated) {
	CGameServerSetDedicatedServerCall* playCall = dynamic_cast<CGameServerSetDedicatedServerCall*>(m_Player->getNextCall(false, false, ECF_GS_SET_DEDICATED_SERVER, true, __func__));
	CGameServerSetDedicatedServerCall(bDedicated).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerSetDedicatedServerCall* playEndCall = dynamic_cast<CGameServerSetDedicatedServerCall*>(m_Player->getNextCall(false, true, ECF_GS_SET_DEDICATED_SERVER, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::LogOn(const char *pszAccountName, const char *pszPassword) {
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerPlayingWrapper::LogOnAnonymous() {
	CGameServerLogOnAnonymousCall* playCall = dynamic_cast<CGameServerLogOnAnonymousCall*>(m_Player->getNextCall(false, false, ECF_GS_LOG_ON_ANONYMOUS, true, __func__));
	CGameServerLogOnAnonymousCall* playEndCall = dynamic_cast<CGameServerLogOnAnonymousCall*>(m_Player->getNextCall(false, true, ECF_GS_LOG_ON_ANONYMOUS, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::LogOff() {
	CGameServerLogOffCall* playCall = dynamic_cast<CGameServerLogOffCall*>(m_Player->getNextCall(false, false, ECF_GS_LOGOFF, true, __func__));
	CGameServerLogOffCall* playEndCall = dynamic_cast<CGameServerLogOffCall*>(m_Player->getNextCall(false, true, ECF_GS_LOGOFF, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

bool CSteamGameServerPlayingWrapper::BLoggedOn() {
	CGameServerBLoggedOnCall* playCall = dynamic_cast<CGameServerBLoggedOnCall*>(m_Player->getNextCall(false, false, ECF_GS_BLOGGEDON, true, __func__));
	CGameServerBLoggedOnCall* playEndCall = dynamic_cast<CGameServerBLoggedOnCall*>(m_Player->getNextCall(false, true, ECF_GS_BLOGGEDON, false, __func__));

	bool res = playEndCall->m_Res;
	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);

	return res;
}

bool CSteamGameServerPlayingWrapper::BSecure() {
	CGameServerBSecureCall* playCall = dynamic_cast<CGameServerBSecureCall*>(m_Player->getNextCall(false, false, ECF_GS_BSECURE, true, __func__));
	CGameServerBSecureCall* playEndCall = dynamic_cast<CGameServerBSecureCall*>(m_Player->getNextCall(false, true, ECF_GS_BSECURE, false, __func__));

	bool res = playEndCall->m_Res;
	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);

	return res;
}

CSteamID CSteamGameServerPlayingWrapper::GetSteamID() {
	CGameServerGetSteamIdCall* playCall = dynamic_cast<CGameServerGetSteamIdCall*>(m_Player->getNextCall(false, false, ECF_GS_GET_STEAM_ID, true, __func__));
	CGameServerGetSteamIdCall* playEndCall = dynamic_cast<CGameServerGetSteamIdCall*>(m_Player->getNextCall(false, true, ECF_GS_GET_STEAM_ID, false, __func__));

	CSteamID res(playEndCall->m_SteamId);
	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);

	return res;
}

bool CSteamGameServerPlayingWrapper::WasRestartRequested() {
	CGameServerWasRestartRequestedCall* playCall = dynamic_cast<CGameServerWasRestartRequestedCall*>(m_Player->getNextCall(false, false, ECF_GS_WAS_RESTART_REQUESTED, true, __func__));
	CGameServerWasRestartRequestedCall* playEndCall = dynamic_cast<CGameServerWasRestartRequestedCall*>(m_Player->getNextCall(false, true, ECF_GS_WAS_RESTART_REQUESTED, false, __func__));

	bool res = playEndCall->m_Result;
	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);

	return res;
}

void CSteamGameServerPlayingWrapper::SetMaxPlayerCount(int cPlayersMax) {
	CGameServerSetMaxPlayersCall* playCall = dynamic_cast<CGameServerSetMaxPlayersCall*>(m_Player->getNextCall(false, false, ECF_GS_SET_MAX_PLAYERS_COUNT, true, __func__));
	CGameServerSetMaxPlayersCall(cPlayersMax).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerSetMaxPlayersCall* playEndCall = dynamic_cast<CGameServerSetMaxPlayersCall*>(m_Player->getNextCall(false, true, ECF_GS_SET_MAX_PLAYERS_COUNT, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::SetBotPlayerCount(int cBotplayers) {
	CGameServerSetBotCountCall* playCall = dynamic_cast<CGameServerSetBotCountCall*>(m_Player->getNextCall(false, false, ECF_GS_SET_BOT_PLAYERS_COUNT, true, __func__));
	CGameServerSetBotCountCall(cBotplayers).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerSetBotCountCall* playEndCall = dynamic_cast<CGameServerSetBotCountCall*>(m_Player->getNextCall(false, true, ECF_GS_SET_BOT_PLAYERS_COUNT, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::SetServerName(const char *pszServerName) {
	CGameServerSetServerNameCall* playCall = dynamic_cast<CGameServerSetServerNameCall*>(m_Player->getNextCall(false, false, ECF_GS_SET_SERVER_NAME, true, __func__));
	CGameServerSetServerNameCall(pszServerName).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerSetServerNameCall* playEndCall = dynamic_cast<CGameServerSetServerNameCall*>(m_Player->getNextCall(false, true, ECF_GS_SET_SERVER_NAME, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::SetMapName(const char *pszMapName) {
	CGameServerSetMapNameCall* playCall = dynamic_cast<CGameServerSetMapNameCall*>(m_Player->getNextCall(false, false, ECF_GS_SET_MAP_NAME, true, __func__));
	CGameServerSetMapNameCall(pszMapName).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerSetMapNameCall* playEndCall = dynamic_cast<CGameServerSetMapNameCall*>(m_Player->getNextCall(false, true, ECF_GS_SET_MAP_NAME, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::SetPasswordProtected(bool bPasswordProtected) {
	CGameServerSetPasswordProtectedCall* playCall = dynamic_cast<CGameServerSetPasswordProtectedCall*>(m_Player->getNextCall(false, false, ECF_GS_SET_PASSWORD_PROTECTED, true, __func__));
	CGameServerSetPasswordProtectedCall(bPasswordProtected).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerSetPasswordProtectedCall* playEndCall = dynamic_cast<CGameServerSetPasswordProtectedCall*>(m_Player->getNextCall(false, true, ECF_GS_SET_PASSWORD_PROTECTED, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::SetSpectatorPort(uint16 unSpectatorPort) {
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerPlayingWrapper::SetSpectatorServerName(const char *pszSpectatorServerName) {
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerPlayingWrapper::ClearAllKeyValues() {
	CGameServerClearAllKVsCall* playCall = dynamic_cast<CGameServerClearAllKVsCall*>(m_Player->getNextCall(false, false, ECF_GS_CLEAR_ALL_KEY_VALUES, true, __func__));
	CGameServerClearAllKVsCall* playEndCall = dynamic_cast<CGameServerClearAllKVsCall*>(m_Player->getNextCall(false, true, ECF_GS_CLEAR_ALL_KEY_VALUES, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::SetKeyValue(const char *pKey, const char *pValue) {
	CGameServerSetKeyValueCall* playCall = dynamic_cast<CGameServerSetKeyValueCall*>(m_Player->getNextCall(false, false, ECF_GS_SET_KEY_VALUE, true, __func__));
	CGameServerSetKeyValueCall(pKey, pValue).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerSetKeyValueCall* playEndCall = dynamic_cast<CGameServerSetKeyValueCall*>(m_Player->getNextCall(false, true, ECF_GS_SET_KEY_VALUE, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::SetGameTags(const char *pchGameTags) {
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerPlayingWrapper::SetGameData(const char *pchGameData) {
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerPlayingWrapper::SetRegion(const char *pszRegion) {
	rehlds_syserror("%s: not implemented", __func__);
}

bool CSteamGameServerPlayingWrapper::SendUserConnectAndAuthenticate(uint32 unIPClient, const void *pvAuthBlob, uint32 cubAuthBlobSize, CSteamID *pSteamIDUser) {
	CGameServerSendUserConnectAndAuthenticateCall* playCall = dynamic_cast<CGameServerSendUserConnectAndAuthenticateCall*>(m_Player->getNextCall(false, false, ECF_GS_SEND_USER_CONNECT_AND_AUTHENTICATE, true, __func__));
	CGameServerSendUserConnectAndAuthenticateCall(unIPClient, pvAuthBlob, cubAuthBlobSize).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerSendUserConnectAndAuthenticateCall* playEndCall = dynamic_cast<CGameServerSendUserConnectAndAuthenticateCall*>(m_Player->getNextCall(false, true, ECF_GS_SEND_USER_CONNECT_AND_AUTHENTICATE, false, __func__));

	bool res = playEndCall->m_Res;
	*pSteamIDUser = CSteamID(playEndCall->m_OutSteamId);
	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);

	return res;
}

CSteamID CSteamGameServerPlayingWrapper::CreateUnauthenticatedUserConnection() {
	CGameServerCreateUnauthUserConnectionCall* playCall = dynamic_cast<CGameServerCreateUnauthUserConnectionCall*>(m_Player->getNextCall(false, false, ECF_GS_CREATE_UNAUTH_USER_CONNECTION, true, __func__));
	CGameServerCreateUnauthUserConnectionCall* playEndCall = dynamic_cast<CGameServerCreateUnauthUserConnectionCall*>(m_Player->getNextCall(false, true, ECF_GS_CREATE_UNAUTH_USER_CONNECTION, false, __func__));

	CSteamID res = playEndCall->m_SteamId;
	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);

	return res;
}

void CSteamGameServerPlayingWrapper::SendUserDisconnect(CSteamID steamIDUser) {
	CGameServerSendUserDisconnectCall* playCall = dynamic_cast<CGameServerSendUserDisconnectCall*>(m_Player->getNextCall(false, false, ECF_GS_SEND_USER_DISCONNECT, true, __func__));
	CGameServerSendUserDisconnectCall(steamIDUser).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerSendUserDisconnectCall* playEndCall = dynamic_cast<CGameServerSendUserDisconnectCall*>(m_Player->getNextCall(false, true, ECF_GS_SEND_USER_DISCONNECT, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

bool CSteamGameServerPlayingWrapper::BUpdateUserData(CSteamID steamIDUser, const char *pchPlayerName, uint32 uScore) {
	CGameServerBUpdateUserDataCall* playCall = dynamic_cast<CGameServerBUpdateUserDataCall*>(m_Player->getNextCall(false, false, ECF_GS_BUPDATE_USER_DATA, true, __func__));
	CGameServerBUpdateUserDataCall(steamIDUser, pchPlayerName, uScore).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerBUpdateUserDataCall* playEndCall = dynamic_cast<CGameServerBUpdateUserDataCall*>(m_Player->getNextCall(false, true, ECF_GS_BUPDATE_USER_DATA, false, __func__));

	bool res = playEndCall->m_Res;
	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);

	return res;
}

HAuthTicket CSteamGameServerPlayingWrapper::GetAuthSessionTicket(void *pTicket, int cbMaxTicket, uint32 *pcbTicket) {
	rehlds_syserror("%s: not implemented", __func__);
	//return k_HAuthTicketInvalid;
}

EBeginAuthSessionResult CSteamGameServerPlayingWrapper::BeginAuthSession(const void *pAuthTicket, int cbAuthTicket, CSteamID steamID) {
	rehlds_syserror("%s: not implemented", __func__);
	//return k_EBeginAuthSessionResultInvalidTicket;
}

void CSteamGameServerPlayingWrapper::EndAuthSession(CSteamID steamID) {
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamGameServerPlayingWrapper::CancelAuthTicket(HAuthTicket hAuthTicket) {
	rehlds_syserror("%s: not implemented", __func__);
}

EUserHasLicenseForAppResult CSteamGameServerPlayingWrapper::UserHasLicenseForApp(CSteamID steamID, AppId_t appID) {
	rehlds_syserror("%s: not implemented", __func__);
	//return k_EUserHasLicenseResultHasLicense;
}

bool CSteamGameServerPlayingWrapper::RequestUserGroupStatus(CSteamID steamIDUser, CSteamID steamIDGroup) {
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

void CSteamGameServerPlayingWrapper::GetGameplayStats() {
	rehlds_syserror("%s: not implemented", __func__);
}

SteamAPICall_t CSteamGameServerPlayingWrapper::GetServerReputation() {
	rehlds_syserror("%s: not implemented", __func__);
	//return k_uAPICallInvalid;
}

uint32 CSteamGameServerPlayingWrapper::GetPublicIP() {
	rehlds_syserror("%s: not implemented", __func__);
	//return 0;
}

bool CSteamGameServerPlayingWrapper::HandleIncomingPacket(const void *pData, int cbData, uint32 srcIP, uint16 srcPort) {
	CGameServerHandleIncomingPacketCall* playCall = dynamic_cast<CGameServerHandleIncomingPacketCall*>(m_Player->getNextCall(false, false, ECF_GS_HANDLE_INCOMING_PACKET, true, __func__));
	CGameServerHandleIncomingPacketCall(pData, cbData, srcIP, srcPort).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerHandleIncomingPacketCall* playEndCall = dynamic_cast<CGameServerHandleIncomingPacketCall*>(m_Player->getNextCall(false, true, ECF_GS_HANDLE_INCOMING_PACKET, false, __func__));

	bool res = playEndCall->m_Res;
	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);

	return res;
}

int CSteamGameServerPlayingWrapper::GetNextOutgoingPacket(void *pOut, int cbMaxOut, uint32 *pNetAdr, uint16 *pPort) {
	CGameServerGetNextOutgoingPacketCall* playCall = dynamic_cast<CGameServerGetNextOutgoingPacketCall*>(m_Player->getNextCall(false, false, ECF_GS_GET_NEXT_OUTGOING_PACKET, true, __func__));
	CGameServerGetNextOutgoingPacketCall(cbMaxOut).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerGetNextOutgoingPacketCall* playEndCall = dynamic_cast<CGameServerGetNextOutgoingPacketCall*>(m_Player->getNextCall(false, true, ECF_GS_GET_NEXT_OUTGOING_PACKET, false, __func__));

	int res = playEndCall->m_Result;
	*pNetAdr = playEndCall->m_Addr;
	*pPort = playEndCall->m_Port;
	memcpy(pOut, playEndCall->m_Buf, playEndCall->m_BufLen);
	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);

	return res;
}

void CSteamGameServerPlayingWrapper::EnableHeartbeats(bool bActive) {
	CGameServerEnableHeartbeatsCall* playCall = dynamic_cast<CGameServerEnableHeartbeatsCall*>(m_Player->getNextCall(false, false, ECF_GS_ENABLE_HEARTBEATS, true, __func__));
	CGameServerEnableHeartbeatsCall(bActive).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerEnableHeartbeatsCall* playEndCall = dynamic_cast<CGameServerEnableHeartbeatsCall*>(m_Player->getNextCall(false, true, ECF_GS_ENABLE_HEARTBEATS, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::SetHeartbeatInterval(int iHeartbeatInterval) {
	CGameServerSetHeartbeatIntervalCall* playCall = dynamic_cast<CGameServerSetHeartbeatIntervalCall*>(m_Player->getNextCall(false, false, ECF_GS_SET_HEARTBEATS_INTERVAL, true, __func__));
	CGameServerSetHeartbeatIntervalCall(iHeartbeatInterval).ensureArgsAreEqual(playCall, m_bStrictChecks, __func__);
	CGameServerSetHeartbeatIntervalCall* playEndCall = dynamic_cast<CGameServerSetHeartbeatIntervalCall*>(m_Player->getNextCall(false, true, ECF_GS_SET_HEARTBEATS_INTERVAL, false, __func__));

	m_Player->freeFuncCall(playCall); m_Player->freeFuncCall(playEndCall);
}

void CSteamGameServerPlayingWrapper::ForceHeartbeat() {
	rehlds_syserror("%s: not implemented", __func__);
}

SteamAPICall_t CSteamGameServerPlayingWrapper::AssociateWithClan(CSteamID steamIDClan) {
	rehlds_syserror("%s: not implemented", __func__);
	//return k_uAPICallInvalid;
}

SteamAPICall_t CSteamGameServerPlayingWrapper::ComputeNewPlayerCompatibility(CSteamID steamIDNewPlayer) {
	rehlds_syserror("%s: not implemented", __func__);
	//return k_uAPICallInvalid;
}







CSteamAppsPlayingWrapper::CSteamAppsPlayingWrapper(CPlayingEngExtInterceptor* player) {
	m_Player = player;
	m_bStrictChecks = player->m_bStrictChecks;
}

bool CSteamAppsPlayingWrapper::BIsSubscribed() {
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

bool CSteamAppsPlayingWrapper::BIsLowViolence() {
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

bool CSteamAppsPlayingWrapper::BIsCybercafe() {
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

bool CSteamAppsPlayingWrapper::BIsVACBanned() {
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

const char* CSteamAppsPlayingWrapper::GetCurrentGameLanguage() {
	rehlds_syserror("%s: not implemented", __func__);
	//return "";
}

const char* CSteamAppsPlayingWrapper::GetAvailableGameLanguages() {
	rehlds_syserror("%s: not implemented", __func__);
	//return "";
}

bool CSteamAppsPlayingWrapper::BIsSubscribedApp(AppId_t appID) {
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

bool CSteamAppsPlayingWrapper::BIsDlcInstalled(AppId_t appID) {
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

uint32 CSteamAppsPlayingWrapper::GetEarliestPurchaseUnixTime(AppId_t nAppID) {
	rehlds_syserror("%s: not implemented", __func__);
	//return 0;
}

bool CSteamAppsPlayingWrapper::BIsSubscribedFromFreeWeekend() {
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

int CSteamAppsPlayingWrapper::GetDLCCount() {
	rehlds_syserror("%s: not implemented", __func__);
	//return 0;
}

bool CSteamAppsPlayingWrapper::BGetDLCDataByIndex(int iDLC, AppId_t *pAppID, bool *pbAvailable, char *pchName, int cchNameBufferSize) {
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

void CSteamAppsPlayingWrapper::InstallDLC(AppId_t nAppID) {
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamAppsPlayingWrapper::UninstallDLC(AppId_t nAppID) {
	rehlds_syserror("%s: not implemented", __func__);
}

void CSteamAppsPlayingWrapper::RequestAppProofOfPurchaseKey(AppId_t nAppID) {
	rehlds_syserror("%s: not implemented", __func__);
}

bool CSteamAppsPlayingWrapper::GetCurrentBetaName(char *pchName, int cchNameBufferSize) {
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

bool CSteamAppsPlayingWrapper::MarkContentCorrupt(bool bMissingFilesOnly) {
	rehlds_syserror("%s: not implemented", __func__);
	//return false;
}

uint32 CSteamAppsPlayingWrapper::GetInstalledDepots(DepotId_t *pvecDepots, uint32 cMaxDepots) {
	rehlds_syserror("%s: not implemented", __func__);
	//return 0;
}

uint32 CSteamAppsPlayingWrapper::GetAppInstallDir(AppId_t appID, char *pchFolder, uint32 cchFolderBufferSize) {
	rehlds_syserror("%s: not implemented", __func__);
	//return 0;
}
