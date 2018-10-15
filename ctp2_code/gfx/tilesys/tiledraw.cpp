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
//   per tile improvement by Martin G�hmann.
// - Allows now to use costumized graphics for ruins/huts by Martin G�hmann
// - Fixed CtD when drawing ruins or huts on an unseen cell.
// - City names and sizes are now drawn if game is in god mode or the fog
//   fog of war is toggled of. - Oct. 22nd 2004 Martin G�hmann
// - Tile improvements under construction with 100% completeness or more
//   are now drawn as complete tile improvements, this allows to retrieve them
//   from the unseen cell without the need to change the unseen cell data
//   structure. - Dec. 21st 2004 Martin G�hmann
// - Current terrain improvements are displayed instead of those from the
//   last visit if the fog of war is toggled off. - Dec 24th 2004 - Martin G�hmann
// - Road like tile improvements are drawn according to the real state
//   instead to the state from the last visit. - Dec. 25th 2004 - Martin G�hmann
// - Draws the city radius from the last visit instead the current one.
//   - Dec. 26th 2004 Martin G�hmann
// - Positions that you own and are explored are now shown as yours.
//   - Mar. 4th 2005 Martin G�hmann
// - If fog of war is off or god mode is on all borders are now shown. Even
//   those of the civs you have no contact to. - Mar. 4th 2005 Martin G�hmann
// - Added option to draw wonders on top of roads.
// - PFT 29 mar 05, show # turns until city next grows a pop.
// - Removed .NET warnings. - April 23rd 2005 Martin G�hmann
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
// - Display non-growth as "---"
// - Repaired crashes when zooming out
// - Added IsCapitol Bool to DrawCityIcons 1-5-2006 EMOD
// - Implemented HasAirport; for some reason it was forgotten 1-5-2006 EMOD
// - Moved citypop box to the left name in the center and turns until next pop
//   to the right for cleaner interface 1-13-2007 EMOD
// - Allowed to select between smooth and square borders. (Feb 4th 2007 Martin G�hmann)
// - Fixed Religion Icon displays - E 6.25.2007
// - Roads now use the TileSetIndex from the TerrainImprovement database. (28-Feb-2008 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "tiledmap.h"               // tiledraw.h does not exist

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
            m_Surface   (a_Surface)
        {
	        if (a_Surface)
            {
	            AUI_ERRCODE result = a_Surface->Lock(NULL, &m_Base, 0);
                Assert(AUI_ERRCODE_OK == result);
                if (AUI_ERRCODE_OK != result)
                {
                    m_Base = NULL;
                }
            }
        };

        ~SurfaceLock()
        {
            if (m_Surface && m_Base)
            {
#if defined(_DEBUG)
                AUI_ERRCODE result  = m_Surface->Unlock(m_Base);
                Assert(AUI_ERRCODE_OK == result);
#else
	            (void) m_Surface->Unlock(m_Base);
#endif
            }
        };

        /// The acquired surface lock (NULL when invalid)
        uint8 * Base() const    { return static_cast<uint8 *>(m_Base); };
        /// A valid lock has been acquired
        bool    IsValid() const { return NULL != m_Base; };

    private:
        /// The acquired surface lock (NULL when invalid)
        LPVOID          m_Base;
        /// The surface to lock
        aui_Surface *   m_Surface;
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
			DrawPartiallyConstructedImprovement(surface, env, type, x, y, index, fog, percent);//percent added by Martin G�hmann
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
				DrawPartiallyConstructedImprovement(surface, env, type, x, y, index, fog, percent);//percent added by Martin G�hmann
			}
			else if (percent < 100)
			{
				index = 1;
				DrawPartiallyConstructedImprovement(surface, env, type, x, y, index, fog, percent);//percent added by Martin G�hmann
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
					DrawPartiallyConstructedImprovement(surface, env, type, x, y, index, fog, percent);//percent added by Martin G�hmann
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
		// Modified by Martin G�hmann to allow modders to customize the graphics of ruins/huts.
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
												   //Added sint32 percentComplete by Martin G�hmann
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

void TiledMap::DrawHitMask(aui_Surface *surf, const MapPoint &pos)
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

	SurfaceLock lock        = SurfaceLock(surf);
	if (!lock.IsValid()) return;

	uint8 *     pSurfBase   = lock.Base();
	sint32      surfPitch   = surf->Pitch();

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

		Pixel16 * pDestPixel = (Pixel16 *)(pSurfBase + ((y+row) * surfPitch) + ((x+start) << 1));
		*pDestPixel++ = selectColorPixel;
		*pDestPixel = selectColorPixel;

		pDestPixel += (end-start-2);

		*pDestPixel++ = selectColorPixel;
		*pDestPixel = selectColorPixel;

		row++;
	}

#if 0
	// Add a timer and a possibility to disable the blinking
	// Move it into the calling method, so that the blinking in the scenario editor would be correct
	g_curSelectColor = (COLOR)(g_curSelectColor + 1);
	if (g_curSelectColor > COLOR_SELECT_2) g_curSelectColor = COLOR_SELECT_0;
#endif
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
	sint32		x, y;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y, mapViewRect);
	x += destRect->left;
	if (x < 0) return;

	y += destRect->top + (sint32) ((double)k_TILE_PIXEL_HEADROOM * m_scale);
	if (y < 0) return;

	sint32 width = GetZoomTilePixelWidth();
	sint32 height = GetZoomTilePixelHeight();

    if (x >= surf->Width()-width) return;
	if (y >= surf->Height() - height) return;

	AddDirtyToMix(x, y, width, height);

    SurfaceLock lock    = SurfaceLock(surf);
    if (!lock.IsValid()) return;

	uint8 * pSurfBase   = lock.Base();
	sint32  surfPitch   = surf->Pitch();

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

		Pixel16 * pDestPixel = (Pixel16 *)(pSurfBase + ((y+row) * surfPitch) + ((x+start) << 1));
		*pDestPixel = selectColorPixel;
		pDestPixel += (end-start);
		*pDestPixel = selectColorPixel;

		row++;
	}

	g_curSelectColor = (COLOR)(g_curSelectColor + 1);
	if (g_curSelectColor > COLOR_SELECT_3)
		g_curSelectColor = COLOR_SELECT_0;
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

