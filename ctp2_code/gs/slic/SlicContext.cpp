//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : 
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Destructor cleaned up.
// - Repaired crashes.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "SlicObject.h"
#include "SlicEngine.h"
#include "SlicSegment.h"
#include "MapPoint.h"
#include "Unit.h"
#include "Civilisation.h"
#include "Advances.h"
#include "SlicFrame.h"
#include "player.h"
#include "civarchive.h"
#include "Agreement.h"
#include "TradeOffer.h"
#include "GameEventArgList.h"
#include "GameEventArgument.h"
#include "Army.h"
#include "SlicBuiltinEnum.h"
#include "SlicArray.h"
#include "SlicSymbol.h"
#include "SimpleDynArr.h"
#include "Order.h"
#include "diplomattypes.h"

SlicContext::SlicContext()
{
	m_cityList = NULL;
	m_unitList = NULL;
	m_playerList = NULL;
	m_intList = NULL;
	m_unitRecordList = NULL;
	m_advanceList = NULL;
	m_agreementList = NULL;
	m_locationList = NULL;
	m_calamityList = NULL;
	m_numCalamities = 0;
	m_goldList = NULL;
	m_numGolds = 0;
	m_goodList = NULL;
	m_rankList = NULL;
	m_numRanks = 0;
	m_wonderList = NULL;
	m_numWonders = 0;
	m_actionList = NULL;
	m_numActions = 0;
	m_tradeOffersList = NULL;
	
	m_governmentList = NULL;
	m_numOrders = 0;
	m_orderList = NULL;
    m_numMadlibs = 0;
    m_madlibNameList = NULL;
    m_madlibChoiceList = NULL;
	m_numAttitudes = 0;
	m_attitudeList = NULL;
	m_numAges = 0;
	m_ageList = NULL;
	m_buildingList = NULL;
	m_numBuildings = 0;
	m_tradeBidList = NULL;
	m_numTradeBids = 0;
	m_armyList = NULL;

	m_eventArgs = NULL;
}

#define COPY_SIMPLE_ARRAY(name, type) \
    if(copy->name) {\
        name = new SimpleDynamicArray<type>;\
        for(i = 0; i < copy->name->Num(); i++) {\
			name->Insert(copy->name->Access(i));\
		}\
	} else {\
		name = NULL;\
	}

SlicContext::SlicContext(SlicContext *copy)
{
	sint32 i;
	COPY_SIMPLE_ARRAY(m_cityList, Unit);
	COPY_SIMPLE_ARRAY(m_unitList, Unit);
	COPY_SIMPLE_ARRAY(m_armyList, Army);
	COPY_SIMPLE_ARRAY(m_playerList, sint32);
	COPY_SIMPLE_ARRAY(m_intList, sint32);
	COPY_SIMPLE_ARRAY(m_unitRecordList, sint32);
	COPY_SIMPLE_ARRAY(m_locationList, MapPoint);
	COPY_SIMPLE_ARRAY(m_agreementList, ai::Agreement);
	COPY_SIMPLE_ARRAY(m_tradeOffersList, TradeOffer);
	COPY_SIMPLE_ARRAY(m_goodList, sint32);
	COPY_SIMPLE_ARRAY(m_governmentList, sint32);
	COPY_SIMPLE_ARRAY(m_advanceList, sint32);

	CopyArray(m_calamityList, copy->m_calamityList, 
			  m_numCalamities, copy->m_numCalamities);
	CopyArray(m_goldList, copy->m_goldList, 
			  m_numGolds, copy->m_numGolds);
	CopyArray(m_rankList, copy->m_rankList, 
			  m_numRanks, copy->m_numRanks);
	CopyArray(m_wonderList, copy->m_wonderList, 
			  m_numWonders, copy->m_numWonders);
	CopyArray(m_orderList, copy->m_orderList,
			  m_numOrders, copy->m_numOrders);

	CopyArray(m_madlibChoiceList, copy->m_madlibChoiceList,
			  m_numMadlibs, copy->m_numMadlibs);
	CopyArray(m_madlibNameList, copy->m_madlibNameList,
			  m_numMadlibs, copy->m_numMadlibs);
	CopyArray(m_attitudeList, copy->m_attitudeList,
			  m_numAttitudes, copy->m_numAttitudes);
	CopyArray(m_ageList, copy->m_ageList,
			  m_numAges, copy->m_numAges);
	CopyArray(m_buildingList, copy->m_buildingList,
			  m_numBuildings, copy->m_numBuildings);
	CopyArray((sint32*&)m_tradeBidList, (sint32*&)copy->m_tradeBidList,
			  m_numTradeBids, copy->m_numTradeBids);

	{
		m_numActions = copy->m_numActions;
		if(m_numActions > 0) {
			m_actionList = new MBCHAR*[copy->m_numActions];
			for(sint32 i = 0; i < m_numActions; i++) {
				m_actionList[i] = new MBCHAR[strlen(copy->m_actionList[i]) + 1];
				strcpy(m_actionList[i], copy->m_actionList[i]);
			}
		} else {
			m_actionList = NULL;
		}
	}

	m_eventArgs = copy->m_eventArgs;
}

SlicContext::~SlicContext()
{
	// m_eventArgs not deleted: reference only
	delete m_cityList;
	delete m_unitList;
	delete m_armyList;
	delete m_playerList;
	delete m_advanceList;
	delete m_locationList;
	delete m_agreementList;
	delete m_intList;
	delete m_unitRecordList;
	delete m_goodList;
	delete m_governmentList;
    delete m_tradeOffersList;

	delete [] m_calamityList; 
	delete [] m_goldList;
    delete [] m_rankList;
    delete [] m_wonderList;
	delete [] m_orderList;
    delete [] m_madlibNameList;
    delete [] m_madlibChoiceList;
	delete [] m_attitudeList;
    delete [] m_ageList;
    delete [] m_buildingList;
    delete [] m_tradeBidList;

	if (m_actionList) 
    {
		for (size_t i = 0; i < m_numActions; ++i) 
        {
			delete [] m_actionList[i];
		}
		delete [] m_actionList;
	}
}

