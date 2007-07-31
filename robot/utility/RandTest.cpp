
#include "c3.h"
#include "player.h"
#include "unit.h"
#include "SelItem.h"

#include "RandGen.h"

extern RandomGenerator *g_rand; 

extern HWND gHwnd; 
extern Player **g_player; 
extern SelectedItem *g_selected_item; 

sint32 g_is_rand_test=FALSE; 
sint32 g_ai_rand_test_wait = 2; 
sint32 g_ai_rand_test_max = 1000; 
sint32 g_ai_rand_test_total = 0; 

void ai_rand_test()

{ 

    return; 

    static count=0; 
    static total=0; 

    if (0 <g_ai_rand_test_wait) { 
       count = (count + 1) %g_ai_rand_test_wait; 
       if (0 < count) {       
            return;
        }
    }

    g_ai_rand_test_total++;
   
    if (g_ai_rand_test_max < g_ai_rand_test_total ) { 
        g_is_rand_test = FALSE; 
    }

  
   
   


   if (g_rand->Next(5) == 0) { 
   	    PostMessage(gHwnd, WM_CHAR, 'b', 0); 
   }

   if (g_rand->Next(20) == 0) { 
       	PostMessage(gHwnd, WM_CHAR, 's', 0); 
   }
   PostMessage(gHwnd, WM_CHAR, '1' + g_rand->Next(9), 0); 
   
}