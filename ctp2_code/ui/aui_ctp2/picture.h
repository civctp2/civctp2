











#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __PICTURE_H__
#define __PICTURE_H__


#include "aui_image.h"


class aui_Surface;




class Picture : public aui_Image
{
public:
	
	Picture(
		AUI_ERRCODE *retval,
		MBCHAR *szFileName = NULL );
	virtual ~Picture();

	
	aui_Surface *TheMipmap( void ) const { return m_mipmap; }

	
	AUI_ERRCODE Draw( aui_Surface *pDestSurf, RECT *pDestRect );

	AUI_ERRCODE MakeMipmap( void );

protected:
	uint16 AveragePixels( uint16 *pBuffer, sint32 width );

	aui_Surface *m_mipmap;	
};


#endif 
