//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : General declarations
// Id           : $Id$
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
// - Compiler version (for the Microsoft C++ compiler only).
//
// __GNUC__
// - Compiler version (for the GNU C++ compiler only).
//
// USE_LOGGING
// - Enable logging facilities - even when not using the debug build.
//
// _BFR_
// - Force CD checking when set (build final release).
// - If not defined it enables some more loogs.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragmas commented out
// - includes fixed for case sensitive filesystems
// - Removed unused CPU investigation references.
// - Prevent memory leak report.
// - Merged GNU and MSVC code (DoFinalCleanup, CivMain).
// - Option added to include multiple data directories.
// - Display the main thread function name in the debugger.
// - Removed references to CivilisationDB. (Aug 20th 2005 Martin Gühmann)
// - Removed references to old SpriteStateDBs. (Aug 29th 2005 Martin Gühmann)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Removed unused local variables. (Sep 9th 2005 Martin Gühmann)
// - Removed some unreachable code. (Sep 9th 2005 Martin Gühmann)
// - Moved debug tools handling to c3.h, so that the leak reporter doesn't
//   report leaks that aren't leaks. (Oct 3rd 2005 Matzin Gühmann)
// - Added version to crash.txt
// - USE_LOGGING now works in a final version. (30-Jun-2008 Martin Gühmann)
// - The log files are now only opened and closed once, this speeds up
//   debugging significantly. (09-Aug-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------
//
/// \file   civ3_main.cpp
/// \brief  Main program entry point (main/WinMain)
/// \mainpage
/// \image html "../common/images/wallpaper1_640x480_2.jpg"

#include "c3.h"         // Pre-compiled header
#include "civ3_main.h"  // Own declarations: consistency check

#include "AdvanceRecord.h"
#include <algorithm>                    // std::fill
#include "ancientwindows.h"
#include "appstrings.h"
#include "aui.h"
#include "aui_directmoviemanager.h"
#include "aui_factory.h"
#include "aui_ldl.h"
#include "background.h"
#include "backgroundwin.h"
#include "bevellesswindow.h"
#include "BuildingRecord.h"
#include "c3_button.h"
#include "c3_dropdown.h"
#include "c3_static.h"
#include "c3blitter.h"
#include "c3cmdline.h"
#include "c3debug.h"                    // c3debug_ExceptionStackTrace
#include "c3errors.h"
#include "c3memmap.h"
#include "c3ui.h"
#include "c3window.h"
#include "c3windows.h"
#include "civapp.h"
#include "CivPaths.h"                   // g_civPaths
#include "civscenarios.h"               // g_civScenarios
#include <clocale>
#include "colorset.h"
#include "controlpanelwindow.h"         // g_controlPanel
#include "ctp2_listbox.h"
#include "ctp2_menubar.h"
#include "ctp2_Window.h"
#include "ctpregistry.h"
#include "cursormanager.h"
#include "DB.h"
#include "debugmemory.h"
#include "debugwindow.h"
#include "director.h"
#include "ErrMsg.h"
#include "gamefile.h"
#include "gameinit.h"
#include "GameOver.h"
#include "Globals.h"                    // allocated::clear, allocated::reassign
#include "grabitem.h"
#include "greatlibrary.h"
#include "helptile.h"
#include "icon.h"
#include "initialplaywindow.h"
#include <iomanip>                      // std::setfill, std::setw
#include "keypress.h"
#include "MainControlPanel.h"
#include "messagewin.h"
#include "network.h"
#include "pattern.h"
#include "picture.h"
#include "pixelutils.h"
#include "player.h"
#include "pollution.h"
#include "primitives.h"
#include "profileDB.h"                  // g_theProfileDB
#include "radarmap.h"
#include "radarwindow.h"
#include "RoboInit.h"
#include "scenariowindow.h"
#include "screenmanager.h"
#include "SelItem.h"
#include "sliccmd.h"                    // sliccmd_clear_symbols
#include "SpecialEffectRecord.h"
#include "splash.h"                     // g_splash_old, SPLASH_STRING
#include "spnewgamewindow.h"
#include "Sprite.h"
#include "SpriteGroupList.h"
#include <sstream>                      // std::basic_stringstream
#include "statswindow.h"
#include "statuswindow.h"
#include "StrDB.h"                      // g_theStringDB
#include <string>                       // std::basic_string
#include "TerrainRecord.h"
#include "tiledmap.h"
#include "TradePool.h"
#include "TurnCnt.h"                    // g_turn
#include "UnitPool.h"                   // g_theUnitPool
#include "UnitSpriteGroup.h"
#include "workwin.h"
#include "workwindow.h"
#include "World.h"                      // g_theWorld

#if !defined(__GNUC__) // TODO: replacement needed (wine doesnt have these headers...)
#include "directvideo.h"
#include "videoutils.h"
#endif

#if defined(USE_SDL)
#include <SDL.h>
#include <SDL_mixer.h>
#endif

#if defined(_DEBUG)
#include "debug.h"          // Os::SetThreadName
#include "SlicSegment.h"    // SlicSegment::Cleanup
#endif // _DEBUG 

extern Background *     g_background;

namespace Os
{
	extern uint32 GetTicks(void);
}

#ifdef _BFR_
extern sint32           g_logCrashes;
#endif

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL   (WM_MOUSELAST+1)
#endif

#define k_LDLName                   "civ3.ldl"
#define k_LDL640Name                "civ3_640.ldl"
#define k_CursorName                "cursor2.tif"
#define k_DisclaimerName            "disclaimer.txt"

#define k_SHARED_SURFACE_WIDTH      1024
#define k_SHARED_SURFACE_HEIGHT     768
#define k_SHARED_SURFACE_BPP        16


#define k_SMOOTH_PIX_SEC_PER_SEC    8.0f
#define k_SMOOTH_MIN_VELOCITY       4.0f
#define k_SMOOTH_MAX_VELOCITY       (k_TILE_PIXEL_HEIGHT>>1)
#define k_SMOOTH_START_TIME         (int)(1000.0f*(float)sqrt(k_SMOOTH_MIN_VELOCITY*2.0f/k_SMOOTH_PIX_SEC_PER_SEC))
#define k_SMOOTH_SLOW_TIME          (int)(500.0f*(float)sqrt(k_SMOOTH_MAX_VELOCITY*2.0f/k_SMOOTH_PIX_SEC_PER_SEC))

HWND                                gHwnd;
HINSTANCE                           gHInstance;
BOOL                                gDone = FALSE;
LPCSTR                              gszMainWindowClass = "CTP II";
LPCSTR                              gszMainWindowName = "CTP II";
sint32                              g_ScreenWidth = 0;
sint32                              g_ScreenHeight = 0;


