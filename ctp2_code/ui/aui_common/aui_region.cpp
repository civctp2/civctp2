//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Activision User Interface region
// Id           : $Id$
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
// _DEBUG
// STATUS_BAR_MOUSE_OVER_LDL_DEBUG_INFORMATION
// - Generate debug version when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_dimension.h"
#include "aui_control.h"
#include "aui_window.h"
#include "aui_dragdropwindow.h"
#include "aui_rectangle.h"
#include "aui_ldl.h"
#include "aui_undo.h"
#include "aui_region.h"
#include "gamesounds.h"
#include "soundmanager.h"
#include "StatusBar.h"
#include "AttractWindow.h"

extern SoundManager		*g_soundManager;


static MBCHAR *k_AUI_REGION_LDL_BLINDNESS	=	"mouseblind";


aui_Region *aui_Region::m_whichSeesMouse = NULL;
aui_Region *aui_Region::m_editChild = NULL;
uint32 aui_Region::m_editSelectionCount = 0;
uint32 aui_Region::m_editSelectionCurrent = 0;
uint32 aui_Region::m_editModeStatus = AUI_EDIT_MODE_CHOOSE_REGION;
tech_WLList<aui_Undo *> *aui_Region::m_undoList = NULL;
uint32 aui_Region::m_regionClassId = aui_UniqueId();



