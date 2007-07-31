#pragma once
#ifndef __EMPTY_TILE_H__
#define __EMPTY_TILE_H__

#include "MapPoint.h"

class EmptyTile
	{
	private	:
		MapPoint	m_pos ;

		sint32	m_food,												
				m_production,										
				m_gold												
				;											


	public :
		EmptyTile(MapPoint &pos, sint32 f, sint32 p, sint32 g) ;
		EmptyTile(EmptyTile *tile) ;
		EmptyTile(void) ;

		MapPoint GetPos(void) { return (m_pos) ; }
		sint32 GetFood(void) { return (m_food) ; }
		sint32 GetProduction(void) { return (m_production) ; }
		sint32 GetGold(void) { return (m_gold) ; }

	} ;

#endif