#ifndef __C3_HEADERSWITCH_H__
#define __C3_HEADERSWITCH_H__

#include "aui_radio.h"
#include "patternbase.h"

#define k_C3_HEADERSWITCH_DEFAULTNUMSTATES	3

#define k_C3_HEADERSWITCH_IMAGE		"headerimage"

class c3_Static;

class c3_HeaderSwitch : public aui_Radio, public PatternBase
{
public:

	c3_HeaderSwitch(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	c3_HeaderSwitch(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		const MBCHAR *text,
		const MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		sint32 state = 0,
		sint32 numStates = k_C3_HEADERSWITCH_DEFAULTNUMSTATES );
	virtual ~c3_HeaderSwitch();

	c3_Static *GetImage( void ) const { return m_image; }

protected:
	c3_HeaderSwitch() : aui_Radio() {}
	AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);
	AUI_ERRCODE InitCommon( void );

protected:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	c3_Static *m_image;
};

#endif
