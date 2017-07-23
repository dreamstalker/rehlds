//=========== (C) Copyright 1999 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// $Header: $
// $NoKeywords: $
//
// The main debug library interfaces
//=============================================================================


#ifndef DBG_H
#define DBG_H

#ifdef _WIN32
#pragma once
#endif

#include "basetypes.h"

#include <math.h>
#include <stdio.h>
#include <stdarg.h>


//-----------------------------------------------------------------------------
// dll export stuff
//-----------------------------------------------------------------------------

#ifdef TIER0_DLL_EXPORT
#define DBG_INTERFACE	DLL_EXPORT
#define DBG_OVERLOAD	DLL_GLOBAL_EXPORT
#define DBG_CLASS		DLL_CLASS_EXPORT
#else
#define DBG_INTERFACE	DLL_IMPORT
#define DBG_OVERLOAD	DLL_GLOBAL_IMPORT
#define DBG_CLASS		DLL_CLASS_IMPORT
#endif


//-----------------------------------------------------------------------------
// Usage model for the Dbg library
//
// 1. Spew.
// 
//   Spew can be used in a static and a dynamic mode. The static
//   mode allows us to display assertions and other messages either only
//   in debug builds, or in non-release builds. The dynamic mode allows us to
//   turn on and off certain spew messages while the application is running.
// 
//   Static Spew messages:
//
//     Assertions are used to detect and warn about invalid states
//     Spews are used to display a particular status/warning message.
//
//     To use an assertion, use
//
//     Assert( (f == 5) );
//     AssertMsg( (f == 5), ("F needs to be %d here!\n", 5) );
//     AssertFunc( (f == 5), BadFunc() );
//     AssertEquals( f, 5 );
//     AssertFloatEquals( f, 5.0f, 1e-3 );
//
//     The first will simply report that an assertion failed on a particular
//     code file and line. The second version will display a print-f formatted message 
//	   along with the file and line, the third will display a generic message and
//     will also cause the function BadFunc to be executed, and the last two
//	   will report an error if f is not equal to 5 (the last one asserts within
//	   a particular tolerance).
//
//     To use a warning, use
//      
//     Warning("Oh I feel so %s all over\n", "yummy");
//
//     Warning will do its magic in only Debug builds. To perform spew in *all*
//     builds, use RelWarning.
//
//	   Three other spew types, Msg, Log, and Error, are compiled into all builds.
//	   These error types do *not* need two sets of parenthesis.
//
//	   Msg( "Isn't this exciting %d?", 5 );
//	   Error( "I'm just thrilled" );
//
//   Dynamic Spew messages
//
//     It is possible to dynamically turn spew on and off. Dynamic spew is 
//     identified by a spew group and priority level. To turn spew on for a 
//     particular spew group, use SpewActivate( "group", level ). This will 
//     cause all spew in that particular group with priority levels <= the 
//     level specified in the SpewActivate function to be printed. Use DSpew 
//     to perform the spew:
//
//     DWarning( "group", level, "Oh I feel even yummier!\n" );
//
//     Priority level 0 means that the spew will *always* be printed, and group
//     '*' is the default spew group. If a DWarning is encountered using a group 
//     whose priority has not been set, it will use the priority of the default 
//     group. The priority of the default group is initially set to 0.      
//
//   Spew output
//   
//     The output of the spew system can be redirected to an externally-supplied
//     function which is responsible for outputting the spew. By default, the 
//     spew is simply printed using printf.
//
//     To redirect spew output, call SpewOutput.
//
//     SpewOutputFunc( OutputFunc );
//
//     This will cause OutputFunc to be called every time a spew message is
//     generated. OutputFunc will be passed a spew type and a message to print.
//     It must return a value indicating whether the debugger should be invoked,
//     whether the program should continue running, or whether the program 
//     should abort. 
//
// 2. Code activation
//
//   To cause code to be run only in debug builds, use DBG_CODE:
//   An example is below.
//
//   DBG_CODE(
//				{
//					int x = 5;
//					++x;
//				}
//           ); 
//
//   Code can be activated based on the dynamic spew groups also. Use
//  
//   DBG_DCODE( "group", level,
//              { int x = 5; ++x; }
//            );
//
// 3. Breaking into the debugger.
//
//   To cause an unconditional break into the debugger in debug builds only, use DBG_BREAK
//
//   DBG_BREAK();
//
//	 You can force a break in any build (release or debug) using
//
//	 DebuggerBreak();
//-----------------------------------------------------------------------------

