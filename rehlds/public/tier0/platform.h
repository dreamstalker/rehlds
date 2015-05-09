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
// Extremely low-level platform-specific stuff
//=============================================================================


#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#pragma once
#endif

#include "osconfig.h"

// need this for _alloca
#include <malloc.h>

// need this for memset
#include <string.h>

#include "archtypes.h"

typedef float  float32;
typedef double float64;

// for when we don't care about how many bits we use
typedef unsigned int uint;

// This can be used to ensure the size of pointers to members when declaring
// a pointer type for a class that has only been forward declared
#ifdef _MSC_VER
#define SINGLE_INHERITANCE __single_inheritance 
#define MULTIPLE_INHERITANCE __multiple_inheritance 
#else
#define SINGLE_INHERITANCE  
#define MULTIPLE_INHERITANCE 
#endif

/*
FIXME: Enable this when we no longer fear change =)

// need these for the limits
#include <limits.h>
#include <float.h>

// Maximum and minimum representable values
#define  INT8_MAX    SCHAR_MAX
#define  INT16_MAX   SHRT_MAX
#define  INT32_MAX   LONG_MAX
#define  INT64_MAX   (((int64)~0) >> 1)

#define  INT8_MIN    SCHAR_MIN
#define  INT16_MIN   SHRT_MIN
#define  INT32_MIN   LONG_MIN
#define  INT64_MIN   (((int64)1) << 63)

#define  UINT8_MAX   ((uint8)~0)
#define  UINT16_MAX  ((uint16)~0)
#define  UINT32_MAX  ((uint32)~0)
#define  UINT64_MAX  ((uint64)~0)

#define  UINT8_MIN   0
#define  UINT16_MIN  0
#define  UINT32_MIN  0
#define  UINT64_MIN  0

#ifndef  UINT_MIN
#define  UINT_MIN    UINT32_MIN
#endif

#define  FLOAT32_MAX FLT_MAX
#define  FLOAT64_MAX DBL_MAX

#define  FLOAT32_MIN FLT_MIN
#define  FLOAT64_MIN DBL_MIN
*/

// portability / compiler settings
#if defined(_WIN32) && !defined(WINDED)

#if defined(_M_IX86)
#define __i386__	1
#endif

#elif __linux__
typedef void * HINSTANCE;
#define _MAX_PATH PATH_MAX
#endif // defined(_WIN32) && !defined(WINDED)


// Defines MAX_PATH
#ifndef MAX_PATH
#define MAX_PATH  260
#endif

// Used to step into the debugger
#define  DebuggerBreak()  __asm { int 3 }

// C functions for external declarations that call the appropriate C++ methods
#ifndef EXPORT
#ifdef _WIN32
#define EXPORT	_declspec( dllexport )
#else 
#define EXPORT	/* */
#endif
#endif

#if defined __i386__ && !defined __linux__
#define id386	1
#else
#define id386	0
#endif  // __i386__

#ifdef _WIN32
// Used for dll exporting and importing
#define  DLL_EXPORT   extern "C" __declspec( dllexport ) 
#define  DLL_IMPORT   extern "C" __declspec( dllimport )

// Can't use extern "C" when DLL exporting a class
#define  DLL_CLASS_EXPORT   __declspec( dllexport ) 
#define  DLL_CLASS_IMPORT   __declspec( dllimport )

// Can't use extern "C" when DLL exporting a global
#define  DLL_GLOBAL_EXPORT   extern __declspec( dllexport ) 
#define  DLL_GLOBAL_IMPORT   extern __declspec( dllimport )
#elif defined __linux__

// Used for dll exporting and importing
#define  DLL_EXPORT   extern "C" 
#define  DLL_IMPORT   extern "C" 

// Can't use extern "C" when DLL exporting a class
#define  DLL_CLASS_EXPORT   
#define  DLL_CLASS_IMPORT  

// Can't use extern "C" when DLL exporting a global
#define  DLL_GLOBAL_EXPORT   extern
#define  DLL_GLOBAL_IMPORT   extern 

#else
#error "Unsupported Platform."
#endif

