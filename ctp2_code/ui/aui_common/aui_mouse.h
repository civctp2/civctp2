//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Mouse User Interface
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Increased k_MOUSE_MAXNUMCURSORS to allow some additional cursors.
//   - April 30th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#ifndef __AUI_MOUSE_H__
#define __AUI_MOUSE_H__

#include "aui_base.h"
#include "aui_input.h"
#include "tech_wllist.h"

class aui_Cursor;
class aui_Surface;
class aui_Window;
class aui_Image;
class aui_DirtyList;
class ldl_datablock;

#define k_MOUSE_LDL_NUMCURSORS	"numcursors"
#define k_MOUSE_LDL_ANIM		"anim"
#define k_MOUSE_LDL_CURSOR		"cursor"
#define k_MOUSE_LDL_HOTSPOTX	"hotspotx"
#define k_MOUSE_LDL_HOTSPOTY	"hotspoty"
#define k_MOUSE_LDL_ANIMDELAY	"animdelay"
#define k_MOUSE_LDL_FIRSTINDEX	"firstindex"
#define k_MOUSE_LDL_LASTINDEX	"lastindex"

#define k_MOUSE_EVENT_FLAG_LSHIFT		0x00000001
#define k_MOUSE_EVENT_FLAG_RSHIFT		0x00000002
#define k_MOUSE_EVENT_FLAG_LCONTROL		0x00000004
#define k_MOUSE_EVENT_FLAG_RCONTROL		0x00000008

struct aui_MouseEvent
{
	POINT   position;
	BOOL    lbutton;
	BOOL    rbutton;
	BOOL    mbutton;
	BOOL    tbutton;
	BOOL    ubutton;
	BOOL    vbutton;
	BOOL    wbutton;
	BOOL    xbutton;
	uint32  time;
	sint32  movecount;
	sint32  framecount;

	uint32  flags;

	bool IsSet (uint32 flag) { return flags & flag; }

	bool IsSetLeftShift   () { return IsSet(k_MOUSE_EVENT_FLAG_LSHIFT); }
	bool IsSetRightShift  () { return IsSet(k_MOUSE_EVENT_FLAG_RSHIFT); }
	bool IsSetLeftControl () { return IsSet(k_MOUSE_EVENT_FLAG_LCONTROL); }
	bool IsSetRightControl() { return IsSet(k_MOUSE_EVENT_FLAG_RCONTROL); }
};

#define k_MOUSE_MAXINPUT			48

#define k_MOUSE_MAXSIZE				64

#define k_MOUSE_MAXNUMCURSORS		96

#define k_MOUSE_DEFAULTANIMDELAY	100

class aui_Mouse : public aui_Base, public virtual aui_Input
{
public:

	aui_Mouse(
		AUI_ERRCODE  *retval,
		const MBCHAR *ldlBlock );
	virtual ~aui_Mouse();

protected:
	aui_Mouse() {}
	AUI_ERRCODE InitCommonLdl( const MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );

	sint32 FindNumCursorsFromLdl( ldl_datablock *block );

public:

	virtual AUI_ERRCODE Start( void );
	virtual AUI_ERRCODE End( void );
	virtual AUI_ERRCODE Suspend( BOOL eraseCursor );
	virtual AUI_ERRCODE Resume( void );

	AUI_ERRCODE Show( void )
	{
		m_showCount++;
		return AUI_ERRCODE_OK;
	}
	AUI_ERRCODE Hide( void )
	{
		if ( !m_showCount )
		{

			Suspend( TRUE );
			m_showCount--;
			Resume();
		}
		return AUI_ERRCODE_OK;
	}

	BOOL IsSuspended( void ) const { return m_suspendCount;  }
	BOOL IsHidden   ( void ) const { return m_showCount < 0; }

	sint32	X( void ) { return m_data.position.x; }
	sint32	Y( void ) { return m_data.position.y; }

	void SetClip( sint32 left, sint32 top, sint32 right, sint32 bottom );
	void SetClip( RECT *clip );

	aui_MouseEvent *GetLatestMouseEvent( void ) { return &m_data; }

	AUI_ERRCODE SetPosition( sint32 x, sint32 y );
	AUI_ERRCODE SetPosition( POINT *point );

	AUI_ERRCODE	GetHotspot( sint32 *x, sint32 *y, sint32 index = 0 );
	AUI_ERRCODE	SetHotspot( sint32  x, sint32  y, sint32 index = 0 );

