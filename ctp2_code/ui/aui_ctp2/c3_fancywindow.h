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

#ifndef __C3_FANCYWINDOW_H__
#define __C3_FANCYWINDOW_H__


#include "aui_ldl.h"
#include "c3window.h"
#include "c3_button.h"

class c3_Static;


enum C3_FANCYBORDER { 
	C3_FANCYBORDER_TL = 0,
	C3_FANCYBORDER_TR,
	C3_FANCYBORDER_TOP
};

#define k_NUM_C3_FANCYBORDERS ( C3_FANCYBORDER_TOP + 1 )


class c3_FancyWindow : public C3Window 
{
public:
	c3_FancyWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		MBCHAR ldlBorder[k_NUM_C3_FANCYBORDERS][ k_AUI_LDL_MAXBLOCK + 1 ] = NULL,
		MBCHAR *ldlTitle = NULL,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_BACKGROUND,
		bool bevel = true,
		void (*exitCallBack)( aui_Control *, uint32, uint32, void *) = NULL);

	virtual ~c3_FancyWindow();
	virtual AUI_ERRCODE Resize( sint32 width, sint32 height );

	
	void BringBorderToTop();
	AUI_ERRCODE AddBordersToUI();
	AUI_ERRCODE RemoveBordersFromUI();

	void MakeDraggable( BOOL draggable );

	c3_Static	*Title( void ) const { return m_title; }

	c3_Button	*Cancel( void ) { return m_cancel; }
	c3_Button	*Ok( void ) { return m_ok; }

	void RemoveCancel( void ) { RemoveControl( m_cancel->Id() ); }

protected:
#if defined(_MSC_VER)
	virtual MouseEventCallback MouseLGrabInside;
	virtual MouseEventCallback MouseLDragAway;
#else
    virtual void	MouseLGrabInside(aui_MouseEvent * data);
    virtual void	MouseLDragAway(aui_MouseEvent * data);
#endif

private:
	
	C3Window *m_border[k_NUM_C3_FANCYBORDERS];
	
	POINT	m_offset[k_NUM_C3_FANCYBORDERS];
	
	POINT	m_originalDimensions;
	void NullBorders() { for(uint32 i =0; i< k_NUM_C3_FANCYBORDERS; i++) m_border[i]=NULL; }

	c3_Static	*m_title;

	c3_Button	*m_cancel;
	c3_Button	*m_ok;
};




enum C3_FANCYBORDER2 { 
	C3_FANCYBORDER2_TL = 0,
	C3_FANCYBORDER2_TR,
	C3_FANCYBORDER2_TOP
};

#define k_NUM_C3_FANCYBORDERS2 ( C3_FANCYBORDER2_TOP + 1 )


class c3_FancyWindow2 : public C3Window 
{
public:
	c3_FancyWindow2(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		MBCHAR ldlBorder[k_NUM_C3_FANCYBORDERS2][ k_AUI_LDL_MAXBLOCK + 1 ] = NULL,
		MBCHAR *ldlTitle = NULL,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_BACKGROUND,
		bool bevel = true,
		void (*exitCallBack)( aui_Control *, uint32, uint32, void *) = NULL);

	virtual ~c3_FancyWindow2();

	
	void BringBorderToTop();
	AUI_ERRCODE AddBordersToUI();
	AUI_ERRCODE RemoveBordersFromUI();

	c3_Static	*Title( void ) const { return m_title; }

	c3_Button	*Cancel( void ) { return m_cancel; }
	c3_Button	*Ok( void ) { return m_ok; }

private:
	
	
	
	C3Window *m_border[k_NUM_C3_FANCYBORDERS2];
	
	POINT	m_offset[k_NUM_C3_FANCYBORDERS2];
	void NullBorders() { for(uint32 i =0; i< k_NUM_C3_FANCYBORDERS; i++) m_border[i]=NULL; }

	c3_Static	*m_title;

	c3_Button	*m_cancel;
	c3_Button	*m_ok;
};


#endif 
