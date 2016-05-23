//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface fancy? window
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

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __C3FancyWindow_H__
#define __C3FancyWindow_H__

#include "c3window.h"
#include "aui_ldl.h"

class c3_Button;


enum FancyBorder { TL = 0,TR,BL,BR,
	TOPL,TOPR,BOTTOM,FB_LEFT,FB_RIGHT,HEADER };

#define k_NUM_BORDERS (HEADER+1)

class C3FancyWindow : public C3Window
{
public:
	C3FancyWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		MBCHAR ldlBorder[k_NUM_BORDERS][ k_AUI_LDL_MAXBLOCK + 1 ] = NULL,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_BACKGROUND,
		bool bevel = true,
		void (*exitCallBack)( aui_Control *, uint32, uint32, void *) = NULL);

	virtual ~C3FancyWindow();
	virtual AUI_ERRCODE Resize( sint32 width, sint32 height );

	void BringBorderToTop();
	AUI_ERRCODE AddBordersToUI();
	AUI_ERRCODE RemoveBordersFromUI();

	void MakeDraggable( BOOL draggable );

protected:
    virtual void	MouseLGrabInside(aui_MouseEvent * data);
    virtual void	MouseLDragAway(aui_MouseEvent * data);

private:
	C3Window *m_border[k_NUM_BORDERS];

	POINT	m_offset[k_NUM_BORDERS];

	POINT	m_originalDimensions;
	void NullBorders() { for(uint32 i =0; i< k_NUM_BORDERS; i++) m_border[i]=NULL; }
	c3_Button *m_exit;
};

#endif
