#ifndef __AUI_BATTLEORDERBOXACTIONS_H__
#define __AUI_BATTLEORDERBOXACTIONS_H__

#include "ui/aui_common/aui_action.h"
#include "gs/gameobj/Army.h"
#include "gs/gameobj/Unit.h"

class BattleOrderBox;
class aui_Control;


class BobButtonAction : public aui_Action
{
public:
	BobButtonAction(BattleOrderBox * bob);

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

	void SetUnit(Unit const & unit) { m_unit = unit; };
	void SetArmy(Army const & army) { m_army = army; };

private:
	BattleOrderBox *    m_bob;
	Unit				m_unit;
	Army				m_army;
};

#endif
