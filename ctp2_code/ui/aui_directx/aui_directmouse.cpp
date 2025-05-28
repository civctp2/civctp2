//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : DirectX mouse handler
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
// __AUI_USE_DIRECTX__
// - When defined, use DirectX.
// - When not defined, this file is not used at all.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Continue reading from the buffer when an overflow is reported.
//
//----------------------------------------------------------------------------

#include "c3.h"

#ifdef __AUI_USE_DIRECTX__

#include "aui_ui.h"
#include "aui_Factory.h"
#include "aui_surface.h"
#include "aui_rectangle.h"

#include "aui_directmouse.h"

#include "profileDB.h"
#include "civapp.h"

extern ProfileDB *g_theProfileDB;
extern CivApp    *g_civApp;

sint32 aui_Mouse::m_mouseRefCount = 0;
LPCRITICAL_SECTION aui_DirectMouse::m_lpcs = NULL;
#define k_AUI_MOUSE_THREAD_SLEEP_TIME	10

aui_DirectMouse::aui_DirectMouse(
	AUI_ERRCODE *retval,
	const MBCHAR *ldlBlock,
	BOOL useExclusiveMode )
:
	aui_Mouse		(retval, ldlBlock),
	aui_DirectInput	(retval, useExclusiveMode)
{
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateDirectMouse();

	Assert(*retval != AUI_ERRCODE_CREATEFAILED);
	Assert(*retval != AUI_ERRCODE_SETFORMATFAILED);
	Assert(*retval != AUI_ERRCODE_SETCOOPLEVELFAILED);
	Assert(*retval != AUI_ERRCODE_SETEVENTFAILED);
	Assert(*retval != AUI_ERRCODE_SETPROPERTYFAILED);
	Assert( AUI_SUCCESS(*retval) );

	// From aui_Mouse::InitCommon start
	m_privateMix = NULL;
	m_pickup = NULL;
	m_prevPickup = NULL;

	m_thread = NULL;
	m_threadId = 0;
	m_threadEvent = NULL;
	m_terminateEvent = NULL;
	m_suspendEvent = NULL;
	m_resumeEvent = NULL;
	m_replyEvent = NULL;
	GetCursorPos(&m_data.position);

	if (!m_mouseRefCount++)
	{
		m_lpcs = new CRITICAL_SECTION;
		Assert(m_lpcs != NULL);
		if (m_lpcs)
			InitializeCriticalSection(m_lpcs);
		else
			Assert(AUI_SUCCESS(AUI_ERRCODE_MEMALLOCFAILED));
	}
	// From aui_Mouse::InitCommon end
}

aui_DirectMouse::~aui_DirectMouse()
{
	DestroyPrivateBuffers();

	if (!--m_mouseRefCount)
	{
		if (m_lpcs)
		{
			DeleteCriticalSection(m_lpcs);
			delete m_lpcs;
			m_lpcs = NULL;
		}
	}
}

