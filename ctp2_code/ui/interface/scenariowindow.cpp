//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Scenario selection window
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
// - Removed refferences to the civilisation database. (Aug 21st 2005 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_ldl.h"
#include "aui_surface.h"
#include "aui_uniqueid.h"
#include "aui_imagebase.h"
#include "aui_textbase.h"
#include "aui_textfield.h"
#include "aui_stringtable.h"
#include "c3ui.h"
#include "ctp2_Window.h"
#include "ctp2_button.h"
#include "ctp2_Static.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "ctp2_dropdown.h"
#include "ctp2_textfield.h"

#include "StrDB.h"

#include "civscenarios.h"

#include "spnewgamewindow.h"
#include "scenariowindow.h"

#include "profileDB.h"

#include "loadsavewindow.h"
#include "scenarioeditor.h"

#include "civapp.h"

extern StringDB                     *g_theStringDB;
extern CivScenarios                 *g_civScenarios;
extern C3UI                         *g_c3ui;
extern sint32                       g_startInfoType;
extern CivApp                       *g_civApp;

ScenarioWindow                      *s_ScenarioWindow = NULL;

extern MBCHAR                       g_scenarioName[k_SCENARIO_NAME_MAX];








ScenarioWindow::ScenarioWindow(AUI_ERRCODE *retval, MBCHAR *ldlBlock)
{
	Assert(AUI_SUCCESS(*retval));

	m_scenario = NULL;
	m_scenarioPack = NULL;

	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(ldlBlock);





	m_available		= (ctp2_ListBox *)aui_Ldl::GetObject(ldlBlock, "AvailableListBox");
	m_available->SetActionFuncAndCookie(ScenarioSelect, NULL);

	m_window->SetType(AUI_WINDOW_TYPE_FLOATING);
	m_window->SetStronglyModal(TRUE);

	m_scenInstructions = (ctp2_Static *)aui_Ldl::GetObject(ldlBlock, "ScenInstructions");
	m_packInstructions = (ctp2_Static *)aui_Ldl::GetObject(ldlBlock, "PackInstructions");

	m_LoadButton = (ctp2_Button *)aui_Ldl::GetObject(ldlBlock, "LoadButton");
	m_SaveButton = (ctp2_Button *)aui_Ldl::GetObject(ldlBlock, "SaveButton");
	m_NewButton = (ctp2_Button *)aui_Ldl::GetObject(ldlBlock, "NewButton");
	m_BackButton = (ctp2_Button *)aui_Ldl::GetObject(ldlBlock, "CancelButton");

	m_LoadButton->SetActionFuncAndCookie(OkPress, NULL);
	m_SaveButton->SetActionFuncAndCookie(SavePress, NULL);
	m_NewButton->SetActionFuncAndCookie(NewPress, NULL);
	m_BackButton->SetActionFuncAndCookie(BackPress, NULL);












	FillListWithScenarioPacks(m_available);
	m_mode = SCENARIO_WINDOW_MODE_LOAD_PACK;
	m_scenInstructions->Hide();
	m_packInstructions->Show();

	m_exitCallback = NULL;

	m_newPackWindow = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot("NewPackWindow");
	aui_Ldl::SetActionFuncAndCookie("NewPackWindow.OkButton", NewPackOk, NULL);
	aui_Ldl::SetActionFuncAndCookie("NewPackWindow.CancelButton", NewPackCancel, NULL);
	m_newPackWindow->SetType(AUI_WINDOW_TYPE_POPUP);

	m_newScenWindow = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot("NewScenWindow");
	aui_Ldl::SetActionFuncAndCookie("NewScenWindow.OkButton", NewScenOk, NULL);
	aui_Ldl::SetActionFuncAndCookie("NewScenWindow.CancelButton", NewScenCancel, NULL);
	m_newScenWindow->SetType(AUI_WINDOW_TYPE_POPUP);
}

