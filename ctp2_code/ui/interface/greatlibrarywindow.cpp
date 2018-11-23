//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The window of the Great Libary
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
// - Memory leaks repaired in LoadText by Martin Gühmann.
// - Added variable and requirement retriever methods. (Sep 13th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

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
#include "conceptdb.h"
#include "GovernmentRecord.h"
#include "prjfile.h"




#include "c3ui.h"
#include "aui_tabgroup.h"
#include "directvideo.h"
#include "CivPaths.h"
#include "ctp2_hypertextbox.h"
#include "ctp2_Static.h"
#include "ctp2_Window.h"

#include "aui_ldl.h"

#include "colorset.h"
#include "textutils.h"
#include "debugwindow.h"
#include "soundmanager.h"

#include "UnitRecord.h"
#include "IconRecord.h"
#include "IconRecord.h"
#include "TerrainRecord.h"

#include "wonderutil.h"

#include "text_hasher.h"

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

static const MBCHAR *s_libraryWindowBlock = "GreatLibrary";

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
#ifdef WIN32
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
#endif
	return AUI_ERRCODE_OK;
}

//----------------------------------------------------------------------------
//
// Name       : LoadText
//
// Description: Load an interpreted text in a user interface item.
//
// Parameters : textbox					: user interface item
//              filename				: name of the text to interpret
//              so						: ?
//
// Globals    : m_great_library_info	: great library database
//
// Returns    : sint32 (bool)			: function result
//
// Remark(s)  : The function result is 1 (true?) when the requested name of
//              the text exist in the database, 0 when it does not exist.
//
//              However, the result is also 1 (illogical) when the passed
//              user interface item is NULL. In that case, this function does
//              nothing.
//
//----------------------------------------------------------------------------
sint32 GreatLibraryWindow::LoadText(ctp2_HyperTextBox *textbox, char *filename, SlicObject &so)
{
    char *text;

    if (textbox == NULL)
        return(1);




	char * lower_case_filename = new char[strlen(filename)+1];
	strcpy(lower_case_filename, filename);

	for (int j = 0; (unsigned) j < strlen(lower_case_filename); j++)
		lower_case_filename[j] = tolower(lower_case_filename[j]);

	text = GreatLibrary::m_great_library_info->Look_Up_Data(lower_case_filename);

	delete [] lower_case_filename;

    if (text == NULL) {
		textbox->SetHyperText(" ");
        return(0);
    }

	MBCHAR interpreted[k_MAX_GL_ENTRY];
	stringutils_Interpret(text, so, interpreted, k_MAX_GL_ENTRY);












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
#ifdef WIN32
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
#else
	return 0;
#endif
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
#ifdef WIN32
	m_techMovie->PlayAll();
#endif
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
		iconRec = g_theAdvanceDB->Get(theMode)->GetIcon();
		break;

	case DATABASE_TERRAIN:
		theMode = g_theTerrainDB->m_alphaToIndex[ theMode ];
		iconRec = g_theTerrainDB->Get(theMode)->GetIcon();
		break;

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
		Assert(!"InvalidDatabase");
		break;
	}

	if(iconRec) {
		strcpy(m_still_file, iconRec->GetFirstFrame());
		strcpy(m_movie_file, iconRec->GetMovie());
		strcpy(m_gameplay_file, iconRec->GetGameplay());
		strcpy(m_history_file, iconRec->GetHistorical());
		strcpy(m_requirement_file, iconRec->GetPrereq());
		strcpy(m_variable_file, iconRec->GetVari());
	}

	return TRUE;

}


char * GreatLibraryWindow::GetIconRecText
(
	int database,
	int item,
	uint8 historical
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
		Assert(!"InvalidDatabase");
		break;
	}

	if(iconRec)
	{

		char * lower_case_filename;

		switch(historical){
			case 0:
			{
				lower_case_filename = new char[strlen(iconRec->GetGameplay())+1];

				if (lower_case_filename)
					strcpy(lower_case_filename, iconRec->GetGameplay());

				break;
			}
			case 1:
			{
				lower_case_filename = new char[strlen(iconRec->GetHistorical())+1];

				if (lower_case_filename)
					strcpy(lower_case_filename, iconRec->GetHistorical());

				break;
			}
			case 2:
			{
				lower_case_filename = new char[strlen(iconRec->GetPrereq())+1];

				if (lower_case_filename)
					strcpy(lower_case_filename, iconRec->GetPrereq());

				break;
			}
			case 3:
			{
				lower_case_filename = new char[strlen(iconRec->GetVari())+1];

				if (lower_case_filename)
					strcpy(lower_case_filename, iconRec->GetVari());
				break;
			}
			default:
				lower_case_filename = NULL;
				Assert(false);
				break;
		}

		if (!lower_case_filename)
			return NULL;

		for (int j = 0; (unsigned) j < strlen(lower_case_filename); j++)
			lower_case_filename[j] = tolower(lower_case_filename[j]);

		the_text = GreatLibrary::m_great_library_info->Look_Up_Data(lower_case_filename);

		if (lower_case_filename)
			delete [] lower_case_filename;
	}

	return the_text;
}

char * GreatLibraryWindow::GetGameplayText( int database, int item )
{
	return GetIconRecText( database, item, 0 );
}

char * GreatLibraryWindow::GetHistoricalText( int database, int item )
{
	return GetIconRecText( database, item, 1 );
}

char * GreatLibraryWindow::GetRequirementsText( int database, int item )
{
	return GetIconRecText( database, item, 2 );
}

char * GreatLibraryWindow::GetVariablesText( int database, int item )
{
	return GetIconRecText( database, item, 3 );
}
