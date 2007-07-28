//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Save and load game window
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
// - Adds special modifications for a japanese version of the executable
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Repaired memory leak.
// - Removed refferences to the civilisation database. (Aug 20th 2005 Martin Gühmann)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Standartized code (May 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "loadsavewindow.h"

#include "aui.h"
#include "aui_ldl.h"
#include "aui_control.h"
#include "aui_surface.h"
#include "aui_uniqueid.h"
#include "aui_imagebase.h"
#include "aui_textbase.h"
#include "aui_textfield.h"
#include "aui_stringtable.h"
#include "aui_tabgroup.h"
#include "c3ui.h"
#include "c3_button.h"
#include "c3_static.h"
#include "c3_listbox.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "texttab.h"
#include "pointerlist.h"
#include "c3textfield.h"
#include "colorset.h"               // g_colorSet
#include "player.h"                 // g_player
#include "StrDB.h"                  // g_theStringDB
#include "profileDB.h"              // g_theProfileDB
#include "TurnCnt.h"                // g_turn
#include "spnewgamewindow.h" 
#include "linegraph.h"
#include "infowin.h"
#include "radarmap.h"
#include "pixelutils.h"
#include "SelItem.h"                // g_selected_item
#include "TurnYearStatus.h"

extern C3UI							*g_c3ui;
extern sint32						g_is565Format;


#define k_LOADSAVE_AUTOSORT_COL		-2










LoadSaveWindow::LoadSaveWindow(AUI_ERRCODE *retval, uint32 id,
		MBCHAR *ldlBlock, sint32 bpp, AUI_WINDOW_TYPE type, bool bevel)
