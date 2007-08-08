//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling of the key mapping screen
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
// _PLAYTEST
// - Builds executable with additional features for playtesting
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "km_screen.h"

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

#include "StrDB.h"
#include "controlpanelwindow.h"
#include "ctp2_Static.h"

extern C3UI			*g_c3ui;
extern KEYMAP		*theKeyMap;
extern StringDB     *g_theStringDB;

namespace
{
    // Key mapping categories
    enum KM 
    {
        KM_BASIC,
	    KM_UNIT,
	    KM_SCREEN,
	    KM_MAP,
	    KM_GAME,

        // placeholder, used as counter
	    KM_MAX
    };

    class TabDescriptor
    {
    public:
        TabDescriptor
        (
            MBCHAR const *      name,
            size_t              count
        )
        :   Name                (name),
            Count               (count)
        { ; };

        MBCHAR const *          Name;
        size_t const            Count;

    private:
        // Not used. Defined to prevent compiler warning.
        TabDescriptor & TabDescriptor::operator = (TabDescriptor const &);

    };

    TabDescriptor const TAB_INVALID ("", 0);

    KEY_FUNCTION const  BASIC_FUNCTION[]        =
        {
            KEY_FUNCTION_MOVE_NORTH,
            KEY_FUNCTION_MOVE_NORTHWEST,
            KEY_FUNCTION_MOVE_NORTHEAST,
            KEY_FUNCTION_MOVE_WEST,
            KEY_FUNCTION_MOVE_EAST,
            KEY_FUNCTION_MOVE_SOUTHWEST,
            KEY_FUNCTION_MOVE_SOUTH,
            KEY_FUNCTION_MOVE_SOUTHEAST,	
            KEY_FUNCTION_UNIT_CITY_TOGGLE,
            KEY_FUNCTION_NEXT_ITEM,
            KEY_FUNCTION_KEYBOARD_SELECT_UNIT,
            KEY_FUNCTION_NEXT_ROUND,
//          KEY_FUNCTION_ENDTURN,           // CTP2: "--" placeholder in km_screen.ldl				
        };

    TabDescriptor const TAB_BASIC 
        ("Basic", sizeof(BASIC_FUNCTION) / sizeof(BASIC_FUNCTION[0]));

    KEY_FUNCTION const  UNIT_FUNCTION[]         =
        {
            KEY_FUNCTION_OPEN_WORK_VIEW,    // CTP2: reused for army manager
            KEY_FUNCTION_SLEEP,
            KEY_FUNCTION_ENTRENCH,
            KEY_FUNCTION_SETTLE, 
            KEY_FUNCTION_PILLAGE,
            KEY_FUNCTION_BOMBARD,
            KEY_FUNCTION_REFORM,
            KEY_FUNCTION_EXPEL,
            KEY_FUNCTION_UNLOAD_TRANS, 
            KEY_FUNCTION_INVESTIGATE_CITY,
            KEY_FUNCTION_PLANT_NUKE,
            KEY_FUNCTION_BIOINFECT,
            KEY_FUNCTION_NANOTERROR,
            KEY_FUNCTION_CREATE_PARK,
            KEY_FUNCTION_SPACE_LAUNCH,
            // Apolyton additions
            KEY_FUNCTION_DESCEND,           // CTP2: "--" placeholder in km_screen.ldl 
            KEY_FUNCTION_PARADROP,          
            KEY_FUNCTION_GROUP_ARMY, 
            KEY_FUNCTION_UNGROUP_ARMY, 
            KEY_FUNCTION_MOVE_ORDER,
            KEY_FUNCTION_END_UNIT_TURN,
            KEY_FUNCTION_EXECUTE_ORDERS,
            KEY_FUNCTION_PROCESS_UNIT_ORDERS,
};
    TabDescriptor const TAB_UNIT
        ("Unit", sizeof(UNIT_FUNCTION) / sizeof(UNIT_FUNCTION[0]));

