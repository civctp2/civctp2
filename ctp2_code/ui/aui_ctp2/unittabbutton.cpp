

#include "c3.h"

#include "aui.h"
#include "aui_Window.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"

#include "Pattern.h"

#include "primitives.h"
#include "c3_coloriconbutton.h"
#include "thermometer.h"

#include "unit.h"
#include "unitrec.h"
#include "battleorderboxactions.h"
#include "c3_coloredstatic.h"



#include "uiutils.h"

#include "unittabbutton.h"

extern	ColorSet	*g_colorSet;


UnitTabButton::UnitTabButton(AUI_ERRCODE *retval, 
					   uint32 id, 
					   MBCHAR *ldlBlock, 
					   ControlActionCallback *ActionFunc, 
					   void *cookie)
	: 
	aui_Control(retval, id, ldlBlock, ActionFunc, cookie),
	PatternBase(ldlBlock, NULL),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_ImageBase( ldlBlock )
{
	
	SetDrawMask( k_AUI_REGION_DRAWFLAG_UPDATE );

	InitCommon();
}

UnitTabButton::UnitTabButton(AUI_ERRCODE *retval, 
					   uint32 id, 
					   sint32 x, 
					   sint32 y, 
					   sint32 width, 
					   sint32 height,
					   MBCHAR *pattern,
					   sint32 barHeight,
					   ControlActionCallback *ActionFunc, 
					   void *cookie)
	:
	aui_Control(retval, id, x, y, width, height, ActionFunc, cookie),
	PatternBase(pattern),
	aui_TextBase(NULL),
	aui_ImageBase( (sint32)0 )
{
	
	SetDrawMask( k_AUI_REGION_DRAWFLAG_UPDATE );

	m_barHeight = barHeight;

	InitCommon();
}


AUI_ERRCODE UnitTabButton::InitCommon( void )
{
	AUI_ERRCODE errcode;
	MBCHAR	buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR	ldlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	sint32 i;

	
	m_healthBar = NULL;
	m_button = NULL;
	m_fortify = NULL;
	m_veteran = NULL;
	m_arrow = NULL;

	for ( i = 0;i < k_CARGO_CAPACITY;i++ ) {
		m_cargo[i] = NULL;
	}

	sprintf( buttonBlock, "UnitTabButton" );

	
	m_button = new c3_ColorIconButton(&errcode, aui_UniqueId(), 0, m_barHeight, 
			m_width, m_height - m_barHeight, "upba0119.tga", "" );

	Assert(m_button);
	if (!m_button) return AUI_ERRCODE_MEMALLOCFAILED;

	m_button->SetImageBltType(AUI_IMAGEBASE_BLTTYPE_STRETCH);

	
	m_button->ShrinkToFit(TRUE);

	errcode = AddSubControl(m_button);
	Assert(errcode == AUI_ERRCODE_OK);

		
		sprintf( ldlBlock, "%s.%s", buttonBlock, "Arrow" );
		m_arrow = new c3_Static( &errcode, aui_UniqueId(), ldlBlock );
		Assert( AUI_NEWOK( m_arrow, errcode) );
		if ( !AUI_NEWOK(m_arrow, errcode) ) return errcode;

		m_arrow->SetBlindness( TRUE );

		errcode = m_button->AddSubControl( m_arrow );
		Assert( errcode == AUI_ERRCODE_OK );

		
		sprintf( ldlBlock, "%s.%s", buttonBlock, "Fortify" );
		m_fortify = new c3_Static( &errcode, aui_UniqueId(), ldlBlock );
		Assert( AUI_NEWOK( m_fortify, errcode) );
		if ( !AUI_NEWOK(m_fortify, errcode) ) return errcode;

		m_fortify->SetBlindness( TRUE );

		errcode = m_button->AddSubControl( m_fortify );
		Assert( errcode == AUI_ERRCODE_OK );

		
		sprintf( ldlBlock, "%s.%s", buttonBlock, "Veteran" );
		m_veteran = new c3_Static( &errcode, aui_UniqueId(), ldlBlock );
		Assert( AUI_NEWOK( m_veteran, errcode) );
		if ( !AUI_NEWOK(m_veteran, errcode) ) return errcode;

		m_veteran->SetBlindness( TRUE );

		errcode = m_button->AddSubControl( m_veteran );
		Assert( errcode == AUI_ERRCODE_OK );

		sprintf( ldlBlock, "%s.%s", buttonBlock, "Cargo" );
		
		for ( i = 0;i < k_CARGO_CAPACITY;i++ ) {
			m_cargo[i] = new c3_ColoredStatic( &errcode, aui_UniqueId(), ldlBlock );
			Assert( AUI_NEWOK( m_cargo[i], errcode) );
			if ( !AUI_NEWOK(m_cargo[i], errcode) ) return errcode;

			m_cargo[i]->SetBlindness( TRUE );

			errcode = m_button->AddSubControl( m_cargo[i] );
			Assert( errcode == AUI_ERRCODE_OK );

			m_cargo[i]->Move( (m_cargo[i]->Width() + k_CARGO_OFFSET) * i, 
				m_button->Height() - m_cargo[i]->Height() );
			m_cargo[i]->SetColor( COLOR_GREEN );
		}

		


	
	m_healthBar = new Thermometer( &errcode, aui_UniqueId(), 0, 0,
		m_width, m_barHeight, "chart.tga", 50 );

	errcode = AddSubControl( m_healthBar );
	Assert( errcode == AUI_ERRCODE_OK );



	return errcode;
}


