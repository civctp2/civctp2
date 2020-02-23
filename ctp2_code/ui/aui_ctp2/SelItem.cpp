//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handles stuff about selected items.
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
// _DEBUG
// - Generates debug information when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Fixed archieve constructor to make sure that the
//   correct stop player is set.
// - #01 Standardization of city selection and focus handling
//   (L. Hirth 6/2004)
// - Entrenching units are treated like Entrenched units. (Oct 16th 2005 Martin Gühmann)
// - Added select city instead of army option. (Oct 16th 2005 Martin Gühmann)
// - Added option to avoid an end turn if there are cities with empty build
//   queues. (Oct. 22nd 2005 Martin Gühmann) Doesn't really work.
// - Added option to allow end turn if the game runs in the background,
//   useful for automatic AI testing. (Oct. 22nd 2005 Martin Gühmann)
// - Added debug pathing for the city astar. (17-Jan-2008 Martin Gühmann)
// - If the keyboard is used for moving the map is only centered if the auto
//   center option is active. (23-Feb-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3errors.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "c3ui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "c3_static.h"
#include "c3_listitem.h"
#include "helptile.h"

#include "aui_tipwindow.h"

#include "player.h"
#include "civarchive.h"
#include "World.h"
#include "director.h"
#include "SelItem.h"
#include "UnitAstar.h"
#include "TurnCnt.h"
#include "AgreementPool.h"
#include "pollution.h"
#include "DiplomaticRequestData.h"
#include "DiplomaticRequestPool.h"
#include "Cell.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "TradeDynArr.h"
#include "citydata.h"
#include "UnitData.h"
#include "grabitem.h"
#include "TradeRouteData.h"
#include "TradeRoute.h"
#include "UnitPool.h"
#include "ArmyPool.h"
#include "Army.h"
#include "cellunitlist.h"
#include "Order.h"
#include "battleorderbox.h"

#include "AICause.h"


#include "pixelutils.h"
#include "colorset.h"
#include "maputils.h"
#include "tiledmap.h"

#include "network.h"
#include "net_action.h"

#include "aui_mouse.h"
#include "controlpanelwindow.h"

#include "workwindow.h"
#include "workwin.h"
#include "resourcemap.h"
#include "radarmap.h"

#include "soundmanager.h"
#include "profileDB.h"

#include "SlicEngine.h"
#include "SlicObject.h"
#include "QuickSlic.h"

#include "sci_advancescreen.h"

#include "message.h"

#include "GameEventManager.h"
#include "UnitRecord.h"

#include "ResourceRecord.h"
#include "MoveFlags.h"

#include "armymanagerwindow.h"
#include "ArmyData.h"

#include "scenarioeditor.h"
#include "OrderRecord.h"

#include "MainControlPanel.h"

#include "AttractWindow.h"
#include "gamesounds.h"
#include "soundmanager.h"
#include "CityAstar.h"
#include "agent.h"

extern ControlPanelWindow       *g_controlPanel;
extern WorkWindow               *g_workWindow;
extern UnitAstar                *g_theUnitAstar;
extern Pollution                *g_thePollution ;

sint32                           g_tradeSelectedState = 0;
extern GrabItem                 *g_grabbedItem;
extern C3UI                     *g_c3ui;

extern ResourceMap              *g_resourceMap;
extern RadarMap                 *g_radarMap;

extern SoundManager             *g_soundManager;
extern CityAstar                 g_city_astar;

#define k_UNIT_SELECT_IS_FIRST  0x00000001

bool CanAutoSelect(const Army &army)
{
	if (!g_theArmyPool->IsValid(army)) return false;
	if(army.Num() < 1) return false;

	if(!army.IsAsleep() &&
	   !army.IsEntrenched() &&
	   !army.IsEntrenching() &&
	   !army.HasLeftMap() &&
	   ((army.NumOrders() < 1) ||
		(army.GetOrder(0)->m_round != g_turn->GetRound() &&
		 !army.ExecutedThisTurn())) &&
	   army.CanMove() &&
	   !army.TurnOver()) {
		return true;
	}
	return false;
}

bool HasContinuingOrder(const Army &army)
{
	if (!g_theArmyPool->IsValid(army)) return false;
	if(army.Num() < 1) return false;

	if(army.IsAsleep() ||
	   army.IsEntrenched() ||
	   army.IsEntrenching() ||
	   army.HasLeftMap()) {
		return true;
	}
	return false;
}

SelectedItem::SelectedItem(sint32 nPlayers)
{
	sint32 i, j;

	m_current_player = 1;

	if(g_player)
	{
		for(i = 0; i < k_MAX_PLAYERS; i++)
		{
			if(g_player[i])
			{
				for(j = i + 1; j < k_MAX_PLAYERS; j++)
				{
					if(g_player[j]) {
						m_next_player[i] = j;
						break;
					}
				}

				if(j >= k_MAX_PLAYERS)
				{
					m_next_player[i] = PLAYER_INDEX_VANDALS;
					break;
				}
			}
		}
	}
	else
	{
		Assert(1 < nPlayers);

		for (i = 0; i < (nPlayers-1); i++)
		{
			m_next_player[i] = PLAYER_INDEX(i+1);
		}

		for ( ; i < k_MAX_PLAYERS; i++)
		{
			m_next_player[i] = PLAYER_INDEX_INVALID;
		}

		m_next_player[PLAYER_INDEX(nPlayers-1)] = PLAYER_INDEX_VANDALS;
	}

	for (i=0; i<k_MAX_PLAYERS; i++)
	{
		m_select_state[i]       = SELECT_TYPE_NONE;
		m_is_at_start[i]        = TRUE;
		m_selected_army[i].m_id = (0);
		m_selected_city[i].m_id = (0);
		m_remote_owner[i]       = -1;
	}

	for (i = 0; i < k_MAX_PLAYERS; i++)
	{
		m_select_pos[i] = MapPoint(0,0);
	}

	Init();
}

SelectedItem::SelectedItem(CivArchive &archive)
{
	for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
	{
		m_select_pos[i] = MapPoint(0,0); // Find something better, like the first unit or the capital
	}

	Serialize(archive);

	SetPlayerOnScreen(m_current_player);

	//Added by Martin Gühmann
	//To make sure that save games saved with other players than
	//player one can be continued without using the cheat editor.
	NewTurnCount::SetStopPlayer(m_current_player);
}

SelectedItem::~SelectedItem()
{
	delete m_good_path;
}

void SelectedItem::Serialize(CivArchive &archive)
{
	CHECKSERIALIZE

	if (archive.IsStoring())
	{
		for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
		{
			archive.PutSINT32(m_select_state[i]);
			m_selected_army[i].Serialize(archive);
			m_selected_city[i].Serialize(archive);
			archive.PutSINT32(m_is_at_start[i]);
			archive.PutSINT32(m_next_player[i]);
			archive.PutSINT32(m_remote_owner[i]);
			m_select_pos[i].Serialize(archive);
		}

		archive.PutSINT32(m_current_player);
	}
	else
	{
		for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
		{
			m_select_state[i] = SELECT_TYPE(archive.GetSINT32());
			m_selected_army[i].Serialize(archive);
			m_selected_city[i].Serialize(archive);
			m_is_at_start[i] = archive.GetSINT32();
			m_next_player[i] = PLAYER_INDEX (archive.GetSINT32());
			m_remote_owner[i] = archive.GetSINT32();
			m_select_pos[i].Serialize(archive);
		}

		m_current_player = PLAYER_INDEX(archive.GetSINT32());
		Init();
	}
}

void SelectedItem::Init()
{
	m_is_pathing                = false;
	m_cur_mouse_tile.Set(0,0);
	m_good_path                 = NULL;
	m_bad_path.Clear();
	m_is_broken_path            = false;
	m_waypoints.Clear();
	m_player_on_screen          = -1;
	m_gotClickSinceLastAutoEnd  = false;
	m_selected_something_since_director_select = false;
	m_ignoreCitySelect          = false;  // completely unused now #01
	m_isDragging                = false;
	m_gotClick                  = false;

	SetupClickFunctions();

	m_moveOrder                 = NULL;

	m_transportOrder            = NULL;

	Assert(g_theOrderDB);

	for(sint32 i = 0; i < g_theOrderDB->NumRecords(); i++)
	{
		if(g_theOrderDB->Get(i)->GetTargetPretestMovePosition())
		{
			m_moveOrder = (OrderRecord *)g_theOrderDB->Get(i);
		}
		else if(g_theOrderDB->Get(i)->GetTargetPretestTransport())
		{
			m_transportOrder = (OrderRecord *)g_theOrderDB->Get(i);
		}
	}
}

