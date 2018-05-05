#include "ctp/c3.h"

#include "ui/aui_common/aui.h"
#include "ui/aui_common/aui_uniqueid.h"
#include "ui/aui_ctp2/c3ui.h"
#include "ui/aui_common/aui_ldl.h"
#include "ui/aui_common/aui_static.h"
#include "ui/aui_common/aui_stringtable.h"

#include "gfx/gfx_utils/pixelutils.h"

#include "ui/aui_ctp2/c3_static.h"
#include "ui/aui_ctp2/c3_coloredswitch.h"

#include "ui/aui_ctp2/statuswindow.h"
#include "ui/interface/controlpanelwindow.h"

#include "ui/interface/workwin.h"
#include "ui/interface/debugwindow.h"

#include "ui/aui_ctp2/textbutton.h"
#include "ui/aui_ctp2/iconbutton.h"
#include "ui/aui_ctp2/picturebutton.h"
#include "ui/aui_ctp2/coloriconbutton.h"

#include "ui/aui_ctp2/c3thumb.h"
#include "ui/aui_ctp2/c3slider.h"
#include "ui/aui_ctp2/c3scroller.h"
#include "ui/aui_ctp2/c3spinner.h"

#include "ui/aui_ctp2/checkbox.h"
#include "ui/aui_ctp2/textswitch.h"
#include "ui/aui_ctp2/coloriconswitch.h"

#include "ui/aui_ctp2/texttab.h"
#include "ui/aui_common/aui_tabgroup.h"

#include "ui/aui_common/aui_static.h"

#include "ui/aui_common/aui_item.h"
#include "ui/aui_ctp2/c3listbox.h"
#include "ui/aui_ctp2/textbox.h"

#include "ui/aui_ctp2/c3dropdown.h"

#include "ui/aui_ctp2/c3textfield.h"

#include "ui/aui_ctp2/c3listbox.h"
#include "ui/aui_ctp2/cityinventorylistbox.h"

#include "ui/aui_common/aui_progressbar.h"

#include "gfx/tilesys/tiledmap.h"
#include "ui/aui_ctp2/c3windows.h"
#include "ui/interface/workwindow.h"
#include "gs/fileio/CivPaths.h"
#include "gfx/gfx_utils/videoutils.h"

#include "toolbar.h"

#include "ui/aui_ctp2/controlsheet.h"
#include "gs/gameobj/TerrImproveData.h"
#include "gs/gameobj/TerrImprovePool.h"
#include "gs/world/MapPoint.h"
#include "gfx/tilesys/tiledmap.h"
#include "gs/gameobj/Player.h"
#include "ui/aui_ctp2/SelItem.h"
#include "InstDB.h"

#include "ui/aui_ctp2/bevellesswindow.h"
#include "ui/interface/radarwindow.h"

#include "ui/interface/UIUtils.h"

#include "ui/aui_ctp2/ctp2_button.h"

#include "ui/interface/citymanager.h"
#include "ui/interface/citywindow.h"
#include "ui/interface/screenutils.h"

#include "ui/aui_ctp2/c3_popupwindow.h"

#include "gs/utility/TurnCnt.h"


extern sint32		g_ScreenWidth;
extern sint32		g_ScreenHeight;
extern sint32		g_isCheatModeOn;

extern C3UI			*g_c3ui;

extern C3Window		*g_testWindow;
extern C3Window		*g_standardWindow;
extern C3Window		*g_floatingWindow;
extern DebugWindow	*g_debugWindow;
extern WorkWindow	*g_workWindow;

extern TerrainImprovementPool	*g_theTerrainImprovementPool;
extern Player					**g_player;
extern SelectedItem				*g_selected_item;
extern TiledMap					*g_tiledMap;
extern InstallationDatabase		*g_theInstallationDB;


extern ProductionTabControl		*g_cp_productionTab;
extern CityTabControl			*g_cp_cityTab;
extern UnitsTabControl			*g_cp_unitsTab;

ControlPanelWindow	*g_controlPanel = NULL;




static TextButton	*s_button;
static TextButton	*s_debugButton;
static TextButton	*s_resourceButton;

static TextButton	*cheatButton;

static ControlSheet	*s_tileMenuControl;
static ControlSheet	*s_landTileControl;
static ControlSheet	*s_seaTileControl;
static ControlSheet *s_spaceTileControl;
static ControlSheet *s_terraTileControl;

static ColorIconButton	*s_donkeys;

static ctp2_Button *s_zoomPlusButton = NULL;
static ctp2_Button *s_zoomMinusButton = NULL;
static ctp2_Button *s_cityManagerButton = NULL;
static ctp2_Button *s_tileImprovementButton = NULL;
static ctp2_Button *s_turnButton = NULL;

static c3_Static *s_pwBox;
static c3_Static *s_goldBox;
static c3_Static *s_yearBoxHolder;
static c3_ColoredSwitch *s_yearBox;

static c3_Static *s_populationLabel;
static c3_Static *s_populationBox;

static aui_ProgressBar	*s_progressBar;

static aui_StringTable	*s_yearString;

extern sint32		g_tileImprovementMode;

#define k_STATUS_WINDOW_HEIGHT		30
#define k_CONTROL_PANEL_WIDTH		709
#define k_CONTROL_PANEL_HEIGHT		150

class c3_NormalProgressBar : public aui_ProgressBar {
public:

	c3_NormalProgressBar(AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock) :
	aui_ProgressBar( retval, id, ldlBlock ),
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ) {}

	c3_NormalProgressBar(AUI_ERRCODE *retval, uint32 id, sint32 x, sint32 y, sint32 width, sint32 height) :
	aui_ProgressBar( retval, id, x, y, width, height ),
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ) {}

protected:
	c3_NormalProgressBar() : aui_ProgressBar() {}

	virtual AUI_ERRCODE CalculateIntervals( double *start, double *stop ) {

		double x = (double)m_curValue / (double)m_maxValue;

		*start = 0.0;
		*stop = x;

		return AUI_ERRCODE_OK;
	}
};

extern sint32 g_modalWindow;

void CityManagerButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

	CityWindow::Initialize();

	Unit city;
	if(g_selected_item->GetSelectedCity(city))
		CityWindow::Display(city.CD());
	else
		CityWindow::Display(NULL);

}

void TileImprovementButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

}

void ZoomPlusButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
    if (g_tiledMap) {
		g_tiledMap->ZoomIn();
	}
}

void ZoomMinusButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	if (g_tiledMap) {
		g_tiledMap->ZoomOut();
	}
}

void DonkeyCallback(aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	MBCHAR filename[_MAX_PATH];
    g_civPaths->FindFile(C3DIR_SOUNDS, "donkey.wav", filename);
    PlaySound (filename, NULL, SND_ASYNC | SND_FILENAME);
}

void TurnButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

	if(g_turn) {
		g_turn->NextRound();
	}
}

sint32 controlpanelwindow_Initialize()
{
	AUI_ERRCODE		errcode;
	MBCHAR			windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1];

	if (g_controlPanel) return 0;

	sint32 windowWidth = k_CONTROL_PANEL_WIDTH;
	sint32 windowHeight = k_CONTROL_PANEL_HEIGHT;
	sint32 windowX = 264;
	sint32 windowY = 618;

	sint32 controlSheetX = 91;
	sint32 controlSheetY = 1;

	strcpy(windowBlock, "ControlPanelWindow");

	g_controlPanel = new ControlPanelWindow(&errcode, k_ID_WINDOW_CONTROLPANEL, windowBlock, 16 );
	Assert( AUI_NEWOK(g_controlPanel, errcode) );
	if ( !AUI_NEWOK(g_controlPanel, errcode) ) return -1;





	g_controlPanel->Resize	(800, g_controlPanel->Height());
	g_controlPanel->Move	(g_controlPanel->X(), g_ScreenHeight - g_controlPanel->Height());








































































































































































	return 0;
}

