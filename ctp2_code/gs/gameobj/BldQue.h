//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Build queue handling
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
// _MSC_VER
// - Compiler version (for the Microsoft C++ compiler only)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added assignment operator. - Jul 16th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __BUILD_QUEUE_H__
#define __BUILD_QUEUE_H__ 1

#include "Unit.h"

class CivArchive;



#define k_BUILD_NODE_FLAG_ALREADY_BUILT 0x01
#define k_BUILD_NODE_FLAG_ALMOST_DONE   0x02

struct BuildNode {
	sint32 m_cost;
	sint32 m_type;
	sint32 m_category;
	uint8 m_flags;
	
	
};


enum CAUSE_REMOVE_BUILD_ITEM {
	CAUSE_REMOVE_BUILD_ITEM_MANUAL,
	CAUSE_REMOVE_BUILD_ITEM_ILLEGAL,
	CAUSE_REMOVE_BUILD_ITEM_NETWORK,
	CAUSE_REMOVE_BUILD_ITEM_BUILT,
};



class MapPoint;
class CityData;
class UnitRecord;
template <class T> class PointerList;

class BuildQueue {

	PointerList<BuildNode> *m_list;
	
	PLAYER_INDEX m_owner;
	Unit m_city;
	sint32 m_wonderStarted, m_wonderStopped;
	MBCHAR m_name[256];
	sint32 m_wonderComplete;
	


	
	BuildNode *m_frontWhenBuilt;
	friend class NetCityBuildQueue;

	
private :
	void SendMsgWormholeProbeStarted(void) ;
	void SendMsgWonderStarted(sint32 type) ;
	void SendMsgWormholeProbeComplete(void) ;
	void SendMsgWonderCompleteOwner(CityData *cd, sint32 wonder) ;
	void SendMsgWonderCompleteEveryone(CityData *cd, sint32 wonder) ;
	void SendMsgWonderAlmostComplete(void) ;
	void SendMsgWonderStopped(sint32 type) ;

public:

	BOOL m_settler_pending;

	BuildQueue();
	~BuildQueue();

	void SendMsgWonderComplete(CityData *cd, sint32 wonder);

	void EndTurn();

	void Clear(BOOL fromServer = FALSE);
	void ClearAllButHead(BOOL fromServer = FALSE);

	void SetOwner(PLAYER_INDEX o) { m_owner = o; }
	void SetCity(Unit &city);

	bool BuildFrontUnit(BOOL forceFinish);
	bool BuildFrontBuilding();
	bool BuildFrontWonder();
	bool BuildFrontEndgame();
	bool BuildFront(sint32 &shieldstore, CityData *cd, const MapPoint &pos, uint64 &built_improvements, uint64 &built_wonders, BOOL forceFinish);

	sint32 Load(const MBCHAR *file);
	sint32 Save(const MBCHAR *file);

	void RawInsertTail(sint32 cat, sint32 t, sint32 cost);
	BOOL InsertTail(sint32 cat, sint32 t, sint32 cost);
	void ReplaceHead(sint32 cat, sint32 item_type, sint32 cost);
	void Serialize(CivArchive &archive);
	void SerializeQueue(CivArchive &archive);

	
	BuildNode *GetHead(void);
	PointerList<BuildNode> *GetList() const { return m_list; }

	void ResetOwner(sint32 new_owner);
	void Dump(const sint32 shieldstore, MBCHAR *s) ;
	void RemoveHead();
	
	
	sint32 RemoveNode(BuildNode *node, CAUSE_REMOVE_BUILD_ITEM cause);
	sint32 RemoveNodeByIndex(sint32 index, CAUSE_REMOVE_BUILD_ITEM cause);
	sint32 InsertAfter(BuildNode *targetNode, BuildNode *node);
	sint32 InsertIndex(sint32 index, BuildNode *node);
	MBCHAR *GetName( void ) { return m_name; }
	void SetName( MBCHAR *name );

	double GetTypeCoeff() const;
	double GetPercentRemaining(sint32 shieldstore) const;
	sint32 GetProductionRemaining(sint32 shieldstore) const;
	sint32 GetPercentCompleted(sint32 shieldstore) const;
	sint32 GetFrontCost();

	void RemoveObjectsOfType(sint32 cat, sint32 type, 
	                         CAUSE_REMOVE_BUILD_ITEM cause);
	void RemoveIllegalItems(BOOL isClientAck = FALSE);

	sint32 GetLen() const;
	BOOL DoInsertChecks(sint32 cat, sint32 t, sint32 cost);
	sint32 GetCost(sint32 cat, sint32 t);
	BOOL InsertBefore(BuildNode *old, sint32 cat, sint32 t);

	BuildNode *GetNodeByIndex(sint32 index);

	void FinishCreatingUnit(Unit &u);
	void FinishBuildFront(Unit &u);

	void MoveNodeUp(sint32 index);
	void MoveNodeDown(sint32 index);

	bool IsItemInQueue(uint32 cat, sint32 type);

	BuildQueue & operator= (const BuildQueue &copy);
};


uint32 BldQue_BuildQueue_GetVersion(void) ;

#endif