AUI_ERRCODE aui_DirectMouse::Start(void)
{
	m_curCursor = m_cursors + m_firstIndex;
	ActivateCursor(*m_curCursor);

	CreatePrivateBuffers();

	m_thread = CreateThread(NULL, 0, MouseThreadProc, (LPVOID)this, 0, &m_threadId);
	if (m_thread)
	{
		SetThreadPriority(m_thread, THREAD_PRIORITY_NORMAL);

		m_threadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_terminateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_suspendEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_resumeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_replyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		Acquire();
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_DirectMouse::End(void)
{
	Unacquire();

	if (m_thread)
	{
		if (m_threadEvent && m_terminateEvent)
		{
			SetEvent(m_terminateEvent);
			SetEvent(m_threadEvent);

			if (WaitForSingleObject(m_thread, 2000) != WAIT_OBJECT_0)
				TerminateThread(m_thread, 1);

			CloseHandle(m_threadEvent);
			m_threadEvent = NULL;

			CloseHandle(m_terminateEvent);
			m_terminateEvent = NULL;
		}
		else
			TerminateThread(m_thread, 1);

		Erase();

		if (m_suspendEvent)
		{
			CloseHandle(m_suspendEvent);
			m_suspendEvent = NULL;
		}

		if (m_resumeEvent)
		{
			CloseHandle(m_resumeEvent);
			m_resumeEvent = NULL;
		}

		if (m_replyEvent)
		{
			CloseHandle(m_replyEvent);
			m_replyEvent = NULL;
		}

		CloseHandle(m_thread);

		m_threadId = 0;
	}

	SetCursorPos(m_data.position.x, m_data.position.y);

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_DirectMouse::Suspend(BOOL eraseCursor)
{
	if (!m_thread) return AUI_ERRCODE_NOTHREAD;

	if (m_suspendCount)
	{
		m_suspendCount++;
		return AUI_ERRCODE_OK;
	}
	SetEvent(m_suspendEvent);
	SetEvent(m_threadEvent);

	if (WaitForSingleObject(m_replyEvent, INFINITE) == WAIT_OBJECT_0)
	{
		ResetEvent(m_replyEvent);

		if (SuspendThread(m_thread) != 0xffffffff)
		{

			if (eraseCursor)
			{
				Erase();
				Unacquire();
			}

			m_suspendCount++;

			return AUI_ERRCODE_OK;
		}
	}

	return AUI_ERRCODE_SUSPENDFAILED;
}

AUI_ERRCODE aui_DirectMouse::Resume(void)
{
	if (!m_thread) return AUI_ERRCODE_NOTHREAD;

	if (m_suspendCount == 0) return AUI_ERRCODE_OK;

	if (m_suspendCount > 1)
	{
		m_suspendCount--;
		return AUI_ERRCODE_OK;
	}

	if (ResumeThread(m_thread) != 0xffffffff)
	{

		if (!IsAcquired())
		{
			Acquire();
		}

		SetEvent(m_resumeEvent);

		m_suspendCount = 0;

		return AUI_ERRCODE_OK;
	}

	return AUI_ERRCODE_RESUMEFAILED;
}

inline BOOL aui_DirectMouse::ShouldTerminateThread(void)
{
	if (WaitForSingleObject(m_threadEvent, 0) == WAIT_OBJECT_0)
	{
		ResetEvent(m_threadEvent);

		if (WaitForSingleObject(m_terminateEvent, 0) == WAIT_OBJECT_0)
		{
			ResetEvent(m_terminateEvent);
			return TRUE;
		}

		if (WaitForSingleObject(m_suspendEvent, 0) == WAIT_OBJECT_0)
		{
			ResetEvent(m_suspendEvent);

			SetEvent(m_replyEvent);
			WaitForSingleObject(m_resumeEvent, INFINITE);

			ResetEvent(m_resumeEvent);
		}
	}
	return FALSE;
}

DWORD WINAPI MouseThreadProc(LPVOID param)
{
	aui_DirectMouse *mouse = (aui_DirectMouse *)param;

	while (!mouse->ShouldTerminateThread())
	{
		Sleep(k_AUI_MOUSE_THREAD_SLEEP_TIME);

		// For that something is wrong with the mouse delay
//		std::this_thread::sleep_for(std::chrono::milliseconds(k_AUI_MOUSE_THREAD_SLEEP_TIME));

		mouse->HandleAnim();
		mouse->GetInput();
		mouse->ReactToInput();
		mouse->ManipulateInputs(mouse->GetLatestMouseEvent(), TRUE);
	}

	return 0;
}

AUI_ERRCODE aui_DirectMouse::CreateDirectMouse( void )
{
	HRESULT hr;

	hr = m_lpdi->CreateDevice( GUID_SysMouse, &m_lpdid, NULL );
	if ( hr != DI_OK ) return AUI_ERRCODE_CREATEFAILED;

	hr = m_lpdid->SetDataFormat( &c_dfDIMouse );
	if ( hr != DI_OK ) return AUI_ERRCODE_SETFORMATFAILED;

	uint32 coopFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;
	if ( m_exclusiveMode )
		coopFlags = DISCL_EXCLUSIVE | DISCL_FOREGROUND;

	hr = m_lpdid->SetCooperativeLevel( g_ui->TheHWND(), coopFlags );
	if ( hr != DI_OK ) return AUI_ERRCODE_SETCOOPLEVELFAILED;

	hr = m_lpdid->SetEventNotification( m_inputEvent );
	if ( hr != DI_OK ) return AUI_ERRCODE_SETEVENTFAILED;

	static DIPROPDWORD dipdw_buffersize =
	{
		{
			sizeof( DIPROPDWORD ),
			sizeof( DIPROPHEADER ),
			0,
			DIPH_DEVICE,
		},
		k_MOUSE_MAXINPUT
	};

	hr = m_lpdid->SetProperty( DIPROP_BUFFERSIZE, &dipdw_buffersize.diph );
	if ( hr != DI_OK ) return AUI_ERRCODE_SETPROPERTYFAILED;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_DirectMouse::CreatePrivateBuffers(void)
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	DestroyPrivateBuffers();

	m_privateMix = aui_Factory::new_Surface(errcode, k_MOUSE_MAXSIZE, k_MOUSE_MAXSIZE);
	Assert(AUI_NEWOK(m_privateMix, errcode));
	if (!AUI_NEWOK(m_privateMix, errcode)) return errcode;

	m_pickup = aui_Factory::new_Surface(errcode, k_MOUSE_MAXSIZE, k_MOUSE_MAXSIZE);
	Assert(AUI_NEWOK(m_pickup, errcode));
	if (!AUI_NEWOK(m_pickup, errcode)) return errcode;

	m_prevPickup = aui_Factory::new_Surface(errcode, k_MOUSE_MAXSIZE, k_MOUSE_MAXSIZE);
	Assert(AUI_NEWOK(m_prevPickup, errcode));
	if (!AUI_NEWOK(m_prevPickup, errcode)) return errcode;

	return errcode;
}

void aui_DirectMouse::DestroyPrivateBuffers(void)
{
	delete m_privateMix;
	m_privateMix = NULL;

	delete m_pickup;
	m_pickup = NULL;

	delete m_prevPickup;
	m_prevPickup = NULL;
}

AUI_ERRCODE aui_DirectMouse::GetInput( void )
{
	if ( !m_lpdid ) return AUI_ERRCODE_NODIRECTINPUTDEVICE;


	m_data.time = GetTickCount();

	bool haveMoves = false;

	for ( sint32 numInputs = 200; numInputs; numInputs-- )
	{
		DWORD numElements = 1;
		static DIDEVICEOBJECTDATA od[ 1 ];
		DIDEVICEOBJECTDATA *ptrOd = od;

		HRESULT hr = m_lpdid->GetDeviceData(
			sizeof( DIDEVICEOBJECTDATA ),
			ptrOd,
			&numElements,
			0 );

		switch(hr)
		{
			case DI_BUFFEROVERFLOW:
			case DI_OK:
				break;

			case DIERR_NOTACQUIRED:
				return AUI_ERRCODE_NOTACQUIRED;

			case DIERR_INPUTLOST:
				Acquire();
				return AUI_ERRCODE_INPUTLOST;

			default:
				return AUI_ERRCODE_GETDEVICEDATAFAILED;
		}

		if ( numElements == 0 )
		{
			if(haveMoves)
				return AUI_ERRCODE_OK;
			else
				return AUI_ERRCODE_NOINPUT;
		}

		switch ( ptrOd->dwOfs )
		{
			case DIMOFS_X:
				m_data.position.x += sint32(m_sensitivity * sint32(ptrOd->dwData));

				if(m_data.position.x >= g_ui->SecondaryWidth())
					m_data.position.x = g_ui->SecondaryWidth() - 1;
				else if(m_data.position.x < 0)
					m_data.position.x = 0;

				m_data.flags = m_flags;
				haveMoves = true;
				break;

			case DIMOFS_Y:
				m_data.position.y += sint32(m_sensitivity * sint32(ptrOd->dwData));

				if(m_data.position.y >= g_ui->SecondaryHeight())
					m_data.position.y = g_ui->SecondaryHeight() - 1;
				else if(m_data.position.y < 0)
					m_data.position.y = 0;

				m_data.flags = m_flags;
				haveMoves = true;
				break;

			case DIMOFS_BUTTON0:
				if(g_theProfileDB && g_theProfileDB->GetLeftHandedMouse())
				{
					m_data.rbutton = uint8(ptrOd->dwData) & 0x80;
					m_data.flags = m_flags;
				}
				else
				{
					m_data.lbutton = uint8(ptrOd->dwData) & 0x80;
					m_data.flags = m_flags;
				}
				return AUI_ERRCODE_OK;

			case DIMOFS_BUTTON1:
				if(g_theProfileDB && g_theProfileDB->GetLeftHandedMouse())
				{
					m_data.lbutton = uint8(ptrOd->dwData) & 0x80;
					m_data.flags = m_flags;
				}
				else
				{
					m_data.rbutton = uint8(ptrOd->dwData) & 0x80;
					m_data.flags = m_flags;
				}
				return AUI_ERRCODE_OK;

			case DIMOFS_BUTTON2:
				m_data.mbutton = uint8(ptrOd->dwData) & 0x80;
				m_data.flags = m_flags;
				return AUI_ERRCODE_OK;

			case DIMOFS_BUTTON3:
				m_data.tbutton = uint8(ptrOd->dwData) & 0x80;
				m_data.flags = m_flags;
				return AUI_ERRCODE_OK;

			case DIMOFS_BUTTON4:
				m_data.ubutton = uint8(ptrOd->dwData) & 0x80;
				m_data.flags = m_flags;
				return AUI_ERRCODE_OK;

			case DIMOFS_BUTTON5:
				m_data.vbutton = uint8(ptrOd->dwData) & 0x80;
				m_data.flags = m_flags;
				return AUI_ERRCODE_OK;

			case DIMOFS_BUTTON6:
				m_data.wbutton = uint8(ptrOd->dwData) & 0x80;
				m_data.flags = m_flags;
				return AUI_ERRCODE_OK;

			case DIMOFS_BUTTON7:
				m_data.xbutton = uint8(ptrOd->dwData) & 0x80;
				m_data.flags = m_flags;
				return AUI_ERRCODE_OK;

			case DIK_LSHIFT:
				if(uint8(ptrOd->dwData) & 0x80)
				{
					SetLeftShift();
				}
				else
				{
					UnsetLeftShift();
				}

				m_data.flags = m_flags;
				return AUI_ERRCODE_OK;

			case DIK_RSHIFT:
				if(uint8(ptrOd->dwData) & 0x80)
				{
					SetRightShift();
				}
				else
				{
					UnsetRightShift();
				}

				m_data.flags = m_flags;
				return AUI_ERRCODE_OK;

			case DIK_LCONTROL:
				if(uint8(ptrOd->dwData) & 0x80)
				{
					SetLeftControl();
				}
				else
				{
					UnsetLeftControl();
				}

				m_data.flags = m_flags;
				return AUI_ERRCODE_OK;

			case DIK_RCONTROL:
				if(uint8(ptrOd->dwData) & 0x80)
				{
					SetRightControl();
				}
				else
				{
					UnsetRightControl();
				}

				m_data.flags = m_flags;
				return AUI_ERRCODE_OK;
		}
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_DirectMouse::ReactToInput(void)
{
	if (IsHidden() || ((*m_curCursor) == NULL)) return AUI_ERRCODE_OK;

	POINT hotspot;
	(*m_curCursor)->GetHotspot(hotspot);

	POINT image =
	{
		m_data.position.x - hotspot.x,
		m_data.position.y - hotspot.y
	};

	static POINT prevImage = image;
	if (m_reset)
		prevImage = image;

	sint32 dx = image.x - prevImage.x;
	sint32 dy = image.y - prevImage.y;

	prevImage.x = image.x;
	prevImage.y = image.y;

	RECT mixRect =
	{
		image.x,
		image.y,
		image.x + k_MOUSE_MAXSIZE,
		image.y + k_MOUSE_MAXSIZE
	};

	RECT unclippedMixRect = mixRect;

	Rectangle_Clip(&mixRect, &m_clip);

	RECT rect =
	{
		mixRect.left - image.x,
		mixRect.top - image.y,
		mixRect.right - image.x,
		mixRect.bottom - image.y
	};

	static RECT prevRect = { 0, 0, 0, 0 };
	static RECT prevUnclippedMixRect = { 0, 0, 0, 0 };
	if (m_reset)
	{
		memset(&prevRect, 0, sizeof(prevRect));
		memset(&prevUnclippedMixRect, 0, sizeof(prevUnclippedMixRect));
	}

	AUI_ERRCODE errcode = g_ui->TheBlitter()->Blt(
		m_pickup,
		rect.left,
		rect.top,
		g_ui->Secondary(),
		&mixRect,
		k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST);
	Assert(errcode == AUI_ERRCODE_OK);

	if (m_reset) m_reset = FALSE;
	else if ((-k_MOUSE_MAXSIZE < dx && dx < k_MOUSE_MAXSIZE)
		&& (-k_MOUSE_MAXSIZE < dy && dy < k_MOUSE_MAXSIZE))
	{
		sint32 fixX = 0;
		sint32 fixY = 0;
		RECT fixRect = { 0, 0, k_MOUSE_MAXSIZE, k_MOUSE_MAXSIZE };

		if (dx > 0)
		{
			fixRect.left += dx;
		}
		else
		{
			fixX = -dx;
			fixRect.right += dx;
		}

		if (dy > 0)
		{
			fixRect.top += dy;
		}
		else
		{
			fixY = -dy;
			fixRect.bottom += dy;
		}

		errcode = g_ui->TheBlitter()->Blt(
			m_pickup,
			fixX,
			fixY,
			m_prevPickup,
			&fixRect,
			k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST);
		Assert(errcode == AUI_ERRCODE_OK);
	}

	errcode = g_ui->TheBlitter()->Blt(
		m_privateMix,
		rect.left,
		rect.top,
		m_pickup,
		&rect,
		k_AUI_BLITTER_FLAG_COPY);
	Assert(errcode == AUI_ERRCODE_OK);

	errcode = g_ui->TheBlitter()->Blt(
		m_privateMix,
		rect.left,
		rect.top,
		(*m_curCursor)->TheSurface(),
		&rect,
		k_AUI_BLITTER_FLAG_CHROMAKEY);
	Assert(errcode == AUI_ERRCODE_OK);

	errcode = g_ui->BltToSecondary(
		mixRect.left,
		mixRect.top,
		m_privateMix,
		&rect,
		k_AUI_BLITTER_FLAG_COPY);
	Assert(errcode == AUI_ERRCODE_OK);

	static RECT eraser1, eraser2;
	Rectangle_GetErasers(
		&eraser1, &eraser2,
		unclippedMixRect.left, unclippedMixRect.top,
		dx, dy,
		unclippedMixRect.right - unclippedMixRect.left,
		unclippedMixRect.bottom - unclippedMixRect.top);

	Rectangle_Clip(&eraser1, &m_clip);
	Rectangle_Clip(&eraser2, &m_clip);

	SetRect(
		&prevRect,
		eraser1.left - prevUnclippedMixRect.left,
		eraser1.top - prevUnclippedMixRect.top,
		eraser1.right - prevUnclippedMixRect.left,
		eraser1.bottom - prevUnclippedMixRect.top);

	errcode = g_ui->BltToSecondary(
		eraser1.left,
		eraser1.top,
		m_prevPickup,
		&prevRect,
		k_AUI_BLITTER_FLAG_COPY);
	Assert(errcode == AUI_ERRCODE_OK);

	SetRect(
		&prevRect,
		eraser2.left - prevUnclippedMixRect.left,
		eraser2.top - prevUnclippedMixRect.top,
		eraser2.right - prevUnclippedMixRect.left,
		eraser2.bottom - prevUnclippedMixRect.top);

	errcode = g_ui->BltToSecondary(
		eraser2.left,
		eraser2.top,
		m_prevPickup,
		&prevRect,
		k_AUI_BLITTER_FLAG_COPY);
	Assert(errcode == AUI_ERRCODE_OK);

	errcode = g_ui->BltSecondaryToPrimary(k_AUI_BLITTER_FLAG_COPY);
	Assert(errcode == AUI_ERRCODE_OK);

	aui_Surface *tempSurf = m_prevPickup;
	m_prevPickup = m_pickup;
	m_pickup = tempSurf;

	CopyRect(&prevRect, &rect);
	CopyRect(&prevUnclippedMixRect, &unclippedMixRect);

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE	aui_DirectMouse::BltWindowToPrimary(aui_Window *window)
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	sint32 windowX = window->X();
	sint32 windowY = window->Y();
	aui_Surface *windowSurface = window->TheSurface();
	aui_DirtyList *windowDirtyList = window->GetDirtyList();

	ListPos position = windowDirtyList->GetHeadPosition();
	for (sint32 j = windowDirtyList->L(); j; j--)
	{

		RECT *windowDirtyRect = windowDirtyList->GetNext(position);

		RECT screenDirtyRect = *windowDirtyRect;
		OffsetRect(&screenDirtyRect, windowX, windowY);

		POINT hotspot;
		(*m_curCursor)->GetHotspot(hotspot);

		POINT image = m_data.position;
		image.x -= hotspot.x;
		image.y -= hotspot.y;

		RECT mixRect =
		{
			image.x,
			image.y,
			image.x + k_MOUSE_MAXSIZE,
			image.y + k_MOUSE_MAXSIZE
		};

		Rectangle_Clip(&mixRect, &m_clip);
		Rectangle_Clip(&mixRect, &screenDirtyRect);

		RECT rect =
		{
			mixRect.left - image.x,
			mixRect.top - image.y,
			mixRect.right - image.x,
			mixRect.bottom - image.y
		};

		RECT windowMixRect = mixRect;
		OffsetRect(&windowMixRect, -windowX, -windowY);

		errcode = g_ui->TheBlitter()->Blt(
			m_prevPickup,
			rect.left,
			rect.top,
			windowSurface,
			&windowMixRect,
			k_AUI_BLITTER_FLAG_COPY);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->Blt(
			windowSurface,
			windowMixRect.left,
			windowMixRect.top,
			(*m_curCursor)->TheSurface(),
			&rect,
			k_AUI_BLITTER_FLAG_CHROMAKEY);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

#ifdef _DEBUG

		static FILE *f = fopen("__seisureblit__", "rb");
		if (f)
		{
			g_ui->ColorBltToSecondary(
				&screenDirtyRect,
				RGB(255, 255, 255),
				0);
			Sleep(20);
			fclose(f);
		}
#endif

		errcode = g_ui->BltToSecondary(
			screenDirtyRect.left,
			screenDirtyRect.top,
			windowSurface,
			windowDirtyRect,
			k_AUI_BLITTER_FLAG_COPY);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->Blt(
			windowSurface,
			windowMixRect.left,
			windowMixRect.top,
			m_prevPickup,
			&rect,
			k_AUI_BLITTER_FLAG_COPY);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}
	}

	return retcode;
}

AUI_ERRCODE	aui_DirectMouse::BltDirtyRectInfoToPrimary(void)
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

	AUI_ERRCODE errcode;

	if (g_civApp->IsInBackground()) return retcode;

	tech_WLList<aui_UI::DirtyRectInfo *> *driList =
		g_ui->GetDirtyRectInfoList();

	uint32 blitFlags;
	LPVOID primaryBuf = NULL;

	if (g_theProfileDB && g_theProfileDB->IsUseDirectXBlitter())
	{
		blitFlags = k_AUI_BLITTER_FLAG_COPY;
	}
	else
	{
		blitFlags = k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST;
		errcode = g_ui->Secondary()->Lock(NULL, &primaryBuf, 0);
		Assert(errcode == AUI_ERRCODE_OK);
	}

	ListPos position = driList->GetHeadPosition();
	for (sint32 j = driList->L(); j; j--)
	{
		aui_UI::DirtyRectInfo *dri = driList->GetNext(position);

		aui_Window *window = dri->window;

		sint32 windowX = window->X();
		sint32 windowY = window->Y();
		aui_Surface *windowSurface = window->TheSurface();

		if (!windowSurface) continue;

		if (g_civApp->IsInBackground()) continue;

		RECT *windowDirtyRect = &dri->rect;

		RECT screenDirtyRect = *windowDirtyRect;
		OffsetRect(&screenDirtyRect, windowX, windowY);

		POINT hotspot;
		(*m_curCursor)->GetHotspot(hotspot);

		POINT image = m_data.position;
		image.x -= hotspot.x;
		image.y -= hotspot.y;

		RECT mixRect =
		{
			image.x,
			image.y,
			image.x + k_MOUSE_MAXSIZE,
			image.y + k_MOUSE_MAXSIZE
		};

		Rectangle_Clip(&mixRect, &m_clip);
		Rectangle_Clip(&mixRect, &screenDirtyRect);

		RECT rect =
		{
			mixRect.left - image.x,
			mixRect.top - image.y,
			mixRect.right - image.x,
			mixRect.bottom - image.y
		};

		RECT windowMixRect = mixRect;
		OffsetRect(&windowMixRect, -windowX, -windowY);

		errcode = g_ui->TheBlitter()->Blt(
			m_prevPickup,
			rect.left,
			rect.top,
			windowSurface,
			&windowMixRect,
			k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->Blt(
			windowSurface,
			windowMixRect.left,
			windowMixRect.top,
			(*m_curCursor)->TheSurface(),
			&rect,
			k_AUI_BLITTER_FLAG_CHROMAKEY | k_AUI_BLITTER_FLAG_FAST);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

#ifdef SEIZUREBLIT
		{
			g_ui->TheBlitter()->ColorBlt(
				g_ui->Secondary(),
				&screenDirtyRect,
				RGB(255, 255, 255),
				0);
			Sleep(20);
		}
#endif

		if (!g_civApp->IsInBackground()) // Actual Drawing
		{
			errcode = g_ui->BltToSecondary(
				screenDirtyRect.left,
				screenDirtyRect.top,
				windowSurface,
				windowDirtyRect,
				blitFlags);
			Assert(errcode == AUI_ERRCODE_OK);
			if (errcode != AUI_ERRCODE_OK)
			{
				retcode = AUI_ERRCODE_BLTFAILED;
				break;
			}
		}

		errcode = g_ui->TheBlitter()->Blt(
			windowSurface,
			windowMixRect.left,
			windowMixRect.top,
			m_prevPickup,
			&rect,
			k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_FAST);

		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}
	}

	errcode = g_ui->BltSecondaryToPrimary(blitFlags);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK)
	{
		retcode = AUI_ERRCODE_BLTFAILED;
	}

	if (!g_theProfileDB || !g_theProfileDB->IsUseDirectXBlitter())
	{
		errcode = g_ui->Secondary()->Unlock(primaryBuf);
		Assert(errcode == AUI_ERRCODE_OK);
	}

	return retcode;
}

AUI_ERRCODE	aui_DirectMouse::BltBackgroundColorToPrimary(
	COLORREF color,
	aui_DirtyList *colorAreas)
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

	AUI_ERRCODE errcode;

	if (g_civApp->IsInBackground()) return retcode;

	POINT hotspot;
	(*m_curCursor)->GetHotspot(hotspot);

	POINT cursorLocation = m_data.position;
	cursorLocation.x -= hotspot.x;
	cursorLocation.y -= hotspot.y;

	RECT screenCursorRect =
	{
		cursorLocation.x,
		cursorLocation.y,
		cursorLocation.x + k_MOUSE_MAXSIZE,
		cursorLocation.y + k_MOUSE_MAXSIZE
	};

	ListPos position = colorAreas->GetHeadPosition();
	for (sint32 j = colorAreas->L(); j; j--)
	{
		RECT *screenDirtyRect = colorAreas->GetNext(position);

		RECT clippedScreenCursorRect = screenCursorRect;
		Rectangle_Clip(&clippedScreenCursorRect, screenDirtyRect);

		RECT clippedCursorRect = clippedScreenCursorRect;
		OffsetRect(&clippedCursorRect, -cursorLocation.x, -cursorLocation.y);

		errcode = g_ui->TheBlitter()->ColorBlt(
			m_prevPickup,
			&clippedCursorRect,
			color,
			0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->ColorBlt(
			m_privateMix,
			&clippedCursorRect,
			color,
			0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->Blt(
			m_privateMix,
			clippedCursorRect.left,
			clippedCursorRect.top,
			(*m_curCursor)->TheSurface(),
			&clippedCursorRect,
			k_AUI_BLITTER_FLAG_CHROMAKEY);

		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->BltToSecondary(
			clippedScreenCursorRect.left,
			clippedScreenCursorRect.top,
			m_privateMix,
			&clippedCursorRect,
			k_AUI_BLITTER_FLAG_COPY);

		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		static RECT rects[4];
		sint32 numRects = Rectangle_Subtract(
			screenDirtyRect,
			&clippedScreenCursorRect,
			rects);

		Assert(numRects != -1);
		if (numRects == -1)
		{
			retcode = AUI_ERRCODE_INVALIDPARAM;
			break;
		}

		for (RECT *rectPtr = rects; numRects; numRects--)
		{
			errcode = g_ui->ColorBltToSecondary(
				rectPtr++,
				color,
				0);
			Assert(errcode == AUI_ERRCODE_OK);
			if (errcode != AUI_ERRCODE_OK)
			{
				retcode = AUI_ERRCODE_BLTFAILED;
				break;
			}
		}
	}

	return retcode;
}

AUI_ERRCODE aui_DirectMouse::BltBackgroundImageToPrimary(
	aui_Image *image,
	RECT *imageRect,
	aui_DirtyList *imageAreas)
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	if (g_civApp->IsInBackground()) return retcode;

	POINT hotspot;
	(*m_curCursor)->GetHotspot(hotspot);

	POINT cursorLocation = m_data.position;
	cursorLocation.x -= hotspot.x;
	cursorLocation.y -= hotspot.y;

	RECT screenCursorRect =
	{
		cursorLocation.x,
		cursorLocation.y,
		cursorLocation.x + k_MOUSE_MAXSIZE,
		cursorLocation.y + k_MOUSE_MAXSIZE
	};

	ListPos position = imageAreas->GetHeadPosition();
	for (sint32 j = imageAreas->L(); j; j--)
	{

		RECT *screenDirtyRect = imageAreas->GetNext(position);

		RECT clippedScreenCursorRect = screenCursorRect;
		Rectangle_Clip(&clippedScreenCursorRect, screenDirtyRect);

		RECT clippedCursorRect = clippedScreenCursorRect;
		OffsetRect(&clippedCursorRect, -cursorLocation.x, -cursorLocation.y);

		RECT clippedImageCursorRect = clippedScreenCursorRect;
		OffsetRect(
			&clippedImageCursorRect, -imageRect->left, -imageRect->top);

		errcode = g_ui->TheBlitter()->Blt(
			m_prevPickup,
			clippedCursorRect.left,
			clippedCursorRect.top,
			image->TheSurface(),
			&clippedImageCursorRect,
			k_AUI_BLITTER_FLAG_COPY);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->Blt(
			m_privateMix,
			clippedCursorRect.left,
			clippedCursorRect.top,
			image->TheSurface(),
			&clippedImageCursorRect,
			k_AUI_BLITTER_FLAG_COPY);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->TheBlitter()->Blt(
			m_privateMix,
			clippedCursorRect.left,
			clippedCursorRect.top,
			(*m_curCursor)->TheSurface(),
			&clippedCursorRect,
			k_AUI_BLITTER_FLAG_CHROMAKEY);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		errcode = g_ui->BltToSecondary(
			clippedScreenCursorRect.left,
			clippedScreenCursorRect.top,
			m_privateMix,
			&clippedCursorRect,
			k_AUI_BLITTER_FLAG_COPY);

		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK)
		{
			retcode = AUI_ERRCODE_BLTFAILED;
			break;
		}

		static RECT rects[4];
		sint32 numRects = Rectangle_Subtract(
			screenDirtyRect,
			&clippedScreenCursorRect,
			rects);
		Assert(numRects != -1);
		if (numRects == -1)
		{
			retcode = AUI_ERRCODE_INVALIDPARAM;
			break;
		}

		for (RECT *rectPtr = rects; numRects; numRects--, rectPtr++)
		{

			RECT imageDirtyRect = *rectPtr;
			OffsetRect(&imageDirtyRect, -imageRect->left, -imageRect->top);

			errcode = g_ui->BltToSecondary(
				rectPtr->left,
				rectPtr->top,
				image->TheSurface(),
				&imageDirtyRect,
				k_AUI_BLITTER_FLAG_COPY);

			Assert(errcode == AUI_ERRCODE_OK);
			if (errcode != AUI_ERRCODE_OK)
			{
				retcode = AUI_ERRCODE_BLTFAILED;
				break;
			}
		}
	}

	return retcode;
}

AUI_ERRCODE aui_DirectMouse::Erase(void)
{
	AUI_ERRCODE errcode;

	POINT hotspot;
	(*m_curCursor)->GetHotspot(hotspot);

	POINT image = m_data.position;
	image.x -= hotspot.x;
	image.y -= hotspot.y;

	RECT mixRect =
	{
		image.x,
		image.y,
		image.x + k_MOUSE_MAXSIZE,
		image.y + k_MOUSE_MAXSIZE
	};

	Rectangle_Clip(&mixRect, &m_clip);

	RECT rect = mixRect;
	OffsetRect(&rect, -image.x, -image.y);

	errcode = g_ui->BltToSecondary(
		mixRect.left,
		mixRect.top,
		m_prevPickup,
		&rect,
		k_AUI_BLITTER_FLAG_COPY);

	m_reset = TRUE;

	return AUI_ERRCODE_OK;
}

sint32 aui_DirectMouse::ManipulateInputs(aui_MouseEvent *data, BOOL add)
{
	sint32 numManipulated = 0;

	static sint32 index = 0;

	EnterCriticalSection(m_lpcs);

	if (add)
	{
		if (index <= k_MOUSE_MAXINPUT)
		{
			if (index > 1)
			{
				aui_MouseEvent *event = m_inputs + index - 2;
				if (event->position.x == data->position.x
					&&   event->position.y == data->position.y
					&&   event->lbutton == data->lbutton
					&&   event->rbutton == data->rbutton)
				{
					event++;
					if (event->position.x == data->position.x
						&&   event->position.y == data->position.y
						&&   event->lbutton == data->lbutton
						&&   event->rbutton == data->rbutton)
						index--;
				}
			}

			if (index < k_MOUSE_MAXINPUT)
			{
				memcpy(m_inputs + index, data, sizeof(aui_MouseEvent));
				numManipulated = 1;
				index++;
			}
		}
	}

	else
	{
		if (index)
			memcpy(data, m_inputs, index * sizeof(aui_MouseEvent));
		numManipulated = index;
		index = 0;
	}

	LeaveCriticalSection(m_lpcs);

	return numManipulated;
}

#endif // __AUI_USE_DIRECTX__
