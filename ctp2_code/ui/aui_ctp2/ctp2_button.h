//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface button
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
// - Use Microsoft C++ extensions when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Event handlers declared in a notation that is more standard C++.
// - #pragma once commented out.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CTP2_BUTTON_H__
#define CTP2_BUTTON_H__


#include "aui_Button.h"


class aui_ImageList;


class ctp2_Button : public aui_Button
{
public:
	
	ctp2_Button(AUI_ERRCODE *retval,
				uint32 id,
				MBCHAR *ldlBlock,
				ControlActionCallback *ActionFunc = NULL,
				void *cookie = NULL) :
	aui_Button(retval, id, ldlBlock, ActionFunc, cookie),
		aui_ImageBase(ldlBlock),
		aui_TextBase(ldlBlock, NULL)
	{ *retval = InitCommonLdl(ldlBlock, NULL, 0, 0, 0, 0); }

	
	ctp2_Button(AUI_ERRCODE *retval,
				uint32 id,
				MBCHAR *ldlBlock,
				MBCHAR *ldlTemplate,
				sint32 x,      
				sint32 y,      
				sint32 width,
				sint32 height,
				ControlActionCallback *ActionFunc = NULL,
				void *cookie = NULL) :
	aui_Button(retval, id, ldlBlock, ActionFunc, cookie),
		aui_ImageBase(ldlBlock),
		aui_TextBase(ldlBlock, NULL)
	{ *retval = InitCommonLdl(ldlBlock, ldlTemplate,
		x, y, width, height); }

	
	virtual ~ctp2_Button();

	
	AUI_ERRCODE InitCommonLdl(MBCHAR *ldlBlock, MBCHAR *ldlTemplate,
		sint32 x, sint32 y, sint32 width, sint32 height);

	
	
	virtual AUI_ERRCODE DrawThis(aui_Surface *surface = NULL,
								 sint32 x = 0, sint32 y = 0);

	
#if defined(_MSC_VER)
	virtual MouseEventCallback MouseLGrabInside;
	virtual MouseEventCallback MouseLDropInside;
	virtual MouseEventCallback MouseLDragOver;
	virtual MouseEventCallback MouseLDragAway;
#else
	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDropInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDragOver(aui_MouseEvent * mouseData);
	virtual void	MouseLDragAway(aui_MouseEvent * mouseData);
#endif
	
	static const sint32 k_CTP2_BUTTON_LAYER_FLAG_UP;
	static const sint32 k_CTP2_BUTTON_LAYER_FLAG_DOWN;
	static const sint32 k_CTP2_BUTTON_LAYER_FLAG_UP_ON;

	virtual bool IgnoreHighlight() { return m_ignoreHighlight; }

	virtual bool CanAttract() { return true; }
	virtual void SetAttract(bool on, uint32 ticksSinceStart) ;
	virtual bool GetAttracting() { return m_attract; }

	void SetToggleState(bool state);
	bool GetToggleState() { return m_toggleState; }

private:
	
	virtual void ResetCurrentRenderFlags();

	
	void RenderDown(bool status = true);

	bool m_ignoreHighlight;
	bool m_attract;
	uint32 m_attractTicks;
	bool m_isDropdownButton;

	bool m_isToggle;
	bool m_toggleState;
};

#endif 
