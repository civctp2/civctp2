//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Multiplayer packet handling
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
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines, and the blocks
//       that are inactive for _MSC_VER value 1200 are modified Apolyton code. 
//       The blocks that are active for _MSC_VER value 1200 are the original 
//       Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Patch 1.1 reimplementation.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef _NET_PACKET_H_
#define _NET_PACKET_H_

#define MAKE_CIV3_ID(x, y) (((x) << 8) | (y))
#define k_PACKET_CELL_ID       MAKE_CIV3_ID('G', 'C')
#define k_PACKET_CELL_LIST_ID  MAKE_CIV3_ID('G', 'L')
#define k_PACKET_UNIT_ID       MAKE_CIV3_ID('U', 'D')
#define k_PACKET_ACTION_ID     MAKE_CIV3_ID('A', 'A')
#define k_PACKET_INFO_ID       MAKE_CIV3_ID('I', 'I')
#define k_PACKET_CITY_ID       MAKE_CIV3_ID('C', 'D')
#define k_PACKET_POP_ID        MAKE_CIV3_ID('G', 'P')
#define k_PACKET_DIFFICULTY_ID MAKE_CIV3_ID('G', 'D')
#define k_PACKET_PLAYER_ID     MAKE_CIV3_ID('G', 'p')
#define k_PACKET_TRADE_ROUTE_ID MAKE_CIV3_ID('G', 'R')
#define k_PACKET_TRADE_OFFER_ID MAKE_CIV3_ID('G', 'O')
#define k_PACKET_RAND_ID       MAKE_CIV3_ID('G', 'r')
#define k_PACKET_TERRAIN_ID    MAKE_CIV3_ID('G', 'T')
#define k_PACKET_INSTALLATION_ID MAKE_CIV3_ID('G', 'I')
#define k_PACKET_CHAT_ID         MAKE_CIV3_ID('N', 'C')
#define k_PACKET_READINESS_ID    MAKE_CIV3_ID('G', 'M')
#define k_PACKET_HAPPY_ID        MAKE_CIV3_ID('G', 'H')
#define k_PACKET_PLAYER_HAPPY_ID MAKE_CIV3_ID('G', 'h')
#define k_PACKET_REPORT_ID       MAKE_CIV3_ID('N', 'R')
#define k_PACKET_UNIT_MOVE_ID    MAKE_CIV3_ID('U', 'M')
#define k_PACKET_UNIT_ORDER_ID   MAKE_CIV3_ID('U', 'O')
#define k_PACKET_AGREEMENT_ID    MAKE_CIV3_ID('G', 'A')
#define k_PACKET_CLIENT_AGREEMENT_ID MAKE_CIV3_ID('C', 'A')
#define k_PACKET_CIVILIZATION_ID     MAKE_CIV3_ID('G', 'c')
#define k_PACKET_CITY_NAME_ID        MAKE_CIV3_ID('C', 'N')
#define k_PACKET_DIP_PROPOSAL_ID      MAKE_CIV3_ID('D', 'P')
#define k_PACKET_DIP_RESPONSE_ID      MAKE_CIV3_ID('D', 'R')
#define k_PACKET_MESSAGE_ID          MAKE_CIV3_ID('M', 'M')
#define k_PACKET_CITY2_ID            MAKE_CIV3_ID('C', '2')
#define k_PACKET_POLLUTION_ID        MAKE_CIV3_ID('P', 'O')
#define k_PACKET_CITY_BQ_ID          MAKE_CIV3_ID('C', 'B')
#define k_PACKET_KEYS_ID             MAKE_CIV3_ID('G', 'K')
#define k_PACKET_GAME_SETTINGS_ID    MAKE_CIV3_ID('G', 'S')
#define k_PACKET_NEW_ARMY_ID         MAKE_CIV3_ID('A', 'N')
#define k_PACKET_REMOVE_ARMY_ID      MAKE_CIV3_ID('A', 'R')
#define k_PACKET_CRC_ID              MAKE_CIV3_ID('C', 'R')
#define k_PACKET_ARMY_ID             MAKE_CIV3_ID('G', 'a')
#define k_PACKET_ACHIEVEMENT_TRACKER_ID   MAKE_CIV3_ID('A', 'T')
#define k_PACKET_WONDER_TRACKER_ID   MAKE_CIV3_ID('W', 'T')
#define k_PACKET_VISION_ID           MAKE_CIV3_ID('V', 'V')
#define k_PACKET_UNSEEN_CELL_ID      MAKE_CIV3_ID('V', 'U')
#define k_PACKET_EXCLUSIONS_ID       MAKE_CIV3_ID('G', 'E')
#define k_PACKET_RESOURCES_ID        MAKE_CIV3_ID('C', 'r')
#define k_PACKET_UNIT_HP_ID          MAKE_CIV3_ID('U', 'H')
#define k_PACKET_CELL_UNIT_ORDER_ID  MAKE_CIV3_ID('C', 'U')
#define k_PACKET_ADD_PLAYER_ID       MAKE_CIV3_ID('A', 'P')
#define k_PACKET_RESEARCH_ID         MAKE_CIV3_ID('G', 'd')
#define k_PACKET_GUID_ID             MAKE_CIV3_ID('N', 'G')
#define k_PACKET_STRENGTH_ID         MAKE_CIV3_ID('G', 's')
#define k_PACKET_FULL_STRENGTHS_ID   MAKE_CIV3_ID('F', 's')
#define k_PACKET_NET_INFO_MESSAGE_ID MAKE_CIV3_ID('N', 'M')
#define k_PACKET_ENDGAME_ID          MAKE_CIV3_ID('G', 'e')
#define k_PACKET_WORMHOLE_ID         MAKE_CIV3_ID('G', 'w')
#define k_PACKET_SET_PLAYER_GUID_ID  MAKE_CIV3_ID('N', 'g')
#define k_PACKET_SET_LEADER_NAME_ID  MAKE_CIV3_ID('G', 'l')
#define k_PACKET_WORLD_ID            MAKE_CIV3_ID('G', 'W')
#define k_PACKET_DIP_AGREEMENT_MATRIX_ID MAKE_CIV3_ID('A', 'M')
#define k_PACKET_GROUP_REQUEST_ID    MAKE_CIV3_ID('A', 'G')
#define k_PACKET_UNGROUP_REQUEST_ID  MAKE_CIV3_ID('A', 'U')
#define k_PACKET_SCORES_ID            MAKE_CIV3_ID('S', 'C')

