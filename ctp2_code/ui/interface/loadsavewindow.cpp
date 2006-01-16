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
//
//----------------------------------------------------------------------------

#include "c3.h"


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
#include "aui_directsurface.h"

#include "c3ui.h"
#include "c3_button.h"
#include "c3_static.h"
#include "c3_listbox.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "texttab.h"
#include "pointerlist.h"
#include "c3textfield.h"

#include "StrDB.h"
#include "profileDB.h"
#include "TurnCnt.h"

#include "spnewgamewindow.h" 
#include "loadsavewindow.h"


#include "linegraph.h"
#include "infowin.h"


#include "radarmap.h"

#include "pixelutils.h"

#include "TurnYearStatus.h"

extern C3UI							*g_c3ui;
extern StringDB						*g_theStringDB;
extern ProfileDB					*g_theProfileDB;
extern TurnCount					*g_turn;

#include "player.h"
extern Player						**g_player;


#include "colorset.h"
extern ColorSet						*g_colorSet;

extern LoadSaveWindow				*g_loadsaveWindow;

extern sint32						g_is565Format;

#include "SelItem.h"
extern SelectedItem					*g_selected_item;

#define k_LOADSAVE_AUTOSORT_COL		-2










LoadSaveWindow::LoadSaveWindow(AUI_ERRCODE *retval, uint32 id,
		MBCHAR *ldlBlock, sint32 bpp, AUI_WINDOW_TYPE type, bool bevel)
		: c3_PopupWindow(retval,id,ldlBlock,bpp,type,bevel)
{
	m_fileList = NULL;
	m_gameInfo = NULL;
	m_saveInfo = NULL;
	m_saveInfoRemember = NULL;
	m_saveInfoToSave = NULL;

	sprintf( m_mostRecentName, "" );

	
	m_type = LSS_TOTAL;

	InitCommonLdl(ldlBlock);
}


