
#pragma once
#ifndef SPWINDOW_FLAG
#define SPWINDOW_FLAG

class aui_Control;
class c3_Static;
class c3_DropDown;
class ctp2_Button;
class C3Window;
class c3_ListItem;
class aui_StringTable;
class TwoChoiceButton;
class aui_TextField;
class ctp2_Button;

sint32 spscreen_displayMyWindow();
sint32 spscreen_removeMyWindow(uint32 action);
AUI_ERRCODE spscreen_Initialize( void );
AUI_ERRCODE spscreen_Cleanup();

void
spscreen_tutorialPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
spscreen_newgamePress(aui_Control *control, uint32 action, uint32 data, void *cookie );







void
spscreen_loadgamePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
spscreen_instaPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
spscreen_returnPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
spscreen_quitPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
spscreen_mapPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
spscreen_optionsPress(aui_Control *control, uint32 action, uint32 data, void *cookie );


class SPWindow : public C3Window
{
public:
	SPWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD,
		bool bevel = true);
	virtual ~SPWindow();

private:

	
	ctp2_Button	*m_newgame;
	ctp2_Button	*m_tutorial,

	



				
				*m_loadgame,

				*m_return,
				*m_quit,

				*m_options;
	c3_Static	*m_background;
};


#endif
