#include "c3.h"
#include "net_exclusions.h"
#include "Exclusions.h"
#include "net_util.h"

void NetExclusions::Packetize(uint8 *buf, uint16 &size)
{
	PUSHID(k_PACKET_EXCLUSIONS_ID);
	PUSHLONG(g_exclusions->m_numUnits);
	PUSHLONG(g_exclusions->m_numBuildings);
	PUSHLONG(g_exclusions->m_numWonders);

	sint32 i;
	sint32 bitPos;
	uint8 next;
	for(i = 0; i < g_exclusions->m_numUnits; i += 8) {
		next = 0;
		for(bitPos = 0; (bitPos < 8) && ((i + bitPos) < g_exclusions->m_numUnits); bitPos++) {
			if(g_exclusions->IsUnitExcluded(i + bitPos))
				next |= 1 << bitPos;
		}
		PUSHBYTE(next);
	}

	for(i = 0; i < g_exclusions->m_numBuildings; i += 8) {
		next = 0;
		for(bitPos = 0; (bitPos < 8) && ((i + bitPos) < g_exclusions->m_numBuildings); bitPos++) {
			if(g_exclusions->IsBuildingExcluded(i + bitPos))
				next |= 1 << bitPos;
		}
		PUSHBYTE(next);
	}

	for(i = 0; i < g_exclusions->m_numWonders; i += 8) {
		next = 0;
		for(bitPos = 0; (bitPos < 8) && ((i + bitPos) < g_exclusions->m_numWonders); bitPos++) {
			if(g_exclusions->IsWonderExcluded(i + bitPos))
				next |= 1 << bitPos;
		}
		PUSHBYTE(next);
	}
}

void NetExclusions::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 packid;
	uint16 pos = 0;

	PULLID(packid);
	Assert(packid == k_PACKET_EXCLUSIONS_ID);

	if(!g_exclusions) {
		g_exclusions = new Exclusions();
	}

	PULLLONG(g_exclusions->m_numUnits);
	PULLLONG(g_exclusions->m_numBuildings);
	PULLLONG(g_exclusions->m_numWonders);

	delete [] g_exclusions->m_units;
	delete [] g_exclusions->m_buildings;
	delete [] g_exclusions->m_wonders;

	g_exclusions->m_units = new sint32[g_exclusions->m_numUnits];
	g_exclusions->m_buildings = new sint32[g_exclusions->m_numBuildings];
	g_exclusions->m_wonders = new sint32[g_exclusions->m_numWonders];
	memset(g_exclusions->m_units, 0, sizeof(sint32) * g_exclusions->m_numUnits);
	memset(g_exclusions->m_buildings, 0, sizeof(sint32) * g_exclusions->m_numBuildings);
	memset(g_exclusions->m_wonders, 0, sizeof(sint32) * g_exclusions->m_numWonders);

	sint32 i;
	sint32 bitPos;
	uint8 next;

	for(i = 0; i < g_exclusions->m_numUnits; i += 8) {
		PULLBYTE(next);
		for(bitPos = 0; (bitPos < 8) && ((i + bitPos) < g_exclusions->m_numUnits); bitPos++) {
			if(next & (1 << bitPos)) {
				g_exclusions->ExcludeUnit(i + bitPos, TRUE);
			}
		}
	}

	for(i = 0; i < g_exclusions->m_numBuildings; i += 8) {
		PULLBYTE(next);
		for(bitPos = 0; (bitPos < 8) && ((i + bitPos) < g_exclusions->m_numBuildings); bitPos++) {
			if(next & (1 << bitPos)) {
				g_exclusions->ExcludeBuilding(i + bitPos, TRUE);
			}
		}
	}

	for(i = 0; i < g_exclusions->m_numWonders; i += 8) {
		PULLBYTE(next);
		for(bitPos = 0; (bitPos < 8) && ((i + bitPos) < g_exclusions->m_numWonders); bitPos++) {
			if(next & (1 << bitPos)) {
				g_exclusions->ExcludeWonder(i + bitPos, TRUE);
			}
		}
	}
}
