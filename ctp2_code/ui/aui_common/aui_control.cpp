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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Standardised min/max usage.
//
//----------------------------------------------------------------------------

#include "c3.h"


#include <string>
#include <strstream>


#include "aui_ui.h"
#include "aui_blitter.h"
#include "aui_uniqueid.h"
#include "aui_tipwindow.h"
#include "aui_surface.h"
#include "aui_ldl.h"
#include "aui_action.h"
#include "aui_stringtable.h"
#include "aui_control.h"
#include "StrDB.h"
#include "StatusBar.h"
#include "aui_ImageList.h"
#include "aui_Static.h"


extern StringDB		*g_theStringDB;


static const MBCHAR *k_AUI_CONTROL_NUMBER_OF_LAYERS		= "numberoflayers";
static const MBCHAR *k_AUI_CONTROL_IMAGES_PER_LAYER		= "imagesperlayer";
static const MBCHAR *k_AUI_CONTROL_LDL_LAYER_ALWAYS		= "layeralways";
static const MBCHAR *k_AUI_CONTROL_LDL_LAYER_HIGHLIGHT	= "layerhighlight";
static const MBCHAR *k_AUI_CONTROL_LDL_LAYER_DISABLED	= "layerdisabled";
static const MBCHAR *k_AUI_CONTROL_LDL_LAYER_ENABLED	= "layerenabled";
static const MBCHAR *k_AUI_CONTROL_LDL_IMAGE			= "image";
static const MBCHAR *k_AUI_CONTROL_IMAGE_STRETCH_X		= "imagestretchx";
static const MBCHAR *k_AUI_CONTROL_IMAGE_STRETCH_Y		= "imagestretchy";
static const MBCHAR *k_AUI_CONTROL_IMAGE_NEXT_ROW		= "imagenextrow";

static const MBCHAR *k_AUI_CONTROL_SHORTCUT             = "shortcut";
static const MBCHAR *k_AUI_CONTROL_CANFOCUS             = "focusindex";


const sint32 aui_Control::k_AUI_CONTROL_LAYER_FLAG_ALWAYS		= 1;
const sint32 aui_Control::k_AUI_CONTROL_LAYER_FLAG_HIGHLIGHT	= 2;
const sint32 aui_Control::k_AUI_CONTROL_LAYER_FLAG_DISABLED		= 4;
const sint32 aui_Control::k_AUI_CONTROL_LAYER_FLAG_ENABLED		= 8;


aui_Control *aui_Control::m_whichOwnsMouse = NULL;	
aui_Control *aui_Control::m_whichHasFocus = NULL;	
uint32 aui_Control::m_controlClassId = aui_UniqueId();


static const MBCHAR *const k_AUI_CONTROL_LDL_STATUS_TEXT	=	"statustext";


