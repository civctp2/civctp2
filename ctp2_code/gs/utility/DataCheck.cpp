











#include "c3.h"

#include "ConstDB.h"
#include "CivArchive.h"
#include "Cell.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "CheckSum.h"
#include "CivPaths.h"
#include "BuildingRecord.h"
#include "SpriteStateDB.h"
#include "DiffDB.h"
#include "AdvanceRecord.h"
#include "RandGen.h"
#include "TerrainRecord.h"
#include "TradePool.h"
#include "TradeOfferPool.h"
#include "Pollution.h"
#include "GWDB.h"
#include "UVDB.h"
#include "WonderRecord.h"
#include "UnitPool.h"
#include "SelItem.h"
#include "CivilisationDB.h"
#include "DiplomaticRequestPool.h"
#include "CivilisationPool.h"
#include "AgreementPool.h"
#include "MessagePool.h"
#include "installation.h"
#include "installationpool.h"

#include "TerrImprovePool.h"
#include "ProfileDB.h"
#include "TopTen.h"
#include "GovernmentRecord.h"


#include "aui.h"
#include "aui_surface.h"
#include "primitives.h"
#include "Player.h"

#include "DataCheck.h"

#include "StrDB.h"
#include "PollutionDB.h"
#include "SpecialEffectRecord.h"

	extern	CivPaths	*g_civPaths ;

	extern	World	*g_theWorld ;
	extern	Player	**g_player ;
	extern	UnitPool	*g_theUnitPool ;
	extern	StringDB	*g_theStringDB ;
	extern	ConstDB					*g_theConstDB ;
	extern	SpriteStateDB			*g_theSpriteStateDB ;




	extern	SpriteStateDB			*g_theGoodsSpriteStateDB;
	extern	DifficultyDB			*g_theDifficultyDB ;
    extern  RandomGenerator			*g_rand; 
    extern  SelectedItem			*g_selected_item; 
	extern	PollutionDatabase		*g_thePollutionDB ;
	extern	GlobalWarmingDatabase	*g_theGWDB ;
	extern	OzoneDatabase			*g_theUVDB ;
	extern	Pollution				*g_thePollution ;
	extern	TopTen					*g_theTopTen ;
	extern	DiplomaticRequestPool	*g_theDiplomaticRequestPool ;
	extern	MessagePool				*g_theMessagePool ;
	extern	CivilisationPool		*g_theCivilisationPool ;
	extern	AgreementPool			*g_theAgreementPool ;
	extern	CivilisationDatabase	*g_theCivilisationDB ;

	extern  TerrainImprovementPool	*g_theTerrainImprovementPool;
	extern	ProfileDB				*g_theProfileDB;
	extern StringDB                 *g_theStringDB;

	DataCheck	*g_dataCheck ;











void DataCheck_Init(void)
	{ 


	} 












void DataCheck_Requiem(void)
	{
	
	
	
	if (g_dataCheck)
		{
		delete g_dataCheck ;
		g_dataCheck = NULL ;
		}

	}












DataCheck::DataCheck()
	{
	sint32	i,
			j ;

    m_is_display = FALSE ;
	for (i=CRC_TYPE_MIN; i<CRC_TYPE_MAX; i++)
		{
        for (j=0; j<CRC_ARRAY_MAX; j++)
			m_old_crc[i][j] = m_crc[i][j] = 0 ;

		m_time[i] = 0 ;
		}

	m_total_time = 0 ;
	}











