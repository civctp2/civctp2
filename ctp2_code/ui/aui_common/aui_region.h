










#ifndef __AUI_REGION_H__
#define __AUI_REGION_H__


#include "aui_base.h"
#include "aui_mouse.h"
#include "tech_wllist.h"


class aui_Surface;
class aui_Dimension;
class aui_Undo;
class aui_DragDropWindow;
class aui_Control;




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


class aui_Region : public aui_Base
{
public:
	
	aui_Region(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock );
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
		return classId == m_regionClassId;
	}
	static uint32 m_regionClassId;

protected:
	aui_Region() : aui_Base() {}
	AUI_ERRCODE InitCommonLdl( uint32 id, MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon(
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height );

public:
	
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
			{ m_editChild = NULL;
			  m_editModeStatus = AUI_EDIT_MODE_CHOOSE_REGION;
			  m_editSelectionCount = 0;
			  m_editSelectionCurrent = 0; }

	AUI_ERRCODE ExpandRect( RECT *rect ); 
	AUI_ERRCODE AddUndo( void );
	AUI_ERRCODE PurgeUndoList( void );
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

	static aui_Region *GetWhichSeesMouse( void ) { return m_whichSeesMouse; }
	static aui_Region *SetWhichSeesMouse(
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

	static aui_Region *m_whichSeesMouse;
								
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
	static	uint32		m_editSelectionCount;
	static	uint32		m_editSelectionCurrent;
	static	aui_Region	*m_editChild;
	static	uint32		m_editModeStatus;
	static  tech_WLList<aui_Undo *> *m_undoList; 

	
	ShowHideCallback	m_showCallback;
	ShowHideCallback	m_hideCallback;
	void				*m_showCallbackData;
	void				*m_hideCallbackData;

	
	
	

	
	typedef void (MouseEventCallback)( aui_MouseEvent *mouseData );

	
	virtual MouseEventCallback PreChildrenCallback {}
	virtual MouseEventCallback PostChildrenCallback {}

	
	virtual MouseEventCallback MouseMoveOver;
	virtual MouseEventCallback MouseMoveAway {}
	virtual MouseEventCallback MouseMoveInside;
	virtual MouseEventCallback MouseMoveOutside {}

	
	virtual MouseEventCallback MouseLDragOver;
	virtual MouseEventCallback MouseLDragAway {}
	virtual MouseEventCallback MouseLDragInside;
	virtual MouseEventCallback MouseLDragOutside {}
	virtual MouseEventCallback MouseRDragOver;
	virtual MouseEventCallback MouseRDragAway {}
	virtual MouseEventCallback MouseRDragInside;
	virtual MouseEventCallback MouseRDragOutside {}

	
	virtual MouseEventCallback MouseLGrabInside;
	virtual MouseEventCallback MouseLGrabOutside {}
	virtual MouseEventCallback MouseLDropInside;
	virtual MouseEventCallback MouseLDropOutside {}
	virtual MouseEventCallback MouseRGrabInside;
	virtual MouseEventCallback MouseRGrabOutside {}
	virtual MouseEventCallback MouseRDropInside;
	virtual MouseEventCallback MouseRDropOutside {}

	
	
	
	virtual MouseEventCallback MouseLDoubleClickInside;
	virtual MouseEventCallback MouseLDoubleClickOutside;
	virtual MouseEventCallback MouseRDoubleClickInside;
	virtual MouseEventCallback MouseRDoubleClickOutside;

	
	virtual MouseEventCallback MouseNoChange;

	
	
	
	MouseEventCallback MouseMoveOverEdit {}
	MouseEventCallback MouseMoveAwayEdit {}
	MouseEventCallback MouseMoveInsideEdit {}
	MouseEventCallback MouseMoveOutsideEdit {}
	
	
	MouseEventCallback MouseLDragOverEdit;
	MouseEventCallback MouseLDragAwayEdit;
	MouseEventCallback MouseLDragInsideEdit;
	MouseEventCallback MouseLDragOutsideEdit;
	MouseEventCallback MouseRDragOverEdit {}
	MouseEventCallback MouseRDragAwayEdit {}
	MouseEventCallback MouseRDragInsideEdit {}
	MouseEventCallback MouseRDragOutsideEdit {}

	
	MouseEventCallback MouseLGrabInsideEdit;
	MouseEventCallback MouseLGrabOutsideEdit;
	MouseEventCallback MouseLDropInsideEdit;
	MouseEventCallback MouseLDropOutsideEdit;
	MouseEventCallback MouseRGrabInsideEdit;
	MouseEventCallback MouseRGrabOutsideEdit;
	MouseEventCallback MouseRDropInsideEdit;
	MouseEventCallback MouseRDropOutsideEdit;

	
	MouseEventCallback MouseLGrabEditMode;
	MouseEventCallback MouseLDropEditMode;
	MouseEventCallback MouseLDragEditMode;

	
	
	
	MouseEventCallback MouseLDoubleClickInsideEdit {}
	MouseEventCallback MouseLDoubleClickOutsideEdit {}
	MouseEventCallback MouseRDoubleClickInsideEdit {}
	MouseEventCallback MouseRDoubleClickOutsideEdit {}
};


#endif 
