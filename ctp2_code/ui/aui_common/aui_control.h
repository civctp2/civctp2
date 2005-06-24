//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface control
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
// - Prevented crash in destructor after using the default constructor.
//
//----------------------------------------------------------------------------

#ifndef __AUI_CONTROL_H__
#define __AUI_CONTROL_H__


#include "aui_region.h"
#include "aui_imagebase.h"
#include "aui_textbase.h"
#include "aui_soundbase.h"
#include "aui_keyboard.h"
#include "aui_joystick.h"


#include <utility>


class aui_ImageList;
class aui_Window;
class aui_Action;
class aui_StringTable;



#define k_CONTROL_ATTRIBUTE_HIDDEN		k_REGION_ATTRIBUTE_HIDDEN
#define k_CONTROL_ATTRIBUTE_DISABLED	k_REGION_ATTRIBUTE_DISABLED
#define k_CONTROL_ATTRIBUTE_DRAGDROP	k_REGION_ATTRIBUTE_DRAGDROP
#define k_CONTROL_ATTRIBUTE_DOWN		0x00000008
#define k_CONTROL_ATTRIBUTE_ON			0x00000010
#define k_CONTROL_ATTRIBUTE_ACTIVE		0x00000020



#define k_CONTROL_DEFAULT_SIZE			20		
#define k_CONTROL_DEFAULT_TIMEOUT		500		
#define k_CONTROL_DEFAULT_REPEATTIME	50		



#define k_AUI_CONTROL_LDL_TIPWINDOW			"tipwindow"
#define k_AUI_CONTROL_LDL_STRINGTABLE		"stringtable"



