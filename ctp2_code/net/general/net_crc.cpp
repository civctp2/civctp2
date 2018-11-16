//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Checks weather the databases match in MP.
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
// - Replaced old civilisation database by new one. (Aug 20th 2005 Martin Gühmann)
// - Fixed SerializeDBs method to fix the database in saync check. (Aug 25th 2005 Martin Gühmann)
// - Added the risk database for sync check. (Aug 29th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "network.h"
#include "net_crc.h"
#include "net_util.h"
#include "civarchive.h"
#include "Checksum.h"

// Database includes
#include "StrDB.h"
#include "profileDB.h"

#include "AdvanceRecord.h"
#include "AdvanceBranchRecord.h"
#include "AdvanceListRecord.h"
#include "AgeRecord.h"
#include "AgeCityStyleRecord.h"
#include "BuildingRecord.h"
#include "BuildingBuildListRecord.h"
#include "BuildListSequenceRecord.h"
#include "CitySizeRecord.h"
#include "CityStyleRecord.h"
#include "CivilisationRecord.h"
#include "ConstDB.h"                   // Old database
#include "DiffDB.h"                    // Old database
#include "DiplomacyRecord.h"
#include "DiplomacyProposalRecord.h"
#include "DiplomacyThreatRecord.h"
#include "EndGameObjectRecord.h"
#include "FeatRecord.h"
#include "gwdb.h"                      // Global warming database
#include "GoalRecord.h"
#include "GovernmentRecord.h"
#include "IconRecord.h"
#include "ImprovementListRecord.h"
#include "OrderRecord.h"
#include "UVDB.h"                      // Ozone database
#include "PersonalityRecord.h"
#include "PollutionDB.h"               // Old database
#include "PopRecord.h"
#include "ResourceRecord.h"
#include "RiskRecord.h"
#include "SoundRecord.h"
#include "SpecialAttackInfoRecord.h"
#include "SpecialEffectRecord.h"
#include "TerrainRecord.h"
#include "SpriteRecord.h"
#include "StrategyRecord.h"
#include "TerrainRecord.h"
#include "TerrainImprovementRecord.h"
#include "UnitRecord.h"
#include "UnitBuildListRecord.h"
#include "WonderRecord.h"
#include "WonderBuildListRecord.h"

#include "civapp.h"
#include "c3_utilitydialogbox.h"

extern StringDB                *g_theStringDB;
extern ProfileDB               *g_theProfileDB;

extern ConstDB                 *g_theConstDB;
extern DifficultyDB            *g_theDifficultyDB;
extern GlobalWarmingDatabase   *g_theGWDB;
extern OzoneDatabase           *g_theUVDB;
extern PollutionDatabase       *g_thePollutionDB;

extern CivApp                  *g_civApp;

//----------------------------------------------------------------------------
//
// Name       : NetFeatTracker::NetFeatTracker
//
// Description: Constructor
//
// Parameters : sint32 startat: First database to check
//              sint32 stopat:  Last database to check
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : No idea why there should be just a subset of the databases
//              to check actual they should be checked all
//
//----------------------------------------------------------------------------
NetCRC::NetCRC(sint32 startat, sint32 stopat)
:	Packetizer()
{
	m_startAt = startat;
	m_stopAt = stopat;
}

//----------------------------------------------------------------------------
//
// Name       : NetFeatTracker::NetFeatTracker
//
// Description: Constructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
NetCRC::NetCRC()
:	Packetizer()
{
	m_startAt = 0;
	m_stopAt = k_MAX_DBS - 1;
}

//----------------------------------------------------------------------------
//
// Name       : CHECKDB(db)
//
// Description: Macro for databse checking
//
// Parameters : db: The databse to check
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Database must be serializable
//
//----------------------------------------------------------------------------
#define CHECKDB(db) \
	if(dbnum < m_startAt || dbnum > m_stopAt) { \
		dbnum++; \
	} else { \
		Assert(dbnum < k_MAX_DBS); \
		archive = new CivArchive; \
		check = new CheckSum; \
		archive->SetStore(); \
		db->Serialize(*archive); \
		check->AddData(archive->GetStream(), archive->StreamLen()); \
		check->Done(m_db_crc[dbnum][0], m_db_crc[dbnum][1], m_db_crc[dbnum][2], m_db_crc[dbnum][3]); \
		delete archive; \
		delete check; \
		dbnum++; \
		numchecked++; \
	}

