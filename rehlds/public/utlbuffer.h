//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// $Header: $
// $NoKeywords: $
//
// Serialization/unserialization buffer
//=============================================================================

#ifndef UTLBUFFER_H
#define UTLBUFFER_H

#include "osconfig.h"
#include "utlmemory.h"
#include <stdarg.h>

//-----------------------------------------------------------------------------
// Command parsing..
//-----------------------------------------------------------------------------

class CUtlBuffer
{
public:
	enum SeekType_t
	{
		SEEK_HEAD = 0,
		SEEK_CURRENT,
		SEEK_TAIL
	};

	CUtlBuffer(int growSize = 0, int initSize = 0, bool text = false);
	CUtlBuffer(void const* pBuffer, int size, bool text = false);

	// Makes sure we've got at least this much memory
	void EnsureCapacity(int num);

	// Attaches the buffer to external memory....
	void SetExternalBuffer(void* pMemory, int numElements, bool text = false);

	// Read stuff out.
	// Binary mode: it'll just read the bits directly in, and characters will be
	//		read for strings until a null character is reached.
	// Text mode: it'll parse the file, turning text #s into real numbers.
	//		GetString will read a string until a space is reaced
	char			GetChar();
	unsigned char	GetUnsignedChar();
	short			GetShort();
	unsigned short	GetUnsignedShort();
	int				GetInt();
	int				GetIntHex();
	unsigned int	GetUnsignedInt();
	float			GetFloat();
	double			GetDouble();
	void			GetString(char* pString, int nMaxLen = 0);
	void			Get(void* pMem, int size);

	// Just like scanf, but doesn't work in binary mode
	int				Scanf(char const* pFmt, ...);
	int				VaScanf(char const* pFmt, va_list list);

	// Eats white space, advances Get index
	void			EatWhiteSpace();

	// Write stuff in
	// Binary mode: it'll just write the bits directly in, and strings will be
	//		written with a null terminating character
	// Text mode: it'll convert the numbers to text versions
	//		PutString will not write a terminating character
	void			PutChar(char c);
	void			PutUnsignedChar(unsigned char uc);
	void			PutShort(short s);
	void			PutUnsignedShort(unsigned short us);
	void			PutInt(int i);
	void			PutUnsignedInt(unsigned int u);
	void			PutFloat(float f);
	void			PutDouble(double d);
	void			PutString(char const* pString);
	void			Put(void const* pMem, int size);

	// Just like printf, writes a terminating zero in binary mode
	void			Printf(char const* pFmt, ...);
	void			VaPrintf(char const* pFmt, va_list list);

	// What am I writing (put)/reading (get)?
	void* PeekPut(int offset = 0);
	void const* PeekGet(int offset = 0) const;

	// Where am I writing (put)/reading (get)?
	int  TellPut() const;
	int  TellGet() const;

	// Change where I'm writing (put)/reading (get)
	void SeekPut(SeekType_t type, int offset);
	void SeekGet(SeekType_t type, int offset);

	// Buffer base
	void const* Base() const;
	void* Base();

	// memory allocation size, does *not* reflect size written or read,
	//	use TellPut or TellGet for that
	int Size() const;

	// Am I a text buffer?
	inline bool IsText() const { return (m_Flags & TEXT_BUFFER) != 0; }

	// Am I valid? (overflow or underflow error), Once invalid it stays invalid
	inline bool IsValid() const { return m_Error == 0; }

private:
	// error flags
	enum
	{
		PUT_OVERFLOW = 0x1,
		GET_OVERFLOW = 0x2,
	};

	// flags
	enum
	{
		TEXT_BUFFER = 0x1,
	};

	// Checks if a get/put is ok
	bool CheckPut(int size);
	bool CheckGet(int size);

	CUtlMemory<unsigned char> m_Memory;
	int m_Get;
	int m_Put;
	unsigned char m_Error;
	unsigned char m_Flags;
};


//-----------------------------------------------------------------------------
// Where am I reading?
//-----------------------------------------------------------------------------

inline int CUtlBuffer::TellGet() const
{
	return m_Get;
}


//-----------------------------------------------------------------------------
// What am I reading?
//-----------------------------------------------------------------------------
inline void const* CUtlBuffer::PeekGet(int offset) const
{
	return &m_Memory[m_Get + offset];
}


