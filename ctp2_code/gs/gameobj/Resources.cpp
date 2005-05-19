#include "c3.h"
#include "Resources.h"
#include "civarchive.h"
#include "ResourceRecord.h"

Resources::Resources()
{ 
	m_numGoods = g_theResourceDB->NumRecords();
	m_supply = new sint32[m_numGoods];
	Clear(); 
}

Resources::Resources(const Resources &copyme)
{
	m_numGoods = copyme.m_numGoods;
	m_totalResources = copyme.m_totalResources;
	sint32 i;
	m_supply = new sint32[m_numGoods];
	for(i = 0; i < m_numGoods; i++) {
		m_supply[i] = copyme.m_supply[i];
	}
}	

void Resources::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive << m_numGoods;
		archive.Store((uint8*)m_supply, m_numGoods * sizeof(sint32));
		archive << m_totalResources;
	} else {
		archive >> m_numGoods;
		delete [] m_supply;
		m_supply = new sint32[m_numGoods];
		archive.Load((uint8*)m_supply, m_numGoods * sizeof(sint32));
		archive >> m_totalResources;
	}
}

Resources & Resources::operator = (Resources &copyme)
{
	Assert(m_numGoods == copyme.m_numGoods);
	m_numGoods = copyme.m_numGoods;
	m_totalResources = copyme.m_totalResources;
	sint32 i;
	for(i = 0; i < m_numGoods; i++) {
		m_supply[i] = copyme.m_supply[i];
	}
	return *this;
}

