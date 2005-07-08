//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Made StartCountingTo message parameter const.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __UI__INTERFACE__PROGRESSWINDOW_H__
#define __UI__INTERFACE__PROGRESSWINDOW_H__ 1

#include "c3_popupwindow.h"
#include "aui_progressbar.h"
#include "tech_wllist.h"

class ProgressWindow : public c3_PopupWindow
{
public:
	
	
	static void BeginProgress(
		ProgressWindow *&progwin,
		MBCHAR *ldlBlock,
		sint32 maxval,
		sint32 x = -1, sint32 y = -1);

	
	
	
	void StartCountingTo( sint32 val, MBCHAR const * message = NULL );

	
	COLORREF SetBarColor(COLORREF color);

	
	
	static void EndProgress( ProgressWindow *&progwin );

private:
	
	ProgressWindow(
		AUI_ERRCODE *retval,
		MBCHAR *ldlBlock,
		sint32 x = -1, sint32 y = -1);
	virtual ~ProgressWindow();

	c3_Static *m_message;
	aui_ProgressBar *m_pbar;

	tech_WLList<sint32> m_prevValList;
	tech_WLList<sint32> m_nextValList;
	tech_WLList<sint32> m_baseList;
	tech_WLList<double> m_factorList;
};



class StandardProgressBar : public aui_ProgressBar
{
public:
	StandardProgressBar(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock )
		:
		aui_ImageBase( ldlBlock ),
		aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
		aui_ProgressBar( retval, id, ldlBlock )
	{}
	virtual ~StandardProgressBar() {}

protected:
	virtual AUI_ERRCODE CalculateIntervals( double *start, double *stop );
};

#endif

