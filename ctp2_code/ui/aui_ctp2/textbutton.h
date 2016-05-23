#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TEXTBUTTON_H__
#define __TEXTBUTTON_H__




#include "c3_button.h"

#define k_TEXTBUTTON_MAXTEXT	64


class TextButton : public c3_Button
{
public:

	TextButton(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	TextButton(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern = NULL,
		MBCHAR *text = NULL,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~TextButton() {}

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
};

#endif
