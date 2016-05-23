#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __BACKGROUNDWIN_H__
#define __BACKGROUNDWIN_H__

sint32      backgroundWin_Initialize(bool fullscreen = false);
void        backgroundWin_Cleanup(void);
AUI_ERRCODE background_draw_handler(LPVOID bg);

#endif
