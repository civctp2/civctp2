






#include "c3.h"


#include "ZoomPad.h"


#include "TiledMap.h"
#include "aui_Ldl.h"
#include "ctp2_Button.h"


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
