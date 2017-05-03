#pragma once
#ifdef _WIN32

#include "osconfig.h"

#include "steam/steam_api.h"
#include "steam/steam_gameserver.h"

#include <string>
#include <iostream>
#include <fstream>

const int HOSTENT_DATA_MAX_ALIASES = 10;
const int HOSTENT_DATA_MAX_ADDRS = 32;

struct hostent_data_t {
	char hostName[256];
	int hostNameLen;

	int numAliases;
	char aliases[HOSTENT_DATA_MAX_ALIASES][256];
	int aliasesLengths[HOSTENT_DATA_MAX_ALIASES];

	short addrtype;
	short addrLen;
	int numAddrs;
	char addrs[HOSTENT_DATA_MAX_ADDRS][32];

	//Used by reader only
	char* preparedAddrs[HOSTENT_DATA_MAX_ADDRS + 1];
	char* preparedAliases[HOSTENT_DATA_MAX_ALIASES + 1];


	void clear() {
		hostName[0] = 0;
		hostNameLen = 0;

		numAliases = 0;
		for (int i = 0; i < HOSTENT_DATA_MAX_ALIASES; i++) {
			aliasesLengths[i] = 0;
			aliases[i][0] = 0;
		}

		addrtype = 0;
		addrLen = 0;
		numAddrs = 0;
	}
};

enum ExtCallFuncs {
	ECF_NONE = 0,
	ECF_SLEEP = 1,
	ECF_QUERY_PERF_FREQ = 2,
	ECF_QUERY_PERF_COUNTER = 3,
	ECF_GET_TICK_COUNT = 4,
	ECF_GET_LOCAL_TIME = 5,
	ECF_GET_SYSTEM_TIME = 6,
	ECF_GET_TIMEZONE_INFO = 7,

	ECF_SOCKET = 8,
	ECF_IOCTL_SOCKET = 9,
	ECF_SET_SOCK_OPT = 10,
	ECF_CLOSE_SOCKET = 11,
	ECF_RECVFROM = 12,
	ECF_SENDTO = 13,
	ECF_BIND = 14,
	ECF_GET_SOCK_NAME = 15,
	ECF_WSA_GET_LAST_ERROR = 16,

	ECF_STEAM_CALLBACK_CALL_1 = 17,
	ECF_STEAM_CALLBACK_CALL_2 = 18,

	ECF_STEAM_API_REGISTER_CALLBACK = 19,
	ECF_STEAM_API_INIT = 20,
	ECF_STEAM_API_UNREGISTER_CALL_RESULT = 21,
	ECF_STEAMAPPS = 22,
	ECF_STEAMAPPS_GET_CURRENT_GAME_LANGUAGE = 23,

	ECF_STEAMGAMESERVER_INIT = 24,
	ECF_STEAMGAMESERVER = 25,
	ECF_GS_SET_PRODUCT = 26,
	ECF_GS_SET_GAME_DIR = 27,
	ECF_GS_SET_DEDICATED_SERVER = 28,
	ECF_GS_SET_GAME_DESC = 29,
	ECF_GS_LOG_ON_ANONYMOUS = 30,
	ECF_GS_ENABLE_HEARTBEATS = 31,
	ECF_GS_SET_HEARTBEATS_INTERVAL = 32,
	ECF_GS_SET_MAX_PLAYERS_COUNT = 33,
	ECF_GS_SET_BOT_PLAYERS_COUNT = 34,
	ECF_GS_SET_SERVER_NAME = 35,
	ECF_GS_SET_MAP_NAME = 36,
	ECF_GS_SET_PASSWORD_PROTECTED = 37,
	ECF_GS_CLEAR_ALL_KEY_VALUES = 38,
	ECF_GS_SET_KEY_VALUE = 39,
	ECF_STEAM_API_SET_BREAKPAD_APP_ID = 40,
	ECF_GS_WAS_RESTART_REQUESTED = 41,
	ECF_STEAMGAMESERVER_RUN_CALLBACKS = 42,
	ECF_GS_GET_NEXT_OUTGOING_PACKET = 43,
	ECF_STEAM_API_RUN_CALLBACKS = 44,
	ECF_GS_GET_STEAM_ID = 45,
	ECF_GS_BSECURE = 46,
	ECF_GS_HANDLE_INCOMING_PACKET = 47,
	ECF_GS_SEND_USER_CONNECT_AND_AUTHENTICATE = 48,
	ECF_GS_SEND_USER_DISCONNECT = 49,
	ECF_GS_BUPDATE_USER_DATA = 50,
	ECF_GS_CREATE_UNAUTH_USER_CONNECTION = 51,