AUI_ERRCODE LoadSaveWindow::InitCommonLdl(MBCHAR *ldlBlock)
{
	MBCHAR			tabGroupBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			tabBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];

	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	

	sprintf( block, "%s.%s", ldlBlock, "Name" );
	AddTitle( block );

	AddOk( loadsavescreen_executePress, NULL, "c3_PopupOk" );
	AddCancel( loadsavescreen_backPress );

	
	Ok()->SetText( g_theStringDB->GetNameStr("str_ldl_CAPS_OK") );


	m_deleteButton = spNew_ctp2_Button(
		&errcode,
		ldlBlock,
		"DeleteButton",
		loadsavescreen_deletePress );

	m_nameString = spNewStringTable(&errcode, "LSSStringTable");
	Assert(m_nameString);
	if (!m_nameString) return AUI_ERRCODE_LOADFAILED;

	
	sprintf(block, "%s.%s", ldlBlock, "TitlePanel");
	m_titlePanel = new c3_Static(&errcode, aui_UniqueId(), block);
	Assert(m_titlePanel);
	if (!m_titlePanel) return AUI_ERRCODE_LOADFAILED;

	m_gameText = spNew_c3_Static(&errcode, block, "GameText");
	Assert(m_gameText);
	if (!m_gameText) return AUI_ERRCODE_LOADFAILED;

	m_gameTextBox = spNewTextEntry(&errcode, block, "GameTextBox");
	Assert(m_gameTextBox);
	if (!m_gameTextBox) return AUI_ERRCODE_LOADFAILED;
	m_gameTextBox->SetIsFileName(TRUE);

	m_saveText = spNew_c3_Static(&errcode, block, "SaveText");
	Assert(m_saveText);
	if (!m_saveText) return AUI_ERRCODE_LOADFAILED;

	m_saveTextBox = spNewTextEntry(&errcode, block, "SaveTextBox");
	Assert(m_saveTextBox);
	if (!m_saveTextBox) return AUI_ERRCODE_LOADFAILED;
	m_saveTextBox->SetIsFileName(TRUE);

	m_noteText = spNew_c3_Static(&errcode, block, "NoteText");
	Assert(m_noteText);
	if (!m_noteText) return AUI_ERRCODE_LOADFAILED;

	m_noteTextBox = spNewTextEntry(&errcode, block, "NoteTextBox");
	Assert(m_noteTextBox);
	if (!m_noteTextBox) return AUI_ERRCODE_LOADFAILED;

	m_playerText = spNew_c3_Static(&errcode, block, "PlayerText");
	Assert(m_playerText);
	if (!m_playerText) return AUI_ERRCODE_LOADFAILED;

	m_civText = spNew_c3_Static(&errcode, block, "CivText");
	Assert(m_civText);
	if (!m_civText) return AUI_ERRCODE_LOADFAILED;

	
	m_listOne = spNew_c3_ListBox(&errcode, ldlBlock, "ListOne", 
									loadsavescreen_ListOneHandler, (void *)this);
	Assert(m_listOne);
	if (!m_listOne) return AUI_ERRCODE_LOADFAILED;

	
	m_listTwo = spNew_c3_ListBox(&errcode, ldlBlock, "ListTwo", 
									loadsavescreen_ListTwoHandler, (void *)this);
	Assert(m_listTwo);
	if (!m_listTwo) return AUI_ERRCODE_LOADFAILED;

	
	sprintf(tabGroupBlock, "%s.%s", ldlBlock, "LoadTabGroup" );
	m_tabGroup = new aui_TabGroup( &errcode, aui_UniqueId(), tabGroupBlock );
	Assert( AUI_NEWOK(m_tabGroup, errcode) );
	if (!m_tabGroup) return AUI_ERRCODE_LOADFAILED;

	
	m_tabGroup->SetDrawMask( k_AUI_REGION_DRAWFLAG_UPDATE );

	
	sprintf(tabBlock, "%s.%s", tabGroupBlock, "InfoTab");
	m_powerTab = new TextTab(&errcode, aui_UniqueId(), tabBlock, NULL);
	Assert( AUI_NEWOK(m_powerTab, errcode) );
	if ( !AUI_NEWOK(m_powerTab, errcode) ) return AUI_ERRCODE_LOADFAILED;

	sprintf(block, "%s.pane.%s", tabBlock, "InfoImage");
	m_powerTabImage = new c3_Static(&errcode, aui_UniqueId(), block);
	Assert(m_powerTabImage);
	if (!m_powerTabImage) return AUI_ERRCODE_LOADFAILED;

	
	
	
	m_powerTabImageBackup = new aui_Image(
		&errcode, m_powerTabImage->GetImage()->GetFilename() );
	Assert(m_powerTabImageBackup);
	if (!m_powerTabImageBackup) return AUI_ERRCODE_LOADFAILED;

	
	m_powerTabImageBackup->Load();

	
	sprintf(tabBlock, "%s.%s", tabGroupBlock, "MapTab");
	m_mapTab = new TextTab(&errcode, aui_UniqueId(), tabBlock, NULL);
	Assert( AUI_NEWOK(m_mapTab, errcode) );
	if ( !AUI_NEWOK(m_mapTab, errcode) ) return AUI_ERRCODE_LOADFAILED;

	sprintf(block, "%s.pane.%s", tabBlock, "MapImage");
	m_mapTabImage = new c3_Static(&errcode, aui_UniqueId(), block);
	Assert(m_mapTabImage);
	if (!m_mapTabImage) return AUI_ERRCODE_LOADFAILED;

	
	
	
	m_mapTabImageBackup = new aui_Image(
		&errcode, m_mapTabImage->GetImage()->GetFilename() );
	Assert(m_mapTabImageBackup);
	if (!m_mapTabImageBackup) return AUI_ERRCODE_LOADFAILED;

	
	m_mapTabImageBackup->Load();

	
	sprintf(tabBlock, "%s.%s", tabGroupBlock, "CivsTab");
	m_civsTab = new TextTab(&errcode, aui_UniqueId(), tabBlock, NULL);
	Assert( AUI_NEWOK(m_civsTab, errcode) );
	if ( !AUI_NEWOK(m_civsTab, errcode) ) return AUI_ERRCODE_LOADFAILED;

	
	m_civsList = spNew_c3_ListBox(&errcode, tabGroupBlock, "CivsTab.pane.CivsList", 
									loadsavescreen_CivListHandler, (void *)this);
	Assert(m_civsList);
	if (!m_civsList) return AUI_ERRCODE_LOADFAILED;

	
	m_civsList->GetHeader()->Enable( FALSE );

	


	return AUI_ERRCODE_OK;
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
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	if (!m_fileList) return;

	if(m_listOne)
		m_listOne->Clear();  

	if (m_fileList->GetCount() <= 0) return;

	if (m_listOne == NULL) return;

	PointerList<GameInfo>::Walker *walker = new PointerList<GameInfo>::Walker(m_fileList);

	
	while (walker->IsValid()) {
		LSGamesListItem *item = new LSGamesListItem(
			&errcode,
			"LSGamesListItem", 
			walker->GetObj());
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;

		m_listOne->AddItem(item);

		walker->Next();
	}

	delete walker;

}



