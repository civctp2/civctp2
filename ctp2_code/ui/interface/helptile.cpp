//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Info/status tile window handling
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
// - Does not show anymore the current terrain stats but those from
//   the last visit. - Dec. 23rd 2004 Martin Gühmann
// - Add in food, shields, and gold from any good that is present. PFT 3 apr 05
// - Moved Peter's last modification to Cell.cpp and UnseenCell.cpp, ideally
//   such code should only be put at one place. - April 12th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_stringtable.h"
#include "c3window.h"
#include "c3ui.h"
#include "c3_static.h"
#include "ctp2_button.h"
#include "c3_listbox.h"
#include "c3_listitem.h"
#include "c3_popupwindow.h"
#include "MapPoint.h"
#include "maputils.h"
#include "tilecontrol.h"

#include "Cell.h"
#include "World.h"
#include "TerrainRecord.h"
#include "StrDB.h"
#include "TerrImprove.h"
#include "TerrImprovePool.h"
#include "Civilisation.h"

#include "c3_dropdown.h"
#include "spnewgamewindow.h"

#include "helptile.h"

#include "ResourceRecord.h"

#include "UnseenCell.h" //Unseen cell info is needed
#include "tiledmap.h"
#include "SelItem.h"
#include "player.h"

extern sint32 g_fog_toggle;
extern sint32 g_god;

extern C3UI                   * g_c3ui;
extern World                  * g_theWorld;
extern StringDB               * g_theStringDB;
extern TerrainImprovementPool * g_theTerrainImprovementPool;

c3_PopupWindow * g_helpTileWindow = NULL;

static c3_Static * s_tileRout      = NULL;
static c3_Static * s_tileRoutValue = NULL;
static c3_Static * s_tileFood      = NULL;
static c3_Static * s_tileFoodValue = NULL;
static c3_Static * s_tileProd      = NULL;
static c3_Static * s_tileProdValue = NULL;
static c3_Static * s_tileMove      = NULL;
static c3_Static * s_tileMoveValue = NULL;
static c3_Static * s_tileGood      = NULL;
static c3_Static * s_tileGoodValue = NULL;
static c3_Static * s_tileSale      = NULL;
static c3_Static * s_tileSaleValue = NULL;
static c3_Static * s_tileGold      = NULL;
static c3_Static * s_tileGoldValue = NULL;

static aui_StringTable * s_stringTable = NULL;
static bool s_helptileIsShown = false;

enum {
	STR_SALE_VALUE = 0,
	STR_NONE       = 1
};

static TileControl * s_tileImage   = NULL;
static c3_Static   * s_tileITop    = NULL;
static c3_Static   * s_tileIBottom = NULL;
static c3_Static   * s_tileITL     = NULL;
static c3_Static   * s_tileITR     = NULL;
static c3_Static   * s_tileIBL     = NULL;
static c3_Static   * s_tileIBR     = NULL;

#define HELPTILE_WINDOWldl "HelpTileWindow"

AUI_ERRCODE newC3Static(MBCHAR * parent, MBCHAR * name, c3_Static ** c3Static)
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	MBCHAR textBlock[k_AUI_LDL_MAXBLOCK + 1];
	sprintf(textBlock, "%s.%s", parent, name);
	*c3Static = new c3_Static(&errcode, aui_UniqueId(), textBlock);
	Assert(AUI_NEWOK(*c3Static, errcode));
	return errcode;
}

sint32 removeTileWindow(uint32 action)
{
	if (action != (uint32)AUI_BUTTON_ACTION_EXECUTE) {
		return 0;
	}

	helptile_closeWindow();
	return 1;
}

void bExitPress(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	removeTileWindow(action);
}

