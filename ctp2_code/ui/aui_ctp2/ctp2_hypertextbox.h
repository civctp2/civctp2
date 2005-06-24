//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface hypertext link box
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
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines, and the blocks
//       that are inactive for _MSC_VER value 1200 are modified Apolyton code. 
//       The blocks that are active for _MSC_VER value 1200 are the original 
//       Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Event handlers declared in a notation that is more standard C++.
// - #pragma once marked as Microsoft specific.
// - Common line formatting code factored out (FormatText).
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CTP2_HYPERTEXTBOX_H__
#define __CTP2_HYPERTEXTBOX_H__

#include "aui_hypertextbox.h"
#include "patternbase.h"

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

#if defined(_MSC_VER)
	virtual MouseEventCallback MouseLDropInside;
	virtual MouseEventCallback MouseLDropOutside;
	virtual MouseEventCallback MouseLGrabInside;
#else
	virtual void	MouseLDropInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDropOutside(aui_MouseEvent * mouseData);
	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
#endif

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

private:
	void FormatText
	(
		MBCHAR const *	a_Text, 
		MBCHAR const *	a_TextEnd,
		sint32			a_Database,
		sint32			a_Index,
		bool			a_IsLink
	);

};

#endif 
