//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Great library
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
// HAVE_PRAGMA_ONCE
// - Uses #pragma once preprocessor derective
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Microsoft extensions marked.
// - Increased maximum library text size to support the German version.
// - Exported database name size max.
// - Added function to look up an item name on creation index.
// - Added alpha <-> index functions. (Sep 13th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------
//
/// \file   GreatLibrary.h
/// \brief  Great library handling

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __GREATLIBRARY_H__
#define __GREATLIBRARY_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include <vector>	// std::vector
#include "CTPDatabase.h"

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class GreatLibrary;
class Great_Library_Item;
class TechListItem;

#define k_GL_TREE_LEFT		5
#define k_GL_TREE_TOP		40
#define k_GL_TREE_WIDTH		750
#define k_GL_TREE_HEIGHT	105

#define k_GL_VIDEO_WIDTH	240
#define k_GL_VIDEO_HEIGHT	180

#define k_GL_TITLE_WIDTH	340
#define k_GL_TITLE_HEIGHT	30

#define k_VIDEO_X		239
#define k_VIDEO_Y		184
#define k_VIDEO_WIDTH	160
#define k_VIDEO_HEIGHT	120

#define k_GL_NUM_BUTTONS		18
#define k_GL_NUM_COLS			1
#define k_GL_BUTTONS_PER_COL	18

//#define k_GL_INDEX_INVALID		-1

size_t const	k_MAX_GL_ENTRY		= 8192;
size_t const	GL_MAX_DB_NAME_SIZE	= 30;

#include "GreatLibraryTypes.h"

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "ctp2_listitem.h"
#include "keyboardhandler.h"
#include "CTPRecord.h"

class Chart;
class ctp2_HyperTextBox;
#ifdef __AUI_USE_DIRECTX__
class DirectVideo;
#endif
class ctp2_Static;
class aui_StringTable;
class ctp2_Button;
class aui_SwitchGroup;
class ctp2_TabGroup;
class ctp2_Tab;
class TextTab;
class ctp2_TextField;
class aui_Control;
class ctp2_ListBox;

class GreatLibraryWindow;
class ctp2_Window;
template <class DATA_TYPE> class Text_Hasher;
class SlicContext;

//----------------------------------------------------------------------------
// Declarations
//----------------------------------------------------------------------------

class Great_Library_Item
{
public:
	enum DATABASE m_database;
	int m_item;
};

class TechListItem: public ctp2_ListItem
{
public:
	TechListItem(AUI_ERRCODE *retval, sint32 index, DATABASE database, MBCHAR *ldlBlock);

	virtual void Update(void);

	sint32	GetIndex(void) const { return m_index; }
	DATABASE GetDatabase(void) const { return m_database; }

	virtual sint32 Compare(ctp2_ListItem *item2, uint32 column);

protected:
    TechListItem()
    :
        ctp2_ListItem        (),
        m_index              (CTPRecord::INDEX_INVALID),
        m_database           (DATABASE_DEFAULT)
    {};

private:
    AUI_ERRCODE InitCommonLdl(MBCHAR const * ldlBlock);

    sint32      m_index;
    DATABASE    m_database;
};

class GreatLibrary final : public KeyboardHandler
{
public:
	GreatLibrary(sint32 theMode);
	~GreatLibrary();

	static void Initialize_Great_Library_Data();
	static void Shutdown_Great_Library_Data();
	static void Load_Great_Library();

	static int Get_Database_From_Name
	(
		char const *    database_name
	);

	static int Get_Object_Index_From_Name
	(
		int             which_database,
		char const *    object_name
	);

	static Text_Hasher<char *> * s_great_library_info;

	void Back();
	void Forward();

	void Display( void );
	void Remove( void );
	void kh_Close();

	sint32 SetLibrary( sint32 theMode, DATABASE theDatabase,
		bool add_to_history = true);


	void    ClearHistory(void);
	void    HandleSetGoal(void);

