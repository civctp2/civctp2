

#ifndef __BATTLEVIEWWINDOW_H__
#define __BATTLEVIEWWINDOW_H__

#include "c3window.h"
#include "aui_action.h"






class Battle;
class BattleView;
class ctp2_Static;
class ctp2_Button;
class c3_Icon;
class BattleViewActor;
class Sequence;

class BattleViewWindow : public C3Window {
public:
	static void Initialize(Sequence *seq);
	static void Cleanup(void);

	BattleViewWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );

	virtual ~BattleViewWindow();

	
	void SetupBattle(Battle *battle);	
	void UpdateBattle(Battle *battle);	
	void EndBattle();					

	void RemoveActor(BattleViewActor *actor);

	void Refresh(void);

	void GetAttackerPos(sint32 column, sint32 row, sint32 *x, sint32 *y);
	void GetDefenderPos(sint32 column, sint32 row, sint32 *x, sint32 *y);

	void SetSequence(Sequence *seq) { m_sequence = seq; }
	Sequence *GetSequence(void) { return m_sequence; }

	
	
	const BattleView *GetBattleView() const { return(m_battleView); }

protected:
	AUI_ERRCODE InitCommonLdl(MBCHAR *ldlBlock);

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
	virtual AUI_ERRCODE Idle(void);

private:
	
	BattleView				*m_battleView;
	
	RECT					m_battleViewRect;

	

	ctp2_Static				*m_topBorder,
							*m_leftBorder,
							*m_rightBorder,
							*m_bottomBorder;

	ctp2_Button				*m_exitButton;
	ctp2_Button				*m_retreatButton;
	ctp2_Static				*m_titleText;

	ctp2_Static				*m_attackersText;
	ctp2_Static				*m_attackersName;
	c3_Icon					*m_attackersFlag;

	ctp2_Static				*m_defendersText;
	ctp2_Static				*m_defendersName;
	c3_Icon					*m_defendersFlag;

	ctp2_Static				*m_terrainBonusText;
	ctp2_Static				*m_terrainBonusValue;
	
	ctp2_Static				*m_cityBonusText;
	ctp2_Static				*m_cityBonusValue;

	ctp2_Static				*m_citylandattackBonusText;
	ctp2_Static				*m_citylandattackBonusValue;
	ctp2_Static				*m_cityairattackBonusText;
	ctp2_Static				*m_cityairattackBonusValue;
	ctp2_Static				*m_cityseaattackBonusText;
	ctp2_Static				*m_cityseaattackBonusValue;
	
	ctp2_Static				*m_cityName;

	ctp2_Static				*m_fortBonusText;
	ctp2_Static				*m_fortBonusValue;
	ctp2_Static				*m_fortBonusImage;

	ctp2_Static				*m_fortifiedBonusText;
	ctp2_Static				*m_fortifiedBonusValue;

	Sequence				*m_sequence;

};



class RemoveBattleViewAction : public aui_Action
{
public:
	RemoveBattleViewAction(bool kill) 
    :   aui_Action      (),
        m_killBattle    (kill)
    { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

private:
	bool m_killBattle;
};

extern BattleViewWindow *g_battleViewWindow;

#endif 
