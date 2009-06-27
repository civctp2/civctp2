

#include "c3.h"
#include "graphicsresscreen.h"

#include "profileDB.h"      // g_theProfileDB
#include "c3window.h"
#include "c3_popupwindow.h"
#include "ctp2_button.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3ui.h"
#include "aui_switch.h"
#include "aui_uniqueid.h"
#include "spnewgamewindow.h"
#include "graphicsscreen.h"
#include "keypress.h"
#include "tiledmap.h"       // g_tiledMap;
#include "civapp.h"
#include "SpriteGroupList.h"

extern C3UI					*g_c3ui;
extern sint32				g_isGridOn;
extern CivApp				*g_civApp;
extern SpriteGroupList		*g_unitSpriteGroupList;
extern SpriteGroupList		*g_goodSpriteGroupList;

static ctp2_Button	*s_resScreenButton;
static c3_PopupWindow *s_graphicsWindow			= NULL;
static C3Slider		*s_bright					= NULL,
					*s_gamma					= NULL,
					*s_color					= NULL,
					*s_contrast					= NULL;


static c3_Static	*s_unitSpeedN				= NULL;
static C3Slider		*s_unitSpeed				= NULL;

static c3_Static	*s_brightN					= NULL,
					*s_gammaN					= NULL,
					*s_colorN					= NULL,
					*s_contrastN				= NULL;
static aui_Switch	*s_walk						= NULL,


					*s_trade					= NULL,
					*s_wonder					= NULL,


					*s_politicalBorders			= NULL,
					*s_tradeRoutes				= NULL,


					*s_cityInfluence			= NULL,
					*s_grid						= NULL,
							
					*s_cityNames				= NULL,
					*s_civflags					= NULL,
					*s_smooth					= NULL,
					*s_armyNames				= NULL,
					*s_goodAnims				= NULL,
					*s_cityProd					= NULL,

					*s_NULL						= NULL;


static BOOL			s_gridToggled				= FALSE;
static BOOL			s_cityInfluenceToggled		= FALSE;
static BOOL			s_politicalBordersToggled	= FALSE;
static BOOL			s_armyNamesToggled			= FALSE;
static BOOL			s_civflagsToggled			= FALSE;
static BOOL			s_smoothToggled				= FALSE;
static BOOL			s_unitAnimToggled			= FALSE;
static BOOL			s_goodAnimToggled			= FALSE;
//static BOOL			s_showCityProdToggled		= FALSE;

enum
{
	GS_WALK,


	GS_TRADE,
	GS_WONDER,


	GS_POLITICALBORDERS,
	GS_TRADEROUTES,
	GS_WATER,

	GS_CITYINFLUENCE,
	GS_GRID,
	GS_CITYNAMES,	
	GS_ARMYNAMES,
	GS_CIVFLAGS,
	GS_SMOOTH,
	GS_GOODANIMS,
	GS_CITYPROD,

	GS_TOTAL
};

static uint32 check[] =
{
	GS_WALK,


	GS_TRADE,
	GS_WONDER,


	GS_POLITICALBORDERS,
	GS_TRADEROUTES,
	GS_WATER,

	GS_CITYINFLUENCE,
	GS_GRID,

	GS_CITYNAMES,
	GS_ARMYNAMES,
	GS_CIVFLAGS,
	GS_SMOOTH,
	GS_GOODANIMS,
	GS_CITYPROD,
	
	GS_TOTAL 
};




sint32	graphicsscreen_displayMyWindow()
{
	sint32 retval=0;
	if (!s_graphicsWindow) { retval = graphicsscreen_Initialize(); }

	AUI_ERRCODE auiErr  = g_c3ui->AddWindow(s_graphicsWindow);
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RegisterHandler(s_graphicsWindow);

	s_unitAnimToggled = FALSE;
	s_goodAnimToggled = FALSE;

	return retval;
}
sint32 graphicsscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow( s_graphicsWindow->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RemoveHandler(s_graphicsWindow);

	return 1;
}



