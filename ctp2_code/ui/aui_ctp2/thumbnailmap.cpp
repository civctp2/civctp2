//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : 
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
// - Resolved ambiguous ceil calls.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_directsurface.h"
#include "aui_blitter.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "aui_action.h"

#include "c3ui.h"
#include "thumbnailmap.h"

#include "player.h"             // g_player
#include "XY_Coordinates.h"
#include "World.h"              // g_theWorld
#include "Cell.h"
#include "UnseenCell.h"
#include "citydata.h"
#include "Unit.h"
#include "UnitData.h"
#include "pixelutils.h"
#include "colorset.h"           // g_colorSet
#include "SelItem.h"            // g_selected_item
#include "tiledmap.h"           // g_tiledMap
#include "primitives.h"


extern C3UI				*g_c3ui;


ThumbnailMap::ThumbnailMap(AUI_ERRCODE *retval,
							sint32 id,
							MBCHAR *ldlBlock,
							ControlActionCallback *ActionFunc,
							void *cookie)
	:
		aui_ImageBase(ldlBlock),
		aui_TextBase(ldlBlock),
		aui_Control(retval, id, ldlBlock, ActionFunc, cookie),
		PatternBase(ldlBlock, NULL)
{
	InitCommonLdl(ldlBlock);
}


ThumbnailMap::ThumbnailMap(AUI_ERRCODE *retval,
							uint32 id,
							sint32 x,
							sint32 y,
							sint32 width,
							sint32 height,
							MBCHAR *pattern,
							ControlActionCallback *ActionFunc,
							void *cookie)
	:
		aui_ImageBase((sint32)0),
		aui_TextBase((MBCHAR *)NULL),
		aui_Control(retval, id, x, y, width, height, ActionFunc, cookie),
		PatternBase(pattern)
{
	InitCommon();	
}

ThumbnailMap::~ThumbnailMap()
{
	delete m_mapSurface;
	delete m_cityList;
}


void ThumbnailMap::InitCommonLdl(MBCHAR *ldlBlock)
{
	aui_Ldl *theLdl = g_c3ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return;



	InitCommon();
}


void ThumbnailMap::InitCommon(void)
{
	AUI_ERRCODE			errcode;

	
	m_mapSurface = NULL;
	m_mapSize = NULL;

	m_tilePixelWidth = 0.0;
	m_tilePixelHeight = 0.0;

	m_selectedRoute = NULL;
	m_selectedCity = Unit();

	m_displayUnits = TRUE;
	m_displayLandOwnership = TRUE;
	m_displayCities = TRUE;
	m_displayCityNames = TRUE;
	m_displayTradeRoutes = TRUE;
	m_displayDirectRoutes = TRUE;
#ifdef BATTLE_FLAGS
	m_displayBattleFlags = TRUE;
#endif
	m_displayUnitMovement = TRUE;
	m_displayOverlay = TRUE;

	m_cityList = NULL;

	m_mapOverlay = NULL;

	m_cityFilterProc = NULL;

	
	m_mapSurface = new aui_DirectSurface(&errcode, m_width, m_height, 16, g_c3ui->DD());
	Assert( AUI_NEWOK(m_mapSurface, errcode) );

	
	CalculateMetrics();

	
	BuildCityList();

	
	RenderMap(m_mapSurface);
}



