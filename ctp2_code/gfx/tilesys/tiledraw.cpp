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
// - Repaired crashes when zooming out
// - Added IsCapitol Bool to DrawCityIcons 1-5-2006 EMOD
// - Implemented HasAirport; for some reason it was forgotten 1-5-2006 EMOD
// - Moved citypop box to the left name in the center and turns until next pop
//   to the right for cleaner interface 1-13-2007 EMOD
// - Allowed to select between smooth and square borders. (Feb 4th 2007 Martin Gühmann)
// - Fixed Religion Icon displays - E 6.25.2007
// - Roads now use the TileSetIndex from the TerrainImprovement database. (28-Feb-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "tiledmap.h"               // tiledraw.h does not exist

#include <inttypes.h>

#include "aui.h"
#include "aui_sdlsurface.h"
#include "aui_bitmapfont.h"
#include "ConstRecord.h"			// g_theConstDB
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
#include "BuildingRecord.h"    //emod
#include "WonderRecord.h"    //emod
#include "buildingutil.h"
#include "wonderutil.h"

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
    int const       THRESHOLD_SLOW_GROWTH       = 100;  // limit to 2 digits
    // Do not display digits when the number of turns for construction is too large
    int const       THRESHOLD_PROD_TIME		    = 1000;  // limit to 3 digits
    /// Text to display when no valid number can be calculated
    char const      TEXT_NONE []                = "---";

    Pixel16 const   DEFAULT_PIXEL[k_NUM_TRANSITIONS]    =
    {
        0xf800, 0x07e0, 0x001f, 0xf81f
    };

    // Simple shorthand functions
    Pixel16 GetColor(COLOR const & a_ColorName, bool a_IsFogged = false)
    {
        return (a_IsFogged) ? g_colorSet->GetDarkColor(a_ColorName)
                            : g_colorSet->GetColor(a_ColorName);
    }

    COLORREF GetColorRef(COLOR const & a_ColorName, bool a_IsFogged = false)
    {
        return (a_IsFogged) ? g_colorSet->GetDarkColorRef(a_ColorName)
                            : g_colorSet->GetColorRef(a_ColorName);
    }

    Pixel16 GetPlayerColor(sint32 a_Player, bool a_IsFogged = false)
    {
        return (a_IsFogged) ? g_colorSet->GetDarkPlayerColor(a_Player)
                            : g_colorSet->GetPlayerColor(a_Player);
    }

    int     StartPixel(int a_Position)
    {
        return (a_Position < k_TILE_PIXEL_HEADROOM)
               ? 2 * ((k_TILE_PIXEL_HEADROOM - 1) - a_Position)
               : 2 * (a_Position - k_TILE_PIXEL_HEADROOM);
    }

    /// Manager class to acquire and release a surface lock.
    /// Using a RAII pattern to release the lock when leaving scope.
    class SurfaceLock
    {
    public:
        /// \param  a_Surface The surface to acquire a lock for.
        /// \remarks When \a a_Surface is NULL, no lock will be acquired.
        SurfaceLock(aui_Surface * a_Surface)
        :
            m_Base      (NULL),
            m_Surface   (a_Surface),
            m_HasLocked (false)
        {
	        if (a_Surface)
            {
	        	m_Base = a_Surface->Buffer();
	        	if (!m_Base) {
			        AUI_ERRCODE result = a_Surface->Lock(NULL, (LPVOID *)&m_Base, 0);
			        Assert(AUI_ERRCODE_OK == result);
			        if (AUI_ERRCODE_OK != result) {
				        m_Base = NULL;
			        }
			        m_HasLocked = true;
		        }
            }
        };

        ~SurfaceLock()
        {
            if (m_Surface && m_Base && m_HasLocked)
            {
#if defined(_DEBUG)
		            AUI_ERRCODE result = m_Surface->Unlock(m_Base);
		            Assert(AUI_ERRCODE_OK == result);
#else
		            (void) m_Surface->Unlock(m_Base);
#endif
            }
        };

        /// The acquired surface lock (NULL when invalid)
        uint8 * Base() const    { return m_Base; };
        /// A valid lock has been acquired
        bool    IsValid() const { return NULL != m_Base; };

    private:
        /// The acquired surface lock (NULL when invalid)
        uint8 *         m_Base;
        /// The surface to lock
        aui_Surface *   m_Surface;
        bool            m_HasLocked;
    };
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
			sint32 width = m_font->GetStringWidth(text);
			sint32 height = m_font->GetMaxHeight();

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
	bool        drewSomething           = false;

	if (!m_localVision || !m_localVision->IsExplored(pos))
		return drewSomething;

	if (!ReadyToDraw())
		return drewSomething;

	bool	    fog                     = !m_localVision->IsVisible(pos);
	bool        visiblePlayerOwnsThis   =
	    (g_selected_item->GetVisiblePlayer() == g_theWorld->GetOwner(pos));
	uint32		env                     = 0x00000000;
	Cell *      cell                    = NULL;
	bool		isAirfield              = false;
    bool		isListeningPost         = false;
    bool        isRadar                 = false;
    bool        isHealUnits             = false;
    bool        isFort                  = false;
	Pixel16	 *  data                    = NULL;
	bool		hasHut                  = false;

	std::vector<Pixel16 *>	drawOnTop;	// things above road level

	TerrainImprovementRecord const * roadRec = NULL;
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

		PointerList<UnseenImprovementInfo> *improvements = ucell.m_unseenCell->GetImprovements();

		for
		(
		    PointerList<UnseenImprovementInfo>::Walker walker =
		        PointerList<UnseenImprovementInfo>::Walker(improvements);
		    walker.IsValid();
		    walker.Next()
		)
		{
			sint32 type		= walker.GetObj()->m_type;
			const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(type);

			if(rec && (rec->GetClassRoad() || rec->GetClassOceanRoad()))
			{
				roadRec = rec;
			}
		}
	}
	else
	{
		hasHut		= (g_theWorld->GetGoodyHut(pos) != NULL);
		cell = g_theWorld->GetCell(pos);

		if (cell)
		{
			env = cell->GetEnv();

			sint32 numImprovements = cell->GetNumDBImprovements();

			for (sint32 i = 0; i < numImprovements; i++)
			{
				sint32  impType = cell->GetDBImprovement(i);
				TerrainImprovementRecord const *
				        rec     = g_theTerrainImprovementDB->Get(impType);

				if (rec==NULL)
					continue;

				if (rec->GetClassRoad() || rec->GetClassOceanRoad())
				{
					roadRec = rec;
					continue;
				}

				TerrainImprovementRecord::Effect const *
				        effect  = terrainutil_GetTerrainEffect(rec, pos);

				if (effect==NULL)
					continue;

				sint32  index   = effect->GetTilesetIndex();

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

	if (cell)
	{
		for (sint32 i=0; i<cell->GetNumImprovements(); i++)
		{
			if (!cell->AccessImprovement(i).IsValid())
				continue;

			sint32 percent = cell->AccessImprovement(i).GetData()->PercentComplete();
			uint16 index;

			if (percent < 5)
				index = 0;
			else if (percent < 100)
					index = 1;
			else
				break;
			sint32 type = cell->AccessImprovement(i).GetData()->GetType();
			DrawPartiallyConstructedImprovement(surface, env, type, x, y, index, fog, percent);
			drewSomething = true;
		}
	}
	else
	{
		PointerList<UnseenImprovementInfo> *improvements = ucell.m_unseenCell->GetImprovements();

		for
		(
		    PointerList<UnseenImprovementInfo>::Walker walker =
		        PointerList<UnseenImprovementInfo>::Walker(improvements);
		    walker.IsValid();
		    walker.Next()
		)
		{
			sint32 type		= walker.GetObj()->m_type;
			sint32 percent	= walker.GetObj()->m_percentComplete;
			uint16 index;

			if (percent < 50)
			{
				index = 0;
				DrawPartiallyConstructedImprovement(surface, env, type, x, y, index, fog, percent);
			}
			else if (percent < 100)
			{
				index = 1;
				DrawPartiallyConstructedImprovement(surface, env, type, x, y, index, fog, percent);
			}
			else
			{

				const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(type);
				const TerrainImprovementRecord::Effect *
				    effect = (rec) ? terrainutil_GetTerrainEffect(rec, pos) : NULL;

				if (rec!=NULL && effect!=NULL)
				{
					if(!rec->GetClassRoad() && !rec->GetClassOceanRoad())
					{
						data = m_tileSet->GetImprovementData(static_cast<uint16>(effect->GetTilesetIndex()));
						DrawAnImprovement(surface,data,x,y,fog);
					}
				}
				else
				{
					index = 2;
					DrawPartiallyConstructedImprovement(surface, env, type, x, y, index, fog, percent);
				}
			}
			drewSomething = true;
		}
	}

	if(roadRec)
	{
		const TerrainImprovementRecord::Effect *
		    effect = terrainutil_GetTerrainEffect(roadRec, pos);

		if(effect)
		{
			DrawRoads(surface, pos, x, y, effect->GetTilesetIndex(), fog, k_OVERLAY_FLAG_SHADOWSONLY);
			DrawRoads(surface, pos, x, y, effect->GetTilesetIndex(), fog, k_OVERLAY_FLAG_NOSHADOWS  );
			drewSomething = true;
		}
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
												   uint16 index, bool fog, sint32 percentComplete)
												   //Added sint32 percentComplete by Martin Gühmann
{
	Pixel16		*data = NULL;

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

void TiledMap::DrawHitMask(aui_Surface * surf, const MapPoint & pos)
{
	extern BOOL g_killMode;
	Pixel16 color = GetColor(g_killMode ? COLOR_RED : g_curSelectColor);

	sint32 x, y;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);

	y += m_zoomTileHeadroom[m_zoomLevel];
	sint32 offsetX = m_zoomTilePixelWidth[m_zoomLevel] / 2;
	sint32 offsetY = m_zoomTilePixelHeight[m_zoomLevel] / 2;
	primitives_ClippedLine16(*surf, x, y + offsetY, x + offsetX, y, color);  // north-west
	primitives_ClippedLine16(*surf, x + offsetX, y, x + offsetX * 2, y + offsetY, color); // north-east
	primitives_ClippedLine16(*surf, x, y + offsetY, x + offsetX, y + offsetY * 2, color); // south-west
	primitives_ClippedLine16(*surf, x + offsetX, y + offsetY * 2, x + offsetX * 2, y + offsetY, color); // south-east

	AddDirtyToMix(x, y, offsetX * 2 + 1, offsetY * 2 + 1);
}

void TiledMap::DrawColoredHitMask(aui_Surface *surf, const MapPoint &pos, COLOR color)
{
	sint32		x, y;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);

	if (x < 0) return;
	y += (sint32) ((double)k_TILE_PIXEL_HEADROOM * m_scale);
	if (y < 0) return;

	sint32 width = GetZoomTilePixelWidth();
	sint32 height = GetZoomTilePixelHeight();

	if (x >= surf->Width()-width) return;
	if (y >= surf->Height() - height) return;

	AddDirtyToMix(x, y, width, height);

	uint8 * surfBase    = g_screenManager->GetSurfBase();
	sint32	surfPitch   = g_screenManager->GetSurfPitch();

	sint32 num = k_TILE_GRID_HEIGHT - k_TILE_PIXEL_HEADROOM;
	sint32 den = height;
	sint32 tot = num;

	sint32 row = 0;
	Pixel16		selectColorPixel = GetColor(color);

	for (sint32 i = k_TILE_PIXEL_HEADROOM; i < k_TILE_GRID_HEIGHT;)
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

		Pixel16 * pDestPixel = (Pixel16 *)(surfBase + ((y+row) * surfPitch) + ((x+start) << 1));
		*pDestPixel = selectColorPixel;
		pDestPixel += (end-start);
		*pDestPixel = selectColorPixel;

		row++;
	}

}

void TiledMap::DrawHitMask(aui_Surface *surf, const MapPoint &pos, RECT *mapViewRect, RECT *destRect)
{
	Pixel16 color = GetColor(g_curSelectColor);

	sint32 x, y;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y, mapViewRect);

	x += destRect->left;
	y += destRect->top + m_zoomTileHeadroom[m_zoomLevel];
	sint32 offsetX = m_zoomTilePixelWidth[m_zoomLevel] / 2;
	sint32 offsetY = m_zoomTilePixelHeight[m_zoomLevel] / 2;
	primitives_ClippedLine16(*surf, x, y + offsetY, x + offsetX, y, color);  // north-west
	primitives_ClippedLine16(*surf, x + offsetX, y, x + offsetX * 2, y + offsetY, color); // north-east
	primitives_ClippedLine16(*surf, x, y + offsetY, x + offsetX, y + offsetY * 2, color); // south-west
	primitives_ClippedLine16(*surf, x + offsetX, y + offsetY * 2, x + offsetX * 2, y + offsetY, color); // south-east

	AddDirtyToMix(x, y, offsetX * 2 + 1, offsetY * 2 + 1);

	g_curSelectColor = (COLOR)(g_curSelectColor + 1);
	if (g_curSelectColor > COLOR_SELECT_3) {
		g_curSelectColor = COLOR_SELECT_0;
	}
}

void TiledMap::DrawColoredHitMaskEdge(aui_Surface *surf, const MapPoint &pos, Pixel16 selectColorPixel, WORLD_DIRECTION side)
{
	sint32		x, y;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);

	if (x < 0) return;
	y += (sint32) ((double)k_TILE_PIXEL_HEADROOM * m_scale);
	if (y < 0) return;

	sint32 width = GetZoomTilePixelWidth();
	sint32 height = GetZoomTilePixelHeight();

	if (x >= surf->Width()-width) return;
	if (y >= surf->Height() - height) return;

	AddDirtyToMix(x, y, width, height);

	uint8	* surfBase = g_screenManager->GetSurfBase();
	sint32  surfPitch = g_screenManager->GetSurfPitch();

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

		Pixel16 * pDestPixel = (Pixel16 *)(surfBase + ((y+row) * surfPitch) + ((x+start) << 1));
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

inline void DrawNorthEastBorder(aui_Surface & surf, sint32 centerX, sint32 centerY, sint32 offsetX, sint32 offsetY,
		sint32 indent, Pixel16 color, uint32 borders, bool usePattern)
{
	if (!(borders & (1 << NORTHEAST))) {
		return;
	}

	bool southEast = borders & (1 << SOUTHEAST);
	bool east      = borders & (1 << EAST);
	bool northWest = borders & (1 << NORTHWEST);

	sint32 x1 = centerX + offsetX - (southEast || east ? indent * 2 : 0);
	sint32 x2 = centerX - (northWest ? 0 : indent * 2);
	sint32 y1 = centerY + (southEast || east ? 0 : indent);
	sint32 y2 = centerY - offsetY + (northWest ? indent : 0);

	if (usePattern) {
		primitives_ClippedPatternLine16(surf, x1, y1, x2, y2, color, LINE_PATTERN_DASH, LINE_PATTERN_DASH_LENGTH);
	} else {
		primitives_ClippedLine16(surf, x1, y1, x2, y2, color);
	}
}

inline void DrawNorthWestBorder(aui_Surface & surf, sint32 centerX, sint32 centerY, sint32 offsetX, sint32 offsetY,
		sint32 indent, Pixel16 color, uint32 borders, bool usePattern)
{
	if (!(borders & (1 << NORTHWEST))) {
		return;
	}

	bool northEast = borders & (1 << NORTHEAST);
	bool north     = borders & (1 << NORTH);
	bool southWest = borders & (1 << SOUTHWEST);

	sint32 x1 = centerX + (northEast || north ? 0 : indent * 2);
	sint32 x2 = centerX - offsetX + (southWest ? indent * 2 : 0);
	sint32 y1 = centerY - offsetY + (northEast || north ? indent : 0);
	sint32 y2 = centerY + (southWest ? 0 : indent);

	if (usePattern) {
		primitives_ClippedPatternLine16(surf, x1, y1, x2, y2, color, LINE_PATTERN_DASH, LINE_PATTERN_DASH_LENGTH);
	} else {
		primitives_ClippedLine16(surf, x1, y1, x2, y2, color);
	}
}

inline void DrawSouthEastBorder(aui_Surface & surf, sint32 centerX, sint32 centerY, sint32 offsetX, sint32 offsetY,
		sint32 indent, Pixel16 color, uint32 borders, bool usePattern)
{
	if (!(borders & (1 << SOUTHEAST))) {
		return;
	}

	bool southWest = borders & (1 << SOUTHWEST);
	bool south     = borders & (1 << SOUTH);
	bool northEast = borders & (1 << NORTHEAST);

	sint32 x1 = centerX - (southWest || south ? 0 : indent * 2);
	sint32 x2 = centerX + offsetX - (northEast ? indent * 2 : 0);
	sint32 y1 = centerY + offsetY - (southWest || south ? indent : 0);
	sint32 y2 = centerY - (northEast ? 0 : indent);

	if (usePattern) {
		primitives_ClippedPatternLine16(surf, x1, y1, x2, y2, color, LINE_PATTERN_DASH, LINE_PATTERN_DASH_LENGTH);
	} else {
		primitives_ClippedLine16(surf, x1, y1, x2, y2, color);
	}
}

