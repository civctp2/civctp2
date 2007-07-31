












#pragma once
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