    KEY_FUNCTION const  SCREEN_FUNCTION[]       =
        {
            KEY_FUNCTION_OPEN_CIV_STATUS,
            KEY_FUNCTION_OPEN_CITY_STATUS,
            KEY_FUNCTION_CITY_MANAGEMENT,
            KEY_FUNCTION_BUILD_QUEUE,
            KEY_FUNCTION_CLEAR_QUEUE,
            KEY_FUNCTION_OPEN_SCIENCE_STATUS,
            KEY_FUNCTION_OPEN_UNIT_STATUS,
            KEY_FUNCTION_OPEN_TRADE_STATUS,
            KEY_FUNCTION_TRADE_SUMMARY,    
            KEY_FUNCTION_OPEN_DIPLOMACY,
            KEY_FUNCTION_NEW_PROPOSAL,
            KEY_FUNCTION_OPEN_MESSAGE,
            KEY_FUNCTION_GAIA,
            KEY_FUNCTION_OPEN_GREAT_LIBRARY,
            KEY_FUNCTION_OPEN_INFO_SCREEN,
            KEY_FUNCTION_TIMELINE,          // CTP2: wonders
            KEY_FUNCTION_RANK,
            KEY_FUNCTION_CIV_TAB,
            KEY_FUNCTION_CITY_TAB,
            KEY_FUNCTION_UNIT_TAB,
            KEY_FUNCTION_MSG_TAB,
            KEY_FUNCTION_TILE_TAB,
            KEY_FUNCTION_CHAT_KEY,
            KEY_FUNCTION_NETWORK_PLAYERS_SCREEN,
            KEY_FUNCTION_OPEN_SCENARIO_EDITOR,
            // Apolyton additions
            KEY_FUNCTION_EXECUTE_EYEPOINT,
            KEY_FUNCTION_YES,
            KEY_FUNCTION_NO,
        };
    TabDescriptor const TAB_SCREEN
        ("Screen", sizeof(SCREEN_FUNCTION) / sizeof(SCREEN_FUNCTION[0]));

    KEY_FUNCTION const  MAP_FUNCTION[]          =
        {
            KEY_FUNCTION_TOGGLE_CITY_NAMES,
            KEY_FUNCTION_TOGGLE_TRADE_ROUTES,
            KEY_FUNCTION_CENTER_MAP,				
            KEY_FUNCTION_CLOSE,
            KEY_FUNCTION_TOGGLE_RADAR,
            KEY_FUNCTION_TOGGLE_CONTROL_PANEL,
            KEY_FUNCTION_TOGGLE_ALL,
            KEY_FUNCTION_ZOOM_IN1,
            KEY_FUNCTION_ZOOM_OUT1,
            // Apolyton additions
#if defined(_PLAYTEST)
            KEY_FUNCTION_GAMESTATE_DEBUG,
            KEY_FUNCTION_RAND_TEST,
            KEY_FUNCTION_ENTER_COMMAND,
            KEY_FUNCTION_ENTER_COMMAND_ALTERNATE,
#endif
#if 0 // Old CTP1 functions
            KEY_FUNCTION_TOGGLE_SPACE,
#endif
        };
    TabDescriptor const TAB_MAP
        ("Map", sizeof(MAP_FUNCTION) / sizeof(MAP_FUNCTION[0]));

    KEY_FUNCTION const  GAME_FUNCTION[]         =
        {
            KEY_FUNCTION_REMAP_KEYBOARD,			
            KEY_FUNCTION_OPEN_OPTIONS_SCREEN,
            KEY_FUNCTION_SOUND_OPTIONS,
            KEY_FUNCTION_GRAPHICS_OPTIONS,
            KEY_FUNCTION_GAMEPLAY_OPTIONS,
            KEY_FUNCTION_ADVANCED_OPTIONS,
            KEY_FUNCTION_RESTART,
            KEY_FUNCTION_NEW_GAME,
            KEY_FUNCTION_SAVE_GAME,
            KEY_FUNCTION_LOAD_GAME,
            KEY_FUNCTION_SAVE_WORLD,
            KEY_FUNCTION_LOAD_WORLD,
            KEY_FUNCTION_QUIT,
            // Apolyton addition (Ahenobarb)
            KEY_FUNCTION_MUSIC_OPTIONS,
        };
    TabDescriptor const TAB_GAME
        ("Game", sizeof(GAME_FUNCTION) / sizeof(GAME_FUNCTION[0]));

    TabDescriptor const * TAB[KM_MAX] =
        {
            &TAB_BASIC,
            &TAB_UNIT,
            &TAB_SCREEN,
            &TAB_MAP,
            &TAB_GAME
        };

//----------------------------------------------------------------------------
// Function prototypes
//----------------------------------------------------------------------------