inline void DrawSouthWestBorder(aui_Surface & surf, sint32 centerX, sint32 centerY, sint32 offsetX, sint32 offsetY,
		sint32 indent, Pixel16 color, uint32 borders, bool usePattern)
{
	if (!(borders & (1 << SOUTHWEST))) {
		return;
	}

	bool northWest = borders & (1 << NORTHWEST);
	bool west      = borders & (1 << WEST);
	bool southEast = borders & (1 << SOUTHEAST);

	sint32 x1 = centerX - offsetX + (northWest || west ? indent * 2 : 0);
	sint32 x2 = centerX + (southEast ? 0 : indent * 2);
	sint32 y1 = centerY - (northWest || west ? 0 : indent);
	sint32 y2 = centerY + offsetY - (southEast ? indent : 0);

	if (usePattern) {
		primitives_ClippedPatternLine16(surf, x1, y1, x2, y2, color, LINE_PATTERN_DASH, LINE_PATTERN_DASH_LENGTH);
	} else {
		primitives_ClippedLine16(surf, x1, y1, x2, y2, color);
	}
}

inline void DrawBorders(aui_Surface & surf, sint32 centerX, sint32 centerY, sint32 offsetX, sint32 offsetY,
		sint32 indent, Pixel16 color, uint32 borders, bool usePattern)
{
	DrawNorthEastBorder(surf, centerX, centerY, offsetX, offsetY, indent, color, borders, usePattern);
	DrawNorthWestBorder(surf, centerX, centerY, offsetX, offsetY, indent, color, borders, usePattern);
	DrawSouthEastBorder(surf, centerX, centerY, offsetX, offsetY, indent, color, borders, usePattern);
	DrawSouthWestBorder(surf, centerX, centerY, offsetX, offsetY, indent, color, borders, usePattern);
}

/// Draw a colored border edge at a tile
/// \param      surf                Surface to draw on
/// \param      pos                 Tile to draw
/// \param      selectColorPixel    Color to use
/// \param      borders             Borders of the tile to draw
/// \param      dashMode            Draw dash, solid or smooth lines
/// \remarks    This function expects that \a surf has been locked
void TiledMap::DrawColoredBorderEdge(aui_Surface & surf, const MapPoint & pos, Pixel16 selectColorPixel, uint32 borders,
		sint32 dashMode, sint32 indent)
{
	if (!borders) {
		return;
	}

	sint32 x1, y1;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x1, &y1);
	y1 += m_zoomTileHeadroom[m_zoomLevel];

	sint32 offsetX = m_zoomTilePixelWidth[m_zoomLevel] / 2;
	sint32 offsetY = m_zoomTilePixelHeight[m_zoomLevel] / 2;
	sint32 centerX = x1 + offsetX;
	sint32 centerY = y1 + offsetY;
	AddDirtyToMix(x1, y1, offsetX * 2 + 1, offsetY * 2 + 1);

	Pixel16 darkColor = pixelutils_Shadow16(selectColorPixel, pixelutils_GetShadow16RGBMask());
	if (dashMode == k_BORDER_SOLID) {
		DrawBorders(surf, centerX, centerY, offsetX, offsetY, indent, selectColorPixel, borders, false);
	}
	else if (dashMode == k_BORDER_SMOOTH)
	{
		DrawBorders(surf, centerX, centerY, offsetX, offsetY, indent, selectColorPixel, borders, false);
		DrawBorders(surf, centerX, centerY, offsetX, offsetY, indent + 1, darkColor, borders, false);
	}
	else if (dashMode == k_BORDER_DASHED)
	{
		DrawBorders(surf, centerX, centerY, offsetX, offsetY, indent, selectColorPixel, borders, true);
	}
}

// Unused
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
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	if(surface == NULL) surface = m_surface;

	if(surface == NULL)
	{
		errcode = AUI_ERRCODE_BLTFAILED;
	}
	else
	{
		errcode = surface->Blank(0);
	}

	Assert(errcode == AUI_ERRCODE_OK);
	return errcode;
}

sint32 TiledMap::DrawBlackTile(aui_Surface *surface, sint32 x, sint32 y) //EMOD this is for unexplored? could add bracket like icons that make it more like civ3
{
	if (x < 0) return 0;
	y+=k_TILE_PIXEL_HEADROOM;
	if (y < 0) return 0;

	if (!surface)
    {
        surface = m_surface;
        if (!surface) return 0;
    }
	if (x >= surface->Width() - k_TILE_PIXEL_WIDTH) return 0;
	if (y >= surface->Height() - k_TILE_PIXEL_HEIGHT) return 0;

	uint8	*surfBase = m_surfBase;
	sint32 surfPitch = m_surfPitch;

	for (sint32 j=0; j<k_TILE_PIXEL_HEIGHT; j++)
	{
		sint32  startX      = StartPixel(j);
		sint32  endX        = k_TILE_PIXEL_WIDTH - startX;
		Pixel16 *
		        destPixel   = (Pixel16 *)(surfBase + ((y + j) * surfPitch) + ((x+startX) * 2));

		for (sint32 i = startX; i < endX; i++) {
			*destPixel++ = 0x0000;
		}
	}

	return 0;
}







sint32 TiledMap::DrawDitheredTile(aui_Surface *surface, sint32 x, sint32 y, Pixel16 color)
{
    if (x < 0) return 0;
	y+=k_TILE_PIXEL_HEADROOM;
    if (y < 0) return 0;

	if (!surface)
    {
        surface = m_surface;
        if (!surface) return 0;
    }
    if (x >= surface->Width() - k_TILE_PIXEL_WIDTH) return 0;
    if (y >= surface->Height() - k_TILE_PIXEL_HEIGHT) return 0;

	uint8	* surfBase = m_surfBase;
	sint32 surfPitch = m_surfPitch;

	for(sint32 j=0; j<k_TILE_PIXEL_HEIGHT; j++)
    {
		sint32  startX      = StartPixel(j);
		sint32  endX        = k_TILE_PIXEL_WIDTH - startX;
		sint32  offset      = j & 0x01;
		unsigned short *
    		destPixel       = (unsigned short *)(surfBase + ((y + j) * surfPitch) + ((x+startX) * 2));
		destPixel += offset;
		for (sint32 i=startX; i<endX; i+=2) {
				*destPixel = color;
				destPixel+=2;
		}
	}

	return 0;
}







void TiledMap::DrawDitheredTileScaled(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,Pixel16 color)
{
    if (x < 0) return;
	y+=(sint32)((double)k_TILE_PIXEL_HEADROOM * m_scale);
    if (y < 0) return;

	uint8 * pSurfBase;
	sint32  surfWidth;
	sint32  surfHeight;
	sint32  surfPitch;

    SurfaceLock lock    = SurfaceLock(surface);

	if (!surface)
    {
		surface     = m_surface;
		pSurfBase   = m_surfBase;
		surfWidth   = m_surfWidth;
		surfHeight  = m_surfHeight;
		surfPitch   = m_surfPitch;
	}
    else if (lock.IsValid())
    {
        pSurfBase   = lock.Base();
		surfWidth   = surface->Width();
		surfHeight  = surface->Height();
		surfPitch   = surface->Pitch();
	}
    else
    {
        return;
    }

    if (!surface ||
        (x >= surface->Width() - destWidth) ||
        (y >= surface->Height() - destHeight)
       )
    {
        return;
    }

	sint32  vaccum      = destHeight*2 - k_TILE_PIXEL_HEIGHT;
	sint32  vincx       = destHeight*2;
	sint32  vincxy      = (destHeight - k_TILE_PIXEL_HEIGHT) * 2 ;
	sint32  vpos2       = (sint32)((double)(k_TILE_PIXEL_HEIGHT - destHeight) / (double)destHeight);
	sint32  vdestpos    = y;
	sint32  vend        = k_TILE_PIXEL_HEIGHT - 1;

	for (sint32 vpos1 = 0; vpos1 < vend; ++vpos1)
    {
		if (vaccum < 0)
        {
			vaccum += vincx;
		}
        else
        {
		    sint32  offset      = vpos1 & 0x01;
	        sint32  startX      = StartPixel(vpos1);
            sint32  endX        = k_TILE_PIXEL_WIDTH - startX;
			sint32  haccum      = destWidth * 2 - k_TILE_PIXEL_WIDTH;
			sint32  hincx       = destWidth * 2;
			sint32  hincxy      = (destWidth - k_TILE_PIXEL_WIDTH) * 2;
			sint32  hdestpos    = x + (sint32)((double)startX * m_scale);

			for (sint32 hpos = startX; hpos < endX; hpos +=2)
            {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					Pixel16 * pDestPixel = (Pixel16 *)
                        (pSurfBase + (vdestpos * surfPitch) + (hdestpos << 1));
					pDestPixel += offset;
					*pDestPixel = color;

					hdestpos+=2;
				}
			}
			vaccum += vincxy;
			vdestpos++;
		}

        vpos2++;
	}
}







sint32 TiledMap::DrawBlendedTile(aui_Surface *surface, const MapPoint &pos,sint32 xpos, sint32 ypos, Pixel16 color, sint32 blend)
{
    if (xpos < 0) return 0;
    ypos+=k_TILE_PIXEL_HEADROOM;
    if (ypos < 0) return 0;

	if (!surface)
    {
        surface = m_surface;
        if (!surface) return 0;
    }

    if (xpos > surface->Width() - k_TILE_PIXEL_WIDTH) return 0;
    if (ypos > surface->Height() - k_TILE_PIXEL_HEIGHT) return 0;

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

    Pixel16 *   t[k_NUM_TRANSITIONS];
    for (uint16 i = 0; i < k_NUM_TRANSITIONS; ++i)
    {
        t[i]    = m_tileSet->GetTransitionData
                    (tilesetIndex_short, tileInfo->GetTransition(i), i);
    }

	BaseTile *  transitionBuffer    =
        m_tileSet->GetBaseTile(static_cast<uint16>((tilesetIndex * 100) + 99));
	Pixel16 *   transData           =
        (transitionBuffer) ? transitionBuffer->GetTileData() : NULL;
    Pixel16 *   transDataPtr        = transData;
	Pixel16 *   dataPtr             = data;
	uint8 *     pSurfBase           = m_surfBase;
	sint32      surfPitch           = m_surfPitch;

	uint32  blendRgbMask = pixelutils_GetBlend16RGBMask();
	Pixel16 transPixel = 0;

	for (sint32 y = 0; y < k_TILE_PIXEL_HEIGHT; y++)
    {
		sint32  startX  = StartPixel(y);
		sint32  endX    = k_TILE_PIXEL_WIDTH - startX;

		for (sint32 x = startX; x < endX; x++)
        {
			Pixel16 srcPixel = *dataPtr++;
			if(transDataPtr)
				transPixel = *transDataPtr++;

			if (srcPixel < k_NUM_TRANSITIONS)
            {
                Pixel16 * &  tI = t[srcPixel];

			    if (tI)
				{
					srcPixel = *tI++;
				}
                else if (transDataPtr)
                {
					srcPixel = transPixel;
				}
                else
                {
					srcPixel = DEFAULT_PIXEL[srcPixel];
				}
			}
			Pixel16 * pDestPixel = (Pixel16 *)
                (pSurfBase + ((y+ypos) * surfPitch) + ((x+xpos) << 1));

			*pDestPixel = pixelutils_Blend16(color, srcPixel, blend << 3, blendRgbMask);
		}
	}

	return 0;
}







void TiledMap::DrawBlendedTileScaled(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,Pixel16 color,sint32 blend)
{
	if (x < 0) return;
	y+=(sint32)((double)k_TILE_PIXEL_HEADROOM * m_scale);
    if (y < 0) return;

	uint8 * pSurfBase;
	sint32  surfWidth;
	sint32  surfHeight;
	sint32  surfPitch;

    SurfaceLock lock    = SurfaceLock(surface);

	if (!surface)
    {
		surface     = m_surface;
		pSurfBase   = m_surfBase;
		surfWidth   = m_surfWidth;
		surfHeight  = m_surfHeight;
		surfPitch   = m_surfPitch;
	}
    else if (lock.IsValid())
    {
        pSurfBase   = lock.Base();
		surfWidth   = surface->Width();
		surfHeight  = surface->Height();
		surfPitch   = surface->Pitch();
	}
    else
    {
        return;
    }

	if (!surface ||
        (x >= surface->Width() - destWidth) ||
		(y >= surface->Height() - destHeight)
       )
    {
		return;
	}

	TileInfo *  tileInfo    = GetTileInfo(pos);
	Assert(tileInfo != NULL);
	if (tileInfo == NULL)
    {
		return;
	}

	uint16      index       = tileInfo->GetTileNum();
    BaseTile *	baseTile    = m_tileSet->GetBaseTile(index);
	if (baseTile == NULL)
    {
		return;
	}

	Pixel16 *   data = baseTile->GetTileData();

	sint32      tilesetIndex =
        g_theTerrainDB->Get(tileInfo->GetTerrainType())->GetTilesetIndex();

	uint16 tilesetIndex_short = (uint16) tilesetIndex;

    Pixel16 *   t[k_NUM_TRANSITIONS];
    for (uint16 i = 0; i < k_NUM_TRANSITIONS; ++i)
    {
        t[i]    = m_tileSet->GetTransitionData
                    (tilesetIndex_short, tileInfo->GetTransition(i), i);
    }

	BaseTile *  transitionBuffer    =
        m_tileSet->GetBaseTile(static_cast<uint16>((tilesetIndex * 100) + 99));
	Pixel16 *   transData           =
        (transitionBuffer) ? transitionBuffer->GetTileData() : NULL;
    Pixel16 *   transDataPtr        = transData;
	Pixel16	*   dataPtr             = data;

	Pixel16 srcPixel, transPixel = 0;

	sint32  vaccum      = destHeight*2 - k_TILE_PIXEL_HEIGHT;
	sint32  vincx       = destHeight*2;
	sint32  vincxy      = (destHeight - k_TILE_PIXEL_HEIGHT) * 2 ;
	sint32  vpos2       = (sint32)((double)(k_TILE_PIXEL_HEIGHT - destHeight) / (double)destHeight);
	sint32  vdestpos    = y;
	sint32  vend        = k_TILE_PIXEL_HEIGHT - 1;

	for (sint32 vpos1 = 0; vpos1 < vend; ++vpos1)
    {
		sint32  startX  = StartPixel(vpos1);
        sint32  endX    = k_TILE_PIXEL_WIDTH - startX;

		if (vaccum < 0)
        {
			vaccum += vincx;

            for (sint32 i = startX; i < endX; i++)
            {
				if(transDataPtr)
					transPixel = *transDataPtr++;

				srcPixel = *dataPtr++;
				if (srcPixel < k_NUM_TRANSITIONS)
                {
                    Pixel16 * &  tI = t[srcPixel];

			        if (tI)
				    {
					    srcPixel = *tI++;
				    }
					else if (transDataPtr)
                    {
                        srcPixel = transPixel;
                    }
				}
			}
		}
        else
        {
        	uint32  blendRgbMask = pixelutils_GetBlend16RGBMask();
			sint32  haccum      = destWidth * 2 - k_TILE_PIXEL_WIDTH;
			sint32  hincx       = destWidth * 2;
			sint32  hincxy      = (destWidth - k_TILE_PIXEL_WIDTH) * 2;
			sint32  hdestpos    = x + (sint32)((double)startX * m_scale);

			for (sint32 hpos = startX; hpos < endX; ++hpos)
            {
				if(transDataPtr)
					transPixel = *transDataPtr++;

				srcPixel = *dataPtr++;
				if (srcPixel < k_NUM_TRANSITIONS)
                {
                    Pixel16 * &  tI = t[srcPixel];

			        if (tI)
				    {
					    srcPixel = *tI++;
				    }
					else if (transDataPtr)
                    {
                        srcPixel = transPixel;
                    }
                    else
                    {
						srcPixel = DEFAULT_PIXEL[srcPixel];
                    }
				}

				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					Pixel16 * pDestPixel = (Pixel16 *)
                        (pSurfBase + (vdestpos * surfPitch) + (hdestpos << 1));

					*pDestPixel = pixelutils_Blend16(color, srcPixel, blend << 3, blendRgbMask);

					hdestpos++;
				}
			}

			vaccum += vincxy;
			vdestpos++;
		}
		vpos2++;
	}
}