bool SelectedItem::IsLocalArmy() const
{
	return m_select_state[GetVisiblePlayer()] == SELECT_TYPE_LOCAL_ARMY;
}

bool SelectedItem::IsLocalCity() const
{
	return m_select_state[GetVisiblePlayer()] == SELECT_TYPE_LOCAL_CITY;
}

void SelectedItem::GetTopCurItem
(
    PLAYER_INDEX &s_player,
    ID           &s_item,
    SELECT_TYPE  &s_state
)
{
	s_player = GetVisiblePlayer();
	s_state = m_select_state[s_player];

	switch (s_state)
	{
		case SELECT_TYPE_NONE:
		case SELECT_TYPE_REMOTE_ARMY:
			s_item = ID();
			break;
		case SELECT_TYPE_REMOTE_CITY:
			s_item = m_selected_city[s_player];
			s_player = m_remote_owner[s_player];
			Assert(s_item != ID());
			break;
		case SELECT_TYPE_LOCAL_ARMY:
		case SELECT_TYPE_LOCAL_ARMY_UNLOADING:
			if(m_selected_army[s_player].IsValid())
			{
				if(m_selected_army[s_player].Num() <= 0)
				{
					s_item = ID();
					s_state = SELECT_TYPE_NONE;
				}
				else
				{
					s_item = m_selected_army[s_player];
					Assert(s_item.m_id != (0));
				}
			}
			else
			{
				Assert(false);
				s_state = SELECT_TYPE_NONE;
				s_item = ID();
			}
			break;
		case SELECT_TYPE_LOCAL_CITY:
			s_item =  m_selected_city[s_player];
			Assert(s_item.m_id != 0);
			break;
		case SELECT_TYPE_TRADE_ROUTE:
			s_item = ID();
			break;
		case SELECT_TYPE_GOOD:
			s_item = ID();
			break;
		default:
			Assert(0);
			return;
	}
}

void SelectedItem::ClipCurrentItem()
{
	PLAYER_INDEX player = GetVisiblePlayer();
	switch(m_select_state[player])
	{
		case SELECT_TYPE_LOCAL_ARMY:
			if(!g_theArmyPool->IsValid(m_selected_army[player]))
			{
				m_select_state[player] = SELECT_TYPE_NONE;
			}
			break;
		case SELECT_TYPE_LOCAL_CITY:
			if(!g_theUnitPool->IsValid(m_selected_city[player]))
			{
				m_select_state[player] = SELECT_TYPE_NONE;
			}
			break;
		case SELECT_TYPE_NONE:
		default:
			break;
	}
}

void SelectedItem::NextItem()
{
	PLAYER_INDEX player = GetVisiblePlayer();
	sint32 curIndex;
	switch(m_select_state[player])
	{
		case SELECT_TYPE_LOCAL_ARMY:
			NextUnmovedUnit();
			break;
		case SELECT_TYPE_LOCAL_CITY:
			curIndex = g_player[player]->FindCityIndex(m_selected_city[player]);
			if(curIndex >= 0)
			{
				curIndex++;
				if(curIndex >= g_player[player]->m_all_cities->Num())
					curIndex = 0;
				m_selected_city[player] = g_player[player]->m_all_cities->Access(curIndex);

				MapPoint pos;
				m_selected_city[player].GetPos( pos );
				m_select_pos[player] = pos;
				if(IsAutoCenterOn())
				{
					if(!g_director->TileWillBeCompletelyVisible(pos.x, pos.y))
					{
						g_director->AddCenterMap(pos);
					}
				}
				if ( g_controlPanel )
				{
					g_selected_item->SetSelectCity(m_selected_city[player]);
				}

			}
			else
			{
				return;
			}
			break;
		case SELECT_TYPE_NONE:
			NextUnmovedUnit(false, true);
			return;
		default:
			return;
	}
}

void SelectedItem::NextUnmovedUnit(bool isFirst, bool manualNextUnit)
{
	PLAYER_INDEX player = GetVisiblePlayer();
	Player *p = g_player[player];
	if(!p)
		return;

	sint32 numArmies = p->m_all_armies->Num();
	bool found = false;
	MapPoint pos;
	Army selectArmy;

	bool done = false;

	do
	{
		bool foundUnmovedSelected = false;
		MapPoint unmovedSelectedPos;
		Army unmovedSelectedArmy;
		found = false;

		numArmies = p->m_all_armies->Num();

		for(sint32 i = 0; i < numArmies; i++)
		{
			if(CanAutoSelect(p->m_all_armies->Access(i)) &&
			   p->m_all_armies->Access(i).GetFirstMoveThisTurn())
			{
				if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY &&
				   m_selected_army[player] == p->m_all_armies->Access(i))
					continue;
				if(!m_select_cycle.IsPresent(p->m_all_armies->Access(i)))
				{
					found = true;
					selectArmy = p->m_all_armies->Access(i);

					p->m_all_armies->Access(i).GetPos(pos);
					if(g_director->TileWillBeCompletelyVisible(pos.x, pos.y)) {
						break;
					}
				}
				else
				{
					if(foundUnmovedSelected)
					{
						if(!g_director->TileWillBeCompletelyVisible(unmovedSelectedPos.x,
													  unmovedSelectedPos.y))
						{
							p->m_all_armies->Access(i).GetPos(pos);
							if(g_director->TileWillBeCompletelyVisible(pos.x, pos.y))
							{
								unmovedSelectedArmy = p->m_all_armies->Access(i);
								unmovedSelectedPos = pos;
							}
						}
					}
					else
					{
						foundUnmovedSelected = true;
						unmovedSelectedArmy = p->m_all_armies->Access(i);
						unmovedSelectedArmy.GetPos(unmovedSelectedPos);
					}
				}
			}
		}

		if(!found && foundUnmovedSelected)
		{
			found = true;
			selectArmy = unmovedSelectedArmy;
			ClearCycle();
		}

		bool foundMovedSelected = false;
		Army movedSelectedArmy;
		MapPoint movedSelectedPos;

		if(!found)
		{
			for(sint32 i = 0; i < numArmies; i++) {
				if(CanAutoSelect(p->m_all_armies->Access(i))) {
					if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY &&
					   m_selected_army[player] == p->m_all_armies->Access(i))
						continue;
					if(!m_select_cycle.IsPresent(p->m_all_armies->Access(i))) {
						found = true;
						selectArmy = p->m_all_armies->Access(i);
						p->m_all_armies->Access(i).GetPos(pos);
						if(g_director->TileWillBeCompletelyVisible(pos.x, pos.y)) {
							break;
						}
					}
					else
					{
						if(foundMovedSelected)
						{
							if(!g_director->TileWillBeCompletelyVisible(movedSelectedPos.x,
														  movedSelectedPos.y))
							{
								p->m_all_armies->Access(i).GetPos(pos);
								if(g_director->TileWillBeCompletelyVisible(pos.x, pos.y))
								{
									movedSelectedArmy = p->m_all_armies->Access(i);
									movedSelectedPos = pos;
								}
							}
						}
						else
						{
							foundMovedSelected = true;
							movedSelectedArmy = p->m_all_armies->Access(i);
							movedSelectedArmy.GetPos(movedSelectedPos);
						}
					}
				}
			}
		}

		if(!found && foundMovedSelected)
		{
			found = true;
			selectArmy = movedSelectedArmy;
			ClearCycle();
		}

		done = true;
		if(found)
		{
			Assert(selectArmy.IsValid());
			if(selectArmy.IsValid())
			{
				SetSelectUnit(selectArmy.GetTopVisibleUnit(GetVisiblePlayer()));
				if(IsAutoCenterOn())
				{
					selectArmy.GetPos(pos);

					if(!g_director->TileWillBeCompletelyVisible(pos.x, pos.y) ||
					   g_player[GetVisiblePlayer()]->m_first_city)
					{
						g_director->AddCenterMap(pos);
					}
				}
				if(selectArmy.NumOrders() > 0)
				{
//					g_director->IncrementPendingGameActions();
					g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BeginTurnExecute,
										   GEA_Army, selectArmy,
										   GEA_End);

					if(!g_theArmyPool->IsValid(selectArmy) ||
					   !CanAutoSelect(selectArmy)
					   || selectArmy.NumOrders() > 0)
					{
						Deselect(player);
						g_director->AddSelectUnit(isFirst ? k_UNIT_SELECT_IS_FIRST : 0);
						m_selected_something_since_director_select = false;
					}
				}
			}
			else
			{
				done = true;
			}
		}
	} while (!done);

	if(!found)
	{
		g_attractWindow->HighlightControl("ControlPanelWindow.ControlPanel.TurnButton");
		if(g_theProfileDB->GetEndTurnSound())
		{
			g_soundManager->AddGameSound(GAMESOUNDS_ENDTURN);
		}
	}

	if(!manualNextUnit)
	{
		MaybeAutoEndTurn(isFirst);
	}
}

