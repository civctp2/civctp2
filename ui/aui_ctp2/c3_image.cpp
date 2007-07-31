



#include "c3.h"

#include "aui.h"
#include "aui_ui.h"

#include "c3_image.h"


c3_Image::c3_Image(
	AUI_ERRCODE *retval,
	MBCHAR *filename )
	:
	aui_Image( retval, filename )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	InitCommon();
}


AUI_ERRCODE c3_Image::InitCommon( void )
{
	return AUI_ERRCODE_OK;
}


BOOL c3_Image::PtOnImage( POINT *p )
{
	AUI_ERRCODE errcode;
	BOOL isOn = FALSE;
	uint8 *buf;

	
	if (m_surface->Buffer() == NULL) {
		errcode = m_surface->Lock( NULL, (LPVOID *)&buf, 0 );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return FALSE;
	} 
	else {
		buf = (uint8 *)m_surface->Buffer();
	}

	
	uint16 *test = (uint16 *)( buf + p->y * m_surface->Pitch() + (p->x << 1) );
	if ( *test ) {
		isOn = TRUE;
	}

	
	errcode = m_surface->Unlock( (LPVOID)buf );
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return FALSE;

	return isOn;
}

