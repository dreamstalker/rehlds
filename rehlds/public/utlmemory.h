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

#pragma once

#include "osconfig.h"
#include "tier0/dbg.h"
#include <string.h>

#pragma warning (disable:4100)
#pragma warning (disable:4514)

// The CUtlMemory class:
// A growable memory class which doubles in size by default.
template <class T, class I = int>
class CUtlMemory
{
public:
	// constructor, destructor
	CUtlMemory(int nGrowSize = 0, int nInitSize = 0);
	CUtlMemory(T *pMemory, int numElements);
	~CUtlMemory();

	// Set the size by which the memory grows
	void Init(int nGrowSize = 0, int nInitSize = 0);

	class Iterator_t
	{
	public:
		Iterator_t(I i) : m_index(i) {}
		I m_index;

		bool operator==(const Iterator_t it) const { return m_index == it.m_index; }
		bool operator!=(const Iterator_t it) const { return m_index != it.m_index; }
	};

	Iterator_t First() const							{ return Iterator_t(IsIdxValid(0) ? 0 : InvalidIndex()); }
	Iterator_t Next(const Iterator_t &it) const			{ return Iterator_t(IsIdxValid(it.index + 1) ? it.index + 1 : InvalidIndex()); }
	I GetIndex(const Iterator_t &it) const				{ return it.index; }
	bool IsIdxAfter(I i, const Iterator_t &it) const	{ return i > it.index; }
	bool IsValidIterator(const Iterator_t &it) const	{ return IsIdxValid(it.index); }
	Iterator_t InvalidIterator() const					{ return Iterator_t(InvalidIndex()); }

	// element access
	T&			Element(I i);
	T const&	Element(I i) const;
	T&			operator[](I i);
	T const&	operator[](I i) const;

	// Can we use this index?
	bool IsIdxValid(I i) const;

	// Specify the invalid ('null') index that we'll only return on failure
	static const I INVALID_INDEX = (I)-1; // For use with COMPILE_TIME_ASSERT
	static I InvalidIndex() { return INVALID_INDEX; }

	// Gets the base address (can change when adding elements!)
	T *Base();
	T const *Base() const;

	// Attaches the buffer to external memory....
	void SetExternalBuffer(T *pMemory, int numElements);

	// Size
	int NumAllocated() const;
	int Count() const;

	// Grows the memory, so that at least allocated + num elements are allocated
	void Grow(int num = 1);

	// Makes sure we've got at least this much memory
	void EnsureCapacity(int num);

	// Memory deallocation
	void Purge();

	// is the memory externally allocated?
	bool IsExternallyAllocated() const;

	// Set the size by which the memory grows
	void SetGrowSize(int size);

private:
	enum
	{
		EXTERNAL_BUFFER_MARKER = -1,
	};

	T *m_pMemory;
	int m_nAllocationCount;
	int m_nGrowSize;
};

// constructor, destructor
template <class T, class I>
CUtlMemory<T, I>::CUtlMemory(int nGrowSize, int nInitSize) : m_pMemory(0),
m_nAllocationCount(nInitSize), m_nGrowSize(nGrowSize)
{
	Assert((nGrowSize >= 0) && (nGrowSize != EXTERNAL_BUFFER_MARKER));
	if (m_nAllocationCount)
	{
		m_pMemory = (T *)malloc(m_nAllocationCount * sizeof(T));
	}
}

template <class T, class I>
CUtlMemory<T, I>::CUtlMemory(T *pMemory, int numElements) : m_pMemory(pMemory),
m_nAllocationCount(numElements)
{
	// Special marker indicating externally supplied memory
	m_nGrowSize = EXTERNAL_BUFFER_MARKER;
}

template <class T, class I>
CUtlMemory<T, I>::~CUtlMemory()
{
	Purge();
}

template <class T, class I>
void CUtlMemory<T,I>::Init(int nGrowSize, int nInitSize)
{
	Purge();

	m_nGrowSize = nGrowSize;
	m_nAllocationCount = nInitSize;
	Assert(nGrowSize >= 0);
	if (m_nAllocationCount)
	{
		m_pMemory = (T *)malloc(m_nAllocationCount * sizeof(T));
	}
}

