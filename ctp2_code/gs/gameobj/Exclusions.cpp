//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Lists of things that may not be built.
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Cut and paste errors corrected.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Exclusions.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "civarchive.h"
#include "UnitRecord.h"

Exclusions *g_exclusions = NULL;

Exclusions::Exclusions()
{
	m_numUnits = g_theUnitDB->NumRecords();
	m_numBuildings = g_theBuildingDB->NumRecords();
	m_numWonders = g_theWonderDB->NumRecords();

	m_units = new sint32[g_theUnitDB->NumRecords()];
	memset(m_units, 0, sizeof(sint32) * m_numUnits);
	m_buildings = new sint32[g_theBuildingDB->NumRecords()];
	memset(m_buildings, 0, sizeof(sint32) * m_numBuildings);
	m_wonders = new sint32[g_theWonderDB->NumRecords()];
	memset(m_wonders, 0, sizeof(sint32) * m_numWonders);
}

Exclusions::~Exclusions()
{
	delete [] m_units;
	delete [] m_buildings;
	delete [] m_wonders;
}

void Exclusions::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive << m_numUnits;
		archive << m_numBuildings;
		archive << m_numWonders;
		archive.Store((uint8 *)m_units, m_numUnits * sizeof(sint32));
		archive.Store((uint8 *)m_buildings, m_numBuildings * sizeof(sint32));
		archive.Store((uint8 *)m_wonders, m_numWonders * sizeof(sint32));
	} else {
		archive >> m_numUnits;
		archive >> m_numBuildings;
		archive >> m_numWonders;

		m_units = new sint32[m_numUnits];
		m_buildings = new sint32[m_numBuildings];
		m_wonders = new sint32[m_numWonders];

		archive.Load((uint8 *)m_units, m_numUnits * sizeof(sint32));
		archive.Load((uint8 *)m_buildings, m_numBuildings * sizeof(sint32));
		archive.Load((uint8 *)m_wonders, m_numWonders * sizeof(sint32));

		if(m_numUnits != g_theUnitDB->NumRecords()) {
			sint32 *newUnits = new sint32[g_theUnitDB->NumRecords()];
			memset(newUnits, 0, g_theUnitDB->NumRecords() * sizeof(sint32));
			memcpy(newUnits, m_units, min(m_numUnits, g_theUnitDB->NumRecords()) * sizeof(sint32));
			delete [] m_units;
			m_units = newUnits;
			m_numUnits = g_theUnitDB->NumRecords();
		}
		if(m_numBuildings != g_theBuildingDB->NumRecords()) {
			sint32 *newBuildings = new sint32[g_theBuildingDB->NumRecords()];
			memset(newBuildings, 0, g_theBuildingDB->NumRecords() * sizeof(sint32));
			memcpy(newBuildings, m_buildings, min(m_numBuildings, g_theBuildingDB->NumRecords()) * sizeof(sint32));
			delete [] m_buildings;
			m_buildings = newBuildings;
			m_numBuildings = g_theBuildingDB->NumRecords();
		}
		if(m_numWonders != g_theWonderDB->NumRecords()) {
			sint32 *newWonders = new sint32[g_theWonderDB->NumRecords()];
			memset(newWonders, 0, g_theWonderDB->NumRecords() * sizeof(sint32));
			memcpy(newWonders, m_wonders, min(m_numWonders, g_theWonderDB->NumRecords()) * sizeof(sint32));
			delete [] m_wonders;
			m_wonders = newWonders;
			m_numWonders = g_theWonderDB->NumRecords();
		}
	}
}
