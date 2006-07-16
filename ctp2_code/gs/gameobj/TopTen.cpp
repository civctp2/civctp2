











#include "c3.h"
#include "TopTen.h"

#include "c3debug.h"
#include "civarchive.h"
#include "gamefile.h"
#include "Globals.h"
#include "player.h"         // g_player
#include "UnitData.h"
#include "World.h"

TopTen::TopTen()
{
	Clear();
}


void TopTen::Clear()
{
	sint32	i ;

	for (i=0; i<TOPTEN_LIST_SIZE; i++) {
		m_biggestCities[i].unit.m_id = (0) ;
		m_biggestCities[i].value = 0 ;
		
		m_happiestCities[i].unit.m_id = (0) ;
		m_happiestCities[i].value = 0 ;
	}
}












TopTen::TopTen(CivArchive &archive)
{
	Serialize(archive) ;
}










TopTen::~TopTen()
{
}












void TopTen::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

#define TOPTEN_MAGIC 0x32657099
	if (archive.IsStoring())
	{
	}
	else
	{
		if (g_saveFileVersion < 55) 
        {
			archive.TestMagic(TOPTEN_MAGIC) ;
			for (sint32 i = 0; i < TOPTEN_LIST_SIZE; i++)
			{
				m_biggestCities[i].unit.Serialize(archive) ;
				archive>>m_biggestCities[i].value ;
				m_happiestCities[i].unit.Serialize(archive) ;
				archive>>m_happiestCities[i].value ;
			}
		}
	}
}


BOOL TopTen::InsertCity(TopEntry *cityList, const Unit &c, const sint32 value, sint32 &pos)
{
	Assert(cityList) ;
	Assert(c.IsValid());
	sint32 i;

	for (i = 0; i < TOPTEN_LIST_SIZE; i++) 
    {
		if (cityList[i].unit == c) 
        {
			memmove(&cityList[i], &cityList[i+1], sizeof(TopEntry) * (TOPTEN_LIST_SIZE - 1 - i));
			cityList[TOPTEN_LIST_SIZE - 1].value = 0;
			cityList[TOPTEN_LIST_SIZE - 1].unit.m_id = 0;
			break;
		}
	}
	
	sint32 insertPos = -1 ;
	for (i = 0; i < TOPTEN_LIST_SIZE; i++)
	{
		if (value > cityList[i].value)
		{
			insertPos = i;
			break;
		}
	}

	
	if (insertPos == -1)
	{
		pos = 0 ;
		return FALSE;
	}

	memmove(&cityList[insertPos+1], &cityList[insertPos], sizeof(TopEntry) * (TOPTEN_LIST_SIZE - 1 - insertPos)) ;
	cityList[insertPos].value = value ;
	cityList[insertPos].unit = c ;
	pos = insertPos ;
	return TRUE;
}


void TopTen::InsertCity(const Unit &c)
{
	sint32	pos;

	InsertCity(m_biggestCities, c, c.PopCount(), pos) ;
	InsertCity(m_happiestCities, c, (sint32)(c.GetHappiness()), pos) ;
}


void TopTen::CalculateBiggestCities(void)
{
	Clear();

	for (sint32 plyr = 0; plyr < k_MAX_PLAYERS; plyr++)
	{
		if (g_player[plyr] && !g_player[plyr]->IsDead())
		{
			sint32 numCities = g_player[plyr]->GetAllCitiesList()->Num();
			for (sint32 city=0; city<numCities; city++)
			{
				InsertCity(g_player[plyr]->GetAllCitiesList()->Get(city));
			}
		}
	}
}


BOOL TopTen::FindCity(const Unit &c, TopEntry * a_List, sint32 &pos)
{
	for (sint32 i = 0; i < TOPTEN_LIST_SIZE; i++)
    {
		if (a_List[i].unit == c)
		{
			pos = i ;
			return TRUE;
		}
    }

	pos = 0;
	return FALSE;
}


BOOL TopTen::IsTopTenCity(const Unit &c, const sint32 category, sint32 &pos)
{
	switch (category)
	{
	case TOPTENTYPE_BIGGEST_CITY:
		return (FindCity(c, m_biggestCities, pos));

	case TOPTENTYPE_HAPPIEST_CITY:
		return (FindCity(c, m_happiestCities, pos));

	default:
		Assert(FALSE);
		return FALSE;

	}
}


BOOL TopTen::GetCityPosition(const Unit &c, const sint32 category, sint32 &pos)
{
	switch (category)
	{
	case TOPTENTYPE_BIGGEST_CITY:
		return (FindCity(c, m_biggestCities, pos));

	case TOPTENTYPE_HAPPIEST_CITY:
		return (FindCity(c, m_happiestCities, pos));

	default:
		Assert(FALSE);
		return FALSE;
	}
}


void TopTen::EndTurn(void)
{
	CalculateBiggestCities();
}
