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

#include "utlrbtree.h"
#include "utlvector.h"

class CUtlSymbolTable;

using UtlSymId_t = unsigned short;

const int MAX_STRING_POOL_SIZE = 256;
const UtlSymId_t UTL_INVAL_SYMBOL = ((UtlSymId_t)~0);
const unsigned int INVALID_STRING_INDEX = -1;

struct LessCtx_t
{
	const char *m_pUserString;
	CUtlSymbolTable *m_pTable;

	LessCtx_t() : m_pUserString(0), m_pTable(0) {}
};

class CUtlSymbol
{
public:
	// constructor, destructor
	CUtlSymbol() : m_Id(UTL_INVAL_SYMBOL) {}
	CUtlSymbol(UtlSymId_t id) : m_Id(id) {}
	CUtlSymbol(const char *pStr);
	CUtlSymbol(CUtlSymbol const &sym) : m_Id(sym.m_Id) {}

	// operator=
	CUtlSymbol &operator=(CUtlSymbol const &src)
	{
		m_Id = src.m_Id;
		return *this;
	}

	// operator==
	bool operator==(CUtlSymbol const &src) const { return m_Id == src.m_Id; }
	bool operator==(const char *pStr) const;

	// Is valid?
	bool IsValid() const { return m_Id != UTL_INVAL_SYMBOL; }

	operator UtlSymId_t() const { return m_Id; }	// Gets at the symbol
	const char *String() const;						// Gets the string associated with the symbol

protected:
	static void Initialize();						// Initializes the symbol table
	static CUtlSymbolTable *CurrTable();			// returns the current symbol table

	UtlSymId_t m_Id;
	static CUtlSymbolTable *s_pSymbolTable;
};

class CUtlSymbolTable
{
public:
	// constructor, destructor
	CUtlSymbolTable(int growSize = 0, int initSize = 32, bool caseInsensitive = false);
	~CUtlSymbolTable();

	CUtlSymbol AddString(const char *pString);		// Finds and/or creates a symbol based on the string
	CUtlSymbol Find(const char *pString);			// Finds the symbol for pString

	const char *String(CUtlSymbol id) const;		// Look up the string associated with a particular symbol
	void RemoveAll();								// Remove all symbols in the table

public:
	static bool SymLess(const unsigned int &i1, const unsigned int &i2);
	static bool SymLessi(const unsigned int &i1, const unsigned int &i2);

private:
	const char *StringFromIndex(const UtlSymId_t &index) const;

protected:
	CUtlRBTree<unsigned int, UtlSymId_t> m_Lookup;
	CUtlVector<char> m_StringPools;
};

inline const char *CUtlSymbolTable::StringFromIndex(const UtlSymId_t &index) const
{
	Assert(index < m_StringPools.Count());
	return &m_StringPools[index];
}