sint32 TiledMap::DrawBlendedOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y,
										Pixel16 color, sint32 blend, sint32 flags)
{
	if (!data || (x < 0) || (y < 0)) return 0;

	uint8 * surfBase;
	sint32  surfWidth;
	sint32  surfHeight;
	sint32  surfPitch;

    SurfaceLock lock    = SurfaceLock(surface);

	if (!surface)
    {
		surface     = m_surface;
		surfBase    = m_surfBase;
		surfWidth   = m_surfWidth;
		surfHeight  = m_surfHeight;
		surfPitch   = m_surfPitch;
	}
    else if (lock.IsValid())
    {
        surfBase    = lock.Base();
		surfWidth   = surface->Width();
		surfHeight  = surface->Height();
		surfPitch   = surface->Pitch();
	}
    else
    {
        return AUI_ERRCODE_SURFACELOCKFAILED;
    }

    if (x >= surfWidth - k_TILE_GRID_WIDTH) return 0;
    if (y >= surfHeight - k_TILE_GRID_HEIGHT) return 0;

	uint32    blendRgbMask  = pixelutils_GetBlend16RGBMask();
	uint32    shadowRgbMask = pixelutils_GetShadow16RGBMask();
	uint16    start         = (uint16)*data++;
	uint16    end           = (uint16)*data++;
	Pixel16 * table         = data;
	Pixel16 * dataStart     = table + (end - start + 1);

	for (sint32 j=start; j<end; j++)
    {
		if ((sint16)table[j-start] == -1) continue;

	    unsigned short *    destPixel = (unsigned short *)
            (surfBase + ((y + j) * surfPitch) + (x * 2));

		Pixel16 *   rowData = dataStart + table[j-start];
		Pixel16		tag;

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
								*destPixel++ = pixelutils_Blend16(color, *rowData, blend << 3, blendRgbMask);
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
								*destPixel = pixelutils_Shadow16(*destPixel, shadowRgbMask);
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

	return 0;
}

void TiledMap::DrawBlendedOverlayScaled(aui_Surface *surface,Pixel16 *data, sint32 x, sint32 y,
										sint32 destWidth, sint32 destHeight, Pixel16 color, sint32 blend,
										sint32 flags)
{
	if (!data || (x < 0) || (y < 0)) return;

	uint8 *     surfBase;
	sint32      surfWidth;
	sint32      surfHeight;
	sint32      surfPitch;

    SurfaceLock lock    = SurfaceLock(surface);

	if (!surface)
    {
		surface     = m_surface;
		surfBase    = m_surfBase;
		surfWidth   = m_surfWidth;
		surfHeight  = m_surfHeight;
		surfPitch   = m_surfPitch;
	}
    else if (lock.IsValid())
    {
        surfBase    = lock.Base();
		surfWidth   = surface->Width();
		surfHeight  = surface->Height();
		surfPitch   = surface->Pitch();
	}
    else
    {
        return;
    }

	if (!surface ||
        (x >= surface->Width() - destWidth) ||
		(y >= surface->Height() - destHeight)
       )
    {
		return;
	}

	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	surfBase += (surfPitch * y + x * 2);

	uint32    blendRgbMask = pixelutils_GetBlend16RGBMask();
	uint16    vstart       = (uint16)*data++;
	uint16    end          = (uint16)*data++;
	Pixel16 * table        = data;
	Pixel16 * dataStart    = table + (end - vstart + 1);
	sint32    vaccum       = destHeight*2 - k_TILE_GRID_HEIGHT;
	sint32    vincx        = destHeight*2;
	sint32    vincxy       = (destHeight-k_TILE_GRID_HEIGHT) * 2;
	sint32    vpos2        = (sint32)((double)((end+1) - destHeight) / (double)destHeight);
	sint32    vdestpos     = y;
	sint32    vend         = (end+1) - 1;

	for (sint32 vpos1 = 0; vpos1 < vend; ++vpos1)
    {
		if (vaccum < 0) {
			vaccum += vincx;
		}
        else
        {
			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos2++;
				continue;
			}

            Pixel16 *   rowData1    =
                (vpos1 < vstart) ? emptyRow : dataStart + table[vpos1-vstart];
			Pixel16 *   rowData2    =
                (vpos2 < vstart) ? emptyRow : dataStart + table[vpos2-vstart];

			sint32      haccum      = destWidth*2 - k_TILE_GRID_WIDTH;
			sint32      hincx       = destWidth*2;
			sint32      hincxy      = (destWidth-k_TILE_GRID_WIDTH) * 2;
			sint32      hdestpos    = x;
			sint32      hend        = k_TILE_GRID_WIDTH-1;

			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;
			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, flags);

			for (sint32 hpos = 0; hpos < hend; ++hpos)
            {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					Pixel16	*   destPixel = (Pixel16 *)
                        (surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));

					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, flags);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {

						pixel = pixel3;
						*destPixel = pixelutils_Blend16(color, pixel, blend << 3, blendRgbMask);
					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
			}

			vaccum += vincxy;
			vdestpos++;
		}

        vpos2++;
	}
}

sint32 TiledMap::DrawBlendedOverlayIntoMix(Pixel16 *data, sint32 x, sint32 y, Pixel16 color, sint32 blend)
{
	if (!data || (x < 0) || (y < 0)) return 0;

	sint32      surfWidth   = g_screenManager->GetSurfWidth();
	sint32      surfHeight  = g_screenManager->GetSurfHeight();

	if (x >= surfWidth - k_TILE_GRID_WIDTH) return 0;
	if (y >= surfHeight - k_TILE_GRID_HEIGHT) return 0;

	uint32    blendRgbMask  = pixelutils_GetBlend16RGBMask();
	uint32    shadowRgbMask = pixelutils_GetShadow16RGBMask();
	sint32    surfPitch     = g_screenManager->GetSurfPitch();
	uint8   * surfBase      = g_screenManager->GetSurfBase();
	uint16    start         = (uint16)*data++;
	uint16    end           = (uint16)*data++;
	Pixel16 * table         = data;
	Pixel16 * dataStart     = table + (end - start + 1);

	for (sint32 j = start; j < end; j++)
    {
		if ((sint16)table[j-start] == -1) continue;

    	unsigned short *    destPixel   = (unsigned short *)
            (surfBase + ((y + j) * surfPitch) + (x * 2));

		Pixel16 *   rowData = dataStart + table[j-start];
		Pixel16		tag;

		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
						destPixel += (tag & 0x00FF);
					break;
				case k_TILE_COPY_RUN_ID			: {
						short len = (tag & 0x00FF);

						for (short i=0; i<len; i++) {
							*destPixel++ = pixelutils_Blend16(color, *rowData, blend << 3, blendRgbMask);
							rowData++;
						}

					}
					break;
				case k_TILE_SHADOW_RUN_ID		: {
						sint32 len = (sint32)(tag & 0x00FF);
						for (sint32 i=0; i<len; i++) {
							*destPixel = pixelutils_Shadow16(*destPixel, shadowRgbMask);
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
	if (!data || (x < 0) || (y < 0)) return;

	sint32      surfWidth   = g_screenManager->GetSurfWidth();
	sint32      surfHeight  = g_screenManager->GetSurfHeight();

	if (x >= surfWidth - destWidth) return;
	if (y >= surfHeight - destHeight) return;

	uint32    blendRgbMask = pixelutils_GetBlend16RGBMask();
	sint32    surfPitch    = g_screenManager->GetSurfPitch();
	uint8   * surfBase     = g_screenManager->GetSurfBase() + (surfPitch * y + x * 2);
	uint16    vstart       = (uint16)*data++;
	uint16    end          = (uint16)*data++;
	Pixel16 * table        = data;
	Pixel16 * dataStart    = table + (end - vstart + 1);
	sint32    vaccum       = destHeight*2 - k_TILE_GRID_HEIGHT;
	sint32    vincx        = destHeight*2;
	sint32    vincxy       = (destHeight-k_TILE_GRID_HEIGHT) * 2;
	sint32    vpos2        = (sint32)((double)((end+1) - destHeight) / (double)destHeight);
	sint32    vdestpos     = y;
	sint32    vend         = (end+1) - 1;

	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	for (sint32 vpos1 = 0; vpos1 < vend; ++vpos1)
    {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos2++;
				continue;
			}

			Pixel16 *   rowData1    =
                (vpos1 < vstart) ? emptyRow : dataStart + table[vpos1-vstart];
            Pixel16 *   rowData2    =
                (vpos2 < vstart) ? emptyRow : dataStart + table[vpos2-vstart];
			sint32      haccum      = destWidth*2 - k_TILE_GRID_WIDTH;
			sint32      hincx       = destWidth*2;
			sint32      hincxy      = (destWidth-k_TILE_GRID_WIDTH) * 2;
			sint32      hdestpos    = x;
			sint32      hend        = k_TILE_GRID_WIDTH-1;

			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;
			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, 0);

			for (sint32 hpos = 0; hpos < hend; ++hpos)
            {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					Pixel16 * destPixel = (Pixel16 *)
                        (surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));

					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, 0);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {

						pixel = pixel3;
						*destPixel = pixelutils_Blend16(color, pixel, blend << 3, blendRgbMask);
					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
			}

			vaccum += vincxy;
			vdestpos++;
		}

        vpos2++;
	}
}
sint32 TiledMap::DrawDitheredOverlayIntoMix(Pixel16 *data, sint32 x, sint32 y, BOOL fogged)
{
	if (!data || (x < 0) || (y < 0)) return 0;

	sint32      surfWidth   = g_screenManager->GetSurfWidth();
	sint32      surfHeight  = g_screenManager->GetSurfHeight();

	if (x >= surfWidth - k_TILE_GRID_WIDTH) return 0;
	if (y >= surfHeight - k_TILE_GRID_HEIGHT) return 0;

	uint32    blendRgbMask  = pixelutils_GetBlend16RGBMask();
	uint32    shadowRgbMask = pixelutils_GetShadow16RGBMask();
	sint32    surfPitch     = g_screenManager->GetSurfPitch();
	uint8   * surfBase      = g_screenManager->GetSurfBase();
	uint16    start         = (uint16)*data++;
	uint16    end           = (uint16)*data++;
	Pixel16 * table         = data;
	Pixel16 * dataStart     = table + (end - start + 1);

	for (sint32 j = start; j < end; j++)
    {
		if ((sint16)table[j-start] == -1) continue;

		unsigned short *    destPixel =
            (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));

		Pixel16 *   rowData = dataStart + table[j-start];
		sint32      hpos    = 0;
		Pixel16		tag;

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
						        if (fogged)
								*destPixel++ = pixelutils_Blend16(k_FOW_COLOR, pixelutils_Blend16(*destPixel, *rowData,
										k_FOW_BLEND_VALUE << 3, blendRgbMask), k_FOW_BLEND_VALUE << 3, blendRgbMask);
							else
								*destPixel++ = pixelutils_Blend16(*destPixel, *rowData, k_FOW_BLEND_VALUE << 3,
										blendRgbMask);

							hpos++;
							rowData++;
						}

					}
					break;
				case k_TILE_SHADOW_RUN_ID		: {
						sint32 len = (sint32)(tag & 0x00FF);
						for (sint32 i=0; i<len; i++) {
							*destPixel = pixelutils_Shadow16(*destPixel, shadowRgbMask);
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
	if (!data || (x < 0) || (y < 0)) return;

    sint32      surfWidth   = g_screenManager->GetSurfWidth();
	sint32      surfHeight  = g_screenManager->GetSurfHeight();

	if (x >= surfWidth - destWidth) return;
	if (y >= surfHeight - destHeight) return;

	uint32    blendRgbMask = pixelutils_GetBlend16RGBMask();
	sint32    surfPitch    = g_screenManager->GetSurfPitch();
	uint8   * surfBase     = g_screenManager->GetSurfBase() + (surfPitch * y + x * 2);
	uint16    vstart       = (uint16)*data++;
	uint16    end          = (uint16)*data++;
	Pixel16 * table        = data;
	Pixel16 * dataStart    = table + (end - vstart + 1);
	sint32    vaccum       = destHeight*2 - k_TILE_GRID_HEIGHT;
	sint32    vincx        = destHeight*2;
	sint32    vincxy       = (destHeight-k_TILE_GRID_HEIGHT) * 2;
	sint32    vpos2        = (sint32)((double)((end+1) - destHeight) / (double)destHeight);
	sint32    vdestpos     = y;
	sint32    vend         = (end+1) - 1;

	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;
	Pixel16	*   destPixel;

	for (sint32 vpos1 = 0; vpos1 < vend; ++vpos1)
    {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {

			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos2++;
				continue;
			}

			Pixel16 *   rowData1    =
                (vpos1 < vstart) ? emptyRow : dataStart + table[vpos1-vstart];
            Pixel16 *   rowData2    =
                (vpos2 < vstart) ? emptyRow : dataStart + table[vpos2-vstart];
			sint32      haccum      = destWidth*2 - k_TILE_GRID_WIDTH;
			sint32      hincx       = destWidth*2;
			sint32      hincxy      = (destWidth-k_TILE_GRID_WIDTH) * 2;
			sint32      hdestpos    = x;
			sint32      hend        = k_TILE_GRID_WIDTH-1;

			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;

			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, 0);

			for (sint32 hpos = 0; hpos < hend; ++hpos)
            {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					destPixel = (Pixel16 *)(surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));

					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, 0);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {

						pixel = pixel3;

						if (fogged)
							*destPixel = pixelutils_Blend16(k_FOW_COLOR, pixelutils_Blend16(*destPixel, pixel,
									k_FOW_BLEND_VALUE << 3, blendRgbMask), k_FOW_BLEND_VALUE << 3, blendRgbMask);
						else
							*destPixel = pixelutils_Blend16(*destPixel, pixel, k_FOW_BLEND_VALUE << 3, blendRgbMask);
					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
			}

			vaccum += vincxy;
			vdestpos++;
		}

        vpos2++;
	}
}
sint32 TiledMap::DrawDitheredOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, Pixel16 color)
{
	if (!data || (x < 0) || (y < 0)) return 0;

	if (!surface)
    {
        surface = m_surface;
        if (!surface) return 0;
    }

    if (x >= surface->Width() - k_TILE_GRID_WIDTH) return 0;
    if (y >= surface->Height() - k_TILE_GRID_HEIGHT) return 0;

    uint32    shadowRgbMask = pixelutils_GetShadow16RGBMask();
	uint8   * surfBase      = m_surfBase;
	sint32    surfPitch     = m_surfPitch;
	uint16    start         = (uint16)*data++;
	uint16    end           = (uint16)*data++;
	Pixel16 * table         = data;
	Pixel16 * dataStart     = table + (end - start + 1);

	for (sint32 j = start; j < end; j++)
    {
		if ((sint16)table[j-start] == -1) continue;

		unsigned short * destPixel  =
            (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));
		Pixel16 *       rowData     = dataStart + table[j-start];

		Pixel16		tag;
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
							*destPixel = pixelutils_Shadow16(*destPixel, shadowRgbMask);
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
	if (!data || (x < 0) || (y < 0)) return;

	if (!surface)
    {
        surface = m_surface;
        if (!surface) return;
    }

    if (x >= surface->Width() - destWidth) return;
    if (y >= surface->Height() - destHeight) return;

	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	sint32      surfPitch   = m_surfPitch;
	uint8 *     surfBase    = m_surfBase + (y * surfPitch + x * 2);
	uint16		vstart      = (uint16)*data++;
	uint16	    end         = (uint16)*data++;
	Pixel16	*   table       = data;
	Pixel16	*   dataStart   = table + (end - vstart + 1);

	sint32  vaccum      = destHeight*2 - k_TILE_GRID_HEIGHT;
	sint32  vincx       = destHeight*2;
	sint32  vincxy      = (destHeight - k_TILE_GRID_HEIGHT) * 2 ;
	sint32  vpos2       = (sint32)((double)((end+1) - destHeight) / (double)destHeight);
    sint32  vdestpos    = y;
	sint32  vend        = (end+1) - 1;

	for (sint32 vpos1 = 0; vpos1 < vend; ++vpos1)
    {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos2++;
				continue;
			}

            Pixel16 *   rowData1    =
                (vpos1 < vstart) ? emptyRow : dataStart + table[vpos1-vstart];
            Pixel16 *   rowData2    =
                (vpos2 < vstart) ? emptyRow : dataStart + table[vpos2-vstart];

			sint32  haccum      = destWidth*2 - k_TILE_GRID_WIDTH;
			sint32  hincx       = destWidth*2;
			sint32  hincxy      = (destWidth-k_TILE_GRID_WIDTH) * 2;
			sint32  hdestpos    = x;
			sint32  hend        = k_TILE_GRID_WIDTH-1;

			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;

			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, 0);

			sint32  offset      = vpos1 & 0x01;

			for (sint32 hpos = 0; hpos < hend; ++hpos)
            {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					Pixel16 * destPixel = (Pixel16 *)
                        (surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));
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
			}

			vaccum += vincxy;
			vdestpos++;
		}
		vpos2++;
	}
}







sint32 TiledMap::DrawTileBorder(aui_Surface *surface, sint32 x, sint32 y, Pixel16 color)
{
    if (x < 0) return 0;
	y+=k_TILE_PIXEL_HEADROOM;
    if (y < 0) return 0;

	if (!surface)
    {
        surface = m_surface;
        if (!surface) return 0;
    }
    if (x > surface->Width() - k_TILE_PIXEL_WIDTH) return 0;
    if (y > surface->Height() - k_TILE_PIXEL_HEIGHT) return 0;

	for (sint32 j = 0; j < k_TILE_PIXEL_HEIGHT; j++)
    {
		sint32              startX      = StartPixel(j);
	    unsigned short *    destPixel   =
            (unsigned short *)(m_surfBase + ((y + j) * m_surfPitch) + ((x+startX) * 2));

		*destPixel++ = color;
		*destPixel = color;
	}

	return 0;
}