C3UI                                *g_c3ui = NULL;
StatusWindow                        *g_statusWindow = NULL;
aui_Surface                         *g_sharedSurface = NULL;

BOOL                                g_smoothScroll = FALSE;



RECT                                g_backgroundViewport = { 0, 0, 0, 0 };
sint32                              g_is565Format = TRUE;
sint32                              g_modalWindow = 0;

BOOL                                g_helpMode = TRUE;

static uint32                       s_scrollcurtick =0;
static uint32                       s_scrolllasttick=0;
static sint32                       s_scrolltime =k_SMOOTH_START_TIME;
static uint32                       s_accelTickStart = 0;

sint32                              g_terrainPollution;


Director                            *g_director;
double                              g_ave_frame_rate = 10.0;
double                              g_ave_frame_time = 200.0;
ScreenManager                       *g_screenManager = NULL;


TiledMap                            *g_tiledMap = NULL;


RadarMap                            *g_radarMap = NULL;


CivApp                              *g_civApp = NULL;


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

#if defined(__GNUC__)
int CivMain(int argc, char **argv);
#else
int WINAPI CivMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow);
#endif


Network g_network;

BOOL g_letUIProcess = FALSE;
BOOL g_useDDBlit = TRUE;
BOOL g_powerPointsMode = FALSE;


BOOL g_exclusiveMode = TRUE;
BOOL g_hideTaskBar = FALSE;
BOOL g_useIntroMovie = TRUE;
BOOL g_noAssertDialogs = FALSE;
BOOL g_runInBackground = FALSE;
BOOL g_eventLog = FALSE;



BOOL g_use_profile_process = FALSE;


BOOL g_createDirectDrawOnSecondary = FALSE;

/// The whole world is visible at all times, even when unexplored.
sint32		g_god;

DWORD		g_dxver = 0;


bool g_autoAltTab = false;

#define RELDBG(x) { FILE *f = fopen("reldbg.txt", "a"); fprintf x; fclose(f); }

namespace Os
{
    /// Get the name of the executable
    /// \remarks Including the full path
    std::basic_string<TCHAR> GetExeName(void)
    {
#if defined(WIN32)
		TCHAR   filepath[MAX_PATH];
		DWORD   filepathLength      = GetModuleFileName(NULL, filepath, MAX_PATH);

        return std::basic_string<TCHAR>(filepath, filepathLength);
#else
        return std::basic_string<TCHAR>();
#endif
    }

    /// Get the version of the executable
    /// \remarks Using the last modification date as version
    std::basic_string<TCHAR> GetExeVersion(void)
    {
        std::basic_stringstream<TCHAR>  exeVersion;

#if defined(WIN32)
        HANDLE		fileHandle	= CreateFile(Os::GetExeName().c_str(), 
											 GENERIC_READ,
			                                 FILE_SHARE_READ, 
											 NULL, 
											 OPEN_ALWAYS, 
											 FILE_ATTRIBUTE_NORMAL, 
											 NULL
											);

		if (fileHandle != INVALID_HANDLE_VALUE) 
		{
			FILETIME	lastWrite;
			SYSTEMTIME	systemTime;

			if (GetFileTime(fileHandle, NULL, NULL, &lastWrite)		&&
				FileTimeToSystemTime(&lastWrite, &systemTime)
			   ) 
			{
                exeVersion << std::setfill('0')
                           << std::setw(4) << systemTime.wYear  << '-'
                           << std::setw(2) << systemTime.wMonth << '-'
                           << std::setw(2) << systemTime.wDay;
			}

			CloseHandle(fileHandle);
		}
#endif  // WIN32

		return exeVersion.str();
	}
}

int ui_Initialize(void)
{
	AUI_ERRCODE auiErr = AUI_ERRCODE_OK;

	pixelutils_Initialize();

	
	MBCHAR ldlfile[_MAX_PATH];
	g_civPaths->FindFile(C3DIR_LAYOUT, k_LDLName, ldlfile);

	g_c3ui = new C3UI(
		&auiErr,
		gHInstance,
		gHwnd,
		g_ScreenWidth,
		g_ScreenHeight,
		16,
		ldlfile,
		g_exclusiveMode );
	Assert( AUI_NEWOK(g_c3ui,auiErr) );
	if ( !AUI_NEWOK(g_c3ui,auiErr) ) return 10;

	if (g_exclusiveMode)
		g_c3ui->MinimizeOnAltTabOut(TRUE);

	if (!g_exclusiveMode)
		main_HideTaskBar();

	
	g_is565Format = AUI_SURFACE_PIXELFORMAT_565 == g_c3ui->PixelFormat();

	
	ColorSet::Initialize();
    g_c3ui->RegisterCleanup(&ColorSet::Cleanup);


	SPLASH_STRING("Creating Blitter...");

	g_c3ui->RegisterObject(new C3Blitter);
	g_c3ui->RegisterObject(new C3MemMap);

	SPLASH_STRING("Initializing Paths...");

    int     i;
	char s[_MAX_PATH+1];

    for (i = 0; g_civPaths->FindPath(C3DIR_PATTERNS, i, s); ++i)
    {
        if (s[0])
        {
            g_c3ui->AddPatternSearchPath(s);
            g_c3ui->AddImageSearchPath(s);
        }
    }

    for (i = 0; g_civPaths->FindPath(C3DIR_ICONS, i, s); ++i)
    {
        if (s[0])
        {
            g_c3ui->AddIconSearchPath(s);
            g_c3ui->AddImageSearchPath(s);
        }
    }
	
    for (i = 0; g_civPaths->FindPath(C3DIR_PICTURES, i, s); ++i)
    {
        if (s[0])
        {
            g_c3ui->AddPatternSearchPath(s);
            g_c3ui->AddPictureSearchPath(s);
            g_c3ui->AddImageSearchPath(s);
        }
    }
	
    for (i = 0; g_civPaths->FindPath(C3DIR_CURSORS, i, s); ++i)
    {
        if (s[0])
        {
            g_c3ui->AddCursorSearchPath(s);
            g_c3ui->AddImageSearchPath(s);
        }
    }
	
    for (i = 0; g_civPaths->FindPath(C3DIR_FONTS, i, s); ++i)
    {
        if (s[0])
        {
            g_c3ui->AddBitmapFontSearchPath(s);
        }
    }

	if (!GetWindowsDirectory(s, _MAX_PATH)) {
		c3errors_FatalDialog(appstrings_GetString(APPSTR_FONTS), 
								appstrings_GetString(APPSTR_NOWINDOWSDIR));
	}
	strcat(s, FILE_SEP "fonts");
	g_c3ui->AddBitmapFontSearchPath(s);

	
    for (i = 0; g_civPaths->FindPath(C3DIR_VIDEOS, i, s); ++i)
    {
        if (s[0])
        {
            g_c3ui->AddMovieSearchPath(s);
        }
    }

	SPLASH_STRING("Creating Mouse...");
	
	BOOL const mouseExclusiveMode = TRUE;
	g_c3ui->RegisterObject(aui_Factory::new_Mouse(auiErr, "CivMouse", mouseExclusiveMode));

	SPLASH_STRING("Creating Keyboard...");

	g_c3ui->RegisterObject(aui_Factory::new_Keyboard(auiErr));


#if !defined(__GNUC__)
	SPLASH_STRING("Creating Movie manager...");
	g_c3ui->RegisterObject(new aui_DirectMovieManager());
#endif

	SPLASH_STRING("Starting Mouse...");
	auiErr = g_c3ui->TheMouse()->Start();
	Assert(auiErr == AUI_ERRCODE_OK);
	if ( auiErr != AUI_ERRCODE_OK ) return 14;

	
	while ( ShowCursor( FALSE ) >= 0 )
	;

	return AUI_ERRCODE_OK;
}

