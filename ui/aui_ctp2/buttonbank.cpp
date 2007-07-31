

#include "c3.h"

#include "aui.h"
#include "aui_Window.h"
#include "aui_ldl.h"
#include "aui_UniqueId.h"
#include "aui_Surface.h"
#include "TipWindow.h"
#include "aui_stringtable.h"

#include "Picture.h"

#include "MapPoint.h"
#include "Unit.h"
#include "UnitRec.h"
#include "Army.h"
#include "CellUnitList.h"
#include "UnitPool.h"
#include "MapPoint.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Cell.h"

#include "ColorIconButton.h"

#include "ControlSheet.h"
#include "buttonbank.h"

#include "player.h"
#include "SelItem.h"
#include "AdvanceRecord.h"

#include "ControlPanelWindow.h"

#include "primitives.h"
#include "advanceutil.h"

extern UnitPool					*g_theUnitPool;
extern ControlPanelWindow	*g_controlPanel;
extern World					*g_theWorld;


extern Player				**g_player;
extern SelectedItem			*g_selected_item;

ButtonBank::ButtonBank(AUI_ERRCODE *retval, 
					   uint32 id, 
					   MBCHAR *ldlBlock, 
					   ControlActionCallback *ActionFunc, 
					   void *cookie)
	: 
	ControlSheet(retval, id, ldlBlock, ActionFunc, cookie),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_ImageBase( ldlBlock )
{
	InitCommon(ldlBlock);
}

ButtonBank::ButtonBank(AUI_ERRCODE *retval, 
					   uint32 id, 
					   sint32 x, 
					   sint32 y, 
					   sint32 width, 
					   sint32 height,
					   MBCHAR *pattern,
					   ControlActionCallback *ActionFunc, 
					   void *cookie)
	:
	ControlSheet(retval, id, x, y, width, height, pattern, ActionFunc, cookie),
	aui_TextBase( NULL ),
	aui_ImageBase( (sint32)0 )
{
	InitCommon(NULL);
}

#define DISPOSE_BUTTON_BANK_BUTTON(memberVar)	\
	action = memberVar->GetAction();			\
	delete action;								\
	delete memberVar;

ButtonBank::~ButtonBank()
{
	aui_Action		*action;
	sint32			i;

	for (i=0; i<ORDERMODE_MAX; i++) {
		DISPOSE_BUTTON_BANK_BUTTON(m_buttons[i]);
	
	}
}









