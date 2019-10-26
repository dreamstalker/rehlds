#include "precompiled.h"

#if defined(HLTV_FIXES) || defined(LAUNCHER_FIXES)
#ifdef _WIN32

#if defined(_MSC_VER) && !defined(_IMAGEHLP_)
#pragma warning(push)
#pragma warning(disable:4091) // A microsoft header has warnings. Very nice.
#include <DbgHelp.h>
#pragma warning(pop)
#endif

// Gets last error.
static inline HRESULT GetLastHresult()
{
	return HRESULT_FROM_WIN32(GetLastError());
}

#define WIDE_TEXT_HELPER_(quote) L##quote
#define WIDE_TEXT(quote) WIDE_TEXT_HELPER_(quote)

// Gets application version which is safe for use in minidump file name.  Can be entirely constexpr with C++17 and std::array.
template<size_t appVersionSize>
static wchar_t* GetAppVersionForMiniDumpName(wchar_t (&appVersion)[appVersionSize]) {
	constexpr wchar_t rawAppVersion[]{WIDE_TEXT(APP_VERSION "-" APP_COMMIT_SHA)};

	static_assert(appVersionSize >= ARRAYSIZE(rawAppVersion), "App version buffer size should be enough to store app version.");

	// See https://docs.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-pathcleanupspec#remarks for details.
	constexpr wchar_t invalidPathChars[]{L'\\', L'/', L':', L'*', L'?', L'"', L'<',  L'>', L'|', L';', L','};

	size_t outIt{0};
	for (size_t rawIt{0}; rawIt < ARRAYSIZE(rawAppVersion); ++rawIt)
	{
		const wchar_t currentRawIt{rawAppVersion[rawIt]};
		bool isValidRaw{true};

		for (size_t invalidIt{0}; invalidIt < ARRAYSIZE(invalidPathChars); ++invalidIt)
		{
			isValidRaw = invalidPathChars[invalidIt] != currentRawIt;

			if (!isValidRaw)
			{
				break;
			}
		}

		if (isValidRaw) {
			appVersion[outIt++] = currentRawIt;
		}
	}

	return appVersion;
}

// Creates a new minidump file and dumps the exception info into it.
static HRESULT WriteMiniDumpUsingExceptionInfo(unsigned int exceptionCode,
	struct _EXCEPTION_POINTERS *exceptionInfo, MINIDUMP_TYPE minidumpType)
{
	// Counter used to make sure minidump names are unique.
	static int minidumpsWrittenCount{0};

	HMODULE dbghelpModule{LoadLibraryW(L"DbgHelp.dll")};
	HRESULT errorCode{dbghelpModule ? S_OK : GetLastHresult()};

	using MiniDumpWriteDumpFn = decltype(&MiniDumpWriteDump);
	MiniDumpWriteDumpFn miniDumpWriteDump{nullptr};
	if (SUCCEEDED(errorCode))
	{
		miniDumpWriteDump = reinterpret_cast<MiniDumpWriteDumpFn>(GetProcAddress(
			dbghelpModule, "MiniDumpWriteDump"));
		errorCode = miniDumpWriteDump ? S_OK : GetLastHresult();
	}

	// Creates a unique filename for the minidump based on the current time and
	// module name.
	const time_t timeNow{time(nullptr)};
	tm *localTimeNow{localtime(&timeNow)};
	if (localTimeNow == nullptr)
	{
		errorCode = E_INVALIDARG;
	}

	// Strip off the rest of the path from the .exe name.
	wchar_t moduleName[MAX_PATH];
	if (SUCCEEDED(errorCode))
	{
		::SetLastError(NOERROR);
		::GetModuleFileNameW(nullptr, moduleName, static_cast<DWORD>(ARRAYSIZE(moduleName)));

		errorCode = GetLastHresult();
	}

	wchar_t fileName[MAX_PATH];
	HANDLE minidumpFile{nullptr};

