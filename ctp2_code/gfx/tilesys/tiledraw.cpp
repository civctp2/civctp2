//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Tile drawing.
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
// _DEBUG
// - Adds some aditional assertions to the code.
//
// _MSC_VER
// - Marked Microsoft specific assembler code
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Modification of DrawPartiallyConstructedImprovement's parameter list
//   to add a percent complete variable to allow more then three construction
//   per tile improvement by Martin Gühmann.
// - Allows now to use costumized graphics for ruins/huts by Martin Gühmann
// - Fixed CtD when drawing ruins or huts on an unseen cell.
// - City names and sizes are now drawn if game is in god mode or the fog
//   fog of war is toggled of. - Oct. 22nd 2004 Martin Gühmann
// - Tile improvements under construction with 100% completeness or more
//   are now drawn as complete tile improvements, this allows to retrieve them
//   from the unseen cell without the need to change the unseen cell data 
//   structure. - Dec. 21st 2004 Martin Gühmann
// - Current terrain improvements are displayed instead of those from the
//   last visit if the fog of war is toggled off. - Dec 24th 2004 - Martin Gühmann
// - Road like tile improvements are drawn according to the real state
//   instead to the state from the last visit. - Dec. 25th 2004 - Martin Gühmann
// - Draws the city radius from the last visit instead the current one.
//   - Dec. 26th 2004 Martin Gühmann
// - Positions that you own and are explored are now shown as yours.
//   - Mar. 4th 2005 Martin Gühmann
// - If fog of war is off or god mode is on all borders are now shown. Even 
//   those of the civs you have no contact to. - Mar. 4th 2005 Martin Gühmann
// - Added option to draw wonders on top of roads.
// - PFT 29 mar 05, show # turns until city next grows a pop.
// - Removed .NET warnings. - April 23rd 2005 Martin Gühmann
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Display non-growth as "---"
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_directsurface.h"
#include "aui_bitmapfont.h"
#include "pixelutils.h"
#include "primitives.h"
#include "background.h"
#include "pointerlist.h"
#include "dynarr.h"
#include "SelItem.h"                // g_selected_item
#include "player.h"
#include "World.h"                  // g_theWorld
#include "Cell.h"
#include "Unit.h"
#include "TerrImprove.h"
#include "TerrImproveData.h"
#include "tiledmap.h"
#include "TileInfo.h"
#include "tileset.h"
#include "BaseTile.h"
#include "colorset.h"               // g_colorSet
#include "maputils.h"
#include "textutils.h"
#include "citydata.h"
#include "UnitData.h"
#include "TileDrawRoad.h"
#include "director.h"
#include "screenmanager.h"
#include "UnitActor.h"
#include "UnseenCell.h"
#include "profileDB.h"              // g_theProfileDB
#include "network.h"
#include "chatlist.h"
#include "pointerlist.h"
#include "StrDB.h"
#include "terrainutil.h"
#include "TerrainRecord.h"
#include "TerrImprove.h"
#include "TerrainImprovementRecord.h"
#include "TerrImprovePool.h"
#include "Civilisation.h"

extern Background	*g_background;
extern ScreenManager *g_screenManager;

extern sint32		g_fog_toggle;
extern sint32		g_god;

static COLOR		g_curSelectColor = COLOR_SELECT_0;
static sint32		g_nano_flash = 0;
static sint32		g_bio_flash = 0;

#define k_POP_BOX_SIZE_MINIMUM	10

namespace
{   
    // Do not display digits when the number of turns till next pop is too large
    int const  THRESHOLD_SLOW_GROWTH    = 100;      // limit to 2 digits
    char const TEXT_NONE []             = "---";

    // Simple shorthand functions
    Pixel16 GetColor(COLOR const & a_ColorName, bool a_IsFogged = false)
    {
        if (a_IsFogged)
        {
            return g_colorSet->GetDarkColor(a_ColorName);
        }
        else
        {
            return g_colorSet->GetColor(a_ColorName);
        }
    }

    COLORREF GetColorRef(COLOR const & a_ColorName, bool a_IsFogged = false)
    {
        if (a_IsFogged)
        {
            return g_colorSet->GetDarkColorRef(a_ColorName);
        }
        else
        {
            return g_colorSet->GetColorRef(a_ColorName);
        }
    }
}

#ifdef _DEBUG

void  TiledMap::DrawRectMetrics()
{  
	return;
#if 0 
	if(num_loops<1.0)
	   num_loops=1.0;

	double ratio=num_rects/num_loops;

	MBCHAR  text[256];

	sprintf(text,"%4.2f",ratio);

	
	if (g_screenManager && g_screenManager->GetSurface()) 
	{
		if (m_font) 
		{
			sint32 width, height;

			width = m_font->GetStringWidth(text);
			height = m_font->GetMaxHeight();

			RECT		tempRect = {0, 0, width, height};
			OffsetRect(&tempRect, 200, 200);

			aui_Surface *tempSurf = g_screenManager->GetSurface();
			g_screenManager->UnlockSurface();

			m_font->DrawString(tempSurf, &tempRect, &tempRect, text, 0, GetColorRef(COLOR_YELLOW), 0);
			OffsetRect(&tempRect, -1, -1);
			m_font->DrawString(tempSurf, &tempRect, &tempRect, text, 0, GetColorRef(COLOR_PURPLE), 0);

			tempRect.right++;
			tempRect.bottom++;

			g_screenManager->LockSurface(tempSurf);

			AddDirtyRectToMix(tempRect);
		}
  }
#endif

}


void  TiledMap::ClearRectMetrics()
{
   num_loops = 1.0;
   num_rects = 0.0;
}

void  TiledMap::IncRectMetric()
{
   num_rects += 1.0;
}

void  TiledMap::RectMetricNewLoop()
{
   num_loops += 1.0;
  
}

#endif // _DEBUG


bool TiledMap::DrawImprovementsLayer(aui_Surface *surface, MapPoint &pos, sint32 x, sint32 y,bool clip)
{
	
	
	
	uint32				env;
	Cell				*cell = NULL;
	BOOL				isAirfield,
						isListeningPost,
						isRadar,
						isHealUnits,
						isFort;
	Pixel16				*data = NULL;
	BOOL				hasHut;


	bool drewSomething = false;

	if(!m_localVision->IsExplored(pos)) 
		return drewSomething;

	if(!ReadyToDraw()) 
		return drewSomething;

	
	bool	  fog                   = 
          m_localVision && m_localVision->IsExplored(pos) && !m_localVision->IsVisible(pos);

	bool    visiblePlayerOwnsThis = 
	    (g_selected_item->GetVisiblePlayer() == g_theWorld->GetOwner(pos)); 

	std::vector<Pixel16 *>	drawOnTop;	// things above road level

	UnseenCellCarton	ucell;
	if(!g_fog_toggle // Draw the right stuff if fog of war is off
	&& !visiblePlayerOwnsThis
	&&  m_localVision->GetLastSeen(pos, ucell)
	)
	{
		env = ucell.m_unseenCell->GetEnv();

		isAirfield		= ucell.m_unseenCell->IsAirfield();
		isListeningPost	= ucell.m_unseenCell->IsListeningPost();
		isRadar			= ucell.m_unseenCell->IsRadar();
		isHealUnits		= ucell.m_unseenCell->IsHealUnits();
		isFort			= ucell.m_unseenCell->IsFort();
		hasHut			= ucell.m_unseenCell->HasHut();

	} 
	else 
	{
		hasHut		= (g_theWorld->GetGoodyHut(pos) != NULL);
		cell = g_theWorld->GetCell(pos);
		
		if (cell)
		{
			env = cell->GetEnv();

			sint32 i,numImprovements=cell->GetNumDBImprovements();
			sint32 index,impType;
	 		const TerrainImprovementRecord *rec;
			const TerrainImprovementRecord::Effect *effect;
		
			
			for(i=0;i<numImprovements;i++)
			{
				impType = cell->GetDBImprovement(i);
				rec     = g_theTerrainImprovementDB->Get(impType);
			
				
				
				if (rec==NULL)
					continue;

				if(rec->GetClassRoad() || rec->GetClassOceanRoad()) {
					
					continue;
				}

				effect  = terrainutil_GetTerrainEffect(rec, pos);
			
				
				
				if (effect==NULL)
					continue;

				
				index   = effect->GetTilesetIndex();

				
				data    = m_tileSet->GetImprovementData((uint16)index);
				
				
				if (rec->GetDisplayLevel() > 0)
				{
					// Postpone display
					drawOnTop.push_back(data);
				}
				else
				{
					DrawAnImprovement(surface, data, x, y, fog);
                              drewSomething = true;
				}
			}
		}
	}


	
// Added by Martin Gühmann
// Do not forget fog of war
	if (m_localVision->GetLastSeen(pos, ucell) && !g_fog_toggle)
	{
		env = ucell.m_unseenCell->GetEnv();
	}

	
	
	if (g_theWorld->EnvIsRoad(k_ROAD_TYPE_1, env)) {
		DrawRoads(surface,pos,x,y,k_ROAD_TYPE_1,k_ROAD_OFFSET_1, fog);
		drewSomething = true;
	}

	if (g_theWorld->EnvIsRoad(k_ROAD_TYPE_2, env)) {
		DrawRoads(surface,pos,x,y,k_ROAD_TYPE_2,k_ROAD_OFFSET_2, fog);
		drewSomething = true;
	}

	if (g_theWorld->EnvIsRoad(k_ROAD_TYPE_3, env)) 
	{
		
		DrawRoads(surface,pos,x,y,k_ROAD_TYPE_3,k_ROAD_OFFSET_3, fog, k_OVERLAY_FLAG_SHADOWSONLY);
		DrawRoads(surface,pos,x,y,k_ROAD_TYPE_3,k_ROAD_OFFSET_3, fog, k_OVERLAY_FLAG_NOSHADOWS);
		drewSomething = true;
	}

	if(g_theWorld->EnvIsTunnel(env))
	{
		DrawRoads(surface, pos, x, y, k_ROAD_TYPE_4, k_ROAD_OFFSET_4, fog);
		drewSomething = true;
	}





	if (hasHut) 
	{
		
		
		
		// Modified by Martin Gühmann to allow modders to customize the graphics of ruins/huts.
		sint32 const			terrain = 
			(cell) ? cell->GetTerrain() : ucell.m_unseenCell->GetTerrainType();
		const TerrainRecord *	rec		= g_theTerrainDB->Get(terrain);
		if (pos.x&1) 
			data = m_tileSet->GetImprovementData((uint16)rec->GetHutTilesetIndexA());
		else 
			data = m_tileSet->GetImprovementData((uint16)rec->GetHutTilesetIndexB());
		DrawAnImprovement(surface,data,x,y,fog);
		drewSomething = true;
	}

	
	if (cell) 
	{
		
		for (sint32 i=0; i<cell->GetNumImprovements(); i++) 
		{
			if(!g_theTerrainImprovementPool->IsValid(cell->AccessImprovement(i)))
				continue;

			sint32 percent = cell->AccessImprovement(i).GetData()->PercentComplete();
			uint16 index;

			if (percent < 5)
				index = 0;
			else 
			  if (percent < 100)
				  index = 1;
			else 
				break;
			sint32 type = cell->AccessImprovement(i).GetData()->GetType();
			DrawPartiallyConstructedImprovement(surface, env, type, x, y, index, fog, percent);//percent added by Martin Gühmann
			drewSomething = true;
		}
	} 
	else 
	{
		
		PointerList<UnseenImprovementInfo> *improvements = ucell.m_unseenCell->GetImprovements();

		PointerList<UnseenImprovementInfo>::Walker *walker = 
			new PointerList<UnseenImprovementInfo>::Walker(improvements);

		while (walker->IsValid()) 
		{
			sint32 type		= walker->GetObj()->m_type;
			sint32 percent	= walker->GetObj()->m_percentComplete;
			uint16 index;


// Added by Martin Gühmann
			if (percent < 50){
				index = 0;
				DrawPartiallyConstructedImprovement(surface, env, type, x, y, index, fog, percent);//percent added by Martin Gühmann
			}
			else if (percent < 100){
				index = 1;
				DrawPartiallyConstructedImprovement(surface, env, type, x, y, index, fog, percent);//percent added by Martin Gühmann
			}
			else{

		 		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(type);
				const TerrainImprovementRecord::Effect *effect = 0;
				if(rec!=NULL)
					effect = terrainutil_GetTerrainEffect(rec, pos);

				if(rec!=NULL && effect!=NULL){
					if(!rec->GetClassRoad() && !rec->GetClassOceanRoad()){
						data = m_tileSet->GetImprovementData(static_cast<uint16>(effect->GetTilesetIndex()));
						DrawAnImprovement(surface,data,x,y,fog);
					}
				}
				else{
					index = 2;
					DrawPartiallyConstructedImprovement(surface, env, type, x, y, index, fog, percent);//percent added by Martin Gühmann
				}
			}
			drewSomething = true;

			walker->Next();
		}
		
		delete walker;
	}

    // Put the special items on top
	for
	(
		std::vector<Pixel16 *>::iterator	p	= drawOnTop.begin();
		p != drawOnTop.end();
		++p
	)
	{
            DrawAnImprovement(surface, *p, x, y, fog);
            drewSomething = true;
	}

	return drewSomething;
}

void TiledMap::DrawPartiallyConstructedImprovement(aui_Surface *surface, uint32 env, 
												   sint32 type, sint32 x, sint32 y, 
												   uint16 index, BOOL fog, sint32 percentComplete)
												   //Added sint32 percentComplete by Martin Gühmann
{
	


	Pixel16		*data = NULL;

#if 0
	switch (type) 
	{
	case TERRAIN_IMPROVEMENT_IRRIGATION_1:
		if (g_theWorld->EnvIsWater(env)) {
			switch (index) {
			case 0:
				data = m_tileSet->GetImprovementData(50);
				break;
			case 1:
				data = m_tileSet->GetImprovementData(61);
				break;
			case 2:
				data = m_tileSet->GetImprovementData(62);
				break;
			default:
				Assert(0);
			}
		} 
		else 
		if (g_theWorld->EnvIsSpace(env)) 
		{
			data = m_tileSet->GetImprovementData(111+index);
		} 
		else 
		{
			data = m_tileSet->GetImprovementData(3+index);
		}

		break;
	case TERRAIN_IMPROVEMENT_IRRIGATION_2:
		if (g_theWorld->EnvIsWater(env)) 
		{
			switch (index) 
			{
			case 0:
			case 1:
				data = m_tileSet->GetImprovementData(50);
				break;
			case 2:
				data = m_tileSet->GetImprovementData(64);
				break;
			}
		} 
		else 
		if (g_theWorld->EnvIsSpace(env)) 
		{
			data = m_tileSet->GetImprovementData(114+index);
		} 
		else 
		{
			data = m_tileSet->GetImprovementData(6+index);
		}

		break;
	case TERRAIN_IMPROVEMENT_IRRIGATION_3:
		if (g_theWorld->EnvIsWater(env)) {
			switch (index) {
			case 0:
			case 1:
				data = m_tileSet->GetImprovementData(50);
				break;
			case 2:
				data = m_tileSet->GetImprovementData(67);
				break;
			}
		} else 
		if (g_theWorld->EnvIsSpace(env)) {
			data = m_tileSet->GetImprovementData(117+index);
		} else {
			data = m_tileSet->GetImprovementData(9+index);
		}
		break;
	case TERRAIN_IMPROVEMENT_MINE_1:
		if (g_theWorld->EnvIsWater(env)) {
			data = m_tileSet->GetImprovementData(52+index);
		} else 
		if (g_theWorld->EnvIsSpace(env)) {
			data = m_tileSet->GetImprovementData(102+index);
		} else {
			data = m_tileSet->GetImprovementData(12+index);
		}
		break;
	case TERRAIN_IMPROVEMENT_MINE_2:
		if (g_theWorld->EnvIsWater(env)) 
		{
			data = m_tileSet->GetImprovementData(55+index);
		} 
		else 
			if (g_theWorld->EnvIsSpace(env)) 
			{
				data = m_tileSet->GetImprovementData(105+index);
			} 
			else 
			{
				data = m_tileSet->GetImprovementData(15+index);
			}
		break;
	case TERRAIN_IMPROVEMENT_MINE_3:
		if (g_theWorld->EnvIsWater(env)) {
			data = m_tileSet->GetImprovementData(58+index);
		} else 
		if (g_theWorld->EnvIsSpace(env)) {
			data = m_tileSet->GetImprovementData(108+index);
		} else {
			data = m_tileSet->GetImprovementData(18+index);
		}
		break;
	default:
		
		if (g_theWorld->EnvIsWater(env)) {
			data = m_tileSet->GetImprovementData(50);
		} else 
		if (g_theWorld->EnvIsSpace(env)) {
			data = m_tileSet->GetImprovementData(100);
		} else {
			data = m_tileSet->GetImprovementData(1);
		}
		break;
	}

#endif
	const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(type);
	if (rec->GetNumConstructionTiles() < 1) 
	{
		data = m_tileSet->GetImprovementData(1);
	}
	else 
	{
		if (percentComplete >= 100)
		{
			data = m_tileSet->GetImprovementData((uint16) rec->GetConstructionTiles(rec->GetNumConstructionTiles() - 1));
		}
		else
		{
			sint32 const	ctIndex = (rec->GetNumConstructionTiles() * percentComplete) / 100;
			if (ctIndex < rec->GetNumConstructionTiles())
			{
				data = m_tileSet->GetImprovementData((uint16) rec->GetConstructionTiles(ctIndex));
			}
			else
			{
				//No idea why the above is not enough, but this fixes the last assert I got when I place terraform improvements with the editor.
				data = m_tileSet->GetImprovementData((uint16) rec->GetConstructionTiles(rec->GetNumConstructionTiles() - 1));
			}
		}
	}

	if(!data)
		return;

	if (fog) {
		if (m_zoomLevel != k_ZOOM_NORMAL) {
			DrawBlendedOverlayScaled(surface, data, x, y, 
				GetZoomTilePixelWidth(), 
				GetZoomTileGridHeight(), 
				k_FOW_COLOR,
				k_FOW_BLEND_VALUE);
		} else {
			DrawBlendedOverlay(surface, data, x, y, k_FOW_COLOR, k_FOW_BLEND_VALUE);
		}
	} else {
		if (m_zoomLevel != k_ZOOM_NORMAL) {
				DrawScaledOverlay(surface, data, x, y, 
					GetZoomTilePixelWidth(), 
					GetZoomTileGridHeight());
		} else {
			DrawOverlay(surface, data, x, y);
		}
	}

}

