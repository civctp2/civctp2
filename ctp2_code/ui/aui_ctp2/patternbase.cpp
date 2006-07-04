\












#include "c3.h"

#include "aui.h"
#include "aui_ldl.h"

#include "c3ui.h"

#include "patternbase.h"


extern C3UI *g_c3ui;



PatternBase::PatternBase( MBCHAR *ldlBlock, MBCHAR *pattern )
{
	InitCommonLdl( ldlBlock, pattern );
}



PatternBase::PatternBase( MBCHAR *pattern )
{
	
	m_srcX = 0;
	m_srcY = 0;
	m_srcWidthPix = 0;
	m_srcHeightPix = 0;

	InitCommon( pattern );

}



AUI_ERRCODE PatternBase::InitCommonLdl( MBCHAR *ldlBlock, MBCHAR *pattern )
{
    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	
	MBCHAR *name = block->GetString( pattern ? pattern : "pattern" );


	if (block->GetAttributeType( k_PATTERNBASE_LDL_SRCXPIX ) == ATTRIBUTE_TYPE_INT) {
		m_srcX = block->GetInt( k_PATTERNBASE_LDL_SRCXPIX );
	} else {
		m_srcX = 0; 
	}

	if (block->GetAttributeType( k_PATTERNBASE_LDL_SRCYPIX ) == ATTRIBUTE_TYPE_INT) {
		m_srcY = block->GetInt( k_PATTERNBASE_LDL_SRCYPIX );
	} else {
		m_srcY = 0; 
	}

	if (block->GetAttributeType( k_PATTERNBASE_LDL_SRCWIDTHPIX ) == ATTRIBUTE_TYPE_INT) {
		m_srcWidthPix = block->GetInt( k_PATTERNBASE_LDL_SRCWIDTHPIX );
	} else {
		m_srcWidthPix = 0; 
	}

	if (block->GetAttributeType( k_PATTERNBASE_LDL_SRCHEIGHTPIX ) == ATTRIBUTE_TYPE_INT) {
		m_srcHeightPix = block->GetInt( k_PATTERNBASE_LDL_SRCHEIGHTPIX );
	} else {
		m_srcHeightPix = 0; 
	}

	InitCommon( name );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE PatternBase::InitCommon( MBCHAR *pattern )
{
	if (pattern != NULL) {
		m_pattern = g_c3ui->LoadPattern( pattern );
		Assert( m_pattern != NULL );
	} else {
		m_pattern = NULL;
	}

	return AUI_ERRCODE_OK;
}



PatternBase::~PatternBase()
{
	if ( m_pattern )
	{
		g_c3ui->UnloadPattern( m_pattern );
		m_pattern = NULL;
	}
}
