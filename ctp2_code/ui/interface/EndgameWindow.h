









#pragma once
#ifndef _BMH_END_GAME_WINDOW_H_
#define _BMH_END_GAME_WINDOW_H_


#include "c3window.h"
#include "keyboardhandler.h"


class aui_Control;
class aui_Static;
class c3_Static;
class c3_ColoredStatic;
class c3_Button;
class c3_Blend;
class c3_Animation;
class c3_DarkenArea;
class c3_YetAnotherProgressBar;
class EndGame;


extern sint32 endgamewindow_Initialize();


extern sint32 endgamewindow_Cleanup();


class EndGameWindow : public C3Window, public KeyboardHandler {
public:
	
	EndGameWindow(
		AUI_ERRCODE *retval, 
		sint32 id, 
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD, bool bevel = true );

	
	EndGameWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD, bool bevel = true );

	
	virtual ~EndGameWindow();

	
	void SetStage(sint32 stage, sint32 lastStage);

	
	void SetEmbryoTank(bool exists, bool existed, bool isBroken, sint32 soundID);

	
	void SetContainmentFields(sint32 number, sint32 alreadyDisplayed, sint32 soundID);
	void SetECDs(sint32 number, sint32 alreadyDisplayed, sint32 soundID);
	void SetSplicers(sint32 number, sint32 alreadyDisplayed, sint32 soundID);

	
	void Update(EndGame *endGame);

	
	void UpdateTurn(EndGame *endGame);

	
	
	virtual AUI_ERRCODE Idle();

	void kh_Close();

protected:

	
	EndGameWindow() : C3Window() {}

	
	void InitCommonLdl(MBCHAR *ldlBlock);
	
	
	void CleanPointers();
	
	
	void CleanUp(aui_Control *control);

	
	void UpdateBlend(sint32 deltaTime, c3_Blend *blendControl);

private:

	
	MBCHAR *m_embryoTankName;
	MBCHAR *m_containmentFieldName;
	MBCHAR *m_ECDName;
	MBCHAR *m_splicerName;

	
	sint32 m_numberOfStages;
	sint32 m_numberOfContainmentFields;
	sint32 m_numberOfECDs;
	sint32 m_numberOfSplicers;
	sint32 m_numberOfBackgroundAnims;

	
	aui_Static *m_background;
	aui_Static *m_border;
	aui_Static *m_brokenTank;
	c3_Blend *m_embryoTank;
	c3_Blend **m_embryoStage;
	c3_Blend **m_containmentField;
	c3_Blend **m_ECD;
	c3_Blend **m_splicer;

	
	c3_Animation **m_backgroundAnim;
	c3_Animation *m_embryoGlow;


	
	c3_DarkenArea *m_darkenArea;
	sint32 m_numberOfLabels;
	aui_Static **m_labels;
	c3_ColoredStatic *m_progressBackground;
	c3_YetAnotherProgressBar *m_turnProgress;
	aui_Static *m_turnsRemaining;
	aui_Static *m_ecdRatio;
	aui_Static *m_containmentFieldRatio;
	aui_Static *m_splicerRatio;
	aui_Static *m_chanceOfFailure;
	sint32 m_numberOfStageLights;
	c3_ColoredStatic **m_stageLights;

	
	c3_Button *m_exitButton;

	
	sint32 m_blendSpeed;

	
	uint32 lastIdle;
};


extern EndGameWindow*	g_endgameWindow;		

#endif 