void DataCheck::BeginTurn(void)
	{
    CheckSum	*check ;
    
    CivArchive	*archive ;
    
    sint32	i, j ;
    
	clock_t	start, finish ;

    for(i=CRC_TYPE_MIN; i<CRC_TYPE_MAX; i++)
        for (j=0; j<CRC_ARRAY_MAX; j++)
            m_old_crc[i][j] = m_crc[i][j] ;
        
    
    
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
    
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_GLOBAL][CRC_ARRAY_0], m_crc[CRC_TYPE_GLOBAL][CRC_ARRAY_1], m_crc[CRC_TYPE_GLOBAL][CRC_ARRAY_2], m_crc[CRC_TYPE_GLOBAL][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_GLOBAL] = finish - start ;
  
    
	start = clock() ;
	archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
    
    g_rand->Serialize(*archive); 
  
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_RAND][CRC_ARRAY_0], m_crc[CRC_TYPE_RAND][CRC_ARRAY_1], m_crc[CRC_TYPE_RAND][CRC_ARRAY_2], m_crc[CRC_TYPE_RAND][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_RAND] = finish - start ;

	
    
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;

	
	
	
	
	
	
	
	g_theConstDB->Serialize(*archive) ;								
	g_theSpriteStateDB->Serialize(*archive) ;						
	




	g_theGoodsSpriteStateDB->Serialize(*archive);					
	g_theDifficultyDB->Serialize(*archive) ;						
	
	g_thePollutionDB->Serialize(*archive) ;							
	g_theUVDB->Serialize(*archive) ;								
	g_theCivilisationDB->Serialize(*archive) ;						


    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_DB] = finish - start ;

    
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
	
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_PROFILE_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_PROFILE_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_PROFILE_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_PROFILE_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_PROFILE_DB] = finish - start ;

    
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
	
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_STRING_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_STRING_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_STRING_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_STRING_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_STRING_DB] = finish - start ;

	
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
	
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_ADVANCE_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_ADVANCE_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_ADVANCE_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_ADVANCE_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_ADVANCE_DB] = finish - start ;

	
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
	
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_UNIT_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_UNIT_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_UNIT_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_UNIT_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_UNIT_DB] = finish - start ;

	
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
	
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_GOVERNMENT_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_GOVERNMENT_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_GOVERNMENT_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_GOVERNMENT_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_GOVERNMENT_DB] = finish - start ;

	
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
	
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_TERRAIN_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_TERRAIN_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_TERRAIN_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_TERRAIN_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_TERRAIN_DB] = finish - start ;
    
	
	start = clock() ;
	archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
	g_theConstDB->Serialize(*archive) ;								
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_CONST_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_CONST_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_CONST_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_CONST_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_CONST_DB] = finish - start ;
    
	
	start = clock() ;
	archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
	g_theSpriteStateDB->Serialize(*archive) ;						
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_SPRITESTATE_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_SPRITESTATE_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_SPRITESTATE_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_SPRITESTATE_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_SPRITESTATE_DB] = finish - start ;
    
	
#if 0
	start = clock() ;
	archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
	g_theSpecialEffectDB->Serialize(*archive) ;						
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_SPRITESTATE_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_SPRITESTATE_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_SPRITESTATE_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_SPRITESTATE_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_SPRITESTATE_DB] = finish - start ;
#endif





























	
	start = clock() ;
	archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
	g_theGoodsSpriteStateDB->Serialize(*archive) ;						
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_PROJECTILEENDSTATE_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_PROJECTILEENDSTATE_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_PROJECTILEENDSTATE_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_PROJECTILEENDSTATE_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_PROJECTILEENDSTATE_DB] = finish - start ;
    
	
	start = clock() ;
	archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
	g_theDifficultyDB->Serialize(*archive) ;						
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_DIFFICULTY_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_DIFFICULTY_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_DIFFICULTY_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_DIFFICULTY_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_DIFFICULTY_DB] = finish - start ;

	













	
	start = clock() ;
   	archive = new CivArchive() ;
	archive->SetStore() ;
	check = new CheckSum() ;
	g_thePollutionDB->Serialize(*archive) ;
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_POLLUTION_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_POLLUTION_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_POLLUTION_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_POLLUTION_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_POLLUTION_DB] = finish - start ;

	
	start = clock() ;
   	archive = new CivArchive() ;
	archive->SetStore() ;
	check = new CheckSum() ;
	g_theGWDB->Serialize(*archive) ;
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_GLOBAL_WARMING_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_GLOBAL_WARMING_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_GLOBAL_WARMING_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_GLOBAL_WARMING_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_GLOBAL_WARMING_DB] = finish - start ;

	
	start = clock() ;
   	archive = new CivArchive() ;
	archive->SetStore() ;
	check = new CheckSum() ;
	g_theUVDB->Serialize(*archive) ;
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_OZONE_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_OZONE_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_OZONE_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_OZONE_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_OZONE_DB] = finish - start ;

	
	start = clock() ;
   	archive = new CivArchive() ;
	archive->SetStore() ;
	check = new CheckSum() ;
	g_theCivilisationDB->Serialize(*archive) ;
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_CIVILISATION_DB][CRC_ARRAY_0], m_crc[CRC_TYPE_CIVILISATION_DB][CRC_ARRAY_1], m_crc[CRC_TYPE_CIVILISATION_DB][CRC_ARRAY_2], m_crc[CRC_TYPE_CIVILISATION_DB][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_CIVILISATION_DB] = finish - start ;

	
	













	
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
    g_theWorld->Serialize(*archive) ;								
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_WORLD][CRC_ARRAY_0], m_crc[CRC_TYPE_WORLD][CRC_ARRAY_1], m_crc[CRC_TYPE_WORLD][CRC_ARRAY_2], m_crc[CRC_TYPE_WORLD][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_WORLD] = finish - start ;
    
    
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
    g_theUnitPool->Serialize(*archive) ;							
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_UNITPOOL][CRC_ARRAY_0], m_crc[CRC_TYPE_UNITPOOL][CRC_ARRAY_1], m_crc[CRC_TYPE_UNITPOOL][CRC_ARRAY_2], m_crc[CRC_TYPE_UNITPOOL][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_UNITPOOL] = finish - start ;
    
    
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
    g_theTradePool->Serialize(*archive) ;							
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_TRADEPOOL][CRC_ARRAY_0], m_crc[CRC_TYPE_TRADEPOOL][CRC_ARRAY_1], m_crc[CRC_TYPE_TRADEPOOL][CRC_ARRAY_2], m_crc[CRC_TYPE_TRADEPOOL][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_TRADEPOOL] = finish - start ;
    
    
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
    g_theTradeOfferPool->Serialize(*archive) ;						
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_TRADEOFFERPOOL][CRC_ARRAY_0], m_crc[CRC_TYPE_TRADEOFFERPOOL][CRC_ARRAY_1], m_crc[CRC_TYPE_TRADEOFFERPOOL][CRC_ARRAY_2], m_crc[CRC_TYPE_TRADEOFFERPOOL][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_TRADEOFFERPOOL] = finish - start ;

    
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
    g_theDiplomaticRequestPool->Serialize(*archive) ;				
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_DIPLOMATICREQUESTPOOL][CRC_ARRAY_0], m_crc[CRC_TYPE_DIPLOMATICREQUESTPOOL][CRC_ARRAY_1], m_crc[CRC_TYPE_DIPLOMATICREQUESTPOOL][CRC_ARRAY_2], m_crc[CRC_TYPE_DIPLOMATICREQUESTPOOL][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_DIPLOMATICREQUESTPOOL] = finish - start ;

    
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
    g_theCivilisationPool->Serialize(*archive) ;				
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_CIVILISATIONPOOL][CRC_ARRAY_0], m_crc[CRC_TYPE_CIVILISATIONPOOL][CRC_ARRAY_1], m_crc[CRC_TYPE_CIVILISATIONPOOL][CRC_ARRAY_2], m_crc[CRC_TYPE_CIVILISATIONPOOL][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_CIVILISATIONPOOL] = finish - start ;

    
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
    g_theMessagePool->Serialize(*archive) ;							
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_MESSAGEPOOL][CRC_ARRAY_0], m_crc[CRC_TYPE_MESSAGEPOOL][CRC_ARRAY_1], m_crc[CRC_TYPE_MESSAGEPOOL][CRC_ARRAY_2], m_crc[CRC_TYPE_MESSAGEPOOL][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_MESSAGEPOOL] = finish - start ;

    
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
    g_theAgreementPool->Serialize(*archive) ;						
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_AGREEMENTPOOL][CRC_ARRAY_0], m_crc[CRC_TYPE_AGREEMENTPOOL][CRC_ARRAY_1], m_crc[CRC_TYPE_AGREEMENTPOOL][CRC_ARRAY_2], m_crc[CRC_TYPE_AGREEMENTPOOL][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_AGREEMENTPOOL] = finish - start ;

	
	start = clock() ;
   	archive = new CivArchive() ;
	archive->SetStore() ;
	check = new CheckSum() ;
	g_thePollution->Serialize(*archive) ;
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_POLLUTION][CRC_ARRAY_0], m_crc[CRC_TYPE_POLLUTION][CRC_ARRAY_1], m_crc[CRC_TYPE_POLLUTION][CRC_ARRAY_2], m_crc[CRC_TYPE_POLLUTION][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_POLLUTION] = finish - start ;

	
	start = clock() ;
   	archive = new CivArchive() ;
	archive->SetStore() ;
	check = new CheckSum() ;
	g_theTopTen->Serialize(*archive) ;
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_TOPTEN][CRC_ARRAY_0], m_crc[CRC_TYPE_TOPTEN][CRC_ARRAY_1], m_crc[CRC_TYPE_TOPTEN][CRC_ARRAY_2], m_crc[CRC_TYPE_TOPTEN][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_TOPTEN] = finish - start ;

	
	start = clock() ;
   	archive = new CivArchive() ;
	archive->SetStore() ;
	check = new CheckSum() ;
	g_theTerrainImprovementPool->Serialize(*archive) ;
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_TERRAIN_IMPROVEMENT_POOL][CRC_ARRAY_0], m_crc[CRC_TYPE_TERRAIN_IMPROVEMENT_POOL][CRC_ARRAY_1], m_crc[CRC_TYPE_TERRAIN_IMPROVEMENT_POOL][CRC_ARRAY_2], m_crc[CRC_TYPE_TERRAIN_IMPROVEMENT_POOL][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_TERRAIN_IMPROVEMENT_POOL] = finish - start ;

	
	start = clock() ;
   	archive = new CivArchive() ;
	archive->SetStore() ;
	check = new CheckSum() ;
	g_theTerrainImprovementPool->Serialize(*archive) ;
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_INSTALLATION_POOL][CRC_ARRAY_0], m_crc[CRC_TYPE_INSTALLATION_POOL][CRC_ARRAY_1], m_crc[CRC_TYPE_INSTALLATION_POOL][CRC_ARRAY_2], m_crc[CRC_TYPE_INSTALLATION_POOL][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_INSTALLATION_POOL] = finish - start ;

    
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
    for (i=0; i<k_MAX_PLAYERS; i++) {
		if(!g_player[i]) continue;
        g_player[i]->Serialize(*archive) ;
	}
    
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_PLAYER][CRC_ARRAY_0], m_crc[CRC_TYPE_PLAYER][CRC_ARRAY_1], m_crc[CRC_TYPE_PLAYER][CRC_ARRAY_2], m_crc[CRC_TYPE_PLAYER][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_PLAYER] = finish - start ;
    
    
	start = clock() ;
    archive = new CivArchive() ;
    archive->SetStore() ;
    check = new CheckSum() ;
    g_selected_item->Serialize(*archive); 
    check->AddData(archive->GetStream(), archive->StreamLen()) ;
    check->Done(m_crc[CRC_TYPE_SELECTED_ITEM][CRC_ARRAY_0], m_crc[CRC_TYPE_SELECTED_ITEM][CRC_ARRAY_1], m_crc[CRC_TYPE_SELECTED_ITEM][CRC_ARRAY_2], m_crc[CRC_TYPE_SELECTED_ITEM][CRC_ARRAY_3]) ;
    delete archive ;
    delete check ;
	finish = clock() ;
	m_time[CRC_TYPE_SELECTED_ITEM] = finish - start ;

	m_total_time = 0 ;
    for (i=0; i<CRC_ARRAY_MAX; i++)
		m_total_time += m_time[i] ;

	}













