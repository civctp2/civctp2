#ifndef __AUI_DIRECTX_H__
#define __AUI_DIRECTX_H__

#ifdef __AUI_USE_DIRECTX__


class aui_DirectX
{
public:

	aui_DirectX() { m_directXRefCount++; m_exclusiveMode = FALSE; }
	virtual ~aui_DirectX();

protected:
	AUI_ERRCODE InitCommon( BOOL useExclusiveMode );

public:
	virtual BOOL IsThisA( uint32 classId )
	{
		return classId == m_directXClassId;
	}

	LPDIRECTDRAW		DD( void ) const { return m_lpdd; }
	LPDIRECTINPUT		DI( void ) const { return m_lpdi; }

	static void DD( LPDIRECTDRAW lpdd );
	static void DI( LPDIRECTINPUT lpdi );

	static uint32 m_directXClassId;

	BOOL GetExclusiveMode(void) const { return m_exclusiveMode; }

protected:
	static LPDIRECTDRAW		m_lpdd;
	static LPDIRECTINPUT	m_lpdi;
	static BOOL m_b_created_dd;
	static BOOL m_b_created_di;

	BOOL					m_exclusiveMode;

private:
	static sint32			m_directXRefCount;
};




#ifndef DDLOCK_NOSYSLOCK
#define DDLOCK_NOSYSLOCK 0
#endif

#ifndef LPDIRECTDRAWSURFACE3
#define LPDIRECTDRAWSURFACE3 LPDIRECTDRAWSURFACE
#endif

#ifndef IID_IDirectDrawSurface3
#define IID_IDirectDrawSurface3 IID_IDirectDrawSurface2
#endif

typedef aui_DirectX aui_Native;

#endif

#endif