/// Draw a colored border edge at a tile
/// \param      surf                Surface to draw on
/// \param      pos                 Tile to draw
/// \param      selectColorPixel    Color to use
/// \param      side                Side of the tile to draw
/// \param      dashMode            Not used
/// \remarks    This function expects that \a surf has been locked
void TiledMap::DrawColoredBorderEdge(aui_Surface *surf, const MapPoint &pos, Pixel16 selectColorPixel, WORLD_DIRECTION side, sint32 dashMode)
{
	sint32		x, y;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);

	if (x < 0) return;
	y += (sint32) ((double)k_TILE_PIXEL_HEADROOM * m_scale);
	if (y < 0) return;

	sint32 width = GetZoomTilePixelWidth();  // changing here got rid of the border
	sint32 height = GetZoomTilePixelHeight();

	if (!surf) surf = m_surface;

	if (x >= surf->Width()-width) return;
	if (y >= surf->Height() - height) return;

	AddDirtyToMix(x, y, width, height);

	uint8	* surfBase = m_surfBase;
	sint32  surfPitch = m_surfPitch;

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

		Pixel16 * pDestPixel = (Pixel16 *)(surfBase + ((y+row) * surfPitch) + ((x+start) << 1)); //EMOD change here
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

    if (xpos >= surface->Width() - k_TILE_PIXEL_WIDTH) return 0;
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

			*pDestPixel = pixelutils_BlendFast(srcPixel,color,blend);
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

					*pDestPixel = pixelutils_BlendFast(srcPixel,color,blend);

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

	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);

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

	uint16		vstart      = (uint16)*data++;
	uint16		end         = (uint16)*data++;
	Pixel16	*   table       = data;
	Pixel16	*   dataStart   = table + (end - vstart + 1);
	sint32      vaccum      = destHeight*2 - k_TILE_GRID_HEIGHT;
	sint32      vincx       = destHeight*2;
	sint32      vincxy      = (destHeight-k_TILE_GRID_HEIGHT) * 2;
	sint32      vpos2       = (sint32)((double)((end+1) - destHeight) / (double)destHeight);
    sint32      vdestpos    = y;
	sint32      vend        = (end+1) - 1;

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


						*destPixel = pixelutils_BlendFast(pixel,color,blend);

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

	sint32      surfPitch   = g_screenManager->GetSurfPitch();
	uint8 *     surfBase    = g_screenManager->GetSurfBase();
	uint16		start       = (uint16)*data++;
	uint16		end         = (uint16)*data++;
	Pixel16	*   table       = data;
	Pixel16	*   dataStart   = table + (end - start + 1);

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
	if (!data || (x < 0) || (y < 0)) return;

	sint32      surfWidth   = g_screenManager->GetSurfWidth();
	sint32      surfHeight  = g_screenManager->GetSurfHeight();

	if (x >= surfWidth - destWidth) return;
	if (y >= surfHeight - destHeight) return;

	sint32      surfPitch   = g_screenManager->GetSurfPitch();
	uint8 *     surfBase    = g_screenManager->GetSurfBase() + (surfPitch * y + x * 2);
	uint16		vstart      = (uint16)*data++;
	uint16		end         = (uint16)*data++;
	Pixel16	*   table       = data;
	Pixel16	*   dataStart   = table + (end - vstart + 1);
	sint32      vaccum      = destHeight*2 - k_TILE_GRID_HEIGHT;
	sint32      vincx       = destHeight*2;
	sint32      vincxy      = (destHeight-k_TILE_GRID_HEIGHT) * 2;
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


						*destPixel = pixelutils_BlendFast(pixel,color,blend);

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

	sint32      surfPitch   = g_screenManager->GetSurfPitch();
	uint8 *     surfBase    = g_screenManager->GetSurfBase();
	uint16		start       = (uint16)*data++;
	uint16		end         = (uint16)*data++;
	Pixel16	*   table       = data;
	Pixel16	*   dataStart   = table + (end - start + 1);

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
	if (!data || (x < 0) || (y < 0)) return;

    sint32      surfWidth   = g_screenManager->GetSurfWidth();
	sint32      surfHeight  = g_screenManager->GetSurfHeight();

	if (x >= surfWidth - destWidth) return;
	if (y >= surfHeight - destHeight) return;

	sint32      surfPitch   = g_screenManager->GetSurfPitch();
	uint8 *     surfBase    = g_screenManager->GetSurfBase() + (surfPitch * y + x * 2);
	uint16		vstart      = (uint16)*data++;
	uint16		end         = (uint16)*data++;
	Pixel16	*   table       = data;
	Pixel16	*   dataStart   = table + (end - vstart + 1);
	sint32      vaccum      = destHeight*2 - k_TILE_GRID_HEIGHT;
	sint32      vincx       = destHeight*2;
	sint32      vincxy      = (destHeight-k_TILE_GRID_HEIGHT) * 2;
	sint32      vpos2       = (sint32)((double)((end+1) - destHeight) / (double)destHeight);
	sint32      vdestpos    = y;
	sint32      vend        = (end+1) - 1;

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

	uint8 *     surfBase    = m_surfBase;
	sint32      surfPitch   = m_surfPitch;
	uint16		start       = (uint16)*data++;
	uint16		end         = (uint16)*data++;
	Pixel16	*   table       = data;
	Pixel16	*   dataStart   = table + (end - start + 1);

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

	if ((x < 0) || (y < 0))
    {
        /// @todo Check whether the destructor is activated and/or
        ///       return 0 would be more appropriate.
        ///       Maybe better test this at the start.
        ///       Note: still using the surfBase of the unlocked surface
        Assert(false);
		lock = SurfaceLock(NULL);
	}

	uint16		start       = (uint16)*data++;
	uint16		end         = (uint16)*data++;
	Pixel16	*   table       = data;
	Pixel16	*   dataStart   = table + (end - start + 1);

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

	uint16		start       = (uint16)*data++;
	uint16		end         = (uint16)*data++;
	Pixel16	*   table       = data;
	Pixel16	*   dataStart   = table + (end - start + 1);

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

	uint16		vstart      = (uint16)*data++;
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


						*destPixel = pixelutils_BlendFast(color, pixel, 8);
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

	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);

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






