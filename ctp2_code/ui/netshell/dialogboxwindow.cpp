//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer dialog box window
// Id           : $Id$
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
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "dialogboxwindow.h"

#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_static.h"
#include "aui_progressbar.h"
#include "aui_screen.h"
#include "aui_button.h"

#include "netshell.h"
#include "ns_chatbox.h"
#include "ns_customlistbox.h"

#include "c3_button.h"

#include "spnewgamewindow.h"

#include "ldl_file.hpp"

DialogBoxWindow::DialogBoxWindow(
	AUI_ERRCODE *retval,
	MBCHAR *ldlBlock,
	aui_Action **actions )
:
	ns_Window	(retval,
				 aui_UniqueId(),
				 ldlBlock,
				 0,
				 AUI_WINDOW_TYPE_FLOATING 
				),
	m_numButtons	(0),
	m_buttons		(NULL) 
{
	if ( !AUI_SUCCESS(*retval) ) return;
	*retval = InitCommon();
	if ( !AUI_SUCCESS(*retval) ) return;
	*retval = CreateControls( ldlBlock, actions );
}


AUI_ERRCODE DialogBoxWindow::InitCommon( void )
{
	m_controls = new aui_Control *[ m_numControls = CONTROL_MAX ];
	Assert( m_controls != NULL );
	if ( !m_controls ) return AUI_ERRCODE_MEMALLOCFAILED;
	memset( m_controls, 0, m_numControls * sizeof( aui_Control *) );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE DialogBoxWindow::CreateControls(
	MBCHAR *ldlBlock,
	aui_Action **actions )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	Assert( ldlBlock != NULL );
	if ( !ldlBlock ) return AUI_ERRCODE_INVALIDPARAM;

	aui_Control *control;
	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	sprintf( block, "%s.titlestatictext", ldlBlock );
	if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
	{
		control = new aui_Static(
			&errcode,
			aui_UniqueId(),
			block );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
	}
	else
		control = NULL;
	m_controls[ CONTROL_TITLESTATICTEXT ] = control;

	sprintf( block, "%s.titlebox", ldlBlock );
    if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
	{
		control = new aui_Static(
			&errcode,
			aui_UniqueId(),
			block );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
	}
	else
		control = NULL;
	m_controls[ CONTROL_TITLEBOX ] = control;

	sprintf( block, "%s.lefttopcorner", ldlBlock );
    if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
	{
		control = new aui_Static(
			&errcode,
			aui_UniqueId(),
			block );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
	}
	else
		control = NULL;
	m_controls[ CONTROL_LEFTTOPCORNER ] = control;

	sprintf( block, "%s.righttopcorner", ldlBlock );
    if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
	{
		control = new aui_Static(
			&errcode,
			aui_UniqueId(),
			block );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
	}
	else
		control = NULL;
	m_controls[ CONTROL_RIGHTTOPCORNER ] = control;

	sprintf( block, "%s.leftbottomcorner", ldlBlock );
    if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
	{
		control = new aui_Static(
			&errcode,
			aui_UniqueId(),
			block );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
	}
	else
		control = NULL;
	m_controls[ CONTROL_LEFTBOTTOMCORNER ] = control;

	sprintf( block, "%s.rightbottomcorner", ldlBlock );
    if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
	{
		control = new aui_Static(
			&errcode,
			aui_UniqueId(),
			block );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
	}
	else
		control = NULL;
	m_controls[ CONTROL_RIGHTBOTTOMCORNER ] = control;

	sprintf( block, "%s.leftedge", ldlBlock );
    if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
	{
		control = new aui_Static(
			&errcode,
			aui_UniqueId(),
			block );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
	}
	else
		control = NULL;
	m_controls[ CONTROL_LEFTEDGE ] = control;

	sprintf( block, "%s.rightedge", ldlBlock );
    if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
	{
		control = new aui_Static(
			&errcode,
			aui_UniqueId(),
			block );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
	}
	else
		control = NULL;
	m_controls[ CONTROL_RIGHTEDGE ] = control;

	sprintf( block, "%s.topedge", ldlBlock );
    if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
	{
		control = new aui_Static(
			&errcode,
			aui_UniqueId(),
			block );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
	}
	else
		control = NULL;
	m_controls[ CONTROL_TOPEDGE ] = control;

	sprintf( block, "%s.bottomedge", ldlBlock );
    if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
	{
		control = new aui_Static(
			&errcode,
			aui_UniqueId(),
			block );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
	}
	else
		control = NULL;
	m_controls[ CONTROL_BOTTOMEDGE ] = control;

	sprintf( block, "%s.descriptionstatictext", ldlBlock );
    if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
	{
		control = new aui_Static(
			&errcode,
			aui_UniqueId(),
			block );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
	}
	else
		control = NULL;
	m_controls[ CONTROL_DESCRIPTIONSTATICTEXT ] = control;

	sprintf( block, "%s.progressbar", ldlBlock );
    if (aui_Ldl::GetLdl()->FindDataBlock( block ) )
	{
		control = new aui_ProgressBar(
			&errcode,
			aui_UniqueId(),
			block );
		Assert( AUI_NEWOK(control,errcode) );
		if ( !AUI_NEWOK(control,errcode) ) return errcode;
	}
	else
		control = NULL;
	m_controls[ CONTROL_PROGRESSBAR ] = control;

	
	do
	{
		sprintf( block, "%s.button%d", ldlBlock, m_numButtons );

        if ( !aui_Ldl::GetLdl()->FindDataBlock( block ) )
			break;

		m_numButtons++;

	} while ( 1 );

	if ( m_numButtons )
	{
		m_buttons = new aui_Button *[ m_numButtons ];
		Assert( m_buttons != NULL );
		if ( !m_buttons ) return AUI_ERRCODE_MEMALLOCFAILED;

		
		memset( m_buttons, 0, m_numButtons * sizeof( aui_Button * ) );

		for ( sint32 i = 0; i < m_numButtons; i++ )
		{

			sprintf( block, "button%d", i );

			m_buttons[ i ] = spNew_ctp2_Button(
				&errcode,
				ldlBlock,
				block, 
				NULL);

			Assert( AUI_NEWOK(m_buttons[i],errcode) );
			if ( !AUI_NEWOK(m_buttons[i],errcode) )
				return AUI_ERRCODE_MEMALLOCFAILED;

			if ( actions )
				m_buttons[ i ]->SetAction( actions[ i ] );
		}
	}

	aui_Ldl::SetupHeirarchyFromRoot( ldlBlock );

	SetStronglyModal( TRUE );

	return AUI_ERRCODE_OK;
}