void TiledMap::DrawHitMask(aui_Surface *surf, const MapPoint &pos)
{
	sint32		x, y;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);

	sint32 width = GetZoomTilePixelWidth();
	sint32 height = GetZoomTilePixelHeight();

	y += (sint32) ((double)k_TILE_PIXEL_HEADROOM * m_scale);

	if (x < 0) return;
	if (x >= surf->Width()-width) return;
	if (y < 0) return;
	if (y >= surf->Height() - height) return;

	AddDirtyToMix(x, y, width, height);

	uint8		*pSurfBase;
	sint32 errcode = surf->Lock(NULL, (LPVOID *)&pSurfBase, 0);
	if ( errcode != AUI_ERRCODE_OK ) return;

	
	sint32 surfPitch = surf->Pitch();

	
	uint16 *pDestPixel;

	sint32 num = k_TILE_GRID_HEIGHT - k_TILE_PIXEL_HEADROOM;
	sint32 den = height;
	sint32 tot = num;

	sint32 row = 0;

    extern BOOL g_killMode;
	Pixel16 selectColorPixel = GetColor(g_killMode ? COLOR_RED : g_curSelectColor);
	
	for (sint32 i = k_TILE_PIXEL_HEADROOM; i < k_TILE_GRID_HEIGHT;) 
	{
		sint32 start = m_tileHitMask[i].start;
		sint32 end   = m_tileHitMask[i].end;

		while (tot >= den) 
		{
			i++;
			tot -= den;
		}
		tot += num;

		pDestPixel = (Pixel16 *)(pSurfBase + ((y+row) * surfPitch) + ((x+start) << 1));
		*pDestPixel++ = selectColorPixel;
		*pDestPixel = selectColorPixel;

		pDestPixel += (end-start-2);

		*pDestPixel++ = selectColorPixel;
		*pDestPixel = selectColorPixel;

		row++;

	}

	g_curSelectColor = (COLOR)(g_curSelectColor + 1);
	if (g_curSelectColor > COLOR_SELECT_2) g_curSelectColor = COLOR_SELECT_0;
	
	errcode = surf->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);
}

void TiledMap::DrawColoredHitMask(aui_Surface *surf, const MapPoint &pos, COLOR color)
{
	sint32		x, y;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);

	sint32 width = GetZoomTilePixelWidth();
	sint32 height = GetZoomTilePixelHeight();

	y += (sint32) ((double)k_TILE_PIXEL_HEADROOM * m_scale);

	if (x < 0) return;
	if (x >= surf->Width()-width) return;
	if (y < 0) return;
	if (y >= surf->Height() - height) return;

	AddDirtyToMix(x, y, width, height);

	uint8		*surfBase = g_screenManager->GetSurfBase();
	sint32	 surfPitch = g_screenManager->GetSurfPitch();

	
	uint16 *pDestPixel;

	sint32 num = k_TILE_GRID_HEIGHT - k_TILE_PIXEL_HEADROOM;
	sint32 den = height;
	sint32 tot = num;

	sint32 row = 0;
	Pixel16		selectColorPixel = GetColor(color);

	for (sint32 i = k_TILE_PIXEL_HEADROOM; i<k_TILE_GRID_HEIGHT;) 
	{
#if 0
		start = (sint32) ((double)m_tileHitMask[i].start * m_scale);
		end = (sint32)((double)m_tileHitMask[i].end * m_scale);
#endif
		sint32 start = m_tileHitMask[i].start;
		sint32 end   = m_tileHitMask[i].end  ;


		while (tot >= den) {
			i++;
			tot -= den;
		}
		tot += num;

		pDestPixel = (Pixel16 *)(surfBase + ((y+row) * surfPitch) + ((x+start) << 1));
		*pDestPixel = selectColorPixel;
		pDestPixel += (end-start);
		*pDestPixel = selectColorPixel;

		row++;
	}

}

void TiledMap::DrawHitMask(aui_Surface *surf, const MapPoint &pos, RECT *mapViewRect, RECT *destRect)
{
	sint32		x, y;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y, mapViewRect);

	sint32 width = GetZoomTilePixelWidth();
	sint32 height = GetZoomTilePixelHeight();

	y += (sint32) ((double)k_TILE_PIXEL_HEADROOM * m_scale);

	x += destRect->left;
	y += destRect->top;

	if (x < 0) return;
	if (x >= surf->Width()-width) return;
	if (y < 0) return;
	if (y >= surf->Height() - height) return;

	AddDirtyToMix(x, y, width, height);

	uint8		*pSurfBase;
	sint32		errcode = surf->Lock(NULL, (LPVOID *)&pSurfBase, 0);
	if ( errcode != AUI_ERRCODE_OK ) return;

	
	sint32 surfPitch = surf->Pitch();

	
	uint16 *pDestPixel;

	sint32 num = k_TILE_GRID_HEIGHT - k_TILE_PIXEL_HEADROOM;
	sint32 den = height;
	sint32 tot = num;

	sint32 row = 0;
	Pixel16		selectColorPixel = GetColor(g_curSelectColor);

	for (sint32 i = k_TILE_PIXEL_HEADROOM; i < k_TILE_GRID_HEIGHT;) 
	{
#if 0
		start = (sint32) ((double)m_tileHitMask[i].start * m_scale);
		end   = (sint32)((double)m_tileHitMask[i].end * m_scale);
#endif
		sint32 start = m_tileHitMask[i].start;
		sint32 end   = m_tileHitMask[i].end  ;

		while (tot >= den) 
		{
			i++;
			tot -= den;
		}
		tot += num;

		pDestPixel = (Pixel16 *)(pSurfBase + ((y+row) * surfPitch) + ((x+start) << 1));
		*pDestPixel = selectColorPixel;
		pDestPixel += (end-start);
		*pDestPixel = selectColorPixel;

		row++;
	}
	
	errcode = surf->Unlock((LPVOID)pSurfBase);
	Assert(errcode == AUI_ERRCODE_OK);

	g_curSelectColor = (COLOR)(g_curSelectColor + 1);
	
	if (g_curSelectColor > COLOR_SELECT_3) 
		g_curSelectColor = COLOR_SELECT_0;
}


void TiledMap::DrawColoredHitMaskEdge(aui_Surface *surf, const MapPoint &pos, Pixel16 selectColorPixel, WORLD_DIRECTION side)
{
	sint32		x, y;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);

	sint32 width = GetZoomTilePixelWidth();
	sint32 height = GetZoomTilePixelHeight();

	y += (sint32) ((double)k_TILE_PIXEL_HEADROOM * m_scale);

	if (x < 0) return;
	if (x >= surf->Width()-width) return;
	if (y < 0) return;
	if (y >= surf->Height() - height) return;

	AddDirtyToMix(x, y, width, height);

	uint8	* surfBase = g_screenManager->GetSurfBase();
	sint32  surfPitch = g_screenManager->GetSurfPitch();

	
	uint16 *pDestPixel;

	sint32 num = k_TILE_GRID_HEIGHT - k_TILE_PIXEL_HEADROOM;
	sint32 den = height;
	sint32 tot = num;


	sint32 startI, endI;
	if(side == NORTHWEST || side == NORTHEAST) {
		startI = k_TILE_PIXEL_HEADROOM;
		endI = k_TILE_PIXEL_HEADROOM + (k_TILE_GRID_HEIGHT / 2);
	} else {
		startI = k_TILE_PIXEL_HEADROOM + (k_TILE_GRID_HEIGHT / 2);
		endI = k_TILE_GRID_HEIGHT;
	}

	bool west = (side == NORTHWEST) || (side == SOUTHWEST);
	bool north = (side == NORTHWEST) || (side == NORTHEAST);

	sint32 row = 0;
	for (sint32 i = k_TILE_PIXEL_HEADROOM; i < k_TILE_GRID_HEIGHT;) 
	{
		if(north && i >= (k_TILE_PIXEL_HEADROOM + k_TILE_GRID_HEIGHT) / 2)
			break;

#if 0
		start = (sint32) ((double)m_tileHitMask[i].start * m_scale);
		end = (sint32)((double)m_tileHitMask[i].end * m_scale);
#endif
		sint32 start = m_tileHitMask[i].start;
		sint32 end   = m_tileHitMask[i].end  ;


		while (tot >= den) {
			i++;
			tot -= den;
		}
		tot += num;

		if(!north && i < (k_TILE_PIXEL_HEADROOM + k_TILE_GRID_HEIGHT) / 2) {
			row++;
			continue;
		}

		pDestPixel = (Pixel16 *)(surfBase + ((y+row) * surfPitch) + ((x+start) << 1));
		if(west) {
			*pDestPixel = selectColorPixel;
			*(pDestPixel + 1) = selectColorPixel;
		}
		pDestPixel += (end-start);

		if(!west) {
			*pDestPixel = selectColorPixel;
			*(pDestPixel - 1) = selectColorPixel;
		}

		row++;
	}

}

void TiledMap::DrawColoredBorderEdge(aui_Surface *surf, const MapPoint &pos, Pixel16 selectColorPixel, WORLD_DIRECTION side, sint32 dashMode)
{
	sint32		x, y;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);

	sint32 width = GetZoomTilePixelWidth();  // changing here got rid of the border
	sint32 height = GetZoomTilePixelHeight();

	y += (sint32) ((double)k_TILE_PIXEL_HEADROOM * m_scale);

	if (x < 0) return;
	if (x >= surf->Width()-width) return;
	if (y < 0) return;
	if (y >= surf->Height() - height) return;

	AddDirtyToMix(x, y, width, height);

	uint8	* surfBase = m_surfBase; 
	sint32  surfPitch = m_surfPitch; 

	
	uint16 *pDestPixel;

	sint32 num = k_TILE_GRID_HEIGHT - k_TILE_PIXEL_HEADROOM;
	sint32 den = height;
	sint32 tot = num;

	sint32 row = 0;

	sint32 startI, endI;
	if(side == NORTHWEST || side == NORTHEAST) {
		startI = k_TILE_PIXEL_HEADROOM;  // E - pixel headroom is the space on the tga above the square tileset.h has these values
		endI = k_TILE_PIXEL_HEADROOM + (k_TILE_GRID_HEIGHT / 2);
	} else {
		startI = k_TILE_PIXEL_HEADROOM + (k_TILE_GRID_HEIGHT / 2);
		endI = k_TILE_GRID_HEIGHT;
	}

	bool west = (side == NORTHWEST) || (side == SOUTHWEST);
	bool north = (side == NORTHWEST) || (side == NORTHEAST);

	for (sint32 i = k_TILE_PIXEL_HEADROOM; i < k_TILE_GRID_HEIGHT;) 
	{
		if(north && i >= (k_TILE_PIXEL_HEADROOM + k_TILE_GRID_HEIGHT) / 2)
			break;

#if 0
		start = (sint32) ((double)m_tileHitMask[i].start * m_scale);
		end = (sint32)((double)m_tileHitMask[i].end * m_scale);
#endif
		sint32 start = m_tileHitMask[i].start;
		sint32 end   = m_tileHitMask[i].end  ;


		while (tot >= den) {
			i++;
			tot -= den;
		}
		tot += num;

		if(!north && i < (k_TILE_PIXEL_HEADROOM + k_TILE_GRID_HEIGHT) / 2) {
			row++;
			continue;
		}

		if(dashMode && (row & 0x2)) {
			row++;
			continue;
		}

		pDestPixel = (Pixel16 *)(surfBase + ((y+row) * surfPitch) + ((x+start) << 1)); //EMOD change here
		if(west) {
			*pDestPixel = selectColorPixel;
			*(pDestPixel + 1) = selectColorPixel;
			*(pDestPixel + 2) = selectColorPixel;
		}
		pDestPixel += (end-start);

		if(!west) {
			*pDestPixel = 0; selectColorPixel;
			*(pDestPixel - 1) = selectColorPixel;
			*(pDestPixel - 2) = selectColorPixel;
		}

		row++;
	}

}
void TiledMap::DrawPath(Path *path)
{
    MapPoint	pos; 

    for (path->Start(pos); !path->IsEnd(); path->Next(pos)) { 
		sint32		x,y;
		maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);

		RECT rect = {x-2, y-2, x+2, y+2};

		primitives_PaintRect16(g_background->TheSurface(), &rect, 0xFFFF);

		AddDirtyRectToMix(rect);
	}
}

sint32 TiledMap::QuickBlackBackGround(aui_Surface *surface)
{
	DDBLTFX ddbltfx;
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = 0;

	if (!surface) surface = m_surface;

	
	LPDIRECTDRAWSURFACE	lpdds = ((aui_DirectSurface *)surface)->DDS();
	
	
	AUI_ERRCODE errcode = static_cast<AUI_ERRCODE>
        (lpdds->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&ddbltfx));

	Assert(errcode == AUI_ERRCODE_OK);
	return (AUI_ERRCODE_OK == errcode) ? AUI_ERRCODE_OK : AUI_ERRCODE_BLTFAILED;
}

sint32 TiledMap::DrawBlackTile(aui_Surface *surface, sint32 x, sint32 y) //EMOD this is for unexplored? could add bracket like icons that make it more like civ3
{

	if (!surface) surface = m_surface;

	uint8	*surfBase = m_surfBase;
	sint32 surfPitch = m_surfPitch;

	unsigned short	*destPixel;

	y+=k_TILE_PIXEL_HEADROOM;

	if (x < 0) return 0;
	if (x >= surface->Width() - k_TILE_PIXEL_WIDTH) return 0;
	if (y < 0) return 0;
	if (y >= surface->Height() - k_TILE_PIXEL_HEIGHT) return 0;

	sint32 startX, endX;

	
	for(sint32 j=0; j<k_TILE_PIXEL_HEIGHT; j++) {
		if (j<=23) {
			startX = (23-j)*2;
		} else {
			startX = (j-24)*2;
		}
		endX = k_TILE_PIXEL_WIDTH - startX;

		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + ((x+startX) * 2));

		for (sint32 i=startX; i<endX; i++) {
			*destPixel++ = 0x0000;
		}
	}

	return 0;
}







sint32 TiledMap::DrawDitheredTile(aui_Surface *surface, sint32 x, sint32 y, Pixel16 color)
{
	if (!surface) surface = m_surface;

	uint8	* surfBase = m_surfBase;
	sint32 surfPitch = m_surfPitch;

	unsigned short	*destPixel;

	y+=k_TILE_PIXEL_HEADROOM;


if (x < 0) return 0;
if (x >= surface->Width() - k_TILE_PIXEL_WIDTH) return 0;
if (y < 0) return 0;
if (y >= surface->Height() - k_TILE_PIXEL_HEIGHT) return 0;

	sint32 startX, endX;
	sint32 offset = 0;

	
	for(sint32 j=0; j<k_TILE_PIXEL_HEIGHT; j++) {
		if (j<=23) {
			startX = (23-j)*2;
		} else {
			startX = (j-24)*2;
		}
		endX = k_TILE_PIXEL_WIDTH - startX;
		if (j & 0x01)
			offset = 1;
		else
			offset = 0;
		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + ((x+startX) * 2));
		destPixel+=offset;
		for (sint32 i=startX; i<endX; i+=2) {
				*destPixel = color;
				destPixel+=2;
		}
	}

	return 0;
}







