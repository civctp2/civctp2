








#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _INSTALLATION_DB_H_
#define _INSTALLATION_DB_H_

#include "DB.h"
#include "InstRec.h"
#include "c3files.h"

class CivArchive;
class Installation;
class Token;

class InstallationDatabase : public Database <InstallationRecord>
{
public:
	InstallationDatabase();
	InstallationDatabase(CivArchive &archive);

	
	double m_maxVisionRange;
	

	

	BOOL Initialize(char *filename, C3DIR dir);
	void Serialize(CivArchive &archive);

	double GetMaxVisionRange();
	uint32 GetVisibilityClasses(sint32 type);
	uint32 GetVisibilityAbilities(sint32 type);
	sint32 GetMaterialsToBuild(sint32 type, TERRAIN_TYPES terrain);
	double GetVisionRange(sint32 type);
	double GetRadarRange(sint32 type);
	sint32 GetTimeToBuild(sint32 type, TERRAIN_TYPES terrain);
	sint32 GetAttack(sint32 type);
	sint32 GetFirepower(sint32 type);
	double GetDefenseBonus(sint32 type);
	BOOL IsAirfield(sint32 type);
	BOOL IsFort(sint32 type);

	BOOL IsListeningPost(sint32 type);
	BOOL IsRadar(sint32 type);
	BOOL IsHealUnits(sint32 type) const;
	uint32 GetTerrain(sint32 type);

	BOOL LandInstallation(sint32 type);
	BOOL WaterInstallation(sint32 type);
	BOOL SpaceInstallation(sint32 type);

	sint32 EnableInstallation(sint32 type) const;

	sint32 GetBuildSoundID(sint32 type);

private:
	BOOL ParseANumber(Token *token, sint32 &val);
	BOOL ParseANumber(Token *token, double &val);
	BOOL ParseAdvance(Token *token, sint32 &val);

	BOOL ParseInstallationDB(char *filename, C3DIR dir);
	BOOL ParseAnInstallation(Token *token);
	BOOL ParseAnAttribute(Token *token, InstallationRecord *rec);

	BOOL m_abort_parse;
};

extern InstallationDatabase *g_theInstallationDB;

#endif
