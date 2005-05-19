

#include "c3.h"
#include "globals.h"
#include "civarchive.h"

#include "TerrainRecord.h"


#include "C3GameState.h"
#include "C3TerrDB.h"
#include "TerrImproveData.h"
#include "ResourceRecord.h"


STDMETHODIMP C3TerrainDB::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3TerrainDB)) {
		*obj = (IC3TerrainDB*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) C3TerrainDB::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) C3TerrainDB::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}


C3TerrainDB::C3TerrainDB(Player *p)
{
  	m_refCount = 0;
    m_player = p;
} 

C3TerrainDB::C3TerrainDB(Player *p, CivArchive &archive)
{
    m_player = p;
    Serialize(archive); 
} 

void C3TerrainDB::Serialize(CivArchive &archive) 
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive << m_refCount; 
    } else { 
        archive >> m_refCount; 
    } 
}

sint32 C3TerrainDB::GetGoodGoldValue(sint32 type_good)
{
	return g_theResourceDB->Get(type_good)->GetGold();
#if 0
    sint32 tt; 
    sint32 tg; 

    tt = type_good % k_BASE_TERRAIN_TYPES;
    tg = (type_good -tt) / k_BASE_TERRAIN_TYPES;

    if (g_theTerrainDB->Get(tt)->HasGood(tg)) { 
        return g_theTerrainDB->Get(tt)->GetGood(tg)->GetGoodGoldValue();	
    } else { 
        return 0; 
    }
#endif
} 

sint32 C3TerrainDB::GetNumTypeGoods()
{
	return g_theResourceDB->NumRecords();
}



sint32 C3TerrainDB::GetNumTerrainTypes()
{
	return (g_theTerrainDB->NumRecords());
}


double  C3TerrainDB::GetMoveCost(const sint8 terrain_type)
{
	return (g_theTerrainDB->Get(terrain_type)->GetEnvBase()->GetMovement()) ;
}