#define SER_ARRAY(list) \
    hasList = list != NULL;\
    archive << hasList;\
    if(hasList) list->Serialize(archive);

#define UNSER_ARRAY(list, type) \
		archive >> hasList;\
		if(hasList) {\
			list = new SimpleDynamicArray<type>;\
			list->Serialize(archive);\
		} else\
			list = NULL;

void SlicContext::Serialize(CivArchive &archive)
{
	sint32 hasList;
	sint32 l;

	if(archive.IsStoring()) {
		SER_ARRAY(m_cityList);
		SER_ARRAY(m_unitList);
		SER_ARRAY(m_armyList);
		SER_ARRAY(m_playerList);
		SER_ARRAY(m_intList);
		SER_ARRAY(m_unitRecordList);
		SER_ARRAY(m_locationList);
		SER_ARRAY(m_agreementList);
		SER_ARRAY(m_tradeOffersList);
		SER_ARRAY(m_goodList);
		SER_ARRAY(m_governmentList);
		SER_ARRAY(m_advanceList);

		archive << m_numCalamities;
		if(m_numCalamities > 0)
			archive.Store((uint8*)m_calamityList, m_numCalamities * sizeof(sint32));

		archive << m_numGolds;
		if(m_numGolds > 0)
			archive.Store((uint8*)m_goldList, m_numGolds * sizeof(sint32));

		archive << m_numRanks;
		if(m_numRanks > 0)
			archive.Store((uint8*)m_rankList, m_numRanks * sizeof(sint32));

		archive << m_numWonders;
		if(m_numWonders > 0)
			archive.Store((uint8*)m_wonderList, m_numWonders * sizeof(sint32));

		archive << m_numOrders;
		if(m_numOrders > 0) {
			archive.Store((uint8*)m_orderList, m_numOrders * sizeof(sint32));
		}

		archive << m_numMadlibs;
		if(m_numMadlibs > 0) {
			archive.Store((uint8*)m_madlibChoiceList, m_numMadlibs * sizeof(sint32));
			archive.Store((uint8*)m_madlibNameList, m_numMadlibs * sizeof(sint32));
        }

		archive << m_numAttitudes;
		if(m_numAttitudes > 0)
			archive.Store((uint8*)m_attitudeList, m_numAttitudes * sizeof(sint32));

		archive << m_numAges;
		if(m_numAges > 0)
			archive.Store((uint8*)m_ageList, m_numAges * sizeof(sint32));

		archive << m_numActions;
		for(sint32 i = 0; i < m_numActions; i++) {
			l = strlen(m_actionList[i]) + 1;
			archive << l;
			archive.Store((uint8*)m_actionList[i], l);
		}

		archive << m_numBuildings;
		if(m_numBuildings > 0) {
			archive.Store((uint8 *)m_buildingList, m_numBuildings * sizeof(sint32));
		}

		archive << m_numTradeBids;
		if(m_numTradeBids > 0) {
			archive.Store((uint8 *)m_tradeBidList, m_numTradeBids * sizeof(uint32));
		}
	} else {
		UNSER_ARRAY(m_cityList, Unit);
		UNSER_ARRAY(m_unitList, Unit);
		UNSER_ARRAY(m_armyList, Army);
		UNSER_ARRAY(m_playerList, sint32);
		UNSER_ARRAY(m_intList, sint32);
		UNSER_ARRAY(m_unitRecordList, sint32);
		UNSER_ARRAY(m_locationList, MapPoint);
		UNSER_ARRAY(m_agreementList, ai::Agreement);
		UNSER_ARRAY(m_tradeOffersList, TradeOffer);
		UNSER_ARRAY(m_goodList, sint32);
		UNSER_ARRAY(m_governmentList, sint32);
		UNSER_ARRAY(m_advanceList, sint32);

		archive >> m_numCalamities;
		if(m_numCalamities > 0) {
			m_calamityList = new sint32[m_numCalamities];
			archive.Load((uint8*)m_calamityList, m_numCalamities * sizeof(sint32));
		} else {
			m_calamityList = NULL;
		}

		archive >> m_numGolds;
		if(m_numGolds > 0) {
			m_goldList = new sint32[m_numGolds];
			archive.Load((uint8*)m_goldList, m_numGolds * sizeof(sint32));
		} else {
			m_goldList = NULL;
		}

		archive >> m_numRanks;
		if(m_numRanks > 0) {
			m_rankList = new sint32[m_numRanks];
			archive.Load((uint8*)m_rankList, m_numRanks * sizeof(sint32));
		} else {
			m_rankList = NULL;
		}

		archive >> m_numWonders;
		if(m_numWonders > 0) {
			m_wonderList = new sint32[m_numWonders];
			archive.Load((uint8*)m_wonderList, m_numWonders * sizeof(sint32));
		} else {
			m_wonderList = NULL;
		}

		archive >> m_numOrders;
		if(m_numOrders > 0) {
			m_orderList = new sint32[m_numOrders];
			archive.Load((uint8*)m_orderList, m_numOrders * sizeof(sint32));
		} else {
			m_orderList = NULL;
		}

		archive >> m_numMadlibs;
		if(m_numMadlibs > 0) {
			m_madlibChoiceList = new sint32[m_numMadlibs];
			archive.Load((uint8*)m_madlibChoiceList, m_numMadlibs * sizeof(sint32));
			m_madlibNameList = new sint32[m_numMadlibs];
			archive.Load((uint8*)m_madlibNameList, m_numMadlibs * sizeof(sint32));
		} else {
			m_madlibChoiceList = NULL;
			m_madlibNameList = NULL;
		}

		archive >> m_numAttitudes;
		if(m_numAttitudes > 0) {
			m_attitudeList = new sint32[m_numAttitudes];
			archive.Load((uint8*)m_attitudeList, m_numAttitudes * sizeof(sint32));
		} else {
			m_attitudeList = NULL;
		}

		archive >> m_numAges;
		if(m_numAges > 0) {
			m_ageList = new sint32[m_numAges];
			archive.Load((uint8*)m_ageList, m_numAges * sizeof(sint32));
		} else {
			m_ageList = NULL;
		}

		archive >> m_numActions;
		if(m_numActions > 0) {
			m_actionList = new MBCHAR *[m_numActions];
			for(sint32 i = 0; i < m_numActions; i++) {
				archive >> l;
				m_actionList[i] = new MBCHAR[l];
				archive.Load((uint8*)m_actionList[i], l);
			}
		} else {
			m_actionList = NULL;
		}

		archive >> m_numBuildings;
		if(m_numBuildings > 0) {
			m_buildingList = new sint32[m_numBuildings];
			archive.Load((uint8*)m_buildingList, m_numBuildings * sizeof(sint32));
		} else {
			m_buildingList = NULL;
		}

		archive >> m_numTradeBids;
		if(m_numTradeBids > 0) {
			m_tradeBidList = new uint32[m_numTradeBids];
			archive.Load((uint8*)m_tradeBidList, m_numTradeBids * sizeof(uint32));
		} else {
			m_tradeBidList = NULL;
		}
	}
}