void ThumbnailMap::BuildCityList(void)
{
	sint32		p, i;

	delete m_cityList;
	m_cityList = new DynamicArray<CityInfo>();

	for (p=0; p<k_MAX_PLAYERS; p++) {
		Player	*player = g_player[p];
		if (player == NULL) continue;

		UnitDynamicArray *cities = player->GetAllCitiesList();
		Assert(cities != NULL);
		if (cities == NULL) return;

		CityInfo	info;
		RECT		cityRect;
		POINT		pt;
		MapPoint	pos;

		for (i=0; i<cities->Num(); i++) {
			Unit city;

			city = cities->Access(i);

			if (m_cityFilterProc) {
				if (!(*m_cityFilterProc)(city)) continue;
			}

			info.city = cities->Access(i);
			
			MapPoint cityPos = info.city.RetPos();
			pt = MapToPixel(&cityPos);
			
			SetRect(&cityRect, pt.x, pt.y, (sint32)(pt.x + m_tilePixelWidth), (sint32)(pt.y + m_tilePixelHeight));
			OffsetRect(&cityRect, (sint32)(m_tilePixelWidth/2), (sint32)(m_tilePixelHeight/2));
			InflateRect(&cityRect, (sint32)(2*m_tilePixelHeight), (sint32)(2*m_tilePixelHeight));
			
			if (cityRect.left < 0) OffsetRect(&cityRect, -cityRect.left, 0);
			if (cityRect.top < 0) OffsetRect(&cityRect, 0, -cityRect.top);
			if (cityRect.right >= m_width) OffsetRect(&cityRect, -(m_width - cityRect.right), 0);
			if (cityRect.bottom >= m_height) OffsetRect(&cityRect, 0, -(m_height - cityRect.bottom));

			info.cityRect = cityRect;

			info.cityBlink = FALSE;
			info.cityBlinkColor = COLOR_BLACK;
			info.cityBlinkTime = GetTickCount() + (rand() % k_THUMBNAIL_CITY_BLINK_RATE);

			m_cityList->Insert(info);
		}
	}
}


void ThumbnailMap::SetCityBlink(Unit city, BOOL blink, COLOR blinkColor)
{
	if (!m_cityList) return;

	BOOL found = FALSE;
	CityInfo info;

	for (sint32 i=0; i<m_cityList->Num(); i++) {
		info = m_cityList->Access(i);
		if (info.city == city) {
			found = TRUE;
			break;
		}
	}

	if (!found) return;

	info.cityBlink = blink;
	info.cityBlinkColor = blinkColor;
}


void ThumbnailMap::ClearMapOverlay(void)
{
	delete[] m_mapOverlay;
	m_mapOverlay = NULL;
}


void ThumbnailMap::SetMapOverlayCell(MapPoint &pos, COLOR color)
{
	if (m_mapOverlay == NULL) {
		sint32 len = m_mapSize->x * m_mapSize->y;
		m_mapOverlay = new COLOR[len];

		
		for (sint32 i=0; i<len; i++) 
			m_mapOverlay[i] = COLOR_MAX;
	}

	m_mapOverlay[pos.x + (pos.y * m_mapSize->x)] = color;
}



AUI_ERRCODE	ThumbnailMap::Resize( sint32 width, sint32 height )
{
	AUI_ERRCODE		errcode;

	errcode = aui_Region::Resize(width, height);
	Assert(errcode == AUI_ERRCODE_OK);

	if (m_mapSurface)
		delete m_mapSurface;

	
	m_mapSurface = new aui_DirectSurface(&errcode, width, height, 16, g_c3ui->DD());
	Assert( AUI_NEWOK(m_mapSurface, errcode) );

	CalculateMetrics();

	
	RenderMap(m_mapSurface);

	return errcode;
}


void ThumbnailMap::CalculateMetrics(void)
{
    Assert(g_theWorld); 
	m_mapSize = g_theWorld->GetSize();
	double		mapRatio;
	double		controlRatio;

    Assert(0 < m_mapSize->x); 
    Assert(0 < m_mapSize->y); 

	

	mapRatio = (double)(m_mapSize->x * 2) / (double)m_mapSize->y;
	controlRatio = (double)m_width / (double)m_height;

	if (mapRatio > controlRatio) {
		
		m_tilePixelWidth = ((double)(m_width-1) / (double)(m_mapSize->x+1));	
		m_tilePixelWidth -= ((m_tilePixelWidth / 2.0) / (double)(m_width));			
		m_tilePixelHeight = m_tilePixelWidth / 2.0;
	} else {
		
		m_tilePixelHeight = ((double)(m_height-1) / (double)(m_mapSize->y+1));
		m_tilePixelWidth = m_tilePixelHeight * 2.0;
	}

	m_centerX = (sint32) (((double)(m_width)/2.0) - (m_tilePixelWidth * ((double)m_mapSize->x))/2.0);
	m_centerY = (sint32) (((double)(m_height)/2.0) - (m_tilePixelHeight * ((double)m_mapSize->y))/2.0);
}


