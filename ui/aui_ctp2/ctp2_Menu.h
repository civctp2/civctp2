
#ifndef CTP2_MENU_H__
#define CTP2_MENU_H__



#include "ctp2_ListItem.h"

class ctp2_Window;
class ctp2_ListBox;
template <class T> class PointerList;
class aui_Control;

enum CTP2_MENU_ACTION {
	CTP2_MENU_ACTION_SELECT,
	CTP2_MENU_ACTION_CANCEL
};

class ctp2_Menu;
class aui_Region;
struct aui_MouseEvent;
class ctp2_MenuButton;

typedef void (CTP2MenuCallback) (
		ctp2_Menu *menu,         
		CTP2_MENU_ACTION action, 
		sint32 itemIndex,        
		void *cookie);           

class ctp2_Menu {
  private:
	class Item {
	public:
		
		Item() : m_item(NULL), m_shortcut(NULL), m_cookie(NULL) { }

		
		
		
		
		
		
		
		~Item() { delete m_shortcut; }

		ctp2_ListItem *m_item;
		MBCHAR *m_shortcut;
		void *m_cookie;
	};

		
	ctp2_Window *m_window;
	ctp2_ListBox *m_list;
	PointerList<Item> *m_items;
	sint32 m_maxTextWidth;
	sint32 m_maxIconWidth;
	sint32 m_maxShortcutWidth;
	CTP2MenuCallback *m_callback;
	sint32 m_maxItemHeight;
	bool m_resized;
	bool m_atMouse;
	aui_Region *m_siblingArea;

	void Reformat(ctp2_Menu::Item *item);
	ctp2_Menu::Item *CreateItem(MBCHAR *block, const MBCHAR *text, const MBCHAR *shortcut, const MBCHAR *icon, void *cookie);
	void Init(const MBCHAR *block, bool atMouse, CTP2MenuCallback *callback);

	void ListCallback(aui_Control *control, uint32 action, uint32 data);
	static void StaticListCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);

	void Resize();

  public:

	
	
	ctp2_Menu(bool atMouse, CTP2MenuCallback *callback);

	
	ctp2_Menu(const MBCHAR *block, bool atMouse, CTP2MenuCallback *callback);

	~ctp2_Menu();

	
	
	void AddItem(const MBCHAR *text, const MBCHAR *shortcut, void *cookie);
	void AddItemWithIcon(const MBCHAR *text, const MBCHAR *icon, const MBCHAR *shortcut, void *cookie);

	
	void Open();

	
	
	void Close();

	
	void Move(sint32 x, sint32 y);

	
	sint32 GetNumItems() const;

	
	const MBCHAR *GetShortcutString(sint32 index);

	
	
	bool HandleShortcut(const MBCHAR *string);

	
	void SetCallback(CTP2MenuCallback *callback);

	
	static void ctp2_Menu::WeaklyModalCancel(aui_MouseEvent *event, ctp2_Window *window, void *cookie, bool &passEventOn);

	
	void	Clear();

	void SetSiblingArea(aui_Region *region) { m_siblingArea = region; }

	void ActivateMenu(aui_MouseEvent *mouseData);

	void SetMenuButton(ctp2_MenuButton *butt);
};

#endif
