











#pragma once
#ifndef __PICTURESWITCH_H__
#define __PICTURESWITCH_H__

#include "aui_switch.h"

class Pattern;
class Picture;

class PictureSwitch : public aui_Switch
{
public:
	
	PictureSwitch(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		Picture *pictureOn = NULL,
		Picture *pictureOff = NULL,
		Picture *pictureActiveOn = NULL,
		Picture *pictureActiveOff = NULL,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		BOOL selected = FALSE );
	virtual ~PictureSwitch() {}

	
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