void ScenarioWindow::FillListWithScenarios(ctp2_ListBox *available)
{
	int					i=0;
	ScenarioPack		*scenPack;
	MBCHAR				*ldlBlock = "ScenarioListItem";

	scenPack = m_scenarioPack;

	if ( scenPack ) {
		for (i=0; i<scenPack->m_numScenarios; i++) {
			Scenario *scen = &scenPack->m_scenarios[i];

			ctp2_ListItem	*item=NULL;
			item = (ctp2_ListItem *) aui_Ldl::BuildHierarchyFromRoot(ldlBlock);
			Assert(item);
			if (item) {
				ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
				Assert(box);
				if(box) {
					ctp2_Static *name = (ctp2_Static *)box->GetChildByIndex(0);
					if(name) {
						name->SetText(scen->m_name);
					}

					ctp2_Static *description = (ctp2_Static *)box->GetChildByIndex(1);
					if(description) {
						description->SetText(scen->m_description);
					}

					ctp2_Static *image = (ctp2_Static *)box->GetChildByIndex(2);
					if(image) {
						MBCHAR imPath[_MAX_PATH];
						sprintf(imPath, "%s\\%s", scen->m_path, "scenicon.tga");
						if(c3files_PathIsValid(imPath)) {
							image->SetImage(imPath);
						}
					}
				}
				item->SetUserData((void *)scen);

				available->AddItem(item );
			}
		}
	}
}

void ScenarioWindow::FillListWithScenarioPacks(ctp2_ListBox *available,bool hideOriginalScenarios)
{
	int					i=0;
	ScenarioPack		*scenPack;
//	MBCHAR				*ldlBlock = "ScenarioPackListItem";
	MBCHAR checkFile[_MAX_PATH];
#ifdef WIN32
	struct _stat fileStatus;
#else
	struct stat fileStatus;
#endif

	for (i=0; i<g_civScenarios->GetNumScenarioPacks(); i++) {
		scenPack = g_civScenarios->GetScenarioPack(i);

		sprintf(checkFile,"%s%s%s",scenPack->m_path, FILE_SEP, "Activision.txt");
#ifdef WIN32
		if(!(hideOriginalScenarios && !_stat(checkFile,&fileStatus)))
#else
		if(!(hideOriginalScenarios && stat(checkFile,&fileStatus) == 0))
#endif
		{
			ctp2_ListItem	*item=NULL;
			item = (ctp2_ListItem*) aui_Ldl::BuildHierarchyFromRoot("ScenarioPackListItem");
			Assert(item);
			if (item) {
				ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
				if(box) {
					ctp2_Static *name = (ctp2_Static *)box->GetChildByIndex(0);
					if(name) {
						name->SetText(scenPack->m_name);
					}

					ctp2_Static *description = (ctp2_Static *)box->GetChildByIndex(1);
					if(description) {
						description->SetText(scenPack->m_description);
					}

					ctp2_Static *image = (ctp2_Static *)box->GetChildByIndex(2);
					if(image) {
						MBCHAR imPath[_MAX_PATH];
						sprintf(imPath, "%s\\%s", scenPack->m_path, "packicon.tga");
						if(c3files_PathIsValid(imPath)) {
							image->SetImage(imPath);
						}
					}
				}
				item->SetUserData(scenPack);
				available->AddItem(item );
			}
		}
	}
}


void ScenarioWindow::SetScenario(Scenario *scenario)
{
	Assert(scenario != NULL);

	m_scenario = scenario;


}



