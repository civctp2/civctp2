//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : User interface - image handling
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
// - Crashes prevented.
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "aui_imagebase.h"


#include "aui_blitter.h"
#include "aui_ldl.h"
#include "aui_surface.h"
#include "aui_ui.h"


MBCHAR *aui_ImageBase::m_substateLdlKeywords[ AUI_IMAGEBASE_SUBSTATE_LAST ] =
{
	"image",
	"activeimage",
	"focusimage",
	"activefocusimage",
	"disabledimage"
};



aui_ImageBase::aui_ImageBase
(
	MBCHAR *        ldlBlock,
	bool            loadOnDemand 
)
:   m_stateImageNames           (NULL),
	m_numberOfStateImageNames   (0),
	m_numStateImageGroups       (0),
	m_stateImageGroups          (NULL),
    m_loadOnDemand              (loadOnDemand),
	m_chromaRed	   		        (k_DEFAULT_CHROMA_RED),
	m_chromaGreen		        (k_DEFAULT_CHROMA_GREEN),
	m_chromaBlue		        (k_DEFAULT_CHROMA_BLUE),
	m_chromaSpecified           (false)
{
	InitCommonLdl(ldlBlock);
}



aui_ImageBase::aui_ImageBase
(
	sint32 numStateImageGroups,
	AUI_IMAGEBASE_BLTTYPE imageblttype,
	AUI_IMAGEBASE_BLTFLAG imagebltflag,
	bool loadOnDemand 
)
:   m_stateImageNames           (NULL),
	m_numberOfStateImageNames   (0),
	m_numStateImageGroups       (0),
	m_stateImageGroups          (NULL),
    m_loadOnDemand              (loadOnDemand),
	m_chromaRed	   		        (k_DEFAULT_CHROMA_RED),
	m_chromaGreen		        (k_DEFAULT_CHROMA_GREEN),
	m_chromaBlue		        (k_DEFAULT_CHROMA_BLUE),
	m_chromaSpecified           (false)
{
	InitCommon(numStateImageGroups, imageblttype, imagebltflag);
}



AUI_ERRCODE aui_ImageBase::InitCommonLdl(
	MBCHAR *ldlBlock )
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	AUI_IMAGEBASE_BLTFLAG imagebltflag = AUI_IMAGEBASE_BLTFLAG_COPY;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	
	if (block->GetAttributeType(k_AUI_IMAGEBASE_LDL_CHROMAKEY_RED  ) == ATTRIBUTE_TYPE_INT)
	{
		m_chromaRed		 =block->GetInt(k_AUI_IMAGEBASE_LDL_CHROMAKEY_RED);
		m_chromaSpecified=true;
	}

	
	if (block->GetAttributeType(k_AUI_IMAGEBASE_LDL_CHROMAKEY_GREEN) == ATTRIBUTE_TYPE_INT) 
	{
		m_chromaGreen	 =block->GetInt(k_AUI_IMAGEBASE_LDL_CHROMAKEY_GREEN);
		m_chromaSpecified=true;
	}
	
	if (block->GetAttributeType(k_AUI_IMAGEBASE_LDL_CHROMAKEY_BLUE ) == ATTRIBUTE_TYPE_INT) 
	{
		m_chromaBlue	 =block->GetInt(k_AUI_IMAGEBASE_LDL_CHROMAKEY_BLUE);
		m_chromaSpecified=true;
	}

	if (m_chromaSpecified)
		imagebltflag = AUI_IMAGEBASE_BLTFLAG_CHROMAKEY;

	AUI_IMAGEBASE_BLTTYPE imageblttype = AUI_IMAGEBASE_BLTTYPE_COPY;
	MBCHAR *type = block->GetString( k_AUI_IMAGEBASE_LDL_BLTTYPE );
	if ( type )
	{
		if ( !stricmp( type, k_AUI_IMAGEBASE_LDL_STRETCH ) )
			imageblttype = AUI_IMAGEBASE_BLTTYPE_STRETCH;
		else if ( !stricmp( type, k_AUI_IMAGEBASE_LDL_TILE ) )
			imageblttype = AUI_IMAGEBASE_BLTTYPE_TILE;
	}

	
	MBCHAR *flags = block->GetString(k_AUI_IMAGEBASE_LDL_BLTFLAG);
	if ((flags)&&(!m_chromaSpecified)) 
	{
		if (!stricmp(flags, k_AUI_IMAGEBASE_LDL_BLTFLAG_CHROMAKEY)) 
		{
			imagebltflag = AUI_IMAGEBASE_BLTFLAG_CHROMAKEY;
			m_chromaSpecified = true;
		} 
		else
			if (!stricmp(flags, k_AUI_IMAGEBASE_LDL_BLTFLAG_BLEND)) {
				imagebltflag = AUI_IMAGEBASE_BLTFLAG_BLEND;
			}
	}

	
	BOOL setImages = TRUE;
	sint32 numStateImageGroups = FindNumStateImageGroupsFromLdl( block );
	if ( !numStateImageGroups )
	{
		setImages = FALSE;
		numStateImageGroups = 1;
	}

	m_centerImage = block->GetBool(k_AUI_IMAGEBASE_LDL_CENTERIMAGE) != FALSE;

	AUI_ERRCODE errcode = InitCommon(
		numStateImageGroups,
		imageblttype,
		imagebltflag );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	
	if ( setImages )
	{
		MBCHAR temp[ k_AUI_LDL_MAXBLOCK + 1 ] = "";
		for ( sint32 i = 0; i < m_numStateImageGroups; i++ )
		for ( sint32 j = 0; j < AUI_IMAGEBASE_SUBSTATE_LAST; j++ )
		{
			sprintf( temp, "%s%d", m_substateLdlKeywords[ j ], i );
			SetImage(
				block->GetString( temp ),
				i,
				(AUI_IMAGEBASE_SUBSTATE)j );
		}
	}

	return AUI_ERRCODE_OK;
}



