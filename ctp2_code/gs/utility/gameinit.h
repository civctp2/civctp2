#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef GAMEINIT_H
#define GAMEINIT_H 1

class CivArchive ;

sint32 gameinit_InitializeGameFiles(void);
sint32 gameinit_Initialize(sint32 mWidth, sint32 mHeight, CivArchive &archive);
sint32 gameinit_CleanupMessages(void);
sint32 gameinit_Cleanup(void);
sint32 gameinit_ResetForNetwork();
sint32 spriteEditor_Initialize(sint32 mWidth, sint32 mHeight);

void gameinit_SpewUnits(sint32 player, MapPoint &pos);

extern BOOL g_startEmailGame;
extern BOOL g_startHotseatGame;

struct HotseatPlayerSetup {
	sint32 civ;
	sint32 isHuman;
	MBCHAR *name;
	MBCHAR *email;
};
extern HotseatPlayerSetup g_hsPlayerSetup[k_MAX_PLAYERS];

#endif
