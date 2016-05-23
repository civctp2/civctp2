#include "c3.h"
#include "net_gamesettings.h"
#include "net_util.h"
#include "network.h"

#include "XY_Coordinates.h"
#include "World.h"
#include "player.h"
#include "tiledmap.h"
#include "UnitDynArr.h"
#include "QuadTree.h"
#include "SelItem.h"
#include "background.h"
#include "radarmap.h"
#include "installationtree.h"
#include "radarwindow.h"
#include "c3ui.h"
#include "gameinit.h"

#include "GameSettings.h"

#include "controlpanelwindow.h"

extern QuadTree<Unit> *g_theUnitTree;
extern World *g_theWorld;
extern Player **g_player;
extern TiledMap *g_tiledMap;
extern RadarMap *g_radarMap;
extern SelectedItem *g_selected_item;
extern Background			*g_background;
extern C3UI					*g_c3ui;

extern ControlPanelWindow	*g_controlPanel;

NetGameSettings::NetGameSettings(sint32 x, sint32 y,
								 sint32 numPlayers,
								 uint8 gameStyle,
								 sint32 movesPerSlice,
								 time_t totalTime,
								 time_t turnTime,
								 time_t cityTime)
{
	m_x = x;
	m_y = y;
	m_numPlayers = numPlayers;
	m_gameStyle = gameStyle;
	m_movesPerSlice = movesPerSlice;
	m_totalTime = totalTime;
	m_turnTime = turnTime;
	m_cityTime = cityTime;
}

void NetGameSettings::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_GAME_SETTINGS_ID);

	PUSHLONG(m_x);
	PUSHLONG(m_y);
	PUSHLONG(m_numPlayers);
	PUSHBYTE(m_gameStyle);
	PUSHLONG(m_movesPerSlice);
	PUSHLONG(m_totalTime);
	PUSHLONG(m_turnTime);
	PUSHLONG(m_cityTime);
	uint32 playerMask = 0;
	for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i])
			playerMask |= (1 << i);
	}
	PUSHLONG(playerMask);

	PUSHLONG(g_theGameSettings->m_difficulty);
	PUSHLONG(g_theGameSettings->m_risk);
	PUSHLONG(g_theGameSettings->m_alienEndGame);
	PUSHLONG(g_theGameSettings->m_pollution);

	PUSHLONG(g_theWorld->m_isYwrap);
	PUSHLONG(g_theWorld->m_isXwrap);
}

void NetGameSettings::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 packid;
	uint16 pos = 0;
	uint32 playerMask;

	PULLID(packid);
	Assert(packid == k_PACKET_GAME_SETTINGS_ID);

	PULLLONG(m_x);
	PULLLONG(m_y);
	PULLLONG(m_numPlayers);
	PULLBYTE(m_gameStyle);
	PULLLONG(m_movesPerSlice);
	PULLLONG(m_totalTime);
	PULLLONG(m_turnTime);
	PULLLONG(m_cityTime);
	PULLLONG(playerMask);

	PULLLONG(g_theGameSettings->m_difficulty);
	PULLLONG(g_theGameSettings->m_risk);
	PULLLONG(g_theGameSettings->m_alienEndGame);
	PULLLONG(g_theGameSettings->m_pollution);

	BOOL isYwrap, isXwrap;
	PULLLONG(isYwrap);
	PULLLONG(isXwrap);

	Assert(pos == size);

	if(g_controlPanel) {

	}

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
	g_theInstallationTree->Clear();
	delete g_theUnitTree;
	g_theUnitTree = NULL;
	delete g_theInstallationTree;
	g_theInstallationTree = NULL;

	g_network.ClearDeadUnits();













	g_theWorld->Reset(sint16(m_x), sint16(m_y), isYwrap, isXwrap);

	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i]) {
			delete g_player[i];
		}
	}
	delete [] g_player;

	g_player = new Player *[k_MAX_PLAYERS];
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		g_player[i] = NULL;
	}

	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(playerMask & (1 << i)) {
			g_player[i] = new Player(i, 0, PLAYER_TYPE_HUMAN);
		}
	}

	delete g_selected_item;
	g_selected_item = new SelectedItem(m_numPlayers);

	g_theUnitTree = new QuadTree<Unit>((sint16)g_theWorld->GetXWidth(),
									   (sint16)g_theWorld->GetYHeight(),
									   g_theWorld->IsYwrap());
	g_theInstallationTree = new InstallationQuadTree((sint16)g_theWorld->GetXWidth(),
													 (sint16)g_theWorld->GetYHeight(),
													 g_theWorld->IsYwrap());

	g_network.SetStyleFromServer(m_gameStyle, m_movesPerSlice, m_totalTime, m_turnTime, m_cityTime);

	g_tiledMap->CopyVision();

	gameinit_ResetForNetwork();

	g_network.SetLoop(TRUE);
}