double SelectedItem::UnitsDoneRatio(){
    Player *p = g_player[GetVisiblePlayer()];
    sint32 numUnits= p->m_all_armies->Num();
    sint32 unitsDone= numUnits;
    
    for(sint32 u = numUnits - 1; u >= 0; u--){
	if(CanAutoSelect(p->m_all_armies->Access(u)) && unitsDone){
	    unitsDone--;
	    }
	}
    
    return(double(unitsDone) / numUnits);
    }

double SelectedItem::UnitsBusyRatio(){
    Player *p = g_player[GetVisiblePlayer()];
    sint32 numUnits= p->m_all_armies->Num();
    sint32 unitsDone= numUnits;
    
    for(sint32 u = numUnits - 1; u >= 0; u--){
	if(!HasContinuingOrder(p->m_all_armies->Access(u)) && unitsDone){
	    unitsDone--;
	    }
	}
    
    return(double(unitsDone) / numUnits);
    }

void SelectedItem::MaybeAutoEndTurn(bool isFirst)
{
	sint32 player = GetVisiblePlayer();
	Player *p = g_player[player];

	if(!g_network.IsActive())
	{
		if(!g_player[GetCurPlayer()]->IsHuman())
			return;
	}
	else
	{
		if(!g_network.IsLocalPlayer(GetCurPlayer())
		|| !g_network.ReadyToStart()
		||  g_player[GetCurPlayer()]->IsRobot()
		){
			return;
		}
	}

	if(g_player[player]->m_endingTurn)
		return;

	if(player != m_current_player)
	{
		return;
	}

	if(g_theProfileDB->IsAutoTurnCycle() && !g_network.IsActive())
	{
		bool endTurn = true;

		if(endTurn)
		{
			if(p->m_first_city && isFirst)
				endTurn = false;
		}

		if(endTurn)
		{
			for(sint32 u = p->m_all_armies->Num() - 1; u >= 0 && endTurn; u--)
			{
				if(CanAutoSelect(p->m_all_armies->Access(u)))
				{
					endTurn = false;
				}
			}
		}

		if(endTurn)
		{
			for(sint32 i = p->m_all_cities->Num() - 1; i >= 0; i--)
			{
				Unit city = p->m_all_cities->Access(i);
				if(city.GetData()->GetCityData()->GetBuildQueue()->GetHead() == NULL
				&&!city.GetData()->GetCityData()->IsBuildingInfrastructure()
				&&!city.GetData()->GetCityData()->IsBuildingCapitalization()
				){
					SetSelectCity(p->m_all_cities->Access(i));
					if(IsAutoCenterOn())
					{
						MapPoint pos;
						p->m_all_cities->Access(i).GetPos(pos);
						if(!g_director->TileWillBeCompletelyVisible(pos.x, pos.y))
						{
							g_director->AddCenterMap(pos);
						}
					}

					endTurn = false;
					// TODO: Figure out why this doesn't work.
					// Hint MaybeAutoEndTurn, well that's obvious, but for now ...
					// But apart from that this code doesn't seem to do the
					// desired effect on the auto end turn code, either.
					if(!g_theProfileDB->GetValueByName("EndTurnWithEmptyBuildQueues")
					&& g_player[GetCurPlayer()]->IsHuman()
					){
						return;
					}
					break;
				}
			}
		}


		if(endTurn
		&& g_c3ui->TopWindowIsNonBackground()
		&& !g_theProfileDB->GetValueByName("RunInBackground")
		){
			endTurn = false;
		}

		if(endTurn && !isFirst)
		{
			for(sint32 o = 0; o < k_MAX_PLAYERS && endTurn; o++)
			{
				if(!g_player[o] || o == player)
					continue;
				for(sint32 u = 0; u < g_player[o]->m_all_units->Num() && endTurn; u++)
				{
					if(g_player[o]->m_all_units->Access(u).GetVisibility() & (1 << p->m_owner))
					{
						MapPoint pos;
						g_player[o]->m_all_units->Access(u).GetPos(pos);
						if(g_theWorld->IsCity(pos)) {

							continue;
						}
						endTurn = false;
					}
				}
			}
		}

		if(endTurn)
		{
			if(!isFirst || (g_theProfileDB->IsAutoEndMulitpleTurns() && !m_gotClickSinceLastAutoEnd))
			{
				m_gotClickSinceLastAutoEnd = false;
				Refresh();
				DPRINTF(k_DBG_GAMESTATE, ("MaybeAutoEndTurn is adding an EndTurn, %d\n", m_current_player));
				g_director->AddEndTurn();
			}
		}
	}
}

void SelectedItem::ClearWaypoints()
{
	m_waypoints.Clear();
}

void SelectedItem::NextPlayer()

{
	SetCurPlayer(m_next_player[m_current_player]);
}

sint32 SelectedItem::GetPlayerAfterThis(const sint32 p)
{
	Assert(0 <= p);
	Assert(p < k_MAX_PLAYERS);

	Assert(0 <= m_next_player[p]);
	Assert(m_next_player[p] < k_MAX_PLAYERS);

	return m_next_player[p];
}

PLAYER_INDEX SelectedItem::GetNextHumanPlayer()
{
	PLAYER_INDEX chk = m_current_player;
	do {
		chk = m_next_player[chk];
		if(g_player[chk]->IsHuman())
			return chk;
	} while(chk != m_current_player);
	return m_current_player;
}

bool SelectedItem::IsAutoCenterOn() const
{
	return g_theProfileDB->IsAutoCenter() != 0;
}

void SelectedItem::SetAutoCenter(const bool on)
{
	g_theProfileDB->SetAutoCenter(on);
}

void SelectedItem::SetCurPlayer(PLAYER_INDEX p)
{
	sint32 visPlayer = GetVisiblePlayer();
	Assert(m_next_player[p] != PLAYER_INDEX_INVALID);
	Assert(PLAYER_INDEX_0 <= p);
	Assert(p < k_MAX_PLAYERS);
	m_auto_unload = false;
	m_current_player = p;

	if(g_network.IsActive())
	{
		if(g_network.GetPlayerIndex() == p
		||(g_network.IsHost()
		&& !g_player[p]->IsNetwork())
		){
			// Do nothing for whatever reason
		}
		else
		{
			g_network.SetMyTurn(false);
		}
	}
	ClearCycle();

	if(visPlayer != GetVisiblePlayer())
	{
		g_turn->InformMessages();
	}
	else if(GetVisiblePlayer() == m_current_player)
	{
		// Do nothing for whatever reason
	}
}

void SelectedItem::NextRound()
{
	SetCurPlayer(0);
}

void SelectedItem::RegisterCreatedUnit(const PLAYER_INDEX owner)
{
	// Do nothing for whatever reason
}

void SelectedItem::RegisterCreatedCity(const PLAYER_INDEX owner)
{
	// Do nothing for whatever reason
}

void SelectedItem::RegisterRemovedArmy(const PLAYER_INDEX owner, const Army &dead_army)
{
	if (m_select_state[owner] == SELECT_TYPE_LOCAL_ARMY)
	{
		if (m_selected_army[owner] == dead_army)
		{
			Deselect(owner);
			m_selected_army[owner].m_id = (0);
		}
	}
}

void SelectedItem::RegisterRemovedCity(const PLAYER_INDEX owner, const Unit &dead_city)
{
	if (m_select_state[owner] == SELECT_TYPE_LOCAL_CITY)
	{
		if (m_selected_city[owner] == dead_city)
		{
			Deselect(owner);
			m_selected_city[owner].m_id = (0);
		}
	}
}

extern sint32 g_keypress_stop_player;

void SelectedItem::RemovePlayer(PLAYER_INDEX p)
{
	Assert(p != PLAYER_INDEX_VANDALS);
	Assert(m_next_player[p] != PLAYER_INDEX_INVALID);
	Assert(PLAYER_INDEX_0 <= p);
	Assert(p < k_MAX_PLAYERS);

	if(m_current_player == p)
	{
		NextPlayer();
	}

	for(sint32 i=0; i<k_MAX_PLAYERS; i++)
	{
		if(m_next_player[i] == p )
		{
			m_next_player[i] = m_next_player[p];

			if(p == g_keypress_stop_player)
			{
				g_keypress_stop_player = m_next_player[i];
			}
			m_next_player[p] = PLAYER_INDEX_INVALID;
			break;
		}
	}
}