void LoadSaveWindow::FillListTwo(GameInfo *info)
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	Assert(m_listTwo);
	if (m_listTwo == NULL) return;

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

	if ( info )
	{
		PointerList<SaveInfo>::Walker *walker = new PointerList<SaveInfo>::Walker(info->files);

		while (walker->IsValid()) {
			LSSavesListItem *item = new LSSavesListItem(
				&errcode,
				"LSSavesListItem",
				walker->GetObj());
			Assert(errcode == AUI_ERRCODE_OK);
			if (errcode != AUI_ERRCODE_OK) return;

			m_listTwo->AddItem(item);

			walker->Next();
		}

		delete walker;
	}






}



void LoadSaveWindow::FillCivList(SaveInfo *info)
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	Assert(m_civsList);
	if (m_civsList == NULL) return;

	m_civsList->Clear();

	if ( info )
	{
		for (sint32 i=0; i<info->numCivs; i++) {
			LSCivsListItem *item = new LSCivsListItem(&errcode, "LSCivsListItem", 
					info->civList[i]);
			Assert(errcode == AUI_ERRCODE_OK);
			if (errcode != AUI_ERRCODE_OK) return;

			m_civsList->AddItem(item);
		}
	}
}


void LoadSaveWindow::SelectCurrentGame(void)
{
	if (!m_listOne) return;

	MBCHAR		currentGame[k_MAX_NAME_LEN] = {0};

	GetGameName(currentGame);

	sint32				foundItem = -1;
	LSGamesListItem		*item;
	GameInfo			*info;

	if (m_listOne->NumItems() <= 0) {
		return;
	}

	for (sint32 i=0; i<m_listOne->NumItems(); i++) {
		item = (LSGamesListItem *)m_listOne->GetItemByIndex(i);
		if (!item) continue;

		info = item->GetGameInfo();
		if (!info) continue;

		if (!strcmp(info->name, currentGame)) {
			foundItem = i;
			break;
		}
	}

	if (foundItem == -1) {
		foundItem = 0;
	} else {
		m_listOne->SelectItem(foundItem);
	}

}


