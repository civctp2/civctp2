//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling of the key mapping screen
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - 
//
//----------------------------------------------------------------------------


#include "c3.h"
#include "c3window.h"
#include "c3_popupwindow.h"
#include "ctp2_button.h"
#include "c3_listitem.h"
#include "c3_static.h"
#include "c3ui.h"
#include "c3_listbox.h"
#include "radiogroup.h"
#include "c3_switch.h"
#include "aui_uniqueid.h"
#include "aui_ranger.h"

#include "aui_stringtable.h"

#include "keymap.h"
#include "keypress.h"

#include "c3_dropdown.h"	
#include "spnewgamewindow.h"
#include "km_screen.h"

#include "StrDB.h"
#include "controlpanelwindow.h"
#include "ctp2_Static.h"

extern C3UI			*g_c3ui;
extern KEYMAP		*theKeyMap;
extern StringDB     *g_theStringDB;

static c3_PopupWindow	*s_km_screen	= NULL;





static ctp2_Static  *s_groupStatic = NULL;
static ctp2_Button	**s_switch			= NULL;

static ctp2_Button	*s_resetButton		= NULL;

static MBCHAR		s_ldl[KM_MAX][255] = {
	"Basic",
	"Unit",
	"Screen",
	"Map",
	"Game"

};

static c3_ListBox	*s_keyList			= NULL;
static aui_StringTable *s_strings		= NULL;

static sint32		s_selected;

sint32	g_isKMScreen = FALSE;




sint32	km_screen_displayMyWindow()
{
	sint32 retval=0;
	g_isKMScreen = TRUE;
	if(!s_km_screen) { retval = km_screen_Initialize(); }
	else {
		km_screen_loadKeyList();
	}

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->AddWindow(s_km_screen);
	Assert( auiErr == AUI_ERRCODE_OK );

	return retval;
}
sint32 km_screen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	if(!s_km_screen) return 0;

	AUI_ERRCODE auiErr;

	if ( s_km_screen ) {
		auiErr = g_c3ui->RemoveWindow( s_km_screen->Id() );
		Assert( auiErr == AUI_ERRCODE_OK );
	}

	g_isKMScreen = FALSE;

	if(theKeyMap) {
		theKeyMap->save();
	}

	return 1;
}



AUI_ERRCODE km_screen_Initialize( void )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		groupBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32 i;

	if ( s_km_screen ) return AUI_ERRCODE_OK; 

	strcpy(windowBlock, "KmScreen");

	{ 
		s_km_screen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING );
		Assert( AUI_NEWOK(s_km_screen, errcode) );
		if ( !AUI_NEWOK(s_km_screen, errcode) ) errcode;

		
		s_km_screen->Resize(s_km_screen->Width(),s_km_screen->Height());
		s_km_screen->GrabRegion()->Resize(s_km_screen->Width(),s_km_screen->Height());
		s_km_screen->SetStronglyModal(TRUE);
	}
	
	s_km_screen->AddClose( km_screen_backPress );


	sprintf( controlBlock, "%s.%s", windowBlock, "Name" );
	s_km_screen->AddTitle( controlBlock );


	
	s_switch = new ctp2_Button*[KM_MAX];

	sprintf( groupBlock, "%s.%s", windowBlock, "Group" );
	
	
	

	s_groupStatic = new ctp2_Static(&errcode, aui_UniqueId(), groupBlock);
	Assert(AUI_NEWOK(s_groupStatic, errcode));
	if(!AUI_NEWOK(s_groupStatic, errcode)) return errcode;

	for ( i = KM_BASIC;i < KM_MAX;i++ ) {
		sprintf( controlBlock, "%s.%s", groupBlock, s_ldl[i] );
		s_switch[i] = new ctp2_Button( &errcode, aui_UniqueId(), controlBlock, km_screen_switchPress );
		Assert( AUI_NEWOK(s_switch[i], errcode) );
		if ( !AUI_NEWOK(s_switch[i], errcode) ) return errcode;
	}
 
	s_switch[0]->SetToggleState(true);

	
	sprintf( controlBlock, "%s.%s", windowBlock, "ResetButton" );
	s_resetButton = new ctp2_Button( &errcode, aui_UniqueId(), controlBlock, km_screen_resetPress );
	Assert( AUI_NEWOK(s_resetButton, errcode) );
	if ( !AUI_NEWOK(s_resetButton, errcode) ) return errcode;

	
	sprintf( controlBlock, "%s.%s", windowBlock, "KeyList" );
	s_keyList = new c3_ListBox( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_keyList, errcode) );
	if ( !AUI_NEWOK(s_keyList, errcode) ) return errcode;
	
	
	sprintf( controlBlock, "%s", "KeylistStrings" );
	s_strings = new aui_StringTable( &errcode, controlBlock );
	Assert( AUI_NEWOK(s_strings , errcode) );
	if ( !AUI_NEWOK(s_strings , errcode) ) return errcode;

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	km_screen_loadKeyList();

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE km_screen_Cleanup()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	if ( !s_km_screen  ) return AUI_ERRCODE_OK; 

	g_c3ui->RemoveWindow( s_km_screen->Id() );




	mycleanup( s_groupStatic );

	for ( sint32 i = KM_BASIC;i < KM_MAX;i++ ) {
		mycleanup( s_switch[i] );
	}
	mycleanup( s_switch );

	mycleanup( s_resetButton );

	mycleanup( s_keyList );
	mycleanup( s_strings );

	delete s_km_screen;
	s_km_screen = NULL;

	return AUI_ERRCODE_OK;

