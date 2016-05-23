#include "c3.h"

#include "aui_ldl.h"
#include "c3ui.h"
#include "ctp2_Window.h"
#include "ctp2_button.h"

#include "aui.h"
#include "aui_control.h"

#include "pattern.h"
#include "primitives.h"

#include "victorywindow.h"
#include "victorywin.h"

extern C3UI *g_c3ui;

static MBCHAR *s_VictoryWindowBlock = "VictoryWindow";
























VictoryWindow::VictoryWindow(AUI_ERRCODE *err)
{
	*err = AUI_ERRCODE_OK;
	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_VictoryWindowBlock);
	Assert(m_window);
	if(!m_window) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}


}

VictoryWindow::~VictoryWindow()
{
	if(m_window) {
		aui_Ldl::DeleteHierarchyFromRoot(s_VictoryWindowBlock);
	}
}
