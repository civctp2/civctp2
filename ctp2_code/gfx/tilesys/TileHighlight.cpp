//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Tile map
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
// - Draw paths in unexplored territory red for land and water based armies,
//   to not give away land and/or water locations.
// - Added comments and cleaned up the code somewhat.
// - Standardised min/max usage.
// - Added some bombard code (PFT)
// - Repaired crash when no order is active.
// - Corrected turn box computation for ship paths through cities.
// - Added debug pathing for the city astar. (17-Jan-2008 Martin Gühmann)
// - Added check if only movebonus units are in an army, and it returns the
//	 highest movebonus value of the army for entry cost (17-Mar-2009 Maq).
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "dynarr.h"
#include "SelItem.h"        // g_selected_item
#include "MapPoint.h"
#include "Path.h"
#include "World.h"          // g_theWorld
#include "ID.h"
#include "Army.h"
#include "Order.h"
#include "cellunitlist.h"
#include "player.h"         // g_player
#include "controlpanelwindow.h"
#include "OrderRecord.h"
#include "aui_surface.h"
#include "maputils.h"
#include "primitives.h"
#include "tiledmap.h"
#include "colorset.h"       // g_colorSet
#include "director.h"
#include "textutils.h"
#include "MoveFlags.h"
#include "ArmyData.h" 
#include "TerrainRecord.h"
#include "UnitData.h"

#define k_TURN_BOX_SIZE_MINIMUM     4
#define k_TURN_BOX_SIZE             8
#define k_TURN_XOFFSET              2
#define k_TURN_YOFFSET              4
#define k_TURN_POINTSIZE            8

#define k_TURN_COLOR                COLOR_BLACK
#define k_TURN_COLOR_GO             COLOR_GREEN
#define k_TURN_COLOR_WAIT           COLOR_YELLOW
#define k_TURN_COLOR_STOP           COLOR_RED
#define k_TURN_COLOR_UNFINISHED     COLOR_GRAY
#define k_TURN_COLOR_SPECIAL        COLOR_GREEN
#define k_TURN_COLOR_PROJECTILE     COLOR_WHITE

#define k_DASH_LENGTH               4
#define k_DOT_LENGTH                2  //PFT 07 Mar 05


namespace // unnamed = static
{

//----------------------------------------------------------------------------
//
// Name       : GetEntryCost
//
// Description: Determines entry cost of a tile for an army.
//
// Parameters : a_Army          : army to enter with
//              a_Place         : tile to enter
//
// Globals    : g_theWorld      : world map information
//
// Returns    : double          : entry cost
//
// Remark(s)  : Will return the entry cost, even when the player officially
//              may not know it.
//
//----------------------------------------------------------------------------
double GetEntryCost
(
	Army const &		a_Army,
	MapPoint const &	a_Place
)
{
	double	cost	= a_Army.GetMovementTypeAir() 
					  ? k_MOVE_AIR_COST 
					  : g_theWorld->GetMoveCost(a_Place);

	if ((a_Army.IsAtLeastOneMoveShallowWater() ||
		 a_Army.IsAtLeastOneMoveWater()
		) &&
		(!a_Army.IsAtLeastOneMoveLand())
	   ) 
	{ 
		// Army without land units: do not use roads/tunnels etc.
		TerrainRecord::Modifiers const * bareTerrainProperties  =
		    (g_theWorld->HasCity(a_Place))
		    ? g_theWorld->GetTerrain(a_Place)->GetEnvCityPtr()
		    : g_theWorld->GetTerrain(a_Place)->GetEnvBase();

		sint32	icost;
		if (bareTerrainProperties->GetMovement(icost)) 
		{
			cost = icost;
		}
	}
	sint32 highestbonus;
	if (a_Army.GetMoveBonusUnits(highestbonus)) {
		cost = highestbonus;
	}

	return cost;
}

//----------------------------------------------------------------------------
//
// Name       : IsKnownEntryCost
//
// Description: Determines whether a player may know the entry cost.
//
// Parameters : a_Army          : army to enter with
//              a_Place         : tile to enter
//
// Globals    : g_player        : player information
//
// Returns    : double          : entry cost
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool IsKnownEntryCost
(
	Army const &		a_Army,
	MapPoint const &	a_Place
)
{
	return g_player[a_Army.GetOwner()]->IsExplored(a_Place) ||
		   a_Army.GetMovementTypeAir(); 
}

} // namespace