AUI_ERRCODE graphicsscreen_Initialize( void )
{
	s_gridToggled = FALSE;
	s_cityInfluenceToggled = FALSE;
	s_politicalBordersToggled = FALSE;


	if ( s_graphicsWindow ) return AUI_ERRCODE_OK; 

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	strcpy(windowBlock, "GraphicsWindow");
	s_graphicsWindow = new c3_PopupWindow(
		&errcode,
		aui_UniqueId(),
		windowBlock,
		16,
		AUI_WINDOW_TYPE_FLOATING,
		false );
	Assert( AUI_NEWOK(s_graphicsWindow, errcode) );
	if ( !AUI_NEWOK(s_graphicsWindow, errcode) ) return errcode;

	s_graphicsWindow->SetStronglyModal(TRUE);

	

	s_walk				= spNew_aui_Switch(&errcode,windowBlock,"WalkButton",graphicsscreen_checkPress,&check[GS_WALK]);
	s_trade				= spNew_aui_Switch(&errcode,windowBlock,"TradeButton",graphicsscreen_checkPress,&check[GS_TRADE]);
	s_wonder			= spNew_aui_Switch(&errcode,windowBlock,"WonderButton",graphicsscreen_checkPress,&check[GS_WONDER]);
	s_politicalBorders	= spNew_aui_Switch(&errcode,windowBlock,"PoliticalBordersButton",graphicsscreen_checkPress,&check[GS_POLITICALBORDERS]);
	s_tradeRoutes		= spNew_aui_Switch(&errcode,windowBlock,"TradeRoutesButton",graphicsscreen_checkPress,&check[GS_TRADEROUTES]);
	s_cityInfluence		= spNew_aui_Switch(&errcode,windowBlock,"CityInflenceButton",graphicsscreen_checkPress,&check[GS_CITYINFLUENCE]);
	s_grid				= spNew_aui_Switch(&errcode,windowBlock,"GridButton",graphicsscreen_checkPress,&check[GS_GRID]);
	s_cityNames			= spNew_aui_Switch(&errcode,windowBlock,"CityNamesButton", graphicsscreen_checkPress, &check[GS_CITYNAMES]);
	s_armyNames			= spNew_aui_Switch(&errcode,windowBlock,"ArmyNamesButton", graphicsscreen_checkPress, &check[GS_ARMYNAMES]);
	s_civflags			= spNew_aui_Switch(&errcode,windowBlock,"CivFlagButton", graphicsscreen_checkPress, &check[GS_CIVFLAGS]);

	s_resScreenButton	= spNew_ctp2_Button( &errcode, windowBlock, "ResolutionButton", graphicsscreen_selectResolution );

	s_smooth			= spNew_aui_Switch(&errcode,windowBlock,"SmoothButton", graphicsscreen_checkPress, &check[GS_SMOOTH]);
	s_goodAnims			= spNew_aui_Switch(&errcode,windowBlock,"GoodsButton",graphicsscreen_checkPress,&check[GS_GOODANIMS]);
	s_cityProd			= spNew_aui_Switch(&errcode,windowBlock,"ShowCityProdButton",graphicsscreen_checkPress,&check[GS_CITYPROD]);

	s_unitSpeed			= spNew_C3Slider(&errcode, windowBlock, "UnitSpeedSlider", graphicsscreen_unitSpeedSlide);
	s_unitSpeedN		= spNew_c3_Static(&errcode, windowBlock, "UnitSpeedName");
	
	if(g_theProfileDB) {
		s_unitSpeed->SetValue(g_theProfileDB->GetUnitSpeed(), 0);
	} else {
		s_unitSpeed->SetValue(0,0);
	}

	
	s_walk				->SetState(g_theProfileDB->IsUnitAnim());
	s_trade				->SetState(g_theProfileDB->IsTradeAnim());
	s_wonder			->SetState(g_theProfileDB->IsWonderMovies());
	s_politicalBorders	->SetState(g_theProfileDB->GetShowPoliticalBorders());
	s_tradeRoutes		->SetState(g_theProfileDB->GetShowTradeRoutes());
	s_cityNames			->SetState(g_theProfileDB->GetShowCityNames());
	s_armyNames			->SetState(g_theProfileDB->GetShowArmyNames());
	s_civflags			->SetState(g_theProfileDB->IsCivFlags());
	s_smooth			->SetState(g_theProfileDB->IsSmoothBorders());
	s_goodAnims			->SetState(g_theProfileDB->IsGoodAnim());
	s_cityInfluence		->SetState(g_theProfileDB->IsShowCityInfluence());
	s_grid				->SetState(g_isGridOn);
	s_cityProd			->SetState(g_theProfileDB->IsShowCityProduction());

	MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( block, "%s.%s", windowBlock, "Name" );
	s_graphicsWindow->AddTitle( block );
	s_graphicsWindow->AddClose( graphicsscreen_exitPress );

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}