	MBCHAR const *  GetItemName(int database, int item) const;      // lexicographic index
    MBCHAR const *  GetObjectName(int database, int index) const;   // database index
	MBCHAR const *  GetSelectionName() const;

	void SetCategoryName
	(
		int the_database
	);

	void    HandleIndexButton( ctp2_Button *button );
	void    HandleListButton
	(
		aui_Control *control,
		uint32 action,
		uint32 data,
		void *cookie
	);

	void UpdateList(DATABASE database);

	bool GetSci( void ) const { return m_sci; }
	void SetSci( bool sci ) { m_sci = sci; }

	ctp2_Button *GetAdvancesButton( void ) const { return m_advancesButton; }





	int Get_Database_Size(int the_database);

	void Search_Great_Library();

	void Force_A_Search();

	void Add_Item_To_Topics_List
	(
		const MBCHAR *name,
		int index
	);

	ctp2_Window * GetWindow( void ) const;

	void FixTabs();
	sint32 GetIndexFromAlpha(sint32 alpha, DATABASE theDatabase) const;
	sint32 GetAlphaFromIndex(sint32 index, DATABASE theDatabase) const;
	bool IsHidden(sint32 index, DATABASE theDatabase) const;

private:
	void Initialize(MBCHAR const * windowBlock);
	template <class T>
	void AddTopicsBasedOnAge(CTPDatabase<T> * database);

	friend void TechListItem::Update(void);
	friend bool greatlibrary_Initialize(sint32 theMode, bool sci);

	ctp2_Button		*m_setGoalButton;

	Chart *m_techTree;

	ctp2_HyperTextBox	*m_techRequirementsText;
	ctp2_HyperTextBox	*m_techVariablesText;

#ifdef WIN32
	DirectVideo		*m_techMovie;
#endif
	ctp2_Static		*m_techStillShot;

	aui_StringTable	*m_string;

	LIB_STRING		m_buttonString;

	ctp2_TabGroup	*m_tabGroup;
	ctp2_Tab		*m_gameplayTab;
	ctp2_Tab		*m_historicalTab;
	ctp2_Tab		*m_techTab;
	ctp2_HyperTextBox	*m_techHistoricalText;
	ctp2_HyperTextBox	*m_techGameplayText;

	ctp2_Button		*m_okButton;

	ctp2_Button		*m_backButton;

	ctp2_Button		*m_forwardButton;

	ctp2_Static		*m_categoryText;

	ctp2_Static		*m_searchLabel;

	ctp2_TextField *m_search_word;

	aui_SwitchGroup *m_indexButtonSwitchGroup;
	ctp2_Button		*m_searchButton;
	ctp2_Button		*m_unitsButton;
	ctp2_Button		*m_improveButton;
	ctp2_Button		*m_wondersButton;
	ctp2_Button		*m_advancesButton;
	ctp2_Button		*m_governButton;
	ctp2_Button		*m_terrainButton;
	ctp2_Button		*m_tileimpButton;
	ctp2_Button		*m_conceptButton;
	ctp2_Button		*m_goodsButton;
	ctp2_Button		*m_ordersButton;

	ctp2_ListBox	*m_topics_list;
	ctp2_Static		*m_indexLeft;
	ctp2_Static		*m_indexMiddle;
	ctp2_Static		*m_indexRight;

	sint32			m_page;
	bool			m_maxPage;
	DATABASE		m_database;
	DATABASE    m_listDatabase;
	sint32			m_selectedIndex;
	sint32			m_maxIndex;

	bool			m_sci;

	ctp2_Static *m_itemLabel;

	std::vector<Great_Library_Item> m_search_results;

	std::vector<Great_Library_Item> m_history;

	int m_history_position;

	GreatLibraryWindow	*m_window;
};

const MBCHAR *  glutil_LoadText(const char * filename, SlicContext & so);
bool            greatlibrary_Initialize(sint32 theMode, bool sci = false);
void            greatlibrary_Cleanup(void);

extern GreatLibrary	*   g_greatLibrary;

#endif
