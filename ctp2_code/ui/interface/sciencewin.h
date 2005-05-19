

#pragma once
#ifndef __SCIENCEWIN_H__
#define __SCIENCEWIN_H__

#include "c3_listitem.h"
#include "aui_stringtable.h"
#include "c3_updateaction.h"
#include "keyboardhandler.h"

class C3Window;
class c3_Static;
class c3_ListBox;
class c3_Button;
class Thermometer;
class Chart;
class c3_Icon;

#define	k_PLAYERS		7
#define k_EXTRA_PLAYERS	8

enum SCI_UPDATE {
	SCI_UPDATE_NOCHART,
	SCI_UPDATE_NOLIST,
	SCI_UPDATE_ALL
};

enum BRANCH {
	BRANCH_PHYSICAL_SCIENCE,
	BRANCH_FLIGHT,
	BRANCH_CONSTRUCTION,
	BRANCH_DEFENSIVE_WAR,
	BRANCH_SEA,
	BRANCH_AGGRESSIVE_WAR,
	BRANCH_CULTURAL_ADVANCEMENT,
	BRANCH_MECHANICAL_DISCOVERIES,
	BRANCH_ELECTRICITY,
	BRANCH_MEDICINE,
	BRANCH_ECONOMICS,

	BRANCH_MAX
};

sint32 sciencewin_Initialize( void );
sint32 sciencewin_Cleanup( void );

class ScienceWin : public KeyboardHandler {
public:
	C3Window	*m_window;

	ScienceWin( void );
	~ScienceWin( void );

	sint32 Initialize( MBCHAR *windowBlock );
protected:
	c3_Button		*m_closeButton;

	c3_Static		*m_title;

	c3_ListBox		*m_advanceList;

	c3_Button		*m_changeButton;
	c3_Static		*m_researchBox;
	Thermometer		*m_researchMeter;
	c3_Static		*m_researchClock;
	c3_Static		*m_turnsBox;
	c3_Static		*m_costLabel;
	c3_Static		*m_costBox;
	c3_Button		*m_plusButton;
	c3_Button		*m_minusButton;
	c3_Static		*m_percentBox;
	c3_Static		*m_sciLabel;
	c3_Static		*m_sciBeaker;
	c3_Static		*m_sciBox;
	c3_Button		*m_libraryButton;

	Chart			*m_tree;

	c3_Static		*m_knownToLabel;

	c3_Static		**m_playerLabel;
	c3_Icon			**m_playerFlag;

	aui_StringTable	*m_string;

public:
	void Display( void );
	void Remove( void );

	void kh_Close();

	sint32 UpdateData( SCI_UPDATE update );
	sint32 UpdateList( void );

	c3_ListBox *AdvanceList( void ) { return m_advanceList; }
	c3_Button *PlusButton( void ) { return m_plusButton; }
	c3_Button *MinusButton( void ) { return m_minusButton; }
	
	Chart *Tree( void ) { return m_tree; }

	MBCHAR *GetString( sint32 index ) { return m_string->GetString(index); }

};

class KnowledgeListItem: public c3_ListItem
{
public:
	
	
	KnowledgeListItem(AUI_ERRCODE *retval, sint32 index, MBCHAR *ldlBlock);

	
	
	virtual void Update(void);

	
	sint32	GetIndex(void) { return m_index; }

protected:
	KnowledgeListItem() : c3_ListItem() {}

	
	
	AUI_ERRCODE InitCommonLdl(sint32 index, MBCHAR *ldlBlock);
	
public:
	
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	sint32	m_index;
};

class EmbassyListItem: public c3_ListItem
{
public:
	
	
	EmbassyListItem(AUI_ERRCODE *retval, sint32 index, MBCHAR *ldlBlock);

	
	
	virtual void Update(void);

	
	sint32	GetIndex(void) { return m_index; }

protected:
	EmbassyListItem() : c3_ListItem() {}

	
	
	AUI_ERRCODE InitCommonLdl(sint32 index, MBCHAR *ldlBlock);
	
public:
	
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	sint32	m_index;
};

class AdvanceListItem: public c3_ListItem
{
public:
	
	
	AdvanceListItem(AUI_ERRCODE *retval, sint32 index, MBCHAR *ldlBlock);
	~AdvanceListItem();

	
	
	virtual void Update(void);

	
	sint32	GetIndex(void) { return m_index; }
	sint32	GetBranchVal( void ) { return m_branchVal; }

protected:
	AdvanceListItem() : c3_ListItem() {}

	
	
	AUI_ERRCODE InitCommonLdl(sint32 index, MBCHAR *ldlBlock);
	
public:
	
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	sint32	m_index;
	sint32	m_branchVal;
};


sint32 knowledgewin_UpdateFromSwitch( void );
sint32 knowledgewin_InitGraphicTrim( MBCHAR *windowBlock );
sint32 knowledgewin_Initialize( void );
sint32 knowledgewin_Cleanup( void );

class SW_UpdateAction : public c3_UpdateAction 
{
public:
	SW_UpdateAction( BOOL all = FALSE ) { m_all = all;};

	virtual	~SW_UpdateAction() {};
	virtual ActionCallback	Execute;
private:

	BOOL m_all;
};

#endif