/// Handle mouse wheel operation
/// \param delta Amount and direction of mouse wheel rotation
void ui_HandleMouseWheel(sint16 delta)
{
	if (!g_civApp)  return;
	if (0 == delta) return; // no rotation

	ctp2_ListBox * box = ctp2_ListBox::GetMouseFocusListBox();

	if(box)
	{
		// Scroll list box
		box->ForceScroll(0, (delta < 0) ? 1 : -1);
	}
	else
	{
		// Scroll main map (when available)
		bool isMapScrolled = 
		    g_civApp->IsGameLoaded() && 
		    g_tiledMap && 
		    g_tiledMap->ScrollMap(0, 2 * ((delta < 0) ? 1 : -1));

		if(isMapScrolled)
		{
			g_tiledMap->RetargetTileSurface(NULL);
			g_tiledMap->Refresh();
			g_tiledMap->InvalidateMap();
			g_tiledMap->ValidateMix();
		}
	}
}

bool compute_scroll_deltas(sint32 time,sint32 &deltaX,sint32 &deltaY)
{
	
	bool retval = true;

	
	float real_time=(float)time/(1000.0f);

	
	real_time *= real_time;

	float velocity = std::max<float>
	    (real_time * 0.5f * k_SMOOTH_PIX_SEC_PER_SEC, k_SMOOTH_MIN_VELOCITY);

	
	deltaX *= (sint32)velocity;
	deltaY *= (sint32)velocity;

	
	sint32 signx = (deltaX < 0 ? -1 : 1);
	sint32 signy = (deltaY < 0 ? -1 : 1);

	if (abs(deltaX) > k_SMOOTH_MAX_VELOCITY)
	{
		deltaX = k_SMOOTH_MAX_VELOCITY * signx;
		retval = false;
	}
	
	if (abs(deltaY)> k_SMOOTH_MAX_VELOCITY)
	{
		deltaY = k_SMOOTH_MAX_VELOCITY * signy;
		retval = false;
	}


	deltaX &= 0xFFFFFFFE;
	deltaY &= 0xFFFFFFFE;

	return retval;
}


bool ui_CheckForScroll(void)
{
	if (!g_tiledMap) return false;

	sint32		hscroll = g_tiledMap->GetZoomTilePixelWidth();
	sint32		vscroll = g_tiledMap->GetZoomTilePixelHeight()/2;

	if (g_controlPanel)
		g_controlPanel->Idle();

	g_tiledMap->SetScrolling(false);

	s_scrolllasttick = s_scrollcurtick;
	s_scrollcurtick	 = GetTickCount();

	if (!g_c3ui->TheMouse()) 
		return false;

	sint32 x = g_c3ui->TheMouse()->X();
	sint32 y = g_c3ui->TheMouse()->Y();

	static bool     isMouseScrolling            = false;
	static sint32   smoothX                     = 0;
	static sint32   smoothY                     = 0;
	static sint32	lastdeltaX                  = 0; 
	static sint32	lastdeltaY                  = 0;
	static bool     scrolled_last_time          = false;
	static uint32   scroll_start;

//	const int       k_MAX_SMOOTH_SCROLL         = 64;
	const int       k_TICKS_PER_ACCELERATION    = 50;

	sint32	deltaX = 0,
			deltaY = 0;


	CURSORINDEX		scrollCursor = CURSORINDEX_DEFAULT;



	
	
	
	
	
	
	bool        scrolled = false;

	if (g_civApp->IsKeyboardScrolling() &&
		(!g_c3ui->TopWindow() || !g_c3ui->TopWindow()->GetFocusControl())) 
	{
		switch (g_civApp->GetKeyboardScrollingKey()) 
		{
		case AUI_KEYBOARD_KEY_UPARROW:
			deltaX = 0;
			deltaY = -1;
			scrolled = true;
			break;
		case AUI_KEYBOARD_KEY_LEFTARROW:
			deltaX = -1;
			deltaY = 0;
			scrolled = true;
			break;
		case AUI_KEYBOARD_KEY_RIGHTARROW:
			deltaX = 1;
			deltaY = 0;
			scrolled = true;
			break;
		case AUI_KEYBOARD_KEY_DOWNARROW:
			deltaX = 0;
			deltaY = 1;
			scrolled = true;
			break;
		}

		if (scrolled) 
		{
			aui_Window *topWindow = g_c3ui ? g_c3ui->TopWindow() : NULL;

			if (topWindow && g_controlPanel) 
            {
				if (topWindow != g_controlPanel->GetWindow() && 
                    topWindow != g_statusWindow && 
                    topWindow != g_radarWindow && 
                    topWindow != g_controlPanel->GetMenuBar()
                   ) 
                {
					return false;
				}
			}

			if (deltaX)
				deltaY = 0;

			lastdeltaX = deltaX;
			lastdeltaY = deltaY;

			g_tiledMap->SetScrolling(true);
			g_tiledMap->ScrollMap(deltaX, deltaY);

			return true;
		}
	} 
	else 
	{
		if (x<4) 
		{
			deltaX = -1;
			deltaY = 0;
			scrolled = true;
			scrollCursor = CURSORINDEX_SCROLLLEFT;
			isMouseScrolling = true;
		}
		else if (x > (g_ScreenWidth-4)) 
		{
			deltaX = 1;
			deltaY = 0;
			scrolled = true;
			scrollCursor = CURSORINDEX_SCROLLRIGHT;
		    isMouseScrolling = true;
		}

		if (y <= 2) 
		{
			deltaX = 0;
			deltaY = -1;
			scrolled = true;
			scrollCursor = CURSORINDEX_SCROLLUP;
			isMouseScrolling = true;
		}
		else if (y >= (g_ScreenHeight-2)) 
		{
			deltaX = 0;
			deltaY = 1;
			scrolled = true;
			scrollCursor = CURSORINDEX_SCROLLDOWN;
		    isMouseScrolling = true;
		}
		
		

		if(scrolled) {
			if (isMouseScrolling)
				g_cursorManager->SetCursor(scrollCursor);

			if(!scrolled_last_time) {
				scroll_start = GetTickCount();			
			}

			if(GetTickCount() < scroll_start + g_theProfileDB->GetScrollDelay() * 100) {
				scrolled_last_time = scrolled;
				return false;
			} else if(!scrolled_last_time) {
				g_tiledMap->AddChatDirtyRectToMap();
				g_tiledMap->UpdateMixFromMap(g_background->TheSurface());
				scrolled_last_time = scrolled;
			}
		}
	}

	if (scrolled) 
	{
		
		if (deltaX)
			smoothY = deltaY = 0;
		else
			smoothX = 0;

		lastdeltaX = deltaX;
		lastdeltaY = deltaY;

		
	   	
	   	
		g_tiledMap->SetScrolling(true);

		
		uint32 accellTickDelta = s_scrollcurtick - s_accelTickStart;

		
		if ((smoothY == 0) && (smoothX == 0))
		{
			
			s_accelTickStart = s_scrollcurtick;

			
			accellTickDelta = 0;
		}

		
		sint32 accel = (accellTickDelta/k_TICKS_PER_ACCELERATION)+1;


		
        smoothX = std::min<sint32>(deltaX * accel, hscroll);
        smoothY = std::min<sint32>(deltaY * accel, vscroll);

        if (smoothX < -hscroll)
			smoothX = -hscroll;
		if (smoothY < -vscroll)
			smoothY = -vscroll;

		if (g_smoothScroll)
			g_tiledMap->ScrollMapSmooth(smoothX, smoothY);
		else
	  		g_tiledMap->ScrollMap(deltaX, deltaY);

	}
	else 
    {
		s_scrolltime = k_SMOOTH_START_TIME;

		if (isMouseScrolling) 
        {
			g_cursorManager->SetCursor(CURSORINDEX_DEFAULT);
		    isMouseScrolling = false;
		}

		smoothX = smoothY = 0;
	}

	scrolled_last_time = scrolled;
	return scrolled;
}

