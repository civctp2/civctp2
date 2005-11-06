//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : 
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Made AddMenuItem itemID parameter const.
//
//----------------------------------------------------------------------------


#include "c3.h"

#include "aui.h"
#include "aui_control.h"
#include "aui_ldl.h"
#include "aui_ui.h"
#include "pattern.h"

#include "primitives.h"

#include "aui_button.h"
#include "ctp2_menubar.h"

#include "aui_action.h"
#include "ctp2_MenuButton.h"




extern sint32 g_ScreenWidth;
extern sint32 g_ScreenHeight;

extern aui_UI *g_ui;


void DefaultCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
}

class OpenMenuAction : public aui_Action
{
public:
	OpenMenuAction(ctp2_Menu * menu) 
    :   aui_Action  (),
        m_menu      (menu) 
    { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	)
    {   
        if (m_menu)
        {
	        m_menu->Open();
        }
    };

protected:
	ctp2_Menu *     m_menu;
};


static void ButtonCallback	(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	
	if(action != (uint32)AUI_BUTTON_ACTION_PRESS) return;
	if(data > 0) return; 

	
	if (cookie==NULL)
		return;

	ctp2_Menu *menu = (ctp2_Menu *)cookie;

	if (control!=NULL) {
		menu->Move(control->X(),control->Y()+control->Height());
		
	}

	g_ui->AddAction(new OpenMenuAction(menu));
}



ctp2_MenuBar::ctp2_MenuBar(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	AUI_WINDOW_TYPE type,
	bool bevel)
	:
	aui_Window( retval, id, ldlBlock, bpp, type ),
	PatternBase( ldlBlock, NULL )
{
	m_bevel = bevel;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

}



ctp2_MenuBar::ctp2_MenuBar(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *pattern,
	AUI_WINDOW_TYPE type,
	bool bevel)
	:
	aui_Window( retval, id, x, y, width, height, bpp, type ),
	PatternBase( pattern )
{
	m_bevel = bevel;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE ctp2_MenuBar::InitCommon( void )
{
	m_runningWidth=0;
	m_largestItemHeight=0;
	m_rightRunningWidth = 0;

	Move(0,0);
	Resize(g_ScreenWidth,Height());
	
	GrabRegion()->Move( 0, 0 );
	GrabRegion()->Resize( m_width, m_height );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE	
ctp2_MenuBar::AddChild(aui_Region *in_child)
{
	
	aui_Control *child=(aui_Control *)in_child;

	if (child==NULL)
		return AUI_ERRCODE_OK;

	aui_Window::AddChild(child);

	sint32 width =child->Width();
	sint32 height=child->Height();

	if (m_largestItemHeight<height)
	{
		m_largestItemHeight=height;
		Resize(Width(),m_largestItemHeight);
	}
		
	if(child->IsThisA(ctp2_MenuButton::m_menuButtonClassId)) {

		child->Move(m_runningWidth,0);
		
		m_runningWidth += width;
		
		
		ctp2_Menu *menu=new ctp2_Menu(false,DefaultCallback);
		menu->SetSiblingArea(this);
		
		
		child->SetActionFuncAndCookie (ButtonCallback,(void *)menu);
		
		ctp2_MenuButton *mbutt = (ctp2_MenuButton *)child;
		mbutt->SetMenu(menu);
		menu->SetMenuButton(mbutt);
	} else {
		
		child->Move(Width() - m_rightRunningWidth - width, (Height() - height) / 2);
		m_rightRunningWidth += width;
	}
		
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE ctp2_MenuBar::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	RECT rect = { 0, 0, m_width, m_height };

	if (m_pattern)
		m_pattern->Draw( m_surface, &rect );

	
	if(m_bevel)
		primitives_BevelRect16( m_surface, &rect, 3, 0, 16, 16 );

	
	m_dirtyList->AddRect( &rect );

	return AUI_ERRCODE_OK;
}


ctp2_Menu *
ctp2_MenuBar::GetMenu(MBCHAR *ldlParent,MBCHAR *menuname)
{
	aui_Control *button=(aui_Control *)aui_Ldl::GetObject(ldlParent,menuname);

	if (button==NULL)
		return NULL;

	return (ctp2_Menu*)button->GetCookie();
}

void			
ctp2_MenuBar::SetMenuCallback(ctp2_Menu *menu,CTP2MenuCallback *callback)
{
	
	if (menu==NULL)
		return;

	menu->SetCallback(callback);
}


void ctp2_MenuBar::AddMenuItem
(
    ctp2_Menu *     menu,
    MBCHAR const *  itemID,
    MBCHAR const *  shortcut,
    void *          cookie
)
{
	if (menu)
    {
        menu->AddItem(itemID, shortcut, cookie);
    }
}


void ctp2_MenuBar::BuildNeighbors()
{
	sint32 i;
	ctp2_MenuButton *lbutt, *mbutt, *rbutt;
	for(i = 0; i < NumChildren() - 1; i++) {
		
		lbutt = (ctp2_MenuButton *)GetChildByIndex(i);
		mbutt = (ctp2_MenuButton *)GetChildByIndex(i + 1);

		lbutt->SetRightNeighbor(mbutt);
		mbutt->SetLeftNeighbor(lbutt);

		if(i + 2 < NumChildren()) {
			rbutt = (ctp2_MenuButton *)GetChildByIndex(i + 2);
			mbutt->SetRightNeighbor(rbutt);
			rbutt->SetLeftNeighbor(mbutt);
		}
	}
}
