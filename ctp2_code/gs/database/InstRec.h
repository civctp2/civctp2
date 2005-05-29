#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _INSTALLATION_RECORD_H_
#define _INSTALLATION_RECORD_H_

#include "Rec.h"


#include "gstypes.h"

class CivArchive;

#define k_BIT_INSTALLATION_IS_AIRFIELD        0x00000001
#define k_BIT_INSTALLATION_IS_FORT            0x00000002
#define k_BIT_INSTALLATION_IS_LAND_MINE       0x00000004
#define k_BIT_INSTALLATION_IS_LISTENING_POST  0x00000008
#define k_BIT_INSTALLATION_IS_RADAR           0x00000010
#define k_BIT_INSTALLATION_IS_HEAL_UNITS      0x00000020

#define k_BIT_INSTALLATION_TERRAIN_SPACE      0x20000000
#define k_BIT_INSTALLATION_TERRAIN_WATER      0x40000000
#define k_BIT_INSTALLATION_TERRAIN_LAND       0x80000000
#define k_MASK_INSTALLATION_TERRAIN           0xe0000000

class InstallationRecord : public Record {
public:
	sint32 m_productionCost;
	sint32 m_productionTime;
	double m_visionRange;
	double m_radarRange;
	sint32 m_attack;
	double m_defenseBonus;
	sint32 m_firepower;
	uint32 m_flags;
	sint32 m_buildSoundID;	

	sint32 m_terrainMaterialCost[TERRAIN_MAX];
	sint32 m_terrainTimeCost[TERRAIN_MAX];

	uint32 m_visibilityClasses;

	
	uint32 m_visibilityAbilities;

	InstallationRecord();
	InstallationRecord(CivArchive &archive);

	void Serialize(CivArchive &archive);

	BOOL IsAirfield() const { return (m_flags & k_BIT_INSTALLATION_IS_AIRFIELD) != 0; }
};

#endif
