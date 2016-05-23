//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Slic watch list
// Id           : $Id$
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

#include "watchlist.h"

#include "SlicEngine.h"
#include "pixelutils.h"
#include "colorset.h"

#include "sliccmd.h"
#include "sc.tab.h"

#include "pointerlist.h"

extern C3UI			*g_c3ui;

WatchList *g_watchList = NULL;

void watchlist_Callback(sint32 arg)
{
}

void watchlist_Display()
{
	if(!g_watchList) {
		g_watchList = new WatchList(watchlist_Callback);
	}
	g_watchList->DisplayWindow();
}

void watchlist_Remove()
{
	if(g_watchList) {
		g_watchList->RemoveWindow();
	}
}

void watchlist_Refresh()
{
	watchlist_Display();
}

void watchlist_AddExpression(char *exp)
{
	watchlist_Display();
	g_watchList->AddExpression(exp);
}

WatchList::WatchList(WatchListCallback callback, MBCHAR *ldlBlock)
:   m_window                (NULL),
    m_list                  (NULL),
    m_newButton             (NULL),
    m_clearButton           (NULL),
    m_exitButton            (NULL),
	m_callback              (callback)
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
    strcpy(windowBlock, ldlBlock ? ldlBlock : "WatchListPopup");

		m_window = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(m_window, errcode) );
	if (AUI_NEWOK(m_window, errcode))
    {
		m_window->Resize(m_window->Width(),m_window->Height());
		m_window->GrabRegion()->Resize(m_window->Width(),m_window->Height());
		m_window->SetStronglyModal(FALSE);
		m_window->SetDraggable(TRUE);

    	Initialize(windowBlock);
	}
}

WatchList::~WatchList(void)
{
    if (g_c3ui && m_window)
    {
	    g_c3ui->RemoveWindow(m_window->Id());
    }

    if (m_list)
    {
	    m_list->Clear();
    }

	delete m_list;
	delete m_newButton;
    delete m_clearButton;
    delete m_exitButton;
	delete m_window;
}

void WatchListActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if((action != (uint32)AUI_LISTBOX_ACTION_SELECT) &&
	   (action != (uint32)AUI_LISTBOX_ACTION_RMOUSESELECT) &&
	   (action != (uint32)AUI_LISTBOX_ACTION_DOUBLECLICKSELECT))
		return;

	WatchList *list = (WatchList *)cookie;

	WatchListItem *item = (WatchListItem *)list->GetList()->GetSelectedItem();

	if(action == AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {

	}

	if(!item)
		return;

	if(action == AUI_LISTBOX_ACTION_RMOUSESELECT) {
		return;
	}

	return;
}

void WatchListButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	if(!g_watchList)
		return;

	if(control == g_watchList->GetNewButton()) {
		g_watchList->AddExpression("");
	}

	if(control == g_watchList->GetClearButton()) {
		g_watchList->Clear();
	}

	if(control == g_watchList->GetExitButton()) {
		watchlist_Remove();
	}
}

sint32 WatchList::Initialize(MBCHAR *windowBlock)
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];




	sprintf( controlBlock, "%s.%s", windowBlock, "WatchList" );
	m_list = new c3_ListBox(&errcode, aui_UniqueId(), controlBlock, WatchListActionCallback, this);
	m_list->SetAbsorbancy(FALSE);
	m_list->Clear();

	Assert( AUI_NEWOK(m_list, errcode) );
	if ( !AUI_NEWOK(m_list, errcode) )
		return -1;

	sprintf(controlBlock, "%s.%s", windowBlock, "NewButton");
	m_newButton = new c3_Button(&errcode, aui_UniqueId(), controlBlock, WatchListButtonCallback, this);

	sprintf(controlBlock, "%s.%s", windowBlock, "ClearButton");
	m_clearButton = new c3_Button(&errcode, aui_UniqueId(), controlBlock, WatchListButtonCallback, this);

	sprintf(controlBlock, "%s.%s", windowBlock, "ExitButton");
	m_exitButton = new c3_Button(&errcode, aui_UniqueId(), controlBlock, WatchListButtonCallback, this);

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return 0;
}

void WatchList::DisplayWindow()
{
	AUI_ERRCODE auiErr;

	UpdateData();

	auiErr = g_c3ui->AddWindow(m_window);
	Assert(auiErr == AUI_ERRCODE_OK);

	keypress_RegisterHandler(this);
}

void WatchList::RemoveWindow(void)
{
	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow(m_window->Id());
	Assert(auiErr == AUI_ERRCODE_OK);

	keypress_RemoveHandler(this);
}

