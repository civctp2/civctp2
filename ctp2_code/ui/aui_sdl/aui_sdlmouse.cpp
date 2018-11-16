#include "ctp2_config.h"
#include "c3.h"

#ifdef __AUI_USE_SDL__

#include "aui_sdlmouse.h"

#include "ctp2_listbox.h"
#include "c3_listbox.h"
#include "aui_ranger.h"

#include "civapp.h"
extern CivApp           *g_civApp;

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

void HandleMouseWheel(sint16 delta){
    if (!g_civApp) return;


    ctp2_ListBox *box = ctp2_ListBox::GetMouseFocusListBox();

    if (box != NULL) {
        if (delta) {
            if (delta < 0) {
                box->ForceScroll(0, 1);
                } else {
                box->ForceScroll(0, -1);
                }

            return;
            }
        }
    else {
        c3_ListBox *box1 = c3_ListBox::GetMouseFocusListBox();
        if (box1 != NULL) {
            if (delta) {
                if (delta < 0) {
                    box1->ForceScroll(0, 1);
                    } else {
                    box1->ForceScroll(0, -1);
                    }

                return;
                }

            }
        else { //this idea came last; might turn the previous ones unnessesary
            aui_Ranger *box2 = aui_Ranger::GetMouseFocusRanger();
            if (box2 != NULL) {
                if (delta) {
                    if (delta < 0) {
                        box2->ForceScroll(0, 1);
                        } else {
                        box2->ForceScroll(0, -1);
                        }

                    return;
                    }

                }

            }
        }

// 		BOOL scrolled = FALSE;

// 		if (g_civApp->IsGameLoaded()) {
// 			if (g_tiledMap) {
// 				if (delta < 0) {
// 					g_tiledMap->ScrollMap(0, 2);
// 					scrolled = TRUE;
// 				} else
// 					if (delta > 0) {
// 						g_tiledMap->ScrollMap(0, -2);
// 						scrolled = TRUE;
// 					}
// 			}
// 		}

// 		if (scrolled) {
// 			g_tiledMap->RetargetTileSurface(NULL);
// 			g_tiledMap->Refresh();
// 			g_tiledMap->InvalidateMap();
// 			g_tiledMap->ValidateMix();
// 		}
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
                SDL_PeepEvents(&od, 1, SDL_GETEVENT,
                    SDL_EVENTMASK(SDL_MOUSEMOTION) | SDL_EVENTMASK(SDL_MOUSEBUTTONDOWN) |
                    SDL_EVENTMASK(SDL_MOUSEBUTTONUP));
            if (0 > numElements) {
                fprintf(stderr, "%s L%d: SDL_PeepEvents: Still events stored! Error?: %s\n", __FILE__, __LINE__, SDL_GetError());
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

                if (od.button.button == SDL_BUTTON_WHEELUP){
                    HandleMouseWheel((sint16)1);
                    break;
                    }
                else if (od.button.button == SDL_BUTTON_WHEELDOWN){
                    HandleMouseWheel((sint16)-1);
                    break;
                    }
//           if (od.button.button == SDL_BUTTON_WHEELUP || od.button.button == SDL_BUTTON_WHEELDOWN){
//               SDL_PushEvent(&od);
//               break;
//               }

                m_data.position.x = od.button.x;
                m_data.position.y = od.button.y;
                if (od.button.button == SDL_BUTTON_LEFT) {
                    m_data.lbutton = od.button.state == SDL_PRESSED;
                    } else if (od.button.button == SDL_BUTTON_RIGHT) {
                    m_data.rbutton = od.button.state == SDL_PRESSED;
                    }
                break;
            default:
                printf("event not handeled: %d\n", od.type);
                continue;
                }
            return AUI_ERRCODE_OK;
            }
        return AUI_ERRCODE_OK;
        }

#endif
