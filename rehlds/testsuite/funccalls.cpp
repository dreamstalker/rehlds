#include "precompiled.h"

void PrintSystemTime(LPSYSTEMTIME t, std::stringstream &ss)
{
	ss << "{"
		<< " Year: " << t->wYear
		<< " Month: " << t->wMonth
		<< " Day: " << t->wDay
		<< " DayOfWeek: " << t->wDayOfWeek
		<< " Hour: " << t->wHour
		<< " Month: " << t->wMonth
		<< " Second: " << t->wSecond
		<< " Millisecond: " << t->wMilliseconds
		<< " }";
}

void PrintFileTime(LPFILETIME t, std::stringstream &ss)
{
	ss << "{"
		<< " highDate: " << t->dwHighDateTime
		<< " lowDate: " << t->dwLowDateTime
		<< " }";
}

void PrintTm(struct tm* t, std::stringstream &ss) {
	ss << "{"
		<< " tm_year: " << t->tm_year
		<< " tm_mon: " << t->tm_mon
		<< " tm_yday: " << t->tm_yday
		<< " tm_wday: " << t->tm_wday
		<< " tm_mday: " << t->tm_mday
		<< " tm_isdst: " << t->tm_isdst
		<< " tm_hour: " << t->tm_hour
		<< " tm_min: " << t->tm_min
		<< " tm_sec: " << t->tm_sec
		<< " }";
}

void PrintBinaryArray(const char* data, int dataLen, std::stringstream &ss)
{
	ss << "[";
	for (int i = 0; i < dataLen; i++)
		ss << " " << (unsigned int)(unsigned char)data[i];
	ss << "]";
}

void IEngExtCall::ensureArgsAreEqual(IEngExtCall* expect, bool strict, const char* callSource) {
	if (compareInputArgs(expect, strict)) return;

	std::string expectedSignature = expect->toString();
	std::string mySignature = toString();
	rehlds_syserror("%s: call arguments mismatch; expected signature:\n%s\n\ncurrent signature:\n%s", callSource, expectedSignature.c_str(), mySignature.c_str());
}

bool CompareSockAddrs(void* ps1, void* ps2) {
	sockaddr* sa1 = (sockaddr*)ps1;
	sockaddr* sa2 = (sockaddr*)ps2;

	if (sa1->sa_family != sa2->sa_family)
		return false;

	int compareSize = 0;
	switch (sa1->sa_family) {
	case AF_INET:
		compareSize = 8; // sa_family(2) + sin_addr(4) + sin_port(2)
		break;

	default:
		rehlds_syserror("%s: Unknown sockaddr family %u", __func__, sa1->sa_family);
	}

	return 0 == memcmp(ps1, ps2, compareSize);
}

/* ============================================================================
								 CSleepExtCall
============================================================================ */
CSleepExtCall::CSleepExtCall(DWORD time)
{
	m_Time = time;
}

std::string CSleepExtCall::toString()
{
	std::stringstream ss;
	ss << "Sleep(" << m_Time << ")";
	return ss.str();
}

bool CSleepExtCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSleepExtCall* otherSleep = dynamic_cast<CSleepExtCall*>(other);
	if (otherSleep == NULL)
		return false;

	return otherSleep->m_Time == this->m_Time;
}

void CSleepExtCall::writePrologue(std::ostream &stream)
{
	stream.write((char*)&m_Time, 4);
}

void CSleepExtCall::readPrologue(std::istream &stream)
{
	stream.read((char*)&m_Time, 4);
}



/* ============================================================================
							   CQueryPerfFreqCall
============================================================================ */
std::string CQueryPerfFreqCall::toString()
{
	LARGE_INTEGER li;
	li.QuadPart = m_Freq;

	std::stringstream ss;
	ss << "QueryPerformanceFrequency(freq = { LowPart: " << li.LowPart << "; HighPart: " << li.HighPart << ") => " << m_Res;
	return ss.str();
}

bool CQueryPerfFreqCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CQueryPerfFreqCall* otherPerfFreq = dynamic_cast<CQueryPerfFreqCall*>(other);
	if (otherPerfFreq == NULL)
		return false;

	return true;
}

void CQueryPerfFreqCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Freq, 8).write((char*)&m_Res, 1);
}

void CQueryPerfFreqCall::readEpilogue(std::istream &stream) {
	m_Res = 0;
	stream.read((char*)&m_Freq, 8).read((char*)&m_Res, 1);
}



/* ============================================================================
							  CQueryPerfCounterCall
============================================================================ */
std::string CQueryPerfCounterCall::toString()
{
	LARGE_INTEGER li;
	li.QuadPart = m_Counter;

	std::stringstream ss;
	ss << "QueryPerformanceCounter(counter = { LowPart: " << li.LowPart << "; HighPart: " << li.HighPart << ") => " << m_Res;
	return ss.str();
}

bool CQueryPerfCounterCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CQueryPerfCounterCall* otherPerfFreq = dynamic_cast<CQueryPerfCounterCall*>(other);
	if (otherPerfFreq == NULL)
		return false;

	return true;
}

void CQueryPerfCounterCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Counter, 8).write((char*)&m_Res, 1);
}

void CQueryPerfCounterCall::readEpilogue(std::istream &stream) {
	m_Res = 0;
	stream.read((char*)&m_Counter, 8).read((char*)&m_Res, 1);
}




/* ============================================================================
							   CGetTickCountCall
============================================================================ */
std::string CGetTickCountCall::toString()
{
	std::stringstream ss;
	ss << "GetTickCount() => " << m_Res;
	return ss.str();
}

bool CGetTickCountCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGetTickCountCall* otherCall = dynamic_cast<CGetTickCountCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CGetTickCountCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 4);
}

void CGetTickCountCall::readEpilogue(std::istream &stream) {
	m_Res = 0;
	stream.read((char*)&m_Res, 4);
}




/* ============================================================================
							 CGetTickCountCall
============================================================================ */

std::string CGetLocalTimeCall::toString()
{
	std::stringstream ss;
	ss << "GetLocalTime() => ";
	PrintSystemTime(&m_Res, ss);
	return ss.str();
}

bool CGetLocalTimeCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGetLocalTimeCall* otherCall = dynamic_cast<CGetLocalTimeCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CGetLocalTimeCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, sizeof(m_Res));
}

void CGetLocalTimeCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_Res, sizeof(m_Res));
}




/* ============================================================================
						CGetSystemTimeCall
============================================================================ */
std::string CGetSystemTimeCall::toString()
{
	std::stringstream ss;
	ss << "GetSystemTime() => ";
	PrintSystemTime(&m_Res, ss);
	return ss.str();
}

bool CGetSystemTimeCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGetSystemTimeCall* otherCall = dynamic_cast<CGetSystemTimeCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CGetSystemTimeCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, sizeof(m_Res));
}

void CGetSystemTimeCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_Res, sizeof(m_Res));
}




/* ============================================================================
							   CGetTimeZoneInfoCall
============================================================================ */
std::string CGetTimeZoneInfoCall::toString()
{
	std::stringstream ss;
	ss << "GetTimeZoneInfo() => {"
		<< " Bias: " << m_Res.Bias
		<< " StandardName: " << m_Res.StandardName
		<< " StandardDate: "; PrintSystemTime(&m_Res.StandardDate, ss);

	ss << " StandardBias: " << m_Res.StandardBias
		<< " DaylightName: " << m_Res.DaylightName
		<< " DaylightDate: "; PrintSystemTime(&m_Res.DaylightDate, ss);
	ss << " DaylightBias: " << m_Res.DaylightBias
		<< " }";
	return ss.str();
}

bool CGetTimeZoneInfoCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGetTimeZoneInfoCall* otherCall = dynamic_cast<CGetTimeZoneInfoCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CGetTimeZoneInfoCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, sizeof(m_Res));
}

void CGetTimeZoneInfoCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_Res, sizeof(m_Res));
}




/* ============================================================================
						 CSocketCall
============================================================================ */
CSocketCall::CSocketCall(int af, int type, int protocol)
{
	m_Af = af;
	m_Type = type;
	m_Protocol = protocol;
	m_Res = INVALID_SOCKET;
}

std::string CSocketCall::toString()
{
	std::stringstream ss;
	ss << "socket(af: " << m_Af << "; type: " << m_Type << "; protocol: " << m_Protocol << ") => " << m_Res;
	return ss.str();
}

bool CSocketCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSocketCall* otherCall = dynamic_cast<CSocketCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_Af != m_Af)
		return false;

	if (otherCall->m_Type != m_Type)
		return false;

	if (otherCall->m_Protocol != m_Protocol)
		return false;

	return true;
}

void CSocketCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_Af, 4).write((char*)&m_Type, 4).write((char*)&m_Protocol, 4);
}

void CSocketCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 4);
}

void CSocketCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_Af, 4).read((char*)&m_Type, 4).read((char*)&m_Protocol, 4);
}

void CSocketCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_Res, 4);
}




/* ============================================================================
							  CIoCtlSocketCall
============================================================================ */
CIoCtlSocketCall::CIoCtlSocketCall(SOCKET s, long cmd, u_long inValue)
{
	m_Socket = s;
	m_Cmd = cmd;
	m_InValue = inValue;
	m_OutValue = 0; m_Res = 0;
}

std::string CIoCtlSocketCall::toString()
{
	std::stringstream ss;
	ss << "ioctlsocket(s: " << m_Socket << "; cmd: " << m_Cmd << "; val: { in: " << m_InValue << "; out: " << m_OutValue << " }) => " << m_Res;
	return ss.str();
}

bool CIoCtlSocketCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CIoCtlSocketCall* otherCall = dynamic_cast<CIoCtlSocketCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_Socket != m_Socket)
		return false;

	if (otherCall->m_Cmd != m_Cmd)
		return false;

	if (otherCall->m_InValue != m_InValue)
		return false;

	return true;
}

void CIoCtlSocketCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_Socket, 4)
		.write((char*)&m_Cmd, 4)
		.write((char*)&m_InValue, 4);
}

void CIoCtlSocketCall::writeEpilogue(std::ostream &stream) {
	stream
		.write((char*)&m_OutValue, 4)
		.write((char*)&m_Res, 4);
}

void CIoCtlSocketCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_Socket, 4)
		.read((char*)&m_Cmd, 4)
		.read((char*)&m_InValue, 4);
}

void CIoCtlSocketCall::readEpilogue(std::istream &stream) {
	stream
		.read((char*)&m_OutValue, 4)
		.read((char*)&m_Res, 4);
}





/* ============================================================================
							   CSetSockOptCall
============================================================================ */
CSetSockOptCall::CSetSockOptCall(SOCKET s, int level, int optname, const char* optval, int optlen)
{
	if (optlen > sizeof(m_OptVal))
		rehlds_syserror("CSetSockOptCall(): too large optlen (%d max %d)", optlen, sizeof(m_OptVal));
	m_Socket = s;
	m_Level = level;
	m_OptName = optname;
	m_OptValLen = optlen;
	memcpy(m_OptVal, optval, m_OptValLen);
	m_Res = 0;
}

std::string CSetSockOptCall::toString()
{
	std::stringstream ss;
	ss << "setsockopt(s: " << m_Socket << "; level: " << m_Level << "; optName: " << m_OptName << "; optVal: ";
	PrintBinaryArray(m_OptVal, m_OptValLen, ss);
	ss << "; optLen: " << m_OptValLen << " ) => " << m_Res;
	return ss.str();
}

