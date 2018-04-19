/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#include "precompiled.h"

// Hooks stuff
#include "hookers/memory.cpp"
#include "hookers/hooker.cpp"

//#define Mem_region
//#define BaseFileSystem_region
//#define CUtlSymbol_Region
//#define Function_References_region
//#define Data_References_region

FunctionHook g_FunctionHooks[] =
{
	// DO NOT DISABLE, other functions depends on memory allocation routines
#ifndef Mem_region

#ifdef _WIN32
	HOOK_SYMBOLDEF(0x01D06F5E, "_malloca", malloc_wrapper),
	HOOK_SYMBOLDEF(0x01D067CA, "_free", free_wrapper),
	HOOK_SYMBOLDEF(0x01D07098, "realloc", realloc_wrapper),
	HOOK_SYMBOLDEF(0x01D06F70, "__nh_malloc", __nh_malloc_wrapper),
#endif //_WIN32

#endif // Mem_region

#ifndef BaseFileSystem_region

	HOOK_DEF(0x01D01080, MethodThunk<CFileSystem_Stdio>::Destructor),
	HOOK_DEF(0x01D01010, (MethodThunk<CFileSystem_Stdio>::Constructor)),

	// virtual functions - CFileSystem_Stdio
	HOOK_VIRTUAL_DEF(0x01D05340, CFileSystem_Stdio::Mount),
	HOOK_VIRTUAL_DEF(0x01D011C0, CFileSystem_Stdio::Unmount),
	HOOK_VIRTUAL_DEF(0x01D05510, CFileSystem_Stdio::GetLocalCopy),
	HOOK_VIRTUAL_DEF(0x01D05520, CFileSystem_Stdio::LogLevelLoadStarted),
	HOOK_VIRTUAL_DEF(0x01D05530, CFileSystem_Stdio::LogLevelLoadFinished),
	HOOK_VIRTUAL_DEF(0x01D05540, CFileSystem_Stdio::HintResourceNeed),
	HOOK_VIRTUAL_DEF(0x01D05550, CFileSystem_Stdio::PauseResourcePreloading),
	HOOK_VIRTUAL_DEF(0x01D05560, CFileSystem_Stdio::ResumeResourcePreloading),
	HOOK_VIRTUAL_DEF(0x01D05570, CFileSystem_Stdio::SetVBuf),
	HOOK_VIRTUAL_DEF(0x01D055B0, CFileSystem_Stdio::GetInterfaceVersion),
	HOOK_VIRTUAL_DEF(0x01D055D0, CFileSystem_Stdio::WaitForResources),
	HOOK_VIRTUAL_DEF(0x01D055E0, CFileSystem_Stdio::GetWaitForResourcesProgress),
	HOOK_VIRTUAL_DEF(0x01D05600, CFileSystem_Stdio::CancelWaitForResources),
	HOOK_VIRTUAL_DEF(0x01D05610, CFileSystem_Stdio::IsAppReadyForOfflinePlay),

	// virtual functions - BaseFileSystem
	HOOK_SYMBOL_VIRTUAL_EX(0x01D01AF0, CFileSystem_Stdio, Open),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D01CC0, CFileSystem_Stdio, OpenFromCacheForRead),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D01D50, CFileSystem_Stdio, Close),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D01DD0, CFileSystem_Stdio, Seek),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D01EA0, CFileSystem_Stdio, Tell),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D01EF0, CFileSystem_Stdio, Size, size_t (FileHandle_t)),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D01F30, CFileSystem_Stdio, Size, size_t (const char *)),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D02310, CFileSystem_Stdio, IsOk),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D02370, CFileSystem_Stdio, Flush),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D02150, CFileSystem_Stdio, EndOfFile),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D021E0, CFileSystem_Stdio, Read),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D02240, CFileSystem_Stdio, Write),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D023B0, CFileSystem_Stdio, ReadLine),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D02290, CFileSystem_Stdio, FPrintf),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D022F0, CFileSystem_Stdio, GetReadBuffer),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D02300, CFileSystem_Stdio, ReleaseReadBuffer),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D012B0, CFileSystem_Stdio, GetCurrentDirectory),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D012B0, CFileSystem_Stdio, GetCurrentDirectory),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D011D0, CFileSystem_Stdio, CreateDirHierarchy),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D01680, CFileSystem_Stdio, IsDirectory),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D024A0, CFileSystem_Stdio, GetLocalPath),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D02400, CFileSystem_Stdio, RemoveFile),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D042B0, CFileSystem_Stdio, RemoveAllSearchPaths),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D01320, CFileSystem_Stdio, AddSearchPath),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D014E0, CFileSystem_Stdio, RemoveSearchPath),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D01AA0, CFileSystem_Stdio, FileExists),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D04000, CFileSystem_Stdio, GetFileTime),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D04060, CFileSystem_Stdio, FileTimeToString),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D036B0, CFileSystem_Stdio, FindFirst),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D03DB0, CFileSystem_Stdio, FindNext),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D03DF0, CFileSystem_Stdio, FindIsDirectory),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D03E10, CFileSystem_Stdio, FindClose),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D03EF0, CFileSystem_Stdio, ParseFile),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D04130, CFileSystem_Stdio, FullPathToRelativePath),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D012A0, CFileSystem_Stdio, PrintOpenedFiles),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D04090, CFileSystem_Stdio, SetWarningFunc),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D040A0, CFileSystem_Stdio, SetWarningLevel),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D02A20, CFileSystem_Stdio, AddPackFile),
	HOOK_SYMBOL_VIRTUAL_EX(0x01D01300, CFileSystem_Stdio, AddSearchPathNoWrite),

	// non-virtual functions - BaseFileSystem
	HOOK_DEF(0x01D02780, CBaseFileSystem::Trace_FOpen),
	HOOK_DEF(0x01D028A0, CBaseFileSystem::Trace_FClose),
	HOOK_DEF(0x01D029E0, CBaseFileSystem::Trace_DumpUnclosedFiles),
	HOOK_DEF(0x01D01340, CBaseFileSystem::AddSearchPathInternal),
	HOOK_DEF(0x01D040B0, CBaseFileSystem::Warning),
	HOOK_DEF(0x01D04350, CBaseFileSystem::FixSlashes),
	HOOK_DEF(0x01D04370, CBaseFileSystem::FixPath),
	//HOOK_DEF(0x01D04320, CBaseFileSystem::StripFilename),			// NOXREF
	HOOK_DEF(0x01D01610, CBaseFileSystem::GetWritePath),
	HOOK_DEF(0x01D01790, CBaseFileSystem::FindFile),
	HOOK_DEF(0x01D01F70, CBaseFileSystem::FastFindFileSize),
	//HOOK_DEF(0x01D034B0, CBaseFileSystem::RemoveAllMapSearchPaths),	// NOXREF
	HOOK_DEF(0x01D033A0, CBaseFileSystem::AddPackFiles),
	HOOK_DEF(0x01D02A40, CBaseFileSystem::AddPackFileFromPath),
	HOOK_DEF(0x01D02D20, CBaseFileSystem::PreparePackFile),
	HOOK_DEF(0x01D03070, CBaseFileSystem::Prepare64BitPackFile),
	HOOK_DEF(0x01D03510, CBaseFileSystem::SearchPakFile),
	//HOOK_DEF(0x01D039B0, CBaseFileSystem::FileInSearchPaths),		// NOXREF
	HOOK_DEF(0x01D03B70, CBaseFileSystem::FindNextFileHelper),
	HOOK_DEF(0x01D03890, CBaseFileSystem::FindFirstHelper),

