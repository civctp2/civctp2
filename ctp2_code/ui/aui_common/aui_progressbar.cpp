










#include "c3.h"
#include "aui_ui.h"
#include "aui_ldl.h"
#include "aui_blitter.h"
#include "aui_surface.h"
#include "aui_window.h"
#include "aui_animate.h"

#include "aui_progressbar.h"



aui_ProgressBar::aui_ProgressBar(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock )
	:
	aui_Control( retval, id, ldlBlock ),
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_ProgressBar::aui_ProgressBar(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height )
	:
	aui_Control( retval, id, x, y, width, height ),
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon(
		AUI_PROGRESSBAR_ORIENTATION_HORIZONTAL,
		AUI_PROGRESSBAR_TYPE_STANDARD,
		k_AUI_PROGRESSBAR_DEFAULT_VPS,
		NULL,
		k_AUI_PROGRESSBAR_DEFAULT_COLOR,
		0,
		k_AUI_PROGRESSBAR_DEFAULT_MAXVALUE );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_ProgressBar::InitCommonLdl( MBCHAR *ldlBlock )
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	
	AUI_PROGRESSBAR_ORIENTATION orientation;
	MBCHAR *keyword = block->GetString( k_AUI_PROGRESSBAR_LDL_ORIENTATION );
	if ( !keyword )
		orientation = AUI_PROGRESSBAR_ORIENTATION_HORIZONTAL;
	else
	{
		if ( stricmp( keyword, k_AUI_PROGRESSBAR_LDL_VERTICAL ) == 0 )
			orientation = AUI_PROGRESSBAR_ORIENTATION_VERTICAL;
		else
			orientation = AUI_PROGRESSBAR_ORIENTATION_HORIZONTAL;
	}

	
	COLORREF color = k_AUI_PROGRESSBAR_DEFAULT_COLOR;
	if ( block->GetAttributeType( k_AUI_PROGRESSBAR_LDL_BARRED )
			== ATTRIBUTE_TYPE_INT
	||   block->GetAttributeType( k_AUI_PROGRESSBAR_LDL_BARGREEN )
		 	== ATTRIBUTE_TYPE_INT
	||   block->GetAttributeType( k_AUI_PROGRESSBAR_LDL_BARBLUE )
		 	== ATTRIBUTE_TYPE_INT )
		color = RGB(
			block->GetInt( k_AUI_PROGRESSBAR_LDL_BARRED ),
			block->GetInt( k_AUI_PROGRESSBAR_LDL_BARGREEN ),
			block->GetInt( k_AUI_PROGRESSBAR_LDL_BARBLUE ));

	
	AUI_PROGRESSBAR_TYPE type;
	keyword = block->GetString( k_AUI_PROGRESSBAR_LDL_TYPE );
	if ( !keyword )
		type = AUI_PROGRESSBAR_TYPE_STANDARD;
	else
	{
		if ( stricmp( keyword, k_AUI_PROGRESSBAR_LDL_BACKANDFORTH ) == 0 )
			type = AUI_PROGRESSBAR_TYPE_BACKANDFORTH;
		else if ( stricmp( keyword, k_AUI_PROGRESSBAR_LDL_LOOP ) == 0 )
			type = AUI_PROGRESSBAR_TYPE_LOOP;
		else
			type = AUI_PROGRESSBAR_TYPE_STANDARD;
	}

	sint32 vps = k_AUI_PROGRESSBAR_DEFAULT_VPS;
	if ( block->GetAttributeType( k_AUI_PROGRESSBAR_LDL_VPS ) )
		vps = block->GetInt( k_AUI_PROGRESSBAR_LDL_VPS );

	AUI_ERRCODE errcode = InitCommon(
		orientation,
		type,
		vps,
		block->GetString( k_AUI_PROGRESSBAR_LDL_BARIMAGE ),
		color,
		block->GetInt( k_AUI_PROGRESSBAR_LDL_CURVALUE ),
		block->GetInt( k_AUI_PROGRESSBAR_LDL_MAXVALUE ) );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_ProgressBar::InitCommon(
	AUI_PROGRESSBAR_ORIENTATION orientation,
	AUI_PROGRESSBAR_TYPE type,
	sint32 vps,
	MBCHAR *image,
	COLORREF color,
	sint32 curValue,
	sint32 maxValue )
{
	m_orientation = orientation;
	m_type = type;
	m_vps = vps;
	m_curValue = curValue;
	m_maxValue =
		maxValue > curValue ?
		maxValue :
		k_AUI_PROGRESSBAR_DEFAULT_MAXVALUE;
	m_barImage = NULL;
	m_barColor = RGB(0,0,0);

	SetBarImage( image );
	SetBarColor( color );

	return AUI_ERRCODE_OK;
}



aui_ProgressBar::~aui_ProgressBar()
{
	if ( m_barImage )
	{
		g_ui->UnloadImage( m_barImage );
		m_barImage = NULL;
	}
}



AUI_PROGRESSBAR_ORIENTATION aui_ProgressBar::SetOrientation(
	AUI_PROGRESSBAR_ORIENTATION orientation )
{
	AUI_PROGRESSBAR_ORIENTATION prevOrientation = m_orientation;

	m_orientation = orientation;
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return prevOrientation;
}



AUI_PROGRESSBAR_TYPE aui_ProgressBar::SetProgressType(
	AUI_PROGRESSBAR_TYPE type )
{
	AUI_PROGRESSBAR_TYPE prevType = m_type;
	m_type = type;
	return prevType;
}



sint32 aui_ProgressBar::SetValuesPerSecond( sint32 vps )
{
	sint32 prevVps = m_vps;
	m_vps = vps;
	return prevVps;
}