void WatchList::kh_Close()
{
	RemoveWindow();
}

sint32 WatchList::UpdateData(void)
{
	MBCHAR ldlBlock[k_AUI_LDL_MAXBLOCK + 1];

	strcpy(ldlBlock, "WatchListItem");

	return 0;
}

void WatchList::AddExpression(char *exp)
{
	AUI_ERRCODE retval;
	WatchListItem *item = new WatchListItem(&retval, 0, exp, "WatchListItem");
	m_list->AddItem(item);
}

void WatchList::Clear()
{
	m_list->Clear();
}




WatchListItem::WatchListItem(AUI_ERRCODE *retval, sint32 index,
							 MBCHAR *line, MBCHAR *ldlBlock) :
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem(retval, ldlBlock)
{
	m_index = index;
	m_break = false;

	strncpy(m_line, line, k_MAX_WATCH_LINE);
	m_watching = new PointerList<SlicSymbolData>;

	Assert(AUI_SUCCESS(*retval));
	if(!AUI_SUCCESS(*retval)) return;

	*retval = InitCommonLdl(ldlBlock);
	Assert(AUI_SUCCESS(*retval));
	if(!AUI_SUCCESS(*retval)) return;
}

WatchListItem::~WatchListItem()
{
	if(m_watching) {
		PointerList<SlicSymbolData>::Walker walk(m_watching);
		while(walk.IsValid()) {
			walk.GetObj()->RemoveWatch(this);
			walk.Next();
		}

		delete m_watching;
		m_watching = NULL;
	}
}

void WatchExpressionItemCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action == (uint32)AUI_TEXTFIELD_ACTION_EXECUTE) {
		WatchListItem *item = (WatchListItem *)cookie;
		item->Update();
	}
}

void WatchBreakItemCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action == (uint32)AUI_TEXTFIELD_ACTION_EXECUTE) {
		WatchListItem *item = (WatchListItem *)cookie;
		item->ToggleBreak();
		item->Update();
	}
}

AUI_ERRCODE WatchListItem::InitCommonLdl(MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	C3TextField *expressionItem;
	c3_Static *valueItem;
	c3_Static *breakItem;

	sprintf(block, "%s.%s", ldlBlock, "Break");
	breakItem = new c3_Static(&retval, aui_UniqueId(), block);
	breakItem->SetActionFuncAndCookie(WatchBreakItemCallback, this);
	AddChild(breakItem);

	sprintf(block, "%s.%s", ldlBlock, "Expression");
	expressionItem = new C3TextField(&retval, aui_UniqueId(), block, WatchExpressionItemCallback, this);
	expressionItem->SetFieldText(m_line);
	AddChild(expressionItem);

	sprintf(block, "%s.%s", ldlBlock, "Value");
	valueItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(valueItem);

	Update();
	return AUI_ERRCODE_OK;
}

void WatchListItem::Update(void)
{
	C3TextField *expressionItem;
	expressionItem = (C3TextField *)GetChildByIndex(1);
	expressionItem->GetFieldText(m_line, k_MAX_WATCH_LINE);

	sint32 res;
	char valbuf[1024];
	char catString = 0;
	res = sliccmd_parse(SLICCMD_WATCH, m_line, valbuf, 1024, 0, &catString);

	while(m_watching->GetHead()) {
		m_watching->RemoveHead();
	}

	sliccmd_add_watch(this);

	if(res != 0) {
		strcpy(valbuf, "--");
	}

	c3_Static *valueItem = (c3_Static *)GetChildByIndex(2);
	valueItem->SetText(valbuf);

	c3_Static *breakItem = (c3_Static *)GetChildByIndex(0);
	if(m_break) {
		breakItem->SetText("X");
	} else {
		breakItem->SetText("-");
	}
}

sint32 WatchListItem::Compare(c3_ListItem *item2, uint32 column)
{
	return 0;
}

void WatchListItem::WatchCallback(SlicSymbolData *symbol, bool isAddCallback)
{
	if(!isAddCallback) {
		Update();
		if(m_break) {
			g_slicEngine->RequestBreak();
		}
	} else {
		m_watching->AddTail(symbol);
	}
}

void WatchListItem::WatchVariableDeleted(SlicSymbolData *symbol)
{
	PointerList<SlicSymbolData>::Walker walk(m_watching);
	while(walk.IsValid()) {
		if(walk.GetObj() == symbol) {
			walk.Remove();
		} else {
			walk.Next();
		}
	}
}
