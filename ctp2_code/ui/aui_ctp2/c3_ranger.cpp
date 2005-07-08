


#include "c3.h"
#include "aui.h"
#include "aui_ldl.h"
#include "aui_ranger.h"
#include "aui_static.h"
#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "c3_button.h"
#include "aui_window.h"
#include "aui_dimension.h"

#include "c3_ranger.h"
#include "c3_thumb.h"

#include "pattern.h"
#include "patternbase.h"
#include "primitives.h"

extern aui_UI		*g_ui;



c3_Ranger::c3_Ranger(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_Ranger()
{
	*retval = aui_Region::InitCommonLdl( id, ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommonLdl( ldlBlock, ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Ranger::InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommonLdl( ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommonLdl( ldlBlock, (MBCHAR *)NULL );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
	
	*retval = CreateButtonsAndThumb(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



c3_Ranger::c3_Ranger(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	AUI_RANGER_TYPE type,
	AUI_RANGER_ORIENTATION orientation,
	MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_Ranger()
{
	*retval = aui_Region::InitCommon( id, x, y, width, height );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Control::InitCommon( ActionFunc, cookie );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_Ranger::InitCommon( type, orientation );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = aui_SoundBase::InitCommon((MBCHAR **) NULL);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = PatternBase::InitCommon( pattern );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
	
	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
	
	*retval = CreateButtonsAndThumb(NULL);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE c3_Ranger::InitCommonLdl( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = InitCommon();
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE c3_Ranger::InitCommon( void )
{
	memset( m_arrows, 0, sizeof( m_arrows ) );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE c3_Ranger::CreateButtonsAndThumb( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode;
	MBCHAR		*patternFilename = NULL;

	if (m_pattern) 
		patternFilename = m_pattern->GetFilename();

	
	aui_Ldl *theLdl = g_ui->GetLdl();
	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	
	sint32 maxButtonSize = 0;

	
	if ( m_type == AUI_RANGER_TYPE_SLIDER
	||   m_type == AUI_RANGER_TYPE_SCROLLER )
	{
		if ( ldlBlock )
		{
			sprintf( block, "%s.%s", ldlBlock, k_AUI_RANGER_LDL_THUMB );

			
			if ( theLdl->GetLdl()->FindDataBlock( block ) )
				m_thumb = new c3_Thumb(
					&errcode,
					aui_UniqueId(),
					block,
					RangerThumbActionCallback,
					this );
		}

		if ( !m_thumb )
			m_thumb = new c3_Thumb(
				&errcode,
				aui_UniqueId(),
				0, 0, 0, 0,
				patternFilename,
				RangerThumbActionCallback,
				this );

		Assert( AUI_NEWOK(m_thumb,errcode) );
		if ( !AUI_NEWOK(m_thumb,errcode) )
			return AUI_ERRCODE_MEMALLOCFAILED;

		AddChild( m_thumb );

		RepositionThumb( FALSE );
	}

	
	if ( m_type == AUI_RANGER_TYPE_SCROLLER
	||   m_type == AUI_RANGER_TYPE_SPINNER )
	{
		if ( m_orientation == AUI_RANGER_ORIENTATION_HORIZONTAL
		||   m_orientation == AUI_RANGER_ORIENTATION_BIDIRECTIONAL )
		{
			if ( ldlBlock )
			{
				sprintf( block, "%s.%s", ldlBlock, k_AUI_RANGER_LDL_INCX );

				if ( theLdl->GetLdl()->FindDataBlock( block ) )
					m_incXButton = new c3_Button(
						&errcode,
						aui_UniqueId(),
						block,
						RangerButtonActionCallback,
						this );
			}

			if ( !m_incXButton ) {
				m_incXButton = new c3_Button(
					&errcode,
					aui_UniqueId(),
					0, 0, 0, 0,
					patternFilename,
					RangerButtonActionCallback,
					this );
			}

			Assert( AUI_NEWOK(m_incXButton,errcode) );
			if ( !AUI_NEWOK(m_incXButton,errcode) )
				return AUI_ERRCODE_MEMALLOCFAILED;

			sint32 i = 2;
			ldl_datablock *dataBlock;

			sprintf( block, "RangerRight" );
			if ((dataBlock = theLdl->GetLdl()->FindDataBlock( block )))
			{
				m_arrows[ i ] = new aui_Static(
					&errcode,
					aui_UniqueId(),
					block );
				Assert( AUI_NEWOK(m_arrows[ i ],errcode) );
				if ( !AUI_NEWOK(m_arrows[ i ],errcode) )
					return AUI_ERRCODE_MEMALLOCFAILED;

				m_arrows[ i ]->SetBlindness( TRUE );
				m_incXButton->AddChild( m_arrows[ i ] );

				
				m_arrows[ i ]->GetDim()->SetParent( m_incXButton );
			}

			AddChild( m_incXButton );

			if ( m_incXButton->Width() > maxButtonSize )
				maxButtonSize = m_incXButton->Width();

			if ( ldlBlock )
			{
				sprintf( block, "%s.%s", ldlBlock, k_AUI_RANGER_LDL_DECX );

				if ( theLdl->GetLdl()->FindDataBlock( block ) )
					m_decXButton = new c3_Button(
						&errcode,
						aui_UniqueId(),
						block,
						RangerButtonActionCallback,
						this );
			}

			if ( !m_decXButton ) {
				m_decXButton = new c3_Button(
					&errcode,
					aui_UniqueId(),
					0, 0, 0, 0,
					patternFilename,
					RangerButtonActionCallback,
					this );
			}

			Assert( AUI_NEWOK(m_decXButton,errcode) );
			if ( !AUI_NEWOK(m_decXButton,errcode) )
				return AUI_ERRCODE_MEMALLOCFAILED;

			i = 3;
			sprintf( block, "RangerLeft" );
			if ((dataBlock = theLdl->GetLdl()->FindDataBlock( block )))
			{
				m_arrows[ i ] = new aui_Static(
					&errcode,
					aui_UniqueId(),
					block );
				Assert( AUI_NEWOK(m_arrows[ i ],errcode) );
				if ( !AUI_NEWOK(m_arrows[ i ],errcode) )
					return AUI_ERRCODE_MEMALLOCFAILED;

				m_arrows[ i ]->SetBlindness( TRUE );
				m_decXButton->AddChild( m_arrows[ i ] );

				
				m_arrows[ i ]->GetDim()->SetParent( m_decXButton );
			}

			AddChild( m_decXButton );

			if ( m_decXButton->Width() > maxButtonSize )
				maxButtonSize = m_decXButton->Width();
		}

		if ( m_orientation == AUI_RANGER_ORIENTATION_VERTICAL
		||   m_orientation == AUI_RANGER_ORIENTATION_BIDIRECTIONAL )
		{
			if ( ldlBlock )
			{
				sprintf( block, "%s.%s", ldlBlock, k_AUI_RANGER_LDL_INCY );

				if ( theLdl->GetLdl()->FindDataBlock( block ) )
					m_incYButton = new c3_Button(
						&errcode,
						aui_UniqueId(),
						block,
						RangerButtonActionCallback,
						this );
			}

			if ( !m_incYButton )
				m_incYButton = new c3_Button(
					&errcode,
					aui_UniqueId(),
					0, 0, 0, 0,
					patternFilename,
					RangerButtonActionCallback,
					this );

			Assert( AUI_NEWOK(m_incYButton,errcode) );
			if ( !AUI_NEWOK(m_incYButton,errcode) )
				return AUI_ERRCODE_MEMALLOCFAILED;

			sint32 i = 0;
			ldl_datablock *dataBlock;

			sprintf( block, "RangerDown" );
			if ((dataBlock = theLdl->GetLdl()->FindDataBlock( block )))
			{
				m_arrows[ i ] = new aui_Static(
					&errcode,
					aui_UniqueId(),
					block );
				Assert( AUI_NEWOK(m_arrows[ i ],errcode) );
				if ( !AUI_NEWOK(m_arrows[ i ],errcode) )
					return AUI_ERRCODE_MEMALLOCFAILED;

				m_arrows[ i ]->SetBlindness( TRUE );
				m_incYButton->AddChild( m_arrows[ i ] );

				
				m_arrows[ i ]->GetDim()->SetParent( m_incYButton );
			}

			AddChild( m_incYButton );

			if ( m_incYButton->Height() > maxButtonSize )
				maxButtonSize = m_incYButton->Height();

			if ( ldlBlock )
			{
				sprintf( block, "%s.%s", ldlBlock, k_AUI_RANGER_LDL_DECY );

				if ( theLdl->GetLdl()->FindDataBlock( block ) )
					m_decYButton = new c3_Button(
						&errcode,
						aui_UniqueId(),
						block,
						RangerButtonActionCallback,
						this );
			}

			if ( !m_decYButton )
				m_decYButton = new c3_Button(
					&errcode,
					aui_UniqueId(),
					0, 0, 0, 0,
					patternFilename,
					RangerButtonActionCallback,
					this );

			Assert( AUI_NEWOK(m_decYButton,errcode) );
			if ( !AUI_NEWOK(m_decYButton,errcode) )
				return AUI_ERRCODE_MEMALLOCFAILED;

			i = 1;
			sprintf( block, "RangerUp" );
			if ((dataBlock = theLdl->GetLdl()->FindDataBlock( block )))
			{
				m_arrows[ i ] = new aui_Static(
					&errcode,
					aui_UniqueId(),
					block );
				Assert( AUI_NEWOK(m_arrows[ i ],errcode) );
				if ( !AUI_NEWOK(m_arrows[ i ],errcode) )
					return AUI_ERRCODE_MEMALLOCFAILED;

				m_arrows[ i ]->SetBlindness( TRUE );
				m_decYButton->AddChild( m_arrows[ i ] );

				
				m_arrows[ i ]->GetDim()->SetParent( m_decYButton );
			}

			AddChild( m_decYButton );

			if ( m_decYButton->Height() > maxButtonSize )
				maxButtonSize = m_decYButton->Height();
		}

		if ( maxButtonSize )
			SetButtonSize( maxButtonSize ); 
		else
			RepositionButtons();
	}

	return AUI_ERRCODE_OK;
}



c3_Ranger::~c3_Ranger()
{
	for ( sint32 i = 0; i < 4; i++ )
	if ( m_arrows[ i ] )
	{
		aui_Ldl::Remove(m_arrows[i]);

		delete m_arrows[ i ];
		m_arrows[ i ] = NULL;
	}
}



AUI_ERRCODE c3_Ranger::RepositionButtons( void )
{
	AUI_ERRCODE errcode = aui_Ranger::RepositionButtons();

	
	
	for ( sint32 i = 0; i < 4; i++ )
	if ( m_arrows[ i ] )
		m_arrows[ i ]->Adjust();

	return errcode;
}



AUI_ERRCODE c3_Ranger::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if ( m_pattern ) m_pattern->Draw( surface, &rect );

	primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