: 
    c3_PopupWindow              (retval, id, ldlBlock, bpp, type, bevel),
    m_type                      (LSS_TOTAL),
	m_nameString                (NULL),
	m_gameInfo                  (NULL),
	m_saveInfo                  (NULL),
	m_saveInfoRemember          (NULL),
    m_saveInfoToSave            (NULL),
	m_fileList                  (NULL),
	m_titlePanel                (NULL),
	m_gameText                  (NULL),
    m_gameTextBox               (NULL),
    m_saveText                  (NULL),
    m_saveTextBox               (NULL),
    m_noteText                  (NULL),
    m_noteTextBox               (NULL),
    m_playerText                (NULL),
    m_civText                   (NULL),
    m_listOne                   (NULL),
    m_listTwo                   (NULL),
    m_tabGroup                  (NULL),
    m_powerTab                  (NULL),
    m_powerTabImage             (NULL),
    m_powerTabImageBackup       (NULL),
    m_mapTab                    (NULL),
    m_mapTabImage               (NULL),
    m_mapTabImageBackup         (NULL),
    m_civsTab                   (NULL),
    m_civsList                  (NULL),
    m_deleteButton              (NULL)
	// MBCHAR m_mostRecentName[_MAX_PATH]
{
	sprintf(m_mostRecentName, "");

    MBCHAR  block[k_AUI_LDL_MAXBLOCK + 1];
	sprintf(block, "%s.%s", ldlBlock, "Name");
	AddTitle(block);
	AddCancel(loadsavescreen_backPress);
	AddOk(loadsavescreen_executePress, NULL, "c3_PopupOk");
	Ok()->SetText(g_theStringDB->GetNameStr("str_ldl_CAPS_OK"));

	m_deleteButton = spNew_ctp2_Button(
		retval,
		ldlBlock,
		"DeleteButton",
		loadsavescreen_deletePress );

	m_nameString = spNewStringTable(retval, "LSSStringTable");
	
	sprintf(block, "%s.%s", ldlBlock, "TitlePanel");
	m_titlePanel = new c3_Static(retval, aui_UniqueId(), block);

	m_gameText = spNew_c3_Static(retval, block, "GameText");

	m_gameTextBox = spNewTextEntry(retval, block, "GameTextBox");
	m_gameTextBox->SetIsFileName(TRUE);

	m_saveText = spNew_c3_Static(retval, block, "SaveText");

	m_saveTextBox = spNewTextEntry(retval, block, "SaveTextBox");
	m_saveTextBox->SetIsFileName(TRUE);

	m_noteText = spNew_c3_Static(retval, block, "NoteText");

	m_noteTextBox = spNewTextEntry(retval, block, "NoteTextBox");
	Assert(m_noteTextBox);

	m_playerText = spNew_c3_Static(retval, block, "PlayerText");

	m_civText = spNew_c3_Static(retval, block, "CivText");
	
	m_listOne = spNew_c3_ListBox(retval, ldlBlock, "ListOne", 
									loadsavescreen_ListOneHandler, (void *)this);
	
	m_listTwo = spNew_c3_ListBox(retval, ldlBlock, "ListTwo", 
									loadsavescreen_ListTwoHandler, (void *)this);

	MBCHAR			tabGroupBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf(tabGroupBlock, "%s.%s", ldlBlock, "LoadTabGroup" );

	m_tabGroup = new aui_TabGroup(retval, aui_UniqueId(), tabGroupBlock );
	m_tabGroup->SetDrawMask( k_AUI_REGION_DRAWFLAG_UPDATE );

	MBCHAR			tabBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf(tabBlock, "%s.%s", tabGroupBlock, "InfoTab");

	m_powerTab = new TextTab(retval, aui_UniqueId(), tabBlock, NULL);

	sprintf(block, "%s.pane.%s", tabBlock, "InfoImage");
	m_powerTabImage = new c3_Static(retval, aui_UniqueId(), block);
	
	m_powerTabImageBackup = new aui_Image(retval, m_powerTabImage->GetImage()->GetFilename());
	m_powerTabImageBackup->Load();

	
	sprintf(tabBlock, "%s.%s", tabGroupBlock, "MapTab");
	m_mapTab = new TextTab(retval, aui_UniqueId(), tabBlock, NULL);

	sprintf(block, "%s.pane.%s", tabBlock, "MapImage");
	m_mapTabImage = new c3_Static(retval, aui_UniqueId(), block);
	
	m_mapTabImageBackup = new aui_Image(retval, m_mapTabImage->GetImage()->GetFilename());
	m_mapTabImageBackup->Load();

	sprintf(tabBlock, "%s.%s", tabGroupBlock, "CivsTab");
	m_civsTab = new TextTab(retval, aui_UniqueId(), tabBlock, NULL);
	
	m_civsList = spNew_c3_ListBox(retval, tabGroupBlock, "CivsTab.pane.CivsList", 
									loadsavescreen_CivListHandler, (void *)this);
	m_civsList->GetHeader()->Enable( FALSE );
}

//----------------------------------------------------------------------------
//
// Name       : LoadSaveWindow::~LoadSaveWindow
//
// Description: Destructor
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
LoadSaveWindow::~LoadSaveWindow()
{
	CleanUpSaveInfo();

	if (m_fileList)
	{
		m_fileList->DeleteAll();
	}

	delete m_fileList;
	delete m_nameString;
	delete m_titlePanel;
	delete m_gameText;
	delete m_gameTextBox;
	delete m_saveText;
	delete m_saveTextBox;
	delete m_noteText;
	delete m_noteTextBox;
	delete m_playerText;
	delete m_civText;
	delete m_listOne;
	delete m_listTwo;
	delete m_tabGroup;
	delete m_powerTab;
	delete m_powerTabImage;
	delete m_powerTabImageBackup;
	delete m_mapTab;
	delete m_mapTabImage;
	delete m_mapTabImageBackup;
	delete m_civsTab;
	delete m_civsList;
	delete m_deleteButton;
}


void LoadSaveWindow::FillListOne(void)
{
	if (!m_fileList) return;
	if (m_fileList->GetCount() <= 0) return;
    if (!m_listOne) return;

    m_listOne->Clear();  

	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

    for 
    (
	    PointerList<GameInfo>::Walker walker = PointerList<GameInfo>::Walker(m_fileList);
        walker.IsValid();
        walker.Next()
    ) 
    {
		m_listOne->AddItem
            (new LSGamesListItem(&errcode, "LSGamesListItem", walker.GetObj()));
	}
}



