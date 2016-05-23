#include "c3.h"

#include "aui_static.h"


aui_Static::aui_Static
(
	AUI_ERRCODE *   retval,
	uint32          id,
	MBCHAR *        ldlBlock
)
:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_Control( retval, id, ldlBlock )
{
	if (AUI_SUCCESS(*retval))
    {
	    *retval = InitCommonLdl(ldlBlock);
    }
}

aui_Static::aui_Static
(
	AUI_ERRCODE *   retval,
	uint32          id,
	sint32          x,
	sint32          y,
	sint32          width,
	sint32          height,
	const MBCHAR *  text,
	uint32          maxLength
)
:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( text, maxLength ),
	aui_Control( retval, id, x, y, width, height )
{
	if (AUI_SUCCESS(*retval))
    {
	    *retval = InitCommon();
    }
}

AUI_ERRCODE aui_Static::InitCommonLdl( MBCHAR *ldlBlock )
{
	return InitCommon();
}


AUI_ERRCODE aui_Static::InitCommon( void )
{




	SetDrawMask( k_AUI_REGION_DRAWFLAG_UPDATE );

	return AUI_ERRCODE_OK;
}
