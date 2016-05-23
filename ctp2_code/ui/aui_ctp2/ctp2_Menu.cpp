#include "c3.h"
#include "ctp2_Menu.h"
#include "aui_ldl.h"
#include "ctp2_Window.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "ctp2_Static.h"
#include "c3ui.h"
#include "pointerlist.h"

#include "StrDB.h"
#include "ctp2_MenuButton.h"

extern  C3UI				*g_c3ui;

#define k_STANDARD_EXTRA_WIDTH 60
#define k_MINIMUM_TEXT_WIDTH 20

#define k_EXTRA_WINDOW_WIDTH 4
#define k_EXTRA_WINDOW_HEIGHT 4
#define k_LIST_HEIGHT_DELTA 20
#define k_LIST_WIDTH_DELTA 0
#define k_EXTRA_SHORTCUT_WIDTH 10

#define k_LEFT_ITEM_MARGIN 3
#define k_RIGHT_ITEM_MARGIN 3

#define k_EXTRA_LEFT_SPACE 4

extern sint32 g_ScreenHeight, g_ScreenWidth;

ctp2_Menu::ctp2_Menu(bool atMouse, CTP2MenuCallback *callback)
{
	Init("CTP2_MENU", atMouse, callback);
}

ctp2_Menu::ctp2_Menu(const MBCHAR *block, bool atMouse, CTP2MenuCallback *callback)
{
	Init(block, atMouse, callback);
}

class CleanupMenuWindowAction : public aui_Action
{
public:
	CleanupMenuWindowAction(aui_Window * window)
    :   aui_Action  (),
        m_window    (window)
    { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	)
    {
        if (m_window)
        {
            delete m_window->GetChildByIndex(0);
            delete m_window;
        }
    };

protected:
	aui_Window *    m_window;
};

ctp2_Menu::~ctp2_Menu()
{
	Close();

	if(m_window) {

		g_c3ui->AddAction(new CleanupMenuWindowAction(m_window));
	}

	if(m_items) {
		if(!m_resized) {

			PointerList<Item>::Walker walk(m_items);
			for(; walk.IsValid(); walk.Next()) {
				delete walk.GetObj()->m_item;
			}
		}

		m_items->DeleteAll();
		delete m_items;
		m_items = NULL;
	}
}

void
ctp2_Menu::SetCallback(CTP2MenuCallback *callback)
{
	m_callback = callback;
}

void ctp2_Menu::Init(const MBCHAR *block, bool atMouse, CTP2MenuCallback *callback)
{
	m_window = NULL;
	m_list = NULL;
	m_items = new PointerList<Item>;
	m_maxTextWidth = k_MINIMUM_TEXT_WIDTH;
	m_maxIconWidth = k_LEFT_ITEM_MARGIN;
	m_maxShortcutWidth = 0;
	m_callback = callback;
	m_maxItemHeight = 0;
	m_resized = false;
	m_atMouse = atMouse;
	m_siblingArea = NULL;

	MBCHAR dammit[k_MAX_NAME_LEN];
	strcpy(dammit, block);

	m_window = (ctp2_Window *)aui_Ldl::GetObject(dammit);
	if(!m_window) {
		m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(dammit);
	}

	Assert(m_window);
	if(!m_window)
		return;

	m_list = (ctp2_ListBox *)m_window->GetChildByIndex(0);
	Assert(m_list);
	if(m_list) {
		m_list->Clear();
		m_list->SetActionFuncAndCookie(ctp2_Menu::StaticListCallback, this);
		m_list->SetIgnoreOutsideDrops(true);
	}

	if (atMouse)
		m_window->Move(g_c3ui->TheMouse()->X(), g_c3ui->TheMouse()->Y());

	m_window->SetWeaklyModalCancelCallback(ctp2_Menu::WeaklyModalCancel, (void *)this);
}

