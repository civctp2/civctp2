









#include "c3.h"

#include "aui.h"
#include "aui_ui.h"
#include "aui_ldl.h"
#include "aui_window.h"
#include "aui_blitter.h"

#include "CivPaths.h"

#include "picture.h"
#include "primitives.h"
#include "staticpicture.h"

extern CivPaths		*g_civPaths;
extern aui_UI		*g_ui;


StaticPicture::StaticPicture(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	MBCHAR *picture )
:
	aui_ImageBase( ldlBlock ),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_Static      (retval, id, ldlBlock),
    m_picture       (NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( ldlBlock, picture );
	Assert( AUI_SUCCESS(*retval) );
}



StaticPicture::StaticPicture(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *picture )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase(NULL),
	aui_Static      (retval, id, x, y, width, height),
    m_picture       (NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( picture );
	Assert( AUI_SUCCESS(*retval) );
}



AUI_ERRCODE StaticPicture::InitCommon( MBCHAR *ldlBlock, MBCHAR *picture )
{
    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	
	MBCHAR *name =
		block->GetString( picture ? picture : k_AUI_STATICIMAGE_LDL_IMAGE );
	Assert( name != NULL );
	if ( name )
	{
		AUI_ERRCODE errcode = InitCommon( name );
		Assert( AUI_SUCCESS(errcode) );
		return errcode;
	}
	
	return AUI_ERRCODE_OK;
}



AUI_ERRCODE StaticPicture::InitCommon( MBCHAR *picture )
{
	MBCHAR filename[_MAX_PATH];

	if (g_civPaths->FindFile(C3DIR_PICTURES, picture, filename)) 
    {
	    AUI_ERRCODE errcode;
		m_picture = new Picture(&errcode, filename);
	} else {
		m_picture = NULL;
	}

	Assert( m_picture != NULL );

	return AUI_ERRCODE_OK;
}



StaticPicture::~StaticPicture()
{
	delete m_picture;
}



AUI_ERRCODE StaticPicture::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };

	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	primitives_BevelRect16( surface, &rect, 2, 1, 16, 16);
	InflateRect( &rect, -2, -2 );

	if ( m_picture )
		m_picture->Draw( surface, &rect );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

void StaticPicture::SetPicture(MBCHAR *picture)
{
	MBCHAR filename[_MAX_PATH];
	
    delete m_picture;
	if (g_civPaths->FindFile(C3DIR_PICTURES, picture, filename)) 
    {
	    AUI_ERRCODE errcode;
		m_picture = new Picture(&errcode, filename);
	} 
    else 
    {
		m_picture = NULL;
	}

	RECT rect = { 0, 0, m_width, m_height };
	ToWindow(&rect);
	m_window->AddDirtyRect(&rect);
}

