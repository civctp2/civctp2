//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Goody hut handling
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
// - Update the display (rush buy buttons) when receiving gold.
// - Speeded up goody hut advance and unit selection.
// - Replaced old risk database by new one. (Aug 29th 2005 Martin Gühmann)
// - GoodyHutExcluded added to unit radomizer to prevent some units from appearing
//   by E 8-MAR-2006
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "GoodyHuts.h"

#include "c3math.h"         // AsPercentage
#include "player.h"
#include "RandGen.h"
#include "AICause.h"
#include "Unit.h"
#include "civarchive.h"
#include "Advances.h"
#include "CivPaths.h"       // g_civPaths
#include "SlicEngine.h"
#include "SlicObject.h"
#include "UnitDynArr.h"
#include "UnitData.h"
#include "soundmanager.h"   // g_soundManager
#include "gamesounds.h"
#include "network.h"
#include "tiledmap.h"
#include "Barbarians.h"
#include "profileDB.h"      // g_theProfileDB
#include "RiskRecord.h"
#include "AdvanceRecord.h"
#include "SelItem.h"
#include "GameSettings.h"
#include "Exclusions.h"
#include "UnitRecord.h"
#include "GameEventManager.h"

#include "MainControlPanel.h"

extern Player **g_player;
extern RandomGenerator *g_rand;
extern TiledMap		*g_tiledMap;

extern SelectedItem	*g_selected_item;

namespace
{
//----------------------------------------------------------------------------
//
// Name       : GoodyRiskData
//
// Description: Helper class to store risk settings for the goody computation.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
    class   GoodyRiskData
    {
    public:
//----------------------------------------------------------------------------
//
// Name       : GoodyRiskData::GoodyRiskData
//
// Description: Constructor
//
// Parameters : settings    : game settings
//              risks       : risk database
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
        GoodyRiskData
        (
            GameSettings const *        settings,
            CTPDatabase<RiskRecord> *   risks
        )
        {
	        sint32 const    gameRiskLevel   = 
                std::min<sint32>(settings->GetRisk(), risks->NumRecords() - 1);
            m_risk                  = risks->Get(gameRiskLevel);
            Assert(m_risk);

            m_BarbarianThreshold    = 
                AsPercentage(m_risk->GetHutChanceBarbarian());
            m_GoldThreshold         = m_BarbarianThreshold +
                AsPercentage(m_risk->GetHutChanceGold());
            m_AdvanceThreshold      = m_GoldThreshold +
                AsPercentage(m_risk->GetHutChanceAdvance());
            m_UnitThreshold         = m_AdvanceThreshold +
                AsPercentage(m_risk->GetHutChanceUnit());
            m_CityThreshold         = m_UnitThreshold +
                AsPercentage(m_risk->GetHutChanceCity());
            m_SettlerThreshold      = m_CityThreshold +
                AsPercentage(m_risk->GetHutChanceSettler());
        };

//----------------------------------------------------------------------------
//
// Name       : GoodyRiskData::Select
//
// Description: Generate a goody type from a random value
//
// Parameters : randomValue     
//
// Globals    : -
//
// Returns    : GOODY       : generated type of goody 
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
        GOODY   Select(sint32 randomValue) const
        {
            if (randomValue < m_BarbarianThreshold)
            {
                return GOODY_BARBARIANS;
            }
            else if (randomValue < m_GoldThreshold)
            {
                return GOODY_GOLD;
            }
            else if (randomValue < m_AdvanceThreshold)
            {
                return GOODY_ADVANCE;
            }
            else if (randomValue < m_UnitThreshold)
            {
                return GOODY_UNIT;
            }
            else if (randomValue < m_CityThreshold)
            {
                return GOODY_CITY;
            }
            else if (randomValue < m_SettlerThreshold)
            {
                return GOODY_SETTLER;
            }

            return GOODY_BOGUS;
        };

//----------------------------------------------------------------------------
//
// Name       : GoodyRiskData::GoldAmount
//
// Description: Generate an amount of gold from a random value
//
// Parameters : randomValue     
//
// Globals    : -
//
// Returns    : sint32      : generated amount of gold
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
        sint32  GoldAmount(sint32 randomValue) const
        {
            return m_risk->GetHutMinGold() +
                   (((m_risk->GetHutMaxGold() - m_risk->GetHutMinGold()) * randomValue) / 
                        k_VALUE_RANGE
                   );        
        }

//----------------------------------------------------------------------------
//
// Name       : GoodyRiskData::GetMaxAdvanceLeap
//
// Description: Get the number of prerequisites that may be skipped when
//              selecting a new advance.
//
// Parameters : -    
//
// Globals    : -
//
// Returns    : sint32      : number of prerequisites that may be skipped
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
        sint32  GetMaxAdvanceLeap(void) const
        {
            return m_risk->GetHutMaxAdvancePrerequisites();
        }

//----------------------------------------------------------------------------
//
// Name       : GoodyRiskData::GetMaxUnitAdvanceLeap
//
// Description: Get the number of prerequisites that may be skipped when
//              selecting a new unit.
//
// Parameters : -    
//
// Globals    : -
//
// Returns    : sint32      : number of prerequisites that may be skipped
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
        sint32  GetMaxUnitAdvanceLeap(void) const
        {
            return m_risk->GetHutMaxUnitPrerequisites();
        }

