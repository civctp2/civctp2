











#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __ICON_H__
#define __ICON_H__


#include "aui_image.h"

#include "CivPaths.h"

extern CivPaths		*g_civPaths;


class Icon : public aui_Image
{
public:
	
	Icon(
		AUI_ERRCODE *retval,
		MBCHAR *szFileName = NULL);

	virtual ~Icon();

	
	AUI_ERRCODE Draw( aui_Surface *pDestSurf, RECT *pDestRect, uint32 color );

};


#endif 