void TiledMap::DrawDitheredTileScaled(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,Pixel16 color)
{
	sint32		startX, endX;
	uint8		*pSurfBase;
	AUI_ERRCODE	errcode;

	BOOL		wasUnlocked = FALSE;

	sint32 surfWidth;
	sint32 surfHeight;
	sint32 surfPitch;

	if (!surface) {
		surface = m_surface;

		pSurfBase = m_surfBase;
		surfWidth = m_surfWidth;
		surfHeight = m_surfHeight;
		surfPitch = m_surfPitch;
	} else {

		errcode = surface->Lock(NULL, (LPVOID *)&pSurfBase, 0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;

		surfWidth = surface->Width();
		surfHeight = surface->Height();
		surfPitch = surface->Pitch();

		wasUnlocked = TRUE;
	}


	y+=(sint32)((double)k_TILE_PIXEL_HEADROOM * m_scale);
	

if (x < 0) goto Exit;
if (x >= surface->Width() - destWidth) goto Exit;
if (y < 0) goto Exit;
if (y >= surface->Height() - destHeight) goto Exit;

	
	uint16 *pDestPixel;

	
	sint32				vaccum;
	sint32				vincx, vincxy;
	sint32				vend;
	sint32				vdestpos;
	sint32				vpos1, vpos2;

	vaccum = destHeight*2 - k_TILE_PIXEL_HEIGHT;
	vincx = destHeight*2;
	vincxy = (destHeight - k_TILE_PIXEL_HEIGHT) * 2 ;

	vpos1 = 0;
	vpos2 = (sint32)((double)(k_TILE_PIXEL_HEIGHT - destHeight) / (double)destHeight);

	vdestpos = y;
	vend = k_TILE_PIXEL_HEIGHT - 1;

	
	while ( vpos1 < vend) {
		sint32 offset = 0;
		if (vpos1 & 0x01)
			offset = 1;

		if (vaccum < 0) {
			vaccum += vincx;

			if (vpos1<=23) {
				startX = (23-vpos1)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			} else {
				startX = (vpos1-24)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			}
		} else {
			
			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			

			haccum = destWidth * 2 - k_TILE_PIXEL_WIDTH;
			hincx = destWidth * 2;
			hincxy = (destWidth - k_TILE_PIXEL_WIDTH) * 2;
			
			hpos = 0;
			hend = k_TILE_PIXEL_WIDTH - 1;

			
			if (vpos1<=23) {
				startX = (23-vpos1)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			} else {
				startX = (vpos1-24)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			}

			hdestpos = x + (sint32)((double)startX * m_scale);

			hpos = startX;
			hend = endX;

			while (hpos < hend) {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					pDestPixel = (Pixel16 *)(pSurfBase + (vdestpos * surfPitch) + (hdestpos << 1));
					pDestPixel += offset;
					*pDestPixel = color;

					hdestpos+=2;
				}
				hpos+=2;
			}
			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}

Exit:

	if (wasUnlocked) {
		
		errcode = surface->Unlock((LPVOID)pSurfBase);
		Assert(errcode == AUI_ERRCODE_OK);
	}
}







sint32 TiledMap::DrawBlendedTile(aui_Surface *surface, const MapPoint &pos,sint32 xpos, sint32 ypos, Pixel16 color, sint32 blend)
{
	if (!surface) surface = m_surface;

ypos+=k_TILE_PIXEL_HEADROOM;


if (xpos < 0) return 0;
if (xpos >= surface->Width() - k_TILE_PIXEL_WIDTH) return 0;
if (ypos < 0) return 0;
if (ypos >= surface->Height() - k_TILE_PIXEL_HEIGHT) return 0;

	TileInfo * tileInfo = GetTileInfo(pos);
	Assert(tileInfo);
	if (tileInfo == NULL) return 0;

	uint16 index = tileInfo->GetTileNum();

	BaseTile * baseTile = m_tileSet->GetBaseTile(index); 

	if (baseTile == NULL) return 0;



	Pixel16 *data = baseTile->GetTileData();

	sint32 tilesetIndex = g_theTerrainDB->Get(tileInfo->GetTerrainType())->GetTilesetIndex();

	
	uint16 tilesetIndex_short = (uint16) tilesetIndex;
	Assert(tilesetIndex == ((sint32) tilesetIndex_short));

	Pixel16 * t0 = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(0), 0);
	Pixel16 * t1 = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(1), 1);
	Pixel16 * t2 = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(2), 2);
	Pixel16 * t3 = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(3), 3);
	
	BaseTile * transitionBuffer = m_tileSet->GetBaseTile(static_cast<uint16>((tilesetIndex * 100) + 99));
	
	Pixel16     *transData, *transDataPtr;
	if(transitionBuffer) {
		transData = transitionBuffer->GetTileData();
		transDataPtr = transData;
	} else {
		transData = NULL;
		transDataPtr = NULL;
	}

	Pixel16 * dataPtr = data;
	uint8 *pSurfBase = m_surfBase;
	sint32 surfPitch = m_surfPitch;












	
	Pixel16 srcPixel, transPixel = 0;
	uint16 *pDestPixel;
	sint32		x;
	sint32		startX, endX;


		for (sint32 y=0; y<k_TILE_PIXEL_HEIGHT; y++) {
			if (y<=23) {
				startX = (23-y)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			} else {
				startX = (y-24)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			}

			for (x = startX; x<endX; x++) {
				srcPixel = *dataPtr++;
				if(transDataPtr)
					transPixel = *transDataPtr++;

				switch (srcPixel) {
				case 0x0000 : 
					{
						if (t0) {
							srcPixel = *t0++;
						} else if(transDataPtr) {
							srcPixel = transPixel;
						} else {
							srcPixel = 0xF800;
						}
					}
					break;
				case 0x0001 : 
					{
						if (t1) {
							srcPixel = *t1++;
						} else if(transDataPtr) {
							srcPixel = transPixel;
						} else {
							srcPixel = 0x07E0;
						}
					}
					break;
				case 0x0002 : 
					{
						if (t2) {
							srcPixel = *t2++;
						} else if(transDataPtr) {
							srcPixel = transPixel;
						} else {
							srcPixel = 0x001F; 
						}
					}
					break;
				case 0x0003 : 
					{
						if (t3) {
							srcPixel = *t3++;
						} else if(transDataPtr) {
							srcPixel = transPixel;
						} else {
							srcPixel = 0xF81F;
						}
					}
					break;
				}
				pDestPixel = (Pixel16 *)(pSurfBase + ((y+ypos) * surfPitch) + ((x+xpos) << 1));


				*pDestPixel = pixelutils_BlendFast(srcPixel,color,blend);
			}
		}

	return 0;
}







void TiledMap::DrawBlendedTileScaled(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,Pixel16 color,sint32 blend)
{
	uint16		len;
	Pixel16		*dataPtr;
	sint32		startX, endX;
	uint32		accumTable[k_TILE_PIXEL_HEIGHT][3];
	TileInfo	*tileInfo;
	BaseTile	*baseTile, *transitionBuffer;
	uint16		index;
	uint8		*pSurfBase;
	BOOL		wasUnlocked = FALSE;
	AUI_ERRCODE	errcode;
	Pixel16 *transData, *transDataPtr;

	sint32 surfWidth;
	sint32 surfHeight;
	sint32 surfPitch;

	if (!surface) {
		surface = m_surface;

		pSurfBase = m_surfBase;
		surfWidth = m_surfWidth;
		surfHeight = m_surfHeight;
		surfPitch = m_surfPitch;
	} else {

		errcode = surface->Lock(NULL, (LPVOID *)&pSurfBase, 0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;

		surfWidth = surface->Width();
		surfHeight = surface->Height();
		surfPitch = surface->Pitch();

		wasUnlocked = TRUE;
	}

	y+=(sint32)((double)k_TILE_PIXEL_HEADROOM * m_scale);

	if ((x < 0) || 
		(x >= surface->Width() - destWidth) || 
		(y < 0) || 
		(y >= surface->Height() - destHeight)) {
		if (wasUnlocked) {
			
			errcode = surface->Unlock((LPVOID)pSurfBase);
			Assert(errcode == AUI_ERRCODE_OK);
		}
		return;
	}

	
	tileInfo = GetTileInfo(pos);
	Assert(tileInfo != NULL);
	if (tileInfo == NULL) {
		if (wasUnlocked) {
			
			errcode = surface->Unlock((LPVOID)pSurfBase);
			Assert(errcode == AUI_ERRCODE_OK);
		}
		return;
	}

	
	index = tileInfo->GetTileNum();

	
	baseTile = m_tileSet->GetBaseTile(index);
	if (baseTile == NULL) {
		if (wasUnlocked) {
			
			errcode = surface->Unlock((LPVOID)pSurfBase);
			Assert(errcode == AUI_ERRCODE_OK);
		}
		return;
	}

	
	Pixel16 *data = baseTile->GetTileData();

	Pixel16	*t0, *t1, *t2, *t3;

	sint32 tilesetIndex = g_theTerrainDB->Get(tileInfo->GetTerrainType())->GetTilesetIndex();

	
	uint16 tilesetIndex_short = (uint16) tilesetIndex;

	t0 = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(0), 0);
	t1 = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(1), 1);
	t2 = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(2), 2);
	t3 = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(3), 3);

	transitionBuffer = m_tileSet->GetBaseTile(static_cast<uint16>((tilesetIndex * 100) + 99));
	if(transitionBuffer) {
		transData = transitionBuffer->GetTileData();
		transDataPtr = transData;
	} else {
		transData = NULL;
		transDataPtr = NULL;
	}

	dataPtr = data;

	
	Pixel16 srcPixel, transPixel = 0;
	uint16 *pDestPixel;

	
	sint32				vaccum;
	sint32				vincx, vincxy;
	sint32				vend;
	sint32				vdestpos;
	sint32				vpos1, vpos2;

	sint32 vstart;

	vaccum = destHeight*2 - k_TILE_PIXEL_HEIGHT;
	vincx = destHeight*2;
	vincxy = (destHeight - k_TILE_PIXEL_HEIGHT) * 2 ;

	vpos1 = 0;
	vpos2 = (sint32)((double)(k_TILE_PIXEL_HEIGHT - destHeight) / (double)destHeight);

	vdestpos = y;
	vend = k_TILE_PIXEL_HEIGHT - 1;

	
	while ( vpos1 < vend) {
		if (vaccum < 0) {
			vaccum += vincx;

			if (vpos1<=23) {
				startX = (23-vpos1)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			} else {
				startX = (vpos1-24)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			}

			for (sint32 i=startX; i<endX; i++) {
				srcPixel = *dataPtr++;
				if(transDataPtr)
					transPixel = *transDataPtr++;

				switch (srcPixel) {
				case 0x0000 : 
					if (t0)	srcPixel = *t0++;
					else if(transDataPtr) srcPixel = transPixel;
					break;
				case 0x0001 : 
					if (t1)	srcPixel = *t1++;
					else if(transDataPtr) srcPixel = transPixel;
					break;
				case 0x0002 : 
					if (t2) srcPixel = *t2++;
					else if(transDataPtr) srcPixel = transPixel;
					break;
				case 0x0003 : 
					if (t3) srcPixel = *t3++;
					else if(transDataPtr) srcPixel = transPixel;
					break;
				}
			}
		} else {
			

			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			

			haccum = destWidth * 2 - k_TILE_PIXEL_WIDTH;
			hincx = destWidth * 2;
			hincxy = (destWidth - k_TILE_PIXEL_WIDTH) * 2;
			
			hpos = 0;
			hend = k_TILE_PIXEL_WIDTH - 1;

			
			if (vpos1<=23) {
				startX = (23-vpos1)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			} else {
				startX = (vpos1-24)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			}

			hdestpos = x + (sint32)((double)startX * m_scale);

			hpos = startX;
			hend = endX;

			while (hpos < hend) {
				srcPixel = *dataPtr++;
				if(transDataPtr)
					transPixel = *transDataPtr++;
				switch (srcPixel) {
				case 0x0000 : 
						if (t0)	srcPixel = *t0++;
						else if(transDataPtr) srcPixel = transPixel;
						else srcPixel = 0xF800;
					break;
				case 0x0001 : 
						if (t1)	srcPixel = *t1++;
						else if(transDataPtr) srcPixel = transPixel;
						else srcPixel = 0x07E0;
					break;
				case 0x0002 : 
						if (t2) srcPixel = *t2++;
						else if(transDataPtr) srcPixel = transPixel;
						else srcPixel = 0x001F; 
					break;
				case 0x0003 : 
						if (t3) srcPixel = *t3++;
						else if(transDataPtr) srcPixel = transPixel;
						else srcPixel = 0xF81F;
					break;
				}
				
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					pDestPixel = (Pixel16 *)(pSurfBase + (vdestpos * surfPitch) + (hdestpos << 1));

					*pDestPixel = pixelutils_BlendFast(srcPixel,color,blend);

					hdestpos++;
				}
				hpos++;
			}

			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}

	if (wasUnlocked) {
		
		errcode = surface->Unlock((LPVOID)pSurfBase);
		Assert(errcode == AUI_ERRCODE_OK);
	}

	len;
	vstart;
	accumTable;
}

sint32 TiledMap::DrawBlendedOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, 
										Pixel16 color, sint32 blend, sint32 flags)
{
	uint8			*surfBase;
	BOOL			wasUnlocked = FALSE;

	if (data == NULL) return 0;



	sint32 surfWidth;
	sint32 surfHeight;
	sint32 surfPitch;

	sint32 errcode;

	if (surface) {
		errcode = surface->Lock(NULL, (LPVOID *)&surfBase, 0);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACELOCKFAILED;

		
		surfWidth = surface->Width();
		surfHeight = surface->Height();
		surfPitch = surface->Pitch();

		wasUnlocked = TRUE;
	} else {
		surfBase = m_surfBase;
		surfWidth = m_surfWidth;
		surfHeight = m_surfHeight;
		surfPitch = m_surfPitch;
	}


if (x < 0) return 0;
if (y < 0) return 0;
if (x >= surfWidth - k_TILE_GRID_WIDTH) return 0;
if (y >= surfHeight - k_TILE_GRID_HEIGHT) return 0;













	
	unsigned short	*destPixel;

	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);

	
	for(sint32 j=start; j<end; j++) {
		Pixel16		*rowData;
		Pixel16		tag;

		if ((sint16)table[j-start] == -1) continue;

		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));

		rowData = dataStart + table[j-start];
		
		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
						destPixel += (tag & 0x00FF);
					break;
				case k_TILE_COPY_RUN_ID			: {
						short len = (tag & 0x00FF);

						for (short i=0; i<len; i++) {
							if (!(flags & k_OVERLAY_FLAG_SHADOWSONLY)) {
								*destPixel++ = pixelutils_BlendFast(*rowData,color,blend);
								rowData++;
							} else {
								rowData++;
								destPixel++;
							}
						}

					}
					break;
				case k_TILE_SHADOW_RUN_ID		: {
						sint32 len = (sint32)(tag & 0x00FF);
						for (sint32 i=0; i<len; i++) {
							if (!(flags & k_OVERLAY_FLAG_NOSHADOWS)) {
								*destPixel = pixelutils_Shadow(*destPixel);
								destPixel++;
							} else {
								destPixel++;
							}
						}
					}
					break;
			}

		} while ((tag & 0xF000) == 0);
	}

	if (wasUnlocked) {
		
		errcode = surface->Unlock((LPVOID *)surfBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return 0;
}

void TiledMap::DrawBlendedOverlayScaled(aui_Surface *surface,Pixel16 *data, sint32 x, sint32 y, 
										sint32 destWidth, sint32 destHeight, Pixel16 color, sint32 blend,
										sint32 flags)
{
	uint8		*surfBase;
	BOOL		wasUnlocked = FALSE;
	AUI_ERRCODE	errcode;

	if (data == NULL) return;

	sint32 surfWidth;
	sint32 surfHeight;
	sint32 surfPitch;

	if (!surface) {
		surface = m_surface;

		surfBase = m_surfBase;
		surfWidth = m_surfWidth;
		surfHeight = m_surfHeight;
		surfPitch = m_surfPitch;
	} else {

		errcode = surface->Lock(NULL, (LPVOID *)&surfBase, 0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;

		surfWidth = surface->Width();
		surfHeight = surface->Height();
		surfPitch = surface->Pitch();

		wasUnlocked = TRUE;
	}

	if ((x < 0) ||
		(y < 0) ||
		(x >= surface->Width() - destWidth) ||
		(y >= surface->Height() - destHeight)) {

		if (wasUnlocked) {
			
			errcode = surface->Unlock((LPVOID)surfBase);
			Assert(errcode == AUI_ERRCODE_OK);
		}
		return;
	}

	
	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	surfBase += (surfPitch * y + x * 2);

	
	Pixel16			*destPixel;
	uint16			vstart = (uint16)*data++;
	uint16			end = (uint16)*data++;
	Pixel16			*table = data;

	Pixel16			*dataStart = table + (end - vstart + 1);

	
	sint32				vaccum;
	sint32				vincx, vincxy;
	sint32				vend;
	sint32				vdestpos;
	sint32				vpos1, vpos2;

	vaccum = destHeight*2 - k_TILE_GRID_HEIGHT;
	vincx = destHeight*2;
	vincxy = (destHeight-k_TILE_GRID_HEIGHT) * 2;

	vpos1 = 0;
	vpos2 = (sint32)((double)((end+1) - destHeight) / (double)destHeight);

	vdestpos = y;
	vend = (end+1) - 1;

	
	while ( vpos1 < vend) {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			
			Pixel16		*rowData1, *rowData2;
			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;

			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos1++;
				vpos2++;
				continue;
			}

			
			if (vpos1 < vstart) rowData1 = emptyRow;
			else rowData1 = dataStart + table[vpos1-vstart];

			if (vpos2 < vstart) rowData2 = emptyRow;
			else rowData2 = dataStart + table[vpos2-vstart];

			haccum = destWidth*2 - k_TILE_GRID_WIDTH;
			hincx = destWidth*2;
			hincxy = (destWidth-k_TILE_GRID_WIDTH) * 2;
			
			hpos = 0;
			hdestpos = x;
			hend = k_TILE_GRID_WIDTH-1;

			
			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, flags);

			while (hpos < hend) {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					destPixel = (Pixel16 *)(surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));
		
					
					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, flags);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {
						
						
						pixel = pixel3;

						

						*destPixel = pixelutils_BlendFast(pixel,color,blend);

					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
				hpos++;
			}

			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}

	if (wasUnlocked) {
		
		errcode = surface->Unlock(surfBase);
		Assert(errcode == AUI_ERRCODE_OK);
	}
}

sint32 TiledMap::DrawBlendedOverlayIntoMix(Pixel16 *data, sint32 x, sint32 y, Pixel16 color, sint32 blend)
{

	if (data == NULL) return 0;

	
	sint32  surfWidth   = g_screenManager->GetSurfWidth();
	sint32  surfHeight  = g_screenManager->GetSurfHeight();
	sint32  surfPitch   = g_screenManager->GetSurfPitch();
	uint8 * surfBase    = g_screenManager->GetSurfBase();

	
	if (x < 0) return 0;
	if (y < 0) return 0;
	if (x >= surfWidth - k_TILE_GRID_WIDTH) return 0;
	if (y >= surfHeight - k_TILE_GRID_HEIGHT) return 0;

	
	unsigned short	*destPixel;

	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);

	
	for(sint32 j=start; j<end; j++) {
		Pixel16		*rowData;
		Pixel16		tag;

		if ((sint16)table[j-start] == -1) continue;

		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));

		rowData = dataStart + table[j-start];
		
		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
						destPixel += (tag & 0x00FF);
					break;
				case k_TILE_COPY_RUN_ID			: {
						short len = (tag & 0x00FF);

						for (short i=0; i<len; i++) {
							*destPixel++ = pixelutils_BlendFast(*rowData,color,blend);
							rowData++;
						}

					}
					break;
				case k_TILE_SHADOW_RUN_ID		: {
						sint32 len = (sint32)(tag & 0x00FF);
						for (sint32 i=0; i<len; i++) {
							*destPixel = pixelutils_Shadow(*destPixel);
							destPixel++;
						}
					}
					break;
			}

		} while ((tag & 0xF000) == 0);
	}

	return 0;
}

