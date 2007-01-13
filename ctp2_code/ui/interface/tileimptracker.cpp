//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Tile improvement handling
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
// - Added option to show info for tile improvements that are too expensive
//   and made it modifiable in-game.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_ldl.h"
#include "c3ui.h"
#include "aui_uniqueid.h"

#include "tileimptracker.h"
#include "c3window.h"
#include "c3_static.h"

#include "primitives.h"

#include "SelItem.h"
#include "TerrImprovePool.h"
#include "player.h"
#include "maputils.h"
#include "World.h"
#include "Cell.h"
#include "colorset.h"
#include "terrainutil.h"
#include "TerrainRecord.h"

extern C3UI *g_c3ui;
extern ColorSet *g_colorSet;

#include "profileDB.h"	// g_theProfileDB

namespace
{
	COLOR				s_trackerBorderColor	= COLOR_GREEN;
}

TileimpTrackerWindow	*g_tileImpTrackerWindow	= NULL;
static c3_Static		*s_trackerTimeN			= NULL;
static c3_Static		*s_trackerTimeV			= NULL;
static c3_Static		*s_trackerMatN			= NULL;
static c3_Static		*s_trackerMatV			= NULL;
static c3_Static		*s_trackerAdvN			= NULL;
static c3_Static		*s_trackerAdvV			= NULL;
static c3_Static		*s_trackerFoodN			= NULL;
static c3_Static		*s_trackerFoodV			= NULL;
static c3_Static		*s_trackerProductionN	= NULL;
static c3_Static		*s_trackerProductionV	= NULL;
static c3_Static		*s_trackerGoldN			= NULL;
static c3_Static		*s_trackerGoldV			= NULL;
static c3_Static        *s_trackerBackground    = NULL;

static sint32 s_tileImprovementNum = -1;

sint32 tileimptracker_Initialize()
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	strcpy( textBlock, "cp_tileimp_tracker");
	g_tileImpTrackerWindow = new TileimpTrackerWindow( &errcode, aui_UniqueId(), textBlock, 16);
	Assert( AUI_NEWOK(g_tileImpTrackerWindow, errcode) );
	if( !AUI_SUCCESS(errcode) ) return -1;
	

	sprintf( controlBlock, "%s.%s", textBlock, "MatN");
	s_trackerMatN = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_trackerMatN, errcode) );
	if( !AUI_SUCCESS(errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", textBlock, "MatV");
	s_trackerMatV = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_trackerMatV, errcode) );
	if( !AUI_SUCCESS(errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", textBlock, "FoodN");
	s_trackerFoodN = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_trackerFoodN, errcode) );
	if( !AUI_SUCCESS(errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", textBlock, "FoodV");
	s_trackerFoodV = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_trackerFoodV, errcode) );
	if( !AUI_SUCCESS(errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", textBlock, "ProductionN");
	s_trackerProductionN = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_trackerProductionN, errcode) );
	if( !AUI_SUCCESS(errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", textBlock, "ProductionV");
	s_trackerProductionV = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_trackerProductionV, errcode) );
	if( !AUI_SUCCESS(errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", textBlock, "GoldN");
	s_trackerGoldN = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_trackerGoldN, errcode) );
	if( !AUI_SUCCESS(errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", textBlock, "GoldV");
	s_trackerGoldV = new c3_Static( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_trackerGoldV, errcode) );
	if( !AUI_SUCCESS(errcode) ) return -1;

	sprintf( controlBlock, "%s.%s", textBlock, "Background");
	s_trackerBackground = new c3_Static(&errcode, aui_UniqueId(), controlBlock);
	Assert(AUI_NEWOK(s_trackerBackground, errcode));
	if(!AUI_SUCCESS(errcode)) return -1;

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( textBlock );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return -1;

	return 0;
}




