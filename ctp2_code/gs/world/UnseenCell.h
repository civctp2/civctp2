//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Map fog of war visibility handling
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
// - Added new functions to calculate the food, shields and gold values produced 
//   at the storing time of this UnseenCell. - Dec. 22nd 2004 Martin Gühmann
// - Added visible city oner flag and get method to be able to figure out
//   the city in whose radius the tile was at the last visit.
//   - Dec. 26th 2994 - Martin Gühmann
//
//----------------------------------------------------------------------------

#pragma once
#ifndef _UNSEEN_CELL_H_
#define _UNSEEN_CELL_H_

#include "MapPoint.h"

class TileInfo;
class CivArchive;
typedef sint32 TERRAIN_IMPROVEMENT;
typedef sint32 PLAYER_INDEX;
template <class T> class PointerList;
class UnitActor;

class UnseenImprovementInfo {
public:
	UnseenImprovementInfo(TERRAIN_IMPROVEMENT type,
						  sint32 percentComplete) {
		m_type = type;
		m_percentComplete = percentComplete;
	}
	UnseenImprovementInfo(CivArchive &archive);
	UnseenImprovementInfo(UnseenImprovementInfo *copy) {
		*this = *copy;
	}

	TERRAIN_IMPROVEMENT m_type;
	sint32 m_percentComplete;
};

class UnseenInstallationInfo {
public:
	UnseenInstallationInfo(sint32 type, uint32 visibility) {
		m_type = type;
		m_visibility = visibility;
	}
	UnseenInstallationInfo(CivArchive &archive);
	UnseenInstallationInfo(UnseenInstallationInfo *copy)
	{
		*this = *copy;
	}

	sint32 m_type; 
	uint32 m_visibility; 
};

#define k_UCF_IS_BIOINFECTED			0x0001
#define k_UCF_IS_NANOINFECTED			0x0002
#define k_UCF_IS_CONVERTED				0x0004
#define k_UCF_IS_FRANCHISED				0x0008
#define k_UCF_IS_INJOINED				0x0010
#define k_UCF_WAS_HAPPINESS_ATTACKED	0x0020
#define k_UCF_HAS_HUT					0x0040
#define k_UCF_IS_RIOTING				0x0080
#define k_UCF_HAS_AIRPORT				0x0100
#define k_UCF_HAS_SLEEPINGUNITS			0x0200
#define k_UCF_IS_WATCHFUL				0x0400



class UnseenCell
{
	friend class NetUnseenCell;

public:
	uint32 m_env;

	sint16 m_terrain_type;
	sint16 m_move_cost;













	
	
	
	uint16	m_flags;
	sint8	m_bioInfectedOwner;
	sint8	m_nanoInfectedOwner;
	
	sint8	m_convertedOwner;
	sint8	m_franchiseOwner;
	sint8	m_injoinedOwner;
	sint8	m_happinessAttackOwner;
	
	sint16	m_citySize;
	sint16	m_cityOwner;
	sint16  m_citySpriteIndex;









	sint8 m_cell_owner;

	
	
	
	uint32	m_slaveBits;	

private:
#ifdef BATTLE_FLAGS
	uint16 m_battleFlags;
#endif

	TileInfo *m_tileInfo;
	MapPoint m_point;
	
	PointerList<UnseenInstallationInfo> *m_installations;
	PointerList<UnseenImprovementInfo> *m_improvements;




	MBCHAR *m_cityName;
	UnitActor *m_actor;

	sint32 m_poolIndex;
	// Contains the ID of the city that owns the tile.
	uint32  m_visibleCityOwner;
public:
	UnseenCell(const MapPoint &point);
	UnseenCell();
	UnseenCell(UnseenCell *old);
	UnseenCell(CivArchive &archive);
	~UnseenCell();
	
	

	void GetPos(MapPoint &pos) { pos = m_point; }
	uint32 GetEnv() { return m_env; }
	uint32 GetTerrainType() { return m_terrain_type; }
#ifdef BATTLE_FLAGS
	uint16 GetBattleFlags() { return m_battleFlags; }
#endif
	TileInfo *GetTileInfo() { return m_tileInfo; }
	PointerList<UnseenInstallationInfo> *GetInstallations() { return m_installations; }
	PointerList<UnseenImprovementInfo> *GetImprovements() { return m_improvements; }
	sint32 GetCityOwner() { return m_cityOwner; }
	sint32 GetCitySize() { return m_citySize; }
	uint32 GetVisibleCityOwner() { return m_visibleCityOwner; }
	const MBCHAR *GetCityName() { return m_cityName; }
	UnitActor *GetActor() { return m_actor; }

