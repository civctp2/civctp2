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

#include <cstring>

#include "controlpanelwindow.h"
#include "cursormanager.h"
#include "SlicEngine.h"
#include "SlicObject.h"

ScriptTargetMode *g_scriptTargetMode = NULL;

//----------------------------------------------------------------------------
//
// Name       : ScriptTargetMode::Initialize
//
// Description: Creates the global instance, replacing any existing one.
//
// Parameters : -
//
// Globals    : g_scriptTargetMode : set to the new instance
//
// Returns    : -
//
//----------------------------------------------------------------------------
void ScriptTargetMode::Initialize(void)
{
	Cleanup();
	g_scriptTargetMode = new ScriptTargetMode();
}

//----------------------------------------------------------------------------
//
// Name       : ScriptTargetMode::Cleanup
//
// Description: Deletes the global instance.
//
// Parameters : -
//
// Globals    : g_scriptTargetMode : deleted and set to NULL
//
// Returns    : -
//
//----------------------------------------------------------------------------
void ScriptTargetMode::Cleanup(void)
{
	delete g_scriptTargetMode;
	g_scriptTargetMode = NULL;
}

ScriptTargetMode::ScriptTargetMode()
:	m_player(0)
{
	m_callbackName[0] = '\0';
}

//----------------------------------------------------------------------------
//
// Name       : ScriptTargetMode::CopyCallbackName
//
// Description: Copies a callback segment name, truncating it so that it fits
//              k_SCRIPT_TARGET_CALLBACK_NAME_SIZE with its terminator.
//
// Parameters : dest    : buffer of k_SCRIPT_TARGET_CALLBACK_NAME_SIZE chars
//              source  : name to copy
//
// Globals    : -
//
// Returns    : -
//
//----------------------------------------------------------------------------
void ScriptTargetMode::CopyCallbackName(char *dest, const char *source)
{
	strncpy(dest, source, k_SCRIPT_TARGET_CALLBACK_NAME_SIZE - 1);
	dest[k_SCRIPT_TARGET_CALLBACK_NAME_SIZE - 1] = '\0';
}

//----------------------------------------------------------------------------
//
// Name       : ScriptTargetMode::Begin
//
// Description: Enters targeting mode: the next left-click on a map tile fires
//              the callback segment.
//
// Parameters : player         : player that is targeting, receives the callback
//              callback_name  : SLIC segment to fire on click, not empty
//
// Globals    : g_controlPanel   : switched to the script targeting mode
//              g_cursorManager  : shows the target cursor
//
// Returns    : -
//
// Remark(s)  : Without a control panel there is no mode to enter and nothing
//              happens. The caller (Slic_BeginTargetMode) rejects an empty name.
//
//----------------------------------------------------------------------------
void ScriptTargetMode::Begin(sint32 player, const char *callback_name)
{
	Assert(callback_name && callback_name[0] != '\0');

	if (!g_controlPanel)
		return;

	m_player = player;
	CopyCallbackName(m_callbackName, callback_name);

	g_controlPanel->SetScriptTargetingMode();

	if (g_cursorManager)
		g_cursorManager->SetCursor(CURSORINDEX_TARGET);
}

//----------------------------------------------------------------------------
//
// Name       : ScriptTargetMode::HandleClick
//
// Description: Fires the callback segment for a click on a map tile.
//
// Parameters : pos  : clicked tile
//
// Globals    : g_slicEngine : executes the callback
//
// Returns    : bool : true when the mode was active and the click was consumed
//
// Remark(s)  : Targeting is one-shot. The mode is left before running
//              arbitrary SLIC, so a callback that creates UI or begins another
//              target mode cannot re-enter the old mode and then be torn down
//              when it returns.
//
//----------------------------------------------------------------------------
bool ScriptTargetMode::HandleClick(const MapPoint &pos)
{
	if (!IsActive())
		return false;

	char callbackName[k_SCRIPT_TARGET_CALLBACK_NAME_SIZE];
	CopyCallbackName(callbackName, m_callbackName);
	const sint32 player = m_player;
	Cancel();

	if (g_slicEngine) {
		SlicObject *so = new SlicObject(callbackName);
		so->AddPlayer(player);
		so->AddLocation(pos);
		so->AddRecipient(player);
		g_slicEngine->Execute(so);
	}

	return true;
}

//----------------------------------------------------------------------------
//
// Name       : ScriptTargetMode::Cancel
//
// Description: Leaves targeting mode. Used for ESC, right-click and after a
//              click has been handled.
//
// Parameters : -
//
// Globals    : g_controlPanel : ClearTargetingMode also restores the cursor
//
// Returns    : -
//
//----------------------------------------------------------------------------
void ScriptTargetMode::Cancel(void)
{
	if (IsActive())
		g_controlPanel->ClearTargetingMode();
}

//----------------------------------------------------------------------------
//
// Name       : ScriptTargetMode::IsActive
//
// Description: Tells whether the script targeting mode is the control panel's
//              current targeting mode.
//
// Parameters : -
//
// Globals    : g_controlPanel : owns the targeting mode
//
// Returns    : bool : true while waiting for the click
//
// Remark(s)  : Starting an order or a tile improvement replaces the mode, and
//              this then reports false without any further bookkeeping.
//
//----------------------------------------------------------------------------
bool ScriptTargetMode::IsActive(void) const
{
	return g_controlPanel &&
	       g_controlPanel->GetTargetingMode() == CP_TARGETING_MODE_SCRIPT_PENDING;
}
