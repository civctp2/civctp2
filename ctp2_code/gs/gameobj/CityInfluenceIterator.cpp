
#include "c3.h"

#include "MapPoint.h"
#include "CityInfluenceIterator.h"

#include "CityRadius.h"
#include "World.h"
#include "Cell.h"
#include "CitySizeRecord.h"

#include "directions.h"
#include "UnitData.h"

#include "network.h"

#include "ConstDB.h"
#include "TiledMap.h"

CityInfluenceIterator::CityInfluenceIterator(const MapPoint &center, sint32 size)
{
	Init(center, size);
}

CityInfluenceIterator::CityInfluenceIterator()
{
	m_intRadius = 0;
	m_cur.Set(-1,-1);
	m_cityId = 0;
}

void CityInfluenceIterator::Init(const MapPoint &center, sint32 size)
{
	m_center = center;
	if(size >= g_theCitySizeDB->NumRecords()) {
		size = g_theCitySizeDB->NumRecords() - 1;
	}

	m_cur.Set(-1,-1);
	m_cityId = g_theWorld->GetCell(center)->GetCityOwner().m_id;
	m_intRadius = (sint16)g_theCitySizeDB->Get(size)->GetIntRadius();
}

void CityInfluenceIterator::Start()
{
	m_startX = m_center.x - m_intRadius;
	m_endX = m_center.x + m_intRadius;
	m_cur.x = m_startX;
	m_row = 0;
	m_cur.y = m_center.y + m_row;

	if(!WrapPoint(m_center, m_cur, m_wrappedCur) ||
	   g_theWorld->GetCell(m_wrappedCur)->GetCityOwner().m_id != m_cityId) {
		
		Next();
	}
}

bool CityInfluenceIterator::End()
{
	return m_row > (2 * m_intRadius);
}

void CityInfluenceIterator::Next()
{
	do {
		m_cur.x++;
		m_cur.y--;
		if(m_cur.x > m_endX) {
			m_row++;
			m_cur.x = m_startX;
			m_cur.y = m_center.y + m_row;
		}
	} while (((!WrapPoint(m_center, m_cur, m_wrappedCur)) ||
			  g_theWorld->GetCell(m_wrappedCur)->GetCityOwner().m_id != m_cityId) &&
			 !End());
}

MapPoint &CityInfluenceIterator::Pos()
{
	return m_wrappedCur;
}

RadiusIterator::RadiusIterator(const MapPoint &center, sint32 size)
{
	Init(center, size, size * size);
}

RadiusIterator::RadiusIterator(const MapPoint &center, sint32 size, sint32 squaredSize)
{
	Init(center, size, squaredSize);
}

RadiusIterator::RadiusIterator()
{
	m_intRadius = 0;
	m_cur.Set(-1,-1);
	m_cityId = 0;
}


void RadiusIterator::Init(const MapPoint &center, sint32 size, sint32 squaredSize)
{
	m_center = center;
	m_cur.Set(-1,-1);
	m_cityId = g_theWorld->GetCell(center)->GetCityOwner().m_id;
	m_intRadius = sint16(size);
	m_squaredRadius = sint16(squaredSize);
}

void RadiusIterator::Start()
{
	m_startX = m_center.x - m_intRadius;
	m_endX = m_center.x + m_intRadius;
	m_cur.x = m_startX;
	m_row = 0;
	m_cur.y = m_center.y + m_row;
	
	if(!WrapPoint(m_center, m_cur, m_wrappedCur) ||
		MapPoint::GetSquaredDistance(m_cur,m_center) > (m_squaredRadius)) {
		
		Next();
	}
}

void RadiusIterator::Next()
{
	do {
		m_cur.x++;
		m_cur.y--;
		if(m_cur.x > m_endX) {
			m_row++;
			m_cur.x = m_startX;
			m_cur.y = m_center.y + m_row;
		}
	} while (((!WrapPoint(m_center, m_cur, m_wrappedCur)) ||
		MapPoint::GetSquaredDistance(m_cur,m_center) > (m_squaredRadius)) &&
		!End());
}

