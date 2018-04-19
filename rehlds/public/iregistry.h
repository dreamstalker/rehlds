//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#if !defined( UTIL_REGISTRY_H )
#define UTIL_REGISTRY_H
#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Interface to registry
//-----------------------------------------------------------------------------
class IRegistry
{
public:
	// Init/shutdown
	virtual void			Init(void) = 0;
	virtual void			Shutdown(void) = 0;

	// Read/write integers
	virtual int				ReadInt(const char *key, int defaultValue = 0) = 0;
	virtual void			WriteInt(const char *key, int value) = 0;

	// Read/write strings
	virtual const char		*ReadString(const char *key, const char *defaultValue = NULL) = 0;
	virtual void			WriteString(const char *key, const char *value) = 0;
};

extern IRegistry *registry;

#endif // UTIL_REGISTRY_H
