#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __C3_HEADER_H__
#define __C3_HEADER_H__

#include "c3_listbox.h"
#include "aui_header.h"

class c3_Header : public aui_Header
{
public:

	c3_Header(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock );
	c3_Header(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height );
	virtual ~c3_Header();

protected:
	c3_Header() : aui_Header() {}
	AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE CreateSwitches(const MBCHAR *ldlBlock = NULL);
};

#endif