void LoadSaveWindow::FillListTwo(GameInfo *info)
{
	Assert(m_listTwo);
	if (!m_listTwo) return;

	m_listTwo->Clear();

	switch ( m_type )
	{
	case LSS_LOAD_GAME:
	case LSS_LOAD_MP:
	case LSS_LOAD_SCEN:
	case LSS_LOAD_SCEN_MP:
		Ok()->Enable(FALSE);
		break;

	default:
		Ok()->Enable(TRUE);
		break;
	}

	SetSaveInfo(NULL);

	if (info)
	{
    	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

        for
        (
		    PointerList<SaveInfo>::Walker walker = PointerList<SaveInfo>::Walker(info->files);
            walker.IsValid();
            walker.Next()
        )
        {
			m_listTwo->AddItem
                (new LSSavesListItem(&errcode, "LSSavesListItem", walker.GetObj()));
		}
	}
}



void LoadSaveWindow::FillCivList(SaveInfo *info)
{
	Assert(m_civsList);
	if (!m_civsList) return;

	m_civsList->Clear();

	if (info)
	{
    	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

		for (int i = 0; i < info->numCivs; i++) 
        {
			m_civsList->AddItem 
                (new LSCivsListItem(&errcode, "LSCivsListItem", info->civList[i]));
		}
	}
}


void LoadSaveWindow::SelectCurrentGame(void)
{
	if (!m_listOne) return;

    MBCHAR		currentGame[k_MAX_NAME_LEN] = {0};
	if (GetGameName(currentGame))
    {
        for (sint32 i = 0; i < m_listOne->NumItems(); i++) 
        {
	        LSGamesListItem	*   item = (LSGamesListItem *) m_listOne->GetItemByIndex(i);
            GameInfo *          info = item ? item->GetGameInfo() : NULL;

	        if (info && (0 == strcmp(info->name, currentGame))) 
            {
		        m_listOne->SelectItem(i);
		        return;
	        }
        }
    }
}


void LoadSaveWindow::SelectCurrentSave(void)
{
	if (!m_listTwo) return;

	MBCHAR		saveName[_MAX_PATH] = {0};
	if (GetSaveName(saveName))
    {
	    for (sint32 i = 0; i < m_listTwo->NumItems(); i++) 
        {
		    LSSavesListItem	*   item = (LSSavesListItem *) m_listTwo->GetItemByIndex(i);
            SaveInfo *          info = item ? item->GetSaveInfo() : NULL;

		    if (info && (0 == strcmp(info->fileName, saveName))) 
            {
			    m_listTwo->SelectItem(i);
			    return;
		    }
	    }
    }
}




void LoadSaveWindow::SetType(uint32 type)
{
	
	m_type = type;

	if((m_type>=LSS_FIRST) && (m_type<LSS_TOTAL)) {
		TitleText()->SetText(m_nameString->GetString(m_type));		
	} else
		Assert(0); 

	switch (m_type)
	{
	case LSS_LOAD_GAME:
	case LSS_LOAD_MP :
	case LSS_LOAD_SCEN :
	case LSS_LOAD_SCEN_MP:
		EnableFields( FALSE );
		break;

	default:
		EnableFields( TRUE );
		break;
	}

	switch (m_type) {
	case LSS_LOAD_GAME :
	case LSS_SAVE_GAME :
		
		if (m_fileList) {
			m_fileList->DeleteAll();
			FillListOne(); 
		}
		delete m_fileList;
		m_fileList = GameFile::BuildSaveList(C3SAVEDIR_GAME);
		m_gameInfo = NULL; 
		break;
	case LSS_LOAD_MP :
	case LSS_SAVE_MP :
		
		if (m_fileList) {
			m_fileList->DeleteAll();
		}
		delete m_fileList;
		m_fileList = GameFile::BuildSaveList(C3SAVEDIR_MP);
		m_gameInfo = NULL; 
		break;
	case LSS_LOAD_SCEN :
	case LSS_SAVE_SCEN :
	case LSS_LOAD_SCEN_MP :
		
		if (m_fileList) {
			m_fileList->DeleteAll();
		}
		delete m_fileList;
		m_fileList = GameFile::BuildSaveList(C3SAVEDIR_SCEN);
		m_gameInfo = NULL; 
		break;
	}

	
	FillListOne();

	if ((m_type == LSS_SAVE_GAME || m_type == LSS_SAVE_MP || m_type == LSS_SAVE_SCEN)) {
		
		if ( CreateSaveInfoIfNeeded( m_saveInfoRemember ) )
			m_saveInfo = m_saveInfoRemember;

		
		
		CreateSaveInfoIfNeeded( m_saveInfoToSave );

		BuildDefaultSaveName(
			m_gameInfo ? m_gameInfo->name : NULL,
			m_saveInfoToSave->fileName);

		FillCivList(m_saveInfoToSave);
	} else {
		
		if ( m_listTwo->NumItems() > 0 && m_listTwo->GetSelectedItem() ) {
			Ok()->Enable( TRUE );
		}
		else {
			Ok()->Enable( FALSE );
		}
	}

	SelectCurrentGame();
	SelectCurrentSave();

}


