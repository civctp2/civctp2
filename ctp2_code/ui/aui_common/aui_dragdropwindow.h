










#ifndef __AUI_DRAGDROPWINDOW_H__
#define __AUI_DRAGDROPWINDOW_H__


#include "aui_window.h"



class aui_DragDropWindow : public aui_Window
{
public:
	
	aui_DragDropWindow(
		AUI_ERRCODE *retval,
		aui_Control *dragDropItem,
		aui_Region *launchPad,
		sint32 x = 0,
		sint32 y = 0,
		sint32 width = 0,
		sint32 height = 0 );
	virtual ~aui_DragDropWindow() {}

protected:
	aui_DragDropWindow() : aui_Window() {}
	AUI_ERRCODE InitCommon(
		aui_Control *dragDropItem,
		aui_Region *launchPad );

public:
	
	void StartDragging( sint32 localX, sint32 localY );

	aui_Region *GetLaunchPad( void ) const { return m_launchPad; }
	void SetLaunchPad( aui_Region *launchPad )
	{ m_launchPad = launchPad; }

	aui_Control *GetDragDropItem( void ) const { return m_dragDropItem; }
	void SetDragDropItem( aui_Control *dragDropItem )
	{ m_dragDropItem = dragDropItem; }

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	
	aui_Control *m_dragDropItem;

	
	aui_Region *m_launchPad;

	
	
	

	
	virtual MouseEventCallback MouseLDropOutside;
};


#endif 