    private:
        RiskRecord const *  m_risk;
        int                 m_BarbarianThreshold;       
        int                 m_GoldThreshold;
        int                 m_AdvanceThreshold;
        int                 m_UnitThreshold;
        int                 m_CityThreshold;
        int                 m_SettlerThreshold;
    };

} // namespace

GoodyHut::GoodyHut()
{   
    m_typeValue = g_rand->Next(100); 
	m_value     = g_rand->Next(k_VALUE_RANGE);
}

GoodyHut::GoodyHut(uint32 type, uint32 value)
{
    m_typeValue = type;
    m_value     = value;
}

GoodyHut::GoodyHut(CivArchive &archive)
{
	Serialize(archive); // sets m_typeValue and m_value
}

//----------------------------------------------------------------------------
//
// Name       : GoodyHut::ChooseType
//
// Description: Generate a goody type for a player
//
// Parameters : owner       : the player     
//
// Globals    : g_theGameSettings
//              g_theRiskDB
//              g_player
//              g_theAdvanceDB
//              g_theUnitDB
//
// Returns    : GOODY       : generated type of goody 
//
// Remark(s)  : When no proper goody can be generated, GOODY_BOGUS is returned.
//              The barbarian player gets this result always.
//              Assumption: the owner is alive (g_player[owner] is non-NULL).
//
//----------------------------------------------------------------------------
GOODY GoodyHut::ChooseType(PLAYER_INDEX const & owner)
{
	DPRINTF(k_DBG_GAMESTATE, ("Choose ruin type for typeValue %d\n", m_typeValue));

	if (owner == PLAYER_INDEX_VANDALS)
	{
		return GOODY_BOGUS;
	}

	GoodyRiskData   risk    = GoodyRiskData(g_theGameSettings, g_theRiskDB);
	GOODY           result  = risk.Select(m_typeValue);

	switch (result)
	{
		default:
			// No special actions
			break;

		case GOODY_ADVANCE:
		{
			Advances const *    advances     = g_player[owner]->m_advances;
			AdvanceType *       possible     = 
			    new AdvanceType[g_theAdvanceDB->NumRecords()];
			size_t              nextPossible = 0;
			sint32 const        maxNovelty   = risk.GetMaxAdvanceLeap();

			for(AdvanceType i = 0; i < g_theAdvanceDB->NumRecords(); ++i)
			{
				if(advances->HasAdvance(i)) 
					continue;   // known

				if(g_theAdvanceDB->Get(i)->GetGoodyHutExcluded())
					continue;   // EMOD new flag to prevent some unts from appearing

				if(g_theAdvanceDB->Get(i)->GetNumPrerequisites() > 0
				&& g_theAdvanceDB->Get(i)->GetPrerequisitesIndex(0) == i)
					continue;   // undiscoverable

				//remove this and use advances->m_researching ??? EMOD

				if (advances->GetMinPrerequisites(i, maxNovelty) <= maxNovelty)
				{
					possible[nextPossible++] = i;
				}
				// else: too advanced
			}

			if (nextPossible) 
			{
				m_value = possible[(nextPossible * m_value) / k_VALUE_RANGE];
			}
			else
			{
				result  = GOODY_BOGUS;
			}

			delete [] possible;
		}
		break;

		case GOODY_GOLD:
			m_value = risk.GoldAmount(m_value);
			break;

		case GOODY_SETTLER:
		{
			for (size_t i = 0; i < static_cast<size_t>(g_theUnitDB->NumRecords()); ++i) 
			{
				if (g_theUnitDB->Get(i, g_player[owner]->GetGovernmentType())->GetSettleLand()) 
				{
					m_value = i;
					return GOODY_UNIT;
				}
			}

			result = GOODY_BOGUS;
		}
		break;

		case GOODY_UNIT:
		{
			Advances const *    advances     = g_player[owner]->m_advances;
			sint32 *            possible     = new sint32[g_theUnitDB->NumRecords()];
			size_t              nextPossible = 0;
			sint32 const        maxNovelty   = risk.GetMaxUnitAdvanceLeap();

			for (sint32 i = 0; i < g_theUnitDB->NumRecords(); ++i) 
			{
				UnitRecord const * rec = g_theUnitDB->Get(i, g_player[owner]->GetGovernmentType());

				if (!rec->GetMovementTypeLand())
					continue;   // would die immediately here
				if (rec->GetAttack() < 1)               
					continue;   // defenseless?
				if (rec->GetGoodyHutExcluded())
					continue;   // EMOD new flag to prevent some unts from appearing
				if (rec->GetHasPopAndCanBuild())
					continue;   // settler, handled separately
				if (g_exclusions->IsUnitExcluded(i))    
					continue;   // excluded (MP, mod)
				if (rec->GetNumGovernmentType() > 0) 
					continue;   // government specific?

                sint32 const    need = rec->GetEnableAdvanceIndex();
                if (need >= g_theAdvanceDB->NumRecords())
                    continue;   // invalid database input

                if (    (need < 0) // no EnablingAdvance defined for this unit
                     || (advances->GetMinPrerequisites(need, maxNovelty) <= maxNovelty)
				   )
				{
					possible[nextPossible++] = i;
				}
				// else : too advanced
			}

            if (nextPossible > 0)
			{
				m_value = possible[(nextPossible * m_value) / k_VALUE_RANGE];
			}
			else
			{
				result  = GOODY_BOGUS;
			}

			delete [] possible;

		}
		break;

	} // switch

	return result;
}

