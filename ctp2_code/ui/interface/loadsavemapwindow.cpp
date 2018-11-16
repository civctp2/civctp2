//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The load/save map window
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
#include "c3textfield.h"
#include "texttab.h"
#include "pointerlist.h"

#include "StrDB.h"
#include "profileDB.h"

#include "spnewgamewindow.h"
#include "loadsavemapwindow.h"

#include "pixelutils.h"

#include "radarmap.h"

extern C3UI							*g_c3ui;
extern StringDB						*g_theStringDB;
extern ProfileDB					*g_theProfileDB;

extern LoadSaveMapWindow			*g_loadSaveMapWindow;

extern sint32						g_is565Format;










LoadSaveMapWindow::LoadSaveMapWindow(AUI_ERRCODE *retval, uint32 id,
		MBCHAR *ldlBlock, sint32 bpp, AUI_WINDOW_TYPE type, bool bevel)
		: c3_PopupWindow(retval,id,ldlBlock,bpp,type,bevel)
{
	m_fileList = NULL;
	m_gameMapInfo = NULL;
	m_saveMapInfo = NULL;
	m_saveMapInfoRemember = NULL;
	m_saveMapInfoToSave = NULL;

	m_type = LSMS_TOTAL;

	InitCommonLdl(ldlBlock);
}

AUI_ERRCODE LoadSaveMapWindow::InitCommonLdl(MBCHAR *ldlBlock)
{
	MBCHAR			tabGroupBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			tabBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];

	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;


	sprintf( block, "%s.%s", ldlBlock, "Name" );
	AddTitle( block );

	AddOk( loadsavemapscreen_executePress, NULL, "c3_PopupOk" );
	AddCancel( loadsavemapscreen_backPress );

	m_deleteButton = spNew_c3_Button(
		&errcode,
		ldlBlock,
		"DeleteButton",
		loadsavemapscreen_deletePress );

	m_nameString = spNewStringTable(&errcode, "LSMSStringTable");
	Assert(m_nameString);
	if (!m_nameString) return AUI_ERRCODE_LOADFAILED;

	sprintf(block, "%s.%s", ldlBlock, "TitlePanel");
	m_titlePanel = new c3_Static(&errcode, aui_UniqueId(), block);
	Assert(m_titlePanel);
	if (!m_titlePanel) return AUI_ERRCODE_LOADFAILED;

	m_gameMapText = spNew_c3_Static(&errcode, block, "GameMapText");
	Assert(m_gameMapText);
	if (!m_gameMapText) return AUI_ERRCODE_LOADFAILED;

	m_gameMapTextBox = spNewTextEntry(&errcode, block, "GameMapTextBox");
	Assert(m_gameMapTextBox);
	if (!m_gameMapTextBox) return AUI_ERRCODE_LOADFAILED;
	m_gameMapTextBox->SetIsFileName(TRUE);

	m_saveMapText = spNew_c3_Static(&errcode, block, "SaveMapText");
	Assert(m_saveMapText);
	if (!m_saveMapText) return AUI_ERRCODE_LOADFAILED;

	m_saveMapTextBox = spNewTextEntry(&errcode, block, "SaveMapTextBox");
	Assert(m_saveMapTextBox);
	if (!m_saveMapTextBox) return AUI_ERRCODE_LOADFAILED;
	m_saveMapTextBox->SetIsFileName(TRUE);

	m_noteText = spNew_c3_Static(&errcode, block, "NoteText");
	Assert(m_noteText);
	if (!m_noteText) return AUI_ERRCODE_LOADFAILED;

	m_noteTextBox = spNewTextEntry(&errcode, block, "NoteTextBox");
	Assert(m_noteTextBox);
	if (!m_noteTextBox) return AUI_ERRCODE_LOADFAILED;

	m_listOne = spNew_c3_ListBox(&errcode, ldlBlock, "ListOne",
									loadsavemapscreen_ListOneHandler, (void *)this);
	Assert(m_listOne);
	if (!m_listOne) return AUI_ERRCODE_LOADFAILED;

	m_listTwo = spNew_c3_ListBox(&errcode, ldlBlock, "ListTwo",
									loadsavemapscreen_ListTwoHandler, (void *)this);
	Assert(m_listTwo);
	if (!m_listTwo) return AUI_ERRCODE_LOADFAILED;

	sprintf(tabGroupBlock, "%s.%s", ldlBlock, "LoadTabGroup" );
	m_tabGroup = new aui_TabGroup( &errcode, aui_UniqueId(), tabGroupBlock );
	Assert( AUI_NEWOK(m_tabGroup, errcode) );
	if (!m_tabGroup) return AUI_ERRCODE_LOADFAILED;

	m_tabGroup->SetDrawMask( k_AUI_REGION_DRAWFLAG_UPDATE );

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




	return AUI_ERRCODE_OK;
}