void SelectedItem::AddPlayer(PLAYER_INDEX p)
{
	Assert(p != PLAYER_INDEX_VANDALS);
	Assert(p != PLAYER_INDEX_INVALID);
	Assert(p >= PLAYER_INDEX_0);
	Assert(p < k_MAX_PLAYERS);

	for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
	{
		if (g_player[i] && m_next_player[i] == p)
			c3errors_FatalDialogFromDB("CIV_ERROR", "CIV_FAILED_TO_ADD_PLAYER");
	}

	m_next_player[p] = m_next_player[m_current_player];
	m_next_player[m_current_player] = p;
	g_turn->RegisterNewPlayer(p);
}

uint32 SelectedItem_GetVersion(void)
{
	return (k_SELECTED_ITEM_VERSION_MAJOR<<16 | k_SELECTED_ITEM_VERSION_MINOR);
}

void SelectedItem::KeyboardSelectFirstUnit()
{
	NextUnmovedUnit();
}

void SelectedItem::SelectFirstUnit(bool setSelect)
{
	sint32 player = GetVisiblePlayer();
	m_select_state[player] = SELECT_TYPE_LOCAL_ARMY;
	sint32 curIndex = 0;
	sint32 tried = 1;

	if(g_player[player]->m_all_armies->Num() > 0)
	{
		m_selected_army[player] = g_player[player]->m_all_armies->Access(0);

		while(tried <= g_player[player]->m_all_armies->Num() &&
			  (!CanAutoSelect(g_player[player]->m_all_armies->Access(curIndex))))
		{
			curIndex++;
			if(curIndex >= g_player[player]->m_all_armies->Num())
				curIndex = 0;
			tried++;
			m_selected_army[player] = g_player[player]->m_all_armies->Access(curIndex);
		}
	}

	if(tried > g_player[player]->m_all_armies->Num())
	{
		if(g_player[player]->m_all_cities->Num() > 0 && setSelect)
		{
			m_select_state[player] = SELECT_TYPE_LOCAL_CITY;
			SetSelectUnit(g_player[player]->m_all_cities->Access(0));
		}
		else
		{
			Deselect(player);
		}
	}
	else if(setSelect)
	{
		SetSelectUnit(m_selected_army[player].GetTopVisibleUnit(GetVisiblePlayer()));
	}
}

void SelectedItem::Refresh()
{
	ClipCurrentItem();

	if ((m_player_on_screen != -1) &&
		(m_current_player != m_player_on_screen)) return;

	if(g_controlPanel)
	{
		PLAYER_INDEX s_player;
		ID s_item;
		SELECT_TYPE s_state;

		if (m_good_path)
			m_good_path->Clear();
		m_bad_path.Clear();

		if (!Player::IsThisPlayerARobot(GetVisiblePlayer()))
		{
			GetTopCurItem(s_player, s_item, s_state);

			if(s_state == SELECT_TYPE_LOCAL_CITY)
			{
				Unit u = s_item;

			}
			else if(s_state == SELECT_TYPE_LOCAL_ARMY)
			{
				Army a = s_item;
				MapPoint pos;
				a.GetPos(pos);
				Unit top;
				if (GetTopUnit(pos, top))
				{
					SetSelectUnit(top);
				}
				else
				{
					SetSelectUnit(Unit());
				}
			}
			else
			{
				// What's this?
				m_ignoreCitySelect = true;
				m_ignoreCitySelect = false;
			}
		}
	}
}

void SelectedItem::SetSelectCity(const Unit& u, bool all, bool isDoubleClick)
{
	SetSelectUnit(u, all, isDoubleClick);
}

void SelectedItem::SetSelectUnit(const Unit& u, bool all, bool isDoubleClick)
{
	m_selected_something_since_director_select = true;

	PLAYER_INDEX o = GetVisiblePlayer();

	if (g_player[o] == NULL) return;

	bool didSelect = false;

	m_auto_unload = false;

	m_waypoints.Clear();

	g_controlPanel->SetStack(Army(), NULL); // empty function

	if(!u.IsValid())
		return;

	if (u.IsCity())
	{
		if(o == u.GetOwner())
		{
			m_select_state[o] = SELECT_TYPE_LOCAL_CITY;
		}
		else
		{
			m_select_state[o] = SELECT_TYPE_REMOTE_CITY;
		}

		m_selected_city[o] = u;
		MapPoint	    pos;
		CellUnitList	army;

		u.GetPos(pos);
		m_select_pos[o] = pos;
		g_theWorld->GetCell(pos)->GetArmy(army);

		g_controlPanel->SetStack(Army(), &army);

		didSelect = true;

		g_slicEngine->RunCitySelectedTriggers(u);

		g_c3ui->AddAction( new WorkWinUpdateAction );

		// Focus on city if option is activated
		if(IsAutoCenterOn())
		{
			if(!g_director->TileWillBeCompletelyVisible(pos.x, pos.y))
			{
				g_director->AddCenterMap(pos);
			}
		}

		if(g_theProfileDB->IsDebugCityAstar())
		{
			SetDrawablePathDest(m_cur_mouse_tile);
		}
	}
	else
	{
		if(o == u.GetOwner())
		{
			m_select_state[o] = SELECT_TYPE_LOCAL_ARMY;
		}
		else
		{
			m_select_state[o] = SELECT_TYPE_REMOTE_ARMY;
		}

		m_selected_army[o] = u.GetArmy();

		m_select_cycle.Insert(u.GetArmy());

		MapPoint	pos;
		CellUnitList	*army;

		u.GetPos(pos);
		m_select_pos[o] = pos;
		army = g_theWorld->GetCell(pos)->UnitArmy();

		if ( all && g_player[o]->IsHuman() &&
			 (g_theProfileDB->IsAutoGroup() || isDoubleClick))
		{
			sint32 i;
			bool selectedCombatUnits = false;
			for(i = 0; i < m_selected_army[o].Num(); i++)
			{
				if(m_selected_army[o][i].GetAttack() > 0) {
					selectedCombatUnits = true;
					break;
				}
			}
			if(selectedCombatUnits)
			{
				for(i = 0; i < army->Num(); i++)
				{
					if(army->Access(i).GetAttack() > 0
					&& army->Access(i).GetArmy() != m_selected_army[o]
					&&!army->Access(i).IsAsleep()
					&&!army->Access(i).IsEntrenched()
					&&!army->Access(i).IsEntrenching()
					){
						m_selected_army[o].AddOrders(UNIT_ORDER_GROUP_UNIT, NULL, pos, (uint32)army->Access(i));
					}
				}
			}
		}

		g_controlPanel->SetStack(m_selected_army[o], army);

		SetDrawablePathDest(m_cur_mouse_tile);

		g_slicEngine->RunSelectedUnitTriggers(u);

		didSelect = true;
	}

	if (g_controlPanel)
	{
		if (u.GetOwner() == o)
		{
			if (u.IsCity()) {
				MainControlPanel::SelectedCity();

			} else {
				MainControlPanel::SelectedUnit();

			}
		}
		else {

			MainControlPanel::SelectedUnit();
		}
	}

	ArmyManagerWindow::NotifySelection();
	ScenarioEditor::NotifySelection();

	if(didSelect)
	{
		if(u.IsCity())
		{
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CitySelected, GEA_City, u, GEA_End);
			if(g_theProfileDB->GetAutoSwitchTabs())
			{
				g_controlPanel->SetTab(CP_TAB_CITY);
			}
		}
		else
		{
			Army a;
			if(GetSelectedArmy(a))
			{
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_ArmySelected, GEA_Army, a, GEA_End);
				if(g_theProfileDB->GetAutoSwitchTabs())
				{
					g_controlPanel->SetTab(CP_TAB_UNIT);
				}
			}
		}
		return;
	}

	Deselect(o);

	Assert(false);
}

void SelectedItem::SetSelectGood(const MapPoint &pos)
{
	PLAYER_INDEX o = GetVisiblePlayer();

	Assert(g_theWorld->IsGood(pos));

	if (!g_theWorld->IsGood(pos)) return;

	m_select_state[o] = SELECT_TYPE_GOOD;
	m_select_pos[o] = pos;

	sint32 goodIndex;

	if (g_theWorld->GetGood(pos, goodIndex))
	{
		sint32 goodSoundID = g_theResourceDB->Get(goodIndex)->GetSoundIndex();
		if (g_soundManager)
		{
			g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)goodIndex, goodSoundID);
		}

		if(g_controlPanel)
		{

		}
	}

	ArmyManagerWindow::NotifySelection();
}

void SelectedItem::Patrol(const MapPoint &pos)
{
}

