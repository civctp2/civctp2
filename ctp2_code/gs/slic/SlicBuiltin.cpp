//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Slic Built In 
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
// - Added a built in field for player to be able to get the the database
//   index of the player's current government, by Peter Triggs and 
//   Martin Gühmann.
// - Added three new globals by Martin Gühmann:
//   - g.num_of_players: Gets the number of players in the current game
//   - g.last_player:    Gets the last player, the player with the highest 
//                       index in the game
//   - g.max_player:     Gets the maximum number of players that is in a game
//                       possible, currently it is 32 including the Barbarians.
// - The following builtins are not only useful anymore in string replacement 
//   but also in slic code (Changed by Martin Gühmann - Oct. 30th 2004):
//   - player.capital:           The player's capital as string or city_t.
//   - player.largestcity:       The player's largest city as string or city_t.
//   - player.researching:       The database index or name of the current 
//                               player's research project.
//   - player.militarystate:     The player's military state as index or string.
//   - player.leaderperonality:  The player's leader's personality type as
//                               string or personality database index.
//   - player.govttype:          The player's government type as string or 
//                               government database index.
//   Note: leaderperonality should be leaderpersonality but as it is used
//   in the text files I don't fix it.
// - Decreased number of Slic errors for mods.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "SlicBuiltin.h"
#include "SlicSymbol.h"


#include "TurnCnt.h"
#include "SelItem.h"

#include "Army.h"
#include "player.h"
#include "UnitDynArr.h"
#include "UnitRecord.h"
#include "TerrImprove.h"
#include "Civilisation.h"
#include "CivilisationDB.h"
#include "StrDB.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "wonderutil.h"
#include "AdvanceRecord.h"
#include "ResourceRecord.h"

#include "TurnYearStatus.h"

#include "citydata.h"
#include "CityControlPanel.h"
#include "UnitData.h"
#include "GovernmentRecord.h"
#include "Diplomat.h"
#include "Readiness.h"

#define DEF_MAKECOPY(t) \
	SlicStructMemberData *MakeCopy(SlicStructInstance *parent) {\
		t *n = new t;\
		n->m_parent = parent;\
		return n;\
	}




class GlobalSymbol_Year : public SlicStructMemberData {
	DEF_MAKECOPY(GlobalSymbol_Year);

	BOOL GetIntValue(sint32 &value) const {
		value = g_turn->GetRound();
		return TRUE;
	}
};

class GlobalSymbol_YearString : public SlicStructMemberData {
	DEF_MAKECOPY(GlobalSymbol_YearString);

	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		strncpy(text, TurnYearStatus::GetCurrentYear(), maxLen);
		return TRUE;
	}
};

class GlobalSymbol_Player : public SlicStructMemberData {
	DEF_MAKECOPY(GlobalSymbol_Player);

	BOOL GetPlayer(sint32 &player) const {
		player = g_selected_item->GetCurPlayer();
		return TRUE;
	};

    BOOL GetIntValue(sint32 &value) const 
    {
        return GetPlayer(value);
    };
};

//Added by Martin Gühmann

class GlobalSymbol_NumOfPlayers : public SlicStructMemberData {
	DEF_MAKECOPY(GlobalSymbol_NumOfPlayers);

	BOOL GetIntValue(sint32 &value) const {
	//	sint32 players = 0;
		value = 0;
		sint32 i;
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i])
	//			players++;
				value++;
		}
	//	value = players;
		return TRUE;
	}
};

class GlobalSymbol_LastPlayer : public SlicStructMemberData {
	DEF_MAKECOPY(GlobalSymbol_LastPlayer);

	BOOL GetPlayer(sint32 &player) const {
	//	sint32 players = 0;
		player = 0;
		sint32 i;
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i])
				player = i;
	//			players = i;
		}
	//	value = players;
	//	player = players;
		return TRUE;
	}

	BOOL GetIntValue(sint32 &value) const 
    {
        return GetPlayer(value);
    };
};

class GlobalSymbol_MaxPlayers : public SlicStructMemberData {
	DEF_MAKECOPY(GlobalSymbol_MaxPlayers);

	BOOL GetIntValue(sint32 &value) const {
		value = k_MAX_PLAYERS;
		return TRUE;
	}
};


SlicStruct_Global::SlicStruct_Global() :
	SlicStructDescription("g", SLIC_BUILTIN_GLOBAL)
{
	AddMember("year", new GlobalSymbol_Year);
	AddMember("year_str", new GlobalSymbol_YearString);
	AddMember("player", new GlobalSymbol_Player);
	AddAccessor("num_of_players", new GlobalSymbol_NumOfPlayers);
	AddAccessor("last_player", new GlobalSymbol_LastPlayer);
	AddAccessor("max_players", new GlobalSymbol_MaxPlayers);
	
}



class UnitSymbol_Owner : public SlicStructMemberData {
	DEF_MAKECOPY(UnitSymbol_Owner);

	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }

	BOOL GetPlayer(sint32 &val) const {
		Unit unit;
		BOOL res = m_parent->GetDataSymbol()->GetUnit(unit);
		Assert(res);
		if(unit.IsValid()) {
			val = unit.GetOwner();
			return TRUE;
		} else {
			return FALSE;
		}
	}

	BOOL GetIntValue(sint32 &value) const 
    {
        return GetPlayer(value);
	}
};

class UnitSymbol_Location : public SlicStructMemberData {
	DEF_MAKECOPY(UnitSymbol_Location);
	SLIC_SYM GetType() const { return SLIC_SYM_LOCATION; }
	BOOL GetPos(MapPoint &pos) const {
		Unit unit;
		BOOL res = m_parent->GetDataSymbol()->GetUnit(unit);
		Assert(res);
		if(unit.IsValid()) {
			unit.GetPos(pos);
			return TRUE;
		} else {
			return FALSE;
		}
	}
};