void TiledMap::DrawBlendedOverlayScaledIntoMix(Pixel16 *data, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight, Pixel16 color, sint32 blend)
{
	uint8		*surfBase;

	if (data == NULL) return;

	
	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	
	sint32 surfWidth = g_screenManager->GetSurfWidth();
	sint32 surfHeight = g_screenManager->GetSurfHeight();
	sint32 surfPitch = g_screenManager->GetSurfPitch();
	surfBase = g_screenManager->GetSurfBase();

	
	if (x < 0) return;
	if (y < 0) return;
	if (x >= surfWidth - destWidth) return;
	if (y >= surfHeight - destHeight) return;

	surfBase += (surfPitch * y + x * 2);

	
	Pixel16			*destPixel;
	uint16			vstart = (uint16)*data++;
	uint16			end = (uint16)*data++;
	Pixel16			*table = data;

	Pixel16			*dataStart = table + (end - vstart + 1);

	
	sint32				vaccum;
	sint32				vincx, vincxy;
	sint32				vend;
	sint32				vdestpos;
	sint32				vpos1, vpos2;

	vaccum = destHeight*2 - k_TILE_GRID_HEIGHT;
	vincx = destHeight*2;
	vincxy = (destHeight-k_TILE_GRID_HEIGHT) * 2;

	vpos1 = 0;
	vpos2 = (sint32)((double)((end+1) - destHeight) / (double)destHeight);

	vdestpos = y;
	vend = (end+1) - 1;

	
	while ( vpos1 < vend) {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			
			Pixel16		*rowData1, *rowData2;
			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;

			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos1++;
				vpos2++;
				continue;
			}

			
			if (vpos1 < vstart) rowData1 = emptyRow;
			else rowData1 = dataStart + table[vpos1-vstart];

			if (vpos2 < vstart) rowData2 = emptyRow;
			else rowData2 = dataStart + table[vpos2-vstart];

			haccum = destWidth*2 - k_TILE_GRID_WIDTH;
			hincx = destWidth*2;
			hincxy = (destWidth-k_TILE_GRID_WIDTH) * 2;
			
			hpos = 0;
			hdestpos = x;
			hend = k_TILE_GRID_WIDTH-1;

			
			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, 0);

			while (hpos < hend) {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					destPixel = (Pixel16 *)(surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));
		
					
					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, 0);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {
						
						
						pixel = pixel3;

						

						*destPixel = pixelutils_BlendFast(pixel,color,blend);

					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
				hpos++;
			}

			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}
}
sint32 TiledMap::DrawDitheredOverlayIntoMix(Pixel16 *data, sint32 x, sint32 y, BOOL fogged)
{
	if (data == NULL) return 0;

	sint32  surfWidth   = g_screenManager->GetSurfWidth();
	sint32  surfHeight  = g_screenManager->GetSurfHeight();
	sint32  surfPitch   = g_screenManager->GetSurfPitch();
	uint8 * surfBase    = g_screenManager->GetSurfBase();

	
	if (x < 0) return 0;
	if (y < 0) return 0;
	if (x >= surfWidth - k_TILE_GRID_WIDTH) return 0;
	if (y >= surfHeight - k_TILE_GRID_HEIGHT) return 0;

	
	unsigned short	*destPixel;

	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);
	sint32		hpos;

	
	for(sint32 j=start; j<end; j++) {
		Pixel16		*rowData;
		Pixel16		tag;

		if ((sint16)table[j-start] == -1) continue;

		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));

		rowData = dataStart + table[j-start];
	
		hpos = 0;
		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
						destPixel += (tag & 0x00FF);
						hpos += (tag & 0x00FF);
					break;
				case k_TILE_COPY_RUN_ID			: {
						short len = (tag & 0x00FF);

						for (short i=0; i<len; i++) {
							if ((hpos + j) & 1) {
								if (fogged)
									*destPixel++ = pixelutils_BlendFast(*rowData,k_FOW_COLOR,k_FOW_BLEND_VALUE);
								else
									*destPixel++ = *rowData;
							} else {
								destPixel++;
							}

							hpos++;
							rowData++;
						}

					}
					break;
				case k_TILE_SHADOW_RUN_ID		: {
						sint32 len = (sint32)(tag & 0x00FF);
						for (sint32 i=0; i<len; i++) {
							*destPixel = pixelutils_Shadow(*destPixel);
							destPixel++;
							hpos++;
						}
					}
					break;
			}

		} while ((tag & 0xF000) == 0);
	}

	return 0;
}

void TiledMap::DrawDitheredOverlayScaledIntoMix(Pixel16 *data, sint32 x, sint32 y, 
												sint32 destWidth, sint32 destHeight, 
												BOOL fogged)
{
	uint8		*surfBase;

	if (data == NULL) return;

	
	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	
	sint32 surfWidth = g_screenManager->GetSurfWidth();
	sint32 surfHeight = g_screenManager->GetSurfHeight();
	sint32 surfPitch = g_screenManager->GetSurfPitch();
	surfBase = g_screenManager->GetSurfBase();

	
	if (x < 0) return;
	if (y < 0) return;
	if (x >= surfWidth - destWidth) return;
	if (y >= surfHeight - destHeight) return;

	surfBase += (surfPitch * y + x * 2);

	
	Pixel16			*destPixel;
	uint16			vstart = (uint16)*data++;
	uint16			end = (uint16)*data++;
	Pixel16			*table = data;

	Pixel16			*dataStart = table + (end - vstart + 1);

	
	sint32				vaccum;
	sint32				vincx, vincxy;
	sint32				vend;
	sint32				vdestpos;
	sint32				vpos1, vpos2;

	vaccum = destHeight*2 - k_TILE_GRID_HEIGHT;
	vincx = destHeight*2;
	vincxy = (destHeight-k_TILE_GRID_HEIGHT) * 2;

	vpos1 = 0;
	vpos2 = (sint32)((double)((end+1) - destHeight) / (double)destHeight);

	vdestpos = y;
	vend = (end+1) - 1;

	
	while ( vpos1 < vend) {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			
			Pixel16		*rowData1, *rowData2;
			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;

			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos1++;
				vpos2++;
				continue;
			}

			
			if (vpos1 < vstart) rowData1 = emptyRow;
			else rowData1 = dataStart + table[vpos1-vstart];

			if (vpos2 < vstart) rowData2 = emptyRow;
			else rowData2 = dataStart + table[vpos2-vstart];

			haccum = destWidth*2 - k_TILE_GRID_WIDTH;
			hincx = destWidth*2;
			hincxy = (destWidth-k_TILE_GRID_WIDTH) * 2;
			
			hpos = 0;
			hdestpos = x;
			hend = k_TILE_GRID_WIDTH-1;

			
			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, 0);

			while (hpos < hend) {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					destPixel = (Pixel16 *)(surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));
		
					
					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, 0);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {
						
						
						pixel = pixel3;

						
						if ((hdestpos + vdestpos) & 1) {
							if (fogged)
								*destPixel = pixelutils_BlendFast(pixel,k_FOW_COLOR,k_FOW_BLEND_VALUE);
							else
								*destPixel = pixel;
						}
					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
				hpos++;
			}

			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}
}
sint32 TiledMap::DrawDitheredOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, Pixel16 color)
{
	uint8			*surfBase;

	if (data == NULL) return 0;

	if (!surface) surface = m_surface;


if (x < 0) return 0;
if (y < 0) return 0;
if (x >= surface->Width() - k_TILE_GRID_WIDTH) return 0;
if (y >= surface->Height() - k_TILE_GRID_HEIGHT) return 0;

	surfBase = m_surfBase;
	sint32 surfPitch = m_surfPitch;













	
	unsigned short	*destPixel;
	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);

	
	for(sint32 j=start; j<end; j++) {
		Pixel16		*rowData;
		Pixel16		tag;

		if ((sint16)table[j-start] == -1) continue;

		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));
		rowData = dataStart + table[j-start];
		
		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
						destPixel += (tag & 0x00FF);
					break;
				case k_TILE_COPY_RUN_ID			: {
						short len = (tag & 0x00FF);

						
						if (j & 0x01)
							destPixel++;

						for (short i=0; i<len; i++) {
							if (i & 0x01)
								*destPixel = color;
							destPixel++;
							rowData++;
						}

					}
					break;
				case k_TILE_SHADOW_RUN_ID		: {
						sint32 len = (sint32)(tag & 0x00FF);
						for (sint32 i=0; i<len; i++) {
							*destPixel = pixelutils_Shadow(*destPixel);
							destPixel++;
						}
					}
					break;
			}

		} while ((tag & 0xF000) == 0);
	}







	return 0;
}

void TiledMap::DrawDitheredOverlayScaled(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight, Pixel16 color)
{
	uint8		*surfBase;

	if (data == NULL) return;

	if (!surface) surface = m_surface;


if (x < 0) return;
if (y < 0) return;
if (x >= surface->Width() - destWidth) return;
if (y >= surface->Height() - destHeight) return;

	
	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	surfBase = m_surfBase;
	sint32 surfPitch = m_surfPitch;

	surfBase += (y * surfPitch + x * 2);


	
	Pixel16			*destPixel;
	uint16			vstart = (uint16)*data++;
	uint16			end = (uint16)*data++;
	Pixel16			*table = data;

	Pixel16			*dataStart = table + (end - vstart + 1);

	
	sint32				vaccum;
	sint32				vincx, vincxy;
	sint32				vend;
	sint32				vdestpos;
	sint32				vpos1, vpos2;






	vaccum = destHeight*2 - k_TILE_GRID_HEIGHT;
	vincx = destHeight*2;
	vincxy = (destHeight - k_TILE_GRID_HEIGHT) * 2 ;

	vpos1 = 0;
	vpos2 = (sint32)((double)((end+1) - destHeight) / (double)destHeight);

	vdestpos = y;
	vend = (end+1) - 1;

	
	while ( vpos1 < vend) {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			
			Pixel16		*rowData1, *rowData2;
			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;

			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos1++;
				vpos2++;
				continue;
			}

			
			if (vpos1 < vstart) rowData1 = emptyRow;
			else rowData1 = dataStart + table[vpos1-vstart];

			if (vpos2 < vstart) rowData2 = emptyRow;
			else rowData2 = dataStart + table[vpos2-vstart];

			haccum = destWidth*2 - k_TILE_GRID_WIDTH;
			hincx = destWidth*2;
			hincxy = (destWidth-k_TILE_GRID_WIDTH) * 2;
			
			hpos = 0;
			hdestpos = x;
			hend = k_TILE_GRID_WIDTH-1;

			
			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, 0);

			sint32 offset;
			if (vpos1 & 0x01)
				offset = 1;
			else
				offset = 0;

			while (hpos < hend) {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					destPixel = (Pixel16 *)(surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));
					destPixel += offset;
					
					
					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, 0);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {
						
						
						pixel = pixel3;

						

						if ((hdestpos+vdestpos) & 0x01)
							*destPixel = color;

					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
				hpos++;
			}

			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}






}







sint32 TiledMap::DrawTileBorder(aui_Surface *surface, sint32 x, sint32 y, Pixel16 color)
{
	uint8			*surfBase;

	if (!surface) surface = m_surface;

	surfBase = m_surfBase;
	sint32 surfPitch = m_surfPitch;

	
	unsigned short	*destPixel;

	y+=k_TILE_PIXEL_HEADROOM;


if (x < 0) return 0;
if (x > surface->Width() - k_TILE_PIXEL_WIDTH) return 0;
if (y < 0) return 0;
if (y > surface->Height() - k_TILE_PIXEL_HEIGHT) return 0;

	sint32 startX, endX;

	
	for(sint32 j=0; j<k_TILE_PIXEL_HEIGHT; j++) {
		if (j<=23) {
			startX = (23-j)*2;////not for border pixels
		} else {
			startX = (j-24)*2; //not for border pixels
		}
		endX = k_TILE_PIXEL_WIDTH - startX;

		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + ((x+startX) * 2));

		*destPixel++ = color;
		*destPixel = color;






	}







	return 0;
}







void TiledMap::DrawTileBorderScaled(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,Pixel16 color)
{
	sint32		startX, endX;

	if (!surface) surface = m_surface;

	y+=(sint32)((double)k_TILE_PIXEL_HEADROOM * m_scale);
	

if (x < 0) return;
if (x >= surface->Width() - destWidth) return;
if (y < 0) return;
if (y >= surface->Height() - destHeight) return;

	uint8 * pSurfBase   = m_surfBase;
	sint32  surfPitch   = m_surfPitch;
	sint32	vaccum      = destHeight*2 - k_TILE_PIXEL_HEIGHT;;
	sint32	vincx       = destHeight*2;
    sint32  vincxy      = (destHeight - k_TILE_PIXEL_HEIGHT) * 2;
	sint32	vpos1       = 0;
    sint32  vpos2       = 
        (sint32)((double)(k_TILE_PIXEL_HEIGHT - destHeight) / (double)destHeight);
	sint32	vdestpos    = y;
	sint32	vend        = k_TILE_PIXEL_HEIGHT - 1;

	uint16 *pDestPixel;
	while ( vpos1 < vend) {

		if (vaccum < 0) {
			vaccum += vincx;

			if (vpos1<=23) {
				startX = (23-vpos1)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			} else {
				startX = (vpos1-24)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			}
		} else {
			
			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			

			haccum = destWidth * 2 - k_TILE_PIXEL_WIDTH;
			hincx = destWidth * 2;
			hincxy = (destWidth - k_TILE_PIXEL_WIDTH) * 2;
			
			hpos = 0;
			hend = k_TILE_PIXEL_WIDTH - 1;

			
			if (vpos1<=23) {
				startX = (23-vpos1)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			} else {
				startX = (vpos1-24)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			}

			hdestpos = x + (sint32)((double)startX * m_scale);

			hpos = startX;
			hend = endX;

			while (hpos < hend) {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					pDestPixel = (Pixel16 *)(pSurfBase + (vdestpos * surfPitch) + (hdestpos << 1));
					if (hpos == startX || hpos == startX+1 ||
						hpos == hend-2 || hpos == hend-1)
						*pDestPixel = color;

					hdestpos++;
				}
				hpos++;
			}
			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}
}

void TiledMap::DrawBlackScaledLow(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight)
{
	if (!surface) surface = m_surface;


	y+=GetZoomTileHeadroom();


if (x < 0) return;
if (x >= surface->Width() - destWidth) return;
if (y < 0) return;
if (y >= surface->Height() - destHeight) return;

	
	TileInfo *  tileInfo = GetTileInfo(pos);
	Assert(tileInfo != NULL);
	if (tileInfo == NULL) return;

	uint16      index   = tileInfo->GetTileNum();
	BaseTile *  baseTile = m_tileSet->GetBaseTile(index);
	if (baseTile == NULL) return;

	


	Pixel16 *data = baseTile->GetTileData();

	Pixel16	*t0, *t1, *t2, *t3;

	t0 = m_tileSet->GetTransitionData(tileInfo->GetTerrainType(), tileInfo->GetTransition(0), 0);
	t1 = m_tileSet->GetTransitionData(tileInfo->GetTerrainType(), tileInfo->GetTransition(1), 1);
	t2 = m_tileSet->GetTransitionData(tileInfo->GetTerrainType(), tileInfo->GetTransition(2), 2);
	t3 = m_tileSet->GetTransitionData(tileInfo->GetTerrainType(), tileInfo->GetTransition(3), 3);
	
	Pixel16 *   dataPtr     = data;
	uint8 *     pSurfBase   = m_surfBase;
	sint32      surfPitch   = m_surfPitch;












	
	Pixel16 srcPixel;
	uint16 *pDestPixel;

	
	sint32				vaccum;
	sint32				vincx, vincxy;
	sint32				vend;
	sint32				vdestpos;
	sint32				vpos1, vpos2;

	vaccum = destHeight*2 - k_TILE_PIXEL_HEIGHT;
	vincx = destHeight*2;
	vincxy = (destHeight - k_TILE_PIXEL_HEIGHT) * 2 ;

	vpos1 = 0;
	vpos2 = (sint32)((double)(k_TILE_PIXEL_HEIGHT - destHeight) / (double)destHeight);

	vdestpos = y;
	vend = k_TILE_PIXEL_HEIGHT - 1;

	sint32		startX, endX;

    while ( vpos1 < vend) {
		if (vaccum < 0) {
			vaccum += vincx;

			if (vpos1<=23) {
				startX = (23-vpos1)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			} else {
				startX = (vpos1-24)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			}

			for (sint32 i=startX; i<endX; i++) {
				srcPixel = *dataPtr++;
				switch (srcPixel) {
				case 0x0000 : if (t0)	srcPixel = *t0++;
					break;
				case 0x0001 : if (t1)	srcPixel = *t1++;
					break;
				case 0x0002 : if (t2) srcPixel = *t2++;
					break;
				case 0x0003 : if (t3) srcPixel = *t3++;
					break;
				}
			}
		} else {
			
			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			

			haccum = destWidth * 2 - k_TILE_PIXEL_WIDTH;
			hincx = destWidth * 2;
			hincxy = (destWidth - k_TILE_PIXEL_WIDTH) * 2;
			
			hpos = 0;
			hend = k_TILE_PIXEL_WIDTH - 1;

			
			if (vpos1<=23) {
				startX = (23-vpos1)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			} else {
				startX = (vpos1-24)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			}

			hdestpos = x + (sint32)((double)startX * m_scale);

			hpos = startX;
			hend = endX;

			while (hpos < hend) {
				srcPixel = *dataPtr++;
				switch (srcPixel) {
				case 0x0000 : 
						if (t0)	srcPixel = *t0++;
						else srcPixel = 0xF800;
					break;
				case 0x0001 : 
						if (t1)	srcPixel = *t1++;
						else srcPixel = 0x07E0;
					break;
				case 0x0002 : 
						if (t2) srcPixel = *t2++;
						else srcPixel = 0x001F; 
					break;
				case 0x0003 : 
						if (t3) srcPixel = *t3++;
						else srcPixel = 0xF81F;
					break;
				}
				
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;



					pDestPixel = (Pixel16 *)(pSurfBase + (vdestpos * surfPitch) + (hdestpos << 1));

					*pDestPixel = 0x0000;

					hdestpos++;
				}
				hpos++;
			}

			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}





}