bool LoadSaveWindow::CreateSaveInfoIfNeeded( SaveInfo *&info )
{
	if ( info == NULL) {
		info = new SaveInfo();

		GameFile::GetExtendedInfoFromProfile(info);
		GetPowerGraph(info);
		GetRadarMap(info);
		SetPowerGraph(info);
		SetRadarMap(info);

		m_tabGroup->ShouldDraw(TRUE);

		
		sint32 numCivs = 0;
		sint32 i;
		for (i=1; i<k_MAX_PLAYERS; i++) {
			
			MBCHAR			s[_MAX_PATH];
			PLAYER_INDEX	currentCiv = g_selected_item->GetVisiblePlayer();

			if ((g_player[i]) && (!g_player[i]->IsDead()) && (i!=currentCiv)) {
				if (g_player[currentCiv]->HasContactWith(i)) {
					g_player[i]->GetSingularCivName(s) ;

					strcpy(info->civList[numCivs], s);
					numCivs++;
				}
			}
		}
		info->numCivs = numCivs;

		
		for (sint32 j = 0; j < k_MAX_PLAYERS; ++j) 
		{
			info->playerCivIndexList[j] = 
				(g_player[j]) 
				? g_player[j]->GetCivilisation()->GetCivilisation() 
				: (CIV_INDEX) 0;
		}

		return true;
	}

	return false;
}


void LoadSaveWindow::CleanUpSaveInfo( void )
{
	delete m_saveInfoToSave;
	m_saveInfoToSave = NULL;

    delete m_saveInfoRemember;
	m_saveInfoRemember = NULL;

	m_saveInfo = NULL;
	m_gameInfo = NULL;
}






void LoadSaveWindow::GetPowerGraph(SaveInfo *info)
{
	AUI_ERRCODE	    errcode = AUI_ERRCODE_OK;
	sint32 const    width   = m_powerTabImage->Width();
	sint32 const    height  = m_powerTabImage->Height();
	LineGraph *     myGraph = new LineGraph(&errcode, aui_UniqueId(), 0, 0, width, height);

	myGraph->EnableYLabel(FALSE);
	myGraph->EnableYNumber(FALSE);
	myGraph->EnablePrecision(FALSE);

	double **   graphData = NULL;
	sint32		xCount, yCount;
	if (infowin_UpdateGraph(myGraph, xCount, yCount, &graphData)) return;

	if (info->powerGraphWidth > 0 && 
		info->powerGraphHeight > 0) 
    {
		delete [] info->powerGraphData;
	}

	info->powerGraphWidth   = width;
	info->powerGraphHeight  = height;
	info->powerGraphData    = new Pixel16[width*height];

	aui_Surface *   surf = myGraph->GetGraphSurface();
	Pixel16	*       buffer;
	if (surf->Lock(NULL, (LPVOID *)&buffer, 0) != AUI_ERRCODE_OK) return;

	Pixel16 *   graphDataPtr    = info->powerGraphData;
	sint32      halfPitch       = surf->Pitch() / 2;

	for (sint32 i = 0; i < height; i++) 
    {
		Pixel16 *   bufferDataPtr = buffer + i * halfPitch;
		memcpy(graphDataPtr, bufferDataPtr, width * sizeof(Pixel16));
		graphDataPtr += width;
	}

	surf->Unlock(buffer);

	
	delete myGraph;

    if (graphData) 
    {
		for (int i = 0; i < yCount; i++) 
        {
			delete [] graphData[i];
		}
		delete [] graphData;
	}
}



