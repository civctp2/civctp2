//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface
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
// -None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Import structure changed to compile with Mingw
// - Moved CalculateHash() to aui_Base
// - Prevented processing of uninitialised input
// - Added graphics DirectX built in double buffering and extended it
//   to manual tripple buffering. (1-Jan-2010 Martin G�hmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef AUI_UI_H_
#define AUI_UI_H_

//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

#include <windows.h>		// HINSTANCE etc.

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

class			aui_UI;

extern class aui_UI *	g_ui;

#define			k_AUI_UI_NOCOLOR	0xff000000

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

#include "aui_action.h"			// aui_Action
#include "aui_audiomanager.h"	// aui_AudioManager
#include "aui_bitmapfont.h"		// aui_BitmapFont
#include "aui_blitter.h"		// aui_Blitter
#include "aui_control.h"		// aui_Control
#include "aui_cursor.h"			// aui_Cursor
#include "aui_dirtylist.h"		// aui_DirtyList
#include "aui_image.h"			// aui_Image
#include "aui_joystick.h"		// aui_Joystick
#include "aui_keyboard.h"		// aui_Keyboard
#include "aui_ldl.h"			// aui_Ldl
#include "aui_memmap.h"			// aui_MemMap
#include "aui_mouse.h"			// aui_Mouse, aui_MouseEvent
#include "aui_moviemanager.h"	// aui_MovieManager
#include "aui_region.h"			// aui_Region
#include "aui_resource.h"		// aui_Resource
#include "aui_sound.h"			// aui_Sound
#include "aui_static.h"			// aui_Static
#include "aui_surface.h"		// aui_Surface
#include "aui_window.h"			// aui_Window
#include "auitypes.h"			// AUI_...
#include "c3types.h"			// MBCHAR, sint32, uint32
#include "tech_wllist.h"		// tech_WLList

class aui_Movie;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class aui_UI : public aui_Region
{
public:

	aui_UI(
		AUI_ERRCODE *retval,
		HINSTANCE hinst,
		HWND hwnd,
		sint32 width,
		sint32 height,
		sint32 bpp,
		const MBCHAR *ldlFilename  = NULL);
	virtual ~aui_UI();

protected:
	aui_UI()
	:
		aui_Region                  (),
		m_dirtyRectInfoMemory       (NULL),
		m_dirtyRectInfoList         (NULL),
		m_hinst                     ((HINSTANCE) INVALID_HANDLE_VALUE),
		m_hwnd                      ((HWND) INVALID_HANDLE_VALUE),
		m_bpp                       (0),
		m_pixelFormat               (AUI_SURFACE_PIXELFORMAT_UNKNOWN),
		m_ldl                       (NULL),
		m_primary                   (NULL),
		m_secondary                 (NULL),
		m_blitter                   (NULL),
		m_memmap                    (NULL),
		m_mouse                     (NULL),
		m_keyboard                  (NULL),
		m_joystick                  (NULL),
		m_dirtyList                 (NULL),
		m_color                     (k_AUI_UI_NOCOLOR),
		m_image                     (NULL),
		m_colorAreas                (NULL),
		m_imageAreas                (NULL),
		m_virtualFocus              (NULL),
		m_dxver                     (0),
		m_editMode                  (false),
		m_editRegion                (NULL),
		m_editWindow                (NULL),
		m_localRectText             (NULL),
		m_absoluteRectText          (NULL),
		m_editModeLdlName           (NULL),
		m_imageResource             (NULL),
		m_cursorResource            (NULL),
		m_bitmapFontResource        (NULL),
		m_audioManager              (NULL),
		m_movieManager              (NULL),
		m_actionList                (NULL),
		m_destructiveActionList     (NULL),
		m_winList                   (NULL),
		m_minimize                  (false),
		m_savedMouseAnimFirstIndex  (0),
		m_savedMouseAnimLastIndex   (0),
		m_savedMouseAnimCurIndex    (0),
		m_savedMouseAnimDelay       (0)
	{};

	AUI_ERRCODE InitCommon(
		HINSTANCE hinst,
		HWND hwnd,
		sint32 bpp,
		const MBCHAR *ldlFilename );
	AUI_ERRCODE CreateScreen( void );

public:
	void RegisterObject( aui_Blitter *blitter );
	void RegisterObject( aui_Mouse *mouse );
	void RegisterObject( aui_Keyboard *keyboard );
	void RegisterObject( aui_MemMap *memmap );
	void RegisterObject( aui_AudioManager *audioManager );
	void RegisterObject( aui_MovieManager *movieManager );
	void RegisterObject( aui_Joystick *joystick );

