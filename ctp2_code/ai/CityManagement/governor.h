//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : AI and automated governor handling.
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
// CTP1_HAS_RISEN_FROM_THE_GRAVE
// - When defined, does not use the CTP2 worker utilisation style.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added terrain boni arguments to FindBestTileImprovement to record the 
//   already given terrain boni for that city, by Martin Gühmann
// - Added GetBestTerraformImprovement function to find the best terraform
//   improvement. - Sep. 21st 2004 Martin Gühmann 
// - Cleaned up GetBestTerraformImprovement function. - Oct. 6th 2004 Martin Gühmann 
// - Replaced ComputeMinimumFoodWorkers by ComputeMinimumWorkers function.
//   - April 4th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#pragma once
#ifndef __GOVERNOR_H__
#define __GOVERNOR_H__



#pragma warning(disable: 4786)


#include <vector>
#include "c3debugstl.h"


#include "Diplomat.h"
#include "MapGrid.h"
#include "Path.h"

class CityData;
class BuildingBuildListRecord;
class WonderBuildListRecord;
class StrategyRecord::PopAssignmentElement;

class Governor {
public:

	
	
	

#ifdef _DEBUG
	
	typedef std::vector<Governor, dbgallocator<Governor> > GovernorVector;
	typedef std::vector<sint16, dbgallocator<sint16> > UnitCountVector;
#else
	
	typedef std::vector<Governor> GovernorVector;
	typedef std::vector<sint16> UnitCountVector;
#endif

	
	static void ResizeAll(const PLAYER_INDEX & newMaxPlayerId);

	
	static void LoadAll(CivArchive & archive);

	
	static void SaveAll(CivArchive & archive);

	
	static Governor & GetGovernor(const PLAYER_INDEX & playerId);

	
	
	enum BUILD_UNIT_LIST {
		BUILD_UNIT_LIST_AIR,
		BUILD_UNIT_LIST_SEA,
		BUILD_UNIT_LIST_FREIGHT,
		BUILD_UNIT_LIST_SPECIAL,
		BUILD_UNIT_LIST_SEA_TRANSPORT,
		BUILD_UNIT_LIST_AIR_TRANSPORT,
		BUILD_UNIT_LIST_SETTLER,
		
		BUILD_UNIT_LIST_RANGED,	  
		BUILD_UNIT_LIST_OFFENSE,
		BUILD_UNIT_LIST_DEFENSE,
		BUILD_UNIT_LIST_MAX
	};

	
	Governor();

	
	void Initialize();

	
	void SetPlayerId(const PLAYER_INDEX &playerId);

	void Resize( const sint16 & xSize,
				 const sint16 & ySize,
				 const sint16 & resolution );

	
	void Load(CivArchive & archive);

	
	void Save(CivArchive & archive) const;


	
	
	

	
	sint32 ComputeBestGovernment() const;

	
	StringId GetGovernmentAdvice() const;
	
	
	
	

	struct SlidersSetting {
		SlidersSetting() 
		{
			
			m_deltaProduction = m_deltaGold = m_deltaFood = 0;
			m_optimizeProduction = m_optimizeGold = m_optimizeFood = false;
		}
		const SlidersSetting & operator=(const SlidersSetting & rval)
		{
			m_deltaProduction = rval.m_deltaProduction;
			m_deltaGold = rval.m_deltaGold;
			m_deltaFood = rval.m_deltaFood;
			m_optimizeProduction = m_optimizeProduction;
			m_optimizeGold = m_optimizeGold;
			m_optimizeFood = m_optimizeFood;
			return *this;
		}

		bool operator==(const SlidersSetting & rval) const
		{
			return (m_deltaProduction == rval.m_deltaProduction  &&
					m_deltaGold == rval.m_deltaGold &&
					m_deltaFood == rval.m_deltaFood );
		}
		bool operator!=(const SlidersSetting & rval) const
		{
			return !(*this==rval);
		}

		
		sint32 m_deltaProduction;  
		sint32 m_deltaGold;		   
		sint32 m_deltaFood;		   
		
		bool m_optimizeProduction;
		bool m_optimizeGold;
		bool m_optimizeFood;

	};

	
	void NormalizeSliders(SlidersSetting & sliders_setting) const;

	
	sint32 SetSliders(const SlidersSetting & sliders_setting, const bool & update_cities) const;

	
	void GetSliders(SlidersSetting & sliders_setting) const;

	
	
	bool ComputeMinimumSliders( SlidersSetting & sliders_setting ) const;

	
	bool ComputeBestSliders( SlidersSetting & sliders_setting ) const;

	
	StringId GetSlidersAdvice() const;

	
	
	

	
	void AddRoadPriority(Path & path, const double & priority_delta);

	
	void ComputeRoadPriorities();

	
	void PlaceTileImprovements();

	
	
	

	
	void AssignPopulations();

	
	void AssignPopulation(CityData *city);

	
	void ComputeMinMaxEntertainers(const CityData *city, sint32 & min, sint32 & max);


