










 

#pragma once
#ifndef __VICTORYWINDOW_H__
#define __VICTORYWINDOW_H__



class ctp2_Window;
class ctp2_TabGroup;
class ctp2_Button;


class VictoryWindow 
{
public:
	VictoryWindow(AUI_ERRCODE *err);
	~VictoryWindow();


























	ctp2_Window		*m_window;
private:
	ctp2_TabGroup	*m_tabGroup;
	ctp2_Button		*m_okButton;

};

extern VictoryWindow	*g_victoryWindow;

#endif 