void TiledMap::DrawTileBorderScaled(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,Pixel16 color)
{
    if (x < 0) return;
	y+=(sint32)((double)k_TILE_PIXEL_HEADROOM * m_scale);
    if (y < 0) return;

	if (!surface)
    {
        surface = m_surface;
        if (!surface) return;
    }

    if (x >= surface->Width() - destWidth) return;
    if (y >= surface->Height() - destHeight) return;

	uint8 * pSurfBase   = m_surfBase;
	sint32  surfPitch   = m_surfPitch;
	sint32	vaccum      = destHeight*2 - k_TILE_PIXEL_HEIGHT;;
	sint32	vincx       = destHeight*2;
    sint32  vincxy      = (destHeight - k_TILE_PIXEL_HEIGHT) * 2;
    sint32  vpos2       =
        (sint32)((double)(k_TILE_PIXEL_HEIGHT - destHeight) / (double)destHeight);
	sint32	vdestpos    = y;
	sint32	vend        = k_TILE_PIXEL_HEIGHT - 1;

	uint16 *pDestPixel;

	for (sint32 vpos1 = 0; vpos1 < vend; ++vpos1)
    {
		if (vaccum < 0)
        {
			vaccum += vincx;
		}
        else
        {
            sint32  startX      = StartPixel(vpos1);
            sint32  endX        = k_TILE_PIXEL_WIDTH - startX;
			sint32  haccum      = destWidth * 2 - k_TILE_PIXEL_WIDTH;
			sint32  hincx       = destWidth * 2;
			sint32  hincxy      = (destWidth - k_TILE_PIXEL_WIDTH) * 2;
			sint32  hdestpos    = x + (sint32)((double)startX * m_scale);

			for (sint32 hpos = startX; hpos < endX; ++hpos)
            {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					pDestPixel = (Pixel16 *)(pSurfBase + (vdestpos * surfPitch) + (hdestpos << 1));
					if (hpos == startX || hpos == startX+1 ||
						hpos == endX-2 || hpos == endX-1)
						*pDestPixel = color;

					hdestpos++;
				}
			}
			vaccum += vincxy;
			vdestpos++;
		}

        vpos2++;
	}
}

void TiledMap::DrawBlackScaledLow(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight)
{
    if (x < 0) return;
	y+=GetZoomTileHeadroom();
    if (y < 0) return;

	if (!surface)
    {
        surface = m_surface;
        if (!surface) return;
    }

    if (x >= surface->Width() - destWidth) return;
    if (y >= surface->Height() - destHeight) return;

	TileInfo *  tileInfo = GetTileInfo(pos);
	Assert(tileInfo != NULL);
	if (tileInfo == NULL) return;

	uint16      index   = tileInfo->GetTileNum();
	BaseTile *  baseTile = m_tileSet->GetBaseTile(index);
	if (baseTile == NULL) return;

	Pixel16 *   data        = baseTile->GetTileData();
    Pixel16 *   t[k_NUM_TRANSITIONS];
    for (uint16 i = 0; i < k_NUM_TRANSITIONS; ++i)
    {
        t[i]    = m_tileSet->GetTransitionData
                    (tileInfo->GetTerrainType(), tileInfo->GetTransition(i), i);
    }

    Pixel16 *   dataPtr     = data;
	uint8 *     pSurfBase   = m_surfBase;
	sint32      surfPitch   = m_surfPitch;

	sint32      vaccum      = destHeight*2 - k_TILE_PIXEL_HEIGHT;
	sint32      vincx       = destHeight*2;
	sint32      vincxy      = (destHeight - k_TILE_PIXEL_HEIGHT) * 2 ;
	sint32      vpos2       =
        (sint32)((double)(k_TILE_PIXEL_HEIGHT - destHeight) / (double)destHeight);
	sint32      vdestpos    = y;
	sint32      vend        = k_TILE_PIXEL_HEIGHT - 1;

	Pixel16     srcPixel;

    for (sint32 vpos1 = 0; vpos1 < vend; ++vpos1)
    {
        sint32  startX  = StartPixel(vpos1);
        sint32  endX    = k_TILE_PIXEL_WIDTH - startX;

		if (vaccum < 0)
        {
			vaccum += vincx;

			for (sint32 i=startX; i<endX; i++) {
				srcPixel = *dataPtr++;
				if (srcPixel < k_NUM_TRANSITIONS)
                {
                    Pixel16 * &  tI = t[srcPixel];

			        if (tI)
				    {
					    srcPixel = *tI++;
				    }
				}
			}
		}
        else
        {
    		sint32  haccum      = destWidth * 2 - k_TILE_PIXEL_WIDTH;
			sint32  hincx       = destWidth * 2;
			sint32  hincxy      = (destWidth - k_TILE_PIXEL_WIDTH) * 2;
			sint32  hdestpos    = x + (sint32)((double)startX * m_scale);

			for (sint32 hpos = startX; hpos < endX; ++hpos)
            {
				srcPixel = *dataPtr++;
				if (srcPixel < k_NUM_TRANSITIONS)
                {
                    Pixel16 * &  tI = t[srcPixel];

			        if (tI)
				    {
					    srcPixel = *tI++;
				    }
					else
                    {
                        srcPixel = DEFAULT_PIXEL[srcPixel];
                    }
				}

				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;


					Pixel16 * pDestPixel = (Pixel16 *)
                        (pSurfBase + (vdestpos * surfPitch) + (hdestpos << 1));

					*pDestPixel = 0x0000;

					hdestpos++;
				}
			}

			vaccum += vincxy;
			vdestpos++;
		}
		vpos2++;
	}
}






sint32 TiledMap::DrawOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, sint32 flags)
{
	if (!data) return 0;

	uint8 * surfBase;
	sint32	surfWidth;
	sint32	surfHeight;
	sint32	surfPitch;

    SurfaceLock lock    = SurfaceLock(surface);

	if (!surface)
    {
		surfBase    = m_surfBase;
		surfWidth   = m_surfWidth;
		surfHeight  = m_surfHeight;
		surfPitch   = m_surfPitch;
    }
    else if (lock.IsValid())
    {
		surfBase    = lock.Base();
		surfWidth   = surface->Width();
		surfHeight  = surface->Height();
		surfPitch   = surface->Pitch();
	}
    else
    {
        return AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if ((x < 0) || (y < 0)) {
		return 0;
	}

	uint32    shadowRgbMask = pixelutils_GetShadow16RGBMask();
	uint16    start        = (uint16)*data++;
	uint16    end          = (uint16)*data++;
	Pixel16 * table        = data;
	Pixel16 * dataStart    = table + (end - start + 1);

	sint32 len;

	for (sint32 j = start; j <= end; j++)
    {
		if ((y+j) >= surfHeight) continue;
		if ((sint16)table[j-start] == -1) continue;

        unsigned short *    destPixel   =
		    (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));

		Pixel16	*   rowData = dataStart + table[j-start];
		Pixel16		tag;
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
								*destPixel = pixelutils_Shadow16(*destPixel, shadowRgbMask);
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

	return 0;
}






sint32 TiledMap::DrawColorBlendedOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, Pixel16 color)
{
	if (!data || (x < 0) || (y < 0)) return 0;

	uint8 * surfBase;
	sint32	surfWidth;
	sint32	surfHeight;
	sint32	surfPitch;

    SurfaceLock lock    = SurfaceLock(surface);

	if (!surface)
    {
		surfBase    = m_surfBase;
		surfWidth   = m_surfWidth;
		surfHeight  = m_surfHeight;
		surfPitch   = m_surfPitch;
    }
    else if (lock.IsValid())
    {
		surfBase    = lock.Base();
		surfWidth   = surface->Width();
		surfHeight  = surface->Height();
		surfPitch   = surface->Pitch();
	}
    else
    {
        return AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if (x >= (surfWidth - k_TILE_GRID_WIDTH) ||
		y >= (surfHeight - k_TILE_GRID_HEIGHT)
       )
    {
		return 0;
	}

	uint32    blendRgbMask  = pixelutils_GetBlend16RGBMask();
	uint32    shadowRgbMask = pixelutils_GetShadow16RGBMask();
	uint16    start         = (uint16)*data++;
	uint16    end           = (uint16)*data++;
	Pixel16 * table         = data;
	Pixel16 * dataStart     = table + (end - start + 1);

	sint32 len;

	for (sint32 j = start; j <= end; j++)
    {
		if ((y+j) >= surfHeight) continue;
		if ((sint16)table[j-start] == -1) continue;

		unsigned short *    destPixel =
            (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));
		Pixel16 *           rowData     = dataStart + table[j-start];
		Pixel16		tag;

		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
						destPixel += (tag & 0x00FF);
					break;
				case k_TILE_COPY_RUN_ID			: {
						len = (tag & 0x00FF);

						while (len--) {
							*destPixel = pixelutils_Blend16(*rowData++, color, 64, blendRgbMask);
							destPixel++;
						}
					}
					break;
				case k_TILE_SHADOW_RUN_ID		: {
						len = (tag & 0x00FF);
						while (len--) {
							*destPixel = pixelutils_Shadow16(*destPixel, shadowRgbMask);
							destPixel++;
						}
					}
					break;
			}

		} while ((tag & 0xF000) == 0);
	}

	return 0;
}


void TiledMap::DrawColorBlendedOverlayScaled(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y,
								 sint32 destWidth, sint32 destHeight, Pixel16 color)
{
	if (!data || (x < 0) || (y < 0)) return;

	uint8 * surfBase;
	sint32	surfWidth;
	sint32	surfHeight;
	sint32	surfPitch;

    SurfaceLock lock    = SurfaceLock(surface);

	if (!surface)
    {
		surfBase    = m_surfBase;
		surfWidth   = m_surfWidth;
		surfHeight  = m_surfHeight;
		surfPitch   = m_surfPitch;
    }
    else if (lock.IsValid())
    {
		surfBase    = lock.Base();
		surfWidth   = surface->Width();
		surfHeight  = surface->Height();
		surfPitch   = surface->Pitch();
	}
    else
    {
        return;
	}

	if ((x >= (surfWidth - destWidth)) ||
		(y >= (surfHeight - destHeight))
       )
    {
		return;
	}

	surfBase += (y * surfPitch + x * 2);

	uint32    blendRgbMask = pixelutils_GetBlend16RGBMask();
	uint16    vstart       = (uint16)*data++;
	uint16    end          = (uint16)*data++;
	Pixel16 * table        = data;
	Pixel16 * dataStart    = table + (end - vstart + 1);
	sint32    vaccum       = destHeight*2 - k_TILE_GRID_HEIGHT;
	sint32    vincx        = destHeight*2;
	sint32    vincxy       = (destHeight-k_TILE_GRID_HEIGHT) * 2 ;
	sint32    vpos2        = (sint32)((double)((end+1) - destHeight) / (double)destHeight);
	sint32    vdestpos     = y;
	sint32    vend         = (end+1) - 1;

	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	for (sint32 vpos1 = 0; vpos1 < vend; ++vpos1)
    {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos2++;
				continue;
			}

			Pixel16	*   rowData1    =
                (vpos1 < vstart) ? emptyRow : dataStart + table[vpos1-vstart];
            Pixel16 *   rowData2    =
                (vpos2 < vstart) ? emptyRow : dataStart + table[vpos2-vstart];
			sint32      haccum      = destWidth*2 - k_TILE_GRID_WIDTH;
			sint32      hincx       = destWidth*2;
			sint32      hincxy      = (destWidth-k_TILE_GRID_WIDTH) * 2;
			sint32      hdestpos    = x;
			sint32      hend        = k_TILE_GRID_WIDTH-1;

			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;

			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, 0);

			for (sint32 hpos = 0; hpos < hend; ++hpos)
            {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					Pixel16 * destPixel = (Pixel16 *)
                        (surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));

					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, 0);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {

						pixel = pixel3;
						*destPixel = pixelutils_Blend16(pixel, color, 64, blendRgbMask);
					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
			}

			vaccum += vincxy;
			vdestpos++;
		}

        vpos2++;
	}
}







sint32 TiledMap::DrawColorizedOverlay(Pixel16 *data, aui_Surface *surface, sint32 x, sint32 y, Pixel16 color)
{
	if (!data || (x < 0) || (y < 0)) return 0;

	uint8 * surfBase;
	sint32	surfWidth;
	sint32	surfHeight;
	sint32	surfPitch;

    SurfaceLock lock    = SurfaceLock(surface);

	if (!surface)
    {
		surfBase    = m_surfBase;
		surfWidth   = m_surfWidth;
		surfHeight  = m_surfHeight;
		surfPitch   = m_surfPitch;
    }
    else if (lock.IsValid())
    {
		surfBase    = lock.Base();
		surfWidth   = surface->Width();
		surfHeight  = surface->Height();
		surfPitch   = surface->Pitch();
	}
    else
    {
        return AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if (!surfBase)
    {
		return 0;
	}

	uint32    shadowRgbMask = pixelutils_GetShadow16RGBMask();
	uint16    start = (uint16)*data++;
	uint16    end = (uint16)*data++;
	Pixel16 * table = data;
	Pixel16 * dataStart = table + (end - start + 1);

	sint32 len;

	for (sint32 j = start; j <= end; j++)
    {
		if ((y+j) >= surfHeight) continue;
		if ((sint16)table[j-start] == -1) continue;

		unsigned short *    destPixel =
            (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));
		Pixel16	*           rowData     = dataStart + table[j-start];
		Pixel16		tag;

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

							*destPixel = pixelutils_Shadow16(*destPixel, shadowRgbMask);
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

void TiledMap::DrawClippedColorizedOverlay(Pixel16 * data, aui_Surface & surface, sint32 x, sint32 y, Pixel16 color,
		uint8 alpha)
{
	Assert(data);
	if (!data) {
		return;
	}

	uint8 * base = surface.Buffer();
	Assert(base);
	if (!base) {
		return;
	}

	sint32 width = surface.Width();
	if (x >= width) {
		return;
	}
	sint32 height = surface.Height();
	if (y >= height) {
		return;
	}
	sint32 pixelPitch = surface.Pitch() >> 1;

	uint32    blendRgbMask = pixelutils_GetBlend16RGBMask();
	uint32    shadowRgbMask = pixelutils_GetShadow16RGBMask();
	uint16    start = (uint16)*data++;
	uint16    end   = (uint16)*data++;
	Pixel16 * table = data;
	Pixel16 * dataStart = table + (end - start + 1);

	Pixel16 * rowPixel = (Pixel16 *) (base) + pixelPitch * (y + start - 1);
	for (sint32 row = start; row <= end; row++)
	{
		rowPixel += pixelPitch;
		if (y + row < 0) {
			continue;
		}
		if (y + row >= height) {
			break;
		}
		if ((sint16)table[row - start] == -1) {
			continue;
		}

		Pixel16 * rowData     = dataStart + table[row - start];
		Pixel16 * pixel       = rowPixel + x;
		Pixel16 * rowEndPixel = rowPixel + width;
		Pixel16   tag;
		do
		{
			tag = *rowData++;
			uint16 runLength = tag & 0x00FF;
			uint16 runCode   = (tag &0x0F00) >> 8;
			Pixel16 * endPixel = pixel + runLength;
			if (endPixel < rowPixel) { // end of run is before start of surface (skip complete run)
				pixel = endPixel;
				if (runCode == k_TILE_COPY_RUN_ID) {
					rowData += runLength;
				}
			} else {
				if (pixel < rowPixel) { // start of run is before start of surface (skip part of run)
					if (runCode == k_TILE_COPY_RUN_ID)
					{
						while (pixel < rowPixel)
						{
							rowData++;
							pixel++;
						}
					} else {
						pixel = rowPixel;
					}
				}
				if (endPixel > rowEndPixel) { // end of run is after end of surface (skip part of run and next runs)
					endPixel = rowEndPixel;
				}
				switch (runCode) {
					case k_TILE_SKIP_RUN_ID:
						pixel = endPixel;
						break;
					case k_TILE_COPY_RUN_ID:
						while (pixel < endPixel) {
							*pixel = pixelutils_Blend16(*pixel, *rowData++, alpha, blendRgbMask);
							pixel++;
						}
						break;
					case k_TILE_SHADOW_RUN_ID:
						while (pixel < endPixel) {
							*pixel = pixelutils_Blend16(*pixel, pixelutils_Shadow16(*pixel, shadowRgbMask), alpha,
									blendRgbMask);
							pixel++;
						}
						break;
					case k_TILE_COLORIZE_RUN_ID:
						while (pixel < endPixel) {
							*pixel = pixelutils_Blend16(*pixel, color, alpha, blendRgbMask);
							pixel++;
						}
						break;
				}
			}
		} while (((tag & 0xF000) == 0) && (pixel < rowEndPixel));
	}
}

sint32 TiledMap::DrawColorizedOverlayIntoMix(Pixel16 *data, sint32 x, sint32 y, Pixel16 color)
{
	if (!data || (x < 0) || (y < 0)) return 0;

	uint8 * surfBase    = g_screenManager->GetSurfBase();
	if (!surfBase) return 0;

	sint32 surfHeight = g_screenManager->GetSurfHeight();
	sint32 surfPitch = g_screenManager->GetSurfPitch();

	uint32    shadowRgbMask = pixelutils_GetShadow16RGBMask();
	uint16    start = (uint16)*data++;
	uint16    end = (uint16)*data++;
	Pixel16 * table = data;
	Pixel16 * dataStart = table + (end - start + 1);

	sint32 len;

	for (sint32 j = start; j <= end; j++)
    {
		if ((y+j) >= surfHeight) continue;
		if ((sint16)table[j-start] == -1) continue;

		unsigned short *    destPixel   =
            (unsigned short *)(surfBase + ((y + j) * surfPitch) + (x * 2));

		Pixel16 *           rowData     = dataStart + table[j-start];

		Pixel16		tag;

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
							*destPixel = pixelutils_Shadow16(*destPixel, shadowRgbMask);
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
    if (!data || (x < 0) || (y < 0)) return;

	uint8 * surfBase;
	sint32	surfWidth;
	sint32	surfHeight;
	sint32	surfPitch;

    SurfaceLock lock    = SurfaceLock(surface);

	if (!surface)
    {
        surface     = m_surface;
		surfBase    = m_surfBase;
		surfWidth   = m_surfWidth;
		surfHeight  = m_surfHeight;
		surfPitch   = m_surfPitch;
    }
    else if (lock.IsValid())
    {
		surfBase    = lock.Base();
		surfWidth   = surface->Width();
		surfHeight  = surface->Height();
		surfPitch   = surface->Pitch();
	}
    else
    {
        return;
	}

	if (!surface || !surfBase)
    {
		return;
	}

	if ((x >= surface->Width() - destWidth) || (y >= surface->Height() - destHeight)) {
		return;
	}

	surfBase += (y * surfPitch + x * 2);

	uint16      vstart      = (uint16)*data++;
	uint16		end         = (uint16)*data++;
	Pixel16	*   table       = data;
	Pixel16	*   dataStart   = table + (end - vstart + 1);
	sint32      vaccum      = destHeight*2 - k_TILE_GRID_HEIGHT;
	sint32      vincx       = destHeight*2;
	sint32      vincxy      = (destHeight-k_TILE_GRID_HEIGHT) * 2 ;
	sint32      vpos2       = (sint32)((double)((end+1) - destHeight) / (double)destHeight);
	sint32      vdestpos    = y;
	sint32      vend        = (end+1) - 1;

	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	for (sint32 vpos1 = 0; vpos1 < vend; ++vpos1)
    {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {

			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos2++;
				continue;
			}

			Pixel16 *   rowData1 =
                (vpos1 < vstart) ? emptyRow : dataStart + table[vpos1-vstart];
            Pixel16 *   rowData2 =
                (vpos2 < vstart) ? emptyRow : dataStart + table[vpos2-vstart];

			sint32  haccum      = destWidth*2 - k_TILE_GRID_WIDTH;
			sint32  hincx       = destWidth*2;
			sint32  hincxy      = (destWidth-k_TILE_GRID_WIDTH) * 2;
			sint32  hdestpos    = x;
			sint32  hend        = k_TILE_GRID_WIDTH-1;

			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;
			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, flags);

			for (sint32 hpos = 0; hpos < hend; ++hpos)
            {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					Pixel16 * destPixel = (Pixel16 *)
                        (surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));

					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, flags);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {

						pixel = pixel3;

						*destPixel = pixel;
					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
			}

			vaccum += vincxy;
			vdestpos++;
		}
		vpos2++;
	}
}






