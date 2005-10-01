//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// - Compiler version (for the Microsoft C++ compiler only).
//
// __GNUC__
// - Compiler version (for the GNU C++ compiler only).
//
// USE_LOGGING
// - Enable logging facilities - even when not using the debug build.
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
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "pixelutils.h"
#include "colorset.h"
#include "civapp.h"

#include "civ3_main.h"


#include "c3ui.h"
#include "c3blitter.h"
#include "c3memmap.h"
#include "background.h"
#include "radarwindow.h"
#include "statuswindow.h"
#include "debugwindow.h"
#include "aui_directmouse.h"
#include "aui_sdlmouse.h"
#include "aui_directsurface.h"
#include "aui_Factory.h"
#include "primitives.h"
#include "aui_directmoviemanager.h"
#include "c3window.h"


#include "pattern.h"
#include "picture.h"
#include "icon.h"
#include "keypress.h"
#include "CivPaths.h"
#include "c3errors.h"
#include "c3windows.h"
#include "c3_static.h"
#include "c3_button.h"
#include "c3_dropdown.h"
#include "aui_ldl.h"
#include "backgroundwin.h"
#include "gamefile.h"
#include "controlpanelwindow.h"
#include "keypress.h"

#if !defined(__GNUC__) // TODO: replacement needed (wine doesnt have these headers...)
#include "directvideo.h"
#include "videoutils.h"
#endif

#if defined(USE_SDL)
#include <SDL.h>
#include <SDL_mixer.h>
#endif
#ifdef HAVE_X11
#include <X11/Xlib.h>
#endif

#include "radarmap.h"
#include "statswindow.h"
#include "aui_directkeyboard.h"
#include "aui_sdlkeyboard.h"
#include "ancientwindows.h"
#include "splash.h"
#include "bevellesswindow.h"


#include "ErrMsg.h"
#include "StrDB.h"
#include "AdvanceRecord.h"

#include "DB.h"
#include "BuildingRecord.h"
#include "TerrainRecord.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "gameinit.h"
#include "TradePool.h"
#include "pollution.h"
#include "SelItem.h"
#include "player.h"
#include "UnitPool.h"
#include "profileDB.h"
#include "CivilisationDB.h"
#include "TurnCnt.h"
#include "ConstDB.h"


#include "RoboInit.h"




#include "screenmanager.h"
#include "Sprite.h"
#include "UnitSpriteGroup.h"
#include "tiledmap.h"
#include "SpriteStateDB.h"
#include "SpriteGroupList.h"
#include "director.h"

#include "GameOver.h"
#include "network.h"


#include "c3cmdline.h"
#include "workwindow.h"
#include "statswindow.h"
#include "greatlibrary.h"
#include "workwin.h"
#include "helptile.h"
#include "messagewin.h"
#include "spnewgamewindow.h"


#include "scenariowindow.h"
#include "initialplaywindow.h"
#include "grabitem.h"

#include "appstrings.h"

#include "debugmemory.h"

#include "SpecialEffectRecord.h"

#include "ctp2_listbox.h"

#include "cursormanager.h"
#include "MainControlPanel.h"
#include "ctp2_Window.h"
#include "ctp2_menubar.h"


#include <locale.h>


#include "civscenarios.h"
extern CivScenarios		*g_civScenarios;

#include "ctpregistry.h"
#include "sliccmd.h"    // sliccmd_clear_symbols

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL (WM_MOUSELAST+1)  
#endif

#ifdef _DEBUG
#include "debug.h"
#endif

#define k_LDLName				"civ3.ldl"
#define k_LDL640Name			"civ3_640.ldl"
#define k_CursorName			"cursor2.tif"

#define k_SHARED_SURFACE_WIDTH	1024
#define k_SHARED_SURFACE_HEIGHT	768
#define k_SHARED_SURFACE_BPP	16

#define k_MAX_CMD_LINE 1024

#define k_SMOOTH_PIX_SEC_PER_SEC	8.0f
#define k_SMOOTH_MIN_VELOCITY		4.0f			
#define k_SMOOTH_MAX_VELOCITY		(k_TILE_PIXEL_HEIGHT>>1) 			
#define k_SMOOTH_START_TIME			(int)(1000.0f*(float)sqrt(k_SMOOTH_MIN_VELOCITY*2.0f/k_SMOOTH_PIX_SEC_PER_SEC))
#define k_SMOOTH_SLOW_TIME			(int)(500.0f*(float)sqrt(k_SMOOTH_MAX_VELOCITY*2.0f/k_SMOOTH_PIX_SEC_PER_SEC))

extern sint32 g_splash_cur; 
extern sint32 g_splash_old; 
extern char g_splash_buf[100]; 


HWND			gHwnd;
HINSTANCE		gHInstance;
BOOL			gDone = FALSE;
LPCSTR			gszMainWindowClass = "CTP II";
LPCSTR			gszMainWindowName = "CTP II";
sint32			g_ScreenWidth = 0;
sint32			g_ScreenHeight = 0;


C3UI				*g_c3ui = NULL;
StatusWindow		*g_statusWindow = NULL;
extern DebugWindow	*g_debugWindow;
aui_Surface			*g_sharedSurface = NULL;

BOOL			g_smoothScroll = FALSE;



RECT			g_backgroundViewport = { 0, 0, 0, 0 };
sint32			g_is565Format = TRUE;
sint32			g_modalWindow = 0;

BOOL			g_helpMode = TRUE;

#ifdef _DEBUG
extern sint32 g_debugOwner;
extern Player** g_player;
extern BOOL		g_toggleAdvances ;
extern SelectedItem *g_selected_item; 
#endif


extern StringDB						*g_theStringDB; 
extern Database <GovernmentRecord>	*g_theGovernmentDB; 
extern CivilisationDatabase			*g_theCivilisationDB ;
extern ConstDB						*g_theConstDB; 
extern World						*g_theWorld; 
extern UnitPool						*g_theUnitPool; 
extern Pollution					*g_thePollution;
extern sint32					    g_is_rand_test; 
extern void ai_rand_test(); 
extern ProfileDB *g_theProfileDB; 
extern TurnCount *g_turn;


static uint32   s_scrollcurtick	=0;
static uint32   s_scrolllasttick=0;
static sint32   s_scrolltime	=k_SMOOTH_START_TIME;
static uint32   s_accelTickStart = 0;


extern CivPaths				*g_civPaths;


extern ColorSet				*g_colorSet;





extern C3Window		*g_turnWindow;
extern StatsWindow			*g_statsWindow;
extern ControlPanelWindow	*g_controlPanel;

