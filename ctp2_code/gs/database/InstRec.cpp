









#include "c3.h"
#include "c3errors.h"
#include "InstRec.h"
#include "CivArchive.h"
#include "Token.h"
#include "DB.h"

InstallationRecord::InstallationRecord()
{
	memset((uint8*)&m_productionCost, 0, (uint8*)&m_visibilityAbilities - (uint8*)&m_productionCost + sizeof(m_visibilityAbilities));

	m_productionCost = m_attack = m_firepower = 0;
	m_productionTime = 0;
	m_visionRange = 0.0;
	m_radarRange = 0.0;
	m_defenseBonus = 0.0;
	m_visibilityClasses = 0;
	m_visibilityAbilities = 0;
	m_flags = 0;
	m_buildSoundID;
	m_enable = -1;
	m_obsolete[0] = -1;
	m_nObsolete = 0;

	sint32 i;
	for(i = 0; i < TERRAIN_MAX; i++) {
		m_terrainMaterialCost[i] = 0;
		m_terrainTimeCost[i] = 0;
	}
}

InstallationRecord::InstallationRecord(CivArchive &archive)
{
	Serialize(archive);
}

void InstallationRecord::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		Record::Serialize(archive);
		archive << m_productionCost;
		archive << m_productionTime;
		archive.PutDoubleString( m_visionRange);
		archive.PutDoubleString( m_radarRange);
		archive << m_attack;
		archive.PutDoubleString( m_defenseBonus);
		archive << m_firepower;
		archive << m_flags;
		archive << m_buildSoundID;	
		
		archive.StoreArray(m_terrainMaterialCost, TERRAIN_MAX);
		archive.StoreArray(m_terrainTimeCost, TERRAIN_MAX);
		
		archive << m_visibilityClasses;
		
		
		archive << m_visibilityAbilities;
	} else {
	}
}	
