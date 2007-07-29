//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface background window
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
// __USING_SPANS
// unknown
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

#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__


#include "aui_window.h"
#include "MapPoint.h"






typedef AUI_ERRCODE (DrawHandler)(LPVOID);



class Background : public aui_Window 
{

    MapPoint m_current_mouse_tile;


public:
	Background(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		DrawHandler *TheDrawHandler )
		:
		aui_Window( retval, id, x, y, width, height, bpp, AUI_WINDOW_TYPE_BACKGROUND ),
		m_TheDrawHandler( TheDrawHandler ) {
            m_lbutton_isdown = FALSE; 
            m_rbutton_isdown = FALSE; 
            m_current_mouse_tile.Set(-1,-1); 
#ifdef __USING_SPANS__
			
			if ( m_dirtyList ) delete m_dirtyList;
			m_dirtyList = new aui_DirtyList( TRUE, width, height );
#endif 
        }

	virtual ~Background() {};

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	virtual AUI_ERRCODE Idle(void);

protected:
    sint32 m_lbutton_isdown; 
    sint32 m_rbutton_isdown; 

    virtual void	MouseLDragOver(aui_MouseEvent * data);
    virtual void	MouseLGrabInside(aui_MouseEvent * data);
    virtual void	MouseLDropInside(aui_MouseEvent * data);
    virtual void	MouseLDropOutside(aui_MouseEvent * data);
    virtual void	MouseLDragInside(aui_MouseEvent * data);
    virtual void	MouseRGrabInside(aui_MouseEvent * data);
    virtual void	MouseMoveOver(aui_MouseEvent * data);
    virtual void	MouseMoveInside(aui_MouseEvent * data);
    virtual void	MouseMoveAway(aui_MouseEvent * data);
    virtual void	MouseMoveOutside(aui_MouseEvent * data);
    virtual void	MouseNoChange(aui_MouseEvent * data);
    virtual void	MouseLDoubleClickInside(aui_MouseEvent * data);
    virtual void	MouseRDoubleClickInside(aui_MouseEvent * data);
    
    void ProcessLastMouseMoveThisFrame(aui_MouseEvent *data);


	DrawHandler *m_TheDrawHandler;
};

#endif 
