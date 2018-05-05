#include "ctp/c3.h"

#include "ui/aui_common/aui.h"

#include "ui/aui_common/aui_button.h"
#include "ui/aui_ctp2/c3_coloriconbutton.h"

#include "robot/aibackdoor/dynarr.h"
#include "gs/gameobj/Unit.h"
#include "gs/gameobj/UnitData.h"
#include "gs/gameobj/UnitPool.h"
#include "gs/gameobj/Army.h"
#include "gs/world/cellunitlist.h"
#include "gs/world/MapPoint.h"
#include "gs/gameobj/XY_Coordinates.h"
#include "gs/world/World.h"
#include "gs/world/Cell.h"
#include "gs/gameobj/Order.h"

#include "ui/interface/controlpanelwindow.h"

#include "gfx/gfx_utils/pixelutils.h"
#include "gfx/gfx_utils/colorset.h"

#include "ui/aui_ctp2/battleorderbox.h"
#include "ui/aui_ctp2/battleorderboxactions.h"
#include "gs/database/profileDB.h"

#include "ui/aui_ctp2/SelItem.h"

extern SelectedItem	*g_selected_item;
extern UnitPool		*g_theUnitPool;
extern ControlPanelWindow	*g_controlPanel;
extern World		*g_theWorld;
extern ProfileDB    *g_theProfileDB;

BobButtonAction::BobButtonAction(BattleOrderBox *bob)
{
	m_bob = bob;
}

void BobButtonAction::Execute(aui_Control *control, uint32 action, uint32 data )
{

	switch ( action ) {
	case AUI_BUTTON_ACTION_EXECUTE:


		if(!g_theProfileDB->IsAutoGroup()) {
			if (g_theUnitPool->IsValid(m_unit)) {

				Army army = m_army;

				if ( m_army.m_id == (0) ) return;

				MapPoint	pos;
				m_unit.GetPos(pos);

				if ( m_army.IsPresent(m_unit) ) {
					if(m_army.Num() == 1) {
						g_selected_item->Deselect(m_army.GetOwner());
						break;
					} else {
						m_unit.AccessData()->CreateOwnArmy();
					}
				}
				else {
					MapPoint crap;
					m_army.AddOrders( UNIT_ORDER_GROUP_UNIT, NULL, crap, (int)(m_unit) );
				}

				CellUnitList	*cellArmy;
				cellArmy = g_theWorld->GetCell(pos)->UnitArmy();


			}
		} else {

			if ( g_theUnitPool->IsValid(m_unit) ) {
				Army army = m_army;
				m_unit.AccessData()->CreateOwnArmy();
				g_selected_item->SetSelectUnit( m_unit, FALSE );

				CellUnitList	*cellArmy;
				MapPoint	pos;

				m_unit.GetPos(pos);
				cellArmy = g_theWorld->GetCell(pos)->UnitArmy();


			}
		}

		break;
	case C3_COLORICONBUTTON_ACTION_RIGHTCLK:
		if ( g_theUnitPool->IsValid(m_unit) ) {

			m_bob->ToggleStackDisplay();

			if ( !m_bob->GetStackDisplay() ) {
				m_bob->SetSingleUnit( m_unit );
			}

			CellUnitList	*cellArmy;
			MapPoint	pos;

			m_unit.GetPos(pos);
			cellArmy = g_theWorld->GetCell(pos)->UnitArmy();


		}
		break;
	case C3_COLORICONBUTTON_ACTION_DOUBLECLK:
		if ( g_theUnitPool->IsValid(m_unit) ) {
			Army army = m_army;
			Unit unit = m_unit;

			unit.AccessData()->CreateOwnArmy();
			g_selected_item->SetSelectUnit( unit, TRUE, TRUE);

			CellUnitList	*cellArmy;
			MapPoint	pos;

			unit.GetPos(pos);
			cellArmy = g_theWorld->GetCell(pos)->UnitArmy();


		}
		break;
	}
}