	sint32 Governor::ComputeMinimumWorkers(const CityData *city, 
	                                       sint32 &farmers, 
	                                       sint32 &laborers, 
	                                       sint32 &merchants, 
	                                       sint32 &scientists,
	                                       sint32 &minFood,
	                                       sint32 &minProd,
	                                       sint32 &minGold,
	                                       sint32 &minScie,
	                                       double &farmersEff,
	                                       double &laborersEff,
	                                       double &merchantsEff,
	                                       double &scientistsEff);
	

	
	void ComputeDesiredUnits();

	
	void FillEmptyBuildQueues();

	
	double PercentUnbuilt(const BUILD_UNIT_LIST unit_list) const;

	
	StringId GetCityBuildQueueAdvice(const CityData *city) const;

	
	StringId GetUnitsAdvice(SlicContext & sc) const;

	
	
	

	
	sint32 ComputeBestMilitaryReadiness() const;

	
	StringId GetTacticalAdvice(SlicContext & sc) const;

	
	
	

	
	void ManageGoodsTradeRoutes();

private:
	
	static GovernorVector s_theGovernors;

	
	
	

	struct BuildUnitList {
		BuildUnitList() {
			m_bestType = -1;
			m_desiredCount = -1;
			m_maximumCount = -1;
			m_garrisonCount = -1;
			m_perCityGarrison = -1;
		}
		sint32 m_bestType;
		sint16 m_desiredCount;
		sint16 m_maximumCount;
		sint16 m_garrisonCount;
		sint16 m_perCityGarrison;
	};

	
	sint32 m_maximumUnitShieldCost;

	
	sint32 m_currentUnitShieldCost;

	
	bool FitSlidersToCities( SlidersSetting & sliders_setting ) const;

	
	bool TestSliderSettings( const SlidersSetting & sliders_setting,
							   bool & production_test,
							   bool & gold_test,
							   bool & food_test,
							   bool & happiness_test) const;

	
	void ComputeNextBuildItem(CityData *city, sint32 & cat, sint32 & type, sint32 & list_num) const;

	
	const BuildListSequenceRecord * GetMatchingSequence(const CityData *city, const bool human_city, StringId & advice) const;

	
	sint32 Governor::GetNeededUnitType(const CityData *city, sint32 & list_num) const;

	
	const UnitBuildListRecord * GetBuildListRecord(const StrategyRecord & strategy, const BUILD_UNIT_LIST list_type) const;

	
	sint32 GetNeededGarrisonUnitType(const CityData * city, sint32 & list_num) const;

	
	sint32 GetNeededBuildingType(const CityData *city, const BuildingBuildListRecord *build_list_rec ) const;

	
	sint32 GetNeededWonderType(const CityData *city, const WonderBuildListRecord *build_list_rec ) const;

	
	sint32 GetNeededFreightType(sint32 & list_num) const;

	
	sint32 ComputeBestUnitType(const UnitBuildListRecord *build_list_rec, const CityData *city = NULL) const;
	
	
	
	

	
	const StrategyRecord::PopAssignmentElement *GetMatchingPopAssignment(const CityData *city) const;

	
	
	

	
	struct TiGoal {
		TiGoal() { utility = -1.0; }
		bool operator>(const TiGoal & rval) const { return ( utility > rval.utility ); }
		MapPoint pos;
		double utility;
		sint32 type;
	};

#ifdef _DEBUG
	
	typedef std::vector<TiGoal, dbgallocator<TiGoal> > TiGoalQueue;
#else
	
	typedef std::vector<TiGoal> TiGoalQueue;
#endif
	
	
	static TiGoalQueue s_tiQueue;

	
	
	bool FindBestTileImprovement(const MapPoint &pos, TiGoal &goal, sint32 &bonusFood, sint32 &bonusProduction, sint32 &bonusCommerce) const;

	
	sint32 GetBestRoadImprovement(const MapPoint & pos) const;

	
	void GetBestFoodProdGoldImprovement(const MapPoint & pos, sint32 & food_imp, sint32 & prod_imp, sint32 & gold_imp) const;

	void GetBestTerraformImprovement(const MapPoint & pos, sint32 & food_imp, sint32 & prod_imp, sint32 & gold_imp, bool pwPerBonus) const;
	
	
	PLAYER_INDEX m_playerId;

	
	BuildUnitList m_buildUnitList[BUILD_UNIT_LIST_MAX];

	
	UnitCountVector m_currentUnitCount;

	
	

	
	double m_neededFreight;

};

#endif  __GOVERNOR_H__
