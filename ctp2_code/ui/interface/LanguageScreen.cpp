//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Language settings screen
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
// - Created file. (04-May-2025 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "LanguageScreen.h"

#include "c3ui.h"
#include "CivPaths.h"
#include "profileDB.h"          // g_theProfileDB
#include "Globals.h"            // allocated::clear
#include "keypress.h"

#include "spnewgamewindow.h"

#include "aui_uniqueid.h"
#include "ctp2_listbox.h"
#include "ctp2_button.h"
#include "ctp2_Static.h"

#include "LanguageRecord.h"
#include "StrDB.h"

LanguageScreen* LanguageScreen::s_languageScreen = NULL;
extern C3UI *   g_c3ui;

LanguageScreen::LanguageScreen(AUI_ERRCODE *errcode, sint32 bpp)
:   c3_PopupWindow(errcode, aui_UniqueId(), "LanguageScreen", bpp, AUI_WINDOW_TYPE_FLOATING, false)
{
	AddTitle("LanguageScreen.Name");
	AddOk(LanguageScreen::AcceptPress);
	AddCancel(LanguageScreen::CancelPress);

	m_LanguageListBox = (ctp2_ListBox *)aui_Ldl::BuildHierarchyFromRoot("LanguageScreen.LanguageBox");
	m_startLanguage = -1;

	Assert(m_LanguageListBox);

	m_LanguageListBox->SortByColumn(0, true);
	m_LanguageListBox->SetForceSelect(TRUE);
	m_LanguageListBox->SetMultiSelect(FALSE);

	m_LanguageListBox->SetActionFuncAndCookie(LanguageScreen::ItemSelected, NULL);

	ctp2_ListItem *selItem = NULL;

	MBCHAR* languageDir = g_theProfileDB->GetLanguageDirectory();

	for(sint32 i = 0; i < g_theLanguageDB->NumRecords(); ++i)
	{
		const LanguageRecord* lanRec = g_theLanguageDB->Get(i);

		if(lanRec->GetHidden())
			continue;

		ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("LanguageItem");
		Assert(item);
		if(!item)
			return;

		item->SetUserData((void *)i);

		ctp2_Static *text = (ctp2_Static *)item->GetChildByIndex(0);
		Assert(text);
		if(!text)
			return;

		text->SetText(lanRec->GetNameText());
		item->SetCompareCallback(LanguageScreen::CompareItems);
		m_LanguageListBox->AddItem(item);

		if(strcmp(lanRec->GetDirectory(), languageDir) == 0)
		{
			selItem = item;
			m_startLanguage = i;
		}
	}

	if (selItem != NULL) m_LanguageListBox->SelectItem(selItem);

	m_languageDescription = (ctp2_Static *)aui_Ldl::BuildHierarchyFromRoot("LanguageScreen.LanguageDescription");

	m_getLanguageFromOS = spNew_ctp2_Button(errcode, "LanguageScreen", "SelectTrackButton", LanguageScreen::GetLanguageFromOS);

	m_warning = new ctp2_Static(errcode, aui_UniqueId(), "LanguageScreen.Warning");

	m_warning->Hide();

	*errcode = aui_Ldl::SetupHeirarchyFromRoot("LanguageScreen");

	SetLanguageDescription();
}

LanguageScreen::~LanguageScreen()
{
	delete m_LanguageListBox;
	delete m_languageDescription;
	delete m_getLanguageFromOS;
	delete m_warning;
}

void LanguageScreen::Init()
{
	if(s_languageScreen == NULL)
	{
		AUI_ERRCODE errcode = AUI_ERRCODE_OK;
		s_languageScreen = new LanguageScreen(&errcode, 16);
	}
}

void LanguageScreen::Cleanup()
{
	if (s_languageScreen != NULL)
	{
		g_c3ui->RemoveWindow(s_languageScreen->Id());
		keypress_RemoveHandler(s_languageScreen);
	}

	allocated::clear(s_languageScreen);
}

void LanguageScreen::DisplayWindow()
{
	Init();

	AUI_ERRCODE auiErr = g_c3ui->AddWindow(s_languageScreen);
	Assert(auiErr == AUI_ERRCODE_OK);
	keypress_RegisterHandler(s_languageScreen);
}

void LanguageScreen::RemoveWindow(uint32 action)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

	if(s_languageScreen == NULL) return;

	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow(s_languageScreen->Id());
	Assert(auiErr == AUI_ERRCODE_OK);
	keypress_RemoveHandler(s_languageScreen);
}

void LanguageScreen::AcceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

	s_languageScreen->ApplyLanguage();
	LanguageScreen::RemoveWindow(action);
}

void LanguageScreen::CancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

	LanguageScreen::RemoveWindow(action);
}

void LanguageScreen::GetLanguageFromOS(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

	if(s_languageScreen == NULL) return;

	s_languageScreen->SelectLocLanguage();
}

sint32 LanguageScreen::CompareItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column)
{
	ctp2_Static *text1 = (ctp2_Static *)item1->GetChildByIndex(0);
	ctp2_Static *text2 = (ctp2_Static *)item2->GetChildByIndex(0);

	return _stricoll(text1->GetText(), text2->GetText());
}

void LanguageScreen::ItemSelected(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	// This is for the text boxes and updated once selected
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT ) return;

	if(s_languageScreen == NULL) return;

	s_languageScreen->SetLanguageDescription();
}

void LanguageScreen::SetLanguageDescription()
{
	ctp2_ListItem *item = (ctp2_ListItem *)m_LanguageListBox->GetSelectedItem();

	if(!item) return;

	sint32 lan = reinterpret_cast<sint32>(item->GetUserData());
	m_languageDescription->SetText(g_theStringDB->GetNameStr(g_theLanguageDB->Get(lan)->GetDescription()));

	Ok()->Enable(!g_theLanguageDB->Get(lan)->GetDisabled());

	if(m_startLanguage == lan)
		m_warning->Hide();
	else
		m_warning->Show();
}

void LanguageScreen::ApplyLanguage()
{
	ctp2_ListItem *item = (ctp2_ListItem *)m_LanguageListBox->GetSelectedItem();

	if(!item) return;

	sint32 lan = reinterpret_cast<sint32>(item->GetUserData());

	const LanguageRecord* lanRec = g_theLanguageDB->Get(lan);
	g_theProfileDB->SetLanguageDirectory(lanRec->GetDirectory());

#if 0
	// That would be needed to change the userinterface language without restart.
	// That's just the first. Then the language database would need to be reloaded.
	// The string IDs in the otherdatabases need then to be updated.
	// And the most difficult the interface would need to be updated.
	g_civPaths->SetLocalizedPath(lanRec->GetDirectory());
#endif
}

void LanguageScreen::SelectLocLanguage()
{
	const LanguageRecord* lanRec = g_civPaths->FindLanguage();

	if(lanRec == NULL)
		return;

	for(sint32 i = 0;  i < m_LanguageListBox->NumItems(); ++i)
	{
		ctp2_ListItem *item = static_cast<ctp2_ListItem*>(m_LanguageListBox->GetItemByIndex(i));

		if(reinterpret_cast<sint32>(item->GetUserData()) == lanRec->GetIndex())
		{
			m_LanguageListBox->SelectItem(item);
			return;
		}
	}
}

