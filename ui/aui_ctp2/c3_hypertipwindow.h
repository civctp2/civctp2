










#ifndef __C3_HYPERTIPWINDOW_H__
#define __C3_HYPERTIPWINDOW_H__


#include "aui_tipwindow.h"
#include "patternbase.h"


class aui_HyperTextBox;



#define k_C3_HYPERTIPWINDOW_LDL_TIP		"hypertip"



class c3_HyperTipWindow : public aui_TipWindow, public PatternBase
{
public:
	
	c3_HyperTipWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock );
	c3_HyperTipWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern );
	virtual ~c3_HyperTipWindow();

protected:
	c3_HyperTipWindow() : aui_TipWindow() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );

public:
	
	aui_HyperTextBox *GetHyperTip( void ) const { return m_hyperTip; }
	AUI_ERRCODE SetHyperTipText(MBCHAR *text);

	
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	BOOL m_allocatedHyperTip;
	aui_HyperTextBox	*m_hyperTip;	
};


#endif 
