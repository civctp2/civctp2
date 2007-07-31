









#include "c3.h"
#include "InstDB.h"
#include "c3errors.h"
#include "StrDB.h"
#include "CivArchive.h"
#include "Token.h"
#include "UnitRec.h"
#include "SoundRecord.h"
#include "AdvanceRecord.h"
#include "SoundRecord.h"

extern StringDB *g_theStringDB;

InstallationDatabase::InstallationDatabase()
{
    m_maxVisionRange = 0; 
}

InstallationDatabase::InstallationDatabase(CivArchive &archive)
{
	Serialize(archive);
}

void InstallationDatabase::Serialize(CivArchive &archive)
{
	sint32	i ;

    CHECKSERIALIZE

	if (archive.IsStoring()) {
		archive<<m_nRec;
		archive<<m_max_nRec;
		archive<<m_maxVisionRange ;
		for(i = 0; i < m_nRec; i++)
			m_rec[i].Serialize(archive);

	} else {
		archive>>m_nRec;
		archive>>m_max_nRec;
		archive>>m_maxVisionRange ;

		Assert(0 < m_nRec);
		m_rec = new InstallationRecord[m_max_nRec] ;
		for(i = 0; i < m_nRec; i++)
			m_rec[i].Serialize(archive) ;
			
	}

}

BOOL InstallationDatabase::Initialize(char *filename, C3DIR dir)
{
	m_abort_parse = FALSE;
	return ParseInstallationDB(filename, dir);
}

BOOL InstallationDatabase::ParseInstallationDB(char *filename, C3DIR dir)
{
	Token *token = new Token(filename, dir);
	sint32 n, count = 0;
	if(token->GetType() != TOKEN_NUMBER) {
		c3errors_ErrorDialog(token->ErrStr(), "Missing number of installations");
		return FALSE;
	}

	token->GetNumber(n);

	if(n < 0) {
		c3errors_ErrorDialog(token->ErrStr(), "Number of installations is negative");
		return FALSE;
	}
	SetSize(n);

	while(ParseAnInstallation(token)) {
		count++;
	}
	if(m_abort_parse) return FALSE;
	if(count != n) {
		c3errors_ErrorDialog(token->ErrStr(), "Incorrect number of installations");
		return FALSE;
	}
	delete token;
	return TRUE;
}

BOOL InstallationDatabase::ParseAnInstallation(Token *token)
{
	char str[k_MAX_NAME_LEN];
	StringId str_id;

	token->Next();

	if(token->GetType() == TOKEN_EOF) {
		return FALSE;
	}

	if(token->GetType() != TOKEN_STRING) {
		c3errors_ErrorDialog(token->ErrStr(), "Installation ID expected");
		m_abort_parse = TRUE;
		return FALSE;
	}

	token->GetString(str);
	if(!g_theStringDB->GetStringID(str, str_id)) {
		c3errors_ErrorDialog(token->ErrStr(), "Could not find %s in string database", str);
		m_abort_parse = TRUE;
		return FALSE;
	}

	StringId u;
	AddRec(str_id, u);

	if(token->Next() != TOKEN_OPEN_BRACE) {
		c3errors_ErrorDialog(token->ErrStr(), "Missing open brace");
		m_abort_parse = TRUE;
		return FALSE;
	}

	while(ParseAnAttribute(token, Access(u)))
		;

	if(!m_abort_parse) {
		if(Access(u)->m_visibilityClasses == 0)
			Access(u)->m_visibilityClasses = 0xffffffff;
		if(Access(u)->m_visibilityAbilities == 0)
			Access(u)->m_visibilityAbilities = 0xffffffff;
		if((Access(u)->m_flags & k_MASK_INSTALLATION_TERRAIN) == 0) {
			Access(u)->m_flags |= k_BIT_INSTALLATION_TERRAIN_LAND;
		}
	}

	return !m_abort_parse;
}

