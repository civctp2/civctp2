//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Turn display
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
// - None
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef ___BMH_TURN_YEAR_STATUS_HEADER
#define ___BMH_TURN_YEAR_STATUS_HEADER


#include <string>


#include "Globals.h"


class aui_Control;
class ctp2_Button;
class ctp2_Static;
class aui_Surface;


class TurnYearStatus {
public:
	
	static const MBCHAR *GetCurrentYear();

	
	static const MBCHAR *GetCurrentRound();

	
	TurnYearStatus(MBCHAR *ldlBlock);

	
	void UpdatePlayer(PLAYER_INDEX player);

	static AUI_ERRCODE DrawDougsProgress(ctp2_Static *control,
								  aui_Surface *surface,
								  RECT &rect,
								  void *cookie);
private:
	
	
	void Update();

	
	static void TurnYearStatusActionCallback(aui_Control *control, uint32 action,
		uint32 data, void *cookie);

	
	ctp2_Button *m_turnYearStatus;

	
	ctp2_Static *m_dougsProgress;

	
	enum DisplayType {
		DISPLAY_YEAR			= 0,
		DISPLAY_TURN			= 1,
		NUMBER_OF_DISPLAY_TYPES	= 2
	};

	
	DisplayType m_displayType;
};





struct sTurnLengthOverride
{
	uint32 turn;
	MBCHAR text[32];
};

extern sTurnLengthOverride *g_pTurnLengthOverride;
extern uint32 g_turnLengthOverrideSize;
extern bool g_useCustomYear;

#endif 