	double		&Sensitivity( void ) { return m_sensitivity; }

	aui_Cursor *GetCursor( sint32 index ) const { return m_cursors[ index ]; }
	void SetCursor( sint32 index, MBCHAR *cursor );

	aui_Cursor *GetCurrentCursor( void ) const { return *m_curCursor; }
	sint32		GetCurrentCursorIndex(void) ;
	void		SetCurrentCursor( sint32 index );

	uint32 GetAnimDelay( void ) const { return m_animDelay; }
	void SetAnimDelay( uint32 animDelay );

	void GetAnimIndexes( sint32 *firstIndex, sint32 *lastIndex )
	{
		if ( firstIndex ) *firstIndex = m_firstIndex;
		if (  lastIndex ) * lastIndex =  m_lastIndex;
	}
	virtual void SetAnimIndexes( sint32 firstIndex, sint32 lastIndex );

	void SetAnim( sint32 anim );

	virtual AUI_ERRCODE ReactToInput( void ) = 0;

	virtual sint32 ManipulateInputs( aui_MouseEvent *data, BOOL add ) = 0;

	virtual AUI_ERRCODE HandleAnim( void );

	virtual AUI_ERRCODE	BltWindowToPrimary( aui_Window *window ) = 0;
	virtual AUI_ERRCODE BltDirtyRectInfoToPrimary( void ) = 0;
	virtual AUI_ERRCODE	BltBackgroundColorToPrimary(
		COLORREF color,
		aui_DirtyList *colorAreas ) = 0;
	virtual AUI_ERRCODE	BltBackgroundImageToPrimary(
		aui_Image *image,
		RECT *imageRect,
		aui_DirtyList *imageAreas ) = 0;

	uint32 GetFlags(void) { return m_flags; }
	void SetFlags(uint32 flags) { m_flags = flags; }

	void   SetFlag(uint32 flag) { m_flags |=  flag; }
	void UnsetFlag(uint32 flag) { m_flags &= ~flag; }

	void   SetLeftShift   () {   SetFlag(k_MOUSE_EVENT_FLAG_LSHIFT); }
	void   SetRightShift  () {   SetFlag(k_MOUSE_EVENT_FLAG_RSHIFT); }
	void   SetLeftControl () {   SetFlag(k_MOUSE_EVENT_FLAG_LCONTROL); }
	void   SetRightControl() {   SetFlag(k_MOUSE_EVENT_FLAG_RCONTROL); }

	void UnsetLeftShift   () { UnsetFlag(k_MOUSE_EVENT_FLAG_LSHIFT); }
	void UnsetRightShift  () { UnsetFlag(k_MOUSE_EVENT_FLAG_RSHIFT); }
	void UnsetLeftControl () { UnsetFlag(k_MOUSE_EVENT_FLAG_LCONTROL); }
	void UnsetRightControl() { UnsetFlag(k_MOUSE_EVENT_FLAG_RCONTROL); }

	bool IsSet (uint32 flag) { return m_flags & flag; }

	bool IsSetLeftShift   () { return IsSet(k_MOUSE_EVENT_FLAG_LSHIFT); }
	bool IsSetRightShift  () { return IsSet(k_MOUSE_EVENT_FLAG_RSHIFT); }
	bool IsSetLeftControl () { return IsSet(k_MOUSE_EVENT_FLAG_LCONTROL); }
	bool IsSetRightControl() { return IsSet(k_MOUSE_EVENT_FLAG_RCONTROL); }

protected:
	static sint32 m_mouseRefCount;
	virtual void ActivateCursor(aui_Cursor *cursor) {}

	aui_MouseEvent	m_data;
	double			m_sensitivity;

	aui_MouseEvent	m_inputs[ k_MOUSE_MAXINPUT ];

	RECT		m_clip;
	aui_Cursor	*m_cursors[ k_MOUSE_MAXNUMCURSORS ];
	aui_Cursor	**m_curCursor;
	sint32		m_firstIndex;
	sint32		m_lastIndex;
	uint32		m_animDelay;
	uint32		m_time;

	tech_WLList<POINT>	m_animIndexList;
	tech_WLList<sint32> m_animDelayList;

	sint32		m_suspendCount;
	sint32		m_showCount;
	BOOL		m_reset;
	uint32		m_flags;
};

#endif