void ScenarioWindow::SetMode(SCENARIO_WINDOW_MODE mode)
{
	m_available->Clear();
	m_LoadButton->Enable(FALSE);
	m_SaveButton->Enable(FALSE);

	switch(mode) {
		case SCENARIO_WINDOW_MODE_LOAD_SCEN:


			FillListWithScenarios(m_available);
			m_packInstructions->Hide();
			m_scenInstructions->Show();
			m_LoadButton->Show();
			m_SaveButton->Hide();
			m_NewButton->Hide();
			break;
		case SCENARIO_WINDOW_MODE_LOAD_PACK:


			FillListWithScenarioPacks(m_available);
			m_packInstructions->Show();
			m_scenInstructions->Hide();
			m_LoadButton->Show();
			m_SaveButton->Hide();
			m_NewButton->Hide();
			break;
		case SCENARIO_WINDOW_MODE_SAVE_SCEN:
			FillListWithScenarios(m_available);
			m_packInstructions->Hide();
			m_scenInstructions->Show();
			m_LoadButton->Hide();
			m_SaveButton->Show();
			m_NewButton->Show();
			break;
		case SCENARIO_WINDOW_MODE_SAVE_PACK:
			FillListWithScenarioPacks(m_available,true);
			m_packInstructions->Show();
			m_scenInstructions->Hide();
			m_LoadButton->Show();
			m_SaveButton->Hide();
			m_NewButton->Show();
			break;

	}
	m_mode = mode;
	s_ScenarioWindow->m_window->ShouldDraw(TRUE);
}


ScenarioWindow::~ScenarioWindow()
{
	if(m_window) {
		aui_Ldl::DeleteHierarchyFromRoot("ScenarioWindow");
		m_window = NULL;
	}

	if(m_newPackWindow) {
		aui_Ldl::DeleteHierarchyFromRoot("NewPackWindow");
		m_newPackWindow = NULL;
	}

	if(m_newScenWindow) {
		aui_Ldl::DeleteHierarchyFromRoot("NewScenWindow");
		m_newScenWindow = NULL;
	}
}

void ScenarioWindow::Initialize()
{
	if(s_ScenarioWindow) return;

	AUI_ERRCODE retval = AUI_ERRCODE_OK;

	s_ScenarioWindow = new ScenarioWindow(&retval, "ScenarioWindow");
	Assert(retval == AUI_ERRCODE_OK);
}

void ScenarioWindow::Display(bool load)
{
	if(!s_ScenarioWindow) {
		Initialize();
	}

	Assert(s_ScenarioWindow);
	if(s_ScenarioWindow) {
		Assert(s_ScenarioWindow->m_window);
		if(s_ScenarioWindow->m_window) {
			g_c3ui->AddWindow(s_ScenarioWindow->m_window);
			s_ScenarioWindow->m_window->Show();

			if(load) {
				s_ScenarioWindow->SetMode(SCENARIO_WINDOW_MODE_LOAD_PACK);
			} else {
				s_ScenarioWindow->SetMode(SCENARIO_WINDOW_MODE_SAVE_PACK);
			}
		}
	}
}

void ScenarioWindow::Hide()
{
	if(!s_ScenarioWindow) return;

	Assert(s_ScenarioWindow->m_window);
	if(s_ScenarioWindow->m_window) {
		g_c3ui->RemoveWindow(s_ScenarioWindow->m_window->Id());
	}
}

void ScenarioWindow::Cleanup()
{
	if(!s_ScenarioWindow) return;

	Hide();

	delete s_ScenarioWindow;
	s_ScenarioWindow = NULL;
}

void ScenarioWindow::SetExitCallback(aui_Control::ControlActionCallback *callback)
{
	if(s_ScenarioWindow) {
		s_ScenarioWindow->m_exitCallback = callback;
	}
}

