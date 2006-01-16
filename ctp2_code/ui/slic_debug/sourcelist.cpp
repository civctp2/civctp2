//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Slic source list
// Id           : $Id:$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#ifdef CTP2_ENABLE_SLICDEBUG


#include "aui.h"
#include "aui_uniqueid.h"
#include "c3ui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_static.h"
#include "aui_hypertextbox.h"

#include "c3_static.h"
#include "thermometer.h"


#include "textbutton.h"
#include "c3_button.h"


#include "c3textfield.h"


#include "c3listbox.h"
#include "c3_listbox.h"
#include "aui_listbox.h"


#include "c3window.h"
#include "c3windows.h"
#include "c3_popupwindow.h"
#include "c3_utilitydialogbox.h"

#include "keypress.h"

#include "sourcelist.h"

#include "SlicEngine.h"
#include "SlicSegment.h"
#include "SlicConditional.h"

#include "pixelutils.h"
#include "colorset.h"

extern C3UI			*g_c3ui;
extern ColorSet						*g_colorSet;
SourceList *g_sourceList = NULL;

class SourceListItemContinueAction : public aui_Action
{
public:
	SourceListItem *m_item;

	SourceListItemContinueAction(SourceListItem *item) {
		m_item = item;
	}

#ifdef _MSC_VER
	aui_Action::ActionCallback Execute
#else
	virtual void Execute(aui_Control* control,
	                     uint32 action,
	                     uint32 data)
#endif
	{
		m_item->Continue();
	}
};


void sourcelist_Callback(sint32 arg)
{
}

void sourcelist_Display(SlicSegment *segment)
{
	if(!g_sourceList) {
		g_sourceList = new SourceList(sourcelist_Callback);
	}
	g_sourceList->DisplayWindow(segment);
}

void sourcelist_Remove()
{
	if(g_sourceList) {
		g_sourceList->RemoveWindow();
	}
}

void sourcelist_RegisterBreak(SlicSegment *segment, sint32 offset)
{
	sourcelist_Display(segment);
	g_sourceList->ShowBreak(offset);
}

SourceList::SourceList(SourceListCallback *callback, MBCHAR *ldlBlock)
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	m_continue = NULL;
	m_list = NULL;
	m_exit = NULL;

	
	if (ldlBlock) strcpy(windowBlock,ldlBlock);	
	else strcpy(windowBlock,"SourceListPopup");

	
	{ 
		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
		if ( !AUI_NEWOK(m_window, errcode) ) return;

		
		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(FALSE);
		m_window->SetDraggable(TRUE);
	}

	
	m_callback = callback;

	
	Initialize( windowBlock );
}

void SourceListActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if((action != (uint32)AUI_LISTBOX_ACTION_SELECT) &&
	   (action != (uint32)AUI_LISTBOX_ACTION_RMOUSESELECT) &&
	   (action != (uint32)AUI_LISTBOX_ACTION_DOUBLECLICKSELECT)) 
		return;

	SourceList *list = (SourceList *)cookie;

	SourceListItem *item = (SourceListItem *)list->GetList()->GetSelectedItem();

	if(action == AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {
		
	}

	if(!item)
		return;

	if(action == AUI_LISTBOX_ACTION_RMOUSESELECT) {
		return;
	}

	return;
}

void SourceListButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action == AUI_BUTTON_ACTION_EXECUTE) {
		if(control == g_sourceList->m_continue) {
			g_sourceList->Continue();
		}

		if(control == g_sourceList->m_exit) {
			sourcelist_Remove();
		}

		if(control == g_sourceList->m_step) {
			
			g_sourceList->StepInto();
		}

		if(control == g_sourceList->m_stepInto) {
			g_sourceList->StepInto();
		}
	}
}

sint32 SourceList::Initialize(MBCHAR *windowBlock)
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	

	
	sprintf( controlBlock, "%s.%s", windowBlock, "SourceList" );
	m_list = new c3_ListBox(&errcode, aui_UniqueId(), controlBlock, SourceListActionCallback, this);
	m_list->SetAbsorbancy(FALSE);
	Assert( AUI_NEWOK(m_list, errcode) );
	if ( !AUI_NEWOK(m_list, errcode) ) 
		return -1;

	sprintf(controlBlock, "%s.%s", windowBlock, "ContinueButton");
	m_continue = new c3_Button(&errcode, aui_UniqueId(), controlBlock, SourceListButtonCallback, this);
	Assert(AUI_NEWOK(m_continue, errcode));
	if( !AUI_NEWOK(m_continue, errcode)) 
		return -1;
	
	sprintf(controlBlock, "%s.%s", windowBlock, "ExitButton");
	m_exit = new c3_Button(&errcode, aui_UniqueId(), controlBlock, SourceListButtonCallback, this);
	Assert(AUI_NEWOK(m_exit, errcode));
	if( !AUI_NEWOK(m_exit, errcode)) 
		return -1;

	sprintf(controlBlock, "%s.%s", windowBlock, "StepButton");
	m_step = new c3_Button(&errcode, aui_UniqueId(), controlBlock, SourceListButtonCallback, this);
	Assert(AUI_NEWOK(m_step, errcode));
	if( !AUI_NEWOK(m_step, errcode)) 
		return -1;

	sprintf(controlBlock, "%s.%s", windowBlock, "StepIntoButton");
	m_stepInto = new c3_Button(&errcode, aui_UniqueId(), controlBlock, SourceListButtonCallback, this);
	Assert(AUI_NEWOK(m_stepInto, errcode));
	if( !AUI_NEWOK(m_stepInto, errcode)) 
		return -1;

	sprintf(controlBlock, "%s.%s", windowBlock, "Status");
	m_status = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	Assert(AUI_NEWOK(m_status, errcode));
	if(!AUI_NEWOK(m_status, errcode))
		return -1;

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}
	