void LoadSaveWindow::SelectCurrentSave(void)
{
	if (!m_listTwo) return;

	MBCHAR		saveName[_MAX_PATH] = {0};
	GetSaveName(saveName);

	sint32				foundItem = -1;
	LSSavesListItem		*item;
	SaveInfo			*info;

	if (m_listTwo->NumItems() <= 0) {
		return;
	}

	for (sint32 i=0; i<m_listTwo->NumItems(); i++) {
		item = (LSSavesListItem *)m_listTwo->GetItemByIndex(i);
		if (!item) continue;

		info = item->GetSaveInfo();
		if (!info) continue;

		if (!strcmp(info->fileName, saveName)) {
			foundItem = i;
			break;
		}
	}

	if (foundItem == -1) {
		foundItem = 0;
	} else {
		m_listTwo->SelectItem(foundItem);
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


BOOL LoadSaveWindow::CreateSaveInfoIfNeeded( SaveInfo *&info )
{
	if ( info == NULL) {
		info = new SaveInfo();

		GameFile::GetExtendedInfoFromProfile(info);
		GetPowerGraph(info);
		GetRadarMap(info);
		SetPowerGraph(info);
		SetRadarMap(info);

		m_tabGroup->ShouldDraw(TRUE);


		sint32 i;		
		sint32 numCivs = 0;
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

		
		for (i=0; i<k_MAX_PLAYERS; i++) {
			CIV_INDEX		civ = (CIV_INDEX)0;
			if (g_player[i]) {
				civ = g_player[i]->GetCivilisation()->GetCivilisation();
			}

			info->playerCivIndexList[i] = civ;
		}

		
		return TRUE;
	}

	
	return FALSE;
}


void LoadSaveWindow::CleanUpSaveInfo( void )
{
	if ( m_saveInfoToSave )
	{
		delete m_saveInfoToSave;
		m_saveInfoToSave = NULL;
	}

	if ( m_saveInfoRemember )
	{
		delete m_saveInfoRemember;
		m_saveInfoRemember = NULL;
	}

	m_saveInfo = NULL;
	m_gameInfo = NULL;
}






void LoadSaveWindow::GetPowerGraph(SaveInfo *info)
{
	double		**graphData;
	sint32		xCount, yCount;
	LineGraph	*myGraph;
	AUI_ERRCODE	errcode;
	sint32		width, height;

	width = m_powerTabImage->Width();
	height = m_powerTabImage->Height();

	
	myGraph = new LineGraph(&errcode, aui_UniqueId(), 0, 0, width, height);
	if (!myGraph) return;

	myGraph->EnableYLabel(FALSE);
	myGraph->EnableYNumber(FALSE);
	myGraph->EnablePrecision(FALSE);

	
	graphData = NULL;
	if (infowin_UpdateGraph(myGraph, xCount, yCount, &graphData)) return;

	
	aui_Surface	*surf = myGraph->GetGraphSurface();

	if (info->powerGraphWidth > 0 && 
		info->powerGraphHeight > 0 && 
		info->powerGraphData != NULL) {
		delete[] info->powerGraphData;
	}

	info->powerGraphWidth = width;
	info->powerGraphHeight = height;
	info->powerGraphData = new Pixel16[width*height];

	sint32		i;
	Pixel16		*buffer, *bufferDataPtr, *graphDataPtr;
	sint32		pitch;

	graphDataPtr = info->powerGraphData;

	if (surf->Lock(NULL, (LPVOID *)&buffer, 0) != AUI_ERRCODE_OK) return;
	pitch = surf->Pitch();

	
	for (i=0; i<height; i++) {
		bufferDataPtr = buffer + i * (pitch/2);
		memcpy(graphDataPtr, bufferDataPtr, width * sizeof(Pixel16));
		graphDataPtr += width;
	}

	surf->Unlock(buffer);

	
	delete myGraph;

	
	if (graphData) {
		for (i=0; i<yCount; i++) {
			if (graphData[i])
				delete[] graphData[i];
		}
		delete[] graphData;
	}
}



void LoadSaveWindow::GetRadarMap(SaveInfo *info)
{
	RadarMap	*radarMap;
	AUI_ERRCODE	errcode;
	sint32		width, height;

	width = m_powerTabImage->Width();
	height = m_powerTabImage->Height();

	radarMap = new RadarMap(&errcode, aui_UniqueId(), 0, 0, width, height, m_pattern->GetFilename());
	if (!radarMap) return;

	
	aui_Surface	*surf = (aui_Surface *)radarMap->GetMapSurface();

	radarMap->RenderMap(surf);	

	if (info->radarMapWidth > 0 && 
		info->radarMapHeight > 0 && 
		info->radarMapData != NULL) {
		delete[] info->radarMapData;
	}

	
	info->radarMapWidth = width;
	info->radarMapHeight = height;
	info->radarMapData = new Pixel16[width*height];

	sint32		i;
	Pixel16		*buffer, *bufferDataPtr, *radarDataPtr;
	sint32		pitch;

	radarDataPtr = info->radarMapData;

	if (surf->Lock(NULL, (LPVOID *)&buffer, 0) != AUI_ERRCODE_OK) return;
	pitch = surf->Pitch();

	
	for (i=0; i<height; i++) {
		bufferDataPtr = buffer + i * (pitch/2);
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
	
	aui_Image		*image = m_powerTabImage->GetImage();

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

	

	aui_Surface		*surface = image->TheSurface();

	sint32		i;
	Pixel16		*buffer, *bufferDataPtr, *radarDataPtr;
	sint32		pitch;

	radarDataPtr = info->powerGraphData;

	if (surface->Lock(NULL, (LPVOID *)&buffer, 0) != AUI_ERRCODE_OK) {
		delete surface;
		delete image;
		return;
	}

	pitch = surface->Pitch();

	
	for (i=0; i<height; i++) {
		bufferDataPtr = buffer + i * (pitch/2);
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

	

	aui_Surface		*surface = image->TheSurface();

	sint32		i;
	Pixel16		*buffer, *bufferDataPtr, *radarDataPtr;
	sint32		pitch;

	radarDataPtr = info->radarMapData;

	if (surface->Lock(NULL, (LPVOID *)&buffer, 0) != AUI_ERRCODE_OK) {
		delete surface;
		delete image;
		return;
	}

	pitch = surface->Pitch();

	
	for (i=0; i<height; i++) {
		bufferDataPtr = buffer + i * (pitch/2);
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

	if (info != NULL) {
		SetGameName(info->name);
		FillListTwo(info);
	}
	else
	{
		SetGameName("");
		FillListTwo(NULL);
	}
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

	MBCHAR		saveName[_MAX_PATH];
	MBCHAR		theGameName[_MAX_PATH];
	MBCHAR		theYear[k_MAX_NAME_LEN];

	
	g_player[g_selected_item->GetVisiblePlayer()]->m_civilisation->GetSingularCivName(civName);
#if !defined(_JAPANESE)
	civName[4] = 0;
#endif

	
	
	c3files_StripSpaces(civName);

	
	if (g_useCustomYear && g_pTurnLengthOverride)
	{
		uint32 round = NewTurnCount::GetCurrentRound();
		if (round > g_turnLengthOverrideSize)
		{
			round = g_turnLengthOverrideSize;
		}

		strcpy(theYear,g_pTurnLengthOverride[round].text);
	}
	else
	{
		
		sint32	year = g_turn->GetYear();
		sint32	yearStringIndex;
		AUI_ERRCODE			errcode;
		aui_StringTable		*table;

		table = new aui_StringTable( &errcode, "YearStrings" );
		Assert(table);
		if (!table) return;

		if (year < 0) {
			yearStringIndex = 0;
		} else {
			yearStringIndex = 1;
		}
		sprintf(theYear, "%ld%s", abs(year), table->GetString(yearStringIndex));
		delete table;
	}



















	if (gameName == NULL) {
		
		
		
		
		
		
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
	theGameName[6] = '\0';
#endif	
	
	
	
	c3files_StripSpaces(theGameName);

	if (gameName == NULL)
		SetGameName(theGameName);

		if (g_startInfoType == STARTINFOTYPE_CIVS ||
			g_startInfoType == STARTINFOTYPE_POSITIONSFIXED) {
			MBCHAR tempName[k_MAX_NAME_LEN];
			strcpy(tempName, g_theProfileDB->GetLeaderName());
#if !defined(_JAPANESE)
			tempName[6] = '\0';
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


BOOL LoadSaveWindow::NoName( void )
{
	MBCHAR s[_MAX_PATH];
	
	m_saveTextBox->GetFieldText( s, _MAX_PATH );

	if ( !strcmp(s, "") ) return TRUE;

	return FALSE;
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
	LSCivsListItem *item = (LSCivsListItem *)item2;

	return 0;
}



LSGamesListItem::LSGamesListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock, GameInfo *info)
:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock),
	m_itemIcon(NULL),
	m_itemText(NULL)
{
	m_info = info;

	m_itemIcon = spNew_c3_Static(retval, ldlBlock, "GamesIcon");
	if(m_itemIcon) {
		
		AddChild(m_itemIcon);
	}

	m_itemText = spNew_c3_Static(retval, ldlBlock, "GamesText");
	if(m_itemText) {
		


		
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
	LSGamesListItem *item = (LSGamesListItem *)item2;

	return 0;
}



LSSavesListItem::LSSavesListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock, SaveInfo *info)
:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock),
	m_itemIcon(NULL),
	m_itemText(NULL)
{
	m_info = info;

	m_itemIcon = spNew_c3_Static(retval, ldlBlock, "SavesIcon");
	if(m_itemIcon) {
		
		AddChild(m_itemIcon);
	}

	m_itemText = spNew_c3_Static(retval, ldlBlock, "SavesText");
	if(m_itemText) {
		


		
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

		
		if (info->isScenario) {
			m_itemText->SetTextColor(g_colorSet->GetColorRef(COLOR_DARK_GREEN));
		} else {
			if (info->startInfoType != STARTINFOTYPE_NONE) {
				m_itemText->SetTextColor(g_colorSet->GetColorRef(COLOR_BLUE));
			}
		}

		
		if (strstr(name, g_theStringDB->GetNameStr("AUTOSAVE_NAME"))) {
			m_itemText->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
		}

		
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

