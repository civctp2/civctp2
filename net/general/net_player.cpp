









#include "c3.h"

#include "net_player.h"
#include "net_util.h"

#include "Player.h"
#include "Advances.h"
#include "BSet.h"
#include "MaterialPool.h"
#include "TaxRate.h"
#include "DynArr.h"
#include "BSet.h"
#include "network.h"
#include "Unit.h"
#include "Score.h"

#include "MainControlPanel.h"
#include "ControlPanelWindow.h"

extern Player** g_player;

NetPlayer::NetPlayer(Player* player)
{
	m_player = player;
}

void NetPlayer::Packetize(uint8* buf, uint16& size)
{
	size = 0;
	PUSHID(k_PACKET_PLAYER_ID);
	PUSHLONG(m_player->m_owner);
	if(g_network.GetPlayerIndex() == m_player->m_owner) {
		PUSHLONG(PLAYER_TYPE_HUMAN);
	} else {
		PUSHLONG(m_player->m_playerType);
	}
	PUSHLONG(m_player->mask_alliance);
	PUSHLONG(m_player->m_government_type);
	PUSHLONG(m_player->m_set_government_type);
	PUSHLONG(m_player->m_change_government_turn);
	PUSHBYTE(m_player->m_changed_government_this_turn);

	PUSHLONG(m_player->m_advances->m_size);
	sint32 i;
	for(i = 0; i < m_player->m_advances->m_size; i+= 8) {
		buf[size] = 0;
		for(sint32 d = 0; d < 8 && i+d < m_player->m_advances->m_size; d++) {
			Assert((m_player->m_advances->m_hasAdvance[i+d] & 0xfe) == 0);
			buf[size] |= m_player->m_advances->m_hasAdvance[i+d] << (7 - d);
		}
		size++;
	}

	PUSHLONG(m_player->m_advances->m_researching);
	PUSHLONG(m_player->m_advances->m_discovered);

	PUSHLONG(m_player->m_materialPool->GetMaterials());
	PUSHLONG(m_player->m_materialPool->GetCap());

	double s;
	m_player->m_tax_rate->GetScienceTaxRate(s);
	PUSHDOUBLE(s);
	PUSHDOUBLE(m_player->m_materialsTax);
	PUSHSHORT((sint16)m_player->m_current_round);
	PUSHBYTE((uint8)m_player->m_is_turn_over);

	PUSHLONG64(m_player->m_builtWonders);
	PUSHLONG64(m_player->m_wonderBuildings);
	PUSHLONG(m_player->m_contactedPlayers);
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		PUSHBYTE(m_player->m_diplomatic_state[i]);
	}
	PUSHLONG(m_player->m_embassies);
	PUSHBYTE(m_player->m_first_city);
	PUSHLONG(m_player->m_capitol->m_id);
	PUSHLONG(m_player->m_networkGroup);

	
	
	PUSHLONG(m_player->m_usedTradeTransportPoints);







	PUSHLONG(m_player->m_score->m_finalScore);
	PUSHLONG(m_player->m_score->m_cities_recaptured);
	PUSHLONG(m_player->m_score->m_opponents_conquered);
	PUSHLONG(m_player->m_score->m_victory_type);
	PUSHLONG(m_player->m_score->m_feats);

	PUSHBYTE(m_player->m_can_build_capitalization);
	PUSHBYTE(m_player->m_can_build_infrastructure);
	PUSHBYTE(m_player->m_can_use_space_button);
	PUSHBYTE(m_player->m_deepOceanVisible);
	PUSHBYTE(m_player->m_can_use_terra_tab);
	PUSHBYTE(m_player->m_can_use_sea_tab);
	PUSHBYTE(m_player->m_can_use_space_tab);
	PUSHBYTE(m_player->m_hasGlobalRadar);

	for(i = 0; i < k_MAX_EVENT_POLLUTION_TURNS; i++) {
		PUSHLONG(m_player->m_event_pollution[i]);
	}
}

