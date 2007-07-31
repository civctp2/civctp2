











#include "c3.h"
#include "aui.h"

#include "DynArr.h"
#include "SelItem.h"
#include "MapPoint.h"
#include "Path.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "ID.h"
#include "Army.h"
#include "order.h"
#include "CellUnitList.h"
#include "Player.h"

#include "aui_Surface.h"

#include "maputils.h"
#include "primitives.h"
#include "TiledMap.h"
#include "ColorSet.h"
#include "Director.h"

#include "buttonbank.h"

#include "textutils.h"

#include "MoveFlags.h"

extern SelectedItem		*g_selected_item;
extern Player			**g_player;
extern World			*g_theWorld;
extern ColorSet			*g_colorSet;


extern ORDERMODE		g_orderModeOrder;

#define k_TURN_BOX_SIZE				8
#define k_TURN_XOFFSET				2
#define k_TURN_YOFFSET				4
#define k_TURN_POINTSIZE			8

#define k_TURN_COLOR				COLOR_BLACK
#define k_TURN_COLOR_GO				COLOR_GREEN
#define k_TURN_COLOR_WAIT		    COLOR_YELLOW
#define k_TURN_COLOR_STOP			COLOR_RED
#define k_TURN_COLOR_UNFINISHED		COLOR_GRAY

#define k_TURN_COLOR_SPECIAL        COLOR_GREEN

#define	k_TURN_BOX_SIZE_MINIMUM		4

#define k_DASH_LENGTH				4

#include "TerrainRecord.h"

#include "ControlPanelWindow.h"
#include "OrderRecord.h"

static Unit GetUnitWithLeastMovePoints(void)
{
	return 0;
}


#define INSURFACE(x, y) (x >= 0 && y >= 0 && x < pSurface->Width() && y < pSurface->Height())

#define FEQUAL(x,y) ((((y) - 0.00001) < x) && (x < ((y)+0.00001)))


BOOL TiledMap::CanDrawSpecialMove(SELECT_TYPE sType, Army &sel_army, MapPoint &old_pos, MapPoint &dest_pos)
{ 
    
    switch (sType) { 
    case SELECT_TYPE_LOCAL_ARMY:    
        if (g_theWorld->HasCity(dest_pos)) { 
			if(g_controlPanel->GetTargetingMode() == CP_TARGETING_MODE_ORDER_PENDING) {
				const OrderRecord *rec = g_controlPanel->GetCurrentOrder();
				if(rec->GetTargetPretestEnemyCity()) {
					if(g_theWorld->GetCell(dest_pos)->GetCity().GetOwner() != g_selected_item->GetVisiblePlayer()) {
						return TRUE;
					}
				}
			}
            return FALSE; 
        } else { 
            return sel_army.CanAtLeastOneCargoUnloadAt(old_pos, dest_pos, TRUE); 
        }
    case SELECT_TYPE_LOCAL_ARMY_UNLOADING:
        return sel_army.CanAtLeastOneCargoUnloadAt(old_pos, dest_pos, TRUE); 
    default:
        return FALSE; 
    } 
} 