int ui_Process(void)
{
	if ( g_c3ui->TheMouse()->IsSuspended() ) return 0;

    uint32			curTicks    = Os::GetTicks();
	static uint32	lastTicks   = curTicks;

	if (ui_CheckForScroll()) 
    {
		do 
        {
			g_tiledMap->CopyMixDirtyRects(g_background->GetDirtyList());
			g_c3ui->Draw();
		} 
        while (ui_CheckForScroll());

		g_tiledMap->RetargetTileSurface(NULL);
		g_tiledMap->Refresh();
		g_tiledMap->InvalidateMap();

		lastTicks = curTicks;
	} 
    else if (curTicks > lastTicks + k_TICKS_PER_GENERIC_FRAME) 
    {
		g_tiledMap->RestoreMixFromMap(g_background->TheSurface());
		g_background->Draw();
		
		lastTicks = curTicks;
	}
	
	g_c3ui->Process();

	return 0;
}



sint32 sharedsurface_Initialize( void ) 
{
	
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	return errcode; // Return ends code here needs to be considered if g_sharedSurface is really needed

#if 0
	Assert( g_sharedSurface == NULL );
	if ( !g_sharedSurface )
	{
		g_sharedSurface = aui_Factory::new_Surface(errcode, k_SHARED_SURFACE_WIDTH, k_SHARED_SURFACE_HEIGHT);

		Assert( AUI_NEWOK(g_sharedSurface,errcode) );
		if ( !AUI_NEWOK(g_sharedSurface,errcode) ) return errcode;
	}

	return errcode;
#endif
}

void sharedsurface_Cleanup( void )
{
	allocated::clear(g_sharedSurface);
}

int sprite_Initialize(void)
{	
	g_screenManager = new ScreenManager();

	spritegrouplist_Initialize();
	
	g_director = new Director();

	return 0;
}

int sprite_Update(aui_Surface *surf)
{



	return 0;
}

void sprite_Cleanup(void)
{
	spritegrouplist_Cleanup();

    allocated::clear(g_director);
    allocated::clear(g_screenManager);
}


void ZoomPad_ZoomCallback()
{
	
	MainControlPanel::UpdateZoom();
}

int tile_Initialize(BOOL isRestoring)
{
	g_tiledMap = new TiledMap(*g_theWorld->GetSize());

	
	ZoomPad_ZoomCallback();		
	g_tiledMap->SetZoomCallback(ZoomPad_ZoomCallback);

	g_tiledMap->LoadTileset();


	if (!isRestoring)
		g_tiledMap->PostProcessMap();

	RECT rect = 
	{
		g_background->X(), 
		g_background->Y(), 
		g_background->X() + g_background->Width(),
		g_background->Y() + g_background->Height() 
	};
	
	g_tiledMap->Initialize(&rect);

	g_tiledMap->Refresh();

	return 0;
}

void tile_Cleanup(void)
{
    allocated::clear(g_tiledMap);
}

int radar_Initialize(void)
{
	g_radarMap->Setup();

	return 0;
}

int main_RestoreGame(const MBCHAR *filename)
{
	return g_civApp->LoadSavedGame(filename);
}


int main_Restart()
{
	return g_civApp->RestartGame();
}

static HWND s_taskBar   = NULL;

void main_HideTaskBar(void)
{
	if (g_hideTaskBar)
	{
		s_taskBar = FindWindow("Shell_TrayWnd", NULL);

		if (s_taskBar)
		{
			ShowWindow(s_taskBar, SW_HIDE);
		}
	}
}

void main_RestoreTaskBar(void)
{
	if (s_taskBar)
	{
		ShowWindow(s_taskBar, SW_SHOWDEFAULT);
	}
}

void ui_CivAppProcess(void)
{
	if (g_civApp)
		g_civApp->Process();
}


void AtExitProc(void)
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	DPRINTF(k_DBG_FIX, ("Existing game\n"));
	c3debug_CloseDebugLog();
#endif

	printf("At exit.\n");

