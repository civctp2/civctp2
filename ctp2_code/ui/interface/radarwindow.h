






#pragma once
#ifndef ___BMH_RADAR_WINDOW_HEADER
#define ___BMH_RADAR_WINDOW_HEADER


class ctp2_Window;


sint32	radarwindow_Initialize();


void	radarwindow_Display();


sint32	radarwindow_Cleanup();

void    radarwindow_Hide();
void    radarwindow_Show();
void    radarwindow_Toggle();


extern ctp2_Window *g_radarWindow;

#endif 