void NetPlayer::Unpacketize(uint16 id, uint8* buf, uint16 size)
{
	Assert(buf[0] == 'G' && buf[1] == 'p');
	uint16 pos = 2;
	Player *p;

	PLAYER_INDEX pidx;
	PULLLONG(pidx);

	Assert(pidx < k_MAX_PLAYERS);
	p = g_player[pidx];
	Assert(p != NULL);
	Assert(p->m_owner == pidx);

	PULLLONGTYPE(p->m_playerType, PLAYER_TYPE);
	PULLLONG(p->mask_alliance);
	PULLLONG(p->m_government_type);
	PULLLONG(p->m_set_government_type);
	PULLLONG(p->m_change_government_turn);
	PULLBYTE(p->m_changed_government_this_turn);

	sint32 advSize;
	PULLLONG(advSize);
	if(p->m_advances->m_hasAdvance) {
		delete [] p->m_advances->m_hasAdvance;
	}

	p->m_advances->m_hasAdvance = new uint8[advSize];

	p->m_advances->m_size = advSize;
	uint8* hasAdvance = p->m_advances->m_hasAdvance;
	sint32 i;
	for(i = 0; i < advSize; i += 8) {
		for(sint32 d = 0; d < 8 && i+d < advSize; d++) {
			hasAdvance[i+d] = uint8((buf[pos] & (1 << (7-d))) >> (7-d));
		}
		pos++;
	}
	PULLLONG(p->m_advances->m_researching);
	PULLLONG(p->m_advances->m_discovered);

	sint32 matLev, matCap;
	PULLLONG(matLev);
	PULLLONG(matCap);
	p->m_materialPool->SetLevel(matLev);
	p->m_materialPool->SetCap(matCap);

	double science;
	PULLDOUBLE(science);
	p->m_tax_rate->SetTaxRates(science, p->m_owner);
	PULLDOUBLE(p->m_materialsTax);

	PULLSHORTTYPE(p->m_current_round, sint32);
	PULLBYTETYPE(p->m_is_turn_over, sint32);

	PULLLONG64(p->m_builtWonders);
	PULLLONG64(p->m_wonderBuildings);
	PULLLONG(p->m_contactedPlayers);
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		PULLBYTETYPE(p->m_diplomatic_state[i], DIPLOMATIC_STATE);
	}

	PULLLONG(p->m_embassies);

	PULLBYTE(p->m_first_city);
	PULLLONG(p->m_capitol->m_id);
	PULLLONG(p->m_networkGroup);
	
	PULLLONG(p->m_usedTradeTransportPoints);






	
	PULLLONG(p->m_score->m_finalScore);
	PULLLONG(p->m_score->m_cities_recaptured);
	PULLLONG(p->m_score->m_opponents_conquered);
	PULLLONG(p->m_score->m_victory_type);
	PULLLONG(p->m_score->m_feats);

	PULLBYTETYPE(p->m_can_build_capitalization, BOOL);
	PULLBYTETYPE(p->m_can_build_infrastructure, BOOL);
	PULLBYTETYPE(p->m_can_use_space_button, BOOL);
	PULLBYTETYPE(p->m_deepOceanVisible, BOOL);
	PULLBYTETYPE(p->m_can_use_terra_tab, BOOL);
	PULLBYTETYPE(p->m_can_use_sea_tab, BOOL);
	PULLBYTETYPE(p->m_can_use_space_tab, BOOL);
	PULLBYTETYPE(p->m_hasGlobalRadar, BOOL);

	for(i = 0; i < k_MAX_EVENT_POLLUTION_TURNS; i++) {
		PULLLONG(p->m_event_pollution[i]);
	}

	if(g_selected_item && g_controlPanel && g_selected_item->GetCurPlayer() == p->m_owner) {
		MainControlPanel::UpdatePlayer(p->m_owner);
	}

	Assert(pos == size);
}

void NetAddPlayer::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_ADD_PLAYER_ID);
	PUSHSHORT(m_id);
	PUSHSTRING(m_name);
}

void NetAddPlayer::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_ADD_PLAYER_ID);

	PULLSHORT(m_id);
	MBCHAR name[1024];
	PULLSTRING(name);

	g_network.AddPlayer(m_id, name);
	Assert(pos == size);
}
