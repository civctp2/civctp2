//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Dialog for altering map properties
//                (dry/wet, ocean/land, etc.)
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Removed a strange reference to the SP menu which I don't understand, but
//   could cause problems in the new interface (Its removal could also cause
//   problems, but I'm not sure what it did, so I don't know...)
//   (JJB)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - Added random map settings option. (5-Apr-2009 Maq)
//
//----------------------------------------------------------------------------


#include "c3.h"
#include "c3window.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3ui.h"
#include "aui_switch.h"
#include "aui_uniqueid.h"
#include "aui_stringtable.h"
#include "aui_action.h"

#include "ConstRecord.h"

#include "RandGen.h"

#include "civapp.h"
#include "profileDB.h"

#include "spnewgamewindow.h"
#include "custommapscreen.h"

#include "keypress.h"
#include "colorset.h"               // g_colorSet

extern C3UI					*g_c3ui;

extern ProfileDB			*g_theProfileDB;
extern CivApp				*g_civApp;

extern sint32				g_god;

static aui_StringTable *s_closeButtonStrings = NULL;
static c3_PopupWindow	*s_customMapWindow	= NULL;
static c3_Button	*s_back				= NULL;
static C3Slider		*s_wetdry			= NULL,
					*s_warmcold			= NULL,
					*s_oceanland		= NULL,
					*s_islandcontinent	= NULL,
					*s_homodiverse		= NULL,
					*s_goodcount		= NULL;
static c3_Static	*s_wet				= NULL,
					*s_dry				= NULL,
					*s_warm				= NULL,
					*s_cold				= NULL,
					*s_ocean			= NULL,
					*s_land				= NULL,
					*s_island			= NULL,
					*s_continent		= NULL,
					*s_homo				= NULL,
					*s_diverse			= NULL,
					*s_poor				= NULL,
					*s_rich				= NULL;

static sint32		s_useMode = 0;

static aui_Switch		*s_RandomCustomMap	= NULL,
						*s_NULL				= NULL;
enum
{
	R_RANDOMCUSTOMMAP,
	GP_TOTAL
};

static uint32 check[] =
{
	R_RANDOMCUSTOMMAP,
	GP_TOTAL
};
RandomGenerator             *custommapscreenRand=NULL;

sint32 custommapscreen_updateData()
{
	if(!g_theProfileDB) return -1;

	s_RandomCustomMap->SetState(g_theProfileDB->IsRandomCustomMap());

	return 1;
}

sint32	custommapscreen_displayMyWindow(BOOL viewMode, sint32 useMode)
{
	sint32 retval=0;
	if(!s_customMapWindow) { retval = custommapscreen_Initialize(); }

	custommapscreen_updateData();
	custommapscreen_updateWindow();

	AUI_ERRCODE auiErr;

	s_useMode = useMode;

	if ( s_useMode )// Not used.
	{
		s_customMapWindow->Cancel()->Show();
		s_customMapWindow->Ok()->SetText( s_closeButtonStrings->GetString( 1 ) );
	}
	else
	{
		s_customMapWindow->Cancel()->Hide();
		s_customMapWindow->Ok()->SetText( s_closeButtonStrings->GetString( 0 ) );
	}

	auiErr = g_c3ui->AddWindow(s_customMapWindow);
	keypress_RegisterHandler(s_customMapWindow);

	Assert( auiErr == AUI_ERRCODE_OK );

	return retval;
}
sint32 custommapscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	if (g_theProfileDB->IsRandomCustomMap())
	{
		sint32 ranWetDry,
			ranWarmCold,
			ranOceanLand,
			ranIslandContinent,
			ranHomoDiverse,
			ranGoodCount;
		ranWetDry = custommapscreenRand->Next(11);
		ranWarmCold = custommapscreenRand->Next(11);
		ranOceanLand = custommapscreenRand->Next(11);
		ranIslandContinent = custommapscreenRand->Next(11);
		ranHomoDiverse = custommapscreenRand->Next(11);
		ranGoodCount = custommapscreenRand->Next(11);
		custommapscreen_setValues(
			ranWetDry,
			ranWarmCold,
			ranOceanLand,
			ranIslandContinent,
			ranHomoDiverse,
			ranGoodCount);
	}
	else
	{
		custommapscreen_setValues(
			s_wetdry->GetValueX(),
			s_warmcold->GetValueX(),
			s_oceanland->GetValueX(),
			s_islandcontinent->GetValueX(),
			s_homodiverse->GetValueX(),
			s_goodcount->GetValueX() );
	}

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( s_customMapWindow->Id() );
	keypress_RemoveHandler(s_customMapWindow);

	Assert( auiErr == AUI_ERRCODE_OK );

	spnewgamescreen_update();

	return 1;
}