class UnitSymbol_Type : public SlicStructMemberData {
	DEF_MAKECOPY(UnitSymbol_Type);

	BOOL GetIntValue(sint32 &value) const {
		Unit unit;
		BOOL res = m_parent->GetDataSymbol()->GetUnit(unit);
		Assert(res);
		if(unit.IsValid()) {
			value = unit.GetType();
			return TRUE;
		} else {
			return FALSE;
		}
	}
};

class UnitSymbol_HP : public SlicStructMemberData {
	DEF_MAKECOPY(UnitSymbol_HP);

	BOOL GetIntValue(sint32 &value) const {
		Unit unit;
		BOOL res = m_parent->GetDataSymbol()->GetUnit(unit);
		Assert(res);
		if(unit.IsValid()) {
			value = (sint32)unit.GetHP();
			return TRUE;
		} else {
			return FALSE;
		}
	}
};

class UnitSymbol_Valid : public SlicStructMemberData {
	DEF_MAKECOPY(UnitSymbol_Valid);

	BOOL GetIntValue(sint32 &value) const {
		Unit unit;
		BOOL res = m_parent->GetDataSymbol()->GetUnit(unit);
		value = unit.IsValid();
		return TRUE;
	}
};

class UnitSymbol_Name : public SlicStructMemberData {
	DEF_MAKECOPY(UnitSymbol_Name);
	SLIC_SYM GetType() const { return SLIC_SYM_STRING; }
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		Unit u;
		BOOL res = m_parent->GetDataSymbol()->GetUnit(u);
		Assert(res);
		if(u.IsValid()) {
			strncpy(text, u.GetName(), maxLen);
			return TRUE;
		}
		return FALSE;
	}

	StringId GetStringId() const {
		Unit u;
		BOOL res = m_parent->GetDataSymbol()->GetUnit(u);
		if(u.IsValid()) {
			return u.GetDBRec()->GetName();
		}
		return -1;
	}
};

SlicStruct_Unit::SlicStruct_Unit() :
	SlicStructDescription("unit", SLIC_BUILTIN_UNIT)
{
	AddMember("owner", new UnitSymbol_Owner);
	AddMember("location", new UnitSymbol_Location);
	AddMember("type", new UnitSymbol_Type);
	AddMember("hp", new UnitSymbol_HP);
	AddMember("valid", new UnitSymbol_Valid);
	AddMember("name", new UnitSymbol_Name);
}




class CitySymbol_Owner : public SlicStructMemberData {
	DEF_MAKECOPY(CitySymbol_Owner);

	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }

    BOOL GetPlayer(sint32 &val) const 
    {
		Unit city;
		BOOL res = m_parent->GetDataSymbol()->GetCity(city);
		if (res)
        {
    		val = city.GetOwner();
        }
		return res;
	};

    BOOL GetIntValue(sint32 & val) const
    {
        return GetPlayer(val);
    };
};

class CitySymbol_Location : public SlicStructMemberData {
	DEF_MAKECOPY(CitySymbol_Location);
	SLIC_SYM GetType() const { return SLIC_SYM_LOCATION; }
	BOOL GetPos(MapPoint &pos) const {
		Unit city;
		BOOL res = m_parent->GetDataSymbol()->GetCity(city);
		Assert(res);
		if(city.IsValid()) {
			city.GetPos(pos);
			return TRUE;
		} else {
			return FALSE;
		}
	}

};

class CitySymbol_Name : public SlicStructMemberData {
	DEF_MAKECOPY(CitySymbol_Name);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		Unit city;
		BOOL res = m_parent->GetDataSymbol()->GetCity(city);
		Assert(res);
		if(city.IsValid()) {
			strncpy(text, city.GetName(), maxLen);
			return TRUE;
		}
		return FALSE;
	}
};

class CitySymbol_Population : public SlicStructMemberData {
	DEF_MAKECOPY(CitySymbol_Population);

	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		Unit city;
		BOOL res = m_parent->GetDataSymbol()->GetCity(city);
		Assert(res);
		if (res)
        {
            value = city.IsValid() ? city.PopCount() : 0;
		} 
        return res;
	}
};

class CitySymbol_Happiness : public SlicStructMemberData {
	DEF_MAKECOPY(CitySymbol_Happiness);

	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		Unit city;
		BOOL res = m_parent->GetDataSymbol()->GetCity(city);
		Assert(res);
		if(city.IsValid()) {
			value = city.GetHappiness();
			return TRUE;
		} else {
			return FALSE;
		}
	}
};

class CitySymbol_NetCityGold : public SlicStructMemberData {
	DEF_MAKECOPY(CitySymbol_NetCityGold);

	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		Unit city;
		BOOL res = m_parent->GetDataSymbol()->GetCity(city);
		Assert(res);
		if(city.IsValid()) {
			value = city.GetNetCityGold();
			return TRUE;
		} else {
			return FALSE;
		}
	}
};

class CitySymbol_GoldFromTradeRoutes : public SlicStructMemberData {
	DEF_MAKECOPY(CitySymbol_GoldFromTradeRoutes);

	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		Unit city;
		BOOL res = m_parent->GetDataSymbol()->GetCity(city);
		Assert(res);
		if(city.IsValid()) {
			value = city.CD()->GetGoldFromTradeRoutes();
			return TRUE;
		} else {
			return FALSE;
		}
	}
};

