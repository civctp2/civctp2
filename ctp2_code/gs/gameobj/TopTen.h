#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TOPTEN_H__
#define __TOPTEN_H__

#include "Unit.h"

class CivArchive ;

typedef struct tagTopEntry
	{
	Unit	unit ;

	sint32	value ;

	} TopEntry ;

enum
	{
	TOPTENTYPE_FIRST,
	TOPTENTYPE_BIGGEST_CITY = TOPTENTYPE_FIRST,
	TOPTENTYPE_HAPPIEST_CITY,
	TOPTENTYPE_MAX,
	} ;

#define TOPTEN_LIST_SIZE	10

class TopTen
	{
private :








	TopEntry m_biggestCities[TOPTEN_LIST_SIZE] ;

	TopEntry m_happiestCities[TOPTEN_LIST_SIZE] ;




	BOOL InsertCity(TopEntry *cityList, const Unit &c, const sint32 value, sint32 &pos) ;

public:

	TopTen() ;
	TopTen(CivArchive &archive) ;
	~TopTen() ;

	void Clear();
	void Serialize(CivArchive &archive) ;
	void CalculateBiggestCities(void) ;
	BOOL FindCity(const Unit &c, TopEntry *list, sint32 &pos) ;
	void EndTurn(void) ;
	BOOL IsTopTenCity(const Unit &c, const sint32 category, sint32 &pos) ;
	BOOL GetCityPosition(const Unit &c, const sint32 category, sint32 &pos) ;
	void InsertCity(const Unit &c) ;
	Unit GetBiggestCity(const sint32 idx) { Assert((idx>=0) && (idx<TOPTEN_LIST_SIZE)) ; return (m_biggestCities[idx].unit) ; }
	Unit GetHappiestCity(const sint32 idx) { Assert((idx>=0) && (idx<TOPTEN_LIST_SIZE)) ; return (m_happiestCities[idx].unit) ; }
	} ;

#else

class TopTen ;

#endif