AUI_ERRCODE custommapscreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	uint32 seed = GetTickCount();
	srand(seed);
	custommapscreenRand = new RandomGenerator(seed); 

	if ( s_customMapWindow ) return AUI_ERRCODE_OK; 

	strcpy(windowBlock, "CustomMapWindow");
	{ 
		s_customMapWindow = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(s_customMapWindow, errcode) );
		if ( !AUI_NEWOK(s_customMapWindow, errcode) ) errcode;

		
		s_customMapWindow->Resize(s_customMapWindow->Width(),s_customMapWindow->Height());
		s_customMapWindow->GrabRegion()->Resize(s_customMapWindow->Width(),s_customMapWindow->Height());
		s_customMapWindow->SetStronglyModal(TRUE);
	}
	
	if ( !callback ) callback = custommapscreen_backPress;
	s_customMapWindow->AddOk( callback );

	
	s_customMapWindow->AddCancel( custommapscreen_cancelPress );

	s_wetdry			= spNew_C3Slider(&errcode,windowBlock,"WetDrySlider",custommapscreen_wetdrySlide);
	s_wet				= spNew_c3_Static(&errcode,windowBlock,"Wet");
	s_dry				= spNew_c3_Static(&errcode,windowBlock,"Dry");
	s_warmcold			= spNew_C3Slider(&errcode,windowBlock,"WarmColdSlider",custommapscreen_warmcoldSlide);
	s_warm				= spNew_c3_Static(&errcode,windowBlock,"Warm");
	s_cold				= spNew_c3_Static(&errcode,windowBlock,"Cold");
	s_oceanland			= spNew_C3Slider(&errcode,windowBlock,"OceanLandSlider",custommapscreen_oceanlandSlide);
	s_ocean				= spNew_c3_Static(&errcode,windowBlock,"Ocean");
	s_land				= spNew_c3_Static(&errcode,windowBlock,"Land");
	s_islandcontinent	= spNew_C3Slider(&errcode,windowBlock,"IslandContinentSlider",custommapscreen_islandcontinentSlide);
	s_island			= spNew_c3_Static(&errcode,windowBlock,"Island");
	s_continent			= spNew_c3_Static(&errcode,windowBlock,"Continent");
	s_homodiverse		= spNew_C3Slider(&errcode,windowBlock,"HomoDiverseSlider",custommapscreen_homodiverseSlide);
	s_homo				= spNew_c3_Static(&errcode,windowBlock,"Homo");
	s_diverse			= spNew_c3_Static(&errcode,windowBlock,"Diverse");
	s_goodcount			= spNew_C3Slider(&errcode,windowBlock,"GoodCountSlider",custommapscreen_goodcountSlide);
	s_poor				= spNew_c3_Static(&errcode,windowBlock,"Poor");
	s_rich				= spNew_c3_Static(&errcode,windowBlock,"Rich");
	s_RandomCustomMap	= spNew_aui_Switch(&errcode, windowBlock, "RandomCustomMap", custommapscreen_checkPress, &check[R_RANDOMCUSTOMMAP  ]);

	custommapscreen_updateData();

	s_wetdry->SetValue(g_theProfileDB->GetWetDry(), 0);
	s_warmcold->SetValue(g_theProfileDB->GetWarmCold(), 0);
	s_oceanland->SetValue(g_theProfileDB->GetOceanLand(), 0);
	s_islandcontinent->SetValue(g_theProfileDB->GetIslandContinent(), 0);
	s_homodiverse->SetValue(g_theProfileDB->GetHomoDiverse(), 0);
	s_goodcount->SetValue(g_theProfileDB->GetGoodCount(), 0);

	sprintf( controlBlock, "%s.%s", windowBlock, "Name" );
	s_customMapWindow->AddTitle( controlBlock );

	s_closeButtonStrings = new aui_StringTable(
		&errcode,
		"CustomMapWindow.CloseButtonStrings" );

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE custommapscreen_Cleanup()
{
#define mycleanup(mypointer) { delete mypointer; mypointer = NULL; };

	if ( !s_customMapWindow  ) return AUI_ERRCODE_OK; 

	g_c3ui->RemoveWindow( s_customMapWindow->Id() );
	keypress_RemoveHandler(s_customMapWindow);

	mycleanup(s_closeButtonStrings);

	mycleanup(s_wetdry);
	mycleanup(s_warmcold);
	mycleanup(s_oceanland);
	mycleanup(s_islandcontinent);
	mycleanup(s_homodiverse);
	mycleanup(s_goodcount);
	mycleanup(s_wet);
	mycleanup(s_dry);
	mycleanup(s_warm);
	mycleanup(s_cold);
	mycleanup(s_ocean);
	mycleanup(s_land);
	mycleanup(s_island);
	mycleanup(s_continent);
	mycleanup(s_homo);
	mycleanup(s_diverse);
	mycleanup(s_poor);
	mycleanup(s_rich);
	mycleanup(s_RandomCustomMap);
	mycleanup(custommapscreenRand);

	delete s_customMapWindow;
	s_customMapWindow = NULL;

	return AUI_ERRCODE_OK;

#undef mycleanup
}




