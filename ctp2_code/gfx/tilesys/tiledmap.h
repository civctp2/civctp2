//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Tile map handling
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
// - Debug version when set.
// 
// __SPRITETEST__
// - ?
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - pragma commented out
// - unused function commmented out
// - Repaired funny combination of protected data with non-virtual destructor.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __TILEDMAP_H__
#define __TILEDMAP_H__

#include "MapPoint.h"
#include "Vision.h"

#ifndef __SPRITETEST__
#include "aui.h"
#include "aui_mouse.h"
#else
#include "mouse.h"
#endif

#include "pixelutils.h"
#include "tileutils.h"
#include "tileset.h"
#include "colorset.h"
#include "World.h"
#include "SelItem.h"

#define k_MEDIUM_KEY	0x4208

#define k_FOW_COLOR	0x0000
#define k_FOW_BLEND_VALUE	16

struct TILEHITMASK {
	uint16		start;
	uint16		end;  
	double      d_start;
	double      d_end;
};

struct GridRect {
	RECT		rect;
	BOOL		dirty;
};

#define k_MAX_ZOOM_LEVELS	6
#define k_ZOOM_NORMAL		5
#define k_ZOOM_SMALLEST		0
#define k_ZOOM_LARGEST		5


#define k_OVERLAY_FLAG_NORMAL		0x00000000
#define k_OVERLAY_FLAG_SHADOWSONLY	0x00000001
#define k_OVERLAY_FLAG_NOSHADOWS	0x00000002


#define k_BORDER_SOLID 0
#define k_BORDER_DASHED 1

class aui_Surface;
class aui_DirtyList;

class BaseTile;
class TileSet;
class TileInfo;

class MapPoint;
class Path;

class CivArchive;
class CityData;

class UnitActor;

class EffectActor;
class GoodActor;

class aui_BitmapFont;
class Army; 
class Unit; 
class CellUnitList; 
class TerrainImprovementRecord;

enum WORLD_DIRECTION;

class TiledMap {
public:
	TiledMap(MapPoint &size);
	virtual ~TiledMap();

	void			AllocateTileInfoStorage(MapPoint *size);
	void			DisposeTileInfoStorage(void);
	TileInfo		*GetTileInfoStoragePtr(MapPoint &pos);
	sint32			Initialize(RECT *rect);

  
	void			InitGrid(sint32 maxPixelsPerGridRectX,sint32 maxPixelsPerGridRectY);
	void			DeleteGrid(void);
	void			CheckRectAgainstGrid(RECT &rect, aui_DirtyList *list);
	void			ClearGrid(void);

	void			AddDirty(sint32 left, sint32 top, sint32 width, sint32 height, aui_DirtyList *list);
	void			AddDirtyRect(RECT &rect, aui_DirtyList *list);
	void			AddDirtyTile(MapPoint &pos, aui_DirtyList *list);

	void			AddDirtyToMap(sint32 left, sint32 top, sint32 width, sint32 height);
	void			AddDirtyRectToMap(RECT &rect);
	void			AddDirtyTileToMap(MapPoint &pos);

	void			AddDirtyToMix(sint32 left, sint32 top, sint32 width, sint32 height);
	void			AddDirtyRectToMix(RECT &rect);
	void			AddDirtyTileToMix(MapPoint &pos);

	void			ClearMixDirtyRects(void);
	void			CopyMixDirtyRects(aui_DirtyList *dest);
	void			InvalidateMap(void);
	void			ValidateMap(void);
	void			InvalidateMix(void);
	void			ValidateMix(void);
	void			RestoreMixFromMap(aui_Surface *destSurf);
	void			UpdateMixFromMap(aui_Surface *mixSurf);
	void			OffsetMixDirtyRects(sint32 deltaX, sint32 deltaY);


	void			LoadTileset(void);

