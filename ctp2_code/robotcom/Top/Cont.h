
#pragma once

#ifndef __CONTINENTS_H__
#define __CONTINENTS_H__ 1

class CivArchive;

class CityTree; 


class AiMain; 
class CityVertex; 
class CityAgent; 
struct MapPointData;


template <class T> class DynamicArray; 
typedef DynamicArray<sint32> DAsint32; 

class Continents { 

    sint32 m_num_land_cont;
    sint32 m_min_land_cont;

    sint32 *m_land_size;
    sint32 *m_land_cities; 
    sint32 *m_land_num_next_to_edge_cities;
    sint32 *m_land_armies; 
    sint32 *m_land_settlers; 
    sint32 *m_land_space_launchers; 
 
    sint32 m_num_water_cont; 
    sint32 m_min_water_cont;

    sint32 *m_water_size;
    sint32 *m_water_cities;
    sint32 *m_water_num_next_to_edge_cities;
    sint32 *m_water_armies; 
    sint32 *m_water_space_launchers; 

    CityTree **m_mst;

	
	
    DynamicArray<DAsint32> *m_water_next_too_land;
    DynamicArray<DAsint32> *m_land_next_too_water;

public:

    Continents(AiMain *ai); 
    Continents(CivArchive &archive);
    void InitMemory(AiMain *ai); 
    ~Continents(); 
    void CleanupMemory();


    sint32 GetNumLandCont() { return m_num_land_cont; } 
    sint32 GetNumWaterCont() { return m_num_water_cont; } 

    void Store(CivArchive &archive);
    void Load(CivArchive &archive);

    void Serialize(CivArchive &archive);
    void Clear(); 
    
    void Update(AiMain *ai);
    BOOL GetContinent(AiMain *ai, MapPointData &pos, sint32 &cont, BOOL &is_land);

    void IncLandSize(const sint32 idx); 
    void IncWaterSize(const sint32 idx); 


    void IncLandCities(const sint32 idx); 
    sint32 GetLandCities(const sint32 idx) const;     
    void IncLandNumNextToEdgeCities(const sint32 idx);
    sint32 GetLandNumNextToEdgeCities(const sint32 idx) const;

    void IncWaterCities(const sint32 idx); 
    sint32 GetWaterCities(const sint32 idx) const;     
    void IncWaterNumNextToEdgeCities(const sint32 idx);
    sint32 GetWaterNumNextToEdgeCities(const sint32 idx) const;

    void IncLandArmies(AiMain *ai, const sint32 idx, const uint32 a_id); 
    sint32 GetLandArmies(const sint32 idx) const; 
    sint32 GetLandSettlers(const sint32 idx) const;
    void IncWaterArmies(AiMain *ai, const sint32 idx, const uint32 a_id); 
    sint32 GetWaterArmies(const sint32 idx) const; 

    BOOL IsOcean(AiMain *ai, sint16 x, sint16 y, sint16 z);

    void InsertWaterNextToLand(const sint32 waterc, const sint32 landc); 
    void InsertLandNextToWater(const sint32 landc, const sint32 waterc); 
  
    BOOL LandShareWater(AiMain *ai, const sint32 land1, const sint32 land2) const; 

    CityTree * GetMst(const sint32 idx_cont); 
    void SetMst(const sint32 idx_const, CityTree *mst); 
    
    void RemoveCityReferance(CityAgent *remove_me);

    void IncLandSpaceLaunchers(const sint32 idx_cont); 
    void IncWaterSpaceLaunchers(const sint32 idx_cont); 

    BOOL LandGotSpaceLauncher(const sint32 idx_cont); 
    BOOL WaterGotSpaceLauncher(const sint32 idx_cont); 

    void FindContinentNeighbors(AiMain *ai);
    void FindAContinentNeighbor(AiMain *ai, MapPointData &pos, const sint32 v, 
      MapPointData &w, const BOOL is_land, const BOOL is_water);

    void RegisterNewContinents(AiMain *ai);

};

#endif __CONTINENTS_H__
