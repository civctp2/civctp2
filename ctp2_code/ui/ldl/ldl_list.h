
#ifndef LDL_LIST_H__
#define LDL_LIST_H__

#include "ldl_attr.hpp"

class ldl_attributelist {
  public:
	ldl_attribute *m_head;
	ldl_attribute *m_tail;

	ldl_attributelist() {
		m_head = m_tail = NULL;
	}

	~ldl_attributelist() {
		ldl_attribute *del = m_head, *next;
		while(del) {
			next = del->m_next;
			delete del;
			del = next;
		}
		m_head = m_tail = NULL;
	}

	ldl_attribute *GetHead() { return m_head; }
	ldl_attribute *GetTail() { return m_tail; }
	ldl_attribute *GetNext(ldl_attribute *att) { return att->m_next; }
	void AddTail(ldl_attribute *att) {
		if(m_tail) {
			m_tail->m_next = att;
			m_tail = att;
		} else {
			m_head = m_tail = att;
		}
	}
	ldl_attribute *FindAttribute(char *name) {
		ldl_attribute *search = m_head;
		while(search) {
			if(search->m_name == name) {
				return search;
			}
			search = search->m_next;
		}
		return NULL;
	}
};


#endif