bool SelectedItem::ResumePatrol()
{
	return false;
}

void SelectedItem::ForgetPatrol()
{
}

void SelectedItem::Deselect(PLAYER_INDEX player)
{
	Army a;
	if(GetSelectedArmy(a))
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ArmyDeselected, GEA_Army, a, GEA_End);
	Unit c;
	if(GetSelectedCity(c))
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CityDeselected, GEA_City, c, GEA_End);

	// #01 Standardization of city selection and focus handling
	if (g_controlPanel)
	{
		m_ignoreCitySelect = true;
		m_ignoreCitySelect = false;
	}

	m_select_state[player] = SELECT_TYPE_NONE;

	if(m_good_path)
	{
		delete m_good_path;
		m_good_path = NULL;
	}

	m_bad_path.Clear();

	ArmyManagerWindow::NotifySelection();
	ScenarioEditor::NotifySelection();

	m_is_pathing = false;

	if (g_controlPanel!=NULL)
		g_controlPanel->Deselect();
}

bool SelectedItem::GetTopUnitOrCity(const MapPoint &pos, Unit &top)
{
	bool	unitIsThere = false;

	if(g_theProfileDB->GetValueByName("CityClick")
	&& g_theWorld->IsCity(pos)
	){
		top = g_theWorld->GetCity(pos);
		unitIsThere = true;
	}
	else
	{
		unitIsThere = g_theWorld->GetTopVisibleUnit(pos, top);
	}

	if(!unitIsThere)
	{
		Cell *cell = g_theWorld->GetCell(pos);
		if(cell->GetCity().m_id != 0)
		{
			top = cell->GetCity();
			unitIsThere = true;
		}
		else if(cell->GetNumUnits() > 0)
		{
			for(sint32 i = 0; i < cell->GetNumUnits(); i++)
			{
				if(!cell->AccessUnit(i).IsEntrenched()
				&& !cell->AccessUnit(i).IsEntrenching()
				){
					top = cell->AccessUnit(i);
					unitIsThere = true;
					break;
				}
			}
		}
	}

	return unitIsThere;
}

bool SelectedItem::GetTopUnit(const MapPoint &pos, Unit &top)
{
	bool unitIsThere = g_theWorld->GetTopVisibleUnitNotCity(pos, top);
	if(!unitIsThere)
	{
		Cell *cell = g_theWorld->GetCell(pos);
		for(sint32 i=0; i< cell->GetNumUnits(); i++)
		{
			Unit u = cell->AccessUnit(i);
			if(!u.IsCity() && !u.IsEntrenched() && !u.IsEntrenching())
			{
				top = u;
				unitIsThere = true;
				break;
			}
		}
	}

	return unitIsThere;
}

void SelectedItem::EnterArmyMove(PLAYER_INDEX player, const MapPoint &pos)
{
	MapPoint		army_pos;
	m_selected_army[player].GetPos(army_pos);

	AddWaypoint(pos);
	m_is_pathing    = false;

	if (m_waypoints.Num() <= 0)
		return;

	Unit	unit;
	bool			moved = false;
	Path *goodPath = NULL;

	sint32 acknowledgeSoundID = 0;
	sint32 cantMoveSoundID = 0;
	sint32 unitX = 0;
	sint32 unitY = 0;

	if (GetTopUnitOrCity(army_pos, unit))
	{
		if (g_theUnitPool->IsValid(unit))
			acknowledgeSoundID = unit.GetAcknowledgeSoundID();

		if (g_theUnitPool->IsValid(unit))
			cantMoveSoundID = unit.GetCantMoveSoundID();

		unitX = unit.RetPos().x;
		unitY = unit.RetPos().y;
	}

	if (army_pos == pos)
	{
		m_waypoints.Clear();
		Deselect(player);
	}
	else
	{
		bool isCircular;
		double cost;
		ConstructPath(isCircular, cost);

		goodPath = m_good_path;
		m_good_path = NULL;

		if(goodPath)
		{
			m_selected_army[player].ClearOrders();

			goodPath->JustSetStart(m_selected_army[player]->RetPos());

			if(GetAutoUnload())
			{
				MapPoint    test(m_selected_army[player]->RetPos());
				goodPath->Start(test);

				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MoveUnloadOrder,
				                       GEA_Army,        m_selected_army[player],
				                       GEA_Path,        goodPath,
				                       GEA_MapPoint,    pos,
				                       GEA_Int,         0,
				                       GEA_End
				                      );

				SetAutoUnload(false);
			}
			else
			{
				MapPoint start, curPos;
				m_selected_army[player].GetPos(start);
				goodPath->Start(start);
				bool includesAttack = false;

				while(!goodPath->IsEnd())
				{
					goodPath->Next(curPos);
					Cell *cell = g_theWorld->GetCell(curPos);
					if(cell->GetCity().m_id != 0 &&
					   cell->GetCity().GetOwner() != player)
					{
						includesAttack = true;
						break;
					}
					if(cell->UnitArmy() && cell->UnitArmy()->GetOwner() != player &&
					   cell->UnitArmy()->IsVisible(player))
					{
						includesAttack = true;
						break;
					}
				}

				goodPath->Restart(start);

				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MoveOrder,
				                       GEA_Army,        m_selected_army[player],
				                       GEA_Path,        goodPath,
				                       GEA_MapPoint,    curPos,
				                       GEA_Int,         includesAttack,
				                       GEA_End
				                      );
			}

			moved = true;
		}
	}

	if (!(rand() % 3))
	{
		if (moved)
		{
			g_soundManager->AddSound(SOUNDTYPE_VOICE, 0, acknowledgeSoundID,
											unitX, unitY);
		}
		else
		{
			g_soundManager->AddSound(SOUNDTYPE_VOICE, 0, cantMoveSoundID,
											unitX, unitY);
		}
	}
}

void SelectedItem::PlaySelectedSound(Unit &unit)
{
	if (!g_theUnitPool->IsValid(unit)) return;

	if (g_soundManager && unit.GetOwner() == GetVisiblePlayer())
	{
		if (!(rand() % 3))
		{
			bool canMove = false;

			if (unit.GetArmy().m_id != 0 && g_theArmyPool->IsValid(unit.GetArmy()))
			{
				if (unit.GetArmy().CanMove())
					canMove = true;
			}

			sint32 id;
			if (canMove)
			{
				if (rand()%2)
					id = unit.GetSelect1SoundID();
				else
					id = unit.GetSelect2SoundID();
			}
			else
			{
				id = unit.GetCantMoveSoundID();
			}

			g_soundManager->AddSound(SOUNDTYPE_VOICE, (uint32)unit, id);
		}
	}
}

void SelectedItem::RegisterClick(const MapPoint &pos,  const aui_MouseEvent *data, bool doubleClick,
								 bool leftDrag, bool leftDrop)

{
	m_gotClickSinceLastAutoEnd = true;

	{
		extern BOOL g_toeMode;
		if(g_toeMode)
		{
			Cell *cell = g_theWorld->GetCell(pos);
			if(cell->UnitArmy())
			{
				cell->UnitArmy()->KillList(CAUSE_REMOVE_ARMY_TOE, -1);
				Deselect(GetVisiblePlayer());
			}

			if(cell->HasCity())
			{
				cell->GetCity().Kill(CAUSE_REMOVE_ARMY_UNKNOWN, -1);
				Deselect(GetVisiblePlayer());
			}

			g_theWorld->CutImprovements(pos);
			return;
		}

#ifdef _DEBUG
		MapPoint south;
		MapPoint converted;
		MapPoint southConverted;
		pos.GetNeighborPosition(SOUTH, south);
		converted.Iso2Norm(pos);
		southConverted.Iso2Norm(south);
		sint32 tileX;
		sint32 tileY = pos.y;
		maputils_MapX2TileX(pos.x, pos.y, &tileX);

		DPRINTF(k_DBG_GAMESTATE, ("POS %d %d : %s\n", pos.x, pos.y, leftDrag ? "DRAG" : (leftDrop ? "DROP" : "CLICK")));
		DPRINTF(k_DBG_GAMESTATE, (" ai %d %d\n", converted.x, converted.y));
		DPRINTF(k_DBG_GAMESTATE, (" xy %d %d\n", tileX, tileY));
		DPRINTF(k_DBG_GAMESTATE, ("zoc 0x%lx, owner %d\n", g_theWorld->GetCell(pos)->GetRawZoc(), g_theWorld->GetCell(pos)->GetOwner()));
		DPRINTF(k_DBG_GAMESTATE, ("\n"));
#endif
	}

	NewRegisterClick(pos, data, doubleClick, leftDrag, leftDrop);
	return;
}