aui_Region::aui_Region(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock )
{
	*retval = InitCommonLdl( id, ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_Region::aui_Region(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height )
	:
	aui_Base()
{
	*retval = InitCommon( id, x, y, width, height );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_Region::InitCommon( uint32 id, sint32 x, sint32 y, sint32 width, sint32 height )
{
	m_id = id,
	m_x = x,
	m_y = y,
	m_width = width,
	m_height = height,
	m_attributes = 0,
	m_parent = NULL,
	m_blind = FALSE,
	m_mouseCode = AUI_ERRCODE_UNHANDLED,
	m_childListChanged = FALSE,
	m_draw = 0,
	m_drawMask = k_AUI_REGION_DRAWFLAG_DEFAULTMASK,
	m_ignoreEvents = FALSE,
	m_isMouseInside = FALSE,
	m_xLastTime = x,
	m_yLastTime = y,
	m_noChange = FALSE,
	m_noChangeTime = 0,
	m_doubleLClickStartWaitTime = 0,
	m_doubleRClickStartWaitTime = 0,
	m_doubleClickingInside = TRUE,

	
	m_showCallback = NULL;
	m_hideCallback = NULL;
	m_showCallbackData = NULL;
	m_hideCallbackData = NULL;

	
	
	m_doubleClickTimeOut = GetDoubleClickTime();

	m_ldlBlock = NULL;

	m_editGrabPoint.x = -1;
	m_editGrabPoint.y = -1;
	m_editGrabPointAttributes = 0;

	
	memset( &m_mouseState, 0, sizeof( m_mouseState ) );

	
	m_childList = new tech_WLList<aui_Region *>;
	Assert( m_childList != NULL );
	if ( !m_childList ) return AUI_ERRCODE_MEMALLOCFAILED;

	
	m_dim = new aui_Dimension;
	Assert( m_dim != NULL );
	if ( !m_dim ) return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::InitCommonLdl( uint32 id, MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = InitCommon( id, 0, 0, 0, 0 );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	aui_Ldl *theLdl = g_ui->GetLdl();

	SetLdlBlock(ldlBlock);

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	
	aui_Region *parent = NULL;

	if ( block->GetAttributeType( k_AUI_LDL_PARENT ) == ATTRIBUTE_TYPE_STRING )
	{
		parent = (aui_Region *)aui_Ldl::GetObject( block->GetString( k_AUI_LDL_PARENT ) );
		Assert( parent != NULL );
	}

	
	if ( !parent )
	{
		
		MBCHAR *lastDot = strrchr( ldlBlock, '.' );
		if ( lastDot )
		{

			MBCHAR tempStr[k_AUI_LDL_MAXBLOCK];
			
			strncpy(tempStr, ldlBlock, (lastDot - ldlBlock));
			tempStr[lastDot - ldlBlock ] = '\0';

			parent = (aui_Region *)aui_Ldl::GetObject( tempStr );

			
			









			
		}
		else
		{
			
			parent = g_ui;
		}
	}

	
	m_dim->SetParent( parent );

	

	
	if(block->GetAttributeType(k_AUI_REGION_LDL_BLINDNESS))
		m_blind = block->GetBool(k_AUI_REGION_LDL_BLINDNESS);

	
	MBCHAR *anchor;

	
	if ( anchor = block->GetString( k_AUI_LDL_HANCHOR ) )
	{
		if ( stricmp( anchor, "right" ) == 0 ) {
			m_dim->AnchorRight();
		}else if ( stricmp( anchor, "center" ) == 0 ) {
			m_dim->AnchorHorizontalCenter();
		} else {
			m_dim->AnchorLeft();
		}

	}
	else
		m_dim->AnchorLeft();

	
	if ( anchor = block->GetString( k_AUI_LDL_VANCHOR ) )
	{
		if ( stricmp( anchor, "bottom" ) == 0 )
			m_dim->AnchorBottom();
		else if ( stricmp( anchor, "center" ) == 0 )
			m_dim->AnchorVerticalCenter();
		else
			m_dim->AnchorTop();
	}
	else
		m_dim->AnchorTop();

	
	if ( block->GetAttributeType( k_AUI_LDL_HRELPOSITION ) == ATTRIBUTE_TYPE_INT )
	{
		m_dim->HorizontalPositionData() = block->GetInt( k_AUI_LDL_HRELPOSITION );
		m_dim->AbsoluteHorizontalPosition( FALSE );
	}
	else
	{
		m_dim->HorizontalPositionData() = block->GetInt( k_AUI_LDL_HABSPOSITION );
		m_dim->AbsoluteHorizontalPosition( TRUE );
	}

	
	if ( block->GetAttributeType( k_AUI_LDL_VRELPOSITION ) == ATTRIBUTE_TYPE_INT )
	{
		m_dim->VerticalPositionData() = block->GetInt( k_AUI_LDL_VRELPOSITION );
		m_dim->AbsoluteVerticalPosition( FALSE );
	}
	else
	{
		m_dim->VerticalPositionData() = block->GetInt( k_AUI_LDL_VABSPOSITION );
		m_dim->AbsoluteVerticalPosition( TRUE );
	}

	
	if ( block->GetAttributeType( k_AUI_LDL_HRELSIZE ) == ATTRIBUTE_TYPE_INT )
	{
		m_dim->HorizontalSizeData() = block->GetInt( k_AUI_LDL_HRELSIZE );
		m_dim->AbsoluteHorizontalSize( FALSE );
	}
	else
	{
		m_dim->HorizontalSizeData() = block->GetInt( k_AUI_LDL_HABSSIZE );
		m_dim->AbsoluteHorizontalSize( TRUE );
	}

	
	if ( block->GetAttributeType( k_AUI_LDL_VRELSIZE ) == ATTRIBUTE_TYPE_INT )
	{
		m_dim->VerticalSizeData() = block->GetInt( k_AUI_LDL_VRELSIZE );
		m_dim->AbsoluteVerticalSize( FALSE );
	}
	else
	{
		m_dim->VerticalSizeData() = block->GetInt( k_AUI_LDL_VABSSIZE );
		m_dim->AbsoluteVerticalSize( TRUE );
	}

	
	m_dim->CalculateAll( &m_x, &m_y, &m_width, &m_height );

	
	errcode = aui_Ldl::Associate( this, ldlBlock );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_HACK;

	return AUI_ERRCODE_OK;
}



aui_Region::~aui_Region()
{
	aui_Ldl::Remove(this);

	delete m_dim;
	delete m_childList;
	delete [] m_ldlBlock;

	if (g_attractWindow && CanAttract()) 
	{
		g_attractWindow->RemoveRegion(this);
	}
}

void aui_Region::DeleteChildren()
{
	ListPos position = m_childList->GetHeadPosition();

	for ( sint32 i = m_childList->L(); i; i-- ) {
		aui_Region *child = m_childList->GetNext(position);
		child->DeleteChildren();
		delete child;
	}
}



aui_Region *aui_Region::SetWhichSeesMouse( aui_Region *region, BOOL force )
{
	aui_Region *prevRegion = GetWhichSeesMouse();
	if ( force )
		m_whichSeesMouse = region;
	else
		if ( !region || !region->IsBlind() ) m_whichSeesMouse = region;

#if FALSE
		
#ifdef _DEBUG
		
		
#define STATUS_BAR_MOUSE_OVER_LDL_DEBUG_INFORMATION
#ifdef STATUS_BAR_MOUSE_OVER_LDL_DEBUG_INFORMATION
		StatusBar::SetText(aui_Ldl::GetBlock(GetWhichSeesMouse()));
#endif	
#endif	
#endif

	return prevRegion;
}



BOOL aui_Region::SetBlindness( BOOL blind )
{
	BOOL wasBlind = m_blind;
	m_blind = blind;
	return wasBlind;
}



AUI_ERRCODE aui_Region::Move( sint32 x, sint32 y )
{
	
	if ( m_parent && ( m_x != x || m_y != y ) ) m_parent->ShouldDraw();

	m_x = x, m_y = y;
	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::Offset( sint32 dx, sint32 dy )
{
	
	if ( m_parent && ( dx || dy ) ) m_parent->ShouldDraw();

	m_x += dx, m_y += dy;
	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::Resize( sint32 width, sint32 height )
{
	



	
	if ( m_parent && ( width < m_width || height < m_height ) )
		m_parent->ShouldDraw();

	if ( (m_width = width) < 0 ) m_width = 0;
	if ( (m_height = height) < 0 ) m_height = 0;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::Adjust( void )
{
	if ( m_dim->GetParent() )
	{
		m_dim->CalculateAll( &m_x, &m_y, &m_width, &m_height );

		
		ListPos position = m_childList->GetHeadPosition();
		for ( sint32 i = m_childList->L(); i; i-- )
		{
			aui_Region *child = m_childList->GetNext( position );
			child->Adjust();
		}
	}

	return AUI_ERRCODE_OK;
}



BOOL aui_Region::IsInside( DWORD point ) const
{
	return IsInside( LOWORD(point), HIWORD(point) );
}



BOOL aui_Region::IsInside( LPPOINT point ) const
{
	return point ? IsInside( point->x, point->y ) : FALSE;
}



BOOL aui_Region::IsInside( LONG x, LONG y ) const
{
	return ( m_x <= x && x < m_x + m_width )
	&&     ( m_y <= y && y < m_y + m_height );
}



AUI_ERRCODE aui_Region::SetParent( aui_Region *region )
{
	m_parent = region;
	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::AddChild( aui_Region *child )
{
	Assert( child != NULL );
	if ( !child ) return AUI_ERRCODE_INVALIDPARAM;

	
	if ( !GetChild( child->Id() ) )
	{
		m_childList->AddTail( child );
		m_childListChanged = TRUE;
	}

	
	child->SetParent( this );

	
	BOOL anscestorHidden = FALSE;
	aui_Region *anscestor = this;
	while ( anscestor && !anscestorHidden )
	{
		anscestorHidden = anscestor->IsHidden();
		anscestor = anscestor->GetParent();
	};
	if ( anscestorHidden )
		
		child->Hide();
	else
		
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::InsertChild( aui_Region *child, sint32 index )
{
	Assert( child != NULL );
	Assert( index >= 0 && index <= (sint32)m_childList->L() );
	if ( !child || index < 0 || index > (sint32)m_childList->L() )
		return AUI_ERRCODE_INVALIDPARAM;

	if ( index == (sint32)m_childList->L() )
		return AddChild( child );

	
	if ( !GetChild( child->Id() ) )
	{
		m_childList->InsertBefore( m_childList->FindIndex( index ), child );
		m_childListChanged = TRUE;
	}

	
	child->SetParent( this );

	
	if ( IsHidden() )
		
		child->Hide();
	else
		
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::RemoveChild( uint32 regionId )
{
	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		ListPos prevPos = position;
		aui_Region *region = m_childList->GetNext( position );
		if ( region->Id() == regionId )
		{
			region->SetParent( NULL );

			
			m_childList->DeleteAt( prevPos );

			m_childListChanged = TRUE;

			break;
		}
	}

	
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return AUI_ERRCODE_OK;
}



aui_Region *aui_Region::GetChild( uint32 regionId )
{
	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Region *region = m_childList->GetNext( position );
		if ( region->Id() == regionId ) return region;
	}

	return NULL;
}




aui_Region *aui_Region::GetChildByIndex(sint32 index)
{
	Assert(m_childList != NULL);
	if (!m_childList) return NULL;

	Assert( index >= 0 && index < (sint32)m_childList->L() );
	if ( index < 0 || index >= (sint32)m_childList->L() )
		return NULL;

	ListPos position = m_childList->FindIndex( index );
	if ( !position ) return NULL;

	return (aui_Region *)m_childList->GetAt( position );
}




sint32 aui_Region::NumChildren(void)
{
	Assert(m_childList != NULL);
	if (m_childList == NULL) return 0;

	return m_childList->L();
}



BOOL aui_Region::IgnoreEvents( BOOL ignore )
{
	BOOL wasIgnoring = m_ignoreEvents;
	m_ignoreEvents = ignore;
	return wasIgnoring;
}



uint32 aui_Region::ShouldDraw( uint32 draw )
{
	uint32 wasDraw = m_draw;
	m_draw |= m_drawMask & draw;
	return wasDraw;
}



uint32 aui_Region::SetDrawMask( uint32 drawMask )
{
	uint32 prevDrawMask = m_drawMask;
	m_drawMask = drawMask;
	return prevDrawMask;
}



AUI_ERRCODE aui_Region::Show( void )
{
	ShowThis();
	ShowChildren();

	
	if(m_showCallback)
		m_showCallback(this, m_showCallbackData);

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::Hide( void )
{
	HideChildren();
	HideThis();

	
	if(m_hideCallback)
		m_hideCallback(this, m_hideCallbackData);

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::ShowThis( void )
{
	if ( IsHidden() ) 
	{
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
		m_attributes &= ~k_REGION_ATTRIBUTE_HIDDEN;

		return ResetThis();
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::HideThis( void )
{
	if ( !IsHidden() )
	{
		m_attributes |= k_REGION_ATTRIBUTE_HIDDEN;

		
		if ( m_parent ) m_parent->ShouldDraw();

		return ResetThis();
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::ShowChildren( void )
{
	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Region *region = m_childList->GetNext( position );
		region->Show();
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::HideChildren( void )
{
	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Region *region = m_childList->GetNext( position );
		region->Hide();
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::Enable( BOOL enable )
{
	BOOL changed = FALSE;

	if ( enable )
	{
		if ( IsDisabled() )
		{
			m_attributes &= ~k_REGION_ATTRIBUTE_DISABLED;
			changed = TRUE;
		}
	}
	else
	{
		if ( !IsDisabled() )
		{
			m_attributes |= k_REGION_ATTRIBUTE_DISABLED;
			changed = TRUE;
		}
	}

	if(m_childList) {
		
		ListPos position = m_childList->GetHeadPosition();
		for ( sint32 i = m_childList->L(); i; i-- )
		{
			aui_Region *region = m_childList->GetNext( position );
			region->Enable( enable );
		}
	}

	if ( changed )
	{
		
		ShouldDraw();

		return Reset();
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::Reset( void )
{
	ResetThis();

	
	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Region *region = m_childList->GetNext( position );
		region->Reset();
	}

	
	ShouldDraw();

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::ResetThis( void )
{
	if ( GetWhichSeesMouse() == this ) SetWhichSeesMouse( NULL );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::EnableDragDrop( BOOL enable )
{
	if ( enable )
		m_attributes |= k_REGION_ATTRIBUTE_DRAGDROP;
	else
		m_attributes &= ~k_REGION_ATTRIBUTE_DRAGDROP;

	return AUI_ERRCODE_OK;
}



BOOL aui_Region::IsDescendent( aui_Region *region )
{
	if ( GetChild( region->Id() ) ) return TRUE;

	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Region *childRegion = m_childList->GetNext( position );
		if ( childRegion->IsDescendent( region ) ) return TRUE;
	}

	return FALSE;
}



inline BOOL aui_Region::HasHeirarchyChanged( void ) const
{
	if ( m_childListChanged ) return TRUE;
	if ( !m_parent ) return this != g_ui;
	return m_parent->HasHeirarchyChanged();
}



void aui_Region::ResetHeirarchyChanged( void )
{
	m_childListChanged = FALSE;
	if ( m_parent ) m_parent->ResetHeirarchyChanged();
}



AUI_ERRCODE aui_Region::HandleMouseEvent( aui_MouseEvent *input, BOOL handleIt )
{
	
	m_mouseCode = AUI_ERRCODE_UNHANDLED;

	
	
	if ( handleIt ) PreChildrenCallback( input );

	
	ListPos position = m_childList->GetHeadPosition();
	if ( position )
	{
		
		aui_MouseEvent localInput;
		memcpy( &localInput, input, sizeof( localInput ) );
		localInput.position.x -= m_x;
		localInput.position.y -= m_y;

		for ( sint32 i = m_childList->L(); i; i-- )
		{
			aui_Region *region = m_childList->GetNext( position );
			AUI_ERRCODE errcode = region->HandleMouseEvent(
				&localInput,
				handleIt && !region->IgnoringEvents() );

			if ( m_mouseCode < errcode )
				m_mouseCode = errcode;

			
			if ( HasHeirarchyChanged() )
			{
				
				
				m_childListChanged = FALSE;
				return m_mouseCode;
			}
		}
	}

	
	if ( handleIt ) PostChildrenCallback( input );

	


	
	
	
	
	
	if ( IsHidden() || IgnoringEvents() ) handleIt = FALSE;

	
	if ( g_ui->GetEditMode() )
		MouseDispatchEdit( input, handleIt );
	else
		MouseDispatch( input, handleIt );

	
	memcpy( &m_mouseState, input, sizeof( aui_MouseEvent ) );

	m_xLastTime = m_x;
	m_yLastTime = m_y;

	
	if ( HasHeirarchyChanged() )
	{
		m_childListChanged = FALSE;
		return m_mouseCode;
	}

	
	
	if ( !input->framecount && m_draw ) Draw();

	
	return m_mouseCode;
}



aui_DragDropWindow *aui_Region::CreateDragDropWindow( aui_Control *dragDropItem )
{
	if ( !IsDragDrop() ) return NULL;

	
	
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	aui_DragDropWindow *ddw = new aui_DragDropWindow(
		&errcode,
		dragDropItem,	
		this,			
		0, 0, m_width, m_height ); 
	Assert( AUI_NEWOK(ddw,errcode) );
	if ( !AUI_NEWOK(ddw,errcode) ) return NULL;

	g_ui->AddChild( ddw );

	return ddw;
}



void aui_Region::DestroyDragDropWindow( aui_DragDropWindow *ddw )
{
	if ( !IsDragDrop() ) return;

	if ( ddw )
	{
		g_ui->RemoveChild( ddw->Id() );
		delete ddw;
	}
}



AUI_ERRCODE aui_Region::Draw( aui_Surface *surface, sint32 x, sint32 y )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	
	if ( !IsHidden() )
		errcode = DrawThis( surface, x, y );

	
	
	
	if ( errcode == AUI_ERRCODE_OK )
		DrawChildren( surface, x, y );

	
	m_draw = 0;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Region::DrawChildren( aui_Surface *surface, sint32 x, sint32 y )
{
	
	ListPos position = m_childList->GetTailPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Region *region = m_childList->GetPrev( position );

		
		region->ShouldDraw( m_draw );

		region->Draw( surface, x, y );
	}

	return AUI_ERRCODE_OK;
}








void aui_Region::MouseMoveOver( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
}



void aui_Region::MouseMoveInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
}



void aui_Region::MouseLDragOver( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
}



void aui_Region::MouseLDragInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
}



void aui_Region::MouseRDragOver( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
}



void aui_Region::MouseRDragInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
}



void aui_Region::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
}



void aui_Region::MouseLDropInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
}



void aui_Region::MouseRGrabInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
}



void aui_Region::MouseRDropInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
}



void aui_Region::MouseLDoubleClickInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	MouseLGrabInside( mouseData );
}



void aui_Region::MouseLDoubleClickOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	MouseLGrabOutside( mouseData );
}



void aui_Region::MouseRDoubleClickInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	MouseRGrabInside( mouseData );
}



void aui_Region::MouseRDoubleClickOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	MouseRGrabOutside( mouseData );
}



void aui_Region::MouseNoChange( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( m_isMouseInside && !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
}





AUI_ERRCODE aui_Region::AddUndo( void )
{
	
	
	
	if ( !m_undoList )
	{
		m_undoList = new tech_WLList<aui_Undo *>;
		Assert( m_undoList != NULL );
		if ( !m_undoList ) return AUI_ERRCODE_MEMALLOCFAILED;
	}

	RECT rect = { X(), Y(), X() + Width(), Y() + Height() };
	if ( m_parent != g_ui )
		(( aui_Control *)this)->ToScreen( &rect );

	aui_Undo *undo = new aui_Undo( this, rect );
	Assert( undo != NULL );
	if ( !undo ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_undoList->AddHead( undo );
	
	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_Region::PurgeUndoList( void )
{
	if ( m_undoList ) {
		ListPos position = m_undoList->GetHeadPosition();
		for ( sint32 i = m_undoList->L(); i; i-- ) {
			aui_Undo *undo = m_undoList->GetNext( position );
			delete undo;
		}
		m_undoList->DeleteAll();
		m_undoList = NULL;
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_Region::UndoEdit( void ) 
{
	if ( m_undoList ) {
		ListPos position = m_undoList->GetHeadPosition();
		aui_Undo *undo = m_undoList->GetNext( position );
		
		
		
		aui_Region *region = undo->GetUndoRegion();
		RECT regionRect = { region->X(), region->Y(), 
			region->X() + region->Width(), 
			region->Y() + region->Height() };
		RECT rect = undo->GetUndoRect();
		
		if ( region->GetParent() != g_ui )
			(( aui_Control *)region)->ToScreen( &regionRect );
		
		if ( rect.left != regionRect.left ) {
			region->Offset( rect.left - regionRect.left, 0 );
			region->GetDim()->SetHorizontalPosition( region->X() );
		}
		if ( rect.top != regionRect.top ) {
			region->Offset( 0, rect.top - regionRect.top );
			region->GetDim()->SetVerticalPosition( region->Y() );
		}
		if ( ( rect.right - rect.left ) != 
			( regionRect.right - regionRect.left ) ) {
			region->Resize( ( rect.right - rect.left ), region->Height() );
			region->GetDim()->SetHorizontalSize( region->Width() );
		}
		if ( ( rect.bottom - rect.top ) !=
			( regionRect.bottom - regionRect.top ) ) {
			region->Resize( region->Width(), ( rect.bottom - rect.top ) );
			region->GetDim()->SetVerticalSize( region->Height() );
		}
		g_ui->SetEditRegion( region );
		
		region->GetParent()->ShouldDraw();
		
		m_undoList->RemoveHead( );
		
		if ( !m_undoList->L() )
			m_undoList = NULL;
	}
	return AUI_ERRCODE_OK;

}


AUI_ERRCODE aui_Region::ExpandRect( RECT *rect ) 
{
	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Region *child = m_childList->GetNext( position );
		RECT crect = { child->X(), child->Y(),
					  child->X() + child->Width(),
					  child->Y() + child->Height() };
		
		
		(( aui_Control *)child)->ToScreen( &crect );

		
		if ( crect.left < rect->left ) rect->left = crect.left;
		if ( crect.top < rect->top ) rect->top = crect.top;
		if ( crect.right > rect->right ) rect->right = crect.right;
		if ( crect.bottom > rect->bottom ) rect->bottom = crect.bottom;

		child->ExpandRect( rect );
	}
	return AUI_ERRCODE_OK;
}


void aui_Region::MouseLGrabInsideEdit( aui_MouseEvent *mouseData )
{
	MouseLGrabEditMode( mouseData );
}


void aui_Region::MouseLGrabOutsideEdit( aui_MouseEvent *mouseData )
{
	MouseLGrabEditMode( mouseData );
}


void aui_Region::MouseLDragAwayEdit( aui_MouseEvent *mouseData )
{
	MouseLDragEditMode( mouseData );
}

void aui_Region::MouseLDragOverEdit( aui_MouseEvent *mouseData )
{
	MouseLDragEditMode( mouseData );
}



void aui_Region::MouseLDragInsideEdit( aui_MouseEvent *mouseData )
{
	MouseLDragEditMode( mouseData );
}



void aui_Region::MouseLDragOutsideEdit( aui_MouseEvent *mouseData )
{
	MouseLDragEditMode( mouseData );
}


void aui_Region::MouseLDropInsideEdit( aui_MouseEvent *mouseData )
{
	MouseLDropEditMode( mouseData );
}


void aui_Region::MouseLDropOutsideEdit( aui_MouseEvent *mouseData )
{
	MouseLDropEditMode( mouseData );
}


void aui_Region::MouseRGrabInsideEdit( aui_MouseEvent *mouseData )
{
	if ( m_editModeStatus == AUI_EDIT_MODE_CHOOSE_REGION ) {
		
		if ( this == m_editChild ) {
			m_editModeStatus = AUI_EDIT_MODE_CHOOSE_PARENT_REGION;				
		} else if ( m_editChild ) {
			
			
			POINT point = { mouseData->position.x, mouseData->position.y };
			if ( m_parent != g_ui )
				(( aui_Control *)this)->ToScreen( &point );
			if ( g_ui->TheEditRegion()->IsInside( point.x, point.y ) ) {
				m_editChild->MouseRGrabInsideEdit( mouseData );
				return;
			} else {
				m_editChild = NULL;
			}
		}
	}
	
	if ( !m_editChild ) {
		m_editChild = this;
		m_editModeStatus = AUI_EDIT_MODE_REGION_SELECTED;				
		m_editSelectionCount = 0;
		m_editSelectionCurrent = 0;
	}

	
	if ( m_editModeStatus == AUI_EDIT_MODE_CHOOSE_PARENT_REGION ) {
		if ( m_editSelectionCurrent < m_editSelectionCount ) {
			m_editSelectionCurrent++;
			
			if ( m_parent == g_ui ) {
				m_editModeStatus = AUI_EDIT_MODE_CHOOSE_REGION;
				m_editSelectionCount = 0;
				m_editSelectionCurrent = 0;

				if ( g_ui->TheEditRegion() )
					g_ui->TheEditRegion()->ShouldDraw( TRUE );

				g_ui->SetEditRegion( NULL );
				if ( m_editChild )
					m_editChild->MouseRGrabInsideEdit( mouseData );
			
			} else 
				m_parent->MouseRGrabInsideEdit( mouseData );

			return;
		} else
			m_editModeStatus = AUI_EDIT_MODE_REGION_SELECTED;
	}

	if ( m_editModeStatus == AUI_EDIT_MODE_REGION_SELECTED )
	{
		m_editSelectionCount++;
		m_editSelectionCurrent = m_editSelectionCount;

		m_editModeStatus = AUI_EDIT_MODE_MODIFY;

		SetWhichSeesMouse( this, TRUE );

		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

		if ( g_ui->TheEditRegion() )
			g_ui->TheEditRegion()->ShouldDraw( TRUE );

		if ( m_editChild ) {
			if ( m_editChild->m_parent == g_ui ) 
				m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
			else
				( ( aui_Control * )m_editChild )->GetParentWindow()->ShouldDraw( TRUE );
		}

		g_ui->SetEditRegion( this );

		m_editGrabPointAttributes = k_REGION_GRAB_NONE;
	}
}


void aui_Region::MouseRGrabOutsideEdit( aui_MouseEvent *mouseData )
{
	if ( this == m_editChild ) {
		if ( m_editChild->m_parent == g_ui ) 
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
		else
			( ( aui_Control * )m_editChild )->GetParentWindow()->ShouldDraw( TRUE );
		
		g_ui->SetEditRegion( NULL );
		m_editGrabPointAttributes = k_REGION_GRAB_NONE;
		m_editChild = NULL;
		m_editModeStatus = AUI_EDIT_MODE_CHOOSE_REGION;
		m_editSelectionCount = 0;
		m_editSelectionCurrent = 0;
	}
}


void aui_Region::MouseRDropInsideEdit( aui_MouseEvent *mouseData )
{
	m_editSelectionCurrent = 0;
	m_editModeStatus = AUI_EDIT_MODE_CHOOSE_REGION;
}


void aui_Region::MouseRDropOutsideEdit( aui_MouseEvent *mouseData )
{
	m_editSelectionCurrent = 0;
	m_editModeStatus = AUI_EDIT_MODE_CHOOSE_REGION;
}


void aui_Region::MouseLGrabEditMode( aui_MouseEvent *mouseData )
{
	if ( this == g_ui->TheEditRegion() ) {
		RECT grabRect = g_ui->TheEditRect();

		AddUndo();
		
		m_editGrabPoint = mouseData->position;
		
		if ( m_parent != g_ui )
			( ( aui_Control * )this)->ToScreen( &m_editGrabPoint );
		
		m_editGrabPointAttributes = k_REGION_GRAB_NONE;
		
		m_editModeStatus = AUI_EDIT_MODE_MODIFY;

		
		if ( ( ( m_editGrabPoint.x - grabRect.left ) < k_REGION_GRAB_SIZE ) &&
			( ( m_editGrabPoint.x - grabRect.left ) > 0 ) )
			m_editGrabPointAttributes |= k_REGION_GRAB_LEFT;
		else if ( ( ( grabRect.right - m_editGrabPoint.x ) < k_REGION_GRAB_SIZE ) &&
			( ( grabRect.right - m_editGrabPoint.x ) > 0 ) )
			m_editGrabPointAttributes |= k_REGION_GRAB_RIGHT;
		
		if ( ( ( m_editGrabPoint.y - grabRect.top ) < k_REGION_GRAB_SIZE ) &&
			( ( m_editGrabPoint.y - grabRect.top ) > 0 ) )
			m_editGrabPointAttributes |= k_REGION_GRAB_TOP;
		else if ( ( ( grabRect.bottom - m_editGrabPoint.y ) < k_REGION_GRAB_SIZE ) &&
			( ( grabRect.bottom - m_editGrabPoint.y ) > 0 ) )
			m_editGrabPointAttributes |= k_REGION_GRAB_BOTTOM;
		
		if ( ( m_editGrabPointAttributes == k_REGION_GRAB_NONE ) &&
			( ( m_editGrabPoint.x - grabRect.left ) > 0 ) &&
			( ( grabRect.right - m_editGrabPoint.x ) > 0 ) &&
			( ( m_editGrabPoint.y - grabRect.top ) > 0 ) &&
			( ( grabRect.bottom - m_editGrabPoint.y ) > 0 ) ) 
			m_editGrabPointAttributes |= k_REGION_GRAB_INSIDE;
	}
}


void aui_Region::MouseLDragEditMode( aui_MouseEvent *mouseData )
{
	if ( m_editModeStatus == AUI_EDIT_MODE_MODIFY ) {
		if ( this == g_ui->TheEditRegion( ) ) {

			sint32 dx = m_editGrabPoint.x;
			sint32 dy = m_editGrabPoint.y;
			POINT point = mouseData->position;

			if ( m_parent != g_ui )
				( ( aui_Control * )this)->ToScreen( &point );
			
			dx = point.x - dx;
			dy = point.y - dy;

			if ( m_editGrabPointAttributes & k_REGION_GRAB_INSIDE )
				Offset( dx, dy );
				
			
			
			if ( m_editGrabPointAttributes & k_REGION_GRAB_LEFT ) {
				if ( Width() - dx < ( k_REGION_GRAB_SIZE << 1 ) ) return;
				Offset( dx, 0 );
				Resize( Width() - dx, Height() );
			}
			
			if ( m_editGrabPointAttributes & k_REGION_GRAB_RIGHT ) {
				if ( Width() + dx < ( k_REGION_GRAB_SIZE << 1 ) ) return;
				Resize( Width() + dx, Height() );
			}
			
			if ( m_editGrabPointAttributes & k_REGION_GRAB_TOP ) {
				if ( Height() - dy < ( k_REGION_GRAB_SIZE << 1 ) ) return;
				Offset( 0, dy );
				Resize( Width(), Height() - dy );
			}

			if ( m_editGrabPointAttributes & k_REGION_GRAB_BOTTOM ) {
				if ( Height() + dy < ( k_REGION_GRAB_SIZE << 1 ) ) return;
				Resize( Width(), Height() + dy );
			}

			m_editGrabPoint = mouseData->position;

			if ( m_parent != g_ui )
				( ( aui_Control * )this)->ToScreen( &m_editGrabPoint );

			g_ui->SetEditRegion( this );

			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
		}
	}
}


void aui_Region::MouseLDropEditMode( aui_MouseEvent *mouseData )
{

	if ( this == g_ui->TheEditRegion( ) ) {
		m_parent->ShouldDraw();

		aui_Ldl *theLdl = g_ui->GetLdl();
		if ( theLdl ) {
			MBCHAR	*ldlBlock = theLdl->GetBlock( this );
			
			
			if ( m_editGrabPointAttributes & k_REGION_GRAB_INSIDE ) {
				m_dim->SetHorizontalPosition( X() );
				m_dim->SetVerticalPosition( Y() );
			} else {
				m_dim->SetHorizontalSize( Width() );
				m_dim->SetVerticalSize( Height() );
			}

			m_editGrabPointAttributes = k_REGION_GRAB_NONE;

			if ( ldlBlock ) {
				theLdl->ModifyAttributes( ldlBlock, m_dim );
			}
		}
	}
}


void aui_Region::EditModeModifyRegion( RECT rect )
{
	aui_Region *region = g_ui->TheEditRegion( );

	if ( region ) {
		aui_Dimension *dim = region->GetDim();

		if ( dim ) {
			if ( ( rect.left ) || ( rect.top ) ) {
				region->Offset( rect.left, rect.top );
				dim->SetHorizontalPosition( region->X() );
				dim->SetVerticalPosition( region->Y() );
			}
		
			if ( ( rect.right - rect.left != 0 ) ||
				 ( rect.bottom - rect.top != 0 ) ) {
				region->Resize( region->Width() + rect.right - rect.left,
								region->Height() + rect.bottom - rect.top );
				dim->SetHorizontalSize( region->Width() );
				dim->SetVerticalSize( region->Height() );
			}

			g_ui->SetEditRegion( region );

			region->GetParent()->ShouldDraw();

			if ( aui_Ldl *theLdl = g_ui->GetLdl() ) {
				if ( MBCHAR	*ldlBlock = theLdl->GetBlock( region ) ) {
					theLdl->ModifyAttributes( ldlBlock, dim );
				}
			}
		}
	}
}



AUI_ERRCODE aui_Region::DoneInstantiating()
{
	
	ListPos position = m_childList->GetTailPosition();
	for(sint32 i = m_childList->L(); i; i-- ) {
		aui_Region *region = m_childList->GetPrev( position );

		
		AUI_ERRCODE errcode = region->DoneInstantiating();
		if(errcode != AUI_ERRCODE_OK)
			return(errcode);
	}

	
	const MBCHAR *ldlBlock = aui_Ldl::GetBlock(this);
	if(ldlBlock) {
		AUI_ERRCODE errcode = DoneInstantiatingThis(ldlBlock);
		if(errcode != AUI_ERRCODE_OK)
			return(errcode);
	} else
		return(AUI_ERRCODE_OK);	

	
	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_Region::DoneInstantiatingThis(const MBCHAR *ldlBlock)
{
	
	return AUI_ERRCODE_OK;
}

const MBCHAR *aui_Region::GetLdlBlock()
{
	return m_ldlBlock;
}

void aui_Region::SetLdlBlock(const MBCHAR *ldlblock)
{
	if(m_ldlBlock) {
		delete [] m_ldlBlock;
		m_ldlBlock = NULL;
	}
	if(!ldlblock)
		return;

	m_ldlBlock = new MBCHAR[strlen(ldlblock) + 1];
	strcpy(m_ldlBlock, ldlblock);
}

