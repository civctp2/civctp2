#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3TerrDB_H__
#define __C3TerrDB_H__ 1

#include "ic3TerrDB.h"

class Player;
class CivArchive;

class C3TerrainDB : public IC3TerrainDB
{
	ULONG m_refCount;

    Player *m_player;

public:

	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

    C3TerrainDB(Player *p);
    C3TerrainDB(Player *p, CivArchive &archive);
    void Serialize(CivArchive &archive);

	STDMETHODIMP_ (sint32) GetNumTerrainTypes() ;
    STDMETHODIMP_ (sint32) GetGoodGoldValue(sint32 type_good);
    STDMETHODIMP_ (sint32) GetNumTypeGoods();
	STDMETHODIMP_ (double) GetMoveCost(const sint8 terrain_type);

};

#endif __C3TerrDB_H__