AUI_ACTION_BASIC(SetupMapEditorAction);

void SetupMapEditorAction::Execute(aui_Control *, uint32, uint32)
{ ; }

void custommapscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{// This fires when you hover over the close button.
	if(custommapscreen_removeMyWindow(action)) {// This fires when you press close.

		if ( s_useMode == 1 ) {// This is not used.

			
			g_theProfileDB->SetSaveNote("");

			g_civApp->PostStartGameAction();

			
			g_c3ui->AddAction( new SetupMapEditorAction );
		}
	}
}

// This is not used.
void custommapscreen_cancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action == (sint32)AUI_BUTTON_ACTION_EXECUTE ) {
		s_wetdry->SetValue(g_theProfileDB->GetWetDry(), 0);
		s_warmcold->SetValue(g_theProfileDB->GetWarmCold(), 0);
		s_oceanland->SetValue(g_theProfileDB->GetOceanLand(), 0);
		s_islandcontinent->SetValue(g_theProfileDB->GetIslandContinent(), 0);
		s_homodiverse->SetValue(g_theProfileDB->GetHomoDiverse(), 0);
		s_goodcount->SetValue(g_theProfileDB->GetGoodCount(), 0);
	}

	custommapscreen_removeMyWindow(action);
}
void custommapscreen_wetdrySlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	


	if ( action != AUI_RANGER_ACTION_VALUECHANGE ) return;
}
void custommapscreen_warmcoldSlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	

	if ( action != AUI_RANGER_ACTION_VALUECHANGE ) return;
}
void custommapscreen_oceanlandSlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	

	if ( action != AUI_RANGER_ACTION_VALUECHANGE ) return;
}
void custommapscreen_islandcontinentSlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	

	if ( action != AUI_RANGER_ACTION_VALUECHANGE ) return;
}
void custommapscreen_homodiverseSlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	

	if ( action != AUI_RANGER_ACTION_VALUECHANGE ) return;
}
void custommapscreen_goodcountSlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	

	if ( action != AUI_RANGER_ACTION_VALUECHANGE ) return;
}





