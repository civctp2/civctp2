










#include "c3.h"
#include "aui_ui.h"
#include "aui_tab.h"
#include "aui_ldl.h"

#include "aui_tabgroup.h"



aui_TabGroup::aui_TabGroup(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_SwitchGroup( retval, id, ldlBlock )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_TabGroup::aui_TabGroup(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	AUI_TABGROUP_ALIGNMENT alignment,
	BOOL autoArrange,
	sint32 paneOffsetX,
	sint32 paneOffsetY )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_SwitchGroup( retval, id, x, y, 0, 0 )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( alignment, autoArrange, paneOffsetX, paneOffsetY );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_TabGroup::InitCommonLdl( MBCHAR *ldlBlock )
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	AUI_TABGROUP_ALIGNMENT alignment = AUI_TABGROUP_ALIGNMENT_TOP;
	MBCHAR *alignmentString = block->GetString( k_AUI_TABGROUP_LDL_ALIGNMENT );
	if ( alignmentString )
	{
		if ( !stricmp( alignmentString, k_AUI_TABGROUP_LDL_LEFT ) )
			alignment = AUI_TABGROUP_ALIGNMENT_LEFT;
		else if ( !stricmp( alignmentString, k_AUI_TABGROUP_LDL_RIGHT ) )
			alignment = AUI_TABGROUP_ALIGNMENT_RIGHT;
		else if ( !stricmp( alignmentString, k_AUI_TABGROUP_LDL_BOTTOM ) )
			alignment = AUI_TABGROUP_ALIGNMENT_BOTTOM;
	}

	BOOL autoArrange = TRUE;
	if ( block->GetAttributeType( k_AUI_TABGROUP_LDL_AUTOARRANGE )
			== ATTRIBUTE_TYPE_BOOL )
		autoArrange = block->GetBool( k_AUI_TABGROUP_LDL_AUTOARRANGE );

	sint32 paneOffsetX = block->GetInt( k_AUI_TABGROUP_LDL_PANEOFFSETX );
	sint32 paneOffsetY = block->GetInt( k_AUI_TABGROUP_LDL_PANEOFFSETY );

	AUI_ERRCODE errcode = InitCommon(
		alignment,
		autoArrange,
		paneOffsetX,
		paneOffsetY );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_TabGroup::InitCommon(
	AUI_TABGROUP_ALIGNMENT alignment,
	BOOL autoArrange,
	sint32 paneOffsetX,
	sint32 paneOffsetY )
{
	m_alignment = alignment;
	m_autoArrange = autoArrange;

	m_paneOffsetX = paneOffsetX;
	m_paneOffsetY = paneOffsetY;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_TabGroup::AddChild( aui_Region *child )
{
	AUI_ERRCODE errcode = aui_SwitchGroup::AddChild( child );
	if ( errcode == AUI_ERRCODE_OK )
	{
		
		FormatTabs();

		
		if ( !MakeSureOnlyOneIsSelected() )
		{
			
			aui_Tab *tab = (aui_Tab *)m_childList->GetHead();
			tab->SetState( 1 );
			m_whichIsSelected = tab->Id();
		}
	}

	return errcode;
}



AUI_ERRCODE aui_TabGroup::FormatTabs( void )
{
	sint32 numTabs = m_childList->L();

	
	sint32 maxPaneWidth = 0;
	sint32 maxPaneHeight = 0;
	ListPos position = m_childList->GetHeadPosition();
	sint32 i;
	for ( i = numTabs; i; i-- )
	{
		aui_Tab *tab = (aui_Tab *)m_childList->GetNext( position );
		if ( tab->GetPane()->Width() > maxPaneWidth )
			maxPaneWidth = tab->GetPane()->Width();
		if ( tab->GetPane()->Height() > maxPaneHeight )
			maxPaneHeight = tab->GetPane()->Height();
	}

	
	sint32 maxTabWidth = 0;
	sint32 maxTabHeight = 0;
	position = m_childList->GetHeadPosition();
	for ( i = numTabs; i; i-- )
	{
		aui_Tab *tab = (aui_Tab *)m_childList->GetNext( position );
		if ( tab->Width() > maxTabWidth ) maxTabWidth = tab->Width();
		if ( tab->Height() > maxTabHeight ) maxTabHeight = tab->Height();
	}

	switch ( m_alignment )
	{
	default:
	case AUI_TABGROUP_ALIGNMENT_TOP:
	{
		
		
		sint32 newTabWidth = sint32(0.9 * maxPaneWidth / numTabs + 0.5);

		
		sint32 tabOffset = sint32(0.05 * maxPaneWidth + 0.5);

		
		position = m_childList->GetHeadPosition();
		for ( i = numTabs; i; i-- )
		{
			aui_Tab *tab = (aui_Tab *)m_childList->GetNext( position );

			if ( m_autoArrange )
			{
				
				tab->Move( tabOffset, 0 );
				tab->Resize( newTabWidth, maxTabHeight );
			}
			else
				tabOffset = tab->X();

			
			tab->GetPane()->Move( -tabOffset, maxTabHeight );
			tab->GetPane()->Resize( maxPaneWidth, maxPaneHeight );
			tab->GetPane()->Offset( m_paneOffsetX, m_paneOffsetY );

			
			tabOffset += newTabWidth;
		}
	}
	break;

	case AUI_TABGROUP_ALIGNMENT_BOTTOM:
	{
		
		
		sint32 newTabWidth = sint32(0.9 * maxPaneWidth / numTabs + 0.5);

		
		sint32 tabOffset = sint32(0.05 * maxPaneWidth + 0.5);

		
		position = m_childList->GetHeadPosition();
		for ( i = numTabs; i; i-- )
		{
			aui_Tab *tab = (aui_Tab *)m_childList->GetNext( position );

			if ( m_autoArrange )
			{
				
				tab->Move( tabOffset, maxPaneHeight );
				tab->Resize( newTabWidth, maxTabHeight );
			}
			else
				tabOffset = tab->X();

			
			tab->GetPane()->Move( -tabOffset, -maxPaneHeight );
			tab->GetPane()->Resize( maxPaneWidth, maxPaneHeight );
			tab->GetPane()->Offset( m_paneOffsetX, m_paneOffsetY );

			
			tabOffset += newTabWidth;
		}
	}
	break;

	case AUI_TABGROUP_ALIGNMENT_LEFT:
	{
		
		
		sint32 newTabHeight = sint32(0.9 * maxPaneHeight / numTabs + 0.5);

		
		sint32 tabOffset = sint32(0.05 * maxPaneHeight + 0.5);

		
		position = m_childList->GetHeadPosition();
		for ( i = numTabs; i; i-- )
		{
			aui_Tab *tab = (aui_Tab *)m_childList->GetNext( position );

			if ( m_autoArrange )
			{
				
				tab->Move( 0, tabOffset );
				tab->Resize( maxTabWidth, newTabHeight );
			}
			else
				tabOffset = tab->Y();

			
			tab->GetPane()->Move( maxTabWidth, -tabOffset );
			tab->GetPane()->Resize( maxPaneWidth, maxPaneHeight );
			tab->GetPane()->Offset( m_paneOffsetX, m_paneOffsetY );

			
			tabOffset += newTabHeight;
		}
	}
	break;

	case AUI_TABGROUP_ALIGNMENT_RIGHT:
	{
		
		
		sint32 newTabHeight = sint32(0.9 * maxPaneHeight / numTabs + 0.5);

		
		sint32 tabOffset = sint32(0.05 * maxPaneHeight + 0.5);

		
		position = m_childList->GetHeadPosition();
		for ( i = numTabs; i; i-- )
		{
			aui_Tab *tab = (aui_Tab *)m_childList->GetNext( position );

			if ( m_autoArrange )
			{
				
				tab->Move( maxPaneWidth, tabOffset );
				tab->Resize( maxTabWidth, newTabHeight );
			}
			else
				tabOffset = tab->Y();

			
			tab->GetPane()->Move( -maxPaneWidth, -tabOffset );
			tab->GetPane()->Resize( maxPaneWidth, maxPaneHeight );
			tab->GetPane()->Offset( m_paneOffsetX, m_paneOffsetY );

			
			tabOffset += newTabHeight;
		}
	}
	break;
	}
	
	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_TabGroup::RemoveChild( uint32 tabId )
{
	AUI_ERRCODE errcode = aui_SwitchGroup::RemoveChild( tabId );

	
	FormatTabs();

	return errcode;
}



AUI_TABGROUP_ALIGNMENT aui_TabGroup::SetTabGroupAlignment(
	AUI_TABGROUP_ALIGNMENT alignment )
{
	AUI_TABGROUP_ALIGNMENT previousAlignment = m_alignment;

	m_alignment = alignment;
	FormatTabs();

	return previousAlignment;
}