BOOL InstallationDatabase::ParseANumber(Token *token, sint32 &val)
{
	if(token->Next() != TOKEN_NUMBER) {
		c3errors_ErrorDialog(token->ErrStr(), "Value not a number");
		m_abort_parse = TRUE;
		return FALSE;
	}
	token->GetNumber(val);
	return TRUE;
}

BOOL InstallationDatabase::ParseANumber(Token *token, double &val)
{
	if(token->Next() != TOKEN_NUMBER) {
		c3errors_ErrorDialog(token->ErrStr(), "Value not a number");
		m_abort_parse = TRUE;
		return FALSE;
	}
	token->GetFloat(val);
	return TRUE;
}

BOOL InstallationDatabase::ParseAdvance(Token *token, sint32 &adv)
{
	if(token->Next() != TOKEN_STRING) {
		c3errors_ErrorDialog(token->ErrStr(), "Expected advance");
		m_abort_parse = TRUE;
		return FALSE;
	}
	char str[k_MAX_NAME_LEN];
	StringId str_id;

	token->GetString(str);
	if(!g_theStringDB->GetStringID(str, str_id)) {
		c3errors_ErrorDialog(token->ErrStr(), "%s not in string database\n", str);
		m_abort_parse = TRUE;
		return FALSE;
	}
	if(!g_theAdvanceDB->GetNamedItem(str_id, adv)) {
		c3errors_ErrorDialog(token->ErrStr(), "%s not in advance database\n", str);
		m_abort_parse = TRUE;
		return FALSE;
	}
	return TRUE;
}