bool CSetSockOptCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSetSockOptCall* otherCall = dynamic_cast<CSetSockOptCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_Socket != m_Socket)
		return false;

	if (otherCall->m_Level != m_Level)
		return false;

	if (otherCall->m_OptName != m_OptName)
		return false;

	if (otherCall->m_OptValLen != m_OptValLen)
		return false;

	if (memcmp(otherCall->m_OptVal, m_OptVal, m_OptValLen))
		return false;

	return true;
}

void CSetSockOptCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_Socket, 4)
		.write((char*)&m_Level, 4)
		.write((char*)&m_OptName, 4)
		.write((char*)&m_OptValLen, 4)
		.write(m_OptVal, m_OptValLen);
}

void CSetSockOptCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 4);
}

void CSetSockOptCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_Socket, 4)
		.read((char*)&m_Level, 4)
		.read((char*)&m_OptName, 4)
		.read((char*)&m_OptValLen, 4);

	stream.read(m_OptVal, m_OptValLen);
}

void CSetSockOptCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_Res, 4);
}




/* ============================================================================
							   CCloseSocketCall
============================================================================ */
CCloseSocketCall::CCloseSocketCall(SOCKET s)
{
	m_Socket = s;
	m_Res = 0;
}

std::string CCloseSocketCall::toString()
{
	std::stringstream ss;
	ss << "closesocket(s: " << m_Socket << " ) => " << m_Res;
	return ss.str();
}

bool CCloseSocketCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CCloseSocketCall* otherCall = dynamic_cast<CCloseSocketCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_Socket != m_Socket)
		return false;

	return true;
}

void CCloseSocketCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_Socket, 4);
}

void CCloseSocketCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 4);
}

void CCloseSocketCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_Socket, 4);
}

void CCloseSocketCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_Res, 4);
}


/* ============================================================================
							 CRecvFromCall
============================================================================ */
CRecvFromCall::CRecvFromCall(SOCKET s, int len, int flags, int fromlen)
{
	m_Socket = s;
	m_Len = len;
	m_Flags = flags;
	m_FromLenIn = fromlen;
	m_FromLenOut = 0; m_Res = -1;
}

std::string CRecvFromCall::toString()
{
	std::stringstream ss;
	ss << "recvfrom(s: " << m_Socket << "; data: ";
	PrintBinaryArray(m_Data, m_Res, ss);
	ss << "; len: " << m_Len << "; flags: " << m_Flags << "; from: ";
	PrintBinaryArray(m_From, m_FromLenOut, ss);
	ss << "; fromlen: { in: " << m_FromLenIn << "; out: " << m_FromLenOut << " } ) => " << m_Res;
	return ss.str();
}

bool CRecvFromCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CRecvFromCall* otherCall = dynamic_cast<CRecvFromCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_Socket != m_Socket)
		return false;

	if (otherCall->m_Len != m_Len)
		return false;

	if (otherCall->m_Flags != m_Flags)
		return false;

	if (otherCall->m_FromLenIn != m_FromLenIn)
		return false;

	return true;
}

void CRecvFromCall::setResult(const void* data, const void* from, int fromLen, int res)
{
	if (res > 0 && res > sizeof(m_Data))
		rehlds_syserror("%s: too large datalen (%d max %d)", __func__, res, sizeof(m_Data));

	if (fromLen > sizeof(m_From))
		rehlds_syserror("%s: too large fromlen (%d max %d)", __func__, res, sizeof(m_From));

	m_FromLenOut = fromLen;
	m_Res = res;

	if (res > 0)
		memcpy(m_Data, data, res);

	if (fromLen > 0)
		memcpy(m_From, from, fromLen);
}

void CRecvFromCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_Socket, 4)
		.write((char*)&m_Len, 4)
		.write((char*)&m_Flags, 4)
		.write((char*)&m_FromLenIn, 4);
}

void CRecvFromCall::writeEpilogue(std::ostream &stream) {
	stream
		.write((char*)&m_FromLenOut, 4)
		.write((char*)&m_Res, 4)
		.write((char*)&m_From, (m_FromLenOut > 0 && m_Res >= 0) ? m_FromLenOut : 0)
		.write((char*)&m_Data, (m_Res > 0) ? m_Res : 0);
}

void CRecvFromCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_Socket, 4)
		.read((char*)&m_Len, 4)
		.read((char*)&m_Flags, 4)
		.read((char*)&m_FromLenIn, 4);
}

void CRecvFromCall::readEpilogue(std::istream &stream) {
	stream
		.read((char*)&m_FromLenOut, 4)
		.read((char*)&m_Res, 4);

	stream.read((char*)&m_From, (m_FromLenOut > 0 && m_Res >= 0) ? m_FromLenOut : 0)
		.read((char*)&m_Data, (m_Res > 0) ? m_Res : 0);
}





/* ============================================================================
							  CSendToCall
============================================================================ */
CSendToCall::CSendToCall(SOCKET s, const void* buf, int len, int flags, const void* to, int tolen)
{
	if (len > sizeof(m_Data))
		rehlds_syserror("%s: too large datalen (%d max %d)", __func__, len, sizeof(m_Data));

	if (tolen > sizeof(m_To))
		rehlds_syserror("%s: too large tolen (%d max %d)", __func__, tolen, sizeof(m_To));

	m_Socket = s;
	m_Len = len;
	m_Flags = flags;
	m_ToLen = tolen;

	memcpy(m_To, to, m_ToLen);
	memcpy(m_Data, buf, m_Len);
	m_Res = 0;
}

std::string CSendToCall::toString()
{
	std::stringstream ss;
	ss << "sendto(s: " << m_Socket << "; data: ";
	PrintBinaryArray(m_Data, m_Len, ss);
	ss << "; len: " << m_Len << "; flags: " << m_Flags << "; to: ";
	PrintBinaryArray(m_To, m_ToLen, ss);
	ss << "; tolen: " << m_ToLen << " ) => " << m_Res;
	return ss.str();
}

bool CSendToCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSendToCall* otherCall = dynamic_cast<CSendToCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_Socket != m_Socket)
		return false;

	if (strict) {
		if (otherCall->m_Len != m_Len)
			return false;
	}
	else {
		int maxDiff;
		if (m_Len < 40) {
			maxDiff = 10;
		}
		else if (m_Len < 90) {
			maxDiff = 15;
		}
		else if (m_Len < 120) {
			maxDiff = 18;
		}
		else {
			maxDiff = m_Len / 8;
		}
		if (abs(otherCall->m_Len - m_Len) > maxDiff)
			return false;
	}

	if (otherCall->m_Flags != m_Flags)
		return false;

	if (otherCall->m_ToLen != m_ToLen)
		return false;

	if (strict) {
		if (memcmp(otherCall->m_Data, m_Data, m_Len))
			return false;
	}

	if (!CompareSockAddrs(otherCall->m_To, m_To))
		return false;

	return true;
}

void CSendToCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_Socket, 4)
		.write((char*)&m_Len, 4)
		.write((char*)&m_Flags, 4)
		.write((char*)&m_ToLen, 4)
		.write((char*)&m_To, m_ToLen)
		.write((char*)&m_Data, m_Len);
}

void CSendToCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 4);
}

void CSendToCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_Socket, 4)
		.read((char*)&m_Len, 4)
		.read((char*)&m_Flags, 4)
		.read((char*)&m_ToLen, 4);

	stream.read((char*)&m_To, m_ToLen)
		.read((char*)&m_Data, m_Len);
}

void CSendToCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_Res, 4);
}





/* ============================================================================
									CBindCall
============================================================================ */
CBindCall::CBindCall(SOCKET s, const void* addr, int addrlen)
{
	if (addrlen > sizeof(m_Addr))
		rehlds_syserror("%s: too large tolen (%d max %d)", __func__, addrlen, sizeof(m_Addr));

	m_Socket = s;
	m_AddrLen = addrlen;

	memcpy(m_Addr, addr, addrlen);
	m_Res = 0;
}

std::string CBindCall::toString()
{
	std::stringstream ss;
	ss << "bind(s: " << m_Socket << "; addr: ";
	PrintBinaryArray(m_Addr, m_AddrLen, ss);
	ss << "; addrlen: " << m_AddrLen << " ) => " << m_Res;
	return ss.str();
}

bool CBindCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CBindCall* otherCall = dynamic_cast<CBindCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_Socket != m_Socket)
		return false;

	if (otherCall->m_AddrLen != m_AddrLen)
		return false;

	if (!CompareSockAddrs(otherCall->m_Addr, m_Addr))
		return false;

	return true;
}

void CBindCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_Socket, 4)
		.write((char*)&m_AddrLen, 4)
		.write((char*)&m_Addr, m_AddrLen);
}

void CBindCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 4);
}

void CBindCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_Socket, 4)
		.read((char*)&m_AddrLen, 4);

	stream.read((char*)&m_Addr, m_AddrLen);
}

void CBindCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_Res, 4);
}




/* ============================================================================
						  CGetSockNameCall
============================================================================ */
CGetSockNameCall::CGetSockNameCall(SOCKET s, int addrlen)
{
	m_Socket = s;
	m_AddrLenIn = addrlen;
	m_AddrLenOut = m_Res = 0;
}

std::string CGetSockNameCall::toString()
{
	std::stringstream ss;
	ss << "getsockname(s: " << m_Socket << "; addr: ";
	PrintBinaryArray(m_Addr, m_AddrLenOut, ss);
	ss << "; addrlen: { in: " << m_AddrLenIn << "; out: " << m_AddrLenOut << " } ) = > " << m_Res;
	return ss.str();
}

void CGetSockNameCall::setResult(const void* addr, int addrlen, int res)
{
	if (addrlen > sizeof(m_Addr))
		rehlds_syserror("%s: too large tolen (%d max %d)", __func__, addrlen, sizeof(m_Addr));

	m_Res = res;
	m_AddrLenOut = addrlen;

	memcpy(m_Addr, addr, addrlen);
}

bool CGetSockNameCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGetSockNameCall* otherCall = dynamic_cast<CGetSockNameCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_Socket != m_Socket)
		return false;

	if (otherCall->m_AddrLenIn != m_AddrLenIn)
		return false;

	return true;
}

void CGetSockNameCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_Socket, 4)
		.write((char*)&m_AddrLenIn, 4);
}

void CGetSockNameCall::writeEpilogue(std::ostream &stream) {
	stream
		.write((char*)&m_AddrLenOut, 4)
		.write((char*)&m_Res, 4)
		.write((char*)&m_Addr, m_AddrLenOut);
}

void CGetSockNameCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_Socket, 4)
		.read((char*)&m_AddrLenIn, 4);
}

void CGetSockNameCall::readEpilogue(std::istream &stream) {
	stream
		.read((char*)&m_AddrLenOut, 4)
		.read((char*)&m_Res, 4);

	stream.read((char*)&m_Addr, m_AddrLenOut);

}






/* ============================================================================
						   CWSAGetLastErrorCall
============================================================================ */
std::string CWSAGetLastErrorCall::toString()
{
	std::stringstream ss;
	ss << "WSAGetLastError() = > " << m_Res;
	return ss.str();
}

bool CWSAGetLastErrorCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CWSAGetLastErrorCall* otherCall = (CWSAGetLastErrorCall*)other;
	if (otherCall == NULL)
		return false;

	return true;
}

void CWSAGetLastErrorCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 4);
}

void CWSAGetLastErrorCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_Res, 4);
}