sint32 TiledMap::DrawOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, sint32 flags)
{
	uint8			*surfBase;
	sint32			surfWidth;
	sint32			surfHeight;
	sint32			surfPitch;
	sint32			errcode;

	if (data == NULL) return 0;

	if (surface) {
		errcode = surface->Lock(NULL, (LPVOID *)&surfBase, 0);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACELOCKFAILED;

		
		surfWidth = surface->Width();
		surfHeight = surface->Height();
		surfPitch = surface->Pitch();
	} else {
		surfBase = m_surfBase;
		surfWidth = m_surfWidth;
		surfHeight = m_surfHeight;
		surfPitch = m_surfPitch;
	}


	if (x < 0) {
		if (surface) {
			
			errcode = surface->Unlock((LPVOID *)surfBase);
			Assert(errcode == AUI_ERRCODE_OK);
			if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	if (y < 0) {
		if (surface) {
			
			errcode = surface->Unlock((LPVOID *)surfBase);
			Assert(errcode == AUI_ERRCODE_OK);
			if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}




	
	unsigned short	*destPixel;

	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);

	sint32 j;
	sint32 len;

	
	for(j=start; j<=end; j++) {
		Pixel16		*rowData;
		Pixel16		tag;

		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));

		if ((y+j) >= surfHeight) continue;
		if ((sint16)table[j-start] == -1) continue;

		rowData = dataStart + table[j-start];
		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
						destPixel += (tag & 0x00FF);
					break;
				case k_TILE_COPY_RUN_ID			: {
						len = (tag & 0x00FF);

						while (len--) {
							if (!(flags & k_OVERLAY_FLAG_SHADOWSONLY)) {
								*destPixel++ = *rowData++;
							} else {
								destPixel++;
								rowData++;
							}

						}
					}
					break;
				case k_TILE_SHADOW_RUN_ID		: {
						len = (tag & 0x00FF);
						while (len--) {
							if (!(flags & k_OVERLAY_FLAG_NOSHADOWS)) {
								*destPixel = pixelutils_Shadow(*destPixel);
								destPixel++;
							} else {
								destPixel++;
							}
						}
					}
					break;
			}

		} while ((tag & 0xF000) == 0);
	}

	if (surface) {
		
		errcode = surface->Unlock((LPVOID *)surfBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return 0;
}






sint32 TiledMap::DrawColorBlendedOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, Pixel16 color)
{
	uint8			*surfBase;
	sint32			surfWidth;
	sint32			surfHeight;
	sint32			surfPitch;
	sint32			errcode;

	if (data == NULL) return 0;

	if (surface) {
		errcode = surface->Lock(NULL, (LPVOID *)&surfBase, 0);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACELOCKFAILED;

		
		surfWidth = surface->Width();
		surfHeight = surface->Height();
		surfPitch = surface->Pitch();
	} else {
		surfBase = m_surfBase;
		surfWidth = m_surfWidth;
		surfHeight = m_surfHeight;
		surfPitch = m_surfPitch;
	}

	if (!surfBase) return 0;

	
	
	
	if (x < 0 || 
		x >= (surfWidth - k_TILE_GRID_WIDTH) ||
		y < 0 || 
		y >= (surfHeight - k_TILE_GRID_HEIGHT)) {
		if (surface) {
			
			errcode = surface->Unlock((LPVOID *)surfBase);
			Assert(errcode == AUI_ERRCODE_OK);
			if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}

		return 0;
	}
	
	
	unsigned short	*destPixel;

	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);

	sint32 j;
	sint32 len;

	
	for(j=start; j<=end; j++) {
		Pixel16		*rowData;
		Pixel16		tag;

		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));

		if ((y+j) >= surfHeight) continue;
		if ((sint16)table[j-start] == -1) continue;

		rowData = dataStart + table[j-start];
		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
						destPixel += (tag & 0x00FF);
					break;
				case k_TILE_COPY_RUN_ID			: {
						len = (tag & 0x00FF);

						while (len--) {
							*destPixel = pixelutils_BlendFast(color, *rowData++, 8);
							destPixel++;
						}
					}
					break;
				case k_TILE_SHADOW_RUN_ID		: {
						len = (tag & 0x00FF);
						while (len--) {
							*destPixel = pixelutils_Shadow(*destPixel);
							destPixel++;
						}
					}
					break;
			}

		} while ((tag & 0xF000) == 0);
	}

	if (surface) {
		
		errcode = surface->Unlock((LPVOID *)surfBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return 0;
}



void TiledMap::DrawColorBlendedOverlayScaled(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, 
								 sint32 destWidth, sint32 destHeight, Pixel16 color)
{
	uint8			*surfBase = NULL, *origBase = NULL;
	AUI_ERRCODE		errcode;

	if (data == NULL) return;

	sint32	surfWidth, surfHeight, surfPitch;

	if (surface) {
		errcode = surface->Lock(NULL, (LPVOID *)&origBase, 0);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return;

		
		surfBase = origBase;
		surfWidth = surface->Width();
		surfHeight = surface->Height();
		surfPitch = surface->Pitch();
	} else {
		surfBase = m_surfBase;
		surfWidth = m_surfWidth;
		surfHeight = m_surfHeight;
		surfPitch = m_surfPitch;
	}

	if (!surfBase) return;

	
	
	
	if ((x < 0) ||
		(y < 0) || 
		(x >= (surfWidth - destWidth)) || 
		(y >= (surfHeight - destHeight))) {
		if (surface) {
			
			errcode = surface->Unlock((LPVOID *)origBase);
			Assert(errcode == AUI_ERRCODE_OK);
			if ( errcode != AUI_ERRCODE_OK ) return;
		}

		return;
	}

	
	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	surfBase += (y * surfPitch + x * 2);

	
	Pixel16			*destPixel;
	uint16			vstart = (uint16)*data++;
	uint16			end = (uint16)*data++;
	Pixel16			*table = data;

	Pixel16			*dataStart = table + (end - vstart + 1);

	
	sint32				vaccum;
	sint32				vincx, vincxy;
	sint32				vend;
	sint32				vdestpos;
	sint32				vpos1, vpos2;

	vaccum = destHeight*2 - k_TILE_GRID_HEIGHT;
	vincx = destHeight*2;
	vincxy = (destHeight-k_TILE_GRID_HEIGHT) * 2 ;

	vpos1 = 0;
	vpos2 = (sint32)((double)((end+1) - destHeight) / (double)destHeight);

	vdestpos = y;

	vend = (end+1) - 1;

	
	while ( vpos1 < vend) {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			

			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos1++;
				vpos2++;
				continue;
			}

			Pixel16		*rowData1, *rowData2;
			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;

			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			
			if (vpos1 < vstart) rowData1 = emptyRow;
			else rowData1 = dataStart + table[vpos1-vstart];

			if (vpos2 < vstart) rowData2 = emptyRow;
			else rowData2 = dataStart + table[vpos2-vstart];

			haccum = destWidth*2 - k_TILE_GRID_WIDTH;
			hincx = destWidth*2;
			hincxy = (destWidth-k_TILE_GRID_WIDTH) * 2;
			
			hpos = 0;
			hdestpos = x;
			hend = k_TILE_GRID_WIDTH-1;

			
			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, 0);

			while (hpos < hend) {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					destPixel = (Pixel16 *)(surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));
		
					
					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, 0);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {
						
						pixel = pixel3;

						

						*destPixel = pixelutils_BlendFast(color, pixel, 8);
					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
				hpos++;
			}

			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}

	if (surface) {
		
		errcode = surface->Unlock((LPVOID *)origBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return;
	}
}







sint32 TiledMap::DrawColorizedOverlay(Pixel16 *data, aui_Surface *surf, sint32 x, sint32 y, Pixel16 color)
{
	uint8			*surfBase;
	sint32			surfWidth;
	sint32			surfHeight;
	sint32			surfPitch;
	AUI_ERRCODE		errcode;

	if (data == NULL) return 0;

	if (surf) {
		errcode = surf->Lock(NULL, (LPVOID *)&surfBase, 0);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACELOCKFAILED;

		
		surfWidth = surf->Width();
		surfHeight = surf->Height();
		surfPitch = surf->Pitch();
	} else {
		surfBase = m_surfBase;
		surfWidth = m_surfWidth;
		surfHeight = m_surfHeight;
		surfPitch = m_surfPitch;
	}

	if (!surfBase) return 0;

	
	
	
	if ((x < 0) ||
		(y < 0)) {





		if (surf) {
			
			errcode = surf	->Unlock((LPVOID *)surfBase);
			Assert(errcode == AUI_ERRCODE_OK);
			if ( errcode != AUI_ERRCODE_OK ) return 0;
		}

		return 0;
	}

	
	unsigned short	*destPixel;

	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);

	sint32 j;
	sint32 len;

	
	for(j=start; j<=end; j++) {
		Pixel16		*rowData;
		Pixel16		tag;

		if ((y+j) >= surfHeight) continue;

		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));

		if ((sint16)table[j-start] == -1) continue;

		rowData = dataStart + table[j-start];
		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
						destPixel += (tag & 0x00FF);
					break;
				case k_TILE_COPY_RUN_ID			: {
						len = (tag & 0x00FF);

						while (len--) {

							*destPixel++ = *rowData++;
						}
					}
					break;
				case k_TILE_SHADOW_RUN_ID		: {
						len = (tag & 0x00FF);

						while (len--) {

							*destPixel = pixelutils_Shadow(*destPixel);
							destPixel++;
						}
					}
					break;
				case k_TILE_COLORIZE_RUN_ID		: {
						len = (tag & 0x00FF);
						while (len--) {

							*destPixel = color;
							destPixel++;
						}
					}
					break;	
			}
		} while ((tag & 0xF000) == 0);
	}

	
	if (surf) {
		errcode = surf->Unlock((LPVOID *)surfBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return 0;
}






sint32 TiledMap::DrawColorizedOverlayIntoMix(Pixel16 *data, sint32 x, sint32 y, Pixel16 color)
{
	if (data == NULL) return 0;

	uint8 * surfBase    = g_screenManager->GetSurfBase();
	if (!surfBase) return 0;

	if (x < 0) return 0;
	if (y < 0) return 0;



	sint32 surfHeight = g_screenManager->GetSurfHeight();
	sint32 surfPitch = g_screenManager->GetSurfPitch();



	
	unsigned short	*destPixel;

	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);
	sint32 j;
	sint32 len;

	
	for(j=start; j<=end; j++) {
		Pixel16		*rowData;
		Pixel16		tag;

		if ((y+j) >= surfHeight) continue;

		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));

		if ((sint16)table[j-start] == -1) continue;

		rowData = dataStart + table[j-start];
		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
						destPixel += (tag & 0x00FF);
					break;
				case k_TILE_COPY_RUN_ID			: {
						len = (tag & 0x00FF);

						while (len--) {
							*destPixel++ = *rowData++;
						}
					}
					break;
				case k_TILE_SHADOW_RUN_ID		: {
						len = (tag & 0x00FF);

						while (len--) {
							*destPixel = pixelutils_Shadow(*destPixel);
							destPixel++;
						}
					}
					break;
				case k_TILE_COLORIZE_RUN_ID		: {
						len = (tag & 0x00FF);

						while (len--) {
							*destPixel = color;
							destPixel++;
						}
					}
					break;	
			}
		} while ((tag & 0xF000) == 0);
	}

	return 0;
}






void TiledMap::DrawScaledOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, 
								 sint32 destWidth, sint32 destHeight, sint32 flags)
{
	uint8			*surfBase = NULL, *origBase = NULL;
	AUI_ERRCODE		errcode;
	sint32			surfWidth, surfHeight, surfPitch;
	BOOL			wasUnlocked = FALSE;

	if (data == NULL) return;

	if (surface) {
		errcode = surface->Lock(NULL, (LPVOID *)&origBase, 0);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return;

		wasUnlocked = TRUE;
		
		surfBase = origBase;
		surfWidth = surface->Width();
		surfHeight = surface->Height();
		surfPitch = surface->Pitch();
	} else {
		surface = m_surface;
		surfBase = m_surfBase;
		surfWidth = m_surfWidth;
		surfHeight = m_surfHeight;
		surfPitch = m_surfPitch;
	}

	
	if ((x < 0) || (y < 0) || (x >= surface->Width() - destWidth) || (y >= surface->Height() - destHeight))
	{
		if (wasUnlocked) {
			
			errcode = surface->Unlock(origBase);
			Assert(errcode == AUI_ERRCODE_OK);
		}
	}




	
	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;








	surfBase += (y * surfPitch + x * 2);

	
	Pixel16			*destPixel;
	uint16			vstart = (uint16)*data++;
	uint16			end = (uint16)*data++;
	Pixel16			*table = data;

	Pixel16			*dataStart = table + (end - vstart + 1);

	
	sint32				vaccum;
	sint32				vincx, vincxy;
	sint32				vend;
	sint32				vdestpos;
	sint32				vpos1, vpos2;

	vaccum = destHeight*2 - k_TILE_GRID_HEIGHT;
	vincx = destHeight*2;
	vincxy = (destHeight-k_TILE_GRID_HEIGHT) * 2 ;

	vpos1 = 0;
	vpos2 = (sint32)((double)((end+1) - destHeight) / (double)destHeight);

	vdestpos = y;

	vend = (end+1) - 1;

	
	while ( vpos1 < vend) {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			

			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos1++;
				vpos2++;
				continue;
			}

			Pixel16		*rowData1, *rowData2;
			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;

			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			
			if (vpos1 < vstart) rowData1 = emptyRow;
			else rowData1 = dataStart + table[vpos1-vstart];

			if (vpos2 < vstart) rowData2 = emptyRow;
			else rowData2 = dataStart + table[vpos2-vstart];

			haccum = destWidth*2 - k_TILE_GRID_WIDTH;
			hincx = destWidth*2;
			hincxy = (destWidth-k_TILE_GRID_WIDTH) * 2;
			
			hpos = 0;
			hdestpos = x;
			hend = k_TILE_GRID_WIDTH-1;

			
			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, flags);

			while (hpos < hend) {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					destPixel = (Pixel16 *)(surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));
		
					
					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, flags);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {
						
						pixel = pixel3;

						
						*destPixel = pixel;
					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
				hpos++;
			}

			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}

	if (wasUnlocked) {
		
		errcode = surface->Unlock(origBase);
		Assert(errcode == AUI_ERRCODE_OK);
	}
}






void TiledMap::DrawScaledOverlayIntoMix(Pixel16 *data, sint32 x, sint32 y, 
												 sint32 destWidth, sint32 destHeight)
{
	if (data == NULL) return;

	sint32  surfWidth   = g_screenManager->GetSurfWidth();
	sint32  surfHeight  = g_screenManager->GetSurfHeight();
	sint32  surfPitch   = g_screenManager->GetSurfPitch();
	uint8 * surfBase    = g_screenManager->GetSurfBase();

	if ((x < 0) || (y < 0) || (x >= surfWidth - destWidth) || (y >= surfHeight - destHeight))
	{
		return;
	}

	
	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	surfBase += (y * surfPitch + x * 2);

	
	Pixel16			*destPixel;
	uint16			vstart = (uint16)*data++;
	uint16			end = (uint16)*data++;
	Pixel16			*table = data;

	Pixel16			*dataStart = table + (end - vstart + 1);

	
	sint32				vaccum;
	sint32				vincx, vincxy;
	sint32				vend;
	sint32				vdestpos;
	sint32				vpos1, vpos2;

	vaccum = destHeight*2 - k_TILE_GRID_HEIGHT;
	vincx = destHeight*2;
	vincxy = (destHeight-k_TILE_GRID_HEIGHT) * 2 ;

	vpos1 = 0;
	vpos2 = (sint32)((double)((end+1) - destHeight) / (double)destHeight);

	vdestpos = y;

	vend = (end+1) - 1;

	
	while ( vpos1 < vend) {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			

			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos1++;
				vpos2++;
				continue;
			}

			Pixel16		*rowData1, *rowData2;
			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;

			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			
			if (vpos1 < vstart) rowData1 = emptyRow;
			else rowData1 = dataStart + table[vpos1-vstart];

			if (vpos2 < vstart) rowData2 = emptyRow;
			else rowData2 = dataStart + table[vpos2-vstart];

			haccum = destWidth*2 - k_TILE_GRID_WIDTH;
			hincx = destWidth*2;
			hincxy = (destWidth-k_TILE_GRID_WIDTH) * 2;
			
			hpos = 0;
			hdestpos = x;
			hend = k_TILE_GRID_WIDTH-1;

			
			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, 0);

			while (hpos < hend) {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					destPixel = (Pixel16 *)(surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));
		
					
					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, 0);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {
						
						pixel = pixel3;

						
						*destPixel = pixel;
					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
				hpos++;
			}

			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}
}

#define k_LABEL_BOX_SIZE			5

