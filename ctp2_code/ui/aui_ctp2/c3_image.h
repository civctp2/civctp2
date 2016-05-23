#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3_IMAGE_H__
#define __C3_IMAGE_H__

#include "aui_image.h"

class c3_Image : public aui_Image
{
public:

	c3_Image(
		AUI_ERRCODE *retval,
		MBCHAR *filename = NULL );
	virtual ~c3_Image() {}

	BOOL PtOnImage( POINT *p );

protected:
	c3_Image() : aui_Image() {}
	AUI_ERRCODE InitCommon( void );
};

#endif
