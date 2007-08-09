//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The advance options profile edit window
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
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_ui.h"
#include "c3ui.h"
#include "ctp2_Window.h"
#include "aui_ldl.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "ctp2_Static.h"
#include "ctp2_button.h"
#include "ctp2_textfield.h"

#include "profileDB.h"
#include "MessageBoxDialog.h"
#include "ProfileEdit.h"
#include "StrDB.h"

#include "CriticalMessagesPrefs.h"

#include "civapp.h"
extern CivApp *g_civApp;

#include "SpriteGroupList.h"
extern SpriteGroupList		*g_unitSpriteGroupList;

extern C3UI *g_c3ui;

ProfileEdit *s_profileEdit = NULL;

BOOL s_origUnitAnim = FALSE;

ProfileEdit::ProfileEdit(AUI_ERRCODE *err)
{
	*err = AUI_ERRCODE_OK;

	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot("ProfileEdit");
	Assert(m_window);

	m_list = (ctp2_ListBox *)aui_Ldl::GetObject("ProfileEdit.List");
	Assert(m_list);

	m_list->SetAbsorbancy(FALSE); 

	aui_Ldl::SetActionFuncAndCookie("ProfileEdit.CloseButton", CloseWindow, NULL);
	aui_Ldl::SetActionFuncAndCookie("ProfileEdit.EnableMessages", EnableMessages, NULL);
}

ProfileEdit::~ProfileEdit()
{
	if(m_window) {
		aui_Ldl::DeleteHierarchyFromRoot("ProfileEdit");
	}
}

AUI_ERRCODE ProfileEdit::Initialize()
{
	
	if(s_profileEdit)
		return AUI_ERRCODE_OK;

	
	AUI_ERRCODE err = AUI_ERRCODE_OK;
	s_profileEdit = new ProfileEdit(&err);

	Assert(err == AUI_ERRCODE_OK);

	return err;
}

AUI_ERRCODE ProfileEdit::Display()
{
	if(!s_profileEdit)
		Initialize();
	
	Assert(s_profileEdit);
	if(!s_profileEdit)
		return AUI_ERRCODE_INVALIDPARAM;

	AUI_ERRCODE err = AUI_ERRCODE_OK;

	s_origUnitAnim = g_theProfileDB->IsUnitAnim();

	if(s_profileEdit->m_window) {
		err = g_c3ui->AddWindow(s_profileEdit->m_window);
		Assert(err == AUI_ERRCODE_OK);
		if(err == AUI_ERRCODE_OK)
			err = s_profileEdit->m_window->Show();

		if(err == AUI_ERRCODE_OK) {
			s_profileEdit->FillList();
		}
	}		
	return err;
}

AUI_ERRCODE ProfileEdit::Hide()
{
	if(!s_profileEdit)
		return AUI_ERRCODE_OK;

	if(!s_profileEdit->m_window) {
		return AUI_ERRCODE_OK;
	}
	s_profileEdit->m_window->Hide();
	g_c3ui->RemoveWindow(s_profileEdit->m_window->Id());

	if(g_theProfileDB && s_origUnitAnim != g_theProfileDB->IsUnitAnim()) {
		if(g_civApp && g_civApp->IsGameLoaded()) {
			g_unitSpriteGroupList->RefreshBasicLoads(GROUPTYPE_UNIT);
		}
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE ProfileEdit::Cleanup()
{
	if(s_profileEdit) {
		if(s_profileEdit->m_window) {
			g_c3ui->RemoveWindow(s_profileEdit->m_window->Id());
		}
		delete s_profileEdit;
		s_profileEdit = NULL;
	}
	return AUI_ERRCODE_OK;
}

void ProfileEdit::FillList()
{
	PointerList<ProfileVar>::Walker walk(g_theProfileDB->GetVars());
	m_list->Clear();
	m_list->BuildListStart();
	for(; walk.IsValid(); walk.Next()) {
		if(!walk.GetObj()->m_visible) {
			
			continue;
		}

		char *itemname = NULL;
		switch(walk.GetObj()->m_type) {
			case PV_NUM:
				itemname = "ProfileEditNumItem";
				break;
			case PV_STRING:
				
				continue;
			case PV_BOOL:
				itemname = "ProfileEditBoolItem";
				break;
		}
		ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot(itemname);
		Assert(item);
		if(!item) break;

		ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
		Assert(box);
		if(box) {
			ctp2_Static *label = (ctp2_Static *)box->GetChildByIndex(0);
			Assert(label);
			if(!label) break;
			
			char localId[k_MAX_NAME_LEN];
			sprintf(localId, "str_profile_%s", walk.GetObj()->m_name);
			const char *localName = g_theStringDB->GetNameStr(localId);

			if(localName) {
				label->SetText(localName);
			} else {
				label->SetText(walk.GetObj()->m_name);
			}
			
			switch(walk.GetObj()->m_type) {
				case PV_BOOL:
				{
					ctp2_Button *button = (ctp2_Button *)box->GetChildByIndex(1);
					Assert(button);
					if(button) {
						if(*walk.GetObj()->m_numValue) {
							button->SetText("X");
						} else {
							button->SetText(" ");
						}
						button->SetActionFuncAndCookie(ToggleBoolVar, walk.GetObj());
					}
					break;
				}
				case PV_NUM:
				{
					ctp2_TextField *field = (ctp2_TextField *)box->GetChildByIndex(1);
					Assert(field);
					if(field) {
						char buf[50];
						sprintf(buf, "%d", *walk.GetObj()->m_numValue);
						field->SetFieldText(buf);
						field->SetActionFuncAndCookie(SetNumVar, walk.GetObj());
					}
					break;
				}
			}
		}
		item->SetUserData(walk.GetObj());

		m_list->AddItem(item);
	}
	m_list->BuildListEnd();
}

void ProfileEdit::ListCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_LISTBOX_ACTION_SELECT && action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT)
		return;

	if(!s_profileEdit)
		return;

}

void ProfileEdit::ToggleBoolVar(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	ProfileVar *var = (ProfileVar *)cookie;
	Assert(g_theProfileDB);
	if(g_theProfileDB) {
		*var->m_numValue = !*var->m_numValue;
		ctp2_Button *button = (ctp2_Button *)control;
		if(*var->m_numValue) {
			button->SetText("X");
		} else {
			button->SetText(" ");
		}
	}
}

void ProfileEdit::SetNumVar(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action == AUI_TEXTFIELD_ACTION_EXECUTE || action == AUI_TEXTFIELD_ACTION_DISMISS) {
		ctp2_TextField *field = (ctp2_TextField *)control;
		char buf[50];
		field->GetFieldText(buf, 50);
		ProfileVar *var = (ProfileVar *)cookie;
		*var->m_numValue = atoi(buf);
	}
}

void ProfileEdit::CloseWindow(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action == AUI_BUTTON_ACTION_EXECUTE) {
		
		g_theProfileDB->Save();

		ProfileEdit::Hide();
	}
}

void ProfileEdit::EnableMessages(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action== AUI_BUTTON_ACTION_EXECUTE) {
		g_theCriticalMessagesPrefs->EnableAll();
	}
}