void ScenarioWindow::ScenarioSelect(aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if(s_ScenarioWindow) {
		ctp2_ListBox *mylistbox = (ctp2_ListBox*)control;




		if (s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_LOAD_PACK ||
			s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_SAVE_PACK) {
			ctp2_ListItem *myitem = (ctp2_ListItem *)mylistbox->GetSelectedItem();


			if (action == (uint32)AUI_LISTBOX_ACTION_SELECT) {
				if (myitem) {
					s_ScenarioWindow->SetScenarioPack((ScenarioPack *)myitem->GetUserData());
				}
			} else


			if (action == (uint32)AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {

				if (myitem) {
					s_ScenarioWindow->SetScenarioPack((ScenarioPack *)myitem->GetUserData());
				}

				if(s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_LOAD_PACK) {
					s_ScenarioWindow->SetMode(SCENARIO_WINDOW_MODE_LOAD_SCEN);
				} else {
					s_ScenarioWindow->SetMode(SCENARIO_WINDOW_MODE_SAVE_SCEN);
				}
			}
			if(mylistbox->GetSelectedItem()) {
				if(s_ScenarioWindow->m_LoadButton) {
					s_ScenarioWindow->m_LoadButton->Enable(TRUE);
					s_ScenarioWindow->m_SaveButton->Enable(TRUE);
				}
			} else {
				s_ScenarioWindow->m_LoadButton->Enable(FALSE);
				s_ScenarioWindow->m_SaveButton->Enable(FALSE);
			}
		} else


		if (s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_LOAD_SCEN ||
			s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_SAVE_SCEN) {
			ctp2_ListItem *myitem = (ctp2_ListItem *)mylistbox->GetSelectedItem();




			if (action == (uint32)AUI_LISTBOX_ACTION_SELECT) {
				if( myitem ) {

					s_ScenarioWindow->SetScenario((Scenario *)myitem->GetUserData());
				}
			} else


			if (action == (uint32)AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {
				if( myitem ) {

					s_ScenarioWindow->SetScenario((Scenario *)myitem->GetUserData());
				}

				if(s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_LOAD_SCEN) {


					SetProfileFromScenario();

					if (s_ScenarioWindow->GetExitCallback()) {
						s_ScenarioWindow->GetExitCallback()(control, action, data, cookie);
					} else {
						g_c3ui->AddAction(new CloseScenarioScreenAction);
					}

				} else {

				}
			}

			if(mylistbox->GetSelectedItem()) {
				if(s_ScenarioWindow->m_LoadButton) {
					s_ScenarioWindow->m_LoadButton->Enable(TRUE);
					s_ScenarioWindow->m_SaveButton->Enable(TRUE);
				}
			} else {
				s_ScenarioWindow->m_LoadButton->Enable(FALSE);
				s_ScenarioWindow->m_SaveButton->Enable(FALSE);
			}

		}
	}
}

void ScenarioWindow::CancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if (action != uint32(AUI_BUTTON_ACTION_EXECUTE)) return;

	if (s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_LOAD_SCEN) {
		s_ScenarioWindow->SetMode(SCENARIO_WINDOW_MODE_LOAD_PACK);
	} else
		if (s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_LOAD_PACK) {


			memset(g_scenarioName, '\0', k_SCENARIO_NAME_MAX);

			g_civPaths->ClearCurScenarioPath();
			g_civPaths->ClearCurScenarioPackPath();

			g_c3ui->AddAction(new CloseScenarioScreenAction);
		}
}

void ScenarioWindow::OkPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if (action != uint32(AUI_BUTTON_ACTION_EXECUTE)) return;

	if (s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_LOAD_SCEN) {
		if (!scenarioscreen_removeMyWindow(action)) return;

		SetProfileFromScenario();


		if (s_ScenarioWindow->GetExitCallback()) {
			s_ScenarioWindow->GetExitCallback()(control, action, data, cookie);
		} else {
			g_c3ui->AddAction(new CloseScenarioScreenAction);
		}
	} else if (s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_LOAD_PACK) {
		if(s_ScenarioWindow->m_scenarioPack) {
			s_ScenarioWindow->SetMode(SCENARIO_WINDOW_MODE_LOAD_SCEN);
		}
	} else if(s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_SAVE_PACK) {
		s_ScenarioWindow->SetMode(SCENARIO_WINDOW_MODE_SAVE_SCEN);
	} else {

		Assert(FALSE);
	}

}