sint32 helptile_Initialize()
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	MBCHAR windowBlock[k_AUI_LDL_MAXBLOCK + 1];
	MBCHAR buttonBlock[k_AUI_LDL_MAXBLOCK + 1];

	if (g_helpTileWindow) {
		return 0;
	}

	strcpy(windowBlock, HELPTILE_WINDOWldl);
	g_helpTileWindow = new c3_PopupWindow(&errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING,false);
	Assert(AUI_NEWOK(g_helpTileWindow, errcode));
	if (!AUI_NEWOK(g_helpTileWindow, errcode)) {
		return -1;
	}

	g_helpTileWindow->AddClose(bExitPress);
	g_helpTileWindow->AddTitle();

	if (newC3Static(windowBlock, "RoutN", &s_tileRout)      != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "FoodN", &s_tileFood)      != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "ProdN", &s_tileProd)      != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "MoveN", &s_tileMove)      != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "GoodN", &s_tileGood)      != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "SaleN", &s_tileSale)      != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "RoutV", &s_tileRoutValue) != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "FoodV", &s_tileFoodValue) != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "ProdV", &s_tileProdValue) != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "MoveV", &s_tileMoveValue) != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "GoodV", &s_tileGoodValue) != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "SaleV", &s_tileSaleValue) != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "GoldN", &s_tileGold)      != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "GoldV", &s_tileGoldValue) != AUI_ERRCODE_OK) return -1;

	s_stringTable = spNewStringTable(&errcode, "HelpTileStringTable");
	if (!s_stringTable) {
		return -1;
	}

	if (newC3Static(windowBlock, "TileITop",    &s_tileITop)    != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "TileITL",     &s_tileITL)     != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "TileITR",     &s_tileITR)     != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "TileIBL",     &s_tileIBL)     != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "TileIBR",     &s_tileIBR)     != AUI_ERRCODE_OK) return -1;
	if (newC3Static(windowBlock, "TileIBottom", &s_tileIBottom) != AUI_ERRCODE_OK) return -1;

	sprintf(buttonBlock, "%s.%s", windowBlock, "TileImage");
	s_tileImage = new TileControl(&errcode, aui_UniqueId(), buttonBlock);
	Assert(AUI_NEWOK(s_tileImage, errcode));
	if (!AUI_NEWOK(s_tileImage, errcode)) {
		return -1;
	}

	errcode = aui_Ldl::SetupHeirarchyFromRoot(windowBlock);
	Assert(AUI_SUCCESS(errcode));
	if (!AUI_SUCCESS(errcode)) {
		return -1;
	}

	return 0;
}

template <typename T>
static void cleanup(T * & pointer)
{
	delete pointer;
	pointer = NULL;
}

void helptile_Cleanup()
{
	if (g_helpTileWindow && g_c3ui) {
		g_c3ui->RemoveWindow(g_helpTileWindow->Id());
	}

	cleanup(s_tileRout);
	cleanup(s_tileFood);
	cleanup(s_tileProd);
	cleanup(s_tileGood);
	cleanup(s_tileMove);
	cleanup(s_tileSale);
	cleanup(s_tileRoutValue);
	cleanup(s_tileFoodValue);
	cleanup(s_tileProdValue);
	cleanup(s_tileGoodValue);
	cleanup(s_tileMoveValue);
	cleanup(s_tileSaleValue);
	cleanup(s_tileGold);
	cleanup(s_tileGoldValue);
	cleanup(s_stringTable);
	cleanup(s_tileImage);
	cleanup(s_tileITop);
	cleanup(s_tileIBottom);
	cleanup(s_tileITL);
	cleanup(s_tileITR);
	cleanup(s_tileIBL);
	cleanup(s_tileIBR);
	cleanup(g_helpTileWindow);
}