void TiledMap::DrawScaledOverlayIntoMix(Pixel16 *data, sint32 x, sint32 y,
												 sint32 destWidth, sint32 destHeight)
{
	if (!data || (x < 0) || (y < 0)) return;

	sint32  surfWidth   = g_screenManager->GetSurfWidth();
	sint32  surfHeight  = g_screenManager->GetSurfHeight();

	if ((x >= surfWidth - destWidth) || (y >= surfHeight - destHeight))
	{
		return;
	}

	sint32  surfPitch   = g_screenManager->GetSurfPitch();

	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	uint8 * surfBase    = g_screenManager->GetSurfBase() + (y * surfPitch + x * 2);

	uint16			vstart = (uint16)*data++;
	uint16			end = (uint16)*data++;
	Pixel16			*table = data;
	Pixel16			*dataStart = table + (end - vstart + 1);

	sint32  vaccum      = destHeight*2 - k_TILE_GRID_HEIGHT;
	sint32  vincx       = destHeight*2;
	sint32  vincxy      = (destHeight-k_TILE_GRID_HEIGHT) * 2 ;
	sint32  vpos2       = (sint32)((double)((end+1) - destHeight) / (double)destHeight);
	sint32  vdestpos    = y;
	sint32  vend        = (end+1) - 1;

	for (sint32 vpos1 = 0; vpos1 < vend; ++vpos1)
    {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos2++;
				continue;
			}

			Pixel16	*   rowData1    =
                (vpos1 < vstart) ? emptyRow : dataStart + table[vpos1-vstart];
            Pixel16 *   rowData2    =
                (vpos2 < vstart) ? emptyRow : dataStart + table[vpos2-vstart];

			sint32  haccum = destWidth*2 - k_TILE_GRID_WIDTH;
			sint32  hincx = destWidth*2;
			sint32  hincxy = (destWidth-k_TILE_GRID_WIDTH) * 2;

			sint32  hdestpos = x;
			sint32  hend = k_TILE_GRID_WIDTH-1;

			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;

			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, 0);

			for (sint32 hpos = 0; hpos < hend; ++hpos)
            {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					Pixel16 * destPixel = (Pixel16 *)
                        (surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));

					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, 0);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {

						pixel = pixel3;

						*destPixel = pixel;
					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
			}

			vaccum += vincxy;
			vdestpos++;
		}
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

    primitives_DrawText(surface, x - 8, y - 8, buf, color , 1);

    LockSurface();
}

void TiledMap::SlowDrawText(aui_Surface *surface, char *buf, sint32 color, sint32 x, sint32 y)
{
	if (!surface) surface = m_surface;

    UnlockSurface();

    primitives_DrawText(surface, x - 32, y - 8, buf, color , 1);

    LockSurface();

}