UnitTabButton::~UnitTabButton()
{
	RemoveControl( m_button );
	RemoveControl( m_healthBar );
	RemoveControl( m_arrow );
	RemoveControl( m_fortify );
	RemoveControl( m_veteran );

	for ( sint32 i = 0;i < k_CARGO_CAPACITY;i++ ) {
		RemoveControl( m_cargo[i] );
	}
}


AUI_ERRCODE UnitTabButton::DrawThis(aui_Surface *surface, sint32 x, sint32 y)
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if ( m_pattern ) {
		if ( m_srcWidthPix || m_srcHeightPix ) {
			RECT srcRect = { m_srcX, m_srcY, m_srcX + m_srcWidthPix, m_srcY + m_srcHeightPix };
			m_pattern->Draw( surface, &rect, &srcRect );
		}
		else {
			m_pattern->Draw( surface, &rect );
		}
	}




	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

sint32 UnitTabButton::UpdateData( Unit *unit )
{
	sint32 i;

	if ( !unit ) {
		RemoveSubControl( m_healthBar->Id() );
		m_button->RemoveSubControl( m_arrow->Id() );
		m_button->RemoveSubControl( m_fortify->Id() );
		m_button->RemoveSubControl( m_veteran->Id() );

		for ( i = 0;i < k_CARGO_CAPACITY;i++ ) {
			m_button->RemoveSubControl( m_cargo[i]->Id() );
		}
	}
	else {
		
		AddSubControl( m_healthBar );
		sint32 healthPercent  = (sint32)( unit->GetHP() * 100  / unit->GetDBRec()->GetMaxHP() );
		m_healthBar->SetPercentFilled( healthPercent );

		
		if ( unit->GetMovementPoints() ) {
			m_button->AddSubControl( m_arrow );
		}
		else {
			m_button->RemoveSubControl( m_arrow->Id() );
		}

		
		if ( unit->IsVeteran() ) {
			m_button->AddSubControl( m_veteran );
			m_fortify->SetTextColor( g_colorSet->GetColorRef(COLOR_WHITE) );
		}
		else {
			m_button->RemoveSubControl( m_veteran->Id() );
		}

		
		if ( unit->IsEntrenched() ) {
			m_button->AddSubControl( m_fortify );
			
			m_fortify->SetTextColor( g_colorSet->GetColorRef(COLOR_WHITE) );
		}
		else if ( unit->IsAsleep() ) {
			m_button->AddSubControl( m_fortify );
			
			m_fortify->SetTextColor( g_colorSet->GetColorRef(COLOR_WHITE) );
		}
		else if ( unit->IsEntrenching() ) {
			m_button->AddSubControl( m_fortify );
			
			m_fortify->SetTextColor( g_colorSet->GetColorRef(COLOR_GRAY) );
		}
		else {
			m_button->RemoveSubControl( m_fortify->Id() );
		}

		
		const UnitRecord *rec = unit->GetDBRec();
		sint32 maxCargo;
		if(rec->GetCargoDataPtr()) {
			maxCargo = rec->GetCargoDataPtr()->GetMaxCargo();
		} else {
			maxCargo = 0;
		}
		sint32 currentCargo = unit->GetNumCarried();

		
		if ( unit->GetDBRec()->GetCanCarry() ) {
			for ( i = 0;i < k_CARGO_CAPACITY;i++ ) {
				
				if ( i < maxCargo ) {
					m_button->AddSubControl( m_cargo[i] );
					
					if ( i < currentCargo ) {
						m_cargo[i]->SetColor( COLOR_GREEN );
					}
					else {
						m_cargo[i]->SetColor( COLOR_WHITE );
					}
				}
				else {
					m_button->RemoveSubControl( m_cargo[i]->Id() );
				}
			}
		}
	}

	return 0;
}


