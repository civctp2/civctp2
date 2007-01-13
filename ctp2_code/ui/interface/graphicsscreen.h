

sint32 graphicsscreen_displayMyWindow();
sint32 graphicsscreen_removeMyWindow(uint32 action);
AUI_ERRCODE graphicsscreen_Initialize( void );
AUI_ERRCODE graphicsscreen_Cleanup();
void graphicsscreen_getValues(sint32 &bright, sint32 &gamma, sint32 &color,sint32 &contrast);

void graphicsscreen_screensizeSelect(aui_Control *control, uint32 action, uint32 data, void *cookie );

void graphicsscreen_exitPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void graphicsscreen_checkPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

void graphicsscreen_selectResolution(aui_Control *control, uint32 action, uint32 data, void *cookie);

void graphicsscreen_brightSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
void graphicsscreen_gammaSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
void graphicsscreen_colorSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
void graphicsscreen_contrastSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
