

#include "c3.h"

#include "profileDB.h"

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
#include "graphicsresscreen.h"

#include "keypress.h"

extern C3UI					*g_c3ui;
extern ProfileDB			*g_theProfileDB;
extern sint32				g_isGridOn;

#include "tiledmap.h"
extern TiledMap				*g_tiledMap;

#include "civapp.h"
extern CivApp				*g_civApp;

#include "SpriteGroupList.h"
extern SpriteGroupList		*g_unitSpriteGroupList;

static ctp2_Button	*s_resScreenButton;
static c3_PopupWindow *s_graphicsWindow	= NULL;
static C3Slider		*s_bright			= NULL,
					*s_gamma			= NULL,
					*s_color			= NULL,
					*s_contrast			= NULL;


static c3_Static	*s_unitSpeedN		 = NULL;
static C3Slider		*s_unitSpeed		 = NULL;

static c3_Static	*s_brightN			= NULL,
					*s_gammaN			= NULL,
					*s_colorN			= NULL,
					*s_contrastN		= NULL;
static aui_Switch			*s_walk		= NULL,


							*s_trade	= NULL,
							*s_wonder	= NULL,


							*s_politicalBorders	= NULL,
							*s_tradeRoutes = NULL,


							*s_cityInfluence = NULL,
							*s_grid		= NULL,
							
							*s_cityNames = NULL;


static BOOL			s_gridToggled = FALSE;
static BOOL			s_cityInfluenceToggled = FALSE;
static BOOL			s_politicalBordersToggled = FALSE;

static BOOL			s_unitAnimToggled = FALSE;

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

	GS_TOTAL };

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
	
	GS_TOTAL 
};




sint32	graphicsscreen_displayMyWindow()
{
	sint32 retval=0;
	if(!s_graphicsWindow) { retval = graphicsscreen_Initialize(); }

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->AddWindow(s_graphicsWindow);
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RegisterHandler(s_graphicsWindow);

	
	s_unitAnimToggled = FALSE;

	return retval;
}
sint32 graphicsscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( s_graphicsWindow->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RemoveHandler(s_graphicsWindow);

	return 1;
}



AUI_ERRCODE graphicsscreen_Initialize( void )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	s_gridToggled = FALSE;
	s_cityInfluenceToggled = FALSE;
	s_politicalBordersToggled = FALSE;


	if ( s_graphicsWindow ) return AUI_ERRCODE_OK; 

	strcpy(windowBlock, "GraphicsWindow");
	s_graphicsWindow = new c3_PopupWindow(
		&errcode,
		aui_UniqueId(),
		windowBlock,
		16,
		AUI_WINDOW_TYPE_FLOATING,
		false );
	Assert( AUI_NEWOK(s_graphicsWindow, errcode) );
	if ( !AUI_NEWOK(s_graphicsWindow, errcode) ) errcode;

	s_graphicsWindow->SetStronglyModal(TRUE);

	

	s_walk		= spNew_aui_Switch(&errcode,windowBlock,"WalkButton",graphicsscreen_checkPress,&check[GS_WALK]);


	s_trade		= spNew_aui_Switch(&errcode,windowBlock,"TradeButton",graphicsscreen_checkPress,&check[GS_TRADE]);
	s_wonder	= spNew_aui_Switch(&errcode,windowBlock,"WonderButton",graphicsscreen_checkPress,&check[GS_WONDER]);


	s_politicalBorders= spNew_aui_Switch(&errcode,windowBlock,"PoliticalBordersButton",graphicsscreen_checkPress,&check[GS_POLITICALBORDERS]);
	s_tradeRoutes= spNew_aui_Switch(&errcode,windowBlock,"TradeRoutesButton",graphicsscreen_checkPress,&check[GS_TRADEROUTES]);

	

	s_cityInfluence = spNew_aui_Switch(&errcode,windowBlock,"CityInflenceButton",graphicsscreen_checkPress,&check[GS_CITYINFLUENCE]);
	s_grid		= spNew_aui_Switch(&errcode,windowBlock,"GridButton",graphicsscreen_checkPress,&check[GS_GRID]);

	
	s_cityNames = spNew_aui_Switch(&errcode,windowBlock,"CityNamesButton", graphicsscreen_checkPress, &check[GS_CITYNAMES]);

	s_resScreenButton = spNew_ctp2_Button( &errcode, windowBlock, "ResolutionButton", graphicsscreen_selectResolution );

	
	s_unitSpeed = spNew_C3Slider(&errcode, windowBlock, "UnitSpeedSlider", graphicsscreen_unitSpeedSlide);
	s_unitSpeedN = spNew_c3_Static(&errcode, windowBlock, "UnitSpeedName");
	
	if(g_theProfileDB) {
		s_unitSpeed->SetValue(g_theProfileDB->GetUnitSpeed(), 0);
	} else {
		s_unitSpeed->SetValue(0,0);
	}

	
	s_walk->SetState(g_theProfileDB->IsUnitAnim());


	s_trade->SetState(g_theProfileDB->IsTradeAnim());
	s_wonder->SetState(g_theProfileDB->IsWonderMovies());


	s_politicalBorders->SetState(g_theProfileDB->GetShowPoliticalBorders());
	s_tradeRoutes->SetState(g_theProfileDB->GetShowTradeRoutes());

	
	s_cityNames->SetState(g_theProfileDB->GetShowCityNames());



	

	s_cityInfluence->SetState( g_theProfileDB->IsShowCityInfluence() );
	s_grid->SetState( g_isGridOn );

	MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( block, "%s.%s", windowBlock, "Name" );
	s_graphicsWindow->AddTitle( block );
	s_graphicsWindow->AddClose( graphicsscreen_exitPress );

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE graphicsscreen_Cleanup()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	if ( !s_graphicsWindow  ) return AUI_ERRCODE_OK; 

	g_c3ui->RemoveWindow( s_graphicsWindow->Id() );
	keypress_RemoveHandler(s_graphicsWindow);

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

	delete s_graphicsWindow;
	s_graphicsWindow = NULL;

	return AUI_ERRCODE_OK;
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
	}

	g_theProfileDB->Save();

	graphicsscreen_removeMyWindow(action);
}
#ifdef WIN32
static
#endif
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





	case GS_TRADE:		func = &ProfileDB::SetTradeAnim; break;
	case GS_WONDER:		func = &ProfileDB::SetWonderMovies; break;


	case GS_POLITICALBORDERS:
		func = &ProfileDB::SetShowPoliticalBorders; 
		s_politicalBordersToggled = TRUE;
		break;
	case GS_TRADEROUTES:	func = &ProfileDB::SetShowTradeRoutes; break;






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
		func = &ProfileDB::SetShowCityNames; break;
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