/* ============================================================================
							  CSteamCallbackCall1
============================================================================ */
CSteamCallbackCall1::CSteamCallbackCall1(int cbId, void* data, int dataSize, CCallbackBase* cb)
{
	if (dataSize > sizeof(m_Data))
		rehlds_syserror("%s: too large data (%d, max %d)", __func__, dataSize, sizeof(m_Data));

	m_CallbackId = cbId;
	m_DataSize = dataSize;
	memcpy(m_Data, data, dataSize);
	m_InState.m_iCallback = cb->GetICallback();
	m_InState.m_nCallbackFlags = cb->GetFlags();
	m_OutState.clear();
}

std::string CSteamCallbackCall1::toString()
{
	std::stringstream ss;
	ss << "Steam_Callback1: { callbackId: " << m_CallbackId << "; data: ";
	PrintBinaryArray(m_Data, m_DataSize, ss);
	ss << "; dataSize: " << m_DataSize << " }";
	return ss.str();
}

bool CSteamCallbackCall1::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamCallbackCall1* otherCall = dynamic_cast<CSteamCallbackCall1*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_InState.m_iCallback != m_InState.m_iCallback) return false;
	if (otherCall->m_InState.m_nCallbackFlags != m_InState.m_nCallbackFlags) return false;

	return true;
}

void CSteamCallbackCall1::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_CallbackId, 4)
		.write((char*)&m_InState, sizeof(m_InState))
		.write((char*)&m_DataSize, 4)
		.write(m_Data, m_DataSize);
}

void CSteamCallbackCall1::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_CallbackId, 4)
		.read((char*)&m_InState, sizeof(m_InState))
		.read((char*)&m_DataSize, 4);

	stream.read(m_Data, m_DataSize);
}

void CSteamCallbackCall1::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_OutState, sizeof(m_OutState));
}

void CSteamCallbackCall1::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_OutState, sizeof(m_OutState));
}



/* ============================================================================
								 CSteamCallbackCall2
============================================================================ */
CSteamCallbackCall2::CSteamCallbackCall2(int cbId, void* data, int dataSize, bool ioFailure, SteamAPICall_t apiCall, CCallbackBase* cb)
{
	if (dataSize > sizeof(m_Data))
		rehlds_syserror("%s: too large data (%d, max %d)", __func__, dataSize, sizeof(m_Data));

	m_CallbackId = cbId;
	m_DataSize = dataSize;
	memcpy(m_Data, data, dataSize);
	m_bIOFailure = ioFailure;
	m_SteamAPICall = apiCall;
	m_InState.m_iCallback = cb->GetICallback();
	m_InState.m_nCallbackFlags = cb->GetFlags();
	m_OutState.clear();
}

std::string CSteamCallbackCall2::toString()
{
	std::stringstream ss;
	ss << "Steam_Callback1: { callbackId: " << m_CallbackId << "; data: ";
	PrintBinaryArray(m_Data, m_DataSize, ss);
	ss << "; dataSize: " << m_DataSize << "; ioFailure: " << m_bIOFailure << "; steamApiCall: " << m_SteamAPICall << " }";
	return ss.str();
}

bool CSteamCallbackCall2::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamCallbackCall2* otherCall = dynamic_cast<CSteamCallbackCall2*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_InState.m_iCallback != m_InState.m_iCallback) return false;
	if (otherCall->m_InState.m_nCallbackFlags != m_InState.m_nCallbackFlags) return false;

	return true;
}

void CSteamCallbackCall2::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_CallbackId, 4)
		.write((char*)&m_InState, sizeof(m_InState))
		.write((char*)&m_DataSize, 4)
		.write((char*)&m_bIOFailure, 1)
		.write((char*)&m_SteamAPICall, sizeof(m_SteamAPICall))
		.write(m_Data, m_DataSize);
}

void CSteamCallbackCall2::readPrologue(std::istream &stream) {
	m_bIOFailure = false;
	stream
		.read((char*)&m_CallbackId, 4)
		.read((char*)&m_InState, sizeof(m_InState))
		.read((char*)&m_DataSize, 4)
		.read((char*)&m_bIOFailure, 1)
		.read((char*)&m_SteamAPICall, sizeof(m_SteamAPICall));

	stream.read(m_Data, m_DataSize);
}

void CSteamCallbackCall2::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_OutState, sizeof(m_OutState));
}

void CSteamCallbackCall2::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_OutState, sizeof(m_OutState));
}

/* ============================================================================
						 CSteamApiRegisterCallbackCall
============================================================================ */
CSteamApiRegisterCallbackCall::CSteamApiRegisterCallbackCall(int rehldsCallbackId, int steamCallbackId, CCallbackBase* cb)
{
	m_RehldsCallbackId = rehldsCallbackId;
	m_iSteamCallbackId = steamCallbackId;
	m_InState.m_iCallback = cb->GetICallback();
	m_InState.m_nCallbackFlags = cb->GetFlags();
	m_OutState.clear();
}

std::string CSteamApiRegisterCallbackCall::toString()
{
	std::stringstream ss;
	ss << "SteamAPI_RegisterCallback( rehldsCallbackId: " << m_RehldsCallbackId << "; steamCallbackId: " << m_iSteamCallbackId << " )";
	return ss.str();
}

bool CSteamApiRegisterCallbackCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamApiRegisterCallbackCall* otherCall = dynamic_cast<CSteamApiRegisterCallbackCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_iSteamCallbackId != m_iSteamCallbackId)
		return false;

	if (otherCall->m_RehldsCallbackId != m_RehldsCallbackId)
		return false;

	if (otherCall->m_InState.m_iCallback != m_InState.m_iCallback) return false;
	if (otherCall->m_InState.m_nCallbackFlags != m_InState.m_nCallbackFlags) return false;

	return true;
}

void CSteamApiRegisterCallbackCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_InState, sizeof(m_InState))
		.write((char*)&m_RehldsCallbackId, 4)
		.write((char*)&m_iSteamCallbackId, 4);
}

void CSteamApiRegisterCallbackCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_InState, sizeof(m_InState))
		.read((char*)&m_RehldsCallbackId, 4)
		.read((char*)&m_iSteamCallbackId, 4);
}

void CSteamApiRegisterCallbackCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_OutState, sizeof(m_OutState));
}

void CSteamApiRegisterCallbackCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_OutState, sizeof(m_OutState));
}




/* ============================================================================
						 CSteamApiInitCall
============================================================================ */

std::string CSteamApiInitCall::toString()
{
	std::stringstream ss;
	ss << "CSteamApiInitCall( ) => " << m_Res;
	return ss.str();
}

bool CSteamApiInitCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamApiInitCall* otherCall = dynamic_cast<CSteamApiInitCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CSteamApiInitCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 1);
}

void CSteamApiInitCall::readEpilogue(std::istream &stream) {
	m_Res = false;
	stream.read((char*)&m_Res, 1);
}





/* ============================================================================
				   CSteamApiUnrigestierCallResultCall
============================================================================ */
CSteamApiUnrigestierCallResultCall::CSteamApiUnrigestierCallResultCall(int rehldsCallbackId, SteamAPICall_t steamApiCall, CCallbackBase* cb)
{
	m_RehldsCallbackId = rehldsCallbackId;
	m_SteamApiCall = steamApiCall;
	m_InState.m_iCallback = cb->GetICallback();
	m_InState.m_nCallbackFlags = cb->GetFlags();
	m_OutState.clear();
}

std::string CSteamApiUnrigestierCallResultCall::toString()
{
	std::stringstream ss;
	ss << "SteamAPI_UnregisterCallResult( rehldsCallbackId: " << m_RehldsCallbackId << "; steamApiCall: " << m_SteamApiCall << " )";
	return ss.str();
}

bool CSteamApiUnrigestierCallResultCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamApiUnrigestierCallResultCall* otherCall = dynamic_cast<CSteamApiUnrigestierCallResultCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_SteamApiCall != m_SteamApiCall)
		return false;

	if (otherCall->m_RehldsCallbackId != m_RehldsCallbackId)
		return false;

	if (otherCall->m_InState.m_iCallback != m_InState.m_iCallback) return false;
	if (otherCall->m_InState.m_nCallbackFlags != m_InState.m_nCallbackFlags) return false;

	return true;
}

void CSteamApiUnrigestierCallResultCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_InState, sizeof(m_InState))
		.write((char*)&m_RehldsCallbackId, 4)
		.write((char*)&m_SteamApiCall, sizeof(m_SteamApiCall));
}

void CSteamApiUnrigestierCallResultCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_InState, sizeof(m_InState))
		.read((char*)&m_RehldsCallbackId, 4)
		.read((char*)&m_SteamApiCall, sizeof(m_SteamApiCall));
}

void CSteamApiUnrigestierCallResultCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_OutState, sizeof(m_OutState));
}

void CSteamApiUnrigestierCallResultCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_OutState, sizeof(m_OutState));
}



/* ============================================================================
							  CSteamAppsCall
============================================================================ */
std::string CSteamAppsCall::toString()
{
	std::stringstream ss;
	ss << "SteamApps( )";
	return ss.str();
}

bool CSteamAppsCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamAppsCall* otherCall = dynamic_cast<CSteamAppsCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CSteamAppsCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_ReturnNull, 1);
}

void CSteamAppsCall::readEpilogue(std::istream &stream) {
	m_ReturnNull = false;
	stream.read((char*)&m_ReturnNull, 1);
}




/* ============================================================================
					   CSteamAppGetCurrentGameLanguageCall
============================================================================ */

std::string CSteamAppGetCurrentGameLanguageCall::toString()
{
	std::stringstream ss;
	ss << "SteamApps::GetCurrentGameLanguage( ) => " << m_Res;
	return ss.str();
}

void CSteamAppGetCurrentGameLanguageCall::setResult(const char* res)
{
	if (res == NULL)
		rehlds_syserror("%s: null result", __func__);

	m_ResLen = strlen(res) + 1;
	if (m_ResLen > sizeof(m_Res))
		rehlds_syserror("%s: too large result", __func__);

	memcpy(m_Res, res, m_ResLen);
}

bool CSteamAppGetCurrentGameLanguageCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamAppGetCurrentGameLanguageCall* otherCall = dynamic_cast<CSteamAppGetCurrentGameLanguageCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CSteamAppGetCurrentGameLanguageCall::writeEpilogue(std::ostream &stream) {
	stream
		.write((char*)&m_ResLen, 4)
		.write((char*)&m_Res, m_ResLen);
}

void CSteamAppGetCurrentGameLanguageCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_ResLen, 4);
	stream.read((char*)&m_Res, m_ResLen);
}





/* ============================================================================
					CSteamGameServerInitCall
============================================================================ */
CSteamGameServerInitCall::CSteamGameServerInitCall(uint32 unIP, uint16 usSteamPort, uint16 usGamePort, uint16 usQueryPort, EServerMode eServerMode, const char *pchVersionString)
{
	if (pchVersionString == NULL)
		rehlds_syserror("%s: version is null", __func__);

	m_VersionLen = strlen(pchVersionString) + 1;
	if (m_VersionLen > sizeof(m_Version))
		rehlds_syserror("%s: too long version string", __func__);

	memcpy(m_Version, pchVersionString, m_VersionLen);
	m_IP = unIP;
	m_SteamPort = usSteamPort;
	m_GamePort = usGamePort;
	m_QueryPort = usQueryPort;
	m_ServerMode = eServerMode;
	m_Res = false;
}

