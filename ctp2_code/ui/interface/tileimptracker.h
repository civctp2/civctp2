#ifndef __TILE_IMP_TRACKER_WINDOW
#define __TILE_IMP_TRACKER_WINDOW

#include "c3window.h"

extern sint32 g_bpp;

class MapPoint;

class TileimpTrackerWindow :public C3Window
{
public:
	TileimpTrackerWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD) : C3Window(retval,id,ldlBlock,g_bpp,type,false) {};
	virtual ~TileimpTrackerWindow() {};

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );


};

sint32 tileimptracker_Initialize();
void tileimptracker_DisplayData(MapPoint &p, sint32 type);
sint32 tileimptracker_Cleanup();

#endif