// Used for standard calling conventions
#ifdef _WIN32
#define  FASTCALL			   __fastcall
#define  FORCEINLINE		   __forceinline	
#else
#define  FASTCALL			   
#define  FORCEINLINE		   inline
#endif

// Force a function call site -not- to inlined. (useful for profiling)
#define DONT_INLINE(a) (((int)(a)+1)?(a):(a))

// Pass hints to the compiler to prevent it from generating unnessecary / stupid code
// in certain situations.  Several compilers other than MSVC also have an equivilent 
// construct.
//
// Essentially the 'Hint' is that the condition specified is assumed to be true at 
// that point in the compilation.  If '0' is passed, then the compiler assumes that
// any subsequent code in the same 'basic block' is unreachable, and thus usually 
// removed.
#ifdef _MSC_VER
#define HINT(THE_HINT)	__assume((THE_HINT))
#else
#define HINT(THE_HINT)	0
#endif

// Marks the codepath from here until the next branch entry point as unreachable,
// and asserts if any attempt is made to execute it.
#define UNREACHABLE() { Assert(0); HINT(0); }

// In cases where no default is present or appropriate, this causes MSVC to generate 
// as little code as possible, and throw an assertion in debug.
#define NO_DEFAULT default: UNREACHABLE();

#ifdef _WIN32
// Alloca defined for this platform
#define  stackalloc( _size ) _alloca( _size )
#define  stackfree( _p )   0
#elif __linux__
// Alloca defined for this platform
#define  stackalloc( _size ) alloca( _size )
#define  stackfree( _p )   0
#endif

#ifdef _WIN32
// Remove warnings from warning level 4.
#pragma warning(disable : 4514) // warning C4514: 'acosl' : unreferenced inline function has been removed
#pragma warning(disable : 4100) // warning C4100: 'hwnd' : unreferenced formal parameter
#pragma warning(disable : 4127) // warning C4127: conditional expression is constant
#pragma warning(disable : 4512) // warning C4512: 'InFileRIFF' : assignment operator could not be generated
#pragma warning(disable : 4611) // warning C4611: interaction between '_setjmp' and C++ object destruction is non-portable
#pragma warning(disable : 4706) // warning C4706: assignment within conditional expression
#pragma warning(disable : 4710) // warning C4710: function 'x' not inlined
#pragma warning(disable : 4702) // warning C4702: unreachable code
#pragma warning(disable : 4505) // unreferenced local function has been removed
#pragma warning(disable : 4239) // nonstandard extension used : 'argument' ( conversion from class Vector to class Vector& )
#pragma warning(disable : 4097) // typedef-name 'BaseClass' used as synonym for class-name 'CFlexCycler::CBaseFlex'
#pragma warning(disable : 4324) // Padding was added at the end of a structure
#pragma warning(disable : 4244) // type conversion warning.
#pragma warning(disable : 4305)	// truncation from 'const double ' to 'float '
#pragma warning(disable : 4786)	// Disable warnings about long symbol names

#if _MSC_VER >= 1300
#pragma warning(disable : 4511)	// Disable warnings about private copy constructors
#endif
#endif
//-----------------------------------------------------------------------------
// Purpose: Standard functions for handling endian-ness
//-----------------------------------------------------------------------------

//-------------------------------------
// Basic swaps
//-------------------------------------

template <typename T>
inline T WordSwapC(T w)
{
	uint16 temp;

	temp = ((*((uint16 *)&w) & 0xff00) >> 8);
	temp |= ((*((uint16 *)&w) & 0x00ff) << 8);

	return *((T*)&temp);
}

template <typename T>
inline T DWordSwapC(T dw)
{
	uint32 temp;

	temp = *((uint32 *)&dw) >> 24;
	temp |= ((*((uint32 *)&dw) & 0x00FF0000) >> 8);
	temp |= ((*((uint32 *)&dw) & 0x0000FF00) << 8);
	temp |= ((*((uint32 *)&dw) & 0x000000FF) << 24);

	return *((T*)&temp);
}

//-------------------------------------
// Fast swaps
//-------------------------------------

#ifdef _MSC_VER

#define WordSwap  WordSwapAsm
#define DWordSwap DWordSwapAsm

