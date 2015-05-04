//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ==========
//	
// Purpose: Defines the more complete set of operations on the string_t defined
// 			These should be used instead of direct manipulation to allow more
//			flexibility in future ports or optimization.
//
// $NoKeywords: $
//=============================================================================

#ifndef STRING_T_H
#define STRING_T_H

#if defined( _WIN32 )
#pragma once
#endif

#include "osconfig.h"
#include <stdio.h>

#ifndef NO_STRING_T

#ifdef WEAK_STRING_T

typedef int	valve_string_t;

//-----------------------------------------------------------------------------
// Purpose: The correct way to specify the NULL string as a constant.
//-----------------------------------------------------------------------------

#define NULL_STRING			0

//-----------------------------------------------------------------------------
// Purpose: Given a string_t, make a C string. By convention the result string 
// 			pointer should be considered transient and should not be stored.
//-----------------------------------------------------------------------------

#define STRING( offset )	( ( offset ) ? reinterpret_cast<const char *>( offset ) : "" )

//-----------------------------------------------------------------------------
// Purpose: Given a C string, obtain a string_t
//-----------------------------------------------------------------------------

#define MAKE_STRING( str )	( ( *str != 0 ) ? reinterpret_cast<int>( str ) : 0 )

//-----------------------------------------------------------------------------

#else // Strong string_t

//-----------------------------------------------------------------------------

struct valve_string_t
{
public:
	bool operator!() const							{ return (pszValue == NULL); }
	bool operator==(const valve_string_t &rhs) const	{ return (pszValue == rhs.pszValue); }
	bool operator!=(const valve_string_t &rhs) const	{ return (pszValue != rhs.pszValue); }

	const char *ToCStr() const						{ return (pszValue) ? pszValue : ""; }

protected:
	const char *pszValue;
};

//-----------------------------------------------------------------------------

struct castable_string_t : public valve_string_t // string_t is used in unions, hence, no constructor allowed
{
	castable_string_t()							{ pszValue = NULL; }
	castable_string_t(const char *pszFrom)	{ pszValue = (*pszFrom != 0) ? pszFrom : 0; }
};

//-----------------------------------------------------------------------------
// Purpose: The correct way to specify the NULL string as a constant.
//-----------------------------------------------------------------------------

#define NULL_STRING			castable_string_t()

//-----------------------------------------------------------------------------
// Purpose: Given a string_t, make a C string. By convention the result string 
// 			pointer should be considered transient and should not be stored.
//-----------------------------------------------------------------------------

#define STRING( string_t_obj )	(string_t_obj).ToCStr()

//-----------------------------------------------------------------------------
// Purpose: Given a C string, obtain a string_t
//-----------------------------------------------------------------------------

#define MAKE_STRING( c_str )	castable_string_t( c_str )

//-----------------------------------------------------------------------------

#endif

#else	// NO_STRING_T

typedef const char *string_t;
#define NULL_STRING				0
#define STRING( c_str )			( c_str )
#define MAKE_STRING( c_str )	( c_str )

#endif	// NO_STRING_T

//=============================================================================

#endif // STRING_T_H
