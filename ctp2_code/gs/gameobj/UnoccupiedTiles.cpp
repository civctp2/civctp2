#include "c3.h"

#include "TileVal.h"
#include "CityData.h"
#include "Unit.h"
#include "UnitData.h"
#include "MapPoint.h"
#include "UnoccupiedTiles.h"





UnoccupiedTiles::UnoccupiedTiles(void)
	{
	sint32	i ;

	m_numTilesFound = 0 ;
	m_numUnoccupiedTiles = 0 ;
	m_numBestFoodTiles = 0 ;
	m_numBestProductionTiles = 0 ;
	m_numBestGoldTiles = 0 ;
	m_numBestTradeTiles = 0 ;
	for (i=0; i<CITY_TILE_SIZE; i++)
		{
		m_unordered[i] = NULL ;
		m_bestFoodTiles[i] = -1 ;
		m_bestProductionTiles[i] = -1 ;
		m_bestGoldTiles[i] = -1 ;
		}

	}


UnoccupiedTiles::~UnoccupiedTiles(void)
	{
	ClearList() ;
	}










void UnoccupiedTiles::ClearList(void)
	{	
	sint32	i ;

	
	for (i=0; i<CITY_TILE_SIZE; i++)
		{
		if (m_unordered[i])
			{
			
			delete m_unordered[i] ;
			m_unordered[i] = NULL ;
			}

		m_bestFoodTiles[i] = -1 ;
		m_bestProductionTiles[i] = -1 ;
		m_bestGoldTiles[i] = -1 ;
		}

	

	
	m_numUnoccupiedTiles = 0 ;

	
	m_numTilesFound = 0 ;

	
	m_numBestFoodTiles = 0 ;
	
	
	m_numBestProductionTiles = 0 ;
	
	
	m_numBestGoldTiles = 0 ;

	
	m_numBestTradeTiles = 0 ;
	}



















void UnoccupiedTiles::InsertTile(const EmptyTile &tile)
	{
	sint32	idx ;

	
	idx = InsertTileIntoUnorderedList(tile) ;

	
	InsertTileIntoBestFoodList(idx) ;

	
	InsertTileIntoBestProductionList(idx) ;

	
	InsertTileIntoBestGoldList(idx) ;


	}
















sint32 UnoccupiedTiles::InsertTileIntoUnorderedList(const EmptyTile &tile)
	{
	sint32	m_insertPos ;

	
	m_insertPos = m_numUnoccupiedTiles ;

	
	m_numUnoccupiedTiles++ ;

	
	m_numTilesFound++ ;

	
	m_unordered[m_insertPos] = new EmptyTile(tile) ;

	return (m_insertPos) ;
	}




















BOOL UnoccupiedTiles::CompareTiles(sint32 *list, const sint32 first, const sint32 second, const sint32 dominant)
	{
	sint32	a1, b1, c1,
			a2, b2, c2;

	Assert((dominant==DOMINANT_FOOD) || (dominant==DOMINANT_PRODUCTION) ||  (dominant==DOMINANT_GOLD)) ;

	
	ObtainItemValues(list[first], dominant, a1, b1, c1) ;
	
	ObtainItemValues(list[second], dominant, a2, b2, c2) ;
	
	if (a1 > a2)
		{
		
		return (TRUE) ;
		}
	
	else if (a1 < a2)
		{
		
		return (FALSE) ;
		}
	
	
	if (b1 > b2)
		{
		
		return (TRUE) ;
		}
	
	else if (b1 < b2)
		{
		
		return (FALSE) ;
		}
	
	
	if (c1 > c2)
		{
		
		return (TRUE) ;
		}
	
	else if (c1 < c2)
		{
		
		return (FALSE) ;
		}
	
	
	return (FALSE) ;
}
	










void UnoccupiedTiles::ObtainItemValues(const sint32 idx, const sint32 dominant, sint32 &a, sint32 &b, sint32 &c)
{
	Assert((dominant==DOMINANT_FOOD) || (dominant==DOMINANT_PRODUCTION) || (dominant==DOMINANT_GOLD)) ;
	Assert(idx>=0) ;
	Assert(idx<m_numTilesFound) ;
	Assert(m_unordered[idx]!=NULL) ;
	
    switch (dominant) { 
    case DOMINANT_FOOD:
		{
		
		a = m_unordered[idx]->GetFood() ;
		
		b = m_unordered[idx]->GetProduction() ;
	
		
		c = m_unordered[idx]->GetGold() ;
        break; 
		}
	
    case DOMINANT_PRODUCTION:
		{
		
		a = m_unordered[idx]->GetProduction() ;
		
		b = m_unordered[idx]->GetFood() ;
		
		c = m_unordered[idx]->GetGold() ;
		}
        break;
    case DOMINANT_GOLD:
		{
		
		a = m_unordered[idx]->GetGold() ;
		
		b = m_unordered[idx]->GetFood() ;
		
		c = m_unordered[idx]->GetProduction() ;
		}
        break; 
    default:
        Assert(0); 
	}
}













