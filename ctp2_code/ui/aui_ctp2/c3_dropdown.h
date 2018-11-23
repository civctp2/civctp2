#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __C3_DROPDOWN_H__
#define __C3_DROPDOWN_H__

#include "aui_dropdown.h"
#include "patternbase.h"

class c3_DropDown : public aui_DropDown, public PatternBase
{
public:

	c3_DropDown(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	c3_DropDown(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		const MBCHAR *pattern,
		sint32 buttonSize = 0,
		sint32 windowSize = 0,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~c3_DropDown() {};

	void Clear(void);

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	c3_DropDown() : aui_DropDown() {}
	AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);
	AUI_ERRCODE InitCommon( sint32 buttonSize, sint32 windowSize );
	AUI_ERRCODE CreateComponents(const MBCHAR *ldlBlock = NULL);

protected:
	virtual AUI_ERRCODE	RepositionButton( void );
	virtual AUI_ERRCODE	RepositionListBoxWindow( void );
};

#endif
