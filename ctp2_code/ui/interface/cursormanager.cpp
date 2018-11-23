#include "c3.h"

#include "aui_mouse.h"

#include "c3ui.h"

#include "cursormanager.h"

extern C3UI			*g_c3ui;

CursorManager		*g_cursorManager = NULL;


void CursorManager::Initialize(void)
{
	Cleanup();

	g_cursorManager = new CursorManager();
}


void CursorManager::Cleanup(void)
{
	if (g_cursorManager != NULL) {
		delete g_cursorManager;
		g_cursorManager = NULL;
	}
}


CursorManager::CursorManager()
{
	m_curCursor = CURSORINDEX_DEFAULT;
	m_savedCursor = CURSORINDEX_MAX;
}


CursorManager::~CursorManager()
{
}


void CursorManager::SetCursor(CURSORINDEX cursor)
{
	if (cursor >= 0 && cursor < CURSORINDEX_MAX) {

		aui_Mouse		*theMouse;
		theMouse = g_c3ui->TheMouse();






		if(theMouse)
			theMouse->SetAnim((sint32)cursor);

		m_curCursor = cursor;
	}
}


void CursorManager::SaveCursor(void)
{
	m_savedCursor = m_curCursor;
}


void CursorManager::RestoreCursor(void)
{
	if (m_savedCursor == CURSORINDEX_MAX) {
		m_curCursor = CURSORINDEX_DEFAULT;
	} else {
		m_curCursor = m_savedCursor;
		m_savedCursor = CURSORINDEX_MAX;
	}
}