LoadSaveMapWindow::~LoadSaveMapWindow()
{
	CleanUpSaveMapInfo();

#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	mycleanup(m_nameString);

	mycleanup(m_titlePanel);
	mycleanup(m_gameMapText);
	mycleanup(m_gameMapTextBox);
	mycleanup(m_saveMapText);
	mycleanup(m_saveMapTextBox);
	mycleanup(m_noteText);
	mycleanup(m_noteTextBox);

	mycleanup(m_listOne);
	mycleanup(m_listTwo);

	mycleanup(m_tabGroup);
	mycleanup(m_mapTab);
	mycleanup(m_mapTabImage);
	mycleanup(m_mapTabImageBackup);

	mycleanup(m_deleteButton);

#undef mycleanup
}


void LoadSaveMapWindow::FillListOne(void)
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	if (!m_fileList) return;
	if (m_fileList->GetCount() <= 0) return;

	if (m_listOne == NULL) return;

	m_listOne->Clear();

	PointerList<GameMapInfo>::Walker *walker = new PointerList<GameMapInfo>::Walker(m_fileList);

	while (walker->IsValid()) {
		LSMGameMapsListItem *item = new LSMGameMapsListItem(
			&errcode,
			"LSMGameMapsListItem",
			walker->GetObj());
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;

		m_listOne->AddItem(item);

		walker->Next();
	}

	delete walker;
}


void LoadSaveMapWindow::FillListTwo(GameMapInfo *info)
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;

	Assert(m_listTwo);
	if (m_listTwo == NULL) return;

	m_listTwo->Clear();

	switch ( m_type )
	{
	case LSMS_LOAD_GAMEMAP:
		Ok()->Enable(FALSE);
		break;

	default:
		Ok()->Enable(TRUE);
		break;
	}

	SetSaveMapInfo(NULL);

	if ( info )
	{
		PointerList<SaveMapInfo>::Walker *walker = new PointerList<SaveMapInfo>::Walker(info->files);

		while (walker->IsValid()) {
			LSMSaveMapsListItem *item = new LSMSaveMapsListItem(
				&errcode,
				"LSMSaveMapsListItem",
				walker->GetObj());
			Assert(errcode == AUI_ERRCODE_OK);
			if (errcode != AUI_ERRCODE_OK) return;

			m_listTwo->AddItem(item);

			walker->Next();
		}

		delete walker;
	}
}

