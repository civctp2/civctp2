










#include "c3.h"


#ifdef __AUI_USE_DIRECTX__

#include <ddraw.h>

#include "aui_ui.h"
#include "aui_uniqueid.h"

#include "aui_directx.h"

#include "display.h"



LPDIRECTDRAW aui_DirectX::m_lpdd =		NULL;
LPDIRECTINPUT aui_DirectX::m_lpdi =		NULL;
sint32 aui_DirectX::m_directXRefCount =	0;
uint32 aui_DirectX::m_directXClassId =	aui_UniqueId();
BOOL aui_DirectX::m_b_created_dd = FALSE;
BOOL aui_DirectX::m_b_created_di = FALSE;

extern DisplayDevice	g_displayDevice;


void aui_DirectX::DD( LPDIRECTDRAW lpdd )
{
	Assert( !m_lpdd );
	if( !m_lpdd )
	{
		m_lpdd = lpdd;
	}
}



void aui_DirectX::DI( LPDIRECTINPUT lpdi )
{
	Assert( !m_lpdi );
	if( !m_lpdi )
	{
		m_lpdi = lpdi;
	}
}


AUI_ERRCODE aui_DirectX::InitCommon( BOOL useExclusiveMode )
{
	m_exclusiveMode = useExclusiveMode;

	
	Assert( g_ui->DXVer() != 0 );
	if ( !g_ui->DXVer() ) return AUI_ERRCODE_HACK;

	
	if ( !m_lpdd )
	{
		
		if (!m_lpdd) {
			HRESULT hr = DirectDrawCreate( g_displayDevice.lpGUID, &m_lpdd, NULL );
			Assert( hr == DD_OK );
			if ( hr != DD_OK ) return AUI_ERRCODE_CREATEFAILED;
		}

		m_b_created_dd = TRUE;
		
		
	}

	
	if ( !m_lpdi )
	{
		HRESULT hr = DirectInputCreate(
			g_ui->TheHINSTANCE(),
			g_ui->DXVer() < 0x500 ? 0x0300 : DIRECTINPUT_VERSION,
			&m_lpdi,
			NULL );
		Assert( hr == DD_OK );
		if ( hr != DI_OK ) return AUI_ERRCODE_CREATEFAILED;
		m_b_created_di = TRUE;
	}

	return AUI_ERRCODE_OK;
}



aui_DirectX::~aui_DirectX()
{
	if ( !--m_directXRefCount )
	{
		if ( m_lpdd && m_b_created_dd )
		{
			m_lpdd->Release();
			m_lpdd = NULL;
			m_b_created_dd = FALSE;
		}

		if ( m_lpdi && m_b_created_di )
		{
			m_lpdi->Release();
			m_lpdi = NULL;
			m_b_created_di = FALSE;
		}
	}
}


#endif 