#if defined(USE_SDL)
# if 0
    // What about this?
    Mix_CloseAudio();
# endif

    SDL_Quit();
#endif
}

BOOL g_no_timeslice; 
BOOL g_no_shell;  
BOOL g_no_exit_action; 
BOOL g_cmdline_load;
char g_cmdline_load_filename[160]; 
sint32 g_cheat_age;




BOOL g_launchScenario = FALSE;

#ifndef _BFR_
char * c3debug_ExceptionStackTraceFromFile(FILE *f);
#endif

void ParseCommandLine(PSTR szCmdLine)
{
#ifndef _BFR_
	if(stricmp(szCmdLine, "crash.txt") == 0) {
		FILE *txt = fopen("crashmap.txt", "w");
		fprintf(txt, "%s\n", c3debug_ExceptionStackTraceFromFile(fopen("crash.txt", "r")));
		fclose(txt);
		exit(0);
	}
#endif

	char * archive_file = strstr(szCmdLine, "-l");

	if (NULL == archive_file)
	{
		g_cmdline_load = FALSE;
	}
	else
	{
		g_cmdline_load = TRUE; 
		g_useIntroMovie = FALSE;
		g_no_shell = TRUE;

		archive_file += 2;

		BOOL gotQuote = FALSE;
		if(*archive_file == '"')
		{
			archive_file++;
			gotQuote = TRUE;
		}
		
		sint32 i;

		// space or end of string mark end of the file name
		for (i=0; i<160 &&  (0 != *archive_file) && 
				 (((!gotQuote && ' ' != *archive_file)) ||
				  (gotQuote && '"' != *archive_file)); i++, archive_file++) { 
			g_cmdline_load_filename[i] = *archive_file; 
		}

		if (160 <= i)
		{
			g_cmdline_load = FALSE;
		}
		else
		{
			g_cmdline_load_filename[i] = 0; 
		}
	}

	
	MBCHAR * scenName = strstr(szCmdLine, "-s");

	if (NULL != scenName) { 
		

		
		scenName += 2;

		
		sint32		i;

		for (i=0; i<k_SCENARIO_NAME_MAX && *scenName != '\0'; i++, scenName++)
		{
			g_scenarioName[i] = *scenName;
		}

		if (i >= k_SCENARIO_NAME_MAX)
		{
			g_launchScenario = FALSE;
			std::fill(g_scenarioName, g_scenarioName + k_SCENARIO_NAME_MAX,'\0');
		}
		else
		{
			g_launchScenario = TRUE;
			g_scenarioName[i] = '\0';
		}
	}

	g_no_timeslice = (NULL != strstr(szCmdLine, "notimeslice"));
	if(!g_cmdline_load)
		g_no_shell =  (NULL != strstr(szCmdLine, "noshell")); 
	g_no_exit_action = (NULL != strstr(szCmdLine, "noexitaction")); 
	g_exclusiveMode = !(NULL != strstr(szCmdLine, "nonexclusive"));
	g_hideTaskBar = (NULL != strstr(szCmdLine, "hidetaskbar"));
	if(!g_cmdline_load)
		g_useIntroMovie = !(NULL != strstr(szCmdLine, "nointromovie"));
	g_noAssertDialogs = (NULL != strstr(szCmdLine, "noassertdialogs"));
	g_runInBackground = (NULL != strstr(szCmdLine, "runinbackground"));
	
	g_eventLog = (NULL != strstr(szCmdLine, "eventlog"));

	
	g_createDirectDrawOnSecondary = (NULL != strstr(szCmdLine, "multimon"));
	g_autoAltTab = (NULL != strstr(szCmdLine, "autoalttab"));

#ifdef _DEBUG
	if (NULL != strstr(szCmdLine, "age1"))
	{
		g_cheat_age = 1;
	}
	else if (NULL != strstr(szCmdLine, "age2"))
	{
		g_cheat_age = 2;
	}
	else if (NULL != strstr(szCmdLine, "age3"))
	{
		g_cheat_age = 3;
	}
	else if (NULL != strstr(szCmdLine, "age4"))
	{
		g_cheat_age = 4;
	}
	else if (NULL != strstr(szCmdLine, "age5"))
	{
		g_cheat_age = 5;
	}
#endif // _DEBUG

	if (g_noAssertDialogs)
	{
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
	}
}

int WINAPI main_filehelper_GetOS(void)
{
	OSVERSIONINFO osvi;
	
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	GetVersionEx( &osvi );

	return osvi.dwPlatformId;
}

static LONG _cdecl main_CivExceptionHandler(LPEXCEPTION_POINTERS pException)
{
#if defined(_DEBUG)

	MBCHAR * s;
	
	switch (pException->ExceptionRecord->ExceptionCode) 
	{
	case EXCEPTION_ACCESS_VIOLATION:        s = "Access Violation";                break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:   s = "Array Bounds Exceeded";           break;
	case EXCEPTION_BREAKPOINT:              s = "Breakpoint";                      break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:   s = "Datatype Misalignment";           break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:    s = "Floating Point Denormal Operand"; break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:      s = "Floating Point Divide by Zero";   break;
	case EXCEPTION_FLT_INEXACT_RESULT:      s = "Floating Point Inexact Result";   break;
	case EXCEPTION_FLT_INVALID_OPERATION:   s = "Floating Point Invalid Operation";break;
	case EXCEPTION_FLT_OVERFLOW:            s = "Floating Point Overflow";         break;
	case EXCEPTION_FLT_STACK_CHECK:         s = "Floating Point Stack Check";      break;
	case EXCEPTION_FLT_UNDERFLOW:           s = "Floating Point Underflow";        break;
	case EXCEPTION_GUARD_PAGE:              s = "Guard Page";                      break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:     s = "Illegal Instruction";             break;
	case EXCEPTION_IN_PAGE_ERROR:           s = "In-page Error";                   break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:      s = "Integer Divide By Zero";          break;
	case EXCEPTION_INT_OVERFLOW:            s = "Integer Overflow";                break;
	case EXCEPTION_INVALID_DISPOSITION:     s = "Invalid Disposition";             break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:s = "Non-Continuable Exception";       break;
	case EXCEPTION_PRIV_INSTRUCTION:        s = "Privileged Instruction";          break;
	case EXCEPTION_SINGLE_STEP:             s = "Single Step";                     break;
	case EXCEPTION_STACK_OVERFLOW:          s = "Stack Overflow";                  break;
	default:                                s = "Unknown";                         break;
	}

	DPRINTF(k_DBG_FIX, ("Exception: '%s' thrown.\n", s));
	s = c3debug_ExceptionStackTrace(pException);
	DPRINTF(k_DBG_FIX, ("Exception Stack Trace:\n%s\n", s));

	return EXCEPTION_CONTINUE_SEARCH;

#else // _DEBUG

#ifdef _BFR_
	if (g_logCrashes) 
#endif
	{
		FILE * crashLog = fopen("logs" FILE_SEP "crash.txt", "w");
		if (!crashLog)
			crashLog = fopen("crash.txt", "w");

		if (crashLog) 
        {
            fprintf(crashLog, "Version %s\n", Os::GetExeVersion().c_str());
			fprintf(crashLog, "%s\n", c3debug_ExceptionStackTrace(pException));
		    fclose(crashLog);
		}
	}

	return EXCEPTION_EXECUTE_HANDLER;

#endif // _DEBUG
}

