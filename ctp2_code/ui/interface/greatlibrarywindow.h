//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// - Memory leaks repaired in LoadText by Martin G�hmann.
// - Added variable and requirement retriever methods. (Sep 13th 2005 Martin G�hmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __GREATLIBRARYWINDOW_H__
#define __GREATLIBRARYWINDOW_H__

#include "greatlibrary.h"
#include "SlicObject.h"

class aui_SwitchGroup;
class aui_Static;
#ifdef WIN32
class DirectVideo;
#endif
class Chart;
class ctp2_Static;
class ctp2_HyperTextBox;

class ctp2_Window;
class ctp2_Static;

class GreatLibraryWindow {
public:
	ctp2_Window		*m_window;


	static const int GREAT_LIBRARY_PANEL_BLANK;
	GreatLibraryWindow(AUI_ERRCODE *err);
	virtual ~GreatLibraryWindow();

	virtual AUI_ERRCODE Idle (void);

	void SetTechTitle( ctp2_Static *text ) { m_techTitle = text; };
	void SetTechTree( Chart *chart ) { m_techTree = chart; }
	void SetTechStillShot( ctp2_Static *image ) { m_techStillShot = image; };
	void SetTechDescriptionGroup( aui_SwitchGroup *group )
	{
		m_techDescriptionGroup = group;
	};
	void SetTechHistoricalText( ctp2_HyperTextBox *text ) { m_techHistoricalText = text; };
	void SetTechGameplayText( ctp2_HyperTextBox *text ) { m_techGameplayText = text; };
#ifdef WIN32
	void SetTechMovie( DirectVideo *movie ) { m_techMovie = movie; };
#endif
	void SetTechRequirementsText( ctp2_HyperTextBox *text )
	{
		m_techRequirementsText = text;
	};
	void SetTechVariablesText( ctp2_HyperTextBox *text )
	{
		m_techVariablesText = text;
	};

	char * GetIconRecText( int database, int item,
		uint8 historical);

	char * GetHistoricalText( int database, int item );
	char * GetGameplayText( int database, int item );
	char * GetRequirementsText( int database, int item );
	char * GetVariablesText( int database, int item );

	sint32 SetTechMode ( sint32 mode, DATABASE database );
	sint32 GetTechMode ( void ) { return m_mode; };

	void SetTechDatabase( DATABASE database ) { m_database = database; }
	DATABASE GetTechDatabase( void ) { return m_database; }

	sint32 LoadText( ctp2_HyperTextBox *textbox, char *filename, SlicObject &so );
	sint32 LoadHistoricalText( SlicObject &so );
	sint32 LoadGameplayText ( SlicObject &so );
	sint32 LoadRequirementsText ( SlicObject &so );
	sint32 LoadVariablesText ( SlicObject &so );

	void PlayTechMovie ( void );
	sint32 LoadTechMovie ( void );
	sint32 LoadTechStill( void );

	char	m_title_name[_MAX_PATH];
	char	m_still_file[_MAX_PATH];
	char	m_history_file[_MAX_PATH];
	char	m_gameplay_file[_MAX_PATH];
	char	m_movie_file[_MAX_PATH];
	char	m_requirement_file[_MAX_PATH];
	char	m_variable_file[_MAX_PATH];

private:

	sint32					m_mode;
	DATABASE				m_database;

	ctp2_Static				*m_techTitle;
	Chart					*m_techTree;
	ctp2_Static				*m_techStillShot;

	aui_SwitchGroup			*m_techDescriptionGroup;
	ctp2_HyperTextBox		*m_techHistoricalText;
	ctp2_HyperTextBox		*m_techGameplayText;
	ctp2_HyperTextBox		*m_techRequirementsText;
	ctp2_HyperTextBox		*m_techVariablesText;
#ifdef WIN32
	DirectVideo				*m_techMovie;
#endif
};

BOOL getNonBlankLine (char *buf, int buf_size, FILE *fp);

#endif
