
 

#pragma once
#ifndef __SCIENCEWINDOW_H__
#define __SCIENCEWINDOW_H__


#include "c3window.h"

enum DATABASE;

class c3_HyperTextBox;


class ScienceWindow : public C3Window 
{
public:
	ScienceWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );
	ScienceWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );

	virtual ~ScienceWindow() {}

	virtual AUI_ERRCODE InitCommon(void);

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	
	sint32 SetTechMode ( sint32 mode, DATABASE database );

	void SetGivesText( c3_HyperTextBox *text ) { m_givesBox = text; };
	sint32 LoadGivesText ( void );

	char	m_givesFile[_MAX_PATH];

private:
	c3_HyperTextBox	*m_givesBox;

};


#endif 