SquareIterator::SquareIterator(const MapPoint &center, sint32 size)
{
	Init(center, size);
}

SquareIterator::SquareIterator()
{
	m_intRadius = 0;
	m_cur.Set(-1,-1);
	m_cityId = 0;
}


void SquareIterator::Init(const MapPoint &center, sint32 size)
{
	m_center = center;
	m_cur.Set(-1,-1);
	m_cityId = g_theWorld->GetCell(center)->GetCityOwner().m_id;
	m_intRadius = sint16(size);
}

void SquareIterator::Start()
{
	m_startX = m_center.x - m_intRadius;
	m_endX = m_center.x + m_intRadius;
	m_cur.x = m_startX;
	m_row = 0;
	m_cur.y = m_center.y + m_row;
	
	if(!WrapPoint(m_center, m_cur, m_wrappedCur)) {
		
		Next();
	}
}

void SquareIterator::Next()
{
	do {
		m_cur.x++;
		m_cur.y--;
		if(m_cur.x > m_endX) {
			m_row++;
			m_cur.x = m_startX;
			m_cur.y = m_center.y + m_row;
		}
	} while ((!WrapPoint(m_center, m_cur, m_wrappedCur)) && !End());
}

bool ExpandInfluence(Unit &city, const MapPoint &centerPos, MapPoint curPos,
					 const CitySizeRecord *rec);


void GenerateCityInfluence(const MapPoint &cpos, sint32 size)
{
	MapPoint cur, wrappedCur;
	sint16 row = 0;
	const CitySizeRecord *rec = g_theCitySizeDB->Get(size);

	sint16 intRadius = (sint16)rec->GetIntRadius();

	
	Assert(g_theWorld->GetCell(cpos)->HasCity());

	Unit city = g_theWorld->GetCell(cpos)->GetCity();

	
	
	while(row <= (2 * intRadius)) {
		sint32 i;
		cur.x = cpos.x - intRadius;
		cur.y = cpos.y + row;
		for(i = 0; i <= 2 * intRadius; i++) {
			if(WrapPoint(cpos, cur, wrappedCur)) {
				g_theWorld->GetCell(wrappedCur)->SetScratch(0);
			}
			cur.x++;
			cur.y--;
		}
		row++;
	}

	g_theWorld->GetCell(cpos)->SetScratch(1);

	if(cpos.GetNeighborPosition(NORTHEAST, cur)) ExpandInfluence(city, cpos, cur, rec);
	if(cpos.GetNeighborPosition(SOUTHEAST, cur)) ExpandInfluence(city, cpos, cur, rec);
	if(cpos.GetNeighborPosition(SOUTHWEST, cur)) ExpandInfluence(city, cpos, cur, rec);
	if(cpos.GetNeighborPosition(NORTHWEST, cur)) ExpandInfluence(city, cpos, cur, rec);

	if(g_tiledMap)
		g_tiledMap->RedrawTile(&cpos);
}

bool ExpandInfluence(Unit &city, const MapPoint &centerPos, MapPoint curPos,
					 const CitySizeRecord *rec)
{
	Cell *cell = g_theWorld->GetCell(curPos);

	if(cell->GetScratch() != 0) return false;
	if(cell->GetCityOwner().m_id != 0 && cell->GetCityOwner().m_id != city.m_id) return false;
	if(UnitData::GetDistance(centerPos, curPos, 0) > rec->GetSquaredRadius()) return false;
	
	cell->SetCityOwner(city);
	sint32 newOwner = -1;
	if (city.m_id == 0x0)
		cell->SetOwner(-1);
	else {
		cell->SetOwner(city.GetOwner());
		newOwner = city.GetOwner();
	}
	cell->SetScratch(1);

	if(newOwner >= 0) {
		g_network.Block(newOwner);
		g_network.Enqueue(cell, curPos.x, curPos.y);
		g_network.Unblock(newOwner);
	}

	MapPoint nextPos;

	bool redrawMe = false;

	if(curPos.GetNeighborPosition(NORTHEAST, nextPos)) 
		if(!ExpandInfluence(city, centerPos, nextPos, rec)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(SOUTHEAST, nextPos))
		if(!ExpandInfluence(city, centerPos, nextPos, rec)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(SOUTHWEST, nextPos))
		if(!ExpandInfluence(city, centerPos, nextPos, rec)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(NORTHWEST, nextPos))
		if(!ExpandInfluence(city, centerPos, nextPos, rec)) {
			redrawMe = true;
		}

	if(redrawMe && g_tiledMap) {
		g_tiledMap->RedrawTile(&curPos);
	}

	return true;
}