class CitySymbol_Building : public SlicStructMemberData {
	DEF_MAKECOPY(CitySymbol_Building);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		Unit city;
		BOOL res = m_parent->GetDataSymbol()->GetCity(city);
		Assert(res);
		if(city.IsValid()) 
		{
			if(city.CD()->GetBuildQueue()->GetHead())
			{
				strncpy(text, CityControlPanel::GetBuildName(city.CD()->GetBuildQueue()->GetHead()), maxLen);
			}
			else
			{
				strncpy(text, g_theStringDB->GetNameStr("str_ldl_Building_Nothing_Label"),maxLen);
			}
			return TRUE;
		}
		return FALSE;
	}

	StringId GetStringId() const {
		Unit city;
		BOOL res = m_parent->GetDataSymbol()->GetCity(city);
		Assert(res);
		if(city.IsValid()) 
		{
			if(city.CD()->GetBuildQueue()->GetHead())
			{
				return CityControlPanel::GetBuildStringId(city.CD()->GetBuildQueue()->GetHead());
			}
			else
			{
				sint32 id;
				
				if(g_theStringDB->GetStringID("str_ldl_Building_Nothing_Label", id))
					return id;
				return -1;
			}
		}
		return -1;
	}
};

class CitySymbol_BuildingTime : public SlicStructMemberData {
	DEF_MAKECOPY(CitySymbol_BuildingTime);

	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		Unit city;
		BOOL res = m_parent->GetDataSymbol()->GetCity(city);
		Assert(res);
		if(city.IsValid()) {
			value = city.CD()->HowMuchLonger();
			return TRUE;
		} else {
			return FALSE;
		}
	}
};

class CitySymbol_BuildQueueLength : public SlicStructMemberData {
	DEF_MAKECOPY(CitySymbol_BuildQueueLength);

	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		Unit city;
		BOOL res = m_parent->GetDataSymbol()->GetCity(city);
		Assert(res);
		if(city.IsValid()) {
			value = city.CD()->GetBuildQueue()->GetLen();
			return TRUE;
		} else {
			return FALSE;
		}
	}
};

class CitySymbol_Valid : public SlicStructMemberData {
	DEF_MAKECOPY(CitySymbol_Valid);

	BOOL GetIntValue(sint32 &value) const {
		Unit city;
		BOOL res = m_parent->GetDataSymbol()->GetCity(city);
		value = city.IsValid();
		return TRUE;
	}
};

SlicStruct_City::SlicStruct_City() :
	SlicStructDescription("city", SLIC_BUILTIN_CITY)
{
	AddMember("owner", new CitySymbol_Owner);
	AddMember("location", new CitySymbol_Location);
	AddMember("name", new CitySymbol_Name);
	AddMember("population", new CitySymbol_Population);
	AddMember("happiness", new CitySymbol_Happiness);
	AddMember("netcitygold", new CitySymbol_NetCityGold);
	AddMember("goldfromtraderoutes", new CitySymbol_GoldFromTradeRoutes);
	AddMember("building", new CitySymbol_Building);
	AddMember("buildingtime", new CitySymbol_BuildingTime);
	AddMember("buildqueuelength", new CitySymbol_BuildQueueLength);
	
}





class PlayerSymbol_Owner : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_Owner);

	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		return m_parent->GetDataSymbol()->GetIntValue(value);
	}

	BOOL GetPlayer(sint32 &value) const {
		return m_parent->GetDataSymbol()->GetPlayer(value);
	}

};

class PlayerSymbol_Cities : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_Cities);
	BOOL GetIntValue(sint32 &value) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if (res)
        {
            value = g_player[pl] ? g_player[pl]->m_all_cities->Num() : 0;
		} 
        return res;
	}
};

class PlayerSymbol_Units : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_Units);
	BOOL GetIntValue(sint32 &value) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if (res) 
        {
            value = g_player[pl] ? g_player[pl]->m_all_units->Num() : 0;
		}
		return res;
	}
};

class PlayerSymbol_Armies : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_Armies);
	BOOL GetIntValue(sint32 &value) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if (res)
        {
            value = g_player[pl] ? g_player[pl]->m_all_armies->Num() : 0;
		}
        return res;
	}
};
// Added by PFT: player[ ].government
// And a minor improvement by Martin Gühmann

class PlayerSymbol_Government : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_Government);
	BOOL GetIntValue(sint32 &value) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if (res)
        {
            value = g_player[pl] ? g_player[pl]->m_government_type : -1;
		}
        return res;
	}
};

class PlayerSymbol_MilitaryUnits : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_MilitaryUnits);
	BOOL GetIntValue(sint32 &value) const {
		sint32 pl;
		BOOL const res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if (res)
        {
			value = 0;
            if (g_player[pl] && g_player[pl]->m_all_units) 
            {
			    for (sint32 i = 0; i < g_player[pl]->m_all_units->Num(); i++) 
                {
				    if (g_player[pl]->m_all_units->Access(i).GetDBRec()->GetAttack() > 0.001) 
                    {
					    value++;
                    }
				}
			}
		} 
        return res;
	}
};

class PlayerSymbol_TotalPopulation : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_TotalPopulation);
	BOOL GetIntValue(sint32 &value) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if (res)
        {
            value = g_player[pl] ? g_player[pl]->GetTotalPopulation() : 0;
		} 
        return res;
	}
};

class PlayerSymbol_TotalPollution : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_TotalPollution);
	BOOL GetIntValue(sint32 &value) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if (res)
        {
            value = g_player[pl] ? g_player[pl]->GetCurrentPollution() : 0;
		}
        return res;
	}
};

class PlayerSymbol_Capital : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_Capital);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		Unit capitol;
		BOOL const res = GetCity(capitol);
		if (res)
        {
            text[0] = 0;
            if (capitol.IsValid())
            {
			    strncpy(text, capitol->GetCityData()->GetName(), maxLen);
            }
		}
		return res;
	}
	// Added by Martin Gühmann to allow to access the capital
	// as city_t.
	BOOL GetCity(Unit &city) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if (res) 
		{
            if (g_player[pl] && g_player[pl]->m_capitol)
            {
			    city = Unit(g_player[pl]->m_capitol->m_id);
            }
            else
            {
                // no capitol
                city = Unit();  
            }
		}
		return res;
	}
};

