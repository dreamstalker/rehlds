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

#include "precompiled.h"

ObjectList::ObjectList()
{
	m_head = m_tail = m_current = nullptr;
	m_number = 0;
}

ObjectList::~ObjectList()
{
	Clear(false);
}

bool ObjectList::AddHead(void *newObject)
{
	// create new element
	element_t *newElement = (element_t *)Mem_ZeroMalloc(sizeof(element_t));

	// out of memory
	if (!newElement)
		return false;

	// insert element
	newElement->object = newObject;

	if (m_head)
	{
		newElement->next = m_head;
		m_head->prev = newElement;
	}

	m_head = newElement;

	// if list was empty set new m_tail
	if (!m_tail)
		m_tail = m_head;

	m_number++;
	return true;
}

void *ObjectList::RemoveHead()
{
	void *retObj;

	// check m_head is present
	if (m_head)
	{
		retObj = m_head->object;
		element_t *newHead = m_head->next;
		if (newHead)
			newHead->prev = nullptr;

		// if only one element is in list also update m_tail
		// if we remove this prev element
		if (m_tail == m_head)
			m_tail = nullptr;

		free(m_head);
		m_head = newHead;

		m_number--;
	}
	else
		retObj = nullptr;

	return retObj;
}

bool ObjectList::AddTail(void *newObject)
{
	// create new element
	element_t *newElement = (element_t *)Mem_ZeroMalloc(sizeof(element_t));

	// out of memory
	if (!newElement)
		return false;

	// insert element
	newElement->object = newObject;

	if (m_tail)
	{
		newElement->prev = m_tail;
		m_tail->next = newElement;
	}

	m_tail = newElement;

	// if list was empty set new m_tail
	if (!m_head)
		m_head = m_tail;

	m_number++;
	return true;
}

void *ObjectList::RemoveTail()
{
	void *retObj;

	// check m_tail is present
	if (m_tail)
	{
		retObj = m_tail->object;
		element_t *newTail = m_tail->prev;
		if (newTail)
			newTail->next = nullptr;

		// if only one element is in list also update m_tail
		// if we remove this prev element
		if (m_head == m_tail)
			m_head = nullptr;

		free(m_tail);
		m_tail = newTail;

		m_number--;

	}
	else
		retObj = nullptr;

	return retObj;
}

bool ObjectList::IsEmpty()
{
	return (m_head == nullptr);
}

int ObjectList::CountElements()
{
	return m_number;
}

bool ObjectList::Contains(void *object)
{
	element_t *e = m_head;

	while (e && e->object != object) { e = e->next; }

	if (e)
	{
		m_current = e;
		return true;
	}
	else
	{
		return false;
	}
}

void ObjectList::Clear(bool freeElementsMemory)
{
	element_t *ne;
	element_t *e = m_head;

	while (e)
	{
		ne = e->next;

		if (freeElementsMemory && e->object)
			free(e->object);

		free(e);
		e = ne;
	}

	m_head = m_tail = m_current = nullptr;
	m_number = 0;
}

bool ObjectList::Remove(void *object)
{
	element_t *e = m_head;

	while (e && e->object != object) { e = e->next; }

	if (e)
	{
		if (e->prev) e->prev->next = e->next;
		if (e->next) e->next->prev = e->prev;

		if (m_head == e) m_head = e->next;
		if (m_tail == e) m_tail = e->prev;
		if (m_current == e) m_current= e->next;

		free(e);
		m_number--;
	}

	return (e != nullptr);
}

void ObjectList::Init()
{
	m_head = m_tail = m_current = nullptr;
	m_number = 0;
}

void *ObjectList::GetFirst()
{
	if (m_head)
	{
		m_current = m_head->next;
		return m_head->object;
	}
	else
	{
		m_current = nullptr;
		return nullptr;
	}
}

void *ObjectList::GetNext()
{
	void *retObj = nullptr;
	if (m_current)
	{
		retObj = m_current->object;
		m_current = m_current->next;
	}

	return retObj;
}

bool ObjectList::Add(void *newObject)
{
	return AddTail(newObject);
}
