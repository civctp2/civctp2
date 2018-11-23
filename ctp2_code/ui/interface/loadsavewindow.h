#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef LOADSAVEWINDOW_H
#define LOADSAVEWINDOW_H

class LoadSaveWindow;

#include "pointerlist.h"
#include "gamefile.h"
#include "c3_popupwindow.h"
#include "c3_listitem.h"

class c3_Static;
class c3_Button;
class ctp2_Button;
class C3Window;
class aui_TextField;
class C3TextField;
class aui_StringTable;
class TextTab;
class c3_ListBox;
class aui_TabGroup;

sint32 loadsavescreen_displayMyWindow(uint32 type);
sint32 loadsavescreen_removeMyWindow(uint32 action);
AUI_ERRCODE loadsavescreen_Initialize( aui_Control::ControlActionCallback
									   *callback = NULL );
void loadsavescreen_Cleanup(void);

void loadsavescreen_SaveGame(const MBCHAR *usePath = NULL, const MBCHAR *useName = NULL);

void loadsavescreen_BeginLoadProcess(SaveInfo *saveInfo, const MBCHAR *directoryPath);

void loadsavescreen_LoadMPGame( void );
void loadsavescreen_LoadSCENGame(void);

void loadsavescreen_delete( void );

void loadsavescreen_executePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void loadsavescreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void loadsavescreen_deletePress(aui_Control *control, uint32 action, uint32 data, void *cookie );

void loadsavescreen_DifficultyScreenActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);

void loadsavescreen_TribeScreenActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
void loadsavescreen_PlayersScreenActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);

void loadsavescreen_ListOneHandler(aui_Control *control, uint32 action, uint32 data, void *cookie );
void loadsavescreen_ListTwoHandler(aui_Control *control, uint32 action, uint32 data, void *cookie );
void loadsavescreen_CivListHandler(aui_Control *control, uint32 action, uint32 data, void *cookie );

enum LSS_TYPE
{
	LSS_FIRST=0,

	LSS_LOAD_GAME=0,
	LSS_SAVE_GAME,

	LSS_LOAD_MP,
	LSS_SAVE_MP,

	LSS_LOAD_SCEN,
	LSS_LOAD_SCEN_MP,
	LSS_SAVE_SCEN,

	LSS_TOTAL
};

class LoadSaveWindow : public c3_PopupWindow
{
public:
	LoadSaveWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD,
		bool bevel = true);

	bool InitCommonLdl(const MBCHAR *ldlBlock);

	virtual ~LoadSaveWindow();

	void FillListOne(void);
	void FillListTwo(GameInfo *info);
	void FillCivList(SaveInfo *info);

	void GetPowerGraph(SaveInfo *info);
	void GetRadarMap(SaveInfo *info);

	void SetPowerGraph(SaveInfo *info);
	void SetRadarMap(SaveInfo *info);

	void SelectCurrentGame(void);
	void SelectCurrentSave(void);

	void SetType(uint32 type);
	uint32 GetType() { return m_type; }

	void SetGameName(const MBCHAR *name);
	void SetSaveName(const MBCHAR *name);
	void SetLeaderName(const MBCHAR *name);
	void SetCivName(const MBCHAR *name);
	void SetNote(const MBCHAR *note);

	BOOL GetGameName(MBCHAR *name);
	BOOL GetSaveName(MBCHAR *name);
	const MBCHAR *GetLeaderName(void);
	const MBCHAR *GetCivName(void);
	BOOL GetNote(MBCHAR *note);

	void EnableFields( BOOL enable );

	GameInfo *GetGameInfo(void) const { return m_gameInfo; }
	SaveInfo *GetSaveInfo(void) const { return m_saveInfo; }
	void SetGameInfo(GameInfo *info);
	void SetSaveInfo(SaveInfo *info);

	void CleanUpSaveInfo( void );
	SaveInfo *GetSaveInfoToSave( void ) const { return m_saveInfoToSave; }

	void BuildDefaultSaveName(const MBCHAR *gameName, MBCHAR *name);

	C3TextField* GetTextField() { return m_gameTextBox; }

	ctp2_Button *GetDeleteButton( void ) {return m_deleteButton; }
	ctp2_Button *GetOkButton( void ) { return Ok(); }
	c3_Static *GetMapStatic( void ) { return m_mapTabImage; }

	c3_ListBox *GetListOne( void ) const { return m_listOne; }
	c3_ListBox *GetListTwo( void ) const { return m_listTwo; }

	bool NoName( void );

private:
	bool        CreateSaveInfoIfNeeded(SaveInfo *& info);

	uint32			 m_type;
	aui_StringTable *m_nameString;

	GameInfo		*m_gameInfo;
	SaveInfo		*m_saveInfo;

	SaveInfo		*m_saveInfoRemember;
	SaveInfo		*m_saveInfoToSave;

	PointerList<GameInfo> *m_fileList;

	c3_Static		*m_titlePanel;

	c3_Static		*m_gameText;
	C3TextField		*m_gameTextBox;

	c3_Static		*m_saveText;
	C3TextField		*m_saveTextBox;

	c3_Static		*m_noteText;
	C3TextField		*m_noteTextBox;

	c3_Static		*m_playerText;
	c3_Static		*m_civText;

	c3_ListBox		*m_listOne;
	c3_ListBox		*m_listTwo;

	aui_TabGroup	*m_tabGroup;

	TextTab			*m_powerTab;
	c3_Static		*m_powerTabImage;
	aui_Image		*m_powerTabImageBackup;

	TextTab			*m_mapTab;
	c3_Static		*m_mapTabImage;
	aui_Image		*m_mapTabImageBackup;

	TextTab			*m_civsTab;
	c3_ListBox		*m_civsList;

	ctp2_Button		*m_deleteButton;
	MBCHAR			m_mostRecentName[_MAX_PATH];
};

class LSCivsListItem : public c3_ListItem
{
public:
	LSCivsListItem(AUI_ERRCODE *retval, const MBCHAR *ldlBlock, const MBCHAR *name);
	~LSCivsListItem();

	sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	c3_Static *m_myItem;
};

class LSGamesListItem : public c3_ListItem
{
public:
	LSGamesListItem(AUI_ERRCODE *retval, const MBCHAR *ldlBlock, GameInfo *info);
	~LSGamesListItem();

	sint32 Compare(c3_ListItem *item2, uint32 column);

	GameInfo	*GetGameInfo(void) const { return m_info; }

private:
	c3_Static	*m_itemIcon;
	c3_Static	*m_itemText;
	GameInfo	*m_info;

};

class LSSavesListItem : public c3_ListItem
{
public:
	LSSavesListItem(AUI_ERRCODE *retval, const MBCHAR *ldlBlock, SaveInfo *info);
	~LSSavesListItem();

	sint32 Compare(c3_ListItem *item2, uint32 column);

	const MBCHAR *GetText(void) const { return m_itemText->GetText(); }

	SaveInfo	*GetSaveInfo(void) const { return m_info; }

private:
	c3_Static	*m_itemIcon;
	c3_Static	*m_itemText;
	SaveInfo	*m_info;
};

AUI_ACTION_BASIC(LSCleanupAction);

extern LoadSaveWindow				*g_loadsaveWindow;

#endif