void SlicContext::AddCity(const Unit &c)
{
	if(!m_cityList)
		m_cityList = new SimpleDynamicArray<Unit>;

	m_cityList->Insert(c);
}

void SlicContext::AddUnit(const Unit &u)
{
	if(!m_unitList)
		m_unitList = new SimpleDynamicArray<Unit>;
	m_unitList->Insert(u);
}

void SlicContext::AddArmy(const Army &a)
{
	if(!m_armyList)
		m_armyList = new SimpleDynamicArray<Army>;
	m_armyList->Insert(a);
}

void SlicContext::AddPlayer(const PLAYER_INDEX player)
{
	if(!m_playerList)
		m_playerList = new SimpleDynamicArray<sint32>;
	m_playerList->Insert(player);
}

void SlicContext::AddInt(sint32 val)
{
	if(!m_intList)
		m_intList = new SimpleDynamicArray<sint32>;
	m_intList->Insert(val);
}

void SlicContext::AddUnitRecord(sint32 rec)
{
	if(!m_unitRecordList)
		m_unitRecordList = new SimpleDynamicArray<sint32>;
	m_unitRecordList->Insert(rec);
}

void SlicContext::AddCivilisation(const PLAYER_INDEX player)
{
	AddPlayer(player);
}

void SlicContext::AddPlayer(const Civilisation &civ)
{
	if(!m_playerList)
		m_playerList = new SimpleDynamicArray<sint32>;
	m_playerList->Insert(civ.GetOwner());
}

void SlicContext::AddCivilisation(const Civilisation &civ)
{
	AddPlayer(civ);
}

void SlicContext::AddAdvance(const AdvanceType advance)
{
	if(!m_advanceList)
		m_advanceList = new SimpleDynamicArray<sint32>;

	m_advanceList->Insert(advance);
}

void SlicContext::AddLocation(const MapPoint &point)
{
	if(!m_locationList)
		m_locationList = new SimpleDynamicArray<MapPoint>;
	m_locationList->Insert(point);
}

void SlicContext::AddCalamity(const sint32 calamity)
{
	m_calamityList = Expand(m_calamityList, m_numCalamities);
	m_calamityList[m_numCalamities++] = calamity;
}

void SlicContext::AddGold(const sint32 goldAmount)
{
	m_goldList = Expand(m_goldList, m_numGolds);
	m_goldList[m_numGolds++] = goldAmount;
}

void SlicContext::AddGood(const sint32 goodIndex)
{
	if(!m_goodList)
		m_goodList = new SimpleDynamicArray<sint32>;
	m_goodList->Insert(goodIndex);
}

void SlicContext::AddRank(const sint32 rank)
{
	m_rankList = Expand(m_rankList, m_numRanks);
	m_rankList[m_numRanks++] = rank;
}

void SlicContext::AddWonder(const sint32 wonderIndex)
{
	m_wonderList = Expand(m_wonderList, m_numWonders);
	m_wonderList[m_numWonders++] = wonderIndex;
}

void SlicContext::AddAction(const MBCHAR *action)
{
	MBCHAR **newList = new MBCHAR*[m_numActions + 1];
	if(m_actionList) {
		memcpy(newList, m_actionList, m_numActions * sizeof(MBCHAR *));
		delete [] m_actionList;
	}
	m_actionList = newList;
    sint32 n = strlen(action) + 1;
	newList[m_numActions] = new MBCHAR[n];
    memset(newList[m_numActions], 0, n); 
	strcpy(newList[m_numActions], action);
	m_numActions++;
}

void SlicContext::SetAction(sint32 index, const MBCHAR *action)
{
	while(m_numActions <= index) {
		AddAction("");
	}
	delete [] m_actionList[index];
	m_actionList[index] = new MBCHAR[strlen(action) + 1];
	strcpy(m_actionList[index], action);
}

sint32 *SlicContext::Expand(sint32 *list, sint32 size)
{
	sint32 *newList = new sint32[size + 1];
	if(list) {
		
		memcpy(newList, list, size * sizeof(sint32));
		delete [] list;
	}

	return newList;
}

void SlicContext::CopyArray(sint32 *&to, sint32 *from, 
							sint32 &tosize, sint32 size)
{
	if(!from || size == 0) {
		to = NULL;
		tosize = 0;
		return;
	}
	to = new sint32[size];
	memcpy(to, from, size * sizeof(sint32));
	tosize = size;
}