void SelectedItem::SelectTradeRoute( const MapPoint &pos )
{
	Cell *cell = g_theWorld->GetCell(pos);
	sint32 tradeNum = cell->GetNumTradeRoutes();
	const UnitData *ud = NULL;
	CityData *cd = NULL;

	for ( sint32 i = 0 ; i < tradeNum ; i++ )
	{
		TradeRoute route = cell->GetTradeRoute(i);
		if(!route.IsActive()) // skip deactivated routes (only exist for drawing until revisited, see #256)
		    continue;

		ud = route.GetSource().GetData();
		cd = ud->GetCityData();

		if (cd->GetOwner() == m_current_player)
		{
			if (!route.InitSelectedData())
			{
				if (route.IsPosInPath(pos))
				{
					break;
				}
				else if (route.IsPosInSelectedPath(pos))
				{
					route.SetPathSelectionState(k_TRADEROUTE_SELECTED_PATH);
				}
			}
			else
			{
				route.SetPathSelectionState(k_TRADEROUTE_ORIGINAL_PATH);
			}

			g_tradeSelectedState = true;

			route.AddSelectedWayPoint(pos);

			TradeRoute tradeRouteItem = cell->GetTradeRoute(i);
			g_grabbedItem->SetGrabbedItem(&tradeRouteItem);

			cell->GetTradeRoute(i).ClearSelectedCellData(cell->GetTradeRoute(i));
			break;
		}
	}

	return;
}

sint32 SelectedItem::GetVisiblePlayer() const
{
	if (m_player_on_screen != -1 && !g_network.IsActive())
		return m_player_on_screen;

	if(!g_network.IsActive())
	{
		return m_current_player;
	}
	else
	{
		if(g_network.IsClient())
		{
			return g_network.GetPlayerIndex();
		}
		else
		{
			if(0 // Never used
			&& g_player[m_current_player]
			&& g_player[m_current_player]->IsHuman()
			){
				return m_current_player;
			}
			else
			{
				if(!g_player[g_network.GetPlayerIndex()])
				{
					return m_current_player;
				}
				else
				{
					return g_network.GetPlayerIndex();
				}
			}
		}
	}
}

void SelectedItem::AddWaypoint(const MapPoint &pos)
{
	m_waypoints.Insert(pos);
}

void SelectedItem::SetDrawablePathDest(MapPoint &dest)
{
	if (!g_theUnitAstar) return;

	if(!ShouldDrawPath())
	{
		m_cur_mouse_tile = dest;
		m_is_pathing = false;
		return;
	}

	PLAYER_INDEX player = GetVisiblePlayer();

	if (m_select_state[player] == SELECT_TYPE_LOCAL_ARMY)
	{
		m_cur_mouse_tile = dest;

		Army a = m_selected_army[player];

		MapPoint start;
		if(m_waypoints.Num() <= 0)
		{
			a.GetPos(start);
		}
		else
		{
			start = m_waypoints[m_waypoints.Num() - 1];
		}

		if (start == dest)
		{
			m_is_pathing = false;
			return;
		}

		if(!m_good_path)
			m_good_path = new Path;

		float total_cost;
		Assert(g_theUnitAstar);
		if(g_player[player]->IsHuman())
		{
			m_is_pathing = g_theUnitAstar->FindPath(a, start,
												player, m_cur_mouse_tile,
												*m_good_path, m_is_broken_path,
												m_bad_path,
												total_cost);
			Assert(m_is_pathing);
		}
		else // For debugging the AI
		{
			m_is_pathing = Agent::FindPath(a, m_cur_mouse_tile, true, *m_good_path, total_cost);
		}
	}
	else if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY_UNLOADING)
	{
		m_cur_mouse_tile = dest;
		Army a = m_selected_army[player];
		MapPoint start;
		a.GetPos(start);
		if( start == dest
		||  a.Num() < 1
		|| !a[0].GetData()->GetCargoList()
		||  a[0].GetData()->GetCargoList()->Num() < 1
		){
			m_is_pathing = false;
			return;
		}

		delete m_good_path;
		m_good_path = NULL;
		m_bad_path.Clear();

		bool r;
		if(!start.IsNextTo(dest))
		{
			r = g_theUnitAstar->StraightLine(start, dest, m_bad_path);
		}
		else
		{
			static CellUnitList units;
			units.Clear();
			if(a[0].GetData()->GetCargoList())
			{
				for(sint32 i = 0; i < a[0].GetData()->GetCargoList()->Num(); i++)
				{
					units.Insert(a[0].GetData()->GetCargoList()->Access(i));
				}
			}

			bool zocViolation;
			bool alliedCity;
			if(units.Num() > 0 && units.CanMoveIntoCell(dest,
														zocViolation,
														false,
														alliedCity)
			){
				if(!m_good_path)
					m_good_path = new Path;
				r = g_theUnitAstar->StraightLine(start, dest, *m_good_path);
			}
			else
			{
				r = g_theUnitAstar->StraightLine(start, dest, m_bad_path);
			}
		}
		m_is_pathing = r;
	}
	else if(m_select_state[player] == SELECT_TYPE_LOCAL_CITY)
	{
		m_cur_mouse_tile = dest;

		Unit c = m_selected_city[player];

		MapPoint start;
		if(m_waypoints.Num() <= 0)
		{
			c.GetPos(start);
		}
		else
		{
			start = m_waypoints[m_waypoints.Num() - 1];
		}

		if (start == dest)
		{
			m_is_pathing = false;
			return;
		}

		if(!m_good_path)
			m_good_path = new Path;

		float total_cost;
		bool r = g_city_astar.FindRoadPath(start, m_cur_mouse_tile,player, *m_good_path, total_cost);

		m_is_pathing = r;
	}
}

void SelectedItem::ConstructPath(bool &isCircular, double &cost)
{
	PLAYER_INDEX player = GetVisiblePlayer();
	Path *partialPath = new Path;
	float partialCost;
	cost = 0.0;

	delete m_good_path;
	m_good_path = NULL;

	Army a = m_selected_army[player];
	MapPoint start;
	a.GetPos(start);
	g_theUnitAstar->FindPath(a, start,
							 player, m_waypoints[0],
							 *partialPath, m_is_broken_path,
							 m_bad_path,
							 partialCost);
	m_good_path = partialPath;

	if (m_is_broken_path)
	{
		m_good_path->Concat(m_bad_path);
	}

	if (m_is_broken_path)
	{
		uint32 movetype = a.GetMovementType();
		if((movetype & k_BIT_MOVEMENT_TYPE_SHALLOW_WATER) &&
		   !(movetype & k_BIT_MOVEMENT_TYPE_WATER) &&
		   g_theWorld->IsShallowWater(m_waypoints[0])
		  )
		{
			QuickSlic("36IATriremeDeepwaterTip", player);
		}
		if((movetype & k_BIT_MOVEMENT_TYPE_LAND) &&
		   !(movetype & k_BIT_MOVEMENT_TYPE_MOUNTAIN) &&
		   g_theWorld->IsMountain(m_waypoints[0])
		  )
		{
			QuickSlic("35IANomountainUnitTip", player);
		}
	}

	cost += partialCost;
	partialPath = new Path;
	for(sint32 i = 1; i < m_waypoints.Num(); i++)
	{
		g_theUnitAstar->FindPath(a, m_waypoints[i-1],
								 player, m_waypoints[i],
								 *partialPath, m_is_broken_path,
								 m_bad_path,
								 partialCost);
		m_good_path->Concat(*partialPath);
		if (m_is_broken_path) {
			m_good_path->Concat(m_bad_path);
		}
		cost += partialCost;
	}
	delete partialPath;

	isCircular = start == m_waypoints[m_waypoints.Num() - 1];

	m_waypoints.Clear();
}

void SelectedItem::ProcessUnitOrders()
{
	if(m_current_player == GetVisiblePlayer()) {
		g_player[m_current_player]->ProcessUnitOrders();
	}
}

void SelectedItem::Settle()
{
	PLAYER_INDEX    s_player;
	ID			    s_item;
	SELECT_TYPE     s_state;
	bool		    isMyTurn = !g_network.IsActive() || g_network.IsMyTurn();

	GetTopCurItem(s_player, s_item, s_state);

	if ((SELECT_TYPE_LOCAL_ARMY == s_state) && isMyTurn)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail,
		                       GEV_SettleOrder,
		                       GEA_Army, s_item.m_id,
		                       GEA_End
		                      );
	}
}