#undef mycleanup
}




void km_screen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if(km_screen_removeMyWindow(action)) {
	}
}

void km_screen_resetPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	cleanup_keymap();
	init_defaultKeymap();
	km_screen_loadKeyList();
}

void km_screen_switchPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	sint32 i;

	switch ( action ) {
	case AUI_SWITCH_ACTION_ON:
		for ( i = KM_BASIC;i < KM_MAX;i++ ) {
			
			if ( control == s_switch[i] ) {
				s_switch[i]->SetToggleState(true);
				s_selected = i;
			} else {
				s_switch[i]->SetToggleState(false);
			}
		}		
		
		km_screen_loadKeyList();
		break;
	case AUI_SWITCH_ACTION_OFF:
		break;
	}
}

sint32 km_screen_remapKey( WPARAM wParam )
{
	KeyListItem *item;

	
	item = (KeyListItem *)s_keyList->GetSelectedItem();
	if ( !item ) return 0;

	KEY_FUNCTION keyFunc = KEY_FUNCTION(item->GetKey());
	uint32 keycode = item->GetKeycode();

	theKeyMap->remap_key( keyFunc, wParam, keycode );
	item->UpdateKey( theKeyMap->get_keycode(keyFunc) );

	
	
	sint32 curIndex = s_keyList->GetSelectedItemIndex();
	sint32 curRangerPos = s_keyList->GetVerticalRanger()->GetValueY();

	km_screen_loadKeyList();

	s_keyList->SelectItem(curIndex);
	s_keyList->GetVerticalRanger()->SetValue(0, curRangerPos);

	return 1;
}