Unit SlicContext::GetCity(sint32 index) const
{
	if(m_eventArgs) {
		Unit city;
		if(m_eventArgs->GetCity(index, city))
			return city;
	}

	if(!m_cityList)
		return Unit(0);

	return m_cityList->Access(index);
}

sint32 SlicContext::GetNumCities() const
{
	if(!m_cityList)
		return 0;

	return m_cityList->Num();
}

Unit SlicContext::GetUnit(sint32 index) const
{
	if(m_eventArgs) {
		Unit u;
		if(m_eventArgs->GetUnit(index, u))
			return u;
	}

	if(!m_unitList)
		return Unit(0);
	return m_unitList->Access(index);
}

sint32 SlicContext::GetNumUnits() const
{
	if(!m_unitList)
		return 0;
	return m_unitList->Num();
}

Army SlicContext::GetArmy(sint32 index) const
{
	if(!m_armyList)
		return Army(0);
	return m_armyList->Access(index);
}

sint32 SlicContext::GetNumArmies() const
{
	if(!m_armyList)
		return 0;
	return m_armyList->Num();
}

sint32 SlicContext::GetPlayer(sint32 index) const
{
	if(m_eventArgs) {
		sint32 p;
		if(m_eventArgs->GetPlayer(index, p))
			return p;
	}

	if(!m_playerList)
		return -1;

	return (*m_playerList)[index];
}

sint32 SlicContext::GetInt(sint32 index) const
{
	if(!m_intList)
		return 0;
	return (*m_intList)[index];
}

sint32 SlicContext::GetUnitRecord(sint32 index) const
{
	if(!m_unitRecordList)
		return 0;
	return (*m_unitRecordList)[index];
}

void SlicContext::SetPlayer(sint32 index, sint32 &player)
{
	if(!m_playerList)
		m_playerList = new SimpleDynamicArray<sint32>;

	if(index < 0)
		return;

	if(index >= m_playerList->Num()) {
		while(index >= m_playerList->Num()) {
			m_playerList->Insert(player);
		}
	} else {
		(*m_playerList)[index] = player;
	}
}

void SlicContext::SetInt(sint32 index, sint32 &val)
{
	if(!m_intList)
		m_intList = new SimpleDynamicArray<sint32>;
	
	if(index < 0)
		return;

	if(index >= m_intList->Num()) {
		while(index >= m_intList->Num()) {
			m_intList->Insert(val);
		}
	} else {
		(*m_intList)[index] = val;
	}
}

void SlicContext::SetUnitRecord(sint32 index, sint32 rec)
{
	if(!m_unitRecordList)
		m_unitRecordList = new SimpleDynamicArray<sint32>;
	
	if(index < 0)
		return;

	if(index >= m_unitRecordList->Num()) {
		while(index >= m_unitRecordList->Num()) {
			m_unitRecordList->Insert(rec);
		}
	} else {
		(*m_unitRecordList)[index] = rec;
	}
}

void SlicContext::SetUnit(sint32 index, Unit &u)
{
	if(!m_unitList)
		m_unitList = new SimpleDynamicArray<Unit>;

	if(index < 0)
		return;
	if(index >= m_unitList->Num()) {
		while(index >= m_unitList->Num()) {
			m_unitList->Insert(u);
		}
	} else {
		m_unitList->Access(index) = u;
	}
}

void SlicContext::SetArmy(sint32 index, Army &a)
{
	if(!m_armyList)
		m_armyList = new SimpleDynamicArray<Army>;
	if(index < 0)
		return ;
	if(index >= m_armyList->Num()) {
		while(index >= m_armyList->Num()) {
			m_armyList->Insert(a);
		}
	} else {
		m_armyList->Access(index) = a;
	}
}

void SlicContext::SetCity(sint32 index, Unit &city)
{
	if(!m_cityList)
		m_cityList = new SimpleDynamicArray<Unit>;

	if(index < 0)
		return;

	if(index >= m_cityList->Num()) {
		while(index >= m_cityList->Num()) {
			m_cityList->Insert(city);
		}
	} else {
		m_cityList->Access(index) = city;
	}
}

void SlicContext::SetLocation(sint32 index, MapPoint &point)
{
	if(!m_locationList)
		m_locationList = new SimpleDynamicArray<MapPoint>;

	if(index < 0)
		return;

	if(index >= m_locationList->Num()) {
		while(index >= m_locationList->Num()) {
			m_locationList->Insert(point);
		}
	} else {
		m_locationList->Access(index) = point;
	}
}

sint32 SlicContext::GetNumPlayers() const
{
	if(!m_playerList)
		return 0;
	return m_playerList->Num();
}

sint32 SlicContext::GetNumInts() const
{
	if(!m_intList)
		return 0;
	return m_intList->Num();
}

sint32 SlicContext::GetNumUnitRecords() const
{
	if(!m_unitRecordList) {
		return 0;
	}
	return m_unitRecordList->Num();
}

AdvanceType SlicContext::GetAdvance(sint32 index) const
{
	if(m_eventArgs) {
		AdvanceType a;
		if(m_eventArgs->GetAdvance(index, a))
			return a;
	}

	if(!m_advanceList)
		return 0;

	Assert(index >= 0 && index < m_advanceList->Num());
	return (*m_advanceList)[index];
}

sint32 SlicContext::GetNumAdvances() const
{
	return m_advanceList ? m_advanceList->Num() : 0;
}

MapPoint SlicContext::GetLocation(sint32 index) const
{
	if(m_eventArgs) {
		MapPoint pos;
		if(m_eventArgs->GetPos(index, pos))
			return pos;
	}

	if(!m_locationList)
		return MapPoint(0,0);
	return m_locationList->Access(index);
}

