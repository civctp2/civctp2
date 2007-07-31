










#ifndef __AUI_HEADERSWITCH_H__
#define __AUI_HEADERSWITCH_H__


#include "aui_radio.h"
#include "patternbase.h"



#define k_NS_HEADERSWITCH_LDL_ICON			"icon"

#define k_NS_HEADERSWITCH_DEFAULTNUMSTATES	3	



class ns_HeaderSwitch : public aui_Radio, public PatternBase
{
public:
	
	ns_HeaderSwitch(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	ns_HeaderSwitch(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *text,
		MBCHAR *icon,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		sint32 state = 0,
		sint32 numStates = k_NS_HEADERSWITCH_DEFAULTNUMSTATES );
	virtual ~ns_HeaderSwitch();

protected:
	ns_HeaderSwitch() : aui_Radio() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( MBCHAR *icon );

protected:
	AUI_ERRCODE	SetIcon( MBCHAR *icon );
	aui_Image	*GetIcon( void ) const { return m_icon; }

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	aui_Image	*m_icon;
};


#endif 
