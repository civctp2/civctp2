

#include "c3.h"
#include "aui.h"
#include "aui_surface.h"

#include "screenmanager.h"

ScreenManager::ScreenManager()
:
m_surface(NULL),
m_surfBase(NULL),
m_surfWidth(0),
m_surfHeight(0),
m_surfPitch(0),
m_isLocked(FALSE)
{
}

ScreenManager::~ScreenManager()
{
}

void ScreenManager::LockSurface(aui_Surface *surf)
{
	AUI_ERRCODE		errcode;

	Assert(surf != NULL);
	if (surf == NULL) return;

	m_surface = surf;

	errcode = surf->Lock(NULL, (LPVOID *)&m_surfBase, 0 );
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode != AUI_ERRCODE_OK ) return;

	m_surfWidth = surf->Width();
	m_surfHeight = surf->Height();
	m_surfPitch = surf->Pitch();

	m_isLocked = TRUE;
}

void ScreenManager::UnlockSurface(void)
{
	AUI_ERRCODE		errcode;

	Assert(m_surface);
	if (m_surface == NULL) return;

	errcode = m_surface->Unlock(m_surfBase);
	Assert(errcode == AUI_ERRCODE_OK);

	m_surface = NULL;
	m_surfBase = NULL;
	m_surface = NULL;
	m_surfWidth = 0;
	m_surfHeight = 0;
	m_surfPitch = 0;
	m_isLocked = FALSE;
}