void LoadSaveWindow::GetRadarMap(SaveInfo *info)
{
	AUI_ERRCODE	    errcode     = AUI_ERRCODE_OK;
	sint32 const    width       = m_powerTabImage->Width();
	sint32 const    height      = m_powerTabImage->Height();
	RadarMap *      radarMap    = 
        new RadarMap(&errcode, aui_UniqueId(), 0, 0, width, height, m_pattern->GetFilename());
	aui_Surface	*   surf        = radarMap->GetMapSurface();
	radarMap->RenderMap(surf);	

	if (info->radarMapWidth > 0 && 
		info->radarMapHeight > 0) 
    {
		delete [] info->radarMapData;
	}

	info->radarMapWidth     = width;
	info->radarMapHeight    = height;
	info->radarMapData      = new Pixel16[width*height];

    Pixel16 *   buffer;
	if (surf->Lock(NULL, (LPVOID *)&buffer, 0) != AUI_ERRCODE_OK) return;

	sint32      halfPitch       = surf->Pitch() / 2;
	Pixel16 *   radarDataPtr    = info->radarMapData;

	for (sint32 i = 0; i < height; i++) 
    {
		Pixel16 * bufferDataPtr = buffer + i * halfPitch;
		memcpy(radarDataPtr, bufferDataPtr, width * sizeof(Pixel16));

		
		if (!g_is565Format) {
			for (sint32 j=0; j<width; j++) {
				radarDataPtr[j] = pixelutils_Convert555to565(radarDataPtr[j]);
			}
		}

		radarDataPtr += width;
	}

	surf->Unlock(buffer);

	delete radarMap;
}



void LoadSaveWindow::SetPowerGraph(SaveInfo *info)
{
	aui_Image * image = m_powerTabImage->GetImage();
	Assert( image != NULL );
	if (image == NULL) return;

	RECT rect =
	{
		0,
		0, 
		m_powerTabImageBackup->TheSurface()->Width(),
		m_powerTabImageBackup->TheSurface()->Height()
	};

	if ( !info )
	{
		
		g_c3ui->TheBlitter()->Blt(
			m_powerTabImage->GetImage()->TheSurface(),
			0, 0,
			m_powerTabImageBackup->TheSurface(),
			&rect,
			k_AUI_BLITTER_FLAG_COPY );

		m_powerTabImage->ShouldDraw();

		return;
	}

	sint32 height = info->powerGraphHeight;
	sint32 width = info->powerGraphWidth;

	Assert( width <= rect.right );
	if ( width > rect.right ) return;
	Assert( height <= rect.bottom );
	if ( height > rect.bottom ) return;

	

	aui_Surface	*   surface = image->TheSurface();
	Pixel16 *       buffer;


	if (surface->Lock(NULL, (LPVOID *)&buffer, 0) != AUI_ERRCODE_OK) {
		delete surface;
		delete image;
		return;
	}

	Pixel16 *   radarDataPtr = info->powerGraphData;
	sint32      pitch = surface->Pitch();

	for (sint32 i = 0; i < height; i++) 
    {
		Pixel16 * bufferDataPtr = buffer + i * (pitch/2);
		memcpy(bufferDataPtr, radarDataPtr, width * sizeof(Pixel16));










		radarDataPtr += width;
	}

	surface->Unlock(buffer);
}