	sint16			TryRiver(BOOL c, BOOL n, BOOL ne, BOOL e, BOOL se, BOOL s, BOOL sw, BOOL w, BOOL nw, BOOL cwater);
	BOOL			TryTransforms(MapPoint &pos, uint16 c, uint16 n, uint16 ne, uint16 e, uint16 se, uint16 s, 
									uint16 sw, uint16 w, uint16 nw, uint16 *newIndex);
	void			TryMegaTiles(MapPoint &pos, BOOL regenTilenum);
	void			PostProcessTile(MapPoint &pos, TileInfo *theTileInfo,
									BOOL regenTilenum = TRUE);
	void			PostProcessMap(BOOL regenTilenums = TRUE);

	void			BreakMegaTile(MapPoint &pos);
	void			TileChanged(MapPoint &pos);

	void			ReloadGoodActors(void);

	void			GenerateHitMask(void);
	void			DrawHitMask(aui_Surface *surf, const MapPoint &pos);
	void			DrawColoredHitMask(aui_Surface *surf, const MapPoint &pos, COLOR color);
	void			DrawColoredHitMaskEdge(aui_Surface *surf, const MapPoint &pos, Pixel16 color, WORLD_DIRECTION d);
	void			DrawColoredBorderEdge(aui_Surface *surf, const MapPoint &pos, Pixel16 color, WORLD_DIRECTION d, sint32 dashMode);
	void			DrawHitMask(aui_Surface *surf, const MapPoint &pos, RECT *mapViewRect, RECT *destRect);
	void			SetHiliteMouseTile(MapPoint &pos);
	void			DrawHiliteMouseTile(aui_Surface *destSurf);
  	
	void			SetTerrainOverlay(TerrainImprovementRecord *rec, MapPoint &pos,Pixel16 color);
	void			DrawTerrainOverlay(aui_Surface *destSurf);

	sint32			RecalculateViewRect(RECT &myRect);
	sint32			CalculateMetrics(void);


	
	
	
	
	
	
	
	
	
	
	
	inline void		GetMapMetrics(sint32 *width,sint32 *height) const
	{
		*width = g_theWorld->GetWidth();
		*height = g_theWorld->GetHeight();
	}

	
	
	inline sint32   GetMapWidth() const
	{
		return g_theWorld->GetWidth();
	}

	sint32			CalculateWrap(aui_Surface *surface, sint32 i, sint32 j);
	sint32			CalculateWrapClipped(aui_Surface *surface, sint32 i, sint32 j);
	sint32			CalculateHatWrap(aui_Surface *surface, sint32 i, sint32 j);

	sint32			DrawImprovements(aui_Surface *surface, sint32 i, sint32 j, bool clip=false);

	sint32			RepaintTiles(RECT *repaintRect);
	sint32			RepaintTilesClipped(RECT *repaintRect);

	
	sint32			RepaintImprovements(RECT *repaintRect,bool clip=false);
	sint32			RepaintHats(RECT *repaintRect,bool clip=false);
	sint32          RepaintBorders(RECT *repaintRect, bool clip=false);
	sint32			RepaintEdgeX(RECT *repaintRect);
	sint32			RepaintEdgeY(RECT *repaintRect);
	sint32			Refresh(void);

	sint32			DrawCityRadius(const MapPoint &cpos, COLOR color,sint32 size=1);
	sint32			DrawCityRadius1(const MapPoint &cpos, COLOR color);
	sint32			PaintColoredTile(sint32 x, sint32 y, COLOR color);

	void			ProcessLayerSprites(RECT *processRect, sint32 layer);
	void			PaintGoodActor(GoodActor *actor, BOOL fog = FALSE);

	
	void			ProcessUnit(Unit unit);
	void			ProcessUnit(CellUnitList *list);

	
	void			ColorMagnitudeToRGB(uint8 col, sint32 *r, sint32 *g, sint32 *b);
	void			DrawSomeText(BOOL mixingPort, MBCHAR *text, sint32 tx, sint32 ty, COLORREF bgColorRef, COLORREF fgColorRef);

