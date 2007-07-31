#include "c3.h"
#include "aui.h"
#include "aui_uniqueid.h"
#include "ctp2_MenuButton.h"
#include "ctp2_Menu.h"

#include "GameSounds.h"
#include "SoundManager.h"

extern SoundManager		*g_soundManager;

uint32 ctp2_MenuButton::m_menuButtonClassId = aui_UniqueId();

ctp2_MenuButton::ctp2_MenuButton(AUI_ERRCODE *retval,
								 uint32 id,
								 MBCHAR *ldlBlock,
								 ControlActionCallback *ActionFunc,
								 void *cookie)
	: 	
	c3_Button( retval, id, ldlBlock, ActionFunc, cookie ),
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	m_menu(NULL),
	m_leftNeighbor(NULL),
	m_rightNeighbor(NULL)
{	
	Assert(AUI_SUCCESS(*retval));
	if(!AUI_SUCCESS(*retval)) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


ctp2_MenuButton::~ctp2_MenuButton()
{
	
	
	
	
	delete m_menu;
	m_menu = NULL;
}

void ctp2_MenuButton::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	aui_Button::MouseLGrabInside(mouseData);
}

void ctp2_MenuButton::MouseLDragAway( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	
	if ( GetMouseOwnership() == this)
	{
		PlaySound( AUI_SOUNDBASE_SOUND_DISENGAGE );

		
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		m_attributes &= ~k_CONTROL_ATTRIBUTE_DOWN;
		m_attributes &= ~k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDRAGAWAY;

		
		m_isRepeating = FALSE;
		if ( m_ActionFunc )
			m_ActionFunc( this, AUI_BUTTON_ACTION_RELEASE, 0, m_cookie );
		else if ( m_action )
			m_action->Execute( this, AUI_BUTTON_ACTION_RELEASE, 0 );

		if(mouseData->position.y >= Y() + Height()) {
			SetWhichSeesMouse(NULL);
			mouseData->position.y -= Height();
			m_menu->ActivateMenu(mouseData);
		} else if(mouseData->position.x >= X() + Width()) {
			if(m_rightNeighbor) {
				m_rightNeighbor->MouseLGrabInside(mouseData);
			}
			m_menu->Close();
		} else if(mouseData->position.x < X()) {
			if(m_leftNeighbor) {
				m_leftNeighbor->MouseLGrabInside(mouseData);
			}
			m_menu->Close();
		}
	}
}