void TiledMap::DrawLabel(aui_Surface *surface, sint32 color, sint32 x, sint32 y)
{
	if (!surface) surface = m_surface;

	RECT		labelRect = {(sint32)(x - k_LABEL_BOX_SIZE*m_scale), 
							 (sint32)(y - k_LABEL_BOX_SIZE*m_scale), 
							 (sint32)(x + k_LABEL_BOX_SIZE*m_scale), 
							 (sint32)(y + k_LABEL_BOX_SIZE*m_scale)		};

	primitives_PaintRect16(surface, &labelRect, (uint16)color);
}

void TiledMap::DrawNumber(aui_Surface *surface, sint32 num, sint32 color, sint32 x, sint32 y)
{
	if (!surface) surface = m_surface;

	MBCHAR buf[80];
	sprintf(buf, "%d", num);

    UnlockSurface();
    
    primitives_DrawText(surface, x - 8, y - 8, (MBCHAR *)buf, color , 1);

    LockSurface();
}

void TiledMap::SlowDrawText(aui_Surface *surface, char *buf, sint32 color, sint32 x, sint32 y)
{
	if (!surface) surface = m_surface;

    UnlockSurface();
    
    primitives_DrawText(surface, x - 32, y - 8, (MBCHAR *)buf, color , 1);

    LockSurface();

}

void TiledMap::DrawTransitionTile(aui_Surface *surface, const MapPoint &pos, sint32 xpos, sint32 ypos)
{

	Pixel16		*dataPtr;
	sint32		x, y;
	sint32		startX, endX;

	TileInfo	*tileInfo;
	BaseTile	*baseTile, *transitionBuffer;
	uint16		index;
	Pixel16     *transData, *transDataPtr;
	static Pixel16 defaultPixel[4] = {0xf800, 0x07e0, 0x001f, 0xf81f};

	if (!surface) surface = m_surface;

	ypos+=k_TILE_PIXEL_HEADROOM;

    
	if (xpos < 0) 
		return;
	if (xpos > surface->Width() - k_TILE_PIXEL_WIDTH) 
		return;
	if (ypos < 0) 
		return;
	if (ypos > surface->Height() - k_TILE_PIXEL_HEIGHT) 
		return;

	tileInfo = GetTileInfo(pos);

	Assert(tileInfo != NULL);
	if (tileInfo == NULL) 
		return;

	index = tileInfo->GetTileNum();

	baseTile = m_tileSet->GetBaseTile(index);
	if (baseTile == NULL) 
		return;



	Pixel16 *data = baseTile->GetTileData();

	Pixel16	*tileData[4];

	sint32 tilesetIndex = g_theTerrainDB->Get(tileInfo->GetTerrainType())->GetTilesetIndex();

	
	uint16 tilesetIndex_short = (uint16) tilesetIndex;

#ifdef _DEBUG
	Assert(tilesetIndex == ((sint32) tilesetIndex_short));
#endif

	tileData[0] = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(0), 0);
	tileData[1] = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(1), 1);
	tileData[2] = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(2), 2);
	tileData[3] = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(3), 3);
	
	transitionBuffer = m_tileSet->GetBaseTile(static_cast<uint16>((tilesetIndex * 100) + 99));
	if(transitionBuffer) {
		transData = transitionBuffer->GetTileData();
		transDataPtr = transData;
	} else {
		transData = NULL;
		transDataPtr = NULL;
	}

	dataPtr = data;

	uint8 * pSurfBase   = m_surfBase;
	sint32  surfPitch   = m_surfPitch;

	Pixel16 srcPixel;

	uint16 *pDestPixel = (Pixel16 *)(pSurfBase + ypos * surfPitch + 2 * xpos);

	{
		for (y=0; y<k_TILE_PIXEL_HEIGHT; y++) {
			if (y<=23) {
				startX = (23-y)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			} else {
				startX = (y-24)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			}

			if (transDataPtr)
			{
#ifdef _MSC_VER
				_asm {
					mov edx, endX
					mov edi, pDestPixel
					mov esi, dataPtr
					mov ecx, startX
					lea edi, [edi + 2*edx]
					sub ecx, edx
					mov ebx, transDataPtr
				
L0:
					mov dx, [esi]
					add esi, 2
					xor eax, eax
					mov ax, dx
					cmp eax, 4
					jge L1
					mov edx, tileData[4*eax]
					test edx, edx
					jz L2
					add	edx, 2
					mov tileData[4*eax], edx
					mov dx, [edx-2]
					jmp L1
L2:
					mov dx, [ebx]
L1:
					add ebx, 2
					mov [edi + 2*ecx], dx
					inc ecx
					jnz L0
					mov transDataPtr, ebx
					mov dataPtr, esi
				}
#else
				Pixel16* pedx;
                                int edx = endX;
                                uint16* edi = pDestPixel;
                                Pixel16* esi = dataPtr;
                                int ecx = startX;
                                edi += edx;
                                ecx -= edx;
                                Pixel16* ebx = transDataPtr;
L0:
                                Pixel16 dx = *esi;
                                ++esi;
                                Pixel16 ax = dx;
                                if (ax >= 4)
                                    goto L1;
                                pedx = tileData[ax];
                                if (pedx == 0)
                                    goto L2;
                                tileData[ax] = pedx;
                                dx = pedx[-1];
                                goto L1;
L2:
                                dx = *ebx;
L1:
                                ++ebx;
                                edi[ecx] = dx;
                                ++ecx;
                                if (ecx != 0)
                                    goto L0;
                                transDataPtr = ebx;
                                dataPtr = esi;
#endif
			}
			else
			{
				for (x = startX; x<endX; x++) 
				{
					srcPixel = *dataPtr++;
					if (srcPixel < 4)
					{
						Pixel16 *tile = tileData[srcPixel];
						if (tile != NULL)
						{
							tileData[srcPixel]++;
							srcPixel = *tile;
						}
						else
						{
							srcPixel = defaultPixel[srcPixel];
						}
					}
					pDestPixel[x] = srcPixel;
				}
			}
			pDestPixel += (surfPitch>>1);
		}
	}





}

void TiledMap::DrawTransitionTileScaled(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight)
{

	Pixel16		*dataPtr;
	sint32		startX, endX;

	TileInfo	*tileInfo;
	BaseTile	*baseTile, *transitionBuffer;
	uint16		index;
	uint8		*pSurfBase;
	BOOL		wasUnlocked = FALSE;
	AUI_ERRCODE	errcode;
	Pixel16 *transData, *transDataPtr;

	sint32 surfWidth;
	sint32 surfHeight;
	sint32 surfPitch;

	if (!surface) {
		surface = m_surface;

		pSurfBase = m_surfBase;
		surfWidth = m_surfWidth;
		surfHeight = m_surfHeight;
		surfPitch = m_surfPitch;
	} else {

		errcode = surface->Lock(NULL, (LPVOID *)&pSurfBase, 0);
		Assert(errcode == AUI_ERRCODE_OK);
		if (errcode != AUI_ERRCODE_OK) return;

		surfWidth = surface->Width();
		surfHeight = surface->Height();
		surfPitch = surface->Pitch();

		wasUnlocked = TRUE;
	}

	y+=GetZoomTileHeadroom();

	if ((x < 0) ||
		(x > surface->Width() - destWidth) ||
		(y < 0) ||
		(y >= surface->Height() - destHeight)) {

		if (wasUnlocked) {
			
			errcode = surface->Unlock((LPVOID)pSurfBase);
			Assert(errcode == AUI_ERRCODE_OK);
		}
		return;
	}

	
	tileInfo = GetTileInfo(pos);
	Assert(tileInfo != NULL);
	if (tileInfo == NULL) {
		if (wasUnlocked) {
			
			errcode = surface->Unlock((LPVOID)pSurfBase);
			Assert(errcode == AUI_ERRCODE_OK);
		}
		return;
	}

	
	index = tileInfo->GetTileNum();

	
	baseTile = m_tileSet->GetBaseTile(index);
	if (baseTile == NULL) {
		if (wasUnlocked) {
			
			errcode = surface->Unlock((LPVOID)pSurfBase);
			Assert(errcode == AUI_ERRCODE_OK);
		}
		return;
	}

	
	Pixel16 *data = baseTile->GetTileData();

	Pixel16	*t0, *t1, *t2, *t3;

	sint32 tilesetIndex = g_theTerrainDB->Get(tileInfo->GetTerrainType())->GetTilesetIndex();

	
	uint16 tilesetIndex_short = (uint16) tilesetIndex;

	t0 = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(0), 0);
	t1 = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(1), 1);
	t2 = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(2), 2);
	t3 = m_tileSet->GetTransitionData(tilesetIndex_short, tileInfo->GetTransition(3), 3);
	
	transitionBuffer = m_tileSet->GetBaseTile(static_cast<uint16>((tilesetIndex * 100) + 99));
	if(transitionBuffer) {
		transData = transitionBuffer->GetTileData();
		transDataPtr = transData;
	} else {
		transData = NULL;
		transDataPtr = NULL;
	}

	dataPtr = data;

	
	Pixel16 srcPixel, transPixel = 0;
	uint16 *pDestPixel;

	
	sint32				vaccum;
	sint32				vincx, vincxy;
	sint32				vend;
	sint32				vdestpos;
	sint32				vpos1, vpos2;



	vaccum = destHeight*2 - k_TILE_PIXEL_HEIGHT;
	vincx = destHeight*2;
	vincxy = (destHeight - k_TILE_PIXEL_HEIGHT) * 2 ;

	vpos1 = 0;
	vpos2 = (sint32)((double)(k_TILE_PIXEL_HEIGHT - destHeight) / (double)destHeight);

	vdestpos = y;
	vend = k_TILE_PIXEL_HEIGHT - 1;

	
	while ( vpos1 < vend) {
		if (vaccum < 0) {
			vaccum += vincx;

			if (vpos1<=23) {
				startX = (23-vpos1)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			} else {
				startX = (vpos1-24)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			}

			for (sint32 i=startX; i<endX; i++) {
				srcPixel = *dataPtr++;
				if(transData) {
					transPixel = *transDataPtr++;
				}
				switch (srcPixel) {
				case 0x0000 : 
					if (t0)	srcPixel = *t0++;
					else if(transData) srcPixel = transPixel;
						
					break;
				case 0x0001 : if (t1)	srcPixel = *t1++;
					else if(transData) srcPixel = transPixel;
					break;
				case 0x0002 : if (t2) srcPixel = *t2++;
					else if(transData) srcPixel = transPixel;
					break;
				case 0x0003 : if (t3) srcPixel = *t3++;
					else if(transData) srcPixel = transPixel;
					break;
				}
			}
		} else {
			
			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			

			haccum = destWidth * 2 - k_TILE_PIXEL_WIDTH;
			hincx = destWidth * 2;
			hincxy = (destWidth - k_TILE_PIXEL_WIDTH) * 2;
			
			hpos = 0;
			hend = k_TILE_PIXEL_WIDTH - 1;

			
			if (vpos1<=23) {
				startX = (23-vpos1)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			} else {
				startX = (vpos1-24)*2;
				endX = k_TILE_PIXEL_WIDTH - startX;
			}

			hdestpos = x + (sint32)((double)startX * m_scale);

			hpos = startX;
			hend = endX;

			while (hpos < hend) {
				srcPixel = *dataPtr++;
				if(transData)
					transPixel = *transDataPtr++;

				switch (srcPixel) {
				case 0x0000 : 
						if (t0)	srcPixel = *t0++;
						else if(transData) srcPixel = transPixel;
						else srcPixel = 0xF800;
					break;
				case 0x0001 : 
						if (t1)	srcPixel = *t1++;
						else if(transData) srcPixel = transPixel;
						else srcPixel = 0x07E0;
					break;
				case 0x0002 : 
						if (t2) srcPixel = *t2++;
						else if(transData) srcPixel = transPixel;
						else srcPixel = 0x001F; 
					break;
				case 0x0003 : 
						if (t3) srcPixel = *t3++;
						else if(transData) srcPixel = transPixel;
						else srcPixel = 0xF81F;
					break;
				}
				
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					pDestPixel = (Pixel16 *)(pSurfBase + (vdestpos * surfPitch) + (hdestpos << 1));
					*pDestPixel = srcPixel;

					hdestpos++;
				}
				hpos++;
			}

			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}

	if (wasUnlocked) {
		
		errcode = surface->Unlock((LPVOID)pSurfBase);
		Assert(errcode == AUI_ERRCODE_OK);
	}

}

#define k_MAX_WATER_DISPLACEMENTS		14
sint32			inctable[k_MAX_WATER_DISPLACEMENTS] = {1, 2, 1, 0, -1, 1, 0, -1, -2, -1, 0, 1,  -1, 0};
sint32			tinc[2]={0,1};

void TiledMap::DrawWater(void)
{
}

