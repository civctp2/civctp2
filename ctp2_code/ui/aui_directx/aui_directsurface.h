#ifndef __AUI_DIRECTSURFACE_H__
#define __AUI_DIRECTSURFACE_H__

#ifdef __AUI_USE_DIRECTX__

#include "aui_surface.h"
#include "aui_directx.h"


class aui_DirectSurface : public aui_Surface, public aui_DirectX
{
public:

	aui_DirectSurface(
		AUI_ERRCODE *retval,
		sint32 width,
		sint32 height,
		sint32 bpp,
		LPDIRECTDRAW lpdd,
		LPDIRECTDRAWSURFACE lpdds = NULL,
		BOOL isPrimary = FALSE,
		BOOL useVideoMemory = FALSE );
	virtual ~aui_DirectSurface();

protected:
	aui_DirectSurface() : aui_Surface() {}
	AUI_ERRCODE InitCommon( void );

public:
	virtual BOOL IsThisA( uint32 classId )
	{
		return classId == m_directSurfaceClassId
		||     aui_Surface::IsThisA( classId )
		||     aui_DirectX::IsThisA( classId );
	}

	virtual uint32 SetChromaKey( uint32 color );

	virtual AUI_ERRCODE Lock( RECT *rect, LPVOID *buffer, DWORD flags );
	virtual AUI_ERRCODE Unlock( LPVOID buffer );

	virtual AUI_ERRCODE GetDC( HDC *hdc );
	virtual AUI_ERRCODE ReleaseDC( HDC hdc );

	LPDIRECTDRAWSURFACE	DDS( void ) const { return m_lpdds; }
	BOOL				IsDCGot( void ) const { return m_dcIsGot; }

	virtual BOOL IsOK( void ) const;

	static uint32 m_directSurfaceClassId;

protected:
	LPDIRECTDRAWSURFACE	m_lpdds;
};

#endif

#endif
