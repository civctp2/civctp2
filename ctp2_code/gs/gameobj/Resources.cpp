//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : City ressource lists
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added Resize method for loading of savegames with different 
//   number of goods than in the database. - May 28th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

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

//----------------------------------------------------------------------------
//
// Name       : Resources::Serialize
//
// Description: Store/Load CityData
//
// Parameters : CivArchive &archive: The archive from/to date should be
//              read/written.
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
//
// Name       : Resources::Resize
//
// Description: Resizes the underlying array m_supply by generating a new one 
//              and copying the data from the old one.
//
// Parameters : newSize: The new size of m_supply
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Added for valid check so that savegames with modified 
//              ressource database can be loaded.
//
//----------------------------------------------------------------------------
void Resources::Resize(sint32 newSize)
{
	sint32* oldSupply = m_supply;
	sint32  oldSize = (m_numGoods < newSize) ? m_numGoods : newSize;

	m_numGoods = newSize;
	m_supply = new sint32[newSize];
	memset(m_supply, 0, m_numGoods * sizeof(sint32));
	memcpy(m_supply, oldSupply, oldSize * sizeof(sint32));

	delete [] oldSupply;
}