#define INSURFACE(x, y) (x >= 0 && y >= 0 && x < pSurface->Width() && y < pSurface->Height())

#define FEQUAL(x,y) ((((y) - 0.00001) < x) && (x < ((y)+0.00001)))

//----------------------------------------------------------------------------
//
// Name       : TiledMap::CanDrawSpecialMove
//
// Description: Test if a selected army is either about to attack a city or 
//              can unload cargo at MapPoint &dest_pos
//
// Parameters : SELECT_TYPE sType   : selection mode (what's being selected)
//              Army &sel_army      : the selected army
//              MapPoint &old_pos   : ?
//              MapPoint &dest_pos  : 
//
// Globals    : g_theWorld      : world map information
//            : g_controlPanel  :
//            : g_selected_item : 
//
// Returns    : bool            : 
//
// Remark(s)  : 
//
//----------------------------------------------------------------------------
bool TiledMap::CanDrawSpecialMove(SELECT_TYPE sType, Army &sel_army, const MapPoint &old_pos, const MapPoint &dest_pos)
{
	switch (sType) {
	case SELECT_TYPE_LOCAL_ARMY:
		if (g_theWorld->HasCity(dest_pos)) { 
			if(g_controlPanel->GetTargetingMode() == CP_TARGETING_MODE_ORDER_PENDING) {
				const OrderRecord *rec = g_controlPanel->GetCurrentOrder();
				if(rec->GetTargetPretestEnemyCity()) {
					if(g_theWorld->GetCell(dest_pos)->GetCity().GetOwner() != g_selected_item->GetVisiblePlayer()) {
						return true;
					}
				}
			}
			return false;
		} else {
			return sel_army.CanAtLeastOneCargoUnloadAt(old_pos, dest_pos, TRUE);
		}
	case SELECT_TYPE_LOCAL_ARMY_UNLOADING:
		return sel_army.CanAtLeastOneCargoUnloadAt(old_pos, dest_pos, TRUE);
	default:
		return false;
	}
}