BOOL main_CheckDirectX(void)
{
	BOOL found = FALSE;

	
	HANDLE dll = LoadLibrary( "dll" FILE_SEP "util" FILE_SEP "dxver" );
	if ( dll ) {
		
		typedef BOOL (WINAPI *FuncType)( DWORD *pVersion );
		FuncType GetDirectXVersion = (FuncType)GetProcAddress( (HINSTANCE)dll, "MicrosoftDirectXInstalled" );
		if ( !GetDirectXVersion )
		{
			FreeLibrary( (HINSTANCE)dll );
			return AUI_ERRCODE_HACK;    /// @todo Check: effectively this means true???
		}

		
		if(GetDirectXVersion( &g_dxver ) > 0) {
			found = TRUE;
		}

		FreeLibrary( (HINSTANCE)dll );
	} else {
		c3errors_FatalDialog("DLL", "Cannot find dxver.dll");
	}

	return found;
}

void main_InitializeLogs(void)
{
	time_t		ltime;
	time(&ltime);
	struct tm * now = localtime(&ltime);

#if defined(_DEBUG) && defined(_DEBUGTOOLS)
	Debug_Open();
#endif

#if defined(_DEBUG)
    g_splash_old = GetTickCount();
#endif

	char		timebuf[100];
	strftime(timebuf, 100, "Log started at %I:%M%p %m/%d/%Y", now);

#if defined(_DEBUG) || defined(USE_LOGGING)
	c3debug_InitDebugLog();
//	c3debug_SetDebugMask(k_DBG_FIX | k_DBG_DATABASE | k_DBG_NET | k_DBG_GAMESTATE | k_DBG_UI | k_DBG_SLIC, 1);
//	c3debug_SetDebugMask(k_DBG_FIX | k_DBG_DATABASE | k_DBG_NET | k_DBG_GAMESTATE | k_DBG_UI | k_DBG_SLIC | k_DBG_DIPLOMACY | k_DBG_SCHEDULER, 1);
//	c3debug_SetDebugMask(k_DBG_FIX | k_DBG_DATABASE | k_DBG_NET | k_DBG_GAMESTATE | k_DBG_UI | k_DBG_SLIC | k_DBG_DIPLOMACY, 1);
//	c3debug_SetDebugMask(k_DBG_FIX | k_DBG_DATABASE | k_DBG_NET | k_DBG_GAMESTATE | k_DBG_UI | k_DBG_SLIC | k_DBG_SCHEDULER | k_DBG_SCHEDULER_ALL, 1);
	c3debug_SetDebugMask(k_DBG_FIX | k_DBG_DATABASE | k_DBG_NET | k_DBG_GAMESTATE | k_DBG_UI | k_DBG_SLIC | k_DBG_SCHEDULER, 1);
//	c3debug_SetDebugMask(k_DBG_FIX | k_DBG_DATABASE | k_DBG_NET | k_DBG_GAMESTATE | k_DBG_UI | k_DBG_SLIC | k_DBG_AI | k_DBG_SCHEDULER, 1);
#endif

	DPRINTF(k_DBG_FIX, ("** BUILD EXE :%s\n", Os::GetExeName().c_str()));

	HANDLE fileHandle = CreateFile(Os::GetExeName().c_str(), 
	                               GENERIC_READ,
	                               FILE_SHARE_READ,
	                               NULL,
	                               OPEN_ALWAYS,
	                               FILE_ATTRIBUTE_NORMAL,
	                               NULL
	                              );
	if(fileHandle != INVALID_HANDLE_VALUE)
	{
		FILETIME	crTime, laTime, lwTime;
		if(GetFileTime( fileHandle, &crTime, &laTime, &lwTime))
		{
			DPRINTF(k_DBG_FIX, ("** BUILD INFO:\n"));
			FILETIME    creationTime;
			SYSTEMTIME	systemTime;
			if (FileTimeToLocalFileTime( &crTime, &creationTime))
			{
				if (FileTimeToSystemTime(&creationTime, &systemTime))
				{
					DPRINTF(k_DBG_FIX, ("**     Creation Time: %#.2d/%#.2d/%d %#.2d:%#.2d:%#.2d.%#.3d\n",
							systemTime.wMonth, systemTime.wDay, systemTime.wYear,
							systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds));
				}
			}
			if (FileTimeToLocalFileTime( &lwTime, &creationTime))
			{
				if (FileTimeToSystemTime(&creationTime, &systemTime))
				{
					DPRINTF(k_DBG_FIX, ("**   Last Write Time: %#.2d/%#.2d/%d %#.2d:%#.2d:%#.2d.%#.3d\n",
							systemTime.wMonth, systemTime.wDay, systemTime.wYear,
							systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds));
				}
			}
		}

		CloseHandle(fileHandle);
	}

	DPRINTF(k_DBG_FIX, ("%s\n", timebuf));

	DPRINTF(k_DBG_FIX, ("** SYSTEM INFO:\n"));

	OSVERSIONINFO		osv;
	osv.dwOSVersionInfoSize = sizeof(osv);
	GetVersionEx(&osv);

	DPRINTF(k_DBG_FIX,     ("**   Windows Version: %d.%d %s build %d, %s\n", osv.dwMajorVersion, osv.dwMinorVersion,
															(osv.dwPlatformId == VER_PLATFORM_WIN32_NT) ? "NT" : "",
															osv.dwBuildNumber,
															osv.szCSDVersion));

	DWORD   cNameSize = MAX_COMPUTERNAME_LENGTH+1;
	MBCHAR  cName[MAX_COMPUTERNAME_LENGTH+1];

	if (GetComputerName((LPTSTR)cName, &cNameSize))
	{
		DPRINTF(k_DBG_FIX, ("**     Computer Name: %s\n", cName));
	}

	cNameSize = MAX_COMPUTERNAME_LENGTH+1;
	if (GetUserName((LPTSTR)cName, &cNameSize))
	{
		DPRINTF(k_DBG_FIX, ("**         User Name: %s\n", cName));
	}

	MEMORYSTATUS		ms;
	ms.dwLength = sizeof(ms);
	GlobalMemoryStatus(&ms);

	DPRINTF(k_DBG_FIX, ("**      Physical RAM: %u bytes\n", ms.dwTotalPhys));
	DPRINTF(k_DBG_FIX, ("**     Available RAM: %u bytes\n", ms.dwAvailPhys));
	DPRINTF(k_DBG_FIX, ("**    Page File Size: %u bytes\n", ms.dwTotalPageFile));
	DPRINTF(k_DBG_FIX, ("**    Page File Free: %u bytes\n", ms.dwAvailPageFile));
	DPRINTF(k_DBG_FIX, ("**          Total VM: %u bytes\n", ms.dwTotalVirtual));
	DPRINTF(k_DBG_FIX, ("**      Available VM: %u bytes\n", ms.dwAvailVirtual));
	DPRINTF(k_DBG_FIX, ("**       Memory load: %u%% used\n", ms.dwMemoryLoad));

	DPRINTF(k_DBG_FIX, ("**  Direct X Version: 0x%x\n", g_dxver));
	DPRINTF(k_DBG_FIX, ("**    Cur ScreenSize: %d x %d\n", g_ScreenWidth, g_ScreenHeight));
}