	BOOL	IsBioInfected(void)			{ return m_flags & k_UCF_IS_BIOINFECTED; }
	BOOL	IsNanoInfected(void)		{ return m_flags & k_UCF_IS_NANOINFECTED; }
	BOOL	IsConverted(void)			{ return m_flags & k_UCF_IS_CONVERTED; }
	BOOL	IsFranchised(void)			{ return m_flags & k_UCF_IS_FRANCHISED; }
	BOOL	IsInjoined(void)			{ return m_flags & k_UCF_IS_INJOINED; }
	BOOL	WasHappinessAttacked(void)	{ return m_flags & k_UCF_WAS_HAPPINESS_ATTACKED; }
	BOOL	HasHut(void)				{ return m_flags & k_UCF_HAS_HUT; }
	BOOL	IsRioting(void)				{ return m_flags & k_UCF_IS_RIOTING; }
	BOOL	HasAirport(void)			{ return m_flags & k_UCF_HAS_AIRPORT; }
	BOOL	HasSleepingUnits(void)		{ return m_flags & k_UCF_HAS_SLEEPINGUNITS; }
	BOOL	IsWatchful(void)			{ return m_flags & k_UCF_IS_WATCHFUL; }

	void	SetIsBioInfected(BOOL yes)			{ if (yes) m_flags |= k_UCF_IS_BIOINFECTED; else m_flags &= ~k_UCF_IS_BIOINFECTED;}
	void	SetIsNanoInfected(BOOL yes)			{ if (yes) m_flags |= k_UCF_IS_NANOINFECTED; else m_flags &= ~k_UCF_IS_NANOINFECTED;}
	void	SetIsConverted(BOOL yes)			{ if (yes) m_flags |= k_UCF_IS_CONVERTED; else m_flags &= ~k_UCF_IS_CONVERTED;}
	void	SetIsFranchised(BOOL yes)			{ if (yes) m_flags |= k_UCF_IS_FRANCHISED; else m_flags &= ~k_UCF_IS_FRANCHISED;}
	void	SetIsInjoined(BOOL yes)				{ if (yes) m_flags |= k_UCF_IS_INJOINED; else m_flags &= ~k_UCF_IS_INJOINED;}
	void	SetWasHappinessAttacked(BOOL yes)	{ if (yes) m_flags |= k_UCF_WAS_HAPPINESS_ATTACKED; else m_flags &= ~k_UCF_WAS_HAPPINESS_ATTACKED;}
	void	SetHasHut(BOOL yes)					{ if (yes) m_flags |= k_UCF_HAS_HUT; else m_flags &= ~k_UCF_HAS_HUT;}
	void	SetIsRioting(BOOL yes)				{ if (yes) m_flags |= k_UCF_IS_RIOTING; else m_flags &= ~k_UCF_IS_RIOTING;}
	void	SetHasAirport(BOOL yes)				{ if (yes) m_flags |= k_UCF_HAS_AIRPORT; else m_flags &= ~k_UCF_HAS_AIRPORT;}
	void	SetHasSleepingUnits(BOOL yes)		{ if (yes) m_flags |= k_UCF_HAS_SLEEPINGUNITS; else m_flags &= ~k_UCF_HAS_SLEEPINGUNITS;}
	void	SetIsWatchful(BOOL yes)				{ if (yes) m_flags |= k_UCF_IS_WATCHFUL; else m_flags &= ~k_UCF_IS_WATCHFUL; }






	sint32 IsAirfield(void);
	sint32 IsListeningPost(void);

	sint32 IsRadar(void);
	sint32 IsHealUnits(void);
	sint32 IsFort(void);

	uint32	GetSlaveBits(void) { return m_slaveBits; }	

// Added by Martin Gühmann to generate these pieces of information
// for hidden tiles correctly as well.

	sint32 GetFoodFromTerrain() const;
    sint32 GetFoodProduced() const;
	sint32 GetShieldsFromTerrain() const;
    sint32 GetShieldsProduced() const;
	sint32 GetGoldFromTerrain() const;
	sint32 GetGoldProduced() const;


	void Serialize(CivArchive &archive);
};

class UnseenCellCarton {
public:
	UnseenCellCarton(UnseenCell *unseenCell) {
		m_unseenCell = unseenCell;
	}
	UnseenCellCarton() {m_unseenCell = NULL;}

	void GetPos(MapPoint &pos) 
	{
		Assert(m_unseenCell);
		m_unseenCell->GetPos(pos);
	}

	void Castrate() {}
	void DelPointers() {}
	sint32 GetOwner() { return 0; }

	void Serialize(CivArchive &) {};

	UnseenCell *m_unseenCell;
};

#endif