//----------------------------------------------------------------------------
//
// Name       : TiledMap::DrawLegalMove
//
// Description: Draws a projected move path at the screen.
//
// Parameters : pSurface        : screen to draw at
//
// Globals    : g_selected_item : item (army) to move
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void TiledMap::DrawLegalMove
(
	aui_Surface *	pSurface	
)
{
	if (!(g_selected_item->GetIsPathing() &&	// path available?
	      ReadyToDraw()
	     )
	   )
	{
		return;
	}
	// check that an army is selected
	PLAYER_INDEX	pIndex;
	ID				id;
	SELECT_TYPE		sType;
	g_selected_item->GetTopCurItem(pIndex, id, sType);

	// Probably there is a more elegant way to do it for the city pathing
	// but since it is a debug tool I leave it like this.
	if(sType == SELECT_TYPE_LOCAL_CITY)
	{
		Unit c;
		g_selected_item->GetSelectedCity(c);
		MapPoint		prevPos;
		c.GetPos(prevPos);

		Path			goodPath(g_selected_item->GetGoodPath());
		MapPoint		currPos;
		goodPath.Start(currPos);
		sint32			line_segment_count	= 0;
		COLOR			lineColor			= k_TURN_COLOR_GO;
		sint32 const	xoffset				= (sint32) ((k_TILE_PIXEL_WIDTH * m_scale) / 2);
		sint32 const	yoffset				= (sint32) (k_TILE_PIXEL_HEIGHT * m_scale);

		while (!goodPath.IsEnd()) 
		{
			prevPos = currPos;
			goodPath.Next(currPos);

			line_segment_count++; 

			if ((prevPos != currPos) &&
				TileIsVisible(prevPos.x, prevPos.y) &&
				TileIsVisible(currPos.x, currPos.y)
			   )
			{
				sint32 x1, y1, x2, y2;

				maputils_MapXY2PixelXY(prevPos.x, prevPos.y, &x1, &y1);
				maputils_MapXY2PixelXY(currPos.x, currPos.y, &x2, &y2);
				
				x1 += xoffset;
				y1 += yoffset;
				x2 += xoffset;
				y2 += yoffset;

				if (INSURFACE(x1, y1) && INSURFACE(x2, y2)) 
				{
					Pixel16 const	pixelColor	= g_colorSet->GetColor(lineColor);

					primitives_DrawAALine16(pSurface, x1, y1, x2, y2, pixelColor);

				}

				AddDirtyTileToMix(prevPos);
				AddDirtyTileToMix(currPos);
			}

		} // while goodPath
		return;
	}

	if ((sType != SELECT_TYPE_LOCAL_ARMY) &&
		(sType != SELECT_TYPE_LOCAL_ARMY_UNLOADING) 
	   )
	{
		return;
	}

	// Get army information
	Army		sel_army(id);
	Assert(sel_army.IsValid());

	//PFT: handle immobile units
	for (sint32 i = 0; i < sel_army.Num(); i++)
	{
		if (sel_army[i]->IsImmobile())
			return;
	}
	
	double			currMovementPoints;
	sel_army.CurMinMovementPoints(currMovementPoints); 
	if (currMovementPoints < 1.0)
	{
		currMovementPoints = -1.0;
	}

	bool			isFirstMove			= sel_army.GetFirstMoveThisTurn();//return Flag(k_UDF_FIRST_MOVE)

	sint32			num_tiles_to_half;//in case we need dotted path lines
	sint32			num_tiles_to_empty;
	sel_army.CalcRemainingFuel(num_tiles_to_half, num_tiles_to_empty);

	MapPoint		prevPos;
	sel_army.GetPos(prevPos); 

	// Draw the colored line segments

	sint32 const	xoffset				= (sint32) ((k_TILE_PIXEL_WIDTH * m_scale) / 2);
	sint32 const	yoffset				= (sint32) (k_TILE_PIXEL_HEIGHT * m_scale);

	Path			goodPath(g_selected_item->GetGoodPath());//this will be colored green/yellow
	MapPoint		currPos;
	goodPath.Start(currPos);

	bool			draw_one_special	= true; 
	sint32			line_segment_count	= 0; 
	sint32			special_line_segment= -1;

	COLOR			actual_line_color	= k_TURN_COLOR_GO; 
	COLOR			old_line_color		= k_TURN_COLOR_GO;// ??
	COLOR			lineColor			= 
		goodPath.IsEnd() ? k_TURN_COLOR_STOP : k_TURN_COLOR_GO;

	//PFT 07 Mar 05
	MapPoint move_pos = prevPos;//move_pos will become a position to move to if trying to bombard out of range
	MapPoint target_pos = goodPath.GetEnd();
	
	const OrderRecord *rec = g_controlPanel->GetCurrentOrder();
	sint32 min_rge, max_rge = 0, dist = 9999;

	if (rec && rec->GetUnitPretest_CanBombard() ){//test if it's a bombard order
	
		sel_army.AccessData()->GetBombardRange(min_rge, max_rge);

		if(max_rge){//army can bombard

			 dist = prevPos.NormalizedDistance(target_pos);

			 if(dist > max_rge){//target is out of range
				while (!goodPath.IsEnd() ) 
				{
					prevPos = move_pos;
					goodPath.Next(move_pos);

					sint32 tmp_dist = move_pos.NormalizedDistance(target_pos);
					if(tmp_dist <= max_rge){ //we're now within range, and have move_pos
						//shorten goodPath to end at move_pos
						sint32 tmp_rge = max_rge;
						while(tmp_rge > 0){
							goodPath.SnipEnd();
							tmp_rge--;
						}
						goodPath.Start(currPos);//reset goodPath
						break;//exit the loop and continue
					}
				}
			}
		}
	}
	// within range, so can bombard now
	if(dist <= max_rge){

		prevPos = currPos;
		currPos=target_pos;

		sint32 x1, y1, x2, y2;

		maputils_MapXY2PixelXY(prevPos.x, prevPos.y, &x1, &y1);
		maputils_MapXY2PixelXY(currPos.x, currPos.y, &x2, &y2);
			
		x1 += xoffset;
		y1 += yoffset;
		x2 += xoffset;
		y2 += yoffset;
	
		if (INSURFACE(x1, y1) && INSURFACE(x2, y2)) 
		{
			Pixel16 const	pixelColor	= g_colorSet->GetColor(k_TURN_COLOR_PROJECTILE);

			primitives_DrawDashedAALine16(pSurface, x1, y1, x2, y2, pixelColor, k_DOT_LENGTH);
		}
		return;
	}
	
	//else draw standard move path

	//start by drawing green/yellow (good) part
	while (!goodPath.IsEnd()) 
	{
		prevPos = currPos;
		goodPath.Next(currPos);

		line_segment_count++; 

		if (!IsKnownEntryCost(sel_army, currPos))
		{
			lineColor = k_TURN_COLOR_STOP;
		}
		
		if (currMovementPoints > 0) 
		{
			currMovementPoints	   -= GetEntryCost(sel_army, currPos);
			if (isFirstMove)
			{
				currMovementPoints  = std::max(currMovementPoints, 0.0);
				isFirstMove			= false;
			}
		}
		
		if (currMovementPoints < 0)	
		{	
			if (lineColor != k_TURN_COLOR_STOP)
			{
				lineColor = k_TURN_COLOR_WAIT;
			}
		}
		else if (FEQUAL(currMovementPoints,0.0)) 
		{
			currMovementPoints = -1;
		}

		if ((prevPos != currPos) &&
			TileIsVisible(prevPos.x, prevPos.y) &&
			TileIsVisible(currPos.x, currPos.y)
		   )
		{
			sint32 x1, y1, x2, y2;

			maputils_MapXY2PixelXY(prevPos.x, prevPos.y, &x1, &y1);
			maputils_MapXY2PixelXY(currPos.x, currPos.y, &x2, &y2);
				
			x1 += xoffset;
			y1 += yoffset;
			x2 += xoffset;
			y2 += yoffset;
				
			if ((sType == SELECT_TYPE_LOCAL_ARMY) || 
				((sType == SELECT_TYPE_LOCAL_ARMY_UNLOADING) && (line_segment_count == 0))
			   ) 
			{ 
				if (draw_one_special && CanDrawSpecialMove(sType, sel_army, prevPos, currPos)) 
				{ 
					draw_one_special		= false; 
					lineColor				= k_TURN_COLOR_SPECIAL;
					special_line_segment	= line_segment_count; 
				} 
			}
				
			if (INSURFACE(x1, y1) && INSURFACE(x2, y2)) 
			{
				Pixel16 const	pixelColor	= g_colorSet->GetColor(lineColor);
				if (num_tiles_to_half < line_segment_count) 
				{
					primitives_DrawDashedAALine16(pSurface, x1, y1, x2, y2, pixelColor, k_DASH_LENGTH);
				} 
				else 
				{
					primitives_DrawAALine16(pSurface, x1, y1, x2, y2, pixelColor);

					if( currPos == move_pos ){

						maputils_MapXY2PixelXY(currPos.x, currPos.y, &x1, &y1);
						maputils_MapXY2PixelXY(target_pos.x, target_pos.y, &x2, &y2);
							
						x1 += xoffset;
						y1 += yoffset;
						x2 += xoffset;
						y2 += yoffset;
					
						if (INSURFACE(x1, y1) && INSURFACE(x2, y2)) 
						{
							Pixel16 const	pixelColor	= g_colorSet->GetColor(k_TURN_COLOR_PROJECTILE);

							primitives_DrawDashedAALine16(pSurface, x1, y1, x2, y2, pixelColor, k_DOT_LENGTH);

							break;

						}
					}
				}
			}

			AddDirtyTileToMix(prevPos);
			AddDirtyTileToMix(currPos);
		}

		if (sType == SELECT_TYPE_LOCAL_ARMY_UNLOADING) 
		{ 
			break;
		}
	} // while goodPath
	// draw red (bad) part
	prevPos			= currPos;
	old_line_color	= lineColor; 

	Path			badPath(g_selected_item->GetBadPath());
//	sint32			badPath_old_index	= badPath.GetNextIndex(); // ??? not used
	badPath.Start(currPos);

	if (TileIsVisible(prevPos.x, prevPos.y) && 
		TileIsVisible(currPos.x, currPos.y)
	   )
	{
		sint32 x1, y1, x2, y2;

		maputils_MapXY2PixelXY(prevPos.x, prevPos.y, &x1, &y1);
		maputils_MapXY2PixelXY(currPos.x, currPos.y, &x2, &y2);
		
		x1 += xoffset;
		y1 += yoffset;
		x2 += xoffset;
		y2 += yoffset;

		if (g_player[g_selected_item->GetVisiblePlayer()]->IsExplored(currPos)) 
		{
			if (((sType == SELECT_TYPE_LOCAL_ARMY) || 
				 ((sType == SELECT_TYPE_LOCAL_ARMY_UNLOADING) && (line_segment_count == 0))
				) &&
				(draw_one_special && CanDrawSpecialMove(sType, sel_army, prevPos, currPos))
			   ) 
			{ 
				draw_one_special		= false; 
				actual_line_color		= k_TURN_COLOR_SPECIAL;
				special_line_segment	= line_segment_count; 
			} 
			else 
			{ 
				actual_line_color		= k_TURN_COLOR_STOP; 
			}
		}
		else 
		{ 
			actual_line_color = k_TURN_COLOR_STOP;
		} 
		line_segment_count++; 

		if (INSURFACE(x1, y1) && INSURFACE(x2, y2)) 
		{
			Pixel16 const	pixelColor	= g_colorSet->GetColor(actual_line_color);

			if (num_tiles_to_half < line_segment_count) 
			{
				primitives_DrawDashedAALine16(pSurface, x1, y1, x2, y2, pixelColor, k_DASH_LENGTH);
			}
			else {
				if(dist-max_rge < line_segment_count){//draw dotted line from bombard launch point to target
					primitives_DrawDashedAALine16(pSurface, x1, y1, x2, y2, pixelColor, k_DOT_LENGTH);
				}
				else
				{
					primitives_DrawAALine16(pSurface, x1, y1, x2, y2, pixelColor);
				}
			}
		}
	
		AddDirtyTileToMix(prevPos);
		AddDirtyTileToMix(currPos);
	}

	old_line_color = actual_line_color;
	
	while (!badPath.IsEnd()) 
	{
		prevPos = currPos;
		badPath.Next(currPos);
		line_segment_count++; 

		if ((prevPos != currPos) &&
			TileIsVisible(prevPos.x, prevPos.y) &&
			TileIsVisible(currPos.x, currPos.y)
		   )
		{
			sint32 x1, y1, x2, y2;

			maputils_MapXY2PixelXY(prevPos.x, prevPos.y, &x1, &y1);
			maputils_MapXY2PixelXY(currPos.x, currPos.y, &x2, &y2);

			x1 += xoffset;
			y1 += yoffset;
			x2 += xoffset;
			y2 += yoffset;

			if (g_player[g_selected_item->GetVisiblePlayer()]->IsExplored(currPos)) 
			{
				if (((sType == SELECT_TYPE_LOCAL_ARMY) || 
					 ((sType == SELECT_TYPE_LOCAL_ARMY_UNLOADING) && (line_segment_count == 1))
					) &&
					(draw_one_special && CanDrawSpecialMove(sType, sel_army, prevPos, currPos))
				   ) 
				{ 
					draw_one_special		= FALSE; 
					actual_line_color		= k_TURN_COLOR_SPECIAL;
					special_line_segment	= line_segment_count; 
				} 
				else 
				{ 
					actual_line_color = k_TURN_COLOR_STOP; 
				}
			}
			else 
			{ 
				actual_line_color = k_TURN_COLOR_STOP;
			} 
			old_line_color = actual_line_color; 

			if (INSURFACE(x1, y1) && INSURFACE(x2, y2))
			{
				primitives_DrawAALine16
					(pSurface, x1, y1, x2, y2, g_colorSet->GetColor(actual_line_color));
			}

			AddDirtyTileToMix(prevPos);
			AddDirtyTileToMix(currPos);
		}
	}

	// Restart to add the turn count boxes.
	
	old_line_color			= actual_line_color; 
	goodPath				= *g_selected_item->GetGoodPath();
	isFirstMove				= sel_army.GetFirstMoveThisTurn();
	line_segment_count		= -1;

	sint32 const	boxEdgeSize			= 
		std::max(static_cast<sint32>(k_TURN_BOX_SIZE_MINIMUM), 
				 static_cast<sint32>(k_TURN_BOX_SIZE * m_scale)
				);
	bool			special_box_done	= false; 
	sint32			turn				= 0;

	double	maxMovementPoints;
	sel_army.MinMovementPoints(maxMovementPoints); 
	if (maxMovementPoints < 1.0)
	{
		maxMovementPoints	= -1.0;
	}

	sel_army.CurMinMovementPoints(currMovementPoints); 
	double	count			= currMovementPoints;
	
	if (currMovementPoints < 1.0)
	{
		currMovementPoints	= -1.0;
		count				= maxMovementPoints;
		isFirstMove			= 1;
	}
	
	double	prevMovementPoints	= 0.0;
	goodPath.Start(currPos);
	
	while (!goodPath.IsEnd())	
	{
		line_segment_count++; 
		prevPos = currPos;
		goodPath.Next(currPos);

		if (!IsKnownEntryCost(sel_army, currPos))
		{
			return;
		}

		double const	cost	= GetEntryCost(sel_army, currPos);
		
		if (currMovementPoints > 0)
		{
			prevMovementPoints	= currMovementPoints;
			currMovementPoints -= cost;
			
			if (isFirstMove)
			{
				currMovementPoints = std::max(currMovementPoints, 0.0);
				// isFirstMove not reset yet: used for count later.
			}
		}
		
		COLOR		turnColor	= k_TURN_COLOR_GO;
		if (prevMovementPoints < 1.0)
		{
			turnColor = k_TURN_COLOR_WAIT;
		}
		else if (currMovementPoints <1.0)
		{
			currMovementPoints = -1;
		}
		
		if (count > 0)
		{
			count -= cost;
			if (isFirstMove)
			{
                count		= std::max(count, 0.0);
				isFirstMove = false;
			}
		}
		
		bool		countWasZero	= false;
		MapPoint	drawPos			= prevPos;

		if ((-0.01 < count) && (count < 0.01))
		{
			countWasZero	= true;
			count			= -1;
			drawPos			= currPos;
			isFirstMove		= true;
		}
		
		if (count < 0)
		{
			count = maxMovementPoints;
			
			if (!countWasZero)
			{
				if (cost > count)
				{
					count = -1;
				}
				else
				{
					count -= cost;
				}
			}
				
			turn++;
			prevMovementPoints = 0;
				
			if (TileIsVisible(drawPos.x, drawPos.y))
			{
				sint32	x;
				sint32	y;
				
				maputils_MapXY2PixelXY(drawPos.x, drawPos.y, &x, &y);
				
				x += xoffset;
				y += yoffset;
				
				RECT	turnRect	= 
					{x - boxEdgeSize, y - boxEdgeSize, x + boxEdgeSize, y + boxEdgeSize};

				if (INSURFACE(turnRect.left, turnRect.top) && INSURFACE(turnRect.right, turnRect.bottom)) 
				{
					MBCHAR turnNumber[80];
					
					if ((!special_box_done) && (special_line_segment == line_segment_count)) 
					{ 
						special_box_done = true; 
						sprintf(turnNumber, "*");
						actual_line_color = k_TURN_COLOR_SPECIAL;
					} 
					else 
					{
						sprintf(turnNumber,"%d",turn);
						actual_line_color = turnColor;
					} 
					
                    dist = drawPos.NormalizedDistance(target_pos);//pft
					if(dist > max_rge){
						primitives_PaintRect16(pSurface, &turnRect, g_colorSet->GetColor(actual_line_color));
						primitives_FrameRect16(pSurface, &turnRect, 0);

						sint32 const	width	= textutils_GetWidth(pSurface, turnNumber);
						sint32 const	height	= textutils_GetHeight(pSurface, turnNumber);
						sint32 const	textX	= x - (width >> 1);
						sint32 const	textY	= y - (height >> 1);
							
						primitives_DrawText(pSurface, textX, textY, turnNumber, 0, 1);
					}
				} 
			} 
		} 
	}
 
	MapPoint drawPos;
	badPath.Start(drawPos);

	if ((0 <= special_line_segment) && (!special_box_done) && !badPath.IsEnd())
	{ 
		if (goodPath.IsEnd() || (goodPath.Num() < 1)) 
		{ 
			badPath.Next(drawPos); 
		} 
		 
		if (TileIsVisible(drawPos.x,drawPos.y)) 
		{
			if (!IsKnownEntryCost(sel_army, drawPos))
			{
				return;
			}
			 
			sint32	x;
			sint32	y; 
			
			maputils_MapXY2PixelXY(drawPos.x, drawPos.y, &x, &y);
			 
			x += xoffset;
			y += yoffset;
			 
			RECT	turnRect	= 
				{x - boxEdgeSize, y - boxEdgeSize, x + boxEdgeSize, y + boxEdgeSize};
			 
			if (INSURFACE(turnRect.left, turnRect.top) && INSURFACE(turnRect.right, turnRect.bottom)) 
			{
                dist = drawPos.NormalizedDistance(target_pos);//pft
				if(dist > max_rge){
					MBCHAR turnNumber[80];						
					sprintf(turnNumber, "*");
					actual_line_color	= k_TURN_COLOR_SPECIAL;
					special_box_done	= true; 

					primitives_PaintRect16(pSurface, &turnRect, g_colorSet->GetColor(actual_line_color));
					primitives_FrameRect16(pSurface, &turnRect, 0);
					 

					sint32 const	width	= textutils_GetWidth(pSurface, turnNumber);
					sint32 const	height	= textutils_GetHeight(pSurface, turnNumber);
					sint32 const	textX	= x - (width >> 1);
					sint32 const	textY	= y - (height >> 1);
					 
					primitives_DrawText(pSurface, textX, textY, turnNumber, 0, 1);
				}
			}
		}
	}
}