void custommapscreen_getValues(
	sint32 &wetdry,
	sint32 &warmcold,
	sint32 &oceanland,
	sint32 &islandcontinent,
	sint32 &homodiverse,
	sint32 &goodcount)
{
	wetdry			= s_wetdry->GetValueX();
	warmcold		= s_warmcold->GetValueX();
	oceanland		= s_oceanland->GetValueX();
	islandcontinent	= s_islandcontinent->GetValueX();
	homodiverse		= s_homodiverse->GetValueX();
	goodcount		= s_goodcount->GetValueX();

	
}






void custommapscreen_setValues(
	sint32 wetdry,
	sint32 warmcold,
	sint32 oceanland,
	sint32 islandcontinent,
	sint32 homodiverse,
	sint32 goodcount)
{
	if(s_wetdry)
		s_wetdry->SetValue( wetdry, 0 );
	if(s_warmcold)
		s_warmcold->SetValue( warmcold, 0 );
	if(s_oceanland)
		s_oceanland->SetValue( oceanland, 0 );
	if(s_islandcontinent)
		s_islandcontinent->SetValue( islandcontinent, 0 );
	if(s_homodiverse)
		s_homodiverse->SetValue( homodiverse, 0 );
	if(s_goodcount)
		s_goodcount->SetValue( goodcount, 0 );

	g_theProfileDB->SetWetDry(wetdry);
	g_theProfileDB->SetWarmCold(warmcold);
	g_theProfileDB->SetOceanLand(oceanland);
	g_theProfileDB->SetIslandContinent(islandcontinent);
	g_theProfileDB->SetHomoDiverse(homodiverse);
	g_theProfileDB->SetGoodCount(goodcount);
	
	
	

	
	
	
	
	
	sint32 forest =
		(sint32)g_theConstDB->Get(0)->GetForestDry() * wetdry +
		(sint32)g_theConstDB->Get(0)->GetForestWet() * ( 10 - wetdry );
	sint32 grass =
		(sint32)g_theConstDB->Get(0)->GetGrassDry() * wetdry +
		(sint32)g_theConstDB->Get(0)->GetGrassWet() * ( 10 - wetdry );
	sint32 plains =
		(sint32)g_theConstDB->Get(0)->GetPlainsDry() * wetdry +
		(sint32)g_theConstDB->Get(0)->GetPlainsWet() * ( 10 - wetdry );
	sint32 desert =
		(sint32)g_theConstDB->Get(0)->GetDesertDry() * wetdry +
		(sint32)g_theConstDB->Get(0)->GetDesertWet() * ( 10 - wetdry );
	

	g_theProfileDB->SetPercentForest( sint32(forest / 10) );
	g_theProfileDB->SetPercentGrass( sint32(grass / 10) );
	g_theProfileDB->SetPercentPlains( sint32(plains / 10) );
	g_theProfileDB->SetPercentDesert( sint32(desert / 10) );


	

	sint32 white =
		(sint32)g_theConstDB->Get(0)->GetWhiteCold() * warmcold +
		(sint32)g_theConstDB->Get(0)->GetWhiteWarm() * ( 10 - warmcold );
	sint32 brown =
		(sint32)g_theConstDB->Get(0)->GetBrownCold() * warmcold +
		(sint32)g_theConstDB->Get(0)->GetBrownWarm() * ( 10 - warmcold );
	sint32 temperatureRangeAdjust =
		(sint32)g_theConstDB->Get(0)->GetTemperatureRangeAdjustCold() * warmcold +
		(sint32)g_theConstDB->Get(0)->GetTemperatureRangeAdjustWarm() * ( 10 - warmcold );
		
	g_theProfileDB->SetPercentWhite( sint32(white / 10) );
	g_theProfileDB->SetPercentBrown( sint32(brown / 10) );
	g_theProfileDB->SetTemperatureRangeAdjust(sint32(temperatureRangeAdjust/10));


	

	
	g_theProfileDB->SetPercentLand( ((double)(0.9 * oceanland) / 10.0) + 0.1 );


	

	
	g_theProfileDB->SetPercentContinent( (double)islandcontinent / 10.0 );


	

	
	g_theProfileDB->SetHomogenous( 10 * homodiverse );


	

	sint32 richness =
		(sint32)g_theConstDB->Get(0)->GetRichnessManyGoods() * goodcount +
		(sint32)g_theConstDB->Get(0)->GetRichnessFewGoods() * ( 10 - goodcount );
#if 0   // Unused
	sint32 riverCellWidth =
		(sint32)g_theConstDB->Get(0)->GetRiverCellWidthManyGoods() * goodcount +
		(sint32)g_theConstDB->Get(0)->GetRiverCellWidthFewGoods() * ( 10 - goodcount );
	sint32 riverCellHeight =
		(sint32)g_theConstDB->Get(0)->GetRiverCellHeightManyGoods() * goodcount +
		(sint32)g_theConstDB->Get(0)->GetRiverCellHeightFewGoods() * ( 10 - goodcount );
#endif
	g_theProfileDB->SetPercentRichness( sint32(richness / 10) );
}