void TiledMap::DrawCityNames(aui_Surface * surf, sint32 layer)
{
	sint32 yoffset = (sint32)(k_TILE_PIXEL_HEADROOM*m_scale)/2;
	bool		fog;
	uint32		slaveBits = 0;

	sint32 surfWidth = surf->Width();
	sint32 surfHeight = surf->Height();

	for (sint32 i = m_mapViewRect.top;i < m_mapViewRect.bottom;i++) {
		for (sint32 j = m_mapViewRect.left;j < m_mapViewRect.right;j++) {
			
			sint32 tileX, tileY;
			maputils_WrapPoint(j,i,&tileX,&tileY);
			
			sint32 mapX, mapY = tileY;
			mapX = maputils_TileX2MapX(tileX,tileY);
			
			MapPoint pos(mapX,mapY);
			
			
			Unit unit;
			
			fog = m_localVision && m_localVision->IsExplored(pos) && !m_localVision->IsVisible(pos);


			if(m_localVision->IsExplored(pos) || g_fog_toggle || g_god) {
				
				BOOL		drawCity = FALSE;
				bool        drawQueueEmpty = false;
				sint32		pop = 0;
                sint32      nextpop = 0;//PFT
				MBCHAR		*name = NULL;
				sint32		owner = 0;

				BOOL	isBioInfected = FALSE, 
						isNanoInfected = FALSE, 
						isConverted = FALSE, 
						isFranchised = FALSE, 
						isInjoined = FALSE, 
						wasHappinessAttacked = FALSE,
						isRioting = FALSE,
						hasAirport = FALSE,
						hasSleepingUnits = FALSE,
						isWatchful = FALSE;
				sint32	bioInfectedOwner=0, 
						nanoInfectedOwner=0, 
						convertedOwner=0, 
						franchiseOwner=0, 
						injoinedOwner=0, 
						happinessAttackOwner=0;

				UnseenCellCarton	ucell;
				// Don't forget if fog was toggled
				if (m_localVision->GetLastSeen(pos, ucell) && !g_fog_toggle) {
					pop = ucell.m_unseenCell->GetCitySize();
					name = (MBCHAR *)ucell.m_unseenCell->GetCityName();
					owner = ucell.m_unseenCell->GetCityOwner();
					isBioInfected = ucell.m_unseenCell->IsBioInfected();
					isNanoInfected = ucell.m_unseenCell->IsNanoInfected();
					isConverted = ucell.m_unseenCell->IsConverted();
					isFranchised = ucell.m_unseenCell->IsFranchised();
					isInjoined = ucell.m_unseenCell->IsInjoined();
					wasHappinessAttacked = ucell.m_unseenCell->WasHappinessAttacked();
					isRioting = ucell.m_unseenCell->IsRioting();
					hasAirport = ucell.m_unseenCell->HasAirport();
					if (owner == g_selected_item->GetVisiblePlayer())
						hasSleepingUnits = ucell.m_unseenCell->HasSleepingUnits();
					else
						hasSleepingUnits = FALSE;
					
					isWatchful = ucell.m_unseenCell->IsWatchful();
					
					bioInfectedOwner = (sint32)ucell.m_unseenCell->m_bioInfectedOwner;
					nanoInfectedOwner = ucell.m_unseenCell->m_nanoInfectedOwner;
					convertedOwner = ucell.m_unseenCell->m_convertedOwner;
					franchiseOwner = ucell.m_unseenCell->m_franchiseOwner;
					injoinedOwner = ucell.m_unseenCell->m_injoinedOwner;
					happinessAttackOwner = ucell.m_unseenCell->m_happinessAttackOwner;
					
					slaveBits = ucell.m_unseenCell->GetSlaveBits();
					
					if (pop > 0)
						drawCity = TRUE;
				}else{
					    //if there's a unit at pos
					if (g_theWorld->GetTopVisibleUnit(pos,unit)) {
                        //and it's a city visible to the current player
						if (unit.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()) && unit.IsCity()) {
							CityData *cityData = unit.GetData()->GetCityData();
							
							if (!unit.GetActor()) {
								pop = cityData->PopCount();
                                nextpop = cityData->TurnsToNextPop();
								owner = cityData->GetOwner();
							} else {
								pop = unit.GetActor()->GetSize();
                                nextpop = unit.GetActor()->GetNextPop();
								owner = unit.GetActor()->GetPlayerNum();
							}
							
							name = cityData->GetName();
							isBioInfected = cityData->IsBioInfected();
							isNanoInfected = cityData->IsNanoInfected();
							isConverted = cityData->IsConverted();
							isFranchised = cityData->IsFranchised();
							isInjoined = cityData->IsInjoined();
							wasHappinessAttacked = cityData->WasHappinessAttacked();
							isWatchful = cityData->IsWatchful();
							
							bioInfectedOwner = cityData->GetOwner();
							nanoInfectedOwner = cityData->GetOwner();
							convertedOwner = cityData->IsConvertedTo();
							franchiseOwner = cityData->GetFranchiseOwner();
							injoinedOwner = cityData->GetOwner();
							happinessAttackOwner = cityData->GetOwner();
							
							slaveBits = cityData->GetSlaveBits();
							
							isRioting = cityData->GetIsRioting();
							hasAirport = cityData->HasAirport();
							if (owner == g_selected_item->GetVisiblePlayer())
								hasSleepingUnits = cityData->HasSleepingUnits();
							else
								hasSleepingUnits = FALSE;
							drawCity = TRUE;
							if(owner == g_selected_item->GetVisiblePlayer() &&
							   !cityData->GetBuildQueue()->GetHead()) {
								drawQueueEmpty = true;
							}
						}
					}
				}

				if (drawCity) {//it's a city now visible to the current player
					
					sint32		x,y;
					maputils_MapXY2PixelXY(mapX,mapY,&x,&y);//change map co-ordinates to pixel co-ordinates
										
					y-= yoffset;
					
					sint32 width, height;
					RECT rect;//the city name rectangle
					RECT boxRect;//boxRect-rect will = the player colored border for the city name
					RECT clipRect;//working surface

					if (x >= 0 && y >= 0 && x < surfWidth && y < surfHeight) {//it's on the screen
						if (m_font) {
							width = m_font->GetStringWidth(name);
							height = m_font->GetMaxHeight();

							Pixel16			pixelColor;
                            //get the proper colors for the city's owner
							if (fog) {
								pixelColor = g_colorSet->GetDarkPlayerColor(owner);
							} else {
								pixelColor = g_colorSet->GetPlayerColor(owner);
							}
							COLORREF color = GetColorRef(g_colorSet->ComputePlayerColor(owner), fog);
							//define rect's screen co-ordinates
							rect.left = x;
							rect.top = y;
							rect.right = x+width;
							rect.bottom = y+height;

							boxRect = rect;//copy rect to boxRect

							InflateRect(&boxRect, 2, 1);//expand boxRect to allow for borders
							
							clipRect = boxRect;//copy boxRect to the working surface clipRect
                            //adjust clipRect to fit on the screen
							if (clipRect.left < 0) clipRect.left = 0;
							if (clipRect.top < 0) clipRect.top = 0;
							if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
							if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;
                            //color clipRect BLACK
							primitives_PaintRect16(surf, &clipRect, GetColor(COLOR_BLACK));
							
							InflateRect(&boxRect, 1, 1);//get ready to do borders (clipRect - boxRect now= a one pixel border)
							                            // this is only for the cityname box EMOD note    

							clipRect = boxRect;//copy boxRect to the working surface clipRect
                            //adjust clipRect to fit on the screen
							if (clipRect.left < 0) clipRect.left = 0;
							if (clipRect.top < 0) clipRect.top = 0;
							if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
							if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;

							//color clipRect to be the player's color (this creates the border)
							primitives_FrameRect16(surf, &clipRect, pixelColor);

							//get the color for the city's name text
							COLORREF nameColor;							
							if (fog) {
								nameColor = GetColorRef(COLOR_WHITE, fog);
							} else {
								if (isRioting) {
									nameColor = GetColorRef(COLOR_RED);
								} else if(drawQueueEmpty) {
									nameColor = GetColorRef(COLOR_YELLOW);
								} else {
									nameColor = GetColorRef(COLOR_WHITE);
								}
							}

							//copy rect to clipRect (this is now the interior rectangle that holds the city name)
							clipRect = rect;
							//adjust to fit on the screen
							if (clipRect.left < 0) clipRect.left = 0;
							if (clipRect.top < 0) clipRect.top = 0;
							if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
							if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;
                            //draw the city name
							m_font->DrawString(surf, &rect, &clipRect, name, 0, nameColor, 0);

							AddDirtyRectToMix(boxRect);
							
                            //start on the city's pop rectangle
                            //put the city's pop in str
							MBCHAR str[80];
							sprintf(str,"%i",pop);
							//the top line of the pop rectangle
							y = boxRect.bottom + 1;


							width = m_font->GetStringWidth(str);
							if (width < k_POP_BOX_SIZE_MINIMUM)
								width = k_POP_BOX_SIZE_MINIMUM;
							height = m_font->GetMaxHeight();

							// the pop rectangle
							RECT popRect = {0, 0, width+4, height+4};
                            //add the top left co-ordinates
							OffsetRect(&popRect, boxRect.left, y);

							//get the proper color for the city's owner
							if (fog) {
								pixelColor = g_colorSet->GetDarkPlayerColor(owner);
							} else {
								pixelColor = g_colorSet->GetPlayerColor(owner);
							}

							//copy popRect to the working surface clipRect
							clipRect = popRect;
                            //adjust clipRect's screen location
							if (clipRect.left < 0) clipRect.left = 0;
							if (clipRect.top < 0) clipRect.top = 0;
							if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
							if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;

							//paint clipRect's surface the proper player color and give it a black frame
							primitives_PaintRect16(surf, &clipRect, pixelColor);
							primitives_FrameRect16(surf, &clipRect, GetColor(COLOR_BLACK));
							
							//width and height of the pop number 
							width = m_font->GetStringWidth(str);
							height = m_font->GetMaxHeight();
                            //this rect will be the inner rect that shows the pop number
							RECT		rect = {0, 0, width, height};

							OffsetRect(&rect, popRect.left + (popRect.right-popRect.left)/2 -
											width/2,
											popRect.top + (popRect.bottom-popRect.top)/2 -
											height/2);


							//copy rect to clipRect (and adjust screen location)
							clipRect = rect;
							if (clipRect.left < 0) clipRect.left = 0;
							if (clipRect.top < 0) clipRect.top = 0;
							if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
							if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;
                            //draw the pop number in black
							m_font->DrawString(surf, &rect, &clipRect, str, 
								0, 
								GetColorRef(COLOR_BLACK),
								0);

							//move two pixels right and do it again ? 
							OffsetRect(&rect, 2, 0);
							clipRect = rect;
							if (clipRect.left < 0) clipRect.left = 0;
							if (clipRect.top < 0) clipRect.top = 0;
							if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
							if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;
							m_font->DrawString(surf, &rect, &clipRect, str, 
								0, 
								GetColorRef(COLOR_BLACK),
								0);

							//move one pixel left and do it again ?
							OffsetRect(&rect, -1, 0);

							clipRect = rect;
							if (clipRect.left < 0) clipRect.left = 0;
							if (clipRect.top < 0) clipRect.top = 0;
							if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
							if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;

							m_font->DrawString(surf, &rect, &clipRect, str, 
								0, 
								GetColorRef(COLOR_WHITE, fog),
								0);

							popRect.bottom++;
							popRect.right++;

							AddDirtyRectToMix(rect);

							//top left co-ordinates of nextPop rect
							x = popRect.left;
                            y = popRect.bottom + 1;

							// nextpop rect, PFT
							if (owner == g_selected_item->GetVisiblePlayer())
                            {
                                //put the number of turns until the city's nextpop in str
								MBCHAR strn[80];
                                if (nextpop < THRESHOLD_SLOW_GROWTH)
                                {
                					 sprintf(strn, "%i", nextpop);
                                }
                                else
                                {
                                     sprintf(strn, "%s", TEXT_NONE);
                                }
                                //width and height of the pop number 
								width = m_font->GetStringWidth(strn);
								if (width < k_POP_BOX_SIZE_MINIMUM)
									width = k_POP_BOX_SIZE_MINIMUM;
								height = m_font->GetMaxHeight();

							    //RECT for next pop
								RECT popRectn = {0, 0, width+4, height+4};
                                //add the top left co-ordinates
								OffsetRect(&popRectn, x, y);

								//get the proper color for the city's owner
								if (fog) {
									pixelColor = g_colorSet->GetDarkPlayerColor(owner);
								} else {
									pixelColor = g_colorSet->GetPlayerColor(owner);
								}

								//copy popRectn to the working surface clipRect
								clipRect = popRectn;
                                //adjust clipRect's screen location
								if (clipRect.left < 0) clipRect.left = 0;
								if (clipRect.top < 0) clipRect.top = 0;
								if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
								if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;

								//paint clipRect's surface the proper player color and give it a black frame
								primitives_PaintRect16(surf, &clipRect, pixelColor);
								primitives_FrameRect16(surf, &clipRect, GetColor(COLOR_BLACK));
	
								//width and height of the nextpop number	
								width = m_font->GetStringWidth(strn);
								height = m_font->GetMaxHeight();

                                //this rect will be the inner rect that shows the nextpop number
								RECT		rect = {0, 0, width, height};

								OffsetRect(&rect, popRectn.left + (popRectn.right-popRectn.left)/2 -
												width/2,
												popRectn.top + (popRectn.bottom-popRectn.top)/2 -
												height/2);


								//copy rect to clipRect (and adjust screen location)
								clipRect = rect;
								if (clipRect.left < 0) clipRect.left = 0;
								if (clipRect.top < 0) clipRect.top = 0;
								if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
								if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;
                                //draw the nextpop number in black
								m_font->DrawString(surf, &rect, &clipRect, strn, 
									0, 
									GetColorRef(COLOR_BLACK),
									0);

								
								OffsetRect(&rect, 2, 0);
								clipRect = rect;
								if (clipRect.left < 0) clipRect.left = 0;
								if (clipRect.top < 0) clipRect.top = 0;
								if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
								if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;
								m_font->DrawString(surf, &rect, &clipRect, strn, 
									0, 
									GetColorRef(COLOR_BLACK),
									0);

								
								OffsetRect(&rect, -1, 0);

								clipRect = rect;

								if (clipRect.left < 0) clipRect.left = 0;
								if (clipRect.top < 0) clipRect.top = 0;
								if (clipRect.right >= surf->Width()) clipRect.right = surf->Width() - 1;
								if (clipRect.bottom >= surf->Height()) clipRect.bottom = surf->Height() - 1;

								m_font->DrawString(surf, &rect, &clipRect, strn, 
									0, 
									GetColorRef(COLOR_WHITE, fog),
									0);

								popRectn.bottom++;
								popRectn.right++;

								AddDirtyRectToMix(rect);

							}
						} else 
							continue;
					}
					DrawCityIcons(surf, pos, owner, fog, rect,
								isBioInfected, isNanoInfected, isConverted, 
								isFranchised, isInjoined, wasHappinessAttacked,
								bioInfectedOwner, nanoInfectedOwner, convertedOwner,
								franchiseOwner, injoinedOwner, happinessAttackOwner, 
								slaveBits, isRioting, hasAirport, hasSleepingUnits,
								isWatchful);

				}
			}
		}
	}
}
//----------------------------------------------------------------------------
//
// Name       : TiledMap::DrawCityIcons
//
// Description: Draw the temporary icons above a city
//
// Parameters : aui_Surface *     surf                 : the surface to draw on
//              MapPoint          &pos                 : the city's position
//              sint32            owner                : the city's owner
//              BOOL              fog                  : if TRUE then draw city under fog of war
//              RECT              &popRect             : the rectangle that contains the city's pop number
//                                                       (used to test screen location)
//              BOOL              isBioInfected        : if TRUE then draw MAPICON_BIODISEASE
//              BOOL              isNanoInfected       : etc.
//              BOOL              isConverted          :
//              BOOL              isFranchised         :
//              BOOL              isInjoined           :
//              BOOL              wasHappinessAttacked :
//              sint32            bioInfectedOwner     : not used (set to the city's owner)
//              sint32            nanoInfectedOwner    : ditto.
//              sint32            convertedOwner       : the player who converted the city
//              sint32            franchiseOwner       : the player who franchised the city
//              sint32            injoinedOwner        : not used (set to the city's owner)
//              sint32            happinessAttackOwner : ditto
//              uint32            slaveBits            : which other player's citizens are enslaved here
//              BOOL              isRioting            : if TRUE then draw MAPICON_UPRISING
//              BOOL              hasAirport           : not used
//              BOOL              hasSleepingUnits     : if TRUE then draw MAPICON_SLEEPINGUNITS
//              BOOL              isWatchful           : if TRUE then draw MAPICON_WATCHFUL
//
// Globals    : 
//				
// Returns    : 
//
// Remark(s)  : 
//
//----------------------------------------------------------------------------
void TiledMap::DrawCityIcons(aui_Surface *surf, MapPoint const & pos, sint32 owner, bool fog, RECT &popRect,
								BOOL isBioInfected, BOOL isNanoInfected, BOOL isConverted, 
								BOOL isFranchised, BOOL isInjoined, BOOL wasHappinessAttacked,
								sint32 bioInfectedOwner, sint32 nanoInfectedOwner, sint32 convertedOwner,
								sint32 franchiseOwner, sint32 injoinedOwner, sint32 happinessAttackOwner,
								uint32 slaveBits, BOOL isRioting, BOOL hasAirport, BOOL hasSleepingUnits,
								BOOL isWatchful)
{
	TileSet	*   tileSet     = GetTileSet();
	POINT       iconDim     = tileSet->GetMapIconDimensions(MAPICON_BIODISEASE);
	Pixel16     color       = (fog) 
                              ? g_colorSet->GetDarkPlayerColor(owner)
                              : g_colorSet->GetPlayerColor(owner);

    RECT		iconRect;
	iconRect.left = popRect.right + 3;
	iconRect.right = iconRect.left + iconDim.x + 1;
	iconRect.top = popRect.top;
	iconRect.bottom = iconRect.top + iconDim.y + 1;







	
	
	if (iconRect.left < 0 || iconRect.top < 0 || 
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	
	Pixel16 *cityIcon;

	if (isBioInfected) {
		cityIcon = tileSet->GetMapIconData(MAPICON_BIODISEASE);
		Assert(cityIcon); 
		if (!cityIcon) return;
		if (++g_bio_flash > 10) {
			DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, GetColor(COLOR_YELLOW));
			g_bio_flash = 0;
		} else {
			DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		}

		AddDirtyRectToMix(iconRect);

		iconRect.left += iconDim.x;
		iconRect.right += iconDim.x;
	}

	
	if (iconRect.left < 0 || iconRect.top < 0 || 
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	if (isNanoInfected) {
		cityIcon = tileSet->GetMapIconData(MAPICON_NANODISEASE);
		Assert(cityIcon); if (!cityIcon) return;
		Pixel16 const flashColor = GetColor(COLOR_YELLOW, fog);

		if (++g_nano_flash > 10) {
			g_nano_flash = 0;
			DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, flashColor);
		} else {
			DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		}

		AddDirtyRectToMix(iconRect);

		iconRect.left += iconDim.x;
		iconRect.right += iconDim.x;
	}

	
	if (iconRect.left < 0 || iconRect.top < 0 || 
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	if (isConverted) {
		cityIcon = tileSet->GetMapIconData(MAPICON_CONVERSION);
		Assert(cityIcon); if (!cityIcon) return;

		if (fog)
			color = g_colorSet->GetDarkPlayerColor(convertedOwner);
		else
			color = g_colorSet->GetPlayerColor(convertedOwner);

		DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		AddDirtyRectToMix(iconRect);

		iconRect.left += iconDim.x;
		iconRect.right += iconDim.x;
	}

		
	if (iconRect.left < 0 || iconRect.top < 0 || 
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	if (isFranchised) {
		cityIcon = tileSet->GetMapIconData(MAPICON_FRANCHISE);
		Assert(cityIcon); 
		if (!cityIcon) return;
		if (fog)
			color = g_colorSet->GetDarkPlayerColor(franchiseOwner);
		else
			color = g_colorSet->GetPlayerColor(franchiseOwner);
		DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		AddDirtyRectToMix(iconRect);

		iconRect.left += iconDim.x;
		iconRect.right += iconDim.x;
	}

	
	if (iconRect.left < 0 || iconRect.top < 0 || 
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	if (isInjoined) {
		cityIcon = tileSet->GetMapIconData(MAPICON_INJUNCTION);
		Assert(cityIcon); 
		if (!cityIcon) return;

		color = GetColor(COLOR_YELLOW, fog);
		DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		AddDirtyRectToMix(iconRect);

		iconRect.left += iconDim.x;
		iconRect.right += iconDim.x;
	}

	
	if (iconRect.left < 0 || iconRect.top < 0 || 
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	if (wasHappinessAttacked) {
		cityIcon = tileSet->GetMapIconData(MAPICON_UNHAPPY);
		Assert(cityIcon); 
		if (!cityIcon) return;

		color = GetColor(COLOR_YELLOW, fog);
		DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		AddDirtyRectToMix(iconRect);

		iconRect.left += iconDim.x;
		iconRect.right += iconDim.x;
	}

	
	uint32 bits = slaveBits;
	for (sint32 i=0; i<k_MAX_PLAYERS; i++) {
		if (bits & 1) {
			cityIcon = tileSet->GetMapIconData(MAPICON_SLAVE);
			iconDim = tileSet->GetMapIconDimensions(MAPICON_SLAVE);
			Assert(cityIcon);
			if (!cityIcon) return;
			if (fog)
				color = g_colorSet->GetDarkPlayerColor(i);
			else
				color = g_colorSet->GetPlayerColor(i);

			DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);

			AddDirtyRectToMix(iconRect);

			iconRect.left += iconDim.x;
			iconRect.right += iconDim.x;
		}

		bits >>= 1;
	}

	if (isRioting) {
		cityIcon = tileSet->GetMapIconData(MAPICON_UPRISING);
		Assert(cityIcon); 
		if (!cityIcon) return;
		iconDim = tileSet->GetMapIconDimensions(MAPICON_UPRISING);

		color = GetColor(COLOR_YELLOW, fog);
		DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		AddDirtyRectToMix(iconRect);

		iconRect.left += iconDim.x;
		iconRect.right += iconDim.x;
	}

	if (hasSleepingUnits) {
		cityIcon = tileSet->GetMapIconData(MAPICON_SLEEPINGUNITS);
		Assert(cityIcon); 
		if (!cityIcon) return;
		cityIcon = tileSet->GetMapIconData(MAPICON_SLEEPINGUNITS);
		iconDim = tileSet->GetMapIconDimensions(MAPICON_SLEEPINGUNITS);

		color = GetColor(COLOR_YELLOW, fog);
		DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		AddDirtyRectToMix(iconRect);

		iconRect.left += iconDim.x;
		iconRect.right += iconDim.x;
	}

	if (isWatchful) {
		cityIcon = tileSet->GetMapIconData(MAPICON_WATCHFUL);
		Assert(cityIcon); 
		if (!cityIcon) return;
		cityIcon = tileSet->GetMapIconData(MAPICON_WATCHFUL);
		iconDim = tileSet->GetMapIconDimensions(MAPICON_WATCHFUL);

		color = GetColor(COLOR_YELLOW, fog);
		DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		AddDirtyRectToMix(iconRect);

		iconRect.left += iconDim.x;
		iconRect.right += iconDim.x;
	}
}








sint32 TiledMap::DrawColorBlendedOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, Pixel16 color, sint32 blendValue)
{
	uint8			*surfBase;
	sint32			surfWidth;
	sint32			surfHeight;
	sint32			surfPitch;
	sint32			errcode;

	if (data == NULL) return 0;

	if (surface) {
		errcode = surface->Lock(NULL, (LPVOID *)&surfBase, 0);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACELOCKFAILED;

		
		surfWidth = surface->Width();
		surfHeight = surface->Height();
		surfPitch = surface->Pitch();
	} else {
		surfBase = m_surfBase;
		surfWidth = m_surfWidth;
		surfHeight = m_surfHeight;
		surfPitch = m_surfPitch;
	}

	
	
	
	if (!surfBase) return 0;

	
	
	
	if (x < 0 || 
		x >= (surfWidth - k_TILE_GRID_WIDTH) ||
		y < 0 || 
		y >= (surfHeight - k_TILE_GRID_HEIGHT)) {
		if (surface) {
			
			errcode = surface->Unlock((LPVOID *)surfBase);
			Assert(errcode == AUI_ERRCODE_OK);
			if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}

		return 0;
	}
	




	
	unsigned short	*destPixel;

	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);

	sint32 j;
	sint32 len;

	
	for(j=start; j<=end; j++) {
		Pixel16		*rowData;
		Pixel16		tag;

		destPixel = (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));

		if ((y+j) >= surfHeight) continue;
		if ((sint16)table[j-start] == -1) continue;

		rowData = dataStart + table[j-start];
		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
						destPixel += (tag & 0x00FF);
					break;
				case k_TILE_COPY_RUN_ID			: {
						len = (tag & 0x00FF);

						while (len--) {
							*destPixel = pixelutils_BlendFast(color, *rowData++, blendValue);
							destPixel++;
						}
					}
					break;
				case k_TILE_SHADOW_RUN_ID		: {
						len = (tag & 0x00FF);
						while (len--) {
							*destPixel = pixelutils_Shadow(*destPixel);
							destPixel++;
						}
					}
					break;
			}

		} while ((tag & 0xF000) == 0);
	}

	if (surface) {
		
		errcode = surface->Unlock((LPVOID *)surfBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return AUI_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return 0;
}



void TiledMap::DrawColorBlendedOverlayScaled(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, 
								 sint32 destWidth, sint32 destHeight, Pixel16 color, sint32 blendValue)
{
	uint8			*surfBase = NULL, *origBase = NULL;
	AUI_ERRCODE		errcode;

	if (data == NULL) return;

	sint32	surfWidth, surfHeight, surfPitch;

	if (surface) {
		errcode = surface->Lock(NULL, (LPVOID *)&origBase, 0);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return;

		
		surfBase = origBase;
		surfWidth = surface->Width();
		surfHeight = surface->Height();
		surfPitch = surface->Pitch();
	} else {
		surfBase = m_surfBase;
		surfWidth = m_surfWidth;
		surfHeight = m_surfHeight;
		surfPitch = m_surfPitch;
	}


	if ((x < 0) ||(y < 0) || (x >= surface->Width() - destWidth) || (y >= surface->Height() - destHeight)) {
		if (surface) {
			
			errcode = surface->Unlock((LPVOID *)origBase);
			Assert(errcode == AUI_ERRCODE_OK);
			if ( errcode != AUI_ERRCODE_OK ) return;
		}
	}

	
	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	surfBase += (y * surfPitch + x * 2);

	
	Pixel16			*destPixel;
	uint16			vstart = (uint16)*data++;
	uint16			end = (uint16)*data++;
	Pixel16			*table = data;

	Pixel16			*dataStart = table + (end - vstart + 1);

	sint32 vaccum = destHeight*2 - k_TILE_GRID_HEIGHT;
	sint32 vincx = destHeight*2;
	sint32 vincxy = (destHeight-k_TILE_GRID_HEIGHT) * 2 ;
	sint32 vpos1 = 0;
	sint32 vpos2 = (sint32)((double)((end+1) - destHeight) / (double)destHeight);
	sint32 vdestpos = y;
	sint32 vend = (end+1) - 1;

	
	while ( vpos1 < vend) {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			

			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos1++;
				vpos2++;
				continue;
			}

			Pixel16		*rowData1, *rowData2;
			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;

			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			
			if (vpos1 < vstart) rowData1 = emptyRow;
			else rowData1 = dataStart + table[vpos1-vstart];

			if (vpos2 < vstart) rowData2 = emptyRow;
			else rowData2 = dataStart + table[vpos2-vstart];

			haccum = destWidth*2 - k_TILE_GRID_WIDTH;
			hincx = destWidth*2;
			hincxy = (destWidth-k_TILE_GRID_WIDTH) * 2;
			
			hpos = 0;
			hdestpos = x;
			hend = k_TILE_GRID_WIDTH-1;

			
			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, 0);

			while (hpos < hend) {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					destPixel = (Pixel16 *)(surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));
		
					
					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, 0);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {
						
						pixel = pixel3;

						

						*destPixel = pixelutils_BlendFast(color, pixel, blendValue);
					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
				hpos++;
			}

			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}

	if (surface) {
		
		errcode = surface->Unlock((LPVOID *)origBase);
		Assert(errcode == AUI_ERRCODE_OK);
		if ( errcode != AUI_ERRCODE_OK ) return;
	}
}






