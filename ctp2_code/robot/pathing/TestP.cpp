
#include "globals.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "TestP.h"
#include "tiledmap.h"
#include "pixelutils.h"
#include "UnitAstar.h"
#include "SelItem.h"




#include "aui.h"
#include "aui_surface.h"
#include "primitives.h"
#include "player.h"

UnitAstar *g_theTestPather; 

extern TiledMap *g_tiledMap; 
extern World *g_theWorld; 
sint32 n; 
sint8 *dir_list; 

extern SelectedItem *g_selected_item; 
extern Player **g_player; 
extern void WhackScreen(); 


Path good_path, bad_path; 
void testloop(aui_Surface *surf)


{ 
   
	static int tcount = 0; 

    
    if (tcount == 0) { 
        g_theTestPather = new UnitAstar; 
    } 

	tcount++; 

    if (tcount < 37) {
        return; 
    }


 


    MapPoint start, dest, pos;
    if (!g_tiledMap->GetMouseTilePos(dest)) 
         return; 


    if (dest.x < 0) 
        return;
    if (dest.y < 0)
        return; 

  sint32 i, j; 
    for (i=0; i<50; i++) { 
        for (j=0; j<100; j++) { 
            pos.Set(i, j, 0); 
            g_theWorld->SetColor(pos, 0);
        }
    }
    sint32 c = 0; 
    if (0 < good_path.Num()) { 
     for (good_path.Start(pos); !good_path.IsEnd(); good_path.Next(pos)) { 
        g_theWorld->SetColor(pos, c); 
     } 
   }

    if (0 < bad_path.Num()) { 
     for (bad_path.Start(pos); !bad_path.IsEnd(); bad_path.Next(pos)) { 
        g_theWorld->SetColor(pos, c); 
     } 
    }
    
   
    PLAYER_INDEX s_player; 
    ID s_item; 
    SELECT_TYPE s_state; 
    sint32 s_index; 

   g_selected_item->GetTopCurItem(s_player, s_item, 
                             s_state,  s_index);
   
double dis;
double total_cost;






sint32 is_broken_path; 

   if (s_state == SELECT_TYPE_LOCAL_ARMY) { 

g_theTestPather->FindPath(*(g_player[s_player]->GetArmy(s_index)), 
                          dest, good_path, is_broken_path, bad_path,  
                          total_cost);



  

   } else { 
       return ;
   }


     char str[80];
    sprintf (str, " (%d %d)", dest.x, dest.y); 
    primitives_DrawText((aui_DirectSurface *)surf, 100, 550, (MBCHAR *)str, RGB(20,0,0), 0);

   
     c = pixelutils_RGB(0,30, 0);
     for (good_path.Start(pos); !good_path.IsEnd(); good_path.Next(pos)) { 
         g_theWorld->SetColor(pos, c); 
     }
    
     c = pixelutils_RGB(1,1,1);
    for (bad_path.Start(pos); !bad_path.IsEnd(); bad_path.Next(pos)) { 
       g_theWorld->SetColor(pos, c); 
    }
   
    WhackScreen();    
    
}
	