void tileimptracker_DisplayData(MapPoint &p, sint32 type)
{
	if(!g_tileImpTrackerWindow) {
		tileimptracker_Initialize();
	}
	Assert(g_tileImpTrackerWindow);
	if(!g_tileImpTrackerWindow)
		return;

	MBCHAR		mytext[256];
	sint32		x, y;
	sint32		visPlayer = g_selected_item->GetVisiblePlayer();

	
	
    
	

	s_tileImprovementNum = type;
	if (s_tileImprovementNum < 0) {
		g_c3ui->RemoveWindow(g_tileImpTrackerWindow->Id());
		return;
	}

	sint32 extraData = 0;
	
	
	
	
	
	

	BOOL alreadyHasIt = g_theTerrainImprovementPool->HasImprovement(p, 
																	TERRAIN_IMPROVEMENT(s_tileImprovementNum), 
																	extraData);

	if(g_player[visPlayer]->IsExplored(p) && !alreadyHasIt) {

		maputils_MapXY2PixelXY(p.x,p.y,&x,&y);

		

		g_tileImpTrackerWindow->Move(x,y);

		TERRAIN_TYPES terr = g_theWorld->GetTerrainType(p);

		sint32	mat, time, 
				food, production,
				gold;

		sint32 extraData;	

		time = terrainutil_GetProductionTime(s_tileImprovementNum, p, 0);
		mat = terrainutil_GetProductionCost(s_tileImprovementNum, p, 0);

		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(s_tileImprovementNum);
		Cell *cell = g_theWorld->GetCell(p);

		if(rec->GetClassTerraform())
		{
			const TerrainRecord *oldT = g_theTerrainDB->Get(cell->GetTerrainType());
			sint32 t;
			if(rec->GetTerraformTerrainIndex(t)) {
				const TerrainRecord *newT = g_theTerrainDB->Get(t);
				sint32 oldFood = cell->GetFoodProduced();
				sint32 oldProd = cell->GetShieldsProduced();
				sint32 oldGold = cell->GetGoldProduced();
				
				sint32 newFood = newT->GetEnvBase()->GetFood();
				if(cell->HasRiver() && newT->GetEnvRiver()) {
					newFood += newT->GetEnvRiverPtr()->GetFood();
				}
				
				sint32 newProd = newT->GetEnvBase()->GetShield();
				if(cell->HasRiver() && newT->GetEnvRiver()) {
					newProd += newT->GetEnvRiverPtr()->GetShield();
				}
				
				sint32 newGold = newT->GetEnvBase()->GetGold();
				if(cell->HasRiver() && newT->GetEnvRiver()) {
					newGold += newT->GetEnvRiverPtr()->GetGold();
				}
				
				food = newFood - oldFood;
				production = newProd - oldProd;
				gold = newGold - oldGold;
			} else {
				Assert(false);
				food = 0;
				production = 0;
				gold = 0;
			}
		} 
		else 
		{
			









			
			
			
			
			
			const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(g_theTerrainImprovementDB->Get(s_tileImprovementNum), p);
																							 
			
			sint32 dFood = 0;
			if(eff) eff->GetBonusFood(dFood);
			sint32 dProd = 0;
			if(eff) eff->GetBonusProduction(dProd);
			sint32 dGold = 0;
			if(eff) eff->GetBonusGold(dGold);

			
			food = dFood + cell->GetFoodFromTerrain();
			production = dProd + cell->GetShieldsFromTerrain();
			extraData = 0;

			gold = dGold + cell->GetGoldFromTerrain();
		}



		sprintf(mytext,"%d", (sint32)mat);
		s_trackerMatV->SetText(mytext);

		sprintf(mytext,"%d", food);
		s_trackerFoodV->SetText(mytext);
		sprintf(mytext,"%d", production);
		s_trackerProductionV->SetText(mytext);
		sprintf(mytext,"%d", gold);
		s_trackerGoldV->SetText(mytext);


		ERR_BUILD_INST err; 
		bool const	checkMaterials	= !g_theProfileDB->GetValueByName("ShowExpensive");

		if (g_player[visPlayer]->CanCreateImprovement
				(TERRAIN_IMPROVEMENT(s_tileImprovementNum), p, extraData, checkMaterials, err)
		   ) 
		{
			if (g_player[visPlayer]->CanCreateImprovement
					(TERRAIN_IMPROVEMENT(s_tileImprovementNum), p, extraData, true, err)
			   ) 
			{
				s_trackerBorderColor = COLOR_GREEN;
			}
			else
			{
				s_trackerBorderColor = COLOR_RED;
			}
			g_c3ui->AddWindow(g_tileImpTrackerWindow);
		} 
		else 
		{
			g_c3ui->RemoveWindow(g_tileImpTrackerWindow->Id());
		}
		g_tileImpTrackerWindow->ShouldDraw();
		

		return;
	}
}

static void mycleanup(c3_Static **mypointer)
{ if(*mypointer) { delete *mypointer; *mypointer = NULL; } }
static void mycleanup(C3Window **mypointer)
{ if(*mypointer) { delete *mypointer; *mypointer = NULL; } }

sint32 tileimptracker_Cleanup()
{
	if ( !g_tileImpTrackerWindow) return 1; 
	
	g_c3ui->RemoveWindow( g_tileImpTrackerWindow->Id() );

	
	
	mycleanup(&s_trackerBackground);




	mycleanup(&s_trackerMatN);
	mycleanup(&s_trackerMatV);




	mycleanup(&s_trackerFoodN);
	mycleanup(&s_trackerFoodV);
	mycleanup(&s_trackerProductionN);
	mycleanup(&s_trackerProductionV);
	mycleanup(&s_trackerGoldN);
	mycleanup(&s_trackerGoldV);

	delete g_tileImpTrackerWindow;
	g_tileImpTrackerWindow = NULL;
	
	return 0;
}

AUI_ERRCODE TileimpTrackerWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y)
{
	if ( !surface ) surface = m_surface;

	RECT rect = { 0, 0, m_width, m_height };
	
	primitives_PaintRect16( surface, &rect, 0x0000 );
	
	C3Window::DrawThis(surface,x,y);

	primitives_FrameRect16(surface, &rect, g_colorSet->GetColor(s_trackerBorderColor));

	return AUI_ERRCODE_OK;
}

