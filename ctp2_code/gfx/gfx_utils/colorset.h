//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Color definitions.
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
// - Added new colors above player 15 by Martin Gühmann
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __COLORSET_H__
#define __COLORSET_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include <vector>      

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class ColorSet;

#define k_MAX_COLOR_SET     100     // File name: Colors##.txt (00 - 99)

enum COLOR {
	COLOR_BLACK,
	COLOR_WHITE,

	COLOR_RED,
	COLOR_GREEN,
	COLOR_BLUE,

	COLOR_YELLOW,
	COLOR_ORANGE,
	COLOR_PURPLE,
	COLOR_DARK_GREEN,

	COLOR_PLAYER1,
	COLOR_PLAYER2,
	COLOR_PLAYER3,
	COLOR_PLAYER4,
	COLOR_PLAYER5,
	COLOR_PLAYER6,
	COLOR_PLAYER7,
	COLOR_PLAYER8,
	COLOR_PLAYER9,
	COLOR_PLAYER10,
	COLOR_PLAYER11,
	COLOR_PLAYER12,
	COLOR_PLAYER13,
	COLOR_PLAYER14,
	COLOR_PLAYER15,
	COLOR_PLAYER16,
	COLOR_PLAYER17,

	// Added by Martin Gühmann to support more players with colors
	COLOR_PLAYER18,
	COLOR_PLAYER19,
	COLOR_PLAYER20,
	COLOR_PLAYER21,
	COLOR_PLAYER22,
	COLOR_PLAYER23,
	COLOR_PLAYER24,
	COLOR_PLAYER25,
	COLOR_PLAYER26,
	COLOR_PLAYER27,
	COLOR_PLAYER28,
	COLOR_PLAYER29,
	COLOR_PLAYER30,
	COLOR_PLAYER31,
	COLOR_PLAYER32,
	COLOR_PLAYER33,

	COLOR_TERRAIN_0,
	COLOR_TERRAIN_1,
	COLOR_TERRAIN_2,
	COLOR_TERRAIN_3,
	COLOR_TERRAIN_4,
	COLOR_TERRAIN_5,
	COLOR_TERRAIN_6,
	COLOR_TERRAIN_7,
	COLOR_TERRAIN_8,
	COLOR_TERRAIN_9,
	COLOR_TERRAIN_10,
	COLOR_TERRAIN_11,
	COLOR_TERRAIN_12,
	COLOR_TERRAIN_13,
	COLOR_TERRAIN_14,
	COLOR_TERRAIN_15,
	COLOR_TERRAIN_16,
	COLOR_TERRAIN_17,
	COLOR_TERRAIN_18,
    COLOR_TERRAIN_19,
    COLOR_TERRAIN_20,
    COLOR_TERRAIN_21,
    COLOR_TERRAIN_22,

	COLOR_SELECT_0,
	COLOR_SELECT_1,
	COLOR_SELECT_2,
	COLOR_SELECT_3,

	COLOR_BUTTON_TEXT_DROP,
	COLOR_BUTTON_TEXT_PLAIN,
	COLOR_BUTTON_TEXT_HILITE,

	COLOR_UI_BOX,
	COLOR_GRAY,

	COLOR_MAX
};

extern ColorSet *	g_colorSet;

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "pixeltypes.h" // Pixel16

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class ColorSet
{
public:
	ColorSet();
	virtual ~ColorSet(void);


	Pixel16		GetColor(COLOR color) const;
	COLORREF	GetColorRef(COLOR color) const;
	Pixel16		GetPlayerColor(sint32 playerNum) const;
	COLOR		ComputePlayerColor(sint32 playerNum) const;

	Pixel16		GetDarkColor(COLOR color) const;
	Pixel16		GetDarkPlayerColor(sint32 playerNum) const;
	COLORREF	GetDarkColorRef(COLOR color) const;
	Pixel16		GetLightColor(COLOR color) const;
	Pixel16		GetLightPlayerColor(sint32 playerNum) const;
	COLORREF	GetLightColorRef(COLOR color) const;

	static void	Initialize(uint32 fileNumber = 0);
    void        Import(uint32 fileNumber = 0);
    static void Cleanup(void);

private:

	std::vector<Pixel16>    m_colors;
};

#endif
