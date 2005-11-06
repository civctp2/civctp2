#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SOURCE_LIST_H__
#define __SOURCE_LIST_H__

#ifdef CTP2_ENABLE_SLICDEBUG

#include "c3_listitem.h"
#include "aui_action.h"
#include "keyboardhandler.h"

class SlicSegment;

typedef void (SourceListCallback)(sint32 arg);

#define k_MAX_SOURCE_LINE 1024

class SourceListItem : public c3_ListItem
{
public:
	SourceListItem(AUI_ERRCODE *retval, sint32 index, SlicSegment *segment, MBCHAR *line, 
				   sint32 lineNumber, MBCHAR *ldlBlock);

	virtual void Update();

	SlicSegment *GetSegment() { return m_segment; }
	MBCHAR *GetLine();
	sint32 GetLineNumber() { return m_lineNumber; }
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

	void Open();
	void ToggleBreak();
	void SetBreak();
	void ClearBreak();
	void ShowBreak();
	void Continue();
	void EditConditional();

protected:
	SourceListItem() : c3_ListItem() {}

	AUI_ERRCODE InitCommonLdl(SlicSegment *segment, MBCHAR *ldlBlock);

private:
	friend class SourceList;

	sint32 m_index;
	sint32 m_lineNumber;
	SlicSegment *m_segment;
	MBCHAR m_line[k_MAX_SOURCE_LINE];
	bool m_break;
	bool m_activeBreak;
	bool m_conditional;
};

class SourceList : public KeyboardHandler
{
public:
	SourceList(SourceListCallback *callback = NULL, MBCHAR *ldlBlock = NULL);
	virtual ~SourceList();

	c3_PopupWindow *m_window;
	c3_ListBox     *m_list;
	c3_Button      *m_continue;
	c3_Button      *m_exit;
	c3_Button      *m_step;
	c3_Button      *m_stepInto;
	c3_Static      *m_status;

	SourceListCallback *m_callback;
	

	SlicSegment *m_segment;

public:
	sint32 Initialize(MBCHAR *ldlBlock);
	void   Cleanup();
	sint32 UpdateData();

	void RemoveWindow();
	void DisplayWindow(SlicSegment *segment);
	
	void kh_Close();

	c3_ListBox *GetList() { return m_list; }
	void ShowBreak(sint32 offset);
	void Continue();
	void StepInto();
};
  


void sourcelist_Display(SlicSegment *segment);
void sourcelist_Remove();
void sourcelist_RegisterBreak(SlicSegment *segment, sint32 offset);

extern SourceList *g_sourceList;
#endif // CTP2_ENABLE_SLICDEBUG

#endif

