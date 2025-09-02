//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : General declarations
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
// _DEBUG
// - Generate debug version
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragmas commented out
// - includes fixed for case sensitive filesystems
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef GAMESOUNDS_H__
#define GAMESOUNDS_H__

struct GameSoundData
{
	sint32		id;
	MBCHAR *    name;
};

enum GAMESOUNDS
{
	GAMESOUNDS_SPACE_LAUNCH,

	GAMESOUNDS_BUILDING_STARTED,
	GAMESOUNDS_BUILDING_COMPLETE,

	GAMESOUNDS_GOODY_BOGUS,
	GAMESOUNDS_GOODY_CITY,
	GAMESOUNDS_GOODY_GOLD,
	GAMESOUNDS_GOODY_ADVANCE,
	GAMESOUNDS_GOODY_UNIT,

	GAMESOUNDS_SETTLE_CITY,

	GAMESOUNDS_NET_YOUR_TURN,

	GAMESOUNDS_EXPLOSION,

	GAMESOUNDS_DEFAULT_FAIL, // short knock on wood sound
	GAMESOUNDS_DEFAULT_CLICK, // short clicking noise
	GAMESOUNDS_BUTTON_PRESS,

	GAMESOUNDS_CHEER_CASTLE,
	GAMESOUNDS_CHEER_ENTERTAINMENT,
	GAMESOUNDS_CHEER_FOUNTAIN,
	GAMESOUNDS_CHEER_STATUE,
	GAMESOUNDS_CHEER_RELIGIOUS,

	GAMESOUNDS_BUTTONCLICK,
	GAMESOUNDS_MINWINDOW,
	GAMESOUNDS_MAXWINDOW,
	GAMESOUNDS_SLIDERUP,
	GAMESOUNDS_SLIDERDOWN,
	GAMESOUNDS_BOUNCEMSG, // wine glas cling
	GAMESOUNDS_ALERT, // syren like sound
	GAMESOUNDS_GENERALFAIL, // drawer shut sound, similar but different to GAMESOUNDS_DISABLED
	GAMESOUNDS_GENERALSUCCEED,

	GAMESOUNDS_CLOAK,
	GAMESOUNDS_UNCLOAK,

	GAMESOUNDS_RAILLAUNCH,

	GAMESOUNDS_CITYCONQUERED,
	GAMESOUNDS_TOOEXPENSIVE, // SOUND_ID_MONEY, cashier sound 

	GAMESOUNDS_DISBANDED,
	GAMESOUNDS_CHAT_MESSAGE,

	GAMESOUNDS_ADVANCE,
	GAMESOUNDS_BOO, // booing hord (barbars under goody-hood)
	GAMESOUNDS_CHANGE_GOV,
	GAMESOUNDS_DIP_AGREE,
	GAMESOUNDS_DIP_ALERT,
	GAMESOUNDS_DIP_PEACE,
	GAMESOUNDS_DIP_WAR,
	GAMESOUNDS_DISABLED, // drawer shut sound
	GAMESOUNDS_DRAGDROP_FAIL,
	GAMESOUNDS_DRAGDROP_PASS,
	GAMESOUNDS_EDIT_TEXT,
	GAMESOUNDS_ENDTURN,
	GAMESOUNDS_ILLEGAL_MOVE, // short "all piano key" hit
	GAMESOUNDS_ILLEGAL_SPECIAL, // longer disonance piano key combo
	GAMESOUNDS_LAUNCH,
	GAMESOUNDS_LOADING,
	GAMESOUNDS_LOSE_PLAYER_BATTLE,
	GAMESOUNDS_LOSE_PLAYER_CITY,
	GAMESOUNDS_MP_ALERT_TURN,
	GAMESOUNDS_MP_HURRY1,
	GAMESOUNDS_MP_HURRY2,
	GAMESOUNDS_POLLUTION_ALERT,
	GAMESOUNDS_POPUP,
	GAMESOUNDS_SCIFI_SCREEN,
	GAMESOUNDS_SUCCESS_FANFARE,
	GAMESOUNDS_SWITCH_OFF,
	GAMESOUNDS_SWITCH_ON,
	GAMESOUNDS_SYS_BUSY,
	GAMESOUNDS_TUTORIAL,
	GAMESOUNDS_VICTORY_FANFARE,
	GAMESOUNDS_VOLUME_DOWN,
	GAMESOUNDS_VOLUME_UP,
	GAMESOUNDS_POTTY, // spoken msg (for pause in network game?)
	GAMESOUNDS_BACK_TO_WORK, // spoken msg
	GAMESOUNDS_HACKNEYED, // spoken msg
	GAMESOUNDS_ARCHER_A,
	GAMESOUNDS_ARCHER_B,
	GAMESOUNDS_ARCHER_C,
	GAMESOUNDS_ARCHER_D,


	GAMESOUNDS_MAX
};

sint32 gamesounds_GetGameSoundID(sint32 gamesound);

void gamesounds_WindowOpened(void);
void gamesounds_WindowClosed(void);

#endif