	ECF_GET_HOST_NAME = 52,
	ECF_GET_HOST_BY_NAME = 53,
	ECF_GET_PROCESS_TIMES = 54,
	ECF_GET_SYSTEM_TIME_AS_FILE_TIME = 55,

	ECF_CSTD_TIME = 56,
	ECF_CSTD_LOCALTIME = 57,
	ECF_CSTD_SRAND_CALL = 58,
	ECF_CSTD_RAND_CALL = 59,

	ECF_GS_LOGOFF = 60,

	ECF_STEAMGAMESERVER_SHUTDOWN = 61,
	ECF_STEAM_API_UNREGISTER_CALLBACK = 62,

	ECF_GS_BLOGGEDON = 63,
};

struct CSteamCallbackState_t {
	uint8 m_nCallbackFlags;
	int m_iCallback;

	void clear() {
		m_nCallbackFlags = 0;
		m_iCallback = 0;
	}
};

class IEngExtCall {
public:
	bool m_Start;
	bool m_End;

public:
	IEngExtCall() { m_Start = m_End = false; }
	virtual ~IEngExtCall() { }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict) = 0;
	virtual std::string toString() = 0;
	virtual ExtCallFuncs getOpcode() = 0;
	virtual void writePrologue(std::ostream &stream) { }
	virtual void writeEpilogue(std::ostream &stream) { }
	virtual void readPrologue(std::istream &stream) { }
	virtual void readEpilogue(std::istream &stream) { }
	void ensureArgsAreEqual(IEngExtCall* other, bool strict, const char* callSource);
};

class IEngCallbackCall : public IEngExtCall {
public:
	virtual void someFuncInVTable() { }
};

class IEngExtCallFactory {
public:
	static IEngExtCall* createByOpcode(ExtCallFuncs opc, void* ptr, int ptrSize);
};

//fake call; eof marker
class CEndRecordCall : public IEngExtCall {
public:
	virtual bool compareInputArgs(IEngExtCall* other, bool strict) { return false; }
	virtual std::string toString() { return std::string("EOF"); }
	virtual ExtCallFuncs getOpcode() { return ECF_NONE; }
};