std::string CSteamGameServerInitCall::toString()
{
	std::stringstream ss;
	ss << "SteamGameServer_Init( unIP: " << m_IP << "; usSteamPort: " << m_SteamPort
		<< "; usQueryPort: " << m_QueryPort << "; eServerMode: " << m_ServerMode
		<< "; pchVersionString: " << m_Version << " ) => " << m_Res;
	return ss.str();
}

bool CSteamGameServerInitCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamGameServerInitCall* otherCall = dynamic_cast<CSteamGameServerInitCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_VersionLen != m_VersionLen)
		return false;

	if (strcmp(otherCall->m_Version, m_Version))
		return false;

	if (otherCall->m_IP != m_IP)
		return false;

	if (otherCall->m_SteamPort != m_SteamPort)
		return false;

	if (otherCall->m_GamePort != m_GamePort)
		return false;

	if (otherCall->m_QueryPort != m_QueryPort)
		return false;

	if (otherCall->m_ServerMode != m_ServerMode)
		return false;

	return true;
}

void CSteamGameServerInitCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_IP, sizeof(m_IP))
		.write((char*)&m_SteamPort, sizeof(m_SteamPort))
		.write((char*)&m_GamePort, sizeof(m_GamePort))
		.write((char*)&m_QueryPort, sizeof(m_QueryPort))
		.write((char*)&m_ServerMode, sizeof(m_ServerMode))
		.write((char*)&m_VersionLen, sizeof(m_VersionLen))
		.write(m_Version, m_VersionLen);
}

void CSteamGameServerInitCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 1);
}

void CSteamGameServerInitCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_IP, sizeof(m_IP))
		.read((char*)&m_SteamPort, sizeof(m_SteamPort))
		.read((char*)&m_GamePort, sizeof(m_GamePort))
		.read((char*)&m_QueryPort, sizeof(m_QueryPort))
		.read((char*)&m_ServerMode, sizeof(m_ServerMode))
		.read((char*)&m_VersionLen, sizeof(m_VersionLen));
	stream.read(m_Version, m_VersionLen);
}

void CSteamGameServerInitCall::readEpilogue(std::istream &stream) {
	m_Res = false;
	stream.read((char*)&m_Res, 1);
}





/* ============================================================================
					 CSteamGameServerCall
============================================================================ */
std::string CSteamGameServerCall::toString()
{
	std::stringstream ss;
	ss << "SteamGameServer( )";
	return ss.str();
}

bool CSteamGameServerCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamGameServerCall* otherCall = dynamic_cast<CSteamGameServerCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CSteamGameServerCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_ReturnNull, 1);
}

void CSteamGameServerCall::readEpilogue(std::istream &stream) {
	m_ReturnNull = false;
	stream.read((char*)&m_ReturnNull, 1);
}





/* ============================================================================
				 CGameServerSetProductCall
============================================================================ */
CGameServerSetProductCall::CGameServerSetProductCall(const char* product)
{
	if (product == NULL)
		rehlds_syserror("%s: product is null", __func__);

	m_ProductLen = strlen(product) + 1;
	if (m_ProductLen > sizeof(m_Product))
		rehlds_syserror("%s: too long product string", __func__);

	memcpy(m_Product, product, m_ProductLen);
}

std::string CGameServerSetProductCall::toString()
{
	std::stringstream ss;
	ss << "GS::SetProduct( product: " << m_Product << " ) ";
	return ss.str();
}

bool CGameServerSetProductCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerSetProductCall* otherCall = dynamic_cast<CGameServerSetProductCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_ProductLen != m_ProductLen)
		return false;

	if (strcmp(otherCall->m_Product, m_Product))
		return false;

	return true;
}

void CGameServerSetProductCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_ProductLen, sizeof(m_ProductLen))
		.write(m_Product, m_ProductLen);
}

void CGameServerSetProductCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_ProductLen, sizeof(m_ProductLen));
	stream.read(m_Product, m_ProductLen);
}





/* ============================================================================
					 CGameServerSetModDirCall
============================================================================ */
CGameServerSetModDirCall::CGameServerSetModDirCall(const char* dir)
{
	if (dir == NULL)
		rehlds_syserror("%s: dir is null", __func__);

	m_DirLen = strlen(dir) + 1;
	if (m_DirLen > sizeof(m_Dir))
		rehlds_syserror("%s: too long dir string", __func__);

	memcpy(m_Dir, dir, m_DirLen);
}

std::string CGameServerSetModDirCall::toString()
{
	std::stringstream ss;
	ss << "GS::SetGameDir( gamedir: " << m_Dir << " ) ";
	return ss.str();
}

bool CGameServerSetModDirCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerSetModDirCall* otherCall = dynamic_cast<CGameServerSetModDirCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_DirLen != m_DirLen)
		return false;

	if (strcmp(otherCall->m_Dir, m_Dir))
		return false;

	return true;
}

void CGameServerSetModDirCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_DirLen, sizeof(m_DirLen))
		.write(m_Dir, m_DirLen);
}

void CGameServerSetModDirCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_DirLen, sizeof(m_DirLen));
	stream.read(m_Dir, m_DirLen);
}





/* ============================================================================
				 CGameServerSetDedicatedServerCall
============================================================================ */
CGameServerSetDedicatedServerCall::CGameServerSetDedicatedServerCall(bool dedicated)
{
	m_Dedicated = dedicated;
}

std::string CGameServerSetDedicatedServerCall::toString()
{
	std::stringstream ss;
	ss << "GS::SetDedicatedServer( dedicated: " << m_Dedicated << " ) ";
	return ss.str();
}

bool CGameServerSetDedicatedServerCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerSetDedicatedServerCall* otherCall = dynamic_cast<CGameServerSetDedicatedServerCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_Dedicated != m_Dedicated)
		return false;

	return true;
}

void CGameServerSetDedicatedServerCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_Dedicated, 1);
}

void CGameServerSetDedicatedServerCall::readPrologue(std::istream &stream) {
	m_Dedicated = false;
	stream.read((char*)&m_Dedicated, 1);
}





/* ============================================================================
				   CGameServerSetGameDescCall
============================================================================ */
CGameServerSetGameDescCall::CGameServerSetGameDescCall(const char* desc)
{
	if (desc == NULL)
		rehlds_syserror("%s: desc is null", __func__);

	m_DescLen = strlen(desc) + 1;
	if (m_DescLen > sizeof(m_Desc))
		rehlds_syserror("%s: too long dir string", __func__);

	memcpy(m_Desc, desc, m_DescLen);
}

std::string CGameServerSetGameDescCall::toString()
{
	std::stringstream ss;
	ss << "GS::SetGameDescription( description: " << m_Desc << " ) ";
	return ss.str();
}

bool CGameServerSetGameDescCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerSetGameDescCall* otherCall = dynamic_cast<CGameServerSetGameDescCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_DescLen != m_DescLen)
		return false;

	if (strcmp(otherCall->m_Desc, m_Desc))
		return false;

	return true;
}

void CGameServerSetGameDescCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_DescLen, sizeof(m_DescLen))
		.write(m_Desc, m_DescLen);
}

void CGameServerSetGameDescCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_DescLen, sizeof(m_DescLen));
	stream.read(m_Desc, m_DescLen);
}




/* ============================================================================
					  CGameServerLogOnAnonymousCall
============================================================================ */
std::string CGameServerLogOnAnonymousCall::toString()
{
	std::stringstream ss;
	ss << "GS::LogOnAnonymous( ) ";
	return ss.str();
}

bool CGameServerLogOnAnonymousCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerLogOnAnonymousCall* otherCall = dynamic_cast<CGameServerLogOnAnonymousCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}





/* ============================================================================
						  CGameServerEnableHeartbeatsCall
============================================================================ */
CGameServerEnableHeartbeatsCall::CGameServerEnableHeartbeatsCall(bool hearbeats)
{
	m_Heartbeats = hearbeats;
}

std::string CGameServerEnableHeartbeatsCall::toString()
{
	std::stringstream ss;
	ss << "GS::EnableHeartbeats( active: " << m_Heartbeats << " ) ";
	return ss.str();
}

bool CGameServerEnableHeartbeatsCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerEnableHeartbeatsCall* otherCall = dynamic_cast<CGameServerEnableHeartbeatsCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_Heartbeats != m_Heartbeats)
		return false;

	return true;
}

void CGameServerEnableHeartbeatsCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_Heartbeats, 1);
}

void CGameServerEnableHeartbeatsCall::readPrologue(std::istream &stream) {
	m_Heartbeats = false;
	stream.read((char*)&m_Heartbeats, 1);
}





/* ============================================================================
				  CGameServerSetHeartbeatIntervalCall
============================================================================ */
CGameServerSetHeartbeatIntervalCall::CGameServerSetHeartbeatIntervalCall(int interval)
{
	m_Interval = interval;
}

std::string CGameServerSetHeartbeatIntervalCall::toString()
{
	std::stringstream ss;
	ss << "GS::SetHeartbeatInterval( interval: " << m_Interval << " ) ";
	return ss.str();
}

bool CGameServerSetHeartbeatIntervalCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerSetHeartbeatIntervalCall* otherCall = dynamic_cast<CGameServerSetHeartbeatIntervalCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_Interval != m_Interval)
		return false;

	return true;
}

void CGameServerSetHeartbeatIntervalCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_Interval, 4);
}

void CGameServerSetHeartbeatIntervalCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_Interval, 4);
}




/* ============================================================================
					   CGameServerSetMaxPlayersCall
============================================================================ */
CGameServerSetMaxPlayersCall::CGameServerSetMaxPlayersCall(int maxPlayers)
{
	m_MaxPlayers = maxPlayers;
}

std::string CGameServerSetMaxPlayersCall::toString()
{
	std::stringstream ss;
	ss << "GS::ServerSetMaxPlayersCount( count: " << m_MaxPlayers << " ) ";
	return ss.str();
}

bool CGameServerSetMaxPlayersCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerSetMaxPlayersCall* otherCall = dynamic_cast<CGameServerSetMaxPlayersCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_MaxPlayers != m_MaxPlayers)
		return false;

	return true;
}

void CGameServerSetMaxPlayersCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_MaxPlayers, 4);
}

void CGameServerSetMaxPlayersCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_MaxPlayers, 4);
}





/* ============================================================================
					   CGameServerSetBotCountCall
============================================================================ */
CGameServerSetBotCountCall::CGameServerSetBotCountCall(int numBots)
{
	m_NumBots = numBots;
}

std::string CGameServerSetBotCountCall::toString()
{
	std::stringstream ss;
	ss << "GS::SetBotCount( count: " << m_NumBots << " ) ";
	return ss.str();
}

bool CGameServerSetBotCountCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerSetBotCountCall* otherCall = dynamic_cast<CGameServerSetBotCountCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_NumBots != m_NumBots)
		return false;

	return true;
}

void CGameServerSetBotCountCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_NumBots, 4);
}

void CGameServerSetBotCountCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_NumBots, 4);
}





/* ============================================================================
					CGameServerSetServerNameCall
============================================================================ */
CGameServerSetServerNameCall::CGameServerSetServerNameCall(const char* serverName)
{
	if (serverName == NULL)
		rehlds_syserror("%s: serverName is null", __func__);

	m_ServerNameLen = strlen(serverName) + 1;
	if (m_ServerNameLen > sizeof(m_ServerName))
		rehlds_syserror("%s: too long serverName string", __func__);

	memcpy(m_ServerName, serverName, m_ServerNameLen);
}

