










 

#pragma once
#ifndef __WonderTab_H__
#define __WonderTab_H__

class aui_Control;
class ctp2_Window;
class ctp2_Button;
class ctp2_ListItem;
class ctp2_ListBox;
class ctp2_Window;
class ctp2_DropDown;
class ctp2_ListBox;



class WonderTab
{
public:
	WonderTab(ctp2_Window *parent);
	~WonderTab();

	static void Open(void);
	static void Close(void);

	
	void UpdateList();

private:
	ctp2_ListBox *m_list;

	
	ctp2_Window *m_info_window;

	
	void Show();

	
	void Hide();

	void LoadData();

	void AddWonderItem(sint32 wonder, sint32 player, sint32 turn);

	static sint32 CompareWonderItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column);

};


#endif 