void TiledMap::DrawLegalMove(
	aui_Surface *pSurface	
	)
{

	
	

    if (g_selected_item->GetIsPathing() == FALSE) 
        return; 

	if(!ReadyToDraw())
		return;

    PLAYER_INDEX pIndex;
	ID id;
	SELECT_TYPE sType;
    g_selected_item->GetTopCurItem(pIndex,id,sType);
    if (sType != SELECT_TYPE_LOCAL_ARMY && 
		sType != SELECT_TYPE_LOCAL_ARMY_UNLOADING) { 
        return; 
    } 

	
	
	Path goodPath(g_selected_item->GetGoodPath());
	Path badPath = g_selected_item->GetBadPath();

	
	
	
	

	sint32 badPath_old_index = badPath.GetNextIndex(); 

	MapPoint currPos, prevPos;

	sint32 xoffset = (sint32)((k_TILE_PIXEL_WIDTH*m_scale)/2);
	sint32 yoffset = (sint32)(k_TILE_PIXEL_HEIGHT*m_scale);
    Army sel_army;
    uint16 old_line_color = g_colorSet->GetColor(k_TURN_COLOR_GO);
    uint16 actual_line_color = g_colorSet->GetColor(k_TURN_COLOR_GO); 
    uint16 lineColor = g_colorSet->GetColor(k_TURN_COLOR_STOP); 

    BOOL draw_one_special = TRUE; 
	double currMovementPoints = 0.0;
	sint32 isFirstMove = FALSE;	
	sint32 fuel = 0;
	sint32 halfFuel = -1;
      
    sint32 line_segment_count=0; 
    sint32 special_line_segment = -1;

	
	if ((sType == SELECT_TYPE_LOCAL_ARMY) ||
        (sType == SELECT_TYPE_LOCAL_ARMY_UNLOADING))
	{
        sel_army = Army(id);
        Assert(sel_army.m_id != (0)); 
        sel_army.CurMinMovementPoints(currMovementPoints); 
		if (currMovementPoints < 1.0)
			currMovementPoints = -1.0;

		isFirstMove =  sel_army.GetFirstMoveThisTurn();
        

        sel_army.GetPos(prevPos); 
    }  else { 
        return; 
    } 

    sint32 num_tiles_to_half;
    sint32 num_tiles_to_empty;

    sel_army.CalcRemainingFuel(num_tiles_to_half, num_tiles_to_empty);
    sint32 owner = sel_army.GetOwner(); 





	goodPath.Start(currPos);

	
	while (!goodPath.IsEnd()) {
		prevPos = currPos;
		goodPath.Next(currPos);
        line_segment_count++; 

		lineColor = g_colorSet->GetColor(k_TURN_COLOR_GO);
		double old, cost;

		if ( sel_army.m_id != (0) && sel_army.GetMovementTypeAir() ) {
			fuel--;
		}

		
		if (currMovementPoints > 0) {
			
			old = currMovementPoints;
			if(sel_army.m_id != (0)) {
				if (sel_army.GetMovementTypeAir()) { 
					cost = k_MOVE_AIR_COST; 
				} else if (((sel_army.IsAtLeastOneMoveShallowWater() ||
							 sel_army.IsAtLeastOneMoveWater())) &&
						   (!sel_army.IsAtLeastOneMoveLand())) { 
					if(g_theWorld->GetTerrain(currPos)->GetEnvBase()->GetMovement()) {
						sint32 icost;
						g_theWorld->GetTerrain(currPos)->GetEnvBase()->GetMovement(icost);
						cost=icost;
					}
				} else { 
					cost = g_theWorld->GetMoveCost(currPos);
				}
			} else {
				
				cost = g_theWorld->GetMoveCost(currPos);
			}
			
			
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
		
		if (currMovementPoints < 0)	
			
			
            
            lineColor = g_colorSet->GetColor(k_TURN_COLOR_WAIT); 
		else if (FEQUAL(currMovementPoints,0.0)) 
			currMovementPoints = -1;
		
		if (prevPos != currPos) 
		{
			sint32 x1, y1, x2, y2;
			
			
			if (TileIsVisible(prevPos.x, prevPos.y) &&
				TileIsVisible(currPos.x, currPos.y))
			{
				
				maputils_MapXY2PixelXY(prevPos.x, prevPos.y, &x1, &y1);
				maputils_MapXY2PixelXY(currPos.x, currPos.y, &x2, &y2);
				
				
				x1 += xoffset;
				y1 += yoffset;
				x2 += xoffset;
				y2 += yoffset;
				
				if (sType == SELECT_TYPE_LOCAL_ARMY || 
					((sType == SELECT_TYPE_LOCAL_ARMY_UNLOADING) && (line_segment_count == 0))) { 
					
					if (draw_one_special && CanDrawSpecialMove(sType, sel_army, prevPos, currPos)) { 
						draw_one_special = FALSE; 
						lineColor = g_colorSet->GetColor(k_TURN_COLOR_SPECIAL);
						special_line_segment = line_segment_count; 
						
					} 
				}
				
				
				
				if (INSURFACE(x1, y1) && INSURFACE(x2, y2)) {
					if (num_tiles_to_half < line_segment_count) {
						primitives_DrawDashedAALine16(pSurface, x1, y1, x2, y2, lineColor, k_DASH_LENGTH);
					} else {
						primitives_DrawAALine16(pSurface, x1, y1, x2, y2, lineColor);
					}
				}

				AddDirtyTileToMix(prevPos);
				AddDirtyTileToMix(currPos);
			}
		}

		if(sType == SELECT_TYPE_LOCAL_ARMY_UNLOADING) { 
			break;
		}
	} 


    old_line_color = lineColor; 

	
	prevPos = currPos;
	badPath.Start(currPos);

	sint32 x1, y1, x2, y2;

	
	if (TileIsVisible(prevPos.x, prevPos.y) &&
		TileIsVisible(currPos.x, currPos.y))
	{
		
		maputils_MapXY2PixelXY(prevPos.x, prevPos.y, &x1, &y1);
		maputils_MapXY2PixelXY(currPos.x, currPos.y, &x2, &y2);

		
		x1 += xoffset;
		y1 += yoffset;
		x2 += xoffset;
		y2 += yoffset;

		
		

        
        
        if (!g_player[g_selected_item->GetVisiblePlayer()]->IsExplored(currPos)) { 
            if ((old_line_color == g_colorSet->GetColor(k_TURN_COLOR_GO)) ||
                 (old_line_color == g_colorSet->GetColor(k_TURN_COLOR_WAIT))) {

                if (sel_army.CanEnter(currPos)) { 
                     actual_line_color = g_colorSet->GetColor(k_TURN_COLOR_WAIT); 
                } else { 
                     actual_line_color = g_colorSet->GetColor(k_TURN_COLOR_STOP); 
                } 
            } else { 
                actual_line_color = g_colorSet->GetColor(k_TURN_COLOR_STOP); 
            } 
        } else {
             if ((sType == SELECT_TYPE_LOCAL_ARMY || 
                   ((sType == SELECT_TYPE_LOCAL_ARMY_UNLOADING) && (line_segment_count == 0))) &&
                 (draw_one_special && CanDrawSpecialMove(sType, sel_army, prevPos, currPos))) { 
                draw_one_special = FALSE; 
                actual_line_color  = g_colorSet->GetColor(k_TURN_COLOR_SPECIAL);
                special_line_segment = line_segment_count; 
            } else { 
                actual_line_color = g_colorSet->GetColor(k_TURN_COLOR_STOP); 
            }
        }
        line_segment_count++; 

        if (INSURFACE(x1, y1) && INSURFACE(x2, y2)) {
			if (num_tiles_to_half < line_segment_count) {                            
				primitives_DrawDashedAALine16(pSurface, x1, y1, x2, y2, actual_line_color, k_DASH_LENGTH);
            } else {
				primitives_DrawAALine16(pSurface, x1, y1, x2, y2, actual_line_color);
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


		if (prevPos != currPos) 
		{
			sint32 x1, y1, x2, y2;

			
			if (TileIsVisible(prevPos.x, prevPos.y) &&
				TileIsVisible(currPos.x, currPos.y))
			{
				
				maputils_MapXY2PixelXY(prevPos.x, prevPos.y, &x1, &y1);
				maputils_MapXY2PixelXY(currPos.x, currPos.y, &x2, &y2);

				
				x1 += xoffset;
				y1 += yoffset;
				x2 += xoffset;
				y2 += yoffset;

                
                
                if (!g_player[g_selected_item->GetVisiblePlayer()]->IsExplored(currPos)) { 
                    if ((old_line_color ==  g_colorSet->GetColor(k_TURN_COLOR_GO)) ||
                         (old_line_color ==  g_colorSet->GetColor(k_TURN_COLOR_WAIT))) {
                             actual_line_color = g_colorSet->GetColor(k_TURN_COLOR_WAIT); 
                    } else { 
                        actual_line_color = g_colorSet->GetColor(k_TURN_COLOR_STOP); 
                    } 
                } else {
                    
                if ((sType == SELECT_TYPE_LOCAL_ARMY || 
                   ((sType == SELECT_TYPE_LOCAL_ARMY_UNLOADING) && (line_segment_count == 1))) &&
                     (draw_one_special && CanDrawSpecialMove(sType, sel_army, prevPos, currPos))) { 
                        draw_one_special = FALSE; 
                        actual_line_color  = g_colorSet->GetColor(k_TURN_COLOR_SPECIAL);
                        special_line_segment = line_segment_count; 
                    } else { 
                       actual_line_color = g_colorSet->GetColor(k_TURN_COLOR_STOP); 
                    }
                }
                old_line_color = actual_line_color; 

				
				
				if (INSURFACE(x1, y1) && INSURFACE(x2, y2))
					primitives_DrawAALine16(pSurface, x1, y1, x2, y2, actual_line_color);

				AddDirtyTileToMix(prevPos);
				AddDirtyTileToMix(currPos);
			}
		}
	}

    old_line_color = actual_line_color; 
	
	
	
	goodPath = *g_selected_item->GetGoodPath();
    line_segment_count = -1;
	sint32 turn = 0;
	currMovementPoints=0.0;
	double prevMovementPoints=0.0;
	double maxMovementPoints=0.0;
	double count=0.0;
	isFirstMove;

	
    BOOL special_box_done = FALSE; 

            
    sel_army = Army(id);
    Assert(sel_army.m_id != (0)); 
    sel_army.CurMinMovementPoints(currMovementPoints); 
	count = currMovementPoints; 
	sel_army.MinMovementPoints(maxMovementPoints); 
	if (maxMovementPoints < 1.0)
		maxMovementPoints = -1.0;

	isFirstMove =  sel_army.GetFirstMoveThisTurn();
	
	if (currMovementPoints < 1.0)
	{
		currMovementPoints = -1;
		prevMovementPoints = 0;
		count = maxMovementPoints;
		isFirstMove = 1;
	}
	
	
	

	goodPath.Start(currPos);
	while (!goodPath.IsEnd())	{
		line_segment_count++; 
		prevPos = currPos;
		goodPath.Next(currPos);
			
		if (!g_player[owner]->IsExplored(currPos)) return; 
		
		
		double cost;
		if(sel_army.m_id != (0)) {
			if (sel_army.GetMovementTypeAir()) { 
				cost  = k_MOVE_AIR_COST; 
			} else if (((sel_army.IsAtLeastOneMoveShallowWater() ||
						 sel_army.IsAtLeastOneMoveWater())) &&
					   (!sel_army.IsAtLeastOneMoveLand())) { 
				if(g_theWorld->GetTerrain(currPos)->GetEnvBase()->GetMovement()) {
					sint32 icost;
					g_theWorld->GetTerrain(currPos)->GetEnvBase()->GetMovement(icost);
					cost=icost;
				}
			} else { 
				cost = g_theWorld->GetMoveCost(currPos);
			}
		} else {
			cost = g_theWorld->GetMoveCost(currPos);
		}

		uint16 turnColor = g_colorSet->GetColor(k_TURN_COLOR_GO);
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
			
			turnColor = g_colorSet->GetColor(k_TURN_COLOR_WAIT);
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
				
				isFirstMove = 0;
			}
			else 
				count -= cost;
		}
		
		sint32 countWasZero = 0;
		if ((-0.01 < count) && (count < 0.01))
		{
			drawPos = currPos;
			isFirstMove = 1;
			countWasZero = 1;
			count = -1;
		}
		
		if (count < 0)
		{
			count = maxMovementPoints;
			
			if (!countWasZero)
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
						
						MBCHAR turnNumber[80];						
                        if ((!special_box_done) && (special_line_segment == line_segment_count)) { 
                            special_box_done = TRUE; 
							sprintf(turnNumber,"*");
							actual_line_color = g_colorSet->GetColor(k_TURN_COLOR_SPECIAL);
							
                        } else { 
							sprintf(turnNumber,"%d",turn);
							actual_line_color = turnColor;
                        } 
						
						primitives_PaintRect16(pSurface,&turnRect,actual_line_color);
						primitives_FrameRect16(pSurface,&turnRect,0);

						sint32 width = textutils_GetWidth((aui_DirectSurface *)pSurface, turnNumber);
						sint32 height = textutils_GetHeight((aui_DirectSurface *)pSurface, turnNumber);
						
						sint32 textX = x - (width>>1);
						sint32 textY = y - (height>>1);
						
						primitives_DrawText((aui_DirectSurface *)pSurface, textX, textY, turnNumber, 0, 1);
						
						
                    } 
                } 
		} 
     }
     
	 

	 sint32 x, y; 
	 MapPoint drawPos; 
	 
	 badPath.Start(drawPos); 
	 if ((0 <= special_line_segment) && (!special_box_done) && !badPath.IsEnd()) { 
		 if (!goodPath.IsEnd()) { 
			 if (goodPath.Num() < 1) { 
				 badPath.Next(drawPos); 
			 } 
		 } else { 
			 badPath.Next(drawPos); 
		 } 
		 
		 if (TileIsVisible(drawPos.x,drawPos.y)) {
			 
			 if (!g_player[owner]->IsExplored(drawPos)) return; 
			 
			 maputils_MapXY2PixelXY(drawPos.x,drawPos.y,&x,&y);
			 
			 x += xoffset;
			 y += yoffset;
			 
			 sint32	boxEdgeSize = (sint32)((double)k_TURN_BOX_SIZE * m_scale);
			 if (boxEdgeSize < k_TURN_BOX_SIZE_MINIMUM) boxEdgeSize = k_TURN_BOX_SIZE_MINIMUM;
			 
			 RECT turnRect = {x - boxEdgeSize, 
				 y - boxEdgeSize, 
				 x + boxEdgeSize, 
				 y + boxEdgeSize};
			 
			 if (INSURFACE(turnRect.left, turnRect.top) && INSURFACE(turnRect.right, turnRect.bottom)) {
				 
				 MBCHAR turnNumber[80];						
				 sprintf(turnNumber,"*");
				 actual_line_color = g_colorSet->GetColor(k_TURN_COLOR_SPECIAL);
				 
				 special_box_done = TRUE; 
				 primitives_PaintRect16(pSurface,&turnRect,actual_line_color);
				 primitives_FrameRect16(pSurface,&turnRect,0);
				 
				 sint32 width = textutils_GetWidth((aui_DirectSurface *)pSurface, turnNumber);
				 sint32 height = textutils_GetHeight((aui_DirectSurface *)pSurface, turnNumber);
				 
				 sint32 textX = x - (width>>1);
				 sint32 textY = y - (height>>1);
				 
				 primitives_DrawText((aui_DirectSurface *)pSurface, textX, textY, turnNumber, 0, 1);
				 
			 }
		 }
	 }
	 
	
	
	

	
	
}