std::string CGameServerSetServerNameCall::toString()
{
	std::stringstream ss;
	ss << "GS::SetServerName( serverName: " << m_ServerName << " ) ";
	return ss.str();
}

bool CGameServerSetServerNameCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerSetServerNameCall* otherCall = dynamic_cast<CGameServerSetServerNameCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_ServerNameLen != m_ServerNameLen)
		return false;

	if (strcmp(otherCall->m_ServerName, m_ServerName))
		return false;

	return true;
}

void CGameServerSetServerNameCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_ServerNameLen, sizeof(m_ServerNameLen))
		.write(m_ServerName, m_ServerNameLen);
}

void CGameServerSetServerNameCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_ServerNameLen, sizeof(m_ServerNameLen));
	stream.read(m_ServerName, m_ServerNameLen);
}





/* ============================================================================
					 CGameServerSetMapNameCall
============================================================================ */
CGameServerSetMapNameCall::CGameServerSetMapNameCall(const char* mapName)
{
	if (mapName == NULL)
		rehlds_syserror("%s: serverName is null", __func__);

	m_MapNameLen = strlen(mapName) + 1;
	if (m_MapNameLen > sizeof(m_MapName))
		rehlds_syserror("%s: too long mapName string", __func__);

	memcpy(m_MapName, mapName, m_MapNameLen);
}

std::string CGameServerSetMapNameCall::toString()
{
	std::stringstream ss;
	ss << "GS::SetMapName( serverName: " << m_MapName << " ) ";
	return ss.str();
}

bool CGameServerSetMapNameCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerSetMapNameCall* otherCall = dynamic_cast<CGameServerSetMapNameCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_MapNameLen != m_MapNameLen)
		return false;

	if (strcmp(otherCall->m_MapName, m_MapName))
		return false;

	return true;
}

void CGameServerSetMapNameCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_MapNameLen, sizeof(m_MapNameLen))
		.write(m_MapName, m_MapNameLen);
}

void CGameServerSetMapNameCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_MapNameLen, sizeof(m_MapNameLen));
	stream.read(m_MapName, m_MapNameLen);
}





/* ============================================================================
				  CGameServerSetPasswordProtectedCall
============================================================================ */
CGameServerSetPasswordProtectedCall::CGameServerSetPasswordProtectedCall(bool passwordProtected)
{
	m_PasswordProtected = passwordProtected;
}

std::string CGameServerSetPasswordProtectedCall::toString()
{
	std::stringstream ss;
	ss << "GS::SetPasswordProtected( protected: " << m_PasswordProtected << " ) ";
	return ss.str();
}

bool CGameServerSetPasswordProtectedCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerSetPasswordProtectedCall* otherCall = dynamic_cast<CGameServerSetPasswordProtectedCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_PasswordProtected != m_PasswordProtected)
		return false;

	return true;
}

void CGameServerSetPasswordProtectedCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_PasswordProtected, 1);
}

void CGameServerSetPasswordProtectedCall::readPrologue(std::istream &stream) {
	m_PasswordProtected = false;
	stream.read((char*)&m_PasswordProtected, 1);
}





/* ============================================================================
					   CGameServerClearAllKVsCall
============================================================================ */
std::string CGameServerClearAllKVsCall::toString()
{
	std::stringstream ss;
	ss << "GS::ClearAllKeyValues( ) ";
	return ss.str();
}

bool CGameServerClearAllKVsCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerClearAllKVsCall* otherCall = dynamic_cast<CGameServerClearAllKVsCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}





/* ============================================================================
					  CGameServerSetKeyValueCall
============================================================================ */
CGameServerSetKeyValueCall::CGameServerSetKeyValueCall(const char* key, const char* value)
{
	if (key == NULL)
		rehlds_syserror("%s: key is null", __func__);

	m_KeyLen = strlen(key) + 1;
	if (m_KeyLen > sizeof(m_Key))
		rehlds_syserror("%s: too long key string", __func__);

	memcpy(m_Key, key, m_KeyLen);

	if (value == NULL)
		rehlds_syserror("%s: value is null", __func__);

	m_ValueLen = strlen(value) + 1;
	if (m_ValueLen > sizeof(m_Value))
		rehlds_syserror("%s: too long value string", __func__);

	memcpy(m_Value, value, m_ValueLen);
}

std::string CGameServerSetKeyValueCall::toString()
{
	std::stringstream ss;
	ss << "GS::SetKeyValue( key: '" << m_Key << "'; value:'" << m_Value << "' ) ";
	return ss.str();
}

bool CGameServerSetKeyValueCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerSetKeyValueCall* otherCall = dynamic_cast<CGameServerSetKeyValueCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_KeyLen != m_KeyLen)
		return false;

	if (otherCall->m_ValueLen != m_ValueLen)
		return false;

	if (strcmp(otherCall->m_Key, m_Key))
		return false;

	if (strcmp(otherCall->m_Value, m_Value))
		return false;

	return true;
}

void CGameServerSetKeyValueCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_ValueLen, sizeof(m_ValueLen))
		.write((char*)&m_KeyLen, sizeof(m_KeyLen))
		.write(m_Value, m_ValueLen)
		.write(m_Key, m_KeyLen);
}

void CGameServerSetKeyValueCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_ValueLen, sizeof(m_ValueLen))
		.read((char*)&m_KeyLen, sizeof(m_KeyLen));

	stream
		.read(m_Value, m_ValueLen)
		.read(m_Key, m_KeyLen);
}




/* ============================================================================
					 CSteamApiSetBreakpadAppIdCall
============================================================================ */
CSteamApiSetBreakpadAppIdCall::CSteamApiSetBreakpadAppIdCall(uint32 appId)
{
	m_AppId = appId;
}

std::string CSteamApiSetBreakpadAppIdCall::toString()
{
	std::stringstream ss;
	ss << "SteamAPI_SetBreakpadAppID( count: " << m_AppId << " ) ";
	return ss.str();
}

bool CSteamApiSetBreakpadAppIdCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamApiSetBreakpadAppIdCall* otherCall = dynamic_cast<CSteamApiSetBreakpadAppIdCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_AppId != m_AppId)
		return false;

	return true;
}

void CSteamApiSetBreakpadAppIdCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_AppId, 4);
}

void CSteamApiSetBreakpadAppIdCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_AppId, 4);
}






/* ============================================================================
				  CGameServerWasRestartRequestedCall
============================================================================ */
std::string CGameServerWasRestartRequestedCall::toString()
{
	std::stringstream ss;
	ss << "GS::WasRestartRequested() => " << m_Result;
	return ss.str();
}

bool CGameServerWasRestartRequestedCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerWasRestartRequestedCall* otherCall = dynamic_cast<CGameServerWasRestartRequestedCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CGameServerWasRestartRequestedCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_Result, 1);
}

void CGameServerWasRestartRequestedCall::readPrologue(std::istream &stream) {
	m_Result = false;
	stream.read((char*)&m_Result, 1);
}





/* ============================================================================
					CSteamGameServerRunCallbacksCall
============================================================================ */
std::string CSteamGameServerRunCallbacksCall::toString()
{
	std::stringstream ss;
	ss << "SteamGameServer_RunCallbacks()";
	return ss.str();
}

bool CSteamGameServerRunCallbacksCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamGameServerRunCallbacksCall* otherCall = dynamic_cast<CSteamGameServerRunCallbacksCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}





/* ============================================================================
				   CGameServerGetNextOutgoingPacketCall
============================================================================ */
CGameServerGetNextOutgoingPacketCall::CGameServerGetNextOutgoingPacketCall(int maxOut)
{
	m_MaxOut = maxOut;
	m_BufLen = m_Result = 0; m_Addr = 0; m_Port = 0;
}

void CGameServerGetNextOutgoingPacketCall::setResult(void* buf, int res, uint32* pAddr, uint16* pPort)
{
	m_BufLen = res > 0 ? res : 0;
	if (m_BufLen > 0) {
		if (m_BufLen > sizeof(m_Buf))
			rehlds_syserror("%s: too long buffer returned", __func__);

		memcpy(m_Buf, buf, m_BufLen);
	}
	m_Result = res;

	if (pAddr == NULL)
		rehlds_syserror("%s: pAddr is NULL", __func__);

	if (pPort == NULL)
		rehlds_syserror("%s: pPort is NULL", __func__);

	m_Addr = *pAddr;
	m_Port = *pPort;
}

std::string CGameServerGetNextOutgoingPacketCall::toString()
{
	std::stringstream ss;
	ss << "GS::GetNextOutgoingPacket(buf: "; PrintBinaryArray(m_Buf, m_BufLen, ss);
	ss << ", maxOut: " << m_MaxOut << ", addr: " << m_Addr << ", port: " << m_Port << " ) => " << m_Result;

	return ss.str();
}

bool CGameServerGetNextOutgoingPacketCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerGetNextOutgoingPacketCall* otherCall = dynamic_cast<CGameServerGetNextOutgoingPacketCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_MaxOut != this->m_MaxOut)
		return false;

	return true;
}

void CGameServerGetNextOutgoingPacketCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_MaxOut, 4);

}

void CGameServerGetNextOutgoingPacketCall::writeEpilogue(std::ostream &stream) {
	stream
		.write((char*)&m_Result, 4)
		.write((char*)&m_Addr, 4)
		.write((char*)&m_Port, 2)
		.write((char*)&m_BufLen, 4)
		.write(m_Buf, m_BufLen);
}

void CGameServerGetNextOutgoingPacketCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_MaxOut, 4);

}

void CGameServerGetNextOutgoingPacketCall::readEpilogue(std::istream &stream) {
	stream
		.read((char*)&m_Result, 4)
		.read((char*)&m_Addr, 4)
		.read((char*)&m_Port, 2)
		.read((char*)&m_BufLen, 4);

	stream.read(m_Buf, m_BufLen);
}



/* ============================================================================
							CSteamApiRunCallbacksCall
============================================================================ */
std::string CSteamApiRunCallbacksCall::toString()
{
	std::stringstream ss;
	ss << "SteamAPI_RunCallbacks()";
	return ss.str();
}

bool CSteamApiRunCallbacksCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamApiRunCallbacksCall* otherCall = dynamic_cast<CSteamApiRunCallbacksCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}





/* ============================================================================
					 CGameServerGetSteamIdCall
============================================================================ */
std::string CGameServerGetSteamIdCall::toString()
{
	std::stringstream ss;
	ss << "GS::GetSteamId( ) => " << m_SteamId;

	return ss.str();
}

bool CGameServerGetSteamIdCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerGetSteamIdCall* otherCall = dynamic_cast<CGameServerGetSteamIdCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CGameServerGetSteamIdCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_SteamId, 8);
}

void CGameServerGetSteamIdCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_SteamId, 8);
}




/* ============================================================================
				  CGameServerBSecureCall
============================================================================ */
std::string CGameServerBSecureCall::toString()
{
	std::stringstream ss;
	ss << "GS::BSecure( ) => " << m_Res;

	return ss.str();
}

bool CGameServerBSecureCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerBSecureCall* otherCall = dynamic_cast<CGameServerBSecureCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CGameServerBSecureCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 1);
}

void CGameServerBSecureCall::readEpilogue(std::istream &stream) {
	m_Res = false;
	stream.read((char*)&m_Res, 1);
}




/* ============================================================================
				   CGameServerHandleIncomingPacketCall
============================================================================ */

