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
// _DEBUG
// - Generate debug version
//
// _MSC_VER		
// - Use Microsoft C++ extensions when set.
//
// __GNUC__
// - We are compiling with gcc
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragmas commented out
// - includes fixed for case sensitive filesystems
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __CIV3_MAIN_H__
#define __CIV3_MAIN_H__


#define k_TICKS_PER_GENERIC_FRAME	100 
#define k_TICKS_PER_SCROLL_FRAME	50	


#define _NO_GAME_WATCH

class aui_Surface;


typedef void (* CivExceptionFunction) (void);

int ui_Initialize(void);
int ui_Process(void);
void ui_HandleMouseWheel(sint16 delta);
BOOL ui_CheckForScroll(void);
int ui_Cleanup(void);

sint32 sharedsurface_Initialize( void );
sint32 sharedsurface_Cleanup( void );

int ai_Initialize(void);
int ai_Cleanup(void);

int sprite_Initialize(void);
int sprite_Update(aui_Surface *surf);
int sprite_Cleanup(void);

int tile_Initialize(BOOL isRestoring);
int tile_Cleanup(void);

int WINAPI main_filehelper_GetOS(void);
DWORD main_GetRemainingSwapSpace(void);
BOOL main_VerifyRAMToRun(void);

static LONG _cdecl main_CivExceptionHandler (LPEXCEPTION_POINTERS pException);
#ifdef _DEBUG
static LONG _cdecl main_CivDebugExceptionHandler (LPEXCEPTION_POINTERS exception_pointers);
#endif

void main_ExceptionExecute(CivExceptionFunction function);

int main_Restart();
int main_RestoreGame(const MBCHAR *filename);

void main_HideTaskBar(void);
void main_RestoreTaskBar(void);

int radar_Initialize(void);


void DoFinalCleanup(void);

BOOL ExitGame(void);

#endif 
