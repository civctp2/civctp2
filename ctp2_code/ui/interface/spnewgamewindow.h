//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Handling of user preferences.
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Addion by Martin Gühmann: Two more world shape options, 
//   flat world and Uranus world
//
//----------------------------------------------------------------------------

#pragma once
#ifndef SPNEWGAMEWINDOW_FLAG
#define SPNEWGAMEWINDOW_FLAG

#include "c3window.h"
#include "c3_dropdown.h"
#include "aui_ldl.h"
#include "c3_button.h"
#include "c3_ListItem.h"
#include "ctp2_button.h"

class aui_Control;
class c3_Static;
class c3_ListBox;
class C3Window;
class C3Slider;
class aui_SwitchGroup;
class aui_StringTable;
class c3_CheckBox;
class c3_Switch;

class aui_TextField;
class C3TextField;
class aui_StringTable;


#include "ctp2_button.h"

enum SP_NEWGAME_STR {
	SP_NEWGAME_STR_CHIEFTAIN,
	SP_NEWGAME_STR_WARLORD,
	SP_NEWGAME_STR_PRINCE,
	SP_NEWGAME_STR_KING,
	SP_NEWGAME_STR_EMPEROR,
	SP_NEWGAME_STR_DEITY,

	SP_NEWGAME_STR_SMALL,
	SP_NEWGAME_STR_MEDIUM,
	SP_NEWGAME_STR_LARGE,
	SP_NEWGAME_STR_VERYLARGE,

	SP_NEWGAME_STR_RANDOMMAP,
	SP_NEWGAME_STR_CUSTOMMAP,

	SP_NEWGAME_STR_EARTH,
	SP_NEWGAME_STR_DOUGHNUT,
	//Added by Martin Gühmann
	SP_NEWGAME_STR_URANUS,
	SP_NEWGAME_STR_FLAT,

	SP_NEWGAME_STR_DEFAULT,
	SP_NEWGAME_STR_CUSTOM,

	SP_NEWGAME_STR_MAX
};




sint32 spnewgamescreen_displayMyWindow();
sint32 spnewgamescreen_removeMyWindow(uint32 action);
sint32 spnewgamescreen_setPlayerName( const MBCHAR *name );
AUI_ERRCODE spnewgamescreen_Initialize( void );
AUI_ERRCODE spnewgamescreen_Cleanup();
sint32 spnewgamescreen_update( void );




void spnewgamescreen_instaPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_startPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_returnPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_quitPress(aui_Control *control, uint32 action, uint32 data, void *cookie );