#define k_PACKET_FEAT_TRACKER_ID			MAKE_CIV3_ID('F', 'T')

#ifdef _PLAYTEST
#define k_PACKET_CHEAT_ID        MAKE_CIV3_ID('C', 'H')
#endif

class Packetizer {
public:
	Packetizer() 
	{ 
		m_reliability = 1;
		m_refCount = 0;
		m_unitId = 0;
		m_packetbuf = NULL;
		m_packetsize = 0;
		m_sendCompressed = FALSE;
	}

	Packetizer(uint8 *rawBuf, sint32 len)
	{
		m_reliability = 1;
		m_refCount = 0;
		m_unitId = 0;
		m_packetbuf = rawBuf;
		m_packetsize = len;
		m_sendCompressed = TRUE;
	}

	virtual ~Packetizer() 
	{
		if(m_packetbuf)
			delete [] m_packetbuf;
	}

	sint32 AddRef() { return ++m_refCount; }
	sint32 Release() 
	{
		--m_refCount;
		if(m_refCount <= 0) {
			delete this;
			return 0;
		}
		return m_refCount;
	}

	virtual void Packetize(uint8* buf, uint16& size)
	{
#pragma warning( disable : 4127)									
		Assert(FALSE);
#pragma warning( default : 4127)									
		buf[0] = 'G';
		buf[1] = 'N';
		size = 2;
		m_reliability = 1;
	};

	virtual void Unpacketize(uint16 id, uint8* buf, uint16 size)
	{
		Assert(buf[0] == 'G' && buf[1] == 'N');
#pragma warning( disable : 4127)									
		Assert(FALSE);
#pragma warning( default : 4127)									
		Assert(id) ;												
		Assert(size) ;												
	}

	void PacketizeAndSave()
	{
 		if(!m_packetbuf) {
			uint8 buf[8192];
			uint16 size = 0;
			Packetize(buf, size);
			m_packetsize = size;
#ifdef LOG_NETWORK_OUTPUT			
			FILE *mtfile = fopen("netmainthread.log", "a");
			if(mtfile) {
				sint32 i;
				for(i = 0; i < m_packetsize; i++) {
					fprintf(mtfile, "%02lx ", buf[i]);
				}
				fprintf(mtfile, "\n");
				fclose(mtfile);
			}
#endif
				
			Assert(m_packetsize < 8192);
			m_packetbuf = new uint8[m_packetsize];
			memcpy(m_packetbuf, buf, m_packetsize);
		}
	}

	BOOL ShouldSendCompressed() { return m_sendCompressed; }

	sint32 m_destination;
	sint32 m_reliability;
	sint32 m_unitId; 
                     
 	                 
	uint8 *m_packetbuf;
	sint32 m_packetsize;
	sint32 m_sendCompressed;

private:
	sint32 m_refCount;
};

#else
class Packetizer;
#endif
