#include "c3.h"
#include "clock.h"


static int start_time_ms; 
                           

static int current_time_ms;  
                              

static int delta_time_ms; 
                           
static float delta_time; 

static float current_time; 












void init_clock()

{ 
   current_time_ms = start_time_ms = GetTickCount(); 
} 











void update_clock()

{  int t;

   t = GetTickCount() - start_time_ms; 
   delta_time_ms = t - current_time_ms; 
   delta_time = float(delta_time_ms) * 0.001f; 
   current_time_ms = t; 
   current_time = float(current_time_ms) * 0.001f; 
}











float dt() { 
   return delta_time; 

} 










float get_time() { 
   return current_time; 
}

