//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface region (rectangle on the display)
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Event handlers declared in a notation that is more standard C++.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __AUI_REGION_H__
#define __AUI_REGION_H__

class aui_Region;

#define k_REGION_ATTRIBUTE_HIDDEN		0x00000001
#define k_REGION_ATTRIBUTE_DISABLED		0x00000002
#define k_REGION_ATTRIBUTE_DRAGDROP		0x00000004

#define k_AUI_REGION_DRAWFLAG_UPDATE					0x00000001 
#define k_AUI_REGION_DRAWFLAG_MOUSEMOVEOVER				0x00000002
#define k_AUI_REGION_DRAWFLAG_MOUSEMOVEAWAY				0x00000004
#define k_AUI_REGION_DRAWFLAG_MOUSEMOVEINSIDE			0x00000008
#define k_AUI_REGION_DRAWFLAG_MOUSEMOVEOUTSIDE			0x00000010
#define k_AUI_REGION_DRAWFLAG_MOUSELDRAGOVER			0x00000020
#define k_AUI_REGION_DRAWFLAG_MOUSELDRAGAWAY			0x00000040
#define k_AUI_REGION_DRAWFLAG_MOUSELDRAGINSIDE			0x00000080
#define k_AUI_REGION_DRAWFLAG_MOUSELDRAGOUTSIDE			0x00000100
#define k_AUI_REGION_DRAWFLAG_MOUSERDRAGOVER			0x00000200
#define k_AUI_REGION_DRAWFLAG_MOUSERDRAGAWAY			0x00000400
#define k_AUI_REGION_DRAWFLAG_MOUSERDRAGINSIDE			0x00000800
#define k_AUI_REGION_DRAWFLAG_MOUSERDRAGOUTSIDE			0x00001000
#define k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE			0x00002000
#define k_AUI_REGION_DRAWFLAG_MOUSELGRABOUTSIDE			0x00004000
#define k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE			0x00008000
#define k_AUI_REGION_DRAWFLAG_MOUSELDROPOUTSIDE			0x00010000
#define k_AUI_REGION_DRAWFLAG_MOUSERGRABINSIDE			0x00020000
#define k_AUI_REGION_DRAWFLAG_MOUSERGRABOUTSIDE			0x00040000
#define k_AUI_REGION_DRAWFLAG_MOUSERDROPINSIDE			0x00080000
#define k_AUI_REGION_DRAWFLAG_MOUSERDROPOUTSIDE			0x00100000
#define k_AUI_REGION_DRAWFLAG_MOUSELDOUBLECLICKINSIDE	0x00200000
#define k_AUI_REGION_DRAWFLAG_MOUSELDOUBLECLICKOUTSIDE	0x00400000
#define k_AUI_REGION_DRAWFLAG_MOUSERDOUBLECLICKINSIDE	0x00800000
#define k_AUI_REGION_DRAWFLAG_MOUSERDOUBLECLICKOUTSIDE	0x01000000
#define k_AUI_REGION_DRAWFLAG_MOUSENOCHANGE				0x02000000
#define k_AUI_REGION_DRAWFLAG_KEYSTATECHANGE			0x04000000
#define k_AUI_REGION_DRAWFLAG_KEYBOARDFOCUSCHANGE		0x08000000
#define k_AUI_REGION_DRAWFLAG_OVERLAP					0x10000000




#define k_AUI_REGION_DRAWFLAG_DEFAULTMASK				0xffffffff




#define k_REGION_GRAB_NONE					0x00000000
#define k_REGION_GRAB_INSIDE				0x00000001
#define k_REGION_GRAB_LEFT					0x00000002
#define k_REGION_GRAB_TOP					0x00000004
#define k_REGION_GRAB_RIGHT					0x00000008
#define k_REGION_GRAB_BOTTOM				0x00000010

#define k_REGION_GRAB_SIZE					4

enum AUI_EDIT_MODE_STATUS
{
	AUI_EDIT_MODE_FIRST = 0,
	AUI_EDIT_MODE_CHOOSE_REGION = 0,
	AUI_EDIT_MODE_CHOOSE_PARENT_REGION,
	AUI_EDIT_MODE_REGION_SELECTED,
	AUI_EDIT_MODE_MODIFY,
	AUI_EDIT_MODE_LAST
};

#include "aui_base.h"        // aui_Base
#include "aui_dimension.h"
#include "aui_mouse.h"       // aui_MouseEvent
#include "ctp2_inttypes.h"   // sintN, uintN
#include "tech_wllist.h"

class aui_Control;
class aui_Dimension;
class aui_DragDropWindow;
class aui_Surface;
class aui_Undo;
// AUI_ERRCODE, MBCHAR, POINT, RECT

