#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __CMS_H__
#define __CMS_H__

sint32 custommapscreen_displayMyWindow(BOOL viewMode = FALSE, sint32 useMode = 0);
sint32 custommapscreen_removeMyWindow(uint32 action);
AUI_ERRCODE custommapscreen_Initialize( aui_Control::ControlActionCallback *callback = NULL );
AUI_ERRCODE custommapscreen_Cleanup();
void custommapscreen_getValues(
	sint32 &wetdry,
	sint32 &warmcold,
	sint32 &oceanland,
	sint32 &islandcontinent,
	sint32 &homodiverse,
	sint32 &goodcount);
void custommapscreen_setValues(
	sint32 wetdry,
	sint32 warmcold,
	sint32 oceanland,
	sint32 islandcontinent,
	sint32 homodiverse,
	sint32 goodcount);

void custommapscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void custommapscreen_cancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void custommapscreen_wetdrySlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
void custommapscreen_warmcoldSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
void custommapscreen_oceanlandSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
void custommapscreen_islandcontinentSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
void custommapscreen_homodiverseSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );
void custommapscreen_goodcountSlide(aui_Control *control, uint32 action, uint32 data, void *cookie );

#endif 