void graphicsscreen_Cleanup()
{
	if (g_c3ui && s_graphicsWindow)
    {
    	g_c3ui->RemoveWindow(s_graphicsWindow->Id());
	    keypress_RemoveHandler(s_graphicsWindow);
    }

#define mycleanup(mypointer) { delete mypointer; mypointer = NULL; };
	mycleanup(s_walk);
	mycleanup(s_trade);
	mycleanup(s_wonder);
	mycleanup(s_cityInfluence);
	mycleanup(s_grid);
	mycleanup(s_politicalBorders);
	mycleanup(s_tradeRoutes);
	mycleanup(s_cityNames);
	mycleanup(s_resScreenButton);
	mycleanup(s_unitSpeed);
	mycleanup(s_unitSpeedN);
	mycleanup(s_graphicsWindow);
	mycleanup(s_armyNames);
	mycleanup(s_civflags);
	mycleanup(s_smooth);
	mycleanup(s_goodAnims);
	mycleanup(s_cityProd);
#undef mycleanup
}




void graphicsscreen_screensizeSelect(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT  ) return;
	
	if(s_graphicsWindow) callbackSetSelected(control,cookie);

}
void graphicsscreen_exitPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	if (g_civApp->IsGameLoaded()) {
		if (s_gridToggled) {
			if (g_tiledMap) {
				g_tiledMap->Refresh();
				g_tiledMap->InvalidateMap();
			}
		}
		if (s_cityInfluenceToggled || s_politicalBordersToggled) {
			if (g_tiledMap) {
				g_tiledMap->Refresh();
				g_tiledMap->InvalidateMap();
			}
		}
		if (s_unitAnimToggled) {
			if (g_civApp->IsGameLoaded()) {
				g_unitSpriteGroupList->RefreshBasicLoads(GROUPTYPE_UNIT);
			}
		}
		// @todo fix updating good anims option mid-game.
		// This doesn't work for goods, unlike units above.
		if (s_goodAnimToggled) {
			if (g_civApp->IsGameLoaded()) {
				g_goodSpriteGroupList->RefreshBasicLoads(GROUPTYPE_GOOD);
			}
		}
	}

	g_theProfileDB->Save();

	graphicsscreen_removeMyWindow(action);
}

void graphicsscreen_checkPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_SWITCH_ACTION_PRESS ) return;

	uint32 checkbox = *((uint32*)cookie);
	void (ProfileDB::*func)(BOOL) = 0;
	uint32 state = data; 

	switch(checkbox) {
	case GS_WALK:		
		func = &ProfileDB::SetUnitAnim; 
		s_unitAnimToggled = TRUE;
		break;
	case GS_TRADE:		
		func = &ProfileDB::SetTradeAnim;
		break;
	case GS_WONDER:		
		func = &ProfileDB::SetWonderMovies;
		break;
	case GS_POLITICALBORDERS:
		func = &ProfileDB::SetShowPoliticalBorders; 
		s_politicalBordersToggled = TRUE;
		break;
	case GS_TRADEROUTES:	
		func = &ProfileDB::SetShowTradeRoutes;
		break;
	case GS_CITYINFLUENCE:
		func = NULL;
		g_theProfileDB->SetShowCityInfluence(!state);
		s_cityInfluenceToggled = TRUE;
		break;
	case GS_GRID:
		func = NULL;
		g_isGridOn = !state;
		s_gridToggled = TRUE;
		break;
	case GS_CITYNAMES:
		func = &ProfileDB::SetShowCityNames;
		break;
	case GS_ARMYNAMES:
		func = &ProfileDB::SetShowArmyNames;
		break;
	case GS_CIVFLAGS:
		func = &ProfileDB::SetShowCivFlags;
		break;
	case GS_SMOOTH:
		func = &ProfileDB::SetShowSmooth;
		break;
	case GS_GOODANIMS:
		func = &ProfileDB::SetGoodAnim;
		//s_goodAnimToggled = TRUE;
		break;
	case GS_CITYPROD:
		func = &ProfileDB::SetShowCityProduction;
		break;
	case GS_TOTAL:  break;
	default:  Assert(0); break;
	};

	if(func)
		(g_theProfileDB->*func)(state ? FALSE : TRUE); 
}

void graphicsscreen_selectResolution(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	if ( graphicsscreen_removeMyWindow(action) ) {
		graphicsresscreen_displayMyWindow();
	}
}

void graphicsscreen_brightSlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{

}
void graphicsscreen_gammaSlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{

}
void graphicsscreen_colorSlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{

}
void graphicsscreen_contrastSlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{

}

void graphicsscreen_unitSpeedSlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != AUI_RANGER_ACTION_VALUECHANGE ) return;

	g_theProfileDB->SetUnitSpeed(s_unitSpeed->GetValueX());
}




void graphicsscreen_getValues(sint32 &bright, sint32 &gamma, sint32 &color,sint32 &contrast)
{
	bright			= s_bright->GetValueX();
	gamma			= s_gamma->GetValueX();
	color			= s_color->GetValueX();
	contrast		= s_contrast->GetValueX();
}