class PlayerSymbol_LargestCity : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_LargestCity);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		Unit        largestCity;
		BOOL const  res = GetCity(largestCity);
		if (res)
        {
            text[0] = 0;
            if (largestCity.IsValid())
            {
			    strncpy(text, largestCity->GetCityData()->GetName(), maxLen);
			}
		}
		return res;
	}
	// Added by Martin Gühmann to allow to access the largest city
	// as city_t.
	BOOL GetCity(Unit &city) const 
    {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if (res)
        {
            if (g_player[pl]                &&
                g_player[pl]->m_all_cities  &&
                (g_player[pl]->m_all_cities->Num() > 0)
               )
            {
                sint32  lpop    = g_player[pl]->m_all_cities->Get(0)->PopCount();
                sint32  lndx    = 0;

			    for (sint32 i = 1; i < g_player[pl]->m_all_cities->Num(); i++)
			    {
                    Unit const &    testCity = g_player[pl]->m_all_cities->Get(i); 
				    if (testCity->PopCount() > lpop)
                    {
					    lpop    = testCity->PopCount();
					    lndx    = i;
                    }
				}

			    city = g_player[pl]->m_all_cities->Get(lndx);
			}
            else
            {
                city = Unit(0); // invalid
            }
        }
		return res;
	}
};

class PlayerSymbol_Researching : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_Researching);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if (pl>=0 && pl<k_MAX_PLAYERS && g_player[pl]!=NULL) 
		{
			strncpy(text, g_theAdvanceDB->Get(g_player[pl]->m_advances->GetResearching())->GetNameText(), maxLen);
			return TRUE;
		}
		return FALSE;
	}
	StringId GetStringId() const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		if(pl>= 0 && pl<k_MAX_PLAYERS && g_player[pl] != NULL) {
			return g_theAdvanceDB->Get(g_player[pl]->m_advances->GetResearching())->GetName();
		}
		return -1;
	}
	// Added by Martin Gühmann to allow to access the database index
	// of the current research project.
	BOOL GetIntValue(sint32 &value) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		if(pl>= 0 && pl<k_MAX_PLAYERS && g_player[pl] != NULL) {
			value = g_player[pl]->m_advances->GetResearching();
			return TRUE;
		}
		value = -1;
		return FALSE;
	}
};

class PlayerSymbol_MilitaryState : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_MilitaryState);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(pl>=0 && pl<k_MAX_PLAYERS && g_player[pl]!=NULL) 
		{
			switch(g_player[pl]->GetReadinessLevel()) 
			{
				case READINESS_LEVEL_PEACE:
					strncpy(text, g_theStringDB->GetNameStr("str_ldl_READINESS_LEVEL_PEACE"), maxLen);
					break;
				case READINESS_LEVEL_ALERT:
					strncpy(text, g_theStringDB->GetNameStr("str_ldl_READINESS_LEVEL_ALERT"), maxLen);
					break;
				case READINESS_LEVEL_WAR:
					strncpy(text, g_theStringDB->GetNameStr("str_ldl_READINESS_LEVEL_WAR"), maxLen);
					break;
				default:
					Assert(false);
					break;
			}
			return TRUE;
		}
		return FALSE;
	}
	StringId GetStringId() const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(pl>=0 && pl<k_MAX_PLAYERS && g_player[pl]!=NULL) 
		{
			sint32 id = -1;
			switch(g_player[pl]->GetReadinessLevel()) 
			{
				case READINESS_LEVEL_PEACE:
					g_theStringDB->GetStringID("str_ldl_READINESS_LEVEL_PEACE", id);
					break;
				case READINESS_LEVEL_ALERT:
					g_theStringDB->GetStringID("str_ldl_READINESS_LEVEL_ALERT", id);
					break;
				case READINESS_LEVEL_WAR:
					g_theStringDB->GetStringID("str_ldl_READINESS_LEVEL_WAR", id);
					break;
				default:
					Assert(false);
					break;
			}
			return id;
		}
		return -1;
	}
	// Added by Martin Gühmann to allow to access the index of 
	// the current military readiness level.
	BOOL GetIntValue(sint32 &value) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		if(pl>= 0 && pl<k_MAX_PLAYERS && g_player[pl] != NULL) {
			value = g_player[pl]->GetReadinessLevel();
			return TRUE;
		}
		value = -1;
		return FALSE;
	}
};


class PlayerSymbol_LeaderPersonality : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_LeaderPersonality);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(pl>=0 && pl<k_MAX_PLAYERS && g_player[pl]!=NULL) 
		{
			
			StringId description = Diplomat::GetDiplomat(pl).GetPersonality()->GetDescription();
			if (description != -1)
				strncpy(text, g_theStringDB->GetNameStr(description) , maxLen);
			else
				strncpy(text, Diplomat::GetDiplomat(pl).GetPersonalityName().c_str(), maxLen);
			return TRUE;
		}
		return FALSE;
	}
	// Added by Martin Gühmann to allow access to the database personality
	// index of the player leader's personality.
	BOOL GetIntValue(sint32 &value) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(pl>=0 && pl<k_MAX_PLAYERS && g_player[pl]!=NULL) 
		{
			value = Diplomat::GetDiplomat(pl).GetPersonality()->GetIndex();
			return TRUE;
		}
		value = -1;
		return FALSE;
	}
};

