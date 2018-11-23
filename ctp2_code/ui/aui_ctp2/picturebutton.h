#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __PICTUREBUTTON_H__
#define __PICTUREBUTTON_H__

#include "aui_button.h"

class Picture;


class PictureButton : public aui_Button
{
public:

	PictureButton(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		const MBCHAR *upPicture,
		const MBCHAR *downPicture,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

	PictureButton(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

	virtual ~PictureButton();

	AUI_ERRCODE InitCommon(const MBCHAR *upPicture, const MBCHAR *downPicture, BOOL isLDL = FALSE);

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );


	Picture *&UpPicture( void ) { return m_upPicture; }
	Picture *&DownPicture( void ) { return m_downPicture; }

protected:
	Picture *m_upPicture;
	Picture *m_downPicture;
};

#endif