sint32 SourceList::Cleanup(void)
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	g_c3ui->RemoveWindow( m_window->Id() );

	mycleanup( m_continue );
	mycleanup( m_list );
	
	m_callback = NULL;

	delete m_window;
	m_window = NULL;

	return 0 ;

#undef mycleanup
}

void SourceList::DisplayWindow(SlicSegment *segment)
{
	AUI_ERRCODE auiErr;

	m_segment = segment;
	UpdateData();

	auiErr = g_c3ui->AddWindow(m_window);
	Assert(auiErr == AUI_ERRCODE_OK);
	
	keypress_RegisterHandler(this);
}

void SourceList::RemoveWindow(void)
{
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow(m_window->Id());
	Assert(auiErr == AUI_ERRCODE_OK);

	keypress_RemoveHandler(this);
}

void SourceList::kh_Close()
{
	RemoveWindow();
}

sint32 SourceList::UpdateData(void)
{
	MBCHAR ldlBlock[k_AUI_LDL_MAXBLOCK + 1];

	AUI_ERRCODE retval;

	strcpy(ldlBlock, "SourceListItem");

	m_list->Clear();

	sint32 i;
	sint32 firstLineNum, firstLineOffset, lastLineNum;

	bool res = m_segment->GetSourceLines(firstLineNum, firstLineOffset, lastLineNum);
	Assert(res);
	if(!res)
		return 1;

	FILE *f = fopen(m_segment->GetFilename(), "r");
	Assert(f);
	if(!f)
		return 1;

	fseek(f, firstLineOffset, SEEK_SET);
	MBCHAR line[k_MAX_SOURCE_LINE];

	SourceListItem *item;

	for(i = firstLineNum; i <= lastLineNum; i++) {
		fgets(line, 1024, f);
		item = new SourceListItem(&retval, i - firstLineNum, 
								   m_segment, line, i, ldlBlock);
		m_list->AddItem((c3_ListItem *)item);
		
	}

	fclose(f);

	return 0;
}
	
void SourceList::ShowBreak(sint32 offset)
{
	sint32 lineNumber;
	lineNumber = m_segment->FindLineNumber(offset);
	sint32 i;

	char statusBuf[1024];
	sprintf(statusBuf, "Break at %s:%d", m_segment->GetName(), lineNumber);


	for(i = 0; i < m_list->NumItems(); i++) {
		SourceListItem *item = (SourceListItem *)m_list->GetItemByIndex(i);
		if(item->m_lineNumber == lineNumber) {
			item->ShowBreak();
			SlicConditional *cond = item->GetSegment()->GetConditional(lineNumber);
			if(cond) {
				strcat(statusBuf, " when ");
				strcat(statusBuf, cond->GetExpression());
			}
			break;
		}
	}

	m_status->SetText(statusBuf);
}

void SourceList::Continue()
{
	sint32 i;

	m_status->SetText("--");

	for(i = 0; i < m_list->NumItems(); i++) {
		SourceListItem *item = (SourceListItem *)m_list->GetItemByIndex(i);
		if(item->m_activeBreak) {
			g_c3ui->AddAction(new SourceListItemContinueAction(item));
			return;
		}
	}

	if(g_slicEngine->AtBreak()) {
		g_slicEngine->Continue();
	}
}

void SourceList::StepInto()
{
	
	g_slicEngine->RequestBreak();
	Continue();
}




SourceListItem::SourceListItem(AUI_ERRCODE *retval, sint32 index,
							   SlicSegment *segment, MBCHAR *line,
							   sint32 lineNumber, MBCHAR *ldlBlock) :
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem(retval, ldlBlock)
{
	m_index = index;
	m_segment = segment;
	strncpy(m_line, line, k_MAX_SOURCE_LINE);
	m_lineNumber = lineNumber;

	
	m_break = m_segment->LineHasBreak(m_lineNumber, m_conditional);
	m_activeBreak = false;

	Assert(AUI_SUCCESS(*retval));
	if(!AUI_SUCCESS(*retval)) return;

	*retval = InitCommonLdl(segment, ldlBlock);
	Assert(AUI_SUCCESS(*retval));
	if(!AUI_SUCCESS(*retval)) return;
}

void SourceBreakItemCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action == k_C3_STATIC_ACTION_LMOUSE) {
		SourceListItem *item = (SourceListItem *)cookie;
		item->ToggleBreak();
	} else if(action == k_C3_STATIC_ACTION_RMOUSE) {
		SourceListItem *item = (SourceListItem *)cookie;
		item->EditConditional();
	}
}

AUI_ERRCODE SourceListItem::InitCommonLdl(SlicSegment *segment,
										   MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	c3_Static *breakItem;
	c3_Static *textItem;

	sprintf(block, "%s.%s", ldlBlock, "Break");
	breakItem = new c3_Static(&retval, aui_UniqueId(), block);
	breakItem->SetActionFuncAndCookie(SourceBreakItemCallback, this);
	AddChild(breakItem);

	sprintf(block, "%s.%s", ldlBlock, "Line");
	textItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(textItem);

	Update();
	return AUI_ERRCODE_OK;
}

void SourceListItem::Update(void)
{
	c3_Static *codeItem;
	codeItem = (c3_Static *)GetChildByIndex(1);
	codeItem->SetText(m_line);

	m_break = m_segment->LineHasBreak(m_lineNumber, m_conditional);

	c3_Static *breakItem = (c3_Static *)GetChildByIndex(0);
	char breakChars[3];
	if(!m_break) {
		breakChars[0] = '-';
	} else {
		if(m_conditional) {
			breakChars[0] = 'C';
		} else {
			breakChars[0] = '+';
		}
	}

	if(m_activeBreak) {
		breakChars[1] = '>';
		breakChars[2] = 0;
		breakItem->SetText(breakChars);
		codeItem->SetTextColor(g_colorSet->GetColorRef(COLOR_RED));
	} else {
		breakChars[1] = 0;
		breakItem->SetText(breakChars);
		codeItem->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));
	}
}

sint32 SourceListItem::Compare(c3_ListItem *item2, uint32 column)
{
	return 0;
}

void SourceListItem::Open()
{
	return;
}

void SourceListItem::Continue()
{
	m_activeBreak = false;
	Update();
	g_slicEngine->Continue();
}
		
void SourceListItem::ToggleBreak()
{
	m_break = !m_break;

	if(m_break) {
		m_segment->SetBreak(m_lineNumber, true);
	} else {
		m_segment->SetBreak(m_lineNumber, false);
	}

	Update();
}

void SourceListItem::SetBreak()
{
	m_break = true;
	m_segment->SetBreak(m_lineNumber, true);
	Update();
}

void SourceListItem::ClearBreak()
{
	m_break = false;
	m_segment->SetBreak(m_lineNumber, false);
	Update();
}

void SourceListItem::ShowBreak()
{
	m_activeBreak = TRUE;
	Update();
}

static c3_UtilityTextFieldPopup *s_conditionalPopup = NULL;
								
class KillConditionalPopupAction : public aui_Action
{
public:
#ifdef _MSC_VER
        virtual ActionCallBack Execute
#else
	virtual void Execute(aui_Control* control,
	                     uint32 action,
	                     uint32 data)
#endif
	{
		if(s_conditionalPopup) {
			delete s_conditionalPopup;
			s_conditionalPopup = NULL;
		}
	}
};

void SourceListItemConditionalCallback(MBCHAR *text, sint32 val2, void *data)
{
	if(!val2) 
		return;

	SourceListItem *item = (SourceListItem *)data;
	if(!text || strlen(text) < 1) {
		item->GetSegment()->RemoveConditional(item->GetLineNumber());
		item->ClearBreak();
		return;
	}

	SlicConditional *cond = item->GetSegment()->GetConditional(item->GetLineNumber());
	if(!cond) {
		cond = item->GetSegment()->NewConditional(item->GetLineNumber(), text);
		Assert(cond);
		item->SetBreak();
	} else {
		cond->SetExpression(text);
		item->SetBreak();
	}

	g_c3ui->AddAction(new KillConditionalPopupAction);
}

void SourceListItem::EditConditional()
{
	SlicConditional *cond = m_segment->GetConditional(m_lineNumber);
	if(!s_conditionalPopup)
	s_conditionalPopup = new c3_UtilityTextFieldPopup(SourceListItemConditionalCallback,
													  NULL,
													  cond ? (char *)cond->GetExpression() : "",
													  NULL,
													  "SourceListConditionalPopup",
													  this,
													  true);
	s_conditionalPopup->DisplayWindow();

	
}

#endif // CTP2_ENABLE_SLICDEBUG

