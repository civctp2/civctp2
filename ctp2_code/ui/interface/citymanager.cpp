#include "c3.h"
#include "aui.h"
#include "aui_window.h"
#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_blitter.h"

#include "ctp2_button.h"

#include "UIUtils.h"

#include "citymanager.h"
#include "c3ui.h"

#include "StrDB.h"

extern C3UI *g_c3ui;

static CityManagerWindow *s_cityManagerWindow = NULL;

void CityManagerWindow::Open()
{
	AUI_ERRCODE err;
	if(!s_cityManagerWindow) {
		s_cityManagerWindow = new CityManagerWindow(&err,
													aui_UniqueId(),
													"CITY_MANAGER_WINDOW");
		Assert(err == AUI_ERRCODE_OK);

		g_c3ui->AddWindow(s_cityManagerWindow);
	}

	s_cityManagerWindow->Show();
}

void CityManagerWindow::Cleanup()
{
	if(s_cityManagerWindow) {
		s_cityManagerWindow->Hide();
		g_c3ui->RemoveWindow(s_cityManagerWindow->Id());
		delete s_cityManagerWindow;
		s_cityManagerWindow = NULL;
	}
}

CityManagerWindow::CityManagerWindow(AUI_ERRCODE *retval,
									 uint32 id,
									 MBCHAR *ldlBlock)
	: aui_Window(retval, id,
				 uiutils_ChooseLdl(ldlBlock, "CITY_MANAGER_WINDOW"),
				 16,
				 AUI_WINDOW_TYPE_STANDARD)
{
	m_ok = m_cancel = NULL;
	m_bg = NULL;
	ldlBlock = uiutils_ChooseLdl(ldlBlock, "CITY_MANAGER_WINDOW");

	*retval = InitCommonLdl(ldlBlock);

}

CityManagerWindow::~CityManagerWindow()
{
	if(m_ok) {
		RemoveControl(m_ok);
		m_ok = NULL;
	}

	if(m_cancel) {
		RemoveControl(m_cancel);
		m_cancel = NULL;
	}

	if(m_bg) {
		g_ui->UnloadImage(m_bg);
		m_bg = NULL;
	}
}

void CityManagerWindowButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;
	if(s_cityManagerWindow)
		s_cityManagerWindow->Hide();
}

AUI_ERRCODE CityManagerWindow::InitCommonLdl(MBCHAR *ldlBlock)
{
	aui_Ldl *theLdl = g_c3ui->GetLdl();
	ldl_datablock *theBlock = NULL;

	if(theLdl->IsValid(ldlBlock)) {
		theBlock = theLdl->GetLdl()->FindDataBlock(ldlBlock);
	} else {
		return AUI_ERRCODE_HACK;
	}

	MBCHAR controlBlock[k_AUI_LDL_MAXBLOCK + 1];
	AUI_ERRCODE ret;


	sprintf(controlBlock, "%s.%s", ldlBlock, "OK_BUTTON");
	m_ok = new ctp2_Button(&ret, aui_UniqueId(), controlBlock,
						   "CTP2_BUTTON_TEXT_RIGHT_LARGE",
						   386, 414,
						   100, 20,

						   CityManagerWindowButtonCallback,
						   this);
	AddControl(m_ok);

	sprintf(controlBlock, "%s.%s", ldlBlock, "CANCEL_BUTTON");
	m_cancel = new ctp2_Button(&ret, aui_UniqueId(), controlBlock,
							   "CTP2_BUTTON_TEXT_RIGHT_LARGE",
							   526, 414,
							   100, 20,

							   CityManagerWindowButtonCallback,
							   this);
	AddControl(m_cancel);


	m_bg = g_ui->LoadImage("CM.tga");

       	Assert(m_bg);
	if(m_bg) {
		Resize(m_bg->TheSurface()->Width(), m_bg->TheSurface()->Height());
	}

	SetDraggable(TRUE);

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE CityManagerWindow::DrawThis(aui_Surface *surface, sint32 x, sint32 y)
{
	if(IsHidden()) return AUI_ERRCODE_OK;

	if(!surface) surface = m_surface;

	AUI_ERRCODE err = AUI_ERRCODE_OK;

	if(m_bg) {
		aui_Surface *srcSurf = m_bg->TheSurface();
		RECT srcRect = {0, 0, srcSurf->Width(), srcSurf->Height()};

		err =
			g_ui->TheBlitter()->Blt(surface,
									0, 0,
									srcSurf,
									&srcRect,
									k_AUI_BLITTER_FLAG_COPY);
	}

	RECT rect = {0,0,m_width,m_height};

	if(surface == m_surface)
		AddDirtyRect(&rect);

	return err;
}
