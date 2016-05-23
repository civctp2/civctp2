#ifndef __SLIC_CONTEXT_H__
#define __SLIC_CONTEXT_H__

class UnitDynamicArray;
template <class t> class DynamicArray;
template <class t> class SimpleDynamicArray;
class Civilisation;
typedef sint32 AdvanceType;
class MapPoint;
class Unit;
class SlicSegment;
class SlicFrame;
typedef sint32 PLAYER_INDEX;
class CivArchive;
class Agreement;
class TradeOffer;
class GameEventArgList;
class Army;
enum UNIT_ORDER_TYPE;
#include "diplomattypes.h"


class SlicContext {
private:
	GameEventArgList *m_eventArgs;

	SimpleDynamicArray<Unit> *m_cityList;
	SimpleDynamicArray<Unit> *m_unitList;
	SimpleDynamicArray<Army> *m_armyList;
	SimpleDynamicArray<sint32> *m_playerList;
	SimpleDynamicArray<sint32> *m_advanceList;

	SimpleDynamicArray<MapPoint> *m_locationList;
	SimpleDynamicArray<ai::Agreement> *m_agreementList;
	SimpleDynamicArray<sint32> *m_intList;
	SimpleDynamicArray<sint32> *m_unitRecordList;
	SimpleDynamicArray<sint32> *m_goodList;
	SimpleDynamicArray<sint32> *m_governmentList;

	sint32 *m_calamityList;
	sint32 m_numCalamities;

	sint32 *m_goldList;
	sint32 m_numGolds;

	sint32 *m_rankList;
	sint32 m_numRanks;

	sint32 *m_wonderList;
	sint32 m_numWonders;

	MBCHAR **m_actionList;
	sint32 m_numActions;




	sint32 *m_orderList;
	sint32 m_numOrders;

    sint32 *m_madlibNameList;
    sint32 *m_madlibChoiceList;
    sint32 m_numMadlibs;

	sint32 *m_attitudeList;
	sint32 m_numAttitudes;

	sint32 *m_ageList;
	sint32 m_numAges;

	sint32 *m_buildingList;
	sint32 m_numBuildings;

	uint32 *m_tradeBidList;
	sint32 m_numTradeBids;

	SimpleDynamicArray<TradeOffer> *m_tradeOffersList;

protected:
	sint32 *Expand(sint32 *list, sint32 size);
	void CopyArray(sint32 *&to, sint32 *from, sint32 &tosize, sint32 size);
    sint32 Hash(char *name);

public:
	SlicContext();
	SlicContext(SlicContext *copy);
	virtual ~SlicContext();
	void Serialize(CivArchive &archive);

#ifdef _DEBUG
	void Dump();
#endif

	void AddCity(const Unit &c);
	void AddUnit(const Unit &u);
	void AddArmy(const Army &a);
	void AddPlayer(const PLAYER_INDEX player);
	void AddCivilisation(const PLAYER_INDEX player);
	void AddPlayer(const Civilisation &civ);
	void AddInt(const sint32 val);
	void AddCivilisation(const Civilisation &civ);
	void AddAdvance(const AdvanceType advance);
	void AddLocation(const MapPoint &point);
	void AddCalamity(const sint32 calamityIndex);
	void AddGold(const sint32 goldAmount);
	void AddGood(const sint32 goodIndex);
	void AddRank(const sint32 rank);
	void AddWonder(const sint32 wonderIndex);
	void AddAction(const MBCHAR *action);
	void AddAgreement(const ai::Agreement &agreement);
	void AddTradeOffer(const TradeOffer &offer);
	void AddGovernment(const sint32 gov);
    void AddMadlib(char *name, const sint32 choice);
	void AddAttitude(const sint32 tude);
    void AddAge(sint32 ageIndex);
	void AddBuilding(sint32 building);
	void AddTradeBid(uint32 tradeBid);
	void AddOrder(UNIT_ORDER_TYPE order);
	void AddUnitRecord(sint32 type);

	void DelCity();
	void DelUnit();
	void DelPlayer();
	void DelAdvance();
	void DelLocation();
	void DelCalamity();
	void DelGold();
	void DelGood();
	void DelRank();
	void DelWonder();

	void DelAgreement();
	void DelTradeOffer();
	void DelGovernment();
	void DelMadlib();
	void DelAttitude();
    void DelAge();
	void DelPop();

	Unit GetCity(sint32 index) const;
	void SetCity(sint32 index, Unit &city);
	sint32 GetNumCities() const;

	Unit GetUnit(sint32 index) const;
	void SetUnit(sint32 index, Unit &unit);
	sint32 GetNumUnits() const;

	Army GetArmy(sint32 index) const;
	void SetArmy(sint32 index, Army &army);
	sint32 GetNumArmies() const;

	sint32 GetPlayer(sint32 index) const;
	void SetPlayer(sint32 index, sint32 &civ);
	sint32 GetNumPlayers() const;

	sint32 GetInt(sint32 index) const;
	void SetInt(sint32 index, sint32 &val);
	sint32 GetNumInts() const;

	AdvanceType GetAdvance(sint32 index) const;
	void SetAdvance(sint32 index, sint32 &adv);
	sint32 GetNumAdvances() const;

	MapPoint GetLocation(sint32 index) const;
	void SetLocation(sint32 index, MapPoint &point);
	sint32 GetNumLocations() const;
	sint32 GetCalamity(sint32 index) const;
	sint32 GetNumCalamities() const;
	sint32 GetGold(sint32 index) const;
	sint32 GetNumGolds() const;
	sint32 GetGood(sint32 index) const;
	void   SetGood(sint32 index, sint32 &good);
	sint32 GetNumGoods() const;
	BOOL   HaveGoodOfType(sint32 good) const;
	sint32 GetRank(sint32 index) const;
	sint32 GetNumRanks() const;
	sint32 GetWonder(sint32 index) const;
	sint32 GetNumWonders() const;
	MBCHAR *GetAction(sint32 index) const;
	sint32 GetNumActions() const;
	void SetAction(sint32 index, const MBCHAR *action);
	ai::Agreement GetAgreement(sint32 index) const;
	sint32 GetNumAgreements() const;
	sint32 GetNumTradeOffers() const;
	TradeOffer GetTradeOffer(sint32 index) const;

	sint32 GetNumGovernments() const;
	sint32 GetGovernment(sint32 index) const;
	void SetGovernment(sint32 index, sint32 &gov);

    sint32 GetNumMadlibs() const;
    sint32 GetMadlib(char *name);
	sint32 GetNumAttitudes() const;
	sint32 GetAttitude(sint32 index) const;
    sint32 GetNumAges() const;
	sint32 GetAge(sint32 index) const;
	sint32 GetBuilding(sint32 index) const;
	sint32 GetNumBuildings() const;
	uint32 GetTradeBid(sint32 index) const;
	sint32 GetNumTradeBids() const;
	sint32 GetNumPops() const;
	sint32 GetNumOrders() const;
	sint32 GetNumUnitRecords() const;

	UNIT_ORDER_TYPE GetOrder(const sint32 index) const;
	void SetOrder(sint32 index, UNIT_ORDER_TYPE order);

	sint32 GetUnitRecord(const sint32 index) const;
	void SetUnitRecord(sint32 index, sint32 rec);

	virtual BOOL ConcernsPlayer(PLAYER_INDEX player);

	void Snarf(GameEventArgList *args);
	void FillBuiltins();
	void CopyFromBuiltins();
};

#endif