void ctp2_Menu::Reformat(ctp2_Menu::Item *menuItem)
{


	ctp2_Static *box = (ctp2_Static *)menuItem->m_item->GetChildByIndex(0);
	Assert(box);
	if(!box)
		return;


	ctp2_Static *textBox = (ctp2_Static *)box->GetChildByIndex(0);
	Assert(textBox);
	if(!textBox)
		return;

	textBox->Resize(m_maxTextWidth, m_maxItemHeight);
	textBox->Move(m_maxIconWidth + k_EXTRA_LEFT_SPACE, textBox->Y());

	ctp2_Static *shortcutBox = (ctp2_Static *)box->GetChildByIndex(1);
	Assert(shortcutBox);
	if(shortcutBox) {
		shortcutBox->Move(m_maxIconWidth + textBox->Width() + k_EXTRA_LEFT_SPACE, shortcutBox->Y());
		shortcutBox->Resize(m_maxShortcutWidth, m_maxItemHeight);
	}

	if(box->NumChildren() > 2) {
		ctp2_Static *iconBox = (ctp2_Static *)box->GetChildByIndex(2);
		Assert(iconBox);
		if(iconBox) {
			iconBox->Resize(m_maxIconWidth - k_LEFT_ITEM_MARGIN, m_maxItemHeight);
			iconBox->Move(k_LEFT_ITEM_MARGIN, iconBox->Y());
		}
	}

	box->Resize(m_maxIconWidth + textBox->Width() + m_maxShortcutWidth + k_RIGHT_ITEM_MARGIN + k_EXTRA_LEFT_SPACE, m_maxItemHeight);
	menuItem->m_item->Resize(box->Width(), box->Height());
}

ctp2_Menu::Item *ctp2_Menu::CreateItem(const MBCHAR *block, const MBCHAR *text, const MBCHAR *shortcut,
									   const MBCHAR *icon, void *cookie)
{
	ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot(block);
	Assert(item);
	if(!item)
		return NULL;

	ctp2_Menu::Item *menuItem = new ctp2_Menu::Item;
	menuItem->m_item = item;
	if(!shortcut) {
		shortcut = "";
	}

	menuItem->m_shortcut = new MBCHAR[(strlen(shortcut) + 1) * sizeof(MBCHAR)];
	strcpy(menuItem->m_shortcut, shortcut);
	menuItem->m_cookie = cookie;


	item->SetUserData(menuItem);

	ctp2_Static *box = (ctp2_Static *)menuItem->m_item->GetChildByIndex(0);
	Assert(box);
	if(!box)
		return menuItem;

	ctp2_Static *textBox = (ctp2_Static *)box->GetChildByIndex(0);
	Assert(textBox);
	if(!textBox)
		return menuItem;

	textBox->SetText(text);

	ctp2_Static *shortcutBox = (ctp2_Static *)box->GetChildByIndex(1);
	Assert(shortcutBox);
	if(!shortcutBox)
		return menuItem;

	shortcutBox->SetText(shortcut);

	m_items->AddTail(menuItem);

	if(!textBox->GetTextFont()) {
		textBox->TextReloadFont();
	}

	if(!shortcutBox->GetTextFont()) {
		shortcutBox->TextReloadFont();
	}

	if(icon) {
		Assert(box->NumChildren() > 2);
		ctp2_Static *iconBox = (ctp2_Static *)box->GetChildByIndex(2);
		Assert(iconBox);
		if(iconBox) {
			iconBox->SetImage((MBCHAR *)icon);
			iconBox->ShouldDraw(TRUE);
		}
	}

	bool resizedSomething = false;
	if(shortcutBox && shortcut) {
		sint32 width = shortcutBox->GetTextFont()->GetStringWidth(shortcut);
		if(width + k_EXTRA_SHORTCUT_WIDTH > m_maxShortcutWidth) {
			m_maxShortcutWidth = width + k_EXTRA_SHORTCUT_WIDTH;
			resizedSomething = true;
		}
	}

	Assert(textBox->GetTextFont());
	if(textBox->GetTextFont()) {
		sint32 width = textBox->GetTextFont()->GetStringWidth(text);
		if(width > m_maxTextWidth) {
			m_maxTextWidth = width;
			resizedSomething = true;
		}

		sint32 iconWidth = 0;
		sint32 iconHeight = 0;

		if(box->NumChildren() > 2) {
			ctp2_Static *iconBox = (ctp2_Static *)box->GetChildByIndex(2);
			Assert(iconBox);
			iconWidth = iconBox->GetImage()->TheSurface()->Width() + k_LEFT_ITEM_MARGIN;
			iconHeight = iconBox->GetImage()->TheSurface()->Width();
		}


		if(iconWidth > m_maxIconWidth) {
			m_maxIconWidth = iconWidth;
			resizedSomething = true;
		}

		if(iconHeight > m_maxItemHeight) {
			m_maxItemHeight = iconHeight;
			resizedSomething = true;
		}
	}

	if(item->Height() > m_maxItemHeight) {
		m_maxItemHeight = item->Height();
	}

	if(resizedSomething) {

		PointerList<Item>::Walker walk(m_items);
		while(walk.IsValid()) {
			Reformat(walk.GetObj());
			walk.Next();
		}
	} else {

		Reformat(menuItem);
	}

	return menuItem;
}