	void            DrawChatText();
	void            AddChatDirtyRectToMap();

#ifdef _DEBUG
	void            DrawRectMetrics();
	void            ClearRectMetrics();
	void            IncRectMetric();
	void            RectMetricNewLoop();

	float           num_loops;
	float           num_rects;

#endif
	
	void			PaintArmyActors(MapPoint &pos);

	void			PaintUnitActor(UnitActor *actor, BOOL fog = FALSE);

	void			PaintEffectActor(EffectActor *actor);

	sint32			RepaintLayerSprites(RECT *paintRect, sint32 layer);
	sint32			RepaintSprites(aui_Surface *surf, RECT *paintRect, bool scrolling);

	sint32			OffsetLayerSprites(RECT *paintRect, sint32 deltaX, sint32 deltaY, sint32 layer);
	sint32			OffsetSprites(RECT *paintRect, sint32 deltaX, sint32 deltaY);

	
	void			DrawStartingLocations(aui_Surface *surf, sint32 layer);

	inline	bool	IsScrolling(){ return m_isScrolling;};
	inline	void	SetScrolling(bool scroll){ m_isScrolling=scroll;};
	void			ScrollPixels(sint32 deltaX, sint32 deltaY, aui_Surface *surf);
	BOOL			ScrollMap(sint32 deltaX, sint32 deltaY);
	BOOL			ScrollMapSmooth(sint32 deltaX, sint32 deltaY);
	bool			SmoothScrollAligned();
	void			GetSmoothScrollOffsets(sint32 &xoff,sint32 &yoff)
					{
						xoff = m_smoothOffsetX;
						yoff = m_smoothOffsetY;
					};
	void			SetSmoothScrollOffsets(sint32 xoff,sint32 yoff)
					{
						m_smoothOffsetX = xoff;
						m_smoothOffsetY = yoff;
					};
	
	
	

	sint32			RedrawHat(aui_Surface *surface, sint32 i, sint32 j,bool clip=false);
	sint32          RedrawBorders(aui_Surface *surface, sint32 i, sint32 j, bool clip = false);
	void			RedrawTile(const MapPoint *point);
	void			RedrawTileClipped(const MapPoint *point);
	void			BlackTile(aui_Surface *surface, const MapPoint *point);


	bool			DrawImprovementsLayer(aui_Surface *surface, MapPoint &pos, sint32 x, sint32 y,bool clip=false);
	void			DrawPartiallyConstructedImprovement(aui_Surface *surface, uint32 env, 
													sint32 type, sint32 x, sint32 y, 
													uint16 index, BOOL fog, sint32 percentComplete);
													//Added by Martin Gühmann sint32 percentComplete by Martin Gühmann
	
	
	