POINT ThumbnailMap::MapToPixel(sint32 x, sint32 y)
{
    Assert(0 < m_mapSize->x); 
	sint32 const	k = ((y / 2) + x) % m_mapSize->x;
	POINT			pt;

	pt.x = (sint32)(k * m_tilePixelWidth) + m_centerX;
	pt.y = (sint32)(y * m_tilePixelHeight) + m_centerY;

	return pt;
}


POINT ThumbnailMap::MapToPixel(MapPoint *pos)
{
	return MapToPixel(pos->x, pos->y);
}


void ThumbnailMap::RenderMap(aui_Surface *surf)
{
	sint32			i, j, k; 
	sint32			terrainType;
	Pixel16			color;
	double			xPos, yPos;
	sint32			xPos1, xPos2, yPos1, yPos2;
	double			nudge;

	if (!g_tiledMap) return;
	if (!g_theWorld) return;

	if (m_mapSize->x <= 0 || m_mapSize->y <= 0) return;

	for (i=0; i<m_mapSize->y; i++){
		for (j=0; j<m_mapSize->x; j++) {
			k = ((i / 2) + j) % m_mapSize->x;
			nudge = 0;
			if (i&1) {
				nudge = m_tilePixelWidth / 2.0;
			}
			
			xPos = (sint32)(k * m_tilePixelWidth);
			yPos = (sint32)(i * m_tilePixelHeight);

			MapPoint			pos(j, i);

			Vision				*vision = g_player[g_selected_item->GetVisiblePlayer()]->m_vision;
			UnseenCellCarton	cellCarton;
			Unit				top;

			
			if (vision->IsExplored(pos)) {
				
				if(m_displayOverlay && m_mapOverlay && m_mapOverlay[i*m_mapSize->x + j] != COLOR_MAX) {
					color = m_mapOverlay[i*m_mapSize->x + j];
				} else {
					if(m_displayUnits && g_theWorld->GetTopRadarUnit(pos, top)) {
						color = g_colorSet->GetPlayerColor(top.GetOwner());
					} else {
						Cell *theLandCell = g_theWorld->GetCell(j, i);			
						if (theLandCell->IsAnyUnitInCell()) { 
						}

						if (vision->GetLastSeen(pos, cellCarton)) {
							terrainType = cellCarton.m_unseenCell->GetTerrainType();
						} else {
							terrainType = theLandCell->GetTerrainType();
						}

						color = g_colorSet->GetColor((COLOR)(COLOR_TERRAIN_0 + terrainType));


						if (g_theWorld->IsGood(j,i)) { 
						}

						if (g_theWorld->IsRiver(j,i)) { 
						}

						
						if (m_displayLandOwnership) {
							PLAYER_INDEX	owner = theLandCell->GetOwner();
							if (owner != -1)
								color = g_colorSet->GetPlayerColor(owner);
						}
					}
				}
			} else {
				color = COLOR_BLACK;
			}

			xPos1 = (sint32)floor(xPos + nudge);
			yPos1 = (sint32)floor(yPos);
			xPos2 = (sint32)ceil(xPos + m_tilePixelWidth + nudge);
			yPos2 = (sint32)ceil(yPos + m_tilePixelHeight);

			RECT tileRect = {xPos1, yPos1, xPos2, yPos2};
			
			Assert(tileRect.right < surf->Width());
			Assert(tileRect.bottom < surf->Height());

			OffsetRect(&tileRect, m_centerX, m_centerY);

			primitives_PaintRect16(surf, &tileRect, color);

			
		}
	}
}