	if (SUCCEEDED(errorCode))
	{
		wchar_t *strippedModuleName{wcsrchr(moduleName, L'.')};
		if (strippedModuleName) *strippedModuleName = L'\0';

		strippedModuleName = wcsrchr(moduleName, L'\\');
		// Move past the last slash.
		if (strippedModuleName) ++strippedModuleName;

		wchar_t appVersion[ARRAYSIZE(APP_VERSION) + ARRAYSIZE(APP_COMMIT_SHA) + 2];
		// <exe name>_<app version-app hash build>_crash_<date YYYYMMDD>_<time HHMMSS>_<unique counter>.mdmp
		swprintf(fileName, ARRAYSIZE(fileName), L"%s_%s_crash_%d%.2d%.2d_%.2d%.2d%.2d_%d.mdmp",
			strippedModuleName ? strippedModuleName : L"unknown",
			GetAppVersionForMiniDumpName(appVersion),
			localTimeNow->tm_year + 1900, /* Year less 2000 */
			localTimeNow->tm_mon + 1,     /* month (0 - 11 : 0 = January) */
			localTimeNow->tm_mday,        /* day of month (1 - 31) */
			localTimeNow->tm_hour,        /* hour (0 - 23) */
			localTimeNow->tm_min,         /* minutes (0 - 59) */
			localTimeNow->tm_sec,         /* seconds (0 - 59) */
			++minidumpsWrittenCount);     // ensures the filename is unique

		minidumpFile = CreateFileW(fileName, GENERIC_WRITE, FILE_SHARE_WRITE,
									nullptr, CREATE_ALWAYS,
									FILE_ATTRIBUTE_NORMAL, nullptr);
		errorCode = minidumpFile != INVALID_HANDLE_VALUE ? S_OK : GetLastHresult();
	}

	if (SUCCEEDED(errorCode) && miniDumpWriteDump)
	{
		// Dump the exception information into the file.
		MINIDUMP_EXCEPTION_INFORMATION exInfo;
		exInfo.ThreadId = GetCurrentThreadId();
		exInfo.ExceptionPointers = exceptionInfo;
		exInfo.ClientPointers = FALSE;

		const BOOL wasWrittenMinidump{miniDumpWriteDump(
			GetCurrentProcess(), GetCurrentProcessId(), minidumpFile,
			minidumpType, &exInfo, nullptr, nullptr)};

		// If the function succeeds, the return value is TRUE; otherwise, the
		// return value is FALSE.  To retrieve extended error information, call
		// GetLastError.  Note that the last error will be an HRESULT value.  If
		// the operation is canceled, the last error code is HRESULT_FROM_WIN32(ERROR_CANCELLED).
		// See
		// https://docs.microsoft.com/en-us/windows/win32/api/minidumpapiset/nf-minidumpapiset-minidumpwritedump
		errorCode = wasWrittenMinidump ? S_OK : static_cast<HRESULT>(GetLastError());
	}

	if (minidumpFile && minidumpFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(minidumpFile);
	}

	if (dbghelpModule)
	{
		FreeLibrary(dbghelpModule);
	}

	return errorCode;
}

// Writes process mini dump by exception code and info.
void WriteMiniDump(unsigned int exceptionCode,
					struct _EXCEPTION_POINTERS *exceptionInfo)
{
	// First try to write it with all the indirectly referenced memory (ie: a
	// large file).  If that doesn't work, then write a smaller one.
	auto minidumpType = static_cast<MINIDUMP_TYPE>(
		MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpWithThreadInfo |
		MiniDumpWithIndirectlyReferencedMemory);

	if (FAILED(WriteMiniDumpUsingExceptionInfo(exceptionCode, exceptionInfo, minidumpType)))
	{
		minidumpType = MiniDumpWithDataSegs;

		WriteMiniDumpUsingExceptionInfo(exceptionCode, exceptionInfo, minidumpType);
	}
}

// Determines either debugger attached to a process or not.
bool HasDebuggerPresent()
{
	return ::IsDebuggerPresent() != FALSE;
}

// Determines either writing mini dumps is enabled or not (-nominidumps).  Same as in Source games.
bool IsWriteMiniDumpsEnabled(const char *commandLine)
{
	if (!commandLine)
	{
		return true;
	}

	const char *noMiniDumps{strstr(commandLine, "-nominidumps")};
	if (!noMiniDumps)
	{
		return true;
	}

	const char *nextCharAfterNoMiniDumps{noMiniDumps + ARRAYSIZE("-nominidumps") - 1};
	// Command line ends with -nominidumps or has space after -nominidumps -> not mini dump.
	return !(nextCharAfterNoMiniDumps[0] == '\0' || isspace(nextCharAfterNoMiniDumps[0]));
}

#endif  // _WIN32
#endif  // HLTV_FIXES || LAUNCHER_FIXES
