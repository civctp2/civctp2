//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The civilization 3 UI header
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

#include "aui.h"
#include "aui_ui.h"
#include "aui_directui.h"
#include "aui_uniqueid.h"
#include "c3_headerswitch.h"
#include "aui_ldl.h"

#include "c3_header.h"

extern aui_UI *g_ui;


c3_Header::c3_Header(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_Header()
{
	*retval = aui_Region::InitCommonLdl( id, ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommonLdl( ldlBlock, NULL, NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SwitchGroup::InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Header::InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateSwitches( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


c3_Header::c3_Header(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_Header()
{
	*retval = aui_Region::InitCommon( id, x, y, width, height );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommon( NULL, NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SwitchGroup::InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Header::InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateSwitches();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE c3_Header::InitCommonLdl( MBCHAR *ldlBlock )
{
	return InitCommon();
}


AUI_ERRCODE c3_Header::InitCommon( void )
{
	return AUI_ERRCODE_OK;
}


AUI_ERRCODE c3_Header::CreateSwitches( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;




	aui_Switch		*dummyItemControl = new aui_Switch(&errcode, aui_UniqueId(), 0, 0, 0, m_height);
	AddChild( dummyItemControl );

	aui_Ldl *theLdl = g_ui->GetLdl();
	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( ldlBlock )
	{
		sint32 i = 0;
		do
		{
			sprintf( block, "%s.%s%d", ldlBlock, k_AUI_HEADER_LDL_SWITCH, i );

			if ( !theLdl->GetLdl()->FindDataBlock( block ) )
				break;

			aui_Switch *theSwitch = new c3_HeaderSwitch(
				&errcode,
				aui_UniqueId(),
				block );
			Assert( AUI_NEWOK(theSwitch,errcode) );
			if ( !AUI_NEWOK(theSwitch,errcode) )
				return AUI_ERRCODE_MEMALLOCFAILED;

			aui_Action *action = new aui_HeaderSwitchAction( i );
			Assert( action != NULL );
			if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;

			theSwitch->SetAction( action );

			AddChild( theSwitch );

			i++;

		} while ( 1 );
	}

	return AUI_ERRCODE_OK;
}


c3_Header::~c3_Header()
{

}