/* Various types of spew messages */
// I'm sure you're asking yourself why SPEW_ instead of DBG_ ?
// It's because DBG_ is used all over the place in windows.h
// For example, DBG_CONTINUE is defined. Feh.
enum SpewType_t
{
	SPEW_MESSAGE = 0,
	SPEW_WARNING,
	SPEW_ASSERT,
	SPEW_ERROR,
	SPEW_LOG,

	SPEW_TYPE_COUNT
};

enum SpewRetval_t
{
	SPEW_DEBUGGER = 0,
	SPEW_CONTINUE,
	SPEW_ABORT
};

/* type of externally defined function used to display debug spew */
typedef SpewRetval_t(*SpewOutputFunc_t)(SpewType_t spewType, char const *pMsg);

/* Used to redirect spew output */
void   SpewOutputFunc(SpewOutputFunc_t func);

/* Used ot get the current spew output function */
SpewOutputFunc_t GetSpewOutputFunc(void);

/* Used to manage spew groups and subgroups */
void   SpewActivate(char const* pGroupName, int level);
bool   IsSpewActive(char const* pGroupName, int level);

/* Used to display messages, should never be called directly. */
void   _SpewInfo(SpewType_t type, char const* pFile, int line);
SpewRetval_t   _SpewMessage(char const* pMsg, ...);
SpewRetval_t   _DSpewMessage(char const *pGroupName, int level, char const* pMsg, ...);

/* Used to define macros, never use these directly. */
#define  _Assert( _exp )			do {															\
									if (!(_exp)) 													\
																											{ 																\
										_SpewInfo( SPEW_ASSERT, __FILE__, __LINE__ );				\
										if (_SpewMessage("Assertion Failed: " #_exp) == SPEW_DEBUGGER)		\
																														{															\
											DebuggerBreak();										\
																														}															\
																											}																\
									} while (0)

#define  _AssertMsg( _exp, _msg )	do {															\
									if (!(_exp)) 													\
																											{ 																\
										_SpewInfo( SPEW_ASSERT, __FILE__, __LINE__ );				\
										if (_SpewMessage(_msg) == SPEW_DEBUGGER)					\
																														{															\
											DebuggerBreak();										\
																														}															\
																											}																\
									} while (0)

#define  _AssertFunc( _exp, _f )	do {															\
									if (!(_exp)) 													\
																											{ 																\
										_SpewInfo( SPEW_ASSERT, __FILE__, __LINE__ );				\
										SpewRetval_t ret = _SpewMessage("Assertion Failed!" #_exp);	\
										_f;															\
										if (ret == SPEW_DEBUGGER)									\
																														{															\
											DebuggerBreak();										\
																														}															\
																											}																\
									} while (0)

#define  _AssertEquals( _exp, _expectedValue ) \
									do {															\
									if ((_exp) != (_expectedValue)) 								\
																											{ 																\
										_SpewInfo( SPEW_ASSERT, __FILE__, __LINE__ );				\
										SpewRetval_t ret = _SpewMessage("Expected %d but got %d!", (_expectedValue), (_exp));	\
										if (ret == SPEW_DEBUGGER)									\
																														{															\
											DebuggerBreak();										\
																														}															\
																											}																\
																											} while (0)

#define  _AssertFloatEquals( _exp, _expectedValue, _tol ) \
									do {															\
									if (fabs((_exp) - (_expectedValue)) > (_tol))					\
																											{ 																\
										_SpewInfo( SPEW_ASSERT, __FILE__, __LINE__ );				\
										SpewRetval_t ret = _SpewMessage("Expected %f but got %f!", (_expectedValue), (_exp));	\
										if (ret == SPEW_DEBUGGER)									\
																														{															\
											DebuggerBreak();										\
																														}															\
																											}																\
																											} while (0)