class aui_Control
	:
	public aui_Region,
	public virtual aui_ImageBase,
	public virtual aui_TextBase,
	public aui_SoundBase
{
public:
	typedef void (ControlActionCallback)(
		aui_Control *control,
		uint32 state,
		uint32 data,
		void *cookie );

	
	aui_Control(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_Control(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~aui_Control();

	virtual BOOL IsThisA( uint32 classId )
	{
		return classId == m_controlClassId
		||     aui_Region::IsThisA( classId );
	}
	static uint32 m_controlClassId;

protected:
	aui_Control() 
    :   aui_ImageBase       (),
	aui_TextBase        (),
	aui_Region          (),
	aui_SoundBase       (),
	m_stringTable       (NULL),
        m_allocatedTip      (false),
		m_statusText        (NULL),
		m_numberOfLayers    (0), 
	m_imagesPerLayer    (0),
		m_imageLayerList    (NULL),
		m_layerRenderFlags  (NULL),
		m_renderFlags       (k_AUI_CONTROL_LAYER_FLAG_ALWAYS)
    {  }

	AUI_ERRCODE InitCommonLdl(
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc,
		void *cookie );
	AUI_ERRCODE InitCommon(
		ControlActionCallback *ActionFunc,
		void *cookie );

public:
	
	aui_StringTable *m_stringTable;

	virtual AUI_ERRCODE ResetThis( void );

	AUI_ERRCODE AddSubControl( aui_Control *control )
	{ return AddChild( (aui_Region *)control ); }
	AUI_ERRCODE InsertSubControl( aui_Control *control, sint32 index )
	{ return InsertChild( (aui_Region *)control, index ); }
	AUI_ERRCODE RemoveSubControl( uint32 controlId )
	{ return RemoveChild( controlId ); }
	aui_Control	*GetSubControl( uint32 controlId )
	{ return (aui_Control *)GetChild( controlId ); }
	virtual AUI_ERRCODE	AddChild( aui_Region *child );
	virtual AUI_ERRCODE InsertChild( aui_Region *child, sint32 index );
	virtual AUI_ERRCODE	RemoveChild( uint32 controlId );

	
	aui_Window	*GetParentWindow( void ) const { return m_window; }
	AUI_ERRCODE	SetParentWindow( aui_Window *window );

	aui_Window	*GetTipWindow( void ) const { return m_tip; }
	aui_Window	*SetTipWindow( aui_Window *window );

	
	AUI_ERRCODE	ToWindow( RECT *rect );
	AUI_ERRCODE	ToWindow( POINT *point );
	AUI_ERRCODE	ToScreen( RECT *rect );
	AUI_ERRCODE	ToScreen( POINT *point );

	BOOL IsDown( void ) const
		{ return m_attributes & k_CONTROL_ATTRIBUTE_DOWN; }
	BOOL IsOn( void ) const
		{ return m_attributes & k_CONTROL_ATTRIBUTE_ON; }
	BOOL IsActive( void ) const
		{ return m_attributes & k_CONTROL_ATTRIBUTE_ACTIVE; }

	virtual AUI_ERRCODE	SetText(
		const MBCHAR *text,
		uint32 maxlen = 0xffffffff )
	{
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
		return aui_TextBase::SetText( text, maxlen );
	}
	virtual AUI_ERRCODE	AppendText( MBCHAR *text )
	{
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
		return aui_TextBase::AppendText( text );
	}

	
	AUI_ERRCODE SetActionFuncAndCookie(
		ControlActionCallback *ActionFunc,
		void *cookie );

	ControlActionCallback *GetActionFunc( void ) const
	{ return m_ActionFunc; }

	
	void *GetCookie( void ) const
	{ Assert( m_ActionFunc != NULL ); return m_cookie; }

	
	aui_Action *SetAction( aui_Action *action );

	
	aui_Action *GetAction( void ) const
	{ Assert( m_ActionFunc == NULL ); return m_action; }

	
	virtual AUI_ERRCODE ShowThis();

	virtual AUI_ERRCODE	HideThis( void );

	uint32		GetTimeOut( void ) const { return m_timeOut; }
	AUI_ERRCODE	SetTimeOut( uint32 timeOut )
		{ m_timeOut = timeOut; return AUI_ERRCODE_OK; }

	uint32		GetRepeatTime( void ) const { return m_repeatTime; }
	AUI_ERRCODE	SetRepeatTime( uint32 repeatTime )
		{ m_repeatTime = repeatTime; return AUI_ERRCODE_OK; }

	static aui_Control	*GetMouseOwnership( void ) { return m_whichOwnsMouse; }
	virtual aui_Control	*SetMouseOwnership( void );
	virtual AUI_ERRCODE	ReleaseMouseOwnership( void );

	static aui_Control	*GetKeyboardFocus( void ) { return m_whichHasFocus; }
	virtual aui_Control	*SetKeyboardFocus( void );
	virtual AUI_ERRCODE	ReleaseKeyboardFocus( void );

	
	virtual AUI_ERRCODE Draw(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	
	AUI_ERRCODE DrawThisStateImage(
		sint32 state,
		aui_Surface *destSurf,
		RECT *destRect );

	
	virtual AUI_ERRCODE	HandleKeyboardEvent( aui_KeyboardEvent *input );
	virtual AUI_ERRCODE	HandleJoystickEvent( aui_JoystickEvent *input );

	BOOL	ShowTipWindow( aui_MouseEvent *mouseData );
	BOOL	HideTipWindow( void );

protected:
	
	union
	{
		void *m_cookie;
		aui_Action *m_action;
	};
	ControlActionCallback *m_ActionFunc;

	aui_Window		*m_window;		
	aui_Window		*m_tip;			
	BOOL			m_allocatedTip; 
	BOOL			m_showingTip;	

	uint32			m_startWaitTime;
	uint32			m_timeOut;		
									

	uint32			m_repeatTime;	
	uint32			m_lastRepeatTime;		

	static aui_Control	*m_whichOwnsMouse;	
	static aui_Control	*m_whichHasFocus;	

	aui_KeyboardEvent	m_keyboardEvent;	
	aui_JoystickEvent	m_joystickEvent;	

	uint32 m_actionKey;                     
	uint32 m_keyboardAction;                
	
	
	

	
	typedef void (KeyboardEventCallback)( aui_KeyboardEvent *mouseData );
	typedef void (JoystickEventCallback)( aui_JoystickEvent *mouseData );

#if defined(_MSC_VER)	
	virtual KeyboardEventCallback KeyboardCallback {}
	virtual JoystickEventCallback JoystickCallback {}

	virtual MouseEventCallback MouseMoveOver;
	virtual MouseEventCallback MouseMoveAway;
	virtual MouseEventCallback MouseMoveInside;

	
	virtual MouseEventCallback MouseLDragInside;
	virtual MouseEventCallback MouseRDragInside;
	virtual MouseEventCallback MouseLDragOver;
	virtual MouseEventCallback MouseLDragAway;
	virtual MouseEventCallback MouseRDragOver;
	virtual MouseEventCallback MouseRDragAway;

	
	virtual MouseEventCallback MouseNoChange;
#else
	virtual void	KeyboardCallback(aui_KeyboardEvent * keyBoardData) {};
	virtual void	JoystickCallback(aui_JoystickEvent * joystickData) {};

	virtual void	MouseMoveOver(aui_MouseEvent * mouseData);	
	virtual void	MouseMoveAway(aui_MouseEvent * 	mouseData);	
	virtual void	MouseMoveInside(aui_MouseEvent * mouseData);

	virtual void	MouseLDragInside(aui_MouseEvent * mouseData);	
	virtual void	MouseRDragInside(aui_MouseEvent * mouseData);	
	virtual void	MouseLDragOver(aui_MouseEvent * mouseData);	
	virtual void	MouseLDragAway(aui_MouseEvent * mouseData);	
	virtual void	MouseRDragOver(aui_MouseEvent * mouseData);	
	virtual void	MouseRDragAway(aui_MouseEvent * mouseData);

	virtual void	MouseNoChange(aui_MouseEvent * mouseData);	
#endif

private:
	
	const MBCHAR *m_statusText;




public:
	
	
	
	void ExchangeImage(sint32 layerIndex, sint32 imageIndex,
		const MBCHAR *imageName);

	
	
	virtual AUI_ERRCODE	Resize(sint32 width, sint32 height);

	virtual void SendKeyboardAction();
	virtual bool HandleKey(uint32 wParam);

	
	
	void SetStatusText(const MBCHAR *text);

	virtual sint32 KeyboardFocusIndex() { return m_focusIndex; }

protected:
	
	
	void InitializeLayerFlag(ldl_datablock *theBlock, sint32 layerIndex,
		const MBCHAR *flagString, sint32 flag,
		const MBCHAR *layerIndexString = NULL);

	
	void DrawLayers(aui_Surface *surface, RECT *rectangle);

	
	void SetCurrentRenderFlags(sint32 flags) { m_renderFlags = flags; }

	
	void AddRenderFlags(sint32 flags) { m_renderFlags |= flags; }

	
	void RemoveRenderFlags(sint32 flags) { m_renderFlags &= ~flags; }

	
	sint32 GetCurrentRenderFlags() const { return(m_renderFlags); }

	
	sint32 GetNumberOfLayers() const { return(m_numberOfLayers); }

	
	
	
	
	virtual void ResetCurrentRenderFlags();

	
	virtual void Highlight(bool status = true);

	
	
	virtual bool IgnoreHighlight() { return(false); }

private:
	
	static const sint32 k_AUI_CONTROL_LAYER_FLAG_ALWAYS;
	static const sint32 k_AUI_CONTROL_LAYER_FLAG_HIGHLIGHT;
	static const sint32 k_AUI_CONTROL_LAYER_FLAG_DISABLED;
	static const sint32 k_AUI_CONTROL_LAYER_FLAG_ENABLED;

	
	
	void InitializeLayerFlags(ldl_datablock *theBlock,
		sint32 layerIndex);

	
	
	bool AllocateImageLayers(ldl_datablock *theBlock);

	
	void InitializeFlagLayers(ldl_datablock *theBlock);

	
	void LoadLayerImages(ldl_datablock *theBlock,
		sint32 layerIndex);

	
	void LoadLayers(ldl_datablock *theBlock);

	
	
	typedef std::pair<sint32, sint32> FillSize;

	
	sint32 DesiredWidth(ldl_datablock *theBlock,
		sint32 layerIndex) const;

	
	
	FillSize WidthToFill(ldl_datablock *theBlock,
		sint32 layerIndex, sint32 imageStart,
		sint32 imageEnd, sint32 desiredWidth) const;

	
	
	bool FillWidth(ldl_datablock *theBlock,
		sint32 layerIndex, sint32 imageStart,
		sint32 imageEnd, const FillSize &desiredSize,
		sint32 &width);

	
	
	
	
	bool ResizeLayerWidth(ldl_datablock *theBlock,
		sint32 layerIndex, sint32 numberOfRows,
		sint32 *rowIndices, sint32 &width);

	
	sint32 DesiredHeight(ldl_datablock *theBlock,
		sint32 layerIndex) const;

	
	
	FillSize HeightToFill(ldl_datablock *theBlock,
		sint32 layerIndex, sint32 numberOfRows,
		sint32 *rowIndices, sint32 column,
		sint32 desiredHeight) const;

	
	
	bool FillHeight(ldl_datablock *theBlock,
		sint32 layerIndex, sint32 numberOfRows,
		sint32 *rowIndices, sint32 column,
		const FillSize &desiredSize,
		sint32 &height);

	
	
	
	
	bool ResizeLayerHeight(ldl_datablock *theBlock,
		sint32 layerIndex, sint32 numberOfRows,
		sint32 *rowIndices, sint32 &height);

	
	
	sint32 PreviousRowColumnIndex(sint32 numberOfRows, sint32 *rowIndices,
		sint32 row, sint32 column) const;

	
	
	sint32 RowColumnIndex(sint32 numberOfRows, sint32 *rowIndices,
		sint32 row, sint32 column) const;

	
	sint32 NumberOfColumns(sint32 numberOfRows,
		sint32 *rowIndices) const;

	
	
	
	
	sint32 SegmentImages(ldl_datablock *theBlock,
		sint32 layerIndex, sint32 *rowIndices) const;

	
	
	
	
	void ResizeLayers(ldl_datablock *theBlock);

	
	void BaseResetCurrentRenderFlags();

	
	void InitializeImageLayers(ldl_datablock *theBlock);

	
	sint32 m_numberOfLayers;

	
	sint32 m_imagesPerLayer;

	
	
	aui_ImageList *m_imageLayerList;

	
	sint32 *m_layerRenderFlags;

	
	sint32 m_renderFlags;

	
	sint32 m_focusIndex;

};


#endif 
