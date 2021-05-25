//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Great library handling
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
// _JAPANESE
// - Handle Japanese 2-byte character input data.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Modified by Martin Gühmann on October the 28th: line added in
//   sint32 GreatLibrary::UpdateList( DATABASE database )
//   to make sure that also goods with the GLHidden flag aren't shown.
// - Start the great library with the current research project of the player.
// - Clears the research goal of the player, when an item is selected that
//   enabling advance has been researched already, by Martin Gühmann.
// - The tech goal can now also set for tile improvements, by Martin Gühmann.
// - Handle Japanese input data, by t.s. (2003.12).
// - Memory leaks repaired.
// - Increased maximum library text size to support the German version.
// - Exported database name size max.
// - Added function to look up an item name on creation index.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Fixed display of topics after the fixing of the alphanumerical
//   indexing of the databases. (Sep 13th 2005 Martin Gühmann)
// - Search now searches now in the topic names, prerq and vari texts. (Sep 13th 2005 Martin Gühmann)
// - Replaced old concept database by new one. (31-Mar-2007 Martin Gühmann)
// - Search does not find items that are supposed to be hidden. (21-Apr-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------
//
/// \file   greatlibrary.cpp
/// \brief  Great library handling

#include "c3.h"
#include "greatlibrary.h"

#include "AdvanceRecord.h"
#include "aui.h"
#include "aui_ldl.h"
#include "aui_listbox.h"
#include "aui_static.h"
#include "aui_stringtable.h"
#include "aui_switchgroup.h"
#include "aui_uniqueid.h"
#include "BuildingRecord.h"
#include "c3_button.h"
#include "c3ui.h"
#include "chart.h"
#include "colorset.h"
#include "ConceptRecord.h"
#include "controlsheet.h"
#include <cstdlib>
#include "ctp2_button.h"
#include "ctp2_hypertextbox.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "ctp2_Static.h"
#include "ctp2_Tab.h"
#include "ctp2_TabGroup.h"
#include "ctp2_textfield.h"
#include "ctp2_Window.h"
#include "directvideo.h"
#include "Globals.h"            // allocated::clear
#include "GovernmentRecord.h"
#include "greatlibrarywindow.h"
#include "keypress.h"
#include "MessageBoxDialog.h"
#include "OrderRecord.h"
#include "pixelutils.h"
#include "player.h"
#include "profileDB.h"          // g_theProfileDB
#include "ResourceRecord.h"
#include "sci_advancescreen.h"
#include "screenutils.h"
#include "SelItem.h"
#include "SlicObject.h"
#include "StrDB.h"              // g_theStringDB
#include "String_Search.h"
#include "TerrainImprovementRecord.h"
#include "TerrainRecord.h"
#include "text_hasher.h"
#include "texttab.h"
#include "UIUtils.h"
#include "UnitRecord.h"
#include <vector>
#include "videoutils.h"
#include "WonderRecord.h"
#include "wonderutil.h"
#include "AgeRecord.h"

extern sint32		g_ScreenWidth;
extern sint32		g_ScreenHeight;
extern C3UI			*g_c3ui;
extern sint32		g_modalWindow;

static char const s_database_names[DATABASE_MAX][GL_MAX_DB_NAME_SIZE] =
{
	"DATABASE_DEFAULT",
	"DATABASE_UNITS",
	"DATABASE_BUILDINGS",
	"DATABASE_WONDERS",
	"DATABASE_ADVANCES",
	"DATABASE_TERRAIN",
	"DATABASE_CONCEPTS",
	"DATABASE_GOVERNMENTS",
	"DATABASE_TILE_IMPROVEMENTS",
	"DATABASE_RESOURCE",
	"DATABASE_ORDERS",
	"DATABASE_SEARCH"

};

GreatLibrary	*g_greatLibrary = NULL;

Text_Hasher<char *> * GreatLibrary::s_great_library_info = NULL;

void GreatLibrary::Initialize_Great_Library_Data()
{
	const int GREAT_LIBRARY_HASH_SIZE = 2000;

	allocated::reassign(s_great_library_info,
	                    new Text_Hasher<char *>(GREAT_LIBRARY_HASH_SIZE, NULL)
	                   );

	Load_Great_Library();
}

void GreatLibrary::Shutdown_Great_Library_Data()
{
	allocated::clear(s_great_library_info);
}

enum Read_Library_State
{
	LOOKING_FOR_NAME,
	IN_NAME,
	IN_TEXT,
	IN_END
};

void GreatLibrary::Load_Great_Library()
{
	Assert(s_great_library_info);
	if (!s_great_library_info)
	{
		return;
	}

	FILE * great_library = c3files_fopen(C3DIR_GL, "Great_Library.txt", "r");
	Assert(great_library);
	if (!great_library)
		return;

	{
		const int MAX_NAME = 1024;
		const int MAX_ENTRY	= k_MAX_GL_ENTRY;
		char the_name[MAX_NAME];
		char the_entry[MAX_ENTRY];
		int name_pos = 0;
		int entry_pos = 0;
		char * end_ptr = NULL;

#if defined(_JAPANESE)
		int end_pos;
		char * end_tag = "[END]";
#endif

		Read_Library_State reading_what = LOOKING_FOR_NAME;

		while (!feof(great_library))
		{

			int ch = fgetc(great_library);

			switch (reading_what)
			{


			case LOOKING_FOR_NAME:
				{

					if (ch == '[')
					{

						reading_what = IN_NAME;

						name_pos = 0;
						entry_pos = 0;
					}

				}
				break;

			case IN_NAME:
				{
					if (ch == ']')
					{
						reading_what = IN_TEXT;

						the_name[name_pos++] = 0;

						ch = fgetc(great_library);
					}
					else
					{
						the_name[name_pos++] = static_cast<char>(ch);

						Assert(name_pos < MAX_NAME);
						if (name_pos >= MAX_NAME)
						{

							the_name[name_pos-1] = 0;

							while (ch != ']')
							{
								ch = fgetc(great_library);
							}

							reading_what = IN_TEXT;

							ch = fgetc(great_library);
						}
					}
				}
				break;

			case IN_TEXT:
				{
					if (ch == '[')
					{
						end_ptr = &(the_entry[entry_pos]);

						Assert(ch == '[');
						the_entry[entry_pos++] = static_cast<char>(ch);

						reading_what = IN_END;
#if defined(_JAPANESE)
						end_pos = 0;
#endif
					}
					else
					{
						the_entry[entry_pos++] = static_cast<char>(ch);

						Assert(entry_pos < MAX_ENTRY - 6);
						if (entry_pos >= MAX_ENTRY - 6)
						{
							end_ptr = &(the_entry[entry_pos]);

							while (ch != '[')
							{
								ch = fgetc(great_library);
							}

							Assert(ch == '[');
							the_entry[entry_pos++] = static_cast<char>(ch);

							reading_what = IN_END;
						}
					}
				}
				break;

			case IN_END:
				{
					the_entry[entry_pos++] = static_cast<char>(ch);

#if defined(_JAPANESE)
					end_pos ++;
#endif

					if (ch == ']')
					{
						the_entry[entry_pos] = 0;

#if !defined(_JAPANESE)
						if (!strcmp(end_ptr, "[END]"))
#else
						if (!strcmp(end_ptr, end_tag))
#endif
						{
							*end_ptr = 0;

							size_t  nameLength  = strlen(the_name);
							char *  name_copy   = new char[nameLength + 1];
							strcpy(name_copy, the_name);
							name_copy[nameLength] = 0;

							size_t  entryLength = strlen(the_entry);
							char *  entry_copy  = new char[entryLength + 1];
							strcpy(entry_copy, the_entry);
							entry_copy[entryLength] = 0;

							s_great_library_info->Add_To_Hash_Table
							    (name_copy, entry_copy);

							reading_what = LOOKING_FOR_NAME;
						}
						else
						{
							reading_what = IN_TEXT;
						}

#if defined(_JAPANESE)
					} else {
						if ( ch != *(end_tag+end_pos) ) // not end tag
						{
							reading_what = IN_TEXT;
						}
#endif
					}
				}
				break;

			};
		}
	}

	fclose(great_library);
}

int GreatLibrary::Get_Database_From_Name
(
	char const * database_name
)
{
	for (int i = 0; i < DATABASE_MAX; i++)
	{
		if (!strcmp(database_name, s_database_names[i]))
		{
			return i;
		}
	}

	Assert(false);
	return -1;
}

int GreatLibrary::Get_Object_Index_From_Name
(
	int             which_database,
	char const *    object_name
)
{
	int     index;

	// By turning which_database into a CTPDatabase<T> pointer we could even
	// simplify this code more, only problem would be the search database,
	// which could be a NULL pointer then.
	switch (which_database)
    {
	default:
        index = CTPRecord::INDEX_INVALID;
        break;
	case DATABASE_UNITS:
		index = g_theUnitDB->FindRecordNameIndex(object_name);
		break;
	case DATABASE_SEARCH:
		return 0;
	case DATABASE_ORDERS:
		index = g_theOrderDB->FindRecordNameIndex(object_name);
		break;
	case DATABASE_RESOURCE:
		index = g_theResourceDB->FindRecordNameIndex(object_name);
		break;
	case DATABASE_BUILDINGS:
		index = g_theBuildingDB->FindRecordNameIndex(object_name);
		break;
	case DATABASE_WONDERS:
		index = g_theWonderDB->FindRecordNameIndex(object_name);
		break;
	case DATABASE_ADVANCES:
		index = g_theAdvanceDB->FindRecordNameIndex(object_name);
		break;
	case DATABASE_TERRAIN:
		index = g_theTerrainDB->FindRecordNameIndex(object_name);
		break;
	case DATABASE_CONCEPTS:
		index = g_theConceptDB->FindRecordNameIndex(object_name);
		break;
	case DATABASE_GOVERNMENTS:
		index = g_theGovernmentDB->FindRecordNameIndex(object_name);
		break;
	case DATABASE_TILE_IMPROVEMENTS:
		index = g_theTerrainImprovementDB->FindRecordNameIndex(object_name);
		break;
	}

    Assert(index != CTPRecord::INDEX_INVALID);
	if (index == CTPRecord::INDEX_INVALID)
    {
		DPRINTF(k_DBG_GAMESTATE, ("Bad database object name: %s, DB: %i\n",
                                  object_name,
                                  which_database
                                 )
               );
	}

	return index;
}

void greatlibrary_SetGoalCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_greatLibrary->HandleSetGoal();

}

void greatlibrary_ExitCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	close_GreatLibrary();

	if ( g_greatLibrary->GetSci() ) {
		open_ScienceStatus();
	}
	if(sci_advancescreen_isOnScreen())
	{
		sci_advancescreen_loadList();
	}
}

void greatlibrary_BackCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_greatLibrary->Back();
}

void greatlibrary_ForwardCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_greatLibrary->Forward();
}

void GreatLibrary::kh_Close()
{
	close_GreatLibrary();
}

void greatlibrary_IndexButtonCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON ) return;

	if ( g_greatLibrary ) g_greatLibrary->HandleIndexButton( (ctp2_Button *)control );
}

void greatlibrary_TechBoxActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)CTP2_HYPERLINK_ACTION_EXECUTE ) return;

	ctp2_HyperLink *hl = ((ctp2_HyperTextBox *)control)->GetSelectedHyperLink();

	if ( hl ) {
		g_greatLibrary->SetLibrary( hl->m_index, (DATABASE)hl->m_db );
	}
}

void greatlibrary_SearchWordActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)CTP2_HYPERLINK_ACTION_EXECUTE ) return;

	g_greatLibrary->Force_A_Search();
}

void greatlibrary_PrereqActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Chart *chart = (Chart *)cookie;
	sint32 numPreReq = chart ? chart->GetNumPreReq() : 0;

	for ( sint32 i = 0;i < numPreReq;i++ )
	{
		if ( control->Id() == chart->GetPreReqButton(i)->Id() )
		{
			g_greatLibrary->SetLibrary( chart->GetPreReqIndex(i), DATABASE_ADVANCES );
		}
	}
}

void greatlibrary_EitherPrereqActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Chart *chart = (Chart *)cookie;
	sint32 numEitherPreReq = chart ? chart->GetNumEitherPreReq() : 0;

	for ( sint32 i = 0;i < numEitherPreReq;i++ )
	{
		if ( control->Id() == chart->GetEitherPreReqButton(i)->Id() )
		{
			g_greatLibrary->SetLibrary( chart->GetEitherPreReqIndex(i), DATABASE_ADVANCES );
		}
	}
}

void greatlibrary_LeadsToActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Chart *chart = (Chart *)cookie;
	sint32 numLeadsTo = chart ? chart->GetNumLeadsTo() : 0;

	for ( sint32 i = 0;i < numLeadsTo;i++ )
	{
		if ( control->Id() == chart->GetLeadsToButton(i)->Id() )
		{
			g_greatLibrary->SetLibrary( chart->GetLeadsToIndex(i), DATABASE_ADVANCES );
		}
	}
}

void GreatLibrary_Topics_List_Callback
(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void *cookie
)
{
	g_greatLibrary->HandleListButton(control, action, data, cookie);
}




TechListItem::TechListItem(AUI_ERRCODE * retval, sint32 index, DATABASE database, MBCHAR * ldlBlock)
:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
    ctp2_ListItem    (retval, ldlBlock),
    m_index          (index),
    m_database       (database)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(ldlBlock);
}

AUI_ERRCODE TechListItem::InitCommonLdl(MBCHAR const * ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf(block, "%s.%s", ldlBlock, "Name");

	AUI_ERRCODE		retval = AUI_ERRCODE_OK;
	AddChild(new ctp2_Static(&retval, aui_UniqueId(), block));
	Assert(AUI_SUCCESS(retval));

	Update();

	return retval;
}

void TechListItem::Update(void)
{
	ctp2_Static *   subItem         = (ctp2_Static *) GetChildByIndex(0);
	enum DATABASE   real_database   = m_database;
	int             real_index      = m_index;

	if (m_database == DATABASE_SEARCH)
	{
		real_database   = g_greatLibrary->m_search_results[m_index].m_database;
		real_index      = g_greatLibrary->m_search_results[m_index].m_item;
	}

	switch ( real_database ) {
	case DATABASE_UNITS:
		subItem->SetText( g_theUnitDB->GetNameStr(real_index) );
		break;

	case DATABASE_ORDERS:
		subItem->SetText( g_theOrderDB->GetNameStr(real_index) );
		break;

	case DATABASE_RESOURCE:
		subItem->SetText( g_theResourceDB->GetNameStr(real_index) );
		break;

	case DATABASE_BUILDINGS:
		subItem->SetText( g_theBuildingDB->GetNameStr(real_index) );
		break;

	case DATABASE_WONDERS:
		subItem->SetText( g_theWonderDB->GetNameStr(real_index) );
		break;

	case DATABASE_ADVANCES:
		subItem->SetText( g_theAdvanceDB->GetNameStr(real_index) );
		break;

	case DATABASE_TERRAIN:
		subItem->SetText( g_theTerrainDB->GetNameStr(real_index) );
		break;

	case DATABASE_CONCEPTS:
		subItem->SetText( g_theConceptDB->GetNameStr(real_index) );
		break;

	case DATABASE_GOVERNMENTS:
		subItem->SetText( g_theGovernmentDB->GetNameStr(real_index) );
		break;

	case DATABASE_TILE_IMPROVEMENTS:
		subItem->SetText( g_theTerrainImprovementDB->GetNameStr(real_index) );
		break;

	default:
		bool invalidDatabase = false;
		Assert( invalidDatabase );
	}
}

sint32 TechListItem::Compare(ctp2_ListItem *item2, uint32 column)
{
	switch (column)
    {
	case 0:
        {
		    sint32 val2 = ((TechListItem *)item2)->GetIndex();
		    if (m_index < val2) return -1;
		    else if (val2 < m_index) return 1;
            else return 0;
        }
		break;
	}

	return 0;
}

bool greatlibrary_Initialize( sint32 theMode, bool sci )
{
    if (g_greatLibrary)
    {
		if ( sci ) {
			g_greatLibrary->SetLibrary( theMode, DATABASE_ADVANCES );
		}
		else {
			g_greatLibrary->SetLibrary
                (g_greatLibrary->m_window->GetTechMode(),
                 g_greatLibrary->m_window->GetTechDatabase()
                );
		}
		g_greatLibrary->SetSci( sci );
		g_greatLibrary->GetWindow()->MoveOG();
    }
    else
    {
        g_greatLibrary = new GreatLibrary(theMode);
        g_greatLibrary->SetSci( sci );
    }

    return false;
}

void greatlibrary_Cleanup(void)
{
    if (g_greatLibrary)
    {
        g_greatLibrary->Remove();
        allocated::clear(g_greatLibrary);
    }
}

GreatLibrary::GreatLibrary(sint32 theMode)
:
    KeyboardHandler             (),
    m_setGoalButton             (NULL),
    m_techTree                  (NULL),
    m_techRequirementsText      (NULL),
    m_techVariablesText         (NULL),
#ifdef __AUI_USE_DIRECTX__
    m_techMovie                 (NULL),
#endif // __AUI_USE_DIRECTX__
    m_techStillShot             (NULL),
    m_string                    (NULL),
    m_buttonString              (LIB_STRING_INDEX),
    m_tabGroup                  (NULL),
    m_gameplayTab               (NULL),
    m_historicalTab             (NULL),
    m_techTab                   (NULL),
    m_techHistoricalText        (NULL),
    m_techGameplayText          (NULL),
    m_okButton                  (NULL),
    m_backButton                (NULL),
    m_forwardButton             (NULL),
    m_categoryText              (NULL),
    m_searchLabel               (NULL),
    m_search_word               (NULL),
    m_indexButtonSwitchGroup    (NULL),
    m_searchButton              (NULL),
    m_unitsButton               (NULL),
    m_improveButton             (NULL),
    m_wondersButton             (NULL),
    m_advancesButton            (NULL),
    m_governButton              (NULL),
    m_terrainButton             (NULL),
    m_tileimpButton             (NULL),
    m_conceptButton             (NULL),
    m_goodsButton               (NULL),
    m_ordersButton              (NULL),
    m_topics_list               (NULL),
    m_indexLeft                 (NULL),
    m_indexMiddle               (NULL),
    m_indexRight                (NULL),
    m_page                      (0),
    m_maxPage                   (false),
    m_database                  (DATABASE_UNITS),
    m_listDatabase              (DATABASE_UNITS),
    m_selectedIndex             (CTPRecord::INDEX_INVALID),
    m_maxIndex                  (0),
    m_sci                       (false),
    m_itemLabel                 (NULL),
    m_search_results            (),
    m_history                   (),
    m_history_position          (0),
    m_window                    (NULL)
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	m_window = new GreatLibraryWindow(&errcode);
	m_window->m_window->SetDraggable(TRUE);

	Initialize("GreatLibrary");

	m_techTree->Update( theMode );
	m_window->SetTechMode( theMode, DATABASE_ADVANCES );

	m_window->SetTechTree ( m_techTree );
	m_window->SetTechStillShot ( m_techStillShot );
	m_window->SetTechHistoricalText ( m_techHistoricalText );
	m_window->SetTechGameplayText ( m_techGameplayText );
#ifdef __AUI_USE_DIRECTX__
	m_window->SetTechMovie ( m_techMovie );
#endif // __AUI_USE_DIRECTX__
	m_window->SetTechRequirementsText ( m_techRequirementsText );
	m_window->SetTechVariablesText ( m_techVariablesText );

	SlicObject so;
	so.AddAdvance(theMode);

	m_window->LoadGameplayText( so );
	m_historicalTab->Enable
        (GreatLibraryWindow::GREAT_LIBRARY_PANEL_BLANK !=
            m_window->LoadHistoricalText(so)
        );

    FixTabs();
	m_window->LoadRequirementsText( so );
	m_window->LoadVariablesText( so );


	if ( g_theProfileDB->IsLibraryAnim() ) {
		m_techStillShot->Hide();
#ifdef __AUI_USE_DIRECTX__
		if (m_techMovie) {
			if (m_techMovie->Open()) m_techMovie->CloseStream();
		}
#endif
		if (!m_window->LoadTechMovie()) {
			m_techStillShot->ShouldDraw();
			m_techStillShot->Show();
		}
	}
	else {
		if ( !m_window->LoadTechStill() ) {
			m_techStillShot->Hide();
		}
		else {
			m_techStillShot->ShouldDraw();
			m_techStillShot->Show();
		}
	}
}

void GreatLibrary::Initialize(MBCHAR const * windowBlock)
{
	m_okButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, "OkButton");
	m_okButton->SetActionFuncAndCookie(greatlibrary_ExitCallback, NULL);

	m_backButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, "BackButton");
	m_backButton->SetActionFuncAndCookie(greatlibrary_BackCallback, NULL);

	m_forwardButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, "ForwardButton");
	m_forwardButton->SetActionFuncAndCookie(greatlibrary_ForwardCallback, NULL);

	m_setGoalButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, "SetGoalButton");
	m_setGoalButton->SetActionFuncAndCookie(greatlibrary_SetGoalCallback, NULL);

	m_categoryText = (ctp2_Static *)aui_Ldl::GetObject(windowBlock, "Category");
	m_categoryText->SetText(g_theStringDB->GetNameStr("str_ldl_CAPS_UNITS"));

	m_searchLabel = (ctp2_Static *)aui_Ldl::GetObject(windowBlock, "SearchLabel");
	m_searchLabel->SetText(g_theStringDB->GetNameStr("str_ldl_SEARCH_WORD"));

	m_techRequirementsText = (ctp2_HyperTextBox *)aui_Ldl::GetObject(windowBlock, "RequirementsText");
	m_techRequirementsText->SetActionFuncAndCookie(greatlibrary_TechBoxActionCallback, NULL);

	m_techVariablesText = (ctp2_HyperTextBox *)aui_Ldl::GetObject(windowBlock, "VariablesText");
	m_techVariablesText->SetActionFuncAndCookie(greatlibrary_TechBoxActionCallback, NULL);

	m_search_word = (ctp2_TextField *)aui_Ldl::GetObject(windowBlock, "SearchWord");
	m_search_word->SetActionFuncAndCookie(greatlibrary_SearchWordActionCallback, NULL);

	m_itemLabel = (ctp2_Static *)aui_Ldl::GetObject(windowBlock, "ItemLabel");

	if( rand()%100 == 1) {
		m_search_word->SetFieldText("Lemur Poo");
	} else {
		m_search_word->SetFieldText("");
	}

	m_search_word->SetKeyboardFocus();
	m_search_word->SelectAll();




	if ( g_theProfileDB->IsLibraryAnim() ) {
		RECT rect = {
			k_VIDEO_X,
			k_VIDEO_Y,
			k_VIDEO_X + k_VIDEO_WIDTH,
			k_VIDEO_Y + k_VIDEO_HEIGHT};

		GetWindow()->SetDynamic(FALSE);

#ifdef __AUI_USE_DIRECTX__
		m_techMovie = new DirectVideo();
		m_techMovie->Initialize((aui_DirectUI *)g_c3ui, (aui_Window *)GetWindow(), FALSE);
		m_techMovie->SetDestRect(&rect);
#endif
	}

	m_techStillShot = (ctp2_Static *)aui_Ldl::GetObject(windowBlock, "TechStillShot");

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	m_string = new aui_StringTable( &errcode, "LibraryStrings");

	ctp2_Static *control = (ctp2_Static *)aui_Ldl::GetObject(windowBlock, "Tabs.TechTreeTab.TabPanel");
	m_techTree = new Chart(&errcode, aui_UniqueId(), "TechTree", NULL, NULL);
	control->AddChild(m_techTree);

	sint32 i;
	for ( i = 0; i < k_MAX_PREREQ ; i++ )
	{
		m_techTree->GetPreReqButton(i)->SetActionFuncAndCookie( greatlibrary_PrereqActionCallback, m_techTree );
	}

	for ( i = 0; i < k_MAX_EITHER_PREREQ ; i++ )
	{
		m_techTree->GetEitherPreReqButton(i)->SetActionFuncAndCookie( greatlibrary_EitherPrereqActionCallback, m_techTree );
	}

	for ( i = 0; i < k_MAX_LEADS_TO ; i++ )
	{
		m_techTree->GetLeadsToButton(i)->SetActionFuncAndCookie( greatlibrary_LeadsToActionCallback, m_techTree );
	}

	m_tabGroup = (ctp2_TabGroup *)aui_Ldl::GetObject(windowBlock, "Tabs");
	m_gameplayTab = (ctp2_Tab *)aui_Ldl::GetObject(windowBlock, "Tabs.Gameplay");
	m_historicalTab = (ctp2_Tab *)aui_Ldl::GetObject(windowBlock, "Tabs.Historical");
	m_techTab = (ctp2_Tab *)aui_Ldl::GetObject(windowBlock, "Tabs.TechTreeTab");

	m_techGameplayText = (ctp2_HyperTextBox *)aui_Ldl::GetObject(windowBlock, "Tabs.Gameplay.TabPanel.GameplayText");
	m_techGameplayText->SetActionFuncAndCookie(greatlibrary_TechBoxActionCallback, NULL);

	m_techHistoricalText = (ctp2_HyperTextBox *)aui_Ldl::GetObject(windowBlock, "Tabs.Historical.TabPanel.HistoricalText");
	m_techHistoricalText->SetActionFuncAndCookie(greatlibrary_TechBoxActionCallback, NULL);

	m_topics_list = (ctp2_ListBox *)aui_Ldl::GetObject(windowBlock, "IndexSheet");
	m_topics_list->SetActionFuncAndCookie( GreatLibrary_Topics_List_Callback, NULL );

    MBCHAR const    controlBlock[]  = "IndexButtonSwitchGroup";
	ctp2_Static *   switchGroup = static_cast<ctp2_Static *>
        (aui_Ldl::GetObject(windowBlock, controlBlock));
	switchGroup->SetBlindness( TRUE );

	MBCHAR		buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( buttonBlock, "%s.%s", controlBlock, "UnitsButton" );
	m_unitsButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, buttonBlock);
	m_unitsButton->SetActionFuncAndCookie( greatlibrary_IndexButtonCallback, NULL );

	sprintf( buttonBlock, "%s.%s", controlBlock, "GoodsButton" );
	m_goodsButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, buttonBlock);
	m_goodsButton->SetActionFuncAndCookie( greatlibrary_IndexButtonCallback, NULL );

	sprintf( buttonBlock, "%s.%s", controlBlock, "OrdersButton" );
	m_ordersButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, buttonBlock);
	m_ordersButton->SetActionFuncAndCookie( greatlibrary_IndexButtonCallback, NULL );

	sprintf( buttonBlock, "%s.%s", controlBlock, "ImproveButton" );
	m_improveButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, buttonBlock);
	m_improveButton->SetActionFuncAndCookie( greatlibrary_IndexButtonCallback, NULL );

	sprintf( buttonBlock, "%s.%s", controlBlock, "WondersButton" );
	m_wondersButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, buttonBlock);
	m_wondersButton->SetActionFuncAndCookie( greatlibrary_IndexButtonCallback, NULL );

	sprintf( buttonBlock, "%s.%s", controlBlock, "AdvancesButton" );
	m_advancesButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, buttonBlock);
	m_advancesButton->SetActionFuncAndCookie( greatlibrary_IndexButtonCallback, NULL );

	sprintf( buttonBlock, "%s.%s", controlBlock, "GovernButton" );
	m_governButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, buttonBlock);
	m_governButton->SetActionFuncAndCookie( greatlibrary_IndexButtonCallback, NULL );

	sprintf( buttonBlock, "%s.%s", controlBlock, "TerrainButton" );
	m_terrainButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, buttonBlock);
	m_terrainButton->SetActionFuncAndCookie( greatlibrary_IndexButtonCallback, NULL );

	sprintf( buttonBlock, "%s.%s", controlBlock, "TileImpButton" );
	m_tileimpButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, buttonBlock);
	m_tileimpButton->SetActionFuncAndCookie( greatlibrary_IndexButtonCallback, NULL );

	sprintf( buttonBlock, "%s.%s", controlBlock, "ConceptButton" );
	m_conceptButton = (ctp2_Button *)aui_Ldl::GetObject(windowBlock, buttonBlock);
	m_conceptButton->SetActionFuncAndCookie( greatlibrary_IndexButtonCallback, NULL );

	if (m_history.size() == 0)
	{
		m_forwardButton->Enable(FALSE);
		m_backButton->Enable(FALSE);
	}
}

GreatLibrary::~GreatLibrary( void )
{
    delete m_window;
    delete m_techTree;
    delete m_string;
#ifdef __AUI_USE_DIRECTX__
    delete m_techMovie;
#endif // __AUI_USE_DIRECTX__
}

void GreatLibrary::Display( void )
{
	GetWindow()->SetType(AUI_WINDOW_TYPE_FLOATING);
	AUI_ERRCODE errcode = g_c3ui->AddWindow( GetWindow() );
	Assert( errcode == AUI_ERRCODE_OK );


	keypress_RegisterHandler(this);

	m_buttonString = LIB_STRING_INDEX;

	ClearHistory();
	// This has already been set: no need to use some dummy advance.
	SetLibrary(m_window->GetTechMode(), m_window->GetTechDatabase());

	FixTabs();

	if(g_player[g_selected_item->GetVisiblePlayer()]->m_researchGoal > -1)
	{
		sci_advancescreen_initAndFillGoalArray(g_player[g_selected_item->GetVisiblePlayer()]->m_researchGoal);
	}
}

void GreatLibrary::Remove(void)
{
	AUI_ERRCODE errcode = g_c3ui->RemoveWindow(GetWindow()->Id());
	Assert(errcode == AUI_ERRCODE_OK);

    keypress_RemoveHandler(this);
}

sint32 GreatLibrary::SetLibrary( sint32 theMode, DATABASE theDatabase, bool add_to_history )
{
    if (theMode < 0)
        return 0;

	m_selectedIndex = theMode;

	if (!g_greatLibrary || !g_greatLibrary->m_window) return 0;

	enum DATABASE real_database = theDatabase;
	int real_index = theMode;

	SetCategoryName(theDatabase);

	if(theDatabase == DATABASE_SEARCH) {

		theDatabase = g_greatLibrary->m_search_results[theMode].m_database;
		theMode = g_greatLibrary->m_search_results[theMode].m_item;
	}

	m_techTab->Enable(DATABASE_ADVANCES == theDatabase);
	FixTabs();

	SlicObject so;
	bool enableGoal = false;

	switch ( theDatabase ) {
	case DATABASE_UNITS:
		g_greatLibrary->m_window->SetTechMode(theMode, DATABASE_UNITS);
		so.AddUnitRecord(theMode);
		enableGoal = true;
		m_itemLabel->SetText(g_theUnitDB->Get(theMode)->GetNameText());
		break;
	case DATABASE_ORDERS:
		g_greatLibrary->m_window->SetTechMode(theMode, DATABASE_ORDERS);
		so.AddInt(theMode);
		m_itemLabel->SetText(g_theOrderDB->Get(theMode)->GetNameText());
		break;
	case DATABASE_RESOURCE:
		g_greatLibrary->m_window->SetTechMode(theMode, DATABASE_RESOURCE);
		so.AddGood(theMode);
		m_itemLabel->SetText(g_theResourceDB->Get(theMode)->GetNameText());
		break;
	case DATABASE_BUILDINGS:
		g_greatLibrary->m_window->SetTechMode(theMode, DATABASE_BUILDINGS);
		so.AddBuilding(theMode);
		enableGoal = true;
		m_itemLabel->SetText(g_theBuildingDB->Get(theMode)->GetNameText());
		break;
	case DATABASE_WONDERS:
		g_greatLibrary->m_window->SetTechMode(theMode, DATABASE_WONDERS);
		so.AddWonder(theMode);
		enableGoal = true;
		m_itemLabel->SetText(g_theWonderDB->Get(theMode)->GetNameText());
		break;
	case DATABASE_ADVANCES:
		g_greatLibrary->m_window->SetTechMode(theMode, DATABASE_ADVANCES);
		so.AddAdvance(theMode);
		m_techTree->Update( theMode );
		if (m_techTree->IsHidden())
        {
			m_techTree->Hide();
		}

		enableGoal = true;
		m_itemLabel->SetText(g_theAdvanceDB->Get(theMode)->GetNameText());
		break;
	case DATABASE_TERRAIN:
		g_greatLibrary->m_window->SetTechMode(theMode, DATABASE_TERRAIN);
		so.AddInt(theMode);
		m_itemLabel->SetText(g_theTerrainDB->Get(theMode)->GetNameText());
		break;
	case DATABASE_CONCEPTS:
		g_greatLibrary->m_window->SetTechMode(theMode, DATABASE_CONCEPTS);
		m_itemLabel->SetText(g_theConceptDB->Get(theMode)->GetNameText());
		break;

	case DATABASE_GOVERNMENTS:
		g_greatLibrary->m_window->SetTechMode( theMode, DATABASE_GOVERNMENTS );
		so.AddGovernment(theMode);
		enableGoal = true;
		m_itemLabel->SetText(g_theGovernmentDB->Get(theMode)->GetNameText());
		break;

	case DATABASE_TILE_IMPROVEMENTS:
		g_greatLibrary->m_window->SetTechMode( theMode, DATABASE_TILE_IMPROVEMENTS );
		so.AddInt(theMode);
		enableGoal = true;
		m_itemLabel->SetText(g_theTerrainImprovementDB->Get(theMode)->GetNameText());
		break;
	}

	m_setGoalButton->Enable(enableGoal);
	m_database = theDatabase;

	if (add_to_history)
	{
		Great_Library_Item new_gl_page;

		new_gl_page.m_database = real_database;
		new_gl_page.m_item = real_index;




		if ((m_history.size() != 0) &&
			(m_history_position + 1 < static_cast<sint32>(m_history.size()))
		   )
		{

			m_history.erase(m_history.begin() + m_history_position + 1, m_history.end());
		}

		m_history.push_back(new_gl_page);
		m_history_position = m_history.size() - 1;
		m_forwardButton->Enable(FALSE);
		m_backButton->Enable(m_history_position > 0);
	}




#ifdef __AUI_USE_DIRECTX__
	if (m_techMovie) {
		if (m_techMovie->Open()) m_techMovie->CloseStream();
		delete m_techMovie;
		m_techMovie = NULL;
	}
#endif

	RECT rect = {
		k_VIDEO_X,
		k_VIDEO_Y,
		k_VIDEO_X + k_VIDEO_WIDTH,
		k_VIDEO_Y + k_VIDEO_HEIGHT};

	if ( g_theProfileDB->IsLibraryAnim() ) {
#ifdef __AUI_USE_DIRECTX__
		m_techMovie = new DirectVideo();
		m_techMovie->Initialize((aui_DirectUI *)g_c3ui, (aui_Window *)GetWindow(), FALSE);
		m_techMovie->SetDestRect(&rect);
#endif

	}
	else {
		m_techStillShot->Show();
		GetWindow()->ShouldDraw();
	}

	m_window->SetTechTree(m_techTree);
#ifdef __AUI_USE_DIRECTX__
	m_window->SetTechMovie(m_techMovie);
#endif // __AUI_USE_DIRECTX__
	m_window->LoadGameplayText(so);
	int const text_load_result = m_window->LoadHistoricalText(so);
	m_historicalTab->Enable
        (text_load_result != GreatLibraryWindow::GREAT_LIBRARY_PANEL_BLANK);
	FixTabs();

	m_window->LoadRequirementsText(so);
	m_window->LoadVariablesText(so);

	if ( g_theProfileDB->IsLibraryAnim() ) {
#ifdef __AUI_USE_DIRECTX__
		if (m_techMovie) {
			if (m_techMovie->Open())
				m_techMovie->CloseStream();
		}
		if (!g_greatLibrary->m_window->LoadTechMovie()) {
		}
#endif // __AUI_USE_DIRECTX__
		if ( m_window->LoadTechStill() ) {
			m_techStillShot->Show();
			GetWindow()->ShouldDraw();
		}
	}
	else
    {
		if (m_window->LoadTechStill())
        {
			m_techStillShot->Show();
			GetWindow()->ShouldDraw();
		}
		else
        {
			m_techStillShot->Hide();
		}
	}

	return 0;
}

void GreatLibrary::ClearHistory( void )
{
	m_history.clear();
	m_history_position  = 0;
	m_selectedIndex     = CTPRecord::INDEX_INVALID;
}