#endif // BaseFileSystem_region

#ifndef CUtlSymbol_Region

	//HOOK_DEF(0x01D05AA0, MethodThunk<CUtlSymbolTable>::Destructor),	// NOXREF
	HOOK_DEF(0x01D05A30, (MethodThunk<CUtlSymbolTable, int, int, bool>::Constructor), void(int, int, bool)),
	HOOK_DEF(0x01D05C50, CUtlSymbolTable::AddString),
	HOOK_DEF(0x01D05B10, CUtlSymbolTable::Find),
	HOOK_DEF(0x01D05DA0, CUtlSymbolTable::String),
	//HOOK_DEF(0x0, CUtlSymbolTable::RemoveAll),	// NOXREF
	HOOK_DEF(0x01D05960, CUtlSymbolTable::SymLess),
	HOOK_DEF(0x01D059E0, CUtlSymbolTable::SymLessi),

	HOOK_DEF(0x01D05890, (MethodThunk<CUtlSymbol, const char *>::Constructor), void(const char *)),
	HOOK_DEF(0x01D05830, CUtlSymbol::Initialize),	// Don't touch it
	HOOK_DEF(0x01D05880, CUtlSymbol::CurrTable),	// Don't touch it
	HOOK_DEF(0x01D058C0, CUtlSymbol::String),
	//HOOK_DEF(0x01D058E0, CUtlSymbol::operator==, bool (const char *) const),	// NOXREF

#endif // CUtlSymbol_Region

	{ NULL, NULL, NULL },
};

AddressRef g_FunctionRefs[] =
{
#ifndef Function_References_region

#endif // Function_References_region

	{ NULL, NULL, NULL },
};

AddressRef g_DataRefs[] =
{
#ifndef Data_References_region

	//GLOBALVAR_LINK(0x01D1B020, "CBaseFileSystem::s_pFileSystem", CBaseFileSystem::s_pFileSystem),
	GLOBALVAR_LINK(0x01D1B0B8, "g_LessCtx", pg_LessCtx),

#endif // Data_References_region

	{ NULL, NULL, NULL },
};
