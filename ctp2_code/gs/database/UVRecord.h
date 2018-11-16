#if 0

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __UV_RECORD_H__
#define __UV_RECORD_H__

#include "Rec.h"
class Token;

#define MAX_RESOURCES	64

enum DEAD_TILE_TYPE
	{
	DEAD_TILE_TYPE_DESERT,
	DEAD_TILE_TYPE_FORESTS,
	DEAD_TILE_TYPE_GRASSLANDS,
	DEAD_TILE_TYPE_HILLS,
	DEAD_TILE_TYPE_MOUNTAINS,
	DEAD_TILE_TYPE_PLAINS,
	DEAD_TILE_TYPE_SWAMPS,
	DEAD_TILE_TYPE_TUNDRA,
	DEAD_TILE_TYPE_CONTINENTAL_SHELF,
	DEAD_TILE_TYPE_MAX
	} ;

enum DEAD_RESOURCE_TYPE
	{
	DEAD_RESOURCE_TYPE_PALM_OIL,
	DEAD_RESOURCE_TYPE_OIL,
	DEAD_RESOURCE_TYPE_PHEASANT,
	DEAD_RESOURCE_TYPE_SILK,
	DEAD_RESOURCE_TYPE_SPICE,
	DEAD_RESOURCE_TYPE_TUSKS,
	DEAD_RESOURCE_TYPE_BLUBBER,
	DEAD_RESOURCE_TYPE_ALIEN_CORPSES,
	DEAD_RESOURCE_TYPE_CLAY,
	DEAD_RESOURCE_TYPE_TOBACCO,
	DEAD_RESOURCE_TYPE_COTTON,
	DEAD_RESOURCE_TYPE_COAL,
	DEAD_RESOURCE_TYPE_GRAPES,
	DEAD_RESOURCE_TYPE_GRANITE,
	DEAD_RESOURCE_TYPE_BANANAS,
	DEAD_RESOURCE_TYPE_JADE,
	DEAD_RESOURCE_TYPE_HARDWOOD,
	DEAD_RESOURCE_TYPE_IRON,
	DEAD_RESOURCE_TYPE_GOLD,
	DEAD_RESOURCE_TYPE_SILVER,
	DEAD_RESOURCE_TYPE_BUFFALO,
	DEAD_RESOURCE_TYPE_WHEAT,
	DEAD_RESOURCE_TYPE_CORN,
	DEAD_RESOURCE_TYPE_PEAT,
	DEAD_RESOURCE_TYPE_MANGROVE,
	DEAD_RESOURCE_TYPE_MINK,
	DEAD_RESOURCE_TYPE_CARIBOU,
	DEAD_RESOURCE_TYPE_FISH,
	DEAD_RESOURCE_TYPE_WHALES,
	DEAD_RESOURCE_TYPE_CRABS,
	DEAD_RESOURCE_TYPE_DIAMONDS,
	DEAD_RESOURCE_TYPE_MAGNESIUM_NODULES,
	DEAD_RESOURCE_TYPE_PHOSPHORUS,
	DEAD_RESOURCE_TYPE_PLANKTON_UPWELLING,
	DEAD_RESOURCE_TYPE_GIANT_CLAM,
	DEAD_RESOURCE_TYPE_GIANT_SQUID,
	DEAD_RESOURCE_TYPE_MAX
	} ;

enum REGION_TYPE
	{
	REGION_TYPE_NORTH_POLAR,
	REGION_TYPE_UPPER_NORTH_MERIDIAN,
	REGION_TYPE_LOWER_NORTH_MERIDIAN,
	REGION_TYPE_EQUATORIAL,
	REGION_TYPE_UPPER_SOUTH_MERIDIAN,
	REGION_TYPE_LOWER_SOUTH_MERIDIAN,
	REGION_TYPE_SOUTH_POLAR,
	REGION_TYPE_MAX,
	} ;




class CivArchive ;

class Token ;

class UVRecord : public Record
	{
	private:


		double	m_meridian_prob[k_NUM_MERIDIANS+1],
				m_meridian_phase_bonus[k_NUM_MERIDIANS+1] ;

		sint32	m_dead_food[MAX_RESOURCES],
				m_dead_shield[MAX_RESOURCES],
				m_dead_trade[MAX_RESOURCES],
				m_dead_gold[MAX_RESOURCES] ;
		sint32 m_dummy;


	public:









		UVRecord() ;
		~UVRecord() ;

		double GetBaseProb(const sint32 region) const
			{
			Assert((region>=0) && (region<=k_NUM_MERIDIANS)) ;
			return (m_meridian_prob[region]) ;
			}

		double GetBonus(const sint32 region) const
			{
			Assert((region>=0) && (region<=k_NUM_MERIDIANS)) ;
			return (m_meridian_phase_bonus[region]) ;
			}

		double GetProb(const sint32 phase, const sint32 region) const
			{
			Assert(phase>=0) ;
			Assert((region>=0) && (region<=k_NUM_MERIDIANS)) ;
			return (m_meridian_prob[region] + m_meridian_phase_bonus[region] * phase) ;
			}

		sint32 ParseUVTriggerRecord(Token *uvToken) ;
		sint32 ParseUVDeadTilesRecord(Token *uvToken) ;
		sint32 ParseUVDeadResourcesRecord(Token *uvToken) ;
		sint32 ParseDeadTileNumbers(Token *uvToken, sint32 &food, sint32 &shield, sint32 &trade) ;
		sint32 ParseDeadResourceNumbers(Token *uvToken, sint32 &food, sint32 &shield, sint32 &trade, sint32 &gold) ;
		void Serialize(CivArchive &archive) ;

	} ;

#endif
#endif