DialogBoxWindow::~DialogBoxWindow()
{
	if (m_buttons)
	{
		for (sint32 i = 0; i < m_numButtons; ++i)
		{
			delete m_buttons[i];
		}
		delete[] m_buttons;
	}
}


DialogBoxWindow *DialogBoxWindow::PopUp(
	MBCHAR *ldlBlock,
	aui_Action **actions )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	DialogBoxWindow *dbw = new DialogBoxWindow(
		&errcode,
		ldlBlock,
		actions );
	Assert( AUI_NEWOK(dbw,errcode) );
	if ( !AUI_NEWOK(dbw,errcode) )
	{
		delete dbw;
		dbw = NULL;
	}

	aui_Screen *screen = g_netshell->GetCurrentScreen();
	if ( screen )
		screen->AddWindow( dbw, TRUE );

	return dbw;
}


void DialogBoxWindow::PopDown( DialogBoxWindow *dbw, aui_Button *button )
{
	Assert( dbw != NULL );
	if ( !dbw ) return;

	aui_Screen *screen = g_netshell->GetCurrentScreen();
	if ( screen )
		screen->RemoveWindow( dbw->Id() );

	if ( button && button->GetAction() )
		button->GetAction()->Execute(
			button,
			AUI_BUTTON_ACTION_EXECUTE,
			0 );

	g_ui->AddAction( new SafeDeleteAction( dbw ) );
}


void DialogBoxWindow::SafeDeleteAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	delete m_dbw;
	m_dbw = NULL;
}
