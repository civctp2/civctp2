//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : SLIC-driven map tile targeting mode
//
//----------------------------------------------------------------------------
//
// When BeginTargetMode("callback") is called from SLIC, this enters a
// crosshair cursor mode. On left-click, fires the named SLIC segment
// with player and location context, then exits targeting mode.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "ScriptTargetMode.h"

#include "SlicEngine.h"
#include "SlicObject.h"
#include "cursormanager.h"
#include "controlpanelwindow.h"

#include <cstring>

extern SlicEngine       *g_slicEngine;
extern CursorManager    *g_cursorManager;
extern ControlPanelWindow *g_controlPanel;

ScriptTargetMode *g_scriptTargetMode = NULL;


void ScriptTargetMode::Initialize(void)
{
	Cleanup();
	g_scriptTargetMode = new ScriptTargetMode();
}

void ScriptTargetMode::Cleanup(void)
{
	if (g_scriptTargetMode != NULL) {
		delete g_scriptTargetMode;
		g_scriptTargetMode = NULL;
	}
}

ScriptTargetMode::ScriptTargetMode()
:	m_active(false),
	m_player(0)
{
	m_callbackName[0] = '\0';
}

ScriptTargetMode::~ScriptTargetMode()
{
}

void ScriptTargetMode::Begin(sint32 player, const char *callback_name)
{
	if (!callback_name || callback_name[0] == '\0')
		return;

	m_active = true;
	m_player = player;
	strncpy(m_callbackName, callback_name, sizeof(m_callbackName) - 1);
	m_callbackName[sizeof(m_callbackName) - 1] = '\0';

	// Set crosshair/target cursor
	if (g_cursorManager)
		g_cursorManager->SetCursor(CURSORINDEX_TARGET);

	// Put the control panel into script targeting mode so the click dispatch
	// routes through ExecuteTargetingModeClick
	if (g_controlPanel)
		g_controlPanel->SetScriptTargetingMode();
}

bool ScriptTargetMode::HandleClick(const MapPoint &pos)
{
	if (!m_active)
		return false;

	// Fire the named SLIC segment with player + location context
	if (g_slicEngine) {
		SlicObject *so = new SlicObject(m_callbackName);
		so->AddPlayer(m_player);
		so->AddLocation(pos);
		so->AddRecipient(m_player);
		g_slicEngine->Execute(so);
	}

	// Exit targeting mode
	Cancel();
	return true;
}

void ScriptTargetMode::Cancel(void)
{
	if (!m_active)
		return;

	m_active = false;
	m_callbackName[0] = '\0';

	// Restore default cursor
	if (g_cursorManager)
		g_cursorManager->SetCursor(CURSORINDEX_DEFAULT);

	// Clear the control panel targeting mode
	if (g_controlPanel)
		g_controlPanel->ClearTargetingMode();
}