void ScenarioWindow::SavePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if (action != uint32(AUI_BUTTON_ACTION_EXECUTE)) return;

	Assert(s_ScenarioWindow);
	if(!s_ScenarioWindow)
		return;

	Assert(s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_SAVE_SCEN);
	if(s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_SAVE_SCEN) {
		if(!s_ScenarioWindow->m_scenario) return;





		g_scenarioName[0] = 0;

		g_isScenario = TRUE;

		switch(ScenarioEditor::GetStartLocMode()) {
			case SCEN_START_LOC_MODE_NONE:
				g_startInfoType = STARTINFOTYPE_NOLOCS;
				break;
			case SCEN_START_LOC_MODE_PLAYER_WITH_CIV:
				g_startInfoType = STARTINFOTYPE_CIVSFIXED;
				break;
			case SCEN_START_LOC_MODE_PLAYER:
				g_startInfoType = STARTINFOTYPE_POSITIONSFIXED;
				break;
			case SCEN_START_LOC_MODE_CIV:
				g_startInfoType = STARTINFOTYPE_CIVS;
				break;
			default:
				Assert(FALSE);
				g_startInfoType = STARTINFOTYPE_NOLOCS;
				break;
		}

		loadsavescreen_Initialize();
		g_loadsaveWindow->SetType(LSS_SAVE_GAME);

		loadsavescreen_SaveGame(s_ScenarioWindow->m_scenario->m_path,
								k_SCENARIO_DEFAULT_SAVED_GAME_NAME);
	}
}

void ScenarioWindow::NewPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if (action != uint32(AUI_BUTTON_ACTION_EXECUTE)) return;

	Assert(s_ScenarioWindow);
	if(!s_ScenarioWindow)
		return;

	if(s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_SAVE_PACK) {
		Assert(s_ScenarioWindow->m_newPackWindow);
		if(s_ScenarioWindow->m_newPackWindow) {
			g_c3ui->AddWindow(s_ScenarioWindow->m_newPackWindow);
		}
	} else if(s_ScenarioWindow->GetMode() == SCENARIO_WINDOW_MODE_SAVE_SCEN) {
		Assert(s_ScenarioWindow->m_newScenWindow);
		if(s_ScenarioWindow->m_newScenWindow) {
			g_c3ui->AddWindow(s_ScenarioWindow->m_newScenWindow);
		}
	}
}

void ScenarioWindow::BackPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_ScenarioWindow);
	if(!s_ScenarioWindow)
		return;

	switch(s_ScenarioWindow->GetMode()) {
		case SCENARIO_WINDOW_MODE_SAVE_PACK:
			Hide();
			break;
		case SCENARIO_WINDOW_MODE_SAVE_SCEN:
			s_ScenarioWindow->SetMode(SCENARIO_WINDOW_MODE_SAVE_PACK);
			break;
		case SCENARIO_WINDOW_MODE_LOAD_PACK:
			Hide();
			break;
		case SCENARIO_WINDOW_MODE_LOAD_SCEN:
			s_ScenarioWindow->SetMode(SCENARIO_WINDOW_MODE_LOAD_PACK);
			break;
	}
}

void ScenarioWindow::SetProfileFromScenario( void )
{
	if (s_ScenarioWindow) {
		if (s_ScenarioWindow->GetScenario() != NULL) {

			g_civPaths->SetCurScenarioPath(s_ScenarioWindow->GetScenario()->m_path);


			g_civPaths->SetCurScenarioPackPath(s_ScenarioWindow->GetScenarioPack()->m_path);

			g_theProfileDB->SetIsScenario(TRUE);

			strcpy(g_scenarioName, s_ScenarioWindow->GetScenario()->m_name);

			g_civApp->CleanupAppDB();
			g_civApp->InitializeAppDB((*(CivArchive *)(NULL)));


















		}
	}
}

void ScenarioWindow::LoadScenarioGame( void )
{
	if (s_ScenarioWindow) {
		if (s_ScenarioWindow->GetScenario() != NULL) {

		}
	}
}