sint32 aui_ImageBase::FindNumStateImageGroupsFromLdl( ldl_datablock *block )
{
	sint32 i = block->GetInt( k_AUI_IMAGEBASE_LDL_NUMIMAGEGROUPS );
	if ( i ) return i;

	i = -1;
	BOOL found = TRUE;
	MBCHAR temp[ k_AUI_LDL_MAXBLOCK + 1 ] = "";
	while ( found )
	{
		i++;
		found = FALSE;
		for ( sint32 j = 0; j < AUI_IMAGEBASE_SUBSTATE_LAST && !found; j++ )
		{
			sprintf( temp, "%s%d", m_substateLdlKeywords[ j ], i );
			if ( block->GetString( temp ) ) found = TRUE;
		}
	}

	return i;
}



AUI_ERRCODE aui_ImageBase::InitCommon(
	sint32 numStateImageGroups,
	AUI_IMAGEBASE_BLTTYPE imageblttype,
	AUI_IMAGEBASE_BLTFLAG imagebltflag )
{
	m_imageblttype = imageblttype;
	m_imagebltflag = imagebltflag;

	if ( !numStateImageGroups ) numStateImageGroups = 1;

	
	if(m_loadOnDemand) {
		
		m_numberOfStateImageNames = numStateImageGroups * AUI_IMAGEBASE_SUBSTATE_LAST;
		m_stateImageNames = new MBCHAR * [m_numberOfStateImageNames];

		
		for(int index = 0; index < m_numberOfStateImageNames; index++)
			m_stateImageNames[index] = NULL;
	} 

	m_stateImageGroups =
		new aui_StateImageGroup[ m_numStateImageGroups = numStateImageGroups ];
	Assert( m_stateImageGroups != NULL );
	if ( !m_stateImageGroups ) return AUI_ERRCODE_MEMALLOCFAILED;

	memset(
		m_stateImageGroups,
		0,
		m_numStateImageGroups * sizeof( aui_StateImageGroup ) );

	return AUI_ERRCODE_OK;
}



aui_ImageBase::~aui_ImageBase()
{
	if (m_stateImageNames) 
    {
		for (int index = 0; index < m_numberOfStateImageNames; index++)
			if(m_stateImageNames[index]) free(m_stateImageNames[index]);

		delete [] m_stateImageNames;
	}

	if (m_stateImageGroups)
	{
		for ( sint32 i = 0; i < m_numStateImageGroups; i++)
		    for (sint32 j = 0; j < AUI_IMAGEBASE_SUBSTATE_LAST; j++)
			    if (m_stateImageGroups[ i ][ j ])
				    g_ui->UnloadImage( m_stateImageGroups[ i ][ j ] );

		delete [] m_stateImageGroups;
	}
}



aui_Image *aui_ImageBase::GetImage(
	sint32 state,
	AUI_IMAGEBASE_SUBSTATE substate ) const
{
	if ( state < 0 || state >= m_numStateImageGroups )
		return NULL;

	Assert( AUI_IMAGEBASE_SUBSTATE_FIRST <= substate
	&&      substate < AUI_IMAGEBASE_SUBSTATE_LAST );
	if ( AUI_IMAGEBASE_SUBSTATE_FIRST > substate
	&&   substate >= AUI_IMAGEBASE_SUBSTATE_LAST )
		return NULL;

	
	
	if(m_loadOnDemand) {
		
		sint32 index = (state * AUI_IMAGEBASE_SUBSTATE_LAST) + substate;

		
		
		if(m_stateImageNames[index] && (!m_stateImageGroups[state][substate])) 
		{
			m_stateImageGroups[ state ][ substate ] = g_ui->LoadImage(m_stateImageNames[index]);
			Assert( m_stateImageGroups[ state ][ substate ] != NULL );

			if ((m_chromaSpecified)&&(m_stateImageGroups[ state ][ substate ]!=NULL))
				m_stateImageGroups[ state ][ substate]->SetChromakey(m_chromaRed,m_chromaGreen,m_chromaBlue); 
		}
	}

	return m_stateImageGroups[ state ][ substate ];
}