sint32	g_terrainPollution ;


SpriteStateDB               *g_theSpriteStateDB;


SpriteStateDB				*g_theGoodsSpriteStateDB;
SpriteStateDB				*g_theCitySpriteStateDB;

Director					*g_director;
double						g_ave_frame_rate = 10.0;
double						g_ave_frame_time = 200.0;
ScreenManager				*g_screenManager = NULL;


TiledMap					*g_tiledMap = NULL;


RadarMap					*g_radarMap = NULL;


CivApp						*g_civApp = NULL;


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

#if defined(__GNUC__)
int CivMain(int argc, char **argv);
#else
int WINAPI CivMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow);
#endif


Network g_network;


DrawHandler background_draw_handler;


extern Background			*g_background;

extern Splash				*g_splash;

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

sint32		g_god;
uint32		g_dxver = 0;


bool g_e3Demo = false;

bool g_autoAltTab = false;

#define RELDBG(x) { FILE *f = fopen("reldbg.txt", "a"); fprintf x; fclose(f); }


int ui_Initialize(void)
{
	AUI_ERRCODE auiErr;

	char s[_MAX_PATH+1];

	
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

	
	if (g_c3ui->Primary()->PixelFormat() == AUI_SURFACE_PIXELFORMAT_555) {
		g_is565Format = FALSE;
	} else {
		g_is565Format = TRUE;
	}

	
	g_colorSet->Initialize();

	SPLASH_STRING("Initializing Paths...");

    int     i;

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

#ifdef WIN32
	if (!GetWindowsDirectory(s, _MAX_PATH)) {
		c3errors_FatalDialog(appstrings_GetString(APPSTR_FONTS), 
								appstrings_GetString(APPSTR_NOWINDOWSDIR));
	}
	strcat(s, FILE_SEP);
	strcat(s, "fonts");
	g_c3ui->AddBitmapFontSearchPath(s);
#elif defined(HAVE_X11)
	Display *display = g_c3ui->getDisplay();
	int ndirs;
	bool noPath = true;
	char **fontpaths = XGetFontPath(display, &ndirs);
	if (fontpaths) {
		struct stat st = { 0 };
		for (int i = 0; i < ndirs; i++) {
			int rc = stat(fontpaths[i], &st);
			if ((rc == 0) && (S_ISDIR(st.st_mode))) {
				g_c3ui->AddBitmapFontSearchPath(fontpaths[i]);
				// Make some default paths get added, too
				//noPath = false;
			}
		}
		XFreeFontPath(fontpaths);
	}
	// Fontpath just contains server(s)?
	if (noPath) {
		const int maxPaths = 3;
		const char* fontPaths[maxPaths] = {
			"/usr/share/fonts",
			"/usr/X11R6/lib/X11/fonts",
			"/usr/lib/X11/fonts"
		};
		const int maxDirs = 4;
		const char* fontDirs[maxDirs] = {
			"TTF",
			"corefonts",
			"truetype",
			"truetype/msttcorefonts"
		};
		for (int pIdx = 0; pIdx < maxPaths; pIdx++) {
			for (int dIdx = 0; dIdx < maxPaths; dIdx++) {
				struct stat st = { 0 };
				snprintf(s, sizeof(s), "%s/%s",
			         	fontPaths[pIdx], fontDirs[dIdx]);
				int rc = stat(s, &st);
				if ((rc == 0) && (S_ISDIR(st.st_mode))) {
					g_c3ui->AddBitmapFontSearchPath(s);
				}
			}
		}
	}
#endif

	
    for (i = 0; g_civPaths->FindPath(C3DIR_VIDEOS, i, s); ++i)
    {
        if (s[0])
        {
            g_c3ui->AddMovieSearchPath(s);
        }
    }
	
	SPLASH_STRING("Creating Blitter...");

	g_c3ui->RegisterObject(new C3Blitter);
	g_c3ui->RegisterObject(new C3MemMap);

	SPLASH_STRING("Creating Mouse...");
	
	BOOL const mouseExclusiveMode = TRUE;
#if defined(__AUI_USE_SDL__)
	g_c3ui->RegisterObject(new aui_SDLMouse(&auiErr, "CivMouse", mouseExclusiveMode));
#else
	g_c3ui->RegisterObject(new aui_DirectMouse(&auiErr, "CivMouse", mouseExclusiveMode));
#endif

	SPLASH_STRING("Creating Keyboard...");

#if defined(__AUI_USE_SDL__)
	g_c3ui->RegisterObject(new aui_SDLKeyboard(&auiErr));
#else
	g_c3ui->RegisterObject(new aui_DirectKeyboard(&auiErr));
#endif


#if !defined(__GNUC__)
	SPLASH_STRING("Creating Movie manager...");
	g_c3ui->RegisterObject(new aui_DirectMovieManager());
#endif

	SPLASH_STRING("Starting Mouse...");
	auiErr = g_c3ui->TheMouse()->Start();
	Assert(auiErr == AUI_ERRCODE_OK);
	if ( auiErr != AUI_ERRCODE_OK ) return 14;

#ifdef WIN32
	while ( ShowCursor( FALSE ) >= 0 )
	;
#endif

	return AUI_ERRCODE_OK;
}

void ui_HandleMouseWheel(sint16 delta)
{
	if (!g_civApp) return;

	
	
	ctp2_ListBox *box = ctp2_ListBox::GetMouseFocusListBox();
	if (box != NULL) {
		if (delta) {
			if (delta < 0) {
				box->ForceScroll(0, 1);
			} else {
				box->ForceScroll(0, -1);
			}
			
			return;
		}
	}


	


		
		BOOL scrolled = FALSE;
		
		if (g_civApp->IsGameLoaded()) {
			if (g_tiledMap) {
				if (delta < 0) {
					g_tiledMap->ScrollMap(0, 2);
					scrolled = TRUE;
				} else
					if (delta > 0) {
						g_tiledMap->ScrollMap(0, -2);
						scrolled = TRUE;
					}
			}
		}

		if (scrolled) {
			g_tiledMap->RetargetTileSurface(NULL);
			g_tiledMap->Refresh();
			g_tiledMap->InvalidateMap();
			g_tiledMap->ValidateMix();
		}
}