void UnoccupiedTiles::SortList(sint32 *list, sint32 numItems, sint32 dominant)
	{
	sint32	comparePos ;

	BOOL	swap,
			optimum ;

	
	Assert(numItems>0) ;
	
	Assert(list) ;
	Assert((list==m_bestFoodTiles) || (list==m_bestProductionTiles) || (list==m_bestGoldTiles)) ;
	
	Assert((dominant==DOMINANT_FOOD) || (dominant==DOMINANT_PRODUCTION) || (dominant==DOMINANT_GOLD)) ;
	
	if (numItems <= 1)
		{
		
		return ;
		}

	
	optimum = FALSE ;
	
	comparePos = numItems - 1 ;
	
	while (!optimum)
		{
		
		if (comparePos == 0)
			{
			
			break ;
			}

		
		Assert(comparePos>=1) ;
		
		swap = CompareTiles(list, comparePos, comparePos - 1, dominant) ;
		
		if (swap)
			{
			
			SwitchTiles(list, numItems, comparePos, comparePos - 1) ;
			
			comparePos-- ;
			}
		
		else
			{
			
			optimum = TRUE ;
			}

		}

	}













void UnoccupiedTiles::SwitchTiles(sint32 *list, sint32 numItems, const sint32 idxA, const sint32 idxB)
	{
	sint32	temp ;

	
	if (numItems <= 1)
		{
		
		
#pragma warning( disable : 4127)									
		Assert(FALSE) ;
#pragma warning( default : 4127)									
		return ;
		}

	
	if ((idxA < 0) || (idxA >= numItems))
		{
		
		
#pragma warning( disable : 4127)									
		Assert(FALSE) ;
#pragma warning( default : 4127)									
		return ;
		}

	
	if ((idxB < 0) || (idxB >= numItems))
		{
		
		
#pragma warning( disable : 4127)									
		Assert(FALSE) ;
#pragma warning( default : 4127)									
		return ;
		}

	
	if (idxA == idxB)
		{
		
		
#pragma warning( disable : 4127)									
		Assert(FALSE) ;
#pragma warning( default : 4127)									
		return ;
		}
	
	
	temp = list[idxA] ;
	
	list[idxA] = list[idxB] ;
	
	list[idxB] = temp ;
	}













void UnoccupiedTiles::InsertTileIntoBestFoodList(const sint32 idx)
	{
	sint32	insertPos ;

	
	if (m_unordered[idx]->GetFood() > 0)
		{
		
		
		insertPos = m_numBestFoodTiles ;
		
		m_bestFoodTiles[insertPos] = idx ;
		m_numBestFoodTiles++ ;
		
		SortList(m_bestFoodTiles, m_numBestFoodTiles, DOMINANT_FOOD) ;
		}

	}








void UnoccupiedTiles::InsertTileIntoBestProductionList(const sint32 idx)
	{
	sint32	insertPos ;

	
	if (m_unordered[idx]->GetProduction() > 0)
		{
		
		
		insertPos = m_numBestProductionTiles ;
		
		m_bestProductionTiles[insertPos] = idx ;
		m_numBestProductionTiles++ ;
		
		SortList(m_bestProductionTiles, m_numBestProductionTiles, DOMINANT_PRODUCTION) ;
		}

	}








void UnoccupiedTiles::InsertTileIntoBestGoldList(const sint32 idx)
	{
	sint32	insertPos ;

	
	if (m_unordered[idx]->GetGold() > 0)
		{
		
		
		insertPos = m_numBestGoldTiles ;
		
		m_bestGoldTiles[insertPos] = idx ;
		m_numBestGoldTiles++ ;
		
		SortList(m_bestGoldTiles, m_numBestGoldTiles, DOMINANT_GOLD) ;
		}

	}













