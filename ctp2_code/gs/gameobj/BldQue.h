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
// - m_name is now a pointer, so that we do not use up so much memory. (13-Jan-2019 Martin Gühmann)
// - Ordered the members by size. (13-Jan-2019 Martin Gühmann)
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
//----------------------------------------------------------------------------
// Sort the member variables by size to save memory
// Before sorting the size was 36 bytes
// After sorting 36 bytes
//----------------------------------------------------------------------------

	// The pointers 4 bytes in a 32 bit and 8 bytes in a 64 bit program
	PointerList<BuildNode> *    m_list;
	MBCHAR *                    m_name;            // Probably unused
	BuildNode *                 m_frontWhenBuilt;

	// 4 bytes
	PLAYER_INDEX                m_owner;
	Unit                        m_city;
	sint32                      m_wonderStarted;
	sint32                      m_wonderStopped;
	sint32                      m_wonderComplete;

	// 1 byte
	bool                        m_settler_pending;
	bool                        m_popcoststobuild_pending;   //EMOD

//----------------------------------------------------------------------------
// End member variable section
//----------------------------------------------------------------------------

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

	BuildQueue();
	//Doesn't seem to copy m_list properly
	//BuildQueue(BuildQueue & copy);
	~BuildQueue();

	bool IsSettlerPending() const { return m_settler_pending; }
	bool IsPopCostToBuildPending() const { return m_popcoststobuild_pending; }   //EMOD

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

	BuildNode *GetHead(void) const;
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

	bool IsHeadUnit() const;
	sint32 GetHeadType() const;

private:
	void SynchroniseNetworkData(void) const;
#if 0
	void PrintSizeOfClass()
	{
		DPRINTF(k_DBG_AI, ("\n"));
		DPRINTF(k_DBG_AI, ("Size of BuildQueue class:\n"));
		DPRINTF(k_DBG_AI, ("BuildQueue: %d\n", sizeof(BuildQueue)));
		DPRINTF(k_DBG_AI, ("m_list: %d\n", sizeof(m_list)));
		DPRINTF(k_DBG_AI, ("m_owner: %d\n", sizeof(m_owner)));
		DPRINTF(k_DBG_AI, ("m_city: %d\n", sizeof(m_city)));
		DPRINTF(k_DBG_AI, ("m_wonderStarted: %d\n", sizeof(m_wonderStarted)));
		DPRINTF(k_DBG_AI, ("m_wonderStopped: %d\n", sizeof(m_wonderStopped)));
		DPRINTF(k_DBG_AI, ("m_name: %d\n", sizeof(m_name)));
		DPRINTF(k_DBG_AI, ("m_wonderComplete: %d\n", sizeof(m_wonderComplete)));
		DPRINTF(k_DBG_AI, ("m_frontWhenBuilt: %d\n", sizeof(m_frontWhenBuilt)));
		DPRINTF(k_DBG_AI, ("\n"));
	}

	void PrintData()
	{
		DPRINTF(k_DBG_AI, ("\n"));
		DPRINTF(k_DBG_AI, ("Data of BuildQueue class:\n"));
		DPRINTF(k_DBG_AI, ("m_list: %x\n", m_list));
		DPRINTF(k_DBG_AI, ("m_owner: %d\n", m_owner));
		DPRINTF(k_DBG_AI, ("m_city: %d\n", m_city));
		DPRINTF(k_DBG_AI, ("m_wonderStarted: %d\n", m_wonderStarted));
		DPRINTF(k_DBG_AI, ("m_wonderStopped: %d\n", m_wonderStopped));
		DPRINTF(k_DBG_AI, ("m_name: %s\n", m_name));
		DPRINTF(k_DBG_AI, ("m_wonderComplete: %x\n", m_wonderComplete));
		DPRINTF(k_DBG_AI, ("m_frontWhenBuilt: %x\n", m_frontWhenBuilt));
		DPRINTF(k_DBG_AI, ("\n"));
	}
#endif
};

uint32 BldQue_BuildQueue_GetVersion(void);

#endif
