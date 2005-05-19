//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source file
// Description  :
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - moved CalculateHash to aui_Base
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_action.h"
#include "aui_surface.h"
#include "aui_mouse.h"
#include "aui_keyboard.h"
#include "aui_joystick.h"
#include "aui_blitter.h"
#include "aui_window.h"
#include "aui_control.h"
#include "aui_dirtylist.h"
#include "aui_region.h"
#include "aui_ldl.h"
#include "aui_static.h"
#include "aui_uniqueid.h"
#include "aui_rectangle.h"

#include "dxver.h"

#include "aui_ui.h"

#include "civ3_main.h"

extern BOOL		g_exclusiveMode;


aui_UI *g_ui = NULL;

aui_UI::aui_UI(
	AUI_ERRCODE *retval,
	HINSTANCE hinst,
	HWND hwnd,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *ldlFilename )
	:
	aui_Region( retval, 0, 0, 0, width, height )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	Assert( aui_Base::GetBaseRefCount() == 2 );
	g_ui = aui_Base::GetBaseRefCount() == 2 ? this : NULL;

	*retval = InitCommon( hinst, hwnd, bpp, ldlFilename );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateScreen();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_UI::InitCommon(
	HINSTANCE hinst,
	HWND hwnd,
	sint32 bpp,
	MBCHAR *ldlFilename )
{
	m_hinst = hinst;
	m_hwnd = hwnd;
	m_bpp = bpp;
	m_pixelFormat = AUI_SURFACE_PIXELFORMAT_UNKNOWN;
	m_primary = NULL;
	m_blitter = NULL;
	m_memmap = NULL;
	m_mouse = NULL;
	m_keyboard = NULL;
	m_joystick = NULL;
	m_audioManager = NULL;
	m_movieManager = NULL;

	
	m_editMode = FALSE;
	m_editRegion = NULL;
	m_editWindow = NULL;
	m_localRectText = NULL;
	m_absoluteRectText = NULL;
	m_editModeLdlName = NULL;

	
	m_color = k_AUI_UI_NOCOLOR;
	m_image = NULL;
	SetRect( &m_imageRect, 0, 0, 0, 0 );
	m_colorAreas = NULL;
	m_imageAreas = NULL;
	m_minimize = FALSE;
	m_virtualFocus = NULL;

	m_savedMouseAnimFirstIndex = 0;
	m_savedMouseAnimLastIndex = 0;
	m_savedMouseAnimCurIndex = 0;
	m_savedMouseAnimDelay = 0;

	m_imageResource = new aui_Resource<aui_Image>;
	Assert( m_imageResource != NULL );
	if ( !m_imageResource ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_cursorResource = new aui_Resource<aui_Cursor>;
	Assert( m_cursorResource != NULL );
	if ( !m_cursorResource ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_bitmapFontResource = new aui_Resource<aui_BitmapFont>;
	Assert( m_bitmapFontResource != NULL );
	if ( !m_bitmapFontResource ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_dirtyList = new aui_DirtyList;
	Assert( m_dirtyList != NULL );
	if ( !m_dirtyList ) return AUI_ERRCODE_MEMALLOCFAILED;

	
	m_dirtyList->AddRect( 0, 0, m_width, m_height );

	
	m_actionList = new tech_WLList<aui_Action *>;
	Assert( m_actionList != NULL );
	if ( !m_actionList ) return AUI_ERRCODE_MEMALLOCFAILED;

	
	
	m_destructiveActionList = new tech_WLList<aui_Action *>;
	Assert( m_destructiveActionList != NULL );
	if ( !m_destructiveActionList ) return AUI_ERRCODE_MEMALLOCFAILED;

	
	m_winList = new tech_WLList<HWND>;
	Assert( m_winList != NULL );
	if ( !m_winList ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_dirtyRectInfoList = new tech_WLList<DirtyRectInfo *>;
	Assert( m_dirtyRectInfoList != NULL );
	if ( !m_dirtyRectInfoList ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_dirtyRectInfoMemory = new tech_Memory<DirtyRectInfo>;
	Assert( m_dirtyRectInfoMemory != NULL );
	if ( !m_dirtyRectInfoMemory ) return AUI_ERRCODE_MEMALLOCFAILED;

	
	if ( ldlFilename )
	{
		AUI_ERRCODE errcode;
		m_ldl = new aui_Ldl( &errcode, ldlFilename );
		Assert( AUI_NEWOK(m_ldl,errcode) );
		if ( !AUI_NEWOK(m_ldl,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

		aui_Ldl::Associate( this, "" );
	}
	else m_ldl = NULL;


	
	m_dxver = 0;
	HANDLE dll = LoadLibrary( "dll\\util\\dxver" );
	if ( dll )
	{
		
		typedef BOOL (WINAPI *FuncType)( DWORD *pVersion );
		FuncType GetDirectXVersion =
			(FuncType)GetProcAddress( (HINSTANCE)dll, "MicrosoftDirectXInstalled" );
		Assert( GetDirectXVersion != NULL );
		if ( !GetDirectXVersion )
		{
			FreeLibrary( (HINSTANCE)dll );
			return AUI_ERRCODE_HACK;
		}

		
		switch ( GetDirectXVersion( &m_dxver ) )
		{
		case 0: break; 
		case DX_SOFTWARE: break; 
		case DX_HARDWARE: break; 
		case DX_NOINFO: break; 
		}

		FreeLibrary( (HINSTANCE)dll );
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_UI::CreateScreen( void )
{
	AUI_ERRCODE retcode;

	
	
	m_primary = new aui_Surface( &retcode, m_width, m_height, m_bpp, 0, NULL, TRUE );
	Assert( AUI_NEWOK(m_primary,retcode) );
	if ( !AUI_NEWOK(m_primary,retcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_pixelFormat = m_primary->PixelFormat();

	return retcode;
}

extern void free_crc();

aui_UI::~aui_UI()
{
	if ( m_editMode )
	{
		SetEditMode( FALSE );
	}

	if ( m_primary )
	{
		delete m_primary;
		m_primary = NULL;
	}

	if ( m_colorAreas )
	{
		delete m_colorAreas;
		m_colorAreas = NULL;
	}
	
	if ( m_imageAreas )
	{
		delete m_imageAreas;
		m_imageAreas = NULL;
	}
	
	if ( m_imageResource )
	{
		delete m_imageResource;
		m_imageResource = NULL;
	}

	if ( m_cursorResource )
	{
		delete m_cursorResource;
		m_cursorResource = NULL;
	}

	if ( m_bitmapFontResource )
	{
		delete m_bitmapFontResource;
		m_bitmapFontResource = NULL;
	}

	if ( m_dirtyList )
	{
		delete m_dirtyList;
		m_dirtyList = NULL;
	}

	if ( m_actionList )
	{
		delete m_actionList;
		m_actionList = NULL;
	}

	if (m_destructiveActionList) 
	{
		delete m_destructiveActionList;
		m_destructiveActionList = NULL;
	}

	if ( m_winList )
	{
		delete m_winList;
		m_winList = NULL;
	}

	if ( m_dirtyRectInfoList )
	{
		delete m_dirtyRectInfoList;
		m_dirtyRectInfoList = NULL;
	}

	if ( m_dirtyRectInfoMemory )
	{
		delete m_dirtyRectInfoMemory;
		m_dirtyRectInfoMemory = NULL;
	}

	aui_Ldl::Remove(this);

	
	if ( m_ldl )
	{
		delete m_ldl;
		m_ldl = NULL;
	}

	
	g_ui = NULL;

	free_crc();

	sint32 test = aui_Base::GetBaseRefCount();
	Assert( aui_Base::GetBaseRefCount() == 2 );
}



void aui_UI::RegisterObject( aui_Blitter *blitter )
{
	Assert( blitter != NULL );
	if ( blitter ) m_blitter = blitter;
}



void aui_UI::RegisterObject( aui_MemMap *memmap )
{
	Assert( memmap != NULL );
	if ( memmap ) m_memmap = memmap;
}



void aui_UI::RegisterObject( aui_Mouse *mouse )
{
	Assert( mouse != NULL );
	if ( mouse ) (m_mouse = mouse)->SetClip( 0, 0, m_width, m_height );
}



void aui_UI::RegisterObject( aui_Keyboard *keyboard )
{
	Assert( keyboard != NULL );
	if ( keyboard ) m_keyboard = keyboard;
}



void aui_UI::RegisterObject( aui_Joystick *joystick )
{
	Assert( joystick != NULL );
	if ( joystick ) m_joystick = joystick;
}



void aui_UI::RegisterObject( aui_AudioManager *audioManager )
{
	Assert( audioManager != NULL );
	if ( audioManager ) m_audioManager = audioManager;
}



void aui_UI::RegisterObject( aui_MovieManager *movieManager )
{
	Assert( movieManager != NULL );
	if ( movieManager ) m_movieManager = movieManager;
}



COLORREF aui_UI::SetBackgroundColor( COLORREF color )
{
	COLORREF prevColor = m_color;
	m_color = color;

	if ( (m_color = color) == k_AUI_UI_NOCOLOR )
	{
		if ( m_colorAreas )
		{
			delete m_colorAreas;
			m_colorAreas = NULL;
		}
	}
	else if ( !m_colorAreas )
	{
		m_colorAreas = new aui_DirtyList;
		Assert( m_colorAreas != NULL );
	}
	
	return prevColor;
}



aui_Image *aui_UI::SetBackgroundImage( aui_Image *image, sint32 x, sint32 y )
{
	aui_Image *prevImage = m_image;

	SetRect( &m_imageRect, x, y, x, y );

	if ( m_image = image )
	{
		m_imageRect.right += m_image->TheSurface()->Width();
		m_imageRect.bottom += m_image->TheSurface()->Height();

		if ( !m_imageAreas )
		{
			m_imageAreas = new aui_DirtyList;
			Assert( m_imageAreas != NULL );
		}
	}
	else if ( m_imageAreas )
	{
		delete m_imageAreas;
		m_imageAreas = NULL;
	}

	return prevImage;
}



AUI_ERRCODE aui_UI::AddChild( aui_Region *child )
{
	Assert(this);
	Assert( child != NULL );
	if ( !child || !this) return AUI_ERRCODE_INVALIDPARAM;

	
	Assert( child->IsThisA( aui_Window::m_windowClassId ) );
	if ( !child->IsThisA( aui_Window::m_windowClassId ) )
		return AUI_ERRCODE_INVALIDPARAM;

	aui_Window *window = (aui_Window *)child;

	
	ListPos position = m_childList->GetHeadPosition();

	if ( !position ) {
		
		m_childList->AddHead( child );
	}
	else
	{
		
		if ( !GetChild( child->Id() ) )
		{
			
			for ( sint32 j = m_childList->L(); j; j-- )
			{
				ListPos curPosition = position;

				aui_Window *curWindow =
					(aui_Window *)m_childList->GetNext( position );

				if ( window->Type() >= curWindow->Type() )
				{
					
					m_childList->InsertBefore( curPosition, window );
					break;
				}
			}

			if ( !j )
				
				m_childList->AddTail( window );
		} else {
			
			
			
			return AUI_ERRCODE_OK;
		}
	}

	m_childListChanged = TRUE;

	
	window->SetParent( this );

	
	window->Draw();

	return AUI_ERRCODE_OK;

}



AUI_ERRCODE aui_UI::RemoveChild( uint32 windowId )
{
	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		ListPos prevPos = position;
		aui_Window *window = (aui_Window *)m_childList->GetNext( position );
		if ( window->Id() == windowId )
		{
			
			window->DeleteSurfaceIfDynamic();

			
  			aui_Control *focus = aui_Control::GetKeyboardFocus();
  			if ( focus && window->GetChild( focus->Id() ) )
  			{
  				focus->ReleaseKeyboardFocus();
  				focus->ReleaseMouseOwnership();
  			}
			
			window->Reset();

			
			
			m_virtualFocus = NULL;

			
			position = window->ChildList()->GetHeadPosition();
			for ( i = window->ChildList()->L(); i; i-- )
			{
				aui_Control *control =
					(aui_Control *)window->ChildList()->GetNext( position );
				control->HideTipWindow();
			}

			window->SetParent( NULL );

			m_childList->DeleteAt( prevPos );

			AddDirtyRect(
				window->X(),
				window->Y(),
				window->X() + window->Width(),
				window->Y() + window->Height() );

			m_childListChanged = TRUE;

			break;
		}
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_UI::AddWin( HWND hwnd )
{
	if ( !m_winList->Find( hwnd ) )
		m_winList->AddTail( hwnd );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_UI::RemoveWin( HWND hwnd )
{
	ListPos position = m_winList->Find( hwnd );
	if ( position ) m_winList->DeleteAt( position );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_UI::AddDirtyRect( RECT *rect )
{
	if ( !rect ) return AddDirtyRect( 0, 0, m_width, m_height );

	return AddDirtyRect( rect->left, rect->top, rect->right, rect->bottom );
}



AUI_ERRCODE aui_UI::AddDirtyRect( sint32 left, sint32 top, sint32 right, sint32 bottom )
{
	RECT windowRect = { 0, 0, m_width, m_height };
	RECT clippedRect = { left, top, right, bottom };

	if ( Rectangle_Clip( &clippedRect, &windowRect ) )
		m_dirtyList->AddRect( &clippedRect );

	return AUI_ERRCODE_OK;
}



aui_Window *aui_UI::BringWindowToTop( uint32 windowId )
{
	return BringWindowToTop( (aui_Window *)GetChild( windowId ) );
}




aui_Window *aui_UI::BringWindowToTop( aui_Window *window )
{
	aui_Window *prevTopWindow = NULL;

	
	uint32 type = window->Type();

	
	
	BOOL found = FALSE;

	m_childListChanged = TRUE;

	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 j = m_childList->L(); j; j-- )
	{
		ListPos curPosition = position;

		aui_Window *curWindow =
			(aui_Window *)m_childList->GetNext( position );

		if ( !found )
		{
			
			if ( window == curWindow ) return window;

			if ( type == curWindow->Type() )
			{
				prevTopWindow = curWindow;

				
				m_childList->InsertBefore( curPosition, window );
				found = TRUE;
			}
		}
		else
		{
			
			if ( window == curWindow )
			{
				m_childList->DeleteAt( curPosition );

				AddDirtyRect(
					window->X(),
					window->Y(),
					window->X() + window->Width(),
					window->Y() + window->Height() );

				break;
			}
		}
	}

	return prevTopWindow;
}



AUI_ERRCODE aui_UI::Idle( aui_Region *recurse )
{
	if ( !recurse )
	{
		ListPos position = m_childList->GetTailPosition();
		for ( sint32 i = m_childList->L(); i; i-- )
		{
			aui_Region *window = m_childList->GetPrev( position );
			window->Idle();

			
			
			
			
			
			ListPos childPosition = window->ChildList()->GetHeadPosition();
			for ( sint32 j = window->ChildList()->L(); j; j-- )
				Idle( window->ChildList()->GetNext( childPosition ) );

			
			if ( m_childListChanged )
			{
				
				
				m_childListChanged = FALSE;
				return AUI_ERRCODE_OK;
			}
		}
	}
	else
	{
		recurse->Idle();

		ListPos childPosition = recurse->ChildList()->GetHeadPosition();
		for ( sint32 j = recurse->ChildList()->L(); j; j-- )
			Idle( recurse->ChildList()->GetNext( childPosition ) );
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_UI::Invalidate( RECT *rect )
{
	if ( rect )
		AddDirtyRect( rect );
	else
	{
		FlushDirtyList();

		
		m_dirtyList->AddRect(
			0,
			0,
			m_width,
			m_height );
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_UI::FlushDirtyList( void )
{
	m_dirtyList->Flush();

	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Window *window = (aui_Window *)m_childList->GetNext( position );
		window->GetDirtyList()->Flush();
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_UI::ShowWindow( uint32 windowId )
{
	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Window *window = (aui_Window *)m_childList->GetNext( position );
		if ( window->Id() == windowId )
		{
			window->Show();

			break;
		}
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_UI::HideWindow( uint32 windowId )
{
	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Window *window = (aui_Window *)m_childList->GetNext( position );
		if ( window->Id() == windowId )
		{
			
			window->Hide();
			break;
		}
	}

	return AUI_ERRCODE_OK;
}





AUI_ERRCODE aui_UI::ClipAndConsolidate(void)
{
	
	static aui_Window *window = NULL;
	static sint32 windowX = 0;
	static sint32 windowY = 0;
	static sint32 windowWidth = 0;
	static sint32 windowHeight = 0;
	static aui_Surface *windowSurface = NULL;
	static BOOL windowOpaqueControls = FALSE;
	static RECT windowRect = { 0, 0, 0, 0 };
	static aui_DirtyList *windowDirtyList = NULL;
	static aui_Stencil *windowStencil = NULL;

	
	
	m_dirtyList->Minimize();

	
	if ( m_colorAreas || m_imageAreas )
	{
		if ( m_colorAreas ) m_colorAreas->Flush();
		if ( m_imageAreas ) m_imageAreas->Flush();

		ListPos position = m_dirtyList->GetHeadPosition();
		for ( sint32 i = m_dirtyList->L(); i; i-- )
		{
			RECT *dirtyRect = m_dirtyList->GetNext( position );

			if ( m_colorAreas) m_colorAreas->AddRect( dirtyRect );
			if ( m_imageAreas )
			{
				RECT clipped = *dirtyRect;
				if ( Rectangle_Clip( &clipped, &m_imageRect ) )
					m_imageAreas->AddRect( &clipped );
			}
		}		

		
		position = m_childList->GetHeadPosition();
		for ( i = m_childList->L(); i; i-- )
		{
			window = (aui_Window *)m_childList->GetNext( position );
			if ( !window->IsHidden() )
			{
				SetRect( &windowRect,
					window->X(),
					window->Y(),
					window->X() + window->Width(),
					window->Y() + window->Height() );

				if ( m_colorAreas ) m_colorAreas->SubtractRect( &windowRect );
				if ( m_imageAreas ) m_imageAreas->SubtractRect( &windowRect );
			}
		}

		if ( m_imageAreas )
		{
			
			if ( m_colorAreas ) m_colorAreas->SubtractRect( &m_imageRect );

			m_imageAreas->Minimize();
		}

		if ( m_colorAreas ) m_colorAreas->Minimize();
		
		
		memset( &windowRect, 0, sizeof( windowRect ) );
	}

	
	ListPos position = m_childList->GetTailPosition();
	const ListPos tailPosition = position;
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		window = (aui_Window *)m_childList->GetPrev( position );

		if ( !window->IsHidden() )
		{
			
			windowX = window->X();
			windowY = window->Y();
			windowWidth = window->Width();
			windowHeight = window->Height();
			windowSurface = window->TheSurface();
			windowOpaqueControls = window->AreControlsOpaque();
			windowRect.right = windowWidth;		
			windowRect.bottom = windowHeight;	
			windowDirtyList = window->m_dirtyList;
			windowStencil = window->GetStencil();

			
			ListPos dirtyPosition = m_dirtyList->GetHeadPosition();
			for ( sint32 j = m_dirtyList->L(); j; j-- )
			{
				RECT *rect = m_dirtyList->GetNext( dirtyPosition );

				RECT convertedRect = *rect;
				OffsetRect( &convertedRect, -windowX, -windowY );

				window->AddDirtyRect( &convertedRect );
			}

			
			if ( !window->IsOpaque() && windowDirtyList->L() )
			{
				
				
				
#if 0
				if(windowStencil) {					
					RECT stencilRect = {
						0, 0,
						windowStencil->Width(),
						windowStencil->Height()
					};

					g_ui->TheBlitter()->ColorStencilBlt(windowSurface, &stencilRect, windowStencil, &stencilRect, windowSurface->GetChromaKey(), 0);
				} else {
					window->Draw();
				}
#endif
				windowDirtyList->Flush();
				windowDirtyList->AddRect( &windowRect );
				
				if ( j = m_childList->L() - i )
				{
					
					
					
					ListPos remainPosition = tailPosition;
					for ( ; j; j-- )
					{
						aui_Window *remainWindow =
							(aui_Window *)m_childList->
								GetPrev( remainPosition );

						if ( !remainWindow->IsHidden() )
						{
							sint32 left = remainWindow->X() - windowX;
							sint32 top = remainWindow->Y() - windowY;
							RECT rect =
							{
								left,
								top,
								left + remainWindow->Width(),
								top + remainWindow->Height()
							};

							if (Rectangle_Clip( &rect, &windowRect ))
							{
								RECT srcRect = rect;
								OffsetRect( &srcRect, -left, -top );

								g_ui->TheBlitter()->StencilMixBlt16(
									windowSurface, 
									&rect,
									NULL, 
									&rect,
									remainWindow->TheSurface(),
									&srcRect,
									windowStencil,
									&rect);
							}


						}
					}
				}



#if 0
				if ( m_colorAreas || m_imageAreas )
				{
					ListPos pos = windowDirtyList->GetHeadPosition();
					for ( j = windowDirtyList->L(); j; j-- )
					{
						RECT *rect = windowDirtyList->GetNext( pos );

						
						
						







						if ( m_imageAreas )
						{
							RECT clippedImageRect = *rect;
							OffsetRect(
								&clippedImageRect,
								windowX,
								windowY );
							Rectangle_Clip(
								&clippedImageRect,
								&m_imageRect );
							OffsetRect(
								&clippedImageRect,
								-m_imageRect.left,
								-m_imageRect.top );

							g_ui->TheBlitter()->Blt(
								windowMixingSurface,
								clippedImageRect.left + m_imageRect.left -
									windowX,
								clippedImageRect.top + m_imageRect.top -
									windowY,
								m_image->TheSurface(),
								&clippedImageRect,
								k_AUI_BLITTER_FLAG_COPY );
						}
					}
				}
#endif

				if ( windowOpaqueControls )
					window->DrawChildren();

				windowDirtyList->Flush();
				windowDirtyList->AddRect( &windowRect );
			}
			else
				
				windowDirtyList->Minimize();

			
			
			ListPos remainPosition = position;
			for ( j = i - 1; j; j-- )
			{
				aui_Window *remainWindow =
					(aui_Window *)m_childList->GetPrev( remainPosition );

				if ( !remainWindow->IsHidden() )
				{
					sint32 left = remainWindow->X() - windowX;
					sint32 top = remainWindow->Y() - windowY;

					AUI_ERRCODE altered = windowDirtyList->SubtractRect(
						left,
						top,
						left + remainWindow->Width(),
						top + remainWindow->Height() );

					
					
					if ( altered == AUI_ERRCODE_HANDLED
					&&   !remainWindow->IsOpaque() )
					{
						remainWindow->m_dirtyList->Flush();
						remainWindow->AddDirtyRect(
							0,
							0,
							remainWindow->Width(),
							remainWindow->Height() );
					}
				}
			}

			
			remainPosition = windowDirtyList->GetHeadPosition();
			for ( j = windowDirtyList->L(); j; j-- )
				InsertDirtyRectInfo(
					windowDirtyList->GetNext( remainPosition ),
					window );
		}
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_UI::InsertDirtyRectInfo( RECT *rect, aui_Window *window )
{
	Assert( !Rectangle_HasZeroArea( rect ) );
	if ( Rectangle_HasZeroArea( rect ) )
		return AUI_ERRCODE_OK;

	
	DirtyRectInfo *newDri = m_dirtyRectInfoMemory->New();
	Assert( newDri != NULL );
	if ( !newDri ) return AUI_ERRCODE_MEMALLOCFAILED;

	
	memcpy( &newDri->rect, rect, sizeof( RECT ) );
	newDri->window = window;

	
	sint32 newDriX = rect->left + window->X();
	sint32 newDriY = rect->top + window->Y();

	
	
	BOOL equals = FALSE;
	ListPos position = m_dirtyRectInfoList->GetHeadPosition();
	for ( sint32 i = m_dirtyRectInfoList->L(); i; i-- )
	{
		ListPos prevPos = position;
		DirtyRectInfo *dri = m_dirtyRectInfoList->GetNext( position );

		sint32 driY = dri->rect.top + dri->window->Y();

		if ( !equals )
		{
			if ( newDriY < driY )
			{
				m_dirtyRectInfoList->InsertBefore( prevPos, newDri );
				return AUI_ERRCODE_OK;
			}
			else if ( newDriY == driY )
			{
				sint32 driX = dri->rect.left + dri->window->X();

				if ( newDriX <= driX )
				{
					m_dirtyRectInfoList->InsertBefore( prevPos, newDri );
					return AUI_ERRCODE_OK;
				}

				equals = TRUE;
			}
		}
		else
		{
			sint32 driX = dri->rect.left + dri->window->X();

			if ( newDriX <= driX || newDriY < driY )
			{
				m_dirtyRectInfoList->InsertBefore( prevPos, newDri );
				return AUI_ERRCODE_OK;
			}
		}
	}

	
	m_dirtyRectInfoList->AddTail( newDri );
	return AUI_ERRCODE_OK;
}



void aui_UI::FlushDirtyRectInfoList( void )
{
	for ( sint32 i = m_dirtyRectInfoList->L(); i; i-- )
		m_dirtyRectInfoMemory->Delete( m_dirtyRectInfoList->RemoveHead() );
}




AUI_ERRCODE aui_UI::DrawOne(aui_Window *window)
{
	AUI_ERRCODE errcode;

	ClipAndConsolidate();

	errcode = m_mouse->Suspend( FALSE );
	Assert( errcode == AUI_ERRCODE_OK );





#if LOCK_SURFACES_ONCE
	LPVOID primaryBuf;
	errcode = m_primary->Lock( NULL, &primaryBuf, 0 );
	Assert( errcode == AUI_ERRCODE_OK );
#endif

	m_mouse->BltWindowToPrimary(window);

#if LOCK_SURFACES_ONCE
	errcode = m_primary->Unlock( primaryBuf );
	Assert( errcode == AUI_ERRCODE_OK );
#endif

	errcode = m_mouse->Resume();
	Assert( errcode == AUI_ERRCODE_OK );

	
	window->m_dirtyList->Flush();

	


	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_UI::Draw( void )
{
	
	if ( !m_primary ) return AUI_ERRCODE_OK;

	AUI_ERRCODE errcode;

	ClipAndConsolidate();

	if (m_mouse == NULL) return AUI_ERRCODE_OK;

	errcode = m_mouse->Suspend( FALSE );
	Assert( errcode == AUI_ERRCODE_OK );

	
	if ( m_colorAreas )
		m_mouse->BltBackgroundColorToPrimary(
			m_color,
			m_colorAreas );
	if ( m_imageAreas )
		m_mouse->BltBackgroundImageToPrimary(
			m_image,
			&m_imageRect,
			m_imageAreas );











	
	
	m_mouse->BltDirtyRectInfoToPrimary();

  if ( m_editMode )
	{
		ShowSelectedRegion( m_editRegion );
	}

	errcode = m_mouse->Resume();
	Assert( errcode == AUI_ERRCODE_OK );

	
	
	FlushDirtyList();

	
	FlushDirtyRectInfoList();

	return AUI_ERRCODE_OK;
}






void aui_UI::SetEditRegion( aui_Region *region )
{
	if ( !region ) {
		if ( m_editRegion )
			m_editRegion->ShouldDraw( TRUE );
		m_editRegion = NULL;
		m_editModeLdlName->SetText( " " );
		m_localRectText->SetText( "000, 000, 000, 000" );
		m_absoluteRectText->SetText( "000, 000, 000, 000" );
		SetRect( &m_editRect, 0, 0, 0, 0 );
		m_editWindow->ShouldDraw( TRUE );
		aui_Region::EditModeClear();
		return;
	}	

	m_editRegion = region;
		
	if ( aui_Ldl *theLdl = g_ui->GetLdl() ) {
		if ( MBCHAR	*ldlBlock = theLdl->GetBlock( region ) ) {
			MBCHAR editBuffer[ 1024 ];
			MBCHAR *p = editBuffer;
			memset( editBuffer, '\0', sizeof( editBuffer ) );

			MBCHAR *lastName = ldlBlock;

			for(; *ldlBlock; *ldlBlock++) {
				*p++ = *ldlBlock;
				if (*ldlBlock == '.') {
					*p++ = ' ';

					lastName = ldlBlock+1;
				}
			}
			m_editModeLdlName->SetText( lastName );
		} else {
			m_editModeLdlName->SetText( " " );
		}
	}
	
	
	SetRect( &m_editRect, region->X(), region->Y(),
						  region->X() + region->Width(),
						  region->Y() + region->Height() );

	MBCHAR buffer[ 256 ];

	sprintf( buffer, "%d, %d, %d, %d", m_editRect.left, m_editRect.top, 
									   m_editRect.right, m_editRect.bottom );	

	m_localRectText->SetText( buffer );

	if ( region->GetParent() != g_ui )
		(( aui_Control *)region)->ToScreen( &m_editRect );

	
	region->ExpandRect( &m_editRect );


	sprintf( buffer, "%d, %d, %d, %d", m_editRect.left, m_editRect.top, 
									   m_editRect.right, m_editRect.bottom );	

	m_absoluteRectText->SetText( buffer );

	m_editWindow->Draw( );

}


void aui_UI::SetEditMode( BOOL mode )
{
	m_editMode = mode;

	aui_Region::EditModeClear();

	if ( !m_editMode ) {
		CreateEditModeDialog( FALSE );

		SetRect( &m_editRect, 0, 0, 0, 0 );

		if ( m_editRegion )
			m_editRegion->ShouldDraw( TRUE );
	
		m_editRegion = NULL;
	} else {
		CreateEditModeDialog( TRUE );
	}
}


AUI_ERRCODE aui_UI::CreateEditModeDialog(BOOL make)
{
	AUI_ERRCODE auiErr;

	if ( make ) {
		if ( m_editWindow ) return AUI_ERRCODE_OK;
		m_editWindow = new aui_Window( &auiErr, aui_UniqueId(),
								0, 0, 
								200,
								82,
								m_primary->BitsPerPixel(),
								AUI_WINDOW_TYPE_FLOATING );
		Assert( AUI_NEWOK( m_editWindow, auiErr ) );
		if ( !AUI_NEWOK( m_editWindow, auiErr ) )
			return AUI_ERRCODE_MEMALLOCFAILED;

		m_editWindow->SetDraggable( TRUE );

		
		m_localRectText = new aui_Static( &auiErr, aui_UniqueId(),
										5, 5,
										190, 17, "000, 000, 000, 000" );
		Assert( AUI_NEWOK( m_localRectText, auiErr ) );
		if ( !AUI_NEWOK( m_localRectText, auiErr ) )
			return AUI_ERRCODE_MEMALLOCFAILED;

		m_localRectText->SetBlindness( TRUE );

		m_editWindow->AddControl( m_localRectText );

		m_absoluteRectText = new aui_Static( &auiErr, aui_UniqueId(),
										5, 21,
										190, 17, "000, 000, 000, 000" );
		Assert( AUI_NEWOK( m_absoluteRectText, auiErr ) );
		if ( !AUI_NEWOK( m_absoluteRectText, auiErr ) )
			return AUI_ERRCODE_MEMALLOCFAILED;

		m_absoluteRectText->SetBlindness( TRUE );

		m_editWindow->AddControl( m_absoluteRectText );

		m_editModeLdlName = new aui_Static( &auiErr, aui_UniqueId(),
										5, 39,
										190, 40, "                                                    " );
		Assert( AUI_NEWOK( m_editModeLdlName, auiErr ) );
		if ( !AUI_NEWOK( m_editModeLdlName, auiErr ) )
			return AUI_ERRCODE_MEMALLOCFAILED;

		m_editModeLdlName->SetBlindness( TRUE );

		m_editWindow->AddControl( m_editModeLdlName );

		AddWindow( m_editWindow );

	} else {

		if ( !m_editWindow ) return AUI_ERRCODE_OK;
		
		RemoveWindow( m_editWindow->Id( ) );

		delete m_localRectText;
		m_localRectText = NULL;

		delete m_absoluteRectText;
		m_absoluteRectText = NULL;

		delete m_editModeLdlName;
		m_editModeLdlName = NULL;

		delete m_editWindow;
		m_editWindow = NULL;

	}

	return AUI_ERRCODE_OK;

}


AUI_ERRCODE aui_UI::ShowSelectedRegion( aui_Region *region )
{
	
	if ( !region ) return AUI_ERRCODE_OK;

	RECT rect;

	sint32 x = m_editRect.left;
	sint32 y = m_editRect.top;
	sint32 width = m_editRect.right - m_editRect.left;
	sint32 height = m_editRect.bottom - m_editRect.top;

	Assert( width  > 3 );
	Assert( height > 3 );

	
	SetRect( &rect, x, y, x + 1, y + height );
	m_blitter->ColorBlt( m_primary, &rect, RGB(0,0,0), 0 );
	
	SetRect( &rect, x + 1, y + 1, x + 2, y + height - 1 );
	m_blitter->ColorBlt( m_primary, &rect, RGB(255,255,255), 0 );

	SetRect( &rect, x + 2, y + 2, x + 3, y + height - 2 );
	m_blitter->ColorBlt( m_primary, &rect, RGB(0,0,0), 0 );

	
	SetRect( &rect, x, y, x + width, y + 1 );
	m_blitter->ColorBlt( m_primary, &rect, RGB(0,0,0), 0 );

	SetRect( &rect, x + 1, y + 1, x + width - 1, y + 2 );
	m_blitter->ColorBlt( m_primary, &rect, RGB(255,255,255), 0 );

	SetRect( &rect, x + 2, y + 2, x + width - 2, y + 3 );
	m_blitter->ColorBlt( m_primary, &rect, RGB(0,0,0), 0 );

	
	SetRect( &rect, x + width - 1, y, x + width, y + height );
	m_blitter->ColorBlt( m_primary, &rect, RGB(0,0,0), 0 );

	SetRect( &rect, x + width - 2, y + 1, x + width - 1, y + height - 1 );
	m_blitter->ColorBlt( m_primary, &rect, RGB(255,255,255), 0 );

	SetRect( &rect, x + width - 3, y + 2, x + width - 2, y + height - 2 );
	m_blitter->ColorBlt( m_primary, &rect, RGB(0,0,0), 0 );

	
	SetRect( &rect, x, y + height - 1, x + width, y + height );
	m_blitter->ColorBlt( m_primary, &rect, RGB(0,0,0), 0 );

	SetRect( &rect, x + 1, y + height - 2, x + width - 1, y + height - 1 );
	m_blitter->ColorBlt( m_primary, &rect, RGB(255,255,255), 0 );

	SetRect( &rect, x + 2, y + height - 3, x + width - 2, y + height - 2 );
	m_blitter->ColorBlt( m_primary, &rect, RGB(0,0,0), 0 );

	return AUI_ERRCODE_OK;

}



AUI_ERRCODE aui_UI::HandleMouseEvents(
	sint32 numEvents,
	aui_MouseEvent *events )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_UNHANDLED;

	if (!m_mouse) return errcode;

	aui_MouseEvent *curEvent = events;

	
	
	static aui_MouseEvent mouseEvents[ k_MOUSE_MAXINPUT ];
	if ( !numEvents && !events )
	{
		numEvents = m_mouse->ManipulateInputs( mouseEvents, FALSE );
		curEvent = mouseEvents;
	}

	
	TagMouseEvents( numEvents, curEvent );

	
	for ( sint32 k = numEvents; k; k--, curEvent++ )
	{
		SetWhichSeesMouse( NULL );

		
		ListPos position = m_childList->GetHeadPosition();
		for ( sint32 i = m_childList->L(); i; i-- )
		{
			aui_Window *window = (aui_Window *)m_childList->GetNext( position );

			if ( !window->IsHidden() && window->Type() != AUI_WINDOW_TYPE_TIP )
			{
				errcode = window->HandleMouseEvent(
					curEvent,
					!window->IgnoringEvents() );

				
				
				
				
				
				if ( m_childListChanged || errcode == AUI_ERRCODE_HANDLEDEXCLUSIVE)
				{
					
					
					m_childListChanged = FALSE;

					
					position = m_childList->GetHeadPosition();
					for ( i = m_childList->L(); i; i-- )
					{
						window = (aui_Window *)m_childList->GetNext( position );
						if ( !window->IsHidden() && window->Type() != AUI_WINDOW_TYPE_TIP )
						{
							window->HandleMouseEvent(
								curEvent,
								FALSE );
						}
					}

					
					if ( k > 1 ) return HandleMouseEvents( k - 1, curEvent + 1 );
					return errcode;
				}
			}
		}
	}

	return errcode;
}



AUI_ERRCODE aui_UI::HandleKeyboardEvents( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_UNHANDLED;







	
	m_keyboard->GetInput();

	
	
	
	return AUI_ERRCODE_OK;

	
	aui_Control *control = aui_Control::GetKeyboardFocus();

	
	for ( sint32 k = 1; k; k-- )
	{
		aui_KeyboardEvent *event = m_keyboard->GetLatestKeyboardEvent();

		if ( control )
			errcode = control->HandleKeyboardEvent( event );

		
		if ( m_childListChanged )
		{
			
			
			m_childListChanged = FALSE;
			return errcode;
		}

		
		if ( event->key == AUI_KEYBOARD_KEY_TAB )
		{
			static BOOL wasJustDown = FALSE;

			if ( !event->down )
			{
				wasJustDown = FALSE;
			}
			else if ( !wasJustDown )
			{
				tech_WLList<aui_Region *> *siblings;

				if ( control ) m_virtualFocus = control;

				if ( m_virtualFocus )
				{
					siblings = m_virtualFocus->GetParent()->ChildList();
					ListPos position = siblings->Find( m_virtualFocus );

					siblings->GetNext( position );

					
					
					if ( position )
						m_virtualFocus = (aui_Control *)siblings->GetNext( position );
					else
						m_virtualFocus = (aui_Control *)siblings->GetHead();
				}
				else if ( m_childList->L() )
				{
					siblings = TopWindow()->ChildList();
					if ( siblings->L() )
						m_virtualFocus = (aui_Control*)siblings->GetHead();
				}

				if ( m_virtualFocus )
					m_virtualFocus->SetKeyboardFocus();

				wasJustDown = TRUE;
			}
		}
	}

	return errcode;
}



AUI_ERRCODE aui_UI::HandleJoystickEvents( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_UNHANDLED;

	
	m_joystick->GetInput();

	
	aui_Control *control = aui_Control::GetKeyboardFocus();
	if ( !control ) return errcode;

	
	for ( sint32 k = 1; k; k-- )
	{
		errcode = control->HandleJoystickEvent(
			m_joystick->GetLatestJoystickEvent() );

		
		if ( m_childListChanged )
		{
			
			
			m_childListChanged = FALSE;
			return errcode;
		}
	}

	return errcode;
}



AUI_ERRCODE aui_UI::HandleWindowsMessage(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_UNHANDLED;

	
	if ( hwnd == m_hwnd )
	switch( message )
	{
	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint( hwnd, &ps );

		
		Invalidate( &ps.rcPaint );

		EndPaint( hwnd, &ps );

		errcode = AUI_ERRCODE_HANDLED;
		break;

	case WM_SETCURSOR:
		
		if ( m_mouse && !m_mouse->IsSuspended() )
		{
			while ( ShowCursor( FALSE ) >= 0 )
				; 
		}
		else
		{
			while ( ShowCursor( TRUE ) < 0 )
				; 
		}

		errcode = AUI_ERRCODE_HANDLED;
		break;

	
	case WM_SYSKEYUP:
		
		if ( !IsIconic( m_hwnd ) )
		switch ( (int)wParam )
		{
		case VK_TAB:	
		case VK_ESCAPE:	
			AltTabOut();
			errcode = AUI_ERRCODE_HANDLED;
			break;
		case VK_MENU:
			errcode = AUI_ERRCODE_HANDLED;
			break;
		}
		break;

	
	
	case WM_ACTIVATE:
		
		if ( LOWORD(wParam) == WA_INACTIVE )
		{

			if (g_exclusiveMode) {
			
				if ( !IsChildWin( (HWND)lParam ) )
				if ( !IsIconic( m_hwnd ) )
				{
					AltTabOut();
					errcode = AUI_ERRCODE_HANDLED;
				}
			} else {
				if ( !IsChildWin( (HWND)lParam ) )
				{
					AltTabOut();
					errcode = AUI_ERRCODE_HANDLED;
				}
			}

		}
		
		else
		{

			if (g_exclusiveMode) {
			
				if (IsIconic( m_hwnd ) )
				{
					AltTabIn();
					errcode = AUI_ERRCODE_HANDLED;
				}
			} else {
				AltTabIn();
				errcode = AUI_ERRCODE_HANDLED;
			}
		}
		break;

	
	
	case WM_SYSCOMMAND:
		{
			if (LOWORD(wParam) == SC_SCREENSAVE) {
				if (g_exclusiveMode) {
				
					if ( !IsChildWin( (HWND)lParam ) )
					if ( !IsIconic( m_hwnd ) )
					{
						AltTabOut();
						errcode = AUI_ERRCODE_HANDLED;
					}
				} else {
					if ( !IsChildWin( (HWND)lParam ) )
					{
						AltTabOut();
						errcode = AUI_ERRCODE_HANDLED;
					}
				}
			}
		}
		break;

	case WM_KEYDOWN: 
#ifdef _DEBUG
		{
		static RECT resizer = { 0, 0, 0, 0 };

		switch ( (int)wParam )
		{
		case VK_ESCAPE:
			
			if ( m_editMode )
				g_ui->SetEditRegion( NULL );
			break;

		case VK_F1:
			
			if ( GetKeyState( VK_SHIFT ) < 0 ) {	
				if ( m_editMode ) {
					aui_Region::PurgeUndoList();
					SetEditMode( FALSE );
				} else {
					SetEditMode( TRUE );
				}
			}
			
			errcode = AUI_ERRCODE_HANDLED;
			break;

		case VK_F2:
			
			if ( m_editMode ) {
				m_ldl->GetLdl()->WriteData();
				errcode = AUI_ERRCODE_HANDLED;
			}
			break;

		case 'Z':
			if ( m_editMode ) {
				if ( GetKeyState( VK_CONTROL ) < 0 ) {
					aui_Region::UndoEdit( );
					errcode = AUI_ERRCODE_HANDLED;
				}
			}
			break;
		
		case VK_LEFT: 
			if ( m_editMode ) {
				if ( GetKeyState( VK_SHIFT ) < 0 )
					SetRect( &resizer, -1, 0, 0, 0 );
				else if ( GetKeyState( VK_CONTROL ) < 0 )
					SetRect( &resizer, 0, 0, -1, 0 );
				else 
					SetRect( &resizer, -1, 0, -1, 0 );

				aui_Region::EditModeModifyRegion( resizer );

				errcode = AUI_ERRCODE_HANDLED;
			}
			break;

		case VK_UP:
			if ( m_editMode ) {
				if ( GetKeyState( VK_SHIFT ) < 0 )
					SetRect( &resizer, 0, -1, 0, 0 );
				else if ( GetKeyState( VK_CONTROL ) < 0 )
					SetRect( &resizer, 0, 0, 0, -1 );
				else
					SetRect( &resizer, 0, -1, 0, -1 );
			
				aui_Region::EditModeModifyRegion( resizer );

				errcode = AUI_ERRCODE_HANDLED;
			}
			break;

		case VK_RIGHT:
			if ( m_editMode ) {
				if ( GetKeyState( VK_SHIFT ) < 0 )
					SetRect( &resizer, 0, 0, 1, 0 );
				else if ( GetKeyState( VK_CONTROL ) < 0 )
					SetRect( &resizer, 1, 0, 0, 0 );
				else
					SetRect( &resizer, 1, 0, 1, 0 );
				
				aui_Region::EditModeModifyRegion( resizer );

				errcode = AUI_ERRCODE_HANDLED;
			}
			break;

		case VK_DOWN:
			if ( m_editMode ) {
				if ( GetKeyState( VK_SHIFT ) < 0 )
					SetRect( &resizer, 0, 0, 0, 1 );
				else if ( GetKeyState( VK_CONTROL ) < 0 )
					SetRect( &resizer, 0, 1, 0, 0 );
				else
					SetRect( &resizer, 0, 1, 0, 1 );

				aui_Region::EditModeModifyRegion( resizer );

				errcode = AUI_ERRCODE_HANDLED;
			}
			break;
		
		}
		}
#endif 
		break;
	}

	return errcode;
}



AUI_ERRCODE aui_UI::AltTabOut( void )
{
	if ( m_keyboard ) m_keyboard->Unacquire();
	if ( m_joystick ) m_joystick->Unacquire();

	if (m_mouse) {
		m_mouse->Suspend( FALSE );
		m_mouse->Unacquire();
	}

	if ( m_minimize )
		if ( m_primary )
		{
			delete m_primary;
			m_primary = NULL;
		}

	while ( ShowCursor( TRUE ) < 0 )
		; 

	if ( m_minimize )
	{
		SetCursorPos( m_mouse->X(), m_mouse->Y() );

		while ( !IsIconic( m_hwnd ) )
			::ShowWindow( m_hwnd, SW_MINIMIZE );
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_UI::AltTabIn( void )
{
	if ( m_minimize )
		while ( GetForegroundWindow() != m_hwnd )
			::ShowWindow( m_hwnd, SW_RESTORE );

	while ( ShowCursor( FALSE ) >= 0 )
		; 

	if ( !m_primary ) CreateScreen();

	if ( m_minimize )
	{
		POINT point;
		GetCursorPos( &point );
		m_mouse->SetPosition( &point );
	}

	m_mouse->Acquire();
	m_mouse->Resume();

	if ( m_joystick ) m_joystick->Acquire();
	if ( m_keyboard ) m_keyboard->Acquire();

	return FlushDirtyList();
}



BOOL aui_UI::MinimizeOnAltTabOut( BOOL minimize )
{
	BOOL wasMinimizing = m_minimize;
	m_minimize = minimize;
	return wasMinimizing;
}



AUI_ERRCODE aui_UI::Process( void )
{
	Idle();
	HandleMouseEvents();
	HandleKeyboardEvents();
	if ( m_joystick ) HandleJoystickEvents();
	HandleActions();
	HandleDestructiveActions();
	Draw();

	return AUI_ERRCODE_OK;
}




void aui_UI::HandleActions( void )
{
	for ( sint32 i = m_actionList->L(); i && m_actionList->L(); i-- )
	{
		aui_Action *action = m_actionList->RemoveHead();
		action->Execute( NULL, 0, 0 );
		delete action;
	}
}



void aui_UI::AddAction( aui_Action *action )
{
	Assert( action != NULL );
	if ( action ) m_actionList->AddTail( action );
}



void aui_UI::HandleDestructiveActions( void )
{
	for ( sint32 i = m_destructiveActionList->L(); i; i-- )
	{
		aui_Action *action = m_destructiveActionList->RemoveHead();
		action->Execute( NULL, 0, 0 );
		delete action;
	}
}


void aui_UI::AddDestructiveAction( aui_Action *action )
{
	Assert( action != NULL );
	if ( action ) m_destructiveActionList->AddTail( action );
}


AUI_ERRCODE aui_UI::TagMouseEvents( sint32 numEvents, aui_MouseEvent *events )
{
	if ( numEvents )
	{
		aui_MouseEvent *baseEvent = events;
		aui_MouseEvent *thisEvent = baseEvent + 1;

		
		
		
		
		BOOL moveCount = 0;
		for ( sint32 i = numEvents - 1; i; i--, thisEvent++ )
		{
			
			if ( baseEvent->lbutton == thisEvent->lbutton
			&&   baseEvent->rbutton == thisEvent->rbutton )
			{
				
				moveCount++;
			}
			else
			{
				
				for ( sint32 j = moveCount; (baseEvent++)->movecount = j; j-- );

				
				moveCount = 0;
			}
		}

		
		for ( i = moveCount; (baseEvent++)->movecount = i; i-- );

		
		
		

		for ( i = numEvents - 1; (events++)->framecount = i; i-- );
	}

	return AUI_ERRCODE_OK;
}