void LoadSaveMapWindow::SelectCurrentGameMap(void)
{
	if (!m_listOne) return;

	MBCHAR		currentGameMap[k_MAX_NAME_LEN] = {0};

	GetGameMapName(currentGameMap);

	sint32				foundItem = -1;
	LSMGameMapsListItem		*item;
	GameMapInfo			*info;

	if (m_listOne->NumItems() <= 0) return;

	for (sint32 i=0; i<m_listOne->NumItems(); i++) {
		item = (LSMGameMapsListItem *)m_listOne->GetItemByIndex(i);
		if (!item) continue;

		info = item->GetGameMapInfo();
		if (!info) continue;

		if (!strcmp(info->name, currentGameMap)) {
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

void LoadSaveMapWindow::SelectCurrentSaveMap(void)
{
	if (!m_listTwo) return;

	MBCHAR		saveMapName[_MAX_PATH] = {0};
	GetSaveMapName(saveMapName);

	sint32				foundItem = -1;
	LSMSaveMapsListItem		*item;
	SaveMapInfo			*info;

	if (m_listTwo->NumItems() <= 0) return;

	for (sint32 i=0; i<m_listTwo->NumItems(); i++) {
		item = (LSMSaveMapsListItem *)m_listTwo->GetItemByIndex(i);
		if (!item) continue;

		info = item->GetSaveMapInfo();
		if (!info) continue;

		if (!strcmp(info->fileName, saveMapName)) {
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




void LoadSaveMapWindow::SetType(uint32 type)
{

	m_type = type;

	if((m_type>=LSMS_FIRST) && (m_type<LSMS_TOTAL)) {
		TitleText()->SetText(m_nameString->GetString(m_type));
	} else
		Assert(0);

	switch (m_type)
	{
	case LSMS_LOAD_GAMEMAP:
		EnableFields( FALSE );
		break;

	default:
		EnableFields( TRUE );
		break;
	}

	m_fileList = GameMapFile::BuildSaveMapList(C3SAVEDIR_MAP);

	FillListOne();

	SelectCurrentGameMap();
	SelectCurrentSaveMap();

	if (m_type == LSMS_SAVE_GAMEMAP) {

		if ( CreateSaveMapInfoIfNeeded( m_saveMapInfoRemember ) )
			m_saveMapInfo = m_saveMapInfoRemember;


		CreateSaveMapInfoIfNeeded( m_saveMapInfoToSave );

		BuildDefaultSaveMapName(
			m_gameMapInfo ? m_gameMapInfo->name : NULL,
			m_saveMapInfoToSave->fileName);
	} else {

	}
}

BOOL LoadSaveMapWindow::CreateSaveMapInfoIfNeeded( SaveMapInfo *&info )
{
	if ( info == NULL) {
		info = new SaveMapInfo();

		GameMapFile::GetExtendedInfoFromProfile(info);
		GetRadarMap(info);
		SetRadarMap(info);

		m_tabGroup->ShouldDraw(TRUE);

		return TRUE;
	}

	return FALSE;
}

void LoadSaveMapWindow::CleanUpSaveMapInfo( void )
{
	if ( m_saveMapInfoToSave )
	{
		delete m_saveMapInfoToSave;
		m_saveMapInfoToSave = NULL;
	}

	if ( m_saveMapInfoRemember )
	{
		delete m_saveMapInfoRemember;
		m_saveMapInfoRemember = NULL;
	}

	m_saveMapInfo = NULL;
	m_gameMapInfo = NULL;
}


void LoadSaveMapWindow::GetRadarMap(SaveMapInfo *info)
{
	RadarMap	*radarMap;
	AUI_ERRCODE	errcode;
	sint32		width, height;

	width = m_mapTabImage->Width();
	height = m_mapTabImage->Height();

	radarMap = new RadarMap(&errcode, aui_UniqueId(), 0, 0, width, height, m_pattern->GetFilename());
	if (!radarMap) return;

	aui_Surface	*surf = (aui_Surface *)radarMap->GetMapSurface();

	radarMap->RenderMap(surf);

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


void LoadSaveMapWindow::SetRadarMap(SaveMapInfo *info)
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

void LoadSaveMapWindow::SetGameMapName(MBCHAR *name)
{
	if (!m_gameMapTextBox) return;
	m_gameMapTextBox->SetFieldText(name);
}

void LoadSaveMapWindow::SetSaveMapName(MBCHAR *name)
{
	if (!m_saveMapTextBox) return;
	m_saveMapTextBox->SetFieldText(name);
}

void LoadSaveMapWindow::SetNote(MBCHAR *note)
{
	if (!m_noteTextBox) return;
	m_noteTextBox->SetFieldText(note);
}





BOOL LoadSaveMapWindow::GetGameMapName(MBCHAR *name)
{
	Assert(m_gameMapTextBox);
	if (!m_gameMapTextBox) return FALSE;

	m_gameMapTextBox->GetFieldText(name, _MAX_PATH);

	return TRUE;
}

BOOL LoadSaveMapWindow::GetSaveMapName(MBCHAR *name)
{
	Assert(m_saveMapTextBox);
	if (!m_saveMapTextBox) return FALSE;

	m_saveMapTextBox->GetFieldText(name, _MAX_PATH);

	return TRUE;
}

BOOL LoadSaveMapWindow::GetNote(MBCHAR *note)
{
	Assert(m_noteTextBox);
	if (!m_noteTextBox) return FALSE;

	m_noteTextBox->GetFieldText(note, _MAX_PATH);

	return TRUE;
}

void LoadSaveMapWindow::SetGameMapInfo(GameMapInfo *info)
{
	m_gameMapInfo = info;

	if (info != NULL) {
		SetGameMapName(info->name);
		FillListTwo(info);
	}
	else
	{
		SetGameMapName("");
		FillListTwo(NULL);
	}
}

void LoadSaveMapWindow::SetSaveMapInfo(SaveMapInfo *info)
{
	m_saveMapInfo = info;

	if (info != NULL) {
		SetSaveMapName(info->fileName);
		SetNote(info->note);

		SetRadarMap(info);
	}
	else
	{
		switch ( m_type )
		{
		case LSMS_LOAD_GAMEMAP:
			SetSaveMapName("");
			SetNote("");

			SetRadarMap(NULL);
			break;

		default:
			CreateSaveMapInfoIfNeeded( m_saveMapInfoToSave );
			SetSaveMapName(m_saveMapInfoToSave->fileName);
			SetNote(m_saveMapInfoToSave->note);

			SetRadarMap(m_saveMapInfoToSave);
			break;
		}
	}

	m_tabGroup->ShouldDraw(TRUE);
}

void LoadSaveMapWindow::BuildDefaultSaveMapName(MBCHAR *gameMapName, MBCHAR *name)
{
	MBCHAR		saveMapName[_MAX_PATH];
	MBCHAR		theGameMapName[_MAX_PATH];

	if (gameMapName == NULL) {
		strcpy(theGameMapName, g_theProfileDB->GetLeaderName());
	} else {
		strcpy(theGameMapName, gameMapName);
	}

	theGameMapName[6] = '\0';

	if (gameMapName == NULL)
		SetGameMapName(theGameMapName);

	sprintf(saveMapName, "%s", theGameMapName);


	strcpy(name, saveMapName);

	SetSaveMapName(saveMapName);
}

void LoadSaveMapWindow::EnableFields( BOOL enable )
{
	m_gameMapTextBox->Enable( enable );
	m_saveMapTextBox->Enable( enable );
	m_noteTextBox->Enable( enable );
}










LSMGameMapsListItem::LSMGameMapsListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock, GameMapInfo *info)
:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock),
	m_itemIcon(NULL),
	m_itemText(NULL)
{
	m_info = info;

	m_itemIcon = spNew_c3_Static(retval, ldlBlock, "GameMapsIcon");
	if(m_itemIcon) {

		AddChild(m_itemIcon);
	}

	m_itemText = spNew_c3_Static(retval, ldlBlock, "GameMapsText");
	if(m_itemText) {

		m_itemText->SetText(info->name);

		m_itemText->Resize(Width()-m_itemIcon->Width()-5, Height());
		m_itemText->Move(m_itemIcon->Width()+5, m_itemText->Y());

		m_itemIcon->AddChild(m_itemText);
	}
}

LSMGameMapsListItem::~LSMGameMapsListItem()
{

}

sint32 LSMGameMapsListItem::Compare(c3_ListItem *item2, uint32 column)
{
	LSMGameMapsListItem *item = (LSMGameMapsListItem *)item2;

	return 0;
}


LSMSaveMapsListItem::LSMSaveMapsListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock, SaveMapInfo *info)
:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock),
	m_itemIcon(NULL),
	m_itemText(NULL)
{
	m_info = info;

	m_itemIcon = spNew_c3_Static(retval, ldlBlock, "SaveMapsIcon");
	if(m_itemIcon) {

		AddChild(m_itemIcon);
	}

	m_itemText = spNew_c3_Static(retval, ldlBlock, "SaveMapsText");
	if(m_itemText) {

		m_itemText->SetText(info->fileName);

		m_itemText->Resize(Width()-m_itemIcon->Width()-5, Height());
		m_itemText->Move(m_itemIcon->Width()+5, m_itemText->Y());

		m_itemIcon->AddChild(m_itemText);
	}
}

LSMSaveMapsListItem::~LSMSaveMapsListItem()
{

}

sint32 LSMSaveMapsListItem::Compare(c3_ListItem *item2, uint32 column)
{
	LSMSaveMapsListItem *item = (LSMSaveMapsListItem *)item2;

	return 0;
}
