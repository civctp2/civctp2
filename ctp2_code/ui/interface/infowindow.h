//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Information window
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
// - Added Update function so that the info window doesn't need to be closed
//   for update during the turns. - Aug 7th 2005 Martin
// - Added cleanup method. (Sep 13th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif 

#ifndef __INFOWINDOW_H__
#define __INFOWINDOW_H__

class InfoWindow;

#include "ctp2_inttypes.h"  // uint32

class aui_Control;
class ctp2_Button;
class ctp2_Window;
class RankingTab;
class ScoreTab;
class WonderTab;

class InfoWindow
{
public:
	InfoWindow();
	~InfoWindow();

	ctp2_Window *GetWindow() {return m_window;}

	static void Open(void);
	static void Close(void);
	static void Update(void);
	static void SelectRankingTab(void);
	static void SelectScoreTab(void);
	static void SelectWonderTab(void);
	static void Cleanup(void);

private:
	void Hide(void);
	void Show(void);
	void UpdateData(void);

	static void CloseButtonActionCallback(aui_Control *control,
	                                      uint32 action, uint32 data, void *cookie);

	ctp2_Window * m_window;
	ctp2_Button * m_closeButton;
	RankingTab *  m_ranking_tab;
	ScoreTab *    m_score_tab;
	WonderTab *   m_wonder_tab;
};

#endif
