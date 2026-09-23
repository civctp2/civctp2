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
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SCRIPT_TARGET_MODE_H__
#define __SCRIPT_TARGET_MODE_H__

#include "MapPoint.h"

class ScriptTargetMode
{
public:
	static void Initialize(void);
	static void Cleanup(void);

	ScriptTargetMode();
	~ScriptTargetMode();

	// Enter targeting mode. callback_name is the SLIC segment to fire on click.
	void Begin(sint32 player, const char *callback_name);

	// Called when the player clicks a map tile while in targeting mode.
	// Returns true if handled (mode was active).
	bool HandleClick(const MapPoint &pos);

	// Cancel targeting mode (ESC or right-click).
	void Cancel(void);

	// Deactivate without calling back into ControlPanel (avoids recursion
	// when ClearTargetingMode calls us).
	void Deactivate(void) { m_active = false; m_callbackName[0] = '\0'; }

	bool IsActive(void) const { return m_active; }
	sint32 GetPlayer(void) const { return m_player; }

private:
	bool   m_active;
	sint32 m_player;
	char   m_callbackName[256];
};

extern ScriptTargetMode *g_scriptTargetMode;

#endif // __SCRIPT_TARGET_MODE_H__
