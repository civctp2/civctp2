#include "ctp/c3.h"

#include "ui/aui_common/aui_ldl.h"
#include "ui/aui_ctp2/c3ui.h"
#include "ui/aui_ctp2/ctp2_Window.h"
#include "ui/aui_ctp2/ctp2_button.h"

#include "ui/aui_common/aui.h"
#include "ui/aui_common/aui_control.h"

#include "ui/aui_ctp2/pattern.h"
#include "ui/aui_utils/primitives.h"

#include "ui/interface/victorywindow.h"
#include "ui/interface/victorywin.h"

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