// Attaches the buffer to external memory....
template <class T, class I>
void CUtlMemory<T, I>::SetExternalBuffer(T *pMemory, int numElements)
{
	// Blow away any existing allocated memory
	Purge();

	m_pMemory = pMemory;
	m_nAllocationCount = numElements;

	// Indicate that we don't own the memory
	m_nGrowSize = EXTERNAL_BUFFER_MARKER;
}

// element access
template <class T, class I>
inline T& CUtlMemory<T, I>::operator[](I i)
{
	Assert(IsIdxValid(i));
	return m_pMemory[i];
}

template <class T, class I>
inline T const& CUtlMemory<T, I>::operator[](I i) const
{
	Assert(IsIdxValid(i));
	return m_pMemory[i];
}

template <class T, class I>
inline T& CUtlMemory<T, I>::Element(I i)
{
	Assert(IsIdxValid(i));
	return m_pMemory[i];
}

template <class T, class I>
inline T const& CUtlMemory<T, I>::Element(I i) const
{
	Assert(IsIdxValid(i));
	return m_pMemory[i];
}

// is the memory externally allocated?
template <class T, class I>
bool CUtlMemory<T, I>::IsExternallyAllocated() const
{
	return m_nGrowSize == EXTERNAL_BUFFER_MARKER;
}

template <class T, class I>
void CUtlMemory<T, I>::SetGrowSize(int nSize)
{
	Assert((nSize >= 0) && (nSize != EXTERNAL_BUFFER_MARKER));
	m_nGrowSize = nSize;
}

// Gets the base address (can change when adding elements!)
template <class T, class I>
inline T *CUtlMemory<T, I>::Base()
{
	return m_pMemory;
}

template <class T, class I>
inline T const *CUtlMemory<T, I>::Base() const
{
	return m_pMemory;
}

// Size
template <class T, class I>
inline int CUtlMemory<T, I>::NumAllocated() const
{
	return m_nAllocationCount;
}

template <class T, class I>
inline int CUtlMemory<T, I>::Count() const
{
	return m_nAllocationCount;
}

// Is element index valid?
template <class T, class I>
inline bool CUtlMemory<T, I>::IsIdxValid(I i) const
{
	return (((int)i) >= 0) && (((int) i) < m_nAllocationCount);
}

// Grows the memory
template <class T, class I>
void CUtlMemory<T, I>::Grow(int num)
{
	Assert(num > 0);

	if (IsExternallyAllocated())
	{
		// Can't grow a buffer whose memory was externally allocated
		Assert(0);
		return;
	}

	// Make sure we have at least numallocated + num allocations.
	// Use the grow rules specified for this memory (in m_nGrowSize)
	int nAllocationRequested = m_nAllocationCount + num;
	while (m_nAllocationCount < nAllocationRequested)
	{
		if (m_nAllocationCount != 0)
		{
			if (m_nGrowSize)
			{
				m_nAllocationCount += m_nGrowSize;
			}
			else
			{
				m_nAllocationCount += m_nAllocationCount;
			}
		}
		else
		{
			// Compute an allocation which is at least as big as a cache line...
			m_nAllocationCount = (31 + sizeof(T)) / sizeof(T);
			Assert(m_nAllocationCount != 0);
		}
	}

	if (m_pMemory)
	{
		m_pMemory = (T *)realloc(m_pMemory, m_nAllocationCount * sizeof(T));
	}
	else
	{
		m_pMemory = (T *)malloc(m_nAllocationCount * sizeof(T));
	}
}

// Makes sure we've got at least this much memory
template <class T, class I>
inline void CUtlMemory<T, I>::EnsureCapacity(int num)
{
	if (m_nAllocationCount >= num)
		return;

	if (IsExternallyAllocated())
	{
		// Can't grow a buffer whose memory was externally allocated
		Assert(0);
		return;
	}

	m_nAllocationCount = num;
	if (m_pMemory)
	{
		m_pMemory = (T *)realloc(m_pMemory, m_nAllocationCount * sizeof(T));
	}
	else
	{
		m_pMemory = (T *)malloc(m_nAllocationCount * sizeof(T));
	}
}

// Memory deallocation
template <class T, class I>
void CUtlMemory<T, I>::Purge()
{
	if (!IsExternallyAllocated())
	{
		if (m_pMemory)
		{
			free((void *)m_pMemory);
			m_pMemory = 0;
		}
		m_nAllocationCount = 0;
	}
}
