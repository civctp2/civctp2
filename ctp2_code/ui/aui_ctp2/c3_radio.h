#ifndef __C3_RADIO_H__
#define __C3_RADIO_H__

#include "patternbase.h"
#include "aui_radio.h"

#define k_C3_RADIO_DEFAULTNUMSTATES		2
#define k_C3_RADIO_DEFAULT_BEVELWIDTH		2
#define k_C3_RADIO_LDL_BEVELWIDTH			"bevelwidth"
#define k_C3_RADIO_LDL_BEVELTYPE			"beveltype"

class aui_Surface;

class c3_Radio : public aui_Radio, public PatternBase
{
public:

	c3_Radio(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	c3_Radio(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		sint32 state = 0,
		sint32 numStates = k_C3_RADIO_DEFAULTNUMSTATES );
	virtual ~c3_Radio() {}

	void SetBevelWidth(uint32 w) { m_bevelWidth = w; };
protected:
	c3_Radio() : aui_Radio() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( sint32 bevelWidth  );

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

private:
	sint32	m_bevelWidth;
	sint32	m_bevelType;
};

#endif
