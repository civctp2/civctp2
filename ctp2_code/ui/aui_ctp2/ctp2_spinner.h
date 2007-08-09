//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Call to Power 2 user interface spinner
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Allowed optional text display instead of a number (May 21th 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef CTP2_SPINNER_H__
#define CTP2_SPINNER_H__

#include "aui_ranger.h"


class ctp2_Spinner : public aui_Ranger
{
public:
	
	ctp2_Spinner(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

protected:
	ctp2_Spinner() : aui_Ranger() {}

public:
	static void ActionCallback(aui_Control *control, uint32 state, uint32 data, void *cookie);

	void SetDisplay();

	AUI_ERRCODE SetSpinnerCallback(ControlActionCallback *actionFunc, void *cookie) {
		m_spinnerActionFunc = actionFunc;
		m_spinnerCookie = cookie;
		return AUI_ERRCODE_OK;
	}

protected:
	ControlActionCallback	*m_spinnerActionFunc;
	void					*m_spinnerCookie;
};


#endif 