#define MAKE_BUTTON_BANK_BUTTON(nameStr, mode)										\
	sprintf( buttonBlock, "ButtonBank.%s", nameStr );								\
	m_buttons[mode] = new ColorIconButton(&errcode, aui_UniqueId(), buttonBlock );	\
	Assert( AUI_NEWOK(m_buttons[mode], errcode) );									\
	if ( !AUI_NEWOK(m_buttons[mode], errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;	\
	action = (aui_Action *)new BaseBankAction();								\
	Assert( AUI_NEWOK(action, errcode) );											\
	if ( !AUI_NEWOK(action, errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;			\
	m_buttons[mode]->SetAction(action);													
		
AUI_ERRCODE ButtonBank::InitCommon(MBCHAR *bankBlock)
{
	MBCHAR				buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR				tableBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	aui_Action			*action;
	AUI_ERRCODE			errcode;
	sint32				i,j;

	m_buttonsInBank = 0;

	for (i=0; i<k_BUTTON_BANK_MAX_BUTTONS; i++) {
		m_bankButtons[i] = NULL;
	}

	aui_StringTable		*bankStringTable;
	strcpy(tableBlock, "ButtonBankStrings");
	bankStringTable = new aui_StringTable(&errcode, tableBlock);
	Assert( AUI_NEWOK(bankStringTable, errcode) );
	if ( !AUI_NEWOK(bankStringTable, errcode) ) return AUI_ERRCODE(-1);

	if (bankStringTable->GetNumStrings() != ORDERMODE_MAX) {
		c3errors_ErrorDialog("Button Bank", "Bad number of button ID strings in LDL.");
		return AUI_ERRCODE(-1);
	}

	
	for (i=0; i<bankStringTable->GetNumStrings(); i++) {
		MAKE_BUTTON_BANK_BUTTON(bankStringTable->GetString(i), (ORDERMODE)i);
	}

	
	delete bankStringTable;

	
	m_bankWidth = 4;
	m_bankHeight = 2;

	m_buttonBorder = 3;

	RECT		cellRect = { 0, 0, (Width()-m_buttonBorder*2)/m_bankWidth, (Height()-m_buttonBorder*2)/m_bankHeight };
	
	m_buttonBounds = cellRect;

	OffsetRect(&cellRect, m_buttonBorder, m_buttonBorder);

	for (i=0; i<m_bankWidth; i++) {
		for (j=0; j<m_bankHeight; j++) {
			RECT		tempRect;

			tempRect = cellRect;
			OffsetRect(&tempRect, m_buttonBounds.right *i, m_buttonBounds.bottom * j);

			m_bankButtonRects[i*m_bankHeight+j] = tempRect;
			m_bankButtonRects[8+(i*m_bankHeight+j)] = tempRect;
		}
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE ButtonBank::DrawThis(aui_Surface *surface, sint32 x, sint32 y)
{
	ControlSheet::DrawThis(surface, x, y);

	return AUI_ERRCODE_OK;
}

void ButtonBank::ClearButtons(void)
{
	sint32		i;

	for (i=0; i<m_buttonsInBank; i++) {
		m_bankButtons[i]->Hide();
		RemoveSubControl(m_bankButtons[i]->Id());
		m_bankButtons[i] = NULL;
	}

	m_buttonsInBank = 0;
}

void ButtonBank::AddButton(ORDERMODE mode)
{
	AUI_ERRCODE			errcode;
	RECT				theRect;
	sint32				i;
	ColorIconButton		*button;

	if (mode >= ORDERMODE_MAX) return;

	button = m_buttons[mode];

	Assert(button != NULL);
	if (button == NULL) return;

	aui_Image			 *image = button->GetImage();
	
	Assert(image);
	if (image == NULL) return;

	Assert(image->TheSurface());
	if (image->TheSurface() == NULL) return;

	
	for (i=0; i<m_buttonsInBank; i++) {
		if (m_bankButtons[i] == button) return;
	}

	Assert(m_buttonsInBank < k_BUTTON_BANK_MAX_BUTTONS);
	if (m_buttonsInBank >= k_BUTTON_BANK_MAX_BUTTONS) return;


	if (m_buttonsInBank == k_BUTTON_BANK_NEXT_PAGE_INDEX) {
		ColorIconButton		*pageButton;

		theRect = m_bankButtonRects[m_buttonsInBank];
		
		pageButton = m_buttons[ORDERMODE_NEXTPAGE];

		sint32 width = image->TheSurface()->Width();
		sint32 height = image->TheSurface()->Height();
		if (m_buttonBounds.right > width+4) {
			InflateRect(&theRect, -((m_buttonBounds.right - width-4)/2), 0);
		}
		if (m_buttonBounds.bottom > height+4) {
			InflateRect(&theRect, 0, -((m_buttonBounds.bottom - height-4)/2));
		}

		errcode = pageButton->Move(theRect.left, theRect.top);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;

		errcode = pageButton->Resize((theRect.right-theRect.left),(theRect.bottom-theRect.top));
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;

		m_bankButtons[m_buttonsInBank] = pageButton;
		
		


		m_buttonsInBank++;

		
		theRect = m_bankButtonRects[m_buttonsInBank];
		
		pageButton = m_buttons[ORDERMODE_READ];

		width = image->TheSurface()->Width();
		height = image->TheSurface()->Height();
		if (m_buttonBounds.right > width+4) {
			InflateRect(&theRect, -((m_buttonBounds.right - width-4)/2), 0);
		}
		if (m_buttonBounds.bottom > height+4) {
			InflateRect(&theRect, 0, -((m_buttonBounds.bottom - height-4)/2));
		}

		errcode = pageButton->Move(theRect.left, theRect.top);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;

		errcode = pageButton->Resize((theRect.right-theRect.left),(theRect.bottom-theRect.top));
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;

		m_bankButtons[m_buttonsInBank] = pageButton;

		


		m_buttonsInBank++;
	}

	theRect = m_bankButtonRects[m_buttonsInBank];
	
	sint32 width = image->TheSurface()->Width();
	sint32 height = image->TheSurface()->Height();
	if (m_buttonBounds.right > width+4) {
		InflateRect(&theRect, -((m_buttonBounds.right - width-4)/2), 0);
	}
	if (m_buttonBounds.bottom > height+4) {
		InflateRect(&theRect, 0, -((m_buttonBounds.bottom - height-4)/2));
	}

	errcode = button->Move(theRect.left, theRect.top);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return;

	errcode = button->Resize((theRect.right-theRect.left),(theRect.bottom-theRect.top));
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return;

	m_bankButtons[m_buttonsInBank] = button;
	


	m_buttonsInBank++;
}

void ButtonBank::SetPage(sint32 page)
{
	sint32 i;

	if (page == 0) {
		for (i=0; i<m_buttonsInBank && i <= k_BUTTON_BANK_NEXT_PAGE_INDEX; i++) {
			m_bankButtons[i]->Show();
			AddSubControl(m_bankButtons[i]);

		}
		if (m_buttonsInBank > (k_BUTTON_BANK_NEXT_PAGE_INDEX+1)) {
			for (i=k_BUTTON_BANK_NEXT_PAGE_INDEX+1; i<m_buttonsInBank; i++) {
				m_bankButtons[i]->Hide();
				RemoveSubControl(m_bankButtons[i]->Id());
			}
		}
	} else 
	if (page == 1) {
		if (m_buttonsInBank <= k_BUTTON_BANK_NEXT_PAGE_INDEX) return;

		for (i=0; i<m_buttonsInBank && i <= k_BUTTON_BANK_NEXT_PAGE_INDEX; i++) {
			m_bankButtons[i]->Hide();
			RemoveSubControl(m_bankButtons[i]->Id());
		}
		if (m_buttonsInBank > (k_BUTTON_BANK_NEXT_PAGE_INDEX+1)) {
			for (i=k_BUTTON_BANK_NEXT_PAGE_INDEX+1; i<m_buttonsInBank; i++) {
				m_bankButtons[i]->Show();
				AddSubControl(m_bankButtons[i]);
			}
		}
	}

	m_bankPage = page;
}

void ButtonBank::FillBank(Army &selected, CellUnitList *all)
{
	sint32				i, count, countAll;
	Unit				unit;
	const UnitRecord	*unitRec;
	BOOL				condition = FALSE;
	MapPoint			pos;
	double				success, death;
	sint32				timer, amount;
	double				chance, deathChance, eliteChance;

	ClearButtons();

	if (selected.m_id == (0)) return;
	if (all == NULL) return;

	count = selected.Num();
	countAll = all->Num();

	
	for (i=0; i<count; i++) {
		unit = selected.Access(i);
		unit.GetPos(pos);	
		unitRec = unit.GetDBRec();

		if (!g_theUnitPool->IsValid(unit)) {
			Assert(FALSE);
			continue;
		}

	
		
		


		
		







		
		if (TRUE) AddButton(ORDERMODE_SLEEP);

		
		
		
		
		
		
		
			



		
		
		if (selected.CanEntrench()) AddButton(ORDERMODE_FORTIFY);
		
		
		if (selected.CanPillage()) AddButton(ORDERMODE_PILLAGE);




		
		if (selected.CanReformCity()) AddButton(ORDERMODE_REFORMCITY);

		
		if ( selected.CanBombard() ) AddButton(ORDERMODE_BOMBARD);

		
		if (selected.CanSettle()) AddButton(ORDERMODE_SETTLE);

		
		if (selected.CanEstablishEmbassy()) AddButton(ORDERMODE_ESTABLISHEMBASSY);
		
		
		
		
		
		
		if (selected.CanSellIndulgences()) AddButton(ORDERMODE_SELLINDULGENCES);

		
		if (selected.CanSoothsay()) AddButton(ORDERMODE_SOOTHSAY);
		
		
		if (selected.CanSue()) AddButton(ORDERMODE_SUE);




		double chance, randChance, escapeChance, success, death;

		if (selected.CanUndergroundRailway(success, death)) AddButton(ORDERMODE_FREESLAVE);

		if (selected.CanSlaveUprising()) AddButton(ORDERMODE_AIDUPRISING);
		
		if (selected.CanInvestigateCity(chance, eliteChance)) AddButton(ORDERMODE_SPY);

		if (selected.CanStealTechnology(randChance, chance)) AddButton(ORDERMODE_STEALDISCOVERY);
	
		if (selected.CanAssasinateRuler(chance, eliteChance)) AddButton(ORDERMODE_BOMBCABINET);

		if (selected.CanPlantNuke(chance, escapeChance)) {
			if ( g_player[g_selected_item->GetVisiblePlayer()]->m_advances->HasAdvance(advanceutil_GetNukeAdvance())) {
				AddButton(ORDERMODE_PLANTNUKE);
			}
		}
		
		


		if (selected.CanCreateFranchise(chance)) AddButton(ORDERMODE_BRANCH);

		
		if (selected.CanAdvertise()) AddButton(ORDERMODE_ADVERTISE);

		if (selected.CanNanoInfect(chance)) AddButton(ORDERMODE_PLANTNANOVIRUS);

		if (selected.CanBioInfect(chance)) AddButton(ORDERMODE_INFECTCITY);

		
		if (selected.CanCloak()) AddButton(ORDERMODE_CLOAK);

		
		if (selected.CanCreatePark()) AddButton(ORDERMODE_CREATEPARK);

		
		if (selected.CanExpel()) AddButton(ORDERMODE_EXPEL);

		
		uint32 uindex;
		if (selected.CanInterceptTrade(uindex)) {
			AddButton(ORDERMODE_PIRACY);
		}
		if (selected.CanInciteRevolution(chance, eliteChance)) {
			AddButton(ORDERMODE_INCITEREVOLUTION);
		}

		
		if ( count == 1 && unitRec->GetCanCarry() ) {	
			AddButton( ORDERMODE_CARGO );
		}

		BOOL canRailLaunch = FALSE;
		
		
		Unit city = g_theWorld->GetCell(unit.RetPos())->GetCity();
		if (city.m_id != (0)) {
			
			
			
			
		}
		
	}

	
	if (selected.CanSlaveRaid(success, death, timer, amount)) {
		AddButton(ORDERMODE_CAPTURESLAVES);
	}

	
	if (selected.CanConvertCity(chance, deathChance)) {
		AddButton(ORDERMODE_CONVERTCITY);
	}

	
	if (selected.CanInjoin()) {
		AddButton(ORDERMODE_FILEINJUNCTION);
	}

	
	if (TRUE) AddButton(ORDERMODE_DISBAND);

}

ORDERMODE ButtonBank::ButtonToMode(ColorIconButton *button)
{
	sint32			i;

	for (i=0; i<ORDERMODE_MAX; i++) {
		if (button == m_buttons[i]) {
			return ORDERMODE(i);
		}
	}

	return ORDERMODE_NONE;
}


void BaseBankAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;


}