sint32 DataCheck::IsWorldChanged () const
	{ 
    sint32	i ;

    for (i=0; i<CRC_ARRAY_MAX; i++)
        if (m_crc[CRC_TYPE_WORLD][i] != m_old_crc[CRC_TYPE_WORLD][i])
            return (TRUE) ;
    
	return (FALSE) ;
	}













sint32 DataCheck::IsGlobalChanged () const 
	{
    sint32	i ;

    for (i=0; i<CRC_ARRAY_MAX; i++)
        if (m_crc[CRC_TYPE_GLOBAL][i] != m_old_crc[CRC_TYPE_GLOBAL][i])
            return (TRUE) ;

	return (FALSE) ;
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
    sint32	i ;

    for (i=0; i<CRC_ARRAY_MAX; i++)
        if (m_crc[CRC_TYPE_DB][i] != m_old_crc[CRC_TYPE_DB][i])
            return (TRUE) ;

	return (FALSE) ;
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
    sint32	i ;

    for (i=0; i<CRC_ARRAY_MAX; i++) { 
        if (m_crc[CRC_TYPE_UNITPOOL][i] != m_old_crc[CRC_TYPE_UNITPOOL][i]) {
            return TRUE;
        }
    }
    return FALSE;
}













BOOL DataCheck::GetCRC(CRC_TYPE group, uint32 &a, uint32 &b, uint32 &c, uint32 &d)
	{
	Assert(group>=CRC_TYPE_MIN) ;
	Assert(group<CRC_TYPE_MAX) ;
	if ((group<CRC_TYPE_MIN) || (group>=CRC_TYPE_MAX))
		return (FALSE) ;

	a = m_crc[group][CRC_ARRAY_0] ;
	b = m_crc[group][CRC_ARRAY_1] ;
	c = m_crc[group][CRC_ARRAY_2] ;
	d = m_crc[group][CRC_ARRAY_3] ;

	return (TRUE) ;
	}













