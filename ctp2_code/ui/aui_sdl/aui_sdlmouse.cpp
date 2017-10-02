#include "ctp2_config.h"
#include "c3.h"

#ifdef __AUI_USE_SDL__

#include "aui_sdlmouse.h"

aui_SDLMouse::aui_SDLMouse(
   AUI_ERRCODE *retval,
   MBCHAR *ldlBlock,
   BOOL useExclusiveMode)
   :
   aui_Input(),
   aui_Mouse(retval, ldlBlock),
   aui_SDLInput(retval, useExclusiveMode)
{
   Assert(AUI_SUCCESS(*retval));
   if (!AUI_SUCCESS(*retval)) return;
}

aui_SDLMouse::~aui_SDLMouse()
{
}

AUI_ERRCODE
aui_SDLMouse::GetInput(void)
{
   m_data.time = SDL_GetTicks();
   bool haveMoves = false;
   SDL_PumpEvents();

   SDL_Event od[10];
   // check for one of the mouse events
   int numElements =
      SDL_PeepEvents(od, 10, SDL_GETEVENT,
            SDL_EVENTMASK(SDL_MOUSEMOTION) | SDL_EVENTMASK(SDL_MOUSEBUTTONDOWN) |
                  SDL_EVENTMASK(SDL_MOUSEBUTTONUP));
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
         default:
            printf("event not handeled: %d\n", od[ev].type);
            break;
      }
      ev++;
   }
   return AUI_ERRCODE_OK;
}

#endif