void LoadSaveWindow::SetRadarMap(SaveInfo *info)
{
	
	aui_Image		*image = m_mapTabImage->GetImage();

	Assert( image != NULL );
	if (image == NULL) return;

	RECT rect =
	{
		0,
		0, 
		m_mapTabImageBackup->TheSurface()->Width(),
		m_mapTabImageBackup->TheSurface()->Height()
	};

	if ( !info )
	{
		
		g_c3ui->TheBlitter()->Blt(
			m_mapTabImage->GetImage()->TheSurface(),
			0, 0,
			m_mapTabImageBackup->TheSurface(),
			&rect,
			k_AUI_BLITTER_FLAG_COPY );

		m_mapTabImage->ShouldDraw();

		return;
	}

	sint32 height = info->radarMapHeight;
	sint32 width = info->radarMapWidth;

	Assert( width <= rect.right );
	if ( width > rect.right ) return;
	Assert( height <= rect.bottom );
	if ( height > rect.bottom ) return;

	

	aui_Surface	*   surface = image->TheSurface();

	Pixel16	*       buffer;
	if (surface->Lock(NULL, (LPVOID *)&buffer, 0) != AUI_ERRCODE_OK) {
		delete surface;
		delete image;
		return;
	}

	Pixel16 *   radarDataPtr = info->radarMapData;
	sint32      pitch = surface->Pitch();

	
	for (sint32 i=0; i<height; i++) {
		Pixel16 * bufferDataPtr = buffer + i * (pitch/2);
		memcpy(bufferDataPtr, radarDataPtr, width * sizeof(Pixel16));










		radarDataPtr += width;
	}

	surface->Unlock(buffer);
}


void LoadSaveWindow::SetGameName(MBCHAR *name)
{
	if (!m_gameTextBox) return;
	m_gameTextBox->SetFieldText(name);
}


void LoadSaveWindow::SetSaveName(MBCHAR *name)
{
	if (!m_saveTextBox) return;
	m_saveTextBox->SetFieldText(name);
}


void LoadSaveWindow::SetLeaderName(MBCHAR *name)
{
	if (!m_playerText) return;
	m_playerText->SetText(name);
}


void LoadSaveWindow::SetCivName(MBCHAR *name)
{
	if (!m_civText) return;
	m_civText->SetText(name);
}


void LoadSaveWindow::SetNote(MBCHAR *note)
{
	if (!m_noteTextBox) return;
	m_noteTextBox->SetFieldText(note);
}





BOOL LoadSaveWindow::GetGameName(MBCHAR *name)
{
	Assert(m_gameTextBox);
	if (!m_gameTextBox) return FALSE;

	m_gameTextBox->GetFieldText(name, _MAX_PATH);

	return TRUE;
}


BOOL LoadSaveWindow::GetSaveName(MBCHAR *name)
{
	Assert(m_saveTextBox);
	if (!m_saveTextBox) return FALSE;

	m_saveTextBox->GetFieldText(name, _MAX_PATH);

	return TRUE;
}


MBCHAR *LoadSaveWindow::GetLeaderName(void)
{
	Assert(m_playerText);
	if (!m_playerText) return NULL;

	return m_playerText->GetText();
}


MBCHAR *LoadSaveWindow::GetCivName(void)
{
	Assert(m_civText);
	if (!m_civText) return NULL;

	return m_civText->GetText();
}


BOOL LoadSaveWindow::GetNote(MBCHAR *note)
{
	Assert(m_noteTextBox);
	if (!m_noteTextBox) return FALSE;

	m_noteTextBox->GetFieldText(note, _MAX_PATH);

	return TRUE;
}


void LoadSaveWindow::SetGameInfo(GameInfo *info) 
{ 
	m_gameInfo = info; 
    SetGameName(info ? info->name : "");
	FillListTwo(info);
}


