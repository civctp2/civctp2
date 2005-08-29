//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Data check utility
// Id           : $Id$
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
// - Replced old civilisation database by new one. (Aug 20th 2005 Martin Gühmann)
// - Fixed the BeginTurn, DumpChecksum and DisplayCRC methods. (Aug 25th 2005 Martin Gühmann)
// - Added the risk database. (Aug 29th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"                        // Precompiled header
#include "DataCheck.h"
#include "primitives.h"
#include "civarchive.h"
#include "Checksum.h"

#include "RandGen.h"

// Database includes
#include "StrDB.h"
#include "profileDB.h"

#include "AdvanceRecord.h"
#include "AdvanceBranchRecord.h"
#include "AdvanceListRecord.h"
#include "AgeRecord.h"
#include "AgeCityStyleRecord.h"
#include "BuildingRecord.h"
#include "BuildingBuildListRecord.h"
#include "BuildListSequenceRecord.h"
#include "CitySizeRecord.h"
#include "CityStyleRecord.h"
#include "CivilisationRecord.h"
#include "ConstDB.h"                   // Old database
#include "DiffDB.h"                    // Old database
#include "DiplomacyRecord.h"
#include "DiplomacyProposalRecord.h"
#include "DiplomacyThreatRecord.h"
#include "EndGameObjectRecord.h"
#include "FeatRecord.h"
#include "gwdb.h"                      // Global warming database
#include "GoalRecord.h"
#include "GovernmentRecord.h"
#include "IconRecord.h"
#include "ImprovementListRecord.h"
#include "OrderRecord.h"
#include "UVDB.h"                      // Ozone database
#include "PersonalityRecord.h"
#include "PollutionDB.h"               // Old database
#include "PopRecord.h"
#include "ResourceRecord.h"
#include "RiskRecord.h"
#include "SoundRecord.h"
#include "SpecialAttackInfoRecord.h"
#include "SpecialEffectRecord.h"
#include "TerrainRecord.h"
#include "SpriteRecord.h"
#include "StrategyRecord.h"
#include "TerrainRecord.h"
#include "TerrainImprovementRecord.h"
#include "UnitRecord.h"
#include "UnitBuildListRecord.h"
#include "WonderRecord.h"
#include "WonderBuildListRecord.h"

// Pool includes
#include "AgreementPool.h"
#include "CivilisationPool.h"
#include "DiplomaticRequestPool.h"
#include "MessagePool.h"
#include "TerrImprovePool.h"
#include "TradePool.h"
#include "TradeOfferPool.h"
#include "UnitPool.h"

// Other includes
#include "pollution.h"
#include "SelItem.h"
#include "TopTen.h"
#include "World.h"
#include "player.h"


extern  RandomGenerator         *g_rand; 

// The Databases
extern  ProfileDB               *g_theProfileDB;
extern  StringDB                *g_theStringDB;
extern  ConstDB                 *g_theConstDB;
extern  GlobalWarmingDatabase   *g_theGWDB;
extern  DifficultyDB            *g_theDifficultyDB;
extern  OzoneDatabase           *g_theUVDB;
extern  PollutionDatabase       *g_thePollutionDB;

// The Pools
extern  AgreementPool           *g_theAgreementPool;
extern  CivilisationPool        *g_theCivilisationPool;
extern  DiplomaticRequestPool   *g_theDiplomaticRequestPool;
extern  MessagePool             *g_theMessagePool;
extern  TerrainImprovementPool  *g_theTerrainImprovementPool;
extern  TradePool               *g_theTradePool;
extern  TradeOfferPool          *g_theTradeOfferPool;
extern  UnitPool                *g_theUnitPool;

// Others
extern  Pollution               *g_thePollution;
extern  SelectedItem            *g_selected_item; 
extern  World                   *g_theWorld;
extern  Player                  **g_player;
extern  TopTen                  *g_theTopTen;


DataCheck                       *g_dataCheck;











void DataCheck_Init(void)
{

}












void DataCheck_Requiem(void)
{
	
	
	
	if (g_dataCheck)
	{
		delete g_dataCheck;
		g_dataCheck = NULL;
	}

}












DataCheck::DataCheck()
{
	sint32	i,
			j;

	m_is_display = FALSE;
	for (i=CRC_TYPE_MIN; i<CRC_TYPE_MAX; i++)
	{
		for (j=0; j<CRC_ARRAY_MAX; j++)
			m_old_crc[i][j] = m_crc[i][j] = 0;

		m_time[i] = 0;
	}

	m_total_time = 0;
}




