










#ifndef __AUI_HYPERTEXTBOX_H__
#define __AUI_HYPERTEXTBOX_H__


#include "aui_hypertextbase.h"
#include "aui_control.h"


class aui_Ranger;



#define k_AUI_HYPERTEXTBOX_LDL_RANGERY			"rangery"
#define k_AUI_HYPERTEXTBOX_LDL_ALWAYSRANGER		"alwaysranger"



enum AUI_HYPERTEXTBOX_ACTION
{
	AUI_HYPERTEXTBOX_ACTION_FIRST = 0,
	AUI_HYPERTEXTBOX_ACTION_NULL = 0,
	AUI_HYPERTEXTBOX_ACTION_LAST
};



class aui_HyperTextBox : public aui_Control, public aui_HyperTextBase
{
public:
	
	aui_HyperTextBox(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_HyperTextBox(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~aui_HyperTextBox();

protected:
	aui_HyperTextBox() : aui_Control(), aui_HyperTextBase() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE CreateRanger( MBCHAR *ldlBlock = NULL );

public:
	virtual AUI_ERRCODE	Resize( sint32 width, sint32 height );

	virtual AUI_ERRCODE Show( void );

	aui_Ranger *GetRanger( void ) const { return m_ranger; }

	sint32		GetRangerSize( void ) const { return m_rangerSize; }
	AUI_ERRCODE	SetRangerSize( sint32 rangerSize )
		{ m_rangerSize = rangerSize; return RepositionRanger(); }
	void		SetAlwaysRanger( BOOL always ) { m_alwaysRanger = always; }

	
	AUI_ERRCODE	RangerMoved( void );

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	virtual AUI_ERRCODE	SetHyperText( const MBCHAR *hyperText );
	virtual AUI_ERRCODE	AppendHyperText( const MBCHAR *hyperText );

	sint32 GetVirtualHeight( void ) const { return m_virtualHeight; }

protected:
	virtual AUI_ERRCODE AddHyperStatics( const MBCHAR *hyperText );

	AUI_ERRCODE	RepositionRanger( void );

	aui_Ranger	*m_ranger;
	sint32		m_rangerSize;
	BOOL		m_alwaysRanger;

	sint32		m_virtualHeight;
	POINT		m_curStaticPos;
};



aui_Control::ControlActionCallback HyperTextBoxRangerActionCallback;


#endif 
