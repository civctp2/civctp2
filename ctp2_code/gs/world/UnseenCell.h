//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Map fog of war visibility handling
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
// BATTLE_FLAGS
//
// HAVE_PRAGMA_ONCE
// - Uses #pragma once preprocessor derective
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
// - added IsCapitol
// - added IsReligion 1-23-2006
//
//----------------------------------------------------------------------------
//
/// \file   UnseenCell.h
/// \brief  Handling of tiles that are not currently visible (declarations)

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef UNSEEN_CELL_H_
#define UNSEEN_CELL_H_

//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

// None

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

class UnseenCell;
class UnseenCellCarton;
class UnseenImprovementInfo;
class UnseenInstallationInfo;

#define k_UCF_IS_BIOINFECTED            0x0001
#define k_UCF_IS_NANOINFECTED           0x0002
#define k_UCF_IS_CONVERTED              0x0004
#define k_UCF_IS_FRANCHISED             0x0008
#define k_UCF_IS_INJOINED               0x0010
#define k_UCF_WAS_HAPPINESS_ATTACKED    0x0020
#define k_UCF_HAS_HUT                   0x0040
#define k_UCF_IS_RIOTING                0x0080
#define k_UCF_HAS_AIRPORT               0x0100
#define k_UCF_HAS_SLEEPINGUNITS         0x0200
#define k_UCF_IS_WATCHFUL               0x0400
#define k_UCF_IS_CAPITOL                0x0800
//#define k_UCF_BIT_13_UNUSED           0x1000
//#define k_UCF_BIT_14_UNUSED           0x2000
//#define k_UCF_BIT_15_UNUSED           0x4000
//#define k_UCF_BIT_16_UNUSED           0x8000

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

#include "civarchive.h"
#include "ctp2_inttypes.h"    // sintN, uintN
#include "MapPoint.h"         // MapPoint
#include "player.h"           // PLAYER_INDEX
#include "TerrImproveData.h"  // TERRAIN_IMPROVEMENT
// MBCHAR

template <class T> class PointerList;
class TileInfo;
class UnitActor;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class UnseenImprovementInfo 
{
public:
    UnseenImprovementInfo(TERRAIN_IMPROVEMENT type,
	                      sint32 percentComplete) 
    :
        m_type              (type),
        m_percentComplete   (percentComplete)
    {};

	UnseenImprovementInfo(CivArchive &archive);
	UnseenImprovementInfo(UnseenImprovementInfo *copy) {
		*this = *copy;
	}

	TERRAIN_IMPROVEMENT     m_type;
	sint32                  m_percentComplete;
};

class UnseenInstallationInfo 
{
public:
    UnseenInstallationInfo(sint32 type, uint32 visibility) 
    :
        m_type              (type),
        m_visibility        (visibility)
    {	};

	UnseenInstallationInfo(CivArchive &archive);
	UnseenInstallationInfo(UnseenInstallationInfo *copy)
	{
		*this = *copy;
	}

	sint32 m_type;
	uint32 m_visibility;
};




class UnseenCell
{
	friend class NetUnseenCell;

public:
	uint32  m_env;

	sint16  m_terrain_type;
	sint16  m_move_cost;













	
	
	
	uint16  m_flags;
	sint8   m_bioInfectedOwner;
	sint8   m_nanoInfectedOwner;
	
	sint8   m_convertedOwner;
	sint8   m_franchiseOwner;
	sint8   m_injoinedOwner;
	sint8   m_happinessAttackOwner;
	
	sint16  m_citySize;
	sint16  m_cityOwner;
	sint16  m_citySpriteIndex;









	sint8   m_cell_owner;

	
	
	
	uint32  m_slaveBits;

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
	/// The ID of the city that owns the tile.
	uint32 m_visibleCityOwner;
public:
	UnseenCell(const MapPoint &point);
	UnseenCell();
	UnseenCell(UnseenCell *old);
	UnseenCell(CivArchive &archive);
	~UnseenCell();
	
	

	void GetPos(MapPoint &pos) const { pos = m_point; }
	uint32 GetEnv() const { return m_env; }
	uint32 GetTerrainType() const { return m_terrain_type; }
#ifdef BATTLE_FLAGS
	uint16 GetBattleFlags() const { return m_battleFlags; }
#endif
	TileInfo *GetTileInfo() const { return m_tileInfo; }
	PointerList<UnseenInstallationInfo> *GetInstallations() const { return m_installations; }
	PointerList<UnseenImprovementInfo> *GetImprovements() const { return m_improvements; }
	sint32 GetCityOwner() const { return m_cityOwner; }
	sint32 GetCitySize() const { return m_citySize; }
	uint32 GetVisibleCityOwner() const { return m_visibleCityOwner; }
	const MBCHAR *GetCityName() const { return m_cityName; }
	UnitActor *GetActor() const { return m_actor; }

