#ifndef __AUI_STATIC_H__
#define __AUI_STATIC_H__

#include "aui_control.h"


class aui_Static : public aui_Control
{
public:

	aui_Static(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock );
	aui_Static(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		const MBCHAR *text = NULL,
		uint32 maxLength = 0 );
	virtual ~aui_Static() {}

protected:
	aui_Static() : aui_Control() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );
};

#endif
