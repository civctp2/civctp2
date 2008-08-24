//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface ranged input handler
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
//
//----------------------------------------------------------------------------

#ifndef __AUI_RANGER_H__
#define __AUI_RANGER_H__


#include "aui_control.h"


class aui_Control;
class aui_Button;
class aui_Thumb;
class aui_Surface;



enum AUI_RANGER_TYPE
{
	AUI_RANGER_TYPE_FIRST = 0,
	AUI_RANGER_TYPE_SLIDER = 0,
	AUI_RANGER_TYPE_SCROLLER,
	AUI_RANGER_TYPE_SPINNER,
	AUI_RANGER_TYPE_LAST
};



enum AUI_RANGER_ORIENTATION
{
	AUI_RANGER_ORIENTATION_FIRST = 0,
	AUI_RANGER_ORIENTATION_VERTICAL = 0,
	AUI_RANGER_ORIENTATION_HORIZONTAL,
	AUI_RANGER_ORIENTATION_BIDIRECTIONAL,
	AUI_RANGER_ORIENTATION_LAST
};



enum AUI_RANGER_ACTION
{
	AUI_RANGER_ACTION_FIRST = 0,
	AUI_RANGER_ACTION_NULL = 0,
	AUI_RANGER_ACTION_VALUECHANGE,
	AUI_RANGER_ACTION_RELEASE,
	AUI_RANGER_ACTION_LAST
};



#define k_AUI_RANGER_DEFAULTMINX	0
#define k_AUI_RANGER_DEFAULTMAXX	100
#define k_AUI_RANGER_DEFAULTMINY	0
#define k_AUI_RANGER_DEFAULTMAXY	100
#define k_AUI_RANGER_DEFAULTINCX	1
#define k_AUI_RANGER_DEFAULTINCY	1
#define k_AUI_RANGER_DEFAULTPAGEX	10
#define k_AUI_RANGER_DEFAULTPAGEY	10
#define k_AUI_RANGER_DEFAULTVALX	0
#define k_AUI_RANGER_DEFAULTVALY	0




#define k_AUI_RANGER_LDL_TYPE			"type"
#define k_AUI_RANGER_LDL_SCROLLER		"scroller"
#define k_AUI_RANGER_LDL_SLIDER			"slider"
#define k_AUI_RANGER_LDL_SPINNER		"spinner"


#define k_AUI_RANGER_LDL_ORIENTATION	"orientation"
#define k_AUI_RANGER_LDL_VERTICAL		"vertical"
#define k_AUI_RANGER_LDL_HORIZONTAL		"horizontal"
#define k_AUI_RANGER_LDL_BIDIRECTIONAL	"bidirectional"


#define k_AUI_RANGER_LDL_THUMB			"thumb"
#define k_AUI_RANGER_LDL_INCX			"incx"
#define k_AUI_RANGER_LDL_INCY			"incy"
#define k_AUI_RANGER_LDL_DECX			"decx"
#define k_AUI_RANGER_LDL_DECY			"decy"


#define k_AUI_RANGER_LDL_MINXVAL		"minxval"
#define k_AUI_RANGER_LDL_MAXXVAL		"maxxval"
#define k_AUI_RANGER_LDL_MINYVAL		"minyval"
#define k_AUI_RANGER_LDL_MAXYVAL		"maxyval"
#define k_AUI_RANGER_LDL_INCXVAL		"incxval"
#define k_AUI_RANGER_LDL_INCYVAL		"incyval"
#define k_AUI_RANGER_LDL_PAGEXVAL		"pagexval"
#define k_AUI_RANGER_LDL_PAGEYVAL		"pageyval"
#define k_AUI_RANGER_LDL_XVAL			"xval"
#define k_AUI_RANGER_LDL_YVAL			"yval"
#define k_AUI_RANGER_LDL_MINTHUMBSIZE	"minthumbsize"


#define k_AUI_RANGER_LDL_RIGIDTHUMB		"rigidthumb"
#define k_AUI_RANGER_LDL_QUANTIZEDRAG	"quantizedrag"



class aui_Ranger : public aui_Control
{
public:
	
