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
// - Does not show anymore the current tarrain stats but those from
//   the last visit. - Dec. 23rd 2004 Martin Gühmann
// - Add in food, shields, and gold from any good that is present. PFT 3 apr 05
// - Moved Peter's last modification to Cell.cpp and UnseenCell.cpp, idially
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

extern sint32		g_fog_toggle;
extern sint32		g_god;

extern C3UI						*g_c3ui;
extern World					*g_theWorld;
extern StringDB					*g_theStringDB;
extern TerrainImprovementPool	*g_theTerrainImprovementPool;

c3_PopupWindow					*g_helpTileWindow = NULL;

static c3_Static			*s_tileRout			= NULL;
static c3_Static			*s_tileRoutV		= NULL;
static c3_Static			*s_tileFood			= NULL;
static c3_Static			*s_tileFoodV		= NULL;
static c3_Static			*s_tileProd			= NULL;
static c3_Static			*s_tileProdV		= NULL;
static c3_Static			*s_tileMove			= NULL;
static c3_Static			*s_tileMoveV		= NULL;
static c3_Static			*s_tileGood			= NULL;
static c3_Static			*s_tileGoodV		= NULL;
static c3_Static			*s_tileSale			= NULL;
static c3_Static			*s_tileSaleV		= NULL;
static c3_Static			*s_tileGold			= NULL;
static c3_Static			*s_tileGoldV		= NULL;

static aui_StringTable		*s_stringTable	= NULL;
static bool s_helptileIsShown = false;

enum { STR_SALE_VALUE=0,STR_NONE=1 };

static TileControl			*s_tileImage		= NULL;
static c3_Static			*s_tileITop			= NULL;
static c3_Static			*s_tileIBottom		= NULL;
static c3_Static			*s_tileITL			= NULL;
static c3_Static			*s_tileITR			= NULL;
static c3_Static			*s_tileIBL			= NULL;
static c3_Static			*s_tileIBR			= NULL;

#define IMPROVEMENT_LISTBOXldl "TileImprovementListBox"
#define HELPTILE_WINDOWldl "HelpTileWindow"
static void bExitPress( aui_Control *control, uint32 action, uint32 data, void *cookie );
static
sint32 removeMyWindow(uint32);

static AUI_ERRCODE newC3Static(MBCHAR *parent,MBCHAR *name,c3_Static **mystatic)
{
	MBCHAR			textBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	sprintf( textBlock, "%s.%s", parent, name );
	*mystatic = new c3_Static( &errcode, aui_UniqueId(), textBlock );
	Assert( AUI_NEWOK(*mystatic, errcode) );
	return errcode;
}

sint32 helptile_Initialize( void )
{
	AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
	MBCHAR			windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if(g_helpTileWindow){
		return 0;
	}

	strcpy(windowBlock, HELPTILE_WINDOWldl);
	g_helpTileWindow = new c3_PopupWindow(
		&errcode,
		aui_UniqueId(),
		windowBlock,
		16,
		AUI_WINDOW_TYPE_FLOATING,
		false );
	Assert( AUI_NEWOK(g_helpTileWindow, errcode) );
	if ( !AUI_NEWOK(g_helpTileWindow, errcode) ) return -1;

	g_helpTileWindow->AddClose( bExitPress );
	g_helpTileWindow->AddTitle();

	if(newC3Static(windowBlock,"RoutN",&s_tileRout)) return -1;
	if(newC3Static(windowBlock,"FoodN",&s_tileFood)) return -1;
	if(newC3Static(windowBlock,"ProdN",&s_tileProd)) return -1;
	if(newC3Static(windowBlock,"MoveN",&s_tileMove)) return -1;
	if(newC3Static(windowBlock,"GoodN",&s_tileGood)) return -1;
	if(newC3Static(windowBlock,"SaleN",&s_tileSale)) return -1;
	if(newC3Static(windowBlock,"RoutV",&s_tileRoutV)) return -1;
	if(newC3Static(windowBlock,"FoodV",&s_tileFoodV)) return -1;
	if(newC3Static(windowBlock,"ProdV",&s_tileProdV)) return -1;
	if(newC3Static(windowBlock,"MoveV",&s_tileMoveV)) return -1;
	if(newC3Static(windowBlock,"GoodV",&s_tileGoodV)) return -1;
	if(newC3Static(windowBlock,"SaleV",&s_tileSaleV)) return -1;

	if(newC3Static(windowBlock,"GoldN",&s_tileGold)) return -1;
	if(newC3Static(windowBlock,"GoldV",&s_tileGoldV)) return -1;

	s_stringTable = spNewStringTable(&errcode, "HelpTileStringTable");
	if (!s_stringTable) return -1;

	if(newC3Static(windowBlock,"TileITop",&s_tileITop)) return -1;
	if(newC3Static(windowBlock,"TileITL",&s_tileITL)) return -1;
	if(newC3Static(windowBlock,"TileITR",&s_tileITR)) return -1;
	if(newC3Static(windowBlock,"TileIBL",&s_tileIBL)) return -1;
	if(newC3Static(windowBlock,"TileIBR",&s_tileIBR)) return -1;
	if(newC3Static(windowBlock,"TileIBottom",&s_tileIBottom)) return -1;

	sprintf( buttonBlock, "%s.%s", windowBlock, "TileImage" );
	s_tileImage = new TileControl( &errcode, aui_UniqueId(), buttonBlock);
	Assert( AUI_NEWOK(s_tileImage, errcode) );
	if ( !AUI_NEWOK(s_tileImage, errcode) ) return -1;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return -1;

	return 0;
}