sint32 TiledMap::DrawColorizedOverlayIntoMix(Pixel16 *data, sint32 x, sint32 y, Pixel16 color)
{
	if (!data || (x < 0) || (y < 0)) return 0;

	uint8 * surfBase    = g_screenManager->GetSurfBase();
	if (!surfBase) return 0;

	sint32 surfHeight = g_screenManager->GetSurfHeight();
	sint32 surfPitch = g_screenManager->GetSurfPitch();

	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);

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

	if ((x >= surface->Width() - destWidth) ||
        (y >= surface->Height() - destHeight)
       )
	{
        /// @todo Check whether the destructor is activated and/or
        ///       return would be more appropriate.
        ///       Note: still using the surfBase of the unlocked surface
        Assert(false);
        lock = SurfaceLock(NULL);
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
	sint32 yoffset = (sint32)(k_TILE_PIXEL_HEADROOM*m_scale)/2;
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
						HasReligionIcon      = FALSE,
						HasSpecialIcon       = FALSE,
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
					isInjoined           = ucell.m_unseenCell->IsInjoined();
					wasHappinessAttacked = ucell.m_unseenCell->WasHappinessAttacked();
					isRioting            = ucell.m_unseenCell->IsRioting();
					hasAirport           = ucell.m_unseenCell->HasAirport();
					if (owner == g_selected_item->GetVisiblePlayer())
						hasSleepingUnits = ucell.m_unseenCell->HasSleepingUnits();
					else
						hasSleepingUnits = FALSE;

					isWatchful           = ucell.m_unseenCell->IsWatchful();

					bioInfectedOwner     = (sint32)ucell.m_unseenCell->m_bioInfectedOwner;
					nanoInfectedOwner    = ucell.m_unseenCell->m_nanoInfectedOwner;
					convertedOwner       = ucell.m_unseenCell->m_convertedOwner;
					franchiseOwner       = ucell.m_unseenCell->m_franchiseOwner;
					injoinedOwner        = ucell.m_unseenCell->m_injoinedOwner;
					happinessAttackOwner = ucell.m_unseenCell->m_happinessAttackOwner;

					slaveBits            = ucell.m_unseenCell->GetSlaveBits();
					isCapitol            = ucell.m_unseenCell->IsCapitol(); //emod
					HasReligionIcon      = ucell.m_unseenCell->IsReligionIcon(); //emod
					HasSpecialIcon       = ucell.m_unseenCell->IsSpecialIcon(); //emod
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
								pop              = unit.GetActor()->GetSize();
								nextpop          = unit.GetActor()->GetNextPop();
								owner            = unit.GetActor()->GetPlayerNum();
							}

							if (showCityProd)
							{
								BuildQueue *bq = cityData->GetBuildQueue();

								if(bq->GetHead())
								{
									BuildNode *bn = bq->GetHead();

									buildItemTime = cityData->HowMuchLonger();

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
											buildItemTime = 1;
											break;
										case k_GAME_OBJ_TYPE_CAPITALIZATION:
											buildItemName = g_theStringDB->GetNameStr("CAPITALIZATION");
											buildItemTime = 1;
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
								HasReligionIcon  = cityData->HasReligionIcon();
								HasSpecialIcon   = cityData->HasSpecialIcon();

								if (!cityData->GetBuildQueue()->GetHead()) {
									drawQueueEmpty   = true;
								}
							}
						}
					}
				}

				if (drawCity) {//it's a city now visible to the current player

					// Move city names up if city production is on,
					// so it doesn't cover top of city sprite.
					if (drawOurCity && showCityProd) {
						yoffset = (sint32)((k_TILE_PIXEL_HEADROOM*m_scale)/2)+19;
					}

					sint32		x,y;
					maputils_MapXY2PixelXY(mapX,mapY,&x,&y);//change map co-ordinates to pixel co-ordinates

					y-= yoffset;

					sint32 width, height;
					RECT rect;//the city name rectangle
					RECT boxRect;//boxRect-rect will = the player colored border for the city name
					RECT clipRect;//working surface

					sint32 right = 0;

					if (x >= 0 && y >= 0 && x < surfWidth && y < surfHeight) {//it's on the screen
						if (m_font) {
							//m_font->SetPointSize(32);  //emod - test to see if font sizes can change
							//width = m_font->GetStringWidth(name); //original
							//height = m_font->GetMaxHeight(); //original
							sint32 widthname = m_font->GetStringWidth(name);
							sint32 heightname = m_font->GetMaxHeight();

							Pixel16 pixelColor = GetPlayerColor(owner, fog);

							//define rect's screen co-ordinates
							rect.left = x;
							rect.top = y;
							rect.right = rect.left + widthname;
							rect.bottom = rect.top + heightname;

							// This is used to position the show city production stuff.
							sint32 brectx = x;

							boxRect = rect;//copy rect to boxRect

							InflateRect(&boxRect, 2, 1);//expand boxRect to allow for borders

							clipRect = primitives_GetScreenAdjustedRectCopy(surf, boxRect);
							//color clipRect BLACK
							primitives_PaintRect16(surf, &clipRect, GetColor(COLOR_BLACK));

							InflateRect(&boxRect, 1, 1);//get ready to do borders (clipRect - boxRect now= a one pixel border)
							                            // this is only for the cityname box EMOD note

							clipRect = primitives_GetScreenAdjustedRectCopy(surf, boxRect);
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

							clipRect = primitives_GetScreenAdjustedRectCopy(surf, rect);
							//draw the city name
							m_font->DrawString(surf, &rect, &clipRect, name, 0, nameColor, 0);

							AddDirtyRectToMix(boxRect);

							///////////////////////////////////////
							// Start on the city's pop rectangle //
							///////////////////////////////////////

							// Put the city's pop in str
							MBCHAR str[80];
							sprintf(str,"%i",pop);
							// The top line of the pop rectangle
							//y = boxRect.bottom + 1; //original

							width = m_font->GetStringWidth(str);
							if (width < k_POP_BOX_SIZE_MINIMUM)
								width = k_POP_BOX_SIZE_MINIMUM;
							height = m_font->GetMaxHeight();

							// The pop rectangle
							RECT popRect = {0, 0, width+4, height+4};
							// Add the top left co-ordinates
							//OffsetRect(&popRect, boxRect.left, y);  //original
							OffsetRect(&popRect, boxRect.left - width - 4, boxRect.top);  //orig OffsetRect(&popRect, boxRect.left, y)

							clipRect = primitives_GetScreenAdjustedRectCopy(surf, popRect);

							// Prevent Asserts in primitives_PaintRect16 when moving the mouse
							if (clipRect.right < clipRect.left) clipRect.right = clipRect.left;

							// Paint clipRect's surface the proper player color and give it a black frame
							primitives_PaintRect16(surf, &clipRect, pixelColor);
							primitives_FrameRect16(surf, &clipRect, GetColor(COLOR_BLACK));

							// Width and height of the pop number
							width = m_font->GetStringWidth(str);
							height = m_font->GetMaxHeight();
							// This rect will be the inner rect that shows the pop number
							RECT		rect = {0, 0, width, height}; // This shadows the outer rect

							OffsetRect(&rect, popRect.left + (popRect.right-popRect.left)/2 -
											width/2,
											popRect.top + (popRect.bottom-popRect.top)/2 -
											height/2);

							clipRect = primitives_GetScreenAdjustedRectCopy(surf, rect);
							// Draw the pop number in black
							m_font->DrawString(surf, &rect, &clipRect, str,
								0,
								GetColorRef(COLOR_BLACK),
								0);

							// Move two pixels right and do it again ?
							OffsetRect(&rect, 2, 0);
							clipRect = primitives_GetScreenAdjustedRectCopy(surf, rect);
							m_font->DrawString(surf, &rect, &clipRect, str,
							                   0,
							                   GetColorRef(COLOR_BLACK),
							                   0);

							// Move one pixel left and do it again ?
							OffsetRect(&rect, -1, 0);

							clipRect = primitives_GetScreenAdjustedRectCopy(surf, rect);
							m_font->DrawString(surf, &rect, &clipRect, str,
								0,
								GetColorRef(COLOR_WHITE, fog),
								0);

							popRect.bottom++;
							popRect.right++;

							AddDirtyRectToMix(rect);

							//top left co-ordinates of nextPop rect
							//x = popRect.left;
							//y = popRect.bottom + 1;
							//emod top left co-ordinates of nextPop rect
							x = boxRect.right; // + widthname + 8; // popRect.left;
							//original  y = popRect.bottom + 1;

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
								//OffsetRect(&popRectn, x, y); //original
								OffsetRect(&popRectn, x, boxRect.top); //OffsetRect(&popRectn, x, y);

								clipRect = primitives_GetScreenAdjustedRectCopy(surf, popRectn);
								//paint clipRect's surface the proper player color and give it a black frame
								//emod to differentiate nextpop to match city name
								primitives_PaintRect16(surf, &clipRect, GetColor(COLOR_BLACK)); //pixelColor);
								primitives_FrameRect16(surf, &clipRect, pixelColor); //GetColor(COLOR_BLACK));

								//width and height of the nextpop number
								width = m_font->GetStringWidth(strn);
								height = m_font->GetMaxHeight();

								//this rect will be the inner rect that shows the nextpop number
								RECT		rect = {0, 0, width, height}; // This shadows the outer rect and the outer outer rect

								OffsetRect(&rect, popRectn.left + (popRectn.right-popRectn.left)/2 -
												width/2,
												popRectn.top + (popRectn.bottom-popRectn.top)/2 -
												height/2);

								clipRect = primitives_GetScreenAdjustedRectCopy(surf, rect);
								//draw the nextpop number in black
								m_font->DrawString(surf, &rect, &clipRect, strn,
									0,
									GetColorRef(COLOR_BLACK),
									0);

								OffsetRect(&rect, 2, 0);
								clipRect = primitives_GetScreenAdjustedRectCopy(surf, rect);
								m_font->DrawString(surf, &rect, &clipRect, strn,
									0,
									GetColorRef(COLOR_BLACK),
									0);

								OffsetRect(&rect, -1, 0);

								clipRect = primitives_GetScreenAdjustedRectCopy(surf, rect);
								m_font->DrawString(surf, &rect, &clipRect, strn,
									0,
									GetColorRef(COLOR_WHITE, fog),
									0);

								popRectn.bottom++;
								popRectn.right++;

								AddDirtyRectToMix(rect);

								right = popRectn.right;

								if (showCityProd && buildItemName != NULL)
								{
									/////////////////////////////////////
									// NOW DRAW THE BUILDING ITEM NAME //
									/////////////////////////////////////

									sint32 widthbuildItemName = m_font->GetStringWidth(buildItemName);
									sint32 heightbuildItemName = m_font->GetMaxHeight();

									// The building name inner rectangle
									RECT brect;

									//define brect's screen co-ordinates
									brect.left = brectx;// same as city name's left
									brect.top = (y+19); // below city name
									brect.right = brect.left + widthbuildItemName;
									brect.bottom = brect.top + heightbuildItemName;

									boxRect = brect;//copy brect to boxRect

									InflateRect(&boxRect, 2, 1);//expand boxRect to allow for borders

									clipRect = primitives_GetScreenAdjustedRectCopy(surf, boxRect);
									primitives_PaintRect16(surf, &clipRect, GetColor(COLOR_BLACK));
									InflateRect(&boxRect, 1, 1);//get ready to do borders (clipRect - boxRect now= a one pixel border)

									clipRect = primitives_GetScreenAdjustedRectCopy(surf, boxRect);
									//color clipRect to be the player's color (this creates the border)
									primitives_FrameRect16(surf, &clipRect, pixelColor);

									//get the color for the city building's name text
									COLORREF buildingnameColor;
									if (fog) {// Should never be under fog?
										buildingnameColor = GetColorRef(COLOR_WHITE, fog);
									} else {
										if (isRioting) {
											buildingnameColor = GetColorRef(COLOR_RED);
										} else if(drawQueueEmpty) {
											buildingnameColor = GetColorRef(COLOR_YELLOW);
										} else {
											buildingnameColor = GetColorRef(COLOR_WHITE);
										}
									}

									clipRect = primitives_GetScreenAdjustedRectCopy(surf, brect);
									//draw what the city is building's name
									m_font->DrawString(surf, &brect, &clipRect, buildItemName, 0, buildingnameColor, 0);

									AddDirtyRectToMix(boxRect);

									////////////////////////////////////
									// NOW DRAW THE CONSTRUCTION TIME //
									////////////////////////////////////

									// Put the construction time in str
									MBCHAR strbt[80];
									if (buildItemTime < THRESHOLD_PROD_TIME
										&& buildItemTime != 0)
									{
										sprintf(strbt, "%i", buildItemTime);
									}
									else
									{
										sprintf(strbt, "%s", TEXT_NONE);
									}

									//width and height of the pop number
									width = m_font->GetStringWidth(strbt);
									if (width < k_POP_BOX_SIZE_MINIMUM)
										width = k_POP_BOX_SIZE_MINIMUM;
									height = m_font->GetMaxHeight();

									//RECT for construction time
									RECT btRect = {0, 0, width+4, height+4};
									//add the top left co-ordinates
									OffsetRect(&btRect, boxRect.right, boxRect.top);

									clipRect = primitives_GetScreenAdjustedRectCopy(surf, btRect);
									//paint clipRect's surface black and give it a player colour frame
									primitives_PaintRect16(surf, &clipRect, GetColor(COLOR_BLACK));
									primitives_FrameRect16(surf, &clipRect, pixelColor);

									//width and height of the construction time number
									width = m_font->GetStringWidth(strbt);
									height = m_font->GetMaxHeight();

									//this rect will be the inner rect that shows the number
									RECT		btInnerRect = {0, 0, width, height}; // This shadows the outer rect and the outer outer rect

									OffsetRect(&btInnerRect, btRect.left + (btRect.right-btRect.left)/2 -
													width/2,
													btRect.top + (btRect.bottom-btRect.top)/2 -
													height/2);

									clipRect = primitives_GetScreenAdjustedRectCopy(surf, btInnerRect);
									m_font->DrawString(surf, &btInnerRect, &clipRect, strbt,
										0,
										GetColorRef(COLOR_BLACK),
										0);

									OffsetRect(&btInnerRect, 2, 0);
									clipRect = primitives_GetScreenAdjustedRectCopy(surf, btInnerRect);
									m_font->DrawString(surf, &btInnerRect, &clipRect, strbt,
										0,
										GetColorRef(COLOR_BLACK),
										0);

									OffsetRect(&btInnerRect, -1, 0);

									clipRect = primitives_GetScreenAdjustedRectCopy(surf, btInnerRect);
									m_font->DrawString(surf, &btInnerRect, &clipRect, strbt,
										0,
										GetColorRef(COLOR_WHITE, fog),
										0);

									btRect.bottom++;
									btRect.right++;

									AddDirtyRectToMix(btInnerRect);

									isProdIcon = true;
								}
							}
						}
						else
							continue;
					}

					if(right > 0) // Actually this belongs under the ifs above, but since the stuff is shadowed it has to be this way
						rect.right = right;

					DrawCityIcons(surf, pos, owner, fog, rect,
								isBioInfected, isNanoInfected, isConverted,
								isFranchised, isInjoined, wasHappinessAttacked,
								bioInfectedOwner, nanoInfectedOwner, convertedOwner,
								franchiseOwner, injoinedOwner, happinessAttackOwner,
								slaveBits, isRioting, hasAirport, hasSleepingUnits,
								isWatchful, isCapitol, isProdIcon, pop, isPollutionRisk);

					//if (CityIcons) {

					/*This may be causing problems in DrawColorizedOverlay,
					but I left it enabled since we still have the religion mod.*/
					DrawCityReligionIcons(surf, pos, owner, fog, boxRect, HasReligionIcon);

					/*This will definitely cause problems now the city name is higher up,
					this stuff will try to draw above the city name and cause problems if
					show city production is enabled. As far as I can see this is not used
					at all at the moment though.
					DrawCitySpecialIcons(surf, pos, owner, fog, boxRect, HasSpecialIcon);*/

					//}

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
//              sint32            bioInfectedOwner     : the player who bio infected the city
//              sint32            nanoInfectedOwner    : the player who nano infected the city
//              sint32            convertedOwner       : the player who converted the city
//              sint32            franchiseOwner       : the player who franchised the city
//              sint32            injoinedOwner        : the player who filed an injunction against the city
//              sint32            happinessAttackOwner : the player who happiness attacked the city
//              uint32            slaveBits            : which other player's citizens are enslaved here
//              BOOL              isRioting            : if TRUE then draw MAPICON_UPRISING
//              BOOL              hasAirport           : not used
//              BOOL              hasSleepingUnits     : if TRUE then draw MAPICON_SLEEPINGUNITS
//              BOOL              isWatchful           : if TRUE then draw MAPICON_WATCHFUL
//				BOOL			  isCapitol			   : if TRUE then draw capitol icon
//				BOOL			  isProdIcon		   : if TRUE draw the production icon
//				sint32			  citySize			   : the city's pop number
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
void TiledMap::DrawCityIcons(aui_Surface *surf, MapPoint const & pos, sint32 owner, bool fog, RECT &popRect,
								BOOL isBioInfected, BOOL isNanoInfected, BOOL isConverted,
								BOOL isFranchised, BOOL isInjoined, BOOL wasHappinessAttacked,
								sint32 bioInfectedOwner, sint32 nanoInfectedOwner, sint32 convertedOwner,
								sint32 franchiseOwner, sint32 injoinedOwner, sint32 happinessAttackOwner,
								uint32 slaveBits, BOOL isRioting, BOOL hasAirport, BOOL hasSleepingUnits,
								BOOL isWatchful, BOOL isCapitol, BOOL isProdIcon, sint32 citySize,
								BOOL isPollutionRisk)
{
	TileSet	*   tileSet     = GetTileSet();
	POINT       iconDim     = tileSet->GetMapIconDimensions(MAPICON_CAPITOL);
	RECT		iconRect;
	iconRect.left   = popRect.right + 3;
	iconRect.right  = iconRect.left + iconDim.x;
	iconRect.top    = popRect.top - 2;
	iconRect.bottom = iconRect.top + iconDim.y;

	if (iconRect.left < 0 || iconRect.top < 0 ||
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	Pixel16     color       = GetPlayerColor(owner, fog);
	Pixel16 *   cityIcon;

	if (isCapitol) {
		cityIcon = tileSet->GetMapIconData(MAPICON_CAPITOL);
		Assert(cityIcon);
		if (!cityIcon) return;

		DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		AddDirtyRectToMix(iconRect);

		iconRect.left += iconDim.x;
		iconRect.right += iconDim.x;
	}

	if (iconRect.left < 0 || iconRect.top < 0 ||
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	if (hasSleepingUnits) {
		cityIcon = tileSet->GetMapIconData(MAPICON_SLEEPINGUNITS);
		Assert(cityIcon);
		if (!cityIcon) return;
		iconDim = tileSet->GetMapIconDimensions(MAPICON_SLEEPINGUNITS);

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

	if (isWatchful) {
		cityIcon = tileSet->GetMapIconData(MAPICON_WATCHFUL);
		Assert(cityIcon);
		if (!cityIcon) return;
		iconDim = tileSet->GetMapIconDimensions(MAPICON_WATCHFUL);

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

	uint32 bits = slaveBits;
	for (sint32 i=0; i<k_MAX_PLAYERS; i++) {
		if (bits & 1) {
			cityIcon = tileSet->GetMapIconData(MAPICON_SLAVE);
			iconDim = tileSet->GetMapIconDimensions(MAPICON_SLAVE);
			Assert(cityIcon);
			if (!cityIcon) return;

			color = GetPlayerColor(i, fog);
			DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);

			AddDirtyRectToMix(iconRect);

			iconRect.left += iconDim.x;
			iconRect.right += iconDim.x;
		}

		bits >>= 1;
	}

	if (iconRect.left < 0 || iconRect.top < 0 ||
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	/* All city icons following this are "tall"
	so they have to be moved upwards to avoid overlapping
	the show city production stuff.*/
	iconRect.top    -= 10;

	if (iconRect.left < 0 || iconRect.top < 0 ||
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

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

	if (iconRect.left < 0 || iconRect.top < 0 ||
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	if (isPollutionRisk) {
		cityIcon = tileSet->GetMapIconData(MAPICON_POLLUTION);
		Assert(cityIcon);
		if (!cityIcon) return;
		iconDim = tileSet->GetMapIconDimensions(MAPICON_POLLUTION);

        color = GetPlayerColor(owner, fog);
		DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		AddDirtyRectToMix(iconRect);

		iconRect.left += iconDim.x;
		iconRect.right += iconDim.x;
	}

	if (iconRect.left < 0 || iconRect.top < 0 ||
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	if (isBioInfected) {
		cityIcon = tileSet->GetMapIconData(MAPICON_BIODISEASE);
		Assert(cityIcon);
		if (!cityIcon) return;
		iconDim = tileSet->GetMapIconDimensions(MAPICON_BIODISEASE);

		Pixel16 const flashColor = GetColor(COLOR_YELLOW, fog);
		color = GetPlayerColor(bioInfectedOwner, fog);

		if (++g_bio_flash > 10) {
			DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, flashColor);
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
		iconDim = tileSet->GetMapIconDimensions(MAPICON_NANODISEASE);

		Pixel16 const flashColor = GetColor(COLOR_YELLOW, fog);
		color = GetPlayerColor(nanoInfectedOwner, fog);

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
		Assert(cityIcon);
		if (!cityIcon) return;
		iconDim = tileSet->GetMapIconDimensions(MAPICON_CONVERSION);

		color = GetPlayerColor(convertedOwner, fog);
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
		iconDim = tileSet->GetMapIconDimensions(MAPICON_FRANCHISE);

		color = GetPlayerColor(franchiseOwner, fog);
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
		iconDim = tileSet->GetMapIconDimensions(MAPICON_INJUNCTION);

		color = GetPlayerColor(injoinedOwner, fog);
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
		iconDim = tileSet->GetMapIconDimensions(MAPICON_UNHAPPY);

		color = GetPlayerColor(happinessAttackOwner, fog);
		DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		AddDirtyRectToMix(iconRect);

		iconRect.left += iconDim.x;
		iconRect.right += iconDim.x;
	}

	/*Airport icon is disabled here for now, since it serves
	no purpose to see that your city has one.

	if (iconRect.left < 0 || iconRect.top < 0 ||
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	if (hasAirport) {
		cityIcon = tileSet->GetMapIconData(MAPICON_AIRPORT);
		Assert(cityIcon);
		if (!cityIcon) return;
		iconDim = tileSet->GetMapIconDimensions(MAPICON_AIRPORT);

		color = GetColor(COLOR_YELLOW, fog);
		DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		AddDirtyRectToMix(iconRect);

		iconRect.left += iconDim.x;
		iconRect.right += iconDim.x;
	}*/

	if (iconRect.left < 0 || iconRect.top < 0 ||
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	if (isProdIcon) {
		POINT		iconDim2;
		if (citySize > 9) {
			iconDim2    = tileSet->GetMapIconDimensions(MAPICON_PRODUCTIONNEWBIG);
			cityIcon	= tileSet->GetMapIconData(MAPICON_PRODUCTIONNEWBIG);
		}
		else
		{
			iconDim2    = tileSet->GetMapIconDimensions(MAPICON_PRODUCTIONNEW);
			cityIcon	= tileSet->GetMapIconData(MAPICON_PRODUCTIONNEW);
		}
		iconRect.left   = popRect.left - iconDim2.x - 3;
		iconRect.right  = iconRect.left + iconDim2.x;
		iconRect.top    = popRect.bottom + 2;
		iconRect.bottom = iconRect.top + iconDim2.y;

		Assert(cityIcon);
		if (!cityIcon) return;

		color = GetPlayerColor(owner, fog);
		DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
		AddDirtyRectToMix(iconRect);
	}
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

	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);

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

	if (!surface ||
        (x >= surface->Width() - destWidth) ||
        (y >= surface->Height() - destHeight)
       )
    {
        /// @todo Check whether the destructor is activated and/or
        ///       return would be more appropriate.
        ///       Note: still using the surfBase of the unlocked surface
        Assert(false);
        lock = SurfaceLock(NULL);
	}

	Pixel16		emptyRow[2];
	emptyRow[0] = (k_TILE_SKIP_RUN_ID << 8) | k_TILE_GRID_WIDTH;

	surfBase += (y * surfPitch + x * 2);

	uint16			vstart = (uint16)*data++;
	uint16			end = (uint16)*data++;
	Pixel16			*table = data;

	Pixel16			*dataStart = table + (end - vstart + 1);

	sint32 vaccum = destHeight*2 - k_TILE_GRID_HEIGHT;
	sint32 vincx = destHeight*2;
	sint32 vincxy = (destHeight-k_TILE_GRID_HEIGHT) * 2 ;
	sint32 vpos2 = (sint32)((double)((end+1) - destHeight) / (double)destHeight);
	sint32 vdestpos = y;
	sint32 vend = (end+1) - 1;

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


						*destPixel = pixelutils_BlendFast(color, pixel, blendValue);
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
			return ucell.m_unseenCell->m_cell_owner;
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

void TiledMap::DrawNationalBorders(aui_Surface *surface, MapPoint &pos)
{
	sint32 myOwner = GetVisibleCellOwner(pos);
	if (myOwner < 0)
		return;

	Player *visP = g_player[g_selected_item->GetVisiblePlayer()];
	if (visP == NULL)
		return;

	uint32 myCityOwner = GetVisibleCityOwner(pos);
	Pixel16 color = g_colorSet->GetPlayerColor(myOwner);
	Pixel16 white = GetColor(COLOR_WHITE);

	Unit myCity(myCityOwner);
	UnitData *myCityData = myCity.IsValid() ? myCity.AccessData() : NULL;

	//emod
	sint32		x, y;
	maputils_MapXY2PixelXY(pos.x, pos.y, &x, &y);
	if ((x < 0) || (y < 0))
		return;

	TileSet *   tileSet     = GetTileSet();
	POINT       iconDim     = tileSet->GetMapIconDimensions(MAPICON_POLBORDERNW);
	RECT        iconRect;
	iconRect.left   = x;
	iconRect.right  = iconRect.left + iconDim.x + 1;
	iconRect.top    = y;
	iconRect.bottom = iconRect.top + iconDim.y + 1;

	sint32  maxWidth    = surface ? surface->Width() : m_surface->Width();
	sint32  maxHeight   = surface ? surface->Height() : m_surface->Height();

	if ((iconRect.right >= maxWidth) || (iconRect.bottom >= maxHeight))
		return;
	//add city wall icons?
	//bool HasCityWall = myCity.hasCityWall
	//add Great Wall Icon?
	// bool PlayerHasGreatWall = g_player->GethasGreatWall future function

	//end emod

	Pixel16 *   borderIcon;
	uint32      neighborCityOwner;
	sint32      neighborOwner;
	MapPoint    neighbor;
	if(pos.GetNeighborPosition(NORTHWEST, neighbor)) {
		neighborOwner = GetVisibleCellOwner(neighbor);
		if(neighborOwner != myOwner
		&&(visP->HasSeen(myOwner)
		|| g_fog_toggle // The sense of fog of and god mode is to see something.
		|| g_god)
		&& g_theProfileDB->GetShowPoliticalBorders()
		){
			if(!g_theProfileDB->IsSmoothBorders())
			{
				DrawColoredBorderEdge(surface, pos, color, NORTHWEST, k_BORDER_SOLID); //EMOD- k_BORDER_SOLID defined in tiledmap.h as 0 and dashed as 1 its a bool?
			}
			else
			{
				// could change this to the same DrawColorizedOverlay but use different icons for NW etc.
				// create a new function like drawcityicons but requires a pos and converts a pos to pixels THEN place Icon.
				// emod
				//X = k_TILE_PIXEL_HEADROOM + (k_TILE_GRID_HEIGHT / 2); -> 24 + 72/2 = 60 FROM DRAWCOLOREDBORDEREDGE
				iconRect.top    = y + 18;
				borderIcon = tileSet->GetMapIconData(MAPICON_POLBORDERNW);
				Assert(borderIcon);
				if (!borderIcon) return;

				//DrawDitheredOverlay(NULL, borderIcon,iconRect.left, iconRect.top,color);
				//AddDirtyToMap(x, y, k_TILE_PIXEL_WIDTH, k_TILE_GRID_HEIGHT);
				DrawColorizedOverlay(borderIcon, surface, iconRect.left, iconRect.top, color);
				AddDirtyRectToMix(iconRect);

				//if ((PlayerHasGreatWall) && (IsLand(pos)) {
					//iconRect.top    = y + 20;  //y
					//GWIcon = tileSet->GetMapIconData(MAPICON_GREATWALLNW);
					//Assert(borderIcon);
					//if (!borderIcon) return;
					//DrawColorizedOverlay(GWIcon, surface, iconRect.left, iconRect.top, color);
					//AddDirtyRectToMix(iconRect);
				//}
			}
			//end emod
		}

		neighborCityOwner = GetVisibleCityOwner(neighbor);
		if(neighborCityOwner != myCityOwner) {
			if(myCityData &&
			   myCityData->GetVisibility() & (1 << visP->m_owner) &&
			   g_theProfileDB->IsShowCityInfluence()) {
				DrawColoredBorderEdge(surface, pos, white, NORTHWEST, k_BORDER_DASHED);
				//if ((PlayerHasGreatWall) && (IsLand(pos)) {
					//iconRect.top    = y + 20;  //y
					//CWIcon = tileSet->GetMapIconData(MAPICON_CITYWALLNW);
					//Assert(CWIcon);
					//if (!CWIcon) return;
					//DrawColorizedOverlay(CWIcon, surface, iconRect.left, iconRect.top, color);
					//AddDirtyRectToMix(iconRect);
				//}
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
			if(!g_theProfileDB->IsSmoothBorders())
			{
				DrawColoredBorderEdge(surface, pos, color, SOUTHWEST, k_BORDER_SOLID); //EMOD- k_BORDER_SOLID defined in tiledmap.h as 0 and dashed as 1 its a bool?
			}
			else
			{
				//emod
				iconRect.top    = y + 46;  //y
				borderIcon = tileSet->GetMapIconData(MAPICON_POLBORDERSW);
				Assert(borderIcon);
				if (!borderIcon) return;
				DrawColorizedOverlay(borderIcon, surface, iconRect.left, iconRect.top, color);
				AddDirtyRectToMix(iconRect);
				//end emod
			}
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

			if(!g_theProfileDB->IsSmoothBorders())
			{
				DrawColoredBorderEdge(surface, pos, color, NORTHEAST, k_BORDER_SOLID);  //original
			}
			else
			{
				//emod
				iconRect.left   = x + 44;
				iconRect.top    = y + 22;  //y

				borderIcon = tileSet->GetMapIconData(MAPICON_POLBORDERNE);
				Assert(borderIcon);
				if (!borderIcon) return;
				DrawColorizedOverlay(borderIcon, surface, iconRect.left, iconRect.top, color);
				AddDirtyRectToMix(iconRect);
				//end emod
			}
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

			if(!g_theProfileDB->IsSmoothBorders())
			{
				DrawColoredBorderEdge(surface, pos, color, SOUTHEAST, k_BORDER_SOLID); //original
			}
			else
			{
				//emod
				iconRect.left   = x + 46;
				iconRect.top    = y + 48;  //y

				borderIcon = tileSet->GetMapIconData(MAPICON_POLBORDERSE);
				Assert(borderIcon);
				if (!borderIcon) return;
				DrawColorizedOverlay(borderIcon, surface, iconRect.left, iconRect.top, color);
				AddDirtyRectToMix(iconRect);
				//end emod
			}
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

//new DrawCityReligionIcons to allow for multiple religions bound only to the number of mapicons
// changed from religion to just special icons to cover both types
void TiledMap::DrawCityReligionIcons
	(
		aui_Surface *       surf,
		MapPoint const &    pos,
		sint32              owner,
		bool                fog,
		RECT &              popRect,
		BOOL                HasReligionIcon
	)
{
	Unit unit;
	if(!g_theWorld->GetTopVisibleUnit(pos,unit))
		return;

	TileSet *   tileSet     = GetTileSet();
	POINT       iconDim     = tileSet->GetMapIconDimensions(MAPICON_BIODISEASE);
	RECT        iconRect;

	iconRect.left   = popRect.right + 3;
	iconRect.right  = iconRect.left + iconDim.x + 1;
	iconRect.top    = popRect.top;
	iconRect.bottom = iconRect.top + iconDim.y + 1;

	if (iconRect.left < 0 || iconRect.top < 0 ||
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	Pixel16     color       = GetPlayerColor(owner, fog);

	if(HasReligionIcon)
	{
		iconRect.left   = popRect.left;
		iconRect.right  = iconRect.left + iconDim.x + 1;
		iconRect.top    = popRect.bottom;
		iconRect.bottom = popRect.top + iconDim.y + 1;
		sint32  cityIcon = 0;
		CityData *cityData = unit.GetData()->GetCityData();
		for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++)
		{
			if(buildingutil_Get(i, owner)->GetIsReligionIconIndex(cityIcon))
			{
				if(cityData->GetImprovements() & ((uint64)1 << i))
				{
					//tileSet->GetMapIconData(cityIcon);
					color = GetPlayerColor(owner, fog);  //optional
					DrawColorizedOverlay(tileSet->GetMapIconData(cityIcon), surf, iconRect.left, iconRect.top, color);
					AddDirtyRectToMix(iconRect);
					iconRect.left += iconDim.x;
					iconRect.right += iconDim.x;
				}
			}
		}
		sint32  wonderIcon = 0;
		for(sint32 j = 0; j < g_theWonderDB->NumRecords(); j++)
		{
			if (wonderutil_Get(j, owner)->GetIsReligionIconIndex(wonderIcon))
			{
				if(cityData->GetBuiltWonders() & ((uint64)1 << j))
				{
					//tileSet->GetMapIconData(cityIcon);
					color = GetPlayerColor(owner, fog);  //optional
					DrawColorizedOverlay(tileSet->GetMapIconData(wonderIcon), surf, iconRect.left, iconRect.top, color);
					AddDirtyRectToMix(iconRect);
					iconRect.left += iconDim.x;
					iconRect.right += iconDim.x;
				}
			}
		}
	}

	if (iconRect.left < 0 || iconRect.top < 0 ||
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;
}

void TiledMap::DrawCitySpecialIcons
	(
		aui_Surface *      surf,
		MapPoint const &   pos,
		sint32             owner,
		bool               fog,
		RECT &             popRect,
		BOOL               HasSpecialIcon
	)
{
	Unit unit;
	if(!g_theWorld->GetTopVisibleUnit(pos,unit))
		return;

	//Future Use - use special icon for everything else and just position the icons elsewhere
	TileSet *   tileSet     = GetTileSet();
	POINT       iconDim     = tileSet->GetMapIconDimensions(MAPICON_BIODISEASE);
	RECT        iconRect;

	iconRect.left   = popRect.right + 3;
	iconRect.right  = iconRect.left + iconDim.x + 1;
	iconRect.top    = popRect.top;
	iconRect.bottom = iconRect.top + iconDim.y + 1;

	if (iconRect.left < 0 || iconRect.top < 0 ||
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	Pixel16     color       = GetPlayerColor(owner, fog);

	iconRect.left   = popRect.left;
	iconRect.right  = iconRect.left + iconDim.x + 1;
	iconRect.top    = popRect.bottom;
	iconRect.bottom = popRect.top + iconDim.y + 1;
	sint32  cityIcon = 0;
	CityData *cityData = unit.GetData()->GetCityData();

	if(HasSpecialIcon)
	{
		for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++)
		{
			if(buildingutil_Get(i, owner)->GetShowCityIconTopIndex(cityIcon))
			{
				if(cityData->GetImprovements() & ((uint64)1 << i))
				{
					//tileSet->GetMapIconData(cityIcon);
					color = GetPlayerColor(owner, fog);  //optional
					DrawColorizedOverlay(tileSet->GetMapIconData(cityIcon), surf, iconRect.left, iconRect.top, color);
					AddDirtyRectToMix(iconRect);
					iconRect.left += iconDim.x;
					iconRect.right += iconDim.x;
				}
			}
		}

		sint32 wonderIcon = 0;
		for(sint32 j = 0; j < g_theWonderDB->NumRecords(); j++)
		{
			if (wonderutil_Get(j, owner)->GetShowCityIconTopIndex(wonderIcon))
			{
				if(cityData->GetBuiltWonders() & ((uint64)1 << j))
				{
					//tileSet->GetMapIconData(cityIcon);
					color = GetPlayerColor(owner, fog);  //optional
					DrawColorizedOverlay(tileSet->GetMapIconData(wonderIcon), surf, iconRect.left, iconRect.top, color);
					AddDirtyRectToMix(iconRect);
					iconRect.left += iconDim.x;
					iconRect.right += iconDim.x;
				}
			}
		}
	}

	if (iconRect.left < 0 || iconRect.top < 0 ||
		iconRect.right >= surf->Width() ||
		iconRect.bottom >= surf->Height())
		return;

	//Add Corporation Icons?
}
