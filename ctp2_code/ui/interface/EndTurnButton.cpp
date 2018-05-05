#include "ctp/c3.h"

#include "ui/interface/EndTurnButton.h"

#include "ui/aui_common/aui_ldl.h"
#include "ui/aui_ctp2/ctp2_button.h"
#include "gs/utility/newturncount.h"
#include "ui/aui_ctp2/SelItem.h"

#include "ui/interface/AttractWindow.h"
#include "gfx/spritesys/director.h"

EndTurnButton::EndTurnButton(MBCHAR *ldlBlock) :
m_endTurn(static_cast<ctp2_Button*>(aui_Ldl::GetObject(ldlBlock, "TurnButton")))
{

	Assert(m_endTurn);

	m_endTurn->SetActionFuncAndCookie(EndTurnButtonActionCallback, this);

	m_endTurn->Enable(false);
}


void EndTurnButton::UpdatePlayer(PLAYER_INDEX player)
{

	if(g_selected_item->GetVisiblePlayer() == player)
		m_endTurn->Enable(true);
	else
		m_endTurn->Enable(false);

	g_attractWindow->RemoveRegion(m_endTurn);
}

void EndTurnButton::EndTurnButtonActionCallback(aui_Control *control, uint32 action,
												uint32 data, void *cookie)
{

	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;







	DPRINTF(k_DBG_GAMESTATE, ("Button end turn, %d\n", g_selected_item->GetCurPlayer()));
	if((g_selected_item->GetCurPlayer() != g_selected_item->GetVisiblePlayer())) {
		DPRINTF(k_DBG_GAMESTATE, ("But not my turn!\n"));
		return;
	}

	g_selected_item->RegisterManualEndTurn();
	g_director->AddEndTurn();
}