BOOL UnoccupiedTiles::RemoveTile(const sint32 tileIdx, EmptyTile &tile)
	{
	sint32	i,
			j ;

	BOOL	foundTile ;

	
	Assert(tileIdx < m_numTilesFound) ;

	
		
	
	if (tileIdx > m_numTilesFound)
		{
		
		return (FALSE) ;
		}

	
	tile = m_unordered[tileIdx] ;

	
	delete m_unordered[tileIdx] ;
	m_unordered[tileIdx] = NULL ;

	
	m_numUnoccupiedTiles-- ;

	
	foundTile = FALSE ;
	for (i=0; i<m_numBestFoodTiles && !foundTile; i++)
		{
		if (m_bestFoodTiles[i] == tileIdx)
			foundTile = TRUE ;

		}

	
	if (foundTile)
		{
		
		for (j=i-1; j<m_numBestFoodTiles; j++)
			m_bestFoodTiles[j] = m_bestFoodTiles[j + 1] ;

		m_bestFoodTiles[m_numBestFoodTiles - 1] = -1 ;

		
		m_numBestFoodTiles-- ;
		}

	
	foundTile = FALSE ;
	for (i=0; i<m_numBestProductionTiles && !foundTile; i++)
		{
		if (m_bestProductionTiles[i] == tileIdx)
			foundTile = TRUE ;

		}

	
	if (foundTile)
		{
		
		for (j=i-1; j<m_numBestProductionTiles; j++)
			m_bestProductionTiles[j] = m_bestProductionTiles[j + 1] ;

		m_bestProductionTiles[m_numBestProductionTiles - 1] = -1 ;

		
		m_numBestProductionTiles-- ;
		}

	
	foundTile = FALSE ;
	for (i=0; i<m_numBestGoldTiles && !foundTile; i++)
		{
		if (m_bestGoldTiles[i] == tileIdx)
			foundTile = TRUE ;

		}

	
	if (foundTile)
		{
		
		for (j=i-1; j<m_numBestFoodTiles; j++)
			m_bestGoldTiles[j] = m_bestGoldTiles[j + 1] ;

		m_bestGoldTiles[m_numBestGoldTiles - 1] = -1 ;

		
		m_numBestGoldTiles-- ;
		}




	return (TRUE) ;
	}












BOOL UnoccupiedTiles::GetTile(const sint32 tileIdx, EmptyTile &tile)
	{
	
	Assert(tileIdx<m_numTilesFound) ;

	
		
	
	if (tileIdx >= m_numTilesFound)
		{
		
		return (FALSE) ;
		}

	
	if (!m_unordered[tileIdx]) return FALSE;

	tile = m_unordered[tileIdx] ;

	return (TRUE) ;
	}

	








sint32 UnoccupiedTiles::GetNumUnoccupiedTiles(void)
	{
	
	return (m_numUnoccupiedTiles) ;
	}
















BOOL UnoccupiedTiles::GetBestFoodTile(sint32 &idx)
	{
	
	if (m_numBestFoodTiles == 0)
		{
		
		idx = -1 ;

		
		return (FALSE) ;
		}

	
	idx = m_bestFoodTiles[0] ;

	
	return (TRUE) ;
	}		















BOOL UnoccupiedTiles::GetBestProductionTile(sint32 &idx)
	{
	
	if (m_numBestProductionTiles == 0)
		{
		
		idx = -1 ;

		
		return (FALSE) ;
		}

	
	idx = m_bestProductionTiles[0] ;

	
	return (TRUE) ;
	}
















BOOL UnoccupiedTiles::GetBestGoldTile(sint32 &idx)
	{
	
	if (m_numBestGoldTiles == 0)
		{
		
		idx = - 1 ;

		
		return (FALSE) ;
		}

	
	idx = m_bestGoldTiles[0] ;

	
	return (TRUE) ;
	}










void UnoccupiedTiles::BuildListOfCityTiles(const Unit city)
	{
	TileValue	openTile[CITY_TILE_SIZE] ;

	EmptyTile	*tile ;
	
	sint32	tiles,
			i ;

	
	ClearList() ;

	
	city.GetPos(m_pos) ;

	

	city.GetData()->GetCityData()->GetOpenTileValues(m_pos, tiles, openTile) ;				

	
	for (i=0; i<tiles; i++)		{
		
		tile = new EmptyTile(openTile[i].pos, openTile[i].food, openTile[i].production, openTile[i].gold) ;
		
		if (tile != NULL)
			{
			
			InsertTile(*tile) ;
			}

		}

	}
