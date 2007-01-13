










#include "c3.h"
#include "aui_ui.h"
#include "aui_ldl.h"
#include "aui_surface.h"
#include "aui_bitmapfont.h"
#include "aui_static.h"

#include "aui_hypertextbase.h"


aui_HyperTextBase::aui_HyperTextBase(
	AUI_ERRCODE *retval,
	MBCHAR *ldlBlock )
{
	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_HyperTextBase::aui_HyperTextBase(
	AUI_ERRCODE *retval,
	MBCHAR *hyperText,
	uint32 hyperMaxLen )
{
	*retval = InitCommon( hyperText, hyperMaxLen );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_HyperTextBase::InitCommonLdl( MBCHAR *ldlBlock )
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	AUI_ERRCODE errcode = InitCommon(
		block->GetString( k_AUI_HYPERTEXTBASE_LDL_TEXT ),
		block->GetInt( k_AUI_HYPERTEXTBASE_LDL_MAXLEN ) );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_HyperTextBase::InitCommon(
	MBCHAR *hyperText,
	uint32 hyperMaxLen )
{
	m_hyperText = NULL,
	m_hyperMaxLen = hyperMaxLen ?
		hyperMaxLen :
		k_AUI_HYPERTEXTBASE_DEFAULT_MAXLEN,
	m_hyperCurLen = 0;

	if ( hyperText )
	{
		m_hyperCurLen = strlen( hyperText );

		if ( m_hyperCurLen > m_hyperMaxLen )
			m_hyperCurLen = m_hyperMaxLen;

		
		m_hyperText = new MBCHAR[ m_hyperMaxLen + 1 ];
		Assert( m_hyperText != NULL );
		if ( !m_hyperText ) return AUI_ERRCODE_MEMALLOCFAILED;

		memset( m_hyperText, '\0', m_hyperMaxLen + 1 );

		
		strncpy( m_hyperText, hyperText, m_hyperMaxLen );
	}
	else
	{
		if ( m_hyperMaxLen )
		{
			
			m_hyperText = new MBCHAR[ m_hyperMaxLen + 1 ];
			Assert( m_hyperText != NULL );
			if ( !m_hyperText ) return AUI_ERRCODE_MEMALLOCFAILED;

			memset( m_hyperText, '\0', m_hyperMaxLen + 1 );
		}
	}

	m_hyperStaticList = new tech_WLList<aui_Static *>;
	Assert( m_hyperStaticList != NULL );
	if ( !m_hyperStaticList ) return AUI_ERRCODE_MEMALLOCFAILED;

	memset( m_hyperTtffile, 0, sizeof( m_hyperTtffile ) );
	strncpy( m_hyperTtffile, "arial.ttf", MAX_PATH );
	m_hyperPointSize = 12;
	m_hyperBold = 0;
	m_hyperItalic = 0;
	m_hyperColor = RGB(255,255,255);
	m_hyperColorOld = RGB( 255, 255, 255 );
	m_hyperUnderline = 0;
	m_hyperShadow = FALSE;
	m_hyperShadowColor = RGB(0,0,0);
	m_hyperFlags = k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT;

	return AUI_ERRCODE_OK;
}



aui_HyperTextBase::~aui_HyperTextBase()
{
	if ( m_hyperText )
	{
		delete[ m_hyperMaxLen + 1 ] m_hyperText;
		m_hyperText = NULL;
	}

	if ( m_hyperStaticList )
	{
		RemoveHyperStatics();

		delete m_hyperStaticList;
		m_hyperStaticList = NULL;
	}
}



AUI_ERRCODE aui_HyperTextBase::SetHyperText(
	const MBCHAR *hyperText,
	uint32 maxlen )
{
	Assert( hyperText != NULL );
	if ( !hyperText ) return AUI_ERRCODE_INVALIDPARAM;

	
	memset( m_hyperText, '\0', m_hyperMaxLen + 1 );

	
	if ( maxlen > m_hyperMaxLen ) maxlen = m_hyperMaxLen;
	strncpy( m_hyperText, hyperText, maxlen );

	m_hyperCurLen = strlen( m_hyperText );

	return AddHyperStatics( NULL );
}



AUI_ERRCODE aui_HyperTextBase::AppendHyperText( const MBCHAR *hyperText )
{
	Assert( hyperText != NULL );
	if ( !hyperText ) return AUI_ERRCODE_INVALIDPARAM;

	

	strncat( m_hyperText, hyperText, m_hyperMaxLen - m_hyperCurLen );

	m_hyperCurLen = strlen( m_hyperText );

	return AddHyperStatics( hyperText );
}



AUI_ERRCODE aui_HyperTextBase::AddHyperStatics( const MBCHAR *hyperText )
{
	if ( !hyperText )
	{
		
		RemoveHyperStatics();
		hyperText = m_hyperText;
	}

	sint32 len = strlen( hyperText );
	if ( !len ) return AUI_ERRCODE_OK;

	
	aui_Static *hs = CreateHyperStatic(
		hyperText,
		len,
		m_hyperTtffile,
		m_hyperPointSize,
		m_hyperBold,
		m_hyperItalic,
		m_hyperColor,
		m_hyperUnderline,
		m_hyperShadow,
		m_hyperShadowColor,
		m_hyperFlags );
	Assert( hs != NULL );
	if ( !hs ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_hyperStaticList->AddTail( hs );

	if ( m_hyperStaticList->L() > k_AUI_HYPERTEXTBOX_LDL_MAXSTATICS )
		DestroyHyperStatic( m_hyperStaticList->RemoveHead() );

	return AUI_ERRCODE_OK;
}



void aui_HyperTextBase::RemoveHyperStatics( void )
{
	ListPos position = m_hyperStaticList->GetHeadPosition();
	for ( sint32 i = m_hyperStaticList->L(); i; i-- )
		DestroyHyperStatic( m_hyperStaticList->RemoveTail() );
}



aui_Static *aui_HyperTextBase::CreateHyperStatic(
	const MBCHAR *string,
	uint32 len,
	MBCHAR ttffile[ MAX_PATH + 1 ],
	sint32 pointSize,
	sint32 bold,
	sint32 italic,
	COLORREF color,
	sint32 underline,
	BOOL shadow,
	COLORREF shadowColor,
	uint32 flags )
{
	
	AUI_ERRCODE errcode;
	aui_Static *hs = new aui_Static(
		&errcode,
		0,
		0, 0, 0, 0,
		string,
		len );
	Assert( AUI_NEWOK(hs,errcode) );
	if ( !AUI_NEWOK(hs,errcode) ) return NULL;

	hs->SetBlindness( TRUE );

	
	hs->TextFlags() = flags;
	hs->SetTextColor( color );
	hs->SetTextUnderline( underline );
	hs->SetTextShadow( shadow );
	hs->SetTextShadowColor( shadowColor );

	
	hs->SetTextFont( ttffile );
	hs->SetTextFontSize( pointSize );
	hs->SetTextBold( bold );
	hs->SetTextItalic( italic );

	
	hs->TextReloadFont();

	
	
	hs->Resize(
		hs->GetTextFont()->GetStringWidth( hs->GetText() ),
		hs->GetTextFont()->GetLineSkip() );

	return hs;
}



void aui_HyperTextBase::DestroyHyperStatic( aui_Static *hs )
{
	Assert( hs != NULL );
	if ( !hs ) return;

	delete hs;
}



AUI_ERRCODE aui_HyperTextBase::DrawThisHyperText(
	aui_Surface *destSurf,
	RECT *clipRect,
	sint32 x,
	sint32 y )
{
	ListPos position = m_hyperStaticList->GetHeadPosition();
	for ( sint32 i = m_hyperStaticList->L(); i; i-- )
	{
		aui_Static *hs = m_hyperStaticList->GetNext( position );

		RECT bound;
		bound.left = x + hs->X() + clipRect->left;
		bound.top = y + hs->Y() + clipRect->top;
		bound.right = bound.left + hs->Width();
		bound.bottom = bound.top + hs->Height();

		if ( m_hyperShadow )
		{
			RECT shadowRect = bound;
			OffsetRect( &shadowRect, -1, 1 );
			AUI_ERRCODE errcode = hs->GetTextFont()->DrawString(
				destSurf,
				&shadowRect,
				clipRect,
				hs->GetText(),
				hs->TextFlags(),
				hs->GetTextShadowColor(),
				hs->GetTextUnderline() );
			Assert( AUI_SUCCESS(errcode) );
			if ( !AUI_SUCCESS(errcode) ) return errcode;
		}

		hs->GetTextFont()->DrawString(
			destSurf,
			&bound,
			clipRect,
			hs->GetText(),
			hs->TextFlags(),
			hs->GetTextColor(),
			hs->GetTextUnderline() );
	}

	return AUI_ERRCODE_OK;
}
