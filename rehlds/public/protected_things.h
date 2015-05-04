//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef PROTECTED_THINGS_H
#define PROTECTED_THINGS_H
#ifdef _WIN32
#pragma once
#endif


// This header tries to prevent people from using potentially dangerous functions
// (like the notorious non-null-terminating strncpy) and functions that will break
// VCR mode (like time, input, registry, etc).
//
// This header should be included by ALL of our source code.

// Eventually, ALL of these should be protected, but one man can only accomplish so much in
// one day AND work on features too!
#if defined( PROTECT_STRING_FUNCTIONS )
#if defined( strncpy )
#undef strncpy
#endif
#define strncpy				strncpy__HEY_YOU__USE_VSTDLIB


#if defined( _snprintf )
#undef _snprintf
#endif
#define _snprintf			snprintf__HEY_YOU__USE_VSTDLIB


#if defined( sprintf )
#undef sprintf
#endif
#define sprintf				sprintf__HEY_YOU__USE_VSTDLIB


#if defined( _vsnprintf )
#undef _vsnprintf
#endif
#define _vsnprintf			vsnprintf__HEY_YOU__USE_VSTDLIB


#if defined( strcat )
#undef strcat
#endif
#define strcat				strcat__HEY_YOU__USE_VSTDLIB
#endif


#if defined( PROTECT_FILEIO_FUNCTIONS )
#if defined( fopen )
#undef fopen
#endif
#define fopen				fopen_USE_FILESYSTEM_INSTEAD
#endif	


#if defined( PROTECTED_THINGS_ENABLE )

#if defined( GetTickCount )
#undef GetTickCount
#endif
#define GetTickCount		GetTickCount__HEY_YOU__USE_PLATFORM_LIB


#if defined( timeGetTime )
#undef timeGetTime
#endif
#define timeGetTime			timeGetTime__HEY_YOU__USE_PLATFORM_LIB


#if defined( clock )
#undef clock
#endif
#define time				time__HEY_YOU__USE_PLATFORM_LIB


#if defined( recvfrom )
#undef recvfrom
#endif
#define recvfrom			recvfrom__HEY_YOU__USE_PLATFORM_LIB


#if defined( GetCursorPos )
#undef GetCursorPos
#endif
#define GetCursorPos		GetCursorPos__HEY_YOU__USE_PLATFORM_LIB


#if defined( ScreenToClient )
#undef ScreenToClient
#endif
#define ScreenToClient		ScreenToClient__HEY_YOU__USE_PLATFORM_LIB


#if defined( GetCommandLine )
#undef GetCommandLine
#endif
#define GetCommandLine		GetCommandLine__HEY_YOU__USE_PLATFORM_LIB


#if defined( RegOpenKeyEx )
#undef RegOpenKeyEx
#endif
#define RegOpenKeyEx		RegOpenKeyEx__HEY_YOU__USE_PLATFORM_LIB


#if defined( RegOpenKey )
#undef RegOpenKey
#endif
#define RegOpenKey			RegOpenKey__HEY_YOU__USE_PLATFORM_LIB


#if defined( RegSetValueEx )
#undef RegSetValueEx
#endif
#define RegSetValueEx		RegSetValueEx__HEY_YOU__USE_PLATFORM_LIB


#if defined( RegSetValue )
#undef RegSetValue
#endif
#define RegSetValue			RegSetValue__HEY_YOU__USE_PLATFORM_LIB


#if defined( RegQueryValueEx )
#undef RegQueryValueEx
#endif		
#define RegQueryValueEx		RegQueryValueEx__HEY_YOU__USE_PLATFORM_LIB


#if defined( RegQueryValue )
#undef RegQueryValue
#endif		
#define RegQueryValue		RegQueryValue__HEY_YOU__USE_PLATFORM_LIB


#if defined( RegCreateKeyEx )
#undef RegCreateKeyEx
#endif
#define RegCreateKeyEx		RegCreateKeyEx__HEY_YOU__USE_PLATFORM_LIB


#if defined( RegCreateKey )
#undef RegCreateKey
#endif
#define RegCreateKey		RegCreateKey__HEY_YOU__USE_PLATFORM_LIB


#if defined( RegCloseKey )
#undef RegCloseKey
#endif
#define RegCloseKey			RegCloseKey__HEY_YOU__USE_PLATFORM_LIB


#if defined( GetNumberOfConsoleInputEvents )
#undef GetNumberOfConsoleInputEvents
#endif
#define GetNumberOfConsoleInputEvents	GetNumberOfConsoleInputEvents__HEY_YOU__USE_PLATFORM_LIB


#if defined( ReadConsoleInput )
#undef ReadConsoleInput
#endif
#define ReadConsoleInput	ReadConsoleInput__HEY_YOU__USE_PLATFORM_LIB


#if defined( GetAsyncKeyState )
#undef GetAsyncKeyState
#endif
#define GetAsyncKeyState	GetAsyncKeyState__HEY_YOU__USE_PLATFORM_LIB


#if defined( GetKeyState )
#undef GetKeyState
#endif
#define GetKeyState			GetKeyState__HEY_YOU__USE_PLATFORM_LIB

#endif


#endif // PROTECTED_THINGS_H