	aui_Ranger(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_Ranger(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		AUI_RANGER_TYPE type = AUI_RANGER_TYPE_SCROLLER,
		AUI_RANGER_ORIENTATION orientation = AUI_RANGER_ORIENTATION_BIDIRECTIONAL,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~aui_Ranger();

protected:
	aui_Ranger() : aui_Control() {}
	AUI_ERRCODE	InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon(
		AUI_RANGER_TYPE type,
		AUI_RANGER_ORIENTATION orientation,
		sint32 minx = k_AUI_RANGER_DEFAULTMINX,
		sint32 maxx = k_AUI_RANGER_DEFAULTMAXX,
		sint32 miny = k_AUI_RANGER_DEFAULTMINY,
		sint32 maxy = k_AUI_RANGER_DEFAULTMAXY,
		sint32 incx = k_AUI_RANGER_DEFAULTINCX,
		sint32 incy = k_AUI_RANGER_DEFAULTINCY,
		sint32 pagex = k_AUI_RANGER_DEFAULTPAGEX,
		sint32 pagey = k_AUI_RANGER_DEFAULTPAGEY,
		sint32 valx = k_AUI_RANGER_DEFAULTVALX,
		sint32 valy = k_AUI_RANGER_DEFAULTVALY,
		BOOL rigidThumb = FALSE,
		BOOL quantizeDrag = FALSE,
		sint32 minThumbSize = k_CONTROL_DEFAULT_SIZE );

public:
	virtual AUI_ERRCODE	Resize( sint32 width, sint32 height );

	aui_Thumb	*GetThumb( void ) const { return m_thumb; }
	aui_Button	*GetIncrementXButton( void ) const { return m_incXButton; }
	aui_Button	*GetIncrementYButton( void ) const { return m_incYButton; }
	aui_Button	*GetDecrementXButton( void ) const { return m_decXButton; }
	aui_Button	*GetDecrementYButton( void ) const { return m_decYButton; }

	sint32		GetValueX( void ) const { return m_valX; }
	sint32		GetValueY( void ) const { return m_valY; }
	sint32		GetMinimumX( void ) const { return m_minX; }
	sint32		GetMinimumY( void ) const { return m_minY; }
	sint32		GetMaximumX( void ) const { return m_maxX; }
	sint32		GetMaximumY( void ) const { return m_maxY; }
	sint32		GetIncrementX( void ) const { return m_incX; }
	sint32		GetIncrementY( void ) const { return m_incY; }
	sint32		GetPageX( void ) const { return m_pageX; }
	sint32		GetPageY( void ) const { return m_pageY; }

	AUI_ERRCODE	SetValue( sint32 valX, sint32 valY );
	AUI_ERRCODE	SetMinimum( sint32 minX, sint32 minY );
	AUI_ERRCODE	SetMaximum( sint32 maxX, sint32 maxY );
	AUI_ERRCODE	SetIncrement( sint32 incX, sint32 incY );
	AUI_ERRCODE	SetPage( sint32 pageX, sint32 pageY );

	AUI_ERRCODE	IncrementUpX( void );
	AUI_ERRCODE	IncrementUpY( void );
	AUI_ERRCODE	IncrementDownX( void );
	AUI_ERRCODE	IncrementDownY( void );

	AUI_ERRCODE	PageUpX( void );
	AUI_ERRCODE	PageUpY( void );
	AUI_ERRCODE	PageDownX( void );
	AUI_ERRCODE	PageDownY( void );

	sint32		GetMinThumbSize( void ) const { return m_minThumbSize; }
	AUI_ERRCODE	SetMinThumbSize( sint32 minThumbSize )
		{ m_minThumbSize = minThumbSize; return RepositionThumb(); }

	sint32		GetButtonSize( void ) const { return m_buttonSize; }
	AUI_ERRCODE	SetButtonSize( sint32 buttonSize )
		{ m_buttonSize = buttonSize; return RepositionButtons(); }

	AUI_RANGER_TYPE GetType( void ) const { return m_type; }

	
	AUI_ERRCODE	ThumbMoved( BOOL forceQuantize = FALSE );

	BOOL		UsingRigidThumb( void ) const { return m_rigidThumb; }
	BOOL		UsingQuantizedDragging( void ) const { return m_quantized; }

	AUI_ERRCODE	UseRigidThumb( BOOL rigidThumb )
		{ m_rigidThumb = rigidThumb; return RepositionThumb(); }
	AUI_ERRCODE	UseQuantizedDragging( BOOL quantized )
		{ m_quantized = quantized; return AUI_ERRCODE_OK; }

	virtual AUI_ERRCODE Show( void );



	void ForceScroll(sint32 deltaX, sint32 deltaY);

	static aui_Ranger *ms_mouseFocusRanger;

	static aui_Ranger *GetMouseFocusRanger(void) { return ms_mouseFocusRanger; }
	static void SetMouseFocusRanger(aui_Ranger *box) { ms_mouseFocusRanger = box; }



protected:
	AUI_ERRCODE	ValueToPosition( sint32 *x, sint32 *y );
	AUI_ERRCODE	PositionToValue( sint32 *x, sint32 *y );
	AUI_ERRCODE	CenterPositionToValue( sint32 *x, sint32 *y );

	AUI_ERRCODE	SlideThumb( void );

	AUI_ERRCODE	RepositionThumb( BOOL callActionFunc = TRUE );

	
	virtual AUI_ERRCODE	RepositionButtons();

	
	aui_Button *CreateArrowButton(const MBCHAR *ldlBlock,
		const MBCHAR *autoLdlName, const MBCHAR *ldlName);

	
	AUI_ERRCODE CreateButtonsAndThumb(MBCHAR *ldlBlock = NULL);

	
	AUI_RANGER_TYPE			m_type;			
	AUI_RANGER_ORIENTATION	m_orientation;	

	
	BOOL		m_quantized;	
	BOOL		m_rigidThumb;	
	sint32		m_minThumbSize;	
	sint32		m_buttonSize;	

	
	aui_Control	*m_rangeContainer;

	
	aui_Button	*m_incXButton;	
	aui_Button	*m_incYButton;	
	aui_Button	*m_decXButton;	
	aui_Button	*m_decYButton;	
	aui_Thumb	*m_thumb;		
	sint32		m_overlap;		

	sint32		m_valX;			
	sint32		m_valY;			
	sint32		m_minX;			
	sint32		m_minY;			
	sint32		m_maxX;			
	sint32		m_maxY;			
	sint32		m_incX;			
	sint32		m_incY;			
	sint32		m_pageX;		
	sint32		m_pageY;		

	BOOL		m_sliding;
	sint32		m_slideDx;
	sint32		m_slideDy;

	virtual void	PostChildrenCallback(aui_MouseEvent * mouseData);

	virtual void	MouseMoveOver(aui_MouseEvent * mouseData);
	virtual void	MouseMoveInside(aui_MouseEvent * mouseData);

	virtual void	MouseLDragOver(aui_MouseEvent * mouseData);
	virtual void	MouseLDragAway(aui_MouseEvent * mouseData);

	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDropInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDropOutside(aui_MouseEvent * mouseData);
	virtual void	MouseRGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseRDropInside(aui_MouseEvent * mouseData);
};



aui_Control::ControlActionCallback RangerThumbActionCallback;
aui_Control::ControlActionCallback RangerButtonActionCallback;


#endif 