	bool    IsBioInfected        (void) const   { return 0 != (m_flags & k_UCF_IS_BIOINFECTED)         ; }
	bool    IsNanoInfected       (void) const   { return 0 != (m_flags & k_UCF_IS_NANOINFECTED)        ; }
	bool    IsConverted          (void) const   { return 0 != (m_flags & k_UCF_IS_CONVERTED)           ; }
	bool    IsFranchised         (void) const   { return 0 != (m_flags & k_UCF_IS_FRANCHISED)          ; }
	bool    IsInjoined           (void) const   { return 0 != (m_flags & k_UCF_IS_INJOINED)            ; }
	bool    WasHappinessAttacked (void) const   { return 0 != (m_flags & k_UCF_WAS_HAPPINESS_ATTACKED) ; }
	bool    HasHut               (void) const   { return 0 != (m_flags & k_UCF_HAS_HUT)                ; }
	bool    IsRioting            (void) const   { return 0 != (m_flags & k_UCF_IS_RIOTING)             ; }
	bool    HasAirport           (void) const   { return 0 != (m_flags & k_UCF_HAS_AIRPORT)            ; }
	bool    HasSleepingUnits     (void) const   { return 0 != (m_flags & k_UCF_HAS_SLEEPINGUNITS)      ; }
	bool    IsWatchful           (void) const   { return 0 != (m_flags & k_UCF_IS_WATCHFUL)            ; }
	bool    IsCapitol            (void) const   { return 0 != (m_flags & k_UCF_IS_CAPITOL)             ; }

	void    SetIsBioInfected        (bool yes)     { if (yes) m_flags |= k_UCF_IS_BIOINFECTED          ; else m_flags &= ~k_UCF_IS_BIOINFECTED         ; }
	void    SetIsNanoInfected       (bool yes)     { if (yes) m_flags |= k_UCF_IS_NANOINFECTED         ; else m_flags &= ~k_UCF_IS_NANOINFECTED        ; }
	void    SetIsConverted          (bool yes)     { if (yes) m_flags |= k_UCF_IS_CONVERTED            ; else m_flags &= ~k_UCF_IS_CONVERTED           ; }
	void    SetIsFranchised         (bool yes)     { if (yes) m_flags |= k_UCF_IS_FRANCHISED           ; else m_flags &= ~k_UCF_IS_FRANCHISED          ; }
	void    SetIsInjoined           (bool yes)     { if (yes) m_flags |= k_UCF_IS_INJOINED             ; else m_flags &= ~k_UCF_IS_INJOINED            ; }
	void    SetWasHappinessAttacked (bool yes)     { if (yes) m_flags |= k_UCF_WAS_HAPPINESS_ATTACKED  ; else m_flags &= ~k_UCF_WAS_HAPPINESS_ATTACKED ; }
	void    SetHasHut               (bool yes)     { if (yes) m_flags |= k_UCF_HAS_HUT                 ; else m_flags &= ~k_UCF_HAS_HUT                ; }
	void    SetIsRioting            (bool yes)     { if (yes) m_flags |= k_UCF_IS_RIOTING              ; else m_flags &= ~k_UCF_IS_RIOTING             ; }
	void    SetHasAirport           (bool yes)     { if (yes) m_flags |= k_UCF_HAS_AIRPORT             ; else m_flags &= ~k_UCF_HAS_AIRPORT            ; }
	void    SetHasSleepingUnits     (bool yes)     { if (yes) m_flags |= k_UCF_HAS_SLEEPINGUNITS       ; else m_flags &= ~k_UCF_HAS_SLEEPINGUNITS      ; }
	void    SetIsWatchful           (bool yes)     { if (yes) m_flags |= k_UCF_IS_WATCHFUL             ; else m_flags &= ~k_UCF_IS_WATCHFUL            ; }
	void    SetIsCapitol            (bool yes)     { if (yes) m_flags |= k_UCF_IS_CAPITOL              ; else m_flags &= ~k_UCF_IS_CAPITOL             ; }

	bool    IsAirfield      (void) const;
	bool    IsListeningPost (void) const;
	bool    IsRadar         (void) const;
	bool    IsHealUnits     (void) const;
	bool    IsFort          (void) const;

	uint32 GetSlaveBits(void) const { return m_slaveBits; }

	sint32 GetFoodFromTerrain() const;
	sint32 GetFoodProduced() const;
	sint32 GetShieldsFromTerrain() const;
	sint32 GetShieldsProduced() const;
	sint32 GetGoldFromTerrain() const;
	sint32 GetGoldProduced() const;


	void Serialize(CivArchive &archive);
};

class UnseenCellCarton
{
public:
    UnseenCellCarton(UnseenCell * unseenCell = NULL)
    :
        m_unseenCell    (unseenCell)
    { };

	void GetPos(MapPoint & pos) const
	{
		Assert(m_unseenCell);
		m_unseenCell->GetPos(pos);
	}

    // Stubs to enable use of QuadTree<UnseenCellCarton>
	void DelPointers() {}
    sint32 GetOwner() const { return 0; }

    UnseenCell *    m_unseenCell;
};

#endif