	COLORREF	SetBackgroundColor( COLORREF color );
	aui_Image	*SetBackgroundImage(
		aui_Image *image,
		sint32 x = 0,
		sint32 y = 0 );

	HINSTANCE	TheHINSTANCE( void ) const { return m_hinst; }
	HWND		TheHWND( void ) const { return m_hwnd; }
	aui_Ldl		*GetLdl( void ) const { return m_ldl; }

	AUI_ERRCODE BltToSecondary
	                          (
	                           sint32       destx,
	                           sint32       desty,
	                           aui_Surface *srcSurf,
	                           RECT        *srcRect,
	                           uint32       flags
	                          )
	{
		Assert(m_secondary);
		return m_blitter->Blt(m_secondary, destx, desty, srcSurf, srcRect, flags);
	};

	AUI_ERRCODE BltSecondaryToPrimary
	                        (
	                         uint32       flags
	                        );

	AUI_ERRCODE ColorBltToSecondary
	                             (
	                              RECT     *destRect,
	                              COLORREF  color,
	                              uint32    flags
	                             )
	{
		return m_blitter->ColorBlt(m_secondary, destRect, color, flags);
	};

	sint32 PrimaryHeight()  { return m_primary->Height(); };
	sint32 PrimaryWidth()   { return m_primary->Width(); };
	sint32 SecondaryHeight(){ return m_secondary->Height(); };
	sint32 SecondaryWidth() { return m_secondary->Width(); };
	bool HasPrimary()       { return m_primary   != NULL; };
	bool HasSecondary()     { return m_secondary != NULL; };

	AUI_ERRCODE BlackScreen()
	{
		if(m_primary != NULL)
		{
			RECT rect = {0, 0, PrimaryWidth(), PrimaryHeight()};
			return m_blitter->ColorBlt(m_primary, &rect, RGB(0,0,0), 0);
		}
		else
		{
			return AUI_ERRCODE_OK;
		}
	}

	AUI_ERRCODE ClearSecondary()
	{
		RECT rect = {0, 0, SecondaryWidth(), SecondaryHeight()};
		return m_blitter->ColorBlt(m_secondary, &rect, RGB(0,0,0), 0);
	}

	aui_Surface		*Secondary( void ) const { return m_secondary; }
	aui_Surface		*Primary( void ) const { return m_primary; }
	aui_Blitter		*TheBlitter( void ) const { return m_blitter; }
	aui_MemMap		*TheMemMap( void ) const { return m_memmap; }
	aui_Mouse		*TheMouse( void ) const { return m_mouse; }
	aui_Keyboard	*TheKeyboard( void ) const { return m_keyboard; }
	aui_Joystick	*TheJoystick( void ) const { return m_joystick; }

	sint32 BitsPerPixel( void ) const { return m_bpp; }
	AUI_SURFACE_PIXELFORMAT PixelFormat( void ) { return m_pixelFormat; }

	uint32			DXVer( void ) const { return m_dxver; }

	aui_DirtyList	*GetDirtyList( void ) { return m_dirtyList; }

	AUI_ERRCODE		FlushDirtyList( void );

	aui_Resource<aui_Image> *GetImageResource( void ) const
		{ return m_imageResource; }

	aui_Image	*LoadImage( const MBCHAR *name )
		{ return m_imageResource->Load( name, C3DIR_PICTURES ); }

	AUI_ERRCODE	UnloadImage( aui_Image *resource )
		{ return m_imageResource->Unload( resource ); }
	AUI_ERRCODE	UnloadImage( const MBCHAR *name )
		{ return m_imageResource->Unload( name ); }

	AUI_ERRCODE	AddImageSearchPath( const MBCHAR *path )
		{ return m_imageResource->AddSearchPath( path ); }
	AUI_ERRCODE	RemoveImageSearchPath( const MBCHAR *path )
		{ return m_imageResource->RemoveSearchPath( path ); }

	aui_Resource<aui_Cursor> *GetCursorResource( void ) const
		{ return m_cursorResource; }

	aui_Cursor	*LoadCursor( const MBCHAR *name )
		{ return m_cursorResource->Load( name, C3DIR_CURSORS ); }

	AUI_ERRCODE	UnloadCursor( aui_Cursor *resource )
		{ return m_cursorResource->Unload( resource ); }
	AUI_ERRCODE	UnloadCursor( const MBCHAR *name )
		{ return m_cursorResource->Unload( name ); }