#pragma warning(push)
#pragma warning (disable:4035) // no return value

template <typename T>
inline T WordSwapAsm(T w)
{
	__asm
	{
		mov ax, w
			xchg al, ah
	}
}

template <typename T>
inline T DWordSwapAsm(T dw)
{
	__asm
	{
		mov eax, dw
			bswap eax
	}
}

#pragma warning(pop)

// The assembly implementation is not compatible with floats
template <>
inline float DWordSwapAsm<float>(float f)
{
	return DWordSwapC(f);
}

#else

#define WordSwap  WordSwapC
#define DWordSwap DWordSwapC

#endif

//-------------------------------------
// The typically used methods. 
//-------------------------------------

#if defined(__i386__)
#define VALVE_LITTLE_ENDIAN 1
#endif

#ifdef _SGI_SOURCE
#define	VALVE_BIG_ENDIAN 1
#endif

#if defined(VALVE_LITTLE_ENDIAN)

#define Valve_BigShort( val )		WordSwap( val )
#define Valve_BigWord( val )		WordSwap( val )
#define Valve_BigLong( val )		DWordSwap( val )
#define Valve_BigDWord( val )		DWordSwap( val )
#define Valve_BigFloat( val )		DWordSwap( val )
#define Valve_LittleShort( val )	( val )
#define Valve_LittleWord( val )	( val )
#define Valve_LittleLong( val )	( val )
#define Valve_LittleDWord( val )	( val )
#define Valve_LittleFloat( val )	( val )

#elif defined(BIG_ENDIAN)

#define Valve_BigShort( val )		( val )
#define Valve_BigWord( val )		( val )
#define Valve_BigLong( val )		( val )
#define Valve_BigDWord( val )		( val )
#define Valve_BigFloat( val )		( val )
#define Valve_LittleShort( val )	WordSwap( val )
#define Valve_LittleWord( val )	WordSwap( val )
#define Valve_LittleLong( val )	DWordSwap( val )
#define Valve_LittleDWord( val )	DWordSwap( val )
#define Valve_LittleFloat( val )	DWordSwap( val )

#else

// @Note (toml 05-02-02): this technique expects the compiler to 
// optimize the expression and eliminate the other path. On any new 
// platform/compiler this should be tested.
inline short BigShort(short val)		{ int test = 1; return (*(char *)&test == 1) ? WordSwap(val) : val; }
inline uint16 BigWord(uint16 val)		{ int test = 1; return (*(char *)&test == 1) ? WordSwap(val) : val; }
inline long BigLong(long val)			{ int test = 1; return (*(char *)&test == 1) ? DWordSwap(val) : val; }
inline uint32 BigDWord(uint32 val)	{ int test = 1; return (*(char *)&test == 1) ? DWordSwap(val) : val; }
inline float BigFloat(float val)		{ int test = 1; return (*(char *)&test == 1) ? DWordSwap(val) : val; }
inline short LittleShort(short val)	{ int test = 1; return (*(char *)&test == 1) ? val : WordSwap(val); }
inline uint16 LittleWord(uint16 val)	{ int test = 1; return (*(char *)&test == 1) ? val : WordSwap(val); }
inline long LittleLong(long val)		{ int test = 1; return (*(char *)&test == 1) ? val : DWordSwap(val); }
inline uint32 LittleDWord(uint32 val)	{ int test = 1; return (*(char *)&test == 1) ? val : DWordSwap(val); }
inline float LittleFloat(float val)	{ int test = 1; return (*(char *)&test == 1) ? val : DWordSwap(val); }

#endif



#ifdef TIER0_DLL_EXPORT
#define PLATFORM_INTERFACE	DLL_EXPORT
#define PLATFORM_OVERLOAD	DLL_GLOBAL_EXPORT
#else
#define PLATFORM_INTERFACE	DLL_IMPORT
#define PLATFORM_OVERLOAD	DLL_GLOBAL_IMPORT
#endif

/*
PLATFORM_INTERFACE double			Plat_FloatTime();		// Returns time in seconds since the module was loaded.
PLATFORM_INTERFACE unsigned long	Plat_MSTime();			// Time in milliseconds.

// b/w compatibility
#define Sys_FloatTime Plat_FloatTime
*/

