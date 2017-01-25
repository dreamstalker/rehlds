#pragma once

#include "archtypes.h"
#include "crc32c.h"

template<typename T_KEY, typename T_VAL, unsigned int ASSOC_2N, unsigned int MAX_VALS>
class CStaticMap {
protected:
	virtual uint32 hash(const T_KEY& val) {
		return crc32c((const unsigned char*)&val, sizeof(T_KEY));
	}

	virtual bool equals(const T_KEY& val1, const T_KEY& val2) {
		return 0 == memcmp(&val1, &val2, sizeof(T_KEY));
	}

	struct map_node_t {
		map_node_t* prev;
		map_node_t* next;
		T_KEY key;
		T_VAL val;
	};

private:
	map_node_t* m_RootNodes[1 << ASSOC_2N];
	map_node_t m_AllNodes[MAX_VALS];
	map_node_t* m_FreeRoot;

	unsigned int GetRoodNodeId(const T_KEY& val) { return hash(val) & (0xFFFFFFFF >> (32 - ASSOC_2N)); }

	void unlink(map_node_t* node) {
		map_node_t* prev = node->prev;
		map_node_t* next = node->next;

		if (prev) {
			prev->next = next;
		}

		if (next) {
			next->prev = prev;
		}

		if (!prev) {
			// this was a root node
			unsigned int rootId = GetRoodNodeId(node->key);
			if (m_RootNodes[rootId] != node) {
				Sys_Error("%s: invalid root node", __FUNCTION__);
				return;
			}

			m_RootNodes[rootId] = next;
		}
	}

	void link(map_node_t* node) {
		unsigned int rootId = GetRoodNodeId(node->key);
		map_node_t* root = m_RootNodes[rootId];
		node->prev = NULL;
		node->next = root;

		if (root) {
			root->prev = node;
		}

		m_RootNodes[rootId] = node;
	}

	void linkToFreeStack(map_node_t* node) {
		node->next = m_FreeRoot;
		if (m_FreeRoot) {
			m_FreeRoot->prev = node;
		}
		m_FreeRoot = node;
	}

public:
	CStaticMap() {
		clear();
	}

	void clear() {
		memset(m_RootNodes, 0, sizeof(m_RootNodes));
		memset(m_AllNodes, 0, sizeof(m_AllNodes));
		m_FreeRoot = NULL;

		for (int i = 0; i < MAX_VALS; i++) {
			linkToFreeStack(&m_AllNodes[i]);
		}
	}

	map_node_t* get(const T_KEY& key) {
		unsigned int rootId = GetRoodNodeId(key);
		map_node_t* n = m_RootNodes[rootId];
		while (n) {
			if (equals(n->key, key)) {
				return n;
			}
			n = n->next;
		}
		return NULL;
	}

	bool put(const T_KEY& key, T_VAL& val) {
		map_node_t* n = get(key);
		if (n) {
			n->val = val;
			return true;
		}

		if (!m_FreeRoot) {
			return false;
		}

		n = m_FreeRoot;
		m_FreeRoot = m_FreeRoot->next;

		n->key = key;
		n->val = val;

		unsigned int rootId = GetRoodNodeId(key);
		map_node_t* root = m_RootNodes[rootId];

		if (root) {
			root->prev = n;
		}

		n->prev = NULL;
		n->next = root;
		m_RootNodes[rootId] = n;

		return true;
	}

	void remove(map_node_t* node) {
		unlink(node);
		linkToFreeStack(node);
	}

	bool remove(const T_KEY& key) {
		map_node_t* n = get(key);
		if (!n) {
			return false;
		}

		remove(n);
		return true;
	}

	class Iterator {
		friend class CStaticMap;
	protected:
		CStaticMap* m_Map;
		map_node_t** m_RootNodes;
		unsigned int m_NextRootNode;
		map_node_t* m_CurNode;

		void searchForNextNode() {
			if (m_CurNode && m_CurNode->next) {
				m_CurNode = m_CurNode->next;
				return;
			}

			m_CurNode = NULL;
			while (!m_CurNode) {
				if (m_NextRootNode >= (1 << ASSOC_2N)) {
					return;
				}
				m_CurNode = m_RootNodes[m_NextRootNode++];
			}
		}

	
		Iterator(CStaticMap* m) {
			m_Map = m;
			m_RootNodes = m_Map->m_RootNodes;
			m_NextRootNode = 0;
			m_CurNode = NULL;
			searchForNextNode();
		}

	public:
		map_node_t* next() {
			searchForNextNode();
			return m_CurNode;
		}

		map_node_t* current() {
			return m_CurNode;
		}

		void remove() {
			m_Map->remove(m_CurNode);
			m_CurNode = NULL;
		}

		bool hasElement() {
			return m_CurNode != NULL;
		}
	};

	Iterator iterator() {
		return Iterator(this);
	}
};

template<typename T_VAL, unsigned int ASSOC_2N, unsigned int MAX_VALS>
class CStringKeyStaticMap : public CStaticMap<const char*, T_VAL, ASSOC_2N, MAX_VALS> {
protected:
	virtual uint32 hash(const char* const &val) {
		return crc32c((const unsigned char*)val, strlen(val));
	}

	virtual bool equals(const char* const &val1, const char* const &val2) {
		return !strcmp(val1, val2);
	}

public:
	CStringKeyStaticMap() {
	}

};

template<typename T_VAL, unsigned int ASSOC_2N, unsigned int MAX_VALS>
class CICaseStringKeyStaticMap : public CStaticMap<const char*, T_VAL, ASSOC_2N, MAX_VALS> {
protected:
	virtual uint32 hash(const char* const &val) {
		uint32 cksum = 0;
		const char* pcc = val;
		if (cpuinfo.sse4_2) {
			while (*pcc) {
				char cc = *(pcc++);
				if (cc >= 'A' || cc <= 'Z') {
					cc |= 0x20;
				}
				cksum = crc32c_t8_sse(cksum, cc);
			}
		} else {
			while (*pcc) {
				char cc = *(pcc++);
				if (cc >= 'A' || cc <= 'Z') {
					cc |= 0x20;
				}
				cksum = crc32c_t8_nosse(cksum, cc);
			}
		}
		return cksum;
	}

	virtual bool equals(const char* const &val1, const char* const &val2) {
		return !_stricmp(val1, val2);
	}

public:
	CICaseStringKeyStaticMap() {
	}

};