// If we have to do copy and paste at all then let 
// make the preprocessor to do the work for us.
#define CHECK_DB(db, db_type) \
	start = clock(); \
	archive = new CivArchive(); \
	archive->SetStore(); \
	check = new CheckSum(); \
	db->Serialize(*archive); \
	check->AddData(archive->GetStream(), archive->StreamLen()); \
	check->Done(m_crc[db_type][CRC_ARRAY_0], m_crc[db_type][CRC_ARRAY_1], m_crc[db_type][CRC_ARRAY_2], m_crc[db_type][CRC_ARRAY_3]); \
	delete archive; \
	delete check; \
	finish = clock(); \
	m_time[db_type] = finish - start;





void DataCheck::BeginTurn(void)
{
	CheckSum	*check;
	
	CivArchive	*archive;
	
	sint32	i, j;
	
	clock_t start, finish;

	for(i=CRC_TYPE_MIN; i<CRC_TYPE_MAX; i++)
		for (j=0; j<CRC_ARRAY_MAX; j++)
			m_old_crc[i][j] = m_crc[i][j];
		
	
	memset(&m_time, 0, CRC_TYPE_MAX * sizeof(uint32));

	start = clock();
	archive = new CivArchive();
	archive->SetStore();
	check = new CheckSum();
	// No idea what was serialized here
	check->AddData(archive->GetStream(), archive->StreamLen());
	check->Done(m_crc[CRC_TYPE_GLOBAL][CRC_ARRAY_0], m_crc[CRC_TYPE_GLOBAL][CRC_ARRAY_1], m_crc[CRC_TYPE_GLOBAL][CRC_ARRAY_2], m_crc[CRC_TYPE_GLOBAL][CRC_ARRAY_3]);
	delete archive;
	delete check;
	finish = clock();
	m_time[CRC_TYPE_GLOBAL] = finish - start;
	// should be replaced by:
//	CHECK_DB(???, CRC_TYPE_GLOBAL);
  
	CHECK_DB(g_rand, CRC_TYPE_RAND);

	start = clock();
	archive = new CivArchive();
	archive->SetStore();
	check = new CheckSum();

	// Fill in missing databases
	g_theAdvanceDB->Serialize(*archive);
	g_theAdvanceBranchDB->Serialize(*archive);
	g_theAdvanceListDB->Serialize(*archive);
	g_theAgeDB->Serialize(*archive);
	g_theAgeCityStyleDB->Serialize(*archive);
	g_theBuildListSequenceDB->Serialize(*archive);
	g_theCitySizeDB->Serialize(*archive);
	g_theCityStyleDB->Serialize(*archive);
	g_theCivilisationDB->Serialize(*archive);
	g_theConstDB->Serialize(*archive);
	g_theDifficultyDB->Serialize(*archive);
	g_theDiplomacyDB->Serialize(*archive);
	g_theDiplomacyProposalDB->Serialize(*archive);
	g_theDiplomacyThreatDB->Serialize(*archive);
	g_theEndGameObjectDB->Serialize(*archive);
	g_theGoalDB->Serialize(*archive);
	g_theGovernmentDB->Serialize(*archive);
	g_theIconDB->Serialize(*archive);
	g_theImprovementListDB->Serialize(*archive);
	g_theOrderDB->Serialize(*archive);
	g_theUVDB->Serialize(*archive);
	g_thePersonalityDB->Serialize(*archive);
	g_thePollutionDB->Serialize(*archive);
	g_thePopDB->Serialize(*archive);
	g_theResourceDB->Serialize(*archive);
	g_theRiskDB->Serialize(*archive);
	g_theSoundDB->Serialize(*archive);
	g_theSpecialAttackInfoDB->Serialize(*archive);
	g_theSpecialEffectDB->Serialize(*archive);
	g_theSpriteDB->Serialize(*archive);
	g_theStrategyDB->Serialize(*archive);
	g_theTerrainDB->Serialize(*archive);
	g_theUnitDB->Serialize(*archive);
	g_theUnitBuildListDB->Serialize(*archive);
	g_theWonderDB->Serialize(*archive);
	g_theWonderBuildListDB->Serialize(*archive);

	check->AddData(archive->GetStream(), archive->StreamLen());
	check->Done(m_crc[CRC_TYPE_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_DB][CRC_ARRAY_3]);
	delete archive;
	delete check;
	finish = clock();
	m_time[CRC_TYPE_DB] = finish - start;

	// Continue with single database check

	start = clock();
	archive = new CivArchive();
	archive->SetStore();
	check = new CheckSum();
//	g_theProfileDB->Serialize(*archive);
	check->AddData(archive->GetStream(), archive->StreamLen());
	check->Done(m_crc[CRC_TYPE_PROFILE_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_PROFILE_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_PROFILE_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_PROFILE_DB][CRC_ARRAY_3]);
	delete archive;
	delete check;
	finish = clock();
	m_time[CRC_TYPE_PROFILE_DB] = finish - start;

	
	start = clock();
	archive = new CivArchive();
	archive->SetStore();
	check = new CheckSum();
//	g_theStringDB->Serialize(*archive);
	check->AddData(archive->GetStream(), archive->StreamLen());
	check->Done(m_crc[CRC_TYPE_STRING_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_STRING_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_STRING_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_STRING_DB][CRC_ARRAY_3]);
	delete archive;
	delete check;
	finish = clock();
	m_time[CRC_TYPE_STRING_DB] = finish - start;

	
	CHECK_DB(g_theAdvanceDB, CRC_TYPE_ADVANCE_DB);
	CHECK_DB(g_theAdvanceBranchDB, CRC_TYPE_ADVANCE_BRANCH_DB);
	CHECK_DB(g_theAdvanceListDB, CRC_TYPE_ADVANCE_LIST_DB);
	CHECK_DB(g_theAgeDB, CRC_TYPE_AGE_DB);
	CHECK_DB(g_theAgeCityStyleDB, CRC_TYPE_AGE_CITY_STYLE_DB);
	CHECK_DB(g_theBuildListSequenceDB, CRC_TYPE_BUILD_LIST_SEQUENCE_DB);
	CHECK_DB(g_theBuildingDB, CRC_TYPE_BUILDING_DB);
	CHECK_DB(g_theBuildingBuildListDB, CRC_TYPE_BUILDING_BUILD_LIST_DB);
	CHECK_DB(g_theCitySizeDB, CRC_TYPE_CITY_SIZE_DB);
	CHECK_DB(g_theCityStyleDB, CRC_TYPE_CITY_STYLE_DB);
	CHECK_DB(g_theCivilisationDB, CRC_TYPE_CIVILISATION_DB);
	CHECK_DB(g_theConstDB, CRC_TYPE_CONST_DB);
	CHECK_DB(g_theDifficultyDB, CRC_TYPE_DIFFICULTY_DB);
	CHECK_DB(g_theDiplomacyDB, CRC_TYPE_DIPLOMACY_DB);
	CHECK_DB(g_theDiplomacyProposalDB, CRC_TYPE_DIPLOMACY_PROPOSAL_DB);
	CHECK_DB(g_theDiplomacyThreatDB, CRC_TYPE_DIPLOMACY_THREAT_DB);
	CHECK_DB(g_theEndGameObjectDB, CRC_TYPE_END_GAME_OBJECT_DB);
	CHECK_DB(g_theFeatDB, CRC_TYPE_FEAT_DB);
	CHECK_DB(g_theGWDB, CRC_TYPE_GLOBAL_WARMING_DB);
	CHECK_DB(g_theGoalDB, CRC_TYPE_GOAL_DB);
	CHECK_DB(g_theGovernmentDB, CRC_TYPE_GOVERNMENT_DB);
	CHECK_DB(g_theIconDB, CRC_TYPE_ICON_DB);
	CHECK_DB(g_theImprovementListDB, CRC_TYPE_IMPROVEMENT_LIST_DB);
	CHECK_DB(g_theOrderDB, CRC_TYPE_ORDER_DB);
	CHECK_DB(g_theUVDB, CRC_TYPE_OZONE_DB);
	CHECK_DB(g_thePersonalityDB, CRC_TYPE_PERSONALITY_DB);
	CHECK_DB(g_thePollutionDB, CRC_TYPE_POLLUTION_DB);
	CHECK_DB(g_thePopDB, CRC_TYPE_POPULATION_DB);
	CHECK_DB(g_theResourceDB, CRC_TYPE_RESOURCE_DB);
	CHECK_DB(g_theRiskDB, CRC_TYPE_RISK_DB);
	CHECK_DB(g_theSoundDB, CRC_TYPE_SOUND_DB);
	CHECK_DB(g_theSpecialAttackInfoDB, CRC_TYPE_SPECIAL_ATTACK_INFO_DB);
	CHECK_DB(g_theSpecialEffectDB, CRC_TYPE_SPECIAL_EFFECT_DB);
	CHECK_DB(g_theSpriteDB, CRC_TYPE_SPRITE_DB);
	CHECK_DB(g_theStrategyDB, CRC_TYPE_STRATEGY_DB);
	CHECK_DB(g_theTerrainDB, CRC_TYPE_TERRAIN_DB);
	CHECK_DB(g_theUnitDB, CRC_TYPE_UNIT_DB);
	CHECK_DB(g_theUnitBuildListDB, CRC_TYPE_UNIT_BUILD_LIST_DB);
	CHECK_DB(g_theWonderDB, CRC_TYPE_WONDER_DB);
	CHECK_DB(g_theWonderBuildListDB, CRC_TYPE_WONDER_BUILD_LIST_DB);

	CHECK_DB(g_theAgreementPool, CRC_TYPE_AGREEMENTPOOL);
	CHECK_DB(g_theCivilisationPool, CRC_TYPE_CIVILISATIONPOOL);
	CHECK_DB(g_theDiplomaticRequestPool, CRC_TYPE_DIPLOMATICREQUESTPOOL);
	CHECK_DB(g_theMessagePool, CRC_TYPE_MESSAGEPOOL);
	CHECK_DB(g_theTerrainImprovementPool, CRC_TYPE_TERRAIN_IMPROVEMENT_POOL);
	CHECK_DB(g_theTradePool, CRC_TYPE_TRADEPOOL);
	CHECK_DB(g_theTradeOfferPool, CRC_TYPE_TRADEOFFERPOOL);
	CHECK_DB(g_theUnitPool, CRC_TYPE_UNITPOOL);

	CHECK_DB(g_thePollution, CRC_TYPE_POLLUTION);
	CHECK_DB(g_selected_item, CRC_TYPE_SELECTED_ITEM);
	CHECK_DB(g_theTopTen, CRC_TYPE_TOPTEN);
	CHECK_DB(g_theWorld, CRC_TYPE_WORLD);

	start = clock();
	archive = new CivArchive();
	archive->SetStore();
	check = new CheckSum();
	for (i=0; i<k_MAX_PLAYERS; i++) {
		if(!g_player[i]) continue;
		g_player[i]->Serialize(*archive);
	}
	
	check->AddData(archive->GetStream(), archive->StreamLen());
	check->Done(m_crc[CRC_TYPE_PLAYER][CRC_ARRAY_0], m_crc[CRC_TYPE_PLAYER][CRC_ARRAY_1], m_crc[CRC_TYPE_PLAYER][CRC_ARRAY_2], m_crc[CRC_TYPE_PLAYER][CRC_ARRAY_3]);
	delete archive;
	delete check;
	finish = clock();
	m_time[CRC_TYPE_PLAYER] = finish - start;

	m_total_time = 0;
	for(i = 0; i < CRC_ARRAY_MAX; ++i)
		m_total_time += m_time[i];
}