// Processor Information:
struct CPUInformation
{
	int	 m_Size;		// Size of this structure, for forward compatability.

	bool m_bRDTSC : 1,	// Is RDTSC supported?
	m_bCMOV : 1,  // Is CMOV supported?
		  m_bFCMOV : 1,  // Is FCMOV supported?
				 m_bSSE : 1,	// Is SSE supported?
					  m_bSSE2 : 1,	// Is SSE2 Supported?
							m_b3DNow : 1,	// Is 3DNow! Supported?
								   m_bMMX : 1,	// Is MMX supported?
										m_bHT : 1;	// Is HyperThreading supported?

	unsigned char m_nLogicalProcessors,		// Number op logical processors.
		m_nPhysicalProcessors;	// Number of physical processors

	int64 m_Speed;						// In cycles per second.

	char* m_szProcessorID;				// Processor vendor Identification.
};

PLATFORM_INTERFACE const CPUInformation& GetCPUInformation();


//-----------------------------------------------------------------------------
// Thread related functions
//-----------------------------------------------------------------------------
// Registers the current thread with Tier0's thread management system. 
// This should be called on every thread created in the game.
PLATFORM_INTERFACE unsigned long Plat_RegisterThread(const char *pName = "Source Thread");

// Registers the current thread as the primary thread.
PLATFORM_INTERFACE unsigned long Plat_RegisterPrimaryThread();

// VC-specific. Sets the thread's name so it has a friendly name in the debugger.
// This should generally only be handled by Plat_RegisterThread and Plat_RegisterPrimaryThread
PLATFORM_INTERFACE void	Plat_SetThreadName(unsigned long dwThreadID, const char *pName);

// These would be private if it were possible to export private variables from a .DLL.
// They need to be variables because they are checked by inline functions at performance
// critical places.
PLATFORM_INTERFACE unsigned long Plat_PrimaryThreadID;

// Returns the ID of the currently executing thread.
PLATFORM_INTERFACE unsigned long Plat_GetCurrentThreadID();

// Returns the ID of the primary thread.
inline unsigned long Plat_GetPrimaryThreadID()
{
	return Plat_PrimaryThreadID;
}

// Returns true if the current thread is the primary thread.
inline bool Plat_IsPrimaryThread()
{
	//return true;
	return (Plat_GetPrimaryThreadID() == Plat_GetCurrentThreadID());
}

//-----------------------------------------------------------------------------
// Security related functions
//-----------------------------------------------------------------------------
// Ensure that the hardware key's drivers have been installed.
PLATFORM_INTERFACE bool Plat_VerifyHardwareKeyDriver();

// Ok, so this isn't a very secure way to verify the hardware key for now.  It 
// is primarially depending on the fact that all the binaries have been wrapped
// with the secure wrapper provided by the hardware keys vendor.
PLATFORM_INTERFACE bool Plat_VerifyHardwareKey();

// The same as above, but notifies user with a message box when the key isn't in 
// and gives him an opportunity to correct the situation.
PLATFORM_INTERFACE bool Plat_VerifyHardwareKeyPrompt();

// Can be called in real time, doesn't perform the verify every frame.  Mainly just
// here to allow the game to drop out quickly when the key is removed, rather than
// allowing the wrapper to pop up it's own blocking dialog, which the engine doesn't 
// like much.
PLATFORM_INTERFACE bool Plat_FastVerifyHardwareKey();



//-----------------------------------------------------------------------------
// Include additional dependant header components.
//-----------------------------------------------------------------------------
#include "tier0/fasttimer.h"


//-----------------------------------------------------------------------------
// Just logs file and line to simple.log
//-----------------------------------------------------------------------------
void* Plat_SimpleLog(const char* file, int line);

//#define Plat_dynamic_cast Plat_SimpleLog(__FILE__,__LINE__),dynamic_cast

//-----------------------------------------------------------------------------
// Methods to invoke the constructor, copy constructor, and destructor
//-----------------------------------------------------------------------------

template <class T>
inline void Construct(T* pMemory)
{
	new(pMemory)T;
}

