












#include "c3.h"

#include "aui.h"
#include "aui_blitter.h"
#include "aui_surface.h"

#include "c3ui.h"
#include "pixelutils.h"

#include "pattern.h"


extern C3UI *g_c3ui;



Pattern::Pattern(
	AUI_ERRCODE *retval,
	MBCHAR const * filename,
	MBCHAR const * lightFilename,
	MBCHAR const * darkFilename )
:
	aui_Image( retval, filename )
{
	m_lightImage = new aui_Image( retval, lightFilename );
	m_darkImage = new aui_Image( retval, darkFilename );
}

Pattern::Pattern(
	AUI_ERRCODE *retval,
	MBCHAR const *filename )
:
	aui_Image( retval, filename ),
	m_lightImage( NULL ),
	m_darkImage( NULL )
{
}







Pattern::~Pattern( void )
{
	if ( m_lightImage )
	{
		delete m_lightImage;
		m_lightImage = NULL;
	}

	if ( m_darkImage )
	{
		delete m_darkImage;
		m_darkImage = NULL;
	}
}








AUI_ERRCODE Pattern::Draw( aui_Surface *pDestSurf, RECT *pDestRect )
{
	if (!m_surface) return AUI_ERRCODE_OK;

	RECT rect = { 0, 0, m_surface->Width(), m_surface->Height() };
	return g_c3ui->TheBlitter()->TileBlt(
		pDestSurf,
		pDestRect,
		m_surface,
		&rect,
		0,
		0,
		k_AUI_BLITTER_FLAG_COPY );
	


































































































}







AUI_ERRCODE Pattern::Draw( aui_Surface *pDestSurf, RECT *pDestRect, RECT *pSrcRect )
{
	if (!m_surface) return AUI_ERRCODE_OK;

	RECT rect = { 0, 0, m_surface->Width(), m_surface->Height() };
	return g_c3ui->TheBlitter()->TileBlt(
		pDestSurf,
		pDestRect,
		m_surface,
		pSrcRect,
		0,
		0,
		k_AUI_BLITTER_FLAG_COPY );
}







AUI_ERRCODE Pattern::DrawDither( aui_Surface *pDestSurf, RECT *pDestRect, BOOL flag )
{
	RECT rect = { 0, 0, m_surface->Width(), m_surface->Height() };
	return g_c3ui->TheBlitter()->TileBlt(
		pDestSurf,
		pDestRect,
		m_surface,
		&rect,
		0,
		0,
		k_AUI_BLITTER_FLAG_COPY );
	





















































































































































}








AUI_ERRCODE Pattern::DrawDither(
	aui_Surface *pDestSurf,		
	RECT *pDestRect,		
	BOOL flag,				
	sint32 lighten,			
	sint32 darken			
	)
{
	RECT rect = { 0, 0, m_surface->Width(), m_surface->Height() };
	return g_c3ui->TheBlitter()->TileBlt(
		pDestSurf,
		pDestRect,
		m_surface,
		&rect,
		0,
		0,
		k_AUI_BLITTER_FLAG_COPY );
	











































































































































}
