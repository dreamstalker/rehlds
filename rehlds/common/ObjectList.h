#ifndef OBJECTLIST_H
#define OBJECTLIST_H
#ifdef _WIN32
#pragma once
#endif

#include "IObjectContainer.h"

class ObjectList: public IObjectContainer {
public:
	void Init();
	bool Add(void *newObject);
	void *GetFirst();
	void *GetNext();

	ObjectList();
	virtual ~ObjectList();

	void Clear(bool freeElementsMemory = false);
	int CountElements();
	void *RemoveTail();
	void *RemoveHead();

	bool AddTail(void *newObject);
	bool AddHead(void *newObject);
	bool Remove(void *object);
	bool Contains(void *object);
	bool IsEmpty();

	typedef struct element_s {
		element_s *prev;	// pointer to the last element or NULL
		element_s *next;	// pointer to the next elemnet or NULL
		void *object;		// the element's object
	} element_t;

protected:
	element_t *head;	// first element in list
	element_t *tail;	// last element in list
	element_t *current;	// current element in list
	int number;
};

#endif // OBJECTLIST_H
