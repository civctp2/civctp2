
#include "c3.h"

#include "aui.h"
#include "aui_control.h"
#include "aui_static.h"

#include "pattern.h"
#include "primitives.h"
#include "greatlibrarywindow.h"
#include "greatlibrary.h"




#include "XY_Coordinates.h"
#include "World.h"
#include "StrDB.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "AdvanceRecord.h"
#include "TerrainImprovementRecord.h"

#include "TerrainRecord.h"
#include "ConceptDB.h"
#include "GovernmentRecord.h"
#include "prjfile.h"




#include "c3ui.h"
#include "aui_TabGroup.h"
#include "directvideo.h"
#include "CivPaths.h"
#include "ctp2_hypertextbox.h"
#include "ctp2_static.h"
#include "ctp2_window.h"

#include "aui_ldl.h"

#include "ColorSet.h"
#include "textutils.h"
#include "debugwindow.h"
#include "SoundManager.h"

#include "UnitRecord.h"
#include "IconRecord.h"
#include "IconRecord.h"
#include "TerrainRecord.h"

#include "wonderutil.h"


#include "Text_Hasher.h"


#include "ResourceRecord.h"


#include "OrderRecord.h"

#include "stringutils.h"



const int GreatLibraryWindow::GREAT_LIBRARY_PANEL_BLANK = 999;

extern ColorSet						*g_colorSet;
extern CivPaths						*g_civPaths;
extern sint32						g_ScreenWidth;
extern sint32						g_ScreenHeight;
extern DebugWindow					*g_debugWindow;
extern ConceptDB					*g_theConceptDB;
extern ProjectFile                  *g_GreatLibPF;
extern SoundManager					*g_soundManager;

static MBCHAR *s_libraryWindowBlock = "GreatLibrary";
static GreatLibraryWindow	*s_libraryWindow;

extern  C3UI				*g_c3ui;






















GreatLibraryWindow::GreatLibraryWindow(AUI_ERRCODE *err)
{



	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_libraryWindowBlock);
	Assert(m_window);
	if(!m_window) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}














}

GreatLibraryWindow::~GreatLibraryWindow()
{
	aui_Ldl::DeleteHierarchyFromRoot(s_libraryWindowBlock);
}
































































































AUI_ERRCODE GreatLibraryWindow::Idle ( void )
{
	if ( !m_techMovie ) return AUI_ERRCODE_OK;

	if ( m_techMovie->Open() ) {
		HRESULT hr;
		hr = m_techMovie->PlayOne();
		Assert(!FAILED(hr));
		if (FAILED(hr)) {
			m_techMovie->CloseStream();
			delete m_techMovie;
			m_techMovie = NULL;
		}
	}

	return AUI_ERRCODE_OK;
}

sint32 GreatLibraryWindow::LoadText(ctp2_HyperTextBox *textbox, char *filename, SlicObject &so)
{
    char *text;


    if (textbox == NULL)
        return(1);



	
	char * lower_case_filename = new char[strlen(filename)+1];
	strcpy(lower_case_filename, filename);

	
	for (int j = 0; j < strlen(lower_case_filename); j++)
		lower_case_filename[j] = tolower(lower_case_filename[j]);

	
	text = GreatLibrary::m_great_library_info->Look_Up_Data(lower_case_filename);



    if (text == NULL) {
		textbox->SetHyperText(" ");
        return(0);
    }

	MBCHAR interpreted[k_MAX_GL_ENTRY];

	stringutils_Interpret(text, so, interpreted);

	

	
	


    






    textbox->SetHyperText(interpreted);



    return(1);
}

sint32 GreatLibraryWindow::LoadHistoricalText ( SlicObject &so )
{
	
	if (!strcmp(m_history_file, "NULL"))
		return GREAT_LIBRARY_PANEL_BLANK;

    return(LoadText(m_techHistoricalText, m_history_file, so));
}

sint32 GreatLibraryWindow::LoadGameplayText ( SlicObject &so )
{
	
	if (!strcmp(m_gameplay_file, "NULL"))
		return GREAT_LIBRARY_PANEL_BLANK;

    return(LoadText(m_techGameplayText, m_gameplay_file, so));
}

sint32 GreatLibraryWindow::LoadRequirementsText ( SlicObject &so )
{
    return(LoadText(m_techRequirementsText, m_requirement_file, so));

}

sint32 GreatLibraryWindow::LoadVariablesText ( SlicObject &so )
{
    return(LoadText(m_techVariablesText, m_variable_file, so));

}

sint32 GreatLibraryWindow::LoadTechMovie ( void )
{
	MBCHAR fullPath[256];

	
	if (!m_techMovie) return 0;
	if (!strcmp(m_movie_file,"null")) return 0;

	
	
	if (g_civPaths->FindFile(C3DIR_VIDEOS, m_movie_file, fullPath, TRUE)) {
		
		g_soundManager->ReleaseSoundDriver();

		m_techMovie->OpenStream(fullPath);

		g_soundManager->ReacquireSoundDriver();
	} else {
		return 0;
	}


	return 1;

}

sint32 GreatLibraryWindow::LoadTechStill( void )
{
	MBCHAR fullPath[256];

	
	if ( !m_techStillShot ) return 0;
	if ( !strcmp(m_still_file, "null") ) return 0;

	
	
	if ( g_civPaths->FindFile(C3DIR_PICTURES, m_still_file, fullPath, TRUE) ) {
		m_techStillShot->SetImage( m_still_file );
	}
	else {
		return 0;
	}

	return 1;
}