void helptile_displayData(const MapPoint & pos)
{
	if (!g_helpTileWindow)
	{
		Assert(false);
		return;
	}

	const Cell * tile = g_theWorld->GetCell(pos);
	UnseenCellCarton ucell;

	bool tileVisible = g_tiledMap->GetLocalVision()->IsVisible(pos)
			|| g_fog_toggle
			|| g_god
			|| (g_player[g_selected_item->GetVisiblePlayer()]
				&& g_player[g_selected_item->GetVisiblePlayer()]->m_hasGlobalRadar)
			|| !g_tiledMap->GetLocalVision()->GetLastSeen(pos, ucell);

	char name[256];
	strcpy(name, (tileVisible ? g_theWorld->GetTerrainName(pos)
			: g_theStringDB->GetNameStr(g_theTerrainDB->Get(ucell.m_unseenCell->GetTerrainType())->GetName())));
	sint32 foodProduced    = tileVisible ? tile->GetFoodProduced()    : ucell.m_unseenCell->GetFoodProduced();
	sint32 shieldsProduced = tileVisible ? tile->GetShieldsProduced() : ucell.m_unseenCell->GetShieldsProduced();
	sint32 goldProduced    = tileVisible ? tile->GetGoldProduced()    : ucell.m_unseenCell->GetGoldProduced();
	double moveCost        = tileVisible ? tile->GetMoveCost()        : (double)(ucell.m_unseenCell->GetMoveCost());

	g_helpTileWindow->TitleText()->SetText(name);

	// Unfortunately this kind of information is not stored in the UnseenCell object.
	sint32 seen[k_MAX_PLAYERS];
	memset(seen, 0, sizeof(seen));
	sint32 numSeenTradeRoutes= 0;
	for (sint32 i = 0; i < tile->GetNumTradeRoutes(); i++)
	{
		TradeRoute route= tile->GetTradeRoute(i);
		if (route.SeenBy(g_selected_item->GetVisiblePlayer()))
		{
			seen[route.GetOwner()]++;
			numSeenTradeRoutes++;
		}
	}

	std::string str= ""; // c++ string to avoid bothering about final length
	str.append(std::to_string(numSeenTradeRoutes));
	str.append(":");
	for (sint32 i = 0; i < k_MAX_PLAYERS; i++)
	{
		if (seen[i] > 0)
		{
			MBCHAR civName[k_MAX_NAME_LEN];
			g_player[i]->m_civilisation->GetSingularCivName(civName);
			str.append(" ");
			str.append(civName);
			str.append(" (");
			str.append(std::to_string(seen[i]));
			str.append(")");
		}
	}
	s_tileRoutValue->SetText(str.c_str());

	char text[256];
	sprintf(text , "%d\n", foodProduced);
	s_tileFoodValue->SetText(text);

	sprintf(text , "%d\n", shieldsProduced);
	s_tileProdValue->SetText(text);

	sprintf(text, "%d", goldProduced);
	s_tileGoldValue->SetText(text);

	// Unfortunately this kind of information is not stored in the UnseenCell object.
	if (g_theWorld->IsGood(pos))
	{
		StringId goodStrID;

		sint32 goods = 0;
		tile->GetGoodsIndex(goods);

		goodStrID = g_theWorld->GetTerrain(pos)->GetResources(goods)->GetName();
		sprintf(text , "%s\n", g_theStringDB->GetNameStr(goodStrID));
		s_tileGoodValue->SetText(text);
	}
	else
	{
		sprintf(text , "%s\n", s_stringTable->GetString(STR_NONE));
		s_tileGoodValue->SetText(text);
	}

	s_tileSaleValue->SetText("\0");
	s_tileSale->SetText("\0");

	sprintf(text , "%.1f\n", moveCost / 100.0);
	s_tileMoveValue->SetText(text);

	if (s_tileImage) {
		s_tileImage->SetMouseTile(pos);
	}

	if (g_c3ui->GetWindow(g_helpTileWindow->Id())) {
		g_helpTileWindow->ShouldDraw();
	}
	else {
		g_c3ui->AddWindow(g_helpTileWindow);
	}

	s_helptileIsShown = true;
}

void helptile_setPosition(const MapPoint & pos)
{
	sint32 x,y;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);
	g_helpTileWindow->Move(x,y);
}

bool helptile_isShown()
{
	return s_helptileIsShown;
}

void helptile_closeWindow()
{
	Assert(s_helptileIsShown);
	if (!s_helptileIsShown) {
		return;
	}

	AUI_ERRCODE errCode = g_c3ui->RemoveWindow(g_helpTileWindow->Id());
	Assert(errCode == AUI_ERRCODE_OK);
	s_helptileIsShown = false;
}
