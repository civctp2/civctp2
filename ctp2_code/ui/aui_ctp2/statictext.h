












#pragma once
#ifndef __STATICTEXT_H__
#define __STATICTEXT_H__


#include "aui_static.h"
#include "textbase.h"

#define k_STATICTEXT_LDL_TEXTSIZE	"textsize"	


class StaticText : public aui_Static, public TextBase
{
public:
	
	StaticText(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock);
	StaticText(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *text = NULL,
		uint32 maxLength = 0,
		uint32 size = 0);
	virtual ~StaticText() {}

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	AUI_ERRCODE InitCommon( MBCHAR *ldlBlock );

	
	sint32	m_border;

public:
	
	void DrawTextBorder( BOOL type ) { m_border = type; }

};


#endif 
