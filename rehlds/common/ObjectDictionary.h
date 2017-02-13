#ifndef OBJECTDICTIONARY_H
#define OBJECTDICTIONARY_H
#ifdef _WIN32
#pragma once
#endif

#include "IObjectContainer.h"

typedef struct entry_s {
	void *object;
	float key;
} entry_t;

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

	void AddToCache(entry_t *entry);
	void AddToCache(entry_t *entry, float key);

	bool RemoveIndex(int index, bool freeObjectMemory = false);
	bool RemoveIndexRange(int minIndex, int maxIndex);
	int FindClosestAsIndex(float key);

protected:

	int currentEntry;
	float findKey;

	enum { MAX_OBJECT_CACHE = 32 };

	entry_t *entries;
	entry_t cache[MAX_OBJECT_CACHE];

	int cacheIndex;
	int size;
	int maxSize;
};

#endif // OBJECTDICTIONARY_H