void LoadSaveWindow::SetSaveInfo(SaveInfo *info) 
{ 
	m_saveInfo = info; 

	if (info != NULL) {
		SetSaveName(info->fileName);
		SetLeaderName(info->leaderName);
		SetCivName(info->civName);
		SetNote(info->note);
		
		FillCivList(info);

		SetPowerGraph(info);
		SetRadarMap(info);
	}
	else
	{
		switch ( m_type )
		{
		case LSS_LOAD_GAME:
		case LSS_LOAD_MP:
		case LSS_LOAD_SCEN:
		case LSS_LOAD_SCEN_MP:
			SetSaveName("");
			SetLeaderName("");
			SetCivName("");
			SetNote("");

			FillCivList(NULL);

			SetPowerGraph(NULL);
			SetRadarMap(NULL);
			break;

		default:
			CreateSaveInfoIfNeeded( m_saveInfoToSave );
			SetSaveName(m_saveInfoToSave->fileName);
			SetLeaderName(m_saveInfoToSave->leaderName);
			SetCivName(m_saveInfoToSave->civName);
			SetNote(m_saveInfoToSave->note);

			FillCivList(m_saveInfoToSave);

			SetPowerGraph(m_saveInfoToSave);
			SetRadarMap(m_saveInfoToSave);
			break;
		}
	}

	m_tabGroup->ShouldDraw(TRUE);
}



void LoadSaveWindow::BuildDefaultSaveName(MBCHAR *gameName, MBCHAR *name)
{
	MBCHAR		civName[k_MAX_NAME_LEN];
	g_player[g_selected_item->GetVisiblePlayer()]->m_civilisation->GetSingularCivName(civName);
#if !defined(_JAPANESE)
	civName[4] = 0;
#endif
	c3files_StripSpaces(civName);

	const MBCHAR* theYear = TurnYearStatus::GetCurrentYear();

	MBCHAR		theGameName[_MAX_PATH];
	if (gameName == NULL) 
	{
		if (g_startInfoType == STARTINFOTYPE_CIVS ||
			g_startInfoType == STARTINFOTYPE_POSITIONSFIXED) {
			strcpy(theGameName, g_theProfileDB->GetGameName());
		} else {
			strcpy(theGameName, g_theProfileDB->GetLeaderName());
		}
	} else {
		strcpy(theGameName, gameName);
	}
#if !defined(_JAPANESE)
	theGameName[SAVE_LEADER_NAME_SIZE] = '\0';
#endif	
	c3files_StripSpaces(theGameName);

	if (gameName == NULL)
	{
		SetGameName(theGameName);
	}

	MBCHAR		saveName[_MAX_PATH];
	if (g_startInfoType == STARTINFOTYPE_CIVS ||
		g_startInfoType == STARTINFOTYPE_POSITIONSFIXED) {
		MBCHAR tempName[k_MAX_NAME_LEN];
		strcpy(tempName, g_theProfileDB->GetLeaderName());
#if !defined(_JAPANESE)
		tempName[SAVE_LEADER_NAME_SIZE] = '\0';
		c3files_StripSpaces(tempName);
		sprintf(saveName, "%s-%s-%s", tempName, civName, theYear);
	} else {
		sprintf(saveName, "%s-%s-%s", theGameName, civName, theYear);
#else
		c3files_StripSpaces(tempName);
		sprintf(saveName, "%s-%s", theYear, tempName);
	} else {
		sprintf(saveName, "%s-%s", theYear, theGameName);
#endif
	}

	strcpy(name, saveName);

	SetSaveName(saveName);
}


void LoadSaveWindow::EnableFields( BOOL enable )
{
	m_gameTextBox->Enable( enable );
	m_saveTextBox->Enable( enable );
	m_noteTextBox->Enable( enable );
}


bool LoadSaveWindow::NoName( void )
{
	MBCHAR s[_MAX_PATH];
	
	m_saveTextBox->GetFieldText( s, _MAX_PATH );

	return 0 == strcmp(s, "");
}










LSCivsListItem::LSCivsListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock, const MBCHAR *name)
:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock),
	m_myItem(NULL)
{
	
	m_myItem = spNew_c3_Static(retval, ldlBlock, "CivText");
	if(m_myItem) {
		
		MBCHAR tempName[ _MAX_PATH + 1 ];
		strncpy( tempName, name, _MAX_PATH );

		
		if ( !m_myItem->GetTextFont() )
			m_myItem->TextReloadFont();

		m_myItem->GetTextFont()->TruncateString(
			tempName,
			m_myItem->Width() );

		m_myItem->SetText(tempName);
		AddChild(m_myItem);
	}
}