void SelectedItem::Entrench()
{
	PLAYER_INDEX player = GetVisiblePlayer();

	switch(m_select_state[player])
	{
		case SELECT_TYPE_LOCAL_ARMY:
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_EntrenchOrder,
								   GEA_Army, m_selected_army[player],
								   GEA_End);

			Deselect(player);
			if(g_theProfileDB->IsAutoSelectNext())
			{
				g_director->AddSelectUnit(0);
				m_selected_something_since_director_select = false;
			}
			break;
		default:
			break;
	}
}

void SelectedItem::Detrench()
{
	PLAYER_INDEX player = GetVisiblePlayer();

	switch(m_select_state[player])
	{
		case SELECT_TYPE_LOCAL_ARMY:
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_DetrenchOrder,
								   GEA_Army, m_selected_army[player],
								   GEA_End);

			break;
		default:
			break;
	}
}

void SelectedItem::Sleep()
{
	PLAYER_INDEX player = GetVisiblePlayer();

	switch(m_select_state[player])
	{
		case SELECT_TYPE_LOCAL_ARMY:
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SleepOrder,
								   GEA_Army, m_selected_army[player],
								   GEA_End);

			Deselect(player);
			if(g_theProfileDB->IsAutoSelectNext())
			{
				g_director->AddSelectUnit(0);
				m_selected_something_since_director_select = false;
			}
			break;
		default:
			break;
	}
}

void SelectedItem::Disband()
{
	PLAYER_INDEX player = GetVisiblePlayer();

	switch(m_select_state[player])
	{
		case SELECT_TYPE_LOCAL_ARMY:
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_DisbandArmyOrder,
								   GEA_Army, m_selected_army[player],
								   GEA_End);

			Deselect(player);
			break;
		default:
			break;
	}
}

void SelectedItem::GroupArmy()
{
	PLAYER_INDEX player = GetVisiblePlayer();

	switch (m_select_state[player])
	{
		case SELECT_TYPE_LOCAL_ARMY:
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_GroupOrder,
								   GEA_Army, m_selected_army[player],
								   GEA_End);

			break;
		default:
			return;
	}
}

void SelectedItem::UngroupArmy()
{
	PLAYER_INDEX player = GetVisiblePlayer();
	switch (m_select_state[player])
	{
		case SELECT_TYPE_LOCAL_ARMY:
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_UngroupOrder,
								   GEA_Army, m_selected_army[player],
								   GEA_End);

			break;
		default:
			return;
	}
}

void SelectedItem::UnloadMode()
{
	PLAYER_INDEX player = GetVisiblePlayer();
	switch (m_select_state[player])
	{
		case SELECT_TYPE_LOCAL_ARMY:
			m_is_pathing = true;
			m_select_state[player] = SELECT_TYPE_LOCAL_ARMY_UNLOADING;

			break;
		default:
			return;
	}
}

void SelectedItem::CancelUnload()
{
	PLAYER_INDEX player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY_UNLOADING)
	{
		m_select_state[player] = SELECT_TYPE_LOCAL_ARMY;
	}
}

void SelectedItem::Paradrop(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	switch(m_select_state[player])
	{
		case SELECT_TYPE_LOCAL_ARMY:
			m_is_pathing = false;

			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ParadropOrder,
								   GEA_Army, m_selected_army[player],
								   GEA_MapPoint, point,
								   GEA_End);
			break;
		default:
			return;
	}
}

void SelectedItem::SpaceLaunch()
{
	PLAYER_INDEX player = GetVisiblePlayer();

	switch(m_select_state[player])
	{
		case SELECT_TYPE_LOCAL_ARMY:
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_LaunchOrder,
								   GEA_Army, m_selected_army[player],
								   GEA_End);

			Deselect(m_current_player);
			break;
		default:
			break;
	}
}

void SelectedItem::InvestigateCity(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_InvestigateCityOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);

		Deselect(m_current_player);
	}
}

void SelectedItem::NullifyWalls(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_NullifyWallsOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);

		Deselect(m_current_player);
	}
}

void SelectedItem::StealTechnology(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_StealTechnologyOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);

		Deselect(m_current_player);
	}
}

void SelectedItem::InciteRevolution(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_InciteRevolutionOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);

		Deselect(m_current_player);
	}
}

void SelectedItem::AssassinateRuler(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_AssassinateRulerOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);

		Deselect(m_current_player);
	}
}

void SelectedItem::InvestigateReadiness(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_InvestigateReadinessOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);

		Deselect(m_current_player);
	}
}

void SelectedItem::Bombard(const MapPoint &pnt)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BombardOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, pnt,
							   GEA_End);
	}
}

void SelectedItem::Franchise(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_FranchiseOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::Sue(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SueOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::SueFranchise(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SueFranchiseOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::Expel(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ExpelOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::EstablishEmbassy(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_EstablishEmbassyOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::Advertise(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_AdvertiseOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::PlantNuke(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_PlantNukeOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::SlaveRaid(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SlaveRaidOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::EnslaveSettler(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		double success, death;
		sint32 timer, amount;
		sint32 uindex;
		bool target_is_city;
		Unit target_city;
		Unit home_city;

		Army al = m_selected_army[player];
		Assert(al.m_id != (0));

		if(!al.IsSlaveRaidPossible(
		   point, success, death, timer, amount, uindex,
		   target_is_city, target_city, home_city))
		{
			return;
		}

		MapPoint mypos;
		al.GetPos(mypos);
		if(!point.IsNextTo(mypos))
		{
			return;
		}

		if (!target_is_city)
		{
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_EnslaveSettlerOrder,
								   GEA_Army, m_selected_army[player],
								   GEA_MapPoint, point,
								   GEA_End);
		}
	}
}

void SelectedItem::UndergroundRailway(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_UndergroundRailwayOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::InciteUprising(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_InciteUprisingOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::BioInfect(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BioInfectOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::NanoInfect(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_NanoInfectOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::ConvertCity(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY  )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ConvertCityOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::ReformCity(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ReformCityOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::IndulgenceSale(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SellIndulgencesOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::Soothsay(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY  )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SoothsayOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::Cloak()
{
	PLAYER_INDEX player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CloakOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_End);
	}
}

void SelectedItem::Uncloak()
{
	PLAYER_INDEX player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_UncloakOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_End);
	}
}

void SelectedItem::Rustle(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY  )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_RustleOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::CreatePark(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CreateParkOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
		Deselect(m_current_player);
	}
}

void SelectedItem::CreateRift(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY  )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CreateRiftOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
		Deselect(m_current_player);
	}
}

void SelectedItem::Pillage()
{
	PLAYER_INDEX player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_PillageOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_End);
	}
}

void SelectedItem::Injoin(const MapPoint &point)
{
	PLAYER_INDEX player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_InjoinOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, point,
							   GEA_End);
	}
}

void SelectedItem::UseSpaceLadder()
{
	PLAYER_INDEX player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_UseSpaceLadderOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_End);
	}
}

void SelectedItem::Airlift(const MapPoint &dest)
{
	PLAYER_INDEX player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_AirliftOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_MapPoint, dest,
							   GEA_End);
	}
}

void SelectedItem::Descend()
{
	sint32 player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_DescendOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_End);

		Deselect( m_current_player );
	}
}

void SelectedItem::Goto(MapPoint &dest)
{
	sint32 owner = m_current_player;

	if (m_select_state[owner] == SELECT_TYPE_LOCAL_ARMY)
	{
		MapPoint src;
		m_selected_army[owner].GetPos(src);

		EnterMovePath(owner, m_selected_army[owner], src, dest);
	}
}

void SelectedItem::EnterMovePath(sint32 owner, Army &army,
								 MapPoint const & src, MapPoint const & dest)
{
	Path *good_path = new Path, bad_path;
	bool is_broken;
	float cost;
	sint32 r = g_theUnitAstar->FindPath(army, src,
										owner, dest,
										*good_path, is_broken,
										bad_path,
										cost);
	if (!r || is_broken)
	{
		delete good_path;
		return;
	}

	army.ClearOrders();
	good_path->JustSetStart(army->RetPos());

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MoveOrder,
						   GEA_Army, army,
						   GEA_Path, good_path,
						   GEA_MapPoint, src,
						   GEA_Int, 0,
						   GEA_End);
}

void SelectedItem::EntrenchArmy(sint32 owner, sint32 index)
{
	Assert(false);
}

void SelectedItem::SleepArmy(sint32 owner, sint32 index)
{
	Assert(false);
}

void SelectedItem::InterceptTrade(void)
{
	sint32 player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_PirateOrder,
							   GEA_Army, m_selected_army[player],
							   GEA_End);
	}
}