void GoodyHut::OpenGoody(PLAYER_INDEX const & owner, MapPoint const & point)
{
	SlicObject *so ;
	
	switch(ChooseType(owner)) {
		case GOODY_BOGUS:
		{
			so = new SlicObject("93BesetByNothing");
			so->AddRecipient(owner) ;
			g_slicEngine->Execute(so) ;
			DPRINTF(k_DBG_GAMESTATE, ("No soup for you!\n"));


			if (g_soundManager) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if (visiblePlayer == owner) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
											gamesounds_GetGameSoundID(GAMESOUNDS_DRAGDROP_FAIL),
											point.x,
											point.y);
				}
			}
			
			break;
		}
		case GOODY_CITY:
		{
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_CreateCity,
								   GEA_Player, owner,
								   GEA_MapPoint, point,
								   GEA_Int, CAUSE_NEW_CITY_GOODY_HUT,
								   GEA_Int, -1,
								   GEA_End);
#if 0
			Unit city = g_player[owner]->CreateCity(0,
										point,
										CAUSE_NEW_CITY_GOODY_HUT,
										NULL);
			if(city.m_id == 0) {
				so = new SlicObject("93BesetByNothing");
				so->AddRecipient(owner);
				g_slicEngine->Execute(so);
				break;
			}
			so = new SlicObject("80RuinBecomesCity") ;
			so->AddRecipient(owner) ;
			so->AddCity(city);
			g_slicEngine->Execute(so) ;
			DPRINTF(k_DBG_GAMESTATE, ("You get a city!\n"));

			
			
			


			if (g_soundManager) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if (visiblePlayer == owner) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
											gamesounds_GetGameSoundID(GAMESOUNDS_GOODY_CITY),
											point.x,
											point.y);
				}
			}