void ThumbnailMap::RenderTradeRoute(aui_Surface *surf, TradeRoute *route)
{
	sint32		i;
	Pixel16		color;
	POINT		p1, p2;
	MapPoint	pt;
	Pixel16		blackColor = g_colorSet->GetColor(COLOR_BLACK);

	const DynamicArray<MapPoint> *path = route->GetPath();
	
	
	if (route == m_selectedRoute) 
		color = g_colorSet->GetColor(COLOR_SELECT_1);
	else 
		color = (Pixel16)route->GetColor();

	
	pt = path->Get(0);
	p1 = MapToPixel(&pt);
	p1.x += (sint32)(m_tilePixelWidth/2);
	p1.y += (sint32)(m_tilePixelHeight/2);
	if (p1.x <= 0) p1.x = 1;
	if (p1.y <= 0) p1.y = 1;

	if (m_displayDirectRoutes) {
		pt = path->Get(path->Num() - 1);
		p2 = MapToPixel(&pt);
		if (p2.x <= 0) p2.x = 1;
		if (p2.y <= 0) p2.y = 1;

		primitives_DrawAALine16(surf, p1.x, p1.y, p2.x, p2.y, blackColor);
		primitives_DrawAALine16(surf, p1.x-1, p1.y-1, p2.x-1, p2.y-1, color);
	} else {
		for (i = 1; i < path->Num(); i++) {
			pt = path->Get(i);
			p2 = MapToPixel(&pt);
			p2.x += (sint32)(m_tilePixelWidth/2);
			p2.y += (sint32)(m_tilePixelHeight/2);
			if (p2.x <= 0) p2.x = 1;
			if (p2.y <= 0) p2.y = 1;

			
			

			primitives_DrawAALine16(surf, p1.x, p1.y, p2.x, p2.y, blackColor);
			primitives_DrawAALine16(surf, p1.x-1, p1.y-1, p2.x-1, p2.y-1, color);

			p1 = p2;
		}
	}
}


void ThumbnailMap::RenderTradeRoutes(aui_Surface *surf)
{
	sint32		i, j;

	Player	*player = g_player[g_selected_item->GetVisiblePlayer()];
	Assert(player != NULL);
	if (player == NULL) return;

	UnitDynamicArray *cities = player->GetAllCitiesList();
	Assert(cities != NULL);
	if (cities == NULL) return;

	for (i=0; i<cities->Num(); i++) {
		CityData			*cityData = cities->Get(i).GetData()->GetCityData();
		TradeDynamicArray	*source = cityData->GetTradeSourceList();

		
		for (j=0; j<source->Num(); j++) {
			TradeRoute route = source->Access(j);

			RenderTradeRoute(surf, &route);
		}
	}
}


void ThumbnailMap::RenderUnitMovement(aui_Surface *surf)
{
}


void ThumbnailMap::UpdateCities(aui_Surface *surf, sint32 x, sint32 y)
{
	sint32		i;

	Assert(m_cityList);
	if (!m_cityList) return;

	CityInfo		info;
	RECT			rect;
	Pixel16			color;

	for (i=0; i<m_cityList->Num(); i++) {
		info = m_cityList->Access(i);
		
		rect = info.cityRect;
		OffsetRect(&rect, x, y);

		
		primitives_FrameRect16(surf, &rect, g_colorSet->GetColor(COLOR_BLACK));
		InflateRect(&rect, -1, -1);
		if (info.cityBlink && GetTickCount() > info.cityBlinkTime) {
			info.cityBlinkTime = GetTickCount() + k_THUMBNAIL_CITY_BLINK_RATE;
			(*m_cityList)[i] = info;
			color = g_colorSet->GetColor(info.cityBlinkColor);
		} else {
			if (info.city == m_selectedCity) {
				color = g_colorSet->GetColor(COLOR_SELECT_1);
			} else {
				color = g_colorSet->GetPlayerColor(info.city.GetOwner());
			}
		}
		primitives_PaintRect16(surf, &rect, color);
		primitives_BevelRect16(surf, &rect, 2, 0, 16, 16);

		
		if (m_displayCityNames) {
		}
	}
}


void ThumbnailMap::RenderAll(aui_Surface *surf)
{
	
	RenderMap(surf);
	
	
	if (m_displayTradeRoutes) RenderTradeRoutes(surf);
	if (m_displayUnitMovement) RenderUnitMovement(surf);
}



void ThumbnailMap::UpdateMap(aui_Surface *surf, sint32 x, sint32 y)
{
	RECT		rect = {0, 0, m_mapSurface->Width(), m_mapSurface->Height()};



	
	g_c3ui->TheBlitter()->Blt(surf, x, y, m_mapSurface, &rect, k_AUI_BLITTER_FLAG_COPY);


	
	
	
	if (m_displayCities)
		UpdateCities(surf, x, y);

}


