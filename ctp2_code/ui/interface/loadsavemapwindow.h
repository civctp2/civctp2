//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Load/save map window
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
// _MSC_VER		
// - Compiler version (for the Microsoft C++ compiler only)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Changed loadsavemapscreen_Cleanup so it can function as UiCleanupCallback.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef LOADSAVEMAPWINDOW_FLAG
#define LOADSAVEMAPWINDOW_FLAG

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// #include <>      

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class LoadSaveMapWindow;
class LSMGameMapsListItem;
class LSMSaveMapsListItem;

enum LSMS_TYPE
{ 
	LSMS_FIRST=0, 
	
	LSMS_LOAD_GAMEMAP=0, 
	LSMS_SAVE_GAMEMAP,

	LSMS_TOTAL
};

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "pointerlist.h"
#include "gamefile.h"
#include "c3_popupwindow.h"
#include "c3_listitem.h"

class c3_Static;
class c3_Button;
class aui_TextField;
class C3TextField;
class aui_StringTable;
class TextTab;
class c3_ListBox;
class aui_TabGroup;

//----------------------------------------------------------------------------
// General declarations
//----------------------------------------------------------------------------

sint32 loadsavemapscreen_displayMyWindow(uint32 type);
sint32 loadsavemapscreen_removeMyWindow(uint32 action);
AUI_ERRCODE loadsavemapscreen_Initialize(
	aui_Control::ControlActionCallback *callback = NULL );
void loadsavemapscreen_Cleanup(void);

void loadsavemapscreen_executePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void loadsavemapscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void loadsavemapscreen_deletePress(aui_Control *control, uint32 action, uint32 data, void *cookie );

void loadsavemapscreen_ListOneHandler(aui_Control *control, uint32 action, uint32 data, void *cookie );
void loadsavemapscreen_ListTwoHandler(aui_Control *control, uint32 action, uint32 data, void *cookie );

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class LoadSaveMapWindow : public c3_PopupWindow
{
public:
	LoadSaveMapWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD,
		bool bevel = true);
	
	AUI_ERRCODE InitCommonLdl(MBCHAR *ldlBlock);

	virtual ~LoadSaveMapWindow();

	
	void FillListOne(void);
	void FillListTwo(GameMapInfo *info);

	void GetRadarMap(SaveMapInfo *info);

	void SetRadarMap(SaveMapInfo *info);

	
	void SelectCurrentGameMap(void);
	void SelectCurrentSaveMap(void);

	void SetType(uint32 type);
	uint32 GetType() { return m_type; }

	void SetGameMapName(MBCHAR *name);
	void SetSaveMapName(MBCHAR *name);
	void SetNote(MBCHAR *note);

	BOOL GetGameMapName(MBCHAR *name);
	BOOL GetSaveMapName(MBCHAR *name);
	BOOL GetNote(MBCHAR *note);

	
	void EnableFields( BOOL enable );

	GameMapInfo *GetGameMapInfo(void) const { return m_gameMapInfo; }
	SaveMapInfo *GetSaveMapInfo(void) const { return m_saveMapInfo; }
	void SetGameMapInfo(GameMapInfo *info);
	void SetSaveMapInfo(SaveMapInfo *info);

	
	void CleanUpSaveMapInfo( void );
	SaveMapInfo *GetSaveMapInfoToSave( void ) const { return m_saveMapInfoToSave; }

	void BuildDefaultSaveMapName(MBCHAR *gameMapName, MBCHAR *name);

	C3TextField* GetTextField() { return m_gameMapTextBox; }

	
	c3_Button *GetDeleteButton( void ) { return m_deleteButton; }
	ctp2_Button *GetOkButton( void ) { return Ok(); }
	c3_Static *GetMapStatic( void ) { return m_mapTabImage; }

	c3_ListBox *GetListOne( void ) const { return m_listOne; }
	c3_ListBox *GetListTwo( void ) const { return m_listTwo; }

private:
	
	
	BOOL CreateSaveMapInfoIfNeeded( SaveMapInfo *&info );

	uint32			 m_type;
	aui_StringTable *m_nameString;

	GameMapInfo		*m_gameMapInfo;
	SaveMapInfo		*m_saveMapInfo;

	
	
	SaveMapInfo		*m_saveMapInfoRemember;
	SaveMapInfo		*m_saveMapInfoToSave;

	
	PointerList<GameMapInfo> *m_fileList;

	
	c3_Static		*m_titlePanel;
	
	c3_Static		*m_gameMapText;
	C3TextField		*m_gameMapTextBox;

	c3_Static		*m_saveMapText;
	C3TextField		*m_saveMapTextBox;

	c3_Static		*m_noteText;
	C3TextField		*m_noteTextBox;

	c3_ListBox		*m_listOne;
	c3_ListBox		*m_listTwo;
	
	
	aui_TabGroup	*m_tabGroup;
	
	
	TextTab			*m_mapTab;
	c3_Static		*m_mapTabImage;
	aui_Image		*m_mapTabImageBackup;

	c3_Button		*m_deleteButton;
};




class LSMGameMapsListItem : public c3_ListItem
{
public:
	LSMGameMapsListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock, GameMapInfo *info);
	~LSMGameMapsListItem();

	sint32 Compare(c3_ListItem *item2, uint32 column);

	GameMapInfo	*GetGameMapInfo(void) const { return m_info; }

private:
	c3_Static	*m_itemIcon;
	c3_Static	*m_itemText;
	GameMapInfo	*m_info;

};


class LSMSaveMapsListItem : public c3_ListItem
{
public:
	LSMSaveMapsListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock, SaveMapInfo *info);
	~LSMSaveMapsListItem();

	sint32 Compare(c3_ListItem *item2, uint32 column);

	SaveMapInfo	*GetSaveMapInfo(void) const { return m_info; }

private:
	c3_Static	*m_itemIcon;
	c3_Static	*m_itemText;
	SaveMapInfo	*m_info;
};

#endif
