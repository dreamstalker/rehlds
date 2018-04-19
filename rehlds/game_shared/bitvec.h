/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#pragma once

class CBitVecAccessor
{
public:
	CBitVecAccessor(uint32 *pDWords, int iBit);

	void operator=(int val);
	operator uint32();

private:
	uint32 *m_pDWords;
	int m_iBit;
};

// CBitVec allows you to store a list of bits and do operations on them like they were
// an atomic type
template<int NUM_BITS>
class CBitVec
{
public:
	CBitVec();

	// Set all values to the specified value (0 or 1..)
	void Init(int val = 0);

	// Access the bits like an array.
	CBitVecAccessor	operator[](int i);

	// Operations on other bit vectors.
	CBitVec &operator=(CBitVec<NUM_BITS> const &other);
	bool operator==(CBitVec<NUM_BITS> const &other);
	bool operator!=(CBitVec<NUM_BITS> const &other);

	// Get underlying dword representations of the bits.
	int GetNumDWords() { return NUM_DWORDS; }
	uint32 GetDWord(int i);
	void SetDWord(int i, uint32 val);
	int GetNumBits();

private:

	enum
	{
		NUM_DWORDS = NUM_BITS / 32 + !!(NUM_BITS & 31)
	};

	unsigned int m_DWords[ NUM_DWORDS ];
};

inline CBitVecAccessor::CBitVecAccessor(uint32 *pDWords, int iBit)
{
	m_pDWords = pDWords;
	m_iBit = iBit;
}

inline void CBitVecAccessor::operator=(int val)
{
	if (val)
		m_pDWords[m_iBit >> 5] |= (1 << (m_iBit & 31));
	else
		m_pDWords[m_iBit >> 5] &= ~(uint32)(1 << (m_iBit & 31));
}

inline CBitVecAccessor::operator uint32()
{
	return m_pDWords[m_iBit >> 5] & (1 << (m_iBit & 31));
}

template<int NUM_BITS>
inline int CBitVec<NUM_BITS>::GetNumBits()
{
	return NUM_BITS;
}

template<int NUM_BITS>
inline CBitVec<NUM_BITS>::CBitVec()
{
	for (int i = 0; i < NUM_DWORDS; ++i)
		m_DWords[i] = 0;
}

template<int NUM_BITS>
inline void CBitVec<NUM_BITS>::Init(int val)
{
	for (int i = 0; i < GetNumBits(); ++i)
	{
		(*this)[i] = val;
	}
}

template<int NUM_BITS>
inline CBitVec<NUM_BITS> &CBitVec<NUM_BITS>::operator=(CBitVec<NUM_BITS> const &other)
{
	Q_memcpy(m_DWords, other.m_DWords, sizeof(m_DWords));
	return *this;
}

template<int NUM_BITS>
inline CBitVecAccessor CBitVec<NUM_BITS>::operator[](int i)
{
	assert(i >= 0 && i < GetNumBits());
	return CBitVecAccessor(m_DWords, i);
}

template<int NUM_BITS>
inline bool CBitVec<NUM_BITS>::operator==(CBitVec<NUM_BITS> const &other)
{
	for (int i = 0; i < NUM_DWORDS; ++i)
	{
		if (m_DWords[i] != other.m_DWords[i])
			return false;
	}

	return true;
}

template<int NUM_BITS>
inline bool CBitVec<NUM_BITS>::operator!=(CBitVec<NUM_BITS> const &other)
{
	return !(*this == other);
}

template<int NUM_BITS>
inline uint32 CBitVec<NUM_BITS>::GetDWord(int i)
{
	assert(i >= 0 && i < NUM_DWORDS);
	return m_DWords[i];
}

template<int NUM_BITS>
inline void CBitVec<NUM_BITS>::SetDWord(int i, uint32 val)
{
	assert(i >= 0 && i < NUM_DWORDS);
	m_DWords[i] = val;
}
