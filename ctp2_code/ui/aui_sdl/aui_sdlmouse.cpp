#include "ctp2_config.h"
#include "c3.h"

#ifdef __AUI_USE_SDL__

#include "aui_ui.h"
#include "aui_sdlmouse.h"

#include "ctp2_listbox.h"
#include "c3_listbox.h"
#include "aui_ranger.h"

#include "civapp.h"
#include "aui_sdlsurface.h"

#include "profileDB.h"

extern CivApp           *g_civApp;

// SKIP_SDL2_EVENT_ISSUES: TODO: scrolling with mouse (screen sides) is too fast to be useful

aui_SDLMouse::aui_SDLMouse(
   AUI_ERRCODE *retval,
   const MBCHAR *ldlBlock,
   BOOL useExclusiveMode)
   :
   aui_Input(),
   aui_Mouse(retval, ldlBlock),
   aui_SDLInput(retval, useExclusiveMode),
   m_currentCursor(NULL)
{
	Assert(AUI_SUCCESS(*retval));
	if (!AUI_SUCCESS(*retval)) return;
}

aui_SDLMouse::~aui_SDLMouse()
{
	End();
}

void HandleMouseWheel(sint16 delta)
{
	if (!g_civApp) return;

	aui_Ranger *box2 = aui_Ranger::GetMouseFocusRanger();
	if (box2 != NULL)
	{
		if (delta)
		{
			if (delta < 0)
			{
				box2->ForceScroll(0, 1);
			}
			else
			{
				box2->ForceScroll(0, -1);
			}
			return;
		}
	}
	else
		fprintf(stderr, "%s L%d: Mouse wheel for SDL not handled!\n", __FILE__, __LINE__);
}

AUI_ERRCODE
aui_SDLMouse::GetInput(void)
{
   m_data.time = SDL_GetTicks();
   bool haveMoves = false;
   SDL_PumpEvents();

   SDL_Event od[10];
   // check for one of the mouse events
   int numElements = SDL_PeepEvents(od, 10, SDL_GETEVENT, SDL_MOUSEMOTION, SDL_MOUSEWHEEL);
   if (0 > numElements) {
      fprintf(stderr, "Mouse PeepEvents failed: %s\n", SDL_GetError());
      return AUI_ERRCODE_GETDEVICEDATAFAILED;
   }
   if (0 == numElements) {
      if(haveMoves)
         return AUI_ERRCODE_OK;
      else
         return AUI_ERRCODE_NOINPUT;
   }
   sint32 ev = 0;
   while(ev < numElements) {
      switch (od[ev].type) {
         case SDL_MOUSEMOTION:
            m_data.position.x = od[ev].motion.x;
            m_data.position.y = od[ev].motion.y;
            m_data.lbutton = !!(od[ev].motion.state & SDL_BUTTON_LMASK);
            m_data.rbutton = !!(od[ev].motion.state & SDL_BUTTON_RMASK);
            break;
         case SDL_MOUSEBUTTONDOWN:
         case SDL_MOUSEBUTTONUP:
            m_data.position.x = od[ev].button.x;
            m_data.position.y = od[ev].button.y;
            if (od[ev].button.button == SDL_BUTTON_LEFT) {
                  m_data.lbutton = od[ev].button.state == SDL_PRESSED;
            } else if (od[ev].button.button == SDL_BUTTON_RIGHT) {
                  m_data.rbutton = od[ev].button.state == SDL_PRESSED;
            }
            break;
         case SDL_MOUSEWHEEL:
	    if (od[ev].wheel.y > 0) {
               HandleMouseWheel((sint16)1);
	    } else if (od[ev].wheel.y < 0) {
	       HandleMouseWheel((sint16)-1);
	    }
	    break;
         default:
            printf("event not handled: %d\n", od[ev].type);
            break;
      }
      ev++;
   }
   return AUI_ERRCODE_OK;
}

void aui_SDLMouse::ActivateCursor(aui_Cursor *cursor)
{
	if (cursor != m_currentCursor) {
		aui_Surface *cursorSurface = cursor->TheSurface();
		aui_SDLSurface *sdlCursorSurface = dynamic_cast<aui_SDLSurface *>(cursorSurface);
		if (sdlCursorSurface != NULL) {
			POINT hotspot;
			cursor->GetHotspot(hotspot);
			SDL_Cursor *sdlCursor = SDL_CreateColorCursor(sdlCursorSurface->GetSDLSurface(), hotspot.x, hotspot.y);
			if (sdlCursor != NULL) {
				SDL_Cursor *currentSDLCursor = SDL_GetCursor();
				SDL_SetCursor(sdlCursor);
				if (currentSDLCursor) {
					SDL_FreeCursor(currentSDLCursor);
				}
				m_currentCursor = cursor;
			}
		}
	}
}