CGameServerHandleIncomingPacketCall::CGameServerHandleIncomingPacketCall(const void *pData, int cbData, uint32 srcIP, uint16 srcPort)
{
	m_Len = cbData;
	if (m_Len > sizeof(m_Data))
		rehlds_syserror("%s: too long packet", __func__);

	memcpy(m_Data, pData, m_Len);
	m_Ip = srcIP;
	m_Port = srcPort;
	m_Res = false;
}

std::string CGameServerHandleIncomingPacketCall::toString()
{
	std::stringstream ss;
	ss << "GS::CGameServerHandleIncomingPacketCall(data: "; PrintBinaryArray(m_Data, m_Len, ss);
	ss << ", len: " << m_Len << ", ip: " << m_Ip << ", port: " << m_Port << ") => " << m_Res;

	return ss.str();
}

bool CGameServerHandleIncomingPacketCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerHandleIncomingPacketCall* otherCall = dynamic_cast<CGameServerHandleIncomingPacketCall*>(other);
	if (otherCall == NULL)
		return false;

	if (m_Len != otherCall->m_Len)
		return false;

	if (memcmp(m_Data, otherCall->m_Data, m_Len))
		return false;

	if (m_Ip != otherCall->m_Ip || m_Port != otherCall->m_Port)
		return false;

	return true;
}

void CGameServerHandleIncomingPacketCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_Ip, 4)
		.write((char*)&m_Port, 2)
		.write((char*)&m_Len, 4)
		.write(m_Data, m_Len);
}

void CGameServerHandleIncomingPacketCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_Ip, 4)
		.read((char*)&m_Port, 2)
		.read((char*)&m_Len, 4);

	stream.read(m_Data, m_Len);
}

void CGameServerHandleIncomingPacketCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 1);
}

void CGameServerHandleIncomingPacketCall::readEpilogue(std::istream &stream) {
	m_Res = false;
	stream.read((char*)&m_Res, 1);
}





/* ============================================================================
			  CGameServerSendUserConnectAndAuthenticateCall
============================================================================ */
CGameServerSendUserConnectAndAuthenticateCall::CGameServerSendUserConnectAndAuthenticateCall(uint32 unIPClient, const void *pvAuthBlob, uint32 cubAuthBlobSize)
{
	m_AuthBlobLen = cubAuthBlobSize;
	if (m_AuthBlobLen > sizeof(m_AuthBlob))
		rehlds_syserror("%s: too long auth blob", __func__);

	memcpy(m_AuthBlob, pvAuthBlob, m_AuthBlobLen);
	m_IP = unIPClient;
	m_OutSteamId = 0;
	m_Res = false;
}

std::string CGameServerSendUserConnectAndAuthenticateCall::toString()
{
	std::stringstream ss;
	ss << "GS::CGameServerSendUserConnectAndAuthenticateCall(ip: " << m_IP << "authBlob: "; PrintBinaryArray(m_AuthBlob, m_AuthBlobLen, ss);
	ss << ", authBlobLen: " << m_AuthBlobLen << ", steamId: " << m_OutSteamId << ") => " << m_Res;

	return ss.str();
}

bool CGameServerSendUserConnectAndAuthenticateCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerSendUserConnectAndAuthenticateCall* otherCall = dynamic_cast<CGameServerSendUserConnectAndAuthenticateCall*>(other);
	if (otherCall == NULL)
		return false;

	if (m_AuthBlobLen != otherCall->m_AuthBlobLen)
		return false;

	if (strict) {
		if (memcmp(m_AuthBlob, otherCall->m_AuthBlob, m_AuthBlobLen))
			return false;
	}

	if (m_IP != otherCall->m_IP)
		return false;

	return true;
}

void CGameServerSendUserConnectAndAuthenticateCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_IP, 4)
		.write((char*)&m_AuthBlobLen, 4)
		.write(m_AuthBlob, m_AuthBlobLen);
}

void CGameServerSendUserConnectAndAuthenticateCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_IP, 4)
		.read((char*)&m_AuthBlobLen, 4);

	stream.read(m_AuthBlob, m_AuthBlobLen);
}

void CGameServerSendUserConnectAndAuthenticateCall::writeEpilogue(std::ostream &stream) {
	stream
		.write((char*)&m_OutSteamId, 8)
		.write((char*)&m_Res, 1);
}

void CGameServerSendUserConnectAndAuthenticateCall::readEpilogue(std::istream &stream) {
	m_Res = false;
	stream
		.read((char*)&m_OutSteamId, 8)
		.read((char*)&m_Res, 1);
}





/* ============================================================================
			 CGameServerSendUserDisconnectCall
============================================================================ */
std::string CGameServerSendUserDisconnectCall::toString()
{
	std::stringstream ss;
	ss << "GS::CGameServerSendUserDisconnectCall(steamId: " << m_SteamId << ") ";
	return ss.str();
}

bool CGameServerSendUserDisconnectCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerSendUserDisconnectCall* otherCall = dynamic_cast<CGameServerSendUserDisconnectCall*>(other);
	if (otherCall == NULL)
		return false;

	if (m_SteamId != otherCall->m_SteamId)
		return false;

	return true;
}

void CGameServerSendUserDisconnectCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_SteamId, 8);
}

void CGameServerSendUserDisconnectCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_SteamId, 8);
}





/* ============================================================================
				   CGameServerBUpdateUserDataCall
============================================================================ */
CGameServerBUpdateUserDataCall::CGameServerBUpdateUserDataCall(CSteamID steamIDUser, const char *pchPlayerName, uint32 uScore)
{
	m_PlayerNameLen = strlen(pchPlayerName) + 1;
	if (m_PlayerNameLen > sizeof(m_PlayerName))
		rehlds_syserror("%s: too long player name", __func__);

	memcpy(m_PlayerName, pchPlayerName, m_PlayerNameLen);
	m_SteamId = steamIDUser.ConvertToUint64();
	m_Score = uScore;
}

std::string CGameServerBUpdateUserDataCall::toString()
{
	std::stringstream ss;
	ss << "GS::BUpdateUserData(steamIDUser: " << m_SteamId << "playerName: "; PrintBinaryArray(m_PlayerName, m_PlayerNameLen, ss);
	ss << ", uScore: " << m_Score << " ) => " << m_Res;

	return ss.str();
}

bool CGameServerBUpdateUserDataCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerBUpdateUserDataCall* otherCall = dynamic_cast<CGameServerBUpdateUserDataCall*>(other);
	if (otherCall == NULL)
		return false;

	if (m_PlayerNameLen != otherCall->m_PlayerNameLen)
		return false;

	if (memcmp(m_PlayerName, otherCall->m_PlayerName, m_PlayerNameLen))
		return false;

	if (m_SteamId != otherCall->m_SteamId)
		return false;

	if (m_Score != otherCall->m_Score)
		return false;

	return true;
}

void CGameServerBUpdateUserDataCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_SteamId, 8)
		.write((char*)&m_Score, 4)
		.write((char*)&m_PlayerNameLen, 4)
		.write(m_PlayerName, m_PlayerNameLen);
}

void CGameServerBUpdateUserDataCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_SteamId, 8)
		.read((char*)&m_Score, 4)
		.read((char*)&m_PlayerNameLen, 4);

	stream.read(m_PlayerName, m_PlayerNameLen);
}

void CGameServerBUpdateUserDataCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 1);
}

void CGameServerBUpdateUserDataCall::readEpilogue(std::istream &stream) {
	m_Res = false;
	stream.read((char*)&m_Res, 1);
}




/* ============================================================================
			  CGameServerCreateUnauthUserConnectionCall
============================================================================ */
std::string CGameServerCreateUnauthUserConnectionCall::toString()
{
	std::stringstream ss;
	ss << "GS::CreateUnauthUserConnection() => " << m_SteamId;

	return ss.str();
}

bool CGameServerCreateUnauthUserConnectionCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerCreateUnauthUserConnectionCall* otherCall = dynamic_cast<CGameServerCreateUnauthUserConnectionCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}


void CGameServerCreateUnauthUserConnectionCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_SteamId, 8);
}

void CGameServerCreateUnauthUserConnectionCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_SteamId, 8);
}





/* ============================================================================
							  CGetHostNameCall
============================================================================ */
std::string CGetHostNameCall::toString()
{
	std::stringstream ss;
	ss << "gethostname(name: " << m_Name << "; nameLen: " << m_NameLenIn << " ) => " << m_Res;

	return ss.str();
}

bool CGetHostNameCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGetHostNameCall* otherCall = dynamic_cast<CGetHostNameCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_NameLenIn != this->m_NameLenIn)
		return false;

	return true;
}

void CGetHostNameCall::setResult(char* hostName, int res) {
	m_NameLenOut = strlen(hostName) + 1;

	if (m_NameLenOut > sizeof(m_Name))
		rehlds_syserror("%s: too long host name", __func__);

	strcpy(m_Name, hostName);
	m_Res = res;
}

void CGetHostNameCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_NameLenIn, 4);
}

void CGetHostNameCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_NameLenIn, 4);
}

void CGetHostNameCall::writeEpilogue(std::ostream &stream) {
	stream
		.write((char*)&m_NameLenOut, 4)
		.write((char*)&m_Res, 4)
		.write(m_Name, m_NameLenOut);
}

void CGetHostNameCall::readEpilogue(std::istream &stream) {
	stream
		.read((char*)&m_NameLenOut, 4)
		.read((char*)&m_Res, 4);

	stream.read(m_Name, m_NameLenOut);
}




/* ============================================================================
						   CGetHostByNameCall
============================================================================ */
CGetHostByNameCall::CGetHostByNameCall(const char* name)
{
	m_NameLen = strlen(name) + 1;
	if (m_NameLen > sizeof(m_Name))
		rehlds_syserror("%s: too long name", __func__);

	strcpy(m_Name, name);
}

std::string CGetHostByNameCall::toString()
{
	std::stringstream ss;
	ss << "gethostbyname(name: " << m_Name << " ) => <>";

	return ss.str();
}

bool CGetHostByNameCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGetHostByNameCall* otherCall = dynamic_cast<CGetHostByNameCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_NameLen != this->m_NameLen)
		return false;

	if (strcmp(otherCall->m_Name, this->m_Name))
		return false;

	return true;
}

void CGetHostByNameCall::setResult(const hostent* hostEnt) {
	m_HostentData.hostNameLen = strlen(hostEnt->h_name) + 1;
	if (m_HostentData.hostNameLen > sizeof(m_HostentData.hostName)) {
		rehlds_syserror("%s: too long host name", __func__);
	}

	strcpy(m_HostentData.hostName, hostEnt->h_name);
	int i = 0;
	while (hostEnt->h_aliases[i]) {
		if (i >= HOSTENT_DATA_MAX_ALIASES) {
			rehlds_syserror("%s: too many aliases", __func__);
		}

		m_HostentData.aliasesLengths[i] = strlen(hostEnt->h_aliases[i]) + 1;
		if (m_HostentData.aliasesLengths[i] > sizeof(m_HostentData.aliases[i])) {
			rehlds_syserror("%s: too long alias", __func__);
		}

		strcpy(m_HostentData.aliases[i], hostEnt->h_aliases[i]);
		i++;
	}
	m_HostentData.numAliases = i;

	m_HostentData.addrtype = hostEnt->h_addrtype;
	m_HostentData.addrLen = hostEnt->h_length;
	if (m_HostentData.addrLen > sizeof(m_HostentData.addrs[0])) {
		rehlds_syserror("%s: too long addr", __func__);
	}

	i = 0;
	while (hostEnt->h_addr_list[i]) {
		if (i >= HOSTENT_DATA_MAX_ADDRS) {
			rehlds_syserror("%s: too many addrs", __func__);
		}

		memcpy(m_HostentData.addrs[i], hostEnt->h_addr_list[i], m_HostentData.addrLen);
		i++;
	}
	m_HostentData.numAddrs = i;
}

void CGetHostByNameCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_NameLen, 4).write(m_Name, m_NameLen);
}

void CGetHostByNameCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_NameLen, 4);
	stream.read(m_Name, m_NameLen);
}

void CGetHostByNameCall::writeEpilogue(std::ostream &stream) {
	stream
		.write((char*)&m_HostentData.hostNameLen, 4)
		.write((char*)&m_HostentData.numAddrs, 4)
		.write((char*)&m_HostentData.numAliases, 4)
		.write((char*)&m_HostentData.addrLen, 2)
		.write((char*)&m_HostentData.addrtype, 2)
		.write(m_HostentData.hostName, m_HostentData.hostNameLen)
		.write((char*)&m_HostentData.aliasesLengths[0], 4 * m_HostentData.numAliases);

	for (int i = 0; i < m_HostentData.numAliases; i++) {
		stream.write(m_HostentData.aliases[i], m_HostentData.aliasesLengths[i]);
	}

	for (int i = 0; i < m_HostentData.numAddrs; i++) {
		stream.write(m_HostentData.addrs[i], m_HostentData.addrLen);
	}
}

void CGetHostByNameCall::readEpilogue(std::istream &stream) {
	stream
		.read((char*)&m_HostentData.hostNameLen, 4)
		.read((char*)&m_HostentData.numAddrs, 4)
		.read((char*)&m_HostentData.numAliases, 4)
		.read((char*)&m_HostentData.addrLen, 2)
		.read((char*)&m_HostentData.addrtype, 2);

	stream
		.read(m_HostentData.hostName, m_HostentData.hostNameLen)
		.read((char*)&m_HostentData.aliasesLengths[0], 4 * m_HostentData.numAliases);

	for (int i = 0; i < m_HostentData.numAliases; i++) {
		stream.read(m_HostentData.aliases[i], m_HostentData.aliasesLengths[i]);
	}

	for (int i = 0; i < m_HostentData.numAddrs; i++) {
		stream.read(m_HostentData.addrs[i], m_HostentData.addrLen);
	}
}





/* ============================================================================
						   CGetProcessTimesCall
============================================================================ */
std::string CGetProcessTimesCall::toString()
{
	std::stringstream ss;

	ss << "GetProcessTimes( creationTime: "; PrintFileTime(&m_CreationTime, ss);
	ss << "; exitTime: "; PrintFileTime(&m_ExitTime, ss);
	ss << "; kernelTime: "; PrintFileTime(&m_KernelTime, ss);
	ss << "; userTime: "; PrintFileTime(&m_UserTime, ss);
	ss << " ) => " << m_Res;
	return ss.str();
}

bool CGetProcessTimesCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGetProcessTimesCall* otherCall = dynamic_cast<CGetProcessTimesCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CGetProcessTimesCall::setResult(BOOL res, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime)
{
	m_Res = res;
	memcpy(&m_CreationTime, lpCreationTime, sizeof(m_CreationTime));
	memcpy(&m_ExitTime, lpExitTime, sizeof(m_ExitTime));
	memcpy(&m_KernelTime, lpKernelTime, sizeof(m_KernelTime));
	memcpy(&m_UserTime, lpUserTime, sizeof(m_UserTime));
}

void CGetProcessTimesCall::writeEpilogue(std::ostream &stream) {
	stream
		.write((char*)&m_CreationTime, sizeof(m_CreationTime))
		.write((char*)&m_ExitTime, sizeof(m_ExitTime))
		.write((char*)&m_KernelTime, sizeof(m_KernelTime))
		.write((char*)&m_UserTime, sizeof(m_UserTime))
		.write((char*)&m_Res, 1);
}

void CGetProcessTimesCall::readEpilogue(std::istream &stream) {
	m_Res = 0;
	stream
		.read((char*)&m_CreationTime, sizeof(m_CreationTime))
		.read((char*)&m_ExitTime, sizeof(m_ExitTime))
		.read((char*)&m_KernelTime, sizeof(m_KernelTime))
		.read((char*)&m_UserTime, sizeof(m_UserTime))
		.read((char*)&m_Res, 1);
}




/* ============================================================================
				   CGetSystemTimeAsFileTimeCall
============================================================================ */
std::string CGetSystemTimeAsFileTimeCall::toString()
{
	std::stringstream ss;

	ss << "GetSystemTimeAsFileTime( systemTime: "; PrintFileTime(&m_SystemTime, ss);
	ss << " )";
	return ss.str();
}

bool CGetSystemTimeAsFileTimeCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGetSystemTimeAsFileTimeCall* otherCall = dynamic_cast<CGetSystemTimeAsFileTimeCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CGetSystemTimeAsFileTimeCall::setResult(LPFILETIME systemTime)
{
	memcpy(&m_SystemTime, systemTime, sizeof(m_SystemTime));
}

void CGetSystemTimeAsFileTimeCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_SystemTime, sizeof(m_SystemTime));
}

void CGetSystemTimeAsFileTimeCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_SystemTime, sizeof(m_SystemTime));
}





/* ============================================================================
							CStdTimeCall
============================================================================ */
CStdTimeCall::CStdTimeCall(uint32* inTime)
{
	m_InTimeNull = (inTime == NULL);
}

std::string CStdTimeCall::toString()
{
	std::stringstream ss;

	ss << "time( isNull: " << m_InTimeNull << ") => " << m_Res;
	return ss.str();
}

bool CStdTimeCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CStdTimeCall* otherCall = dynamic_cast<CStdTimeCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_InTimeNull != this->m_InTimeNull)
		return false;

	return true;
}

void CStdTimeCall::setResult(uint32 res)
{
	m_Res = res;
}

void CStdTimeCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_InTimeNull, 1);
}

void CStdTimeCall::readPrologue(std::istream &stream) {
	m_InTimeNull = false;
	stream.read((char*)&m_InTimeNull, 1);
}

void CStdTimeCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 4);
}

void CStdTimeCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_Res, 4);
}





/* ============================================================================
						  CStdLocalTimeCall
============================================================================ */
CStdLocalTimeCall::CStdLocalTimeCall(uint32 inTime)
{
	m_Time = inTime;
}

std::string CStdLocalTimeCall::toString()
{
	std::stringstream ss;

	ss << "localTime( time: " << m_Time << ") => ";
	PrintTm(&m_Res, ss);
	return ss.str();
}

bool CStdLocalTimeCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CStdLocalTimeCall* otherCall = dynamic_cast<CStdLocalTimeCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_Time != this->m_Time)
		return false;

	return true;
}

void CStdLocalTimeCall::setResult(struct tm *res)
{
	memcpy(&m_Res, res, sizeof(m_Res));
}

void CStdLocalTimeCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_Time, 4);
}

void CStdLocalTimeCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_Time, 4);
}

void CStdLocalTimeCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, sizeof(m_Res));
}

void CStdLocalTimeCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_Res, sizeof(m_Res));
}





/* ============================================================================
					CStdSrandCall
============================================================================ */
std::string CStdSrandCall::toString()
{
	std::stringstream ss;

	ss << "srand( seed: " << m_Seed << ") ";
	return ss.str();
}

bool CStdSrandCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CStdSrandCall* otherCall = dynamic_cast<CStdSrandCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_Seed != this->m_Seed)
		return false;

	return true;
}

void CStdSrandCall::writePrologue(std::ostream &stream) {
	stream.write((char*)&m_Seed, 4);
}

void CStdSrandCall::readPrologue(std::istream &stream) {
	stream.read((char*)&m_Seed, 4);
}





/* ============================================================================
							  CStdRandCall
============================================================================ */
std::string CStdRandCall::toString()
{
	std::stringstream ss;

	ss << "rand( ) => " << m_Res;
	return ss.str();
}

bool CStdRandCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CStdRandCall* otherCall = dynamic_cast<CStdRandCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CStdRandCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 4);
}

void CStdRandCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_Res, 4);
}





/* ============================================================================
				  CGameServerLogOffCall
============================================================================ */
std::string CGameServerLogOffCall::toString()
{
	std::stringstream ss;

	ss << "GS:LogOff( ) ";
	return ss.str();
}

bool CGameServerLogOffCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerLogOffCall* otherCall = dynamic_cast<CGameServerLogOffCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}




/* ============================================================================
						 CSteamGameServerShutdownCall
============================================================================ */
std::string CSteamGameServerShutdownCall::toString()
{
	std::stringstream ss;

	ss << "SteamGameServer_Shutdown( ) ";
	return ss.str();
}

bool CSteamGameServerShutdownCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamGameServerShutdownCall* otherCall = dynamic_cast<CSteamGameServerShutdownCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}





/* ============================================================================
				   CSteamApiUnregisterCallbackCall
============================================================================ */
CSteamApiUnregisterCallbackCall::CSteamApiUnregisterCallbackCall(int rehldsCallbackId, CCallbackBase* cb)
{
	m_RehldsCallbackId = rehldsCallbackId;
	m_InState.m_iCallback = cb->GetICallback();
	m_InState.m_nCallbackFlags = cb->GetFlags();
	m_OutState.clear();
}

std::string CSteamApiUnregisterCallbackCall::toString()
{
	std::stringstream ss;
	ss << "SteamAPI_UnregisterCallback( rehldsCallbackId: " << m_RehldsCallbackId << " )";
	return ss.str();
}

bool CSteamApiUnregisterCallbackCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CSteamApiUnregisterCallbackCall* otherCall = dynamic_cast<CSteamApiUnregisterCallbackCall*>(other);
	if (otherCall == NULL)
		return false;

	if (otherCall->m_RehldsCallbackId != m_RehldsCallbackId)
		return false;

	if (otherCall->m_InState.m_iCallback != m_InState.m_iCallback) return false;
	if (otherCall->m_InState.m_nCallbackFlags != m_InState.m_nCallbackFlags) return false;

	return true;
}

void CSteamApiUnregisterCallbackCall::writePrologue(std::ostream &stream) {
	stream
		.write((char*)&m_InState, sizeof(m_InState))
		.write((char*)&m_RehldsCallbackId, 4);
}

void CSteamApiUnregisterCallbackCall::readPrologue(std::istream &stream) {
	stream
		.read((char*)&m_InState, sizeof(m_InState))
		.read((char*)&m_RehldsCallbackId, 4);
}

void CSteamApiUnregisterCallbackCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_OutState, sizeof(m_OutState));
}

void CSteamApiUnregisterCallbackCall::readEpilogue(std::istream &stream) {
	stream.read((char*)&m_OutState, sizeof(m_OutState));
}




/* ============================================================================
				  CGameServerBLoggedOnCall
============================================================================ */
std::string CGameServerBLoggedOnCall::toString()
{
	std::stringstream ss;
	ss << "GS::BLoggedOn( ) => " << m_Res;

	return ss.str();
}

bool CGameServerBLoggedOnCall::compareInputArgs(IEngExtCall* other, bool strict)
{
	CGameServerBLoggedOnCall* otherCall = dynamic_cast<CGameServerBLoggedOnCall*>(other);
	if (otherCall == NULL)
		return false;

	return true;
}

void CGameServerBLoggedOnCall::writeEpilogue(std::ostream &stream) {
	stream.write((char*)&m_Res, 1);
}