    AUI_ERRCODE km_screen_Initialize(void);
    void        km_screen_loadKeyList(void);

//----------------------------------------------------------------------------
// Variables
//----------------------------------------------------------------------------

    ctp2_Static  *      s_groupStatic       = NULL;
    c3_ListBox	*       s_keyList			= NULL;
    c3_PopupWindow *    s_km_screen	        = NULL;
    ctp2_Button	*       s_resetButton		= NULL;
    KM                  s_selected          = KM_BASIC;
    aui_StringTable *   s_strings		    = NULL;
    ctp2_Button	**      s_switch			= NULL;

//----------------------------------------------------------------------------
// Function definitions
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
// Name       : km_screen_loadKeyList
//
// Description: Generate a list of key mappings to display
//
// Parameters : -
//
// Globals    : s_selected  : (input) category of key mappings to display
//              s_keyList   : (output) the generated list
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void km_screen_loadKeyList(void)
{
	MBCHAR ldl[_MAX_PATH];
	sprintf(ldl, "KeyListItem");

    KEY_FUNCTION const  *   functionList;
    size_t                  functionCount;

	switch (s_selected) 
    {
    default:
        functionCount   = TAB_INVALID.Count;
        functionList    = NULL;
        break;

	case KM_BASIC:
        functionCount   = TAB[s_selected]->Count;
        functionList    = BASIC_FUNCTION;
		break;

	case KM_UNIT:
        functionCount   = TAB[s_selected]->Count;
        functionList    = UNIT_FUNCTION;
		break;

	case KM_SCREEN:
        functionCount   = TAB[s_selected]->Count;
        functionList    = SCREEN_FUNCTION;
        break;

	case KM_MAP:
        functionCount   = TAB[s_selected]->Count;
        functionList    = MAP_FUNCTION;
		break;

	case KM_GAME:
        functionCount   = TAB[s_selected]->Count;
        functionList    = GAME_FUNCTION;
		break;
	}


	s_keyList->Clear();
	for (size_t i = 0; i < functionCount; ++i) 
    {
        AUI_ERRCODE             errcode = AUI_ERRCODE_OK;
        KEY_FUNCTION const &    f       = functionList[i];
    	KeyListItem *           item    = 
            new KeyListItem(&errcode, f, theKeyMap->get_keycode(f), ldl);
		Assert(AUI_NEWOK(item, errcode));

		if (AUI_NEWOK(item, errcode))
        {
            if (item->GetName())
            {
    		    s_keyList->AddItem(item);
}
            else
            {
                // Function not supported by this mod
                delete item;
            }
        }
    }
}


//----------------------------------------------------------------------------
//
// Name       : km_screen_Initialize
//
// Description: Initialise static variables
//
// Parameters : -
//
// Globals    : s_groupStatic
//              s_keyList
//              s_km_screen
//              s_resetButton
//              s_strings
//              s_switch
//
// Returns    : AUI_ERRCODE : initialisation result
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
AUI_ERRCODE km_screen_Initialize( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
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
		sprintf(controlBlock, "%s.%s", groupBlock, TAB[i]->Name);
		s_switch[i] = new ctp2_Button( &errcode, aui_UniqueId(), controlBlock, km_screen_switchPress );
		Assert( AUI_NEWOK(s_switch[i], errcode) );
		if ( !AUI_NEWOK(s_switch[i], errcode) ) return errcode;
	}
 
	s_switch[s_selected]->SetToggleState(true);

	
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

	return AUI_ERRCODE_OK;
}


} // namespace








sint32	g_isKMScreen = FALSE;




sint32	km_screen_displayMyWindow()
{
	g_isKMScreen = TRUE;

	sint32 const    retval  = s_km_screen ? 0 : km_screen_Initialize();
	km_screen_loadKeyList();

    if (g_c3ui)
    {
	    AUI_ERRCODE const   auiErr = g_c3ui->AddWindow(s_km_screen);
	    Assert(auiErr == AUI_ERRCODE_OK);
        g_c3ui->RegisterCleanup(&km_screen_Cleanup);
}

	return retval;
}

