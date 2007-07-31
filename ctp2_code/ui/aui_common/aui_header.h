










#ifndef __AUI_HEADER_H__
#define __AUI_HEADER_H__


#include "aui_switchgroup.h"
#include "aui_action.h"



#define k_AUI_HEADER_LDL_SWITCH		"switch"



class aui_Header : public aui_SwitchGroup
{
public:
	
	aui_Header(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock );
	aui_Header(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height );
	virtual ~aui_Header();

protected:
	aui_Header() : aui_SwitchGroup() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE CreateSwitches( MBCHAR *ldlBlock = NULL );

public:
	virtual AUI_ERRCODE	AddChild( aui_Region *child );
	virtual AUI_ERRCODE	RemoveChild( uint32 switchId );

protected:
	AUI_ERRCODE	CalculateDimensions( void );
	AUI_ERRCODE	RepositionSwitches( void );
};



class aui_HeaderSwitchAction : public aui_Action
{
public:
	aui_HeaderSwitchAction(sint32 column) 
    :   aui_Action  (),
        m_column    (column) 
    { ; };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

protected:
	sint32 m_column;
};


#endif 