class aui_Region : public aui_Base
{
public:
    aui_Region
    (
        AUI_ERRCODE *   retval,
	    uint32          id,
        MBCHAR const *  ldlBlock
    );
	aui_Region(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height );
	virtual ~aui_Region();

	virtual BOOL IsThisA( uint32 classId )
	{
		return classId == s_regionClassId;
	}

	uint32	&Id( void ) { return m_id; }
	uint32	Attributes( void ) const { return m_attributes; }

	
	virtual AUI_ERRCODE	Move( sint32 x, sint32 y );
	virtual AUI_ERRCODE Offset( sint32 dx, sint32 dy );
	virtual AUI_ERRCODE	Resize( sint32 width, sint32 height );

	
	virtual AUI_ERRCODE	Adjust( void );

	
	sint32	X( void ) const { return m_x; }
	sint32	Y( void ) const { return m_y; }
	sint32	Width( void ) const { return m_width; }
	sint32	Height( void ) const { return m_height; }

	aui_Dimension	*GetDim( void ) const { return m_dim; }

	aui_Region			*GetParent( void ) const { return m_parent; }
	virtual AUI_ERRCODE	SetParent( aui_Region *region );

	tech_WLList<aui_Region *>	*ChildList( void ) const { return m_childList; }

	
	BOOL	IsInside( DWORD point ) const;
	BOOL	IsInside( LPPOINT point ) const;
	BOOL	IsInside( LONG x, LONG y ) const;

	
	virtual AUI_ERRCODE	Show( void );
	virtual AUI_ERRCODE	Hide( void );
	virtual AUI_ERRCODE	ShowThis( void );
	virtual AUI_ERRCODE	HideThis( void );
	AUI_ERRCODE	ShowChildren( void );
	AUI_ERRCODE	HideChildren( void );

	
	
	
	
	AUI_ERRCODE Reset( void );
	virtual AUI_ERRCODE ResetThis( void );

	
	AUI_ERRCODE	Enable( BOOL enable );
	AUI_ERRCODE EnableDragDrop( BOOL enable );

	BOOL IsDescendent( aui_Region *region );

	BOOL IsHidden( void ) const
		{ return m_attributes & k_REGION_ATTRIBUTE_HIDDEN; }
	BOOL IsDisabled( void ) const
		{ return m_attributes & k_REGION_ATTRIBUTE_DISABLED; }
	BOOL IsDragDrop( void ) const
		{ return m_attributes & k_REGION_ATTRIBUTE_DRAGDROP; }

	AUI_ERRCODE	HandleMouseEvent( aui_MouseEvent *input, BOOL handleIt = TRUE );
	void MouseDispatch( aui_MouseEvent *input, BOOL handleIt );

	
	void MouseDispatchEdit( aui_MouseEvent *input, BOOL handleIt );
	void EditModeModifyRegion( RECT rect );
	static void EditModeClear( void ) 
			{ s_editChild = NULL;
			  s_editModeStatus = AUI_EDIT_MODE_CHOOSE_REGION;
			  s_editSelectionCount = 0;
			  s_editSelectionCurrent = 0; }

	AUI_ERRCODE ExpandRect( RECT *rect ); 
	AUI_ERRCODE AddUndo( void );
	void        PurgeUndoList( void );
	AUI_ERRCODE UndoEdit( void );

	
	uint32 GetDrawMask() const { return(m_drawMask); }
	uint32 SetDrawMask(uint32 drawMask);	
											

	
	
	
	
	virtual uint32 ShouldDraw(uint32 draw = k_AUI_REGION_DRAWFLAG_UPDATE);

