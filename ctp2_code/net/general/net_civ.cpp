//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer civ packet handling.
// Id           : $Id net_civ.cpp 442 2005-08-28 18:05:04Z Martin GÃ¼hmann $
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
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "net_civ.h"
#include "network.h"
#include "net_util.h"

#include "CivilisationPool.h"
#include "CivilisationRecord.h"

#include "player.h"

#include "c3_utilitydialogbox.h"

extern Player **g_player;
extern c3_UtilityPlayerListPopup *g_networkPlayersScreen;

NetCivilization::NetCivilization(CivilisationData *data)
{
	m_data = data;
}

void NetCivilization::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_CIVILIZATION_ID);

	PUSHLONG(m_data->m_id);
	PUSHBYTE((uint8)m_data->m_owner);
	PUSHBYTE((uint8)m_data->m_gender);
	PUSHSHORT((uint16)m_data->m_civ);
	PUSHSTRING(m_data->m_leader_name);
	PUSHSTRING(m_data->m_civilisation_name);
	PUSHSTRING(m_data->m_country_name);
	PUSHSTRING(m_data->m_singular_name);
	PUSHSTRING(m_data->m_personality_description);
	
	uint16 numCityNames = (uint16)g_theCivilisationDB->Get(m_data->m_civ)->GetNumCityName();
	PUSHSHORT(numCityNames);
	sint32 i;
	for(i = 0; i < numCityNames; i++) {
		PUSHBYTE(m_data->m_cityname_count[i]);
	}
}

void NetCivilization::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	uint16 tmp ;
	PULLID(packid);
	Assert(packid == k_PACKET_CIVILIZATION_ID);
	Civilisation cid;

	PULLLONGTYPE(cid, Civilisation);
	
	g_network.CheckReceivedObject((uint32)cid);

	if(!g_theCivilisationPool)
		return;

	if(!g_theCivilisationPool->IsValid(cid)) {
		m_data = new CivilisationData(cid);
	} else {
		m_data = g_theCivilisationPool->AccessData(cid);
	}
	
	m_data->m_id = (uint32)cid;
	PULLBYTETYPE(m_data->m_owner, PLAYER_INDEX);
	PULLBYTETYPE(m_data->m_gender, GENDER);
	PULLSHORT(tmp) ;
	m_data->m_civ = static_cast<sint32>(tmp);
	PULLSTRING(m_data->m_leader_name);
	PULLSTRING(m_data->m_civilisation_name);
	PULLSTRING(m_data->m_country_name);
	PULLSTRING(m_data->m_singular_name);
	PULLSTRING(m_data->m_personality_description);

	uint16 numCityNames;	
	PULLSHORT(numCityNames);
	sint32 i;
	for(i = 0; i < numCityNames; i++) {
		PULLBYTE(m_data->m_cityname_count[i]);
	}

	if(!g_theCivilisationPool->IsValid(cid)) {
		g_theCivilisationPool->HackSetKey(((uint32)cid + k_ID_KEY_MASK) + 1);
		g_theCivilisationPool->Insert(m_data);
		*g_player[m_data->m_owner]->m_civilisation = cid;
	}
}

void NetSetLeaderName::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_SET_LEADER_NAME_ID);
	
	PUSHBYTE(m_player);
	PUSHSTRING(g_player[m_player]->m_civilisation->GetLeaderName());
}

void NetSetLeaderName::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_SET_LEADER_NAME_ID);
	
	MBCHAR nbuf[1024];
	PULLBYTE(m_player);
	PULLSTRING(nbuf);
	if(g_player[m_player]) {
		g_player[m_player]->m_civilisation->AccessData()->SetLeaderName(nbuf);

		if(g_networkPlayersScreen) {
			g_networkPlayersScreen->UpdateData();
		}
	}
}