/* Spew macros... */

#ifdef _DEBUG

#define  Assert( _exp )           _Assert( _exp )
#define  AssertMsg( _exp, _msg )  _AssertMsg( _exp, _msg )
#define  AssertFunc( _exp, _f )   _AssertFunc( _exp, _f )
#define  AssertEquals( _exp, _expectedValue )              _AssertEquals( _exp, _expectedValue ) 
#define  AssertFloatEquals( _exp, _expectedValue, _tol )   _AssertFloatEquals( _exp, _expectedValue, _tol )
#define  Verify( _exp )           _Assert( _exp )

#define  AssertMsg1( _exp, _msg, a1 )									_AssertMsg( _exp, CDbgFmtMsg( _msg, a1 ) )
#define  AssertMsg2( _exp, _msg, a1, a2 )								_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2 ) )
#define  AssertMsg3( _exp, _msg, a1, a2, a3 )							_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3 ) )
#define  AssertMsg4( _exp, _msg, a1, a2, a3, a4 )						_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4 ) )
#define  AssertMsg5( _exp, _msg, a1, a2, a3, a4, a5 )					_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4, a5 ) )
#define  AssertMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6 ) )
#define  AssertMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6 ) )
#define  AssertMsg7( _exp, _msg, a1, a2, a3, a4, a5, a6, a7 )			_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6, a7 ) )
#define  AssertMsg8( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8 )		_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6, a7, a8 ) )
#define  AssertMsg9( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8, a9 )	_AssertMsg( _exp, CDbgFmtMsg( _msg, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) )


#else /* Not _DEBUG */

#define  Assert( _exp )           ((void)0)
#define  AssertMsg( _exp, _msg )  ((void)0)
#define  AssertFunc( _exp, _f )   ((void)0)
#define  AssertEquals( _exp, _expectedValue )              ((void)0)
#define  AssertFloatEquals( _exp, _expectedValue, _tol )   ((void)0)
#define  Verify( _exp )			  (_exp)

#define  AssertMsg1( _exp, _msg, a1 )									((void)0)
#define  AssertMsg2( _exp, _msg, a1, a2 )								((void)0)
#define  AssertMsg3( _exp, _msg, a1, a2, a3 )							((void)0)
#define  AssertMsg4( _exp, _msg, a1, a2, a3, a4 )						((void)0)
#define  AssertMsg5( _exp, _msg, a1, a2, a3, a4, a5 )					((void)0)
#define  AssertMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				((void)0)
#define  AssertMsg6( _exp, _msg, a1, a2, a3, a4, a5, a6 )				((void)0)
#define  AssertMsg7( _exp, _msg, a1, a2, a3, a4, a5, a6, a7 )			((void)0)
#define  AssertMsg8( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8 )		((void)0)
#define  AssertMsg9( _exp, _msg, a1, a2, a3, a4, a5, a6, a7, a8, a9 )	((void)0)

#endif   /* _DEBUG */



/* These are always compiled in */
void Msg(char const* pMsg, ...);
void DMsg(char const *pGroupName, int level, char const *pMsg, ...);

void Warning(char const *pMsg, ...);
void DWarning(char const *pGroupName, int level, char const *pMsg, ...);

void Log(char const *pMsg, ...);
void DLog(char const *pGroupName, int level, char const *pMsg, ...);

void Error(char const *pMsg, ...);

// You can use this macro like a runtime assert macro.
// If the condition fails, then Error is called with the message. This macro is called
// like AssertMsg, where msg must be enclosed in parenthesis:
//
// ErrorIfNot( bCondition, ("a b c %d %d %d", 1, 2, 3) );
#define ErrorIfNot( condition, msg ) \
	if ( (condition) )		\
		;					\
				else 					\
	{						\
		Error msg;			\
	}

/* A couple of super-common dynamic spew messages, here for convenience */
/* These looked at the "developer" group */
void DevMsg(int level, char const* pMsg, ...);
void DevWarning(int level, char const *pMsg, ...);
void DevLog(int level, char const *pMsg, ...);