void spnewgamescreen_tribePress( aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_malePress( aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_femalePress( aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_difficultyPress( aui_Control *control, uint32 action, uint32 data, void *cookie );

void spnewgamescreen_mapSizePress( aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_playersPress( aui_Control *control, uint32 action, uint32 data, void *cookie );

void spnewgamescreen_mapPress( aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_rulesPress( aui_Control *control, uint32 action, uint32 data, void *cookie );





void spnewgamescreen_mapTypePress( aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_worldShapePress( aui_Control *control, uint32 action, uint32 data, void *cookie );


void spnewgamescreen_editorPress( aui_Control *control, uint32 action, uint32 data, void *cookie );


void spnewgamescreen_scenarioPress(aui_Control *control, uint32 action, uint32 data, void *cookie );





void spnewgamescreen_clanSelect(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_genderSelect(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_preferencePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_pCustomPress(aui_Control *control, uint32 action, uint32 data, void *cookie );


void spnewgamescreen_mapSizeSelect(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_worldTypeSelect(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_worldShapeSelect(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_difficultySelect(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_riskLevelSelect(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_opponentSelect(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_wCustomPress(aui_Control *control, uint32 action, uint32 data, void *cookie );


void spnewgamescreen_genocidePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_tradePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_combatPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgamescreen_pollutionPress(aui_Control *control, uint32 action, uint32 data, void *cookie );


void spnewgamescreen_scenarioExitCallback(aui_Control *control, uint32 action, uint32 data, void *cookie );



class TwoChoiceButton : public ctp2_Button
{
public:
	TwoChoiceButton(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		MBCHAR *choiceOff, MBCHAR *choiceOn, uint32 onoff = 0,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	~TwoChoiceButton() {};
	
	uint32	GetChoice() { return m_choice; };
	uint32 Switch();
private:
	MBCHAR m_choices[2][ k_AUI_LDL_MAXBLOCK + 1 ];
	uint32	m_choice;
};



class SPProfileBox 
{
public:
	SPProfileBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock );
	virtual ~SPProfileBox();
	void	SetLeader(uint32 index);
private:
	
	c3_DropDown		*m_spClan;
	c3_DropDown		*m_spGender;
	C3TextField		*m_spName;
	ctp2_Button		*m_spPreferences;
	ctp2_Button		*m_spCustom;
	
	c3_Static		*m_PTOP;
	c3_Static		*m_PHEADER;
	c3_Static		*m_PBOT;
	c3_Static		*m_PLEFT;
	c3_Static		*m_PRIGHT;
};



class SPWorldBox 
{
public:
	SPWorldBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock );
	virtual ~SPWorldBox();
	AUI_ERRCODE DrawThis(
		aui_Surface *surface,
		sint32 x,
		sint32 y );
	uint32 GetMapSize() { return m_mapSize->GetListBox()->GetSelectedItemIndex(); };
	uint32 GetWorldType() { return m_mapSize->GetListBox()->GetSelectedItemIndex(); };
	uint32 GetWorldShape() { return m_mapSize->GetListBox()->GetSelectedItemIndex(); };
	uint32 GetDifficulty() { return m_mapSize->GetListBox()->GetSelectedItemIndex(); };
	uint32 GetRiskLevel() { return m_mapSize->GetListBox()->GetSelectedItemIndex(); };
	uint32 GetNumOpponents() { return m_mapSize->GetListBox()->GetSelectedItemIndex()+3; };

private:
	c3_DropDown		*m_mapSize,
					*m_worldType,
					*m_worldShape,
					*m_difficulty,
					*m_riskLevel,
					*m_opponent;
	ctp2_Button		*m_spCustom;
	
	c3_Static		*m_WTOP;
	c3_Static		*m_WHEADER;
	c3_Static		*m_WBOT;
	c3_Static		*m_WLEFT;
	c3_Static		*m_WRIGHT;

};



class SPRulesBox 
{
public:
	SPRulesBox (
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock );
	virtual ~SPRulesBox();
	
	uint32 GetGenocideRules();
	uint32 GetTradeRules();
	uint32 GetCombatRules();
	uint32 GetPollutionRules();
private:
	c3_CheckBox			*m_spGenocide,
						*m_spTrade,
						*m_spCombat,
						*m_spPollution;
	
	c3_Static		*m_RTOP;
	c3_Static		*m_RHEADER;
	c3_Static		*m_RBOT;
	c3_Static		*m_RLEFT;
	c3_Static		*m_RRIGHT;
};



class SPNewGameWindow : public C3Window
{
public:
	SPNewGameWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD,
		bool bevel = true);
	virtual ~SPNewGameWindow();

	void Update( void );

	
	
	ctp2_Button		*m_spStart;
	c3_Static		*m_spOk;
	ctp2_Button		*m_spReturn;

	
	
	ctp2_Button		*m_spTribe;
	ctp2_Button		*m_spDifficulty;
	ctp2_Button		*m_spMapSize;
	ctp2_Button		*m_spPlayers;
	ctp2_Button		*m_spMap;
	ctp2_Button		*m_spRules;

	
	ctp2_Button		*m_spEditor;

	
	ctp2_Button		*m_spScenario;

	c3_Static		*m_spGeneral;
	c3_Static		*m_spWorld;
	c3_Static		*m_spCustom;

	C3TextField		*m_spName;



	

	
	ctp2_Button		*m_mapTypeButton;
	c3_Static		*m_mapTypeLabel;
	ctp2_Button		*m_worldShapeButton;
	c3_Static		*m_worldShapeLabel;

	bool			m_useCustomMap;

	c3_Static		*m_civilizationLabel;
	c3_Static		*m_leaderNameLabel;
	c3_Static		*m_difficultyLabel;
	c3_Static		*m_worldSizeLabel;
	c3_Static		*m_rulesLabel;
	c3_Static		*m_playersLabel;
	c3_Static		*m_worldTypeLabel;

	ctp2_Button		*m_quitButton;

	
	c3_Static		*m_spTitle;
	c3_Static		*m_spBackground;

	aui_StringTable	*m_string;

	
	c3_Static		*m_scenarioName;
	c3_Static		*m_scenarioStaticText;

};



class SPDropDownListItem : public c3_ListItem
{
public:
	SPDropDownListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock,MBCHAR *type,const MBCHAR *name);
	~SPDropDownListItem();
	sint32 Compare(c3_ListItem *item2, uint32 column){return 0; };
private:
	c3_Static *m_myItem;
};



sint32				callbackSetSelected(aui_Control *control, void *cookie);

aui_StringTable		*spNewStringTable(AUI_ERRCODE *errcode, MBCHAR *ldlme);
c3_Button			*spNew_c3_Button(AUI_ERRCODE *errcode, MBCHAR *ldlParent,MBCHAR *ldlMe,
									void (*callback)(aui_Control*,uint32,uint32,void*));
ctp2_Button			*spNew_ctp2_Button(AUI_ERRCODE *errcode, MBCHAR *ldlParent,MBCHAR *ldlMe,
									   void (*callback)(aui_Control*,uint32,uint32,void*));
c3_Switch			*spNew_c3_Switch(AUI_ERRCODE *errcode, MBCHAR *ldlParent,MBCHAR *ldlMe,
									void (*callback)(aui_Control*,uint32,uint32,void*)=NULL, 
									void *cookie=NULL );
aui_Switch			*spNew_aui_Switch(
									AUI_ERRCODE *errcode,
									MBCHAR *ldlParent,MBCHAR *ldlMe,
									void (*callback)(aui_Control*,uint32,uint32,void*)=NULL,
									void *cookie=NULL );
c3_Static			*spNew_c3_Static(AUI_ERRCODE *errcode, MBCHAR *ldlParent,MBCHAR *ldlMe);
C3TextField			*spNewTextEntry(AUI_ERRCODE *errcode, MBCHAR *ldlParent,MBCHAR *ldlMe,
									void (*callback)(aui_Control*,uint32,uint32,void*)=NULL ,void *cookie=NULL );
c3_ListBox			*spNew_c3_ListBox(AUI_ERRCODE *errcode, MBCHAR *ldlParent,MBCHAR *ldlMe,
									void (*callback)(aui_Control*,uint32,uint32,void*) = NULL,
									void *cookie = NULL);
c3_DropDown			*spNew_c3_DropDown(AUI_ERRCODE *errcode, MBCHAR *ldlParent,MBCHAR *ldlMe,void (*callback)(aui_Control*,uint32,uint32,void*));
void				spFillDropDown(AUI_ERRCODE *retval, c3_DropDown *mydrop, aui_StringTable *mytable, MBCHAR *listitemparent, MBCHAR *listitemme);
void				spFillListBox(AUI_ERRCODE *retval, c3_ListBox *mylist, aui_StringTable *mytable, MBCHAR *listitemparent, MBCHAR *listitemme);
TwoChoiceButton		*spNewTwoChoiceButton(AUI_ERRCODE *errcode, MBCHAR* ldlParent, MBCHAR *ldlMe,MBCHAR *ldlstringtable,
									uint32 state,void (*callback)(aui_Control*,uint32,uint32,void*) = NULL);
C3Slider			*spNew_C3Slider(AUI_ERRCODE *errcode, MBCHAR *ldlParent, MBCHAR *ldlMe,
	 								void (*callback)(aui_Control*,uint32,uint32,void*));
c3_CheckBox			*spNew_c3_CheckBox(AUI_ERRCODE *errcode, MBCHAR* ldlParent, MBCHAR *ldlMe,
									uint32 state,void (*callback)(aui_Control*,uint32,uint32,void*) = NULL, void *cookie = NULL);
aui_SwitchGroup		*spNew_aui_SwitchGroup( AUI_ERRCODE *errcode, MBCHAR *ldlParent, MBCHAR *ldlMe );




ctp2_Button* 
spNew_ctp2_Button(AUI_ERRCODE *errcode, 
				  MBCHAR *ldlParent,
				  MBCHAR *ldlMe,
				  MBCHAR *default_text,
				  void (*callback)(aui_Control*,uint32,uint32,void*),
				  MBCHAR *buttonFlavor);

#endif 
