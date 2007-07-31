










#ifndef __NS_HEADER_H__
#define __NS_HEADER_H__

#include "ns_listbox.h"
#include "aui_header.h"


class ns_Header : public aui_Header
{
public:
	
	ns_Header(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock );
	ns_Header(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height );
	virtual ~ns_Header() {}

protected:
	ns_Header() : aui_Header() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE CreateSwitches( MBCHAR *ldlBlock = NULL );
};


#endif 
