

sint32 gameplayoptions_displayMyWindow();
sint32 gameplayoptions_removeMyWindow(uint32 action);
AUI_ERRCODE gameplayoptions_Initialize( void );
AUI_ERRCODE gameplayoptions_Cleanup();

void gameplayoptions_checkPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void gameplayoptions_exitPress(aui_Control *control, uint32 action, uint32 data, void *cookie );


void gameplayoptions_mouseSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