aui_Control::aui_Control(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_Region( retval, id, ldlBlock ),
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_SoundBase( ldlBlock ),
	m_statusText(NULL),
	m_numberOfLayers(0), m_imagesPerLayer(0),
	m_imageLayerList(NULL),
	m_layerRenderFlags(NULL),
	m_renderFlags(k_AUI_CONTROL_LAYER_FLAG_ALWAYS)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock, ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_Control::aui_Control(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_Region( retval, id, x, y, width, height ),
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_SoundBase( (MBCHAR **)NULL ),
	m_statusText(NULL),
	m_numberOfLayers(0), m_imagesPerLayer(0),
	m_imageLayerList(NULL),
	m_layerRenderFlags(NULL),
	m_renderFlags(k_AUI_CONTROL_LAYER_FLAG_ALWAYS)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_Control::InitCommonLdl(
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
{
	
	m_statusText= NULL;
	m_numberOfLayers = 0;
	m_imagesPerLayer = 0;
	m_imageLayerList = NULL;
	m_layerRenderFlags = NULL;
	m_renderFlags = k_AUI_CONTROL_LAYER_FLAG_ALWAYS;

	aui_Ldl *theLdl = g_ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	AUI_ERRCODE errcode = InitCommon( ActionFunc, cookie );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	
	MBCHAR *tip = block->GetString(k_AUI_CONTROL_LDL_TIPWINDOW);
	if (tip) {
		m_tip = new aui_TipWindow(&errcode, aui_UniqueId(), "DefaultTipWindow");
		Assert( AUI_NEWOK(m_tip,errcode) );
		if ( !AUI_NEWOK(m_tip,errcode) ) return errcode;

		
		aui_Ldl::Remove( m_tip );

		m_allocatedTip = TRUE;

		StringId	id;
		if (g_theStringDB->GetStringID(tip, id)) {
			const MBCHAR *text = g_theStringDB->GetNameStr(id);
			((aui_TipWindow*)m_tip)->SetTipText((MBCHAR *)text);
		}

		SetTipWindow( m_tip );
	}

	MBCHAR *shortcut = block->GetString(k_AUI_CONTROL_SHORTCUT);
	m_keyboardAction = 0; 
	if(shortcut) {
		if(shortcut[0] == '^') {
			if(shortcut[1] >= '@' && shortcut[1] <= '[')
				m_actionKey = shortcut[0] - '@';
			else if(shortcut[1] >= 'a' && shortcut[1] <= 'z')
				m_actionKey = shortcut[1] - 'a' + 1;
			else
				m_actionKey = '^';
		} else if(stricmp(shortcut, "ESC") == 0) {
			m_actionKey = VK_ESCAPE;
		} else {
			m_actionKey = shortcut[0];
		}
	}

	
	m_statusText = block->GetString(k_AUI_CONTROL_LDL_STATUS_TEXT);
	StringId statusTextID = 0;
	if(m_statusText && g_theStringDB->GetStringID(m_statusText, statusTextID)) {
		m_statusText = g_theStringDB->GetNameStr(statusTextID);
	}

	
	static MBCHAR stblock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( stblock, "%s.%s", ldlBlock, k_AUI_CONTROL_LDL_STRINGTABLE );
	ldl_datablock *ldlblock = theLdl->GetLdl()->FindDataBlock( stblock );
	if ( ldlblock )
	{
		m_stringTable = new aui_StringTable( &errcode, stblock );
		Assert( AUI_NEWOK(m_stringTable,errcode) );
		if ( !AUI_NEWOK(m_stringTable,errcode) )
			return AUI_ERRCODE_MEMALLOCFAILED;
	}
	else
		m_stringTable = NULL;

	
	InitializeImageLayers(block);

	if(block->GetAttributeType(k_AUI_CONTROL_CANFOCUS) != ATTRIBUTE_TYPE_INT)
		m_focusIndex = -1;
	else
		m_focusIndex = block->GetInt(k_AUI_CONTROL_CANFOCUS);

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Control::InitCommon(
	ControlActionCallback *ActionFunc,
	void *cookie )
{
	m_stringTable = NULL;

	m_window = NULL,
	m_allocatedTip = FALSE,
	m_tip = NULL,
	m_showingTip = FALSE,
	m_startWaitTime = 0,
	m_timeOut = k_CONTROL_DEFAULT_TIMEOUT,
	m_repeatTime = k_CONTROL_DEFAULT_REPEATTIME,
	m_lastRepeatTime = 0,
	m_ActionFunc = ActionFunc,
	m_cookie = cookie;
	m_focusIndex = -1;

	memset( &m_keyboardEvent, 0, sizeof( m_keyboardEvent ) );
	memset( &m_joystickEvent, 0, sizeof( m_joystickEvent ) );

	return AUI_ERRCODE_OK;
}



aui_Control::~aui_Control()
{
	if ( m_stringTable )
	{
		delete m_stringTable;
		m_stringTable = NULL;
	}

	
	ReleaseKeyboardFocus();

	if ( m_allocatedTip && m_tip )
	{
		delete m_tip;
		m_tip = NULL;
		m_allocatedTip = FALSE;
	}
	
	
	delete m_imageLayerList;
	m_imageLayerList = NULL;
	delete [] m_layerRenderFlags;
	m_layerRenderFlags = NULL;
}



AUI_ERRCODE aui_Control::SetActionFuncAndCookie(
	ControlActionCallback *ActionFunc,
	void *cookie )
{
	m_ActionFunc = ActionFunc;

	
	m_cookie = cookie;

	return AUI_ERRCODE_OK;
}



aui_Action *aui_Control::SetAction( aui_Action *action )
{
	
	m_ActionFunc = NULL;

	
	aui_Action *prevAction = m_action;
	m_action = action;
	return prevAction;
}



AUI_ERRCODE aui_Control::SetParentWindow( aui_Window *window )
{
	Assert( m_childList != NULL );
	ListPos position = m_childList->GetHeadPosition();
	if ( position )
	{
		for ( sint32 i = m_childList->L(); i; i-- )
		{
			aui_Control *subControl =
				(aui_Control *)m_childList->GetNext( position );
			Assert( subControl != NULL );
			subControl->SetParentWindow( window );
		}
	}

	m_window = window;
	if(m_focusIndex >= 0 && m_window)
		m_window->AddFocusControl(this);

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Control::ResetThis( void )
{
	if ( GetMouseOwnership() == this ) ReleaseMouseOwnership();
	if ( GetKeyboardFocus() == this ) ReleaseKeyboardFocus();

	
	ResetCurrentRenderFlags();

	return aui_Region::ResetThis();
}



AUI_ERRCODE aui_Control::AddChild( aui_Region *child )
{
	
	Assert( child != NULL );
	if ( !child ) return AUI_ERRCODE_INVALIDPARAM;








	AUI_ERRCODE errcode = aui_Region::AddChild( child );
	if ( errcode == AUI_ERRCODE_OK )
	{
		aui_Control *control = (aui_Control *)child;

		control->SetParentWindow( m_window );
	}

	return errcode;
}



AUI_ERRCODE aui_Control::InsertChild( aui_Region *child, sint32 index )
{
	AUI_ERRCODE errcode = aui_Region::InsertChild( child, index );
	if ( errcode == AUI_ERRCODE_OK )
	{
		aui_Control *control = (aui_Control *)child;

		control->SetParentWindow( m_window );
	}

	return errcode;
}



AUI_ERRCODE aui_Control::RemoveChild( uint32 controlId )
{
	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		ListPos prevPos = position;
		aui_Control *control = (aui_Control *)m_childList->GetNext( position );
		if ( control->Id() == controlId )
		{
			
			control->ReleaseKeyboardFocus();

			control->SetParent( NULL );

			
			m_childList->DeleteAt( prevPos );

			m_childListChanged = TRUE;

			break;
		}
	}

	
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Control::ToWindow( RECT *rect )
{
	
	
	
	
	
	
	
	if (!m_parent) return AUI_ERRCODE_OK;
	
	

	if ( m_parent != (aui_Region *)m_window )
	{
        Assert(m_parent); 
		OffsetRect( rect, m_parent->X(), m_parent->Y() );
		((aui_Control *)m_parent)->ToWindow( rect );
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Control::ToWindow( POINT *point )
{
	if ( m_parent != (aui_Region *)m_window )
	{
		point->x += m_parent->X();
		point->y += m_parent->Y();
		((aui_Control *)m_parent)->ToWindow( point );
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Control::ToScreen( RECT *rect )
{
	ToWindow( rect );
	OffsetRect( rect, m_window->X(), m_window->Y() );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Control::ToScreen( POINT *point )
{
	ToWindow( point );
	point->x += m_window->X();
	point->y += m_window->Y();

	return AUI_ERRCODE_OK;
}



BOOL aui_Control::ShowTipWindow( aui_MouseEvent *mouseData )
{
	if ( m_tip && !m_showingTip )
	{
		aui_Static *tipStatic = ((aui_TipWindow *)m_tip)->GetStatic();

		
		if(tipStatic && (!tipStatic->GetText() || tipStatic->GetText()[0] == 0))
			return FALSE;

		PlaySound( AUI_SOUNDBASE_SOUND_TIP );

		
		m_showingTip = g_ui->AddWindow( m_tip ) == AUI_ERRCODE_OK;

		RECT rect = { mouseData->position.x, m_y, 0, 0 };
		ToWindow( &rect );

		
		m_tip->Move(
			m_window->X() + rect.left - m_tip->Width() / 2,
			m_window->Y() + rect.top - m_tip->Height() );

		
		if ( m_tip->X() < 0 ) m_tip->Move( 0, m_tip->Y() );
		if ( m_tip->Y() < 0 ) m_tip->Move( m_tip->X(), 0 );
		sint32 adjustX = m_tip->X() + m_tip->Width() - g_ui->Width();
		sint32 adjustY = m_tip->Y() + m_tip->Height() - g_ui->Height();
		if ( adjustX > 0 ) m_tip->Offset( -adjustX, 0 );
		if ( adjustY > 0 ) m_tip->Offset( -adjustY, 0 );

		return m_showingTip;
	}

	return FALSE;
}



aui_Window *aui_Control::SetTipWindow( aui_Window *window )
{
	aui_Window *prevTip = m_tip;

	m_tip = window;

	if (window != NULL)
		m_allocatedTip = TRUE;
	
	return prevTip;
}



BOOL aui_Control::HideTipWindow( void )
{
	if ( m_tip && m_showingTip )
	{
		
		m_showingTip = !(g_ui->RemoveWindow( m_tip->Id() ) == AUI_ERRCODE_OK);

		return !m_showingTip;
	}

	return FALSE;
}

	
AUI_ERRCODE aui_Control::ShowThis()
{
	
	if(IsHidden()) {
		
		ResetCurrentRenderFlags();
	}

	
	return aui_Region::ShowThis();
}


AUI_ERRCODE aui_Control::HideThis( void )
{
	
	HideTipWindow();

	return aui_Region::HideThis();
}



aui_Control *aui_Control::SetMouseOwnership( void )
{
	aui_Control *prevOwner = GetMouseOwnership();
	if ( prevOwner ) prevOwner->ReleaseMouseOwnership();
	m_whichOwnsMouse = this;

#if		FALSE
		
#ifdef	_DEBUG
		
		
#define STATUS_BAR_MOUSE_OVER_LDL_DEBUG_INFORMATION
#ifdef	STATUS_BAR_MOUSE_OVER_LDL_DEBUG_INFORMATION
		StatusBar::SetText(aui_Ldl::GetBlock(GetMouseOwnership()));
#endif	
#endif	
#endif

	return prevOwner;
}



AUI_ERRCODE aui_Control::ReleaseMouseOwnership( void )
{
	if ( GetMouseOwnership() == this )
		m_whichOwnsMouse = 0;
	else
		return AUI_ERRCODE_NOCONTROL;

	return AUI_ERRCODE_OK;
}



aui_Control *aui_Control::SetKeyboardFocus( void )
{
	aui_Control *prevFocus = GetKeyboardFocus();
	if ( prevFocus ) prevFocus->ReleaseKeyboardFocus();

	if ( !IsDisabled() )
	{
		m_whichHasFocus = this;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_KEYBOARDFOCUSCHANGE;
	}

	return prevFocus;
}



AUI_ERRCODE aui_Control::ReleaseKeyboardFocus( void )
{
	if ( GetKeyboardFocus() == this )
	{
		SetFocus( g_ui->TheHWND() );
		m_whichHasFocus = NULL;
	}
	else
		return AUI_ERRCODE_NOCONTROL;

	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_KEYBOARDFOCUSCHANGE;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Control::Draw( aui_Surface *surface, sint32 x, sint32 y )
{
	if (!surface && m_window) surface = m_window->TheSurface();

	return surface ?
		aui_Region::Draw( surface, x, y ) :
		AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Control::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	if(m_numberOfLayers > 0)	
		DrawLayers(surface, &rect);
	else						
		DrawThisStateImage(0, surface, &rect);

	DrawThisText(
		surface,
		&rect );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Control::DrawThisStateImage(
	sint32 state,
	aui_Surface *destSurf,
	RECT *destRect )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_NOIMAGE;

	BOOL hasFocus = aui_Control::GetKeyboardFocus() == this;

	if ( IsActive() )
	{
		if ( hasFocus )
		{
			if ( GetImage(
				state,
				AUI_IMAGEBASE_SUBSTATE_STATEACTIVEFOCUS ) )
				errcode = DrawImage(
					destSurf,
					destRect,
					state,
					AUI_IMAGEBASE_SUBSTATE_STATEACTIVEFOCUS );
			else if ( GetImage(
				state,
				AUI_IMAGEBASE_SUBSTATE_STATEFOCUS ) )
				errcode = DrawImage(
					destSurf,
					destRect,
					state,
					AUI_IMAGEBASE_SUBSTATE_STATEFOCUS );
		}
		if ( GetImage(
			state,
			AUI_IMAGEBASE_SUBSTATE_STATEACTIVE ) )
			errcode = DrawImage(
				destSurf,
				destRect,
				state,
				AUI_IMAGEBASE_SUBSTATE_STATEACTIVE );
		else if ( GetImage(
			state,
			AUI_IMAGEBASE_SUBSTATE_STATE ) )
			errcode = DrawImage(
				destSurf,
				destRect,
				state,
				AUI_IMAGEBASE_SUBSTATE_STATE );
	}
	else if ( IsDisabled() )
	{
		if ( GetImage(
			state,
			AUI_IMAGEBASE_SUBSTATE_STATEDISABLED ) )
			errcode = DrawImage(
				destSurf,
				destRect,
				state,
				AUI_IMAGEBASE_SUBSTATE_STATEDISABLED );
		else if ( GetImage(
			state,
			AUI_IMAGEBASE_SUBSTATE_STATE ) )
			errcode = DrawImage(
				destSurf,
				destRect,
				state,
				AUI_IMAGEBASE_SUBSTATE_STATE );
	}
	else if ( hasFocus )
	{
		if ( GetImage(
			state,
			AUI_IMAGEBASE_SUBSTATE_STATEFOCUS ) )
			errcode = DrawImage(
				destSurf,
				destRect,
				state,
				AUI_IMAGEBASE_SUBSTATE_STATEFOCUS );
		else if ( GetImage(
			state,
			AUI_IMAGEBASE_SUBSTATE_STATE ) )
			errcode = DrawImage(
				destSurf,
				destRect,
				state,
				AUI_IMAGEBASE_SUBSTATE_STATE );
	}
	else if ( GetImage(
		state,
		AUI_IMAGEBASE_SUBSTATE_STATE ) )
		errcode = DrawImage(
			destSurf,
			destRect,
			state,
			AUI_IMAGEBASE_SUBSTATE_STATE );

	
	if ( m_stringTable )
	{
		
		if ( state < 0 )
			state = 0;
		if ( state > m_stringTable->GetNumStrings() )
			state = m_stringTable->GetNumStrings() - 1;

		if ( state >= 0 )
		{
			aui_TextBase::SetText(
				m_stringTable->GetString( state ),
				m_maxLength );
		}
		else
		{
			aui_TextBase::SetText( "" );
		}
	}

	return errcode;
}



AUI_ERRCODE aui_Control::HandleKeyboardEvent( aui_KeyboardEvent *input )
{
	if ( IsHidden() || IsDisabled() || IgnoringEvents() )
		return AUI_ERRCODE_UNHANDLED;

	
	KeyboardCallback( input );

	
	memcpy( &m_keyboardEvent, input, sizeof( aui_KeyboardEvent ) );

	if ( m_draw ) Draw();

	return AUI_ERRCODE_HANDLED;
}



AUI_ERRCODE aui_Control::HandleJoystickEvent( aui_JoystickEvent *input )
{
	if ( IsHidden() || IsDisabled() || IgnoringEvents() )
		return AUI_ERRCODE_UNHANDLED;

	
	JoystickCallback( input );

	
	memcpy( &m_joystickEvent, input, sizeof( aui_JoystickEvent ) );

	if ( m_draw ) Draw();

	return AUI_ERRCODE_HANDLED;
}








void aui_Control::MouseMoveInside( aui_MouseEvent *mouseData )
{
	
	if(IsDisabled())
		return;

	if ( GetWhichSeesMouse() && GetWhichSeesMouse() != this )
		MouseMoveAway( mouseData );
	else if ( !IsActive() )
		MouseMoveOver( mouseData );
}



void aui_Control::MouseMoveOver( aui_MouseEvent *mouseData )
{
	if(IsDisabled())
		return;

	if(!GetWhichSeesMouse() || GetWhichSeesMouse() == this) {
		SetWhichSeesMouse( this );

		
		if(GetWhichSeesMouse() == this) {
			
			Highlight();

			PlaySound( AUI_SOUNDBASE_SOUND_ACTIVATE );

			
			if(m_statusText) {
				StatusBar::SetText(m_statusText, this);
			}

			
			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;

			
			m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSEMOVEOVER;

			if ( m_ActionFunc )
				m_ActionFunc( this, 0, 0, m_cookie );
			else if ( m_action )
				m_action->Execute( this, 0, 0 );
		}
	} else MouseMoveOutside( mouseData );
}



void aui_Control::MouseMoveAway( aui_MouseEvent *mouseData )
{
	
	HideTipWindow();

	if(IsDisabled())
		return;

	
	Highlight(false);

	if(IsActive()) {
		PlaySound( AUI_SOUNDBASE_SOUND_DEACTIVATE );

		if(m_statusText && StatusBar::GetOwner() == this) {
			StatusBar::SetText("", NULL);
		}

		
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		
		m_attributes &= ~k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSEMOVEAWAY;

		if ( m_ActionFunc )
			m_ActionFunc( this, 0, 0, m_cookie );
		else if ( m_action )
			m_action->Execute( this, 0, 0 );
	}
}



void aui_Control::MouseLDragInside( aui_MouseEvent *mouseData )
{
	
	if(IsDisabled())
		return;

	if ( GetWhichSeesMouse() && GetWhichSeesMouse() != this )
		MouseLDragAway( mouseData );
	else if ( !IsActive() )
		MouseLDragOver( mouseData );
}



void aui_Control::MouseRDragInside( aui_MouseEvent *mouseData )
{
	
	if(IsDisabled())
		return;

	if ( GetWhichSeesMouse() && GetWhichSeesMouse() != this )
		MouseRDragAway( mouseData );
	else if ( !IsActive() )
		MouseRDragOver( mouseData );
}



void aui_Control::MouseLDragOver( aui_MouseEvent *mouseData )
{
	if(GetMouseOwnership() == this)
		Highlight();	
}



void aui_Control::MouseLDragAway( aui_MouseEvent *mouseData )
{
	if(GetMouseOwnership() == this)
		Highlight(false);	
}



void aui_Control::MouseRDragOver( aui_MouseEvent *mouseData )
{
	if(IsDisabled())
		return;

	if(GetMouseOwnership() == this)
		Highlight();	

	if(!GetMouseOwnership())
		MouseMoveOver(mouseData);
}



void aui_Control::MouseRDragAway( aui_MouseEvent *mouseData )
{
	if(IsDisabled())
		return;

	if(GetMouseOwnership() == this)
		Highlight(false);	

	if(!GetMouseOwnership())
		MouseMoveAway(mouseData);
}



void aui_Control::MouseNoChange( aui_MouseEvent *mouseData )
{


	if ( m_isMouseInside )
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	if ( !mouseData->lbutton && !mouseData->rbutton )
	if ( mouseData->time - m_noChangeTime > m_timeOut )
	{
		SetWhichSeesMouse( this );

		if ( ShowTipWindow( mouseData ) )
			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;
	}
}








void aui_Control::ExchangeImage(sint32 layerIndex, sint32 imageIndex,
								const MBCHAR *imageName)
{
	Assert(m_imageLayerList);
	if(!m_imageLayerList) return;
	
	m_imageLayerList->ExchangeImage(layerIndex, imageIndex, imageName);
}



AUI_ERRCODE	aui_Control::Resize(sint32 width, sint32 height)
{
	uint32 oldRenderFlags = m_renderFlags;

	
	AUI_ERRCODE errorCode = aui_Region::Resize(width, height);

	
	if(m_numberOfLayers) {
		MBCHAR *ldlBlock = (MBCHAR *)GetLdlBlock();
		if(!ldlBlock)
			
			
			ldlBlock = aui_Ldl::GetBlock(this);

		if(ldlBlock) {
			
			delete m_imageLayerList;
			m_imageLayerList = NULL;
			
			
			
			
			InitializeImageLayers(g_ui->GetLdl()->GetLdl()->FindDataBlock(ldlBlock));
		}
	}

	SetCurrentRenderFlags(oldRenderFlags);

	
	return(errorCode);
}



void aui_Control::InitializeLayerFlag(ldl_datablock *theBlock, sint32 layerIndex,
									  const MBCHAR *flagString, sint32 flag,
									  const MBCHAR *layerIndexString)
{
	
	
	static const sint32 LAYER_INDEX_STRING_SIZE = 16;
	static char localLayerIndexString[LAYER_INDEX_STRING_SIZE];

	
	if(!layerIndexString) {
		
		
		
		sprintf(localLayerIndexString, "%d", layerIndex);
		
		
		layerIndexString = localLayerIndexString;
	}

	
	
	
	std::string layerFlagString;
	if(theBlock->GetAttributeType(layerFlagString.assign(
		flagString).append(layerIndexString).c_str())
		&& theBlock->GetBool(layerFlagString.c_str()))
		m_layerRenderFlags[layerIndex] |= flag;
}


void aui_Control::DrawLayers(aui_Surface *surface, RECT *rectangle)
{
	
	for(int layerIndex = 0; layerIndex < m_numberOfLayers; layerIndex++) {
		
		
		if(m_layerRenderFlags[layerIndex] & m_renderFlags) {
			
			m_imageLayerList->SetState(layerIndex);

			
			m_imageLayerList->DrawImages(surface, rectangle);
		}
	}
}


void aui_Control::Highlight(bool status)
{
	
	if(IgnoreHighlight())
		return;

	
	sint32 renderFlags = GetCurrentRenderFlags();

	
	if(status && !IsDisabled())
		m_renderFlags |= k_AUI_CONTROL_LAYER_FLAG_HIGHLIGHT;
	else
		m_renderFlags &= ~k_AUI_CONTROL_LAYER_FLAG_HIGHLIGHT;

	
	if(GetCurrentRenderFlags() != renderFlags)
		ShouldDraw();
}





void aui_Control::ResetCurrentRenderFlags()
{
	
	BaseResetCurrentRenderFlags();
}



void aui_Control::InitializeLayerFlags(ldl_datablock *theBlock,
									   sint32 layerIndex)
{
	
	
	
	
	char layerIndexString[20];
	sprintf(layerIndexString, "%d", layerIndex);

	
	InitializeLayerFlag(theBlock, layerIndex, k_AUI_CONTROL_LDL_LAYER_ALWAYS,
		k_AUI_CONTROL_LAYER_FLAG_ALWAYS, layerIndexString);
	InitializeLayerFlag(theBlock, layerIndex, k_AUI_CONTROL_LDL_LAYER_HIGHLIGHT,
		k_AUI_CONTROL_LAYER_FLAG_HIGHLIGHT, layerIndexString);
	InitializeLayerFlag(theBlock, layerIndex, k_AUI_CONTROL_LDL_LAYER_DISABLED,
		k_AUI_CONTROL_LAYER_FLAG_DISABLED, layerIndexString);
	InitializeLayerFlag(theBlock, layerIndex, k_AUI_CONTROL_LDL_LAYER_ENABLED,
		k_AUI_CONTROL_LAYER_FLAG_ENABLED, layerIndexString);
}


bool aui_Control::AllocateImageLayers(ldl_datablock *theBlock)
{
	
	if(theBlock->GetAttributeType(k_AUI_CONTROL_NUMBER_OF_LAYERS) ==
		ATTRIBUTE_TYPE_INT)
		m_numberOfLayers = theBlock->GetInt(k_AUI_CONTROL_NUMBER_OF_LAYERS);

	
	if(!m_numberOfLayers)
		return(false);

	
	m_imagesPerLayer = 1;	
	if(theBlock->GetAttributeType(k_AUI_CONTROL_IMAGES_PER_LAYER) ==
		ATTRIBUTE_TYPE_INT)
		m_imagesPerLayer = theBlock->GetInt(k_AUI_CONTROL_IMAGES_PER_LAYER);

	
	m_imageLayerList = new aui_ImageList(m_numberOfLayers, m_imagesPerLayer);

	
	if(!m_layerRenderFlags)
		m_layerRenderFlags = new sint32[m_numberOfLayers];

	
	return(true);
}


void aui_Control::InitializeFlagLayers(ldl_datablock *theBlock)
{
	
	for(sint32 layerIndex = 0; layerIndex < m_numberOfLayers; layerIndex++) {
		
		m_layerRenderFlags[layerIndex] = 0;

		
		InitializeLayerFlags(theBlock, layerIndex);
	}
}


void aui_Control::LoadLayerImages(ldl_datablock *theBlock,
								  sint32 layerIndex)
{
	
	for(int imageIndex = 0; imageIndex < m_imagesPerLayer; imageIndex++) {
		
		
		
		
		
		char imageAttributeString[k_MAX_NAME_LEN];
		sprintf(imageAttributeString, "%s%d%d", k_AUI_CONTROL_LDL_IMAGE, layerIndex, imageIndex);

		
		const char *imageName = theBlock->GetString(imageAttributeString);

		
		
		RECT imageRectangle = { 0, 0, -1, -1 };

		
		
		
		
		char imageBltTypeStringName[k_MAX_NAME_LEN];
		sprintf(imageBltTypeStringName, "%s%d%d", k_AUI_IMAGEBASE_LDL_BLTTYPE, layerIndex, imageIndex);
		const char *imageBltTypeString = theBlock->GetString(imageBltTypeStringName);

		AUI_IMAGEBASE_BLTTYPE imageBltType = AUI_IMAGEBASE_BLTTYPE_COPY;
		if(imageBltTypeString) {
			if(stricmp(imageBltTypeString, k_AUI_IMAGEBASE_LDL_COPY) == 0) {
				imageBltType = AUI_IMAGEBASE_BLTTYPE_COPY;
			} else 	if(stricmp(imageBltTypeString, k_AUI_IMAGEBASE_LDL_STRETCH) == 0) {
				imageBltType = AUI_IMAGEBASE_BLTTYPE_STRETCH;
			} else if(stricmp(imageBltTypeString, k_AUI_IMAGEBASE_LDL_TILE) == 0) {
				imageBltType = AUI_IMAGEBASE_BLTTYPE_TILE;
			}
		} else {
			
			
			
			
			
			
			
			
			char stretchXAttributeString[k_MAX_NAME_LEN];
			char stretchYAttributeString[k_MAX_NAME_LEN];
			sprintf(stretchXAttributeString, "%s%d%d", k_AUI_CONTROL_IMAGE_STRETCH_X, layerIndex, imageIndex);
			sprintf(stretchYAttributeString, "%s%d%d", k_AUI_CONTROL_IMAGE_STRETCH_Y, layerIndex, imageIndex);

			if(theBlock->GetBool(stretchXAttributeString) ||
			   theBlock->GetBool(stretchYAttributeString)) {
				imageBltType = AUI_IMAGEBASE_BLTTYPE_TILE;
			}
		}
			

		
		
		
		char imageBltFlagStringName[k_MAX_NAME_LEN];
		sprintf(imageBltFlagStringName, "%s%d%d", k_AUI_IMAGEBASE_LDL_BLTFLAG, layerIndex, imageIndex);
		const char *imageBltFlagString = theBlock->GetString(imageBltFlagStringName);

		AUI_IMAGEBASE_BLTFLAG imageBltFlag = GetImageBltFlag();
		if(imageBltFlagString) {
			if(stricmp(imageBltFlagString, k_AUI_IMAGEBASE_LDL_BLTFLAG_COPY) == 0) {
				imageBltFlag = AUI_IMAGEBASE_BLTFLAG_COPY;
			} else if(stricmp(imageBltFlagString, k_AUI_IMAGEBASE_LDL_BLTFLAG_CHROMAKEY) == 0) {
				imageBltFlag = AUI_IMAGEBASE_BLTFLAG_CHROMAKEY;
			} else if(stricmp(imageBltFlagString, k_AUI_IMAGEBASE_LDL_BLTFLAG_BLEND) == 0) {
				imageBltFlag = AUI_IMAGEBASE_BLTFLAG_BLEND;
			}
		}

		char sizeStrName[k_MAX_NAME_LEN];
		sprintf(sizeStrName, "%s%d%d", k_AUI_LDL_HABSSIZE, layerIndex, imageIndex);
		sint32 width = theBlock->GetInt(sizeStrName);
		if(width > 0) {
			imageRectangle.right = imageRectangle.left + width;
		}

		sprintf(sizeStrName, "%s%d%d", k_AUI_LDL_VABSSIZE, layerIndex, imageIndex);
		sint32 height = theBlock->GetInt(sizeStrName);
		if(height > 0) {
			imageRectangle.bottom = imageRectangle.top + height;
		}

		
		m_imageLayerList->SetImage(layerIndex, imageIndex, imageBltType,
			imageBltFlag, &imageRectangle, imageName,
			m_chromaRed, m_chromaGreen, m_chromaBlue);
	}
}


void aui_Control::LoadLayers(ldl_datablock *theBlock)
{
	
	for(sint32 layerIndex = 0; layerIndex < m_numberOfLayers; layerIndex++) {
		
		LoadLayerImages(theBlock, layerIndex);
	}
}


sint32 aui_Control::DesiredWidth(ldl_datablock *theBlock,
								 sint32 layerIndex) const
{
	
	sint32 layerWidth = Width();

	
	
	
	
	
	char widthLayerString[k_MAX_NAME_LEN];
	sprintf(widthLayerString, "%s%d", k_AUI_LDL_HABSSIZE, layerIndex);

	
	if(theBlock->GetAttributeType(widthLayerString) == ATTRIBUTE_TYPE_INT) {
		
		layerWidth = theBlock->GetInt(widthLayerString);
	}

	
	
	if(!layerWidth && GetText() && GetTextFont()) {
		layerWidth = GetTextFont()->GetStringWidth(GetText()) + 24;	
	}

	
	return(layerWidth);
}





aui_Control::FillSize aui_Control::WidthToFill(ldl_datablock *theBlock,
											   sint32 layerIndex,
											   sint32 imageStart,
											   sint32 imageEnd,
											   sint32 desiredWidth) const
{
	
	FillSize result(0, desiredWidth);

	
	for(int imageIndex = imageStart; imageIndex <= imageEnd; imageIndex++) {
		
		
		
		
		
		
		char stretchXAttributeString[k_MAX_NAME_LEN];
		sprintf(stretchXAttributeString, "%s%d%d", k_AUI_CONTROL_IMAGE_STRETCH_X, layerIndex, imageIndex);

		
		if(theBlock->GetAttributeType(stretchXAttributeString) &&
			theBlock->GetBool(stretchXAttributeString)) {
			result.first++;
		} else {
			
#if defined(ACTIVISION_ORIGINAL)			
			result.second = std::_MAX(0L,
#else
				result.second = std::max(0L,
#endif
				result.second -
				m_imageLayerList->GetSize(layerIndex, imageIndex)->right);
		}
	}

	
	return(result);
}



bool aui_Control::FillWidth(ldl_datablock *theBlock,
							sint32 layerIndex, sint32 imageStart,
							sint32 imageEnd, const FillSize &desiredSize,
							sint32 &width)
{
	
	sint32 desiredWidth =
		desiredSize.first ? (desiredSize.second / desiredSize.first) : 0;

	
	for(sint32 imageIndex = imageStart; imageIndex <= imageEnd; imageIndex++) {
		
		
		
		
		
		char xAttributeString[k_MAX_NAME_LEN];
		sprintf(xAttributeString, "%s%d%d", k_AUI_LDL_HABSPOSITION, layerIndex, imageIndex);

		
		if(theBlock->GetAttributeType(xAttributeString) == ATTRIBUTE_TYPE_INT) {
			
			OffsetRect(m_imageLayerList->GetSize(layerIndex, imageIndex),
				theBlock->GetInt(xAttributeString), 0);
		} else if(imageIndex != imageStart) {	
			
			OffsetRect(m_imageLayerList->GetSize(layerIndex, imageIndex),
				m_imageLayerList->GetSize(layerIndex, imageIndex - 1)->right -
				m_imageLayerList->GetSize(layerIndex, imageIndex)->left, 0);
		}

		
		
		
		
		
		
		char stretchXAttributeString[k_MAX_NAME_LEN];
		sprintf(stretchXAttributeString, "%s%d%d", k_AUI_CONTROL_IMAGE_STRETCH_X, layerIndex, imageIndex);

		
		if(theBlock->GetAttributeType(stretchXAttributeString) &&
			theBlock->GetBool(stretchXAttributeString)) {
			m_imageLayerList->GetSize(layerIndex, imageIndex)->right =
				m_imageLayerList->GetSize(layerIndex, imageIndex)->left +
				desiredWidth;
		}

#if defined(ACTIVISION_ORIGINAL)					
		width = std::_MAX(width,
#else
		width = std::max(width,
#endif
			m_imageLayerList->GetSize(layerIndex, imageIndex)->right);
	}

	
	
	return(desiredSize.first > 0);
}





bool aui_Control::ResizeLayerWidth(ldl_datablock *theBlock,
								   sint32 layerIndex, sint32 numberOfRows,
								   sint32 *rowIndices, sint32 &width)
{
	
	bool sizedImage = false;

	
	sint32 desiredWidth = DesiredWidth(theBlock, layerIndex);

	
	for(sint32 rowIndex = 0; rowIndex < numberOfRows; rowIndex++) {
		
		sint32 imageStart = rowIndices[rowIndex];
		sint32 imageEnd = (rowIndex < (numberOfRows - 1)) ?
			(rowIndices[rowIndex + 1] - 1) : (m_imagesPerLayer - 1);

		
		sizedImage |= FillWidth(theBlock, layerIndex, imageStart, imageEnd,
			WidthToFill(theBlock, layerIndex, imageStart, imageEnd,
			desiredWidth), width);
	}

	
	return(sizedImage);
}


sint32 aui_Control::DesiredHeight(ldl_datablock *theBlock,
								  sint32 layerIndex) const
{
	
	sint32 layerHeight = Height();

	
	
	
	
	
	char heightLayerString[k_MAX_NAME_LEN];
	sprintf(heightLayerString, "%s%d", k_AUI_LDL_VABSSIZE, layerIndex);

	
	if(theBlock->GetAttributeType(heightLayerString) == ATTRIBUTE_TYPE_INT) {
		
		layerHeight = theBlock->GetInt(heightLayerString);
	}

	
	return(layerHeight);
}



aui_Control::FillSize aui_Control::HeightToFill(ldl_datablock *theBlock,
												sint32 layerIndex,
												sint32 numberOfRows,
												sint32 *rowIndices,
												sint32 column,
												sint32 desiredHeight) const
{
	
	FillSize result(0, desiredHeight);

	
	for(int rowIndex = 0; rowIndex < numberOfRows; rowIndex++) {
		
		sint32 imageIndex = RowColumnIndex(numberOfRows, rowIndices,
			rowIndex, column);

		
		if(imageIndex < 0)
			continue;

		
		
		
		
		
		
		char stretchYAttributeString[k_MAX_NAME_LEN];
		sprintf(stretchYAttributeString, "%s%d%d", k_AUI_CONTROL_IMAGE_STRETCH_Y, layerIndex, imageIndex);

		
		if(theBlock->GetAttributeType(stretchYAttributeString) &&
			theBlock->GetBool(stretchYAttributeString)) {
			result.first++;
		} else {
			
#if defined(ACTIVISION_DEFAULT)
			result.second = std::_MAX(0L,
#else
			result.second = std::max(0L,
#endif
				result.second -
				m_imageLayerList->GetSize(layerIndex, imageIndex)->bottom);
		}
	}

	
	return(result);
}



bool aui_Control::FillHeight(ldl_datablock *theBlock,
							 sint32 layerIndex, sint32 numberOfRows,
							 sint32 *rowIndices, sint32 column,
							 const FillSize &desiredSize,
							 sint32 &height)
{
	
	sint32 desiredHeight =
		desiredSize.first ? (desiredSize.second / desiredSize.first) : 0;

	
	for(int rowIndex = 0; rowIndex < numberOfRows; rowIndex++) {
		
		sint32 imageIndex = RowColumnIndex(numberOfRows, rowIndices,
			rowIndex, column);

		
		if(imageIndex < 0)
			continue;

		
		
		sint32 previousIndex = PreviousRowColumnIndex(numberOfRows, rowIndices,
			rowIndex, column);

		
		
		
		
		
		char yAttributeString[k_MAX_NAME_LEN];
		sprintf(yAttributeString, "%s%d%d", k_AUI_LDL_VABSPOSITION, layerIndex, imageIndex);

		
		if(theBlock->GetAttributeType(yAttributeString) == ATTRIBUTE_TYPE_INT) {
			
			OffsetRect(m_imageLayerList->GetSize(layerIndex, imageIndex),
				0, theBlock->GetInt(yAttributeString));
		} else if(previousIndex >= 0) {	
			
			OffsetRect(m_imageLayerList->GetSize(layerIndex, imageIndex), 0,
				m_imageLayerList->GetSize(layerIndex, previousIndex)->bottom -
				m_imageLayerList->GetSize(layerIndex, imageIndex)->top);
		}

		
		
		
		
		
		
		char stretchYAttributeString[k_MAX_NAME_LEN];
		sprintf(stretchYAttributeString, "%s%d%d", k_AUI_CONTROL_IMAGE_STRETCH_Y, layerIndex, imageIndex);

		
		if(theBlock->GetAttributeType(stretchYAttributeString) &&
			theBlock->GetBool(stretchYAttributeString)) {
			m_imageLayerList->GetSize(layerIndex, imageIndex)->bottom =
				m_imageLayerList->GetSize(layerIndex, imageIndex)->top +
				desiredHeight;
		}

#if defined(ACTIVISION_DEFAULT)
		height = std::_MAX(height,
#else
		height = std::max(height,
#endif
			m_imageLayerList->GetSize(layerIndex, imageIndex)->bottom);
	}

	
	
	return(desiredSize.first > 0);
}





bool aui_Control::ResizeLayerHeight(ldl_datablock *theBlock,
									sint32 layerIndex, sint32 numberOfRows,
									sint32 *rowIndices, sint32 &height)
{
	
	bool sizedImage = false;

	
	sint32 numberOfColumns = NumberOfColumns(numberOfRows, rowIndices);

	
	sint32 desiredHeight = DesiredHeight(theBlock, layerIndex);

	
	for(sint32 columnIndex = 0; columnIndex < numberOfColumns; columnIndex++) {
		
		sizedImage |= FillHeight(theBlock, layerIndex, numberOfRows, rowIndices,
			columnIndex, HeightToFill(theBlock, layerIndex, numberOfRows,
			rowIndices, columnIndex, desiredHeight), height);
	}

	
	return(sizedImage);
}



sint32 aui_Control::PreviousRowColumnIndex(sint32 numberOfRows, sint32 *rowIndices,
										   sint32 row, sint32 column) const
{
	
	sint32 imageIndex = -1;

	
	
	while((row > 0) && (imageIndex < 0))
		imageIndex = RowColumnIndex(numberOfRows, rowIndices, --row, column);

	
	return(imageIndex);
}



sint32 aui_Control::RowColumnIndex(sint32 numberOfRows, sint32 *rowIndices,
								   sint32 row, sint32 column) const
{
	
	sint32 imageIndex = rowIndices[row] + column;

	
	
	sint32 imageNextRow = (row < (numberOfRows - 1)) ?
		rowIndices[row + 1] : m_imagesPerLayer;

	
	if(!(imageIndex < imageNextRow))
		return(-1);

	
	return(imageIndex);
}


sint32 aui_Control::NumberOfColumns(sint32 numberOfRows,
									sint32 *rowIndices) const
{
	
	sint32 numberOfColumns = 0;

	
	for(sint32 rowIndex = 0; rowIndex < numberOfRows; rowIndex++) {
		
		sint32 imageStart = rowIndices[rowIndex];
		sint32 imageEnd = (rowIndex < (numberOfRows - 1)) ?
			(rowIndices[rowIndex + 1] - 1) : (m_imagesPerLayer - 1);

		
#if defined(ACTIVISION_DEFAULT)		
		numberOfColumns = std::_MAX(numberOfColumns,
#else
		numberOfColumns = std::max(numberOfColumns,
#endif
			(imageEnd - imageStart + 1L));
	}

	
	return(numberOfColumns);
}





sint32 aui_Control::SegmentImages(ldl_datablock *theBlock,
								  sint32 layerIndex,
								  sint32 *rowIndices) const
{
	
	sint32 currentSegment = 0;

	
	rowIndices[currentSegment++] = 0;

	
	for(int imageIndex = 1; imageIndex < m_imagesPerLayer; imageIndex++) {
		
		
		rowIndices[imageIndex] = -1;

		
		
		
		
		
		
		char nextRowAttributeString[k_MAX_NAME_LEN];
		sprintf(nextRowAttributeString, "%s%d%d", k_AUI_CONTROL_IMAGE_NEXT_ROW, layerIndex, imageIndex);

		
		if(theBlock->GetAttributeType(nextRowAttributeString) &&
			theBlock->GetBool(nextRowAttributeString)) {
			
			rowIndices[currentSegment++] = imageIndex;
		}
	}

	
	return(currentSegment);
}





void aui_Control::ResizeLayers(ldl_datablock *theBlock)
{
	
	TextReloadFont();

	
	sint32 width = Width();
	sint32 height = Height();

	
	for(sint32 layerIndex = 0; layerIndex < m_numberOfLayers; layerIndex++) {
		
		sint32 *rowIndices = new sint32[m_imagesPerLayer];
		sint32 numberOfRows = SegmentImages(theBlock,
			layerIndex, rowIndices);

		
		ResizeLayerWidth(theBlock, layerIndex, numberOfRows,
			rowIndices, width);

		
		ResizeLayerHeight(theBlock, layerIndex, numberOfRows,
			rowIndices, height);

		
		delete [] rowIndices;
	}

	
	
	
	aui_Region::Resize(width, height);
}


void aui_Control::BaseResetCurrentRenderFlags()
{
	
	SetCurrentRenderFlags(k_AUI_CONTROL_LAYER_FLAG_ALWAYS);

	
	
	if(IsDisabled())
		AddRenderFlags(k_AUI_CONTROL_LAYER_FLAG_DISABLED);
	else
		AddRenderFlags(k_AUI_CONTROL_LAYER_FLAG_ENABLED);

	
	ShouldDraw();
}


void aui_Control::InitializeImageLayers(ldl_datablock *theBlock)
{
	
	
	bool initializeFlags = (m_layerRenderFlags == NULL);

	
	
	if(!AllocateImageLayers(theBlock))
		return;

	
	if(initializeFlags)
		InitializeFlagLayers(theBlock);

	
	LoadLayers(theBlock);

	
	ResizeLayers(theBlock);

	
	BaseResetCurrentRenderFlags();
}

void aui_Control::SendKeyboardAction()
{
	if(m_ActionFunc) {
		m_ActionFunc(this, m_keyboardAction, 0, m_cookie);
	} else if(m_action) {
		m_action->Execute(this, m_keyboardAction, 0);
	}
}

bool aui_Control::HandleKey(uint32 wParam)
{
	if(m_actionKey == wParam) {
		SendKeyboardAction();
		return true;
	}

	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Control *control = (aui_Control *)m_childList->GetNext( position );
		
		if(control && control->HandleKey(wParam))
			return true;
	}
	return false;
}

void aui_Control::SetStatusText(const MBCHAR *text)
{
	m_statusText = text;
}
