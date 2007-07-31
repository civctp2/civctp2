

#include "c3.h"
#include "c3window.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_listitem.h"
#include "c3_listbox.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3_switch.h"
#include "c3_checkbox.h"
#include "c3ui.h"
#include "profiledb.h"
#include "spnewgamewindow.h"
#include "aui_uniqueid.h"
#include "aui_stringtable.h"
#include "SoundManager.h"
#include "civ3_main.h"
#include "PointerList.h"


#include "display.h"

#include "graphicsscreen.h"
#include "graphicsresscreen.h"

extern C3UI				*g_c3ui;
extern ProfileDB		*g_theProfileDB;

static c3_PopupWindow	*s_graphicsResScreen= NULL;
static c3_Button		*s_accept			= NULL;
static c3_ListBox		*s_resList			= NULL;
static c3_Static		*s_warning			= NULL;
static sint32			s_currentResIndex	= 0;
static CTPDisplayMode	*s_selectedDisplayMode = NULL;

extern sint32 g_ScreenWidth, g_ScreenHeight;

extern PointerList<CTPDisplayMode> *g_displayModes;




sint32	graphicsresscreen_displayMyWindow()
{
	sint32 retval=0;
	if(!s_graphicsResScreen) { retval = graphicsresscreen_Initialize(); }

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->AddWindow(s_graphicsResScreen);
	Assert( auiErr == AUI_ERRCODE_OK );

	return retval;
}
sint32 graphicsresscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( s_graphicsResScreen->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	return 1;
}



void ScreenResListCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT ) return;

	
	ScreenResListItem *item = (ScreenResListItem *) s_resList->GetSelectedItem();
	if (!item)
		return;

	s_selectedDisplayMode = item->GetDisplayMode();

	if (s_selectedDisplayMode->width == g_ScreenWidth && 
		s_selectedDisplayMode->height == g_ScreenHeight) {
		s_warning->Hide();
	} else {
		s_warning->Show();
	}
}





AUI_ERRCODE graphicsresscreen_Initialize( void )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( s_graphicsResScreen ) return AUI_ERRCODE_OK; 

	strcpy(windowBlock, "ScreenResScreen");
	{ 
		s_graphicsResScreen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false );
		Assert( AUI_NEWOK(s_graphicsResScreen, errcode) );
		if ( !AUI_NEWOK(s_graphicsResScreen, errcode) ) errcode;

		
		s_graphicsResScreen->Resize(s_graphicsResScreen->Width(),s_graphicsResScreen->Height());
		s_graphicsResScreen->GrabRegion()->Resize(s_graphicsResScreen->Width(),s_graphicsResScreen->Height());
		s_graphicsResScreen->SetStronglyModal(TRUE);
	}
	
	s_graphicsResScreen->AddTitle( "ScreenResScreen.Name" );
	s_graphicsResScreen->AddClose( graphicsresscreen_acceptPress );

	sprintf( controlBlock, "%s.%s", windowBlock, "ResList" );
	s_resList = new c3_ListBox( &errcode, aui_UniqueId(), controlBlock, ScreenResListCallback, NULL);
	Assert( AUI_NEWOK(s_resList, errcode) );
	if ( !AUI_NEWOK(s_resList, errcode) ) return errcode;

	sprintf(controlBlock, "%s.%s", windowBlock, "Warning");
	s_warning = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	Assert( AUI_NEWOK(s_warning, errcode) );
	if ( !AUI_NEWOK(s_warning, errcode) ) return errcode;

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	
	s_warning->Hide();

	sprintf(controlBlock, "%s.%s", windowBlock, "ScreenResListItem");

	
	
	
	PointerList<CTPDisplayMode>::PointerListNode	*node;
	sint32		i=0;

	node = g_displayModes->GetHeadNode();
	while (node) {
		CTPDisplayMode *mode = node->GetObj();
		if (mode) {
			ScreenResListItem		*item = new ScreenResListItem(&errcode, mode, controlBlock);
			s_resList->AddItem((aui_Item *)item);
			if (mode->width == g_ScreenWidth && mode->height == g_ScreenHeight) {
				
				s_currentResIndex = i;
			}
		}
		i++;
		node = node->GetNext();
	}

	s_resList->SetForceSelect(TRUE);
	s_resList->SelectItem(s_currentResIndex);

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE graphicsresscreen_Cleanup()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	if ( !s_graphicsResScreen  ) return AUI_ERRCODE_OK; 

	g_c3ui->RemoveWindow( s_graphicsResScreen->Id() );

	mycleanup(s_resList);
	mycleanup(s_warning);

	delete s_graphicsResScreen;
	s_graphicsResScreen = NULL;

	return AUI_ERRCODE_OK;

#undef mycleanup
}




void graphicsresscreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( graphicsresscreen_removeMyWindow(action) ) {
		if (s_selectedDisplayMode) {
			if (s_selectedDisplayMode->width != g_ScreenWidth ||
				s_selectedDisplayMode->height != g_ScreenHeight) {
				g_theProfileDB->SetScreenResWidth(s_selectedDisplayMode->width);
				g_theProfileDB->SetScreenResHeight(s_selectedDisplayMode->height);
				g_theProfileDB->SetTryWindowsResolution(FALSE);
				g_theProfileDB->Save();
			}
		}

		graphicsscreen_displayMyWindow();
	}
}





ScreenResListItem::ScreenResListItem(AUI_ERRCODE *retval, CTPDisplayMode *mode, MBCHAR *ldlBlock)
	:
	c3_ListItem( retval, ldlBlock),
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	
	*retval = InitCommonLdl(mode, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;	
}

AUI_ERRCODE ScreenResListItem::InitCommonLdl(CTPDisplayMode *mode, MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "ScreenRes");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	Assert(subItem);

	MBCHAR resName[80];
	sprintf(resName, "%d x %d", mode->width, mode->height);
	subItem->SetText(resName);

	AddChild(subItem);

	m_mode = mode;
	return AUI_ERRCODE_OK;
}

