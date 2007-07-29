#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __PICTURETAB_H__
#define __PICTURETAB_H__

#include "aui_tab.h"

class Picture;

class PictureTab : public aui_Tab
{
public:
	
	PictureTab(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 paneWidth,
		sint32 paneHeight,
		Picture *pictureOn,
		Picture *pictureOff,
		Picture *pictureActiveOn,
		Picture *pictureActiveOff,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		BOOL selected = FALSE );
	virtual ~PictureTab() {}

	
	Picture *&ThePictureOn( void ) { return m_pictureOn; }
	Picture *&ThePictureOff( void ) { return m_pictureOff; }
	Picture *&ThePictureActiveOn( void ) { return m_pictureActiveOn; }
	Picture *&ThePictureActiveOff( void ) { return m_pictureActiveOff; }

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	Picture *m_pictureOn;
	Picture *m_pictureOff;
	Picture *m_pictureActiveOn;
	Picture *m_pictureActiveOff;
};

#endif 
