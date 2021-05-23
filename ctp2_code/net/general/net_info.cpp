//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : net_info is used to propagate misc information from host to
//                player(s).
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
// - Propagate PW each turn update.
// - Unblock the client user interface when start of turn processing is ready.
// - Added NET_INFO_CODE_DISBANDED_CITY_SETTLER handling.
// - Added NET_INFO_CODE_ACCOMPLISHED_FEAT handling.
// - Include finish production handling in MP.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3errors.h"
#include "Cell.h"

#include "network.h"
#include "net_info.h"
#include "net_util.h"
#include "net_action.h"

#include "UnitData.h"
#include "player.h"
#include "World.h"
#include "Advances.h"
#include "AdvanceRecord.h"
#include "citydata.h"
#include "UnitPool.h"
#include "ArmyPool.h"
#include "TradeOffer.h"
#include "TradeOfferPool.h"
#include "TerrImprove.h"
#include "TerrImprovePool.h"
#include "installation.h"
#include "PlayHap.h"

#include "QuadTree.h"
#include "Vision.h"
#include "TaxRate.h"
#include "Gold.h"
#include "Sci.h"
#include "message.h"
#include "MessagePool.h"
#include "DiplomaticRequest.h"
#include "DiplomaticRequestPool.h"
#include "Agreement.h"
#include "AgreementPool.h"

#include "GoodyHuts.h"

#include "WonderRecord.h"

#include "newturncount.h"

#include "SelItem.h"
#include "TurnCnt.h"
#include "c3ui.h"
extern TurnCount *g_turn;
extern C3UI					*g_c3ui;

#include "pixelutils.h"
#include "primitives.h"

#include "aui.h"
#include "ctp2_Window.h"

#include "tiledmap.h"
#include "background.h"
#include "radarmap.h"
#include "radarwindow.h"
#include "director.h"
#include "AICause.h"
#include "CivPaths.h"
#include "installationpool.h"
#include "soundmanager.h"
#include "gamesounds.h"
#include "HappyTracker.h"
#include "WonderTracker.h"
#include "AchievementTracker.h"
#include "ArmyData.h"

#include "SlicEngine.h"
#include "SlicObject.h"
#include "TradePool.h"

#include "Globals.h"

#include "wonderutil.h"

#include "GameEventManager.h"

#include "ctpai.h"
#include "GameOver.h"

#include "settlemap.h"

#include "MainControlPanel.h"

#include "citywindow.h"
#include "EditQueue.h"

#include "DomesticManagementDialog.h"
#include "unitmanager.h"
#include "armymanagerwindow.h"
#include "trademanager.h"

extern Background	*g_background;
extern RadarMap     *g_radarMap;
extern CivPaths     *g_civPaths;
extern SoundManager	*g_soundManager;
extern QuadTree<Unit> *g_theUnitTree;

extern void player_ActivateSpaceButton(sint32 pl);
extern void network_VerifyGameData();

#include "FeatTracker.h"	// Propagate feats
#include "MaterialPool.h"	// Propagate PW

const uint32 NetInfo::m_args[NET_INFO_CODE_NULL] = {
	// this array tells how many arguments each NET_INFO has

	2, // NET_INFO_CODE_PLAYER_INDEX
	1, // NET_INFO_CODE_BEGIN_TURN
	0, // NET_INFO_CODE_MAP_DONE
	0, // NET_INFO_CODE_START_UNITS
	2, // NET_INFO_CODE_END_UNITS
	1, // NET_INFO_CODE_KILL_UNIT
	4, // NET_INFO_CODE_TAX_RATE
	3, // NET_INFO_CODE_GOLD_SCIENCE
	4, // NET_INFO_CODE_ADVANCE
	2, // NET_INFO_CODE_SUPPORT_UNIT

	1, // NET_INFO_CODE_KILL_POP
	2, // NET_INFO_CODE_KILL_TRADE_ROUTE
	2, // NET_INFO_CODE_BUILDING_UNIT
	1, // NET_INFO_CODE_KILL_TRADE_OFFER
	1, // NET_INFO_CODE_ACK_OBJECT
	2, // NET_INFO_CODE_NAK_OBJECT
	1, // NET_INFO_CODE_KILL_IMPROVEMENT
	1, // NET_INFO_CODE_KILL_INSTALLATION
	2, // NET_INFO_CODE_GOLD
	2, // NET_INFO_CODE_MATERIALS_TAX

	2, // NET_INFO_CODE_WORKDAY_LEVEL
	2, // NET_INFO_CODE_WAGES_LEVEL
	2, // NET_INFO_CODE_RATIONS_LEVEL
	3, // NET_INFO_CODE_NEW_CIVILIZATION
	1, // NET_INFO_CODE_BUILT_FRONT
	1, // NET_INFO_CODE_KILL_MESSAGE
	1, // NET_INFO_CODE_KILL_DIP_REQUEST
	4, // NET_INFO_CODE_RESET_CITY_OWNER
	2, // NET_INFO_CODE_RESEARCH
	2, // NET_INFO_CODE_REMOVE_HUT

	1, // NET_INFO_CODE_GLOBAL_WARMING
	1, // NET_INFO_CODE_OZONE_DEPLETION
	2, // NET_INFO_CODE_BATTLE
	3, // NET_INFO_CODE_KILL_PLAYER
	3, // NET_INFO_CODE_BUILD_IMP
	4, // NET_INFO_CODE_CHANGE_BUILD
	2, // NET_INFO_CODE_YEAR
	1, // NET_INFO_CODE_BEGIN_SLICE
	0, // NET_INFO_CODE_REQUEST_SLICE
	2, // NET_INFO_CODE_SET_GOVERNMENT

	1, // NET_INFO_CODE_ENACT_REQUEST
	1, // NET_INFO_CODE_REJECT_REQUEST
	0, // NET_INFO_CODE_CLASSIC_STYLE
	2, // NET_INFO_CODE_UNIT_MOVES_STYLE
	3, // NET_INFO_CODE_SPEED_STYLE
	2, // NET_INFO_CODE_TIMED_STYLE
	1, // NET_INFO_CODE_SIMULTANEOUS_STYLE
	0, // NET_INFO_CODE_TURN_SYNC
	1, // NET_INFO_CODE_CARRYOVER_STYLE
	1, // NET_INFO_CODE_SET_SETUP_MODE

	4, // NET_INFO_CODE_SET_SETUP_AREA
	2, // NET_INFO_CODE_POWER_POINTS
	1, // NET_INFO_CODE_CHOOSE_RESEARCH
	2, // NET_INFO_CODE_SET_OPTIMAL_OUTPUT
	1, // NET_INFO_CODE_END_TURN_FOR
	2, // NET_INFO_CODE_POP_TYPE
	2, // NET_INFO_CODE_ACTUALLY_SET_GOVERNMENT
	3, // NET_INFO_CODE_CHANGE_INSTALLATION_OWNER
	2, // NET_INFO_CODE_REMOVE_BUILD_ITEM
	3, // NET_INFO_CODE_HAPPINESS_ATTACK

	1, // NET_INFO_CODE_CLEAR_ORDERS
	1, // NET_INFO_CODE_EXECUTE_ORDERS
	4, // NET_INFO_CODE_ADD_ARMY
	1, // NET_INFO_CODE_SET_MOVEMENT_TO_ZERO
	2, // NET_INFO_CODE_SET_ARMY
	3, // NET_INFO_CODE_CHANGE_ARMY
	2, // NET_INFO_CODE_ACHIEVEMENTS
	2, // NET_INFO_CODE_BUILT_WONDERS
	2, // NET_INFO_CODE_BUILD_WONDER
	5, // NET_INFO_CODE_SEND_TRADE_BID

	2, // NET_INFO_CODE_SEND_SLAVE_TO
	1, // NET_INFO_CODE_ACK_POP_MOVE
	1, // NET_INFO_CODE_CLEAR_ORDERS_EXCEPT_GROUP
	1, // NET_INFO_CODE_BUILD_CAPITALIZATION
	1, // NET_INFO_CODE_BUILD_INFRASTRUCTURE
	2, // NET_INFO_CODE_BUILD_END_GAME
	1, // NET_INFO_CODE_PROGRESS
	4, // NET_INFO_CODE_TAKE_TRADE_OFFER
	2, // NET_INFO_CODE_SOLD_BUILDING
	1, // NET_INFO_CODE_KILL_AGREEMENT
	3, // NET_INFO_CODE_BREAK_CEASE_FIRE
	2, // NET_INFO_CODE_BREAK_ALLIANCE
	1, // NET_INFO_CODE_CLEAR_QUEUE
	0, // NET_INFO_CODE_RESYNC
	2, // NET_INFO_CODE_WONDER_BUILT
	3, // NET_INFO_CODE_SEND_OFFER_ACCEPT_MESSAGE
	1, // NET_INFO_CODE_ATTACH_ROBOT
	1, // NET_INFO_CODE_DETACH_ROBOT
	3, // NET_INFO_CODE_SET_DIP_STATE
	1, // NET_INFO_CODE_SET_TURN
	3, // NET_INFO_CODE_RESET_UNIT_OWNER
	2, // NET_INFO_CODE_CELL_OWNER
	1, // NET_INFO_CODE_ENACT_REQUEST_NEED_ACK
	1, // NET_INFO_CODE_NAK_ENACT
	1, // NET_INFO_CODE_REMOVE_ILLEGAL_ITEMS
	1, // NET_INFO_CODE_MAKE_UNIT_PERMANENT
	1, // NET_INFO_CODE_CLEAR_QUEUE_EXCEPT_HEAD
	0, // NET_INFO_CODE_REMOVE_DEAD_PLAYERS
	2, // NET_INFO_CODE_WONDER_STARTED
	2, // NET_INFO_CODE_WONDER_OBSOLETE
	1, // NET_INFO_CODE_BEGIN_TURN_ENEMY_UNITS

	3, // NET_INFO_CODE_WONDER_ALMOST_DONE
	2, // NET_INFO_CODE_WONDER_STOPPED
	0, // NET_INFO_CODE_ALL_PLAYERS_READY
	2, // NET_INFO_CODE_SET_FRANCHISE_PRODUCTION
	4, // NET_INFO_CODE_REVOLT_NOTICES
	2, // NET_INFO_CODE_ADD_UNSEEN
	2, // NET_INFO_CODE_OFFER_REJECTED_MESSAGE
	1, // NET_INFO_CODE_WON_END_GAME
	2, // NET_INFO_CODE_RECOVERED_PROBE

	1, // NET_INFO_CODE_OTHER_CIV_LAB_MSG
	1, // NET_INFO_CODE_OTHER_CIV_SEQUENCE_MSG
	1, // NET_INFO_CODE_ALIEN_ALMOST_DONE_OTHERS_MSG
	1, // NET_INFO_CODE_ALIEN_SCRAPPED_OWNER
	1, // NET_INFO_CODE_CATACLYSM_OTHER

	1, // NET_INFO_CODE_GAME_OVER_OUT_OF_TIME
	1, // NET_INFO_CODE_ACTIVATE_SPACE_BUTTON
	0, // NET_INFO_CODE_TIMES_ALMOST_UP
	2, // NET_INFO_CODE_VIOLATE_AGREEMENT
	3, // NET_INFO_CODE_SET_MAYOR
	1, // NET_INFO_CODE_FINISH_AI_TURN
	1, // NET_INFO_CODE_CITIES_DONE
	2, // NET_INFO_CODE_SET_ROUND
	1, // NET_INFO_CODE_BEGIN_SCHEDULER
	2, // NET_INFO_CODE_REMOTE_GROUP
	2, // NET_INFO_CODE_REMOTE_UNGROUP
	1, // NET_INFO_CODE_REENTER
	3, // NET_INFO_CODE_PLAYER_TRADE_DATA
	2, // NET_INFO_CODE_CREATED_WONDER
	3, // NET_INFO_CODE_GAME_OVER
	2, // NET_INFO_CODE_SET_EMBASSIES
	3, // NET_INFO_CODE_ACCOMPLISHED_FEAT
	1, // NET_INFO_CODE_DISBANDED_CITY_SETTLER
	1, // NET_INFO_CODE_MATERIALS				(unconfirmed value)
};