	void            DrawAnImprovement(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y,bool fog,bool clip=false);

	
	void	DrawTransitionTileClipped(aui_Surface *surface, MapPoint &pos, sint32 x, sint32 y);
	sint32	DrawOverlayClipped(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y,sint32 flags = k_OVERLAY_FLAG_NORMAL);
	sint32	DrawBlendedTileClipped(aui_Surface *surface, const MapPoint &pos,sint32 x, sint32 y, Pixel16 color, sint32 blend);
	sint32	DrawBlendedOverlayClipped(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y,Pixel16 color, sint32 blend, sint32 flags = k_OVERLAY_FLAG_NORMAL);
	sint32	DrawDitheredTileClipped(aui_Surface *surface, sint32 x, sint32 y, Pixel16 color);
	sint32	DrawDitheredOverlayClipped(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, Pixel16 color);
	
	
	sint32			DrawOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, 
									sint32 flags = k_OVERLAY_FLAG_NORMAL);
	sint32			DrawOverlayIntoSurface(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y);
	sint32			DrawColorBlendedOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, 
												Pixel16 color);
	void			DrawColorBlendedOverlayScaled(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, 
													sint32 destWidth, sint32 destHeight, Pixel16 color);
	sint32			DrawColorBlendedOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, Pixel16 color, sint32 blendValue);
	void			DrawColorBlendedOverlayScaled(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, 
								 sint32 destWidth, sint32 destHeight, Pixel16 color, sint32 blendValue);
	sint32			DrawColorizedOverlay(Pixel16 *data, aui_Surface *surf, sint32 x, sint32 y, Pixel16 color=0x0000);
	sint32			DrawColorizedOverlayIntoMix(Pixel16 *data, sint32 x, sint32 y, Pixel16 color=0x0000);
	sint32			DrawColorizedOverlayIntoSurface(Pixel16 *data, aui_Surface *surf, sint32 x, sint32 y, Pixel16 color);
	sint32			DrawDitheredOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, Pixel16 color);
	void			DrawDitheredOverlayScaled(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight, Pixel16 color);
	sint32			DrawBlendedOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, 
										Pixel16 color, sint32 blend, sint32 flags = k_OVERLAY_FLAG_NORMAL);
	void			DrawBlendedOverlayScaled(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight, 
												Pixel16 color, sint32 blend, sint32 flags = k_OVERLAY_FLAG_NORMAL);
	void			DrawScaledOverlay(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, 
											sint32 destWidth, sint32 destHeight, 
											sint32 flags = k_OVERLAY_FLAG_NORMAL);
	void			DrawScaledOverlayIntoMix(Pixel16 *data, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight);

	sint32			DrawBlendedOverlayIntoMix(Pixel16 *data, sint32 x, sint32 y, Pixel16 color, sint32 blend);
	void			DrawBlendedOverlayScaledIntoMix(Pixel16 *data, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight, Pixel16 color, sint32 blend);

	sint32			DrawDitheredOverlayIntoMix(Pixel16 *data, sint32 x, sint32 y, BOOL fogged);
	void			DrawDitheredOverlayScaledIntoMix(Pixel16 *data, sint32 x, sint32 y, 
												sint32 destWidth, sint32 destHeight, 
												BOOL fogged);

	sint32			DrawBlackTile(aui_Surface *surface, sint32 x, sint32 y);
	sint32			QuickBlackBackGround(aui_Surface *surface);
	sint32			DrawDitheredTile(aui_Surface *surface, sint32 x, sint32 y, Pixel16 color);
	void			DrawDitheredTileScaled(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,Pixel16 color);

	sint32			DrawBlendedTile(aui_Surface *surface, const MapPoint &pos,sint32 x, sint32 y, Pixel16 color, sint32 blend);
	void			DrawBlendedTileScaled(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,Pixel16 color,sint32 blend);

	sint32			DrawTileBorder(aui_Surface *surface, sint32 x, sint32 y, Pixel16 color);
	void			DrawTileBorderScaled(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,Pixel16 color);
	
	
	inline Pixel16	average(Pixel16 pixel1, Pixel16 pixel2, Pixel16 pixel3, Pixel16 pixel4);


	void			DrawBlackScaledLow(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight);

	void			ProcessRun(Pixel16 **rowData1, Pixel16 **rowData2, Pixel16 *pix1, Pixel16 *pix2, 
							sint32 pos, Pixel16 destPixel, short transparency, Pixel16 outlineColor, 
							sint32 flags = k_OVERLAY_FLAG_NORMAL);
	inline sint32	ReadTag(sint32 *mode, Pixel16 **rowData, sint32 *alpha);


	void		Blt(aui_Surface *surf);

	BOOL		TileIsVisible(sint32 mapX, sint32 mapY);
	BOOL		TileIsCompletelyVisible(sint32 mapX, sint32 mapY, RECT *viewRect = NULL);
	
	BOOL        TileIsVisible(sint32 mapX, sint32 mapY, sint32 mapZ);

	double		GetScale(void) { return m_scale; }
	void		SetScale(double s) { m_scale = s; }

	void		InitLUT(void);

	TileInfo   *GetTileInfo(const MapPoint &pos);
	RECT		*GetMapViewRect(void) { return &m_mapViewRect; }

	TileSet		*GetTileSet(void) { return m_tileSet; }

	void		DrawLabel(aui_Surface *surface, sint32 color, sint32 x, sint32 y);

    void        SlowDrawText(aui_Surface *surface, char *buf, sint32 color, sint32 x, sint32 y);
	void        DrawNumber(aui_Surface *surface, sint32 num, sint32 color, 
                    sint32 x, sint32 y);

	void		HandleCheat(MapPoint &pos);

	UnitActor	*GetClickedUnit(aui_MouseEvent *data);

	BOOL		PointInMask(POINT hitPt);
	BOOL		MousePointToTilePos(POINT point, MapPoint &tilePos);
	void		AdjustForOverlappingSprite(POINT point, MapPoint &pos);

	void		MouseDrag(aui_MouseEvent *data);
	void		Click(aui_MouseEvent *data, BOOL doubleClick);
	void		Drop(aui_MouseEvent *data);
	void		Idle(void);

	BOOL		GetMousePos(POINT &pos);
	BOOL		GetMouseTilePos(MapPoint &pt);

	void		DrawTransitionTile(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y);
	void		DrawTransitionTileScaled(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight);

	void		DrawWater(void);

    BOOL        CanDrawSpecialMove(SELECT_TYPE sType, Army &sel_army, const MapPoint &old_pos, const MapPoint &cur_pos);
	void		DrawLegalMove(aui_Surface *pSurface);
	void		DrawUnfinishedMove(aui_Surface *pSurface);

	void		DrawPath(Path *path);

	void		DrawARoadPiece(aui_Surface *surface, Pixel16 *data, sint32 x, sint32 y, 
									BOOL fog, sint32 flags = k_OVERLAY_FLAG_NORMAL);
	void		DrawRoads(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, 
									sint32 roadType, uint16 roadOffset, BOOL fog, 
									sint32 flags = k_OVERLAY_FLAG_NORMAL);
	void		DrawCityRoads(aui_Surface *surface, const MapPoint &pos, sint32 x, sint32 y, 
									sint32 roadType, uint16 roadOffset, BOOL fog, 
									sint32 flags = k_OVERLAY_FLAG_NORMAL);

	void		DrawCityNames(aui_Surface *surf, sint32 layer);
	void		DrawCityIcons(aui_Surface *surf, MapPoint &pos, sint32 owner, BOOL fog, RECT &popRect,
								BOOL isBioInfected, BOOL isNanoInfected, BOOL isConverted, 
								BOOL isFranchised, BOOL isInjoined, BOOL wasHappinessAttacked,
								sint32 bioInfectedOnwer, sint32 nanoInfectedOwner, sint32 convertedOwner,
								sint32 franchiseOwner, sint32 injoinedOwner, sint32 happinessAttackOwner,
								uint32 slaveBits, BOOL isRioting, BOOL hasAirport, BOOL hasSleepingUnits,
								BOOL isWatchful);

	void		Serialize(CivArchive &archive);

	void		NextPlayer(void);
	void        CopyVision();
	Vision		*GetLocalVision(void) { return m_localVision; }
	void        ReallocateVision();

	aui_Surface *GetSurface(void) { return m_surface; } 

	TILEHITMASK	*GetTileHitMask( void ) { return m_tileHitMask; }
	void		GetMapBounds( RECT &rect ) { rect = m_mapBounds; }

	void		RetargetTileSurface(aui_Surface *surface);

	void		LockSurface(void);
	void		LockThisSurface(aui_Surface *surface);
	void		UnlockSurface(void);
	BOOL		IsLocked(void) const { return m_surfIsLocked; }

	void		DrawHilite( BOOL drawHilite ) { m_drawHilite = drawHilite; }

	BOOL        ReadyToDraw() const;

	aui_BitmapFont		*GetFont(void) { return m_font; }
	MBCHAR				*GetFortifyString(void) { return m_fortifyString; }

	sint32 GetVisibleCellOwner(MapPoint &pos);
	uint32 GetVisibleCityOwner(MapPoint &pos);
	void DrawNationalBorders(aui_Surface *surface, MapPoint &pos);

	
	
	
	
	void SetZoomLevel(sint32 level);
	
	void SetZoomCallback(void (*callback)()) { m_zoomCallback = callback; }

	
	bool CanZoomIn() const;		
	bool ZoomIn();				
	bool CanZoomOut() const;	
	bool ZoomOut();				

	
	sint32 GetZoomLevel() const { return m_zoomLevel; }
	sint32 GetZoomTilePixelWidth() const { return m_zoomTilePixelWidth[m_zoomLevel];}
	sint32 GetZoomTilePixelHeight() const { return m_zoomTilePixelHeight[m_zoomLevel];}
	sint32 GetZoomTileGridHeight() const { return m_zoomTileGridHeight[m_zoomLevel]; }
	sint32 GetZoomTileHeadroom() const { return m_zoomTileHeadroom[m_zoomLevel]; }
	double GetZoomScale(sint32 level) const { return m_zoomTileScale[level]; }

