//========= Copyright  1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#if !defined( SAVEGAME_VERSION_H )
#define SAVEGAME_VERSION_H
#ifdef _WIN32
#pragma once
#endif

#define SAVEFILE_HEADER		MAKEID('V','A','L','V')		// little-endian "VALV"
#define SAVEGAME_HEADER		MAKEID('J','S','A','V')		// little-endian "JSAV"
#define SAVEGAME_VERSION	0x0071				// Version 0.71

#endif // SAVEGAME_VERSION_H