sint32 DataCheck::IsWorldChanged () const
{ 
	sint32	i;

	for (i=0; i<CRC_ARRAY_MAX; i++)
		if (m_crc[CRC_TYPE_WORLD][i] != m_old_crc[CRC_TYPE_WORLD][i])
			return (TRUE);
	
	return (FALSE);
}













sint32 DataCheck::IsGlobalChanged () const 
{
	sint32	i;

	for (i=0; i<CRC_ARRAY_MAX; i++)
		if (m_crc[CRC_TYPE_GLOBAL][i] != m_old_crc[CRC_TYPE_GLOBAL][i])
			return (TRUE);

	return (FALSE);
}














sint32 DataCheck::IsRandChanged () const 

{ 
	sint32	i;

	for (i=0; i<CRC_ARRAY_MAX; i++) {
		if (m_crc[CRC_TYPE_RAND][i] != m_old_crc[CRC_TYPE_RAND][i]) {
			return TRUE;
		}
	}

	return FALSE;
}













sint32 DataCheck::IsDBChanged () const
{ 
	sint32	i;

	for (i=0; i<CRC_ARRAY_MAX; i++)
		if (m_crc[CRC_TYPE_DB][i] != m_old_crc[CRC_TYPE_DB][i])
			return (TRUE);

	return (FALSE);
}