//----------------------------------------------------------------------------
//
// Name       : NetCRC::SerializeDBs
//
// Description: Serializes the databases and creates check sums from the
//              archieves, so that the databases can be checked for synchronicity.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 NetCRC::SerializeDBs()
{
	CivArchive *archive;
	CheckSum *check;
	sint32 dbnum = 0;
	sint32 numchecked = 0;

	CHECKDB(g_theAdvanceDB);           //  0
	CHECKDB(g_theAdvanceBranchDB);     //  1
	CHECKDB(g_theAdvanceListDB);       //  2
	CHECKDB(g_theAgeDB);               //  3
	CHECKDB(g_theAgeCityStyleDB);      //  4
	CHECKDB(g_theBuildListSequenceDB); //  5
	CHECKDB(g_theBuildingDB);          //  6
	CHECKDB(g_theBuildingBuildListDB); //  7
	CHECKDB(g_theCitySizeDB);          //  8
	CHECKDB(g_theCityStyleDB);         //  9
	CHECKDB(g_theCivilisationDB);      // 10
	CHECKDB(g_theConstDB);             // 11
	CHECKDB(g_theDifficultyDB);        // 12
	CHECKDB(g_theDiplomacyDB);         // 13
	CHECKDB(g_theDiplomacyProposalDB); // 14
	CHECKDB(g_theDiplomacyThreatDB);   // 15
	CHECKDB(g_theEndGameObjectDB);     // 16
	CHECKDB(g_theFeatDB);              // 17
	CHECKDB(g_theGWDB);                // 18
	CHECKDB(g_theGoalDB);              // 19
	CHECKDB(g_theGovernmentDB);        // 20
	CHECKDB(g_theIconDB);              // 21
	CHECKDB(g_theImprovementListDB);   // 22
	CHECKDB(g_theOrderDB);             // 23
	CHECKDB(g_theUVDB);                // 24
	CHECKDB(g_thePersonalityDB);       // 25
	CHECKDB(g_thePollutionDB);         // 26
	CHECKDB(g_thePopDB);               // 27
	CHECKDB(g_theResourceDB);          // 28
	CHECKDB(g_theRiskDB);              // 29
	CHECKDB(g_theSoundDB);             // 30
	CHECKDB(g_theSpecialAttackInfoDB); // 31
	CHECKDB(g_theSpecialEffectDB);     // 32
	CHECKDB(g_theSpriteDB);            // 33
	CHECKDB(g_theStrategyDB);          // 34
	CHECKDB(g_theTerrainDB);           // 35
	CHECKDB(g_theUnitDB);              // 36
	CHECKDB(g_theUnitBuildListDB);     // 37
	CHECKDB(g_theWonderDB);            // 38
	CHECKDB(g_theWonderBuildListDB);   // 39

	return numchecked;
}

//----------------------------------------------------------------------------
//
// Name       : NetFeatTracker::Packetize
//
// Description: Generate an application data packet to transmit.
//
// Parameters : buf         : buffer to store the message
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void NetCRC::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_CRC_ID);

	sint32 num = SerializeDBs();

	PUSHLONG(m_startAt);
	PUSHLONG(m_stopAt);
	PUSHLONG(num);
	for(sint32 j = 0; j < num; j++) {
		for(sint32 i = 0; i < 4; i++) {
			PUSHLONG(m_db_crc[j+m_startAt][i]);
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : NetFeatTracker::Unpacketize
//
// Description: Retrieve the data from a received application data packet.
//
// Parameters : id          : Sender identification?
//              buf         : Buffer with received message
//              size        : Length of received message (in bytes)
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void NetCRC::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 packid;
	uint16 pos = 0;

	PULLID(packid);
	Assert(packid == k_PACKET_CRC_ID);

	PULLLONG(m_startAt);
	PULLLONG(m_stopAt);
	sint32 num = SerializeDBs();
	sint32 remoteNum;
	PULLLONG(remoteNum);

	if(num != remoteNum) {
		// No idea what this should do, this can only happen if the
		// remote executable is compiled from a different version
		// of the source code.
		Error("Number of databases doesn't even match, sheesh!");
		return;
	}

	sint32 i,j;
	uint32 part;
	BOOL alreadybad;

	for(j = 0; j < num; j++) {
		alreadybad = FALSE;
		for(i = 0; i < 4; i++) {
			PULLLONG(part);
			if(!alreadybad && (part != m_db_crc[j+m_startAt][i])) {
				char buf[2048];
				sprintf(buf, "Database #%d is out of synch", j + m_startAt);
				Error(buf);
				alreadybad = TRUE;

			}
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : NetFeatTracker::Packetize
//
// Description: Generate an application data packet to transmit.
//
// Parameters : buf         : buffer to store the message
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void NetCRC::Error(char *buf)
{
	extern void network_AbortCallback( sint32 type );

	DPRINTF(k_DBG_NET, ("NetCRC: %s\n", buf));
	const char *str = g_theStringDB->GetNameStr("str_ldl_mp_dbase_out_of_synch");
	char nonConstStr[1024];
	if (str) {
		strcpy(nonConstStr, str);
	} else {
		strcpy(nonConstStr, "Databases out of sync, returning to lobby");
	}
	c3_RemoveAbortMessage();
	g_civApp->ProcessGraphicsCallback();
	c3_AbortMessage(nonConstStr, k_UTILITY_ABORT, network_AbortCallback );
	g_network.SetCRCError();
}