	virtual AUI_ERRCODE Draw(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
	AUI_ERRCODE DrawChildren(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 ) { return AUI_ERRCODE_OK; }

	
	virtual AUI_ERRCODE Idle( void ) { return AUI_ERRCODE_OK; }

	
	virtual AUI_ERRCODE	AddChild( aui_Region *child );
	virtual AUI_ERRCODE InsertChild( aui_Region *child, sint32 index );
	virtual AUI_ERRCODE	RemoveChild( uint32 regionId );
	aui_Region			*GetChild( uint32 regionId );

	
	aui_Region	*GetChildByIndex(sint32 index);
	sint32		NumChildren(void);

	
	virtual void DeleteChildren();

	BOOL	IgnoreEvents( BOOL ignore );
	BOOL	IgnoringEvents( void ) const { return m_ignoreEvents; }

	uint32		GetDoubleClickTimeOut( void ) const
		{ return m_doubleClickTimeOut; }
	AUI_ERRCODE	SetDoubleClickTimeOut( uint32 doubleClickTimeOut )
		{ m_doubleClickTimeOut = doubleClickTimeOut; return AUI_ERRCODE_OK; }

	static aui_Region * GetWhichSeesMouse( void ) { return s_whichSeesMouse; }
	static aui_Region * SetWhichSeesMouse(
		aui_Region *region,
		BOOL force = FALSE );

	BOOL IsBlind( void ) const { return m_blind; }
	BOOL SetBlindness( BOOL blind );

	
	
	AUI_ERRCODE DoneInstantiating();

	
	virtual bool CanAttract() { return false; }
	virtual void SetAttract(bool on, uint32 ticksSinceStart) {}
	virtual bool GetAttracting() { return false; }

	virtual const MBCHAR *GetLdlBlock();
	virtual void SetLdlBlock(const MBCHAR *ldlblock);

	
	typedef void (*ShowHideCallback)(aui_Region *region, void *userData);
	void SetShowCallback(ShowHideCallback callback, void *userData = NULL)
	{ m_showCallback = callback; m_showCallbackData = userData; }
	void SetHideCallback(ShowHideCallback callback, void *userData = NULL)
	{ m_hideCallback = callback; m_hideCallbackData = userData; }

protected:
	aui_Region() 
    :
        aui_Base                    (),
        m_id                        (0),
        m_x                         (0),
        m_y                         (0),
        m_width                     (0),	
        m_height                    (0),
        m_dim                       (new aui_Dimension()),
        m_attributes                (0),
        m_parent                    (NULL),
        m_childList                 (new tech_WLList<aui_Region *>()),
        m_childListChanged          (false),
        m_blind                     (false),
        m_mouseCode                 (AUI_ERRCODE_UNHANDLED),
        m_draw                      (0),
        m_drawMask                  (k_AUI_REGION_DRAWFLAG_DEFAULTMASK),
        m_ignoreEvents              (false),
        m_isMouseInside             (false),
        //	aui_MouseEvent	m_mouseState;
        m_xLastTime                 (0),	
        m_yLastTime                 (0),
        m_noChange                  (false),
        m_noChangeTime              (0),
        m_doubleLClickStartWaitTime (0),
        m_doubleRClickStartWaitTime (0),
        m_doubleClickingInside      (true),
        m_doubleClickTimeOut        (0),
        // POINT		m_doubleClickOldPos;
        m_ldlBlock                  (NULL),
        // POINT		m_editGrabPoint;			
        m_editGrabPointAttributes   (0),
        m_showCallback              (NULL),
        m_hideCallback              (NULL),
        m_showCallbackData          (NULL),
        m_hideCallbackData          (NULL)
    {
        InitCommon();
    };
    // @todo Remove later
	AUI_ERRCODE InitCommonLdl(uint32 id, MBCHAR const * ldlBlock)
    {
        m_id = id;
        return InitCommonLdl(ldlBlock);
    };
    // @todo Remove later
	AUI_ERRCODE InitCommon
    (
        uint32 id, 
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height
    )
    {
        m_id        = id;
        m_x         = x;
        m_y         = y;
        m_width     = width;
        m_height    = height;
        InitCommon();
        return AUI_ERRCODE_OK;
    };
	
	virtual AUI_ERRCODE DoneInstantiatingThis(const MBCHAR *ldlBlock);

	
	BOOL HasHeirarchyChanged( void ) const;
	void ResetHeirarchyChanged( void );
	virtual aui_DragDropWindow *CreateDragDropWindow( aui_Control *dragDropItem );
	virtual void DestroyDragDropWindow( aui_DragDropWindow *ddw );

	uint32		m_id;			

	sint32		m_x;			
	sint32		m_y;			
	sint32		m_width;		
	sint32		m_height;		

	aui_Dimension	*m_dim;		

	uint32		m_attributes;	

	aui_Region	*m_parent;		
	tech_WLList<aui_Region *> *m_childList;
								
	BOOL		m_childListChanged;

	BOOL		m_blind;		

	AUI_ERRCODE	m_mouseCode;	
								

	uint32		m_draw;			
	uint32		m_drawMask;		

	BOOL		m_ignoreEvents;	

	BOOL		m_isMouseInside;
	aui_MouseEvent	m_mouseState;

	sint32		m_xLastTime;	
	sint32		m_yLastTime;	

	BOOL		m_noChange;		
	uint32		m_noChangeTime;	

	uint32		m_doubleLClickStartWaitTime;
								
	uint32		m_doubleRClickStartWaitTime;
								
	BOOL		m_doubleClickingInside;	
	uint32		m_doubleClickTimeOut;	
	POINT		m_doubleClickOldPos;	

	MBCHAR     *m_ldlBlock;  

	
	POINT				m_editGrabPoint;			
	uint32				m_editGrabPointAttributes;

	
	ShowHideCallback	m_showCallback;
	ShowHideCallback	m_hideCallback;
	void				*m_showCallbackData;
	void				*m_hideCallbackData;

	
	
	

	typedef void (MouseEventCallback)( aui_MouseEvent *mouseData );
	
	virtual void	PreChildrenCallback(aui_MouseEvent * mouseData) {};
	virtual void	PostChildrenCallback(aui_MouseEvent * mouseData) {};

	virtual void	MouseMoveOver(aui_MouseEvent * mouseData);
	virtual void	MouseMoveAway(aui_MouseEvent * mouseData) {};
	virtual void	MouseMoveInside(aui_MouseEvent * mouseData);
	virtual void	MouseMoveOutside(aui_MouseEvent * mouseData) {};

	virtual void	MouseLDragOver(aui_MouseEvent * mouseData);
	virtual void	MouseLDragAway(aui_MouseEvent * mouseData) {};
	virtual void	MouseLDragInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDragOutside(aui_MouseEvent * mouseData) {};
	virtual void	MouseRDragOver(aui_MouseEvent * mouseData);
	virtual void	MouseRDragAway(aui_MouseEvent * mouseData) {};
	virtual void	MouseRDragInside(aui_MouseEvent * mouseData);
	virtual void	MouseRDragOutside(aui_MouseEvent * mouseData) {};
	
	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseLGrabOutside(aui_MouseEvent * mouseData) {};
	virtual void	MouseLDropInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDropOutside(aui_MouseEvent * mouseData) {};
	virtual void	MouseRGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseRGrabOutside(aui_MouseEvent * mouseData) {};
	virtual void	MouseRDropInside(aui_MouseEvent * mouseData);
	virtual void	MouseRDropOutside(aui_MouseEvent * mouseData) {};
	
	virtual void	MouseLDoubleClickInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDoubleClickOutside(aui_MouseEvent * mouseData);
	virtual void	MouseRDoubleClickInside(aui_MouseEvent * mouseData);
	virtual void	MouseRDoubleClickOutside(aui_MouseEvent * mouseData);
	
	virtual void	MouseNoChange(aui_MouseEvent * mouseData);
	
	void			MouseMoveOverEdit(aui_MouseEvent * mouseData) {};
	void			MouseMoveAwayEdit(aui_MouseEvent * mouseData) {};
	void			MouseMoveInsideEdit(aui_MouseEvent * mouseData) {};
	void			MouseMoveOutsideEdit(aui_MouseEvent * mouseData) {};
	
	void			MouseLDragOverEdit(aui_MouseEvent * mouseData);
	void			MouseLDragAwayEdit(aui_MouseEvent * mouseData);
	void			MouseLDragInsideEdit(aui_MouseEvent * mouseData);
	void			MouseLDragOutsideEdit(aui_MouseEvent * mouseData);
	void			MouseRDragOverEdit(aui_MouseEvent * mouseData) {};
	void			MouseRDragAwayEdit(aui_MouseEvent * mouseData) {};
	void			MouseRDragInsideEdit(aui_MouseEvent * mouseData) {};
	void			MouseRDragOutsideEdit(aui_MouseEvent * mouseData) {};

	void			MouseLGrabInsideEdit(aui_MouseEvent * mouseData);
	void			MouseLGrabOutsideEdit(aui_MouseEvent * mouseData);
	void			MouseLDropInsideEdit(aui_MouseEvent * mouseData);
	void			MouseLDropOutsideEdit(aui_MouseEvent * mouseData);
	void			MouseRGrabInsideEdit(aui_MouseEvent * mouseData);
	void			MouseRGrabOutsideEdit(aui_MouseEvent * mouseData);
	void			MouseRDropInsideEdit(aui_MouseEvent * mouseData);
	void			MouseRDropOutsideEdit(aui_MouseEvent * mouseData);

	void			MouseLGrabEditMode(aui_MouseEvent * mouseData);
	void			MouseLDropEditMode(aui_MouseEvent * mouseData);
	void			MouseLDragEditMode(aui_MouseEvent * mouseData);

	void			MouseLDoubleClickInsideEdit(aui_MouseEvent * mouseData) {};
	void			MouseLDoubleClickOutsideEdit(aui_MouseEvent * mouseData) {};
	void			MouseRDoubleClickInsideEdit(aui_MouseEvent * mouseData) {};
	void			MouseRDoubleClickOutsideEdit(aui_MouseEvent * mouseData) {};

private:
	AUI_ERRCODE InitCommonLdl(MBCHAR const * ldlBlock);
	void        InitCommon(void);

	static aui_Region *                 s_whichSeesMouse;
	static uint32                       s_regionClassId;
	static uint32		                s_editSelectionCount;
	static uint32		                s_editSelectionCurrent;
	static aui_Region *                 s_editChild;
	static uint32	        	        s_editModeStatus;
	static tech_WLList<aui_Undo *> *    s_undoList; 
};


#endif 