sint32 km_screen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	if(!s_km_screen) return 0;


	if (g_c3ui && s_km_screen) 
{
	    AUI_ERRCODE const auiErr = g_c3ui->RemoveWindow(s_km_screen->Id());
		Assert( auiErr == AUI_ERRCODE_OK );
	}

	g_isKMScreen = FALSE;

	if(theKeyMap) {
		theKeyMap->save();
	}

	return 1;
}




void km_screen_Cleanup()
{
    g_isKMScreen    = FALSE;

#define mycleanup(mypointer) { delete mypointer; mypointer = NULL; };

	if (g_c3ui && s_km_screen)
    {
    	g_c3ui->RemoveWindow(s_km_screen->Id());
    }

	mycleanup( s_groupStatic );

    if (s_switch)
    {
	    for (size_t i = 0; i < KM_MAX; ++i) 
        {
		mycleanup( s_switch[i] );
	}
        delete [] s_switch;
        s_switch = NULL;
    }

	mycleanup( s_resetButton );
	mycleanup( s_keyList );
	mycleanup( s_strings );
	mycleanup(s_km_screen);
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
	if (AUI_SWITCH_ACTION_ON == action) 
    {
		s_switch[s_selected]->SetToggleState(false);

		for (size_t i = 0; i < KM_MAX; ++i) 
        {
			if (control == s_switch[i]) 
            {
				s_switch[i]->SetToggleState(true);
				s_selected = static_cast<KM>(i);
		        km_screen_loadKeyList();
			}
		}		
	}
}

sint32 km_screen_remapKey( WPARAM wParam, LPARAM lParam )
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

//----------------------------------------------------------------------------
//
// Name       : km_GetKeyName
//
// Description: Get the name of a key code.
//
// Parameters : code            : the key code
//
// Globals    : g_theStringDB
//
// Returns    : MBCHAR const *  : the name
//
// Remark(s)  : The returned result is only valid until the next call of this
//              function.
//              TODO: replace this with something like a std::string, to get
//                    rid of this life-time problem.
//
//----------------------------------------------------------------------------
MBCHAR const * km_GetKeyName(uint32 code)
{
	static MBCHAR str[_MAX_PATH];
	const char *ctrl = g_theStringDB->GetNameStr("str_control_key");

	switch(code) {
		case '\t' + 128: strcpy(str, g_theStringDB->GetNameStr("KEY_NAME_TAB")); break;
		case '\r' + 128: strcpy(str, g_theStringDB->GetNameStr("KEY_NAME_ENTER"));  break;
		case ' ': strcpy(str, g_theStringDB->GetNameStr("KEY_NAME_SPACE"));   break;
		case 8 + 128:   strcpy(str, g_theStringDB->GetNameStr("KEY_NAME_BACKSPACE")); break;
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

	return const_cast<MBCHAR const *>(str);
}

void KeyListItem::Update(void)
{
	c3_Static * subName = static_cast<c3_Static *>(GetChildByIndex(0));
    subName->SetText(m_name);

    c3_Static * subKey  = static_cast<c3_Static *>(GetChildByIndex(1));
    subKey->SetText(km_GetKeyName(m_keycode));
}

void KeyListItem::UpdateKey( uint32 keycode )
{
	m_keycode = keycode;

	c3_Static * subKey  = static_cast<c3_Static *>(GetChildByIndex(1));
	subKey->SetText(km_GetKeyName(m_keycode));

	s_keyList->ShouldDraw();
}

//----------------------------------------------------------------------------
//
// Name       : KeyListItem::GetKeyFromKMScreen
//
// Description: Get the name of a key code.
//
// Parameters : keycode         : the key code
//
// Globals    : -
//
// Returns    : MBCHAR const *  : the name
//
// Remark(s)  : The returned result is only valid until the next call of this
//              function.
//              TODO: replace this with something like a std::string, to get
//                    rid of this life-time problem.
//
//----------------------------------------------------------------------------
MBCHAR const * KeyListItem::GetKeyFromKMScreen(uint32 keycode)
{
	return km_GetKeyName(keycode);
}

sint32 KeyListItem::Compare(c3_ListItem *item2, uint32 column)
{
	switch (column) 
    {
    default:
        break;

	case 0:
	case 1:
        {
		    c3_Static * i1 = (c3_Static *)this->GetChildByIndex(column);
		    c3_Static * i2 = (c3_Static *)item2->GetChildByIndex(column);

		return strcmp(i1->GetText(), i2->GetText());
	}
	}

	return 0;
}


