
#pragma once
#ifndef INITIALPLAYWINDOW_FLAG
#define INITIALPLAYWINDOW_FLAG

#include "c3window.h"

class c3_Static;
class c3_Button;
class ctp2_Button;

class InitPlayWindow;

sint32 initialplayscreen_displayMyWindow();
sint32 initialplayscreen_removeMyWindow(uint32 action);
AUI_ERRCODE initialplayscreen_Initialize( void );
AUI_ERRCODE initialplayscreen_Cleanup();


void
spritetest_spPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

void
initialplayscreen_spPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_mpPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_loadPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_continuePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_instantPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_mapeditorPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_quitPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_creditsPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void initialplayscreen_hotseatPress(aui_Control *control, uint32 action, uint32 data, void *cookie);
void initialplayscreen_emailPress(aui_Control *control, uint32 action, uint32 data, void *cookie);



C3Window *GetInitialPlayScreen();

class InitPlayWindow : public C3Window
{
public:
	InitPlayWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD,
		bool bevel = true);
	virtual ~InitPlayWindow();

private:

	ctp2_Button	    *m_spriteTest,
					*m_sp,
					*m_email, 
					*m_hotseat,
					*m_mp,
					*m_credits,
					*m_quit;

	c3_Button	   	*m_load,
					*m_continue,
					*m_instant,
					*m_mapeditor;
		            
		            
	
	c3_Static		*m_background;
};


#endif