AUI_ERRCODE	aui_SDLMouse::BltWindowToPrimary(aui_Window *window)
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

	sint32 windowX = window->X();
	sint32 windowY = window->Y();
	aui_Surface *windowSurface = window->TheSurface();
	aui_DirtyList *windowDirtyList = window->GetDirtyList();

	ListPos position = windowDirtyList->GetHeadPosition();
	for (sint32 j = windowDirtyList->L(); j; j--)
	{
		RECT *windowDirtyRect = windowDirtyList->GetNext(position);

		RECT screenDirtyRect = *windowDirtyRect;
		OffsetRect(&screenDirtyRect, windowX, windowY);

		AUI_ERRCODE errcode = g_ui->BltToSecondary(
				screenDirtyRect.left,
				screenDirtyRect.top,
				windowSurface,
				windowDirtyRect,
				k_AUI_BLITTER_FLAG_COPY );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}
	}

	return retcode;
}

AUI_ERRCODE	aui_SDLMouse::BltDirtyRectInfoToPrimary()
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	if (g_civApp->IsInBackground()) return AUI_ERRCODE_OK;

	tech_WLList<aui_UI::DirtyRectInfo *> *driList = g_ui->GetDirtyRectInfoList();

	uint32 blitFlags;
	LPVOID primaryBuf = NULL;

	if (g_theProfileDB && g_theProfileDB->IsUseDirectXBlitter())
	{
		blitFlags = k_AUI_BLITTER_FLAG_COPY;
	}
	else
	{
		blitFlags = k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST;
		errcode = g_ui->Secondary()->Lock(NULL, &primaryBuf, 0);
		Assert( errcode == AUI_ERRCODE_OK );
	}

	ListPos position = driList->GetHeadPosition();
	for (sint32 j = driList->L(); j; j--)
	{
		aui_UI::DirtyRectInfo *dri = driList->GetNext(position);

		aui_Window *window = dri->window;

		sint32 windowX = window->X();
		sint32 windowY = window->Y();
		aui_Surface *windowSurface = window->TheSurface();

		if (!windowSurface) continue;

		if (g_civApp->IsInBackground()) continue;

		RECT *windowDirtyRect = &dri->rect;

		RECT screenDirtyRect = *windowDirtyRect;
		OffsetRect(&screenDirtyRect, windowX, windowY);

		if (!g_civApp->IsInBackground()) // Actual Drawing
		{
			errcode = g_ui->BltToSecondary(
					screenDirtyRect.left,
					screenDirtyRect.top,
					windowSurface,
					windowDirtyRect,
					blitFlags);
			Assert(errcode == AUI_ERRCODE_OK);
			if (errcode != AUI_ERRCODE_OK)
			{
				retcode = AUI_ERRCODE_BLTFAILED;
				break;
			}
		}
	}

	errcode = g_ui->BltSecondaryToPrimary(blitFlags);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK)
	{
		retcode = AUI_ERRCODE_BLTFAILED;
	}

	if (!g_theProfileDB || !g_theProfileDB->IsUseDirectXBlitter())
	{
		errcode = g_ui->Secondary()->Unlock( primaryBuf );
		Assert( errcode == AUI_ERRCODE_OK );
	}

	return retcode;
}

AUI_ERRCODE	aui_SDLMouse::BltBackgroundColorToPrimary(COLORREF color, aui_DirtyList *colorAreas)
{
	if (g_civApp->IsInBackground()) return AUI_ERRCODE_OK;

	ListPos position = colorAreas->GetHeadPosition();
	for (sint32 j = colorAreas->L(); j; j--)
	{
		RECT *screenDirtyRect = colorAreas->GetNext(position);
		AUI_ERRCODE errcode = g_ui->ColorBltToSecondary(screenDirtyRect, color,0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			return AUI_ERRCODE_BLTFAILED;
		}
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE	aui_SDLMouse::BltBackgroundImageToPrimary(aui_Image *image, RECT *imageRect, aui_DirtyList *imageAreas)
{
	if (g_civApp->IsInBackground()) return AUI_ERRCODE_OK;

	ListPos position = imageAreas->GetHeadPosition();
	for ( sint32 j = imageAreas->L(); j; j-- )
	{
		RECT *screenDirtyRect = imageAreas->GetNext(position);
		AUI_ERRCODE errcode = g_ui->BltToSecondary(
			screenDirtyRect->left,
			screenDirtyRect->top,
			image->TheSurface(),
			screenDirtyRect,
			k_AUI_BLITTER_FLAG_COPY);

		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			return AUI_ERRCODE_BLTFAILED;
		}
	}

	return AUI_ERRCODE_OK;
}

#endif
