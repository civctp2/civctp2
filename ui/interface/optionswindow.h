
#pragma once
#ifndef OPTIONSWINDOW_FLAG
#define OPTIONSWINDOW_FLAG

#include "c3_popupwindow.h"

class c3_Static;
;class c3_Button;
class ctp2_Button;

sint32 optionsscreen_displayMyWindow(sint32 from);
sint32 optionsscreen_removeMyWindow(uint32 action);
AUI_ERRCODE optionsscreen_Initialize( void );
AUI_ERRCODE optionsscreen_Cleanup();

void optionsscreen_graphicsPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionsscreen_soundPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionsscreen_musicPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionsscreen_newgamePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionsscreen_savegamePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionsscreen_savescenarioPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionsscreen_loadgamePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionsscreen_restartPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionsscreen_quitPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionsscreen_returnPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionsscreen_gameplayPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionsscreen_mapeditorPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionsscreen_keyboardPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void optionsscreen_quitToShellPress(aui_Control *control, uint32 action, uint32 data, void *cookie );


class OptionsWindow : public c3_PopupWindow
{
public:
	OptionsWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD,
		bool bevel = true);
	virtual ~OptionsWindow();

	sint32 EnableButtons( void );
	sint32 DisableButtons( void );

	
	void RemoveQuitToWindowsButton( void );
	void AddQuitToWindowsButton( void );

	ctp2_Button *SaveGameButton() const { return m_savegame; }
	ctp2_Button *LoadGameButton() const { return m_loadgame; }
	ctp2_Button *QuitToShellButton() const { return m_quittoshell; }

	ctp2_Button *GraphicsButton() const { return m_graphics; }
	ctp2_Button *SoundButton() const { return m_sound; }
	ctp2_Button *MusicButton() const { return m_music; }
	ctp2_Button *NewGameButton() const { return m_newgame; }
	ctp2_Button *RestartButton() const { return m_restart; }
	ctp2_Button *GamePlayButton() const { return m_gameplay; }
	ctp2_Button *MapEditorButton() const { return m_mapeditor; }
	ctp2_Button *KeyboardButton() const { return m_keyboard; }

private:
	
	ctp2_Button		*m_graphics,
					*m_sound,
					*m_music,
					*m_newgame,
					*m_savegame,
					*m_loadgame,
					*m_restart,
					*m_gameplay,
					*m_mapeditor,
					*m_keyboard,
					*m_quittoshell;
	

	c3_Static		*m_configHeader;
	c3_Static		*m_gameHeader;

};


#endif
