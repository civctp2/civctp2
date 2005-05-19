//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer game setup window 
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected delete type.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_directui.h"
#include "aui_blitter.h"
#include "aui_image.h"
#include "aui_surface.h"
#include "aui_ldl.h"
#include "aui_control.h"
#include "aui_action.h"

#include "ns_window.h"
#include "c3_static.h"



ns_Window::ns_Window(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	AUI_WINDOW_TYPE type )
	:

	c3_PopupWindow( retval, id, ldlBlock, bpp, type, FALSE )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE ns_Window::InitCommonLdl( MBCHAR *ldlBlock )
{
	aui_Ldl *theLdl = g_ui->GetLdl();
	

	
	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	MBCHAR *tile = block->GetString( k_NS_WINDOW_TILE_LDL_NAME );
	BOOL retired = block->GetBool( k_NS_WINDOW_RETIRED_LDL_NAME );

	AUI_ERRCODE errcode = InitCommon(
		tile,
		retired );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;









































































	










































	return AUI_ERRCODE_OK;
}



AUI_ERRCODE ns_Window::InitCommon( MBCHAR *tile, BOOL retired )
{
	m_numControls = 0;
	m_controls = NULL;
	m_tile = NULL;
	m_retired = FALSE;



	SetRetired( retired );
	SetTile( tile );
	SetDynamic( TRUE );

	SetStronglyModal( TRUE );





	return AUI_ERRCODE_OK;
}



ns_Window::~ns_Window()
{
	if ( m_tile )
	{
		g_ui->UnloadImage( m_tile );
		m_tile = NULL;
	}












	if ( m_controls )
	{
		for ( sint32 i = 0; i < m_numControls; i++ )
		{
			aui_Control *control = m_controls[ i ];
			if ( control )
			{
				if ( !control->GetActionFunc() )
				{
					aui_Action *action = control->GetAction();
					if ( action )
						delete action;
				}

				delete control;
			}
		}

		delete [] m_controls;
	}
}



aui_Control *ns_Window::FindControl( uint32 index )
{
	Assert( (sint32)index < m_numControls );
	if ( (sint32)index >= m_numControls ) return NULL;

	return m_controls[ index ];
}



aui_Image *ns_Window::SetTile( MBCHAR *tile )
{
	aui_Image *prevTile = m_tile;

	if ( tile )
	{
		m_tile = g_ui->LoadImage( tile );
		Assert( m_tile != NULL );
		if ( !m_tile )
		{
			m_tile = prevTile;
			return NULL;
		}
	}
	else
		m_tile = NULL;

	if ( prevTile )
		g_ui->UnloadImage( prevTile );

	return prevTile;
}



BOOL ns_Window::SetRetired( BOOL retired )
{
	BOOL wasRetired = m_retired;
	if ( m_retired = retired && !wasRetired )
	{
		
		MBCHAR *blockIdentifier = aui_Ldl::GetBlock( this );
		Assert( blockIdentifier != NULL );
		if ( !blockIdentifier ) return wasRetired;

		ldl *theLdl = aui_Ldl::GetLdl();
		Assert( theLdl != NULL );
		if ( !theLdl ) return wasRetired;

		ldl_datablock *block = theLdl->FindDataBlock( blockIdentifier );
		Assert( block != NULL );
		if ( !block ) return wasRetired;

		if ( block->GetAttributeType( k_NS_WINDOW_RETIRED_LDL_NAME )
			== ATTRIBUTE_TYPE_BOOL )
			block->SetValue( k_NS_WINDOW_RETIRED_LDL_NAME, true );
		else
			block->AddAttribute( k_NS_WINDOW_RETIRED_LDL_NAME, true );

		theLdl->WriteData();
	}

	return wasRetired;
}

