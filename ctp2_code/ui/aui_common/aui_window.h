//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface window
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
// - Updated friend declarations.
//
//----------------------------------------------------------------------------

#ifndef __AUI_WINDOW_H__
#define __AUI_WINDOW_H__


#include "aui_region.h"
#include "aui_dirtylist.h"
#include "aui_mouse.h"
#include "PointerList.h"

class aui_UI;
class aui_Mouse;
class aui_Control;
class aui_Surface;
struct aui_Stencil;




enum AUI_WINDOW_TYPE
{
	AUI_WINDOW_TYPE_FIRST = 0,
	AUI_WINDOW_TYPE_BACKGROUND = 0,
	AUI_WINDOW_TYPE_SINKING,
	AUI_WINDOW_TYPE_STANDARD,
	AUI_WINDOW_TYPE_FLOATING,
	AUI_WINDOW_TYPE_POPUP,
	AUI_WINDOW_TYPE_TIP,
	AUI_WINDOW_TYPE_LAST
};




#define k_WINDOW_ATTRIBUTE_HIDDEN			k_REGION_ATTRIBUTE_HIDDEN
#define k_WINDOW_ATTRIBUTE_DISABLED			k_REGION_ATTRIBUTE_DISABLED
#define k_WINDOW_ATTRIBUTE_DRAGDROP			k_REGION_ATTRIBUTE_DRAGDROP
#define k_WINDOW_ATTRIBUTE_TRANSPARENT		0x00000008
#define k_WINDOW_ATTRIBUTE_TRANSLUCENT		0x00000010
#define k_WINDOW_ATTRIBUTE_STRONGLYMODAL	0x00000020
#define k_WINDOW_ATTRIBUTE_WEAKLYMODAL		0x00000040
#define k_WINDOW_ATTRIBUTE_DRAGGABLE		0x00000080
#define k_WINDOW_ATTRIBUTE_DYNAMIC 	   	   	0x00000100



class aui_Window : public aui_Region
{
public:
	
	aui_Window(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp = 0, 
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );
	aui_Window(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp = 0, 
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );
	virtual ~aui_Window();

	virtual BOOL IsThisA( uint32 classId )
	{
		return classId == m_windowClassId
		||     aui_Region::IsThisA( classId );
	}
	static uint32 m_windowClassId;

protected:
	aui_Window() : aui_Region() {}
	AUI_ERRCODE InitCommon( sint32 bpp, AUI_WINDOW_TYPE type );
#if defined(_MSC_VER)
	friend aui_UI;
	friend aui_Mouse;
#else
#endif

public:
	virtual AUI_ERRCODE	Move( sint32 x, sint32 y );
	virtual AUI_ERRCODE Offset( sint32 dx, sint32 dy );
	virtual AUI_ERRCODE	Resize( sint32 width, sint32 height );

	virtual AUI_ERRCODE MoveOG( void );


	
	aui_DirtyList *GetDirtyList( void ) { return m_dirtyList; }

	
	aui_Surface *TheSurface( void ) const { return m_surface; }


	BOOL		AreControlsOpaque( void ) const { return m_opaqueControls; }
	uint32		Type( void ) const { return m_type; }

	
	AUI_ERRCODE	AddControl( aui_Control *control )
	{ return AddChild( (aui_Region *)control ); }
	AUI_ERRCODE	RemoveControl( uint32 controlId )
	{ return RemoveChild( controlId ); }
	aui_Control	*GetControl( uint32 controlId )
	{ return (aui_Control *)GetChild( controlId ); }
	virtual AUI_ERRCODE	AddChild( aui_Region *child );
	virtual AUI_ERRCODE	RemoveChild( uint32 controlId );

	
	virtual AUI_ERRCODE	ShowThis( void );
	virtual AUI_ERRCODE	HideThis( void );

	
	BOOL	IsDirty( void ) const { return !m_dirtyList->IsEmpty(); }

	
	BOOL IsOpaque( void ) const
		{ return !IsTransparent() && !IsTranslucent(); }
	BOOL IsTransparent( void ) const
		{ return m_attributes & k_WINDOW_ATTRIBUTE_TRANSPARENT; }
	BOOL IsTranslucent( void ) const
		{ return m_attributes & k_WINDOW_ATTRIBUTE_TRANSLUCENT; }
	BOOL IsModal( void ) const
		{ return IsStronglyModal() || IsWeaklyModal(); }
	BOOL IsStronglyModal( void ) const
		{ return m_attributes & k_WINDOW_ATTRIBUTE_STRONGLYMODAL; }
	BOOL IsWeaklyModal( void ) const
		{ return m_attributes & k_WINDOW_ATTRIBUTE_WEAKLYMODAL; }
	BOOL IsDraggable( void ) const
		{ return m_attributes & k_WINDOW_ATTRIBUTE_DRAGGABLE; }
	BOOL IsDynamic( void ) const
		{ return m_attributes & k_WINDOW_ATTRIBUTE_DYNAMIC; }

