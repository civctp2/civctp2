//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface switch group (radio button?)
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

#ifndef __AUI_SWITCHGROUP_H__
#define __AUI_SWITCHGROUP_H__

#include "aui_control.h"

class aui_Switch;


class aui_SwitchGroup : public aui_Control
{
public:

	aui_SwitchGroup(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock );
	aui_SwitchGroup(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height );
	virtual ~aui_SwitchGroup() {}

protected:
	aui_SwitchGroup() : aui_Control() {}
	AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);
	AUI_ERRCODE InitCommon( void );

public:

	uint32	WhichIsSelected( void ) const { return m_whichIsSelected; }
	sint32	WhichIndexIsSelected( void );

	AUI_ERRCODE	AddSwitch( aui_Switch *theSwitch ) { return AddChild( (aui_Region *)theSwitch ); }
	AUI_ERRCODE RemoveSwitch( uint32 switchId ) { return RemoveChild( switchId ); }
	aui_Switch	*GetSwitch( uint32 switchId ) { return (aui_Switch *)GetChild( switchId ); }
	virtual AUI_ERRCODE	AddChild( aui_Region *child );
	virtual AUI_ERRCODE	RemoveChild( uint32 switchId );

protected:

	BOOL	MakeSureOnlyOneIsSelected( void );

	uint32	m_whichWasSelected;
	uint32	m_whichIsSelected;

	virtual void	PreChildrenCallback(aui_MouseEvent * mouseData);
	virtual void	PostChildrenCallback(aui_MouseEvent * mouseData);
};

#endif
