











#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __BATTLEORDERBOX_H__
#define __BATTLEORDERBOX_H__

class BattleOrderBox;

#include "controlsheet.h"
#include "pixelutils.h"
#include "colorset.h"
#include "Unit.h"
#include "unittabbutton.h"


#define k_MAX_BOB_UNITS		9


class Army;
class CellUnitList;
class c3_Static;
class c3_Button;
class Thermometer;
class c3_ColorIconButton;
class c3_ColoredStatic;

class BattleOrderBox : public ControlSheet
{
public:
	BattleOrderBox(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	BattleOrderBox(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

	AUI_ERRCODE InitCommon( MBCHAR *ldlBlock);

	virtual ~BattleOrderBox();

	virtual AUI_ERRCODE	Show( void );
	virtual AUI_ERRCODE DrawThis(aui_Surface *surface = NULL,
								sint32 x = 0,
								sint32 y = 0);

	void	SetSingleUnit(Unit theUnit);
	void	SetStackMode(BOOL stackDisplay);
	void	SetStack(Army &selectedArmy, CellUnitList *fullArmy, Unit singleUnit = Unit());

	sint32	ToggleStackDisplay( void );

	BOOL	GetStackDisplay( void ) { return m_stackDisplay; }

private:
	RECT			m_unitRect[k_MAX_BOB_UNITS];
	UnitTabButton	*m_unitControls[k_MAX_BOB_UNITS];
	COLOR			m_unitRectColors[k_MAX_BOB_UNITS];

	Thermometer		*m_unitHealthBar;

	BOOL			m_stackDisplay;

	c3_ColorIconButton	*m_unitImage;
	c3_Static		*m_unitFortify;			
	c3_Static		*m_unitVeteran;			

	c3_Static		*m_unitARD;
	c3_Static		*m_unitMovement;

	c3_Static		*m_unitARDText;
	c3_Static		*m_unitMText;

	c3_Static		*m_activeDefenseIcon;
	c3_Static		*m_veteranIcon;

	c3_Static		*m_fuelLabel;
	c3_Static		*m_fuelBox;

	c3_Static		*m_unitName;

	c3_ColoredStatic *m_cargo[ k_CARGO_CAPACITY ];
};

#endif 