void CGameServerBLoggedOnCall::readEpilogue(std::istream &stream) {
	m_Res = false;
	stream.read((char*)&m_Res, 1);
}

/*
class CGameServerBLoggedOnCall : public IEngExtCall {
public:
bool m_Res;

public:
CGameServerBLoggedOnCall() { m_Res = false; }

void setResult(bool res) { m_Res = res; }
virtual bool compareInputArgs(IEngExtCall* other);
virtual std::string toString();
virtual ExtCallFuncs getOpcode() { return ECF_GS_BLOGGEDON; }
virtual void writeEpilogue(std::ostream &stream);
virtual void readEpilogue(std::istream &stream);
};
*/

#define IEngExtCallFactory_CreateFuncCall(clazz, buf, bufLen) \
	if (sizeof(clazz) > bufLen) rehlds_syserror("%s: buffer to small", __func__); \
	return new(buf) clazz();

IEngExtCall* IEngExtCallFactory::createByOpcode(ExtCallFuncs opc, void* buf, int ptrSize) {
	switch (opc) {
	case ECF_SLEEP:	IEngExtCallFactory_CreateFuncCall(CSleepExtCall, buf, ptrSize);
	case ECF_QUERY_PERF_FREQ: IEngExtCallFactory_CreateFuncCall(CQueryPerfFreqCall, buf, ptrSize);
	case ECF_QUERY_PERF_COUNTER: IEngExtCallFactory_CreateFuncCall(CQueryPerfCounterCall, buf, ptrSize);
	case ECF_GET_TICK_COUNT: IEngExtCallFactory_CreateFuncCall(CGetTickCountCall, buf, ptrSize);
	case ECF_GET_LOCAL_TIME: IEngExtCallFactory_CreateFuncCall(CGetLocalTimeCall, buf, ptrSize);
	case ECF_GET_SYSTEM_TIME: IEngExtCallFactory_CreateFuncCall(CGetSystemTimeCall, buf, ptrSize);
	case ECF_GET_TIMEZONE_INFO: IEngExtCallFactory_CreateFuncCall(CGetTimeZoneInfoCall, buf, ptrSize);

	case ECF_SOCKET: IEngExtCallFactory_CreateFuncCall(CSocketCall, buf, ptrSize);
	case ECF_IOCTL_SOCKET: IEngExtCallFactory_CreateFuncCall(CIoCtlSocketCall, buf, ptrSize);
	case ECF_SET_SOCK_OPT: IEngExtCallFactory_CreateFuncCall(CSetSockOptCall, buf, ptrSize);
	case ECF_CLOSE_SOCKET: IEngExtCallFactory_CreateFuncCall(CCloseSocketCall, buf, ptrSize);
	case ECF_RECVFROM: IEngExtCallFactory_CreateFuncCall(CRecvFromCall, buf, ptrSize);
	case ECF_SENDTO: IEngExtCallFactory_CreateFuncCall(CSendToCall, buf, ptrSize);
	case ECF_BIND: IEngExtCallFactory_CreateFuncCall(CBindCall, buf, ptrSize);
	case ECF_GET_SOCK_NAME: IEngExtCallFactory_CreateFuncCall(CGetSockNameCall, buf, ptrSize);
	case ECF_WSA_GET_LAST_ERROR: IEngExtCallFactory_CreateFuncCall(CWSAGetLastErrorCall, buf, ptrSize);

	case ECF_STEAM_CALLBACK_CALL_1: IEngExtCallFactory_CreateFuncCall(CSteamCallbackCall1, buf, ptrSize);
	case ECF_STEAM_CALLBACK_CALL_2: IEngExtCallFactory_CreateFuncCall(CSteamCallbackCall2, buf, ptrSize);

	case ECF_STEAM_API_REGISTER_CALLBACK: IEngExtCallFactory_CreateFuncCall(CSteamApiRegisterCallbackCall, buf, ptrSize);
	case ECF_STEAM_API_INIT: IEngExtCallFactory_CreateFuncCall(CSteamApiRegisterCallbackCall, buf, ptrSize);
	case ECF_STEAM_API_UNREGISTER_CALL_RESULT: IEngExtCallFactory_CreateFuncCall(CSteamApiUnrigestierCallResultCall, buf, ptrSize);
	case ECF_STEAMAPPS: IEngExtCallFactory_CreateFuncCall(CSteamAppsCall, buf, ptrSize);
	case ECF_STEAMAPPS_GET_CURRENT_GAME_LANGUAGE: IEngExtCallFactory_CreateFuncCall(CSteamAppGetCurrentGameLanguageCall, buf, ptrSize);

	case ECF_STEAMGAMESERVER_INIT: IEngExtCallFactory_CreateFuncCall(CSteamGameServerInitCall, buf, ptrSize);
	case ECF_STEAMGAMESERVER: IEngExtCallFactory_CreateFuncCall(CSteamGameServerCall, buf, ptrSize);
	case ECF_GS_SET_PRODUCT: IEngExtCallFactory_CreateFuncCall(CGameServerSetProductCall, buf, ptrSize);
	case ECF_GS_SET_GAME_DIR: IEngExtCallFactory_CreateFuncCall(CGameServerSetModDirCall, buf, ptrSize);
	case ECF_GS_SET_DEDICATED_SERVER: IEngExtCallFactory_CreateFuncCall(CGameServerSetDedicatedServerCall, buf, ptrSize);
	case ECF_GS_SET_GAME_DESC: IEngExtCallFactory_CreateFuncCall(CGameServerSetGameDescCall, buf, ptrSize);
	case ECF_GS_LOG_ON_ANONYMOUS: IEngExtCallFactory_CreateFuncCall(CGameServerLogOnAnonymousCall, buf, ptrSize);
	case ECF_GS_ENABLE_HEARTBEATS: IEngExtCallFactory_CreateFuncCall(CGameServerEnableHeartbeatsCall, buf, ptrSize);
	case ECF_GS_SET_HEARTBEATS_INTERVAL: IEngExtCallFactory_CreateFuncCall(CGameServerSetHeartbeatIntervalCall, buf, ptrSize);
	case ECF_GS_SET_MAX_PLAYERS_COUNT: IEngExtCallFactory_CreateFuncCall(CGameServerSetMaxPlayersCall, buf, ptrSize);
	case ECF_GS_SET_BOT_PLAYERS_COUNT: IEngExtCallFactory_CreateFuncCall(CGameServerSetBotCountCall, buf, ptrSize);
	case ECF_GS_SET_SERVER_NAME: IEngExtCallFactory_CreateFuncCall(CGameServerSetServerNameCall, buf, ptrSize);
	case ECF_GS_SET_MAP_NAME: IEngExtCallFactory_CreateFuncCall(CGameServerSetMapNameCall, buf, ptrSize);
	case ECF_GS_SET_PASSWORD_PROTECTED: IEngExtCallFactory_CreateFuncCall(CGameServerSetPasswordProtectedCall, buf, ptrSize);
	case ECF_GS_CLEAR_ALL_KEY_VALUES: IEngExtCallFactory_CreateFuncCall(CGameServerClearAllKVsCall, buf, ptrSize);
	case ECF_GS_SET_KEY_VALUE: IEngExtCallFactory_CreateFuncCall(CGameServerSetKeyValueCall, buf, ptrSize);
	case ECF_STEAM_API_SET_BREAKPAD_APP_ID: IEngExtCallFactory_CreateFuncCall(CSteamApiSetBreakpadAppIdCall, buf, ptrSize);
	case ECF_GS_WAS_RESTART_REQUESTED: IEngExtCallFactory_CreateFuncCall(CGameServerWasRestartRequestedCall, buf, ptrSize);
	case ECF_STEAMGAMESERVER_RUN_CALLBACKS: IEngExtCallFactory_CreateFuncCall(CSteamGameServerRunCallbacksCall, buf, ptrSize);
	case ECF_GS_GET_NEXT_OUTGOING_PACKET: IEngExtCallFactory_CreateFuncCall(CGameServerGetNextOutgoingPacketCall, buf, ptrSize);
	case ECF_STEAM_API_RUN_CALLBACKS: IEngExtCallFactory_CreateFuncCall(CSteamApiRunCallbacksCall, buf, ptrSize);
	case ECF_GS_GET_STEAM_ID: IEngExtCallFactory_CreateFuncCall(CGameServerGetSteamIdCall, buf, ptrSize);
	case ECF_GS_BSECURE: IEngExtCallFactory_CreateFuncCall(CGameServerBSecureCall, buf, ptrSize);
	case ECF_GS_HANDLE_INCOMING_PACKET: IEngExtCallFactory_CreateFuncCall(CGameServerHandleIncomingPacketCall, buf, ptrSize);
	case ECF_GS_SEND_USER_CONNECT_AND_AUTHENTICATE: IEngExtCallFactory_CreateFuncCall(CGameServerSendUserConnectAndAuthenticateCall, buf, ptrSize);
	case ECF_GS_SEND_USER_DISCONNECT: IEngExtCallFactory_CreateFuncCall(CGameServerSendUserDisconnectCall, buf, ptrSize);
	case ECF_GS_BUPDATE_USER_DATA: IEngExtCallFactory_CreateFuncCall(CGameServerBUpdateUserDataCall, buf, ptrSize);
	case ECF_GS_CREATE_UNAUTH_USER_CONNECTION: IEngExtCallFactory_CreateFuncCall(CGameServerCreateUnauthUserConnectionCall, buf, ptrSize);
	case ECF_GET_HOST_BY_NAME: IEngExtCallFactory_CreateFuncCall(CGetHostByNameCall, buf, ptrSize);
	case ECF_GET_HOST_NAME: IEngExtCallFactory_CreateFuncCall(CGetHostNameCall, buf, ptrSize);

	case ECF_GET_PROCESS_TIMES: IEngExtCallFactory_CreateFuncCall(CGetProcessTimesCall, buf, ptrSize);
	case ECF_GET_SYSTEM_TIME_AS_FILE_TIME: IEngExtCallFactory_CreateFuncCall(CGetSystemTimeAsFileTimeCall, buf, ptrSize);

	case ECF_CSTD_TIME: IEngExtCallFactory_CreateFuncCall(CStdTimeCall, buf, ptrSize);
	case ECF_CSTD_LOCALTIME: IEngExtCallFactory_CreateFuncCall(CStdLocalTimeCall, buf, ptrSize);
	case ECF_CSTD_SRAND_CALL: IEngExtCallFactory_CreateFuncCall(CStdSrandCall, buf, ptrSize);
	case ECF_CSTD_RAND_CALL: IEngExtCallFactory_CreateFuncCall(CStdRandCall, buf, ptrSize);

	case ECF_GS_LOGOFF: IEngExtCallFactory_CreateFuncCall(CGameServerLogOffCall, buf, ptrSize);

	case ECF_STEAMGAMESERVER_SHUTDOWN: IEngExtCallFactory_CreateFuncCall(CSteamGameServerShutdownCall, buf, ptrSize);

	case ECF_STEAM_API_UNREGISTER_CALLBACK: IEngExtCallFactory_CreateFuncCall(CSteamApiUnregisterCallbackCall, buf, ptrSize);

	case ECF_GS_BLOGGEDON: IEngExtCallFactory_CreateFuncCall(CGameServerBLoggedOnCall, buf, ptrSize);


	default:
		rehlds_syserror("%s: unknown funccall opcode %d", __func__, opc);
	}
}
