

sint32 soundscreen_displayMyWindow();
sint32 soundscreen_removeMyWindow(uint32 action);
AUI_ERRCODE soundscreen_Initialize( void );
AUI_ERRCODE soundscreen_Cleanup();
void soundscreen_getValues(sint32 &sfx, sint32 &music, sint32 &voice);


void soundscreen_exitPress(aui_Control *control, uint32 action, uint32 data, void *cookie );


void soundscreen_sfxSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
void soundscreen_musicSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
void soundscreen_voiceSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );

