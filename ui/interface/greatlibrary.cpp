


#include "c3.h"


#include "aui.h"
#include "aui_uniqueid.h"
#include "c3ui.h"
#include "aui_ldl.h"
#include "aui_stringtable.h"


#include "ctp2_window.h"


#include "ctp2_Button.h"

#include "aui_switchgroup.h"
#include "ctp2_tabgroup.h"
#include "ctp2_tab.h"
#include "texttab.h"
#include "c3_button.h"


#include "aui_static.h"


#include "aui_listbox.h"
#include "ctp2_HyperTextBox.h"


#include "ctp2_Button.h"
#include "ctp2_ListBox.h"
#include "ctp2_ListItem.h"
#include "ctp2_Static.h"

#include "SlicObject.h"
#include "SelItem.h"
#include "Player.h"

#include "ControlSheet.h"
#include "videoutils.h"

#include "pixelutils.h"
#include "ColorSet.h"


#include "AdvanceRecord.h"

#include "ConceptDB.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "StrDB.h"
#include "TerrainRecord.h"
#include "GovernmentRecord.h"
#include "TerrainImprovementRecord.h"


#include "ResourceRecord.h"


#include "OrderRecord.h"

#include "profileDB.h"


#include "chart.h"

#include "uiutils.h"
#include "screenutils.h"

#include "greatlibrarywindow.h"
#include "directvideo.h"
#include "greatlibrary.h"

#include "keypress.h"
#include "UnitRecord.h"
#include "GovernmentRecord.h"

#include "wonderutil.h"


#include "Text_Hasher.h"


#include "MessageBoxDialog.h"


#include "ctp2_TextField.h"
#include <Vector>
#include "String_Search.h"

#include "Sci_Advancescreen.h"
#include "stdlib.h"




extern sint32		g_ScreenWidth;
extern sint32		g_ScreenHeight;

extern C3UI			*g_c3ui;
extern ColorSet		*g_colorSet;

extern sint32		g_modalWindow;

extern ConceptDB					*g_theConceptDB;
extern StringDB						*g_theStringDB;
extern ProfileDB			*g_theProfileDB;

extern aui_Surface			*g_sharedSurface;


static char s_database_names[DATABASE_MAX][200] =
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


Text_Hasher<char *> * GreatLibrary::m_great_library_info = NULL;






void GreatLibrary::Initialize_Great_Library_Data()
{
	
	const int GREAT_LIBRARY_HASH_SIZE = 2000;

	
	m_great_library_info = new Text_Hasher<char *> 
		(
			GREAT_LIBRARY_HASH_SIZE,
			NULL
		);

	
	Load_Great_Library();

}











void GreatLibrary::Shutdown_Great_Library_Data()
{
	
	delete m_great_library_info;
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
	
	char gl_path[512];

	MBCHAR path[_MAX_PATH];
	int i=0;
	while(g_civPaths->FindPath(C3DIR_GL, i++, path)) 
	{
		if (path[0]) 
			break;
	}

	
	sprintf(gl_path, "%s\\Great_Library.txt", path);

	
	FILE * great_library = c3files_fopen(C3DIR_GL, "Great_Library.txt", "r");

	Assert(great_library);

	
	if (!great_library)
		return;

	
	{
		
		char ch;
		
		
		const int MAX_NAME = 1024;
		const int MAX_ENTRY = 4096;
		char the_name[MAX_NAME];
		char the_entry[MAX_ENTRY];
		int name_pos;
		int entry_pos;
		char * end_ptr;		

		
		Read_Library_State reading_what = LOOKING_FOR_NAME;

		
		while (!feof(great_library))
		{
			
			ch = fgetc(great_library);

			
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
						
						the_name[name_pos++] = ch;

						
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

						
						the_entry[entry_pos++] = ch;

						
						reading_what = IN_END;

					}
					else
					{
						
						the_entry[entry_pos++] = ch;

						
						
						Assert(entry_pos < MAX_ENTRY - 6);
						if (entry_pos >= MAX_ENTRY - 6)
						{
							
							end_ptr = &(the_entry[entry_pos]);

							
							while (ch != '[')
							{
								ch = fgetc(great_library);
							}

							
							the_entry[entry_pos++] = ch;

							
							reading_what = IN_END;
						}
					}


				}
				break;

			
			case IN_END:
				{
					
					the_entry[entry_pos++] = ch;

					
					if (ch == ']')
					{
						
						the_entry[entry_pos] = 0;

						
						if (!strcmp(end_ptr, "[END]"))
						{
							
							*end_ptr = 0;

							
							char * name_copy = new char[strlen(the_name)+1];
							char * entry_copy = new char[strlen(the_entry)+1];
							strcpy(name_copy, the_name);
							strcpy(entry_copy, the_entry);
							name_copy[strlen(the_name)] = 0;
							entry_copy[strlen(the_entry)] = 0;

							
							for (int j = 0; j < strlen(name_copy); j++)
								name_copy[j] = tolower(name_copy[j]);

							
							m_great_library_info->Add_To_Hash_Table(
								name_copy,
								entry_copy
								);

							
							reading_what = LOOKING_FOR_NAME;
						}
						
						else
						{
							
							reading_what = IN_TEXT;
						}

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
	char * database_name
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

	
	return 0;

}











int GreatLibrary::Get_Object_Index_From_Name
(
	int which_database,					
	char * object_name
)
{
	
	
	int index;
	
	
	char * db_item_name = NULL;


	
	switch ( which_database ) {
	case DATABASE_UNITS:
		for (index = 0; index < g_theUnitDB->NumRecords(); index++)
		{
			
			db_item_name = g_theStringDB->GetIdStr(g_theUnitDB->Get(index)->m_name);

			
			if (!strcmp(db_item_name, object_name))
			{
				
				return index;

			} 

		} 

		break;

	case DATABASE_SEARCH:
		
		
		return 0;
		break;

	case DATABASE_ORDERS:
		for (index = 0; index < g_theOrderDB->NumRecords(); index++)
		{
			
			db_item_name = g_theStringDB->GetIdStr(g_theOrderDB->Get(index)->m_name);

			
			if (!strcmp(db_item_name, object_name))
			{
				
				return index;

			} 

		} 


		break;

	case DATABASE_RESOURCE:
		for (index = 0; index < g_theResourceDB->NumRecords(); index++)
		{
			
			db_item_name = g_theStringDB->GetIdStr(g_theResourceDB->Get(index)->m_name);

			
			if (!strcmp(db_item_name, object_name))
			{
				
				return index;

			} 

		} 


		break;

	case DATABASE_BUILDINGS:
		for (index = 0; index < g_theBuildingDB->NumRecords(); index++)
		{
			
			db_item_name = g_theStringDB->GetIdStr(g_theBuildingDB->Get(index)->m_name);

			
			if (!strcmp(db_item_name, object_name))
			{
				
				return index;

			} 

		} 

		break;

	case DATABASE_WONDERS:
		for (index = 0; index < g_theWonderDB->NumRecords(); index++)
		{
			
			db_item_name = g_theStringDB->GetIdStr(g_theWonderDB->Get(index)->m_name);

			
			if (!strcmp(db_item_name, object_name))
			{
				
				return index;

			} 

		} 

		break;

	case DATABASE_ADVANCES:

		for (index = 0; index < g_theAdvanceDB->NumRecords(); index++)
		{
			
			db_item_name = g_theStringDB->GetIdStr(g_theAdvanceDB->Get(index)->m_name);

			
			if (!strcmp(db_item_name, object_name))
			{
				
				return index;

			} 

		} 


		break;

	case DATABASE_TERRAIN:
	{
		for (index = 0; index < g_theTerrainDB->NumRecords(); index++)
		{
			
			db_item_name = g_theStringDB->GetIdStr(g_theTerrainDB->Get(index)->m_name);

			
			if (!strcmp(db_item_name, object_name))
			{
				
				return index;

			} 

		} 

		break;
	}
	case DATABASE_CONCEPTS:
		for (index = 0; index < g_theConceptDB->m_nConcepts; index++)
		{
			
			db_item_name = g_theStringDB->GetIdStr(g_theConceptDB->GetConceptInfo(index)->m_name);

			
			if (!strcmp(db_item_name, object_name))
			{
				
				return index;

			} 

		} 

		break;

	case DATABASE_GOVERNMENTS:
		for (index = 0; index < g_theGovernmentDB->NumRecords(); index++)
		{
			
			db_item_name = g_theStringDB->GetIdStr(g_theGovernmentDB->Get(index)->m_name);

			
			if (!strcmp(db_item_name, object_name))
			{
				
				return index;

			} 

		} 

		break;

	case DATABASE_TILE_IMPROVEMENTS:
		for (index = 0; index < g_theTerrainImprovementDB->NumRecords(); index++)
		{
			
			db_item_name = g_theStringDB->GetIdStr(g_theTerrainImprovementDB->Get(index)->m_name);

			
			if (!strcmp(db_item_name, object_name))
			{
				
				return index;

			} 

		} 

		break;

	default:
		BOOL InvalidDatabase = FALSE;
		Assert(InvalidDatabase);
		break;
	}

	
	bool bad_object_name = false;

	
	Assert(bad_object_name);

	
	return 0;
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
	sint32 numPreReq = chart->GetNumPreReq();

	for ( sint32 i = 0;i < numPreReq;i++ ) {
		if ( control->Id() == chart->GetPreReqButton(i)->Id() ) {
			g_greatLibrary->SetLibrary( chart->GetPreReqIndex(i), DATABASE_ADVANCES );
		}
	}

}

void greatlibrary_LeadsToActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Chart *chart = (Chart *)cookie;
	sint32 numLeadsTo = chart->GetNumLeadsTo();

	for ( sint32 i = 0;i < numLeadsTo;i++ ) {
		if ( control->Id() == chart->GetLeadsToButton(i)->Id() ) {
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




TechListItem::TechListItem(AUI_ERRCODE *retval, sint32 index, DATABASE database, MBCHAR *ldlBlock)
	:
	ctp2_ListItem( retval, ldlBlock),
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(index, database, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;	
}


AUI_ERRCODE TechListItem::InitCommonLdl(sint32 index, DATABASE database, MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	m_index = index;
	m_database = database;

	ctp2_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "Name");
	subItem = new ctp2_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	Update();

	return AUI_ERRCODE_OK;
}

void TechListItem::Update(void)
{

	ctp2_Static *subItem;

	
	subItem = (ctp2_Static *)GetChildByIndex(0);

	
	enum DATABASE real_database = m_database;
	int real_index = m_index;

	
	if (m_database == DATABASE_SEARCH)
	{
		
		real_database = g_greatLibrary->m_search_results[m_index].m_database;
		real_index = g_greatLibrary->m_search_results[m_index].m_item;

	} 

	switch ( real_database ) {
	case DATABASE_UNITS:
		subItem->SetText( g_theStringDB->GetNameStr(g_theUnitDB->Get(real_index)->GetName()) );
		break;

	case DATABASE_ORDERS:
		subItem->SetText( g_theStringDB->GetNameStr(g_theOrderDB->Get(real_index)->GetName()) );
		break;
		
	case DATABASE_RESOURCE:
		subItem->SetText( g_theStringDB->GetNameStr(g_theResourceDB->Get(real_index)->GetName()) );
		break;
		
	case DATABASE_BUILDINGS:
		subItem->SetText( g_theStringDB->GetNameStr(g_theBuildingDB->Get(real_index)->GetName()) );
		break;

	case DATABASE_WONDERS:
		subItem->SetText( g_theStringDB->GetNameStr(wonderutil_Get(real_index)->GetName()) );
		break;

	case DATABASE_ADVANCES:
		subItem->SetText( g_theAdvanceDB->GetNameStr(real_index) );
		break;

	case DATABASE_TERRAIN:
		subItem->SetText( g_theStringDB->GetNameStr(g_theTerrainDB->GetName(real_index)) );
		break;

	case DATABASE_CONCEPTS:
		subItem->SetText( g_theConceptDB->GetNameStr(real_index) );
		break;

	case DATABASE_GOVERNMENTS:
		subItem->SetText( g_theStringDB->GetNameStr(g_theGovernmentDB->GetName(real_index)) );
		break;

	case DATABASE_TILE_IMPROVEMENTS:
		subItem->SetText( g_theTerrainImprovementDB->GetNameStr(real_index) );
		break;

	default:
		BOOL invalidDatabase = FALSE;
		Assert( invalidDatabase );
	}
}

sint32 TechListItem::Compare(ctp2_ListItem *item2, uint32 column)
{
	sint32	val1, val2;

	if (column < 0) return 0;

	switch (column) {
	case 0:
		val1 = m_index; 
		val2 = ((TechListItem *)item2)->GetIndex();
		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;

		break;
	}

	return 0;
}

sint32 greatlibrary_Initialize( sint32 theMode, BOOL sci )
{
	if ( g_greatLibrary ) {
		if ( sci ) {
			g_greatLibrary->SetLibrary( theMode, DATABASE_ADVANCES );
		}
		else {
			g_greatLibrary->SetLibrary( g_greatLibrary->m_window->GetTechMode(), g_greatLibrary->m_window->GetTechDatabase() );
		}
		g_greatLibrary->SetSci( sci );
		g_greatLibrary->GetWindow()->MoveOG();
		return 0;	
	}

	g_greatLibrary = new GreatLibrary( theMode );
	g_greatLibrary->SetSci( sci );


	return 0;
}

sint32 greatlibrary_Cleanup( void )
{
	if ( g_greatLibrary ) {
		
		
		
		g_greatLibrary->Remove();
		delete g_greatLibrary;
	}
	g_greatLibrary = NULL;

	return 0;
}

GreatLibrary::GreatLibrary( sint32 theMode )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	m_page = 0;
	m_maxPage = FALSE;
	m_selectedIndex = -1;
	m_database = DATABASE_UNITS;
	m_listDatabase = DATABASE_UNITS;
	m_maxIndex = 0;

	
	m_history_position = 0;

	strcpy(windowBlock,"GreatLibrary");

	m_window = new GreatLibraryWindow(&errcode);

	m_window->m_window->SetDraggable(TRUE);






















	m_techMovie = NULL;


	
	Initialize( windowBlock );

	m_techTree->Update( theMode );
	m_window->SetTechMode( theMode, DATABASE_ADVANCES );

	
	m_window->SetTechTree ( m_techTree );
	m_window->SetTechStillShot ( m_techStillShot );
	m_window->SetTechHistoricalText ( m_techHistoricalText );
	m_window->SetTechGameplayText ( m_techGameplayText );
	m_window->SetTechMovie ( m_techMovie );
	m_window->SetTechRequirementsText ( m_techRequirementsText );
	m_window->SetTechVariablesText ( m_techVariablesText );

	
	SlicObject so;
	so.AddAdvance(theMode);

 	
	m_window->LoadGameplayText( so );
	if(m_window->LoadHistoricalText( so ) == GreatLibraryWindow::GREAT_LIBRARY_PANEL_BLANK) {
		m_historicalTab->Enable(FALSE);
	} else {
		m_historicalTab->Enable(TRUE);
	}
	FixTabs();
	m_window->LoadRequirementsText( so );
	m_window->LoadVariablesText( so );


	
	if ( g_theProfileDB->IsLibraryAnim() ) {
		m_techStillShot->Hide();
		if (m_techMovie) {
			if (m_techMovie->Open()) m_techMovie->CloseStream();
		}
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

	
	m_buttonString = LIB_STRING_INDEX;

}

sint32 GreatLibrary::Initialize( MBCHAR *windowBlock )
{
	AUI_ERRCODE errcode;
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

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

		m_techMovie = new DirectVideo();
		m_techMovie->Initialize((aui_DirectUI *)g_c3ui, (aui_Window *)GetWindow(), FALSE);
		m_techMovie->SetDestRect(&rect);
	}


	m_techStillShot = (ctp2_Static *)aui_Ldl::GetObject(windowBlock, "TechStillShot");


	sprintf( controlBlock, "LibraryStrings" );
	m_string = new aui_StringTable( &errcode, controlBlock );
	TestControl( m_string );


	ctp2_Static *control = (ctp2_Static *)aui_Ldl::GetObject(windowBlock, "Tabs.TechTreeTab.TabPanel");
	m_techTree = new Chart(&errcode, aui_UniqueId(), "TechTree", NULL, NULL);
	control->AddChild(m_techTree);

	sint32 i;
	for ( i = 0;i < 4 ;i++ ) {
		m_techTree->GetPreReqButton(i)->SetActionFuncAndCookie( greatlibrary_PrereqActionCallback, m_techTree );
	}

	for ( i = 0;i < 4 ;i++ ) {
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

	sprintf( controlBlock, "%s", "IndexButtonSwitchGroup" );
	ctp2_Static *switchGroup = (ctp2_Static *)aui_Ldl::GetObject(windowBlock, controlBlock);
	switchGroup->SetBlindness( TRUE );

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

	return 0;
}

GreatLibrary::~GreatLibrary( void )
{
	
	





	if (m_window) delete m_window;

	
	
	
	if(m_techTree)
	{
		delete m_techTree;
		m_techTree = NULL;
	}
	if (m_string)
	{
		delete m_string;
		m_string = NULL;
	}
	if (m_techMovie)
	{
		delete m_techMovie;
		m_techMovie = NULL;
	}
}

void GreatLibrary::Display( void )
{
	AUI_ERRCODE errcode;
	
	GetWindow()->SetType(AUI_WINDOW_TYPE_FLOATING);
	errcode = g_c3ui->AddWindow( GetWindow() );
	Assert( errcode == AUI_ERRCODE_OK );
	
	

	keypress_RegisterHandler(this);

	
	m_buttonString = LIB_STRING_INDEX;

	ClearHistory();
	SetLibrary( 0, DATABASE_ADVANCES );

	
	
	
	
	FixTabs();
}

void GreatLibrary::Remove( void )
{
	AUI_ERRCODE errcode;

	errcode = g_c3ui->RemoveWindow( GetWindow()->Id() );
	Assert( errcode == AUI_ERRCODE_OK );

	
	

	keypress_RemoveHandler(this);

}

sint32 GreatLibrary::SetLibrary( sint32 theMode, DATABASE theDatabase, bool add_to_history )
{

	m_selectedIndex = theMode;

	
	bool there_is_a_gameplay_panel = true;

	
	bool there_is_an_historical_panel = true;

	
	enum DATABASE real_database = theDatabase;
	int real_index = theMode;

	if (!g_greatLibrary || !g_greatLibrary->m_window) return 0;

	
	sint32 videoWidth = 242;
	sint32 videoHeight = 182;
	sint32 videoX = 382;
	sint32 videoY = 316;

	
	sint32 windowWidth = g_ScreenWidth;
	sint32 windowHeight = g_ScreenHeight;
	sint32 windowX = 0;
	sint32 windowY = 0;

	sint32 hidden;

	
	SetCategoryName(theDatabase);

	
	SlicObject so;
	
	if(theDatabase == DATABASE_SEARCH) {
		
		theDatabase = g_greatLibrary->m_search_results[theMode].m_database;
		theMode = g_greatLibrary->m_search_results[theMode].m_item;
	}
	else
	{
		if(theDatabase == DATABASE_CONCEPTS)
		{
			theMode = g_theConceptDB->m_alphaToIndex[ theMode ];
		}
	}

	
	
	if ( theDatabase == DATABASE_ADVANCES)
	{
		m_techTab->Enable( TRUE );
		FixTabs();
	} else
	{
		m_techTab->Enable( FALSE );
		FixTabs();
	}
	
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

		hidden = m_techTree->IsHidden();
		if ( hidden ) {
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
		m_itemLabel->SetText(g_theConceptDB->GetNameStr(theMode));
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
		m_itemLabel->SetText(g_theTerrainImprovementDB->Get(theMode)->GetNameText());
		break;
	}

	if(enableGoal) {
		m_setGoalButton->Enable(TRUE);
	} else {
		m_setGoalButton->Enable(FALSE);
	}

	m_database = theDatabase;

	
	if (add_to_history)
	{
		
		Great_Library_Item new_gl_page;

		
		new_gl_page.m_database = real_database;
		new_gl_page.m_item = real_index;

		
		
		
		if ((m_history.size() != 0) && (m_history_position < m_history.size() - 1))
		{
			
			
			m_history.erase(m_history.begin() + m_history_position + 1, m_history.end());
		}

		
		m_history.push_back(new_gl_page);

		
		m_history_position = m_history.size() - 1;

		
		m_forwardButton->Enable(FALSE);

		
		if (m_history_position == 0)
		{
			
			m_backButton->Enable(FALSE);

		} 

		else
		{
			
			m_backButton->Enable(TRUE);
		}

	} 
	
	

	
	
	if (m_techMovie) {
		if (m_techMovie->Open()) m_techMovie->CloseStream();
		delete m_techMovie;
		m_techMovie = NULL;
	}

	RECT rect = {
		k_VIDEO_X,
		k_VIDEO_Y,
		k_VIDEO_X + k_VIDEO_WIDTH,
		k_VIDEO_Y + k_VIDEO_HEIGHT};

	if ( g_theProfileDB->IsLibraryAnim() ) {
		m_techMovie = new DirectVideo();
		m_techMovie->Initialize((aui_DirectUI *)g_c3ui, (aui_Window *)GetWindow(), FALSE);
		m_techMovie->SetDestRect(&rect);

	}
	else {
		m_techStillShot->Show();
		GetWindow()->ShouldDraw();
	}

	
	m_window->SetTechTree ( m_techTree );
	m_window->SetTechMovie ( m_techMovie );

	
	int text_load_result;

	
	text_load_result = m_window->LoadGameplayText(so);


























	
	text_load_result = m_window->LoadHistoricalText(so);
	if(text_load_result == GreatLibraryWindow::GREAT_LIBRARY_PANEL_BLANK) {
		m_historicalTab->Enable(FALSE);
	} else {
		m_historicalTab->Enable(TRUE);
	}
	FixTabs();


























	

	m_window->LoadRequirementsText(so);
	m_window->LoadVariablesText(so);

	
	if ( g_theProfileDB->IsLibraryAnim() ) {
		if (m_techMovie) {
			if (m_techMovie->Open()) 
				m_techMovie->CloseStream();
		}
		if (!g_greatLibrary->m_window->LoadTechMovie()) {
		}
		if ( m_window->LoadTechStill() ) {
			m_techStillShot->Show();
			GetWindow()->ShouldDraw();
		}
	}
	else {
		if ( !m_window->LoadTechStill() ) {
			m_techStillShot->Hide();
		}
		else {
			m_techStillShot->Show();
			GetWindow()->ShouldDraw();
		}
	}




















	return 0;
}

sint32 GreatLibrary::ClearHistory( void )
{
	m_history_position = 0;
	m_history.clear();

	m_selectedIndex = 0;
	return 0;
}
















































































































sint32 GreatLibrary::HandleSetGoal( void )
{

	
	MBCHAR goal_set_message[500];

	
	const MBCHAR *selection_name = GetItemName(m_database, m_window->GetTechMode());

	
	if (!selection_name)
		return -1;

	
	const MBCHAR *fmt = g_theStringDB->GetNameStr("str_ldl_GreatLibraryGoalSetTo");
	if(!fmt) fmt = "Goal set to: %s";
	sprintf(goal_set_message, fmt, selection_name);

	
	MessageBoxDialog::Information(goal_set_message, "InfoSetGoal");

	g_player[g_selected_item->GetVisiblePlayer()]->SetResearchGoal(m_database, m_window->GetTechMode());

	return 0;
}










const MBCHAR * GreatLibrary::GetItemName(int database, int item)
{
	
	if (item == -1)
		return NULL;

	
	switch ( database ) 
	{
	case DATABASE_UNITS:

		return g_theStringDB->GetNameStr(g_theUnitDB->GetName(
				g_theUnitDB->m_alphaToIndex[item])) ;

		break;

	case DATABASE_SEARCH:
		
		enum DATABASE real_database;
		int real_index;

		
		real_database = g_greatLibrary->m_search_results[item].m_database;
		real_index = g_greatLibrary->m_search_results[item].m_item;

		
		return GetItemName(real_database, real_index);
		break;

	case DATABASE_ORDERS:

		return g_theStringDB->GetNameStr(g_theOrderDB->GetName(
				g_theOrderDB->m_alphaToIndex[item])) ;

		break;

	case DATABASE_RESOURCE:

		return g_theStringDB->GetNameStr(g_theResourceDB->GetName(
				g_theResourceDB->m_alphaToIndex[item])) ;

		break;

	case DATABASE_BUILDINGS:

		return g_theStringDB->GetNameStr(g_theBuildingDB->GetName(
				g_theBuildingDB->m_alphaToIndex[item])) ;

		break;

	case DATABASE_WONDERS:

		return g_theStringDB->GetNameStr(g_theWonderDB->GetName(
				g_theWonderDB->m_alphaToIndex[item])) ;

		break;

	case DATABASE_ADVANCES:

		return g_theStringDB->GetNameStr(g_theAdvanceDB->GetName(
				g_theAdvanceDB->m_alphaToIndex[item])) ;

		break;

	case DATABASE_TERRAIN:

		return g_theStringDB->GetNameStr(g_theTerrainDB->GetName(
				g_theTerrainDB->m_alphaToIndex[item])) ;

		break;

	case DATABASE_CONCEPTS:

		return g_theConceptDB->GetNameStr(
				g_theConceptDB->m_alphaToIndex[item]) ;

		break;

	case DATABASE_GOVERNMENTS:

		return g_theStringDB->GetNameStr(g_theGovernmentDB->GetName(
				g_theGovernmentDB->m_alphaToIndex[item])) ;
		break;

	case DATABASE_TILE_IMPROVEMENTS:

		return g_theStringDB->GetNameStr(g_theTerrainImprovementDB->GetName(
				g_theTerrainImprovementDB->m_alphaToIndex[item])) ;
		break;

	default:
		return NULL;
	};

}











const MBCHAR * GreatLibrary::GetSelectionName()
{ 

	
	return GetItemName(m_database, m_selectedIndex);

}




ctp2_Window *GreatLibrary::GetWindow( void )
{
	return m_window->m_window;
}







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

	default:
		return;

	}

}





sint32 GreatLibrary::HandleIndexButton( ctp2_Button *button )
{


	DATABASE newDatabase;

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
	m_selectedIndex = -1;
	m_topics_list->ShouldDraw();
	UpdateList( newDatabase );
	
	return 0;
}








sint32 GreatLibrary::HandleListButton
( 
	aui_Control *control, 
	uint32 action, 
	uint32 data, 
	void *cookie 
)
{

	
	int index;

	switch ( action ) {
	case AUI_LISTBOX_ACTION_SELECT:

		ctp2_ListItem *item;

		
		item = (ctp2_ListItem *)m_topics_list->GetSelectedItem();
		if (!item) 
			return 0;

		
		index = (int) item->GetUserData();

		
		SetLibrary( index, m_listDatabase );


		break;

	case AUI_LISTBOX_ACTION_DOUBLECLICKSELECT:
		break;
	}

	return 0;
}











sint32 GreatLibrary::UpdateList( DATABASE database ) 
{
	sint32 index;

	
	m_topics_list->Clear();
	m_listDatabase = database;

	switch ( database ) {
	case DATABASE_UNITS:

#define HIDE(db, index) (db->Get(index)->GetGLHidden())

		
		for (index = 0; index < g_theUnitDB->NumRecords(); index++)
		{
			if(HIDE(g_theUnitDB, index))
				continue;

			
			Add_Item_To_Topics_List(g_theStringDB->GetNameStr(g_theUnitDB->GetName(
						g_theUnitDB->m_alphaToIndex[index])), index);

		} 

		break;

	case DATABASE_SEARCH:
		
		
		Search_Great_Library();

		
		for (index = 0; index < m_search_results.size(); index++)
		{
			
			int real_index = m_search_results[index].m_item;
			enum DATABASE real_database = m_search_results[index].m_database;

			
			if(real_database == DATABASE_CONCEPTS)
			{
				real_index = g_theConceptDB->m_indexToAlpha[ real_index ];
			}

			
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
			
			Add_Item_To_Topics_List(g_theStringDB->GetNameStr(g_theResourceDB->GetName(
						g_theResourceDB->m_alphaToIndex[ index ])), index);

		} 

		break;

	case DATABASE_BUILDINGS:
		
		for (index = 0; index < g_theBuildingDB->NumRecords(); index++)
		{
			if(HIDE(g_theBuildingDB, index)) continue;

			
			Add_Item_To_Topics_List(g_theStringDB->GetNameStr(g_theBuildingDB->GetName(
						g_theBuildingDB->m_alphaToIndex[ index ])), index);

		} 

		break;

	case DATABASE_WONDERS:
		
		for (index = 0; index < g_theWonderDB->NumRecords(); index++)
		{
			if(HIDE(g_theWonderDB, index)) continue;

			
			Add_Item_To_Topics_List(g_theStringDB->GetNameStr(g_theWonderDB->GetName(
						g_theWonderDB->m_alphaToIndex[ index ])), index);

		} 

		break;

	case DATABASE_ADVANCES:
		
		for (index = 0; index < g_theAdvanceDB->NumRecords(); index++)
		{
			if(HIDE(g_theAdvanceDB, index)) continue;

			
			Add_Item_To_Topics_List(g_theStringDB->GetNameStr(g_theAdvanceDB->GetName(
						g_theAdvanceDB->m_alphaToIndex[ index ])), index);

		} 

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
		
		for (index = 0; index < g_theConceptDB->GetNumConcepts(); index++)
		{
			
			Add_Item_To_Topics_List(g_theConceptDB->GetNameStr(
							g_theConceptDB->m_alphaToIndex[ index ]), index);

		} 

		break;

	case DATABASE_GOVERNMENTS:
		
		for (index = 0; index < g_theGovernmentDB->NumRecords(); index++)
		{
			if(HIDE(g_theGovernmentDB, index)) continue;

			
			Add_Item_To_Topics_List(g_theStringDB->GetNameStr(g_theGovernmentDB->GetName(
						g_theGovernmentDB->m_alphaToIndex[ index ])), index);

		} 

		break;

	case DATABASE_TILE_IMPROVEMENTS:
		
		for (index = 0; index < g_theTerrainImprovementDB->NumRecords(); index++)
		{
			if(HIDE(g_theTerrainImprovementDB, index)) continue;

			
			Add_Item_To_Topics_List(g_theStringDB->GetNameStr(g_theTerrainImprovementDB->GetName(
						g_theTerrainImprovementDB->m_alphaToIndex[ index ])), index);

		} 

		break;

	default:


		return FALSE;
	}

	m_maxIndex = index;

	return 0;

}










int GreatLibrary::Get_Database_Size(int the_database)
{
	int num = 0;

	switch ( the_database ) {
	case DATABASE_UNITS:
		num = g_theUnitDB->NumRecords();

		break;

	case DATABASE_SEARCH:
		
		num = m_search_results.size();
		break;

	case DATABASE_ORDERS:
		num = g_theOrderDB->NumRecords();

		break;

	case DATABASE_RESOURCE:
		num = g_theResourceDB->NumRecords();

		break;

	case DATABASE_BUILDINGS:
		num = g_theBuildingDB->NumRecords();

		break;

	case DATABASE_WONDERS:
		num = g_theWonderDB->NumRecords();

		break;

	case DATABASE_ADVANCES:
		num = g_theAdvanceDB->NumRecords();

		break;

	case DATABASE_TERRAIN:
		num = g_theTerrainDB->NumRecords();

		break;

	case DATABASE_CONCEPTS:
		num = g_theConceptDB->GetNumConcepts();

		break;

	case DATABASE_GOVERNMENTS:
		num = g_theGovernmentDB->NumRecords();

		break;

	case DATABASE_TILE_IMPROVEMENTS:
		num = g_theTerrainImprovementDB->NumRecords();

		break;

	default:

		
		break;

	}

	return num;
}










void GreatLibrary::Search_Great_Library()
{
	
	const int MAX_SEARCH_KEY = 100;

	
	char search_key[MAX_SEARCH_KEY];

	
	char * gameplay_text = NULL;
	char * historical_text = NULL;

	
	Great_Library_Item current;

	
	bool match = false;

	
	m_search_results.erase( 
		m_search_results.begin(), 
		m_search_results.end()
		);


	
	m_search_word->GetFieldText(search_key, MAX_SEARCH_KEY);

	
	String_Search::Set_Case_Sensitive(false);
	
	
	String_Search::Set_Search_Key(search_key);

	
	for (int i = DATABASE_UNITS; i < DATABASE_SEARCH; i++)
	{
		
		for (int j = 0; j < Get_Database_Size(i); j++)
		{
			
			match = false;

			
			current.m_database = (enum DATABASE) i;
			current.m_item = j;

			
			gameplay_text = m_window->GetGameplayText(i, j);

			
			if (gameplay_text)
				match |= String_Search::Search(gameplay_text);

			
			historical_text = m_window->GetHistoricalText(i, j);

			
			if (historical_text)
				match |= String_Search::Search(historical_text);

			
			if (match)
			{
				
				m_search_results.push_back(current);

			} 

		} 
	} 


}









void GreatLibrary::Force_A_Search()
{
	
	m_listDatabase = DATABASE_SEARCH;

	
	SetCategoryName(m_listDatabase);

	m_page = 0;
	m_selectedIndex = -1;
	m_topics_list->ShouldDraw();

	
	UpdateList( m_listDatabase );

}









void GreatLibrary::Back()
{
	
	enum DATABASE next_database;
	int next_index;

	
	if (m_history_position <= 0)
		return;

	
	m_history_position--;

	
	if (m_history_position == 0)
	{
		
		m_backButton->Enable(FALSE);

	} 

	
	m_forwardButton->Enable(TRUE);

	
	next_database = m_history[m_history_position].m_database;
	next_index = m_history[m_history_position].m_item;

	
	SetLibrary(next_index, next_database, false);
}










void GreatLibrary::Forward()
{
	
	enum DATABASE next_database;
	int next_index;

	
	if (m_history_position >= m_history.size() - 1)
		return;

	
	m_history_position++;

	
	if (m_history_position == m_history.size() - 1)
	{
		
		m_forwardButton->Enable(FALSE);

	} 

	
	m_backButton->Enable(TRUE);

	
	next_database = m_history[m_history_position].m_database;
	next_index = m_history[m_history_position].m_item;

	
	SetLibrary(next_index, next_database, false);
}










void GreatLibrary::Add_Item_To_Topics_List
(
	const MBCHAR *name,
	int index
)
{
	
	void *cookie = (void *) index;
	
	Assert(m_topics_list);
	if(!m_topics_list) return;

	ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("GreatLibraryTopicItem");
	Assert(item);
	if(!item) return;

	ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);

	Assert(box);
	if(!box) return;

	box->SetText(name);
	
	
	item->SetUserData(cookie);

	m_topics_list->AddItem(item);
}





void GreatLibrary::FixTabs()
{
	
	ctp2_Tab *curTab=m_tabGroup->GetCurrentTab();
	m_tabGroup->SelectTab( m_historicalTab );
	m_tabGroup->SelectTab( m_gameplayTab );

	if(curTab->IsDisabled()) {
		m_tabGroup->SelectTab(m_gameplayTab);
	} else {
		m_tabGroup->SelectTab( curTab );
	}
}
