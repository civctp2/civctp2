












#pragma once
#ifndef __TEXTBOX_H__
#define __TEXTBOX_H__


#include "aui_textbox.h"
#include "patternbase.h"


class aui_Surface;


#define k_TEXTBOX_MAXTEXT		256		
#define k_TEXTBOX_MAXITEMS		200		
#define k_TEXTBOX_LDL_TEXTSIZE	"textsize"	


class TextBox : public aui_TextBox, public PatternBase
{
public:
	
	TextBox(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL);
	TextBox(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		MBCHAR *text = NULL,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL);
	virtual ~TextBox() {}

	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( BOOL fromLDL );

	virtual void		SetTextFont(MBCHAR *name) { aui_TextBox::SetTextFont(name); }
	virtual void		SetTextFontSize( uint8 size ) { aui_TextBox::SetTextFontSize(size); }
	virtual AUI_ERRCODE	RepositionItems( void );

protected:
	AUI_ERRCODE	CreateRangers( MBCHAR *ldlBlock );

public:

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
};


#endif 