sint32 DataCheck::IsPlayerChanged () const

{ 
	sint32	i; 

	for (i=0; i<CRC_ARRAY_MAX; i++) { 
		if (m_crc[CRC_TYPE_PLAYER][i] != m_old_crc[CRC_TYPE_PLAYER][i]) {
			return TRUE;
		}
	}
	return FALSE;
}












sint32 DataCheck::IsPopChanged () const

{ 
	return FALSE;
}












sint32 DataCheck::IsUnitChanged () const

{ 
	sint32	i;

	for (i=0; i<CRC_ARRAY_MAX; i++) { 
		if (m_crc[CRC_TYPE_UNITPOOL][i] != m_old_crc[CRC_TYPE_UNITPOOL][i]) {
			return TRUE;
		}
	}
	return FALSE;
}













BOOL DataCheck::GetCRC(CRC_TYPE group, uint32 &a, uint32 &b, uint32 &c, uint32 &d)
{
	Assert(group>=CRC_TYPE_MIN);
	Assert(group<CRC_TYPE_MAX);
	if ((group<CRC_TYPE_MIN) || (group>=CRC_TYPE_MAX))
		return (FALSE);

	a = m_crc[group][CRC_ARRAY_0];
	b = m_crc[group][CRC_ARRAY_1];
	c = m_crc[group][CRC_ARRAY_2];
	d = m_crc[group][CRC_ARRAY_3];

	return (TRUE);
}