void TiledMap::DrawTransitionTile(aui_Surface *surface, const MapPoint &pos, sint32 xpos, sint32 ypos)
{
	if (xpos < 0) return;
	ypos+=k_TILE_PIXEL_HEADROOM;
	if (ypos < 0) return;

	if (!surface)
    {
        surface = m_surface;
        if (!surface) return;
    }

	if ((xpos > surface->Width() - k_TILE_PIXEL_WIDTH) ||
		(ypos > surface->Height() - k_TILE_PIXEL_HEIGHT)
       )
    {
		return;
    }

	TileInfo *  tileInfo    = GetTileInfo(pos);
	Assert(tileInfo != NULL);
	if (tileInfo == NULL)
		return;

	uint16      index       = tileInfo->GetTileNum();
	BaseTile*   baseTile    = m_tileSet->GetBaseTile(index);
	if (baseTile == NULL)
		return;

    Pixel16 *   data        = baseTile->GetTileData();

	sint32 tilesetIndex = g_theTerrainDB->Get(tileInfo->GetTerrainType())->GetTilesetIndex();

	uint16 tilesetIndex_short = (uint16) tilesetIndex;

#ifdef _DEBUG
	Assert(tilesetIndex == ((sint32) tilesetIndex_short));
#endif

	Pixel16	*   tileData[k_NUM_TRANSITIONS];
    for (uint16 i = 0; i < k_NUM_TRANSITIONS; ++i)
    {
	    tileData[i] = m_tileSet->GetTransitionData
                        (tilesetIndex_short, tileInfo->GetTransition(i), i);
    }

	BaseTile *  transitionBuffer =
        m_tileSet->GetBaseTile(static_cast<uint16>((tilesetIndex * 100) + 99));
	Pixel16 *   transData       =
        (transitionBuffer) ? transitionBuffer->GetTileData() : NULL;
	Pixel16 *   transDataPtr    = transData;

	Pixel16 *   dataPtr         = data;
	uint8 *     pSurfBase       = m_surfBase;
	sint32      surfPitch       = m_surfPitch;

	Pixel16 srcPixel;

	uint16 *pDestPixel = (Pixel16 *)(pSurfBase + ypos * surfPitch + 2 * xpos);

	{
		for (sint32 y = 0; y < k_TILE_PIXEL_HEIGHT; y++)
        {
			sint32  startX  = StartPixel(y);
			sint32  endX    = k_TILE_PIXEL_WIDTH - startX;

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
								++pedx;
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
				for (sint32 x = startX; x<endX; x++)
				{
					srcPixel = *dataPtr++;
					if (srcPixel < k_NUM_TRANSITIONS)
					{
                        Pixel16 * &  tI = tileData[srcPixel];

			            if (tI)
				        {
					        srcPixel = *tI++;
				        }
						else
						{
							srcPixel = DEFAULT_PIXEL[srcPixel];
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
    if (x < 0) return;
    y+=GetZoomTileHeadroom();
    if (y < 0) return;

	uint8 * surfBase;
	sint32	surfWidth;
	sint32	surfHeight;
	sint32	surfPitch;

    SurfaceLock lock    = SurfaceLock(surface);

	if (!surface)
    {
        surface     = m_surface;
		surfBase    = m_surfBase;
		surfWidth   = m_surfWidth;
		surfHeight  = m_surfHeight;
		surfPitch   = m_surfPitch;
    }
    else if (lock.IsValid())
    {
		surfBase    = lock.Base();
		surfWidth   = surface->Width();
		surfHeight  = surface->Height();
		surfPitch   = surface->Pitch();
	}
    else
    {
        return;
	}

	if (!surface ||
        (x > surface->Width() - destWidth) ||
		(y >= surface->Height() - destHeight)
       )
    {
		return;
	}

	TileInfo *  tileInfo = GetTileInfo(pos);
	Assert(tileInfo != NULL);
	if (tileInfo == NULL)
    {
		return;
	}

	uint16      index = tileInfo->GetTileNum();
	BaseTile *  baseTile = m_tileSet->GetBaseTile(index);
	if (baseTile == NULL)
    {
		return;
	}

	Pixel16 *data = baseTile->GetTileData();

	sint32 tilesetIndex = g_theTerrainDB->Get(tileInfo->GetTerrainType())->GetTilesetIndex();
	uint16 tilesetIndex_short = (uint16) tilesetIndex;

    Pixel16 * t[k_NUM_TRANSITIONS];
    for (uint16 i = 0; i < k_NUM_TRANSITIONS; ++i)
    {
        t[i] = m_tileSet->GetTransitionData
                (tilesetIndex_short, tileInfo->GetTransition(i), i);
    }

	BaseTile *  transitionBuffer    =
        m_tileSet->GetBaseTile(static_cast<uint16>((tilesetIndex * 100) + 99));
	Pixel16 *   transData           =
        (transitionBuffer) ? transitionBuffer->GetTileData() : NULL;
	Pixel16 *   transDataPtr        = transData;
    Pixel16 *   dataPtr             = data;

	Pixel16 srcPixel, transPixel = 0;

	sint32 vaccum   = destHeight*2 - k_TILE_PIXEL_HEIGHT;
	sint32 vincx    = destHeight*2;
	sint32 vincxy   = (destHeight - k_TILE_PIXEL_HEIGHT) * 2 ;
	sint32 vpos2    =
        (sint32)((double)(k_TILE_PIXEL_HEIGHT - destHeight) / (double)destHeight);
	sint32 vdestpos = y;
	sint32 vend     = k_TILE_PIXEL_HEIGHT - 1;

	for (sint32 vpos1 = 0; vpos1 < vend; ++vpos1)
    {
		sint32  startX  = StartPixel(vpos1);
        sint32  endX    = k_TILE_PIXEL_WIDTH - startX;

		if (vaccum < 0) {
			vaccum += vincx;

			for (sint32 i=startX; i<endX; i++) {
				if(transData) {
					transPixel = *transDataPtr++;
				}

				srcPixel = *dataPtr++;
                if (srcPixel < k_NUM_TRANSITIONS)
                {
                    Pixel16 * &  tI = t[srcPixel];

			        if (tI)
				    {
					    srcPixel = *tI++;
				    }
					else if (transData)
                    {
                        srcPixel = transPixel;
                    }
                }
			}
		}
        else
        {
			sint32 haccum   = destWidth * 2 - k_TILE_PIXEL_WIDTH;
			sint32 hincx    = destWidth * 2;
			sint32 hincxy   = (destWidth - k_TILE_PIXEL_WIDTH) * 2;
			sint32 hdestpos = x + (sint32)((double)startX * m_scale);
			sint32 hpos     = startX;
			sint32 hend     = endX;

			while (hpos < hend)
            {
				if(transData)
					transPixel = *transDataPtr++;

				srcPixel = *dataPtr++;
				if (srcPixel < k_NUM_TRANSITIONS)
                {
                    Pixel16 * &  tI = t[srcPixel];

			        if (tI)
				    {
					    srcPixel = *tI++;
				    }
					else if (transData)
                    {
                        srcPixel = transPixel;
                    }
                    else
                    {
						srcPixel = DEFAULT_PIXEL[srcPixel];
                    }
				}

				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					Pixel16 * pDestPixel = (Pixel16 *)
                        (surfBase + (vdestpos * surfPitch) + (hdestpos << 1));
					*pDestPixel = srcPixel;

					hdestpos++;
				}
				hpos++;
			}

			vaccum += vincxy;
			vdestpos++;
		}

        vpos2++;
	}
}

#if 0   // Unused (for water animations?)
#define k_MAX_WATER_DISPLACEMENTS		14
sint32			inctable[k_MAX_WATER_DISPLACEMENTS] = {1, 2, 1, 0, -1, 1, 0, -1, -2, -1, 0, 1,  -1, 0};
sint32			tinc[2]={0,1};
#endif

void TiledMap::DrawWater(void)
{
}

void TiledMap::DrawCityNames(aui_Surface * surf, sint32 layer)
{
	if (!m_font) {
		return;
	}

	sint32 showCityProd = g_theProfileDB->IsShowCityProduction();

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

			fog = m_localVision && m_localVision->IsExplored(pos) && !m_localVision->IsVisible(pos);

			if(m_localVision->IsExplored(pos) || g_fog_toggle || g_god) {

				bool    drawCity             = false;
				bool    drawOurCity          = false;
				bool    drawQueueEmpty       = false;
				sint32  pop                  = 0;
				sint32  nextpop              = 0;
				MBCHAR *name                 = NULL;
				const char *buildItemName    = NULL;
				sint32  buildItemTime        = 0;

				sint32  owner = 0;

				sint32  franchiseLoss        = 0;
				sint32  convertedLoss        = 0;
				BOOL	isBioInfected        = FALSE,
						isNanoInfected       = FALSE,
						isConverted          = FALSE,
						isFranchised         = FALSE,
						isInjoined           = FALSE,
						wasHappinessAttacked = FALSE,
						isRioting            = FALSE,
						hasAirport           = FALSE,
						hasSleepingUnits     = FALSE,
						isWatchful           = FALSE,
						isCapitol            = FALSE,
						hasReligionIcon      = FALSE,
						hasSpecialIcon       = FALSE,
						isProdIcon			 = FALSE,
						isPollutionRisk      = FALSE;
				sint32	bioInfectedOwner     = 0,
						nanoInfectedOwner    = 0,
						convertedOwner       = 0,
						franchiseOwner       = 0,
						injoinedOwner        = 0,
						happinessAttackOwner = 0;

				UnseenCellCarton	ucell;
				// Don't forget if fog was toggled
				if (m_localVision->GetLastSeen(pos, ucell) && !g_fog_toggle)
				{
					pop                  = ucell.m_unseenCell->GetCitySize();
					name                 = (MBCHAR *)ucell.m_unseenCell->GetCityName();
					owner                = ucell.m_unseenCell->GetCityOwner();
					isBioInfected        = ucell.m_unseenCell->IsBioInfected();
					isNanoInfected       = ucell.m_unseenCell->IsNanoInfected();
					isConverted          = ucell.m_unseenCell->IsConverted();
					isFranchised         = ucell.m_unseenCell->IsFranchised();
					Unit unit;
					if (g_theWorld->GetTopVisibleUnit(pos,unit) && unit.IsCity()){// city might be destroyed
					    convertedLoss    = unit.GetData()->GetCityData()->GetConvertedGold();
					    franchiseLoss    = unit.GetData()->GetCityData()->GetProductionLostToFranchise();
					    }
					else{// in case city was destroyed since last visit
					    isConverted      = false; // do not show conversion city icon to not give away that city was destroyed
					    isFranchised     = false; // do not show franchise city icon to not give away that city was destroyed
					    convertedLoss    = 0; // no info on loss if city was destroyed
					    franchiseLoss    = 0; // no info on loss if city was destroyed
					    }
					isInjoined           = ucell.m_unseenCell->IsInjoined();
					wasHappinessAttacked = ucell.m_unseenCell->WasHappinessAttacked();
					isRioting            = ucell.m_unseenCell->IsRioting();
					hasAirport           = ucell.m_unseenCell->HasAirport();
					if (owner == g_selected_item->GetVisiblePlayer())
						hasSleepingUnits = ucell.m_unseenCell->HasSleepingUnits();
					else
						hasSleepingUnits = FALSE;

					isWatchful           = ucell.m_unseenCell->IsWatchful();

					bioInfectedOwner     = (sint32)ucell.m_unseenCell->GetBioInfectedOwner();
					nanoInfectedOwner    = ucell.m_unseenCell->GetNanoInfectedOwner();
					convertedOwner       = ucell.m_unseenCell->GetConvertedOwer();
					franchiseOwner       = ucell.m_unseenCell->GetFranchiseOwner();
					injoinedOwner        = ucell.m_unseenCell->GetInjoinedOwner();
					happinessAttackOwner = ucell.m_unseenCell->GetHappinessAttackOwner();

					slaveBits            = ucell.m_unseenCell->GetSlaveBits();
					isCapitol            = ucell.m_unseenCell->IsCapitol(); //emod
					hasReligionIcon      = ucell.m_unseenCell->IsReligionIcon(); //emod
					hasSpecialIcon       = ucell.m_unseenCell->IsSpecialIcon(); //emod
					isPollutionRisk      = ucell.m_unseenCell->IsPollutionRisk();

					if (pop > 0)
						drawCity = true;
				}
				else
				{
					Unit unit;
					// if there's a unit at pos
					if (g_theWorld->GetTopVisibleUnit(pos,unit)) {
						//and it's a city visible to the current player
						if (unit.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()) && unit.IsCity())
						{
							CityData *cityData   = unit.GetData()->GetCityData();

							if (!unit.GetActor())
							{
								pop              = cityData->PopCount();
								nextpop          = cityData->TurnsToNextPop();
								owner            = cityData->GetOwner();
							}
							else
							{
								pop              = unit.GetActor()->GetCitySize();
								nextpop          = unit.GetActor()->GetNextPop();
								owner            = unit.GetActor()->GetPlayerNum();
							}

							if (showCityProd)
							{
								BuildQueue *bq = cityData->GetBuildQueue();

								if(bq->GetHead())
								{
									BuildNode *bn = bq->GetHead();

									buildItemTime = cityData->AlreadyBoughtFront() ? 0 : cityData->HowMuchLonger();

									switch(bn->m_category) {
										case k_GAME_OBJ_TYPE_UNIT:
											buildItemName = g_theUnitDB->Get(bn->m_type)->GetNameText();
											break;
										case k_GAME_OBJ_TYPE_IMPROVEMENT:
											buildItemName = g_theBuildingDB->Get(bn->m_type)->GetNameText();
											break;
										case k_GAME_OBJ_TYPE_WONDER:
											buildItemName = g_theWonderDB->Get(bn->m_type)->GetNameText();
											break;
										case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
											buildItemName = g_theStringDB->GetNameStr("INFRASTRUCTURE");
											buildItemTime = 0;
											break;
										case k_GAME_OBJ_TYPE_CAPITALIZATION:
											buildItemName = g_theStringDB->GetNameStr("CAPITALIZATION");
											buildItemTime = 0;
											break;
									}
								}
								else
								{
									buildItemName = g_theStringDB->GetNameStr("BUILDINGNONE");
									buildItemTime = 0;
								}
							}

							name                 = cityData->GetName();
							isBioInfected        = cityData->IsBioInfected();
							isNanoInfected       = cityData->IsNanoInfected();
							isConverted          = cityData->IsConverted();
							isFranchised         = cityData->IsFranchised();
							convertedLoss        = cityData->GetConvertedGold();
							franchiseLoss        = cityData->GetProductionLostToFranchise();
							isInjoined           = cityData->IsInjoined();
							wasHappinessAttacked = cityData->WasHappinessAttacked();
							isWatchful           = cityData->IsWatchful();
							isCapitol            = cityData->IsCapitol();
							bioInfectedOwner     = cityData->GetBioInfectedBy();
							nanoInfectedOwner    = cityData->GetNanoInfectedBy();
							convertedOwner       = cityData->IsConvertedTo();
							franchiseOwner       = cityData->GetFranchiseOwner();
							injoinedOwner        = cityData->InjoinedBy();
							happinessAttackOwner = cityData->GetHappinessAttackedBy();
							slaveBits            = cityData->GetSlaveBits();
							isRioting            = cityData->GetIsRioting();
							hasAirport           = cityData->HasAirport();

							sint32 pollution     = cityData->GetPollution();
							isPollutionRisk      = (pollution > g_theConstDB->Get(0)->GetLocalPollutionLevel());

							if (owner == g_selected_item->GetVisiblePlayer())
								hasSleepingUnits = cityData->HasSleepingUnits();
							else
								hasSleepingUnits = FALSE;

							drawCity = true;

							if(owner == g_selected_item->GetVisiblePlayer())
							{
								drawOurCity = true;
								hasReligionIcon  = cityData->HasReligionIcon();
								hasSpecialIcon   = cityData->HasSpecialIcon();

								if (!cityData->GetBuildQueue()->GetHead()) {
									drawQueueEmpty   = true;
								}
							}
						}
					}
				}

				if (drawCity)
				{
					sint32 x,y;
					maputils_MapXY2PixelXY(mapX, mapY, &x, &y);
					y-= (sint32)(k_TILE_PIXEL_HEADROOM * m_scale) / 2;

					// Move city names up if city production is on, so it doesn't cover top of city sprite.
					if (drawOurCity && showCityProd && buildItemName) {
						y -= 18;
					}

					if (x >= 0 && y >= 0 && x < surfWidth && y < surfHeight)
					{
						SurfaceLock surfaceLock(surf);
						uint32 alpha = g_theProfileDB->GetShowCityNameAlpha();

						Pixel16 playerColor = GetPlayerColor(owner);
						Pixel16 black       = GetColor(COLOR_BLACK);

						POINT cityNamePos = { x, y };
						// only show empty queue: when production is not shown
						COLORREF cityNameColorRef = GetCityNameColorRef(fog, isRioting,
								drawQueueEmpty && !(drawOurCity && showCityProd && buildItemName));
						RECT outerCityNameRect = DrawCityName(*surf, name, cityNamePos, cityNameColorRef, alpha);

						if (isCapitol)
						{
							POINT capitalIconPos = {outerCityNameRect.right, outerCityNameRect.top - 1 };
							RECT capitolRect = DrawCityIsCapitol(*surf, capitalIconPos, black, alpha);
							outerCityNameRect.right += (capitolRect.right - capitolRect.left);
						}
						if (hasSleepingUnits)
						{
							POINT sleepingUnitsPos = {outerCityNameRect.right - 1, outerCityNameRect.top };
							RECT sleepingUnitsRect = DrawCitySleepingUnits(*surf, sleepingUnitsPos, playerColor,
									alpha);
							outerCityNameRect.right += (sleepingUnitsRect.right - sleepingUnitsRect.left - 1);
						}

						RECT populationRect = DrawCityPopulation(*surf, pop, outerCityNameRect, playerColor, alpha);
						outerCityNameRect.left = populationRect.left;

						if (owner == g_selected_item->GetVisiblePlayer())
						{
							RECT nextPopulationRect = DrawCityNextPopulation(*surf, nextpop, outerCityNameRect,
									playerColor, alpha);
							outerCityNameRect.right = nextPopulationRect.right;
						}

						if (isWatchful)
						{
							POINT watchfulIconPos = {outerCityNameRect.right, outerCityNameRect.top };
							RECT watchfulRect = DrawCityIsWatchful(*surf, watchfulIconPos, playerColor, alpha);
							outerCityNameRect.right += (watchfulRect.right - watchfulRect.left + 1);
						}
						InflateRect(&outerCityNameRect, 1, 1);

						if (drawOurCity && showCityProd && buildItemName)
						{
							POINT productionPos = { outerCityNameRect.left, outerCityNameRect.bottom - 1 };
							RECT outerBuildItemRect = DrawCityProductionIcon(*surf, pop, productionPos, playerColor,
									alpha);

							POINT buildItemNamePos = { outerBuildItemRect.right + 1, outerBuildItemRect.top + 1 };
							COLORREF buildItemNameColor = GetCityNameColorRef(fog, isRioting, drawQueueEmpty);
							RECT buildItemNameRect = DrawCityBuildItemName(*surf, buildItemName, buildItemNamePos,
									buildItemNameColor, alpha);
							InflateRect(&buildItemNameRect, 1, 1);
							primitives_ClippedFrame16(*surf, buildItemNameRect.left, buildItemNameRect.top,
									buildItemNameRect.right - buildItemNameRect.left,
									buildItemNameRect.bottom - buildItemNameRect.top, playerColor, alpha);
							outerBuildItemRect.right = buildItemNameRect.right;

							RECT buildItemTimeRect = DrawCityBuildItemTime(*surf, buildItemTime,
									outerBuildItemRect, playerColor, alpha);
							outerBuildItemRect.right = buildItemTimeRect.right;

							AddDirtyRectToMix(outerBuildItemRect);
						}
						RECT cityIconsRect = DrawCityIcons(*surf, owner, fog, outerCityNameRect,isBioInfected,
								isNanoInfected, isConverted, isFranchised, isInjoined, wasHappinessAttacked,
								bioInfectedOwner, nanoInfectedOwner, convertedOwner, convertedLoss, franchiseOwner,
								franchiseLoss, injoinedOwner, happinessAttackOwner,hasAirport, isPollutionRisk, alpha);
						if (fog)
						{
							cityIconsRect.left -= 1;
							primitives_ClippedShadowRect16(*surf, cityIconsRect);
						}
						AddDirtyRectToMix(cityIconsRect);

						RECT cityEnslavedRect = DrawCityEnslaved(*surf, outerCityNameRect, owner, slaveBits, alpha);
						if (fog) {
							primitives_ClippedShadowRect16(*surf, cityEnslavedRect);
						}
						AddDirtyRectToMix(cityEnslavedRect);

						primitives_ClippedFrame16(*surf, outerCityNameRect.left, outerCityNameRect.top,
								outerCityNameRect.right - outerCityNameRect.left,
								outerCityNameRect.bottom - outerCityNameRect.top, playerColor, alpha);
						if (fog) {
							primitives_ClippedShadowRect16(*surf, outerCityNameRect);
						}
						AddDirtyRectToMix(outerCityNameRect);

						RECT religionIconsRect = DrawCityReligionIcons(*surf, pos, owner, cityIconsRect,
								hasReligionIcon, alpha);
						if (fog)
						{
							religionIconsRect.left -= 1;
							primitives_ClippedShadowRect16(*surf, religionIconsRect);
						}
						AddDirtyRectToMix(religionIconsRect);

						/* As far as could be determined this is not used.
						RECT specialIconsRect = DrawCitySpecialIcons(*surf, pos, owner, religionIconsRect,
								hasSpecialIcon, alpha);
						if (fog)
						{
							specialIconsRect.left -= 1;
							primitives_ClippedShadowRect16(*surf, specialIconsRect);
						}
						AddDirtyRectToMix(specialIconsRect);
						*/
					}
				}
			}
		}
	}
}

COLORREF TiledMap::GetCityNameColorRef(bool fog, bool isRioting, bool drawQueueEmpty)
{
	if (fog) {
		return GetColorRef(COLOR_WHITE);
	} else {
		if (isRioting) {
			return GetColorRef(COLOR_RED);
		} else if(drawQueueEmpty) {
			return GetColorRef(COLOR_YELLOW);
		} else {
			return GetColorRef(COLOR_WHITE);
		}
	}
	return GetColorRef(COLOR_WHITE);
}

RECT TiledMap::DrawCityName(aui_Surface & surf, const MBCHAR * const name, const POINT & position, COLORREF colorRef,
		uint8 alpha)
{
	RECT cityNameRect = {
			position.x,
			position.y,
			position.x + m_font->GetStringWidth(name),
			position.y + m_font->GetMaxHeight()
	};
	RECT outerNameRect = cityNameRect;
	InflateRect(&outerNameRect, 2, 1); // expand for borders
	primitives_ClippedPaint16(surf, outerNameRect.left, outerNameRect.top, outerNameRect.right - outerNameRect.left,
			outerNameRect.bottom - outerNameRect.top, GetColor(COLOR_BLACK), alpha);
	RECT clipRect = primitives_GetScreenAdjustedRectCopy(&surf, cityNameRect);
	m_font->DrawString(&surf, &cityNameRect, &clipRect, name, 0, colorRef, 0);
	return outerNameRect;
}

RECT TiledMap::DrawCityIsCapitol(aui_Surface & surf, const POINT & position, Pixel16 color, uint8 alpha)
{
	static const MAPICON capital = MAPICON_CAPITOL;
	Pixel16 * capitalIcon = GetTileSet()->GetMapIconData(capital);
	Assert(capitalIcon);
	if (capitalIcon)
	{
		DrawClippedColorizedOverlay(capitalIcon, surf, position.x, position.y, color, alpha);

		// Draw over borders
		POINT iconDimensions = GetTileSet()->GetMapIconDimensions(capital);
		RECT border = { position.x, position.y, position.x + iconDimensions.x, position.y + iconDimensions.y };
		return border;
	}
	return RECT { position.x, position.y, position.x, position.y };
}

RECT TiledMap::DrawCitySleepingUnits(aui_Surface & surf, const POINT & position, Pixel16 color, uint8 alpha)
{
	static const MAPICON sleepingUnits = MAPICON_SLEEPINGUNITS;
	Pixel16 * sleepingUnitsIcon = GetTileSet()->GetMapIconData(sleepingUnits);
	Assert(sleepingUnitsIcon);
	if (sleepingUnitsIcon)
	{
		POINT iconDimensions = GetTileSet()->GetMapIconDimensions(MAPICON_SLEEPINGUNITS);
		RECT rect = { position.x, position.y, position.x + iconDimensions.x + 1, position.y + iconDimensions.y + 1 };
		primitives_ClippedPaint16(surf, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
				GetColor(COLOR_BLACK), alpha);
		DrawClippedColorizedOverlay(sleepingUnitsIcon, surf, position.x, position.y, color, alpha);
		return rect;
	}
	return RECT { position.x, position.y, position.x, position.y };
}

RECT TiledMap::DrawCityIsWatchful(aui_Surface & surf, const POINT & position, Pixel16 color, uint8 alpha)
{
	static const MAPICON capital = MAPICON_WATCHFUL;
	Pixel16 * watchFulIcon = GetTileSet()->GetMapIconData(capital);
	Assert(watchFulIcon);
	if (watchFulIcon)
	{
		POINT iconDimensions = GetTileSet()->GetMapIconDimensions(capital);
		RECT rect = { position.x+1, position.y, position.x + iconDimensions.x-1, position.y + iconDimensions.y+1 };
		DrawClippedColorizedOverlay(watchFulIcon, surf, position.x, position.y, color, alpha);
		return rect;
	}

	return RECT { position.x, position.y, position.x, position.y };
}

RECT TiledMap::DrawCityPopulation(aui_Surface & surf, sint32 population, const RECT & rect, Pixel16 color, uint8 alpha)
{
	MBCHAR populationString[80];
	sprintf(populationString,"%i", population);
	RECT populationRect = { rect.left - (population < 10 ? 13 : 17), rect.top, rect.left, rect.bottom };
	primitives_ClippedPaint16(surf, populationRect.left, populationRect.top,
			populationRect.right - populationRect.left, populationRect.bottom - populationRect.top, color, alpha);
	COLOR fontColor = ColorSet::UseDarkFontColor(color) ? COLOR_BLACK : COLOR_WHITE;
	RECT textRect = { populationRect.left + (population < 10 ? 4 : 2), populationRect.top + 1,
				populationRect.right - (population < 10 ? 3 : 1), populationRect.bottom - 1 };
	RECT clipRect = primitives_GetScreenAdjustedRectCopy(&surf, textRect);
	m_font->DrawString(&surf, &textRect, &clipRect, populationString, 0, GetColorRef(fontColor));
	return populationRect;
}

RECT TiledMap::DrawCityNextPopulation(aui_Surface & surf, sint32 nextPopulation, const RECT & rect, Pixel16 color,
		uint8 alpha)
{
	MBCHAR nextPopulationString[80];
	if (nextPopulation < THRESHOLD_SLOW_GROWTH) {
		sprintf(nextPopulationString, "%i", nextPopulation);
	} else {
		sprintf(nextPopulationString, "%s", TEXT_NONE);
	}

	sint32 offsetX = nextPopulation < 10 ? 1 : 0;
	sint32 width = nextPopulation < 10 ? 12 : (nextPopulation < THRESHOLD_SLOW_GROWTH ? 17 : 15);
	RECT nextPopulationRect = { rect.right + 1, rect.top, rect.right + width, rect.bottom - 1 };
	primitives_ClippedPaintRect16(surf, nextPopulationRect, GetColor(COLOR_BLACK), alpha);

	RECT textRect = { nextPopulationRect.left + 2 + offsetX, nextPopulationRect.top + 1,
				nextPopulationRect.right - offsetX, nextPopulationRect.bottom - 3 };
	RECT clipRect = primitives_GetScreenAdjustedRectCopy(&surf, textRect);
	m_font->DrawString(&surf, &textRect, &clipRect, nextPopulationString, 0, GetColorRef(COLOR_WHITE));

	InflateRect(&nextPopulationRect, 1, 1);
	primitives_ClippedFrameRect16(surf, nextPopulationRect, color, alpha);
	return nextPopulationRect;
}

RECT TiledMap::DrawCityProductionIcon(aui_Surface & surf, sint32 population, const POINT & position, Pixel16 color,
		uint8 alpha)
{
	const MAPICON production = population > 9 ? MAPICON_PRODUCTIONNEWBIG : MAPICON_PRODUCTIONNEW;
	Pixel16 *productionIcon = GetTileSet()->GetMapIconData(production);
	Assert(productionIcon);
	if (productionIcon)
	{
		POINT iconDimensions = GetTileSet()->GetMapIconDimensions(production);
		RECT rect = {position.x, position.y, position.x + iconDimensions.x - 1, position.y + iconDimensions.y };
		DrawClippedColorizedOverlay(productionIcon, surf, position.x, position.y, color, alpha);
		return rect;
	}
	return RECT{ position.x, position.y, position.x, position.y};
}

RECT TiledMap::DrawCityBuildItemName(aui_Surface & surf, const MBCHAR * const name, const POINT & position,
		COLORREF colorRef, uint8 alpha)
{
	sint32 widthBuildItemName  = m_font->GetStringWidth(name);
	sint32 heightBuildItemName = m_font->GetMaxHeight();

	// The building name inner rectangle
	RECT buildNameItemRect = {
		position.x,
		position.y,
		position.x + m_font->GetStringWidth(name) + 4,
		position.y + m_font->GetMaxHeight() + 2
	};

	primitives_ClippedPaint16(surf, buildNameItemRect.left, buildNameItemRect.top,
			buildNameItemRect.right - buildNameItemRect.left, buildNameItemRect.bottom - buildNameItemRect.top,
			GetColor(COLOR_BLACK), alpha);

	RECT textRect = buildNameItemRect;
	InflateRect(&textRect, -2, -1);
	RECT clipRect = primitives_GetScreenAdjustedRectCopy(&surf, textRect);
	m_font->DrawString(&surf, &textRect, &clipRect, name, 0, colorRef, 0);

	return buildNameItemRect;
}

RECT TiledMap::DrawCityBuildItemTime(aui_Surface & surf, sint32 buildItemTime, const RECT & rect, Pixel16 color,
		uint8 alpha)
{
	MBCHAR buildItemNameString[80];
	if (buildItemTime < THRESHOLD_PROD_TIME && buildItemTime != 0) {
		sprintf(buildItemNameString, "%i", buildItemTime);
	} else {
		sprintf(buildItemNameString, "%s", TEXT_NONE);
	}

	sint32 offsetX = (buildItemTime == 0 || buildItemTime >= THRESHOLD_PROD_TIME) ? 2 : (buildItemTime < 10 ? 3 : 2);
	sint32 width = (buildItemTime == 0 || buildItemTime >= THRESHOLD_PROD_TIME) ? 15 : (buildItemTime < 10 ? 12 : 17);
	RECT builtItemTimeRect = { rect.right, rect.top + 1, rect.right + width, rect.bottom - 1 };
	primitives_ClippedPaint16(surf, builtItemTimeRect.left, builtItemTimeRect.top,
			builtItemTimeRect.right - builtItemTimeRect.left, builtItemTimeRect.bottom - builtItemTimeRect.top,
			GetColor(COLOR_BLACK), alpha);

	RECT textRect = { builtItemTimeRect.left + offsetX, builtItemTimeRect.top + 1,
	                  builtItemTimeRect.right - offsetX + 1, builtItemTimeRect.bottom - 2 };
	RECT clipRect = primitives_GetScreenAdjustedRectCopy(&surf, textRect);
	m_font->DrawString(&surf, &textRect, &clipRect, buildItemNameString, 0, GetColorRef(COLOR_WHITE));

	InflateRect(&builtItemTimeRect, 1, 1);
	primitives_ClippedFrame16(surf, builtItemTimeRect.left, builtItemTimeRect.top,
			builtItemTimeRect.right - builtItemTimeRect.left,
			builtItemTimeRect.bottom - builtItemTimeRect.top, color, alpha);
	return builtItemTimeRect;
}

//----------------------------------------------------------------------------
//
// Name       : TiledMap::DrawCityIcons
//
// Description: Draw the temporary icons above a city
//
// Parameters : aui_Surface *     surf                 : the surface to draw on
//              sint32            owner                : the city's owner
//              BOOL              fog                  : if TRUE then draw city under fog of war
//              RECT              &outerCityRect       : the rectangle that contains the city's outer rectangle
//                                                       (used to set screen location)
//              BOOL              isBioInfected        : if TRUE then draw MAPICON_BIODISEASE
//              BOOL              isNanoInfected       : etc.
//              BOOL              isConverted          :
//              BOOL              isFranchised         :
//              BOOL              isInjoined           :
//              BOOL              wasHappinessAttacked :
//              sint32            bioInfectedOwner     : the player who bio infected the city
//              sint32            nanoInfectedOwner    : the player who nano infected the city
//              sint32            convertedOwner       : the player who converted the city
//              sint32            franchiseOwner       : the player who franchised the city
//              sint32            injoinedOwner        : the player who filed an injunction against the city
//              sint32            happinessAttackOwner : the player who happiness attacked the city
//              BOOL              hasAirport           : not used
//				BOOL              isPollutionRisk      : if city is at risk of creating polluted tiles
//                                                     : then draw pollution icon.
//
// Globals    :
//
// Returns    :
//
// Remark(s)  :
//
//----------------------------------------------------------------------------
RECT TiledMap::DrawCityIcons(aui_Surface & surf, sint32 owner, bool fog, const RECT & outerCityRect, bool isBioInfected,
		bool isNanoInfected, bool isConverted, bool isFranchised, bool isInjoined, bool wasHappinessAttacked,
		sint32 bioInfectedOwner, sint32 nanoInfectedOwner, sint32 convertedOwner, sint32 convertedLoss,
		sint32 franchiseOwner, sint32 franchiseLoss, sint32 injoinedOwner, sint32 happinessAttackOwner,
		bool hasAirport, bool isPollutionRisk, uint8 alpha)
{
	TileSet * tileSet     = GetTileSet();
	RECT      outerRect   = { outerCityRect.right, outerCityRect.bottom, outerCityRect.right, outerCityRect.bottom };
	Pixel16   playerColor = GetPlayerColor(owner);

	if (isPollutionRisk)
	{
		static const MAPICON pollution = MAPICON_POLLUTION;
		Pixel16 * icon = tileSet->GetMapIconData(pollution);
		Assert(icon);
		if (icon)
		{
			POINT dimensions = tileSet->GetMapIconDimensions(pollution);

			DrawClippedColorizedOverlay(icon, surf, outerRect.right-1, outerRect.bottom - dimensions.y, playerColor,
					alpha);
			primitives_ClippedFrame16(surf, outerRect.right - 1, outerRect.bottom - dimensions.y, dimensions.x,
					dimensions.y, playerColor, alpha);
			outerRect.top   = outerRect.bottom - dimensions.y;
			outerRect.right += dimensions.x - 1;
		}
	} else {
		// No icon in player color, do not overwrite frame in player color
		outerRect.right += 1;
	}

	if (isConverted)
	{
		static const MAPICON conversion = MAPICON_CONVERSION;
		Pixel16 * icon = tileSet->GetMapIconData(conversion);
		Assert(icon);
		if (icon)
		{
			POINT dimensions = tileSet->GetMapIconDimensions(conversion);

			Pixel16 color = GetPlayerColor(convertedOwner);
			DrawClippedColorizedOverlay(icon, surf, outerRect.right-1, outerRect.bottom - dimensions.y, color, alpha);
			primitives_ClippedFrame16(surf, outerRect.right - 1, outerRect.bottom - dimensions.y, dimensions.x,
					dimensions.y, color, alpha);

			sint32 myOwner = g_selected_item->GetVisiblePlayer();
			if (convertedLoss && myOwner >= 0 && (myOwner == owner || myOwner == convertedOwner))
			{
				MBCHAR convertedLossString[80];
				sprintf(convertedLossString, "%i", convertedLoss);

				RECT textRect = { outerRect.right + (convertedLoss < 10 ? 6 : 2), outerRect.bottom - 17,
						outerRect.right + dimensions.x - 1, outerRect.bottom - 5 };
				if (ColorSet::UseDarkFontColor(color))
				{
					RECT darkenRect = {textRect.left - (convertedLoss < 10 ? 3 : 1), textRect.top + 2,
					                   textRect.right - (convertedLoss < 10 ? 4 : 2),textRect.bottom + 1 };
					primitives_ClippedShadowRect16(surf, darkenRect);
				}
				RECT clipRect = primitives_GetScreenAdjustedRectCopy(&surf, textRect);
				m_font->DrawString(&surf, &textRect, &clipRect, convertedLossString, 0, GetColorRef(COLOR_WHITE));
			}
			outerRect.top   = outerRect.bottom - dimensions.y;
			outerRect.right += dimensions.x - 1;
		}
	}

	if (isFranchised)
	{
		static const MAPICON franchise = MAPICON_FRANCHISE;
		Pixel16 * icon = tileSet->GetMapIconData(franchise);
		Assert(icon);
		if (icon)
		{
			POINT dimensions = tileSet->GetMapIconDimensions(franchise);

			Pixel16 color = GetPlayerColor(franchiseOwner);
			DrawClippedColorizedOverlay(icon, surf, outerRect.right-1, outerRect.bottom - dimensions.y, color, alpha);
			primitives_ClippedFrame16(surf, outerRect.right - 1, outerRect.bottom - dimensions.y, dimensions.x,
					dimensions.y, color, alpha);

			sint32 myOwner = g_selected_item->GetVisiblePlayer();
			if (franchiseLoss && myOwner >= 0 && (myOwner == owner || myOwner == franchiseOwner))
			{
				MBCHAR franchiseLossString[80];
				sprintf(franchiseLossString, "%i", franchiseLoss);

				RECT textRect = { outerRect.right + (franchiseLoss < 10 ? 6 : 2), outerRect.bottom - 17,
				                  outerRect.right + dimensions.x - 1, outerRect.bottom - 5 };
				if (ColorSet::UseDarkFontColor(color))
				{
					RECT darkenRect = {textRect.left - (franchiseLoss < 10 ? 3 : 1), textRect.top + 2,
					                   textRect.right - (franchiseLoss < 10 ? 4 : 2),textRect.bottom + 1 };
					primitives_ClippedShadowRect16(surf, darkenRect);
				}
				RECT clipRect = primitives_GetScreenAdjustedRectCopy(&surf, textRect);
				m_font->DrawString(&surf, &textRect, &clipRect, franchiseLossString, 0, GetColorRef(COLOR_WHITE));
			}
			outerRect.top   = outerRect.bottom - dimensions.y;
			outerRect.right += dimensions.x - 1;
		}
	}

	if (isInjoined)
	{
		static const MAPICON injunction = MAPICON_INJUNCTION;
		Pixel16 * icon = tileSet->GetMapIconData(injunction);
		Assert(icon);
		if (icon)
		{
			POINT dimensions = tileSet->GetMapIconDimensions(injunction);

			Pixel16 color = GetPlayerColor(injoinedOwner);
			DrawClippedColorizedOverlay(icon, surf, outerRect.right-1, outerRect.bottom - dimensions.y, color, alpha);
			primitives_ClippedFrame16(surf, outerRect.right - 1, outerRect.bottom - dimensions.y, dimensions.x,
					dimensions.y, color, alpha);

			outerRect.top   = outerRect.bottom - dimensions.y;
			outerRect.right += dimensions.x - 1;
		}
	}

	if (wasHappinessAttacked)
	{
		static const MAPICON unhappy = MAPICON_UNHAPPY;
		Pixel16 * icon = tileSet->GetMapIconData(unhappy);
		Assert(icon);
		if (icon)
		{
			POINT dimensions = tileSet->GetMapIconDimensions(unhappy);

			Pixel16 color = GetPlayerColor(happinessAttackOwner);
			DrawClippedColorizedOverlay(icon, surf, outerRect.right-1, outerRect.bottom - dimensions.y, color, alpha);
			primitives_ClippedFrame16(surf, outerRect.right - 1, outerRect.bottom - dimensions.y, dimensions.x,
					dimensions.y, color, alpha);

			outerRect.top   = outerRect.bottom - dimensions.y;
			outerRect.right += dimensions.x - 1;
		}
	}

	if (isBioInfected)
	{
		static const MAPICON bioDisease = MAPICON_BIODISEASE;
		Pixel16 * icon = tileSet->GetMapIconData(bioDisease);
		Assert(icon);
		if (icon)
		{
			POINT dimensions = tileSet->GetMapIconDimensions(bioDisease);

			Pixel16 color = (++g_bio_flash > 10) ? GetColor(COLOR_YELLOW) : GetPlayerColor(bioInfectedOwner);
			if (g_bio_flash > 10) {
				g_bio_flash = 0;
			}

			DrawClippedColorizedOverlay(icon, surf, outerRect.right-1, outerRect.bottom - dimensions.y, color, alpha);
			primitives_ClippedFrame16(surf, outerRect.right - 1, outerRect.bottom - dimensions.y, dimensions.x,
					dimensions.y, color, alpha);

			outerRect.top   = outerRect.bottom - dimensions.y;
			outerRect.right += dimensions.x - 1;
		}
	}

	if (isNanoInfected)
	{
		static const MAPICON nanoDisease = MAPICON_NANODISEASE;
		Pixel16 * icon = tileSet->GetMapIconData(nanoDisease);
		Assert(icon);
		if (icon)
		{
			POINT dimensions = tileSet->GetMapIconDimensions(nanoDisease);

			Pixel16 color = (++g_nano_flash > 10) ? GetColor(COLOR_YELLOW) : GetPlayerColor(bioInfectedOwner);
			if (g_nano_flash > 10) {
				g_nano_flash = 0;
			}

			DrawClippedColorizedOverlay(icon, surf, outerRect.right-1, outerRect.bottom - dimensions.y, color, alpha);
			primitives_ClippedFrame16(surf, outerRect.right - 1, outerRect.bottom - dimensions.y, dimensions.x,
					dimensions.y, color, alpha);

			outerRect.top   = outerRect.bottom - dimensions.y;
			outerRect.right += dimensions.x - 1;
		}
	}
	return outerRect;
}

RECT TiledMap::DrawCityEnslaved(aui_Surface & surf, const RECT & outerCityRect, sint32 owner, uint32 slaveBits,
		uint8 alpha)
{
	static const MAPICON slave = MAPICON_SLAVE;
	Pixel16 * icon = GetTileSet()->GetMapIconData(slave);
	Assert(icon);
	if (icon)
	{
		Pixel16 playerColor = GetPlayerColor(owner);
		uint32 bits = slaveBits;
		POINT dimensions = GetTileSet()->GetMapIconDimensions(slave);
		RECT outerRect = { outerCityRect.right-1, outerCityRect.top+1, outerCityRect.right-1, outerCityRect.top+1};
		for (sint32 i = 0; i < k_MAX_PLAYERS; i++)
		{
			if (bits & 1)
			{
				outerRect.left -= dimensions.x;
				outerRect.top = outerRect.bottom - dimensions.y;
				DrawClippedColorizedOverlay(icon, surf, outerRect.left, outerRect.top, GetPlayerColor(i), alpha);
				primitives_ClippedFrame16(surf, outerRect.left, outerRect.top, dimensions.x, dimensions.y,
						GetPlayerColor(i), alpha);
			}

			bits >>= 1;
		}
		return outerRect;
	}
	return outerCityRect;
}

sint32 TiledMap::DrawColorBlendedOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, Pixel16 color, sint32 blendValue)
{
	if (!data || (x < 0) || (y < 0)) return 0;

	uint8 * surfBase;
	sint32	surfWidth;
	sint32	surfHeight;
	sint32	surfPitch;

	SurfaceLock lock    = SurfaceLock(surface);

	if (!surface)
	{
		surfBase    = m_surfBase;
		surfWidth   = m_surfWidth;
		surfHeight  = m_surfHeight;
		surfPitch   = m_surfPitch;
	}
	else if (lock.IsValid())
	{
		surfBase    = lock.Base();
		surfWidth   = surface->Width();
		surfHeight  = surface->Height();
		surfPitch   = surface->Pitch();
	}
	else
	{
		return AUI_ERRCODE_SURFACELOCKFAILED;
	}

	if (!surfBase) return 0;

	if (x >= (surfWidth - k_TILE_GRID_WIDTH) ||
		y >= (surfHeight - k_TILE_GRID_HEIGHT)
       )
    {
		return 0;
	}

	uint32    blendRgbMask  = pixelutils_GetBlend16RGBMask();
	uint32    shadowRgbMask = pixelutils_GetShadow16RGBMask();
	uint16    start         = (uint16)*data++;
	uint16    end           = (uint16)*data++;
	Pixel16 * table         = data;
	Pixel16 * dataStart     = table + (end - start + 1);

	sint32 len;

	for (sint32 j = start; j <= end; j++)
	{
		if ((y+j) >= surfHeight) continue;
		if ((sint16)table[j-start] == -1) continue;

		unsigned short * destPixel = (unsigned short *)
		    (surfBase + ((y + j) * surfPitch) + (x * 2));

		Pixel16 *   rowData = dataStart + table[j-start];
		Pixel16		tag;
		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
						destPixel += (tag & 0x00FF);
					break;
				case k_TILE_COPY_RUN_ID			: {
						len = (tag & 0x00FF);

						while (len--) {
							*destPixel = pixelutils_Blend16(*rowData++, color, blendValue << 3, blendRgbMask);
							destPixel++;
						}
					}
					break;
				case k_TILE_SHADOW_RUN_ID		: {
						len = (tag & 0x00FF);
						while (len--) {
							*destPixel = pixelutils_Shadow16(*destPixel, shadowRgbMask);
							destPixel++;
						}
					}
					break;
			}

		} while ((tag & 0xF000) == 0);
	}

	return 0;
}