#if defined(__GNUC__)

int main(int argc, char **argv)
{
	int const   r = CivMain(argc, argv);

	if (r < 0) 
	{
		DoFinalCleanup(r);
	}

	return 0;
}

#else // __GNUC__

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	// This stuff will have to be moved into a new int main(int argc, char **argv)
	// once graphics are also ported to SDL

#if defined(WIN32) || defined(_WINDOWS)

#if defined(_DEBUG)
	Os::SetThreadName("WinMain");
#endif // _DEBUG

	// Make sure old versions of DDHELP.EXE won't keep files open
	HINSTANCE handle = LoadLibrary("DDRAW.DLL");
	if (0 != handle) {
		FreeLibrary(handle);
	}
#endif // WIN32 || _WINDOWS

	atexit(AtExitProc);

	__try
	{
		return CivMain(hInstance, hPrevInstance, szCmdLine, iCmdShow);
	}
	__except (main_CivExceptionHandler(GetExceptionInformation()))
	{
		DoFinalCleanup();
	}

	return 0;
}

#endif // __GNUC__


void main_DisplayPatchDisclaimer()
{
	bool    isDisclaimerShown   = false;
	FILE *  f                   = fopen(k_DisclaimerName, "rb");

	if (f && (0 == fseek(f, 0, SEEK_END))) 
	{
		long const  positionEnd = ftell(f);

		// Reposition at begin
		fclose(f);
		f = fopen(k_DisclaimerName, "rb");

		if (f && (positionEnd > 0))
		{
			size_t const    filesize    = static_cast<size_t>(positionEnd);
			MBCHAR *        message     = new MBCHAR[filesize+1];
			size_t const    readCount   = c3files_fread
			                                (message, sizeof(MBCHAR), filesize, f);
			message[readCount]  = 0;
			MessageBox(NULL, message, "Call to Power", MB_OK | MB_ICONEXCLAMATION);
			isDisclaimerShown   = true;
		}
	}

	if (f)
	{
		fclose(f);
	}

	if (!isDisclaimerShown)
	{
		c3errors_FatalDialog(appstrings_GetString(APPSTR_INITIALIZE),
		                     appstrings_GetString(APPSTR_CANTFINDFILE)
		                    );
	}
}

