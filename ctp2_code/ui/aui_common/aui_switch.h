//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface switch
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

#ifndef __AUI_SWITCH_H__
#define __AUI_SWITCH_H__

#include "aui_control.h"


enum AUI_SWITCH_ACTION
{
	AUI_SWITCH_ACTION_FIRST = 0,
	AUI_SWITCH_ACTION_NULL = 0,
	AUI_SWITCH_ACTION_PRESS,
	AUI_SWITCH_ACTION_RELEASE,
	AUI_SWITCH_ACTION_ON,
	AUI_SWITCH_ACTION_OFF,
	AUI_SWITCH_ACTION_DOUBLECLICK,
	AUI_SWITCH_ACTION_LAST
};


#define k_AUI_SWITCH_LDL_STATE		"state"
#define k_AUI_SWITCH_LDL_NUMSTATES	"numstates"

#define k_AUI_SWITCH_DEFAULTNUMSTATES	2


class aui_Switch : public aui_Control
{
public:

	aui_Switch(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		ControlActionCallback *StateFunc = NULL,
		void *cookie = NULL );
	aui_Switch(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		ControlActionCallback *StateFunc = NULL,
		void *cookie = NULL,
		sint32 state = 0,
		sint32 numStates = k_AUI_SWITCH_DEFAULTNUMSTATES );
	virtual ~aui_Switch() {}

protected:
	aui_Switch() : aui_Control () {}
	AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);
	AUI_ERRCODE InitCommon( sint32 state, sint32 numStates );

public:
	virtual AUI_ERRCODE ResetThis( void );

	BOOL			IsSelected( void ) const { return m_state != 0; }
	sint32			GetState( void ) const { return m_state; }
	virtual sint32	SetState( sint32 state );

	sint32			SetNumStates( sint32 numStates );

	sint32			ExtractState( uint32 data ) { return (sint32)data; }

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	typedef AUI_ERRCODE (SwitchDrawCallback)(
		aui_Switch *control,
		aui_Surface *surface,
		RECT &rect,
		void *cookie );
	void SetDrawCallbackAndCookie(SwitchDrawCallback *func,
		void *cookie,bool exclusive=true);

protected:
	sint32	m_state;
	sint32	m_numStates;

	bool	m_drawCallbackExclusive;
	SwitchDrawCallback *m_drawFunc;
	void *m_drawCookie;

	virtual void	MouseLDragOver(aui_MouseEvent * mouseData);
	virtual void	MouseLDragAway(aui_MouseEvent * mouseData);

	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDropInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDropOutside(aui_MouseEvent * mouseData);

	virtual void	MouseRDropInside(aui_MouseEvent * mouseData);
};

#endif