bool
compute_scroll_deltas(sint32 time,sint32 &deltaX,sint32 &deltaY)
{
	
	bool retval = true;

	
	float	real_time=(float)time/(1000.0f);

		
	float t=k_SMOOTH_START_TIME;

	
	real_time*=real_time;

	float velocity=real_time*0.5f*k_SMOOTH_PIX_SEC_PER_SEC;

	
   	if (velocity<k_SMOOTH_MIN_VELOCITY)
		velocity=k_SMOOTH_MIN_VELOCITY;

	
	deltaX *= (sint32)velocity;
	deltaY *= (sint32)velocity;

	
	sint32 signx=(deltaX<0?-1:1);
	sint32 signy=(deltaY<0?-1:1);	

	sint32 w=k_TILE_PIXEL_WIDTH;			
	sint32 h=k_TILE_PIXEL_HEIGHT;			

	
	
	if (abs(deltaX)>k_SMOOTH_MAX_VELOCITY)
	{
		deltaX 	= k_SMOOTH_MAX_VELOCITY*signx;
		retval	= false;
	}
	
	if (abs(deltaY)>k_SMOOTH_MAX_VELOCITY)
	{
		deltaY	= k_SMOOTH_MAX_VELOCITY*signy;
		retval	= false;
	}

	
  	deltaX &= 0xFFFFFFFE;
  	deltaY &= 0xFFFFFFFE;

	return retval;
}


