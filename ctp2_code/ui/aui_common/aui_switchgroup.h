










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
		MBCHAR *ldlBlock );
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
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
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

	
	
	

	virtual MouseEventCallback PreChildrenCallback;
	virtual MouseEventCallback PostChildrenCallback;
};


#endif 