sint32 controlpanelwindow_InitializeHats()
{
	AUI_ERRCODE		errcode;
	MBCHAR			windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	strcpy(windowBlock, "ControlPanelRightHat");

	sprintf( controlBlock, "%s.%s", windowBlock, "ZoomPlusButton" );
	s_zoomPlusButton = new ctp2_Button( &errcode, aui_UniqueId(), controlBlock, ZoomPlusButtonActionCallback );
	Assert( AUI_NEWOK(s_zoomPlusButton, errcode) );
	if ( !AUI_NEWOK(s_zoomPlusButton, errcode) ) return -3;

	sprintf( controlBlock, "%s.%s", windowBlock, "ZoomMinusButton" );
	s_zoomMinusButton = new ctp2_Button( &errcode, aui_UniqueId(), controlBlock, ZoomMinusButtonActionCallback );
	Assert( AUI_NEWOK(s_zoomMinusButton, errcode) );
	if ( !AUI_NEWOK(s_zoomMinusButton, errcode) ) return -3;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return -1;

	return 0;
}

sint32 controlpanelwindow_Cleanup( void )
{

	if (!g_controlPanel) return 0;




if (s_donkeys) {
	delete s_donkeys;
	s_donkeys = NULL;
}

	ControlPanelWindow_TileImp_Cleanup();
	ControlPanelWindow_Land_Cleanup();
	ControlPanelWindow_Sea_Cleanup();
	ControlPanelWindow_Space_Cleanup();
	ControlPanelWindow_Terra_Cleanup();

	cp_tileimp_trackerCleanup();

	specialattackwindow_Cleanup();

	delete s_tileMenuControl;
	s_tileMenuControl = NULL;
	delete s_landTileControl;
	s_landTileControl = NULL;
	delete s_seaTileControl;
	s_seaTileControl = NULL;
	delete s_spaceTileControl;
	s_spaceTileControl = NULL;
	delete s_terraTileControl;
	s_terraTileControl = NULL;

	controlpanelwindow_CleanupCitySelectedTabGroup();

	delete s_yearBox;
	s_yearBox = NULL;

	delete s_progressBar;
	s_progressBar = NULL;

	delete s_goldBox;
	s_goldBox = NULL;

	delete s_pwBox;
	s_pwBox = NULL;

	delete s_zoomMinusButton;
	s_zoomMinusButton = NULL;

	delete s_zoomPlusButton;
	s_zoomPlusButton = NULL;

	delete s_tileImprovementButton;
	s_tileImprovementButton = NULL;

	delete s_cityManagerButton;
	s_cityManagerButton = NULL;

	delete s_turnButton;
	s_turnButton = NULL;

	RemoveControl( s_populationLabel );
	RemoveControl( s_populationBox );
	RemoveControl( s_yearString );
	RemoveControl( s_yearBoxHolder );

	CityWindow::Cleanup();

	delete g_controlPanel;
	g_controlPanel = NULL;

	return 0;
}





void
HideElement(aui_Region *element)
{
	if (element!=NULL)
		element->Hide();
}




void
HideControlPanel()
{
	HideElement(g_testWindow            );
	HideElement(g_standardWindow        );
	HideElement(g_floatingWindow        );
	HideElement(g_debugWindow           );
	HideElement(g_workWindow            );

	HideElement(g_radarWindow           );


	HideElement((aui_Window*)g_controlPanel->GetWindow());
	HideElement(s_button                );
	HideElement(s_debugButton           );
	HideElement(s_resourceButton        );
	HideElement(cheatButton             );

	HideElement(s_tileMenuControl       );
	HideElement(s_landTileControl       );
	HideElement(s_seaTileControl        );
	HideElement(s_spaceTileControl      );
	HideElement(s_terraTileControl      );
	HideElement(s_donkeys               );
	HideElement(s_zoomPlusButton        );
	HideElement(s_zoomMinusButton       );
	HideElement(s_cityManagerButton     );
	HideElement(s_tileImprovementButton );
	HideElement(s_turnButton            );
	HideElement(s_pwBox                 );
	HideElement(s_goldBox               );
	HideElement(s_yearBoxHolder         );
	HideElement(s_yearBox               );
	HideElement(s_populationLabel       );
	HideElement(s_populationBox         );
}
