

#pragma once

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

	
	
	
	void StartCountingTo( sint32 val, MBCHAR *message = NULL );

	
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
		aui_ProgressBar( retval, id, ldlBlock ),
		aui_ImageBase( ldlBlock ),
		aui_TextBase( ldlBlock, (const MBCHAR *)NULL ) {}
	virtual ~StandardProgressBar() {}

protected:
	virtual AUI_ERRCODE CalculateIntervals( double *start, double *stop );
};