void TiledMap::DrawUnfinishedMove(aui_Surface * pSurface)
{
	PLAYER_INDEX pIndex;
	ID id;
	SELECT_TYPE sType;
	g_selected_item->GetTopCurItem(pIndex,id,sType);
	if (sType != SELECT_TYPE_LOCAL_ARMY) {
		return;
	}

	Army 	sel_army 			= Army(id);
	Assert(sel_army.IsValid()); 
	if ( !sel_army.GetOrder(0) ) return;
	if ( !sel_army.GetOrder(0)->m_path ) return;
	
	//PFT: handle immobile units
	for (sint32 i = 0; i < sel_army.Num(); i++)
	{
		if (sel_army[i].IsImmobile())
			return;
	}
	
	Path goodPath(sel_army.GetOrder(0)->m_path);

	double 	currMovementPoints 	= 0.0;
	sel_army.CurMinMovementPoints(currMovementPoints); 
	if (currMovementPoints < 1.0)
		currMovementPoints = -1.0;

	bool	isFirstMove 		= sel_army.GetFirstMoveThisTurn();

	sint32 num_tiles_to_half; 
	sint32 num_tiles_to_empty;
	sel_army.CalcRemainingFuel(num_tiles_to_half, num_tiles_to_empty);

	sint32 line_segement_count = 0; 
	//get the army's currPos
	MapPoint	currPos;
	sel_army.GetPos(currPos);

	MapPoint    tempPos;
	goodPath.Start(tempPos);
	//update line_segement_count to the army's currPos
	while ( tempPos != currPos && !goodPath.IsEnd()) {
		goodPath.Next(tempPos);
		line_segement_count++;
	}

	//draw the (COLOR_GRAY) path from currPos
	sint32 xoffset = (sint32)((k_TILE_PIXEL_WIDTH*m_scale)/2);
	sint32 yoffset = (sint32)(k_TILE_PIXEL_HEIGHT*m_scale);
	
	MapPoint prevPos;
	while (!goodPath.IsEnd()) 
	{
		prevPos = currPos;
		goodPath.Next(currPos);
		uint16 lineColor = g_colorSet->GetColor(k_TURN_COLOR_UNFINISHED);
		double old, cost;
		line_segement_count++;
		
		if (!(sel_army.GetMovementTypeAir() || m_localVision->IsExplored(currPos)))
			break;
		
		if (currMovementPoints > 0)
		{
			old = currMovementPoints;
			Assert(sel_army.m_id != (0)); 
			cost = GetEntryCost(sel_army, currPos);
			
			if (isFirstMove)
			{
				
				if (cost > currMovementPoints)
					currMovementPoints = 0;
				else
					currMovementPoints -= cost;
				
				isFirstMove = 0;
			}
			else
				currMovementPoints -= cost;
		}
		
		if (FEQUAL(currMovementPoints,0.0)) 
			currMovementPoints = -1;
		
		if (prevPos != currPos) 
		{
			if (TileIsVisible(prevPos.x, prevPos.y) &&
				TileIsVisible(currPos.x, currPos.y))
			{
				sint32 x1, y1, x2, y2;
				maputils_MapXY2PixelXY(prevPos.x, prevPos.y, &x1, &y1);
				maputils_MapXY2PixelXY(currPos.x, currPos.y, &x2, &y2);
				
				
				x1 += xoffset;
				y1 += yoffset;
				x2 += xoffset;
				y2 += yoffset;
				
				
				
				if (INSURFACE(x1, y1) && INSURFACE(x2, y2)) {
					if (num_tiles_to_half < line_segement_count) {
						primitives_DrawDashedAALine16(pSurface, x1, y1, x2, y2, lineColor, k_DASH_LENGTH);
					} else {
						primitives_DrawAALine16(pSurface, x1, y1, x2, y2, lineColor);
					} 
				}
				
				AddDirtyTileToMix(prevPos);
				AddDirtyTileToMix(currPos);
			}
		}
	} 

	line_segement_count++;
	sint32 turn = 0;
	currMovementPoints=0.0;
	double prevMovementPoints=0.0;
	double maxMovementPoints=0.0;
	
	sel_army = Army(id);
	sel_army.CurMinMovementPoints(currMovementPoints); 
	double count = currMovementPoints; 
	sel_army.MinMovementPoints(maxMovementPoints); 
	if (maxMovementPoints < 1.0)
		maxMovementPoints = -1.0;
		
	isFirstMove =  sel_army.GetFirstMoveThisTurn();
		
	if (currMovementPoints < 1.0)
	{
		currMovementPoints = -1;
		prevMovementPoints = 0;
		count = maxMovementPoints;
		isFirstMove = true;
	}
	
	sel_army.GetPos(currPos);
	goodPath.Start( tempPos );
	//move down goodPath to reach currPos
	while ( tempPos != currPos && !goodPath.IsEnd()) {
		goodPath.Next(tempPos);
	}
	while (!goodPath.IsEnd())
	{
		prevPos = currPos;
		goodPath.Next(currPos);
		
		if (!(m_localVision->IsExplored(currPos) || sel_army.GetMovementTypeAir()))
			break;

		double const	cost	= GetEntryCost(sel_army, currPos);
		
		uint16 turnColor = g_colorSet->GetColor(k_TURN_COLOR_UNFINISHED);
		MapPoint drawPos = prevPos;
		
		
		if (currMovementPoints > 0)
		{
			prevMovementPoints = currMovementPoints;
			
			if (isFirstMove)
			{
				if (cost > currMovementPoints) 
					currMovementPoints = 0;
				else
					currMovementPoints -= cost;
				
			}
			else
				currMovementPoints -= cost;
		}
		
		if (prevMovementPoints < 1.0)		
			turnColor = g_colorSet->GetColor(k_TURN_COLOR_UNFINISHED);
		else if (currMovementPoints <1.0)	
			currMovementPoints = -1;
		
		if (count > 0)
		{
			if (isFirstMove)
			{
				if (cost > count) 
					count = 0;
				else
					count -= cost;
				
				isFirstMove = false;
			}
			else 
				count -= cost;
		}
		
		bool countWasZero = false;
		if ((-0.01 < count) && (count < 0.01))
		{
			drawPos 		= currPos;
			isFirstMove 	= true;
			countWasZero 	= true;
			count 			= -1;
		}
		
		if (count < 0)
		{
			count = maxMovementPoints;
			
			if (!countWasZero)
			{
				if (cost > count)
					count = -1;
				else
					count -= cost;
				
				turn++;
				prevMovementPoints = 0;
				
				sint32 x,y;
				
				maputils_MapXY2PixelXY(drawPos.x,drawPos.y,&x,&y);
				
				x += xoffset;
				y += yoffset;
				
				sint32	boxEdgeSize = (sint32)((double)k_TURN_BOX_SIZE * m_scale);
				if (boxEdgeSize < k_TURN_BOX_SIZE_MINIMUM) boxEdgeSize = k_TURN_BOX_SIZE_MINIMUM;
				
				RECT turnRect = {x - boxEdgeSize, 
					y - boxEdgeSize, 
					x + boxEdgeSize, 
					y + boxEdgeSize};
				
				if (TileIsVisible(drawPos.x,drawPos.y))
				{
					if (INSURFACE(turnRect.left, turnRect.top) && INSURFACE(turnRect.right, turnRect.bottom)) {
						primitives_PaintRect16(pSurface,&turnRect,turnColor);
						primitives_FrameRect16(pSurface,&turnRect,g_colorSet->GetColor(k_TURN_COLOR));
						
						MBCHAR turnNumber[80];
						
						sprintf(turnNumber,"%d",turn);
						
						COLORREF color = g_colorSet->GetColorRef(k_TURN_COLOR);
						
						sint32 width = textutils_GetWidth(pSurface, turnNumber);
						sint32 height = textutils_GetHeight(pSurface, turnNumber);
						
						sint32 textX = x - (width>>1);
						sint32 textY = y - (height>>1);
						
						primitives_DrawText(pSurface, textX, textY, turnNumber, color, 1);
					}
				}
			}
		}
	}
}
