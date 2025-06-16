#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __aui_sdl__aui_sdlmouse_h__
#define __aui_sdl__aui_sdlmouse_h__ 1

#include "ctp2_config.h"

#if defined(__AUI_USE_SDL__)

#include "aui_mouse.h"
#include "aui_sdlinput.h"

class aui_SDLMouse : public aui_Mouse, public aui_SDLInput
{
public:
	aui_SDLMouse(AUI_ERRCODE *retval, const MBCHAR *ldlBlock, BOOL useExclusiveMode = FALSE);
	virtual ~aui_SDLMouse();

	virtual AUI_ERRCODE HandleAnim();
	virtual void SetAnimIndexes(sint32 firstIndex, sint32 lastIndex);

	virtual AUI_ERRCODE BltWindowToPrimary(aui_Window *window);
	virtual AUI_ERRCODE BltDirtyRectInfoToPrimary();
	virtual AUI_ERRCODE BltBackgroundColorToPrimary(COLORREF color, aui_DirtyList *colorAreas);
	virtual AUI_ERRCODE BltBackgroundImageToPrimary(aui_Image *image, RECT *imageRect, aui_DirtyList *imageAreas);

	virtual sint32 ManipulateInputs(aui_MouseEvent *data, BOOL add);
	virtual void GetInput(SDL_Event* event);

protected:
	aui_SDLMouse() {}

	virtual void ActivateCursor(aui_Cursor *cursor);
	virtual AUI_ERRCODE Acquire(void) { return AUI_ERRCODE_OK; }
	virtual AUI_ERRCODE Unacquire(void) { return AUI_ERRCODE_OK; }

	virtual AUI_ERRCODE GetInput() { return AUI_ERRCODE_OK; };
	virtual AUI_ERRCODE ReactToInput() { return AUI_ERRCODE_OK; }

private:
	aui_Cursor* m_currentCursor;
	SDL_TimerID m_animationTimer;
	uint32      m_lastFrameTick;

	static SDL_mutex* s_cs;
};

typedef aui_SDLMouse aui_NativeMouse;

#endif // defined(__AUI_USE_SDL__)

#endif