sint32 aui_ProgressBar::SetCurValue( sint32 value )
{
	if ( value < 0 ) value = 0;
	if ( value > m_maxValue ) value = m_maxValue;

	sint32 prevValue = m_curValue;

	m_curValue = value;
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return prevValue;
}



sint32 aui_ProgressBar::SetMaxValue( sint32 value )
{
	if ( value < m_curValue ) value = m_curValue;

	sint32 prevValue = m_maxValue;

	m_maxValue = value;
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return prevValue;
}



aui_Image *aui_ProgressBar::SetBarImage( MBCHAR *image )
{
	aui_Image *prevImage = m_barImage;

	if ( image )
	{
		m_barImage = g_ui->LoadImage( image );
		Assert( m_barImage != NULL );
		if ( !m_barImage )
		{
			m_barImage = prevImage;
			return NULL;
		}
	}
	else
		m_barImage = NULL;

	if ( prevImage ) g_ui->UnloadImage( prevImage );

	return prevImage;
}



COLORREF aui_ProgressBar::SetBarColor( COLORREF color )
{
	COLORREF prevColor = m_barColor;

	m_barColor = color;

	return prevColor;
}



AUI_ERRCODE aui_ProgressBar::CalculateIntervals( double *start, double *stop )
{
	double x = (double)m_curValue / (double)m_maxValue;

	



	
	
	
	
	
	static POINT startKeyFrames[ 5 ] =
	{
		{ 0, 0 },
		{ 100, 25 },
		{ 200, 90 },
		{ 300, 315 },
		{ 400, 400 }
	};
	static POINT stopKeyFrames[ 5 ] =
	{
		{ 0, 0 },
		{ 100, 85 },
		{ 200, 310 },
		{ 300, 375 },
		{ 400, 400 }
	};

	POINT point;
	double t = x * 4.0;

	aui_Animate::Hermite( &point, t, startKeyFrames, 5 );
	*start = point.y / 400.0;

	aui_Animate::Hermite( &point, t, stopKeyFrames, 5 );
	*stop = point.y / 400.0;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_ProgressBar::DrawBar( aui_Surface *surface, RECT *bound )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	RECT destRect = *bound;
	double dimension = 0.0;

	
	
	
	double start, stop;
	CalculateIntervals( &start, &stop );

	
	if ( start < 0.0 ) start = 0.0;
	if ( start >= 1.0 ) start = 1.0;
	if ( stop < 0.0 ) stop = 0.0;
	if ( stop >= 1.0 ) stop = 1.0;
	if ( start > stop ) start = stop;

	switch ( m_orientation )
	{
	default:
	case AUI_PROGRESSBAR_ORIENTATION_HORIZONTAL:
		
		dimension = bound->right - bound->left;
		destRect.left += sint32(start * dimension + 0.5);
		destRect.right = bound->left + sint32(stop * dimension + 0.5);

		if ( m_barImage )
		{
			RECT srcRect =
			{
				0,
				0,
				m_barImage->TheSurface()->Width(),
				m_barImage->TheSurface()->Height()
			};
			srcRect.left = sint32(start * (double)srcRect.right + 0.5);
			srcRect.right = sint32(stop * (double)srcRect.right + 0.5);

			errcode = g_ui->TheBlitter()->Blt(
				surface,
				destRect.left,
				destRect.top,
				m_barImage->TheSurface(),
				&srcRect,
				k_AUI_BLITTER_FLAG_COPY );
		}
		else
			errcode = g_ui->TheBlitter()->ColorBlt(
				surface,
				&destRect,
				m_barColor,
				0 );
		break;

	case AUI_PROGRESSBAR_ORIENTATION_VERTICAL:
		
		dimension = bound->bottom - bound->top;
		destRect.bottom -= sint32(start * dimension + 0.5);
		destRect.top = bound->bottom - sint32(stop * dimension + 0.5);

		if ( m_barImage )
		{
			RECT srcRect =
			{
				0,
				0,
				m_barImage->TheSurface()->Width(),
				m_barImage->TheSurface()->Height()
			};
			srcRect.bottom = sint32(start * (double)srcRect.bottom + 0.5);
			srcRect.top = sint32(stop * (double)srcRect.bottom + 0.5);

			errcode = g_ui->TheBlitter()->Blt(
				surface,
				destRect.left,
				destRect.top,
				m_barImage->TheSurface(),
				&srcRect,
				k_AUI_BLITTER_FLAG_COPY );
		}
		else
			errcode = g_ui->TheBlitter()->ColorBlt(
				surface,
				&destRect,
				m_barColor,
				0 );
		break;
	}

	return errcode;
}



AUI_ERRCODE aui_ProgressBar::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{
	
	

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	DrawThisStateImage(
		0,
		surface,
		&rect );

	DrawBar(
		surface,
		&rect );

	DrawThisText(
		surface,
		&rect );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_ProgressBar::Idle( void )
{
	if ( m_type == AUI_PROGRESSBAR_TYPE_LOOP
	||   m_type == AUI_PROGRESSBAR_TYPE_BACKANDFORTH )
	{
		static uint32 time = GetTickCount();

		sint32 addValue = m_vps * sint32(GetTickCount() - time) / 1000;
		if ( addValue )
		{
			m_curValue += addValue;

			if ( m_curValue < 0 || m_curValue > m_maxValue )
			{
				m_curValue = Mod(m_curValue,m_maxValue+1);

				if ( m_type == AUI_PROGRESSBAR_TYPE_BACKANDFORTH )
				{
					m_curValue = m_maxValue - m_curValue;
					m_vps = -m_vps;
				}
			}

			time = GetTickCount();
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
		}
	}

	return AUI_ERRCODE_OK;
}