AUI_IMAGEBASE_BLTTYPE aui_ImageBase::SetImageBltType(
	AUI_IMAGEBASE_BLTTYPE imageblttype )
{
	AUI_IMAGEBASE_BLTTYPE prevtype = m_imageblttype;
	m_imageblttype = imageblttype;
	return prevtype;
}



AUI_IMAGEBASE_BLTFLAG aui_ImageBase::SetImageBltFlag(
	AUI_IMAGEBASE_BLTFLAG imagebltflag )
{
	AUI_IMAGEBASE_BLTFLAG prevtype = m_imagebltflag;
	m_imagebltflag = imagebltflag;
	return prevtype;
}



aui_Image *aui_ImageBase::SetImage(
	MBCHAR *image,
	sint32 state,
	AUI_IMAGEBASE_SUBSTATE substate )
{
	
	
	Assert(state >= 0 && state < m_numStateImageGroups);
	if (state < 0 || state >= m_numStateImageGroups)
		return NULL;

	Assert(substate >= AUI_IMAGEBASE_SUBSTATE_FIRST &&
			substate < AUI_IMAGEBASE_SUBSTATE_LAST);

	if (substate < AUI_IMAGEBASE_SUBSTATE_FIRST ||
		substate >= AUI_IMAGEBASE_SUBSTATE_LAST) 
		return NULL;

	aui_Image *prevImage = GetImage( state, substate );

	if ( image )
	{
		
		if(m_loadOnDemand) {
			
			Assert( state >= 0 && state < m_numStateImageGroups );

			
			sint32 index = (state * AUI_IMAGEBASE_SUBSTATE_LAST) + substate;

			
			if(m_stateImageNames[index]) {
				free(m_stateImageNames[index]);
			}

			
			m_stateImageNames[index] = strdup(image);

			
			m_stateImageGroups[ state ][ substate ] = NULL;
		} 
		else 
		{	
			m_stateImageGroups[ state ][ substate ] = g_ui->LoadImage( image );
			Assert( m_stateImageGroups[ state ][ substate ] != NULL );
			if ( !m_stateImageGroups[ state ][ substate ] )
			{
				m_stateImageGroups[ state ][ substate ] = prevImage;
				return NULL;
			}

			if (m_chromaSpecified)
				m_stateImageGroups[ state ][ substate]->SetChromakey(m_chromaRed,m_chromaGreen,m_chromaBlue); 

		}
	}
	else {
		
		if(m_loadOnDemand) {
			
			Assert( state >= 0 && state < m_numStateImageGroups );

			
			sint32 index = (state * AUI_IMAGEBASE_SUBSTATE_LAST) + substate;

			
			if(m_stateImageNames[index]) {
				free(m_stateImageNames[index]);
				m_stateImageNames[index] = NULL;
			}
		}
		
		
		m_stateImageGroups[ state ][ substate ] = NULL;
	}

	if ( prevImage ) g_ui->UnloadImage( prevImage );

	return prevImage;
}



AUI_ERRCODE aui_ImageBase::DrawImage(
	aui_Surface *destSurf,
	RECT *destRect,
	sint32 state,
	AUI_IMAGEBASE_SUBSTATE substate )
{
	
	aui_Image *image = GetImage( state, substate );
	if ( !image ) return AUI_ERRCODE_OK;

	aui_Surface *srcSurf = image->TheSurface();

	if (!srcSurf) return AUI_ERRCODE_OK;

	RECT srcRect =
	{
		0,
		0,
		srcSurf->Width(),
		srcSurf->Height()
	};

	uint32 flag = 0;

	switch ( m_imagebltflag )
	{
	default:
	case AUI_IMAGEBASE_BLTFLAG_COPY:
		flag |= k_AUI_BLITTER_FLAG_COPY;
		break;
	case AUI_IMAGEBASE_BLTFLAG_CHROMAKEY:
		flag |= k_AUI_BLITTER_FLAG_CHROMAKEY;
		break;
	case AUI_IMAGEBASE_BLTFLAG_BLEND:
		flag |= k_AUI_BLITTER_FLAG_BLEND;
		break;
	}

	RECT realDestRect = *destRect;
	if(m_centerImage) {
		sint32 w = realDestRect.right - realDestRect.left;
		realDestRect.left += w / 2 - srcRect.right / 2;

		sint32 h = realDestRect.bottom - realDestRect.top;
		realDestRect.top += h / 2 - srcRect.bottom / 2;
	}

	switch ( m_imageblttype )
	{
	default:
	case AUI_IMAGEBASE_BLTTYPE_COPY:
		return g_ui->TheBlitter()->Blt(
			destSurf,
			realDestRect.left,
			realDestRect.top,
			srcSurf,
			&srcRect,
			flag );

	case AUI_IMAGEBASE_BLTTYPE_STRETCH:
		return g_ui->TheBlitter()->StretchBlt(
			destSurf,
			&realDestRect,
			srcSurf,
			&srcRect,
			flag );

	case AUI_IMAGEBASE_BLTTYPE_TILE:
		return g_ui->TheBlitter()->TileBlt(
			destSurf,
			&realDestRect,
			srcSurf,
			&srcRect,
			0,
			0,
			flag );
	}
}