class PlayerSymbol_GovtType : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_GovtType);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(pl>=0 && pl<k_MAX_PLAYERS && g_player[pl]!=NULL) 
		{
			
			strncpy(text, g_theGovernmentDB->Get(g_player[pl]->m_government_type)->GetNameText(), maxLen);
			return TRUE;
		}
		return FALSE;
	}

	StringId GetStringId() const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(pl>=0 && pl<k_MAX_PLAYERS && g_player[pl]!=NULL) 
		{
			return g_theGovernmentDB->Get(g_player[pl]->m_government_type)->GetName();
		}
		return -1;
	}
	// Added by Martin Gühmann to access the database index of the
	// current player's government.
	BOOL GetIntValue(sint32 &value) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(pl>=0 && pl<k_MAX_PLAYERS && g_player[pl]!=NULL) 
		{
			value = g_player[pl]->m_government_type;
			return TRUE;
		}
		value = -1;
		return FALSE;
	}
};

class PlayerSymbol_CivNamePlural : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_CivNamePlural);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			Assert(pl >= 0 && pl < k_MAX_PLAYERS && g_player[pl]);
			if(pl >= 0 && pl < k_MAX_PLAYERS && g_player[pl]) {
				g_player[pl]->m_civilisation->GetPluralCivName(text);
				return TRUE;
			} else {
				return FALSE;
			}
		}
		return FALSE;
	}
	StringId GetStringId() const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			Assert(pl >= 0 && pl < k_MAX_PLAYERS && g_player[pl]);
			if(pl >= 0 && pl < k_MAX_PLAYERS && g_player[pl]) {
				return g_theCivilisationDB->GetPluralCivName((*(g_player[pl]->m_civilisation))->GetCivilisation());
			} else {
				return -1;
			}
		}
		return -1;
	}
};

class PlayerSymbol_CivNameSingular : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_CivNameSingular);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res && g_player[pl]) {
			g_player[pl]->m_civilisation->GetSingularCivName(text);
			return TRUE;
		}
		return FALSE;
	}
	StringId GetStringId() const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res && g_player[pl]) {
			return g_theCivilisationDB->GetSingularCivName((*(g_player[pl]->m_civilisation))->GetCivilisation());
		}
		return -1;
	}
};

class PlayerSymbol_LeaderName : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_LeaderName);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			strcpy(text, g_player[pl]->GetLeaderName());
			return TRUE;
		}
		return FALSE;
	}

	StringId GetStringId() const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			StringId id = -1;
			
			
			if(g_player[pl]->m_civilisation->GetGender() == GENDER_MALE) {
				id = g_theCivilisationDB->GetLeaderName((*(g_player[pl]->m_civilisation))->GetCivilisation());
			} else {
				id = g_theCivilisationDB->GetLeaderNameFemale((*(g_player[pl]->m_civilisation))->GetCivilisation());
			}
			if(id >= 0 &&
			   strcmp(g_player[pl]->GetLeaderName(),
					  g_theStringDB->GetNameStr(id)) == 0) {
				return id;
			}
		}
		return -1;
	}
};

class PlayerSymbol_CountryName : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_CountryName);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			g_player[pl]->m_civilisation->GetCountryName(text);
			return TRUE;
		}
		return FALSE;
	}
	StringId GetStringId() const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			return g_theCivilisationDB->GetCountryName((*(g_player[pl]->m_civilisation))->GetCivilisation());
		}
		return -1;
	}
};

class PlayerSymbol_He : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_He);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			if(g_player[pl]->m_civilisation->GetGender() == GENDER_MALE) {
				strcpy(text, g_theStringDB->GetNameStr("PRONOUN_HE_MALE"));
			} else {
				strcpy(text, g_theStringDB->GetNameStr("PROUNOUN_HE_FEMALE"));
			}
			return TRUE;
		}
		return FALSE;
	}
};

class PlayerSymbol_Him : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_Him);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			if(g_player[pl]->m_civilisation->GetGender() == GENDER_MALE) {
				strcpy(text, g_theStringDB->GetNameStr("PRONOUN_HIM_MALE"));
			} else {
				strcpy(text, g_theStringDB->GetNameStr("PROUNOUN_HIM_FEMALE"));
			}
			return TRUE;
		}
		return FALSE;
	}
};

class PlayerSymbol_His : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_His);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			if(g_player[pl]->m_civilisation->GetGender() == GENDER_MALE) {
				strcpy(text, g_theStringDB->GetNameStr("PRONOUN_HIS_MALE"));
			} else {
				strcpy(text, g_theStringDB->GetNameStr("PROUNOUN_HIS_FEMALE"));
			}
			return TRUE;
		}
		return FALSE;
	}
};

class PlayerSymbol_HeCap : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_HeCap);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			if(g_player[pl]->m_civilisation->GetGender() == GENDER_MALE) {
				strcpy(text, g_theStringDB->GetNameStr("PRONOUN_HE_MALE_CAP"));
			} else {
				strcpy(text, g_theStringDB->GetNameStr("PROUNOUN_HE_FEMALE_CAP"));
			}
			return TRUE;
		}
		return FALSE;
	}
};

class PlayerSymbol_HimCap : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_HimCap);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			if(g_player[pl]->m_civilisation->GetGender() == GENDER_MALE) {
				strcpy(text, g_theStringDB->GetNameStr("PRONOUN_HIM_MALE_CAP"));
			} else {
				strcpy(text, g_theStringDB->GetNameStr("PROUNOUN_HIM_FEMALE_CAP"));
			}
			return TRUE;
		}
		return FALSE;
	}
};

class PlayerSymbol_HisCap : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_HisCap);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			if(g_player[pl]->m_civilisation->GetGender() == GENDER_MALE) {
				strcpy(text, g_theStringDB->GetNameStr("PRONOUN_HIS_MALE_CAP"));
			} else {
				strcpy(text, g_theStringDB->GetNameStr("PROUNOUN_HIS_FEMALE_CAP"));
			}
			return TRUE;
		}
		return FALSE;
	}
};

