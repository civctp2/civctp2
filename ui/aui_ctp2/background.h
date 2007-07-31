










 

#pragma once
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

	virtual MouseEventCallback MouseLDragOver;
	virtual MouseEventCallback MouseLGrabInside;
    virtual MouseEventCallback MouseLDropInside;
    virtual MouseEventCallback MouseLDropOutside;
	virtual MouseEventCallback MouseLDragInside;
    virtual MouseEventCallback MouseRGrabInside;

    virtual MouseEventCallback MouseMoveOver;
	virtual MouseEventCallback MouseMoveInside;
    virtual MouseEventCallback MouseMoveAway;
	virtual MouseEventCallback MouseMoveOutside;

	virtual MouseEventCallback MouseNoChange;

	virtual MouseEventCallback MouseLDoubleClickInside;
	virtual MouseEventCallback MouseRDoubleClickInside;
    
    void ProcessLastMouseMoveThisFrame(aui_MouseEvent *data);


	DrawHandler *m_TheDrawHandler;
};

#endif 
