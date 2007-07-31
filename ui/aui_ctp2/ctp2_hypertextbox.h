

#pragma once

#ifndef __CTP2_HYPERTEXTBOX_H__
#define __CTP2_HYPERTEXTBOX_H__

#include "aui_hypertextbox.h"
#include "PatternBase.h"

class aui_Static;


struct ctp2_HyperLink {
	aui_Static *m_static;	
	sint32 m_db;
	sint32 m_index;
	sint32 m_frame;
	COLORREF m_selectColor;		
	COLORREF m_oldColor;
};


enum CTP2_HYPERLINK_ACTION
{
	CTP2_HYPERLINK_ACTION_FIRST = 0,
	CTP2_HYPERLINK_ACTION_EXECUTE,
	CTP2_HYPERLINK_ACTION_LAST
};


class ctp2_HyperTextBox : public aui_HyperTextBox, public PatternBase
{
public:
	
	ctp2_HyperTextBox(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	ctp2_HyperTextBox(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~ctp2_HyperTextBox();

protected:
	ctp2_HyperTextBox() : aui_HyperTextBox() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE CreateRanger( MBCHAR *ldlBlock = NULL );
	virtual MouseEventCallback MouseLDropInside;
	virtual MouseEventCallback MouseLDropOutside;
	virtual MouseEventCallback MouseLGrabInside;

public:
	void RemoveHyperLinks( void );
	static void DestroyHyperLink( ctp2_HyperLink *hl );

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	
	ctp2_HyperLink *GetSelectedHyperLink( void ) { return m_selectedHyperLink; }

protected:
	virtual AUI_ERRCODE AddHyperStatics( const MBCHAR *hyperText );

	tech_WLList<ctp2_HyperLink *>	*m_hyperLinkList;	

	ctp2_HyperLink *m_selectedHyperLink;


	sint32 m_bevelWidth;
};

#endif 
