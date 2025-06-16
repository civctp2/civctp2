#ifndef __AUI_DIRECTMOUSE_H__
#define __AUI_DIRECTMOUSE_H__

#ifdef __AUI_USE_DIRECTX__

#include "aui_mouse.h"
#include "aui_directinput.h"

class aui_DirectMouse : public aui_Mouse, public aui_DirectInput
{
public:

	aui_DirectMouse(
		AUI_ERRCODE *retval,
		const MBCHAR *ldlBlock,
		BOOL useExclusiveMode = FALSE );
	virtual ~aui_DirectMouse();

protected:
	aui_DirectMouse() {}
	AUI_ERRCODE CreateDirectMouse( void );

public:
	virtual AUI_ERRCODE Start(void);
	virtual AUI_ERRCODE End(void);
	virtual AUI_ERRCODE Suspend(BOOL eraseCursor);
	virtual AUI_ERRCODE Resume(void);

	virtual AUI_ERRCODE Acquire( void ) { return aui_DirectInput::Acquire(); }
	virtual AUI_ERRCODE Unacquire( void ) { return aui_DirectInput::Unacquire(); }

	virtual AUI_ERRCODE ReactToInput(void);
	virtual AUI_ERRCODE GetInput( void );

	virtual sint32 ManipulateInputs(aui_MouseEvent *data, BOOL add);
	virtual AUI_ERRCODE	BltWindowToPrimary(aui_Window *window);
	virtual AUI_ERRCODE BltDirtyRectInfoToPrimary(void);
	virtual AUI_ERRCODE	BltBackgroundColorToPrimary(
		COLORREF color,
		aui_DirtyList *colorAreas);
	virtual AUI_ERRCODE	BltBackgroundImageToPrimary(
		aui_Image *image,
		RECT *imageRect,
		aui_DirtyList *imageAreas);

	LPCRITICAL_SECTION LPCS(void) const { return m_lpcs; }

	BOOL ShouldTerminateThread(void);

private:
	AUI_ERRCODE CreatePrivateBuffers(void);
	void DestroyPrivateBuffers(void);
	AUI_ERRCODE Erase(void);

	aui_Surface		*m_privateMix;
	aui_Surface		*m_pickup;
	aui_Surface		*m_prevPickup;

	static LPCRITICAL_SECTION m_lpcs;
	HANDLE		m_thread;
	DWORD		m_threadId;
	HANDLE		m_threadEvent;
	HANDLE		m_terminateEvent;
	HANDLE		m_suspendEvent;
	HANDLE		m_resumeEvent;
	HANDLE		m_replyEvent;
};

DWORD WINAPI MouseThreadProc(LPVOID lpVoid);

typedef aui_DirectMouse aui_NativeMouse;

#endif

#endif