LSCivsListItem::~LSCivsListItem()
{
}

sint32 LSCivsListItem::Compare(c3_ListItem *item2, uint32 column)
{
//	LSCivsListItem *item = (LSCivsListItem *)item2;

	return 0;
}



LSGamesListItem::LSGamesListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock, GameInfo *info)
:
	c3_ListItem     (retval, ldlBlock),
	m_itemIcon      (NULL),
	m_itemText      (NULL),
    m_info          (info)
{
	m_itemIcon = spNew_c3_Static(retval, ldlBlock, "GamesIcon");
	if (m_itemIcon) 
    {
		AddChild(m_itemIcon);
	}

	m_itemText = spNew_c3_Static(retval, ldlBlock, "GamesText");
	if (m_itemText) 
    {
		m_itemText->Resize(Width()-m_itemIcon->Width()-5, Height());
		m_itemText->Move(m_itemIcon->Width()+5, m_itemText->Y());

		
		MBCHAR name[ _MAX_PATH + 1 ];
		strncpy( name, info->name, _MAX_PATH );

		
		if ( !m_itemText->GetTextFont() )
			m_itemText->TextReloadFont();

		m_itemText->GetTextFont()->TruncateString(
			name,
			m_itemText->Width() );

		m_itemText->SetText(name);

		
		m_itemIcon->AddChild(m_itemText);
	}
}

LSGamesListItem::~LSGamesListItem()
{
	delete m_itemText;
	delete m_itemIcon;
	
	m_childList->DeleteAll();
}

sint32 LSGamesListItem::Compare(c3_ListItem *item2, uint32 column)
{
//	LSGamesListItem *item = (LSGamesListItem *)item2;

	return 0;
}



LSSavesListItem::LSSavesListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock, SaveInfo *info)
:
	c3_ListItem (retval, ldlBlock),
	m_itemIcon  (NULL),
	m_itemText  (NULL),
    m_info      (info)
{
	m_itemIcon = spNew_c3_Static(retval, ldlBlock, "SavesIcon");
	if (m_itemIcon) 
    {
		AddChild(m_itemIcon);
	}

	m_itemText = spNew_c3_Static(retval, ldlBlock, "SavesText");
	if (m_itemText) 
    {
		m_itemText->Resize(Width()-m_itemIcon->Width()-5, Height());
		m_itemText->Move(m_itemIcon->Width()+5, m_itemText->Y());

		
		MBCHAR name[ _MAX_PATH + 1 ];
		strncpy( name, info->fileName, _MAX_PATH );

		
		if ( !m_itemText->GetTextFont() )
			m_itemText->TextReloadFont();

		m_itemText->GetTextFont()->TruncateString(
			name,
			m_itemText->Width() );

		m_itemText->SetText(name);

		
		if (strstr(name, g_theStringDB->GetNameStr("AUTOSAVE_NAME"))) 
        {
			m_itemText->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
		}
		else if (info->isScenario) 
        {
			m_itemText->SetTextColor(g_colorSet->GetColorRef(COLOR_DARK_GREEN));
		} 
        else if (info->startInfoType != STARTINFOTYPE_NONE) 
        {
			m_itemText->SetTextColor(g_colorSet->GetColorRef(COLOR_BLUE));
		}
        // else No action: Keep default color

		
		m_itemIcon->AddChild(m_itemText);

	}
}

LSSavesListItem::~LSSavesListItem()
{
	delete m_itemIcon;
	delete m_itemText;

	m_childList->DeleteAll();
}

sint32 LSSavesListItem::Compare(c3_ListItem *item2, uint32 column)
{
	LSSavesListItem *item = (LSSavesListItem *)item2;

	switch (column) {
	case k_LOADSAVE_AUTOSORT_COL:
		
		return strcmp(this->GetText(), item->GetText());
		break;
	}

	return 0;
}

