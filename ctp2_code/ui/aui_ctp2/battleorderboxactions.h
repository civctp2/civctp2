









#ifndef __AUI_BATTLEORDERBOXACTIONS_H__
#define __AUI_BATTLEORDERBOXACTIONS_H__


#include "aui_action.h"
#include "Army.h"

class BattleOrderBox;
class aui_Control;



class BobButtonAction : public aui_Action
{
public:
	
	BobButtonAction(BattleOrderBox *bob);

	virtual ActionCallback Execute;

	void SetUnit(const Unit &unit) { m_unit = unit; }
	void SetArmy(const Army &army ) { m_army = army; }

private:
	BattleOrderBox		*m_bob;
	Unit				m_unit;
	Army				m_army;
};

#endif 
