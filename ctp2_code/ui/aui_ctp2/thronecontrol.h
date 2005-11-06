//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface ?
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

#ifndef __THRONECONTROL_H__
#define __THRONECONTROL_H__

#include "patternbase.h"
#include "aui_control.h"

#define k_THRONE_LDL_BLENDSPEED		"blendspeed"

#define k_THRONE_LDL_BACKGROUND		"background"
#define k_THRONE_LDL_CASTLE			"castle"
#define k_THRONE_LDL_ENTERTAINMENT	"entertainment"
#define k_THRONE_LDL_FOUNTAIN		"fountain"
#define k_THRONE_LDL_STATUE			"statue"
#define k_THRONE_LDL_RELIGIOUS		"religious"

#define k_THRONE_LDL_MESSAGE		"message"

#define k_THRONE_IMAGES			5
#define k_THRONE_BLENDVAL_MAX	32

#define k_THRONE_HIGHLIGHT_IMAGE	64




enum C3_THRONE_ACTION {
	C3_THRONE_ACTION_NULL,
	C3_THRONE_ACTION_SELECTEDIMAGE,
	C3_THRONE_ACTION_HIGHLIGHTIMAGE,
	C3_THRONE_ACTION_UNHIGHLIGHTIMAGE,

	C3_THUMBNAIL_ACTION_MAX
};

class aui_Surface;
class c3_Image;
class c3_Static;
class c3_Button;


class ThroneControl : public aui_Control, public PatternBase
{
public:
	
	ThroneControl(
		AUI_ERRCODE *retval, 
		sint32 id, 
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL);
	ThroneControl(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~ThroneControl();

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	virtual void	MouseLDropInside(aui_MouseEvent * mouseData);
	virtual void	MouseMoveInside(aui_MouseEvent * mouseData);

	AUI_ERRCODE Idle( void );

	sint32		GetSelectedImage( void ) const { return m_selectedImage; }
	void		SetSelectedImage( sint32 selectedImage ) { m_selectedImage = selectedImage; }

	sint32		GetHilightedImage( void ) const { return m_hilightedImage; }
	void		SetHilightedImage( sint32 hilightedImage ) { m_hilightedImage = hilightedImage; }

	aui_Surface *GetThroneSurface( void ) const { return m_throneSurface; }

	sint32		RenderThrone( aui_Surface *surf );
	sint32		RepaintFrames( aui_Surface *surf );

	
	void		UpdateThrone(aui_Surface *surf, sint32 x, sint32 y);

	void		SetDrawOrder( sint8 a, sint8 b, sint8 c, sint8 d, sint8 e );

	aui_Surface *InitializeNewBG( MBCHAR *filename );
	void		CrossFadeImage( MBCHAR *filename );

	void		DisplayZoomedImage( MBCHAR *filename );

	sint32		GetBlendSpeed( void ) const { return m_blendSpeed; }
	void		SetBlendSpeed( sint32 blendSpeed ) { m_blendSpeed = blendSpeed; }

	void		HilightImage( sint32 index );
	void		UnhilightImage( sint32 index );

	void		DisplayHilightedText( void );

	void		SetZoomedMode( BOOL on );

protected:
	ThroneControl() : aui_Control() {}
	void InitCommonLdl( MBCHAR *ldlBlock );
	void InitCommon( void  );

private:
	aui_Surface	*m_throneSurface;

	c3_Image	*m_background;
	c3_Image	*m_upgradeImage[k_THRONE_IMAGES];

	c3_Button	*m_zoomedImage;
	BOOL		m_isZoomed;

	RECT		m_upgradeRect[k_THRONE_IMAGES];
	sint8		m_drawOrder[k_THRONE_IMAGES];

	c3_Static	*m_messageText;
	c3_Static	*m_currentText[ k_THRONE_IMAGES ];
	c3_Static	*m_upgradeText[ k_THRONE_IMAGES ]; 

	sint32		m_selectedImage;
	sint32		m_hilightedImage;
	sint32		m_wasHilighted;

	
	aui_Surface *m_oldCutout;
	aui_Surface *m_newCutout;

	BOOL		m_isCrossFade;
	sint32		m_blendVal;
	sint32		m_blendSpeed;		
};


#endif 
