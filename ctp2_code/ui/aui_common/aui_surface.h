//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Surface (part of the screen)
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
// USE_SDL
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected a reported memory leak.
// - Added back buffering capability. (1-Jan-2010 Martin Gühmann)
//
//----------------------------------------------------------------------------
#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __AUI_SURFACE_H__
#define __AUI_SURFACE_H__

#include "aui_base.h"
#ifdef USE_SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#endif

enum AUI_SURFACE_PIXELFORMAT
{
	AUI_SURFACE_PIXELFORMAT_FIRST = 0,
	AUI_SURFACE_PIXELFORMAT_UNKNOWN = 0,
	AUI_SURFACE_PIXELFORMAT_332,
	AUI_SURFACE_PIXELFORMAT_555,
	AUI_SURFACE_PIXELFORMAT_565,
	AUI_SURFACE_PIXELFORMAT_888,
	AUI_SURFACE_PIXELFORMAT_LAST
};

struct aui_SurfaceSubset
{
	LPVOID	buffer;
	RECT	rect;
};

#define k_SURFACE_MAXLOCK 4

enum AUI_SURFACE_LOCKOP
{
	AUI_SURFACE_LOCKOP_FIRST = 0,
	AUI_SURFACE_LOCKOP_CHECK = 0,
	AUI_SURFACE_LOCKOP_ADD,
	AUI_SURFACE_LOCKOP_REMOVE,
	AUI_SURFACE_LOCKOP_LAST
};

class aui_Surface : public aui_Base
{
public:

	aui_Surface(
		AUI_ERRCODE *retval,
		sint32 width,
		sint32 height,
		sint32 bpp,
		sint32 pitch = 0,
		uint8 *buffer = NULL,
		BOOL isPrimary = FALSE,
		HDC hdc = NULL);
	virtual ~aui_Surface();

protected:
	aui_Surface() : aui_Base() {}
	AUI_ERRCODE InitCommon( sint32 width, sint32 height, sint32 bpp, BOOL isPrimary );

public:
	static AUI_SURFACE_PIXELFORMAT TransformBppToSurfacePixelFormat(int bpp);

	virtual BOOL IsThisA( uint32 classId )
	{
		return classId == m_surfaceClassId;
	}

	sint32 Width( void ) const { return m_width; }
	sint32 Height( void ) const { return m_height; }
	sint32 BitsPerPixel( void ) const { return m_bpp; }
	sint32 BytesPerPixel( void ) const { return m_Bpp; }
	sint32 Bytewidth( void ) const { return m_bytewidth; }
	sint32 Pitch( void ) const { return m_pitch; }
	sint32 Size( void ) const { return m_size; }
	uint8 *Buffer( void ) const { return m_buffer; }
	AUI_SURFACE_PIXELFORMAT PixelFormat( void ) const { return m_pixelFormat; }

	uint32 GetChromaKey( void ) const { return m_chromaKey; }
	virtual uint32 SetChromaKey( uint32 color );
	uint32 SetChromaKey( uint8 red, uint8 green, uint8 blue );

	BOOL IsPrimary( void ) const { return m_isPrimary; }


	virtual AUI_ERRCODE Lock( RECT *rect, LPVOID *buffer, DWORD flags );
	virtual AUI_ERRCODE Unlock( LPVOID buffer );

#ifndef USE_SDL
	virtual AUI_ERRCODE GetDC( HDC *hdc );
	virtual AUI_ERRCODE ReleaseDC( HDC hdc );
#endif

#ifdef USE_SDL
	SDL_mutex *LPCS( void ) const { return m_cs; };
#else
	LPCRITICAL_SECTION LPCS( void ) const { return &m_cs; };
#endif

	virtual BOOL IsOK( void ) const { return m_saveBuffer != NULL; }

	virtual AUI_ERRCODE BlankRGB(const uint8 &red, const uint8 &green, const uint8 &blue);
	virtual AUI_ERRCODE Blank(const uint32 &color);

	virtual void Flip(){};
	virtual void ReverseFlip(){};

	static uint32 m_surfaceClassId;

protected:
	static sint32 m_surfaceRefCount;
#ifdef USE_SDL
	static SDL_mutex *m_cs;
#else
	static	CRITICAL_SECTION	m_cs;
#endif
	AUI_ERRCODE ManipulateLockList( RECT *rect, LPVOID *buffer, AUI_SURFACE_LOCKOP op );

	sint32	m_width;
	sint32	m_height;
	sint32	m_bpp;
	sint32	m_Bpp;
	sint32	m_bytewidth;
	sint32	m_pitch;
	sint32	m_size;
	uint8	*m_buffer;

	HDC		m_hdc;
	bool	m_dcIsGot;
	HBITMAP	m_hbitmap;
	HBITMAP	m_holdbitmap;

	AUI_SURFACE_PIXELFORMAT m_pixelFormat;
	uint32	m_chromaKey;

	BOOL	m_isPrimary;

	BOOL	m_allocated;
	uint8	*m_saveBuffer;

	aui_SurfaceSubset m_locklist[ k_SURFACE_MAXLOCK ];
	sint32	m_locksRemain;

private:

	BOOL IsLocked( RECT *rect );
	BOOL IsLocked( LPVOID buffer );
};

#endif