template <typename T>
static void mycleanup(T * & mypointer)
{ delete mypointer; mypointer = NULL; }

void helptile_Cleanup( void )
{
	if (g_helpTileWindow && g_c3ui)
	{
		g_c3ui->RemoveWindow(g_helpTileWindow->Id());
	}

	mycleanup(s_tileRout);
	mycleanup(s_tileFood);
	mycleanup(s_tileProd);
	mycleanup(s_tileGood);
	mycleanup(s_tileMove);
	mycleanup(s_tileSale);
	mycleanup(s_tileRoutV);
	mycleanup(s_tileFoodV);
	mycleanup(s_tileProdV);
	mycleanup(s_tileGoodV);
	mycleanup(s_tileMoveV);
	mycleanup(s_tileSaleV);
	mycleanup(s_tileGold);
	mycleanup(s_tileGoldV);
	mycleanup(s_stringTable);
	mycleanup(s_tileImage);
	mycleanup(s_tileITop);
	mycleanup(s_tileIBottom);
	mycleanup(s_tileITL);
	mycleanup(s_tileITR);
	mycleanup(s_tileIBL);
	mycleanup(s_tileIBR);
	mycleanup(g_helpTileWindow);
}

void helptile_displayData(const MapPoint &p)
{
	if(!g_helpTileWindow) { Assert(0); return; }

	char myname[256];
	char mytext[256];
	sint32 goods=0;

	const Cell *myTile = g_theWorld->GetCell(p);

	UnseenCellCarton ucell;
	if(!g_tiledMap->GetLocalVision()->IsVisible(p)
	&& !g_fog_toggle
	&& !g_god
	&& (g_player[g_selected_item->GetVisiblePlayer()]
	&& !g_player[g_selected_item->GetVisiblePlayer()]->m_hasGlobalRadar)
	&& g_tiledMap->GetLocalVision()->GetLastSeen(p, ucell)
	){
		strcpy(myname, g_theStringDB->GetNameStr(g_theTerrainDB->Get(ucell.m_unseenCell->GetTerrainType())->GetName()));
		g_helpTileWindow->TitleText()->SetText( myname );

		// Unfortunatly this kind of information is not stored in the
		// UnseenCell object.

		sint32 seen[k_MAX_PLAYERS];
		memset(seen, 0, sizeof(seen));
		sint32 numSeenTradeRutes= 0;
		for(int i= 0; i < myTile->GetNumTradeRoutes(); i++) {
		    TradeRoute route= myTile->GetTradeRoute(i);
		    if(route.SeenBy(g_selected_item->GetVisiblePlayer())){
			seen[route.GetOwner()]++;
			numSeenTradeRutes++;
			}
		    }

		std::string str= ""; // c++ string to avoid bothering about final length
		str.append(std::to_string(numSeenTradeRutes));
		str.append(":");

		for(int i = 0; i < k_MAX_PLAYERS; i++)
		    {
		    if(seen[i] > 0){
			MBCHAR civName[k_MAX_NAME_LEN];
			g_player[i]->m_civilisation->GetSingularCivName(civName);
			str.append(" ");
			str.append(civName);
			str.append(" (");
			str.append(std::to_string(seen[i]));
			str.append(")");
			}
		    }
		s_tileRoutV->SetText(str.c_str());

		sprintf( mytext , "%d\n", ucell.m_unseenCell->GetFoodProduced());
		s_tileFoodV->SetText(mytext);

		sprintf( mytext , "%d\n", ucell.m_unseenCell->GetShieldsProduced());
		s_tileProdV->SetText(mytext);

		sprintf( mytext, "%d", ucell.m_unseenCell->GetGoldProduced());
		s_tileGoldV->SetText(mytext);

		// Unfortunatly this kind of information is not stored in the
		// UnseenCell object.
		if(g_theWorld->IsGood(p))
		{
			StringId	goodStrID;

			myTile->GetGoodsIndex(goods);

			goodStrID = g_theWorld->GetTerrain(p)->GetResources(goods)->GetName();
			sprintf( mytext , "%s\n", g_theStringDB->GetNameStr(goodStrID));
			s_tileGoodV->SetText(mytext);
		}
		else
		{
			sprintf( mytext , "%s\n", s_stringTable->GetString(STR_NONE));
			s_tileGoodV->SetText(mytext);
		}

		s_tileSaleV->SetText("\0");
		s_tileSale->SetText("\0");

		sprintf( mytext , "%.1f\n", (float)(ucell.m_unseenCell->GetMoveCost() / 100.0) );
		s_tileMoveV->SetText(mytext);
	}
	else
	{
		strcpy(myname, g_theWorld->GetTerrainName(p));
		g_helpTileWindow->TitleText()->SetText( myname );

		sint32 seen[k_MAX_PLAYERS];
		memset(seen, 0, sizeof(seen));
		sint32 numSeenTradeRutes= 0;
		for(int i= 0; i < myTile->GetNumTradeRoutes(); i++) {
		    TradeRoute route= myTile->GetTradeRoute(i);
		    if(route.SeenBy(g_selected_item->GetVisiblePlayer())){
			seen[route.GetOwner()]++;
			numSeenTradeRutes++;
			}
		    }

		std::string str= ""; // c++ string to avoid bothering about final length
		str.append(std::to_string(numSeenTradeRutes));
		str.append(":");

		for(int i = 0; i < k_MAX_PLAYERS; i++)
		    {
		    if(seen[i] > 0){
			MBCHAR civName[k_MAX_NAME_LEN];
			g_player[i]->m_civilisation->GetSingularCivName(civName);
			str.append(" ");
			str.append(civName);
			str.append(" (");
			str.append(std::to_string(seen[i]));
			str.append(")");
			}
		    }
		s_tileRoutV->SetText(str.c_str());

		sprintf( mytext , "%d\n", myTile->GetFoodProduced());
		s_tileFoodV->SetText(mytext);

		sprintf( mytext , "%d\n", myTile->GetShieldsProduced());
		s_tileProdV->SetText(mytext);

		sprintf( mytext, "%d", myTile->GetGoldProduced());
		s_tileGoldV->SetText(mytext);

		if(g_theWorld->IsGood(p))
		{
			StringId	goodStrID;

			myTile->GetGoodsIndex(goods);

			goodStrID = g_theWorld->GetTerrain(p)->GetResources(goods)->GetName();
			sprintf( mytext , "%s\n", g_theStringDB->GetNameStr(goodStrID));
			s_tileGoodV->SetText(mytext);
		}
		else
		{
			sprintf( mytext , "%s\n", s_stringTable->GetString(STR_NONE));
			s_tileGoodV->SetText(mytext);
		}

		s_tileSaleV->SetText("\0");
		s_tileSale->SetText("\0");

		sprintf( mytext , "%.1f\n", (float)(myTile->GetMoveCost() / 100.0) );
		s_tileMoveV->SetText(mytext);
	}

	if(s_tileImage)
		s_tileImage->SetMouseTile(p);

	if ( g_c3ui->GetWindow(g_helpTileWindow->Id()) )
	{
		g_helpTileWindow->ShouldDraw();
	}
	else
	{
		g_c3ui->AddWindow(g_helpTileWindow);
	}

	s_helptileIsShown = true;
}

void helptile_setPosition(const MapPoint& p)
{
	sint32 i,j;
	maputils_WrapPoint(p.x,p.y,&j,&i);

	MapPoint pos (maputils_TileX2MapX(j,i), i);

	sint32 x,y;
	maputils_MapXY2PixelXY(p.x,p.y,&x,&y);
	g_helpTileWindow->Move(x,y);
}

bool helptile_isShown(void) { return s_helptileIsShown; }

void helptile_closeWindow(void)
{
	Assert(s_helptileIsShown);

  AUI_ERRCODE auiErr;
  auiErr = g_c3ui->RemoveWindow(g_helpTileWindow->Id());
  Assert(auiErr == AUI_ERRCODE_OK);

  s_helptileIsShown = false;
}

static
void bExitPress(aui_Control *control, uint32 action, uint32 data,
											 void *cookie) {
	removeMyWindow(action);
}

static
sint32 removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	helptile_closeWindow();

	return 1;
}