class PlayerSymbol_Sir : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_Sir);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			if(g_player[pl]->m_civilisation->GetGender() == GENDER_MALE) {
				strcpy(text, g_theStringDB->GetNameStr("LEADER_ADDRESS_MALE"));
			} else {
				strcpy(text, g_theStringDB->GetNameStr("LEADER_ADDRESS_FEMALE"));
			}
			return TRUE;
		}
		return FALSE;
	}
};

class PlayerSymbol_SirCap : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_SirCap);
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res) {
			if(g_player[pl]->m_civilisation->GetGender() == GENDER_MALE) {
				strcpy(text, g_theStringDB->GetNameStr("LEADER_ADDRESS_MALE_CAP"));
			} else {
				strcpy(text, g_theStringDB->GetNameStr("LEADER_ADDRESS_FEMALE_CAP"));
			}
			return TRUE;
		}
		return FALSE;
	}
};


class PlayerSymbol_PublicWorksTax : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_PublicWorksTax);

	BOOL GetIntValue(sint32 &value) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res)
			{
			value = g_player[pl]->m_materialsTax*100;
			return TRUE;
			}
		else
			{
			value = 0;
			return FALSE;
			}
		}

};

class PlayerSymbol_PublicWorksLevel : public SlicStructMemberData {
	DEF_MAKECOPY(PlayerSymbol_PublicWorksLevel);

	BOOL GetIntValue(sint32 &value) const {
		sint32 pl;
		BOOL res = m_parent->GetDataSymbol()->GetPlayer(pl);
		Assert(res);
		if(res)
			{
			value = g_player[pl]->GetMaterialsStored();
			return TRUE;
			}
		else
			{
			value = 0;
			return FALSE;
			}
		}

};


SlicStruct_Player::SlicStruct_Player() :
	SlicStructDescription("player", SLIC_BUILTIN_PLAYER)
{
	AddMember("owner", new PlayerSymbol_Owner);
	AddMember("cities", new PlayerSymbol_Cities);
	AddMember("units", new PlayerSymbol_Units);
	AddMember("militaryunits", new PlayerSymbol_MilitaryUnits);
	AddMember("armies", new PlayerSymbol_Armies);
	AddAccessor("government", new PlayerSymbol_Government);
	AddMember("totalpopulation", new PlayerSymbol_TotalPopulation);
	AddMember("totalpollution", new PlayerSymbol_TotalPollution);
	AddMember("capital", new PlayerSymbol_Capital);
	AddMember("largestcity", new PlayerSymbol_LargestCity);
	AddMember("researching", new PlayerSymbol_Researching);
	AddMember("militarystate", new PlayerSymbol_MilitaryState);
	AddMember("leaderperonality", new PlayerSymbol_LeaderPersonality);
	AddMember("govttype", new PlayerSymbol_GovtType);

	
	AddMember("civ_name_plural", new PlayerSymbol_CivNamePlural);
	AddMember("civ_name_singular", new PlayerSymbol_CivNameSingular);
	AddMember("country_name", new PlayerSymbol_CountryName);
	AddMember("leader_name", new PlayerSymbol_LeaderName);
	AddMember("he", new PlayerSymbol_HeCap);
	AddMember("him", new PlayerSymbol_HimCap);
	AddMember("his", new PlayerSymbol_HisCap);
	AddMember("he_cap", new PlayerSymbol_HeCap);
	AddMember("him_cap", new PlayerSymbol_HimCap);
	AddMember("his_cap", new PlayerSymbol_HisCap);
	AddMember("sir", new PlayerSymbol_Sir);
	AddMember("sir_cap", new PlayerSymbol_SirCap);

	
	AddMember("publicworkstax", new PlayerSymbol_PublicWorksTax);
	AddMember("publicworkslevel", new PlayerSymbol_PublicWorksLevel);
}


class ArmySymbol_Owner : public SlicStructMemberData {
	DEF_MAKECOPY(ArmySymbol_Owner);

	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetPlayer(sint32 &val) const {
		Army army;
		BOOL res = m_parent->GetDataSymbol()->GetArmy(army);

		if(!res)
			return FALSE;

		val = army.GetOwner();
		return TRUE;
	}

	BOOL GetIntValue(sint32 &value) const 
    {
		return GetPlayer(value);
	}
};

class ArmySymbol_Location : public SlicStructMemberData {
	DEF_MAKECOPY(ArmySymbol_Location);
	SLIC_SYM GetType() const { return SLIC_SYM_LOCATION; }

	BOOL GetPos(MapPoint &pos) const {
		Army army;
		BOOL res = m_parent->GetDataSymbol()->GetArmy(army);

		if(!res)
			return FALSE;

		army.GetPos(pos);
		return TRUE;
	}
};

class ArmySymbol_Size : public SlicStructMemberData {
	DEF_MAKECOPY(ArmySymbol_Size);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }

	BOOL GetIntValue(sint32 &value) const 
    {
		Army army;
		BOOL const res = m_parent->GetDataSymbol()->GetArmy(army);
		if (res) 
        {
            value = army.IsValid() ? army.Num() : 0;
        }
		return res;
	};
};

SlicStruct_Army::SlicStruct_Army() :
	SlicStructDescription("army", SLIC_BUILTIN_ARMY)
{
	AddMember("owner", new ArmySymbol_Owner);
	AddMember("location", new ArmySymbol_Location);
	AddMember("size", new ArmySymbol_Size);
}




class LocationSymbol_Location : public SlicStructMemberData {
	DEF_MAKECOPY(LocationSymbol_Location);
	SLIC_SYM GetType() const { return SLIC_SYM_LOCATION; }
	BOOL GetPos(MapPoint &pos) const {
		Assert(m_parent);
		return m_parent->GetDataSymbol()->GetPos(pos);
	}
};