/* default level versions (level 1) */
void DevMsg(char const* pMsg, ...);
void DevWarning(char const *pMsg, ...);
void DevLog(char const *pMsg, ...);

/* Code macros, debugger interface */

#ifdef _DEBUG

#define DBG_CODE( _code )            if (0) ; else { _code }
#define DBG_DCODE( _g, _l, _code )   if (IsSpewActive( _g, _l )) { _code } else {}
#define DBG_BREAK()                  DebuggerBreak()	/* defined in platform.h */ 

#else /* not _DEBUG */

#define DBG_CODE( _code )            ((void)0)
#define DBG_DCODE( _g, _l, _code )   ((void)0)
#define DBG_BREAK()                  ((void)0)

#endif /* _DEBUG */

//-----------------------------------------------------------------------------
// Macro to assist in asserting constant invariants during compilation

#define UID_PREFIX generated_id_
#define UID_CAT1(a,c) a ## c
#define UID_CAT2(a,c) UID_CAT1(a,c)
#define UNIQUE_ID UID_CAT2(UID_PREFIX,__LINE__)


#ifdef _DEBUG
#define COMPILE_TIME_ASSERT( pred )	switch(0){case 0:case pred:;}
#define ASSERT_INVARIANT( pred )	static void UNIQUE_ID() { COMPILE_TIME_ASSERT( pred ) }
#else
#define COMPILE_TIME_ASSERT( pred )
#define ASSERT_INVARIANT( pred )
#endif


//-----------------------------------------------------------------------------
// Templates to assist in validating pointers:

// Have to use these stubs so we don't have to include windows.h here.
void _AssertValidReadPtr(void* ptr, int count = 1);
void _AssertValidWritePtr(void* ptr, int count = 1);
void _AssertValidReadWritePtr(void* ptr, int count = 1);

 void AssertValidStringPtr(const char* ptr, int maxchar = 0xFFFFFF);
template<class T> inline void AssertValidReadPtr(T* ptr, int count = 1)		     { _AssertValidReadPtr((void*)ptr, count); }
template<class T> inline void AssertValidWritePtr(T* ptr, int count = 1)		     { _AssertValidWritePtr((void*)ptr, count); }
template<class T> inline void AssertValidReadWritePtr(T* ptr, int count = 1)	     { _AssertValidReadWritePtr((void*)ptr, count); }

#define AssertValidThis() AssertValidReadWritePtr(this,sizeof(*this))

//-----------------------------------------------------------------------------
// Macro to protect functions that are not reentrant

#ifdef _DEBUG
class CReentryGuard
{
public:
	CReentryGuard(int *pSemaphore)
		: m_pSemaphore(pSemaphore)
	{
		++(*m_pSemaphore);
	}

	~CReentryGuard()
	{
		--(*m_pSemaphore);
	}

private:
	int *m_pSemaphore;
};

#define ASSERT_NO_REENTRY() \
	static int fSemaphore##__LINE__; \
	Assert( !fSemaphore##__LINE__ ); \
	CReentryGuard ReentryGuard##__LINE__( &fSemaphore##__LINE__ )
#else
#define ASSERT_NO_REENTRY()
#endif

//-----------------------------------------------------------------------------
//
// Purpose: Inline string formatter
//

class CDbgFmtMsg
{
public:
	CDbgFmtMsg(const char *pszFormat, ...)
	{
		va_list arg_ptr;

		va_start(arg_ptr, pszFormat);
		_vsnprintf(m_szBuf, sizeof(m_szBuf) - 1, pszFormat, arg_ptr);
		va_end(arg_ptr);

		m_szBuf[sizeof(m_szBuf) - 1] = 0;
	}

	operator const char *() const
	{
		return m_szBuf;
	}

private:
	char m_szBuf[256];
};

//-----------------------------------------------------------------------------
//
// Purpose: Embed debug info in each file.
//

//#ifdef _WIN32
//#ifdef _DEBUG
//#pragma comment(compiler)
//#pragma comment(exestr,"*** DEBUG file detected, Last Compile: " __DATE__ ", " __TIME__ " ***")
//#endif
//#endif

#endif /* DBG_H */
