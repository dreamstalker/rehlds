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

#include "IObjectContainer.h"

class ObjectDictionary: public IObjectContainer {
public:
	ObjectDictionary();
	virtual ~ObjectDictionary();

	void Init();
	void Init(int baseSize);

	bool Add(void *object);
	bool Contains(void *object);
	bool IsEmpty();
	int CountElements();

	void Clear(bool freeObjectssMemory = false);

	bool Add(void *object, float key);
	bool ChangeKey(void *object, float newKey);
	bool UnsafeChangeKey(void *object, float newKey);

	bool Remove(void *object);
	bool RemoveSingle(void *object);
	bool RemoveKey(float key);
	bool RemoveRange(float startKey, float endKey);

	void *FindClosestKey(float key);
	void *FindExactKey(float key);

	void *GetFirst();
	void *GetLast();
	void *GetNext();

	int FindKeyInCache(float key);
	int FindObjectInCache(void *object);

	void ClearCache();
	bool CheckSize();

	typedef struct entry_s {
		void *object;
		float key;
	} entry_t;

	void AddToCache(entry_t *entry);
	void AddToCache(entry_t *entry, float key);

	bool RemoveIndex(int index, bool freeObjectMemory = false);
	bool RemoveIndexRange(int minIndex, int maxIndex);
	int FindClosestAsIndex(float key);

protected:
	int m_currentEntry;
	float m_findKey;

	enum { MAX_OBJECT_CACHE = 32 };

	entry_t *m_entries;
	entry_t m_cache[MAX_OBJECT_CACHE];

	int m_cacheIndex;
	int m_size;
	int m_maxSize;
};
