#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __SEGMENT_LIST_H__
#define __SEGMENT_LIST_H__

#ifdef CTP2_ENABLE_SLICDEBUG

#include "c3_listitem.h"
#include "aui_action.h"
#include "keyboardhandler.h"

class SlicSegment;

typedef void (SegmentListCallback)(sint32 arg);

class SegmentListItem : public c3_ListItem
{
public:
	SegmentListItem(AUI_ERRCODE *retval, sint32 index, SlicSegment *segment, MBCHAR *ldlBlock);

	virtual void Update();

	SlicSegment *GetSegment() { return m_segment; }
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

	void Open();

protected:
	SegmentListItem() : c3_ListItem() {}

	AUI_ERRCODE InitCommonLdl(SlicSegment *segment, MBCHAR *ldlBlock);

private:
	sint32 m_index;
	SlicSegment *m_segment;
};

class SegmentList : public KeyboardHandler
{
public:
	SegmentList(SegmentListCallback* callback = 0, MBCHAR *ldlBlock = 0);
	virtual ~SegmentList();

	c3_PopupWindow *m_window;
	c3_ListBox     *m_list;
	c3_Button      *m_watchButton;
	c3_Button      *m_exitButton;

	SegmentListCallback *m_callback;
	SegmentListItem **m_items;

public:
	sint32 Initialize(MBCHAR *ldlBlock);
	sint32 Cleanup();
	sint32 UpdateData();

	void RemoveWindow();
	void DisplayWindow();
	
	virtual void kh_Close();

	c3_ListBox *GetList() { return m_list; }
};
  


void segmentlist_Display();
void segmentlist_Remove();

extern SegmentList *g_segmentList;

#endif //CTP2_ENABLE_SLICDEBUG

#endif