protected:
	
	void CalculateZoomViewRectangle(sint32 zoomLevel, RECT &rectangle) const;

	
	void ZoomHitMask();

	
	void ZoomUpdate(sint32 zoomLevel);

	
	
	
	sint32	m_zoomLevel;
	sint32	m_zoomTilePixelWidth[k_MAX_ZOOM_LEVELS];
	sint32	m_zoomTilePixelHeight[k_MAX_ZOOM_LEVELS];
	sint32	m_zoomTileGridHeight[k_MAX_ZOOM_LEVELS];
	sint32	m_zoomTileHeadroom[k_MAX_ZOOM_LEVELS];
	double	m_zoomTileScale[k_MAX_ZOOM_LEVELS];
	void	(*m_zoomCallback)();

	
	bool				 m_isScrolling;

	aui_Surface		*m_surface;			
	aui_Surface		*m_mapSurface;		

	
	aui_Surface		*m_lockedSurface;	
	uint8			*m_surfBase;
	sint32			m_surfWidth;
	sint32			m_surfHeight;
	sint32			m_surfPitch;
	BOOL			m_surfIsLocked;

	RECT			m_displayRect;		
	RECT			m_surfaceRect;		
	
	RECT			m_mapBounds;		
	RECT			m_mapViewRect;		

	double			m_scale;
	sint32			m_smoothOffsetX,m_smoothOffsetY;
	sint32			m_smoothLastX,m_smoothLastY;

	
	bool						m_overlayActive;
	TerrainImprovementRecord	*m_overlayRec;
	MapPoint					m_overlayPos;
	Pixel16						m_overlayColor;


	TILEHITMASK		m_tileHitMask[k_TILE_GRID_HEIGHT];

	TileSet			*m_tileSet;

	MapPoint		m_hiliteMouseTile;
	BOOL			m_drawHilite;

	aui_DirtyList	*m_mapDirtyList;
	aui_DirtyList	*m_mixDirtyList;
	aui_DirtyList	*m_oldMixDirtyList;
	Vision			*m_localVision;
	BOOL			m_nextPlayer; 

	PLAYER_INDEX    m_oldPlayer;

#ifdef _DEBUG
public:
	BOOL			m_showPopHack;
#endif

	aui_BitmapFont	*m_font;
	MBCHAR			m_fortifyString[4];

	sint32			m_gridWidth;
	sint32			m_gridHeight;

	
	float			m_one_over_gridWidth;
	float			m_one_over_gridHeight;

	GridRect		**m_gridRects;

	RECT m_chatRect;
};

extern TiledMap *g_tiledMap;

#endif

