#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __UNOCCUPIED_TILES_H__
#define __UNOCUUPIED_TILES_H__

#include "Unit.h"
#include "EmptyTile.h"

#define k_MAX_TILES_IN_LIST	22

enum
	{
	DOMINANT_FOOD,
	DOMINANT_PRODUCTION,
	DOMINANT_GOLD,
	} ;

class UnoccupiedTiles
	{
	private :
		Unit	m_city ;

		MapPoint	m_pos ;

		sint32	m_numUnoccupiedTiles,
				m_numTilesFound,
				m_numBestFoodTiles,
				m_numBestProductionTiles,
				m_numBestGoldTiles,
				m_numBestTradeTiles ;

		EmptyTile	*m_unordered[k_MAX_TILES_IN_LIST] ;

		sint32		m_bestFoodTiles[k_MAX_TILES_IN_LIST],
					m_bestProductionTiles[k_MAX_TILES_IN_LIST],
					m_bestGoldTiles[k_MAX_TILES_IN_LIST];

		BOOL CompareTiles(sint32 *list, const sint32 first, const sint32 second, const sint32 dominant) ;
		void ObtainItemValues(const sint32 idx, const sint32 dominant, sint32 &a, sint32 &b,
            sint32 &c);
		void SortList(sint32 *list, sint32 numItems, sint32 dominant) ;
		void SwitchTiles(sint32 *list, sint32 numItems, const sint32 idxA, const sint32 idxB) ;

		sint32 InsertTileIntoUnorderedList(const EmptyTile &tile) ;
		void InsertTileIntoBestFoodList(const sint32 tileIdx) ;
		void InsertTileIntoBestProductionList(const sint32 tileIdx) ;
		void InsertTileIntoBestGoldList(const sint32 tileIdx) ;

	public :
		UnoccupiedTiles(void) ;
		~UnoccupiedTiles(void) ;

		void ClearList(void) ;
		void InsertTile(const EmptyTile &tile) ;
		void BuildListOfCityTiles(const Unit city) ;
		BOOL RemoveTile(const sint32 tileIdx, EmptyTile &tile) ;
		BOOL GetTile(const sint32 tileIdx, EmptyTile &tile) ;
		sint32 GetNumUnoccupiedTiles(void) ;
		BOOL GetBestFoodTile(sint32 &tileIdx) ;
		BOOL GetBestProductionTile(sint32 &tileIdx) ;
		BOOL GetBestGoldTile(sint32 &tileIdx) ;

	} ;

#endif