void DataCheck::SetDisplay(sint32 val)
{
	m_is_display = val;
}













sint32 DataCheck::IsChanged(sint32 t) const 
{
	sint32	i;

	Assert((t>=0) && (t<CRC_TYPE_MAX));
	for(i=0; i<CRC_ARRAY_MAX; i++)
		if (m_crc[t][i] != m_old_crc[t][i])
			return (TRUE);

	return (FALSE); 
}

 







void DataCheck::draw_crc(aui_Surface *surf, char str1[80], sint32 t, sint32 x, sint32 y) const
{
	MBCHAR	str2[80];

	primitives_DrawText((aui_DirectSurface *)surf, x, y, (MBCHAR *)str1, 0, 0);

	if (IsChanged(t))
		{
		sprintf(str2, "***");
		primitives_DrawText((aui_DirectSurface *)surf, x+100, y, (MBCHAR *)str2, 3050, 1);
		}

	sprintf (str2, "%08X %08X %08X %08X  %4.2lf", m_crc[t][CRC_ARRAY_0], m_crc[t][CRC_ARRAY_1], m_crc[t][CRC_ARRAY_2], m_crc[t][CRC_ARRAY_3], (double)(m_time[t]) / CLOCKS_PER_SEC);
	primitives_DrawText((aui_DirectSurface *)surf, x+125, y, (MBCHAR *)str2, 0, 0);
}









void DataCheck::draw_time(aui_Surface *surf, sint32 x, sint32 y) const
{
	MBCHAR	s[80];

	sprintf(s, "Total time %4.2lf", (double)(m_total_time) / CLOCKS_PER_SEC);
	primitives_DrawText((aui_DirectSurface *)surf, x, y, s, 0, 0);
}

	