AUI_ERRCODE ThumbnailMap::DrawThis(aui_Surface *surface, sint32 x,	sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) 
		surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	UpdateMap(surface, rect.left, rect.top);

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}


void ThumbnailMap::MouseLGrabInside(aui_MouseEvent *data)
{
	if (IsDisabled()) return;

	BOOL		doRender = FALSE;
	sint32		i;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

	Assert(g_tiledMap != NULL);
	if (g_tiledMap == NULL) return;

	
	data->position.x -= X();
	data->position.y -= Y();

	
	CityInfo	info;
	for (i=0; i<m_cityList->Num(); i++) {
		info = m_cityList->Access(i);
		if (PtInRect(&info.cityRect, data->position)) {
			
			m_selectedCity = info.city;

			
			if ( m_ActionFunc )
				m_ActionFunc( this, C3_THUMBNAIL_ACTION_SELECTEDCITY, 0, m_cookie );
			else if ( m_action )
				m_action->Execute( this, C3_THUMBNAIL_ACTION_SELECTEDCITY, 0 );

			doRender = TRUE;
			break;
		}
	}

	


	if (doRender)
		RenderMap(m_mapSurface);
}


void ThumbnailMap::MouseRGrabInside(aui_MouseEvent *data)
{
	if (IsDisabled()) return;

	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

	Assert(g_tiledMap != NULL);
	if (g_tiledMap == NULL) return;

	
	data->position.x -= X();
	data->position.y -= Y();

}


BOOL ThumbnailMap::ShowTipWindow( aui_MouseEvent *mouseData )
{
	if ( m_tip && !m_showingTip )
	{
		PlaySound( AUI_SOUNDBASE_SOUND_TIP );

		
		m_showingTip = g_c3ui->AddWindow( m_tip ) == AUI_ERRCODE_OK;

		RECT rect = { mouseData->position.x, mouseData->position.y, 0, 0 };
		ToWindow( &rect );

		
		m_tip->Move(
			m_window->X() + rect.left - m_tip->Width() / 2,
			m_window->Y() + rect.top - m_tip->Height() );

		
		if ( m_tip->X() < 0 ) m_tip->Move( 0, m_tip->Y() );
		if ( m_tip->Y() < 0 ) m_tip->Move( m_tip->X(), 0 );
		sint32 adjustX = m_tip->X() + m_tip->Width() - g_c3ui->Width();
		sint32 adjustY = m_tip->Y() + m_tip->Height() - g_c3ui->Height();
		if ( adjustX > 0 ) m_tip->Offset( -adjustX, 0 );
		if ( adjustY > 0 ) m_tip->Offset( -adjustY, 0 );

		return m_showingTip;
	}

	return FALSE;
}



void ThumbnailMap::MouseNoChange( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( m_isMouseInside )
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	if ( !mouseData->lbutton && !mouseData->rbutton )
	if ( mouseData->time - m_noChangeTime > m_timeOut )
	{
		
		POINT		pos = mouseData->position;

		pos.x -= X();
		pos.y -= Y();

		
		CityInfo	info;
		sint32		i;

		for (i=0; i<m_cityList->Num(); i++) {
			info = m_cityList->Access(i);
			if (PtInRect(&info.cityRect, pos)) {
				

				

				SetWhichSeesMouse( this );

				if ( ShowTipWindow( mouseData ) )
					if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
						m_mouseCode = AUI_ERRCODE_HANDLED;

				break;
			}
		}

	}
}


void ThumbnailMap::MouseMoveInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	HideTipWindow();

	aui_Control::MouseMoveInside(mouseData);
}



AUI_ERRCODE ThumbnailMap::Idle( void )
{
	
	ShouldDraw(TRUE) ;


	return AUI_ERRCODE_OK;
}



void ThumbnailMap::UpdateAll( void )
{
	
	m_mapSize = g_theWorld->GetSize();

	BuildCityList();
	if (m_mapSurface) RenderAll(m_mapSurface);
}

