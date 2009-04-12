//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Build queue handling
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
// CTP1_HAS_RISEN_FROM_THE_GRAVE
// Activate CTP1 wormhole probe production reporting
//
// HAVE_PRAGMA_ONCE
// Compiler supports #pragma once
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added assignment operator. - Jul 16th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __BUILD_QUEUE_H__
#define __BUILD_QUEUE_H__ 1

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// #include <>      

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class   BuildQueue;
struct  BuildNode; 

enum    CAUSE_REMOVE_BUILD_ITEM 
{
	CAUSE_REMOVE_BUILD_ITEM_MANUAL,
	CAUSE_REMOVE_BUILD_ITEM_ILLEGAL, 
	CAUSE_REMOVE_BUILD_ITEM_NETWORK,
	CAUSE_REMOVE_BUILD_ITEM_BUILT,
};

#define k_BUILD_NODE_FLAG_ALREADY_BUILT 0x01
#define k_BUILD_NODE_FLAG_ALMOST_DONE   0x02

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

class CivArchive;
class MapPoint;
class CityData; 
class UnitRecord;
template <class T> class PointerList;

#include "player.h" // PLAYER_INDEX
#include "Unit.h"   // Unit

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

struct BuildNode 
{ 
	sint32  m_cost; 
	sint32  m_type; 
	sint32  m_category; 
	uint8   m_flags; 
};

class BuildQueue 
{ 
private:
	PointerList<BuildNode> *    m_list;
	
//----------------------------------------------------------------------------
// Do not change anything in the types or order of the following variable 
// declarations. Doing so will break reading in of save files.
// See the Serialize implementation for more details.
//----------------------------------------------------------------------------

	PLAYER_INDEX                m_owner;
	Unit                        m_city;
	sint32                      m_wonderStarted;
	sint32                      m_wonderStopped;
	MBCHAR                      m_name[256];

//----------------------------------------------------------------------------
// Changing the order below this line should not break anything.
//----------------------------------------------------------------------------

	sint32                      m_wonderComplete;
	BuildNode *                 m_frontWhenBuilt;

	void HandleProductionComplete(void);
	void HandleProductionStart(void);
	void HandleProductionStop(void);
#if defined(CTP1_HAS_RISEN_FROM_THE_GRAVE)
	void SendMsgWormholeProbeStarted(void);
	void SendMsgWormholeProbeComplete(void);
#endif
	void SendMsgWonderStarted(sint32 type);
	void SendMsgWonderCompleteOwner(CityData *cd, sint32 wonder);
	void SendMsgWonderCompleteEveryone(CityData *cd, sint32 wonder);
	void SendMsgWonderAlmostComplete(void);
	void SendMsgWonderStopped(sint32 type);

	friend class NetCityBuildQueue;

public:

	bool m_settler_pending;
	bool m_popcoststobuild_pending;   //EMOD

	BuildQueue();
	~BuildQueue();

	void SendMsgWonderComplete(CityData *cd, sint32 wonder);

	void EndTurn();

	void Clear(bool fromServer = false) ;
	void ClearAllButHead(bool fromServer = false);

	void SetOwner(PLAYER_INDEX o) { m_owner = o; }
	void SetCity(Unit &city);

	bool BuildFrontUnit(bool forceFinish);
	bool BuildFrontBuilding();
	bool BuildFrontWonder();
	bool BuildFrontEndgame();
	bool BuildFront(sint32 &shieldstore, CityData *cd, const MapPoint &pos, uint64 &built_improvements, uint64 &built_wonders, bool forceFinish);

	sint32 Load(const MBCHAR *file);
	sint32 Save(const MBCHAR *file);

	void RawInsertTail(sint32 cat, sint32 t, sint32 cost);
	bool InsertTail(sint32 cat, sint32 t, sint32 cost);
	void ReplaceHead(sint32 cat, sint32 item_type, sint32 cost);
	void Serialize(CivArchive &archive) ;
	
	BuildNode *GetHead(void);
	PointerList<BuildNode> *GetList() const { return m_list; }

	void ResetOwner(sint32 new_owner);
	void Dump(const sint32 shieldstore, MBCHAR *s);
	void RemoveHead();
	
	
	bool RemoveNode(BuildNode *node, CAUSE_REMOVE_BUILD_ITEM cause);
	bool RemoveNodeByIndex(sint32 index, CAUSE_REMOVE_BUILD_ITEM cause);
	bool InsertAfter(BuildNode *targetNode, BuildNode *node);
	bool InsertIndex(sint32 index, BuildNode *node);
	MBCHAR *GetName( void ) { return m_name; }
	void SetName( MBCHAR *name );

	double GetTypeCoeff() const;
	double GetPercentRemaining(sint32 shieldstore) const;
	sint32 GetProductionRemaining(sint32 shieldstore) const;
	sint32 GetPercentCompleted(sint32 shieldstore) const;
	sint32 GetFrontCost();

	void RemoveObjectsOfType(sint32 cat, sint32 type, 
							 CAUSE_REMOVE_BUILD_ITEM cause);
	void RemoveIllegalItems(bool isClientAck = false);

	sint32 GetLen() const;
	bool DoInsertChecks(sint32 cat, sint32 t, sint32 cost);
	sint32 GetCost(sint32 cat, sint32 t);
	bool InsertBefore(BuildNode *old, sint32 cat, sint32 t);

	BuildNode *GetNodeByIndex(sint32 index);

	void FinishCreatingUnit(Unit &u);
	void FinishBuildFront(Unit &u);

	void MoveNodeUp(sint32 index);
	void MoveNodeDown(sint32 index);

	bool IsItemInQueue(uint32 cat, sint32 type);

	BuildQueue & operator= (const BuildQueue &copy);

private:
	void SynchroniseNetworkData(void) const;
};


uint32 BldQue_BuildQueue_GetVersion(void) ;

#endif


