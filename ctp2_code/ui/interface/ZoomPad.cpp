#include "ctp/c3.h"

#include "ui/interface/ZoomPad.h"

#include "gfx/tilesys/tiledmap.h"
#include "ui/aui_common/aui_ldl.h"
#include "ui/aui_ctp2/ctp2_button.h"

ZoomPad::ZoomPad(MBCHAR *ldlBlock) :
m_zoomIn(static_cast<ctp2_Button*>(aui_Ldl::GetObject(ldlBlock, "ZoomPad.ZoomInButton"))),
m_zoomOut(static_cast<ctp2_Button*>(aui_Ldl::GetObject(ldlBlock, "ZoomPad.ZoomOutButton")))
{

	Assert(m_zoomIn);
	Assert(m_zoomOut);

	m_zoomIn->SetActionFuncAndCookie(ZoomInButtonActionCallback, NULL);
	m_zoomOut->SetActionFuncAndCookie(ZoomOutButtonActionCallback, NULL);

	Update();
}


void ZoomPad::Update()
{

	if(g_tiledMap) {

		m_zoomIn->Enable(g_tiledMap->CanZoomIn());
		m_zoomOut->Enable(g_tiledMap->CanZoomOut());
	}
}

void ZoomPad::ZoomInButtonActionCallback(aui_Control *control, uint32 action,
										 uint32 data, void *cookie)
{

	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	if(g_tiledMap) {

		g_tiledMap->ZoomIn();
	}
}

void ZoomPad::ZoomOutButtonActionCallback(aui_Control *control, uint32 action,
										  uint32 data, void *cookie)
{

	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	if(g_tiledMap) {

		g_tiledMap->ZoomOut();
	}
}