void ScenarioWindow::NewPackOk(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	MBCHAR dir[_MAX_PATH];
	MBCHAR name[_MAX_PATH];
	MBCHAR desc[_MAX_PATH];

	ctp2_TextField *field = (ctp2_TextField *)aui_Ldl::GetObject("NewPackWindow", "DirField");
	if(field) {
		field->GetFieldText(dir, _MAX_PATH - 1);
	} else {
		dir[0] = 0;
	}

	field = (ctp2_TextField *)aui_Ldl::GetObject("NewPackWindow", "NameField");
	if(field) {
		field->GetFieldText(name, _MAX_PATH - 1);
	} else {
		name[0] = 0;
	}

	field = (ctp2_TextField *)aui_Ldl::GetObject("NewPackWindow", "DescField");
	if(field) {
		field->GetFieldText(desc, _MAX_PATH - 1);
	} else {
		desc[0] = 0;
	}

	MBCHAR *d;

	for(d = desc; *d != 0; d++) {
		if((*d) == '\n' || (*d) == '\r')
			*d = ' ';
	}

	if((strlen(dir) < 1) || (strlen(name) < 1) || (strlen(desc) < 1)) {

		return;
	}

	CIV_SCEN_ERR err = g_civScenarios->MakeNewPack(dir, name, desc);
	Assert(err == CIV_SCEN_OK);

	Assert(s_ScenarioWindow);
	if(s_ScenarioWindow) {

		s_ScenarioWindow->SetMode(SCENARIO_WINDOW_MODE_SAVE_PACK);

		if(s_ScenarioWindow->m_newPackWindow) {
			g_c3ui->RemoveWindow(s_ScenarioWindow->m_newPackWindow->Id());
		}
	}
}

void ScenarioWindow::NewPackCancel(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	if(s_ScenarioWindow) {
		if(s_ScenarioWindow->m_newPackWindow) {
			g_c3ui->RemoveWindow(s_ScenarioWindow->m_newPackWindow->Id());
		}
	}
}

void ScenarioWindow::NewScenOk(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	MBCHAR name[_MAX_PATH];
	MBCHAR desc[_MAX_PATH];

	ctp2_TextField *field = (ctp2_TextField *)aui_Ldl::GetObject("NewScenWindow", "NameField");
	if(field) {
		field->GetFieldText(name, _MAX_PATH - 1);
	} else {
		name[0] = 0;
	}

	field = (ctp2_TextField *)aui_Ldl::GetObject("NewScenWindow", "DescField");
	if(field) {
		field->GetFieldText(desc, _MAX_PATH - 1);
	} else {
		desc[0] = 0;
	}

	MBCHAR *d;

	for(d = desc; *d != 0; d++) {
		if((*d) == '\n' || (*d) == '\r')
			*d = ' ';
	}

	if((strlen(name) < 1) || (strlen(desc) < 1)) {

		return;
	}

	Assert(s_ScenarioWindow);
	if(s_ScenarioWindow) {
		MBCHAR scenPackDir[_MAX_PATH];
		strcpy(scenPackDir, s_ScenarioWindow->m_scenarioPack->m_path);

		CIV_SCEN_ERR err = g_civScenarios->MakeNewScenario(s_ScenarioWindow->m_scenarioPack, name, desc);
		Assert(err == CIV_SCEN_OK);

		s_ScenarioWindow->m_scenarioPack = g_civScenarios->GetScenarioPackByPath(scenPackDir);
		Assert(s_ScenarioWindow->m_scenarioPack);
		if(s_ScenarioWindow->m_scenarioPack) {

			s_ScenarioWindow->SetMode(SCENARIO_WINDOW_MODE_SAVE_SCEN);
		} else {
			s_ScenarioWindow->SetMode(SCENARIO_WINDOW_MODE_SAVE_PACK);
		}

		if(s_ScenarioWindow->m_newScenWindow) {
			g_c3ui->RemoveWindow(s_ScenarioWindow->m_newScenWindow->Id());
		}
	}
}

void ScenarioWindow::NewScenCancel(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	if(s_ScenarioWindow) {
		if(s_ScenarioWindow->m_newScenWindow) {
			g_c3ui->RemoveWindow(s_ScenarioWindow->m_newScenWindow->Id());
		}
	}
}
