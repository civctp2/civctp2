











#pragma once
#ifndef __BACKGROUNDWIN_H__
#define __BACKGROUNDWIN_H__

sint32 backgroundWin_Initialize(bool fullscreen=false);
sint32 backgroundWin_Cleanup(void);
AUI_ERRCODE background_draw_handler(LPVOID bg);


#endif