void custommapscreen_checkPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_PRESS ) return;

	uint32 checkbox = *((uint32*)cookie);
	void (ProfileDB::*func)(BOOL) = 0;
	uint32 state = data;

	switch(checkbox)
	{
		case R_RANDOMCUSTOMMAP  : func = &ProfileDB::SetRandomCustomMap     ; break;

		default             : Assert(0)                                     ; break;
	};

	if(func)
		(g_theProfileDB->*func)(state ? FALSE : TRUE);

	custommapscreen_updateWindow();
}

void custommapscreen_updateWindow()
{
	if (g_theProfileDB->IsRandomCustomMap())
	{
		s_wet->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
		s_dry->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
		s_warm->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
		s_cold->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
		s_ocean->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
		s_land->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
		s_island->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
		s_continent->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
		s_homo->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
		s_diverse->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
		s_poor->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
		s_rich->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));

		s_wetdry->HideChildren();
		s_warmcold->HideChildren();
		s_oceanland->HideChildren();
		s_islandcontinent->HideChildren();
		s_homodiverse->HideChildren();
		s_goodcount->HideChildren();

		sint32 ranWetDry,
			ranWarmCold,
			ranOceanLand,
			ranIslandContinent,
			ranHomoDiverse,
			ranGoodCount;
		ranWetDry = custommapscreenRand->Next(11);
		ranWarmCold = custommapscreenRand->Next(11);
		ranOceanLand = custommapscreenRand->Next(11);
		ranIslandContinent = custommapscreenRand->Next(11);
		ranHomoDiverse = custommapscreenRand->Next(11);
		ranGoodCount = custommapscreenRand->Next(11);
		custommapscreen_setValues(
			ranWetDry,
			ranWarmCold,
			ranOceanLand,
			ranIslandContinent,
			ranHomoDiverse,
			ranGoodCount);

		s_wetdry->Enable(false);
		s_warmcold->Enable(false);
		s_oceanland->Enable(false);
		s_islandcontinent->Enable(false);
		s_homodiverse->Enable(false);
		s_goodcount->Enable(false);
	}
	else
	{
		s_wet->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));
		s_dry->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));
		s_warm->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));
		s_cold->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));
		s_ocean->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));
		s_land->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));
		s_island->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));
		s_continent->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));
		s_homo->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));
		s_diverse->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));
		s_poor->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));
		s_rich->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));

		s_wetdry->ShowChildren();
		s_warmcold->ShowChildren();
		s_oceanland->ShowChildren();
		s_islandcontinent->ShowChildren();
		s_homodiverse->ShowChildren();
		s_goodcount->ShowChildren();

		s_wetdry->Enable(true);
		s_warmcold->Enable(true);
		s_oceanland->Enable(true);
		s_islandcontinent->Enable(true);
		s_homodiverse->Enable(true);
		s_goodcount->Enable(true);
	}

	s_customMapWindow->ShouldDraw(TRUE);
}