sint32 SlicContext::GetNumLocations() const
{
	if(!m_locationList)
		return 0;
	return m_locationList->Num();
}

sint32 SlicContext::GetCalamity(sint32 index) const
{
	if(!m_calamityList)
		return 0;
	Assert(index >= 0 && index < m_numCalamities);
	return m_calamityList[index];
}

sint32 SlicContext::GetNumCalamities() const
{
	return m_numCalamities;
}

sint32 SlicContext::GetGold(sint32 index) const
{
	if(!m_goldList)
		return 0;
	Assert(index >= 0 && index < m_numGolds);
	return m_goldList[index];
}

sint32 SlicContext::GetNumGolds() const
{
	return m_numGolds;
}

sint32 SlicContext::GetGood(sint32 index) const
{
	if(!m_goodList)
		return 0;
	Assert(index >= 0 && index < m_goodList->Num());
	return m_goodList->Access(index);
}

void SlicContext::SetGood(sint32 index, sint32 &good)
{
	if(!m_goodList)
		m_goodList = new SimpleDynamicArray<sint32>;

	if(index < 0)
		return;

	if(index >= m_goodList->Num()) {
		while(index >= m_goodList->Num()) {
			m_goodList->Insert(good);
		}
	} else {
		(*m_goodList)[index] = good;
	}
}

void SlicContext::SetAdvance(sint32 index, sint32 &adv)
{
	if(!m_advanceList)
		m_advanceList = new SimpleDynamicArray<sint32>;

	if(index < 0)
		return;

	if(index >= m_advanceList->Num()) {
		while(index >= m_advanceList->Num()) {
			m_advanceList->Insert(adv);
		}
	} else {
		(*m_advanceList)[index] = adv;
	}
}

void SlicContext::SetGovernment(sint32 index, sint32 &gov)
{
	if(!m_governmentList)
		m_governmentList = new SimpleDynamicArray<sint32>;

	if(index < 0)
		return;

	if(index >= m_governmentList->Num()) {
		while(index >= m_governmentList->Num()) {
			m_governmentList->Insert(gov);
		}
	} else {
		(*m_governmentList)[index] = gov;
	}
}

sint32 SlicContext::GetNumGoods() const
{
	return m_goodList->Num();
}

BOOL SlicContext::HaveGoodOfType(sint32 good) const
{
	sint32 i;
	for(i = 0; i < m_goodList->Num(); i++) {
		if(m_goodList->Access(i) == good)
			return TRUE;
	}
	return FALSE;
}

sint32 SlicContext::GetRank(sint32 index) const
{
	if(!m_rankList)
		return 0;
	Assert(index >= 0 && index < m_numRanks);
	return m_rankList[index];
}

sint32 SlicContext::GetNumRanks() const
{
	return m_numRanks;
}

sint32 SlicContext::GetWonder(sint32 index) const
{
	if(m_eventArgs) {
		sint32 w;
		if(m_eventArgs->GetWonder(index, w))
			return w;
	}

	if(!m_wonderList)
		return 0;
	Assert(index >= 0 && index < m_numWonders);
	return m_wonderList[index];
}

sint32 SlicContext::GetNumWonders() const
{
	return m_numWonders;
}

MBCHAR *SlicContext::GetAction(sint32 index) const
{
	if(!m_actionList)
		return NULL;
	Assert(index >= 0 && index < m_numActions);
	return m_actionList[index];
}

sint32 SlicContext::GetNumActions() const
{
	return m_numActions;
}

BOOL SlicContext::ConcernsPlayer(PLAYER_INDEX player)
{
	if(!m_playerList)
		return FALSE;

	for(sint32 i = 0; i < m_playerList->Num(); i++) {
		if((*m_playerList)[i] == player)
			return TRUE;
	}
	return FALSE;
}

void SlicContext::AddAgreement(const ai::Agreement &agreement)
{
	if(!m_agreementList) {
		m_agreementList = new SimpleDynamicArray<ai::Agreement>;
	}

	m_agreementList->Insert(agreement);
}

ai::Agreement SlicContext::GetAgreement(sint32 index) const
{
	ai::Agreement agreement;
	if(!m_agreementList || index < 0 || index >= m_agreementList->Num())
		return agreement;

	return m_agreementList->Access(index);
}

sint32 SlicContext::GetNumAgreements() const
{
	if(!m_agreementList) return 0;
	return m_agreementList->Num();
}

void SlicContext::AddTradeOffer(const TradeOffer &offer)
{
	if(!m_tradeOffersList)
		m_tradeOffersList = new SimpleDynamicArray<TradeOffer>;
	
	m_tradeOffersList->Insert(offer);
}

sint32 SlicContext::GetNumTradeOffers() const
{
	if(!m_tradeOffersList) return 0;
	return m_tradeOffersList->Num();
}

TradeOffer SlicContext::GetTradeOffer(sint32 index) const
{
	if(!m_tradeOffersList || index < 0 || index >= m_tradeOffersList->Num())
		return TradeOffer(0);

	return m_tradeOffersList->Access(index);
}

void SlicContext::AddGovernment(const sint32 gov)
{
	if(!m_governmentList)
		m_governmentList = new SimpleDynamicArray<sint32>;
	m_governmentList->Insert(gov);
}

sint32 SlicContext::GetNumGovernments() const
{
	return m_governmentList ? m_governmentList->Num() : 0;
}

sint32 SlicContext::GetGovernment(const sint32 index) const
{
	if(!m_governmentList)
		return -1;

	return (*m_governmentList)[index];
}

void SlicContext::AddOrder(UNIT_ORDER_TYPE order)
{
	m_orderList = Expand(m_orderList, m_numOrders);
	m_orderList[m_numOrders++] = (sint32)order;
}