#endif
			break;
		}
		case GOODY_GOLD:
			so = new SlicObject("83DiscoveredAncientTreasure") ;
			so->AddRecipient(owner) ;
			so->AddGold(m_value) ;
			g_slicEngine->Execute(so) ;
			DPRINTF(k_DBG_GAMESTATE, ("You get %d gold!\n", m_value));
			g_player[owner]->AddGold(m_value);
			if (owner == g_selected_item->GetVisiblePlayer())
			{
				if (g_soundManager)
				{
					g_soundManager->AddSound(SOUNDTYPE_SFX, 
											 0, // no associated object
											 gamesounds_GetGameSoundID(GAMESOUNDS_TOOEXPENSIVE),
											 point.x,
											 point.y
											);
				}

				MainControlPanel::Update();	// the received gold may enable rush buying
			}
			break;
		case GOODY_ADVANCE:
			so = new SlicObject("79DiscoveredRemnantsOfAncientCivilisation") ;
			DPRINTF(k_DBG_GAMESTATE, ("You find advance %d\n", m_value));

            g_player[owner]->m_advances->GiveAdvance(m_value, CAUSE_SCI_GOODY);
			so->AddRecipient(owner);
			so->AddAdvance(m_value);
			g_slicEngine->Execute(so);

			if (g_soundManager) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if (visiblePlayer == owner) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
											gamesounds_GetGameSoundID(GAMESOUNDS_ADVANCE),
											point.x,
											point.y);
				}
			}
			break;
		case GOODY_UNIT:
		{
			if (g_theUnitDB->Get(m_value, g_player[owner]->GetGovernmentType())->GetSettle())
				so = new SlicObject("81NomadsHaveJoinedYourCivilisation") ;
			else
				so = new SlicObject("82MercenariesHaveJoinedYourCivilisation") ;

			so->AddRecipient(owner) ;
			g_slicEngine->Execute(so) ;
			DPRINTF(k_DBG_GAMESTATE, ("You get unit %d\n", m_value));
			Unit u = g_player[owner]->CreateUnit(m_value,
												 point,
												 Unit(),
												 FALSE,
												 CAUSE_NEW_ARMY_GOODY_HUT);








			if (g_soundManager) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if (visiblePlayer == owner) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
											gamesounds_GetGameSoundID(GAMESOUNDS_GOODY_UNIT),
											point.x,
											point.y);
				}
			}
			break;
		}	
		case GOODY_BARBARIANS:
			if(Barbarians::AddBarbarians(point, owner, TRUE)) {
				so = new SlicObject("84BesetByVandals") ;
				so->AddRecipient(owner);
				g_slicEngine->Execute(so);

				if (g_soundManager) {
					sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
					if (visiblePlayer == owner) {
						g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
												 gamesounds_GetGameSoundID(GAMESOUNDS_BOO),
												 point.x,
												 point.y);
					}	
				}
			} else {
				so = new SlicObject("93BesetByNothing");
				so->AddRecipient(owner) ;
				g_slicEngine->Execute(so) ;
				DPRINTF(k_DBG_GAMESTATE, ("No soup for you!\n"));
				
				
				if (g_soundManager) {
					sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
					if (visiblePlayer == owner) {
						g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
												 gamesounds_GetGameSoundID(GAMESOUNDS_DRAGDROP_FAIL),
												 point.x,
												 point.y);
					}
				}
			}
			break;
		default:
			Assert(FALSE);
			break;
	}

}

void GoodyHut::Serialize(CivArchive &archive)
{
	CHECKSERIALIZE

	if(archive.IsStoring()) {
		archive.StoreChunk((uint8 *)&m_value, ((uint8 *)&m_typeValue)+sizeof(m_typeValue));

	} else {
		archive.LoadChunk((uint8 *)&m_value, ((uint8 *)&m_typeValue)+sizeof(m_typeValue));

	}
}