	uint32 SetTransparent( BOOL transparent, BOOL transparentControls = FALSE );
	uint32 SetTranslucent( BOOL translucent, BOOL translucentControls = FALSE );
	uint32 SetStronglyModal( BOOL stronglyModal );
	uint32 SetWeaklyModal( BOOL weaklyModal );
	uint32 SetDraggable( BOOL draggable );
	uint32 SetDynamic( BOOL dynamic );

	
	aui_Region *GrabRegion( void ) const { return m_grabRegion; }

	
	virtual AUI_ERRCODE Draw(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	
	AUI_ERRCODE	Invalidate( RECT *rect = NULL );

	
	AUI_ERRCODE AddDirtyRect( RECT *rect );
	AUI_ERRCODE AddDirtyRect(
		sint32 left,
		sint32 top,
		sint32 right,
		sint32 bottom );

	
	void SetSurface( aui_Surface *surface ) { m_surface = surface; }
	void SetType( AUI_WINDOW_TYPE type ) { m_type = type; }

	
	BOOL IsDragging() { return m_isDragging; }

	void SetStencilFromImage(const MBCHAR *imageFileName);
	aui_Stencil *GetStencil() { return m_stencil; }

	virtual bool HandleKey(uint32 wParam);
	aui_Control *GetFocusControl();
	aui_Control *NextFocusControl();
	void AddFocusControl(aui_Control *control);

protected:
	virtual AUI_ERRCODE CreateSurface( void );



	void MakeSureSurfaceIsValid( void );
	void DeleteSurfaceIfDynamic( void );

	aui_Surface	*m_surface;		


	BOOL m_opaqueControls;		
	sint32 m_bpp;				

	AUI_WINDOW_TYPE m_type;		

	aui_DirtyList *m_dirtyList;	

	BOOL		m_isDragging;	
	POINT		m_grabPoint;	
	aui_Region	*m_grabRegion;	

	sint32 m_ogX;
	sint32 m_ogY;

	aui_Stencil *m_stencil;     

	aui_Control *m_focusControl;
	tech_WLList<aui_Region *> *m_focusList;

#if defined(_MSC_VER)
	virtual MouseEventCallback PostChildrenCallback;

	
	virtual MouseEventCallback MouseLDragOver;
	virtual MouseEventCallback MouseLDragAway;
	virtual MouseEventCallback MouseLDragInside;
	virtual MouseEventCallback MouseLDragOutside;

	
	virtual MouseEventCallback MouseLGrabInside;
	virtual MouseEventCallback MouseLGrabOutside;
	virtual MouseEventCallback MouseLDropInside;
	virtual MouseEventCallback MouseLDropOutside;
#else
	virtual void	PostChildrenCallback(aui_MouseEvent * mouseData);	
	virtual void	MouseLDragOver(aui_MouseEvent * mouseData);	
	virtual void	MouseLDragAway(aui_MouseEvent * mouseData);	
	virtual void	MouseLDragInside(aui_MouseEvent * mouseData);	
	virtual void	MouseLDragOutside(aui_MouseEvent * mouseData);	
	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);	
	virtual void	MouseLGrabOutside(aui_MouseEvent * mouseData);	
	virtual void	MouseLDropInside(aui_MouseEvent * mouseData);	
	virtual void	MouseLDropOutside(aui_MouseEvent * mouseData);	
#endif
};


#endif 