BOOL InstallationDatabase::ParseAnAttribute(Token *token, 
											InstallationRecord *rec)
{
	char str[k_MAX_NAME_LEN];
	
	switch(token->Next()) {
		case TOKEN_PRODUCTION_COST:
			ParseANumber(token, rec->m_productionCost);
			break;
		case TOKEN_PRODUCTION_TIME:
			ParseANumber(token, rec->m_productionTime);
			break;
		case TOKEN_VISION_RANGE:
			ParseANumber(token, rec->m_visionRange);
			if(rec->m_visionRange > m_maxVisionRange) {
				m_maxVisionRange = rec->m_visionRange;
			}
			break;
		case TOKEN_RADAR_RANGE:
			ParseANumber(token, rec->m_radarRange);
			break;
		case TOKEN_ATTACK:
			ParseANumber(token, rec->m_attack);
			break;
		case TOKEN_DEFENSE_BONUS:
			ParseANumber(token, rec->m_defenseBonus);
			break;
		case TOKEN_FIREPOWER:
			ParseANumber(token, rec->m_firepower);
			break;

		case TOKEN_VISIBILITY_CLASS_0:
			
			break;
		case TOKEN_VISIBILITY_CLASS_1:
			
			break;
		case TOKEN_VISIBILITY_CLASS_2:
			
			break;
		case TOKEN_VISIBILITY_CLASS_3:
			
			break;
		case TOKEN_VISIBILITY_CLASS_4:
			
			break;
		case TOKEN_VISIBILITY_CLASS_5:
			
			break;
		case TOKEN_VISIBILITY_CLASS_6:
			
			break;
		case TOKEN_VISIBILITY_CLASS_7:
			
			break;
		case TOKEN_VISIBILITY_CLASS_8:
			
			break;

		case TOKEN_VISIBILITY_CAN_SEE_0:
			
			break;
		case TOKEN_VISIBILITY_CAN_SEE_1:
			
			break;
		case TOKEN_VISIBILITY_CAN_SEE_2:
			
			break;
		case TOKEN_VISIBILITY_CAN_SEE_3:
			
			break;
		case TOKEN_VISIBILITY_CAN_SEE_4:
			
			break;
		case TOKEN_VISIBILITY_CAN_SEE_5:
			
			break;
		case TOKEN_VISIBILITY_CAN_SEE_6:
			
			break;
		case TOKEN_VISIBILITY_CAN_SEE_7:
			
			break;
		case TOKEN_VISIBILITY_CAN_SEE_8:
			
			break;

		case TOKEN_IS_AIRFIELD:
			rec->m_flags |= k_BIT_INSTALLATION_IS_AIRFIELD;
			break;
		case TOKEN_IS_FORT:
			rec->m_flags |= k_BIT_INSTALLATION_IS_FORT;
			break;
		case TOKEN_IS_LAND_MINE:
			rec->m_flags |= k_BIT_INSTALLATION_IS_LAND_MINE;
			break;
		case TOKEN_IS_LISTENING_POST:
			rec->m_flags |= k_BIT_INSTALLATION_IS_LISTENING_POST;
			break;
		case TOKEN_IS_RADAR:
			rec->m_flags |= k_BIT_INSTALLATION_IS_RADAR;
			break;
		case TOKEN_IS_HEAL_UNITS:
			rec->m_flags |= k_BIT_INSTALLATION_IS_HEAL_UNITS;
			break;

		case TOKEN_INSTALLATION_LAND:
			rec->m_flags |= k_BIT_INSTALLATION_TERRAIN_LAND;
			break;
		case TOKEN_INSTALLATION_WATER:
			rec->m_flags |= k_BIT_INSTALLATION_TERRAIN_WATER;
			break;
		case TOKEN_INSTALLATION_SPACE:
			rec->m_flags |= k_BIT_INSTALLATION_TERRAIN_SPACE;
			break;

		case TOKEN_ENABLING_ADVANCE:
		{
			sint32 adv;
			if(!ParseAdvance(token, adv)) {
				m_abort_parse = TRUE;
				return FALSE;
			}
			rec->m_enable = adv;
			return TRUE;
		}
		case TOKEN_OBSOLETE_ADVANCE:
		{
			sint32 adv;
			if(!ParseAdvance(token, adv)) {
				m_abort_parse = TRUE;
				return FALSE;
			}
			rec->SetObsolete(adv, rec->GetNumObsolete());
			break;
		}

		case TOKEN_FOREST_COST:
		case TOKEN_PLAINS_COST:
		case TOKEN_TUNDRA_COST:
		case TOKEN_GLACIER_COST:
		case TOKEN_GRASSLAND_COST:
		case TOKEN_DESERT_COST:
		case TOKEN_SWAMP_COST:
		case TOKEN_JUNGLE_COST:
		case TOKEN_MOUNTAIN_COST:
		case TOKEN_HILL_COST:
		case TOKEN_SHALLOW_COST:
		case TOKEN_DEEP_COST:
		case TOKEN_RIFT_COST:
		case TOKEN_TRENCH_COST:
		case TOKEN_BEACH_COST:
		case TOKEN_SHELF_COST:
		case TOKEN_VOLCANO_COST:
		case TOKEN_SPACE_COST:
		case TOKEN_DEAD_COST:
		case TOKEN_DEAD_HILL_COST:
		case TOKEN_BROWN_HILL_COST: 
		case TOKEN_BROWN_MOUNTAIN_COST: 
		case TOKEN_WHITE_HILL_COST: 
		case TOKEN_WHITE_MOUNTAIN_COST: 
		{
			sint32 origtoken = token->GetType();
			sint32 materials, timecost;
			if(!ParseANumber(token, materials))
				return FALSE;
			if(!ParseANumber(token, timecost))
				return FALSE;

			sint32 index = origtoken - TOKEN_FOREST_COST;
			Assert(index >= 0);
			Assert(index < TERRAIN_MAX);
			if(index < 0 || index >= TERRAIN_MAX)
				return FALSE;
			rec->m_terrainMaterialCost[index] = materials;
			rec->m_terrainTimeCost[index] = timecost;
			break;
		}

		case TOKEN_BUILD_SOUND_ID:
				
				if (token->Next() != TOKEN_STRING) { 
   					c3errors_ErrorDialog  (token->ErrStr(), "ID string for TOKEN_BUILD_SOUND_ID expected"); 
					m_abort_parse = TRUE;
					return FALSE;
				}
				token->GetString(str);
				rec->m_buildSoundID = g_theSoundDB->FindTypeIndex(str); 
				if (rec->m_buildSoundID  == -1) { 
					c3errors_ErrorDialog  (token->ErrStr(), "Could not find sound %s", str); 
					m_abort_parse = TRUE;
					return FALSE;
				}
			break;

		case TOKEN_CLOSE_BRACE:
			return FALSE;

		default:
			token->GetString(str);
			c3errors_ErrorDialog(token->ErrStr(), "Unknown token %s\n", str);
			m_abort_parse = TRUE;
			return FALSE;
	}
	return TRUE;
}

