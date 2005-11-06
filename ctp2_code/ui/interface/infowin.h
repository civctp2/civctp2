








#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __INFOWIN_H__
#define __INFOWIN_H__

class InfoBigListItem;
class InfoPlayerListItem;
class InfoScoreLabelListItem;
class InfoScoreListItem;
class InfoWonderListItem;

enum {
	k_INFOWIN_DATA_NULL,
	k_INFOWIN_DATA_OFF,
	k_INFOWIN_DATA_ON
};

enum {
	k_INFOWIN_NULL_SETTING,
	k_INFOWIN_BIG_SETTING,
	k_INFOWIN_WONDER_SETTING,
	k_INFOWIN_STRENGTH_SETTING,
	k_INFOWIN_SCORE_SETTING,
	k_INFOWIN_POLLUTION_SETTING
};

#include "aui_action.h"     // AUI_ACTION_BASIC
#include "c3_listitem.h"    // c3_ListItem
#include "Unit.h"           // Unit

class LineGraph;


class InfoBigListItem : public c3_ListItem
{
public:
	
	
	InfoBigListItem(AUI_ERRCODE *retval, Unit *city, sint32 index, MBCHAR *ldlBlock);
	~InfoBigListItem();
	
	
	
	virtual void Update(void);

	
	Unit	*GetCity( void ) { return &m_city; }
	sint32	GetIndex( void ) { return m_index; }

protected:
	InfoBigListItem() : c3_ListItem() {}

	
	
	AUI_ERRCODE InitCommonLdl(Unit *city, sint32 index, MBCHAR *ldlBlock);
	
public:
	
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	Unit			m_city;
	sint32			m_index;

	MBCHAR			m_name[256];
	MBCHAR			m_civ_name[256];
	sint32			m_size;

};





class InfoPlayerListItem : public c3_ListItem
{
public:
	
	
	InfoPlayerListItem(AUI_ERRCODE *retval, MBCHAR *name, sint32 index, MBCHAR *ldlBlock);
	
	
	
	virtual void Update(void);

	
	sint32	GetIndex( void ) { return m_index; }
	MBCHAR	*GetName( void ) { return m_name; }
	void	SetIndex( sint32 index ) { m_index = index; }

protected:
	InfoPlayerListItem() : c3_ListItem() {}

	
	
	AUI_ERRCODE InitCommonLdl(MBCHAR *name, sint32 index, MBCHAR *ldlBlock);
	
public:
	
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	sint32			m_index;
	MBCHAR			m_name[256];
};

class InfoWonderListItem : public c3_ListItem
{
public:
	
	
	InfoWonderListItem(AUI_ERRCODE *retval, sint32 player, sint32 index, MBCHAR *ldlBlock);
	
	
	
	virtual void Update(void);

	
	Unit	*GetCity( void ) { return &m_city; }
	sint32	GetIndex( void ) { return m_index; }
	sint32	GetPlayer( void ) { return m_player; }

protected:
	InfoWonderListItem() : c3_ListItem() {}

	
	
	AUI_ERRCODE InitCommonLdl(sint32 player, sint32 index, MBCHAR *ldlBlock);
	
public:
	
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	Unit			m_city;
	sint32			m_index;
	sint32			m_player;
};

class InfoScoreListItem : public c3_ListItem
{
public:
	
	
	InfoScoreListItem(AUI_ERRCODE *retval, sint32 player, sint32 index, MBCHAR *ldlBlock);
	
	
	
	virtual void Update(void);

	
	sint32	GetIndex( void ) { return m_index; }
	sint32	GetPlayer( void ) { return m_player; }
	sint32	GetValue( void ) { return m_value; }

protected:
	InfoScoreListItem() : c3_ListItem() {}

	
	
	AUI_ERRCODE InitCommonLdl(sint32 player, sint32 index, MBCHAR *ldlBlock);
	
public:
	
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	sint32			m_index;
	sint32			m_player;
	sint32			m_value;
};

class InfoScoreLabelListItem : public c3_ListItem
{
public:
	
	
	InfoScoreLabelListItem(AUI_ERRCODE *retval, MBCHAR *label, MBCHAR *text, MBCHAR *ldlBlock);
	
	
	
	virtual void Update(void);

protected:
	InfoScoreLabelListItem() : c3_ListItem() {}

	
	
	AUI_ERRCODE InitCommonLdl(MBCHAR *label, MBCHAR *text, MBCHAR *ldlBlock);
	
public:
	
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	MBCHAR			m_label[256];
	MBCHAR			m_text[256];

};

AUI_ACTION_BASIC(InfoCleanupAction);

sint32  infowin_Initialize( void );
void    infowin_Cleanup(void);
void    infowin_Cleanup_Controls(void);


sint32 infowin_Init_Controls( MBCHAR *windowBlock );


void infowin_SetMinRoundForGraphs(sint32 minRound);

sint32 infowin_LoadData( void );
sint32 infowin_UpdateBigList( void );
sint32 infowin_UpdateScoreList( void );
sint32 infowin_UpdateWonderList( void );

sint32 infowin_UpdatePlayerList( void );
sint32 infowin_UpdateCivData( void );
sint32 infowin_UpdatePollutionData( void );

sint32 infowin_UpdateGraph( LineGraph *infoGraph, 
							sint32 &infoXCount,  
							sint32 &infoYCount,
							double ***infoGraphData);

sint32 infowin_UpdatePollutionGraph( LineGraph *infoGraph, 
							sint32 &infoXCount,  
							sint32 &infoYCount,
							double ***infoGraphData);

sint32 infowin_ChangeSetting( sint32 type );
sint32 infowin_ChangeDataSetting( sint32 type );
sint32 infowin_DisplayLab( void );
sint32 infowin_GetCivScore( sint32 player );
sint32 infowin_LabReady( void );
sint32 infowin_GetWonderCityName( sint32 index, MBCHAR *name);

#endif
