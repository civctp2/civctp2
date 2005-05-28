

#include "c3.h"
#include "network.h"
#include "net_crc.h"
#include "net_util.h"
#include "civarchive.h"
#include "Checksum.h"

#include "ConstDB.h"
#include "BuildingRecord.h"
#include "SpriteStateDB.h"
#include "DiffDB.h"
#include "AdvanceRecord.h"
#include "TerrainRecord.h"
#include "gwdb.h"
#include "UVDB.h"
#include "WonderRecord.h"
#include "CivilisationDB.h"
#include "profileDB.h"
#include "StrDB.h"
#include "GovernmentRecord.h"
#include "PollutionDB.h"
#include "civapp.h"
#include "c3_utilitydialogbox.h"
#include "UnitRecord.h"

extern	StringDB	*g_theStringDB ;
extern	ConstDB					*g_theConstDB ;
extern	SpriteStateDB			*g_theSpriteStateDB ;
extern	SpriteStateDB			*g_theSpecialEffectDB;




extern	SpriteStateDB			*g_theGoodsSpriteStateDB;
extern	DifficultyDB			*g_theDifficultyDB ;
extern	PollutionDatabase		*g_thePollutionDB ;
extern	GlobalWarmingDatabase	*g_theGWDB ;
extern	OzoneDatabase			*g_theUVDB ;
extern	CivilisationDatabase	*g_theCivilisationDB ;

extern	ProfileDB				*g_theProfileDB;
extern StringDB                 *g_theStringDB;
extern CivApp                   *g_civApp;

NetCRC::NetCRC(sint32 startat, sint32 stopat)
{
	m_startAt = startat;
	m_stopAt = stopat;

	
}

NetCRC::NetCRC()
{
}

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

sint32 NetCRC::SerializeDBs()
{
	CivArchive *archive;
	CheckSum *check;
	sint32 dbnum = 0;
	sint32 numchecked = 0;

	
	
	
	
	
	CHECKDB(g_theConstDB);
	CHECKDB(g_theDifficultyDB);
	CHECKDB(g_thePollutionDB);
	
	

	
	
	
	

	
	

	return numchecked;
}

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