void SlicContext::SetOrder(sint32 index, UNIT_ORDER_TYPE order)
{
	if(index < 0)
		return;

	if(index >= m_numOrders) {
		m_orderList = Expand(m_orderList, index);
		m_numOrders = index + 1;
	}
	m_orderList[index] = order;	
}

sint32 SlicContext::GetNumOrders() const
{
	return m_numOrders;
}

UNIT_ORDER_TYPE SlicContext::GetOrder(const sint32 index) const
{
	if(!m_orderList || index < 0 || index >= m_numOrders)
		return UNIT_ORDER_NONE;
	return (UNIT_ORDER_TYPE)m_orderList[index];
}

void SlicContext::AddBuilding(sint32 building)
{
	m_buildingList = Expand(m_buildingList, m_numBuildings);
	m_buildingList[m_numBuildings++] = building;
}

sint32 SlicContext::GetNumBuildings() const
{
	return m_numBuildings;
}

sint32 SlicContext::GetBuilding(sint32 index) const
{
	if(!m_buildingList || index < 0 || index >= m_numBuildings)
		return -1;
	return m_buildingList[index];
}

void SlicContext::AddTradeBid(uint32 bid)
{
	m_tradeBidList = (uint32*)Expand((sint32*)m_tradeBidList, m_numTradeBids);
	m_tradeBidList[m_numTradeBids++] = bid;
}

sint32 SlicContext::GetNumTradeBids() const
{
	return m_numTradeBids;
}

uint32 SlicContext::GetTradeBid(sint32 index) const
{
	if(!m_tradeBidList || index < 0 || index >= m_numTradeBids)
		return 0;
	return m_tradeBidList[index];
}

void SlicContext::AddMadlib(char *name, const sint32 choice)
{
    
	m_madlibChoiceList = Expand(m_madlibChoiceList, m_numMadlibs);
	m_madlibChoiceList[m_numMadlibs] = choice;

	m_madlibNameList = Expand(m_madlibNameList, m_numMadlibs);
	m_madlibNameList[m_numMadlibs++] = Hash(name);
}

sint32 SlicContext::GetNumMadlibs() const
{
	return m_numMadlibs;
}

sint32 SlicContext::GetMadlib(char *name)
{
    int i;

	if(!m_madlibNameList || !m_madlibChoiceList || 
       !name || (m_numMadlibs <= 0))
		return -1;

    sint32 nameHash = Hash(name);

    for(i=0; i<m_numMadlibs; i++) {
        if (m_madlibNameList[i] == nameHash) {
            return m_madlibChoiceList[i];
        }
    }

	return -1;
}

void SlicContext::AddAttitude(const sint32 tude)
{
	m_attitudeList = Expand(m_attitudeList, m_numAttitudes);
	m_attitudeList[m_numAttitudes++] = tude;
}

sint32 SlicContext::GetNumAttitudes() const
{
	return m_numAttitudes;
}

sint32 SlicContext::GetAttitude(const sint32 index) const
{
	if(!m_attitudeList || index < 0 || index >= m_numAttitudes)
		return -1;

	return m_attitudeList[index];
}

void SlicContext::AddAge(const sint32 tude)
{
	m_ageList = Expand(m_ageList, m_numAges);
	m_ageList[m_numAges++] = tude;
}

sint32 SlicContext::GetNumAges() const
{
	return m_numAges;
}

sint32 SlicContext::GetAge(const sint32 index) const
{
	if(!m_ageList || index < 0 || index >= m_numAges)
		return -1;

	return m_ageList[index];
}

sint32 SlicContext::Hash(char *name)
{
    char *p;
    unsigned long rval = 0;

    for(p=name; *p; p++) {
        rval = (rval << 8 | rval >> 24) + *p;
    }

    return((sint32)rval);
}



void SlicContext::DelCity()
{
    if (m_cityList) {
        m_cityList->DelIndex(m_cityList->Num() - 1);
    }
}

void SlicContext::DelUnit()
{
    if (m_unitList) {
        m_unitList->DelIndex(m_unitList->Num() - 1);
    }
}

void SlicContext::DelPlayer()
{
	Assert(FALSE);
    
    
    
}

void SlicContext::DelAdvance()
{
	Assert(FALSE);
}

void SlicContext::DelLocation()
{
    if (m_locationList) {
        m_locationList->DelIndex(m_locationList->Num() - 1);
    }
}

void SlicContext::DelCalamity()
{
    if (m_numCalamities > 0)
        m_numCalamities--;
}

void SlicContext::DelGold()
{
    if (m_numGolds > 0)
        m_numGolds--;
}

void SlicContext::DelGood()
{
    if (m_goodList->Num() > 0)
        m_goodList->DelIndex(m_goodList->Num() - 1);
}

void SlicContext::DelRank()
{
    if (m_numRanks > 0)
        m_numRanks--;
}

void SlicContext::DelWonder()
{
    if (m_numWonders > 0)
        m_numWonders--;
}








void SlicContext::DelAgreement()
{
    if (m_agreementList) {
        m_agreementList->DelIndex(m_agreementList->Num() - 1);
    }
}

void SlicContext::DelTradeOffer()
{
    if (m_tradeOffersList) {
        m_tradeOffersList->DelIndex(m_tradeOffersList->Num() - 1);
    }
}

void SlicContext::DelGovernment()
{
	Assert(FALSE);
}

void SlicContext::DelMadlib()
{
    if (m_numMadlibs > 0)
        m_numMadlibs--;
}

void SlicContext::DelAttitude()
{
    if (m_numAttitudes > 0)
        m_numAttitudes--;
}

void SlicContext::DelAge()
{
    if (m_numAges > 0)
        m_numAges--;
}