void TiledMap::DrawColorBlendedOverlayScaled(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y,
								 sint32 destWidth, sint32 destHeight, Pixel16 color, sint32 blendValue)
{
    if (!data || (x < 0) || (y < 0)) return;

	uint8 * surfBase;
	sint32	surfWidth;
	sint32	surfHeight;
	sint32	surfPitch;

	SurfaceLock lock    = SurfaceLock(surface);

	if (!surface)
	{
		surfBase    = m_surfBase;
		surfWidth   = m_surfWidth;
		surfHeight  = m_surfHeight;
		surfPitch   = m_surfPitch;
	}
	else if (lock.IsValid())
	{
		surfBase    = lock.Base();
		surfWidth   = surface->Width();
		surfHeight  = surface->Height();
		surfPitch   = surface->Pitch();
	}
	else
	{
		return;
	}

	if (!surface || (x >= surface->Width() - destWidth) || (y >= surface->Height() - destHeight)) {
		return;
	}

	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	surfBase += (y * surfPitch + x * 2);

	uint32    blendRgbMask = pixelutils_GetBlend16RGBMask();
	uint16    vstart       = (uint16)*data++;
	uint16    end          = (uint16)*data++;
	Pixel16 * table        = data;
	Pixel16 * dataStart    = table + (end - vstart + 1);
	sint32    vaccum       = destHeight*2 - k_TILE_GRID_HEIGHT;
	sint32    vincx        = destHeight*2;
	sint32    vincxy       = (destHeight-k_TILE_GRID_HEIGHT) * 2 ;
	sint32    vpos2        = (sint32)((double)((end+1) - destHeight) / (double)destHeight);
	sint32    vdestpos     = y;
	sint32    vend         = (end+1) - 1;

	for (sint32 vpos1 = 0; vpos1 < vend; ++vpos1)
    {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			if ((sint16)table[vpos1-vstart] == -1 || (sint16)table[vpos2-vstart] == -1) {
				vpos2++;
				continue;
			}

			Pixel16	*   rowData1 =
                (vpos1 < vstart) ? emptyRow : dataStart + table[vpos1-vstart];
			Pixel16 *   rowData2 =
                (vpos2 < vstart) ? emptyRow : dataStart + table[vpos2-vstart];

			sint32  haccum  = destWidth*2 - k_TILE_GRID_WIDTH;
			sint32  hincx   = destWidth*2;
			sint32  hincxy  = (destWidth-k_TILE_GRID_WIDTH) * 2;
			sint32  hdestpos = x;
			sint32  hend    = k_TILE_GRID_WIDTH-1;

			Pixel16		pixel1, pixel2, pixel3, pixel4;
			Pixel16		pixel;
			ProcessRun(&rowData1, &rowData2, &pixel1, &pixel2, -1, 0x0000, 0, 0, 0);

			for (sint32 hpos = 0; hpos < hend; ++hpos)
            {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					Pixel16 * destPixel = (Pixel16 *)
                        (surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));

					ProcessRun(&rowData1, &rowData2, &pixel3, &pixel4, hpos, *destPixel, 0, 0, 0);

					if (pixel1 != k_MEDIUM_KEY || pixel2 != k_MEDIUM_KEY || pixel3 != k_MEDIUM_KEY || pixel4 != k_MEDIUM_KEY) {

						pixel = pixel3;


						*destPixel = pixelutils_Blend16(pixel, color, blendValue << 3, blendRgbMask);
					}

					pixel1 = pixel3;
					pixel2 = pixel4;

					hdestpos++;
				}
			}

			vaccum += vincxy;
			vdestpos++;
		}
		vpos2++;
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

	const TerrainImprovementRecord::Effect * effect =
	    (m_overlayRec->GetClassTerraform() || m_overlayRec->GetClassOceanform())
	    ? m_overlayRec->GetTerrainEffect(0)
	    : terrainutil_GetTerrainEffect(m_overlayRec, m_overlayPos);

	if (effect==NULL)
		return;

	sint32 index   = effect->GetTilesetIndex();

	Pixel16 *data    = m_tileSet->GetImprovementData((uint16)index);

	if (data==NULL)
		return;

	sint32 x,y;
	maputils_MapXY2PixelXY(m_overlayPos.x,m_overlayPos.y,&x,&y);

	sint32	destWidth  = k_TILE_PIXEL_WIDTH;
	sint32	destHeight = k_TILE_GRID_HEIGHT;

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

