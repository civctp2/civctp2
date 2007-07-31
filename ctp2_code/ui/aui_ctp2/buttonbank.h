











#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __BUTTONBANK_H__
#define __BUTTONBANK_H__

#include "aui_action.h"
#include "controlsheet.h"

#define k_BUTTON_BANK_MAX_BUTTONS		16
#define k_BUTTON_BANK_NEXT_PAGE_INDEX	7

class ColorIconButton;
class Army;
class CellUnitList;
class aui_Control;
struct aui_MouseEvent;
class MapPoint;

enum ORDERMODE {
	ORDERMODE_NONE = -1,

	ORDERMODE_READ,
	ORDERMODE_GROUP,
	ORDERMODE_UNGROUP,
	ORDERMODE_MOVE,
	ORDERMODE_GOTO,
	ORDERMODE_SLEEP,
	ORDERMODE_ASSAULT,
	ORDERMODE_BOMBARD,
	ORDERMODE_FORTIFY,
	ORDERMODE_PILLAGE,
	ORDERMODE_PATROL,
	ORDERMODE_REFORMCITY,
	ORDERMODE_PARADROP,
	ORDERMODE_LAUNCH,
	ORDERMODE_DESCEND,
	ORDERMODE_SETTLE,
	ORDERMODE_CAPTURESLAVES,
	ORDERMODE_ESTABLISHEMBASSY,
	ORDERMODE_HEARGOSSIP,
	ORDERMODE_THROWPARTY,
	ORDERMODE_CONVERTCITY,
	ORDERMODE_SELLINDULGENCES,
	ORDERMODE_SOOTHSAY,
	ORDERMODE_FILEINJUNCTION,
	ORDERMODE_SUE,
	ORDERMODE_FREESLAVE,
	ORDERMODE_AIDUPRISING,
	ORDERMODE_BOMBCABINET,		
	ORDERMODE_PLANTNUKE,
	ORDERMODE_SPY,
	ORDERMODE_STEALDISCOVERY,
	ORDERMODE_DEFUSE,
	ORDERMODE_BRANCH,
	ORDERMODE_ADVERTISE,
	ORDERMODE_CONDUCTHIT,
	ORDERMODE_PLANTNANOVIRUS,
	ORDERMODE_INFECTCITY,
	ORDERMODE_CLOAK,
	ORDERMODE_RUSTLE,
	ORDERMODE_CREATEPARK,
	ORDERMODE_PIRACY,
	ORDERMODE_EXPEL,
	ORDERMODE_CREATERIFT,
	ORDERMODE_INCITEREVOLUTION,
	ORDERMODE_CARGO,
	ORDERMODE_DISBAND,

	ORDERMODE_NEXTPAGE,

	ORDERMODE_MAX
};

class ButtonBank : public ControlSheet
{
public:

	
	ButtonBank(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	ButtonBank(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~ButtonBank();

	AUI_ERRCODE InitCommon(MBCHAR *bankBlock);

	virtual AUI_ERRCODE DrawThis(aui_Surface *surface = NULL,
								sint32 x = 0,
								sint32 y = 0);

	void ClearButtons(void);
	void AddButton(ORDERMODE mode);
	void SetPage(sint32 page);
	sint32 GetPage(void) const { return m_bankPage; }

	void FillBank(Army &selected, CellUnitList *all);

	ORDERMODE ButtonToMode(ColorIconButton *button);


private:
	ColorIconButton		*m_bankButtons[k_BUTTON_BANK_MAX_BUTTONS];
	RECT				m_bankButtonRects[k_BUTTON_BANK_MAX_BUTTONS];

	sint32				m_buttonsInBank;

	sint32				m_bankWidth;
	sint32				m_bankHeight;
	sint32				m_buttonBorder;

	sint32				m_bankPage;

	RECT				m_buttonBounds;
	
	ColorIconButton		*m_buttons[ORDERMODE_MAX];
};

AUI_ACTION_BASIC(BaseBankAction);

#endif 
