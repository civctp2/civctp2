//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Background screen handling
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Prevented crash
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"

#include "background.h"

#include "TiledMap.h"
#include "DynArr.h"
#include "SelItem.h"
#include "CursorManager.h"
#include "c3window.h"
#include "c3ui.h"

#include "messageactions.h"
#include "messagewindow.h"
#include "InfoBar.h"
#include "ProfileDB.h"

#include "controlpanelwindow.h"
#include "StatusBar.h"
#include "ScenarioEditor.h"
#include "ctp2_Static.h"
#include "aui_ldl.h"

extern CursorManager	*g_cursorManager;
extern C3UI				*g_c3ui;
extern MessageWindow	*g_currentMessageWindow;

enum SEV_TYPE {
	SEV_LGRAB,
	SEV_RGRAB,
	SEV_LDROP,
	SEV_LDRAG,
	SEV_LDOUBLE
};

struct SavedMouseEvent {
	aui_MouseEvent event;
	SEV_TYPE type;
};



PointerList<SavedMouseEvent> s_savedEvents;

sint32				doubleClickTimeout = 100;


AUI_ERRCODE Background::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	return m_TheDrawHandler ?
		m_TheDrawHandler( this ) : AUI_ERRCODE_NODRAWHANDLER;
}

void Background::MouseLDragOver(aui_MouseEvent *data)
{
	if (IsDisabled()) return;

	sint16 hold;

	hold = 0;
}

void Background::MouseLGrabInside(aui_MouseEvent *data)
{
	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

	Assert(g_tiledMap != NULL);
	if (g_tiledMap == NULL) return;

	
	data->position.x -= X();
	data->position.y -= Y();

	SavedMouseEvent *ev = new SavedMouseEvent;	
	memcpy(&ev->event, data, sizeof(aui_MouseEvent));
	ev->type = SEV_LGRAB;
	s_savedEvents.AddTail(ev);



    m_lbutton_isdown = TRUE; 

	
	
	
	
	
	

	
	
	data->position.x += X();
	data->position.y += Y();
}

void Background::MouseRGrabInside(aui_MouseEvent *data)
{
	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

	

	Assert(g_tiledMap != NULL);
	if(g_tiledMap == NULL) return;

	data->position.x -= X();
	data->position.y -= Y();

	SavedMouseEvent *ev = new SavedMouseEvent;
	memcpy(&ev->event, data, sizeof(aui_MouseEvent));
	ev->type = SEV_RGRAB;
	s_savedEvents.AddTail(ev);

	



	m_rbutton_isdown = TRUE;

	
	
	
	
	
	

	
	
	data->position.x += X();
	data->position.y += Y();
}

void Background::MouseLDropInside(aui_MouseEvent *data)
{
	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

	data->position.x -= X();
	data->position.y -= Y();

	SavedMouseEvent *ev = new SavedMouseEvent;
	memcpy(&ev->event, data, sizeof(aui_MouseEvent));
	ev->type = SEV_LDROP;
	s_savedEvents.AddTail(ev);

	

    m_lbutton_isdown = FALSE;

	
	
	data->position.x += X();
	data->position.y += Y();
}



void Background::MouseLDropOutside(aui_MouseEvent *data)
{
	if (IsDisabled()) return;

    m_lbutton_isdown = FALSE; 

}


extern SelectedItem *g_selected_item; 


void Background::MouseMoveInside(aui_MouseEvent *data)

{
	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

    Assert(data); 

    MapPoint tmp;

    if (g_tiledMap && g_tiledMap->GetMouseTilePos(tmp)){
        m_current_mouse_tile = tmp; 
    }

    if (data->movecount==0) { 
        ProcessLastMouseMoveThisFrame(data);
    }

	if (g_tiledMap != NULL) {
		g_tiledMap->SetHiliteMouseTile(m_current_mouse_tile);
		g_tiledMap->DrawHilite( TRUE );
	}
}

void Background::MouseLDragInside( aui_MouseEvent *data )
{
	if ( IsDisabled() ) return;
	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );

	MapPoint tmp;
	
	data->position.x -= X();
	data->position.y -= Y();

    if (g_tiledMap && g_tiledMap->GetMouseTilePos(tmp)){
		if (m_current_mouse_tile != tmp) {
			SavedMouseEvent *ev = new SavedMouseEvent;
			memcpy(&ev->event, data, sizeof(aui_MouseEvent));
			ev->type = SEV_LDRAG;
			s_savedEvents.AddTail(ev);

			
			
		}
        m_current_mouse_tile = tmp; 
    }

    if (data->movecount==0) { 
        ProcessLastMouseMoveThisFrame(data);
    }

	if (g_tiledMap != NULL) {
		g_tiledMap->SetHiliteMouseTile(m_current_mouse_tile);
		g_tiledMap->DrawHilite( TRUE );
	}

	
	
	data->position.x += X();
	data->position.y += Y();
}

void Background::MouseMoveOver(aui_MouseEvent *data)

{
	if (IsDisabled()) return;

    Assert(data); 


    MapPoint tmp;
	if (g_tiledMap && g_tiledMap->GetMouseTilePos(tmp))
	{
        m_current_mouse_tile = tmp; 
    }

    if (data->movecount==0) { 
        ProcessLastMouseMoveThisFrame(data);
    }

	g_cursorManager->RestoreCursor();
}

void Background::MouseMoveOutside(aui_MouseEvent *data)

{
	if (IsDisabled()) return;

    Assert(data); 

    if (data->movecount==0) { 
        ProcessLastMouseMoveThisFrame(data);
    }






	m_lbutton_isdown = FALSE;






}