sint32 TiledMap::GetVisibleCellOwner(const MapPoint &pos) const
{
	if(!m_localVision->IsVisible(pos)
	&& g_selected_item->GetVisiblePlayer() != g_theWorld->GetCell(pos)->GetOwner()
	){
		UnseenCellCarton ucell;
		if(m_localVision->GetLastSeen(pos, ucell))
		{
			return ucell.m_unseenCell->GetCellOwner();
		}
	}

	return g_theWorld->GetCell(pos)->GetOwner();
}

uint32 TiledMap::GetVisibleCityOwner(const MapPoint &pos) const
{
	if(!m_localVision->IsVisible(pos)
	// Show the city influence radius from the last visit.
	&& g_selected_item->GetVisiblePlayer() != g_theWorld->GetCell(pos)->GetOwner()
	){
		UnseenCellCarton ucell;
		if(m_localVision->GetLastSeen(pos, ucell))
		{
			return ucell.m_unseenCell->GetVisibleCityOwner();
		}
	}

	return g_theWorld->GetCell(pos)->GetCityOwner().m_id;
}

uint32 TiledMap::GetVisibleTerrainType(const MapPoint &pos) const
{
	if(!m_localVision->IsVisible(pos)
	// Show the city influence radius from the last visit.
	&& g_selected_item->GetVisiblePlayer() != g_theWorld->GetCell(pos)->GetOwner()
	){
		UnseenCellCarton ucell;
		if(m_localVision->GetLastSeen(pos, ucell))
		{
			return ucell.m_unseenCell->GetTerrainType();
		}
	}

	return g_theWorld->GetCell(pos)->GetTerrainType();
}

bool TiledMap::HasVisibleCity(const MapPoint &pos) const
{
	if(!m_localVision->IsVisible(pos)
	// Show the city influence radius from the last visit.
	&& g_selected_item->GetVisiblePlayer() != g_theWorld->GetCell(pos)->GetOwner()
	){
		UnseenCellCarton ucell;
		if(m_localVision->GetLastSeen(pos, ucell))
		{
			return ucell.m_unseenCell->GetActor() != NULL;
		}
	}

	return g_theWorld->GetCell(pos)->GetCity().IsValid();
}

void TiledMap::DrawNationalBorders(aui_Surface & surf, const MapPoint & pos)
{
	sint32 myOwner = GetVisibleCellOwner(pos);
	if (myOwner < 0) {
		return;
	}

	Player *visiblePlayer = g_player[g_selected_item->GetVisiblePlayer()];
	if (visiblePlayer == NULL) {
		return;
	}

	static const WORLD_DIRECTION NEIGHBOR_DIRECTIONS[] = {
			NORTH, NORTHEAST, EAST, NORTHWEST, SOUTHEAST, WEST, SOUTHWEST, SOUTH
	};

	bool fog = m_localVision && m_localVision->IsExplored(pos) && !m_localVision->IsVisible(pos);

	if (g_theProfileDB->GetShowPoliticalBorders())
	{
		uint32 borders = 0;

		for (auto neighborDirection : NEIGHBOR_DIRECTIONS)
		{
			MapPoint neighborPos;
			if (pos.GetNeighborPosition(neighborDirection, neighborPos)) {
				sint32 neighborOwner = GetVisibleCellOwner(neighborPos);
				if ((neighborOwner != myOwner) && (visiblePlayer->HasSeen(myOwner) || g_fog_toggle || g_god)) {
					borders |= (1 << neighborDirection);
				}
			}
		}
		sint32 borderMode = g_theProfileDB->IsSmoothBorders() ? k_BORDER_SMOOTH : k_BORDER_SOLID;
		DrawColoredBorderEdge(surf, pos, GetPlayerColor(myOwner, fog), borders, borderMode, 2);
	}

	uint32 myCityOwner = GetVisibleCityOwner(pos);
	Unit   myCity(myCityOwner);
	bool   isCityVisible = myCity.IsValid() ? (myCity.GetVisibility() & (1 << visiblePlayer->m_owner)) : false;

	if (g_theProfileDB->IsShowCityInfluence() && isCityVisible) {
		uint32 borders = 0;

		for (auto neighborDirection : NEIGHBOR_DIRECTIONS) {
			MapPoint neighborPos;
			if (pos.GetNeighborPosition(neighborDirection, neighborPos)) {
				uint32 neighborCityOwner = GetVisibleCityOwner(neighborPos);
				if (neighborCityOwner != myCityOwner) {
					borders |= (1 << neighborDirection);
				}
			}
		}
		DrawColoredBorderEdge(surf, pos, GetColor(COLOR_WHITE, fog), borders, k_BORDER_DASHED, 0);
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
					sprintf(timebuf, "%s: %" PRId64, g_theStringDB->GetNameStr("NETWORK_TIME_LEFT"), timeleft);
					timeRect.right = timeRect.left + m_font->GetStringWidth(timebuf);
					m_font->DrawString(tempSurf, &timeRect, &timeRect, timebuf, 0, GetColorRef(COLOR_BLACK), 0);
					OffsetRect(&timeRect, -1, -1);
					m_font->DrawString(tempSurf, &timeRect, &timeRect, timebuf, 0, GetColorRef(COLOR_WHITE), 0);
					timeRect.right++;
					timeRect.bottom++;
					AddDirtyRectToMix(timeRect);
				} else {
					sprintf(timebuf, "%s: %s", g_theStringDB->GetNameStr("NETWORK_CURRENT_PLAYER"), g_player[g_selected_item->GetCurPlayer()] && g_player[g_selected_item->GetCurPlayer()]->m_civilisation->m_id != 0 ? g_player[g_selected_item->GetCurPlayer()]->m_civilisation->GetLeaderName() : "---");
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

RECT TiledMap::DrawCityReligionIcons(aui_Surface & surf, const MapPoint & pos, sint32 owner, const RECT & rect,
		bool hasReligionIcon, uint8 alpha)
{
	Unit unit;
	if(!g_theWorld->GetTopVisibleUnit(pos,unit)) {
		return rect;
	}

	TileSet * tileSet     = GetTileSet();
	POINT     dimensions  = tileSet->GetMapIconDimensions(MAPICON_BIODISEASE);
	RECT      outerRect   = { rect.right, rect.bottom, rect.right, rect.bottom };
	Pixel16   playerColor = GetPlayerColor(owner);

	if (hasReligionIcon)
	{
		CityData *cityData = unit.GetData()->GetCityData();
		for (sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++)
		{
			sint32 religionIcon = 0;
			if (buildingutil_Get(i, owner)->GetIsReligionIconIndex(religionIcon))
			{
				if (cityData->GetImprovements() & ((uint64) 1 << i))
				{
					Pixel16 * icon = tileSet->GetMapIconData(religionIcon);
					Assert(icon);
					if (icon)
					{
						DrawClippedColorizedOverlay(icon, surf, outerRect.right - 1, outerRect.bottom - dimensions.y,
						                            playerColor, alpha);
						primitives_ClippedFrame16(surf, outerRect.right - 1, outerRect.bottom - dimensions.y,
								dimensions.x, dimensions.y, playerColor, alpha);
						outerRect.top = outerRect.bottom - dimensions.y;
						outerRect.right += dimensions.x - 1;
					}
				}
			}
		}

		sint32 wonderIcon = 0;
		for (sint32 i = 0; i < g_theWonderDB->NumRecords(); i++)
		{
			if (wonderutil_Get(i, owner)->GetIsReligionIconIndex(wonderIcon))
			{
				if (cityData->GetBuiltWonders() & ((uint64) 1 << i))
				{
					Pixel16 * icon = tileSet->GetMapIconData(wonderIcon);
					Assert(icon);
					if (icon)
					{
						DrawClippedColorizedOverlay(icon, surf, outerRect.right - 1, outerRect.bottom - dimensions.y,
						                            /*playerColor*/ 0xF800, alpha);
						primitives_ClippedFrame16(surf, outerRect.right - 1, outerRect.bottom - dimensions.y,
								dimensions.x, dimensions.y, playerColor, alpha);
						outerRect.top = outerRect.bottom - dimensions.y;
						outerRect.right += dimensions.x - 1;
					}
				}
			}
		}
	}
	return outerRect;
}

RECT TiledMap::DrawCitySpecialIcons(aui_Surface & surf, const MapPoint & pos, sint32 owner, const RECT & rect,
		bool hasSpecialIcon, uint8 alpha)
{
	Unit unit;
	if(!g_theWorld->GetTopVisibleUnit(pos,unit)) {
		return rect;
	}

	TileSet * tileSet     = GetTileSet();
	POINT     dimensions  = tileSet->GetMapIconDimensions(MAPICON_BIODISEASE);
	RECT      outerRect   = { rect.right, rect.bottom, rect.right, rect.bottom };
	Pixel16   playerColor = GetPlayerColor(owner);

	if (hasSpecialIcon)
	{
		CityData *cityData = unit.GetData()->GetCityData();
		for (sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++)
		{
			sint32 religionIcon = 0;
			if (buildingutil_Get(i, owner)->GetShowCityIconTopIndex(religionIcon))
			{
				if (cityData->GetImprovements() & ((uint64) 1 << i))
				{
					Pixel16 * icon = tileSet->GetMapIconData(religionIcon);
					Assert(icon);
					if (icon)
					{
						DrawClippedColorizedOverlay(icon, surf, outerRect.right - 1, outerRect.bottom - dimensions.y,
						                            playerColor, alpha);
						primitives_ClippedFrame16(surf, outerRect.right - 1, outerRect.bottom - dimensions.y,
								dimensions.x, dimensions.y, playerColor, alpha);
						outerRect.top = outerRect.bottom - dimensions.y;
						outerRect.right += dimensions.x - 1;
					}
				}
			}
		}

		sint32 wonderIcon = 0;
		for (sint32 i = 0; i < g_theWonderDB->NumRecords(); i++)
		{
			if (wonderutil_Get(i, owner)->GetShowCityIconTopIndex(wonderIcon))
			{
				if (cityData->GetBuiltWonders() & ((uint64) 1 << i))
				{
					Pixel16 * icon = tileSet->GetMapIconData(wonderIcon);
					Assert(icon);
					if (icon)
					{
						DrawClippedColorizedOverlay(icon, surf, outerRect.right - 1, outerRect.bottom - dimensions.y,
								playerColor, alpha);
						primitives_ClippedFrame16(surf, outerRect.right - 1, outerRect.bottom - dimensions.y,
								dimensions.x, dimensions.y, playerColor, alpha);
						outerRect.top = outerRect.bottom - dimensions.y;
						outerRect.right += dimensions.x - 1;
					}
				}
			}
		}
	}
	return outerRect;
}
