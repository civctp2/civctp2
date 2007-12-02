//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : Key map handling
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
// - Added music options key (Ahenobarb)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __KEYMAP_H__
#define __KEYMAP_H__ 1

// Don't change the order of the following otherwise you have also to update
// six km_screen.ldl files. For each language one file.
// And next time check before you fix anything if it was really broken.
// The modified keymap went more than 300 revisions without any problems.
// Noone reported misaligned keys. And problems at loading the keymap.txt are
// handled.
//
// This version is now closer to CTP2 as it should be, of course the should
// be does not only refer to the surface but also to the stuff under the
// hood.
//
// Well you still can revert to a format that keeps backwards compatibility
// with original versions of the km_screen.ldl, but what would be the
// advantage? There is no mod out there that modifies this file. And if
// you have a private mod than you have the controll to update it.
// If you really need to devert from CTP2 as it should be then make at least
// sure that the stuff work with the files everyones uses.
enum KEY_FUNCTION
{
    // No action
    KEY_FUNCTION_NOOP,

    // Basic commands
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
    KEY_FUNCTION_ENDTURN,

    // Unit controls
    KEY_FUNCTION_OPEN_WORK_VIEW,       // Reused for army manager
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
    KEY_FUNCTION_DESCEND,
    KEY_FUNCTION_PARADROP,
    KEY_FUNCTION_GROUP_ARMY,
    KEY_FUNCTION_UNGROUP_ARMY,
    KEY_FUNCTION_MOVE_ORDER,
    KEY_FUNCTION_END_UNIT_TURN,
    KEY_FUNCTION_EXECUTE_ORDERS,
    KEY_FUNCTION_PROCESS_UNIT_ORDERS,

    // Screen controls
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
    KEY_FUNCTION_TIMELINE,
    KEY_FUNCTION_RANK,
    KEY_FUNCTION_CIV_TAB,
    KEY_FUNCTION_CITY_TAB,
    KEY_FUNCTION_UNIT_TAB,
    KEY_FUNCTION_MSG_TAB,
    KEY_FUNCTION_TILE_TAB,
    KEY_FUNCTION_CHAT_KEY,
    KEY_FUNCTION_NETWORK_PLAYERS_SCREEN,
    KEY_FUNCTION_OPEN_SCENARIO_EDITOR,
    KEY_FUNCTION_EXECUTE_EYEPOINT,
    KEY_FUNCTION_YES,
    KEY_FUNCTION_NO,

    // Game options
    KEY_FUNCTION_REMAP_KEYBOARD,
    KEY_FUNCTION_OPEN_OPTIONS_SCREEN,
    KEY_FUNCTION_SOUND_OPTIONS,
    KEY_FUNCTION_MUSIC_OPTIONS,
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

    // Map controls
    KEY_FUNCTION_TOGGLE_CITY_NAMES,
    KEY_FUNCTION_TOGGLE_TRADE_ROUTES,
    KEY_FUNCTION_CENTER_MAP,
    KEY_FUNCTION_CLOSE,
    KEY_FUNCTION_TOGGLE_RADAR,
    KEY_FUNCTION_TOGGLE_CONTROL_PANEL,
    KEY_FUNCTION_TOGGLE_ALL,
    KEY_FUNCTION_ZOOM_IN1,
    KEY_FUNCTION_ZOOM_OUT1,

    // Playtest
    KEY_FUNCTION_RAND_TEST,
    KEY_FUNCTION_GAMESTATE_DEBUG,
    KEY_FUNCTION_ENTER_COMMAND,
    KEY_FUNCTION_ENTER_COMMAND_ALTERNATE,

    // Miscellaneous (CTP1, not shown at option|keyboard)
    // For these there are no strings available in km_screen.ldl.
    // Everything without strings in km_screen.ldl comes below.
    KEY_FUNCTION_TOGGLE_SPACE,
    KEY_FUNCTION_ZOOM_OUT2,
    KEY_FUNCTION_ZOOM_IN2,
    KEY_FUNCTION_HELP_MODE_TOGGLE,
    KEY_FUNCTION_MOVE_UP_OR_DOWN,
    KEY_FUNCTION_OPEN_CITY_VIEW,
    KEY_FUNCTION_CONTROL_BUILD,
    KEY_FUNCTION_CONTROL_NEXT,
    KEY_FUNCTION_CONTROL_PREV,

    // Placeholder for counting, keep this one at the end
    KEY_FUNCTION_LAST_NOOP
};

struct FUNC_TABLE {
    unsigned long keycode;
    KEY_FUNCTION function;
    char str[80];
};

struct KEYPRESS_MAP { 
    unsigned long keycode;
    KEY_FUNCTION function;
};

#define MAX_KEYPRESS_MAP 512
#define MASK_MAX_KEYPRESS_MAP 0x01ff

class KEYMAP
{
    KEYPRESS_MAP key2func[MAX_KEYPRESS_MAP];

    uint32 hash_32to16(unsigned long val);

public:
    KEYMAP(BOOL useDefault);
    KEY_FUNCTION get_function(unsigned long wParam); 
    uint32 get_keycode( KEY_FUNCTION key );
    sint32 remap_key( KEY_FUNCTION key, uint32 wParam, uint32 oldKeycode );
    void save();
};

#endif