sint32 km_screen_loadKeyList( void )
{
	AUI_ERRCODE errcode;
	MBCHAR ldl[_MAX_PATH];

	sint32 i;
	sint32 keycode = 0;

	
	s_keyList->Clear();

	KeyListItem *item;
	sprintf( ldl, "KeyListItem" );
	
	switch ( s_selected ) {
	case KM_BASIC:
		for ( i = KEY_FUNCTION_MOVE_NORTH;i <= KEY_FUNCTION_NEXT_ROUND;i++ ) {
			item = new KeyListItem( &errcode, i, theKeyMap->get_keycode(KEY_FUNCTION(i)), ldl );
			Assert( AUI_NEWOK(item, errcode) );
			if ( !AUI_NEWOK(item, errcode) ) return -1;

			s_keyList->AddItem( (c3_ListItem *)item );
		}
		break;

	case KM_UNIT:
		for ( i = KEY_FUNCTION_OPEN_WORK_VIEW;i <= KEY_FUNCTION_PROCESS_UNIT_ORDERS;i++ ) {
			item = new KeyListItem( &errcode, i, theKeyMap->get_keycode(KEY_FUNCTION(i)), ldl );
			Assert( AUI_NEWOK(item, errcode) );
			if ( !AUI_NEWOK(item, errcode) ) return -1;

			s_keyList->AddItem( (c3_ListItem *)item );
		}
		break;

	case KM_SCREEN:
		for ( i = KEY_FUNCTION_OPEN_CIV_STATUS;i <= KEY_FUNCTION_NO;i++ ) {
			if ( i == KEY_FUNCTION_OPEN_CITY_VIEW ) continue;

			item = new KeyListItem( &errcode, i, theKeyMap->get_keycode(KEY_FUNCTION(i)), ldl );
			Assert( AUI_NEWOK(item, errcode) );
			if ( !AUI_NEWOK(item, errcode) ) return -1;

			s_keyList->AddItem( (c3_ListItem *)item );
		}
		break;

	case KM_MAP:
#ifdef _PLAYTEST
		for ( i = KEY_FUNCTION_TOGGLE_CITY_NAMES;i <= KEY_FUNCTION_TOGGLE_SPACE;i++ ) {
#else 
		for ( i = KEY_FUNCTION_TOGGLE_CITY_NAMES;i <= KEY_FUNCTION_ZOOM_OUT1;i++ ) {
#endif
			item = new KeyListItem( &errcode, i, theKeyMap->get_keycode(KEY_FUNCTION(i)), ldl );
			Assert( AUI_NEWOK(item, errcode) );
			if ( !AUI_NEWOK(item, errcode) ) return -1;

			s_keyList->AddItem( (c3_ListItem *)item );
		}
		break;
	case KM_GAME:
		for ( i = KEY_FUNCTION_REMAP_KEYBOARD;i <= KEY_FUNCTION_MUSIC_OPTIONS;i++ ) {
			item = new KeyListItem( &errcode, i, theKeyMap->get_keycode(KEY_FUNCTION(i)), ldl );
			Assert( AUI_NEWOK(item, errcode) );
			if ( !AUI_NEWOK(item, errcode) ) return -1;

			s_keyList->AddItem( (c3_ListItem *)item );
		}
		break;
	}

	return 0;
}


KeyListItem::KeyListItem(AUI_ERRCODE *retval, sint32 index, uint32 keycode, MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(index, keycode, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;	
}


AUI_ERRCODE KeyListItem::InitCommonLdl(sint32 index, uint32 keycode, MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	m_name = s_strings->GetString(index);
	m_index = index;
	m_keycode = keycode;

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "Name");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	sprintf( block, "%s.%s", ldlBlock, "Keycode" );
	subItem = new c3_Static( &retval, aui_UniqueId(), block );
	AddChild( subItem );

	Update();

 	return AUI_ERRCODE_OK;
}

MBCHAR *km_GetKeyName(uint32 code)
{
	static MBCHAR str[_MAX_PATH];
	const char *ctrl = g_theStringDB->GetNameStr("str_control_key");

	switch(code) {
		case '\t' + 128: strcpy(str, g_theStringDB->GetNameStr("KEY_NAME_TAB")); break;
		case '\r' + 128: strcpy(str, g_theStringDB->GetNameStr("KEY_NAME_ENTER"));  break;
		case ' ': strcpy(str, g_theStringDB->GetNameStr("KEY_NAME_SPACE"));   break;
		case '\b' + 128:   strcpy(str, g_theStringDB->GetNameStr("KEY_NAME_BACKSPACE")); break;
		case '0' + 128: strcpy(str, "F10"); break;
		case '!' + 128: strcpy(str, "F11"); break;
		case '@' + 128: strcpy(str, "F12"); break;
		default:
			if(code >= ('1' | 0x80) && code <= ('9' | 0x80)) {
				sprintf(str, "F%d", code & 0x7f - '1' + 1);
			} else if(isprint(code)) {
				sprintf(str, "%c", code);
			} else if(code >= 1 && code <= 26) {
				sprintf(str, "%s + %c", ctrl, code + 'a' - 1);
			} else {
				sprintf(str, "%c", code); 
			}
			break;
	}
	return str;
}

void KeyListItem::Update(void)
{
	c3_Static *subItem;
	MBCHAR *str;

	str = km_GetKeyName(m_keycode);

	
	subItem = (c3_Static *)GetChildByIndex(0);
	subItem->SetText(m_name);

	
	subItem = (c3_Static *)GetChildByIndex(1);
	subItem->SetText( str );
}

void KeyListItem::UpdateKey( uint32 keycode )
{
	c3_Static *subItem;
	MBCHAR *str;

	m_keycode = keycode;

	str = km_GetKeyName(m_keycode);

	
	subItem = (c3_Static *)GetChildByIndex(1);
	subItem->SetText( str );

	s_keyList->ShouldDraw();
}

MBCHAR *KeyListItem::GetKeyFromKMScreen(uint32 keycode)
{
	return km_GetKeyName(keycode);
}

sint32 KeyListItem::Compare(c3_ListItem *item2, uint32 column)
{
	c3_Static		*i1, *i2;

	if (column < 0) return 0;

	switch (column) {
	case 0:
		i1 = (c3_Static *)this->GetChildByIndex(column);
		i2 = (c3_Static *)item2->GetChildByIndex(column);

		return strcmp(i1->GetText(), i2->GetText());

		break;
	case 1:
		i1 = (c3_Static *)this->GetChildByIndex(column);
		i2 = (c3_Static *)item2->GetChildByIndex(column);

		return strcmp(i1->GetText(), i2->GetText());

		break;
	}

	return 0;
}


