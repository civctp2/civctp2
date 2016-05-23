#include "c3.h"
#include "net_keys.h"
#include "net_util.h"

#include "UnitPool.h"
#include "TradePool.h"
#include "TradeOfferPool.h"
#include "TerrImprovePool.h"
#include "installationpool.h"
#include "CivilisationPool.h"
#include "DiplomaticRequestPool.h"
#include "MessagePool.h"
#include "AICause.h"
#include "ArmyPool.h"
#include "AgreementPool.h"

void NetKeys::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_KEYS_ID);
	PUSHLONG(g_theUnitPool->HackGetKey());
	PUSHLONG(g_theTradePool->HackGetKey());
	PUSHLONG(g_theTradeOfferPool->HackGetKey());
	PUSHLONG(g_theTerrainImprovementPool->HackGetKey());
	PUSHLONG(g_theInstallationPool->HackGetKey());
	PUSHLONG(g_theCivilisationPool->HackGetKey());
	PUSHLONG(g_theDiplomaticRequestPool->HackGetKey());
	PUSHLONG(g_theMessagePool->HackGetKey());
	PUSHLONG(g_theArmyPool->HackGetKey());
	PUSHLONG(g_theAgreementPool->HackGetKey());
}

void NetKeys::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_KEYS_ID);

	uint32 key;
	PULLLONG(key); g_theUnitPool->HackSetKey(key);
	PULLLONG(key); g_theTradePool->HackSetKey(key);
	PULLLONG(key); g_theTradeOfferPool->HackSetKey(key);
	PULLLONG(key); g_theTerrainImprovementPool->HackSetKey(key);
	PULLLONG(key); g_theInstallationPool->HackSetKey(key);
	PULLLONG(key); g_theCivilisationPool->HackSetKey(key);
	PULLLONG(key); g_theDiplomaticRequestPool->HackSetKey(key);
	PULLLONG(key); g_theMessagePool->HackSetKey(key);
	PULLLONG(key); g_theArmyPool->HackSetKey(key);
	PULLLONG(key); g_theAgreementPool->HackSetKey(key);

	Assert(pos == size);
}
