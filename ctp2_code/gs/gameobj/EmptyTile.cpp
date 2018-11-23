#include "c3.h"

#include "EmptyTile.h"

EmptyTile::EmptyTile(MapPoint &pos, sint32 f, sint32 p, sint32 g)
{
	m_pos = pos ;
	m_food = f ;
	m_production = p ;
	m_gold = g ;
}

EmptyTile::EmptyTile(EmptyTile *tile)
	{
	m_pos = tile->GetPos() ;
	m_food = tile->GetFood() ;
	m_production = tile->GetProduction() ;
	m_gold = tile->GetGold() ;
	}

EmptyTile::EmptyTile(void)
	{
	m_pos = MapPoint(0,0) ;
	m_food = 0 ;
	m_production = 0 ;
	m_gold = 0 ;
	}