void			
TiledMap::SetTerrainOverlay(TerrainImprovementRecord *rec,MapPoint &pos,Pixel16 color)
{
	m_overlayActive	=true;
	m_overlayRec	=rec;
	m_overlayPos	=pos;
	m_overlayColor	=color;
}




void			
TiledMap::DrawTerrainOverlay(aui_Surface *surf)
{
	if (!m_overlayActive)
		return;

	m_overlayActive=false;
	
	
	if (m_overlayRec==NULL)
		return;

	const TerrainImprovementRecord::Effect *effect = NULL;
   
	if(!m_overlayRec->GetClassTerraform()) {
		effect  = terrainutil_GetTerrainEffect(m_overlayRec, m_overlayPos);
	} else {
		effect = m_overlayRec->GetTerrainEffect(0);
	}

	if (effect==NULL)
		return;

	
	sint32 index   = effect->GetTilesetIndex();

	
	Pixel16 *data    = m_tileSet->GetImprovementData((uint16)index);

	if (data==NULL)
		return;

	sint32 x,y;
	
	sint32	destWidth  = k_TILE_PIXEL_WIDTH;  
	sint32	destHeight = k_TILE_GRID_HEIGHT;

	
	maputils_MapXY2PixelXY(m_overlayPos.x,m_overlayPos.y,&x,&y);

	
	if (GetZoomLevel() == k_ZOOM_NORMAL) 
		DrawBlendedOverlayIntoMix(data,x, y, m_overlayColor, k_FOW_BLEND_VALUE);
	else
	{
		destWidth	= GetZoomTilePixelWidth(), 
		destHeight	= GetZoomTileGridHeight();

		DrawBlendedOverlayScaledIntoMix(data, x, y, destWidth, destHeight, m_overlayColor, k_FOW_BLEND_VALUE);
	}
	AddDirtyToMix(x, y, destWidth, destHeight);
}







void            
TiledMap::DrawAnImprovement(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y,bool fog,bool clip)
{
	if (fog) 
	{
		if (m_zoomLevel!=k_ZOOM_NORMAL) 
			DrawBlendedOverlayScaled(surface, data, x, y, 
				GetZoomTilePixelWidth(), 
				GetZoomTileGridHeight(), 
				k_FOW_COLOR,
				k_FOW_BLEND_VALUE);
		else 
			DrawBlendedOverlay(surface, data, x, y, k_FOW_COLOR, k_FOW_BLEND_VALUE);
	} 
	else 
	{
		if (m_zoomLevel != k_ZOOM_NORMAL) 
			DrawScaledOverlay(surface, data, x, y, 
				GetZoomTilePixelWidth(), 
				GetZoomTileGridHeight());
		else 
			DrawOverlay(surface, data, x, y);
	}
}

sint32 TiledMap::GetVisibleCellOwner(MapPoint &pos)
{
	if(!m_localVision->IsVisible(pos) 
	&& g_selected_item->GetVisiblePlayer() != g_theWorld->GetCell(pos)->GetOwner()
	){
		UnseenCellCarton ucell;
		if(m_localVision->GetLastSeen(pos, ucell)) {
			return ucell.m_unseenCell->m_cell_owner;
		}
	}
	return g_theWorld->GetCell(pos)->GetOwner();
}

uint32 TiledMap::GetVisibleCityOwner(MapPoint &pos)
{
// Added by Martin Gühmann
	if(!m_localVision->IsVisible(pos) 
	// Show the city influence radius from the last visit.
	&& g_selected_item->GetVisiblePlayer() != g_theWorld->GetCell(pos)->GetOwner()
	){
		UnseenCellCarton ucell;
		if(m_localVision->GetLastSeen(pos, ucell)) {
			return ucell.m_unseenCell->GetVisibleCityOwner(); 
		}
	}
	return g_theWorld->GetCell(pos)->GetCityOwner().m_id;
}

void TiledMap::DrawNationalBorders(aui_Surface *surface, MapPoint &pos)
{
	sint32 myOwner = GetVisibleCellOwner(pos);
	sint32 neighborOwner;
	uint32 myCityOwner = GetVisibleCityOwner(pos);
	uint32 neighborCityOwner;

	if(myOwner < 0)
		return;

	Pixel16 color = g_colorSet->GetPlayerColor(myOwner);
	Pixel16 white = GetColor(COLOR_WHITE);

	if(!surface) surface = m_surface;

	Player *visP = g_player[g_selected_item->GetVisiblePlayer()];

	
	if (visP == NULL)
		return;

	Unit myCity(myCityOwner);
	UnitData *myCityData = myCity.IsValid() ? myCity.AccessData() : NULL;

	MapPoint neighbor;
	if(pos.GetNeighborPosition(NORTHWEST, neighbor)) {
		neighborOwner = GetVisibleCellOwner(neighbor);
		if(neighborOwner != myOwner 
		&&(visP->HasSeen(myOwner)
		|| g_fog_toggle // The sense of fog of and god mode is to see something.
		|| g_god)
		&& g_theProfileDB->GetShowPoliticalBorders()
		){
			DrawColoredBorderEdge(surface, pos, color, NORTHWEST, k_BORDER_SOLID); //EMOD- k_BORDER_SOLID defined in tiledmap.h as 0 and dashed as 1 its a bool?
		}

		neighborCityOwner = GetVisibleCityOwner(neighbor);		
		if(neighborCityOwner != myCityOwner) {
			if(myCityData && 
			   myCityData->GetVisibility() & (1 << visP->m_owner) &&
			   g_theProfileDB->IsShowCityInfluence()) {
				DrawColoredBorderEdge(surface, pos, white, NORTHWEST, k_BORDER_DASHED);
			}
		}
	}

	if(pos.GetNeighborPosition(SOUTHWEST, neighbor)) {
		neighborOwner = GetVisibleCellOwner(neighbor);
		if(neighborOwner != myOwner 
		&&(visP->HasSeen(myOwner)
		|| g_fog_toggle
		|| g_god)
		&& g_theProfileDB->GetShowPoliticalBorders()
		){
			DrawColoredBorderEdge(surface, pos, color, SOUTHWEST, k_BORDER_SOLID);
		}		
		neighborCityOwner = GetVisibleCityOwner(neighbor);
		if(neighborCityOwner != myCityOwner) {
			if(myCityData &&
			   myCityData->GetVisibility() & (1 << visP->m_owner) &&
			   g_theProfileDB->IsShowCityInfluence()) {
				DrawColoredBorderEdge(surface, pos, white, SOUTHWEST, k_BORDER_DASHED);
			}
		}
	}

	if(pos.GetNeighborPosition(NORTHEAST, neighbor)) {
		neighborOwner = GetVisibleCellOwner(neighbor);
		if(neighborOwner != myOwner 
		&&(visP->HasSeen(myOwner)
		|| g_fog_toggle
		|| g_god)
		&& g_theProfileDB->GetShowPoliticalBorders()
		){
			DrawColoredBorderEdge(surface, pos, color, NORTHEAST, k_BORDER_SOLID);
		}
		neighborCityOwner = GetVisibleCityOwner(neighbor);
		if(neighborCityOwner != myCityOwner) {
			if(myCityData &&
			   myCityData->GetVisibility() & (1 << visP->m_owner) &&
			   g_theProfileDB->IsShowCityInfluence()) {
				DrawColoredBorderEdge(surface, pos, white, NORTHEAST, k_BORDER_DASHED);
			}
		}
	}

	if(pos.GetNeighborPosition(SOUTHEAST, neighbor)) {
		neighborOwner = GetVisibleCellOwner(neighbor);
		if(neighborOwner != myOwner 
		&&(visP->HasSeen(myOwner)
		|| g_fog_toggle
		|| g_god)
		&& g_theProfileDB->GetShowPoliticalBorders()
		){
			DrawColoredBorderEdge(surface, pos, color, SOUTHEAST, k_BORDER_SOLID);
		}
		neighborCityOwner = GetVisibleCityOwner(neighbor);
		if(neighborCityOwner != myCityOwner) {
			if(myCityData &&
			   myCityData->GetVisibility() & (1 << visP->m_owner) &&
			   g_theProfileDB->IsShowCityInfluence()) {
				DrawColoredBorderEdge(surface, pos, white, SOUTHEAST, k_BORDER_DASHED);
			}
		}
	}
}


#define k_NUM_CHAT_LINES 5

void TiledMap::DrawChatText()
{
	if(g_screenManager && g_screenManager->GetSurface())
	{
		if(m_font)
		{
			sint32 height = m_font->GetMaxHeight();
			sint32 x = 150;
			sint32 y = k_NUM_CHAT_LINES * height + 100;
			RECT rect;
			m_chatRect.left = rect.left = x;
			m_chatRect.right = rect.right = x;
			m_chatRect.top = rect.top = y + height;
			m_chatRect.bottom = rect.bottom = y + height;
			sint32 c = 0;

			aui_Surface *tempSurf = g_screenManager->GetSurface();
			g_screenManager->UnlockSurface();

			
			RECT timeRect = { x, 100, x, 100 + height};
			char timebuf[256];
			if(g_network.IsActive()) {
				if(g_network.IsSpeedStyle() && g_selected_item->GetCurPlayer() == g_selected_item->GetVisiblePlayer()) {
					time_t const timeleft = g_network.GetTurnEndsAt() - time(0);
					sprintf(timebuf, "%s: %d", g_theStringDB->GetNameStr("NETWORK_TIME_LEFT"), timeleft);
					timeRect.right = timeRect.left + m_font->GetStringWidth(timebuf);
					m_font->DrawString(tempSurf, &timeRect, &timeRect, timebuf, 0, GetColorRef(COLOR_BLACK), 0);
					OffsetRect(&timeRect, -1, -1);
					m_font->DrawString(tempSurf, &timeRect, &timeRect, timebuf, 0, GetColorRef(COLOR_WHITE), 0);
					timeRect.right++;
					timeRect.bottom++;
					AddDirtyRectToMix(timeRect);
				} else {
					sprintf(timebuf, "%s: %s", g_theStringDB->GetNameStr("NETWORK_CURRENT_PLAYER"), g_player[g_selected_item->GetCurPlayer()] ? g_player[g_selected_item->GetCurPlayer()]->m_civilisation->GetLeaderName() : "---");
					timeRect.right = timeRect.left + m_font->GetStringWidth(timebuf);
					m_font->DrawString(tempSurf, &timeRect, &timeRect, timebuf, 0, GetColorRef(COLOR_BLACK), 0);
					OffsetRect(&timeRect, -1, -1);
					COLOR      color = g_colorSet->ComputePlayerColor(g_selected_item->GetCurPlayer());
					m_font->DrawString(tempSurf, &timeRect, &timeRect, timebuf, 0, GetColorRef(color), 0);
					timeRect.right++;
					timeRect.bottom++;
					AddDirtyRectToMix(timeRect);
				}
			}

			PointerList<ChatText>::Walker walk(&g_network.GetChatList()->m_list);
			for(; walk.IsValid() && c < k_NUM_CHAT_LINES; walk.Next(), c++) 
			{
				rect.right = rect.left + m_font->GetStringWidth(walk.GetObj()->m_text);
                m_chatRect.right = std::max(m_chatRect.right, rect.right);
				rect.top -= height;
				m_chatRect.top -= height;
				COLOR      color = g_colorSet->ComputePlayerColor(walk.GetObj()->m_sender);
				m_font->DrawString(tempSurf, &rect, &rect, walk.GetObj()->m_text, 0, GetColorRef(COLOR_BLACK), 0);
				OffsetRect(&rect, -1, -1);
				m_font->DrawString(tempSurf, &rect, &rect, walk.GetObj()->m_text, 0, GetColorRef(color), 0);
				OffsetRect(&rect, 1, 1);
				rect.bottom -= height;
			}
			g_screenManager->LockSurface(tempSurf);

			if(c > 0) {
				m_chatRect.right++;
				m_chatRect.bottom++;
				AddDirtyRectToMix(m_chatRect);
			}
		}
	}
}

void TiledMap::AddChatDirtyRectToMap()
{
	if(m_chatRect.top != m_chatRect.bottom &&
		m_chatRect.left != m_chatRect.right) {
		AddDirtyRectToMap(m_chatRect);
	}
}