void Background::MouseMoveAway(aui_MouseEvent *data)

{
	if (IsDisabled()) return;

    Assert(data); 

    if (data->movecount==0) { 
        ProcessLastMouseMoveThisFrame(data);
    }

	g_cursorManager->SaveCursor();
	g_cursorManager->SetCursor(CURSORINDEX_DEFAULT);

	m_lbutton_isdown = FALSE;
}

void Background::MouseNoChange(aui_MouseEvent *data)

{
	if (IsDisabled()) return;

    Assert(data); 

    if (data->movecount==0) { 
        ProcessLastMouseMoveThisFrame(data);
    }

}


void Background::ProcessLastMouseMoveThisFrame(aui_MouseEvent *data)

{
    Assert(data); 
    MapPoint cur, old; 

    if ((m_lbutton_isdown == FALSE || g_theProfileDB->IsUseCTP2Mode()) && 
		(g_selected_item->IsLocalArmy() ||
		 g_selected_item->GetState() == SELECT_TYPE_LOCAL_ARMY_UNLOADING)) { 

		g_selected_item->GetOldMouseTilePos(old); 

        if ((m_current_mouse_tile.x != -1) && (m_current_mouse_tile != old)) { 
			g_selected_item->SetDrawablePathDest(m_current_mouse_tile); 
			g_infoBar->SetTextFromMap(m_current_mouse_tile);

			
			
			if(!GetWhichSeesMouse() || GetWhichSeesMouse() == this)
				StatusBar::SetText(g_infoBar->GetText());
				
				if(ScenarioEditor::IsShown()) {
					sint32 x,y;
					char lemurpoo[_MAX_PATH];
					x = m_current_mouse_tile.x;
					y = m_current_mouse_tile.y;
					sprintf(lemurpoo, "x: %d, y: %d", x, y);
					ctp2_Static *tf = (ctp2_Static *)aui_Ldl::GetObject("ScenarioEditor.WorldControls.PosField");
					tf->SetText(lemurpoo);

				}
		}
	} else {
		g_selected_item->GetOldMouseTilePos(old);
		if(m_current_mouse_tile.x != -1 && m_current_mouse_tile != old) {
			g_selected_item->SetCurMouseTile(m_current_mouse_tile);
			g_infoBar->SetTextFromMap(m_current_mouse_tile);

			
			
			if(!GetWhichSeesMouse() || GetWhichSeesMouse() == this)
				StatusBar::SetText(g_infoBar->GetText());
			
			if(!ScenarioEditor::IsShown()) {
				return;
			} else {
				sint32 x,y;
				char lemurpoo[_MAX_PATH];
				x = m_current_mouse_tile.x;
				y = m_current_mouse_tile.y;
				sprintf(lemurpoo, "x: %d, y: %d", x, y);
				ctp2_Static *tf = (ctp2_Static *)aui_Ldl::GetObject("ScenarioEditor.WorldControls.PosField");
				tf->SetText(lemurpoo);
			}
		}
	}
}

AUI_ERRCODE Background::Idle(void)
{
	if(g_tiledMap) {
		g_tiledMap->Idle();
	}

	
	while(s_savedEvents.GetCount() > 0) {
		SavedMouseEvent *ev = s_savedEvents.GetHead();
		switch(ev->type) {
			case SEV_LGRAB:
			{
				uint32 curTicks = GetTickCount();
				if (curTicks > (ev->event.time + doubleClickTimeout)) {
					g_tiledMap->Click(&ev->event, FALSE);					
				} else {
					return AUI_ERRCODE_OK;
				}
				break;
			}
			case SEV_RGRAB:
				g_tiledMap->Click(&ev->event, FALSE);
				break;
			case SEV_LDROP:
				g_tiledMap->Drop(&ev->event);
				break;
			case SEV_LDRAG:
				g_tiledMap->MouseDrag(&ev->event);
				break;
			case SEV_LDOUBLE:
				g_tiledMap->Click(&ev->event, TRUE);
				break;
		}
		delete s_savedEvents.RemoveHead();
	}
				
#if 0
	if (hasSavedEvent) {
		uint32 curTicks = GetTickCount();
		if (curTicks > (savedEvent.time + doubleClickTimeout)) {
			g_tiledMap->Click(&savedEvent, FALSE);
			hasSavedEvent = FALSE;
		}
	}
#endif
	return AUI_ERRCODE_OK;
}


void Background::MouseLDoubleClickInside(aui_MouseEvent *data)
{
	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

	Assert(g_tiledMap != NULL);
	if (g_tiledMap == NULL) return;

	
	data->position.x -= X();
	data->position.y -= Y();

	if (s_savedEvents.GetCount() > 0) {
		SavedMouseEvent *ev = s_savedEvents.GetTail();
		if(ev->type == SEV_LGRAB) {
			ev->type = SEV_LDOUBLE;
			memcpy(&ev->event, data, sizeof(aui_MouseEvent));
		} else {
			ev = new SavedMouseEvent;
			memcpy(&ev->event, data, sizeof(aui_MouseEvent));
			ev->type = SEV_LDOUBLE;
			s_savedEvents.AddTail(ev);
		}
	} else {
		g_tiledMap->Click(data, TRUE);
	}

    m_lbutton_isdown = TRUE; 

}

void Background::MouseRDoubleClickInside(aui_MouseEvent *data)
{
	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

	Assert(g_tiledMap != NULL);
	if (g_tiledMap == NULL) return;

	
	data->position.x -= X();
	data->position.y -= Y();

	
	g_tiledMap->Click(data, TRUE);

    m_lbutton_isdown = TRUE; 

}
