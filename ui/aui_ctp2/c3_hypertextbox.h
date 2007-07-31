

#pragma once

#ifndef __C3_HYPERTEXTBOX_H__
#define __C3_HYPERTEXTBOX_H__

#include "aui_hypertextbox.h"
#include "PatternBase.h"

class aui_Static;


struct c3_HyperLink {
	aui_Static *m_static;	
	sint32 m_db;
	sint32 m_index;
	sint32 m_frame;
	COLORREF m_selectColor;		
	COLORREF m_oldColor;
};


enum C3_HYPERLINK_ACTION
{
	C3_HYPERLINK_ACTION_FIRST = 0,
	C3_HYPERLINK_ACTION_EXECUTE,
	C3_HYPERLINK_ACTION_LAST
};


class c3_HyperTextBox : public aui_HyperTextBox, public PatternBase
{
public:
	
	c3_HyperTextBox(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		bool draw_frame = true
		);
	c3_HyperTextBox(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~c3_HyperTextBox();

protected:
	bool m_draw_frame;
	c3_HyperTextBox() : aui_HyperTextBox() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE CreateRanger( MBCHAR *ldlBlock = NULL );
	virtual MouseEventCallback MouseLDropInside;
	virtual MouseEventCallback MouseLDropOutside;
	virtual MouseEventCallback MouseLGrabInside;

public:
	void RemoveHyperLinks( void );
	static void DestroyHyperLink( c3_HyperLink *hl );

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	
	c3_HyperLink *GetSelectedHyperLink( void ) { return m_selectedHyperLink; }

protected:
	virtual AUI_ERRCODE AddHyperStatics( const MBCHAR *hyperText );

	tech_WLList<c3_HyperLink *>	*m_hyperLinkList;	

	c3_HyperLink *m_selectedHyperLink;

	sint32 m_bevelWidth;
};

#endif 