class CSleepExtCall : public IEngExtCall {
public:
	DWORD m_Time;

public:
	CSleepExtCall() { m_Time = 0; }
	CSleepExtCall(DWORD time);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_SLEEP; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CQueryPerfFreqCall : public IEngExtCall {
public:
	int64 m_Freq;
	BOOL m_Res;

public:
	CQueryPerfFreqCall() { m_Freq = 0; m_Res = false; }

	void SetResult(LARGE_INTEGER freq, BOOL res) { m_Freq = freq.QuadPart; m_Res = res; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_QUERY_PERF_FREQ; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CQueryPerfCounterCall : public IEngExtCall {
public:
	int64 m_Counter;
	BOOL m_Res;

public:
	CQueryPerfCounterCall() { m_Counter = 0; m_Res = 0; }

	void SetResult(LARGE_INTEGER counter, BOOL res) { m_Counter = counter.QuadPart; m_Res = res; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_QUERY_PERF_COUNTER; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};


class CGetTickCountCall : public IEngExtCall {
public:
	DWORD m_Res;

public:
	CGetTickCountCall() { m_Res = 0; }

	void SetResult(DWORD res) { m_Res = res; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GET_TICK_COUNT; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGetLocalTimeCall : public IEngExtCall {
public:
	SYSTEMTIME m_Res;

public:
	CGetLocalTimeCall() { memset(&m_Res, 0, sizeof(m_Res)); }

	void SetResult(LPSYSTEMTIME res) { m_Res = *res; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GET_LOCAL_TIME; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGetSystemTimeCall : public IEngExtCall {
public:
	SYSTEMTIME m_Res;

public:
	CGetSystemTimeCall() { memset(&m_Res, 0, sizeof(m_Res)); }

	void SetResult(LPSYSTEMTIME res) { m_Res = *res; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GET_SYSTEM_TIME; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGetTimeZoneInfoCall : public IEngExtCall {
public:
	TIME_ZONE_INFORMATION m_Res;

public:
	CGetTimeZoneInfoCall() { memset(&m_Res, 0, sizeof(m_Res)); }
	
	void SetResult(LPTIME_ZONE_INFORMATION res) { m_Res = *res; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GET_TIMEZONE_INFO; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CSocketCall : public IEngExtCall {
public:
	int m_Af;
	int m_Type;
	int m_Protocol;
	SOCKET m_Res;

public:
	CSocketCall() { m_Af = m_Type = m_Protocol = 0; m_Res = INVALID_SOCKET; }
	CSocketCall(int af, int type, int protocol);
	void setResult(SOCKET s) { m_Res = s;  }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_SOCKET; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CIoCtlSocketCall : public IEngExtCall {
public:
	SOCKET m_Socket;
	long m_Cmd;
	u_long m_InValue;

	u_long m_OutValue;
	int m_Res;

public:
	CIoCtlSocketCall() { m_Socket = INVALID_SOCKET; m_Cmd = 0; m_InValue = 0; m_OutValue = 0; m_Res = 0; }
	CIoCtlSocketCall(SOCKET s, long cmd, u_long inValue);
	void setResult(u_long outValue, int res) { m_Res = res; m_OutValue = outValue; }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_IOCTL_SOCKET; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CSetSockOptCall : public IEngExtCall {
public:
	SOCKET m_Socket;
	int m_Level;
	int m_OptName;
	char m_OptVal[32];
	int m_OptValLen;

	int m_Res;

public:
	CSetSockOptCall() { m_Socket = INVALID_SOCKET; m_Level = m_OptName = m_OptValLen = 0; m_Res = 0; }
	CSetSockOptCall(SOCKET s, int level, int optname, const char* optval, int optlen);
	void setResult(int res) { m_Res = res; }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_SET_SOCK_OPT; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CCloseSocketCall : public IEngExtCall {
public:
	SOCKET m_Socket;
	int m_Res;

public:
	CCloseSocketCall() { m_Socket = INVALID_SOCKET; m_Res = 0; }
	CCloseSocketCall(SOCKET s);
	void setResult(int res) { m_Res = res; }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_CLOSE_SOCKET; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CRecvFromCall : public IEngExtCall {
public:
	SOCKET m_Socket;
	int m_Len;
	int m_Flags;
	int m_FromLenIn;
	
	char m_Data[8192];
	int m_FromLenOut;
	char m_From[32];
	int m_Res;

public:
	CRecvFromCall() { m_Socket = INVALID_SOCKET; m_Len = m_Flags = m_FromLenIn = 0; m_FromLenOut = 0; m_Res = -1; }
	CRecvFromCall(SOCKET s, int len, int flags, int fromlen);
	void setResult(const void* data, const void* from, int fromLen, int res);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_RECVFROM; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CSendToCall : public IEngExtCall {
public:
	SOCKET m_Socket;
	char m_Data[8192];
	int m_Len;
	int m_Flags;
	char m_To[32];
	int m_ToLen;

	int m_Res;

public:
	CSendToCall() { m_Socket = INVALID_SOCKET, m_Len = m_Flags = m_ToLen = 0; m_Res = -1; }
	CSendToCall(SOCKET s, const void* buf, int len, int flags, const void* to, int tolen);
	void setResult(int res) { m_Res = res;  }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_SENDTO; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CBindCall : public IEngExtCall {
public:
	SOCKET m_Socket;
	char m_Addr[32];
	int m_AddrLen;

	int m_Res;

public:
	CBindCall() { m_Socket = INVALID_SOCKET; m_AddrLen = 0; m_Res = 0; }
	CBindCall(SOCKET s, const void* addr, int addrlen);
	void setResult(int res) { m_Res = res; }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_BIND; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGetSockNameCall : public IEngExtCall {
public:
	SOCKET m_Socket;
	int m_AddrLenIn;

	char m_Addr[32];
	int m_AddrLenOut;
	int m_Res;

public:
	CGetSockNameCall() { m_Socket = INVALID_SOCKET; m_AddrLenIn = 0; m_AddrLenOut = m_Res = 0; }
	CGetSockNameCall(SOCKET s, int addrlen);
	void setResult(const void* addr, int addrlen, int res);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GET_SOCK_NAME; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CWSAGetLastErrorCall : public IEngExtCall {
public:
	int m_Res;

public:
	CWSAGetLastErrorCall() { m_Res = 0; }
	void setResult(int res) { m_Res = res; }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_WSA_GET_LAST_ERROR; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CSteamCallbackCall1 : public IEngCallbackCall {
public:
	int m_CallbackId;
	char m_Data[1024];
	int m_DataSize;
	CSteamCallbackState_t m_InState;
	CSteamCallbackState_t m_OutState;

public:
	CSteamCallbackCall1() { m_CallbackId = 0; m_DataSize = 0; m_InState.clear(), m_OutState.clear();  }
	CSteamCallbackCall1(int cbId, void* data, int dataSize, CCallbackBase* cb);

	void setResult(CCallbackBase* cb) { m_OutState.m_iCallback = cb->GetICallback(); m_OutState.m_nCallbackFlags = cb->GetFlags(); }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAM_CALLBACK_CALL_1; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CSteamCallbackCall2 : public IEngCallbackCall {
public:
	int m_CallbackId;
	char m_Data[1024];
	int m_DataSize;
	bool m_bIOFailure;
	SteamAPICall_t m_SteamAPICall;
	CSteamCallbackState_t m_InState;
	CSteamCallbackState_t m_OutState;

public:
	CSteamCallbackCall2() { m_CallbackId = m_DataSize = 0; m_bIOFailure = false; m_SteamAPICall = k_uAPICallInvalid; m_InState.clear(), m_OutState.clear(); }
	CSteamCallbackCall2(int cbId, void* data, int dataSize, bool ioFailure, SteamAPICall_t apiCall, CCallbackBase* cb);

	void setResult(CCallbackBase* cb) { m_OutState.m_iCallback = cb->GetICallback(); m_OutState.m_nCallbackFlags = cb->GetFlags(); }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAM_CALLBACK_CALL_2; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CSteamApiRegisterCallbackCall : public IEngExtCall {
public:
	int m_RehldsCallbackId;
	int m_iSteamCallbackId;
	CSteamCallbackState_t m_InState;
	CSteamCallbackState_t m_OutState;

public:
	CSteamApiRegisterCallbackCall() { m_RehldsCallbackId = 0; m_iSteamCallbackId = 0; m_InState.clear(), m_OutState.clear(); }
	CSteamApiRegisterCallbackCall(int rehldsCallbackId, int steamCallbackId, CCallbackBase* cb);

	void setResult(CCallbackBase* cb) { m_OutState.m_iCallback = cb->GetICallback(); m_OutState.m_nCallbackFlags = cb->GetFlags(); }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAM_API_REGISTER_CALLBACK; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CSteamApiInitCall : public IEngExtCall {
public:
	bool m_Res;

public:
	CSteamApiInitCall() { m_Res = false; }
	void setResult(bool res) { m_Res = res; }
	
	
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAM_API_INIT; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CSteamApiUnrigestierCallResultCall : public IEngExtCall {
public:
	int m_RehldsCallbackId;
	SteamAPICall_t m_SteamApiCall;
	CSteamCallbackState_t m_InState;
	CSteamCallbackState_t m_OutState;

public:
	CSteamApiUnrigestierCallResultCall() { m_RehldsCallbackId = 0; m_SteamApiCall = k_uAPICallInvalid; m_InState.clear(), m_OutState.clear(); }
	CSteamApiUnrigestierCallResultCall(int rehldsCallbackId, SteamAPICall_t steamApiCall, CCallbackBase* cb);

	void setResult(CCallbackBase* cb) { m_OutState.m_iCallback = cb->GetICallback(); m_OutState.m_nCallbackFlags = cb->GetFlags(); }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAM_API_UNREGISTER_CALL_RESULT; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CSteamAppsCall : public IEngExtCall {
public:
	bool m_ReturnNull;

public:
	CSteamAppsCall() { m_ReturnNull = false; }

	void setReturnNull(bool retNull) { m_ReturnNull = retNull; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAMAPPS; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CSteamAppGetCurrentGameLanguageCall : public IEngExtCall {
public:
	int m_ResLen;
	char m_Res[256];

public:
	CSteamAppGetCurrentGameLanguageCall() { m_ResLen = 0; }

	void setResult(const char* res);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAMAPPS_GET_CURRENT_GAME_LANGUAGE; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CSteamGameServerInitCall : public IEngExtCall {
public:
	uint32 m_IP;
	uint16 m_SteamPort;
	uint16 m_GamePort;
	uint16 m_QueryPort;
	EServerMode m_ServerMode;
	char m_Version[256];
	int m_VersionLen;

	bool m_Res;

public:
	CSteamGameServerInitCall() { m_IP = 0; m_SteamPort = m_GamePort = m_QueryPort = 0; m_ServerMode = eServerModeInvalid; m_VersionLen = 0; m_Res = false; }
	CSteamGameServerInitCall(uint32 unIP, uint16 usSteamPort, uint16 usGamePort, uint16 usQueryPort, EServerMode eServerMode, const char *pchVersionString);

	void setResult(bool res) { m_Res = res; }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAMGAMESERVER_INIT; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CSteamGameServerCall : public IEngExtCall {
public:
	bool m_ReturnNull;

public:
	CSteamGameServerCall() { m_ReturnNull = false; }

	void setReturnNull(bool retNull) { m_ReturnNull = retNull; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAMGAMESERVER; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGameServerSetProductCall : public IEngExtCall {
public:
	char m_Product[32];
	int m_ProductLen;

public:
	CGameServerSetProductCall() { m_ProductLen = 0; }
	CGameServerSetProductCall(const char* product);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_SET_PRODUCT; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CGameServerSetModDirCall : public IEngExtCall {
public:
	char m_Dir[32];
	int m_DirLen;

public:
	CGameServerSetModDirCall() { m_DirLen = 0; }
	CGameServerSetModDirCall(const char* dir);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_SET_GAME_DIR; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CGameServerSetDedicatedServerCall : public IEngExtCall {
public:
	bool m_Dedicated;

public:
	CGameServerSetDedicatedServerCall() { m_Dedicated = false; }
	CGameServerSetDedicatedServerCall(bool dedicated);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_SET_DEDICATED_SERVER; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CGameServerSetGameDescCall : public IEngExtCall {
public:
	char m_Desc[128];
	int m_DescLen;

public:
	CGameServerSetGameDescCall() { m_DescLen = 0; }
	CGameServerSetGameDescCall(const char* desc);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_SET_GAME_DESC; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CGameServerLogOnAnonymousCall : public IEngExtCall {
public:
	CGameServerLogOnAnonymousCall() { }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_LOG_ON_ANONYMOUS; }
};

class CGameServerEnableHeartbeatsCall : public IEngExtCall {
public:
	bool m_Heartbeats;

public:
	CGameServerEnableHeartbeatsCall() { m_Heartbeats = false; }
	CGameServerEnableHeartbeatsCall(bool hearbeats);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_ENABLE_HEARTBEATS; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CGameServerSetHeartbeatIntervalCall : public IEngExtCall {
public:
	int m_Interval;

public:
	CGameServerSetHeartbeatIntervalCall() { m_Interval = 0; }
	CGameServerSetHeartbeatIntervalCall(int interval);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_SET_HEARTBEATS_INTERVAL; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CGameServerSetMaxPlayersCall : public IEngExtCall {
public:
	int m_MaxPlayers;

public:
	CGameServerSetMaxPlayersCall() { m_MaxPlayers = 0; }
	CGameServerSetMaxPlayersCall(int maxPlayers);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_SET_MAX_PLAYERS_COUNT; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CGameServerSetBotCountCall : public IEngExtCall {
public:
	int m_NumBots;

public:
	CGameServerSetBotCountCall() { m_NumBots = 0; }
	CGameServerSetBotCountCall(int numBots);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_SET_BOT_PLAYERS_COUNT; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CGameServerSetServerNameCall : public IEngExtCall {
public:
	char m_ServerName[128];
	int m_ServerNameLen;

public:
	CGameServerSetServerNameCall() { m_ServerNameLen = 0; }
	CGameServerSetServerNameCall(const char* serverName);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_SET_SERVER_NAME; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CGameServerSetMapNameCall : public IEngExtCall {
public:
	char m_MapName[128];
	int m_MapNameLen;

public:
	CGameServerSetMapNameCall() { m_MapNameLen = 0; }
	CGameServerSetMapNameCall(const char* mapName);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_SET_MAP_NAME; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CGameServerSetPasswordProtectedCall : public IEngExtCall {
public:
	bool m_PasswordProtected;

public:
	CGameServerSetPasswordProtectedCall() { m_PasswordProtected = false; }
	CGameServerSetPasswordProtectedCall(bool passwordProtected);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_SET_PASSWORD_PROTECTED; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CGameServerClearAllKVsCall : public IEngExtCall {
public:
	CGameServerClearAllKVsCall() { }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_CLEAR_ALL_KEY_VALUES; }
};

class CGameServerSetKeyValueCall : public IEngExtCall {
public:
	char m_Key[128];
	int m_KeyLen;
	char m_Value[128];
	int m_ValueLen;
public:
	CGameServerSetKeyValueCall() { m_KeyLen = m_ValueLen = 0; }
	CGameServerSetKeyValueCall(const char* key, const char* value);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_SET_KEY_VALUE; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CSteamApiSetBreakpadAppIdCall : public IEngExtCall {
public:
	uint32 m_AppId;

public:
	CSteamApiSetBreakpadAppIdCall() { m_AppId = 0; }
	CSteamApiSetBreakpadAppIdCall(uint32 appId);

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAM_API_SET_BREAKPAD_APP_ID; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};


class CGameServerWasRestartRequestedCall : public IEngExtCall {
public:
	bool m_Result;

public:
	CGameServerWasRestartRequestedCall() { m_Result = false; }

	void setResult(bool res) { m_Result = res; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_WAS_RESTART_REQUESTED; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CSteamGameServerRunCallbacksCall : public IEngExtCall {
public:
	CSteamGameServerRunCallbacksCall() { }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAMGAMESERVER_RUN_CALLBACKS; }
};

class CGameServerGetNextOutgoingPacketCall : public IEngExtCall {
public:
	int m_MaxOut;

	char m_Buf[8192];
	int m_BufLen;
	int m_Result;
	uint32 m_Addr;
	uint16 m_Port;
	
public:
	CGameServerGetNextOutgoingPacketCall() { m_MaxOut = 0; m_BufLen = m_Result = 0; m_Addr = 0; m_Port = 0; }
	CGameServerGetNextOutgoingPacketCall(int maxOut);

	void setResult(void* buf, int res, uint32* pAddr, uint16* pPort);
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_GET_NEXT_OUTGOING_PACKET; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CSteamApiRunCallbacksCall : public IEngExtCall {
public:
	CSteamApiRunCallbacksCall() { }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAM_API_RUN_CALLBACKS; }
};

class CGameServerGetSteamIdCall : public IEngExtCall {
public:
	uint64 m_SteamId;

public:
	CGameServerGetSteamIdCall() { m_SteamId = 0; }

	void setResult(CSteamID &res) { m_SteamId = res.ConvertToUint64(); }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_GET_STEAM_ID; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGameServerBSecureCall : public IEngExtCall {
public:
	bool m_Res;

public:
	CGameServerBSecureCall() { m_Res = false; }

	void setResult(bool res) { m_Res = res; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_BSECURE; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGameServerHandleIncomingPacketCall : public IEngExtCall {
public:
	char m_Data[8192];
	int m_Len;
	uint32 m_Ip;
	uint16 m_Port;
	bool m_Res;

public:
	CGameServerHandleIncomingPacketCall() { m_Len = 0; m_Ip = 0; m_Port = 0; m_Res = false; }
	CGameServerHandleIncomingPacketCall(const void *pData, int cbData, uint32 srcIP, uint16 srcPort);

	void setResult(bool res) { m_Res = res; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_HANDLE_INCOMING_PACKET; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGameServerSendUserConnectAndAuthenticateCall : public IEngExtCall {
public:
	char m_AuthBlob[4096];
	int m_AuthBlobLen;
	uint32 m_IP;

	uint64 m_OutSteamId;
	bool m_Res;

public:
	CGameServerSendUserConnectAndAuthenticateCall() { m_IP = 0; m_AuthBlobLen = 0; m_OutSteamId = 0; m_Res = false; }
	CGameServerSendUserConnectAndAuthenticateCall(uint32 unIPClient, const void *pvAuthBlob, uint32 cubAuthBlobSize);

	void setResult(CSteamID& steamId, bool res) { m_OutSteamId = steamId.ConvertToUint64(); m_Res = res; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_SEND_USER_CONNECT_AND_AUTHENTICATE; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGameServerSendUserDisconnectCall : public IEngExtCall {
public:
	uint64 m_SteamId;

public:
	CGameServerSendUserDisconnectCall() { m_SteamId = 0; }
	CGameServerSendUserDisconnectCall(CSteamID& steamId) { m_SteamId = steamId.ConvertToUint64(); }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_SEND_USER_DISCONNECT; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CGameServerBUpdateUserDataCall : public IEngExtCall {
public:
	uint64 m_SteamId;
	char m_PlayerName[64];
	int m_PlayerNameLen;
	uint32 m_Score;

	bool m_Res;

public:
	CGameServerBUpdateUserDataCall() { m_SteamId = 0; m_Score = 0; m_PlayerNameLen = 0; m_Res = false; }
	CGameServerBUpdateUserDataCall(CSteamID steamIDUser, const char *pchPlayerName, uint32 uScore);

	void setResult(bool res) { m_Res = res; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_BUPDATE_USER_DATA; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGameServerCreateUnauthUserConnectionCall : public IEngExtCall {
public:
	uint64 m_SteamId;

public:
	CGameServerCreateUnauthUserConnectionCall() { m_SteamId = 0; }

	void setResult(CSteamID &steamId) { m_SteamId = steamId.ConvertToUint64(); }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_CREATE_UNAUTH_USER_CONNECTION; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGetHostNameCall : public IEngExtCall {
public:
	int m_NameLenIn;

	int m_NameLenOut;
	char m_Name[2048];
	int m_Res;

public:
	CGetHostNameCall() { m_NameLenIn = 0; m_NameLenOut = 0; m_Res = 0; m_Name[0] = 0; }
	CGetHostNameCall(int namelen) { m_NameLenIn = namelen; m_NameLenOut = 0; m_Res = 0; m_Name[0] = 0; }

	void setResult(char* hostName, int res);
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GET_HOST_NAME; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGetHostByNameCall : public IEngExtCall {
public:
	char m_Name[256];
	int m_NameLen;

	hostent_data_t m_HostentData;

public:
	CGetHostByNameCall() { m_Name[0] = 0; m_NameLen = 0; m_HostentData.clear(); }
	CGetHostByNameCall(const char* name);

	void setResult(const hostent* hostEnt);
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GET_HOST_BY_NAME; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGetProcessTimesCall : public IEngExtCall {
public:
	BOOL m_Res;
	FILETIME m_CreationTime;
	FILETIME m_ExitTime;
	FILETIME m_KernelTime;
	FILETIME m_UserTime;

public:
	CGetProcessTimesCall() { 
		m_Res = FALSE; 
		memset(&m_CreationTime, 0, sizeof(m_CreationTime)); 
		memset(&m_ExitTime, 0, sizeof(m_ExitTime));
		memset(&m_KernelTime, 0, sizeof(m_KernelTime));
		memset(&m_UserTime, 0, sizeof(m_UserTime));
	}

	void setResult(BOOL res, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime);
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GET_PROCESS_TIMES; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGetSystemTimeAsFileTimeCall : public IEngExtCall {
public:
	FILETIME m_SystemTime;

public:
	CGetSystemTimeAsFileTimeCall() { memset(&m_SystemTime, 0, sizeof(m_SystemTime)); }

	void setResult(LPFILETIME systemTime);
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GET_SYSTEM_TIME_AS_FILE_TIME; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CStdTimeCall : public IEngExtCall {
public:
	bool m_InTimeNull;

	uint32 m_Res;

public:
	CStdTimeCall(uint32* inTime);
	CStdTimeCall() { m_InTimeNull = false; m_Res = 0; }

	void setResult(uint32 res);
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_CSTD_TIME; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CStdLocalTimeCall : public IEngExtCall {
public:
	uint32 m_Time;

	struct tm m_Res;

public:
	CStdLocalTimeCall(uint32 inTime);
	CStdLocalTimeCall() { m_Time = 0; memset(&m_Res, 0, sizeof(m_Res)); }

	void setResult(struct tm *res);
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_CSTD_LOCALTIME; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CStdSrandCall : public IEngExtCall {
public:
	uint32 m_Seed;

public:
	CStdSrandCall() { m_Seed = 0; }
	CStdSrandCall(uint32 seed) { m_Seed = seed;  }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_CSTD_SRAND_CALL; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
};

class CStdRandCall : public IEngExtCall {
public:
	int m_Res;

public:
	CStdRandCall() { m_Res = 0; }

	void setResult(int res) { m_Res = res; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_CSTD_RAND_CALL; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGameServerLogOffCall : public IEngExtCall {

public:
	CGameServerLogOffCall() {  }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_LOGOFF; }

};

class CSteamGameServerShutdownCall : public IEngExtCall {

public:
	CSteamGameServerShutdownCall() {  }

	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAMGAMESERVER_SHUTDOWN; }

};

class CSteamApiUnregisterCallbackCall : public IEngExtCall {
public:
	int m_RehldsCallbackId;
	CSteamCallbackState_t m_InState;
	CSteamCallbackState_t m_OutState;

public:
	CSteamApiUnregisterCallbackCall() { m_RehldsCallbackId = 0; m_InState.clear(), m_OutState.clear(); }
	CSteamApiUnregisterCallbackCall(int rehldsCallbackId, CCallbackBase* cb);

	void setResult(CCallbackBase* cb) { m_OutState.m_iCallback = cb->GetICallback(); m_OutState.m_nCallbackFlags = cb->GetFlags(); }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_STEAM_API_UNREGISTER_CALLBACK; }
	virtual void writePrologue(std::ostream &stream);
	virtual void readPrologue(std::istream &stream);
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

class CGameServerBLoggedOnCall : public IEngExtCall {
public:
	bool m_Res;

public:
	CGameServerBLoggedOnCall() { m_Res = false; }

	void setResult(bool res) { m_Res = res; }
	virtual bool compareInputArgs(IEngExtCall* other, bool strict);
	virtual std::string toString();
	virtual ExtCallFuncs getOpcode() { return ECF_GS_BLOGGEDON; }
	virtual void writeEpilogue(std::ostream &stream);
	virtual void readEpilogue(std::istream &stream);
};

#endif