class LocationSymbol_X : public SlicStructMemberData {
	DEF_MAKECOPY(LocationSymbol_X);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		MapPoint pos;
		BOOL res = m_parent->GetDataSymbol()->GetPos(pos);

		if(!res)
			return FALSE;

		value=pos.x;
		return TRUE;
	}
};


class LocationSymbol_Y : public SlicStructMemberData {
	DEF_MAKECOPY(LocationSymbol_Y);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		MapPoint pos;
		BOOL res = m_parent->GetDataSymbol()->GetPos(pos);

		if(!res)
			return FALSE;

		value=pos.y;
		return TRUE;
	}
};


SlicStruct_Location::SlicStruct_Location() :
	SlicStructDescription("location", SLIC_BUILTIN_LOCATION)
{
	AddMember("location", new LocationSymbol_Location);
    
	
	AddMember("x", new LocationSymbol_X);
	AddMember("y", new LocationSymbol_Y);
}




class GovernmentSymbol_Type : public SlicStructMemberData {
	DEF_MAKECOPY(GovernmentSymbol_Type);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		return m_parent->GetDataSymbol()->GetIntValue(value);
	}
};

SlicStruct_Government::SlicStruct_Government() :
	SlicStructDescription("government", SLIC_BUILTIN_GOVERNMENT)
{
	AddMember("type", new GovernmentSymbol_Type);
}

	



class AdvanceSymbol_Type : public SlicStructMemberData {
	DEF_MAKECOPY(AdvanceSymbol_Type);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		return m_parent->GetDataSymbol()->GetIntValue(value);
	}
};

class AdvanceSymbol_Name : public SlicStructMemberData {
	DEF_MAKECOPY(AdvanceSymbol_Name);
	SLIC_SYM GetType() const { return SLIC_SYM_SVAR; }
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 adv;
		if(m_parent->GetDataSymbol()->GetIntValue(adv)) {
			strncpy(text, g_theAdvanceDB->Get(adv)->GetNameText(), maxLen);
			return TRUE;
		} else {
			return FALSE;
		}
	}
	StringId GetStringId() const {
		sint32 adv;
		if(m_parent->GetDataSymbol()->GetIntValue(adv)) {
			return g_theAdvanceDB->Get(adv)->GetName();
		}
		return -1;
	}

};

SlicStruct_Advance::SlicStruct_Advance() :
	SlicStructDescription("advance", SLIC_BUILTIN_ADVANCE)
{
	AddMember("type", new AdvanceSymbol_Type);
	AddMember("name", new AdvanceSymbol_Name);
}




class ActionSymbol_Text : public SlicStructMemberData {
	DEF_MAKECOPY(ActionSymbol_Text);
	SLIC_SYM GetType() const { return SLIC_SYM_STRING; }
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		return m_parent->GetDataSymbol()->GetText(text, maxLen);
	}
};

SlicStruct_Action::SlicStruct_Action() :
	SlicStructDescription("action", SLIC_BUILTIN_ACTION)
{
	AddMember("text", new ActionSymbol_Text);
	AddMember("name", new ActionSymbol_Text);
}




class ImprovementSymbol_Type : public SlicStructMemberData {
	DEF_MAKECOPY(ImprovementSymbol_Type);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		TerrainImprovement imp;
		BOOL res = m_parent->GetDataSymbol()->GetImprovement(imp);
		if(res && imp.IsValid()) {
			value = imp->GetType();
			return TRUE;
		}
		return FALSE;
	}
};

SlicStruct_Improvement::SlicStruct_Improvement() :
	SlicStructDescription("improvement", SLIC_BUILTIN_IMPROVEMENT)
{
	AddMember("type", new ImprovementSymbol_Type);
}




class ValueSymbol_Value : public SlicStructMemberData {
	DEF_MAKECOPY(ValueSymbol_Value);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		return m_parent->GetDataSymbol()->GetIntValue(value);
	}
};

SlicStruct_Value::SlicStruct_Value() :
	SlicStructDescription("value", SLIC_BUILTIN_VALUE)
{
	AddMember("value", new ValueSymbol_Value);
}




class BuildingSymbol_Name : public SlicStructMemberData {
	DEF_MAKECOPY(BuildingSymbol_Name);
	SLIC_SYM GetType() const { return SLIC_SYM_STRING; }
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 type;
		BOOL res = m_parent->GetDataSymbol()->GetIntValue(type);
		Assert(res);
		if(!res) {
			type = 0;
		}

		strncpy(text, g_theStringDB->GetNameStr(g_theBuildingDB->Get(type)->GetName()), maxLen);
		return TRUE;
	}

	StringId GetStringId() const {
		sint32 type;
		BOOL res = m_parent->GetDataSymbol()->GetIntValue(type);
		Assert(res);
		if(res) {
			return g_theBuildingDB->Get(type)->GetName();
		}
		return -1;
	}
};

class BuildingSymbol_Type : public SlicStructMemberData {
	DEF_MAKECOPY(BuildingSymbol_Type);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		return m_parent->GetDataSymbol()->GetIntValue(value);
	}
};

SlicStruct_Building::SlicStruct_Building() :
	SlicStructDescription("building", SLIC_BUILTIN_BUILDING)
{
	AddMember("name", new BuildingSymbol_Name);
	AddMember("type", new BuildingSymbol_Type);
}




class WonderSymbol_Name : public SlicStructMemberData {
	DEF_MAKECOPY(WonderSymbol_Name);
	SLIC_SYM GetType() const { return SLIC_SYM_STRING; }
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 type;
		BOOL res = m_parent->GetDataSymbol()->GetIntValue(type);
		Assert(res);
		if(!res) {
			type = 0;
		}