	AUI_ERRCODE	AddCursorSearchPath( const MBCHAR *path )
		{ return m_cursorResource->AddSearchPath( path ); }
	AUI_ERRCODE	RemoveCursorSearchPath( const MBCHAR *path )
		{ return m_cursorResource->RemoveSearchPath( path ); }

	aui_Resource<aui_BitmapFont> *GetBitmapFontResource( void ) const
		{ return m_bitmapFontResource; }




	aui_BitmapFont	*LoadBitmapFont( const MBCHAR *name, uint32 size = 0 )
		{ return m_bitmapFontResource->Load( name, C3DIR_DIRECT, size ); }

	AUI_ERRCODE	UnloadBitmapFont( aui_BitmapFont *resource )
		{ return m_bitmapFontResource->Unload( resource ); }
	AUI_ERRCODE	UnloadBitmapFont( const MBCHAR *name )
		{ return m_bitmapFontResource->Unload( name ); }

	AUI_ERRCODE	AddBitmapFontSearchPath( const MBCHAR *path )
		{ return m_bitmapFontResource->AddSearchPath( path ); }
	AUI_ERRCODE	RemoveBitmapFontSearchPath( const MBCHAR *path )
		{ return m_bitmapFontResource->RemoveSearchPath( path ); }

	aui_AudioManager *TheAudioManager( void ) const { return m_audioManager; }

	aui_Sound	*LoadSound( const MBCHAR *name )
		{ return m_audioManager ? m_audioManager->Load( name ) : NULL; }

	AUI_ERRCODE	UnloadSound( aui_Sound *resource )
		{ return m_audioManager ? m_audioManager->Unload( resource ) : AUI_ERRCODE_HACK; }
	AUI_ERRCODE	UnloadSound( const MBCHAR *name )
		{ return m_audioManager ? m_audioManager->Unload( name ) : AUI_ERRCODE_HACK; }

	AUI_ERRCODE	AddSoundSearchPath( const MBCHAR *path )
		{ return m_audioManager ? m_audioManager->AddSearchPath( path ) : AUI_ERRCODE_HACK; }
	AUI_ERRCODE	RemoveSoundSearchPath( const MBCHAR *path )
		{ return m_audioManager ? m_audioManager->RemoveSearchPath( path ) : AUI_ERRCODE_HACK; }

	aui_MovieManager *TheMovieManager( void ) const { return m_movieManager; }

	aui_Movie	*LoadMovie( const MBCHAR *name)
		{ return m_movieManager ? m_movieManager->Load( name, C3DIR_VIDEOS  ) : NULL; }

	AUI_ERRCODE	UnloadMovie( aui_Movie *resource )
		{ return m_movieManager ? m_movieManager->Unload( resource ) : AUI_ERRCODE_HACK; }
	AUI_ERRCODE	UnloadMovie( const MBCHAR *name )
		{ return m_movieManager ? m_movieManager->Unload( name ) : AUI_ERRCODE_HACK; }

	AUI_ERRCODE	AddMovieSearchPath( const MBCHAR *path )
		{ return m_movieManager ? m_movieManager->AddSearchPath( path ) : AUI_ERRCODE_HACK; }
	AUI_ERRCODE	RemoveMovieSearchPath( const MBCHAR *path )
		{ return m_movieManager ? m_movieManager->RemoveSearchPath( path ) : AUI_ERRCODE_HACK; }

	aui_Window		*TopWindow( void ) const
	{ return m_childList->L() ? (aui_Window *)m_childList->GetHead() : NULL; }
	aui_Window		*BringWindowToTop( uint32 windowId );
	aui_Window		*BringWindowToTop( aui_Window *window );

	virtual AUI_ERRCODE DrawOne( aui_Window *window );

	AUI_ERRCODE Draw( void );

	AUI_ERRCODE	AddWindow( aui_Window *window )
	{ return AddChild( (aui_Region *)window ); }
	AUI_ERRCODE	RemoveWindow( uint32 windowId )
	{ return RemoveChild( windowId ); }
	aui_Window	*GetWindow( uint32 windowId )
	{ return (aui_Window *)GetChild( windowId ); }
	virtual AUI_ERRCODE	AddChild( aui_Region *child );
	virtual AUI_ERRCODE	RemoveChild( uint32 windowId );

	AUI_ERRCODE	ShowWindow( uint32 windowId );
	AUI_ERRCODE	HideWindow( uint32 windowId );

	virtual AUI_ERRCODE	Idle( aui_Region *recurse = NULL );