void ctp2_Menu::AddItem(const MBCHAR *text, const MBCHAR *shortcut, void *cookie)
{
#ifdef _DEBUG
	ctp2_Menu::Item *item =
#endif
	    CreateItem("PlainMenuListItem", text, shortcut, NULL, cookie);
	Assert(item);
}

void ctp2_Menu::AddItemWithIcon(const MBCHAR *text, const MBCHAR *icon, const MBCHAR *shortcut, void *cookie)
{
#ifdef _DEBUG
	ctp2_Menu::Item *item =
#endif
	    CreateItem("IconMenuListItem", text, shortcut, icon, cookie);

	Assert(item);
}

void ctp2_Menu::Resize()
{

	m_window->Resize(m_maxTextWidth + m_maxIconWidth + m_maxShortcutWidth + k_LEFT_ITEM_MARGIN + k_RIGHT_ITEM_MARGIN + k_EXTRA_WINDOW_WIDTH + k_EXTRA_LEFT_SPACE,
					 m_items->GetCount() * m_maxItemHeight + k_EXTRA_WINDOW_HEIGHT);
	m_list->Resize(m_maxTextWidth + m_maxIconWidth + m_maxShortcutWidth + k_LEFT_ITEM_MARGIN + k_RIGHT_ITEM_MARGIN + k_LIST_WIDTH_DELTA + k_EXTRA_LEFT_SPACE,
				   m_window->Height() + k_LIST_HEIGHT_DELTA);

	PointerList<Item>::Walker walk(m_items);
	m_list->BuildListStart();
	while(walk.IsValid()) {
		m_list->AddItem(walk.GetObj()->m_item);
		walk.Next();
	}
	m_list->BuildListEnd();

	m_resized = true;
}

void ctp2_Menu::Open()
{

	if (m_atMouse)
		m_window->Move(g_c3ui->TheMouse()->X(), g_c3ui->TheMouse()->Y());

	Assert(m_window);

	if(!m_window)
	{

		Init("CTP2_MENU", true, NULL);
		Assert(m_window);
		if(!m_window)
			return;
	}

	if(!m_resized)
		Resize();

	m_window->SetWeaklyModal(TRUE);

	if(m_window->X() + m_window->Width() >= g_ScreenWidth) {
		Move(g_ScreenWidth - m_window->Width(), m_window->Y());
	}

	if(m_window->Y() + m_window->Height() >= g_ScreenHeight) {
		sint32 y = g_ScreenHeight - m_window->Height();
		if(y < 0)
			y = 0;

		Move(m_window->X(), y);
	}

	g_c3ui->AddWindow(m_window);
	m_window->ResetCurrentMouseState();





}

