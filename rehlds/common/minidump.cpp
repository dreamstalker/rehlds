#include "precompiled.h"


#if defined(HLTV_FIXES) || defined(LAUNCHER_FIXES)
#ifdef _WIN32

// Gets last error.
static inline HRESULT GetLastHresult()
{
	return HRESULT_FROM_WIN32(GetLastError());
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
	MiniDumpWriteDumpFn minidumWriteDump{nullptr};
	if (SUCCEEDED(errorCode)) 
	{
		minidumWriteDump = reinterpret_cast<MiniDumpWriteDumpFn>(GetProcAddress(
			dbghelpModule, "MiniDumpWriteDump"));
		errorCode = minidumWriteDump ? S_OK : GetLastHresult();
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

		swprintf(fileName, ARRAYSIZE(fileName), L"%s_crash_%d%.2d%.2d_%.2d%.2d%.2d_%d.mdmp",
			strippedModuleName ? strippedModuleName : L"unknown",
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

	if (SUCCEEDED(errorCode) && minidumWriteDump)
	{
		// Dump the exception information into the file.
		MINIDUMP_EXCEPTION_INFORMATION exInfo;
		exInfo.ThreadId = GetCurrentThreadId();
		exInfo.ExceptionPointers = exceptionInfo;
		exInfo.ClientPointers = FALSE;

		const BOOL wasWrittenMinidump{minidumWriteDump(
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

// Determines either debugger attached to process or not.
bool HasDebuggerPresent()
{
	return ::IsDebuggerPresent() != FALSE;
}

#endif  // _WIN32
#endif  // HLTV_FIXES || LAUNCHER_FIXES