void DataCheck::SetDisplay(sint32 val)
	{ 
    m_is_display = val ;
	}













sint32 DataCheck::IsChanged(sint32 t) const 
	{
    sint32	i ;

	Assert((t>=0) && (t<CRC_TYPE_MAX)) ;
    for(i=0; i<CRC_ARRAY_MAX; i++)
        if (m_crc[t][i] != m_old_crc[t][i])
            return (TRUE) ;

	return (FALSE) ; 
	}

 







void DataCheck::draw_crc(aui_Surface *surf, char str1[80], sint32 t, sint32 x, sint32 y) const
	{
    MBCHAR	str2[80] ;

    primitives_DrawText((aui_DirectSurface *)surf, x, y, (MBCHAR *)str1, 0, 0) ;

    if (IsChanged(t))
		{
        sprintf(str2, "***") ;
      	primitives_DrawText((aui_DirectSurface *)surf, x+100, y, (MBCHAR *)str2, 3050, 1) ;
	    }

    sprintf (str2, "%08X %08X %08X %08X  %4.2lf", m_crc[t][CRC_ARRAY_0], m_crc[t][CRC_ARRAY_1], m_crc[t][CRC_ARRAY_2], m_crc[t][CRC_ARRAY_3], (double)(m_time[t]) / CLOCKS_PER_SEC) ;
 	primitives_DrawText((aui_DirectSurface *)surf, x+125, y, (MBCHAR *)str2, 0, 0) ;
	}









