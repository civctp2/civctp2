


#ifndef __C3_CHECKBOX_H__
#define __C3_CHECKBOX_H__

#include "patternbase.h"
#include "aui_switch.h"

#define k_C3_CHECKBOX_DEFAULTNUMSTATES		2
#define k_C3_CHECKBOX_DEFAULT_BEVELWIDTH		2
#define k_C3_CHECKBOX_LDL_BEVELWIDTH			"bevelwidth"

class aui_Surface;


class c3_CheckBox : public aui_Switch, public PatternBase
{
public:
	
	c3_CheckBox(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	c3_CheckBox(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *text,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		sint32 state = 0,
		sint32 numStates = k_C3_CHECKBOX_DEFAULTNUMSTATES );
	virtual ~c3_CheckBox() {}

	void SetBevelWidth(uint32 w) { m_bevelWidth = w; };
protected:
	c3_CheckBox() : aui_Switch() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( sint32 bevelWidth  );

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

private:
	sint32	m_bevelWidth;
};


#endif 
