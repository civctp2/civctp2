












#pragma once
#ifndef __C3TEXTFIELD_H__
#define __C3TEXTFIELD_H__


#include "aui_textfield.h"
#include "patternbase.h"

#define k_C3_TEXTFIELD_DEFAULT_BEVELWIDTH		2
#define k_C3_TEXTFIELD_LDL_BEVELWIDTH			"bevelwidth"



class C3TextField : public aui_TextField, public PatternBase
{
public:
	
	C3TextField(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	C3TextField(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		MBCHAR *text = NULL,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~C3TextField() {}

	void SetBevelWidth(uint32 w) { m_bevelWidth = w; };

protected:
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( sint32 bevelWidth  );

public:
	virtual AUI_ERRCODE	DrawThis(
		aui_Surface *surface,
		sint32 x,
		sint32 y );

private:
	sint32	m_bevelWidth;
};


#endif 