void DataCheck::draw_time(aui_Surface *surf, sint32 x, sint32 y) const
	{
    MBCHAR	s[80] ;

    sprintf(s, "Total time %4.2lf", (double)(m_total_time) / CLOCKS_PER_SEC) ;
 	primitives_DrawText((aui_DirectSurface *)surf, x, y, s, 0, 0) ;
	}

	







void DataCheck::DisplayCRC(aui_Surface *surf) const 
	{
    sint32	x=100,
			y=80,
			d=20 ;

    if (!m_is_display)
        return ;

	d=16 ;
	if(m_is_display > 1) {
		draw_crc(surf, "DB", CRC_TYPE_DB, x, y);
		y+=d;
		draw_crc(surf, "PROFILE", CRC_TYPE_PROFILE_DB, x, y) ;
		y+=d;
		draw_crc(surf, "STRING", CRC_TYPE_STRING_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "ADVANCE", CRC_TYPE_ADVANCE_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "UNIT", CRC_TYPE_UNIT_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "WONDER", CRC_TYPE_WONDER_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "GOV", CRC_TYPE_GOVERNMENT_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "TERRAIN", CRC_TYPE_TERRAIN_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "POP", CRC_TYPE_POPULATION_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "CONST", CRC_TYPE_CONST_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "SPRITE", CRC_TYPE_SPRITESTATE_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "PROJECTILE", CRC_TYPE_PROJECTILESTATE_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "PROJECTILEEND", CRC_TYPE_PROJECTILEENDSTATE_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "DIFFICULTY", CRC_TYPE_DIFFICULTY_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "IMPROVE", CRC_TYPE_IMPROVEMENTS_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "GW", CRC_TYPE_GLOBAL_WARMING_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "UV", CRC_TYPE_OZONE_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "CIV", CRC_TYPE_CIVILISATION_DB, x, y) ;
		y+=d ;
		draw_crc(surf, "INSTALLATION", CRC_TYPE_INSTALLATION_DB, x, y) ;
  
		y += 25 ;
	}


    draw_crc(surf, "GLOBAL", CRC_TYPE_GLOBAL, x, y) ;
	x+=300 ;
	d=16 ;
    y += d ;
    draw_crc(surf, "RAND", CRC_TYPE_RAND, x, y) ;
    y += d ;
    draw_crc(surf, "WORLD", CRC_TYPE_WORLD, x, y) ;
    y += d ;
    draw_crc(surf, "PLAYER", CRC_TYPE_PLAYER, x, y) ;
    y += d ;
    draw_crc(surf, "SELECTED", CRC_TYPE_SELECTED_ITEM, x, y) ;
    y += d ;
    draw_crc(surf, "UNITPOOL", CRC_TYPE_UNITPOOL, x, y) ;
    y += d ;
    draw_crc(surf, "POLLUTION", CRC_TYPE_POLLUTION, x, y) ;
    y += d ;
    draw_crc(surf, "DIPLOMACY", CRC_TYPE_DIPLOMATICREQUESTPOOL, x, y) ;
    y += d ;
    draw_crc(surf, "MESSAGE", CRC_TYPE_MESSAGEPOOL, x, y) ;
    y += d ;
    draw_crc(surf, "CIVILISATION", CRC_TYPE_CIVILISATIONPOOL, x, y) ;
    y += d ;
    draw_crc(surf, "TRADE", CRC_TYPE_TRADEPOOL, x, y) ;
    y += d ;
    draw_crc(surf, "TRADEOFFER", CRC_TYPE_TRADEOFFERPOOL, x, y) ;
    y += d ;
    draw_crc(surf, "AGREEMENT", CRC_TYPE_AGREEMENTPOOL, x, y) ;
    y += d ;
    draw_crc(surf, "TERRAINIMPROVEMENT", CRC_TYPE_TERRAIN_IMPROVEMENT_POOL, x, y) ;
    y += d ;
    draw_crc(surf, "INSTALLATION", CRC_TYPE_INSTALLATION_POOL, x, y) ;

	y += d ;
	draw_time(surf, x, y) ;
	}

