#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __SEGMENT_LIST_H__
#define __SEGMENT_LIST_H__

#ifdef CTP2_ENABLE_SLICDEBUG

class SegmentListItem;
class SegmentList;

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
	virtual ~SegmentList(void);

	void DisplayWindow();
	void RemoveWindow();

	c3_ListBox *GetList() const { return m_list; }
	virtual void kh_Close();

private:
	sint32 Initialize(MBCHAR *ldlBlock);
	sint32 UpdateData();

	c3_PopupWindow *m_window;
	c3_ListBox     *m_list;
	c3_Button      *m_watchButton;
	c3_Button      *m_exitButton;

	SegmentListCallback *           m_callback;

    friend void SegmentListButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
};


void segmentlist_Display();
void segmentlist_Remove();

extern SegmentList *g_segmentList;

#endif //CTP2_ENABLE_SLICDEBUG

#endif