void DataCheck::DisplayCRC(aui_Surface *surf) const 
{
	sint32	x=100,
			y=80,
			d=16;

	if (!m_is_display)
		return;

	if(m_is_display > 1) {
		draw_crc(surf, "DB", CRC_TYPE_DB, x, y);                                      y+=d;
		draw_crc(surf, "PROFILE", CRC_TYPE_PROFILE_DB, x, y);                         y+=d;
		draw_crc(surf, "STRING", CRC_TYPE_STRING_DB, x, y);                           y+=d;
		draw_crc(surf, "ADVANCE", CRC_TYPE_ADVANCE_DB, x, y);                         y+=d;
		draw_crc(surf, "ADVANCE_BRANCH", CRC_TYPE_ADVANCE_BRANCH_DB, x, y);           y+=d;
		draw_crc(surf, "ADVANCE_LIST", CRC_TYPE_ADVANCE_LIST_DB, x, y);               y+=d;
		draw_crc(surf, "AGE", CRC_TYPE_AGE_DB, x, y);                                 y+=d;
		draw_crc(surf, "AGE_CITY_STYLE", CRC_TYPE_AGE_CITY_STYLE_DB, x, y);           y+=d;
		draw_crc(surf, "BUILD_LIST_SEQUENCE", CRC_TYPE_BUILD_LIST_SEQUENCE_DB, x, y); y+=d;
		draw_crc(surf, "BUILDING", CRC_TYPE_BUILDING_DB, x, y);                       y+=d;
		draw_crc(surf, "BUILDING_BUILD_LIST", CRC_TYPE_BUILDING_BUILD_LIST_DB, x, y); y+=d;
		draw_crc(surf, "CITY_SIZE", CRC_TYPE_CITY_SIZE_DB, x, y);                     y+=d;
		draw_crc(surf, "CITY_STYLE", CRC_TYPE_CITY_STYLE_DB, x, y);                   y+=d;
		draw_crc(surf, "CIVILISATION", CRC_TYPE_CIVILISATION_DB, x, y);               y+=d;
		draw_crc(surf, "CONST", CRC_TYPE_CONST_DB, x, y);                             y+=d;
		draw_crc(surf, "DIFFICULTY", CRC_TYPE_DIFFICULTY_DB, x, y);                   y+=d;
		draw_crc(surf, "DIPLOMACY", CRC_TYPE_DIPLOMACY_DB, x, y);                     y+=d;
		draw_crc(surf, "DIPLOMACY_PROPOSAL", CRC_TYPE_DIPLOMACY_PROPOSAL_DB, x, y);   y+=d;
		draw_crc(surf, "DIPLOMACY_THREAT", CRC_TYPE_DIPLOMACY_THREAT_DB, x, y);       y+=d;
		draw_crc(surf, "END_GAME_OBJECT", CRC_TYPE_END_GAME_OBJECT_DB, x, y);         y+=d;
		draw_crc(surf, "FEAT", CRC_TYPE_FEAT_DB, x, y);                               y+=d;
		draw_crc(surf, "GLOBAL_WARMING", CRC_TYPE_GLOBAL_WARMING_DB, x, y);           y+=d;
		draw_crc(surf, "GOAL", CRC_TYPE_GOAL_DB, x, y);                               y+=d;
		draw_crc(surf, "GOVERNMENT", CRC_TYPE_GOVERNMENT_DB, x, y);                   y+=d;
		draw_crc(surf, "ICON", CRC_TYPE_ICON_DB, x, y);                               y+=d;
		draw_crc(surf, "IMPROVEMENT_LIST", CRC_TYPE_IMPROVEMENT_LIST_DB, x, y);       y+=d;
		draw_crc(surf, "ORDER", CRC_TYPE_ORDER_DB, x, y);                             y+=d;
		draw_crc(surf, "OZONE", CRC_TYPE_OZONE_DB, x, y);                             y+=d;
		draw_crc(surf, "PERSONALITY", CRC_TYPE_PERSONALITY_DB, x, y);                 y+=d;
		draw_crc(surf, "POLLUTION", CRC_TYPE_POLLUTION_DB, x, y);                     y+=d;
		draw_crc(surf, "POP", CRC_TYPE_POPULATION_DB, x, y);                          y+=d;
		draw_crc(surf, "RESOURCE", CRC_TYPE_RESOURCE_DB, x, y);                       y+=d;
		draw_crc(surf, "RISK", CRC_TYPE_RISK_DB, x, y);                               y+=d;
		draw_crc(surf, "SOUND", CRC_TYPE_SOUND_DB, x, y);                             y+=d;
		draw_crc(surf, "SPECIAL_ATTACK_INFO", CRC_TYPE_SPECIAL_ATTACK_INFO_DB, x, y); y+=d;
		draw_crc(surf, "SPECIAL_EFFECT", CRC_TYPE_SPECIAL_EFFECT_DB, x, y);           y+=d;
		draw_crc(surf, "SPRITE", CRC_TYPE_SPRITE_DB, x, y);                           y+=d;
		draw_crc(surf, "STRATEGY", CRC_TYPE_STRATEGY_DB, x, y);                       y+=d;
		draw_crc(surf, "TERRAIN", CRC_TYPE_TERRAIN_DB, x, y);                         y+=d;
		draw_crc(surf, "UNIT", CRC_TYPE_UNIT_DB, x, y);                               y+=d;
		draw_crc(surf, "UNIT_BUILD_LIST", CRC_TYPE_UNIT_BUILD_LIST_DB, x, y);         y+=d;
		draw_crc(surf, "WONDER", CRC_TYPE_WONDER_DB, x, y);                           y+=d;
		draw_crc(surf, "WONDER_BUILD_LIST", CRC_TYPE_WONDER_BUILD_LIST_DB, x, y);     y+=25;
	}

	x+=300;
	draw_crc(surf, "GLOBAL", CRC_TYPE_GLOBAL, x, y);                                     y+=d;
	draw_crc(surf, "RAND", CRC_TYPE_RAND, x, y);                                       	 y+=d;
	draw_crc(surf, "AGREEMENT_POOL", CRC_TYPE_AGREEMENTPOOL, x, y);                      y+=d;
	draw_crc(surf, "CIVILISATION_POOL", CRC_TYPE_CIVILISATIONPOOL, x, y);                y+=d;
	draw_crc(surf, "DIPLOMACY_REQUEST_POOL", CRC_TYPE_DIPLOMATICREQUESTPOOL, x, y);      y+=d;
	draw_crc(surf, "TERRAINIMPROVEMENT_POOL", CRC_TYPE_TERRAIN_IMPROVEMENT_POOL, x, y);  y+=d;
	draw_crc(surf, "MESSAGE_POOL", CRC_TYPE_MESSAGEPOOL, x, y);                          y+=d;
	draw_crc(surf, "TERRAIN_IMPROVEMENT_POOL", CRC_TYPE_TERRAIN_IMPROVEMENT_POOL, x, y); y+=d;
	draw_crc(surf, "TRADE_POOL", CRC_TYPE_TRADEPOOL, x, y);                              y+=d;
	draw_crc(surf, "TRADEOFFER_POOL", CRC_TYPE_TRADEOFFERPOOL, x, y);                    y+=d;
	draw_crc(surf, "UNIT_POOL", CRC_TYPE_UNITPOOL, x, y);                                y+=d;

	draw_crc(surf, "POLLUTION", CRC_TYPE_POLLUTION, x, y);                               y+=d;
	draw_crc(surf, "SELECTED", CRC_TYPE_SELECTED_ITEM, x, y);                            y+=d;
	draw_crc(surf, "TOPTEN", CRC_TYPE_TOPTEN, x, y);                                     y+=d;
	draw_crc(surf, "WORLD", CRC_TYPE_WORLD, x, y);                                       y+=d;
	draw_crc(surf, "PLAYER", CRC_TYPE_PLAYER, x, y);                                     y+=d;

	draw_time(surf, x, y);
}

