//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : SLIC-driven map tile targeting mode
//
//----------------------------------------------------------------------------
//
// Provides a "pick a tile" targeting cursor that fires a named SLIC
// callback segment with (player, location) when the player clicks.
// Allows SLIC scripts to request point-at-map targeting for any purpose
// (offensive abilities, teleport destinations, terrain effects, etc.).
//
// Whether the mode is active is owned by the control panel: it is active
// exactly when ControlPanelWindow::GetTargetingMode() is
// CP_TARGETING_MODE_SCRIPT_PENDING, like every other targeting mode. This
// class only holds the callback and the player to call it for.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SCRIPT_TARGET_MODE_H__
#define __SCRIPT_TARGET_MODE_H__

#include "MapPoint.h"

// Size of the buffer that holds the callback segment name, terminator included.
#define k_SCRIPT_TARGET_CALLBACK_NAME_SIZE 256

class ScriptTargetMode
{
public:
	static void Initialize(void);
	static void Cleanup(void);

	ScriptTargetMode();

	void Begin(sint32 player, const char *callback_name);
	bool HandleClick(const MapPoint &pos);
	void Cancel(void);
	bool IsActive(void) const;

private:
	static void CopyCallbackName(char *dest, const char *source);

	sint32 m_player;
	char   m_callbackName[k_SCRIPT_TARGET_CALLBACK_NAME_SIZE];
};

extern ScriptTargetMode *g_scriptTargetMode;

#endif // __SCRIPT_TARGET_MODE_H__
