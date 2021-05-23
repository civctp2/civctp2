//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : CTP2 drop down menu
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// -None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - ForceSelect can now be disabled. (Feb 4th 2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CTP2_DROPDOWN_H__
#define __CTP2_DROPDOWN_H__

#include "aui_dropdown.h"
#include "patternbase.h"

class ctp2_ListItem;

class ctp2_DropDown : public aui_DropDown, public PatternBase
{
public:
	ctp2_DropDown(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	ctp2_DropDown(
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
	virtual ~ctp2_DropDown() {};

	void	Clear(void);

	virtual AUI_ERRCODE Draw(aui_Surface * surface, sint32 x, sint32 y);
	virtual AUI_ERRCODE DrawThis(aui_Surface * surface = NULL, sint32 x = 0, sint32 y = 0 );
	virtual AUI_ERRCODE DoneInstantiatingThis(const MBCHAR * ldlBlock);

	AUI_ERRCODE		AddItem(ctp2_ListItem *item);


	void			BuildListStart(void);


	void			BuildListEnd(void);
	void			SetForceSelect(bool forceSelect);

protected:
	ctp2_DropDown() : aui_DropDown() {}
	AUI_ERRCODE		InitCommonLdl( const MBCHAR *ldlBlock );
	AUI_ERRCODE		InitCommon( sint32 buttonSize, sint32 windowSize );
	AUI_ERRCODE		CreateComponents( const MBCHAR *ldlBlock = NULL );

protected:
	virtual AUI_ERRCODE	RepositionButton( void );
	virtual AUI_ERRCODE	RepositionListBoxWindow( void );
private:
	sint32 SetWindowSizeThis();
};

#endif
