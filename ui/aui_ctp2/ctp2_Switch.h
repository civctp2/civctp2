



#ifndef __CTP2_SWITCH_H__
#define __CTP2_SWITCH_H__

#include "patternbase.h"
#include "aui_switch.h"

#define k_CTP2_SWITCH_DEFAULTNUMSTATES		2
#define k_CTP2_SWITCH_DEFAULT_BEVELWIDTH		2
#define k_CTP2_SWITCH_LDL_BEVELWIDTH			"bevelwidth"

class aui_Surface;


class ctp2_Switch : public aui_Switch, public PatternBase
{
public:
	
	ctp2_Switch(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	ctp2_Switch(
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
		sint32 numStates = k_CTP2_SWITCH_DEFAULTNUMSTATES );
	virtual ~ctp2_Switch() {}

	void SetBevelWidth(uint32 w) { m_bevelWidth = w; };
	sint32 BevelWidth( void ) const { return m_bevelWidth; }
protected:
	ctp2_Switch() : aui_Switch() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( sint32 bevelWidth  );

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	virtual MouseEventCallback MouseLDoubleClickInside;

private:
	sint32	m_bevelWidth;
};


#endif 