bool SelectedItem::GetSelectedCity( Unit &city )
{
	PLAYER_INDEX pIndex;
	ID id;
	SELECT_TYPE sType;

	GetTopCurItem(pIndex,id,sType);

	if (sType == SELECT_TYPE_LOCAL_CITY)
	{
		city = id;
		return true;
	}

	return false;
}

void SelectedItem::UnitCityToggle()
{
	MapPoint pos;
	Unit u;

	sint32 player = GetVisiblePlayer();
	switch(m_select_state[player])
	{
		case SELECT_TYPE_LOCAL_ARMY:
		case SELECT_TYPE_REMOTE_ARMY:
			m_selected_army[player].GetPos(pos);
			if(g_theWorld->GetCell(pos)->GetCity().m_id != (0))
			{
				SetSelectUnit(g_theWorld->GetCell(pos)->GetCity());
			}
			else if(g_theWorld->IsGood(pos))
			{
				SetSelectGood(pos);
			}
			break;
		case SELECT_TYPE_LOCAL_CITY:
		case SELECT_TYPE_REMOTE_CITY:
			m_selected_city[player].GetPos(pos);
			if(g_theWorld->IsGood(pos))
			{
				SetSelectGood(pos);
			}
			else if(GetTopUnit(pos, u))
			{
				SetSelectUnit(u);
			}
			break;
		case SELECT_TYPE_GOOD:
			pos = m_select_pos[player];
			if(GetTopUnit(pos, u))
			{
				SetSelectUnit(u);
			}
			else if(g_theWorld->GetCell(pos)->GetCity().m_id != (0))
			{
				SetSelectUnit(g_theWorld->GetCell(pos)->GetCity());
			}
			break;
		default:
			break;
	}
}

void SelectedItem::ClearCycle()
{
	sint32 player = GetVisiblePlayer();
	m_select_cycle.Clear();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY)
	{
		m_select_cycle.Insert(m_selected_army[player]);
	}
}

void SelectedItem::DidKeyboardMove()
{
	sint32 player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY)
	{
		if(m_selected_army[player].Num() < 1 ||
		   !m_selected_army[player].CanMove())
		{
			Deselect(player);
			if(g_theProfileDB->IsAutoSelectNext())
			{
				g_director->AddSelectUnit(0);
				m_selected_something_since_director_select = false;
			}
		}
		else
		{
			if(IsAutoCenterOn())
			{
				MapPoint pos;
				m_selected_army[player].GetPos(pos);
				if(!g_director->TileWillBeCompletelyVisible(pos.x, pos.y))
				{
					g_director->AddCenterMap(pos);
				}
			}
		}
	}
	else
	{
		if(g_theProfileDB->IsAutoSelectNext())
		{
			g_director->AddSelectUnit(0);
			m_selected_something_since_director_select = false;
		}
	}
}

void SelectedItem::EndUnitTurn()
{
	sint32 player = GetVisiblePlayer();
	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY)
	{
		m_selected_army[player].SetTurnOver();
		Deselect(player);
		if(g_theProfileDB->IsAutoSelectNext())
		{
			g_director->AddSelectUnit(0);
			m_selected_something_since_director_select = false;
		}
	}
}

void SelectedItem::DirectorUnitSelection(uint32 flags)
{
	bool isFirst = (flags & k_UNIT_SELECT_IS_FIRST) != 0;

	if(!m_selected_something_since_director_select)
	{
		if(m_force_select_army.IsValid()
		&& CanAutoSelect(m_force_select_army)
		){
			SetSelectUnit(m_force_select_army[0]);
			m_force_select_army.m_id = 0;
		}
		else
		{
			m_force_select_army.m_id = 0;
			NextUnmovedUnit(isFirst);
		}
	}
}

void SelectedItem::ForceDirectorSelect(const Army &army)
{
	if(army.IsValid() && IsPlayerVisible(army.GetOwner()))
	{
		m_force_select_army = army;
	}
}

void SelectedItem::RegisterUIClick()
{
	m_gotClickSinceLastAutoEnd = true;
}

void SelectedItem::RegisterManualEndTurn()
{
	m_gotClickSinceLastAutoEnd = false;
}

sint32 SelectedItem::GetIsPathing() const
{
	return m_is_pathing
	    && ( m_select_state[GetVisiblePlayer()] == SELECT_TYPE_LOCAL_ARMY
	    ||   m_select_state[GetVisiblePlayer()] == SELECT_TYPE_LOCAL_ARMY_UNLOADING
	    ||  (g_theProfileDB->IsDebugCityAstar()
	    &&   m_select_state[GetVisiblePlayer()] == SELECT_TYPE_LOCAL_CITY)
	       );
}

void SelectedItem::UpdateSelectedItem( void )
{
	ClipCurrentItem();

	if ((m_player_on_screen != -1) &&
		(m_current_player != m_player_on_screen)) return;

	if(g_controlPanel)
	{
		PLAYER_INDEX s_player;
		ID s_item;
		SELECT_TYPE s_state;

		if (m_good_path)
			m_good_path->Clear();
		m_bad_path.Clear();

		if (!Player::IsThisPlayerARobot(GetVisiblePlayer()))
		{
			GetTopCurItem(s_player, s_item, s_state);

			if(s_state == SELECT_TYPE_LOCAL_CITY)
			{
				Unit u = s_item;
			}
			else if(s_state == SELECT_TYPE_LOCAL_ARMY)
			{
				Army a = s_item;
				SetSelectUnit( a.GetTopVisibleUnit(GetVisiblePlayer() ) );
			}
			else
			{

			}
		}
	}
}

bool SelectedItem::GetInciteRevolutionCost( const MapPoint &point, sint32 &cost )
{
	sint32 player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		return m_selected_army[player].GetInciteRevolutionCost( point, cost );
	}

	return false;
}

bool SelectedItem::GetInciteUprisingCost( const MapPoint &point, sint32 &cost )
{
	sint32 player = GetVisiblePlayer();

	if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY )
	{
		return m_selected_army[player].GetInciteUprisingCost( point, cost );
	}

	return false;
}

void SelectedItem::SetPlayerOnScreen(PLAYER_INDEX player)
{
	m_player_on_screen = player;

	ScenarioEditor::NotifyPlayerChange();
}

void SelectedItem::ArmyMovedCallback(Army &a)
{
	sint32 player = GetVisiblePlayer();
	if(m_select_state[player] != SELECT_TYPE_LOCAL_ARMY ||
	   a.m_id != m_selected_army[player].m_id)
	{
		return;
	}
	m_select_pos[player] = a->RetPos();

	bool deselected = false;
	MapPoint army_pos;
	if(g_theProfileDB->IsAutoDeselect())
	{
		if(m_select_state[player] == SELECT_TYPE_LOCAL_ARMY)
		{
			if(m_selected_army[player].Num() < 1 ||
			   !m_selected_army[player].CanMove())
			{
				Deselect(player);
				deselected = true;
				if(g_theProfileDB->IsAutoSelectNext())
				{
					g_director->AddSelectUnit(0);
					m_selected_something_since_director_select = false;
				}
			}
			else
			{
				m_selected_army[player].GetPos(army_pos);
			}
		}
	}
	if(!deselected)
	{
		if(g_controlPanel)
		{
			if(g_theArmyPool->IsValid(m_selected_army[player]))
			{
				m_selected_army[player].GetPos(army_pos);
				g_controlPanel->SetStack(m_selected_army[player],
										 g_theWorld->GetCell(army_pos)->UnitArmy());
			}
		}
	}

	if(g_slicEngine->GetTutorialActive())
	{
		bool allMoved = true;
		for(sint32 i = g_player[player]->m_all_armies->Num() - 1; i >= 0; i--)
		{
			if(CanAutoSelect(g_player[player]->m_all_armies->Access(i)))
			{
				allMoved = false;
				break;
			}
		}
		if(allMoved)
		{
			g_slicEngine->RunAllUnitsMovedTriggers();
		}
	}
}

bool SelectedItem::ShouldDrawPath()
{
	Assert(g_controlPanel != NULL);
	if(( g_theProfileDB->IsUseCTP2Mode()
	&&   g_controlPanel->GetTargetingMode() != CP_TARGETING_MODE_ORDER_PENDING)
	&& (!IsLocalCity()
	||  !g_theProfileDB->IsDebugCityAstar()))
		return false;
	else
		return true;
}

bool SelectedItem::GetSelectedArmy(Army &a)
{
	switch(m_select_state[GetVisiblePlayer()])
	{
		case SELECT_TYPE_LOCAL_ARMY:
		case SELECT_TYPE_LOCAL_ARMY_UNLOADING:

			a = m_selected_army[GetVisiblePlayer()];
			return true;
		default:
			return false;
	}
}