void TiledMap::DrawUnfinishedMove(
	aui_Surface *pSurface	
	)
{
    PLAYER_INDEX pIndex;
	ID id;
	SELECT_TYPE sType;
    g_selected_item->GetTopCurItem(pIndex,id,sType);
    if (sType != SELECT_TYPE_LOCAL_ARMY) { 
        return; 
    } 

	MapPoint currPos, prevPos;

	sint32 xoffset = (sint32)((k_TILE_PIXEL_WIDTH*m_scale)/2);
	sint32 yoffset = (sint32)(k_TILE_PIXEL_HEIGHT*m_scale);
    Army sel_army;

	

	double currMovementPoints = 0.0;
	sint32 isFirstMove = FALSE;	
	sint32 fuel = 0;
	sint32 halfFuel = -1;
  
    sel_army = Army(id);
    Assert(sel_army.m_id != (0)); 

	if ( !sel_army.GetOrder(0) ) return;
	if ( !sel_army.GetOrder(0)->m_path ) return;
	
	
	
	Path goodPath(sel_army.GetOrder(0)->m_path);

    sel_army.CurMinMovementPoints(currMovementPoints); 
	if (currMovementPoints < 1.0)
		currMovementPoints = -1.0;

	isFirstMove =  sel_army.GetFirstMoveThisTurn();

    sint32 num_tiles_to_half; 
    sint32 num_tiles_to_empty;

	
	
	
	

    sel_army.CalcRemainingFuel(num_tiles_to_half, num_tiles_to_empty);
    sint32 line_segement_count = 0; 

	sel_army.GetPos(currPos);
	MapPoint tempPos;
	goodPath.Start( tempPos );
	
	while ( tempPos != currPos && !goodPath.IsEnd()) {
		goodPath.Next(tempPos);
		line_segement_count++;
	}
	
	
	uint16 
		r = RGB (30, 0, 0), 
		y = RGB (30, 30, 0); 
	
	y = 0x01ff; 
	y = 0x3ff0; 
	y = 0xff40; 
	
	while (!goodPath.IsEnd()) 
	{
		prevPos = currPos;
		goodPath.Next(currPos);
		uint16 lineColor = g_colorSet->GetColor(k_TURN_COLOR_UNFINISHED);
		double old, cost;
		line_segement_count++;
		
		
		if ( !m_localVision->IsExplored(currPos) ) break;
		
		if ( sel_army.GetMovementTypeAir() ) {
			fuel--;
		}
		
		
		if (currMovementPoints > 0)
		{
			old = currMovementPoints;
			Assert(sel_army.m_id != (0)); 
			if (sel_army.GetMovementTypeAir()) { 
				cost = k_MOVE_AIR_COST; 
			} else if (((sel_army.IsAtLeastOneMoveShallowWater() ||
						 sel_army.IsAtLeastOneMoveWater())) &&
					   (!sel_army.IsAtLeastOneMoveLand())) { 
				if(g_theWorld->GetTerrain(currPos)->GetEnvBase()->GetMovement()) {
					sint32 icost;
					g_theWorld->GetTerrain(currPos)->GetEnvBase()->GetMovement(icost);
					cost=icost;
				}
			} else { 
				cost = g_theWorld->GetMoveCost(currPos);
			}
			
			
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
		
		if (currMovementPoints < 0)	{ 
			
			
            
			
		}
		else if (FEQUAL(currMovementPoints,0.0)) 
			currMovementPoints = -1;
		
		if (prevPos != currPos) 
		{
			sint32 x1, y1, x2, y2;
			
			
			if (TileIsVisible(prevPos.x, prevPos.y) &&
				TileIsVisible(currPos.x, currPos.y))
			{
				
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
	double count=0.0;
	
	
	
	if (sType == SELECT_TYPE_LOCAL_ARMY)
	{
		sel_army = Army(id);
		Assert(sel_army.m_id != (0)); 
		sel_army.CurMinMovementPoints(currMovementPoints); 
		count = currMovementPoints; 
		sel_army.MinMovementPoints(maxMovementPoints); 
		if (maxMovementPoints < 1.0)
			maxMovementPoints = -1.0;
		
		isFirstMove =  sel_army.GetFirstMoveThisTurn();
		
		if (currMovementPoints < 1.0)
		{
			currMovementPoints = -1;
			prevMovementPoints = 0;
			count = maxMovementPoints;
			isFirstMove = 1;
		}
	}
	
	
	sel_army.GetPos(currPos);
	goodPath.Start( tempPos );
	
	while ( tempPos != currPos && !goodPath.IsEnd()) {
		goodPath.Next(tempPos);
	}
	while (!goodPath.IsEnd())
	{
		prevPos = currPos;
		goodPath.Next(currPos);
		
		
		if ( !m_localVision->IsExplored(currPos) ) break;
		
		
		double cost;
		Assert(sel_army); 
		if (sel_army.GetMovementTypeAir()) { 
			cost  = k_MOVE_AIR_COST; 
		} else if (((sel_army.IsAtLeastOneMoveShallowWater() ||
					 sel_army.IsAtLeastOneMoveWater())) &&
				   (!sel_army.IsAtLeastOneMoveLand())) { 
			if(g_theWorld->GetTerrain(currPos)->GetEnvBase()->GetMovement()) {
				sint32 icost;
				g_theWorld->GetTerrain(currPos)->GetEnvBase()->GetMovement(icost);
				cost=icost;
			}
		} else { 
			cost = g_theWorld->GetMoveCost(currPos);
		}
		
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
				
				isFirstMove = 0;
			}
			else 
				count -= cost;
		}
		
		sint32 countWasZero = 0;
		if ((-0.01 < count) && (count < 0.01))
		{
			drawPos = currPos;
			isFirstMove = 1;
			countWasZero = 1;
			count = -1;
		}
		
		if (count < 0)
		{
			count = maxMovementPoints;
			
			if (!countWasZero)
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
						
						sint32 width = textutils_GetWidth((aui_DirectSurface *)pSurface, turnNumber);
						sint32 height = textutils_GetHeight((aui_DirectSurface *)pSurface, turnNumber);
						
						sint32 textX = x - (width>>1);
						sint32 textY = y - (height>>1);
						
						primitives_DrawText((aui_DirectSurface *)pSurface, textX, textY, turnNumber, color, 1);
						
						
					}
				}
		}
	}

	
	
	
}