//-----------------------------------------------------------------------------
// Unserialization
//-----------------------------------------------------------------------------
#define GET_TYPE( _type, _val, _fmt )	\
	if (!IsText())						\
			{									\
		if (CheckGet( sizeof(_type) ))	\
						{								\
			_val = *(_type *)PeekGet();	\
			m_Get += sizeof(_type);		\
						}								\
								else							\
		{								\
			_val = 0;					\
		}								\
			}									\
				else								\
	{									\
		_val = 0;						\
		Scanf( _fmt, &_val );			\
	}

inline char CUtlBuffer::GetChar()
{
	char c;
	GET_TYPE(char, c, "%c");
	return c;
}

inline unsigned char CUtlBuffer::GetUnsignedChar()
{
	unsigned char c;
	GET_TYPE(unsigned char, c, "%u");
	return c;
}

inline short CUtlBuffer::GetShort()
{
	short s;
	GET_TYPE(short, s, "%d");
	return s;
}

inline unsigned short CUtlBuffer::GetUnsignedShort()
{
	unsigned short s;
	GET_TYPE(unsigned short, s, "%u");
	return s;
}

inline int CUtlBuffer::GetInt()
{
	int i;
	GET_TYPE(int, i, "%d");
	return i;
}

inline int CUtlBuffer::GetIntHex()
{
	int i;
	GET_TYPE(int, i, "%x");
	return i;
}

inline unsigned int CUtlBuffer::GetUnsignedInt()
{
	unsigned int u;
	GET_TYPE(unsigned int, u, "%u");
	return u;
}

inline float CUtlBuffer::GetFloat()
{
	float f;
	GET_TYPE(float, f, "%f");
	return f;
}

inline double CUtlBuffer::GetDouble()
{
	double d;
	GET_TYPE(double, d, "%f");
	return d;
}


//-----------------------------------------------------------------------------
// Where am I writing?
//-----------------------------------------------------------------------------
inline int CUtlBuffer::TellPut() const
{
	return m_Put;
}


//-----------------------------------------------------------------------------
// What am I reading?
//-----------------------------------------------------------------------------
inline void* CUtlBuffer::PeekPut(int offset)
{
	return &m_Memory[m_Put + offset];
}


//-----------------------------------------------------------------------------
// Various put methods
//-----------------------------------------------------------------------------
#define PUT_TYPE( _type, _val, _fmt )	\
	if (!IsText())						\
			{									\
		if (CheckPut( sizeof(_type) ))	\
						{								\
			*(_type *)PeekPut() = _val;	\
			m_Put += sizeof(_type);		\
						}								\
			}									\
				else								\
	{									\
		Printf( _fmt, _val ); 			\
	}


inline void CUtlBuffer::PutChar(char c)
{
	PUT_TYPE(char, c, "%c");
}

inline void CUtlBuffer::PutUnsignedChar(unsigned char c)
{
	PUT_TYPE(unsigned char, c, "%u");
}

inline void  CUtlBuffer::PutShort(short s)
{
	PUT_TYPE(short, s, "%d");
}

inline void CUtlBuffer::PutUnsignedShort(unsigned short s)
{
	PUT_TYPE(unsigned short, s, "%u");
}

inline void CUtlBuffer::PutInt(int i)
{
	PUT_TYPE(int, i, "%d");
}

inline void CUtlBuffer::PutUnsignedInt(unsigned int u)
{
	PUT_TYPE(unsigned int, u, "%u");
}

inline void CUtlBuffer::PutFloat(float f)
{
	PUT_TYPE(float, f, "%f");
}

inline void CUtlBuffer::PutDouble(double d)
{
	PUT_TYPE(double, d, "%f");
}

//-----------------------------------------------------------------------------
// Buffer base and size
//-----------------------------------------------------------------------------

inline void const* CUtlBuffer::Base() const
{
	return m_Memory.Base();
}

inline void* CUtlBuffer::Base()
{
	return m_Memory.Base();
}

inline int CUtlBuffer::Size() const
{
	return m_Memory.NumAllocated();
}


#endif // UTLBUFFER_H