		strncpy(text, g_theStringDB->GetNameStr(wonderutil_Get(type)->GetName()), maxLen);
		return TRUE;
	}

	StringId GetStringId() const {
		sint32 type;
		BOOL res = m_parent->GetDataSymbol()->GetIntValue(type);
		Assert(res);
		if(res) {
			return wonderutil_Get(type)->GetName();
		}
		return -1;
	}
};

class WonderSymbol_Type : public SlicStructMemberData {
	DEF_MAKECOPY(WonderSymbol_Type);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		return m_parent->GetDataSymbol()->GetIntValue(value);
	}
};

SlicStruct_Wonder::SlicStruct_Wonder() :
	SlicStructDescription("wonder", SLIC_BUILTIN_WONDER)
{
	AddMember("name", new WonderSymbol_Name);
	AddMember("type", new WonderSymbol_Type);
}




class UnitRecordSymbol_Name : public SlicStructMemberData {
	DEF_MAKECOPY(UnitRecordSymbol_Name);
	SLIC_SYM GetType() const { return SLIC_SYM_STRING; }
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 type;
		BOOL res = m_parent->GetDataSymbol()->GetIntValue(type);
		Assert(res);
		if(!res)
			type = 0;
		
		strncpy(text, g_theUnitDB->Get(type)->GetNameText(), maxLen);
		return TRUE;
	}
	StringId GetStringId() const {
		sint32 type;
		BOOL res = m_parent->GetDataSymbol()->GetIntValue(type);
		Assert(res);
		if(res) {
			return g_theUnitDB->Get(type)->GetName();
		}
		return -1;
	}
};

class UnitRecordSymbol_Cost : public SlicStructMemberData {
	DEF_MAKECOPY(UnitRecordSymbol_Cost);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		sint32 type;
		BOOL res = m_parent->GetDataSymbol()->GetIntValue(type);
		Assert(res);
		if(!res)
			type = 0;

		value = g_theUnitDB->Get(type)->GetShieldCost();
		return TRUE;
	}
};

class UnitRecordSymbol_Attack : public SlicStructMemberData {
	DEF_MAKECOPY(UnitRecordSymbol_Attack);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		sint32 type;
		BOOL res = m_parent->GetDataSymbol()->GetIntValue(type);
		Assert(res);
		if(!res)
			type = 0;

		value = (sint32)g_theUnitDB->Get(type)->GetAttack();
		return TRUE;
	}
};

class UnitRecordSymbol_Defense : public SlicStructMemberData {
	DEF_MAKECOPY(UnitRecordSymbol_Defense);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		sint32 type;
		BOOL res = m_parent->GetDataSymbol()->GetIntValue(type);
		Assert(res);
		if(!res)
			type = 0;

		value = (sint32)g_theUnitDB->Get(type)->GetDefense();
		return TRUE;
	}
};

class UnitRecordSymbol_Ranged : public SlicStructMemberData {
	DEF_MAKECOPY(UnitRecordSymbol_Ranged);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		sint32 type;
		BOOL res = m_parent->GetDataSymbol()->GetIntValue(type);
		Assert(res);
		if(!res)
			type = 0;

		value = g_theUnitDB->Get(type)->GetZBRangeAttack();
		return TRUE;
	}
};

class UnitRecordSymbol_Movement : public SlicStructMemberData {
	DEF_MAKECOPY(UnitRecordSymbol_Movement);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		sint32 type;
		BOOL res = m_parent->GetDataSymbol()->GetIntValue(type);
		Assert(res);
		if(!res)
			type = 0;

		value = (sint32)g_theUnitDB->Get(type)->GetMaxMovePoints();
		return TRUE;
	}
};

SlicStruct_UnitRecord::SlicStruct_UnitRecord() :
	SlicStructDescription("unitrecord", SLIC_BUILTIN_UNITRECORD)
{
	AddMember("name", new UnitRecordSymbol_Name);
	AddMember("attack", new UnitRecordSymbol_Attack);
	AddMember("defense", new UnitRecordSymbol_Defense);
	AddMember("cost", new UnitRecordSymbol_Cost);
	AddMember("ranged", new UnitRecordSymbol_Ranged);
	AddMember("movement", new UnitRecordSymbol_Movement);
}




class GoldSymbol_Value : public SlicStructMemberData {
	DEF_MAKECOPY(GoldSymbol_Value);
	SLIC_SYM GetType() const { return SLIC_SYM_IVAR; }
	BOOL GetIntValue(sint32 &value) const {
		return m_parent->GetDataSymbol()->GetIntValue(value);
	}
};

SlicStruct_Gold::SlicStruct_Gold() :
	SlicStructDescription("gold", SLIC_BUILTIN_GOLD)
{
	AddMember("value", new GoldSymbol_Value);
}




class GoodSymbol_Name : public SlicStructMemberData {
	DEF_MAKECOPY(GoodSymbol_Name);
	SLIC_SYM GetType() const { return SLIC_SYM_STRING; }
	BOOL GetText(MBCHAR *text, sint32 maxLen) const {
		sint32 type;
		BOOL res = m_parent->GetDataSymbol()->GetIntValue(type);
		Assert(res);
		if(!res)
			type = 0;

		strncpy(text, g_theResourceDB->Get(type)->GetNameText(), maxLen);
		return TRUE;
	}

	StringId GetStringId() const {
		sint32 type;
		BOOL res = m_parent->GetDataSymbol()->GetIntValue(type);
		Assert(res);
		if(res) {
			return g_theResourceDB->Get(type)->GetName();
		}
		return -1;
	}
};

SlicStruct_Good::SlicStruct_Good() :
	SlicStructDescription("good", SLIC_BUILTIN_GOOD)
{
	AddMember("name", new GoodSymbol_Name);
}

