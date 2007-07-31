

enum MS_STRING {
	MS_STRING_AUTO_ON,
	MS_STRING_AUTO_OFF,
	MS_STRING_RANDOM_ON,
	MS_STRING_RANDOM_OFF
};

sint32 musicscreen_displayMyWindow();
sint32 musicscreen_removeMyWindow(uint32 action);
AUI_ERRCODE musicscreen_Initialize( void );
AUI_ERRCODE musicscreen_Cleanup();

void musicscreen_checkPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void musicscreen_selectTrackPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void musicscreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