void ctp2_Menu::Close()
{
	if(!m_window) {
		return;
	}

	g_c3ui->RemoveWindow(m_window->Id());
}

void ctp2_Menu::Move(sint32 x, sint32 y)
{
	Assert(m_window);
	if(!m_window)
		return;

	m_window->Move(x, y);
}

void ctp2_Menu::ListCallback(aui_Control *control, uint32 action, uint32 data)
{
	if(action != AUI_LISTBOX_ACTION_SELECT)
		return;

	static bool inCallback = false;
	if(inCallback)
		return;

	inCallback = true;
	Close();

	if(m_callback) {
		ctp2_ListItem *item = (ctp2_ListItem *)m_list->GetSelectedItem();
		if(item) {
			sint32 index = m_list->GetSelectedItemIndex();
			Item *itemInfo = (Item *)item->GetUserData();
			Assert(itemInfo);

			m_list->DeselectItem(item);
			m_callback(this, CTP2_MENU_ACTION_SELECT, index, itemInfo ? itemInfo->m_cookie : NULL);
		} else {
			m_callback(this, CTP2_MENU_ACTION_CANCEL, -1, NULL);
		}
	}


	inCallback = false;
}

void ctp2_Menu::StaticListCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	ctp2_Menu *menu = (ctp2_Menu *)cookie;
	menu->ListCallback(control, action, data);
}

sint32 ctp2_Menu::GetNumItems() const
{
	return m_items->GetCount();
}

const MBCHAR *ctp2_Menu::GetShortcutString(sint32 index)
{
	sint32 i = 0;
	PointerList<Item>::Walker walk(m_items);
	while(walk.IsValid()) {
		if(i == index) {
			return walk.GetObj()->m_shortcut;
		}
		walk.Next();
	}

	return NULL;
}

bool ctp2_Menu::HandleShortcut(const MBCHAR *shortcut)
{

	sint32 index = 0;
	PointerList<Item>::Walker walk(m_items);
	while(walk.IsValid()) {
		if(stricmp(walk.GetObj()->m_shortcut, shortcut) == 0) {
			if(m_callback) {
				m_callback(this, CTP2_MENU_ACTION_SELECT, index, walk.GetObj()->m_cookie);
			}

			return true;
		}
		index++;
		walk.Next();
	}

	return false;
}

void ctp2_Menu::WeaklyModalCancel(aui_MouseEvent *event, ctp2_Window *window, void *cookie, bool &passEventOn)
{
	ctp2_Menu *menu = (ctp2_Menu *)cookie;
	Assert(menu);

	if(menu) {
		if(menu->m_siblingArea) {
			if((event->position.x >= menu->m_siblingArea->X()) &&
			   (event->position.x <= menu->m_siblingArea->X() + menu->m_siblingArea->Width()) &&
			   (event->position.y >= menu->m_siblingArea->Y()) &&
		       (event->position.y <= menu->m_siblingArea->Y() + menu->m_siblingArea->Height())) {
				passEventOn = true;
			}
		}

		if(menu->m_callback) {
			menu->m_callback(menu, CTP2_MENU_ACTION_CANCEL, -1, NULL);

		}
	}

}




void
ctp2_Menu::Clear()
{

	if (m_items!=NULL)
		m_items->DeleteAll();

	if (m_list!=NULL)
		m_list->Clear();

	m_maxTextWidth	= k_MINIMUM_TEXT_WIDTH;
	m_maxIconWidth	= 0;
	m_maxItemHeight = 0;
	m_resized		= false;
}

void ctp2_Menu::ActivateMenu(aui_MouseEvent *mouseData)
{
	m_list->MouseLGrabInside(mouseData);
}

void ctp2_Menu::SetMenuButton(ctp2_MenuButton *butt)
{
	m_list->SetMenuButton(butt);
}
