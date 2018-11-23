#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _BMH_CREDITS_SCREEN_H_
#define _BMH_CREDITS_SCREEN_H_

class CreditsWindow;

#include "c3window.h"

class aui_Control;
class aui_Static;
class aui_Button;
class ctp2_Button;
class c3_SimpleAnimation;
class c3_TriggeredAnimation;

class aui_BitmapFont;
class c3_CreditsText;


sint32  creditsscreen_Initialize(void);
void    creditsscreen_Cleanup(void);

class CreditsWindow : public C3Window {
public:

	CreditsWindow(
		AUI_ERRCODE *retval,
		sint32 id,
		const MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD, bool bevel = true );

	CreditsWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		const MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD, bool bevel = true );

	virtual ~CreditsWindow();


	virtual AUI_ERRCODE Idle();

	void ToggleAnimation();

	void ShowSecretImage();

protected:

	CreditsWindow() : C3Window() {}

	void InitCommonLdl(const MBCHAR *ldlBlock);

	void CleanPointers();

	void CleanUp(aui_Control *control);

private:

	sint32 m_numberOfBackgroundAnims;
	sint32 m_numberOfTriggeredAnims;

	aui_Static *m_background;
	aui_Static *m_border;

	ctp2_Button *m_exitButton;

	aui_Button *m_pauseButton;

	aui_Static *m_secretImage;
	aui_Button *m_secretButton;

	c3_SimpleAnimation **m_backgroundAnim;
	c3_TriggeredAnimation **m_triggeredAnim;


	c3_CreditsText *m_creditsText;

	bool m_animating;

	sint32 m_animationSpeed;

	uint32 lastIdle;
};






extern CreditsWindow*	g_creditsWindow;

#endif
