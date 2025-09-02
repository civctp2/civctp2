//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : General declarations
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragmas commented out
// - includes fixed for case sensitive filesystems.
// - Merged DoFinalCleanup versions, removed some unused exports.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __CIV3_MAIN_H__
#define __CIV3_MAIN_H__

#include <string>

#define k_TICKS_PER_GENERIC_FRAME	100
#define k_TICKS_PER_SCROLL_FRAME	50

#ifdef __AUI_USE_SDL__
union SDL_Event;
#endif

class aui_Surface;

int ui_Initialize(void);
int ui_Process(void);
bool ui_CheckForScroll(void);

sint32 sharedsurface_Initialize( void );
void   sharedsurface_Cleanup( void );

int sprite_Initialize(void);
int sprite_Update(aui_Surface *surf);
void sprite_Cleanup(void);

int tile_Initialize(BOOL isRestoring);
void tile_Cleanup(void);

int main_Restart();
int main_RestoreGame(const MBCHAR *filename);

void main_HideTaskBar(void);
void main_RestoreTaskBar(void);

std::basic_string<TCHAR> main_GetExeName();

int radar_Initialize(void);

void DoFinalCleanup(int exitCode = -1);

BOOL ExitGame(void);

#ifdef __AUI_USE_SDL__
int SDLMessageHandler(void* userdata, SDL_Event* event);
#endif

#endif