	AUI_ERRCODE	Invalidate( RECT *rect = NULL );

	AUI_ERRCODE AddDirtyRect( RECT *rect );
	AUI_ERRCODE AddDirtyRect( sint32 left, sint32 top, sint32 right, sint32 bottom );

	AUI_ERRCODE HandleMouseEvents(
		sint32 numEvents = 0,
		aui_MouseEvent *events = NULL );
	AUI_ERRCODE HandleKeyboardEvents( void );
	AUI_ERRCODE HandleJoystickEvents( void );
	virtual AUI_ERRCODE HandleWindowsMessage(
		HWND hwnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam );
	virtual AUI_ERRCODE Process( void );

	void AddAction( aui_Action *action );
	void HandleActions( void );







	void AddDestructiveAction(aui_Action *action);
	void HandleDestructiveActions( void );

	virtual AUI_ERRCODE AltTabOut( void );
	virtual AUI_ERRCODE AltTabIn( void );
	BOOL	MinimizeOnAltTabOut( BOOL minimize );

	BOOL		IsChildWin( HWND hwnd ) const
	{ return (m_winList->Find(hwnd) ? TRUE : FALSE); }
	AUI_ERRCODE	AddWin( HWND hwnd );
	void		RemoveWin( HWND hwnd );

	aui_Region	*TheEditRegion( void ) const { return m_editRegion; }
	void		SetEditRegion( aui_Region *region );
	void		SetEditMode( BOOL mode );
	BOOL		GetEditMode( void ) { return m_editMode; }
	AUI_ERRCODE ShowSelectedRegion( aui_Region *region );
	RECT		TheEditRect( void ) const { return m_editRect; }
	AUI_ERRCODE	CreateEditModeDialog( BOOL bMake );

	struct DirtyRectInfo
	{
		RECT rect;
		aui_Window *window;
	};

	tech_WLList<DirtyRectInfo *> *GetDirtyRectInfoList( void )
	{ return m_dirtyRectInfoList; }

protected:
	AUI_ERRCODE	TagMouseEvents( sint32 numEvents, aui_MouseEvent *events );

	virtual AUI_ERRCODE ClipAndConsolidate( void );

#ifdef __AUI_USE_SDL__
	virtual AUI_ERRCODE SDLDrawScreen( void ) {}
#endif

	AUI_ERRCODE InsertDirtyRectInfo( RECT *rect, aui_Window *window );
	void FlushDirtyRectInfoList( void );

	tech_Memory<DirtyRectInfo>		*m_dirtyRectInfoMemory;
	tech_WLList<DirtyRectInfo *>	*m_dirtyRectInfoList;

	HINSTANCE		m_hinst;
	HWND			m_hwnd;
	sint32			m_bpp;
	AUI_SURFACE_PIXELFORMAT m_pixelFormat;

	aui_Ldl			*m_ldl;

	aui_Surface		*m_primary;
	aui_Surface		*m_secondary;
	aui_Blitter		*m_blitter;
	aui_MemMap		*m_memmap;
	aui_Mouse		*m_mouse;
	aui_Keyboard	*m_keyboard;
	aui_Joystick	*m_joystick;
	aui_DirtyList	*m_dirtyList;

	COLORREF		m_color;
	aui_Image		*m_image;
	RECT			m_imageRect;
	aui_DirtyList	*m_colorAreas;
	aui_DirtyList	*m_imageAreas;

	aui_Control		*m_virtualFocus;

	DWORD			m_dxver;

	BOOL			m_editMode;
	aui_Region		*m_editRegion;
	RECT			m_editRect;
	aui_Window		*m_editWindow;
	aui_Static		*m_localRectText;
	aui_Static		*m_absoluteRectText;
	aui_Static		*m_editModeLdlName;

	aui_Resource<aui_Image>			*m_imageResource;
	aui_Resource<aui_Cursor>		*m_cursorResource;
	aui_Resource<aui_BitmapFont>	*m_bitmapFontResource;
	aui_AudioManager				*m_audioManager;
	aui_MovieManager				*m_movieManager;

	tech_WLList<aui_Action *>	*m_actionList;

	tech_WLList<aui_Action *>	*m_destructiveActionList;

	tech_WLList<HWND>			*m_winList;

	BOOL m_minimize;




	sint32			m_savedMouseAnimFirstIndex;
	sint32			m_savedMouseAnimLastIndex;
	sint32			m_savedMouseAnimCurIndex;
	sint32			m_savedMouseAnimDelay;
};

#endif
