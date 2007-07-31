




#pragma once
#ifndef __WATCH_LIST_H__
#define __WATCH_LIST_H__

#include "c3_listitem.h"
#include "aui_action.h"
#include "keyboardhandler.h"
#include "SlicSymbol.h"

class c3_PopupWindow;
class c3_ListBox;
template <class T> class PointerList;

typedef void (WatchListCallback)(sint32 arg);

#define k_MAX_WATCH_LINE 1024

class WatchListItem : public c3_ListItem, public SlicSymbolWatchCallback
{
public:
	WatchListItem(AUI_ERRCODE *retval, sint32 index, MBCHAR *line, 
				  MBCHAR *ldlBlock);
	~WatchListItem();

	virtual void Update();
	
	
	void WatchCallback(SlicSymbolData *symbol, bool isAddCallback);
	void WatchVariableDeleted(SlicSymbolData *symbol);

	MBCHAR *GetLine();
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);
	void ToggleBreak() { m_break = !m_break; }

protected:
	WatchListItem() : c3_ListItem() {}

	AUI_ERRCODE InitCommonLdl(MBCHAR *ldlBlock);

private:
	friend class WatchList;

	sint32 m_index;
	MBCHAR m_line[k_MAX_WATCH_LINE];
	PointerList<SlicSymbolData> *m_watching;
	bool m_break;
};

class WatchList : public KeyboardHandler
{
public:
	WatchList(WatchListCallback *callback = NULL, MBCHAR *ldlBlock = NULL);
	~WatchList();

	c3_PopupWindow *m_window;
	c3_ListBox     *m_list;
	c3_Button      *m_newButton;
	c3_Button      *m_clearButton;
	c3_Button      *m_exitButton;

	WatchListCallback *m_callback;

public:
	sint32 Initialize(MBCHAR *ldlBlock);
	sint32 Cleanup();
	sint32 UpdateData();

	void RemoveWindow();
	void DisplayWindow();
	
	void kh_Close();

	c3_ListBox *GetList() { return m_list; }
	void ShowBreak(sint32 offset);

	void AddExpression(char *exp);
	void Clear();

	c3_Button *GetNewButton() { return m_newButton; }
	c3_Button *GetClearButton() { return m_clearButton; }
	c3_Button *GetExitButton() { return m_exitButton; }
};
  


void watchlist_Display();
void watchlist_Remove();
void watchlist_Refresh();
void watchlist_AddExpression(char *exp);

extern WatchList *g_watchList;
#endif
