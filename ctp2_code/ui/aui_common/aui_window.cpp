//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Activision User Interface window
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_window.h"

#include "aui_Factory.h"

#include "aui_blitter.h"
#include "aui_control.h"
#include "aui_rectangle.h"
#include "aui_surface.h"
#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "c3ui.h"           // C3UI

extern C3UI *   g_c3ui;

uint32 aui_Window::m_windowClassId = aui_UniqueId();

aui_Window::aui_Window(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	sint32 bpp,
	AUI_WINDOW_TYPE type )
	:
	aui_Region( retval, id, ldlBlock )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( bpp, type );
	Assert( AUI_SUCCESS(*retval) );
}

aui_Window::aui_Window(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 bpp,
	AUI_WINDOW_TYPE type )
	:
	aui_Region( retval, id, x, y, width, height )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( bpp, type );
	Assert( AUI_SUCCESS(*retval) );
}

AUI_ERRCODE aui_Window::InitCommon( sint32 bpp, AUI_WINDOW_TYPE type )
{
	m_surface = NULL;

	m_stencil = NULL;

	m_bpp = bpp ? bpp : g_ui->BitsPerPixel();
	m_type = type;
	m_surface = NULL;
	m_isDragging = FALSE;
	m_opaqueControls = FALSE;

	m_dirtyList = new aui_DirtyList;
	Assert( m_dirtyList != NULL );
	if ( !m_dirtyList ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_grabPoint.x = m_grabPoint.y = 0;

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	m_grabRegion = new aui_Region(
		&errcode,
		aui_UniqueId(),
		0,
		0,
		m_width,
		m_height );
	Assert( m_grabRegion != NULL );
	if ( !m_grabRegion ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_ogX = m_x;
	m_ogY = m_y;

	m_focusControl = NULL;
	m_focusList = new tech_WLList<aui_Region *>;

	return errcode;
}

AUI_ERRCODE aui_Window::CreateSurface( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	Assert( m_surface == NULL );
	if ( !m_surface )
	{
		m_surface = aui_Factory::new_Surface(errcode, m_width, m_height);

		Assert( AUI_NEWOK(m_surface,errcode) );
		if ( !AUI_NEWOK(m_surface,errcode) ) return errcode;
	}

	if(m_stencil) {
		m_surface->SetChromaKey(255,0,255);
	}
	return errcode;
}

aui_Window::~aui_Window()
{
	if (g_c3ui)
	{
		g_c3ui->RemoveWindow(Id());
	}

	delete m_surface;
	delete m_dirtyList;
	delete m_grabRegion;
	free(m_stencil);
	// Just a pointer not created here directly
	// delete m_focusControl;
	delete m_focusList;
}

AUI_ERRCODE aui_Window::Move( sint32 x, sint32 y )
{
	RECT oldRect = { m_x, m_y, m_x + m_width, m_y + m_height };

	AUI_ERRCODE errcode = aui_Region::Move( x, y );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	RECT newRect = { m_x, m_y, m_x + m_width, m_y + m_height };

	RECT consolidatedRect;
	if ( Rectangle_SmartConsolidate( &consolidatedRect, &oldRect, &newRect ) )
		g_ui->AddDirtyRect( &consolidatedRect );
	else
	{
		g_ui->AddDirtyRect( &oldRect );
		g_ui->AddDirtyRect( &newRect );
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Window::MoveOG( void )
{
	return Move(m_ogX, m_ogY);
}

AUI_ERRCODE aui_Window::Offset( sint32 dx, sint32 dy )
{
	RECT oldRect = { m_x, m_y, m_x + m_width, m_y + m_height };

	AUI_ERRCODE errcode = aui_Region::Offset( dx, dy );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	RECT newRect = { m_x, m_y, m_x + m_width, m_y + m_height };

	RECT consolidatedRect;
	if ( Rectangle_SmartConsolidate( &consolidatedRect, &oldRect, &newRect ) )
		g_ui->AddDirtyRect( &consolidatedRect );
	else
	{
		g_ui->AddDirtyRect( &oldRect );
		g_ui->AddDirtyRect( &newRect );
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Window::Resize( sint32 width, sint32 height )
{
	BOOL reallocSurface = FALSE;
	if ( m_surface )
	{
		reallocSurface =
			( width > m_surface->Width() || height > m_surface->Height() );

		if ( reallocSurface )
		{
			delete m_surface;
			m_surface = NULL;

		}
	}

	RECT oldRect = { m_x, m_y, m_x + m_width, m_y + m_height };

	AUI_ERRCODE errcode = aui_Region::Resize( width, height );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	RECT newRect = { m_x, m_y, m_x + m_width, m_y + m_height };

	RECT consolidatedRect;
	if ( Rectangle_SmartConsolidate( &consolidatedRect, &oldRect, &newRect ) )
		g_ui->AddDirtyRect( &consolidatedRect );
	else
	{
		g_ui->AddDirtyRect( &oldRect );
		g_ui->AddDirtyRect( &newRect );
	}

	if ( reallocSurface ) MakeSureSurfaceIsValid();

	m_dirtyList->Flush();
	Draw();

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Window::AddChild( aui_Region *child )
{
	Assert( child != NULL );
	if ( !child ) return AUI_ERRCODE_INVALIDPARAM;


	AUI_ERRCODE errcode = aui_Region::AddChild( child );
	if ( errcode == AUI_ERRCODE_OK )
	{
		aui_Control *control = (aui_Control *)child;

		control->SetParentWindow( this );
	}

	return errcode;
}

AUI_ERRCODE aui_Window::RemoveChild( uint32 controlId )
{
	ListPos position = m_childList->GetHeadPosition();
	for ( size_t i = m_childList->L(); i; i-- )
	{
		ListPos prevPos = position;
		aui_Control *control = (aui_Control *)m_childList->GetNext( position );
		if ( control->Id() == controlId )
		{

			control->ReleaseKeyboardFocus();

			control->SetParent( NULL );
			control->SetParentWindow( NULL );

			m_childList->DeleteAt( prevPos );

			m_childListChanged = TRUE;

			Draw();

			break;
		}
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_Window::ShowThis( void )
{
	MakeSureSurfaceIsValid();

	m_dirtyList->Flush();

	g_ui->AddDirtyRect(
		m_x,
		m_y,
		m_x + m_width,
		m_y + m_height );

	return aui_Region::ShowThis();
}


AUI_ERRCODE aui_Window::HideThis( void )
{
	DeleteSurfaceIfDynamic();

	m_dirtyList->Flush();

	if ( g_ui->GetWindow( m_id ) )
		g_ui->AddDirtyRect(
			m_x,
			m_y,
			m_x + m_width,
			m_y + m_height );

	return aui_Region::HideThis();
}


void aui_Window::MakeSureSurfaceIsValid( void )
{
	if (!m_surface)
	{
		CreateSurface();
	}
}

void aui_Window::DeleteSurfaceIfDynamic( void )
{
	if (IsDynamic())
	{
		delete m_surface;
		m_surface = NULL;
	}
}

AUI_ERRCODE aui_Window::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_surface;

	RECT rect = { 0, 0, m_width, m_height };

	g_ui->TheBlitter()->ColorBlt( surface, &rect, RGB(0,0,0), 0 );

	if ( surface == m_surface )
		AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Window::Invalidate( RECT *rect )
{
	if ( !IsHidden() )
	{
		if ( rect )
			m_dirtyList->AddRect( rect );
		else
		{
			m_dirtyList->Flush();

			m_dirtyList->AddRect(
				0,
				0,
				m_width,
				m_height );
		}
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Window::AddDirtyRect( RECT *rect )
{
	if ( !rect ) return AddDirtyRect( 0, 0, m_width, m_height );

	return AddDirtyRect( rect->left, rect->top, rect->right, rect->bottom );
}

AUI_ERRCODE aui_Window::AddDirtyRect( sint32 left, sint32 top, sint32 right, sint32 bottom )
{
	RECT windowRect = { 0, 0, m_width, m_height };
	RECT clippedRect = { left, top, right, bottom };

	if ( Rectangle_Clip( &clippedRect, &windowRect ) )
	{

		if ( m_surface )
		switch ( m_surface->BytesPerPixel() )
		{
		default:

			Assert( FALSE );
			break;

		case 2:
			if ( clippedRect.left & 1 )
				clippedRect.left--;
			if ( (clippedRect.right & 1) && clippedRect.right < m_width - 1 )
				clippedRect.right++;
			break;
		}

		m_dirtyList->AddRect( &clippedRect );
	}

	return AUI_ERRCODE_OK;
}

void aui_Window::SetTransparent(bool transparent, bool opaqueControls)
{
	if (transparent)
	{
		GetAttributes().Set(WindowAttribute::Transparent);

		if (!IsDynamic() || !IsHidden())
		{
			if (!m_surface) {
				CreateSurface();
			}
		}
	}
	else
	{
		GetAttributes().Reset(WindowAttribute::Transparent);
	}

	m_opaqueControls = opaqueControls;
	m_dirtyList->AddRect(0, 0, m_width, m_height);
}

void aui_Window::SetTranslucent(bool translucent, bool opaqueControls)
{
	if (translucent)
	{
		GetAttributes().Set(WindowAttribute::Translucent);

		if (!IsDynamic() || !IsHidden())
		{
			if (!m_surface) {
				CreateSurface();
			}
		}
	}
	else
	{
		GetAttributes().Reset(WindowAttribute::Translucent);
	}

	m_opaqueControls = opaqueControls;
	m_dirtyList->AddRect(0, 0, m_width, m_height);
}

void aui_Window::SetStronglyModal(bool stronglyModal)
{
	if (stronglyModal)
	{
		GetAttributes().Set(WindowAttribute::StronglyModal);
		GetAttributes().Reset(WindowAttribute::WeaklyModal);
	}
	else {
		GetAttributes().Reset(WindowAttribute::StronglyModal);
	}
}

void aui_Window::SetWeaklyModal(bool weaklyModal)
{
	if (weaklyModal)
	{
		GetAttributes().Set(WindowAttribute::WeaklyModal);
		GetAttributes().Reset(WindowAttribute::StronglyModal);
	}
	else {
		GetAttributes().Reset(WindowAttribute::WeaklyModal);
	}
}

void aui_Window::SetDraggable(bool draggable)
{
	if (draggable) {
		GetAttributes().Set(WindowAttribute::Draggable);
	}
	else {
		GetAttributes().Reset(WindowAttribute::Draggable);
	}
}

void aui_Window::SetDynamic(bool dynamic)
{
	if (dynamic)
	{
		GetAttributes().Set(WindowAttribute::Dynamic);
		if (IsHidden())
		{
			delete m_surface;
			m_surface = NULL;
		}
	}
	else
	{
		GetAttributes().Reset(WindowAttribute::Dynamic);
		MakeSureSurfaceIsValid();
	}
}

AUI_ERRCODE aui_Window::Draw( aui_Surface *surface, sint32 x, sint32 y )
{
	MakeSureSurfaceIsValid();

	return aui_Region::Draw( surface, x, y );
}

void aui_Window::PostChildrenCallback( aui_MouseEvent *mouseData )
{
	if ( IsStronglyModal() )

		if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
}

void aui_Window::MouseLDragAway( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( m_isDragging )
	{

		sint32 dx = m_x;
		sint32 dy = m_y;

		sint32 newPositionX = mouseData->position.x - m_grabPoint.x;
		sint32 newPositionY = mouseData->position.y - m_grabPoint.y;

		Offset( dx = newPositionX - dx, dy = newPositionY - dy );

		RECT rect1, rect2;
		Rectangle_GetErasers(
			&rect1, &rect2,
			newPositionX, newPositionY,
			dx, dy,
			m_width, m_height );

		RECT screenRect = { 0, 0, g_ui->Width(), g_ui->Height() };
		Rectangle_Clip( &rect1, &screenRect );
		Rectangle_Clip( &rect2, &screenRect );

		g_ui->AddDirtyRect( &rect1 );
		g_ui->AddDirtyRect( &rect2 );

		if ( !IsOpaque() ) Invalidate();

		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;
	}
}


void aui_Window::MouseLDragOver( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
	MouseLDragAway( mouseData );
}


void aui_Window::MouseLDragInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
	MouseLDragAway( mouseData );
}


void aui_Window::MouseLDragOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	MouseLDragAway( mouseData );
}


void aui_Window::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		if ( m_type > AUI_WINDOW_TYPE_BACKGROUND )
			g_ui->BringWindowToTop( this );

		if ( IsDraggable() )
		{
			sint32 localX = mouseData->position.x - m_x;
			sint32 localY = mouseData->position.y - m_y;

			if ( m_grabRegion->IsInside( localX, localY ) )
			{
				m_isDragging = TRUE;
				m_grabPoint.x = localX;
				m_grabPoint.y = localY;
			}
		}

		aui_Control *focus = aui_Control::GetKeyboardFocus();
		if ( focus && !GetChild( focus->Id() ) )
			focus->ReleaseKeyboardFocus();

		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;
	}
	else if ( IsDescendent( GetWhichSeesMouse() ) )
		if ( m_type > AUI_WINDOW_TYPE_BACKGROUND )
			g_ui->BringWindowToTop( this );
}


void aui_Window::MouseLGrabOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( IsWeaklyModal() )
	{

		AUI_ERRCODE errcode = g_ui->RemoveWindow( m_id );
		if ( errcode == AUI_ERRCODE_OK && m_mouseCode == AUI_ERRCODE_UNHANDLED )

			m_mouseCode = AUI_ERRCODE_HANDLED;
	}
}


void aui_Window::MouseLDropInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() ) SetWhichSeesMouse( this );
	MouseLDropOutside( mouseData );
}


void aui_Window::MouseLDropOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( m_isDragging )
	{
		m_isDragging = FALSE;
		m_grabPoint.x = m_grabPoint.y = 0;

		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;
	}
}

void aui_Window::SetStencilFromImage(const MBCHAR *imageFileName)
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	if(m_stencil) {
		free(m_stencil);
	}

	aui_Image *image = g_ui->LoadImage((MBCHAR *)imageFileName);
	Assert(image);
	if(!image)
		return;

	aui_Surface tempSurface(
		&errcode,
		m_surface->Width(),
		m_surface->Height(),
		16 );

	RECT imageRect = {
		0, 0,
		image->TheSurface()->Width(), image->TheSurface()->Height()
	};
	g_ui->TheBlitter()->Blt(&tempSurface, 0, 0,
							image->TheSurface(),
							&imageRect,
							k_AUI_BLITTER_FLAG_COPY);

	g_ui->UnloadImage(image);

	m_stencil = aui_CreateStencil(&tempSurface);

	m_surface->SetChromaKey(255,0,255);
}

void aui_Window::AddFocusControl(aui_Control *control)
{
	ListPos position = m_focusList->GetHeadPosition();
	for(size_t i = m_focusList->L(); i; i--) {
		aui_Control *c = (aui_Control *)m_focusList->GetNext(position);
		if(control->KeyboardFocusIndex() < c->KeyboardFocusIndex()) {
			m_focusList->InsertBefore(position, control);
			return;
		}
	}

	m_focusList->AddTail(control);
}

aui_Control *aui_Window::GetFocusControl()
{
	return m_focusControl;
}

aui_Control *aui_Window::NextFocusControl()
{
	bool next = false;

	if(m_focusList->L() < 1)
		return NULL;

	ListPos position = m_focusList->GetHeadPosition();
	size_t i;
	for ( i = m_focusList->L(); i; i-- )	{
		aui_Control *control = (aui_Control *)m_focusList->GetNext( position );
		if((!m_focusControl || next) && !control->IsHidden() && !control->IsDisabled()) {
			return control;
		} else if(m_focusControl == control) {
			next = true;
		}
	}

	Assert(next);

	position = m_focusList->GetHeadPosition();
	for(i = m_focusList->L(); i; i--) {
		aui_Control *control = (aui_Control *)m_focusList->GetNext(position);
		if(control == m_focusControl) {

			return NULL;
		} else if(!control->IsHidden() && !control->IsDisabled()) {
			return control;
		}
	}

	return (aui_Control *)m_focusList->GetHead();
}

bool aui_Window::HandleKey(uint32 wParam)
{
	switch(wParam) {
		case '\t' + 128:
			if(m_focusControl) {
				m_focusControl->ShouldDraw(k_AUI_REGION_DRAWFLAG_KEYBOARDFOCUSCHANGE);
			}
			m_focusControl = NextFocusControl();
			if(m_focusControl) {
				m_focusControl->ShouldDraw(k_AUI_REGION_DRAWFLAG_KEYBOARDFOCUSCHANGE);
			}

			ShouldDraw(TRUE);
			return true;
			break;
/* commented, because it seems to have no effect except that it eats up space that shoudl go into e.g. a text field
		case ' ':
			if(m_focusControl) {
				m_focusControl->SendKeyboardAction();
			}
			break;
*/
		default:
			if(m_focusControl && m_focusControl->HandleKey(wParam))
				return true;
			else
			{
				ListPos position = m_childList->GetHeadPosition();
				for ( size_t i = m_childList->L(); i; i-- )	{
					aui_Control *control = (aui_Control *)m_childList->GetNext( position );

					if(control->HandleKey(wParam))
						return true;
				}
			}
			break;

	}

	return false;
}