void DataCheck::DumpSingleCRC(MBCHAR *grp, sint32 t)
	{
	DPRINTF(k_DBG_INFO, ("%s     %08X %08X %08X %08X  %4.2lf\n", grp, m_crc[t][CRC_ARRAY_0], m_crc[t][CRC_ARRAY_1], m_crc[t][CRC_ARRAY_2], m_crc[t][CRC_ARRAY_3], (double)(m_time[t]) / CLOCKS_PER_SEC)) ;
	}









void DataCheck::DumpChecksum(void)
	{
	DumpSingleCRC("DB", CRC_TYPE_DB) ;
	DumpSingleCRC("PROFILE", CRC_TYPE_PROFILE_DB) ;
	DumpSingleCRC("STRING", CRC_TYPE_STRING_DB) ;
	DumpSingleCRC("ADVANCE", CRC_TYPE_ADVANCE_DB) ;
	DumpSingleCRC("UNIT", CRC_TYPE_UNIT_DB) ;
	DumpSingleCRC("WONDER", CRC_TYPE_WONDER_DB) ;
	DumpSingleCRC("GOV", CRC_TYPE_GOVERNMENT_DB) ;
	DumpSingleCRC("TERRAIN", CRC_TYPE_TERRAIN_DB) ;
	DumpSingleCRC("POP", CRC_TYPE_POPULATION_DB) ;
	DumpSingleCRC("CONST", CRC_TYPE_CONST_DB) ;
	DumpSingleCRC("SPRITE", CRC_TYPE_SPRITESTATE_DB) ;
	DumpSingleCRC("PROJECTILE", CRC_TYPE_PROJECTILESTATE_DB) ;
	DumpSingleCRC("PROJECTILEEND", CRC_TYPE_PROJECTILEENDSTATE_DB) ;
	DumpSingleCRC("DIFFICULTY", CRC_TYPE_DIFFICULTY_DB) ;
	DumpSingleCRC("IMPROVE", CRC_TYPE_IMPROVEMENTS_DB) ;
	DumpSingleCRC("GW", CRC_TYPE_GLOBAL_WARMING_DB) ;
	DumpSingleCRC("UV", CRC_TYPE_OZONE_DB) ;
	DumpSingleCRC("INSTALLATION", CRC_TYPE_INSTALLATION_DB) ;
	DumpSingleCRC("CIV", CRC_TYPE_CIVILISATION_DB) ;
	DumpSingleCRC("GLOBAL", CRC_TYPE_GLOBAL) ;
	DumpSingleCRC("RAND", CRC_TYPE_RAND) ;
	DumpSingleCRC("WORLD", CRC_TYPE_WORLD) ;
	DumpSingleCRC("PLAYER", CRC_TYPE_PLAYER) ;
	DumpSingleCRC("SELECTED", CRC_TYPE_SELECTED_ITEM) ;
	DumpSingleCRC("UNITPOOL", CRC_TYPE_UNITPOOL) ;
	DumpSingleCRC("POLLUTION", CRC_TYPE_POLLUTION) ;
	DumpSingleCRC("DIPLOMACY", CRC_TYPE_DIPLOMATICREQUESTPOOL) ;
	DumpSingleCRC("MESSAGE", CRC_TYPE_MESSAGEPOOL) ;
	DumpSingleCRC("CIVILISATION", CRC_TYPE_CIVILISATIONPOOL) ;
	DumpSingleCRC("TRADE", CRC_TYPE_TRADEPOOL) ;
	DumpSingleCRC("TRADEOFFER", CRC_TYPE_TRADEOFFERPOOL) ;
	DumpSingleCRC("AGREEMENT", CRC_TYPE_AGREEMENTPOOL) ;
	DumpSingleCRC("TERRAINIMPROVEMENT", CRC_TYPE_TERRAIN_IMPROVEMENT_POOL) ;
	DumpSingleCRC("INSTALLATION", CRC_TYPE_INSTALLATION_POOL) ;

	DPRINTF(k_DBG_INFO, ("Total time %4.2lf\n", (double)(m_total_time) / CLOCKS_PER_SEC)) ;
	}