#if defined(__GNUC__)
int CivMain
(
	int		    iCmdShow,   // argc 
	char *	    szCmdLine   // argv
)
{
	void *      hInstance   = NULL;
#else	// __GNUC__
int WINAPI CivMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{

	HWND hwnd = FindWindow (gszMainWindowClass, gszMainWindowName);
	if (hwnd) {
		
		if (IsIconic(hwnd)) {
			ShowWindow(hwnd, SW_RESTORE);
		}
		SetForegroundWindow (hwnd);
		
		return FALSE;
	}
#endif // __GNUC__
	
	char exepath[_MAX_PATH];
	if (GetModuleFileName(NULL, exepath, _MAX_PATH) != 0) 
	{
		ctpregistry_SetKeyValue(HKEY_CLASSES_ROOT,
								".c2g",
								NULL,
								"c2gfile");
	
		ctpregistry_SetKeyValue(HKEY_CLASSES_ROOT,
								"c2gfile",
								NULL, 
								"Call to Power 2 saved game");

		char launchcommand[_MAX_PATH];
		strcpy(launchcommand, exepath);
		strcat(launchcommand, " -l\"%1\"");
		ctpregistry_SetKeyValue(HKEY_CLASSES_ROOT,
								"c2gfile\\Shell\\Open\\command",
								NULL,
								launchcommand);
			
		char * lastbackslash = strrchr(exepath, FILE_SEPC);
		if(lastbackslash) {
			*lastbackslash = 0;
			SetCurrentDirectory(exepath);
		}
	}

	appstrings_Initialize();

	setlocale(LC_COLLATE, appstrings_GetString(APPSTR_LOCALE));

	if (!main_CheckDirectX()) {

		c3errors_FatalDialog(appstrings_GetString(APPSTR_DIRECTX),
		                     appstrings_GetString(APPSTR_NEEDDIRECTX));
	}

	
#if defined(_DEBUG) || defined(USE_LOGGING)
	main_InitializeLogs();
#endif
#if !defined(_DEBUG) && !defined(_BFR_)
	
	SECURITY_ATTRIBUTES		sa;

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	
	CreateDirectory((LPCTSTR)"logs", &sa);
#endif

	ParseCommandLine(szCmdLine);

	allocated::reassign(g_civApp, new CivApp());

	if (g_cmdline_load) {
		g_civApp->InitializeApp(hInstance, iCmdShow);

		ScenarioPack	*pack;
		Scenario		*scen;

		if (g_civScenarios->FindScenarioFromSaveFile(g_cmdline_load_filename, &pack, &scen)) {
			
			g_civPaths->SetCurScenarioPath(scen->m_path);
			
			
			g_civPaths->SetCurScenarioPackPath(pack->m_path);
			
			
			g_theProfileDB->SetIsScenario(TRUE);

			
			if (g_civScenarios->ScenarioHasSavedGame(scen)) {
				
				spnewgamescreen_scenarioExitCallback(NULL, 0, NULL, NULL);
			} else {
				
				
				spnewgamescreen_displayMyWindow();
			}
		} else {
			main_RestoreGame(g_cmdline_load_filename);
		}
	} else if (g_no_shell) {
		g_civApp->QuickInit(hInstance, iCmdShow);
	} else if (g_launchScenario) {	
		g_civApp->InitializeApp(hInstance, iCmdShow);
		ScenarioPack	*pack;
		Scenario		*scen;

		if (g_civScenarios->FindScenario(g_scenarioName, &pack, &scen)) {
			
			g_civPaths->SetCurScenarioPath(scen->m_path);
			
			
			g_civPaths->SetCurScenarioPackPath(pack->m_path);
			
			
			g_theProfileDB->SetIsScenario(TRUE);

			
			if (g_civScenarios->ScenarioHasSavedGame(scen)) {
				
				spnewgamescreen_scenarioExitCallback(NULL, 0, NULL, NULL);
			} else {
				
				
				spnewgamescreen_displayMyWindow();
			}
		}
	} else {
		g_civApp->InitializeApp(hInstance, iCmdShow);
	}

	MSG			msg;
	msg.wParam  = 0;

	for (gDone = FALSE; !gDone; )
	{
		g_civApp->Process();

		while (PeekMessage(&msg, gHwnd, 0, 0, PM_REMOVE) && !g_letUIProcess)
		{
			if (WM_QUIT == msg.message)
			{
				gDone = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		g_letUIProcess = FALSE;
	}

	return msg.wParam;
}

#if defined(_DEBUG)

void DoFinalCleanup(int)
{
	if (!g_exclusiveMode)
	{
		main_RestoreTaskBar();
	}

	if (g_civApp)
	{
		g_civApp->QuitGame();
		allocated::clear(g_civApp);
	}

	sliccmd_clear_symbols();
	SlicSegment::Cleanup();
	appstrings_Cleanup();

#if defined(_DEBUGTOOLS)
	Debug_Close();
#endif
}

#else // _DEBUG

void DoFinalCleanup(int exitCode)
{
	static bool s_cleaningUpTheApp = false;

	if (g_c3ui)
	{
		g_c3ui->DestroyDirectScreen();
	}

	ShowWindow(gHwnd, SW_HIDE);

	if (!s_cleaningUpTheApp)
	{
		s_cleaningUpTheApp = true;

		if (g_civApp)
		{
			g_civApp->CleanupApp();
		}
	}

	exit(exitCode);
}

#endif  // _DEBUG

#define k_MSWHEEL_ROLLMSG		0xC7AF

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	if (!gDone && g_c3ui)
	{
		(void) g_c3ui->HandleWindowsMessage(hwnd, iMsg, wParam, lParam);
	}

	static bool swallowNextChar = false;

	switch (iMsg) 
	{
	case WM_CHAR:
		if (!swallowNextChar)
		{
			ui_HandleKeypress(wParam, lParam);
		}
		swallowNextChar = false;
		break;
		
	case WM_KEYDOWN:
		switch (wParam) 
		{
		case VK_F1:
		case VK_F2:
		case VK_F3:
		case VK_F4:
		case VK_F5:
		case VK_F6:
		case VK_F7:
		case VK_F8:
		case VK_F9:
			if(!(GetKeyState(VK_SHIFT) & 0x8000)) {
				ui_HandleKeypress(wParam - VK_F1 + '1' + 128, lParam);
			}
			break;
		case VK_F10:
			if(!(GetKeyState(VK_SHIFT) & 0x8000)) {
				ui_HandleKeypress(wParam - VK_F10 + '0' + 128, lParam);
			}
			break;
		case VK_F11:
			if(!(GetKeyState(VK_SHIFT) & 0x8000)) {
				ui_HandleKeypress('!' + 128, lParam);
			}
			break;
		case VK_F12:
			if(!(GetKeyState(VK_SHIFT) & 0x8000)) {
				ui_HandleKeypress('@' + 128, lParam);
			}
			break;
		case VK_TAB:
			ui_HandleKeypress('\t' + 128, lParam);
			swallowNextChar = true;
			return 0;
		case VK_RETURN:
			ui_HandleKeypress('\r' + 128, lParam);
			swallowNextChar = true;
			return 0;
		case VK_BACK:
			ui_HandleKeypress(8 + 128, lParam);
			swallowNextChar = true;
			return 0;
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			ui_HandleKeypress(wParam + 256, lParam);
			break;
		}
		break;
	case WM_SYSKEYDOWN:
		
		if(wParam == VK_F10) {
			if(!(GetKeyState(VK_SHIFT) & 0x8000)) {
				ui_HandleKeypress(wParam - VK_F10 + '0' + 128, lParam);
			}
		}
		break;
	case WM_CLOSE:
		if (hwnd != gHwnd) break;

		gDone = TRUE;
		DoFinalCleanup();
		DestroyWindow(hwnd);
		gHwnd = NULL;
		return 0;

	case k_MSWHEEL_ROLLMSG :
		{
			sint16 dir = HIWORD(wParam);
			if (dir >= 0) dir = 1;
			if (dir < 0) dir = -1;
			ui_HandleMouseWheel(dir);
		}

	case WM_VSCROLL: 
		{
			sint16 scrollCode = LOWORD(wParam);
			if (scrollCode == SB_LINEDOWN) 
			{
				ui_HandleMouseWheel((sint16)-1);
			}
			else if (scrollCode == SB_LINEUP) 
			{
				ui_HandleMouseWheel((sint16)1);
			}
		}
		break;
	case WM_MOUSEWHEEL:
		ui_HandleMouseWheel((sint16)HIWORD(wParam));
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void DisplayFrame(aui_Surface *surf)
{
	static double   fr_decay        = 0.85;
	static uint32   g_old_last_tick = Os::GetTicks();

	uint32          new_tick        = Os::GetTicks(); 
	double          d               = double(new_tick) - double(g_old_last_tick);

	g_old_last_tick = new_tick;

	if (d < 1.0) 
		return; 

	g_ave_frame_time = fr_decay * g_ave_frame_time + (1.0 - fr_decay) * d;
	g_ave_frame_rate = fr_decay * g_ave_frame_rate + (1.0 - fr_decay) * (1000.0/d);

	char str[80];
	sprintf(str, "ave frame rate %4.2f/sec - ave frame time %5.1fms", 
	        g_ave_frame_rate, g_ave_frame_time
	       );
	primitives_DrawText(surf, 100, 100, str, 1, 0);
}

BOOL ExitGame(void)
{
	return PostMessage(gHwnd, WM_CLOSE, 0, 0);
}
