












#pragma once
#ifndef __TEXTTABLE_H__
#define __TEXTTABLE_H__


#include "aui_listbox.h"
#include "patternbase.h"


class aui_Surface;
class aui_Static;	
class TextSwitch;


#define k_TEXTTABLE_MAXTEXT			256		
#define k_TEXTTABLE_MAXITEMS		200		
#define k_TEXTTABLE_MAXHEADERS		20		
#define k_TEXTTABLE_LDL_COLUMNS		"columns"	
#define k_TEXTTABLE_LDL_TEXTSIZE	"textsize"	


class TextTable : public aui_ListBox, public PatternBase
{
public:
	
	TextTable(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		aui_Surface *surface = NULL);
	TextTable(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 columns,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		uint32 size = 0,
		aui_Surface *surface = NULL);
	virtual ~TextTable();

protected:
	TextTable() : aui_ListBox() {}
	AUI_ERRCODE InitCommon( uint32 columns = 0, aui_Surface *surface = NULL );
	AUI_ERRCODE InitCommon( MBCHAR *ldlBlock, aui_Surface *surface = NULL );
	AUI_ERRCODE	CreateRangers( void );

public:
	void			InitHeaders( void );
	BOOL			AddColumn( uint32 pos = 0, MBCHAR *text = NULL );
	BOOL			RemoveColumn( uint32 pos = 0 ){return TRUE;};
	BOOL			AddRow( uint32 pos = 0, MBCHAR *text = NULL );
	BOOL			RemoveRow( uint32 pos = 0 ){return TRUE;};

	BOOL			SetTextEntry( uint32 row, uint32 col, MBCHAR *text );
	BOOL			SetTextHeader( uint32 pos, MBCHAR *text );
	BOOL			GetTextEntry( uint32 row, uint32 col, MBCHAR *strbuf );
	BOOL			GetTextHeader( uint32 pos, MBCHAR *strbuf );
	uint32			GetNumItems( void ) { return m_numItems; };

	BOOL			CleanTextTable( void );
	BOOL			SortTextTable( sint32 type = 0 ){return TRUE;};

	uint32			GetNumRows( void ) { return m_rows; };
	uint32			GetNumColumns( void ) { return m_columns; };

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	
	uint32		m_columns;						
	uint32		m_rows;							
	uint32		m_item_width;					
	uint32		m_item_height;					

	
	uint32		m_numItems;						
	
	aui_Static	*m_items[ k_TEXTTABLE_MAXITEMS ];	
	TextSwitch	*m_table_headers[ k_TEXTTABLE_MAXHEADERS ];	
};


#endif 