template <class T>
inline void CopyConstruct(T* pMemory, T const& src)
{
	new(pMemory)T(src);
}

template <class T>
inline void Destruct(T* pMemory)
{
	pMemory->~T();

#ifdef _DEBUG
	memset(pMemory, 0xDD, sizeof(T));
#endif
}


//
// GET_OUTER()
//
// A platform-independent way for a contained class to get a pointer to its
// owner. If you know a class is exclusively used in the context of some
// "outer" class, this is a much more space efficient way to get at the outer
// class than having the inner class store a pointer to it.
//
//	class COuter
//	{
//		class CInner // Note: this does not need to be a nested class to work
//		{
//			void PrintAddressOfOuter()
//			{
//				printf( "Outer is at 0x%x\n", GET_OUTER( COuter, m_Inner ) );
//			}
//		};
//		
//		CInner m_Inner;
//		friend class CInner;
//	};

#define GET_OUTER( OuterType, OuterMember ) \
   ( ( OuterType * ) ( (char *)this - offsetof( OuterType, OuterMember ) ) )


/*	TEMPLATE_FUNCTION_TABLE()

(Note added to platform.h so platforms that correctly support templated
functions can handle portions as templated functions rather than wrapped
functions)

Helps automate the process of creating an array of function
templates that are all specialized by a single integer.
This sort of thing is often useful in optimization work.

For example, using TEMPLATE_FUNCTION_TABLE, this:

TEMPLATE_FUNCTION_TABLE(int, Function, ( int blah, int blah ), 10)
{
return argument * argument;
}

is equivilent to the following:

(NOTE: the function has to be wrapped in a class due to code
generation bugs involved with directly specializing a function
based on a constant.)

template<int argument>
class FunctionWrapper
{
public:
int Function( int blah, int blah )
{
return argument*argument;
}
}

typedef int (*FunctionType)( int blah, int blah );

class FunctionName
{
public:
enum { count = 10 };
FunctionType functions[10];
};

FunctionType FunctionName::functions[] =
{
FunctionWrapper<0>::Function,
FunctionWrapper<1>::Function,
FunctionWrapper<2>::Function,
FunctionWrapper<3>::Function,
FunctionWrapper<4>::Function,
FunctionWrapper<5>::Function,
FunctionWrapper<6>::Function,
FunctionWrapper<7>::Function,
FunctionWrapper<8>::Function,
FunctionWrapper<9>::Function
};
*/

bool vtune(bool resume);


#define TEMPLATE_FUNCTION_TABLE(RETURN_TYPE, NAME, ARGS, COUNT)			\
																		\
typedef RETURN_TYPE (FASTCALL *__Type_##NAME) ARGS;						\
																		\
template<const int nArgument>											\
struct __Function_##NAME												\
{																		\
	static RETURN_TYPE FASTCALL Run ARGS;								\
};																		\
																		\
template <int i>														\
struct __MetaLooper_##NAME : __MetaLooper_##NAME<i-1>					\
{																		\
	__Type_##NAME func;													\
	inline __MetaLooper_##NAME() { func = __Function_##NAME<i>::Run; }	\
};																		\
																		\
template<>																\
struct __MetaLooper_##NAME<0>											\
{																		\
	__Type_##NAME func;													\
	inline __MetaLooper_##NAME() { func = __Function_##NAME<0>::Run; }	\
};																		\
																		\
class NAME																\
{																		\
private:																\
    static const __MetaLooper_##NAME<COUNT> m;							\
public:																	\
	enum { count = COUNT };												\
	static const __Type_##NAME* functions;								\
};																		\
const __MetaLooper_##NAME<COUNT> NAME::m;								\
const __Type_##NAME* NAME::functions = (__Type_##NAME*)&m;				\
template<int nArgument>													\
RETURN_TYPE FASTCALL __Function_##NAME<nArgument>::Run ARGS						


#define LOOP_INTERCHANGE(BOOLEAN, CODE)\
	if( (BOOLEAN) )\
		{\
		CODE;\
		} else\
	{\
		CODE;\
	}


#endif /* PLATFORM_H */