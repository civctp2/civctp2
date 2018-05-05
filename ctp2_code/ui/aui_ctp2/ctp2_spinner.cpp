//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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

#include "ctp/c3.h"

#include "ui/aui_common/aui.h"
#include "ui/aui_common/aui_uniqueid.h"
#include "ui/aui_common/aui_surface.h"
#include "ui/aui_common/aui_window.h"
#include "ui/aui_common/aui_ranger.h"
#include "ui/aui_common/aui_ldl.h"

#include "ui/aui_ctp2/c3ui.h"
#include "ui/aui_ctp2/c3thumb.h"
#include "ui/aui_ctp2/textbutton.h"
#include "ui/aui_ctp2/pattern.h"

#include "ui/aui_utils/primitives.h"

#include "ui/aui_ctp2/ctp2_spinner.h"
#include "ui/aui_ctp2/ctp2_Static.h"
#include <string>

extern C3UI *g_c3ui;


ctp2_Spinner::ctp2_Spinner(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie,
	bool displayValue )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_Ranger(retval, id, ldlBlock),
	m_displayValue(displayValue)
{
	m_spinnerActionFunc = ActionFunc;
	m_spinnerCookie = cookie;

	aui_Ranger::SetActionFuncAndCookie(ctp2_Spinner::ActionCallback, this);

	SetDisplay();
}

void ctp2_Spinner::ActionCallback(aui_Control *control, uint32 state, uint32 data, void *cookie)
{
	ctp2_Spinner *spinner = (ctp2_Spinner *)cookie;

	Assert(spinner);
	if(!spinner)
		return;

	if(spinner->m_spinnerActionFunc)
		spinner->m_spinnerActionFunc(control, state, data, spinner->m_spinnerCookie);

	if(state == AUI_RANGER_ACTION_VALUECHANGE)
		spinner->SetDisplay();
}

void ctp2_Spinner::SetDisplay()
{
	if(m_displayValue)
	{
		static char s[20];
		sprintf(s, "%d",
			((m_orientation == AUI_RANGER_ORIENTATION_VERTICAL) ?
			GetValueY() : GetValueX()));

		m_rangeContainer->SetText(s);
	}
	else
	{
		m_rangeContainer->SetText(GetText());
	}
}