void
NetInfo::Packetize(uint8* buf, uint16& size)
{
	buf[0] = 'I';
	buf[1] = 'I';
	putshort(&buf[2], static_cast<sint16>(m_type));
	size = 4;
	if(m_args[m_type] > 0) {
		putlong(&buf[4], m_data); size += 4;
	}
	if(m_args[m_type] > 1) {
		putlong(&buf[8], m_data2); size += 4;
	}
	if(m_args[m_type] > 2) {
		putlong(&buf[12], m_data3); size += 4;
	}
	if(m_args[m_type] > 3) {
		putlong(&buf[16], m_data4); size += 4;
	}
	if(m_args[m_type] > 4) {
		putlong(&buf[20], m_data5); size += 4;
	}
}

void
NetInfo::Unpacketize(uint16 id, uint8* buf, uint16 size)
{
	Assert(buf[0] == 'I' && buf[1] == 'I');
	m_type = (NET_INFO_CODE)getshort(&buf[2]);

	// Test code for detection of unexpected messages
	Assert(size == 4 + (4 * m_args[m_type]));

	if(m_args[m_type] > 0) {
		m_data = getlong(&buf[4]);
	}
	if(m_args[m_type] > 1) {
		m_data2 = getlong(&buf[8]);
	}
	if(m_args[m_type] > 2) {
		m_data3 = getlong(&buf[12]);
	}
	if(m_args[m_type] > 3) {
		m_data4 = getlong(&buf[16]);
	}
	if(m_args[m_type] > 4) {
		m_data5 = getlong(&buf[20]);
	}

	switch(m_type) {
		case NET_INFO_CODE_PLAYER_INDEX:

			g_network.SetPlayerIndex(m_data, (uint16)m_data2);
			break;
		case NET_INFO_CODE_BEGIN_TURN:

			g_network.SetLoop(FALSE);
			DPRINTF(k_DBG_NET, ("Starting turn for player %d\n", m_data));
			g_selected_item->SetCurPlayer(PLAYER_INDEX(m_data));
			if(m_data == 0) {
				NewTurnCount::ClientStartNewYear();
			}

			MainControlPanel::UpdatePlayer(m_data);

			if(g_selected_item->GetCurPlayer() == g_network.GetPlayerIndex()) {
				if(g_network.ShouldAckBeginTurn()) {
					CityWindow::Hide();
					EditQueue::Hide();
					DomesticManagementDialog::Close();
					UnitManager::Hide();

					g_network.SetSensitiveUIBlocked(true);

					g_gevManager->AddEvent(GEV_INSERT_Tail,
										   GEV_BeginTurn,
										   GEA_Player, m_data,
										   GEA_Int, g_player[m_data]->GetCurRound() + 1,
										   GEA_End);

					if (g_soundManager)
						g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0,
												 gamesounds_GetGameSoundID(GAMESOUNDS_NET_YOUR_TURN),
												 0,
												 0);
					g_selected_item->Refresh();
				} else {

					g_network.SendAction(new NetAction(NET_ACTION_NAK_BEGIN_TURN));
				}
			} else {
				g_network.SetMyTurn(FALSE);
			}
			break;
		case NET_INFO_CODE_SET_TURN:
			g_network.SetLoop(FALSE);
			DPRINTF(k_DBG_NET, ("It's already player %d's turn\n", m_data));
			g_selected_item->SetCurPlayer(PLAYER_INDEX(m_data));
			if(m_data == (uint32)g_network.GetPlayerIndex()) {
				g_network.SetMyTurn(TRUE);
			} else {
				g_network.SetMyTurn(FALSE);
			}
			break;
		case NET_INFO_CODE_MAP_DONE:
		{

			g_c3ui->RemoveWindow(g_radarWindow->Id());
			radarwindow_Cleanup();

			delete g_tiledMap;
			g_tiledMap = NULL;

			MapPoint mapsize(g_theWorld->GetXWidth(),
							 g_theWorld->GetYHeight());
			g_tiledMap = new TiledMap(mapsize);
			g_tiledMap->LoadTileset();

			RECT rect =
			{
				g_background->X(),
				g_background->Y(),
				g_background->X() + g_background->Width(),
				g_background->Y() + g_background->Height()
			};

			g_tiledMap->Initialize(&rect);
			g_tiledMap->Refresh();

			radarwindow_Initialize();

			radarwindow_Display();


			g_tiledMap->PostProcessMap();
			g_tiledMap->Refresh();

			g_theWorld->NumberContinents();





			g_background->Draw();

			g_network.SetLoop(FALSE);
			break;
		}
		case NET_INFO_CODE_START_UNITS:
		{
			g_tiledMap->CopyVision();

			for(sint32 p = 0; p < k_MAX_PLAYERS; p++) {
				if(g_player[p]) {
					g_player[p]->m_all_armies->FastKillList();
					g_player[p]->GetAllUnitList()->FastKillList();
					g_player[p]->GetAllCitiesList()->FastKillList();
					g_player[p]->GetTradersList()->FastKillList();
					g_player[p]->m_vision->Clear();
				}
			}
			g_theUnitTree->Clear();
			g_network.ClearDeadUnits();

			CtpAi::Initialize();
			break;
		}
		case NET_INFO_CODE_END_UNITS:
		{
			g_theUnitPool->HackSetKey(m_data);
			g_theArmyPool->HackSetKey(m_data2);




			g_network.ClearDeadUnits();

			g_director->NextPlayer();

			g_radarMap->Update();

			sint32 i;
			for(i = 0; i < k_MAX_PLAYERS; i++) {
				if(!g_player[i]) continue;
				sint32 j;
				for(j = 0; j < g_player[i]->m_all_cities->Num(); j++) {
					g_player[i]->m_all_cities->Access(j).GetData()->GetCityData()->UpdateSprite();
					SettleMap::s_settleMap.HandleCityGrowth(g_player[i]->m_all_cities->Access(j), g_player[i]->m_all_cities->Access(j).CD()->GetSizeIndex());
				}
			}

			break;
		}
		case NET_INFO_CODE_KILL_UNIT:
		{
			if(g_network.DeadUnit(m_data)) {
				g_network.RemoveDeadUnit(m_data);
			} else {
				Unit unit(m_data);
				if(unit.IsValid()) {
					unit.KillUnit(CAUSE_REMOVE_ARMY_UNKNOWN, -1);

					g_network.RemoveDeadUnit(m_data);
				}
			}
			break;
		}
		case NET_INFO_CODE_TAX_RATE:
		{
			double s;
			s = (double)m_data2 / 100000.;
			Assert(0.0 <= s);
            Assert(s <= 1.0);
			if(g_player[m_data])
				g_player[m_data]->m_tax_rate->SetTaxRates(s, m_data);
			DPRINTF(k_DBG_NET, ("Set taxes for player %d to %lf\n", m_data, s));
			break;
		}
		case NET_INFO_CODE_GOLD_SCIENCE:
			if(g_player[m_data]) {
				g_player[m_data]->m_gold->SetLevel(m_data2);
				g_player[m_data]->m_science->SetLevel(m_data3);
				DPRINTF(k_DBG_NET, ("Set for player %d gold to %d, science to %d\n", m_data, m_data2, m_data3));
			}
			break;
		case NET_INFO_CODE_ADVANCE:
		{
			DPRINTF(k_DBG_NET, ("Player %d accquired %s\n",
								m_data, g_theAdvanceDB->GetNameStr(m_data2)));
			if(g_player[m_data]) {
				g_player[m_data]->m_advances->SetHasAdvance(m_data2);
				g_player[m_data]->m_advances->m_discovered = m_data3;
				g_player[m_data]->m_science->SetLevel(m_data4);

				sint32 i;
				for(i = 0; i < g_player[m_data]->m_all_cities->Num(); i++) {
					g_player[m_data]->m_all_cities->Access(i).GetData()->
						GetCityData()->GetBuildQueue()->RemoveIllegalItems(TRUE);
				}
			}
			break;
		}
		case NET_INFO_CODE_SUPPORT_UNIT:
		{
			DPRINTF(k_DBG_NET, ("Net: Supporting unit %d from city %d\n",
								m_data, m_data2));
			Assert(FALSE);
			break;
		}
		case NET_INFO_CODE_KILL_TRADE_ROUTE:
		{
			DPRINTF(k_DBG_NET, ("Net: Killing trade route %d\n", m_data));


			if(g_network.DeadUnit(m_data)) {
				g_network.RemoveDeadUnit(m_data);
			} else {
				TradeRoute route(m_data);
				if(g_theTradePool->IsValid(route)) {
					route.KillRoute(CAUSE_KILL_TRADE_ROUTE(m_data2));

					g_network.RemoveDeadUnit(m_data);
				}
			}
			break;
		}
		case NET_INFO_CODE_BUILDING_UNIT:
		{
			DPRINTF(k_DBG_NET, ("Net: City %d building unit %d\n",
								m_data2, m_data));
			Unit u(m_data2);
			Assert(g_theUnitPool->IsValid(u));
			if(u.IsValid()) {
				u.BuildUnit(m_data);
			}
			break;
		}
		case NET_INFO_CODE_KILL_TRADE_OFFER:
		{
			DPRINTF(k_DBG_NET, ("Net: Killing trade offer %d\n", m_data));
			if(g_network.DeadUnit(m_data)) {
				g_network.RemoveDeadUnit(m_data);
			} else {
				TradeOffer offer(m_data);
				Assert(g_theTradeOfferPool->IsValid(offer));
				if(g_theTradeOfferPool->IsValid(offer)) {
					offer.KillOffer();
				}
				g_network.RemoveDeadUnit(m_data);
			}
			break;
		}
		case NET_INFO_CODE_ACK_OBJECT:
			DPRINTF(k_DBG_NET, ("Net: Object %lx ACKed\n", m_data));
			g_network.HandleObjectACK(m_data);
			break;
		case NET_INFO_CODE_NAK_OBJECT:
			DPRINTF(k_DBG_NET, ("Net: Object %lx NAKed, should be %lx\n",
								m_data, m_data2));
			g_network.HandleObjectNAK(m_data, m_data2);
			break;
		case NET_INFO_CODE_KILL_IMPROVEMENT:
		{
			DPRINTF(k_DBG_NET, ("Net: Killing improvement %lx\n", m_data));
			TerrainImprovement imp(m_data);
			if(g_theTerrainImprovementPool->IsValid(imp))
				TerrainImprovement(m_data).Kill();
			break;
		}
		case NET_INFO_CODE_KILL_INSTALLATION:
		{
			DPRINTF(k_DBG_NET, ("Net: Killing installation %lx\n", m_data));
			Installation inst(m_data);
			if(g_theInstallationPool->IsValid(inst)) {
				Installation(m_data).Kill();
			}
			break;
		}
		case NET_INFO_CODE_GOLD:
			DPRINTF(k_DBG_NET, ("Net: Setting player %d's gold to %d\n",
								m_data, m_data2));
			if(g_player[m_data]) {
				g_player[m_data]->m_gold->SetLevel(m_data2);
			}
			break;
		case NET_INFO_CODE_MATERIALS_TAX:
			DPRINTF(k_DBG_NET, ("Net: Setting player %d's material tax to %d\n",
								m_data, m_data2));
			if(g_player[m_data]) {
				g_player[m_data]->m_materialsTax = (double)((double)m_data2 / 100.0);
			}
			break;
		case NET_INFO_CODE_WORKDAY_LEVEL:
			if(g_player[m_data])
				g_player[m_data]->m_global_happiness->SetWorkdayLevel(m_data2);
			break;
		case NET_INFO_CODE_WAGES_LEVEL:
			if(g_player[m_data])
				g_player[m_data]->m_global_happiness->SetWagesLevel(m_data2);
			break;
		case NET_INFO_CODE_RATIONS_LEVEL:
			if(g_player[m_data])
				g_player[m_data]->m_global_happiness->SetRationsLevel(m_data2);
			break;
		case NET_INFO_CODE_NEW_CIVILIZATION:
			DPRINTF(k_DBG_NET, ("Server sent new civilisation\n"));





			if(g_player[m_data]) {

			} else {
				g_player[m_data] = new Player(PLAYER_INDEX(m_data), 0, PLAYER_TYPE(m_data2));
				g_selected_item->AddPlayer(PLAYER_INDEX(m_data));





			}

			break;
		case NET_INFO_CODE_BUILT_FRONT:
		{
			DPRINTF(k_DBG_NET, ("Server says city %lx built something\n",
								m_data));
			Unit city(m_data);
			Assert(g_theUnitPool->IsValid(city));
			if(!g_theUnitPool->IsValid(city))
				return;
			break;
		}
		case NET_INFO_CODE_KILL_MESSAGE:
		{
			DPRINTF(k_DBG_NET, ("Server says kill message %lx\n", m_data));
			Message message(m_data);

			if(!g_theMessagePool->IsValid(message))
				return;

			break;
		}
		case NET_INFO_CODE_KILL_DIP_REQUEST:
		{
			DPRINTF(k_DBG_NET, ("Server says kill dip request %lx\n", m_data));
			DiplomaticRequest request(m_data);

			g_network.RemoveEnact(request);
			if(!g_theDiplomaticRequestPool->IsValid(request))
				return;
			request.Kill();
			break;
		}
		case NET_INFO_CODE_RESET_CITY_OWNER:
		{
			DPRINTF(k_DBG_NET, ("Server says reset city %lx to owner %d, conq: %d\n",
								m_data, m_data2, m_data3));
			Unit unit(m_data);
			if(g_theUnitPool->IsValid(unit)) {
				if(unit.GetOwner() != (sint32)m_data2) {

					unit.ResetCityOwner(m_data2, m_data3, CAUSE_REMOVE_CITY(m_data4));
				}
			}
			break;
		}
		case NET_INFO_CODE_RESEARCH:
		{
			DPRINTF(k_DBG_NET, ("Server says player %d is researching %d\n",
								m_data, m_data2));
			if(g_player[m_data])
				g_player[m_data]->SetResearching(m_data2);
			break;
		}
		case NET_INFO_CODE_REMOVE_HUT:
		{
			DPRINTF(k_DBG_NET, ("Server removed a goody hut at %d,%d\n",
								m_data, m_data2));
			Cell *cell = g_theWorld->GetCell(m_data, m_data2);
			cell->DeleteGoodyHut();
			break;
		}
		case NET_INFO_CODE_GLOBAL_WARMING:
		{
			DPRINTF(k_DBG_NET, ("Server said global warming %d happened\n",
								m_data));
			g_theWorld->GlobalWarming(m_data);
			break;
		}
		case NET_INFO_CODE_OZONE_DEPLETION:
		{
			DPRINTF(k_DBG_NET, ("Server said ozone depletion %d occurred\n",
								m_data));
			g_theWorld->OzoneDepletion();
			break;
		}

		case NET_INFO_CODE_BATTLE:
		{
			DPRINTF(k_DBG_NET, ("Server says do a battle\n"));
			break;
		}
		case NET_INFO_CODE_KILL_PLAYER:
			DPRINTF(k_DBG_NET, ("Player %d is DEAD (%d,%d)\n", m_data,
								m_data2, m_data3));
			if(g_player[m_data])
				g_player[m_data]->StartDeath((GAME_OVER)m_data2, m_data3);
			break;
		case NET_INFO_CODE_BUILD_IMP:
		{
			DPRINTF(k_DBG_NET, ("Server: Player %d's city #%d is building %d\n",
								m_data, m_data3, m_data2));
			Unit city(m_data3);
			if(city.IsValid()) {
				Assert(city.GetOwner() == (PLAYER_INDEX)m_data);
				city.BuildImprovement(m_data2);
			} else {
				g_network.RequestResync(RESYNC_INVALID_UNIT);
			}
			break;
		}
		case NET_INFO_CODE_CHANGE_BUILD:
		{
			DPRINTF(k_DBG_NET, ("Server: Player %d's build queue changed to %d,%d\n", m_data3, m_data4));
			Unit city(m_data2);
			if(city.IsValid()) {
				city.ChangeCurrentlyBuildingItem(m_data3, m_data4);
			} else {
				g_network.RequestResync(RESYNC_INVALID_UNIT);
			}
			break;
		}
		case NET_INFO_CODE_YEAR:
			DPRINTF(k_DBG_NET, ("Server set year: %d (round %d)\n", m_data2, m_data));
			g_turn->m_year = m_data2;
			g_turn->m_round = m_data;
			break;

		case NET_INFO_CODE_BEGIN_SLICE:

			DPRINTF(k_DBG_NET, ("Starting slice for player %d\n", m_data));
			g_selected_item->SetCurPlayer(PLAYER_INDEX(m_data));
			if(g_selected_item->GetCurPlayer() == g_network.GetPlayerIndex()) {
				if (g_soundManager && !g_network.IsMyTurn())
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0,
												gamesounds_GetGameSoundID(GAMESOUNDS_NET_YOUR_TURN),
												0,
												0);
				g_network.SetMyTurn(TRUE);
				g_network.UnitsMoved(-g_network.GetUnitMovesUsed());
				g_turn->BeginNewSlice();
			} else {
				g_network.SetMyTurn(FALSE);
			}
			break;
		case NET_INFO_CODE_REQUEST_SLICE:
			Assert(g_selected_item->GetCurPlayer() == g_network.GetPlayerIndex());
			g_network.SetMyTurn(FALSE);
			g_network.SendAction(new NetAction(NET_ACTION_END_SLICE));
			break;
		case NET_INFO_CODE_SET_GOVERNMENT:
			DPRINTF(k_DBG_NET, ("Server: Player %d's governement is now %d\n",
								m_data, m_data2));
			if(g_player[m_data])
				g_player[m_data]->SetGovernmentType(m_data2);
			break;
		case NET_INFO_CODE_ENACT_REQUEST:
		case NET_INFO_CODE_ENACT_REQUEST_NEED_ACK:
		{
			DPRINTF(k_DBG_NET, ("Server: Diplomatic request %lx enacted\n",
								m_data));
			DiplomaticRequest req(m_data);
			g_network.RemoveEnact(req);

			if(m_type == NET_INFO_CODE_ENACT_REQUEST_NEED_ACK) {
				g_network.SendAction(new NetAction(NET_ACTION_ACK_ENACT,
												   req.m_id));
			}
			if(!g_theDiplomaticRequestPool->IsValid(req))
				break;
			req.Enact(TRUE);

			break;
		}
		case NET_INFO_CODE_REJECT_REQUEST:
		{
			DPRINTF(k_DBG_NET, ("Server: Diplomatic request %lx rejected\n",
								m_data));
			DiplomaticRequest req(m_data);

			if(!g_theDiplomaticRequestPool->IsValid(req))
				break;
			req.Reject(TRUE);
			break;
		}
		case NET_INFO_CODE_CLASSIC_STYLE:
			DPRINTF(k_DBG_NET, ("Server: game is now classic style\n"));
			g_network.SetClassicStyle(TRUE);
			break;
		case NET_INFO_CODE_UNIT_MOVES_STYLE:
			DPRINTF(k_DBG_NET, ("Server: unit moves style %s, moves=%d\n",
								m_data ? "ON" : "OFF", m_data2));
			g_network.SetUnitMovesStyle(m_data, m_data2, TRUE);
			break;
		case NET_INFO_CODE_SPEED_STYLE:
			DPRINTF(k_DBG_NET, ("Server: speed style %s, time per turn=%d seconds\n",
								m_data ? "ON" : "OFF", m_data2));
			g_network.SetSpeedStyle(m_data, m_data2, TRUE, m_data3);
			break;
		case NET_INFO_CODE_TIMED_STYLE:
			DPRINTF(k_DBG_NET, ("Server: timed style %s, time per game=%d seconds\n",
								m_data ? "ON" : "OFF", m_data2));
			g_network.SetTimedStyle(m_data, m_data2, TRUE);
			break;
		case NET_INFO_CODE_SIMULTANEOUS_STYLE:
			DPRINTF(k_DBG_NET, ("Server: simultaneous style %s\n",
								m_data ? "ON" : "OFF"));
			g_network.SetSimultaneousStyle(m_data, TRUE);
			break;
		case NET_INFO_CODE_TURN_SYNC:
			DPRINTF(k_DBG_NET, ("Server: turn sync\n"));
			g_network.TurnSync();
			break;
		case NET_INFO_CODE_CARRYOVER_STYLE:
			DPRINTF(k_DBG_NET, ("Server: Carryover style %s\n",
								m_data ? "ON" : "OFF"));
			g_network.SetCarryoverStyle(m_data, TRUE);
			break;
		case NET_INFO_CODE_SET_SETUP_MODE:
			DPRINTF(k_DBG_NET, ("Server: Setup mode %s\n",
								m_data ? "ON" : "OFF"));
			if(m_data) {
				g_network.EnterSetupMode();
			} else {
				g_network.ExitSetupMode();
			}
			break;
		case NET_INFO_CODE_SET_SETUP_AREA:
		{
			DPRINTF(k_DBG_NET, ("Server: Set setup area for %d to (%d,%d)x%d\n",
								m_data, m_data2, m_data3, m_data4));
			MapPoint pnt(m_data2, m_data3);
			g_network.SetSetupArea(m_data, pnt, m_data4);
			break;
		}
		case NET_INFO_CODE_POWER_POINTS:
		{
			DPRINTF(k_DBG_NET, ("Server: Player %d has %d power points\n",
								m_data, m_data2));
			g_network.SetPowerPoints(m_data, m_data2);
			break;
		}
		case NET_INFO_CODE_CHOOSE_RESEARCH:
			DPRINTF(k_DBG_NET, ("Server: Can choose new research\n"));
			if(g_player[g_network.GetPlayerIndex()]) {
				g_player[g_network.GetPlayerIndex()]->BuildResearchDialog(m_data);
			}
			break;
		case NET_INFO_CODE_END_TURN_FOR:
		{
			DPRINTF(k_DBG_NET, ("Server: Run EndTurn() for player %d\n", m_data));

			if(g_player[m_data]) {
				g_player[m_data]->EndTurn();
			}
			g_director->NextPlayer();
			break;
		}
		case NET_INFO_CODE_ACTUALLY_SET_GOVERNMENT:
		{
			DPRINTF(k_DBG_NET, ("Server: Player %d's new government is %d\n",
								m_data, m_data2));
			if(g_player[m_data]) {
				g_player[m_data]->ActuallySetGovernment(m_data2);
			}
			break;
		}
		case NET_INFO_CODE_CHANGE_INSTALLATION_OWNER:
		{
			DPRINTF(k_DBG_NET, ("Server: Installation %d changed owner from %d to %d\n",
								m_data, m_data2, m_data3));
			Installation inst(m_data);
			if(g_theInstallationPool->IsValid(inst)) {
				if(m_data2 != (uint32)inst.GetOwner()) {
					inst.ChangeOwner(m_data3);
				} else {
					DPRINTF(k_DBG_NET, ("But already owned by %d\n", m_data2));
				}
			}
			break;
		}
		case NET_INFO_CODE_REMOVE_BUILD_ITEM:
		{
			DPRINTF(k_DBG_NET, ("Server: City %lx had build item %d removed\n",
								m_data, m_data2));
			Unit city(m_data);
			Assert(g_theUnitPool->IsValid(city));
			if(g_theUnitPool->IsValid(city)) {
				city.AccessData()->GetCityData()->GetBuildQueue()->
					RemoveNodeByIndex(m_data2, CAUSE_REMOVE_BUILD_ITEM_NETWORK);
			}
			break;
		}
		case NET_INFO_CODE_HAPPINESS_ATTACK:
		{
			DPRINTF(k_DBG_NET, ("Server: City %lx suffered a happiness attack of %d for %d turns\n",
								m_data, m_data2, m_data3));
			Unit city(m_data);
			Assert(g_theUnitPool->IsValid(city));
			if(g_theUnitPool->IsValid(city)) {
				city.AddHappyTimer(m_data2, double(-((sint32)m_data3)), HAPPY_REASON_HAPPINESS_ATTACK);
			}
			break;
		}
		case NET_INFO_CODE_CLEAR_ORDERS:
		{
			DPRINTF(k_DBG_NET, ("Server: Army %lx orders cleared\n",
								m_data));
			Army army(m_data);

			if(g_theArmyPool->IsValid(army)) {
				army.ClearOrders();
			} else {
				DPRINTF(k_DBG_NET, ("Server: Received CLEAR_ORDERS for invalid army %lx\n", m_data));
			}

			break;
		}
		case NET_INFO_CODE_EXECUTE_ORDERS:
		{
			DPRINTF(k_DBG_NET, ("Server: Army %lx executing orders\n",
					m_data));
			Army army(m_data);

			if(g_theArmyPool->IsValid(army)) {
				g_gevManager->Pause();
				BOOL res = army->ExecuteOrders();

				Assert(res);
				g_gevManager->Resume();
			} else {
				DPRINTF(k_DBG_NET, ("Server: Received EXECUTE_ORDERS for invalid army %lx\n", m_data));
			}

			break;
		}
		case NET_INFO_CODE_ADD_ARMY:
		{
			DPRINTF(k_DBG_NET, ("Server: Player %d had army %lx inserted, cause: %d\n",
								m_data, m_data3, m_data2));
			Army army(m_data3);
			if(!army.IsValid()) {
				g_network.RequestResync(RESYNC_INVALID_ARMY_OTHER);
				break;
			}

			if(g_player[m_data]) {
				g_player[m_data]->AddArmy(army, (CAUSE_NEW_ARMY)m_data2,
										  TRUE, Unit(m_data4));
			}
			break;
		}
		case NET_INFO_CODE_SET_MOVEMENT_TO_ZERO:
		{
			DPRINTF(k_DBG_NET, ("Server: Unit %lx movement set to 0\n", m_data));
			Unit unit(m_data);
			if(unit.IsValid()) {
				unit.SetMovementPoints(0.0);
			} else {
				g_network.RequestResync(RESYNC_INVALID_UNIT);
			}
			break;
		}

		case NET_INFO_CODE_SET_ARMY:
		{
			DPRINTF(k_DBG_NET, ("Server: Unit %lx now belongs to Army %lx\n",
								m_data, m_data2));
			Unit unit(m_data);
			Assert(g_theUnitPool->IsValid(unit) || g_network.DeadUnit(unit.m_id));
			if(g_theUnitPool->IsValid(unit)) {
				if(!g_theArmyPool->IsValid(m_data2)) {
					g_network.RequestResync(RESYNC_INVALID_ARMY_OTHER);
				} else {
					unit.SetArmy(m_data2);
				}
			} else {
				if(!g_network.DeadUnit(unit.m_id)) {
					g_network.RequestResync(RESYNC_INVALID_UNIT);
				}
			}
			break;
		}

		case NET_INFO_CODE_CHANGE_ARMY:
		{
			DPRINTF(k_DBG_NET, ("Server: Unit %lx changing armies to %lx from %lx\n",
					m_data, m_data3, m_data2));
			Unit unit(m_data);
			if(!unit.IsValid()) {
				g_network.RequestResync(RESYNC_INVALID_UNIT);
				break;
			}

			Army oldArmy(m_data2);
			Army newArmy(m_data3);
			Assert(unit.GetArmy() == oldArmy || unit.GetArmy() == newArmy);
			if(unit.GetArmy() != oldArmy && unit.GetArmy() != newArmy) {


				g_network.RequestResync(RESYNC_INVALID_POP);
			}
			if(unit.GetArmy() != newArmy) {
				if(!newArmy.IsValid()) {
					g_network.RequestResync(RESYNC_INVALID_ARMY_OTHER);
				} else {
					unit.ChangeArmy(newArmy, CAUSE_NEW_ARMY_NETWORK);
				}
			}
			break;
		}
		case NET_INFO_CODE_ACHIEVEMENTS:
		{
			DPRINTF(k_DBG_NET, ("Server: Achievements changed"));
			uint64 built = (uint64)m_data | (((uint64)m_data2) << 32);
			g_theAchievementTracker->SetData(built);
			break;
		}
		case NET_INFO_CODE_BUILT_WONDERS:
		{
			DPRINTF(k_DBG_NET, ("Server: Built wonders changed"));
			uint64 built = (uint64)m_data | (((uint64)m_data2) << 32);
			g_theWonderTracker->SetBuiltWonders(built);
			break;
		}
		case NET_INFO_CODE_BUILD_WONDER:
		{
			DPRINTF(k_DBG_NET, ("Server: City %lx is building wonder %d\n"));
			Unit city(m_data);
			Assert(g_theUnitPool->IsValid(city));
			if(g_theUnitPool->IsValid(city)) {
				if(g_player[city.GetOwner()]) {
					g_player[city.GetOwner()]->BuildWonder(m_data2, city);
				}
			}
			break;
		}
		case NET_INFO_CODE_SEND_TRADE_BID:
		{
			DPRINTF(k_DBG_NET, ("server: Trade bid from %d, fc: %lx, res: %d, tc: %lx, price: %d",
								m_data, m_data2, m_data3, m_data4, m_data5));
			Assert(g_player[m_data]);
			if(g_player[m_data]) {
				Unit fromCity(m_data2);
				Unit toCity(m_data4);
				if(!fromCity.IsValid() || !toCity.IsValid()) {
					g_network.RequestResync(RESYNC_INVALID_UNIT);
				} else {
					g_player[m_data]->SendTradeBid(fromCity, m_data3, toCity, m_data5);
				}
			}
			break;
		}
		case NET_INFO_CODE_SEND_SLAVE_TO:
		{
			DPRINTF(k_DBG_NET, ("Server: Send a slave from %lx to %lx\n",
								m_data, m_data2));
			Unit fromCity(m_data);
			Unit toCity(m_data2);
			Assert(g_theUnitPool->IsValid(fromCity));
			Assert(g_theUnitPool->IsValid(toCity));
			if(g_theUnitPool->IsValid(fromCity) &&
			   g_theUnitPool->IsValid(toCity)) {
				fromCity.SendSlaveTo(toCity);
			}
			break;
		}
		case NET_INFO_CODE_CLEAR_ORDERS_EXCEPT_GROUP:
		{
			Assert(FALSE);
			break;
		}
		case NET_INFO_CODE_BUILD_CAPITALIZATION:
		{
			DPRINTF(k_DBG_NET, ("Server: city %lx building capitalization\n", m_data));
			Unit city(m_data);
			Assert(g_theUnitPool->IsValid(city));
			if(g_theUnitPool->IsValid(city)) {
				city.BuildCapitalization();
			}
			break;
		}
		case NET_INFO_CODE_BUILD_INFRASTRUCTURE:
		{
			DPRINTF(k_DBG_NET, ("Server: city %lx building infrastructure\n", m_data));
			Unit city(m_data);
			Assert(g_theUnitPool->IsValid(city));
			if(g_theUnitPool->IsValid(city)) {
				city.BuildInfrastructure();
			}
			break;
		}
		case NET_INFO_CODE_BUILD_END_GAME:
		{
			DPRINTF(k_DBG_NET, ("Server: city %lx building end game %d\n", m_data, m_data2));
			Unit city(m_data);
			Assert(g_theUnitPool->IsValid(city));
			if(g_theUnitPool->IsValid(city)) {
				city.BuildEndGame(m_data2);
			}
			break;
		}
		case NET_INFO_CODE_PROGRESS:
		{
			g_network.SetProgress(m_data);
			break;
		}
		case NET_INFO_CODE_TAKE_TRADE_OFFER:
		{
			DPRINTF(k_DBG_NET, ("Server: Trade offer %lx taken by %d\n",
								m_data2, m_data));
			if(g_player[m_data]) {
				TradeOffer offer(m_data2);
				Assert(g_theTradeOfferPool->IsValid(offer));
				if(g_theTradeOfferPool->IsValid(offer)) {
					Unit unit1(m_data3);
					Unit unit2(m_data4);
					g_player[m_data]->AcceptTradeOffer(offer, unit1, unit2);
				}
			}
			break;
		}
		case NET_INFO_CODE_SOLD_BUILDING:
		{
			DPRINTF(k_DBG_NET, ("City %lx sold building %d\n",
								m_data, m_data2));
			Unit city(m_data);
			if(g_theUnitPool->IsValid(city)) {
				city.GetData()->GetCityData()->SellBuilding(m_data2);
			}
			break;
		}
		case NET_INFO_CODE_KILL_AGREEMENT:
		{
			DPRINTF(k_DBG_NET, ("Agreement %lx killed\n", m_data));
			Agreement agreement(m_data);

			if(g_theAgreementPool->IsValid(agreement)) {
				agreement.Kill();
			}
			break;
		}
		case NET_INFO_CODE_BREAK_ALLIANCE:
			DPRINTF(k_DBG_NET, ("Break alliances between %d and %d\n",
								m_data, m_data2));
			if(g_player[m_data]) {
				g_player[m_data]->BreakAlliance(m_data2);
			}
			break;
		case NET_INFO_CODE_BREAK_CEASE_FIRE:
			DPRINTF(k_DBG_NET, ("Break cease fires between %d and %d\n",
								m_data, m_data2));
			if(g_player[m_data]) {
				g_player[m_data]->BreakCeaseFire(m_data2, m_data3 != 0);
			}
			break;

		case NET_INFO_CODE_CLEAR_QUEUE:
		{
			DPRINTF(k_DBG_NET, ("Build queue for city %lx cleared\n",
								m_data));
			Unit city(m_data);
			if(g_theUnitPool->IsValid(city)) {
				city.GetData()->GetCityData()->GetBuildQueue()->Clear(TRUE);
			}
			break;
		}
		case NET_INFO_CODE_RESYNC:
		{
			g_network.StartResync();
			break;
		}
		case NET_INFO_CODE_WONDER_BUILT:
		{
			DPRINTF(k_DBG_NET, ("Wonder %d built at %lx\n", m_data, m_data2));
			Unit city(m_data2);
			if(g_theUnitPool->IsValid(city)) {
				CityData *cd = city.GetData()->GetCityData();
				cd->SetWonders(cd->GetBuiltWonders() | ((uint64)1 << (uint64)m_data));
				wonderutil_AddBuilt(m_data);
				if(g_player[cd->GetOwner()]) {
					g_player[cd->GetOwner()]->AddWonder(m_data, city);
				}
				cd->GetBuildQueue()->SendMsgWonderComplete(cd, m_data);
			}
			break;
		}
		case NET_INFO_CODE_SEND_OFFER_ACCEPT_MESSAGE:
		{
			DPRINTF(k_DBG_NET, ("Send offer accept message, Src: %lx, dest: %lx, res: %d\n",
								m_data, m_data2, m_data3));
			Unit src(m_data), dest(m_data2);
			if(g_theUnitPool->IsValid(src) && g_theUnitPool->IsValid(dest)) {
				SlicObject *so = new SlicObject("363TradeOfferAccepted");
				so->AddRecipient(dest.GetOwner());
				so->AddCivilisation(src.GetOwner());
				so->AddCity(src);
				so->AddCity(dest);
				so->AddGood(m_data3);
				g_slicEngine->Execute(so);
			}
			break;
		}
		case NET_INFO_CODE_ATTACH_ROBOT:
		{
			DPRINTF(k_DBG_NET, ("Player %d is a robot\n", m_data));
			if(g_player[m_data])
				g_player[m_data]->m_playerType = PLAYER_TYPE_ROBOT;
			break;
		}
		case NET_INFO_CODE_DETACH_ROBOT:
		{
			DPRINTF(k_DBG_NET, ("Player %d is a non-robot\n", m_data));
			if (g_player[m_data]) {
				if (static_cast<sint32>(m_data) == g_network.GetPlayerIndex()) {
					g_player[m_data]->m_playerType = PLAYER_TYPE_HUMAN;
				} else {
					g_player[m_data]->m_playerType = PLAYER_TYPE_NETWORK;
				}
			}
			break;
		}
		case NET_INFO_CODE_SET_DIP_STATE:
		{
			DPRINTF(k_DBG_NET, ("Player %d's dip state towards %d is %d\n",
								m_data, m_data2, m_data3));
			if(g_player[m_data]) {
				g_player[m_data]->SetDiplomaticState(m_data2,
													 (DIPLOMATIC_STATE)m_data3);
			}
			break;
		}
		case NET_INFO_CODE_RESET_UNIT_OWNER:
		{
			DPRINTF(k_DBG_NET, ("Reset unit %lx to owner %d, cause= %d\n",
								m_data, m_data2, m_data3));
			Unit unit(m_data);
			if(g_theUnitPool->IsValid(m_data)) {
				if(unit.GetOwner() != (PLAYER_INDEX)m_data2) {
					unit.ResetUnitOwner(m_data2, (CAUSE_REMOVE_ARMY)m_data3);
				}
			}
			break;
		}
		case NET_INFO_CODE_CELL_OWNER:
		{
			MapPoint pos;
			g_network.UnpackedPos(m_data, pos);
			DPRINTF(k_DBG_NET, ("Cell (%d,%d)'s new owner is %d\n",
								pos.x, pos.y, m_data2));
			g_theWorld->GetCell(pos)->SetOwner(m_data2);
			break;
		}
		case NET_INFO_CODE_NAK_ENACT:
		{
			DPRINTF(k_DBG_NET, ("Enact of %lx NAKed\n", m_data));
			DiplomaticRequest req(m_data);
			g_network.RemoveEnact(req);
			break;
		}
		case NET_INFO_CODE_REMOVE_ILLEGAL_ITEMS:
		{
			DPRINTF(k_DBG_NET, ("Server: remove illegal items at city %lx\n",
								m_data));
			Unit city(m_data);
			if(g_theUnitPool->IsValid(city)) {
				city.GetData()->GetCityData()->GetBuildQueue()->RemoveIllegalItems();
			}
			g_network.SendAction(new NetAction(NET_ACTION_ACK_REMOVE_ILLEGAL,
											   m_data));
			break;
		}
		case NET_INFO_CODE_MAKE_UNIT_PERMANENT:
		{
			DPRINTF(k_DBG_NET, ("Server: Unit %lx is now a real unit\n",
								m_data));
			Unit unit(m_data);
			if(g_theUnitPool->IsValid(unit)) {
				if(!g_player[unit.GetOwner()]) {
					g_network.RequestResync(RESYNC_BAD_PLAYER);
					break;
				}
				unit.SetTempSlaveUnit(FALSE);
				unit.AddUnitVision();
				MapPoint pos;
				unit.GetPos(pos);

				g_theWorld->InsertUnit(pos, unit);
				g_player[unit.GetOwner()]->InsertUnitReference(unit,
															   CAUSE_NEW_ARMY_UPRISING,
															   Unit());
			}
			break;
		}
		case NET_INFO_CODE_CLEAR_QUEUE_EXCEPT_HEAD:
		{
			DPRINTF(k_DBG_NET, ("Build queue for city %lx cleared\n",
								m_data));
			Unit city(m_data);
			if(g_theUnitPool->IsValid(city)) {
				city.GetData()->GetCityData()->GetBuildQueue()->ClearAllButHead(TRUE);
			}
			break;
		}
		case NET_INFO_CODE_REMOVE_DEAD_PLAYERS:
		{
			DPRINTF(k_DBG_NET, ("RemoveDeadPlayers\n"));
			Player::RemoveDeadPlayers();
			break;
		}
		case NET_INFO_CODE_WONDER_STARTED:
		{
			DPRINTF(k_DBG_NET, ("Wonder %d started by player %d\n",
								m_data2, m_data));
			if(g_player[m_data]) {
				SlicObject *so;
				so = new SlicObject("44WonderStarted");
				so->AddCivilisation(m_data);
				so->AddWonder(m_data2);
				so->AddRecipient(g_network.GetPlayerIndex());
				g_slicEngine->Execute(so);
			}
			break;
		}
		case NET_INFO_CODE_WONDER_OBSOLETE:
		{
			DPRINTF(k_DBG_NET, ("Wonder %d obsoleted by advance %d\n",
								m_data2, m_data));
			SlicObject *so = new SlicObject("097aWonderObsolete");
			so->AddRecipient(g_network.GetPlayerIndex());
			so->AddAdvance(m_data);
			so->AddWonder(m_data2);
			g_slicEngine->Execute(so);
			break;
		}
		case NET_INFO_CODE_BEGIN_TURN_ENEMY_UNITS:
		{
			DPRINTF(k_DBG_NET, ("BeginTurnEnemyUnits for Player %d\n",
								m_data));
			if(g_player && g_player[m_data]) {
				g_player[m_data]->BeginTurnEnemyUnits();
			}
			break;
		}
		case NET_INFO_CODE_WONDER_ALMOST_DONE:
		{
			DPRINTF(k_DBG_NET, ("Wonder %d almost finished by %d at %lx\n",
								m_data2, m_data, m_data3));
			if(g_theUnitPool->IsValid(m_data3)) {
				SlicObject *so = new SlicObject("45WonderAlmostFinished");
				so->AddAllRecipientsBut(m_data);
				so->AddWonder(m_data2);
				so->AddCivilisation(m_data);
				so->AddCity(Unit(m_data3));
				g_slicEngine->Execute(so);
			}
			break;
		}
		case NET_INFO_CODE_WONDER_STOPPED:
		{
			DPRINTF(k_DBG_NET, ("Wonder %d stopped by %d\n", m_data2, m_data));
			SlicObject *so = new SlicObject("44aWonderStopped");
			so->AddCivilisation(m_data);
			so->AddWonder(m_data2);
			so->AddAllRecipientsBut(m_data);
			g_slicEngine->Execute(so);
			break;
		}
		case NET_INFO_CODE_ALL_PLAYERS_READY:
			DPRINTF(k_DBG_NET, ("All players ready\n"));
			g_network.SetAllPlayersReady();
			break;
		case NET_INFO_CODE_SET_FRANCHISE_PRODUCTION:
		{
			DPRINTF(k_DBG_NET, ("Player %d's production from franchises is %d\n",
								m_data, m_data2));
			if(g_player[m_data]) {
				g_player[m_data]->m_productionFromFranchises = m_data2;
			}
			break;
		}
		case NET_INFO_CODE_REVOLT_NOTICES:
		{
			DPRINTF(k_DBG_NET, ("Player %d's city %lx revolted to join %d (egalitarians = %d)\n",
								m_data, m_data3, m_data2, m_data4));
			Unit city(m_data3);
			if(g_theUnitPool->IsValid(city)) {
				SlicObject *so = new SlicObject("010NewCiv");
				so->AddAllRecipients();
				so->AddCivilisation(m_data);
				so->AddCivilisation(m_data2);
				so->AddCity(city);
				g_slicEngine->Execute(so) ;

				if (m_data4) {
					so = new SlicObject("011CityJoinedYourCiv");
					so->AddRecipient(m_data2);
					so->AddCivilisation(m_data);
					so->AddCivilisation(m_data2);
					so->AddCity(city);
					g_slicEngine->Execute(so) ;
				}
			}
			break;
		}
		case NET_INFO_CODE_ADD_UNSEEN:
		{
			MapPoint p;
			g_network.UnpackedPos(m_data2, p);
			if(g_player[m_data])
			{
				g_player[m_data]->m_vision->AddUnseen(p);
			}
			break;
		}
		case NET_INFO_CODE_OFFER_REJECTED_MESSAGE:
		{
			if(g_player[m_data] && g_player[m_data2]) {
				SlicObject *so = new SlicObject("91OfferRejected");
				so->AddRecipient(m_data);
				so->AddCivilisation(m_data2);
				g_slicEngine->Execute(so);
			}
			break;
		}
		case NET_INFO_CODE_WON_END_GAME:
		{
			DPRINTF(k_DBG_NET, ("%d won the alien end game.\n", m_data));

			SlicObject *so = new SlicObject("309EndGameWon");
			so->AddAllRecipientsBut(m_data);
			so->AddCivilisation(m_data);
			g_slicEngine->Execute(so);

			if(g_player[m_data]) {
				g_player[m_data]->GameOver(GAME_OVER_WON_SCIENCE, -1);
			}
			uint32 i;
			for(i = 0; i < k_MAX_PLAYERS; i++) {
				if(g_player[i] && i != m_data) {
					g_player[i]->GameOver(GAME_OVER_LOST_SCIENCE, m_data);
				}
			}

			break;
		}
		case NET_INFO_CODE_RECOVERED_PROBE:
		{
			DPRINTF(k_DBG_NET, ("%d recovered the probe\n", m_data));
			if(g_player[m_data]) {
				Unit city(m_data2);

				g_player[m_data]->RecoveredProbe(city);
			}
			break;
		}
		case NET_INFO_CODE_OTHER_CIV_LAB_MSG:
		{
			SlicObject *so = new SlicObject("302EndGameOtherCivBuiltLab");
			so->AddAllRecipientsBut(m_data);
			so->AddCivilisation(m_data);
			g_slicEngine->Execute(so);
			break;
		}
		case NET_INFO_CODE_OTHER_CIV_SEQUENCE_MSG:
		{
			SlicObject *so = new SlicObject("303EndGameOtherCivStartedSequence");
			so->AddAllRecipientsBut(m_data);
			so->AddCivilisation(m_data);
			g_slicEngine->Execute(so);
			break;
		}
		case NET_INFO_CODE_ALIEN_ALMOST_DONE_OTHERS_MSG:
		{
			SlicObject *so = new SlicObject("054AlienAlmostDoneOthers");
			so->AddAllRecipientsBut(m_data);
			so->AddCivilisation(m_data);
			g_slicEngine->Execute(so);
			break;
		}
		case NET_INFO_CODE_ALIEN_SCRAPPED_OWNER:
		{
			SlicObject *so = new SlicObject("058AlienScrappedOwner");
			so->AddAllRecipientsBut(m_data);
			so->AddCivilisation(m_data);
			g_slicEngine->Execute(so);
			break;
		}
		case NET_INFO_CODE_CATACLYSM_OTHER:
		{
			SlicObject *so = new SlicObject("301EndGameCataclysmOtherCiv");
			so->AddAllRecipientsBut(m_data);
			so->AddCivilisation(m_data);
			g_slicEngine->Execute(so);
			break;
		}
		case NET_INFO_CODE_GAME_OVER_OUT_OF_TIME:
		{
			DPRINTF(k_DBG_NET, ("Game over out of time, %d won\n", m_data));
			uint32 i;
			for(i = 0; i < k_MAX_PLAYERS; i++) {
				if(!g_player[i]) continue;
				if(i == m_data) {
					g_player[i]->GameOver(GAME_OVER_WON_OUT_OF_TIME, -1);
				} else {
					g_player[i]->GameOver(GAME_OVER_LOST_OUT_OF_TIME, -1);
				}
			}
			break;
		}
		case NET_INFO_CODE_ACTIVATE_SPACE_BUTTON:
		{
			DPRINTF(k_DBG_NET, ("Space Button active for %d\n", m_data));
			if(g_player[m_data]) {
				player_ActivateSpaceButton(m_data);
			}
			break;
		}
		case NET_INFO_CODE_TIMES_ALMOST_UP:
		{
			DPRINTF(k_DBG_NET, ("Time's almost up!\n"));
			if(g_turn) {
				g_turn->SendMsgEndOfGameEarlyWarning();
			}
			break;
		}
		case NET_INFO_CODE_VIOLATE_AGREEMENT:
		{
			DPRINTF(k_DBG_NET, ("Agreement %lx violated by %d\n", m_data, m_data2));
			Agreement ag(m_data);
			if(g_theAgreementPool->IsValid(ag)) {
				ag.AccessData()->RecipientIsViolating((PLAYER_INDEX)m_data2, TRUE);
			}
			break;
		}
		case NET_INFO_CODE_SET_MAYOR:
		{
			DPRINTF(k_DBG_NET, ("set city 0x%lx mayor to %d,%d\n", m_data, m_data2, m_data3));
			Unit city(m_data);
			if(!city.IsValid()) {
				g_network.RequestResync(RESYNC_INVALID_UNIT);
			} else {
				city.CD()->SetUseGovernor(m_data3 != 0);
				city.CD()->SetBuildListSequenceIndex(m_data2);
			}
			break;
		}
		case NET_INFO_CODE_FINISH_AI_TURN:
		{
			DPRINTF(k_DBG_NET, ("Server says run AIFinishBeginTurn for player %d\n", m_data));
			if(g_network.IsLocalPlayer(m_data)) {





				g_gevManager->AddEvent(GEV_INSERT_Tail,
									   GEV_AIFinishBeginTurn,
									   GEA_Player, m_data,
									   GEA_End);

			}
			break;
		}
		case NET_INFO_CODE_CITIES_DONE:
		{
			DPRINTF(k_DBG_NET, ("Server says all initial city states sent for player %d\n", m_data));
			if(g_network.IsLocalPlayer(m_data)) {
				if(g_player[m_data]->IsRobot()) {
					CtpAi::NetworkClientBeginTurn(m_data);
				}
				else
				{
					g_gevManager->AddEvent(GEV_INSERT_Tail,
										   GEV_FinishBuildPhase, // GEV_StartMovePhase,
										   GEA_Player, m_data,
										   GEA_End
										  );
				}
				g_network.SetSensitiveUIBlocked(false);
			}
			MainControlPanel::SelectedCity();
			if (static_cast<sint32>(m_data) == g_network.GetPlayerIndex())
            {
				network_VerifyGameData();
			}
			break;
		}
		case NET_INFO_CODE_SET_ROUND:
		{
			DPRINTF(k_DBG_NET, ("Server says it's round %d for player %d\n", m_data2, m_data));
			if(g_player[m_data]) {
				g_player[m_data]->m_current_round = m_data2;
			}
			break;
		}
		case NET_INFO_CODE_BEGIN_SCHEDULER:
		{
			DPRINTF(k_DBG_NET, ("Server says ok to begin scheduler for player %d now\n", m_data));
			if(m_data >= 0 && m_data < k_MAX_PLAYERS && g_player[m_data] && g_network.IsLocalPlayer(m_data)) {
				g_director->AddBeginScheduler(m_data);
			}
			break;
		}
		case NET_INFO_CODE_REMOTE_GROUP:
		{

			DPRINTF(k_DBG_NET, ("Server says group unit %lx into army %lx\n", m_data2, m_data));
			if(!g_theArmyPool->IsValid(m_data)) {
				g_network.RequestResync(RESYNC_INVALID_ARMY_OTHER);
				break;
			}

			if(!g_theUnitPool->IsValid(m_data2)) {
				g_network.RequestResync(RESYNC_INVALID_UNIT);
				break;
			}

			Army army(m_data);
			Unit unit(m_data2);
			army->GroupUnit(unit);

			if(army.GetOwner() == g_selected_item->GetVisiblePlayer()) {

				ArmyManagerWindow::NotifyRemoteGroupComplete(army);
			}

			break;
		}
		case NET_INFO_CODE_REMOTE_UNGROUP:
		{
			DPRINTF(k_DBG_NET, ("Server says ungrouping of Army %lx done (owner=%d)\n", m_data, m_data2));

			if (static_cast<PLAYER_INDEX>(m_data2) == g_selected_item->GetVisiblePlayer())
            {
			    Army army(m_data);
				if (army.IsValid())
                {
					ArmyManagerWindow::NotifyRemoteGroupComplete(army);
				}
                else
                {
					ArmyManagerWindow::NotifySelection();
				}
			}
			break;
		}
		case NET_INFO_CODE_REENTER:
		{
			DPRINTF(k_DBG_NET, ("Reentry for army %lx\n", m_data));
			if(!g_theArmyPool->IsValid(m_data)) {
				g_network.RequestResync(RESYNC_INVALID_ARMY_OTHER);
			} else {
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_Reentry,
									   GEA_Army, m_data,
									   GEA_End);
			}
			break;
		}
		case NET_INFO_CODE_PLAYER_TRADE_DATA:
		{
			DPRINTF(k_DBG_NET, ("Player %d trade points: %d/%d\n", m_data, m_data2, m_data3));
			if(!g_player[m_data]) {
				g_network.RequestResync(RESYNC_BAD_PLAYER);
				break;
			}

			g_player[m_data]->m_usedTradeTransportPoints = m_data2;
			g_player[m_data]->m_tradeTransportPoints = m_data3;
			TradeManager::Notify();
			break;
		}
		case NET_INFO_CODE_CREATED_WONDER:
		{
			DPRINTF(k_DBG_NET, ("Server says city %lx created wonder %d\n", m_data, m_data2));
			Unit city(m_data);
			if(!city.IsValid()) {
				g_network.RequestResync(RESYNC_INVALID_UNIT);
			} else {
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CreateWonder,
									   GEA_City, m_data,
									   GEA_Int, m_data2,
									   GEA_End);
			}
			break;
		}
		case NET_INFO_CODE_GAME_OVER:
		{
			DPRINTF(k_DBG_NET, ("Server says game over for player %d, reason %d (%d)\n", m_data2, m_data, m_data3));
			if(m_data >= 0 && m_data < k_MAX_PLAYERS) {
				if(g_player[m_data2]) {
					g_player[m_data2]->GameOver((GAME_OVER)m_data, m_data3);
				}
			}
			break;
		}
		case NET_INFO_CODE_SET_EMBASSIES:
		{
			DPRINTF(k_DBG_NET, ("Server says embassies for player %d is %lx\n", m_data, m_data2));
			if(g_player[m_data]) {
				g_player[m_data]->m_embassies = m_data2;
			}
			break;
		}

		// propagate the accomplishment of a feat
		case NET_INFO_CODE_ACCOMPLISHED_FEAT:
			DPRINTF(k_DBG_NET,
				    ("Server says player %d accomplished feat %d in turn %d",
					 m_data2, m_data, m_data3
					)
				   );
			if (g_player[m_data2])
			{
				g_featTracker->AddFeat(m_data, m_data2, m_data3);
			}
			break;

		// propagate the immobility of the settler created when disbanding a city
		case NET_INFO_CODE_DISBANDED_CITY_SETTLER:
		{
			DPRINTF(k_DBG_NET,
					("Server says unit %lx is a disbanded city settler\n", m_data)
				   );
			Unit unit(m_data);
			if (unit.IsValid())
			{
				unit.ClearFlag(k_UDF_FIRST_MOVE);
				unit.SetMovementPoints(0.0);
			}
			else
			{
				g_network.RequestResync(RESYNC_INVALID_UNIT);
			}
			break;
		}

		// propagate PW each turn update
		case NET_INFO_CODE_MATERIALS:
			DPRINTF(k_DBG_NET, ("Net: Setting player %d's materials to %d\n",
								m_data, m_data2));
			if(g_player[m_data]) {
				g_player[m_data]->m_materialPool->SetLevel((sint32)m_data2);
			}
			break;

		default:
			Assert(FALSE);
			break;
	}
}