#ifdef _DEBUG
void SlicContext::Dump()
{
	sint32 i, n;
	if(m_cityList) {
		n = m_cityList->Num();
		for(i = 0; i < n; i++) {
			DPRINTF(k_DBG_INFO, (" City.%d: %lx\n", i, m_cityList->Access(i)));
		}
	}

	if(m_unitList) {
		n = m_unitList->Num();
		for(i = 0; i < n; i++) {
			DPRINTF(k_DBG_INFO, (" Unit.%d: %lx\n", i, m_unitList->Access(i)));
		}
	}

	if(m_playerList) {
		n = m_playerList->Num();
		for(i = 0; i < n; i++) {
			DPRINTF(k_DBG_INFO, (" Civ.%d: %lx\n", i, (*m_playerList)[i]));
		}
	}

	if(m_locationList) {
		n = m_locationList->Num();
		for(i = 0; i < n; i++) {
			MapPoint pos = m_locationList->Access(i);
			DPRINTF(k_DBG_INFO, (" Location.%d: (%d,%d)\n", i,
								 pos.x, pos.y));
		}
	}

	if(m_calamityList) {
		n = m_numCalamities;
		for(i = 0; i < n; i++) {
			DPRINTF(k_DBG_INFO, (" Calamity.%d: %d\n",i, m_calamityList[i]));
		}
	}

	if(m_goldList) {
		n = m_numGolds;
		for(i = 0; i < n; i++) {
			DPRINTF(k_DBG_INFO, (" Gold.%d: %d\n", i, m_goldList[i]));
		}
	}

	if(m_goodList) {
		n = m_goodList->Num();
		for(i = 0; i < n; i++) {
			DPRINTF(k_DBG_INFO, (" Good.%d: %d\n", i, m_goodList->Access(i)));
		}
	}

	if(m_rankList) {
		n = m_numRanks;
		for(i = 0; i < n; i++) {
			DPRINTF(k_DBG_INFO, (" Rank.%d: %d\n", i, m_rankList[i]));
		}
	}

	if(m_wonderList) {
		n = m_numWonders;
		for(i = 0; i < n; i++) {
			DPRINTF(k_DBG_INFO, (" Wonder.%d: %d\n", i, m_wonderList[i]));
		}
	}

	if(m_actionList) {
		for(sint32 i = 0; i < m_numActions; i++) {
			DPRINTF(k_DBG_INFO, (" Action.%d: %s\n", i, m_actionList[i]));
		}
	}
}
#endif

void SlicContext::Snarf(GameEventArgList *args)
{
	sint32 i;
	sint32 c;

	Unit u;
	for(i = 0, c = args->GetArgCount(GEA_Unit); i < c; i++) {
		args->GetArg(GEA_Unit, i)->GetUnit(u);
		SetUnit(i, u);
	}

	Unit city;
	for(i = 0, c = args->GetArgCount(GEA_City); i < c; i++) {
		args->GetArg(GEA_City, i)->GetCity(city);
		SetCity(i, city);
	}

	Army a;
	for(i = 0, c = args->GetArgCount(GEA_Army); i < c; i++) {
		args->GetArg(GEA_Army, i)->GetArmy(a);
		SetArmy(i, a);
	}

	sint32 p;
	for(i = 0, c = args->GetArgCount(GEA_Player); i < c; i++) {
		args->GetArg(GEA_Player, i)->GetPlayer(p);
		SetPlayer(i, p);
	}
		
	MapPoint pos;
	for(i = 0, c = args->GetArgCount(GEA_MapPoint); i < c; i++) {
		args->GetArg(GEA_MapPoint, i)->GetPos(pos);
		SetLocation(i, pos);
	}

	sint32 val;
	for(i = 0, c = args->GetArgCount(GEA_Int); i < c; i++) {
		args->GetArg(GEA_Int, i)->GetInt(val);
		SetInt(i, val);
	}
}

//----------------------------------------------------------------------------
//
// Name       : FILL
//
// Description: -
//
// Parameters : builtin     : type of built-in object
//              list        : list to fill
//              symtype     : type of Slic object
//              setMethod   : method to use to copy from built-in to list
//
// Globals    : g_slicEngine
//
// Returns    : -
//
// Remark(s)  : MACRO
//
//              The local setSym variable is reset to SLIC_SYM_UNDEFINED type
//              before going out of scope, to prevent deleting member data
//              that has been copied to the list.
//
//----------------------------------------------------------------------------
#define FILL(builtin, list, symtype, setMethod)                     \
	array = g_slicEngine->GetBuiltinSymbol(builtin)->GetArray();    \
    array->Prune(list ? list->Num() : 0);                           \
	if(list) {                                                      \
        SlicSymbolData setSym(symtype);                             \
        stackVal.m_sym = &setSym;                                   \
		for(sint32 i = 0; i < list->Num(); i++) {                   \
			setSym.setMethod(list->Access(i));                      \
			array->Insert(i, SS_TYPE_SYM, stackVal);                \
		}                                                           \
        setSym.SetType(SLIC_SYM_UNDEFINED);                         \
	}

