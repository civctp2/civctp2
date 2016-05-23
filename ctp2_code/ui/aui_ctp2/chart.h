#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CHART_H__
#define __CHART_H__

#include "ctp2_Static.h"


#define k_CHART_LDL_BUTTON			"button"
#define k_CHART_LDL_LEFTIMAGE		"leftimage"
#define k_CHART_LDL_RIGHTIMAGE		"rightimage"

#define k_MAX_PREREQ (4)
#define k_MAX_LEADS_TO (4)
#define k_MAX_EITHER_PREREQ (4)
#define k_TOTAL_BUTTONS (((k_MAX_PREREQ) + (k_MAX_EITHER_PREREQ)) + (k_MAX_LEADS_TO) + 1)

class ctp2_Button;
class c3_HyperTipWindow;

class Chart : public ctp2_Static {
public:

	Chart(
		AUI_ERRCODE	*retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	Chart(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

	AUI_ERRCODE InitCommon( MBCHAR *ldlBlock );

	virtual ~Chart();

	virtual AUI_ERRCODE Draw(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	virtual AUI_ERRCODE DrawThis( aui_Surface *surface = NULL,
									sint32 x = 0,
									sint32 y = 0 );

	AUI_ERRCODE Update( sint32 index );
	AUI_ERRCODE Show();

	ctp2_Button	*GetPreReqButton( sint32 index ) { return m_preReqButton[index]; }
	ctp2_Button	*GetEitherPreReqButton( sint32 index ) { return m_eitherPreReqButton[index]; }
	ctp2_Button	*GetLeadsToButton( sint32 index ) { return m_leadsToButton[index]; }

	sint32		GetPreReqIndex( sint32 index ) { return m_preReqIndex[index]; }
	sint32		GetEitherPreReqIndex( sint32 index ) { return m_eitherPreReqIndex[index]; }
	sint32		GetLeadsToIndex( sint32 index ) { return m_leadsToIndex[index]; }

	sint32		GetNumPreReq( void ) { return m_numPreReq; }
	sint32		GetNumEitherPreReq( void ) { return m_numEitherPreReq; }
	sint32		GetNumLeadsTo( void ) { return m_numLeadsTo; }

	sint32		GetCenterIndex( void ) { return m_centerIndex; }

	sint32		SetTipInfo( ctp2_Button *button, sint32 index );

private:
	ctp2_Button	*m_preReqButton[ k_MAX_PREREQ ];
	ctp2_Button	*m_eitherPreReqButton[ k_MAX_EITHER_PREREQ ];
	ctp2_Button	*m_leadsToButton[ k_MAX_LEADS_TO ];
	ctp2_Button	*m_centerButton;

	sint32	m_preReqIndex[ k_MAX_PREREQ ];
	sint32	m_eitherPreReqIndex[ k_MAX_EITHER_PREREQ ];
	sint32	m_leadsToIndex[ k_MAX_LEADS_TO ];
	sint32	m_centerIndex;

	sint32	m_numPreReq;
	sint32	m_numEitherPreReq;
	sint32	m_numLeadsTo;

	sint32	m_preReqColor[ k_MAX_PREREQ ];
	sint32	m_eitherPreReqColor[ k_MAX_EITHER_PREREQ ];
	sint32	m_leadsToColor[ k_MAX_LEADS_TO ];

	sint32	m_centerColor;

	c3_HyperTipWindow	*m_tip[ k_TOTAL_BUTTONS ];

	ctp2_Static	*m_left;
	ctp2_Static	*m_right;

	sint32	m_heightBetweenButtons;
	sint32	m_distFromCenter;
	sint32	m_distFromSide;
};

void ChartPreReqActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie );
void ChartEitherPreReqActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie );
void ChartLeadsToActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie );
void ChartCenterActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie );

#endif