bool ExpandBorders(const MapPoint &center, MapPoint curPos, sint32 player, sint32 squaredRadius);


void GenerateBorders(const MapPoint &cpos, sint32 player, sint32 intRadius, sint32 squaredRadius)
{
	MapPoint cur, wrappedCur;
	sint16 row = 0;
	
	sint16 intRad = (sint16) intRadius;

	
	
	while(row <= (2 * intRad)) {
		sint32 i;
		cur.x = cpos.x - intRad;
		cur.y = cpos.y + row;
		for(i = 0; i <= 2 * intRad; i++) {
			if(WrapPoint(cpos, cur, wrappedCur)) {
				g_theWorld->GetCell(wrappedCur)->SetScratch(0);
			}
			cur.x++;
			cur.y--;
		}
		row++;
	}

	Cell *cell = g_theWorld->GetCell(cpos);
	cell->SetOwner(player);
	cell->SetScratch(1);

	g_network.Block(player);
	g_network.Enqueue(cell, cpos.x, cpos.y);
	g_network.Unblock(player);

	if(cpos.GetNeighborPosition(NORTHEAST, cur)) ExpandBorders(cpos, cur, player, squaredRadius);
	if(cpos.GetNeighborPosition(SOUTHEAST, cur)) ExpandBorders(cpos, cur, player, squaredRadius);
	if(cpos.GetNeighborPosition(SOUTHWEST, cur)) ExpandBorders(cpos, cur, player, squaredRadius);
	if(cpos.GetNeighborPosition(NORTHWEST, cur)) ExpandBorders(cpos, cur, player, squaredRadius);

	g_tiledMap->RedrawTile(&cpos);
}

bool ExpandBorders(const MapPoint &center, MapPoint curPos, sint32 player, sint32 squaredRadius)
{
	Cell *cell = g_theWorld->GetCell(curPos);
	if(cell->GetScratch() != 0) return false; 

	cell->SetScratch(1);

	if(cell->GetOwner() >= 0 && cell->GetOwner() != player) return false; 
	if(UnitData::GetDistance(center, curPos, 0) > squaredRadius) return false;

	cell->SetOwner(player);

	if(player >= 0)
		g_network.Block(player);
	g_network.Enqueue(cell, curPos.x, curPos.y);
	if(player >= 0)
		g_network.Unblock(player);

	MapPoint nextPos;

	bool redrawMe = false;

	if(curPos.GetNeighborPosition(NORTHEAST, nextPos)) 
		if(!ExpandBorders(center, nextPos, player, squaredRadius)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(SOUTHEAST, nextPos))
		if(!ExpandBorders(center, nextPos, player, squaredRadius)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(SOUTHWEST, nextPos))
		if(!ExpandBorders(center, nextPos, player, squaredRadius)) {
			redrawMe = true;
		}
	if(curPos.GetNeighborPosition(NORTHWEST, nextPos))
		if(!ExpandBorders(center, nextPos, player, squaredRadius)) {
			redrawMe = true;
		}

	if(redrawMe) {
		g_tiledMap->RedrawTile(&curPos);
	}

	return true;
}
