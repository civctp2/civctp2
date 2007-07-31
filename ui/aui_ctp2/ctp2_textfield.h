












#pragma once
#ifndef __ctp2_TEXTFIELD_H__
#define __ctp2_TEXTFIELD_H__


#include "aui_textfield.h"
#include "patternbase.h"

#define k_CTP2_TEXTFIELD_DEFAULT_BEVELWIDTH		2
#define k_CTP2_TEXTFIELD_LDL_BEVELWIDTH			"bevelwidth"



class ctp2_TextField : public aui_TextField, public PatternBase
{
public:
	
	ctp2_TextField(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	ctp2_TextField(
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
	virtual ~ctp2_TextField() {}

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
