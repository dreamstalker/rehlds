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

class ObjectList: public IObjectContainer {
public:
	EXT_FUNC void Init();
	EXT_FUNC bool Add(void *newObject);
	EXT_FUNC void *GetFirst();
	EXT_FUNC void *GetNext();

	ObjectList();
	virtual ~ObjectList();

	EXT_FUNC void Clear(bool freeElementsMemory = false);
	EXT_FUNC int CountElements();
	void *RemoveTail();
	void *RemoveHead();

	bool AddTail(void *newObject);
	bool AddHead(void *newObject);
	EXT_FUNC bool Remove(void *object);
	EXT_FUNC bool Contains(void *object);
	EXT_FUNC bool IsEmpty();

	typedef struct element_s {
		struct element_s *prev;	// pointer to the last element or NULL
		struct element_s *next;	// pointer to the next elemnet or NULL
		void *object;		// the element's object
	} element_t;

protected:
	element_t *m_head;    // first element in list
	element_t *m_tail;    // last element in list
	element_t *m_current; // current element in list
	int m_number;
};
