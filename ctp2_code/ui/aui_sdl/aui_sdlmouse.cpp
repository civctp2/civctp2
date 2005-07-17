#include "ctp2_config.h"
#include "c3.h"

#ifdef __AUI_USE_SDL__

#include "aui_sdlmouse.h"

aui_SDLMouse::aui_SDLMouse(
   AUI_ERRCODE *retval,
   MBCHAR *ldlBlock,
   BOOL useExclusiveMode)
   :
   aui_Input(retval),
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

   for ( sint32 numInputs = 200; numInputs; numInputs-- ) {
      SDL_Event od;
      // check for one of the mouse events
      int numElements =
         SDL_PeepEvents(&od, 1, SDL_GETEVENT, SDL_MOUSEEVENTMASK);
      if (0 > numElements) {
         fprintf(stderr, "PeepEvents failed: %s\n", SDL_GetError());
         return AUI_ERRCODE_GETDEVICEDATAFAILED;
      }
      if (0 == numElements) {
         if(haveMoves)
            return AUI_ERRCODE_OK;
         else
            return AUI_ERRCODE_NOINPUT;
      }
      switch (od.type) {
      case SDL_MOUSEMOTION:
         m_data.position.x = od.motion.x;
         m_data.position.y = od.motion.y;
         m_data.lbutton = !!(od.motion.state & SDL_BUTTON_LMASK);
         m_data.rbutton = !!(od.motion.state & SDL_BUTTON_RMASK);
         break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
         m_data.position.x = od.button.x;
         m_data.position.y = od.button.y;
         if (od.button.button == SDL_BUTTON_LEFT) {
            m_data.lbutton = od.button.state == SDL_PRESSED;
         } else if (od.button.button == SDL_BUTTON_RIGHT) {
            m_data.rbutton = od.button.state == SDL_PRESSED;
         }
         break;
      default:
         printf("event %d\n", od.type);
         continue;
      }
      return AUI_ERRCODE_OK;
   }
   return AUI_ERRCODE_OK;
}

#endif