void SlicContext::FillBuiltins()
{
    SlicArray *             array;
	SlicStackValue          stackVal;

    // The following lists all have SimpleDynamicArray structure.

	FILL(SLIC_BUILTIN_CITY, m_cityList, SLIC_SYM_CITY, SetCity);
	FILL(SLIC_BUILTIN_UNIT, m_unitList, SLIC_SYM_UNIT, SetUnit);
	FILL(SLIC_BUILTIN_PLAYER, m_playerList, SLIC_SYM_IVAR, SetIntValue);
	FILL(SLIC_BUILTIN_ARMY, m_armyList, SLIC_SYM_ARMY, SetArmy);
	FILL(SLIC_BUILTIN_LOCATION, m_locationList, SLIC_SYM_LOCATION, SetPos);
	FILL(SLIC_BUILTIN_VALUE, m_intList, SLIC_SYM_IVAR, SetIntValue);
	FILL(SLIC_BUILTIN_UNITRECORD, m_unitRecordList, SLIC_SYM_IVAR, SetIntValue);
	FILL(SLIC_BUILTIN_GOOD, m_goodList, SLIC_SYM_IVAR, SetIntValue);
	FILL(SLIC_BUILTIN_ADVANCE, m_advanceList, SLIC_SYM_IVAR, SetIntValue);
	FILL(SLIC_BUILTIN_GOVERNMENT, m_governmentList, SLIC_SYM_IVAR, SetIntValue);

    // The following lists do not have SimpleDynamicArray structure,
    // but are plain arrays, with a separate count.

	array = g_slicEngine->GetBuiltinSymbol(SLIC_BUILTIN_ACTION)->GetArray();
    array->Prune(m_actionList ? m_numActions : 0);                          
	if (m_actionList) {
		SlicSymbolData setSym(SLIC_SYM_STRING);
        stackVal.m_sym = &setSym;                    
		for(sint32 i = 0; i < m_numActions; i++) {
			setSym.SetString(m_actionList[i]);
			array->Insert(i, SS_TYPE_SYM, stackVal);
		}
        setSym.SetType(SLIC_SYM_UNDEFINED);
	}

	array = g_slicEngine->GetBuiltinSymbol(SLIC_BUILTIN_BUILDING)->GetArray();
    array->Prune(m_buildingList ? m_numBuildings : 0);                         
	if(m_buildingList) {
		SlicSymbolData setSym(SLIC_SYM_IVAR);
        stackVal.m_sym = &setSym;         
		for(sint32 i = 0; i < m_numBuildings; i++) {
			setSym.SetIntValue(m_buildingList[i]);
			array->Insert(i, SS_TYPE_SYM, stackVal);
		}
        setSym.SetType(SLIC_SYM_UNDEFINED);
	}

	array = g_slicEngine->GetBuiltinSymbol(SLIC_BUILTIN_WONDER)->GetArray();
    array->Prune(m_wonderList ? m_numWonders : 0);                         
	if(m_wonderList) {
		SlicSymbolData setSym(SLIC_SYM_IVAR);
        stackVal.m_sym = &setSym; 
		for(sint32 i = 0; i < m_numWonders; i++) {
			setSym.SetIntValue(m_wonderList[i]);
			array->Insert(i, SS_TYPE_SYM, stackVal);
		}
        setSym.SetType(SLIC_SYM_UNDEFINED);
	}

	array = g_slicEngine->GetBuiltinSymbol(SLIC_BUILTIN_GOLD)->GetArray();
    array->Prune(m_goldList ? m_numGolds : 0);                         
	if(m_goldList) {
		SlicSymbolData setSym(SLIC_SYM_IVAR);
        stackVal.m_sym = &setSym; 
		for(sint32 i = 0; i < m_numGolds; i++) {
			setSym.SetIntValue(m_goldList[i]);
			array->Insert(i, SS_TYPE_SYM, stackVal);
		}
        setSym.SetType(SLIC_SYM_UNDEFINED);
	}
}

#undef FILL

#define UNFILL(list, type, GetMethod, SetMethod) \
		if(array->GetSize() > 0) {\
			for(i = 0; i < array->GetSize(); i++) {\
				if(array->Lookup(i, stype, sval)) {\
					type v;\
					if(sval.m_sym->GetMethod(v)) {\
						SetMethod(i, v);\
					}\
				}\
			}\
		} else if(list) {\
			delete list;\
			list = NULL;\
		}

void SlicContext::CopyFromBuiltins()
{
	sint32 i, b;
	for(b = 0; b < SLIC_BUILTIN_MAX; b++) {
		SlicSymbolData const * sym = g_slicEngine->GetBuiltinSymbol((SLIC_BUILTIN)b);
		if(!sym) continue;

		if(sym->GetType() != SLIC_SYM_ARRAY) continue;

		SlicArray *array = sym->GetArray();
		Assert(array);
		if(!array) continue;
		
		SS_TYPE stype;
		SlicStackValue sval;

		if(array->GetSize() < 1) continue;

		
		switch(b) {
			case SLIC_BUILTIN_PLAYER:       UNFILL(m_playerList, sint32, GetPlayer, SetPlayer); break;
			case SLIC_BUILTIN_CITY:  		UNFILL(m_cityList, Unit, GetCity, SetCity); break;
			case SLIC_BUILTIN_UNIT:			UNFILL(m_unitList, Unit, GetUnit, SetUnit); break;
			case SLIC_BUILTIN_GLOBAL:       break;
			case SLIC_BUILTIN_ARMY:         UNFILL(m_armyList, Army, GetArmy, SetArmy); break;
			case SLIC_BUILTIN_LOCATION:     UNFILL(m_locationList, MapPoint, GetPos, SetLocation); break;
				
			case SLIC_BUILTIN_ADVANCE:      UNFILL(m_advanceList, sint32, GetIntValue, SetAdvance); break;
			case SLIC_BUILTIN_GOVERNMENT:   UNFILL(m_governmentList, sint32, GetIntValue, SetGovernment); break;
			case SLIC_BUILTIN_GOLD:         break;
			case SLIC_BUILTIN_POP:          break;
			case SLIC_BUILTIN_GOOD:         UNFILL(m_goodList, sint32, GetIntValue, SetGood); break;
			case SLIC_BUILTIN_ACTION:       break;
			case SLIC_BUILTIN_IMPROVEMENT:  break;
			case SLIC_BUILTIN_VALUE:        UNFILL(m_intList, sint32, GetIntValue, SetInt); break;
			case SLIC_BUILTIN_BUILDING:     break;
			case SLIC_BUILTIN_WONDER:       break;
			case SLIC_BUILTIN_UNITRECORD:   UNFILL(m_unitRecordList, sint32, GetIntValue, SetUnitRecord); break;
			default:
				Assert(FALSE);
				break;
		}				
	}
}

#undef UNFILL