//----------------------------------------------------------------------------
//
// Name       : HandleSetGoal
//
// Description: Handles the settings of goals, takes the items from the
//              Great Libary window, shows a message what will be researched.
//              And make the civilization to set the goal if the advance is
//              not already researched.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void GreatLibrary::HandleSetGoal( void )
{
	const MBCHAR * selection_name = GetObjectName(m_database, m_window->GetTechMode());

	if (selection_name)
	{
		MBCHAR goal_set_message[500];
		int tmp = g_player[g_selected_item->GetVisiblePlayer()]->SetResearchGoal(m_database, m_window->GetTechMode());

		if(tmp == 1)
		{
			sci_advancescreen_initAndFillGoalArray(g_player[g_selected_item->GetVisiblePlayer()]->m_researchGoal);
			const MBCHAR *fmt = g_theStringDB->GetNameStr("str_ldl_GreatLibraryGoalSetTo");
			if (!fmt) fmt = "Goal set to: %s";
			sprintf(goal_set_message, fmt, selection_name);
		}
		else if(tmp == 0)
		{
			sci_advancescreen_clearGoalArray();
			const MBCHAR *fmt = g_theStringDB->GetNameStr("str_ldl_GreatLibraryGoalKnown");
			if (!fmt) fmt = "%s is already known. No goal was set.";
			sprintf(goal_set_message, fmt, selection_name);
		}
		else
		{
			sci_advancescreen_clearGoalArray();
			const MBCHAR *fmt = g_theStringDB->GetNameStr("str_ldl_GreatLibraryNoGoalPossible");
			if (!fmt) fmt = "%s cannot be researched.";
			sprintf(goal_set_message, fmt, selection_name);
		}

		MessageBoxDialog::Information(goal_set_message, "InfoSetGoal");

		if(!m_techTree->IsHidden())
		{
			m_techTree->Update(m_techTree->GetCenterIndex());
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : GreatLibrary::GetItemName
//
// Description: Get the - language dependent - name of a database item.
//
// Parameters : database        : database
//              item            : *lexicographic* index of the item in the
//                                database
//
// Globals    : g_theStringDB
//              g_the<whatever>DB
//              g_greatLibrary
//
// Returns    : MBCHAR const *  : name of the database item
//
// Remark(s)  : See also GetObjectName
//
//----------------------------------------------------------------------------
MBCHAR const * GreatLibrary::GetItemName(int database, int item) const
{
	if (item < 0)
		return NULL;

	switch ( database )
	{
	default:
		return NULL;

	case DATABASE_UNITS:
		return g_theStringDB->GetNameStr(g_theUnitDB->GetName(
				g_theUnitDB->m_alphaToIndex[item])) ;

	case DATABASE_SEARCH:
		{
		    enum DATABASE   real_database   =
                g_greatLibrary->m_search_results[item].m_database;
		    int             real_index      =
                g_greatLibrary->m_search_results[item].m_item;

    		return GetItemName(real_database, real_index);
        }

	case DATABASE_ORDERS:
		return g_theStringDB->GetNameStr(g_theOrderDB->GetName(
				g_theOrderDB->m_alphaToIndex[item])) ;

	case DATABASE_RESOURCE:
		return g_theStringDB->GetNameStr(g_theResourceDB->GetName(
				g_theResourceDB->m_alphaToIndex[item])) ;

	case DATABASE_BUILDINGS:
		return g_theStringDB->GetNameStr(g_theBuildingDB->GetName(
				g_theBuildingDB->m_alphaToIndex[item])) ;

	case DATABASE_WONDERS:
		return g_theStringDB->GetNameStr(g_theWonderDB->GetName(
				g_theWonderDB->m_alphaToIndex[item])) ;

	case DATABASE_ADVANCES:
		return g_theStringDB->GetNameStr(g_theAdvanceDB->GetName(
				g_theAdvanceDB->m_alphaToIndex[item])) ;

	case DATABASE_TERRAIN:
		return g_theStringDB->GetNameStr(g_theTerrainDB->GetName(
				g_theTerrainDB->m_alphaToIndex[item])) ;

	case DATABASE_CONCEPTS:
		return g_theConceptDB->GetNameStr(
				g_theConceptDB->m_alphaToIndex[item]) ;

	case DATABASE_GOVERNMENTS:
		return g_theStringDB->GetNameStr(g_theGovernmentDB->GetName(
				g_theGovernmentDB->m_alphaToIndex[item])) ;

	case DATABASE_TILE_IMPROVEMENTS:
		return g_theStringDB->GetNameStr(g_theTerrainImprovementDB->GetName(
				g_theTerrainImprovementDB->m_alphaToIndex[item])) ;

	} // switch
}

//----------------------------------------------------------------------------
//
// Name       : GreatLibrary::GetObjectName
//
// Description: Get the - language dependent - name of a database item.
//
// Parameters : database        : database
//              item            : index of the item in the database
//
// Globals    : g_theStringDB
//              g_the<whatever>DB
//
// Returns    : MBCHAR const *  : name of the database item
//
// Remark(s)  : See also GetItemName
//
//----------------------------------------------------------------------------
MBCHAR const * GreatLibrary::GetObjectName(int database, int index) const
{
	if (index < 0)
		return NULL;

	switch (database)
	{
	default:
		return NULL;

	case DATABASE_UNITS:
		return g_theUnitDB->GetNameStr(index);

	case DATABASE_ORDERS:
		return g_theOrderDB->GetNameStr(index);

	case DATABASE_RESOURCE:
		return g_theResourceDB->GetNameStr(index);

	case DATABASE_BUILDINGS:
		return g_theBuildingDB->GetNameStr(index);

	case DATABASE_WONDERS:
		return g_theWonderDB->GetNameStr(index);

	case DATABASE_ADVANCES:
		return g_theAdvanceDB->GetNameStr(index);

	case DATABASE_TERRAIN:
		return g_theTerrainDB->GetNameStr(index);

	case DATABASE_CONCEPTS:
		return g_theConceptDB->GetNameStr(index);

	case DATABASE_GOVERNMENTS:
		return g_theGovernmentDB->GetNameStr(index);

	case DATABASE_TILE_IMPROVEMENTS:
		return g_theTerrainImprovementDB->GetNameStr(index);

	} // switch database
}










const MBCHAR * GreatLibrary::GetSelectionName() const
{
	return GetItemName(m_database, m_selectedIndex);
}




ctp2_Window *GreatLibrary::GetWindow(void) const
{
    return m_window ? m_window->m_window : NULL;
}

//----------------------------------------------------------------------------
//
// Name       : GreatLibrary::SetCategoryName
//
// Description: Get the - language dependent - name of a database item.
//
// Parameters : the_database    : The database category to set
//              item            : index of the item in the database
//
// Globals    : g_theStringDB
//
// Returns    : -
//
// Remark(s)  : Added database search category
//
//----------------------------------------------------------------------------
void GreatLibrary::SetCategoryName
(
	int the_database
)
{

	switch ( the_database )
	{
	case DATABASE_UNITS:

		m_categoryText->SetText(g_theStringDB->GetNameStr("str_ldl_CAPS_UNITS"));

		break;
	case DATABASE_BUILDINGS:

		m_categoryText->SetText(g_theStringDB->GetNameStr("str_ldl_IMPROVE"));

		break;
	case DATABASE_WONDERS:

		m_categoryText->SetText(g_theStringDB->GetNameStr("str_ldl_CAPS_WONDERS"));

		break;
	case DATABASE_ADVANCES:

		m_categoryText->SetText(g_theStringDB->GetNameStr("str_ldl_CAPS_ADVANCES"));

		break;
	case DATABASE_TERRAIN:

		m_categoryText->SetText(g_theStringDB->GetNameStr("str_ldl_TERRAIN"));

		break;
	case DATABASE_CONCEPTS:

		m_categoryText->SetText(g_theStringDB->GetNameStr("str_ldl_CONCEPTS"));

		break;

	case DATABASE_GOVERNMENTS:

		m_categoryText->SetText(g_theStringDB->GetNameStr("str_ldl_GOVERN"));

		break;

	case DATABASE_TILE_IMPROVEMENTS:

		m_categoryText->SetText(g_theStringDB->GetNameStr("str_ldl_CAPS_TILE_IMP"));

		break;

	case DATABASE_ORDERS:

		m_categoryText->SetText(g_theStringDB->GetNameStr("str_ldl_ORDERS"));

		break;

	case DATABASE_RESOURCE:

		m_categoryText->SetText(g_theStringDB->GetNameStr("str_ldl_CAPS_GOODS"));

		break;

	case DATABASE_SEARCH:

		m_categoryText->SetText(g_theStringDB->GetNameStr("str_ldl_CAPS_SEARCH"));

		break;

	default:
		return;

	}

}





void GreatLibrary::HandleIndexButton( ctp2_Button *button )
{

	DATABASE newDatabase = DATABASE_DEFAULT;

	if ( button == m_unitsButton ) {
		newDatabase = DATABASE_UNITS;
	}
	else if ( button == m_searchButton ) {
		newDatabase = DATABASE_SEARCH;
	}
	else if ( button == m_goodsButton ) {
		newDatabase = DATABASE_RESOURCE;
	}
	else if ( button == m_ordersButton ) {
		newDatabase = DATABASE_ORDERS;
	}
	else if ( button == m_improveButton ) {
		newDatabase = DATABASE_BUILDINGS;
	}
	else if ( button == m_wondersButton ) {
		newDatabase = DATABASE_WONDERS;
	}
	else if ( button == m_advancesButton ) {
		newDatabase = DATABASE_ADVANCES;
	}
	else if ( button == m_governButton ) {
		newDatabase = DATABASE_GOVERNMENTS;
	}
	else if ( button == m_terrainButton ) {
		newDatabase = DATABASE_TERRAIN;
	}
	else if ( button == m_tileimpButton ) {
		newDatabase = DATABASE_TILE_IMPROVEMENTS;
	}
	else if ( button == m_conceptButton ) {
		newDatabase = DATABASE_CONCEPTS;
	}

	SetCategoryName(newDatabase);

	m_page = 0;
	m_selectedIndex = CTPRecord::INDEX_INVALID;
	m_topics_list->ShouldDraw();
	UpdateList( newDatabase );
}

//----------------------------------------------------------------------------
//
// Name       : GreatLibrary::GetIndexFromAlpha
//
// Description: Gets the real database index from an language specific
//              alphanumeric index.
//
// Parameters : sint32 alpha:         The language specific alphanumeric index
//              DATABASE theDatabase: Index of the item in the database
//
// Globals    : g_the<whatever>DB
//
// Returns    : The database index
//
// Remark(s)  : If database is invalid or search alpha is returned.
//
//----------------------------------------------------------------------------
sint32 GreatLibrary::GetIndexFromAlpha(sint32 alpha, DATABASE theDatabase) const
{
	switch ( theDatabase )
	{
		case DATABASE_UNITS:
			return g_theUnitDB->m_alphaToIndex[alpha];
		case DATABASE_BUILDINGS:
			return g_theBuildingDB->m_alphaToIndex[alpha];
		case DATABASE_WONDERS:
			return g_theWonderDB->m_alphaToIndex[alpha];
		case DATABASE_ADVANCES:
			return g_theAdvanceDB->m_alphaToIndex[alpha];
		case DATABASE_TERRAIN:
			return g_theTerrainDB->m_alphaToIndex[alpha];
		case DATABASE_CONCEPTS:
			return g_theConceptDB->m_alphaToIndex[alpha];
		case DATABASE_GOVERNMENTS:
			return g_theGovernmentDB->m_alphaToIndex[alpha];
		case DATABASE_TILE_IMPROVEMENTS:
			return g_theTerrainImprovementDB->m_alphaToIndex[alpha];
		case DATABASE_ORDERS:
			return g_theOrderDB->m_alphaToIndex[alpha];
		case DATABASE_RESOURCE:
			return g_theResourceDB->m_alphaToIndex[alpha];
		default:
			return alpha;
	}
}

//----------------------------------------------------------------------------
//
// Name       : GreatLibrary::GetAlphaFromIndex
//
// Description: Gets the alphanumeric language specific alphanumeric index
//              from an index into the according database.
//
// Parameters : sint32 index:         An index into the given database
//              DATABASE theDatabase: Database index
//
// Globals    : g_the<whatever>DB
//
// Returns    : The alphanumeric language specific index
//
// Remark(s)  : If database is invalid or search index is returned.
//
//----------------------------------------------------------------------------
sint32 GreatLibrary::GetAlphaFromIndex(sint32 index, DATABASE theDatabase) const
{
	switch ( theDatabase )
	{
		case DATABASE_UNITS:
			return g_theUnitDB->m_indexToAlpha[index];
		case DATABASE_BUILDINGS:
			return g_theBuildingDB->m_indexToAlpha[index];
		case DATABASE_WONDERS:
			return g_theWonderDB->m_indexToAlpha[index];
		case DATABASE_ADVANCES:
			return g_theAdvanceDB->m_indexToAlpha[index];
		case DATABASE_TERRAIN:
			return g_theTerrainDB->m_indexToAlpha[index];
		case DATABASE_CONCEPTS:
			return g_theConceptDB->m_indexToAlpha[index];
		case DATABASE_GOVERNMENTS:
			return g_theGovernmentDB->m_indexToAlpha[index];
		case DATABASE_TILE_IMPROVEMENTS:
			return g_theTerrainImprovementDB->m_indexToAlpha[index];
		case DATABASE_ORDERS:
			return g_theOrderDB->m_indexToAlpha[index];
		case DATABASE_RESOURCE:
			return g_theResourceDB->m_indexToAlpha[index];
		default:
			return index;
	}
}

//----------------------------------------------------------------------------
//
// Name       : GreatLibrary::HandleListButton
//
// Description: Displays the selected topic when it is klicked in the listbox.
//
// Parameters :
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void GreatLibrary::HandleListButton
(
	aui_Control *   control,
	uint32          action,
	uint32          data,
	void *          cookie
)
{
	if (AUI_LISTBOX_ACTION_SELECT == action)
    {
		ctp2_ListItem *item = (ctp2_ListItem *) m_topics_list->GetSelectedItem();

		if (item)
		{
		    int const   index = reinterpret_cast<int>(item->GetUserData());
		    if (index >= 0) {
			    SetLibrary(GetIndexFromAlpha(index, m_listDatabase), m_listDatabase);
		    } else {
			    m_topics_list->SelectItem(m_topics_list->GetSelectedItemIndex() + 1);
		    }
        }
    }
}

//----------------------------------------------------------------------------
//
// Name       : GreatLibrary::IsHidden
//
// Description: Figures out whether a given item in a given database should
//              not be displayed in the Great Library
//
// Parameters : sint32 index:         An index into the given database
//              DATABASE theDatabase: Database index
//
// Globals    : g_the<whatever>DB
//
// Returns    : Whether the given item in the given database has the GLHidden
//              attribute.
//
// Remark(s)  : If database is invalid or search false is returned.
//
//----------------------------------------------------------------------------
bool GreatLibrary::IsHidden(sint32 index, DATABASE theDatabase) const
{
	switch ( theDatabase )
	{
		case DATABASE_UNITS:
			return g_theUnitDB->Get(index)->GetGLHidden();
		case DATABASE_BUILDINGS:
			return g_theBuildingDB->Get(index)->GetGLHidden();
		case DATABASE_WONDERS:
			return g_theWonderDB->Get(index)->GetGLHidden();
		case DATABASE_ADVANCES:
			return g_theAdvanceDB->Get(index)->GetGLHidden();
		case DATABASE_TERRAIN:
			return g_theTerrainDB->Get(index)->GetGLHidden();
		case DATABASE_CONCEPTS:
			return g_theConceptDB->Get(index)->GetGLHidden();
		case DATABASE_GOVERNMENTS:
			return g_theGovernmentDB->Get(index)->GetGLHidden();
		case DATABASE_TILE_IMPROVEMENTS:
			return g_theTerrainImprovementDB->Get(index)->GetGLHidden();
		case DATABASE_ORDERS:
			return g_theOrderDB->Get(index)->GetGLHidden();
		case DATABASE_RESOURCE:
			return g_theResourceDB->Get(index)->GetGLHidden();
		default:
			return false;
	}
}

//----------------------------------------------------------------------------
//
// Name       : GreatLibrary::UpdateList
//
// Description: Fills the topic listbox.
//
// Parameters : DATABASE Database: Database index
//
// Globals    : g_the<whatever>DB
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void GreatLibrary::UpdateList( DATABASE database )
{
	sint32 index;

	m_topics_list->Clear();
	m_listDatabase = database;

#define HIDE(db, index) (db->Get(db->m_alphaToIndex[index])->GetGLHidden())

	switch ( database ) {
	case DATABASE_UNITS:
		AddTopicsBasedOnAge<UnitRecord>(g_theUnitDB);
		break;

	case DATABASE_SEARCH:

		Search_Great_Library();

		for (index = 0; index < static_cast<sint32>(m_search_results.size()); index++)
		{

			int real_index = m_search_results[index].m_item;
			enum DATABASE real_database = m_search_results[index].m_database;

			if(IsHidden(real_index, real_database)) continue;

			real_index = GetAlphaFromIndex(real_index, real_database);

			Add_Item_To_Topics_List(GetItemName(real_database, real_index), index);

		}

		break;

	case DATABASE_ORDERS:

		for (index = 0; index < g_theOrderDB->NumRecords(); index++)
		{
			if(HIDE(g_theOrderDB, index)) continue;

			Add_Item_To_Topics_List(g_theStringDB->GetNameStr(g_theOrderDB->GetName(
						g_theOrderDB->m_alphaToIndex[ index ])), index);

		}

		break;

	case DATABASE_RESOURCE:

		for (index = 0; index < g_theResourceDB->NumRecords(); index++)
		{
			if(HIDE(g_theResourceDB, index)) continue;

			Add_Item_To_Topics_List(g_theStringDB->GetNameStr(g_theResourceDB->GetName(
						g_theResourceDB->m_alphaToIndex[ index ])), index);

		}

		break;

	case DATABASE_BUILDINGS:
		AddTopicsBasedOnAge<BuildingRecord>(g_theBuildingDB);
		break;

	case DATABASE_WONDERS:
		AddTopicsBasedOnAge<WonderRecord>(g_theWonderDB);
		break;

	case DATABASE_ADVANCES:
		AddTopicsBasedOnAge<AdvanceRecord>(g_theAdvanceDB);
		break;

	case DATABASE_TERRAIN:

		for (index = 0; index < g_theTerrainDB->NumRecords(); index++)
		{
			if(HIDE(g_theTerrainDB, index)) continue;

			Add_Item_To_Topics_List(g_theStringDB->GetNameStr(g_theTerrainDB->GetName(
						g_theTerrainDB->m_alphaToIndex[ index ])), index);

		}

		break;

	case DATABASE_CONCEPTS:

		for (index = 0; index < g_theConceptDB->NumRecords(); index++)
		{

			if(HIDE(g_theConceptDB, index)) continue;

			Add_Item_To_Topics_List(g_theConceptDB->GetNameStr(
							g_theConceptDB->m_alphaToIndex[ index ]), index);

		}

		break;

	case DATABASE_GOVERNMENTS:
		AddTopicsBasedOnAge<GovernmentRecord>(g_theGovernmentDB);
		break;

	case DATABASE_TILE_IMPROVEMENTS:
		AddTopicsBasedOnAge<TerrainImprovementRecord>(g_theTerrainImprovementDB);
		break;

	default:
        return;

	}

	m_maxIndex = index;
}

//----------------------------------------------------------------------------
//
// Name       : GreatLibrary::Get_Database_Size
//
// Description: Gets the size of a database.
//
// Parameters : DATABASE the_database: Database index
//
// Globals    : g_the<whatever>DB
//
// Returns    : Size of the given database
//
// Remark(s)  : Returns 0 if database is invalid
//
//----------------------------------------------------------------------------
int GreatLibrary::Get_Database_Size(int the_database)
{
	switch (the_database)
    {
	default:
        return 0;

	case DATABASE_UNITS:
        return g_theUnitDB->NumRecords();

	case DATABASE_SEARCH:
		return m_search_results.size();

	case DATABASE_ORDERS:
		return g_theOrderDB->NumRecords();

	case DATABASE_RESOURCE:
		return g_theResourceDB->NumRecords();

	case DATABASE_BUILDINGS:
		return g_theBuildingDB->NumRecords();

	case DATABASE_WONDERS:
		return g_theWonderDB->NumRecords();

	case DATABASE_ADVANCES:
		return g_theAdvanceDB->NumRecords();

	case DATABASE_TERRAIN:
		return g_theTerrainDB->NumRecords();

	case DATABASE_CONCEPTS:
		return g_theConceptDB->NumRecords();

	case DATABASE_GOVERNMENTS:
		return g_theGovernmentDB->NumRecords();

	case DATABASE_TILE_IMPROVEMENTS:
		return g_theTerrainImprovementDB->NumRecords();

	}
}

//----------------------------------------------------------------------------
//
// Name       : GreatLibrary::Search_Great_Library
//
// Description: Searches in the topic titles, historic gameplay, vari and prereq
//              texts of the Great Library for a key word.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void GreatLibrary::Search_Great_Library()
{
	const int   MAX_SEARCH_KEY              = 100;
	char        search_key[MAX_SEARCH_KEY];
	m_search_word->GetFieldText(search_key, MAX_SEARCH_KEY);

    String_Search::Set_Case_Sensitive(false);
	String_Search::Set_Search_Key(search_key);

	Great_Library_Item  current;
	m_search_results.clear();

	for (int i = DATABASE_UNITS; i < DATABASE_SEARCH; i++)
	{
		for (int j = 0; j < Get_Database_Size(i); j++)
		{
			bool match = false;

			char const * gameplay_text = m_window->GetGameplayText(i, j);
			if (gameplay_text)
				match |= String_Search::Search(gameplay_text);

			char const * historical_text = m_window->GetHistoricalText(i, j);
			if (historical_text)
				match |= String_Search::Search(historical_text);

			char const * title_text = GetObjectName(i, j);
			if (title_text)
				match |= String_Search::Search(title_text);

			char const * requirements_text = m_window->GetRequirementsText(i, j);
			if (requirements_text)
				match |= String_Search::Search(requirements_text);

			char const * variables_text = m_window->GetVariablesText(i, j);
			if (variables_text)
				match |= String_Search::Search(variables_text);

			if (match)
			{
				current.m_database = (enum DATABASE) i;
				current.m_item = j;

				m_search_results.push_back(current);
			}
		}
	}
}









void GreatLibrary::Force_A_Search()
{
	m_listDatabase  = DATABASE_SEARCH;
	m_page          = 0;
	m_selectedIndex = CTPRecord::INDEX_INVALID;
	SetCategoryName(m_listDatabase);
	m_topics_list->ShouldDraw();
	UpdateList(m_listDatabase);
}

//----------------------------------------------------------------------------
//
// Name       : GreatLibrary::Back
//
// Description: Shows the previous topic.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void GreatLibrary::Back()
{
	if (m_history_position > 0)
    {
	    m_history_position--;

	    m_backButton->Enable(m_history_position > 0);
	    m_forwardButton->Enable(true);

	    enum DATABASE   next_database   = m_history[m_history_position].m_database;
	    int             next_index      = m_history[m_history_position].m_item;

	    SetLibrary(next_index, next_database, false);
    }
}

//----------------------------------------------------------------------------
//
// Name       : GreatLibrary::Forward
//
// Description: Shows the next topic, given back was used before.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void GreatLibrary::Forward()
{
	if (m_history_position + 1 < static_cast<sint32>(m_history.size()))
    {
    	m_history_position++;

    	m_backButton->Enable(true);
		m_forwardButton->Enable(m_history_position + 1 < static_cast<sint32>(m_history.size()));

	    enum DATABASE   next_database   = m_history[m_history_position].m_database;
	    int             next_index      = m_history[m_history_position].m_item;

	    SetLibrary(next_index, next_database, false);
    }
}

void GreatLibrary::Add_Item_To_Topics_List
(
	const MBCHAR *name,
	int index
)
{
	Assert(m_topics_list);
	if(!m_topics_list) return;

	ctp2_ListItem *item = (index == CTPRecord::INDEX_INVALID)
			? (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("GreatLibraryHeaderItem")
			: (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("GreatLibraryTopicItem");
	Assert(item);
	if(!item) return;

	ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
	Assert(box);
	if(!box) return;

	box->SetText(name);
	item->SetUserData((void *) index);
	m_topics_list->AddItem(item);
}

void GreatLibrary::FixTabs()
{
	ctp2_Tab *curTab = m_tabGroup->GetCurrentTab();

	m_tabGroup->SelectTab(m_historicalTab);
	m_tabGroup->SelectTab(m_gameplayTab);

	m_tabGroup->SelectTab(curTab->IsDisabled() ? m_gameplayTab : curTab);
}

class AgeSortRecord {
public:
	AgeSortRecord()
	: m_age(-1),
		m_name(NULL),
		m_index(CTPRecord::INDEX_INVALID),
		m_isHidden(false)
	{}

	void Initialize(sint32 age, const MBCHAR * name, sint32 index, bool isHidden)
	{
		m_age      = age;
		m_name     = name;
		m_index    = index;
		m_isHidden = isHidden;
	}

	bool IsHidden() const {
		return m_isHidden;
	}

	sint32 GetAge() const {
		return m_age;
	}

	const MBCHAR * GetName() const {
		return m_name;
	}

	sint32 GetIndex() const {
		return m_index;
	}

	bool operator < (const AgeSortRecord & other) const
	{
		if (m_age != other.m_age) {
			return (m_age < other.m_age);
		} else {
			return _stricoll(m_name, other.m_name) < 0;
		}
	}

private:
	sint32         m_age;
	const MBCHAR * m_name;
	sint32         m_index;
	bool           m_isHidden;
};

template <class T>
sint32 GetAgeIndex(const T * topic)
{
	sint32 advanceIndex = topic->GetEnableAdvanceIndex();
	return advanceIndex >= 0 ? GetAgeIndex(g_theAdvanceDB->Get(advanceIndex)) : 0;
}

template <>
sint32 GetAgeIndex<AdvanceRecord>(const AdvanceRecord * topic)
{
	return topic->GetAgeIndex();
}

template <>
sint32 GetAgeIndex<TerrainImprovementRecord>(const TerrainImprovementRecord * topic)
{
	sint32 advanceIndex = topic->GetTerrainEffect(0)->GetEnableAdvanceIndex();
	return advanceIndex >= 0 ? GetAgeIndex(g_theAdvanceDB->Get(advanceIndex)) : 0;
}

template <class T>
void GreatLibrary::AddTopicsBasedOnAge(CTPDatabase<T> * database)
{
	std::vector<AgeSortRecord> ageSortRecords(database->NumRecords());
	for (sint32 index = 0; index < database->NumRecords(); index++)
	{
		const T * topic = database->Get(database->m_alphaToIndex[index]);
		ageSortRecords[index].Initialize(GetAgeIndex(topic), g_theStringDB->GetNameStr(topic->GetName()), index,
				topic->GetGLHidden());
	}

	// Sort ageSortRecords using std::sort
	std::sort(ageSortRecords.begin(), ageSortRecords.end());

	sint32 currentAge = -1;
	for (const auto& ageSortRecord : ageSortRecords)
	{
		if (ageSortRecord.IsHidden()) {
			continue;
		}

		if (ageSortRecord.GetAge() != currentAge) {
			Add_Item_To_Topics_List(g_theAgeDB->GetNameStr(ageSortRecord.GetAge()), CTPRecord::INDEX_INVALID);
			currentAge = ageSortRecord.GetAge();
		}
		Add_Item_To_Topics_List(ageSortRecord.GetName(), ageSortRecord.GetIndex());
	}
}