double InstallationDatabase::GetMaxVisionRange()
{
	return m_maxVisionRange;
}

uint32 InstallationDatabase::GetVisibilityClasses(sint32 type)
{
	return Get(type)->m_visibilityClasses;
}

uint32 InstallationDatabase::GetVisibilityAbilities(sint32 type)
{
	return Get(type)->m_visibilityAbilities;
}

sint32 InstallationDatabase::GetMaterialsToBuild(sint32 type, TERRAIN_TYPES terrain)
{
	const InstallationRecord *rec = Get(type);
	return max(rec->m_terrainMaterialCost[terrain], rec->m_productionCost);
}

double InstallationDatabase::GetVisionRange(sint32 type)
{
	return Get(type)->m_visionRange;
}

double InstallationDatabase::GetRadarRange(sint32 type)
{
	return Get(type)->m_radarRange;
}

sint32 InstallationDatabase::GetTimeToBuild(sint32 type, TERRAIN_TYPES terrain)
{
	const InstallationRecord *rec = Get(type);
	return max(rec->m_terrainTimeCost[terrain], rec->m_productionTime);
}

sint32 InstallationDatabase::GetAttack(sint32 type)
{
	return Get(type)->m_attack;
}

sint32 InstallationDatabase::GetFirepower(sint32 type)
{
	return Get(type)->m_firepower;
}

double InstallationDatabase::GetDefenseBonus(sint32 type)
{
	return Get(type)->m_defenseBonus;
}

BOOL InstallationDatabase::IsAirfield(sint32 type)
{
	return (Get(type)->m_flags & k_BIT_INSTALLATION_IS_AIRFIELD) != 0;
}

BOOL InstallationDatabase::IsFort(sint32 type)
{
	return (Get(type)->m_flags & k_BIT_INSTALLATION_IS_FORT) != 0;
}






BOOL InstallationDatabase::IsListeningPost(sint32 type)
{
	return (Get(type)->m_flags & k_BIT_INSTALLATION_IS_LISTENING_POST) != 0;
}

BOOL InstallationDatabase::IsRadar(sint32 type)
{
	return (Get(type)->m_flags & k_BIT_INSTALLATION_IS_RADAR) != 0;
}

uint32 InstallationDatabase::GetTerrain(sint32 type)
{
	return Get(type)->m_flags & k_MASK_INSTALLATION_TERRAIN;
}

BOOL InstallationDatabase::LandInstallation(sint32 type)
{
	return (Get(type)->m_flags & k_BIT_INSTALLATION_TERRAIN_LAND) != 0;
}

BOOL InstallationDatabase::WaterInstallation(sint32 type)
{
	return (Get(type)->m_flags & k_BIT_INSTALLATION_TERRAIN_WATER) != 0;
}

BOOL InstallationDatabase::SpaceInstallation(sint32 type)
{
	return (Get(type)->m_flags & k_BIT_INSTALLATION_TERRAIN_SPACE) != 0;
}

sint32 InstallationDatabase::EnableInstallation(sint32 type) const
{
	return Get(type)->m_enable;
}

BOOL InstallationDatabase::IsHealUnits(sint32 type) const
{
	return (Get(type)->m_flags & k_BIT_INSTALLATION_IS_HEAL_UNITS);
}

sint32 InstallationDatabase::GetBuildSoundID(sint32 type) 
{
	return Get(type)->m_buildSoundID;
}
