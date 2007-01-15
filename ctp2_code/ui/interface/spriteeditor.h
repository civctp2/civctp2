




#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef SPRITEEDITOR_H__
#define SPRITEEDITOR_H__

class SpriteEditWindow;

#include "c3window.h"       // C3Window
#include "ctp2_inttypes.h"  // sintNN, uintNN
#include "MapPoint.h"       // MapPoint
// AUI_ERRCODE, AUI_WINDOW_TYPE
// BOOL, MBCHAR, RECT

class Action;
class Anim;
class aui_Surface;
class c3_Static;
class C3TextField;
class ctp2_Button;
class Sprite;
class UnitSpriteGroup;


class SpriteEditWindow : public C3Window
{
public:
	
	
	
	SpriteEditWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_BACKGROUND );
	
	
	
	
	SpriteEditWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_BACKGROUND );

	
	
	
	virtual ~SpriteEditWindow();

	
	
	
	virtual AUI_ERRCODE InitCommon(void);
	virtual AUI_ERRCODE InitCommonLdl(MBCHAR *ldlBlock);

	
	
	
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	
	
	
	virtual AUI_ERRCODE	Idle( void );

	
	
	
	void	InitializeControls(AUI_ERRCODE *errcode,MBCHAR *windowsBlock);
	void	TopLevel();

	
	void			LoadSprite(char *name=NULL);

	
	void			SaveSprite(char *name=NULL);

	
	void			DrawSprite();
	void			ReDrawLargeSprite();

	
	void	AddFrame	(sint32 add);
	void	SetFrame	(sint32 frame);
	void	AddFacing	(sint32 add);
	void	SetFacing	(sint32 facing);
	void	SetAnimation(sint32 anim);
	void	Animate();
	void	BeginAnimation();

	
	bool	FileExists(char *name);

	
	bool			m_loopInProgress;
	bool			m_stopAfterLoop;

	sint32			m_drawFlag; 
	sint32			m_facing;   
	sint32			m_frame;    
	sint32			m_animation;

	sint32			m_lastTime;	
private:

	sint32	m_dest;
	sint32	m_current;
	BOOL	m_scroll;
	sint32	m_mouseChangeY;
	sint32	m_drawX,m_drawY;

	
	
	
	UnitSpriteGroup		*m_currentSprite;
	Anim				*m_currentAnim;
	Sprite				*m_spriteData;
	aui_Surface			*m_spriteSurface;
	RECT				m_spriteRect;
	
	
	Action				*m_actionObj;


	
	
	
	
	ctp2_Button		*m_Load;	 
	ctp2_Button		*m_Save;	 

	
	C3TextField		*m_fileName;

	
	ctp2_Button		*m_MOVEAnim;  
	ctp2_Button		*m_ATTACKAnim;
	ctp2_Button		*m_IDLEAnim;  
	ctp2_Button		*m_VICTORYAnim;
	ctp2_Button		*m_WORKAnim;  

	
	ctp2_Button		*m_stepPlus;
	ctp2_Button		*m_stepMinus;
	ctp2_Button		*m_playOnce;
	ctp2_Button		*m_playLoop;
	ctp2_Button		*m_facingPlus;
	ctp2_Button		*m_facingMinus;

	
	C3Window		*m_largeImage;
	aui_Surface		*m_largeSurface;
	RECT			m_largeRect;
	RECT			m_largeRectAbs;
	
	
	c3_Static		*m_hotCoordsCurrent;
	c3_Static		*m_hotCoordsMouse;
	c3_Static       *m_hotCoordsHerald;

	
	float			m_widthRatio;
	float			m_heightRatio;
	float			m_oneOverWidthRatio;
	float			m_oneOverHeightRatio;

	MapPoint        m_drawPoint;

};




int SpriteEditWindow_Initialize( void );
void SpriteEditWindow_Cleanup(void);
#endif