void GreatLibraryWindow::PlayTechMovie ( void )
{
	m_techMovie->PlayAll();
}

sint32 GreatLibraryWindow::SetTechMode ( sint32 theMode, DATABASE theDatabase )
{
	m_mode = theMode;
	m_database = theDatabase;

	const IconRecord *iconRec = NULL;

	
	switch ( theDatabase ) {
	case DATABASE_UNITS:
		iconRec = g_theUnitDB->Get(theMode)->GetDefaultIcon();
		break;

	case DATABASE_ORDERS:
		iconRec = g_theOrderDB->Get(theMode)->GetDefaultIcon();
		break;

	case DATABASE_RESOURCE:
		iconRec = g_theResourceDB->Get(theMode)->GetIcon();
		break;

	case DATABASE_BUILDINGS:
		iconRec = g_theBuildingDB->Get(theMode)->GetDefaultIcon();
		break;

	case DATABASE_WONDERS:
		iconRec = g_theWonderDB->Get(theMode)->GetDefaultIcon();
		break;

	case DATABASE_ADVANCES:
	{
		const IconRecord *rec = g_theAdvanceDB->Get(theMode)->GetIcon();

		sprintf( m_still_file, rec->GetIcon());
		sprintf( m_movie_file, rec->GetMovie());
		sprintf( m_gameplay_file, rec->GetGameplay());
		sprintf( m_history_file, rec->GetHistorical());
		sprintf( m_requirement_file, rec->GetPrereq());
		sprintf( m_variable_file, rec->GetVari());

		break;
	}
	case DATABASE_TERRAIN:
	{
		iconRec = g_theTerrainDB->Get(theMode)->GetIcon();
		break;
	}
	case DATABASE_CONCEPTS:
		iconRec = g_theIconDB->Get(g_theConceptDB->GetConceptInfo(theMode)->m_iconDBIndex);
		break;

	case DATABASE_GOVERNMENTS:
		iconRec = g_theGovernmentDB->Get(theMode)->GetIcon();
		break;

	case DATABASE_TILE_IMPROVEMENTS:
		iconRec = g_theTerrainImprovementDB->Get(theMode)->GetIcon();
		break;

	default:
		BOOL InvalidDatabase = FALSE;
		Assert(InvalidDatabase);
		break;
	}

	if(iconRec) {
		sprintf( m_still_file, iconRec->GetFirstFrame());
		sprintf( m_movie_file, iconRec->GetMovie());
		sprintf( m_gameplay_file, iconRec->GetGameplay());
		sprintf( m_history_file, iconRec->GetHistorical());
		sprintf( m_requirement_file, iconRec->GetPrereq());
		sprintf( m_variable_file, iconRec->GetVari());
	}

	return TRUE;

}



char * GreatLibraryWindow::GetIconRecText
( 
	int database, 
	int item,
	bool historical
)
{
	char * the_text = NULL;

	const IconRecord *iconRec = NULL;

	
	switch ( database ) {
	case DATABASE_UNITS:
		iconRec = g_theUnitDB->Get(item)->GetDefaultIcon();
		break;

	case DATABASE_ORDERS:
		iconRec = g_theOrderDB->Get(item)->GetDefaultIcon();
		break;

	case DATABASE_RESOURCE:
		iconRec = g_theResourceDB->Get(item)->GetIcon();
		break;

	case DATABASE_BUILDINGS:
		iconRec = g_theBuildingDB->Get(item)->GetDefaultIcon();
		break;

	case DATABASE_WONDERS:
		iconRec = g_theWonderDB->Get(item)->GetDefaultIcon();
		break;

	case DATABASE_ADVANCES:
	{
		iconRec = g_theAdvanceDB->Get(item)->GetIcon();
		break;
	}
	case DATABASE_TERRAIN:
	{
		iconRec = g_theTerrainDB->Get(item)->GetIcon();
		break;
	}
	case DATABASE_CONCEPTS:
		iconRec = g_theIconDB->Get(g_theConceptDB->GetConceptInfo(item)->m_iconDBIndex);
		break;

	case DATABASE_GOVERNMENTS:
		iconRec = g_theGovernmentDB->Get(item)->GetIcon();
		break;

	case DATABASE_TILE_IMPROVEMENTS:
		iconRec = g_theTerrainImprovementDB->Get(item)->GetIcon();
		break;

	default:
		BOOL InvalidDatabase = FALSE;
		Assert(InvalidDatabase);
		break;
	}

	if(iconRec) 
	{
		
		char * lower_case_filename;

		
		if (historical)
		{
			lower_case_filename = new char[strlen(iconRec->GetHistorical())+1];

			if (lower_case_filename)
				strcpy(lower_case_filename, iconRec->GetHistorical());
		}
		else
		{
			lower_case_filename = new char[strlen(iconRec->GetGameplay())+1];

			if (lower_case_filename)
				strcpy(lower_case_filename, iconRec->GetGameplay());
		}

		
		if (!lower_case_filename)
			return NULL;

		
		for (int j = 0; j < strlen(lower_case_filename); j++)
			lower_case_filename[j] = tolower(lower_case_filename[j]);

		
		the_text = GreatLibrary::m_great_library_info->Look_Up_Data(lower_case_filename);

		
		if (lower_case_filename)
			delete [] lower_case_filename;
	}

	return the_text;
}



char * GreatLibraryWindow::GetHistoricalText( int database, int item )
{
	return GetIconRecText( database, item, true );
}



char * GreatLibraryWindow::GetGameplayText( int database, int item )
{
	return GetIconRecText( database, item, false );
}