void DataCheck::DumpSingleCRC(MBCHAR *grp, sint32 t)
{
	DPRINTF(k_DBG_INFO, ("%s     %08X %08X %08X %08X  %4.2lf\n", grp, m_crc[t][CRC_ARRAY_0], m_crc[t][CRC_ARRAY_1], m_crc[t][CRC_ARRAY_2], m_crc[t][CRC_ARRAY_3], (double)(m_time[t]) / CLOCKS_PER_SEC));
}









void DataCheck::DumpChecksum(void)
{
	DumpSingleCRC("GLOBAL", CRC_TYPE_GLOBAL);
	DumpSingleCRC("RAND", CRC_TYPE_RAND);
	DumpSingleCRC("DB", CRC_TYPE_DB);
	DumpSingleCRC("PROFILE", CRC_TYPE_PROFILE_DB);
	DumpSingleCRC("STRING", CRC_TYPE_STRING_DB);
	DumpSingleCRC("ADVANCE", CRC_TYPE_ADVANCE_DB);
	DumpSingleCRC("ADVANCE_BRANCH", CRC_TYPE_ADVANCE_BRANCH_DB);
	DumpSingleCRC("ADVANCE_LIST", CRC_TYPE_ADVANCE_LIST_DB);
	DumpSingleCRC("AGE", CRC_TYPE_AGE_DB);
	DumpSingleCRC("AGE_CITY_STYLE", CRC_TYPE_AGE_CITY_STYLE_DB);
	DumpSingleCRC("BUILD_LIST_SEQUENCE", CRC_TYPE_BUILD_LIST_SEQUENCE_DB);
	DumpSingleCRC("BUILDING", CRC_TYPE_BUILDING_DB);
	DumpSingleCRC("BUILDING_BUILD_LIST", CRC_TYPE_BUILDING_BUILD_LIST_DB);
	DumpSingleCRC("CITY_SIZE", CRC_TYPE_CITY_SIZE_DB);
	DumpSingleCRC("CITY_STYLE", CRC_TYPE_CITY_STYLE_DB);
	DumpSingleCRC("CIVILISATION", CRC_TYPE_CIVILISATION_DB);
	DumpSingleCRC("CONST", CRC_TYPE_CONST_DB);
	DumpSingleCRC("DIFFICULTY", CRC_TYPE_DIFFICULTY_DB);
	DumpSingleCRC("DIPLOMACY", CRC_TYPE_DIPLOMACY_DB);
	DumpSingleCRC("DIPLOMACY_PROPOSAL", CRC_TYPE_DIPLOMACY_PROPOSAL_DB);
	DumpSingleCRC("DIPLOMACY_THREAT", CRC_TYPE_DIPLOMACY_THREAT_DB);
	DumpSingleCRC("END_GAME_OBJECT", CRC_TYPE_END_GAME_OBJECT_DB);
	DumpSingleCRC("FEAT", CRC_TYPE_FEAT_DB);
	DumpSingleCRC("GLOBAL_WARMING", CRC_TYPE_GLOBAL_WARMING_DB);
	DumpSingleCRC("GOAL", CRC_TYPE_GOAL_DB);
	DumpSingleCRC("GOVERNMENT", CRC_TYPE_GOVERNMENT_DB);
	DumpSingleCRC("ICON", CRC_TYPE_ICON_DB);
	DumpSingleCRC("IMPROVEMENT_LIST", CRC_TYPE_IMPROVEMENT_LIST_DB);
	DumpSingleCRC("ORDER", CRC_TYPE_ORDER_DB);
	DumpSingleCRC("OZONE", CRC_TYPE_OZONE_DB);
	DumpSingleCRC("PERSONALITY", CRC_TYPE_PERSONALITY_DB);
	DumpSingleCRC("POLLUTION", CRC_TYPE_POLLUTION_DB);
	DumpSingleCRC("POP", CRC_TYPE_POPULATION_DB);
	DumpSingleCRC("RESOURCE", CRC_TYPE_RESOURCE_DB);
	DumpSingleCRC("RISK", CRC_TYPE_RISK_DB);
	DumpSingleCRC("SOUND", CRC_TYPE_SOUND_DB);
	DumpSingleCRC("SPECIAL_ATTACK_INFO", CRC_TYPE_SPECIAL_ATTACK_INFO_DB);
	DumpSingleCRC("SPECIAL_EFFECT", CRC_TYPE_SPECIAL_EFFECT_DB);
	DumpSingleCRC("SPRITE", CRC_TYPE_SPRITE_DB);
	DumpSingleCRC("STRATEGY", CRC_TYPE_STRATEGY_DB);
	DumpSingleCRC("TERRAIN", CRC_TYPE_TERRAIN_DB);
	DumpSingleCRC("UNIT", CRC_TYPE_UNIT_DB);
	DumpSingleCRC("UNIT_BUILD_LIST", CRC_TYPE_UNIT_BUILD_LIST_DB);
	DumpSingleCRC("WONDER", CRC_TYPE_WONDER_DB);
	DumpSingleCRC("WONDER_BUILD_LIST", CRC_TYPE_WONDER_BUILD_LIST_DB);

	DumpSingleCRC("AGREEMENT_POOL", CRC_TYPE_AGREEMENTPOOL);
	DumpSingleCRC("CIVILISATION_POOL", CRC_TYPE_CIVILISATIONPOOL);
	DumpSingleCRC("DIPLOMACY_REQUEST_POOL", CRC_TYPE_DIPLOMATICREQUESTPOOL);
	DumpSingleCRC("TERRAINIMPROVEMENT_POOL", CRC_TYPE_TERRAIN_IMPROVEMENT_POOL);
	DumpSingleCRC("MESSAGE_POOL", CRC_TYPE_MESSAGEPOOL);
	DumpSingleCRC("TERRAIN_IMPROVEMENT_POOL", CRC_TYPE_TERRAIN_IMPROVEMENT_POOL);
	DumpSingleCRC("TRADE_POOL", CRC_TYPE_TRADEPOOL);
	DumpSingleCRC("TRADEOFFER_POOL", CRC_TYPE_TRADEOFFERPOOL);
	DumpSingleCRC("UNIT_POOL", CRC_TYPE_UNITPOOL);

	DumpSingleCRC("POLLUTION", CRC_TYPE_POLLUTION);
	DumpSingleCRC("SELECTED", CRC_TYPE_SELECTED_ITEM);
	DumpSingleCRC("TOPTEN", CRC_TYPE_TOPTEN);
	DumpSingleCRC("WORLD", CRC_TYPE_WORLD);
	DumpSingleCRC("PLAYER", CRC_TYPE_PLAYER);

	DPRINTF(k_DBG_INFO, ("Total time %4.2lf\n", (double)(m_total_time) / CLOCKS_PER_SEC));
}