BOOL ui_CheckForScroll(void)
{
	if(!g_tiledMap) return FALSE;

	BOOL scrolled = FALSE;
	static BOOL isMouseScrolling = FALSE;

	
	const int k_MAX_SMOOTH_SCROLL = 64;
	const int k_TICKS_PER_ACCELERATION = 50;

	sint32		hscroll = g_tiledMap->GetZoomTilePixelWidth();
	sint32		vscroll = g_tiledMap->GetZoomTilePixelHeight()/2;

	static sint32 smoothX = 0;
	static sint32 smoothY = 0;

	
	sint32			tickDelta;

	
	if (g_controlPanel!=NULL)
		g_controlPanel->Idle();


	
	g_tiledMap->SetScrolling(false);

	s_scrolllasttick = s_scrollcurtick;
#ifdef USE_SDL
	s_scrollcurtick	 = SDL_GetTicks();
#else
	s_scrollcurtick	 = GetTickCount();
#endif

	tickDelta = s_scrollcurtick-s_scrolllasttick;
	
	if (!g_c3ui->TheMouse()) 
		return FALSE;

	sint32 x = g_c3ui->TheMouse()->X();
	sint32 y = g_c3ui->TheMouse()->Y();

	sint32	deltaX = 0, 
			deltaY = 0;

	static sint32	lastdeltaX = 0; 
	static sint32	lastdeltaY = 0;

	CURSORINDEX		scrollCursor = CURSORINDEX_DEFAULT;

	static BOOL scrolled_last_time = FALSE;

	static uint32 scroll_start;

	
	
	
	
	
	

	if (g_civApp->IsKeyboardScrolling() &&
		(!g_c3ui->TopWindow() || !g_c3ui->TopWindow()->GetFocusControl())) 
	{
		switch (g_civApp->GetKeyboardScrollingKey()) 
		{
			case AUI_KEYBOARD_KEY_UPARROW:
					deltaX = 0;
					deltaY = -1;
					scrolled = TRUE;
				break;
			case AUI_KEYBOARD_KEY_LEFTARROW:
					deltaX = -1;
					deltaY = 0;
					scrolled = TRUE;
				break;
			case AUI_KEYBOARD_KEY_RIGHTARROW:
					deltaX = 1;
					deltaY = 0;
					scrolled = TRUE;
				break;
			case AUI_KEYBOARD_KEY_DOWNARROW:
					deltaX = 0;
					deltaY = 1;
					scrolled = TRUE;
				break;
		}
		if (scrolled) 
		{
			aui_Window *topWindow = g_c3ui ? g_c3ui->TopWindow() : NULL;
			if(topWindow) {
				if(g_controlPanel) {
					if(topWindow != g_controlPanel->GetWindow() && topWindow != g_statusWindow && topWindow != g_radarWindow && topWindow != g_controlPanel->GetMenuBar()) {
						return FALSE;
					}
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
			scrolled = TRUE;
			scrollCursor = CURSORINDEX_SCROLLLEFT;
			isMouseScrolling = TRUE;
		}
		else 
			if (x > (g_ScreenWidth-4)) 
			{
				deltaX = 1;
				deltaY = 0;
				scrolled = TRUE;
				scrollCursor = CURSORINDEX_SCROLLRIGHT;
			isMouseScrolling = TRUE;
			}

		if (y <= 2) 
		{
			deltaX = 0;
			deltaY = -1;
			scrolled = TRUE;
			scrollCursor = CURSORINDEX_SCROLLUP;
			isMouseScrolling = TRUE;
		}
		else 
			if (y >= (g_ScreenHeight-2)) 
			{
				deltaX = 0;
				deltaY = 1;
				scrolled = TRUE;
				scrollCursor = CURSORINDEX_SCROLLDOWN;
			isMouseScrolling = TRUE;
			}


		
		
		
		
		
		

		if(scrolled) {
			if (isMouseScrolling)
				g_cursorManager->SetCursor(scrollCursor);

			if(!scrolled_last_time) {
#ifdef USE_SDL
				scroll_start = SDL_GetTicks();
#else
				scroll_start = GetTickCount();			
#endif
			}

#ifdef USE_SDL
			if(SDL_GetTicks() < scroll_start + g_theProfileDB->GetScrollDelay() * 100) {
#else
			if(GetTickCount() < scroll_start + g_theProfileDB->GetScrollDelay() * 100) {
#endif
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


		
		smoothX = deltaX*accel;
		smoothY = deltaY*accel;

		
		if (smoothX > hscroll)
			smoothX = hscroll;
		if (smoothY > vscroll)
			smoothY = vscroll;
		if (smoothX < -hscroll)
			smoothX = -hscroll;
		if (smoothY < -vscroll)
			smoothY = -vscroll;

		
		if (g_smoothScroll)
			g_tiledMap->ScrollMapSmooth(smoothX, smoothY);
		else
	  		g_tiledMap->ScrollMap(deltaX, deltaY);

	}
	else {
		s_scrolltime=k_SMOOTH_START_TIME;
		if (isMouseScrolling) {
			g_cursorManager->SetCursor(CURSORINDEX_DEFAULT);
		}

		isMouseScrolling = FALSE;

		
		smoothX = smoothY = 0;
	}

	scrolled_last_time = scrolled;
	return scrolled;
}

int ui_Process(void)
{
	if ( g_c3ui->TheMouse()->IsSuspended() ) return 0;

	
	
#ifdef USE_SDL	
	uint32 curTicks = SDL_GetTicks();
#else
	uint32			curTicks = GetTickCount();
#endif
	static uint32	lastTicks = curTicks;

	
	if (ui_CheckForScroll()) {
		do {
			
			
			g_tiledMap->CopyMixDirtyRects(g_background->GetDirtyList());

			
			
			
			
			
			g_c3ui->Draw();
		} while (ui_CheckForScroll());

		
		g_tiledMap->RetargetTileSurface(NULL);
		g_tiledMap->Refresh();
		g_tiledMap->InvalidateMap();

		
		lastTicks = curTicks;
	} else if (curTicks - lastTicks > k_TICKS_PER_GENERIC_FRAME) {
		
		
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
	return errcode;

	Assert( g_sharedSurface == NULL );
	if ( !g_sharedSurface )
	{
#if defined(__AUI_USE_DIRECTX__) || defined(__AUI_USE_SDL__)
		g_sharedSurface = aui_Factory::new_Surface(
			errcode,
			k_SHARED_SURFACE_WIDTH,
			k_SHARED_SURFACE_HEIGHT,
			k_SHARED_SURFACE_BPP);
#else
		g_sharedSurface = new aui_Surface(
			&errcode,
			k_SHARED_SURFACE_WIDTH,
			k_SHARED_SURFACE_HEIGHT,
			k_SHARED_SURFACE_BPP );
#endif 
		Assert( AUI_NEWOK(g_sharedSurface,errcode) );
		if ( !AUI_NEWOK(g_sharedSurface,errcode) ) return errcode;
	}

	return errcode;
}

sint32 sharedsurface_Cleanup( void )
{
	Assert( !g_sharedSurface );
	return TRUE;

	if ( !g_sharedSurface ) return FALSE;

	delete g_sharedSurface;
	g_sharedSurface = NULL;

	return TRUE;
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

int sprite_Cleanup(void)
{
	spritegrouplist_Cleanup();

	if (g_director) {
		delete g_director;
		g_director = NULL;
	}

	if (g_screenManager) {
		delete g_screenManager;
		g_screenManager = NULL;
	}

	return 0;
}


void ZoomPad_ZoomCallback()
{
	
	MainControlPanel::UpdateZoom();
}

int tile_Initialize(BOOL isRestoring)
{
 
	MapPoint		*size = g_theWorld->GetSize();

	g_tiledMap = new TiledMap(*size);

	
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

int tile_Cleanup(void)
{
    

    if (g_tiledMap) 
        delete g_tiledMap; 

	g_tiledMap = NULL;

	return 0;
}

int radar_Initialize(void)
{
	g_radarMap->Setup();

	return 0;
}

int main_RestoreGame(const MBCHAR *filename)
{

	g_civApp->LoadSavedGame((MBCHAR *)filename);

return 0;

    char filepath[_MAX_PATH]={0}; 
    FILE *fin=NULL; 
    g_civPaths->GetSavePath(C3SAVEDIR_GAME, filepath) ;				
	strcat(filepath, filename) ;									
    fin = fopen(filepath, "r"); 
    if (fin == NULL) { 
        c3errors_ErrorDialog("Load save game", "Could not open %s", filename);
        return 0; 
    } 
    fclose (fin); 

	
	g_network.Cleanup() ;											

	radarwindow_Cleanup();
	tile_Cleanup() ;
	gameinit_Cleanup() ;											

	messagewin_PurgeMessages();
	workwin_Cleanup();
	sprite_Cleanup() ;
	
	
	

	sprite_Initialize() ;
	GameFile::RestoreGame(filename) ;
	tile_Initialize(TRUE) ;
	radar_Initialize() ;

	
	g_tiledMap->InvalidateMap();

	return (0) ;
}


int main_Restart()
{
	g_civApp->RestartGame();

return 0;

	


	
	g_network.Cleanup();
	
	

	radarwindow_Cleanup();

	
	tile_Cleanup();
	
	
	gameinit_Cleanup();
	
	

	messagewin_PurgeMessages();
	workwin_Cleanup();
	
	
	sprite_Cleanup();
	
	



	
	sprite_Initialize();

	
	
	gameinit_Initialize(-1, -1, (*(CivArchive *)(NULL)));

	
	tile_Initialize(FALSE);


	
	


    
    roboinit_Initalize(*(CivArchive *)(NULL)); 

	
	radar_Initialize();


	g_tiledMap->InvalidateMap();

	return 0;
}

static HWND s_taskBar;

void main_HideTaskBar(void)
{
#ifndef __AUI_USE_SDL__
	s_taskBar = FindWindow("Shell_TrayWnd", NULL);

	if (g_hideTaskBar)
		ShowWindow(s_taskBar, SW_HIDE);
#endif // !__AUI_USE_SDL__
}

void main_RestoreTaskBar(void)
{
#ifndef __AUI_USE_SDL__
	ShowWindow(s_taskBar,SW_SHOWDEFAULT);
#endif // !__AUI_USE_SDL__
}

void ui_CivAppProcess(void)
{
	if (g_civApp)
		g_civApp->Process();
}


void AtExitProc(void)
{
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



#ifdef __GNUC__
void ParseCommandLine(int argc, char **argv)
#else
void ParseCommandLine(MBCHAR *szCmdLine)
#endif
{
#ifdef WIN32
#ifndef _BFR_
	if(stricmp(szCmdLine, "crash.txt") == 0) {
		FILE *txt = fopen("crashmap.txt", "w");
		fprintf(txt, "%s\n", c3debug_ExceptionStackTraceFromFile(fopen("crash.txt", "r")));
		fclose(txt);
		exit(0);
	}
#endif// _BFR_
#else // WIN32
	MBCHAR szCmdLine[k_MAX_CMD_LINE] = { '\0' };
	MBCHAR tmp[k_MAX_CMD_LINE] = { '\0' };
	for (int i=1; i<argc; i++) {
		strncpy(tmp, szCmdLine, k_MAX_CMD_LINE-1);
		snprintf(szCmdLine, k_MAX_CMD_LINE-1, "%s %s", tmp, argv[i]);
	}
	szCmdLine[k_MAX_CMD_LINE-1]='\0';
#endif // WIN32

    char *archive_file = NULL;
    archive_file = strstr(szCmdLine, "-l");
    if (NULL == archive_file) {
        g_cmdline_load = FALSE; 
    } else { 
        g_cmdline_load = TRUE; 
		g_useIntroMovie = FALSE;
		g_no_shell = TRUE;

        archive_file += 2; 

		BOOL gotQuote = FALSE;
		if(*archive_file == '"') {
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

        if (160 <= i) { 
            g_cmdline_load = FALSE;         
        } else { 
            g_cmdline_load_filename[i] = 0; 
        } 
    } 

	
    MBCHAR		*scenName = NULL; 

	scenName = strstr(szCmdLine, "-s");
    if (NULL != scenName) { 
		

		
        scenName += 2;

		
        sint32		i;

        for (i=0; i<k_SCENARIO_NAME_MAX && *scenName != '\0'; i++, scenName++) {
           g_scenarioName[i] = *scenName;
        } 

		
        if (i >= k_SCENARIO_NAME_MAX) { 
            g_launchScenario = FALSE;
			memset(g_scenarioName, '\0', k_SCENARIO_NAME_MAX);
        } else { 
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
    if (NULL != strstr(szCmdLine, "age1")) { 
        g_cheat_age = 1; 
    } else if (NULL != strstr(szCmdLine, "age2")) { 
        g_cheat_age = 2; 
    } else if (NULL != strstr(szCmdLine, "age3")) { 
        g_cheat_age = 3; 
    } else if (NULL != strstr(szCmdLine, "age4")) { 
        g_cheat_age = 4; 
    } else if (NULL != strstr(szCmdLine, "age5")) { 
        g_cheat_age = 5; 
    }
#endif // _DEBUG

#ifdef WIN32
	if (g_noAssertDialogs) {
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
	}
#endif // WIN32
} 

#ifdef WIN32
int WINAPI main_filehelper_GetOS(void)
{
	OSVERSIONINFO osvi;
	
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	GetVersionEx( &osvi );

	return osvi.dwPlatformId;
}
#endif

/// \brief Returns the amount of free memory available [kB].
///
/// This function determines the amount of physical and virtual memory 
/// available (in kBytes) and returns the sum of them.
DWORD main_GetRemainingSwapSpace(void)
{
#ifdef WIN32
	DWORD dwRet = 0;
	MEMORYSTATUS ms;
	GlobalMemoryStatus( &ms );
	
	dwRet = ms.dwAvailPageFile;

	if( main_filehelper_GetOS() != VER_PLATFORM_WIN32_NT )
		dwRet += ms.dwAvailPhys;
	
	return dwRet;
#elif defined(LINUX)
	FILE *meminfo = fopen("/proc/meminfo", "r");
	char fbuf[1024] = { 0 };
	char whatbuf[1024] = { 0 };
	unsigned long size = 0;
	unsigned long totalsize = 0;
	char units = 'k';
	
	if (!meminfo)
		return 0;
	
	while (fgets(fbuf, sizeof(fbuf) - 1, meminfo)) {
		sscanf(fbuf, "%s: %lu %cB", whatbuf, &size, &units);
		// TODO: Perhaps conversion of k/M later on
		switch (units) {
		case 'M': size *= 1024UL;
		          break;
		}
		
		if (0 == strncasecmp("MemFree", whatbuf, 7)) {
			totalsize += size;
		} else if (0 == strncasecmp("Buffers", whatbuf, 7)) {
			totalsize += size;
		} else if (0 == strncasecmp("Cached", whatbuf, 6)) {
			totalsize += size;
		} else if (0 == strncasecmp("SwapFree", whatbuf, 8)) {
			totalsize += size;
		}
	}
	
	fclose(meminfo);
	return totalsize;
#endif
// Compiler error on other systems due to missing return.
}


BOOL main_VerifyRAMToRun(void)
{
	DWORD space = main_GetRemainingSwapSpace();

	DWORD neededSpace = 60 * 1024 * 1024;

	if (space < neededSpace) {
	
		c3errors_FatalDialog(appstrings_GetString(APPSTR_MEMORY),
								appstrings_GetString(APPSTR_NEEDMOREMEMORY));
		return FALSE;
	}

	return TRUE;
}

void main_OutputCrashInfo(uint32 eip, uint32 ebp, uint32 *outguid)
{
// we get a core file on linux, and there are POSIX 1003.1-2003 
// incompabilities within the code
#ifdef WIN32
	uint32	crawl[100];
	uint32	pos = 0;

	crawl[pos] = eip;
	pos++;
	
	uint32	caller = 0;
	uint32	basePtr = ebp;

	BOOL done = FALSE;
#if !defined(__GNUC__)
	while(!done) {
		__try {
			caller = *((unsigned *) (basePtr + 4));
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			done = 1;
		}
		
		if (caller >= 0x80000000)
		{
			done = 1;
		}
		
		crawl[pos] = caller;
		pos++;

		
		basePtr = * ((unsigned *) basePtr);  
	}
#endif

	uint32 guid = rand() * rand() + rand();

	*outguid = guid;

	
	MBCHAR fName[MAX_PATH];
	sprintf(fName, "CRASH-%X.DAT", guid);
	FILE *outFile = fopen(fName, "wb");

	if (outFile) {
#ifndef USE_SDL
		srand(GetTickCount());
#endif

		fwrite((void *)&guid, 4, 1, outFile);
		fwrite((void *)&pos, 4, 1, outFile);
		fwrite((void *)crawl, 4, pos, outFile);

		
		
		
		char			filepath[MAX_PATH];
		sint32			filepathlen;


		
		char		timebuf[100];
		time_t		ltime;
		struct tm	*now;
		time(&ltime);
		now = localtime(&ltime);
		strftime(timebuf, 100, "> Log started at %I:%M%p %m/%d/%Y", now);
		fprintf(outFile, "%s\n", timebuf);

		
		
		filepathlen = GetModuleFileName ( NULL, filepath, sizeof(filepath));
		
		fprintf(outFile, "> Executable :%s\n", filepath);

		FILETIME					creationTime, crTime, laTime, lwTime;
		SYSTEMTIME					systemTime;
		HANDLE						fileHandle;

		fileHandle = CreateFile(filepath, GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								OPEN_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
		if (fileHandle != INVALID_HANDLE_VALUE) {
			if (GetFileTime( fileHandle, &crTime, &laTime, &lwTime)) {
				if (FileTimeToLocalFileTime( &lwTime, &creationTime)) {
					if (FileTimeToSystemTime(&creationTime, &systemTime)) {
						fprintf(outFile, "> Last Written: %#.2d/%#.2d/%d %#.2d:%#.2d:%#.2d.%#.3d\n",
								systemTime.wMonth, systemTime.wDay, systemTime.wYear,
								systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
					}
				}
			}

			CloseHandle(fileHandle);
		}

		
		OSVERSIONINFO		osv;
		osv.dwOSVersionInfoSize = sizeof(osv);

		GetVersionEx(&osv);

		fprintf(outFile, "> Windows Version: %d.%d %s build %d, %s\n", osv.dwMajorVersion, osv.dwMinorVersion,
																(osv.dwPlatformId == VER_PLATFORM_WIN32_NT) ? "NT" : "",
																osv.dwBuildNumber,
																osv.szCSDVersion);
		
		uint32 cNameSize = MAX_COMPUTERNAME_LENGTH+1;
		MBCHAR cName[MAX_COMPUTERNAME_LENGTH+1];

		if (GetComputerName((LPTSTR)cName, &cNameSize))
			fprintf(outFile, "> Computer Name: %s\n", cName);
		
		cNameSize = MAX_COMPUTERNAME_LENGTH+1;
		if (GetUserName((LPTSTR)cName, &cNameSize))
			fprintf(outFile, ">     User Name: %s\n", cName);

		

		MEMORYSTATUS		ms;

		ms.dwLength = sizeof(ms);
		GlobalMemoryStatus(&ms);

		fprintf(outFile, ">      Physical RAM: %u bytes\n", ms.dwTotalPhys);
		fprintf(outFile, ">     Available RAM: %u bytes\n", ms.dwAvailPhys);
		fprintf(outFile, ">    Page File Size: %u bytes\n", ms.dwTotalPageFile);
		fprintf(outFile, ">    Page File Free: %u bytes\n", ms.dwAvailPageFile);
		fprintf(outFile, ">          Total VM: %u bytes\n", ms.dwTotalVirtual);
		fprintf(outFile, ">      Available VM: %u bytes\n", ms.dwAvailVirtual);
		fprintf(outFile, ">       Memory load: %u%% used\n", ms.dwMemoryLoad);

		
		fprintf(outFile, ">  Direct X Version: 0x%x\n", g_dxver);
		fprintf(outFile, ">    Cur ScreenSize: %d x %d\n", g_ScreenWidth, g_ScreenHeight);

		fclose(outFile);
	}
#endif
}

static uint32 s_guid = 0;

#ifdef WIN32
#ifndef _DEBUG
char * c3debug_ExceptionStackTrace(LPEXCEPTION_POINTERS exception);
#endif // !_DEBUG
#endif // WIN32

#ifdef _BFR_
extern sint32 g_logCrashes;
#endif

#ifdef WIN32
static LONG _cdecl main_CivExceptionHandler(LPEXCEPTION_POINTERS pException)
{
#if defined(_DEBUG)

	MBCHAR * s;
	
	switch (pException->ExceptionRecord->ExceptionCode) 
    {
	case EXCEPTION_ACCESS_VIOLATION:		s = "Access Violation";		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:	s = "Array Bounds Exceeded"; break;
	case EXCEPTION_BREAKPOINT:				s = "Breakpoint"; break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:	s = "Datatype Misalignment"; break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:	s = "Floating Point Denormal Operand"; break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:		s = "Floating Point Divide by Zero"; break;
	case EXCEPTION_FLT_INEXACT_RESULT:		s = "Floating Point Inexact Result"; break;
	case EXCEPTION_FLT_INVALID_OPERATION:	s = "Floating Point Invalid Operation"; break;
	case EXCEPTION_FLT_OVERFLOW:			s = "Floating Point Overflow"; break;
	case EXCEPTION_FLT_STACK_CHECK:			s = "Floating Point Stack Check"; break;
	case EXCEPTION_FLT_UNDERFLOW:			s = "Floating Point Underflow"; break;
	case EXCEPTION_GUARD_PAGE:				s = "Guard Page"; break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:		s = "Illegal Instruction"; break;
	case EXCEPTION_IN_PAGE_ERROR:			s = "In-page Error"; break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:		s = "Integer Divide By Zero"; break;
	case EXCEPTION_INT_OVERFLOW:			s = "Integer Overflow"; break;
	case EXCEPTION_INVALID_DISPOSITION:		s = "Invalid Disposition"; break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:s = "Non-Continuable Exception"; break;
	case EXCEPTION_PRIV_INSTRUCTION:		s = "Privileged Instruction"; break;
	case EXCEPTION_SINGLE_STEP:				s = "Single Step"; break;
	case EXCEPTION_STACK_OVERFLOW:			s = "Stack Overflow"; break;
	default:                        		s = "Unknown"; break;
	}

	DPRINTF(k_DBG_FIX, ("Exception: '%s' thrown.\n", s));
	s = c3debug_ExceptionStackTrace(pException);
	DPRINTF(k_DBG_FIX, ("Exception Stack Trace:\n%s\n", s));

	return EXCEPTION_CONTINUE_SEARCH;

#else // _DEBUG

#ifdef _BFR_
    if (g_logCrashes) 
#endif // _BFR_
	{
		FILE *crashLog = fopen("logs" FILE_SEP "crash.txt", "w");
		if(!crashLog)
			crashLog = fopen("crash.txt", "w");

		if(crashLog) {
			fprintf(crashLog, "%s\n", c3debug_ExceptionStackTrace(pException));
		}
		fclose(crashLog);
	}
				
	return EXCEPTION_EXECUTE_HANDLER;

#endif // _DEBUG
}
#endif // WIN32

#ifdef __AUI_USE_DIRECTX__
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
			return AUI_ERRCODE_HACK;
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
#endif // WIN32

#define k_MEMORY_THRESHHOLD_1		(10 * 1024 * 1024)
#define k_MEMORY_THRESHHOLD_2		(1 * 1024 * 1024)

static BOOL s_reportedThreshhold1 = FALSE;
static BOOL s_reportedThreshhold2 = FALSE;

void main_CheckMemory(void)
{
	DWORD memory = main_GetRemainingSwapSpace();

	
	if (memory < k_MEMORY_THRESHHOLD_1) {
		
		if (!s_reportedThreshhold1) {
			

			c3errors_ErrorDialog(appstrings_GetString(APPSTR_MEMORY),
									appstrings_GetString(APPSTR_LOWMEMORY1));

			s_reportedThreshhold1 = TRUE;
		} else {
			
			if (memory < k_MEMORY_THRESHHOLD_2) {
				
				if (!s_reportedThreshhold2) {
					
					c3errors_ErrorDialog(appstrings_GetString(APPSTR_MEMORY),
											appstrings_GetString(APPSTR_LOWMEMORY2));

					s_reportedThreshhold2 = TRUE;
				}
			}
		}
	}
}


void main_InitializeLogs(void)
{
	char		timebuf[100];
	time_t		ltime;
	struct tm	*now;

	
	time(&ltime);
	now = localtime(&ltime);

#ifdef USE_SDL
    g_splash_old = SDL_GetTicks();
#else
    g_splash_old = GetTickCount();
#endif

	strftime(timebuf, 100, "Log started at %I:%M%p %m/%d/%Y", now);

#ifdef WIN32
#if defined(_DEBUG) || defined(USE_LOGGING)
	c3debug_InitDebugLog();
	c3debug_SetDebugMask(k_DBG_FIX | k_DBG_DATABASE | k_DBG_NET | k_DBG_GAMESTATE | k_DBG_UI | k_DBG_SLIC, 1);
#endif

	MBCHAR filepath[_MAX_PATH];
	sint32 filepathlen; 

	filepathlen = GetModuleFileName ( NULL, filepath, sizeof(filepath));
	
	DPRINTF(k_DBG_FIX, ("** BUILD EXE :%s\n", filepath));

	FILETIME					creationTime, crTime, laTime, lwTime;
	SYSTEMTIME					systemTime;
	HANDLE						fileHandle;

	fileHandle = CreateFile(filepath, GENERIC_READ,
							FILE_SHARE_READ,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL);
	if (fileHandle != INVALID_HANDLE_VALUE) {
		if (GetFileTime( fileHandle, &crTime, &laTime, &lwTime)) {
			DPRINTF(k_DBG_FIX, ("** BUILD INFO:\n"));
			if (FileTimeToLocalFileTime( &crTime, &creationTime)) {
				if (FileTimeToSystemTime(&creationTime, &systemTime)) {
					DPRINTF(k_DBG_FIX, ("**     Creation Time: %#.2d/%#.2d/%d %#.2d:%#.2d:%#.2d.%#.3d\n",
							systemTime.wMonth, systemTime.wDay, systemTime.wYear,
							systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds));
				}
			}
			if (FileTimeToLocalFileTime( &lwTime, &creationTime)) {
				if (FileTimeToSystemTime(&creationTime, &systemTime)) {
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
	
	uint32 cNameSize = MAX_COMPUTERNAME_LENGTH+1;
	MBCHAR cName[MAX_COMPUTERNAME_LENGTH+1];

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

#ifdef _DEBUGTOOLS
	
	Debug_Open();
#endif
#endif // !WIN32
}




#if defined(__GNUC__)

int main(int argc, char **argv)
{
    atexit(AtExitProc);
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
	SetThreadName("WinMain");
#endif	// _DEBUG

    // Make sure old versions of DDHELP.EXE won't keep files open
    HINSTANCE handle = LoadLibrary("DDRAW.DLL");
    if (0 != handle) {
        FreeLibrary(handle);
        handle = 0;
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
	MBCHAR		*message;
	sint32		filesize = 0;

	FILE *f = fopen("disclaimer.txt", "rb");

	if (!f)
		goto Error;

	if (fseek(f, 0, SEEK_END) == 0) {
		filesize = ftell(f);
	} else {
		goto Error;
	}
 	
	fclose(f);

	message = new MBCHAR[filesize+1];
	memset(message, 0, filesize+1);

	f = fopen("disclaimer.txt", "rb");
	if (!f) 
		goto Error;

	c3files_fread( message, 1, filesize, f );

	fclose(f);

	MessageBox(NULL, message, "Call to Power", MB_OK | MB_ICONEXCLAMATION);

	return;

Error:
	c3errors_FatalDialog(appstrings_GetString(APPSTR_INITIALIZE),
							appstrings_GetString(APPSTR_CANTFINDFILE));

}

#ifdef __AUI_USE_SDL__
int SDLMessageHandler(const SDL_Event &event);
#endif

#ifdef __GNUC__
int CivMain(int argc, char **argv)
#else
int WINAPI CivMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
#endif
{
#ifdef WIN32
	MSG			msg;

	HWND hwnd = FindWindow (gszMainWindowClass, gszMainWindowName);
	if (hwnd) {
		
		if (IsIconic(hwnd)) {
			ShowWindow(hwnd, SW_RESTORE);
		}
		SetForegroundWindow (hwnd);
		
		return FALSE;
	}
	
	char exepath[_MAX_PATH];
	char launchcommand[_MAX_PATH];
	if(GetModuleFileName(NULL, exepath, _MAX_PATH) != 0) {
				
			ctpregistry_SetKeyValue(HKEY_CLASSES_ROOT,
									".c2g",
									NULL,
									"c2gfile");
			ctpregistry_SetKeyValue(HKEY_CLASSES_ROOT,
									"c2gfile",
									NULL, 
									"Call to Power 2 saved game");
			strcpy(launchcommand, exepath);
			strcat(launchcommand, " -l\"%1\"");
			ctpregistry_SetKeyValue(HKEY_CLASSES_ROOT,
									"c2gfile\\Shell\\Open\\command",
									NULL,
									launchcommand);
		char *lastbackslash = strrchr(exepath, FILE_SEPC);
		if(lastbackslash) {
			*lastbackslash = 0;
			chdir(exepath);
		}
	}
#endif // __WIN32__

	
	g_e3Demo = false;

	
	appstrings_Initialize();
	
	setlocale(LC_COLLATE, appstrings_GetString(APPSTR_LOCALE));

#ifdef __AUI_USE_DIRECTX__
	if (!main_CheckDirectX()) {
		c3errors_FatalDialog(appstrings_GetString(APPSTR_DIRECTX),
							appstrings_GetString(APPSTR_NEEDDIRECTX));
	}
#endif

	
#ifdef _DEBUG
	main_InitializeLogs();
#endif
#if !defined(_DEBUG) && !defined(_BFR_)
#ifdef WIN32
	SECURITY_ATTRIBUTES		sa;

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	
	CreateDirectory((LPCTSTR)"logs", &sa);
#else // WIN32
	mode_t mode = 0777;
	mkdir("logs", mode);
#endif // WIN32
#endif // !defined(_DEBUG) && !defined(_BFR_)

#ifdef __GNUC__
	ParseCommandLine(argc, argv);
#else
    ParseCommandLine(szCmdLine);
#endif

	if(g_e3Demo) {
		if(!g_no_shell && !g_launchScenario) {
			g_no_shell = TRUE;
		}
	}

	g_civApp = new CivApp();

#ifndef WIN32
// FIXME: Remove unneeded arguments.
	HINSTANCE hInstance = NULL;
	int iCmdShow = 0;
#endif
	
    if (g_cmdline_load) {
        g_civApp->InitializeApp(hInstance, iCmdShow);

		ScenarioPack	*pack;
		Scenario		*scen;

		if (g_civScenarios->FindScenarioFromSaveFile(g_cmdline_load_filename, &pack, &scen)) {
			g_civPaths->SetCurScenarioPath(scen->m_path);
			g_civPaths->SetCurScenarioPackPath(pack->m_path);
			
			g_theProfileDB->SetIsScenario(TRUE);

			
			if (g_civScenarios->ScenarioHasSavedGame(scen)) {
				spnewgamescreen_scenarioExitCallback(NULL, 0, 0, NULL);
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
				spnewgamescreen_scenarioExitCallback(NULL, 0, 0, NULL);
			} else {
				spnewgamescreen_displayMyWindow();
			}
		}
	} else {
		g_civApp->InitializeApp(hInstance, iCmdShow);
	}

	
	for (gDone = FALSE; !gDone; )
	{
		g_civApp->Process();

#ifdef __AUI_USE_SDL__
		SDL_Event event;
		while (1)
#else
		while (PeekMessage(&msg, gHwnd, 0, 0, PM_REMOVE) && !g_letUIProcess)
#endif
		{
#ifdef __AUI_USE_SDL__
			int n = SDL_PeepEvents(&event, 1, SDL_GETEVENT,
			                       ~SDL_MOUSEEVENTMASK);
			if (0 > n) {
				fprintf(stderr, "[CivMain] PeepEvents failed: %s\n", SDL_GetError());
				break;
			}
			if (0 == n) {
				// other events are handled in other threads
				// or no more events
				break;
			}
			if (SDL_QUIT == event.type)
#else // __AUI_USE_SDL__
			if (WM_QUIT == msg.message)
#endif // __AUI_USE_SDL__
			{
				gDone = TRUE;
			}
#ifndef __AUI_USE_SDL__
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
#else // !__AUI_USE_SDL__
			SDLMessageHandler(event);
#endif // !__AUI_USE_SDL__
		}

		g_letUIProcess = FALSE;
	}

#ifdef WIN32
	return msg.wParam;
#else
	return 0;
#endif
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

    	delete g_civApp;
        g_civApp = NULL;
    }

    sliccmd_clear_symbols();

#ifdef _DEBUGTOOLS
	Debug_Close();
#endif

	appstrings_Cleanup();
}

#else   // _DEBUG

void DoFinalCleanup(int exitCode)
{
    static bool s_cleaningUpTheApp  = false;

    if (g_c3ui)
    {
		g_c3ui->DestroyNativeScreen();
    }

#ifdef WIN32
	ShowWindow(gHwnd, SW_HIDE);
#endif

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

#ifndef __AUI_USE_SDL__
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
#else
int SDLMessageHandler(const SDL_Event &event)
#endif
{
	AUI_ERRCODE		errcode;
	
	if ( gDone )
		return 0;

#ifndef __AUI_USE_SDL__
	if (g_c3ui != NULL) {
		errcode = g_c3ui->HandleWindowsMessage(hwnd, iMsg, wParam, lParam);

	}
#endif

	static int swallowNextChar = 0;

#ifndef __AUI_USE_SDL__
	switch (iMsg) {
	case WM_CHAR :
		if(!swallowNextChar)
			ui_HandleKeypress(wParam, lParam);
		swallowNextChar = FALSE;
		break;
	case WM_KEYDOWN :
#else
	switch(event.type) {
	case SDL_KEYDOWN:
		// TODO: Add Event handling here
		break;
#endif
#ifndef __AUI_USE_SDL__
		switch (wParam) {
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
			swallowNextChar = TRUE;
			return 0;
		case VK_RETURN:
			ui_HandleKeypress('\r' + 128, lParam);
			swallowNextChar = TRUE;
			return 0;
		case VK_BACK:
			ui_HandleKeypress(8 + 128, lParam);
			swallowNextChar = TRUE;
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
#else
	case SDL_QUIT:
#endif
#ifndef __AUI_USE_SDL__
		if ( hwnd != gHwnd ) break;
#endif		
		
		gDone = TRUE;

		
		DoFinalCleanup();

#ifndef __AUI_USE_SDL__
		DestroyWindow( hwnd );
		gHwnd = NULL;
#endif

		return 0;
#ifndef __AUI_USE_SDL__
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
		if (scrollCode == SB_LINEDOWN) {
			ui_HandleMouseWheel((sint16)-1);
		}
		else
			if (scrollCode == SB_LINEUP) {
				ui_HandleMouseWheel((sint16)1);
			}
		}
 		break;
	case WM_MOUSEWHEEL:
		ui_HandleMouseWheel((sint16)HIWORD(wParam));
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
#else
	}
	
	return 0;
#endif
}

#ifdef _DEBUG
#include "DataCheck.h"
extern DataCheck *g_dataCheck; 
#endif



void DisplayFrame (aui_Surface *surf)
{ 
    static double fr_decay = 0.85; 
    g_ave_frame_rate = 10.0; 
    g_ave_frame_time = 200.0; 
    static sint32 is_init = 1; 
    static sint32 g_old_last_tick; 
    sint32 new_tick; 
    char str[80]; 

    if (is_init) { 
        g_old_last_tick; 
        is_init = 0; 
    }

#ifdef USE_SDL
    new_tick = SDL_GetTicks(); 
#else
    new_tick = GetTickCount(); 
#endif

    double d = double (new_tick - g_old_last_tick); 
    g_old_last_tick = new_tick;
    if (d < 1) 
        return; 

    g_ave_frame_time = fr_decay * g_ave_frame_time + (1-fr_decay) * (d);
    g_ave_frame_rate = fr_decay * g_ave_frame_rate + (1-fr_decay) * (1000.0/d); 

    sprintf (str, "ave frame rate %4.2f/sec - ave frame time %5.1fms", g_ave_frame_rate, g_ave_frame_time); 
    primitives_DrawText(surf, 100, 100, (MBCHAR *)str, 1, 0);

}




BOOL ExitGame(void)
{
#if defined(__AUI_USE_SDL__)
	static SDL_Event quit = { 0 };
	quit.type = SDL_QUIT;
	quit.quit.type = SDL_QUIT;
	int e = SDL_PushEvent(&quit);
	return (e != 0);
#elif defined(WIN32)
	return PostMessage(gHwnd, WM_CLOSE, 0, 0);
#else
	return TRUE;
#endif
}


