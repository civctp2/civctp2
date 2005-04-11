//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source file
// Description  : Gaia controller handling
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Marked MS version specific code.
// - Standardised list import.
//
//----------------------------------------------------------------------------

#ifndef __GAIA_CONTROLLER_H__
#define __GAIA_CONTROLLER_H__

#include "Bit_Table.h"
#include <list>
#include <utility>
#include "MapPoint.h"
#include "c3debugstl.h"

typedef std::list<MapPoint, dbgallocator<MapPoint> > MapPoint_List;
typedef std::list<std::pair<sint32, MapPoint>, dbgallocator<std::pair<sint32, MapPoint> > > Scored_MapPoint_List;


class MapPoint;
class Unit;

class GaiaController {
  public:
	
	static uint64 sm_endgameImprovements;
	static uint64 sm_endgameBuildings;
	static uint64 sm_endgameWonders;

	GaiaController(const PLAYER_INDEX player);

	
	
	~GaiaController();
	void Serialize(CivArchive &archive);

	
	static void InitializeStatics();

	void Initialize();

	
	void RecomputeCoverage();

	static void InitializeEvents();
	static void CleanupEvents();

	void HandleBuildingChange(const sint32 type, Unit & city, const sint16 delta);
	void HandleWonderChange(const sint32 type, const sint16 delta);
	void HandleTerrImprovementChange(const sint32 type, const MapPoint & pos, const sint16 delta);

	sint16 NumSatellitesLaunched() const;
	sint16 NumMainframesBuilt() const;
	sint16 NumTowersBuilt() const;
	sint16 NumWondersBuilt() const;
	sint16 MaxSatellitesAllowed() const;
	sint16 NumSatellitesRequired() const;
	sint16 NumMainframesRequired() const;
	sint16 NumTowersRequired() const;
	double TowerCoverageRequired() const;

	sint16 GetTowerRadius() const;

	
	float GetTowerCoverage() const;

	
	float NewCoverageFrom(const MapPoint & pos, const sint16 radius) const;

	
	const Bit_Table & GetCoverage() const;

	
	bool CanStartCountdown() const;

	bool HasReqTowerCoverage() const;
	bool HasMinTowersBuilt() const;
	bool HasMinCoresBuilt() const;
	bool HasMinSatsBuilt() const;
	bool HasMaxSatsBuilt() const;

	
	bool StartCountdown();

	
	sint32 TotalCountdownTurns() const;

	
	sint16 TurnsToComplete() const;

	
	bool GaiaControllerTileImp(const sint32 type) const;

	
	inline sint32 GetTowerTileImpIndex() const { return sm_towerTileImpIndex; }

	
	bool CanBuildTowers(const bool & check_pw) const;

	
	static sint32 GetMainframeBuildingIndex()
	{ return(sm_mainframeBuildingIndex); }

	
	static sint32 GetSatelliteBuildingIndex()
	{ return(sm_satelliteBuildingIndex); }

	
	
	void ComputeTowerPositions();

	
	float GetMaxTowerCoverage() const;

	
	bool PopNextTowerPosition(MapPoint & pos);

	
	void BuildProcessingTowers();

  private:
	
	static sint32 sm_towerEndgameIndex;
	static sint32 sm_satelliteEndgameIndex;
	static sint32 sm_mainframeEndgameIndex;
	static sint32 sm_towerTileImpIndex;
	static sint32 sm_satelliteBuildingIndex;
	static sint32 sm_mainframeBuildingIndex;

	
	sint32 ScoreTowerPosition(MapPoint & pos, const MapPoint empire_center, MapPoint_List & towers) const;

	
	void ComputeTowerCandidates(Scored_MapPoint_List & candidates) const;

	PLAYER_INDEX m_playerId;
	sint16 m_numMainframes;
	sint16 m_numSatellites;
	sint16 m_numTowersBuilt;
	sint16 m_numWondersBuilt;
	float m_percentCoverage;
	sint16 m_completedTurn;
	Bit_Table m_coveredCells;
	float m_maxPercentCoverage;
	MapPoint_List m_newTowerPositions;
};

#endif __GAIA_CONTROLLER_H__

