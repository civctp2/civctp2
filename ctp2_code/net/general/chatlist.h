
#ifndef xCHAT_LIST_H__
#define xCHAT_LIST_H__

#include "PointerList.h"

#define k_CHAT_TEXT_TIME 15

class ChatText
{
  public:
	MBCHAR *m_text;
	sint32 m_sender;
	sint32 m_timeAdded;

	ChatText(sint32 sender, const char *text) {
		m_text = new MBCHAR[strlen(text) + 1];
		strcpy(m_text, text);
		m_sender = sender;
		m_timeAdded = time(0);
	}

	~ChatText() {
		delete [] m_text;
	}
};

	
class ChatList
{
  public:
	PointerList<ChatText> m_list;

	ChatList() {}

	~ChatList() {
		m_list.DeleteAll();
	}

	void AddLine(sint32 sender, const char *text) {
		m_list.AddHead(new ChatText(sender, text));
	}

	void RemoveExpired() {
		while(m_list.GetTail() && (m_list.GetTail()->m_timeAdded + k_CHAT_TEXT_TIME < time(0))) {
			delete m_list.RemoveTail();
		}
	}
};

#endif
