













#pragma once
#ifndef __DATACHECK_H__
#define __DATACHECK_H__



class aui_Surface; 


enum CRC_ARRAY
	{
	CRC_ARRAY_0,
	CRC_ARRAY_1,
	CRC_ARRAY_2,
	CRC_ARRAY_3,
	CRC_ARRAY_MAX
	} ;



enum CRC_TYPE
	{
	CRC_TYPE_MIN,
	CRC_TYPE_WORLD = CRC_TYPE_MIN,
	CRC_TYPE_GLOBAL,
    CRC_TYPE_RAND,
	CRC_TYPE_DB,
	CRC_TYPE_PROFILE_DB,
	CRC_TYPE_STRING_DB,
	CRC_TYPE_ADVANCE_DB,
	CRC_TYPE_UNIT_DB,
	CRC_TYPE_WONDER_DB,
	CRC_TYPE_GOVERNMENT_DB,
	CRC_TYPE_TERRAIN_DB,
	CRC_TYPE_POPULATION_DB,
	CRC_TYPE_CONST_DB,
	CRC_TYPE_SPRITESTATE_DB,
	CRC_TYPE_PROJECTILESTATE_DB,
	CRC_TYPE_PROJECTILEENDSTATE_DB,
	CRC_TYPE_DIFFICULTY_DB,
	CRC_TYPE_IMPROVEMENTS_DB,
	CRC_TYPE_PLAYER,
    CRC_TYPE_SELECTED_ITEM,
	CRC_TYPE_UNITPOOL,
	CRC_TYPE_TRADEPOOL,
	CRC_TYPE_TRADEOFFERPOOL,
	CRC_TYPE_POLLUTION_DB,
	CRC_TYPE_GLOBAL_WARMING_DB,
	CRC_TYPE_OZONE_DB,
	CRC_TYPE_CIVILISATION_DB,
	CRC_TYPE_POLLUTION,
	CRC_TYPE_TOPTEN,
	CRC_TYPE_DIPLOMATICREQUESTPOOL,
	CRC_TYPE_MESSAGEPOOL,
	CRC_TYPE_CIVILISATIONPOOL,
	CRC_TYPE_AGREEMENTPOOL,
	CRC_TYPE_INSTALLATION_DB,
	CRC_TYPE_TERRAIN_IMPROVEMENT_POOL,
	CRC_TYPE_INSTALLATION_POOL,
	CRC_TYPE_MAX,
	} ;


class DataCheck
	{
    private:
		sint32	m_is_display ;										
    
	public:
		uint32	m_crc[CRC_TYPE_MAX][CRC_ARRAY_MAX],					
				m_old_crc[CRC_TYPE_MAX][CRC_ARRAY_MAX],			
				m_time[CRC_TYPE_MAX],
				m_total_time ;
    
	    DataCheck() ;
    
		sint32 IsChanged(sint32 t) const ;							
		sint32 IsWorldChanged () const ;
		sint32 IsGlobalChanged () const ;
		sint32 IsRandChanged () const ;
		sint32 IsDBChanged () const ;								
		sint32 IsPlayerChanged () const ;
	    sint32 IsPopChanged () const ;
		sint32 IsUnitChanged () const ;

		void BeginTurn(void) ;
		BOOL GetCRC(CRC_TYPE group, uint32 &a, uint32 &b, uint32 &c, uint32 &d) ;	
		void SetDisplay(sint32 val) ;								
		void DisplayCRC(aui_Surface *surf) const ;					
		void DumpChecksum(void) ;
		void DumpSingleCRC(MBCHAR *grp, sint32 t) ;
		void draw_crc(aui_Surface *surf, char str1[80], sint32 t, sint32 x, sint32 y) const ;
		void draw_time(aui_Surface *surf, sint32 x, sint32 y) const ;
	} ;

void DataCheck_Init() ;
void DataCheck_Requiem() ;

extern DataCheck *g_dataCheck ;

#endif
