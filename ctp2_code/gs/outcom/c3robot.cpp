#define INITGUID
#include "c3.h"
#include "c3errors.h"
#include "globals.h"

#include "aui.h"
#include "debugwindow.h"
extern DebugWindow *g_debugWindow;

#include "ConstDB.h"
extern ConstDB *g_theConstDB;

#include "dynarr.h"
#include "SelItem.h"
extern SelectedItem *g_selected_item;

#include "TurnCnt.h"
extern TurnCount *g_turn;

#include "bset.h"
#include "player.h"
#include "Unit.h"

#include <objbase.h>
#include "IRobot.h"
#include "C3GameState.h"

#include "aicause.h"

#include "IMapGen.h"
#include "ic3BlgDB.h"
#include "ic3Rand.h"

#include "C3UnitDB.h"
#include "C3BlgDB.h"
#include "C3InstDB.h"
#include "CivPaths.h"
#include "C3TerrDB.h"
#include "C3ErrorReport.h"
#include "C3String.h"
#include "C3Rand.h"
#include "C3Player.h"
#include "C3World.h"
#include "C3Science.h"
#include "C3Government.h"
#include "C3Wonder.h"
#include "C3Population.h"
#include "RobotAstar.h"
#include "c3endgamedb.h"

#include "aui_surface.h"

#include "maputils.h"
#include "primitives.h"

#include "tileset.h"

#include "XY_Coordinates.h"
#include "World.h"
#include "ic3DipReq.h"

#include "profileDB.h"

#include "C3Trade.h"
#include "TradeOffer.h"

#include "SlicObject.h"
#include "SlicEngine.h"

#include "TradeOfferPool.h"
#include "UnitDynArr.h"
#include "UnitData.h"

#include "tiledmap.h"
#include "radarmap.h"

#include "Civilisation.h"

#include "debugmemory.h"
#include "log.h"
#include "debugcallstack.h"
#include "player.h"

extern World *g_theWorld;
extern ProfileDB *g_theProfileDB;
extern Player **g_player;
extern TiledMap		*g_tiledMap;
extern RadarMap		*g_radarMap;

#include "tiledmap.h"
extern TiledMap *g_tiledMap;

#include "chatbox.h"
extern ChatBox  *g_chatBox;

#include "network.h"
#include "net_info.h"
#include "net_action.h"
#include "order.h"
#include "director.h"

#include "AdvanceRecord.h"
#include "UnitRecord.h"

#define NO_TIMING_FILENAME_YET "notiming.txt"

static char memory_file_name[300] = NO_TIMING_FILENAME_YET;
static char time_stamp[300] = NO_TIMING_FILENAME_YET;

#ifndef _BFR_











	#ifdef TIME_TEST

		static char timing_file_name[300] = NO_TIMING_FILENAME_YET;
	#endif

#endif

extern Director *g_director;

extern sint32 g_scenarioUsePlayerNumber;

RobotInterface::RobotInterface()
{
    MapPoint *size = g_theWorld->GetSize();

    m_the_stop_player = 1;
	if(g_scenarioUsePlayerNumber != 0)
		m_the_stop_player = g_scenarioUsePlayerNumber;

    m_my_turn_is_over = FALSE;

    sint32 z, x;
    m_map_value = new MapValueStruct**[size->z];
    for (z=0; z<size->z; z++) {
        m_map_value[z] = new MapValueStruct *[size->x];
        for (x=0; x<size->x; x++) {
            m_map_value[z][x] = new MapValueStruct [size->y];
        }
    }

    Clear();
}

void RobotInterface::ResizeMap(sint32 sx, sint32 sy, sint32 sz)
{

    MapPoint *size = g_theWorld->GetSize();
    sint32 z, x;

    for (z=0; z<size->z; z++) {
        for (x=0; x<size->x; x++) {
            delete[] m_map_value[z][x];
        }
        delete[] m_map_value[z];
    }
    delete[] m_map_value;

    m_map_value = new MapValueStruct**[sz];
    for (z=0; z<sz; z++) {
        m_map_value[z] = new MapValueStruct *[sx];
        for (x=0; x<sx; x++) {
            m_map_value[z][x] = new MapValueStruct [sy];
        }
    }
}

void RobotInterface::Clear()
{
    sint32 i, j;

    for (i=0; i<k_MAX_PLAYERS; i++) {
        for (j=0; j<_MAX_PATH; j++) {
             m_name[i][j] = 0;
        }
        m_robot_dll[i] = NULL;
       	m_robot_create[i] = NULL;

        m_accumulated_frame_time[i] = 0;

        m_new_age_unit[i].m_underwater = FALSE;
        m_new_age_unit[i].m_space = FALSE;
        m_new_age_unit[i].m_wormhole= FALSE;
    }
}

RobotInterface::RobotInterface(CivArchive &archive)

{
    sint32 z, x;
    MapPoint *size = g_theWorld->GetSize();
    m_map_value = new MapValueStruct**[size->z];
    for (z=0; z<size->z; z++) {
        m_map_value[z] = new MapValueStruct *[size->x];
        for (x=0; x<size->x; x++) {
            m_map_value[z][x] = new MapValueStruct [size->y];
        }
    }

    Clear();
    Serialize(archive);
}

RobotInterface::~RobotInterface()
{
    sint32 i, j;
    BOOL searching;




































    MapPoint *size = g_theWorld->GetSize();
    sint32 z, x;

    for (z=0; z<size->z; z++) {
        for (x=0; x<size->x; x++) {
            delete[] m_map_value[z][x];
        }
        delete[] m_map_value[z];
    }
    delete[] m_map_value;

}

BOOL RobotInterface::Init(sint32 n_ai, PLAYER_INDEX player_idx[k_MAX_PLAYERS])
{
    sint32 i;

    InitStaticSettle();







































    return TRUE;
}

BOOL RobotInterface::AddRobotPlayer(const sint32 player_index)
{



































    return TRUE;
}

BOOL RobotInterface::AttachRobotTo(sint32 playerIndex)
{
	Assert(g_player[playerIndex]);
	if(g_player[playerIndex]) {


















		sint32 i, n, j, k;
		Player *p = g_player[playerIndex];
		n = p->m_all_cities->Num();
		for(i = 0; i < n; i++) {
			MapPoint pos;
			p->m_all_cities->Access(i).GetPos(pos);
			BuildQueue *bq = p->m_all_cities->Access(i).AccessData()->GetCityData()->GetBuildQueue();




			bq->ClearAllButHead();

			p->m_all_cities->Access(i).AccessData()->GetCityData()->KillAllTradeRoutes();
			if(g_network.IsClient()) {
				g_network.SendAction(new NetAction(NET_ACTION_KILL_ALL_TRADE_ROUTES,
												   p->m_all_cities->Access(i).m_id));
			}

			BSetID id = p->m_all_cities_id->Get(i);
			RegisterCreatedCity(p->m_all_cities->Access(i),
								playerIndex,
								id,
								CAUSE_NEW_CITY_INITIAL,
								pos);
			if(bq->GetHead()) {
				RegisterSetBuildFront(playerIndex,
									  p->m_all_cities->Access(i),
									  bq->GetHead()->m_category,
									  bq->GetHead()->m_type);
			}
		}

		for(i = 0; i < g_theAdvanceDB->NumRecords(); i++) {
			if(p->m_advances->HasAdvance(i)) {
				RegisterLearnedScience(playerIndex, i, CAUSE_SCI_INITIAL);
			}
		}

		n = p->m_all_armies->Num();
		Army a;
		sint32 unit_type[k_MAX_ARMY_SIZE];
        sint32 unit_hp[k_MAX_ARMY_SIZE];
		for(i = 0; i < n; i++) {
			a = p->m_all_armies->Access(i);

			for(j = 0; j < a.Num(); j++) {
				unit_type[j] = a[j].GetType();
                unit_hp[j] = sint32(a[j].GetHP());
			}

			BSetID id = p->m_all_armies_id->Get(i);
			RegisterCreatedArmy(playerIndex,
								id,
								CAUSE_NEW_ARMY_INITIAL,
								0,
								a.Num(),
								unit_type, unit_hp);

			for(j = 0; j < a.Num(); j++) {
				if(a[j].GetNumCarried() > 0) {
					UnitDynamicArray *list = a[j].AccessData()->GetCargoList();
					for(k = 0; k < list->Num(); k++) {
						id = p->m_all_armies_id->Get(i);
						RegisterInsertCargo(playerIndex,
											id.GetVal(),
											list->Access(k).GetType(),
                                            (sint32)list->Access(k).GetHP()
                                            );
					}
				}
			}
		}

		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(p->HasContactWith(i))
				RegisterContactMade(playerIndex, i);
		}




		n = g_theTradeOfferPool->GetNumTradeOffers();
		for(i = 0; i < n; i++) {
			C3TradeOffer *c3to = new C3TradeOffer(g_theTradeOfferPool->GetTradeOffer(i), playerIndex);
			c3to->AddRef();

			c3to->Release();
		}

		g_player[playerIndex]->m_playerType = PLAYER_TYPE_ROBOT;
		if(g_network.IsHost()) {
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_ATTACH_ROBOT,
										  playerIndex));
		}

		if(g_selected_item->GetCurPlayer() == playerIndex) {
			m_my_turn_is_over = TRUE;
			if(g_network.IsActive()) {
				if(playerIndex == g_network.GetPlayerIndex()) {

					g_director->AddEndTurn();
				} else if(g_network.IsLocalPlayer(playerIndex)) {

					g_turn->EndThisTurnBeginNewTurn(FALSE);
				}
			}
		}
	}
	return TRUE;
}

BOOL RobotInterface::DetachRobotFrom(sint32 playerIndex)
{

	if(!g_player[playerIndex])
		return FALSE;










	if(g_player[playerIndex]) {
		g_player[playerIndex]->m_playerType = PLAYER_TYPE_HUMAN;




		m_my_turn_is_over = FALSE;

		if(g_network.IsHost()) {
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_DETACH_ROBOT,
										  playerIndex));
		}
	}




	sint32 rounded = sint32(g_player[playerIndex]->m_materialsTax * 10.0);
	g_player[playerIndex]->SetMaterialsTax(double(rounded) / 10.0);

	return TRUE;
}

BOOL RobotInterface::LoadCom(sint32 p)
{
    sint32 i;











































    return TRUE;
}

void RobotInterface::Serialize(CivArchive &archive)

{
    sint32 i;

    CHECKSERIALIZE

    BOOL r;
    BOOL has_robot;
    if (archive.IsStoring()) {

       archive << m_the_stop_player;
       archive.PutSINT32(m_my_turn_is_over);
       archive << m_nAIPlayers;
       archive.Store((uint8*) m_name, k_MAX_PLAYERS * _MAX_PATH);
       archive.Store((uint8*) m_accumulated_frame_time, k_MAX_PLAYERS * sizeof(uint32));
       for (i=0; i<k_MAX_PLAYERS; i++) {
           if (IsLivingRobot(i)) {
               archive.PutSINT32(1);





           } else {
               archive.PutSINT32(0);
           }
       }

        sint32 player_idx;
        for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) {
            archive.PutSINT32(m_new_age_unit[player_idx].m_underwater);
            archive.PutSINT32(m_new_age_unit[player_idx].m_space);
            archive.PutSINT32(m_new_age_unit[player_idx].m_wormhole);
        }

        sint32 z, x, y;
        MapPoint *size = g_theWorld->GetSize();
        for (z=0; z<size->z; z++) {
            for (x=0; x<size->x; x++) {
                for (y=0; y<size->y; y++) {
                    archive << m_map_value[z][x][y].m_settle_value;
                }
            }
        }

    } else {

       archive >> m_the_stop_player;
       m_my_turn_is_over =  archive.GetSINT32();
       archive >> m_nAIPlayers;
       archive.Load((uint8 *)m_name, k_MAX_PLAYERS * _MAX_PATH);


















       archive.Load((uint8 *)m_accumulated_frame_time, k_MAX_PLAYERS * sizeof(uint32));
       for (i=0; i<k_MAX_PLAYERS; i++) {
           has_robot = archive.GetSINT32();
           if (has_robot) {



















           }
       }

        sint32 player_idx;
        for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) {
            m_new_age_unit[player_idx].m_underwater = archive.GetSINT32();
            m_new_age_unit[player_idx].m_space = archive.GetSINT32();
            m_new_age_unit[player_idx].m_wormhole = archive.GetSINT32();
        }

        sint32 z, x, y;
        MapPoint *size = g_theWorld->GetSize();
        for (z=0; z<size->z; z++) {
            for (x=0; x<size->x; x++) {
                for (y=0; y<size->y; y++) {
                    archive >> m_map_value[z][x][y].m_settle_value;
                }
            }
        }

		if(g_scenarioUsePlayerNumber != 0)
			m_the_stop_player = g_scenarioUsePlayerNumber;
    }

    CHECKSERIALIZE
}

extern double g_ai_count;
BOOL RobotInterface::AiIsMovingThisTurn() const

{




	return FALSE;
}

BOOL RobotInterface::BeginTurn()
{
    PLAYER_INDEX p = g_selected_item->GetCurPlayer();
    m_accumulated_frame_time[p] = 0;
    m_my_turn_is_over=FALSE;
    return FALSE;
}





void C3Robot_Make_Time_Stamp_String()
{

	time_t the_local_time;
	struct tm *ptr;
	char *dest_ch = time_stamp;

	the_local_time=time(NULL);

	ptr=localtime(&the_local_time);
	char *ch = asctime(ptr);

	while (*ch != 0)
	{
		if (isalnum(*ch))
			*dest_ch = *ch;
		else
			*dest_ch = '_';

		ch++;
		dest_ch++;
	}

	*dest_ch = 0;

}

static DWORD start_timing_time = 0;
static DWORD last_round_time = 0;


#ifdef TIME_TEST





void C3Robot_Time_Logging()
{
	int turn_num = g_turn->GetRound();

	DWORD my_time = GetTickCount();




	if ((turn_num % 10) == 0)
	{
		FILE *time_file;


		if ((turn_num == 0) || (!strcmp(timing_file_name, NO_TIMING_FILENAME_YET)))
		{

			if (!strcmp(time_stamp, NO_TIMING_FILENAME_YET))
			{

				C3Robot_Make_Time_Stamp_String();

			}

			sprintf(timing_file_name, "Timing_%s.txt", time_stamp);

			time_file = fopen(timing_file_name, "w");
			start_timing_time = my_time;

			fprintf(time_file, "Turn\tSeconds\tThis_Round\n");

			fclose(time_file);
		}
		else
		{
			time_file = fopen(timing_file_name, "a");

			fprintf(time_file, "%d\t%d\t%5.2f\n", turn_num,
				(int) ((my_time - start_timing_time)/1000.0),
				(my_time - last_round_time)/1000.0
				);

			fclose(time_file);
		}
	}

	last_round_time = my_time;
}
#endif

#ifdef _DEBUG
#ifdef _MEMORYLOGGING


static long int memory_last_turn = 1000000000;






void C3Robot_Memory_Logging()
{

	if (!strcmp(memory_file_name, NO_TIMING_FILENAME_YET))
	{

		if (!strcmp(time_stamp, NO_TIMING_FILENAME_YET))
		{

			C3Robot_Make_Time_Stamp_String();

		}

		sprintf(memory_file_name, "memorylog_%s.txt", time_stamp);

	}

#ifdef _DEBUG_MEMORY

	FILE *f = NULL;

	long int total_memory, dll_memory, exe_memory;

	f = fopen(memory_file_name, "at");
	if (!f) {
		f = fopen(memory_file_name, "wt");
		fprintf(f, "Round\tEXE\tDLL\n");
	}
	if (f) {
		dll_memory = DebugMemory_GetTotalFromDLL();
		exe_memory = DebugMemory_GetTotalFromEXE();
		total_memory = dll_memory + exe_memory;

		fprintf(f, "%d\t%d\t%d\n", g_turn->GetRound(),
				exe_memory, dll_memory);

		fclose(f);
	}


















#else

	FILE *f = NULL;
	f = fopen(memory_file_name, "at");
	if (!f) {
		f = fopen(memory_file_name, "wt");
		Assert(f);
		if (f)
		{
			fprintf(f, "Round\tBytes\n");
		}
	}
	if (f) {
		_CrtMemState state;
		_CrtMemCheckpoint( &state );

		fprintf(f, "%d\t%d\n", g_turn->GetRound(), state.lSizes[_NORMAL_BLOCK]);

		fclose(f);
	}
#endif DEBUG_MEMORY
}
#endif
#endif


sint32 RobotInterface::ProcessRobot(const uint32 target_milliseconds)
{
    BOOL time_is_remaining = TRUE;
    uint32 start_frame_ms = GetTickCount();
    uint32 end_frame_by = start_frame_ms + target_milliseconds;
    char debug_str[80];

#ifdef _DEBUG
    sint32 finite_loop_count=0;
#endif

    while(time_is_remaining) {
        time_is_remaining = FALSE;
        Assert(finite_loop_count++ < 40);

        PLAYER_INDEX p = g_selected_item->GetCurPlayer();
        if ((NULL == g_player[p]) ||
            (g_player[p]->IsDead())) {







			if(!g_network.IsActive()) {
				g_turn->EndThisTurnBeginNewTurn(FALSE);
			} else {

				if(p == g_selected_item->GetVisiblePlayer()) {
					g_director->AddEndTurn();
				} else {
					g_turn->EndThisTurnBeginNewTurn(FALSE);
				}
			}

            m_my_turn_is_over = FALSE;
            return 0;
        }

        if (g_player[p] && g_player[p]->m_playerType != PLAYER_TYPE_ROBOT) {
            if (!g_network.IsActive()) {
                if (m_the_stop_player != p) {
					if(!g_turn->IsHotSeat() && !g_turn->IsEmail()) {
						g_turn->EndThisTurnBeginNewTurn(FALSE);
					} else {
						m_the_stop_player = p;
						g_selected_item->SetPlayerOnScreen(p);
						g_slicEngine->BlankScreen(TRUE);
						g_director->NextPlayer();
						g_director->AddCopyVision();
						g_tiledMap->InvalidateMix();
						g_tiledMap->InvalidateMap();
						g_tiledMap->Refresh();
						g_radarMap->Update();
						g_turn->InformMessages();
						g_turn->SendNextPlayerMessage();
					}
                }
            }
            return 0;
        }

        if (!m_my_turn_is_over) {

			if (m_accumulated_frame_time[p] < 1)  {




					m_accumulated_frame_time[p] = 1;

					m_my_turn_is_over = TRUE;

			}

			if (!m_my_turn_is_over && (GetTickCount() < end_frame_by)) {

				m_accumulated_frame_time[p] += GetTickCount() - start_frame_ms;
			}











		}

        if (m_my_turn_is_over) {

			sint32 oldVisPlayer = g_selected_item->GetVisiblePlayer();
			if(g_network.IsClient()) {
				g_director->AddEndTurn();
			} else {
				if (m_the_stop_player != p) {
					g_turn->EndThisTurnBeginNewTurn(FALSE);
				} else if(g_network.IsActive() && g_network.IsHost() && g_player[m_the_stop_player]->GetPlayerType() == PLAYER_TYPE_ROBOT) {
					if(g_selected_item->GetCurPlayer() == g_selected_item->GetVisiblePlayer()) {
						g_director->AddEndTurn();
					} else {
						g_turn->EndThisTurnBeginNewTurn(FALSE);
					}
				}
            }




            if (g_selected_item->GetVisiblePlayer() != oldVisPlayer)
			{
                g_selected_item->SetPlayerOnScreen((PLAYER_INDEX)-1);
                g_tiledMap->InvalidateMix();
	     	    g_tiledMap->InvalidateMap();
		        g_tiledMap->Refresh();
		        g_radarMap->Update();

			}





			if (p == 0)
			{
				#ifdef TIME_TEST


				C3Robot_Time_Logging();

				#endif

				#ifdef _DEBUG
				#ifdef _MEMORYLOGGING


				C3Robot_Memory_Logging();
				#endif
				#endif

			}

            sint32 time_left = sint32(end_frame_by) - sint32(GetTickCount());
            if ((m_the_stop_player != p) && (sint32(m_accumulated_frame_time[p]) < time_left)) {
                time_is_remaining = TRUE;
            }

            m_accumulated_frame_time[p] = 0;
        }
    }

    return 1;
}

void  RobotInterface::DoWholeTurn()
{
    ProcessRobot(10000000);
}

void RobotInterface::CleanupARobot(sint32 p)
{

    Assert(0 <= p);
    Assert(p < k_MAX_PLAYERS);











}






BOOL RobotInterface::RegisterCreatedArmy(PLAYER_INDEX owner, BSetID &id,
   const  CAUSE_NEW_ARMY cause, const uint32 hc_id,  sint32 nUnits,
   sint32 unit_type[k_MAX_ARMY_SIZE], sint32 unit_hp[k_MAX_ARMY_SIZE])
{











    return FALSE;
}

BOOL RobotInterface::RegisterDeadArmy(PLAYER_INDEX owner, BSetID &id,
                                      const  CAUSE_REMOVE_ARMY cause)
{

















    return TRUE;
}

void RobotInterface::RegisterDeathDifferential (double adjustment, PLAYER_INDEX player, MapPoint &position)
{









}

void RobotInterface::Load_AIP(char *aip_file_name, PLAYER_INDEX player)
{
	Assert (player >= 0);
	Assert (player < k_MAX_PLAYERS);







}

void RobotInterface::Set_AILog_Verbosity(sint32 log_level, PLAYER_INDEX player)
{
	Assert (player >= 0);
	Assert (player < k_MAX_PLAYERS);








}

void RobotInterface::RegisterAddInstallation(PLAYER_INDEX owner,
    MapPointData &pos, sint32 inst_type)
{








}

void RobotInterface::RegisterRemoveInstallation(PLAYER_INDEX owner,
    MapPointData &pos, sint32 inst_type)
{
    MapPoint ipos;
    ipos.Iso2Norm(pos);






}

BOOL RobotInterface::RegisterDeadUnit(PLAYER_INDEX owner,
									  uint32 army_id,
									  const sint32 unit_type)
{





	return TRUE;
}

BOOL RobotInterface::RegisterDeadCity(PLAYER_INDEX owner, Unit &killme,
									  BSetID &id,
                                      const  CAUSE_REMOVE_CITY cause)
{














    return TRUE;
}

void RobotInterface::RegisterCityDiedAt(MapPoint &ipos)
{











}

BOOL RobotInterface::RegisterCreatedCity(Unit u,  PLAYER_INDEX owner,
    BSetID &id, const  CAUSE_NEW_CITY cause, MapPoint &pos)
{



















    return TRUE;
}

void RobotInterface::RegisterClearCargo(sint32 owner, uint32 army_id)
{





}

void RobotInterface::RegisterInsertCargo(sint32 owner, uint32 army_id, const sint32 unit_type,
    sint32 hp)
{





}

void RobotInterface::RegisterUnloadCargo(sint32 owner, uint32 army_id, const sint32 unit_type,
    sint32 hp, CAUSE_REMOVE_ARMY cause )
{





}

BOOL RobotInterface::RegisterArmyWasJoined(PLAYER_INDEX owner, uint32 target,
        sint32 nUnits, sint32 unit_type[k_MAX_ARMY_SIZE],
        sint32 unit_hp[k_MAX_ARMY_SIZE])
{












    return FALSE;
}

BOOL RobotInterface::RegisterYourArmyWasMoved(const PLAYER_INDEX owner,
    const uint32 army_id, const MapPoint &pos)
{









    return FALSE;
}

void RobotInterface::RegisterHisArmyTeleported (PLAYER_INDEX his_owner, uint32 i_moved,
        MapPointData *new_pos)
{










}

BOOL RobotInterface::RegisterCreateBuilding(PLAYER_INDEX owner,
    uint32 city_id, sint32 blg_type)
{









    return FALSE;
}

BOOL RobotInterface::RegisterLostBuilding(PLAYER_INDEX owner,
    uint32 city_id, sint32 blg_type)
{









    return FALSE;
}

BOOL RobotInterface::RegisterNewGovernment(PLAYER_INDEX owner,
    uint32 city_id, sint32 government_type)
{
    Assert(0 <= owner);
    Assert(owner < k_MAX_PLAYERS);









	return TRUE;
}

void RobotInterface::RegisterNewWGF(PLAYER_INDEX owner, sint32 w, sint32 g, sint32 f)
{
    Assert(0 <= owner);
    Assert(owner < k_MAX_PLAYERS);






}

BOOL RobotInterface::RegisterNewCapitolBuilding(PLAYER_INDEX owner,
    uint32 city_id)
{









    return FALSE;
}

BOOL RobotInterface::RegisterCreateWonder(PLAYER_INDEX owner,
    uint32 city_id, sint32 wonder_type)
{












    return FALSE;
}

BOOL RobotInterface::RegisterLearnedScience(PLAYER_INDEX owner, sint32 idx_sci, CAUSE_SCI cause)
{
    Assert(0 <= owner);
    Assert(owner < k_MAX_PLAYERS);










	return FALSE;
}

void RobotInterface::RegisterNewPlayer(PLAYER_INDEX player_id,
     CAUSE_NEW_PLAYER cause, NEW_PLAYER_IS type, PLAYER_INDEX old_owner)
{
    sint32 i;

    if (NEW_PLAYER_IS_ROBOT == type) {
        m_nAIPlayers++;
        LoadCom(player_id);
        AddRobotPlayer(player_id);
    }








}

void RobotInterface::RegisterDeadPlayer(PLAYER_INDEX player_id)
{
    sint32 i;





















}


BOOL RobotInterface::IsComLoaded(const sint32 p) const
{

	return FALSE;
}

#include "profileDB.h"
extern ProfileDB *g_theProfileDB;
extern RobotInterface *g_robot_interface;

BOOL IsThisPlayerARobot(sint32 p)
{
    Assert(0 <= p);
    Assert(p < k_MAX_PLAYERS);
    if ((p < 0) || (k_MAX_PLAYERS <= p)) return FALSE;









	Assert(g_player);
	Assert(g_player[p]);
	return (g_player[p]->GetPlayerType() != PLAYER_TYPE_HUMAN);
}

sint32 RobotInterface::GetNumFuzzySections() const
{
    PLAYER_INDEX p = g_selected_item->GetCurPlayer();








	return 0;
}

sint32 RobotInterface::GetNumFuzzyVariables(const sint32 idx_section) const
{
     PLAYER_INDEX p = g_selected_item->GetCurPlayer();








    return 0;
}

void RobotInterface::GetFuzzyGraph(const sint32 idx_section, const sint32 idx_variable,
        char *&label, double &minx, double &maxx, double &miny, double &maxy,
        sint32 &num_graphs, sint32 &num_x, double ***height, double &defuzz_val)
{
     PLAYER_INDEX p = g_selected_item->GetCurPlayer();








}

void RobotInterface::ReloadFuzzyLogic(const PLAYER_INDEX p, const BOOL run_rules)
{
    Assert(p);












}

void RobotInterface::ResetFuzzyInput(const sint32 idx_section,
                                     const sint32 idx_variable,
        double new_defuzz_val)
{
    PLAYER_INDEX p = g_selected_item->GetCurPlayer();







}

void RobotInterface::DumpStats()
{
    PLAYER_INDEX p = g_selected_item->GetCurPlayer();







}

#define INSURFACE(x, y) (x >= 0 && y >= 0 && x < pSurface->Width() && y < pSurface->Height())
extern void WhackScreen();


BOOL RobotInterface::RegisterCityAttack(PLAYER_INDEX owner,
										uint32 my_city_id,
										PLAYER_INDEX his_owner,
										uint32 his_unit_id,
										UNIT_ORDER_TYPE attack_type)
{










	return FALSE;
}


BOOL RobotInterface::RegisterDiplomaticRequest(PLAYER_INDEX owner,
											   IC3DiplomaticRequest *request)
{








	return FALSE;
}

BOOL RobotInterface::RegisterDiplomaticResponse(PLAYER_INDEX owner,
												IC3DiplomaticRequest *request)
{








	return FALSE;
}

void RobotInterface::RegisterAttack(PLAYER_INDEX agressor,
									PLAYER_INDEX defender)
{

















}

void RobotInterface::RegisterFightWinner(sint32 attack_owner, sint32 attack_start_num,
    sint32 defense_owner, sint32 defense_start_num, sint32 winner)
{











}

void RobotInterface::RegisterContactMade(PLAYER_INDEX contacter,
										 PLAYER_INDEX contactee)
{





}

void RobotInterface::RegisterLeaveOurLands(PLAYER_INDEX i_leave, PLAYER_INDEX get_off_me)
{





}

void RobotInterface::RegisterLeaveOurLandsExpired(PLAYER_INDEX i_leave, PLAYER_INDEX get_off_me)
{





}

void RobotInterface::RegisterPlayerInfo(PLAYER_INDEX index,
										C3PlayerInfo *pi)
{







}

void RobotInterface::SetInputLogComment(MBCHAR *comment)
{








}

BOOL RobotInterface::GetFliLoggingEnabled(sint32 owner)
{









	return FALSE;
}

void RobotInterface::SetFliLoggingEnabled(sint32 owner, BOOL enabled)
{







}

void RobotInterface::SendTradeBid(PLAYER_INDEX bidder,
								  PLAYER_INDEX biddee,
								  uint32 fromCity_id,
								  sint32 resource,
								  uint32 toCity_id,
								  sint32 price,
								  uint32 bidId)
{










}

void RobotInterface::RegisterTradeOffer(TradeOffer &offer)
{












}

void RobotInterface::RemoveTradeOffer(TradeOffer &offer)
{









}

void RobotInterface::AcceptTradeOffer(TradeOffer &offer,
									  Unit &sourceCity,
									  Unit &destCity,
									  PLAYER_INDEX player)
{


































}

void RobotInterface::RegisterNewContinents()
{
    sint32 p;









}
void RobotInterface::RegisterBrokenCeaseFire(PLAYER_INDEX breaker,
											 PLAYER_INDEX breakee)
{










}

void RobotInterface::RegisterBrokenAlliance(PLAYER_INDEX breaker,
											PLAYER_INDEX breakee)
{










}

void RobotInterface::ToggleSuperFastDebugMode(PLAYER_INDEX player, BOOL on)
{






}

void RobotInterface::RegisterHeBuiltUnit(PLAYER_INDEX he_did_it,
                                        sint32 unit_type)
{








































}

void RobotInterface::RegisterHostileAction(UNIT_ORDER_TYPE attack_type,
    PLAYER_INDEX target_player, uint32 target_army_id, PLAYER_INDEX his_owner,
     uint32 his_unit_id, MapPointData *target_pos)
{

    if (target_player >= 0 && target_player < k_MAX_PLAYERS && IsLivingRobot(target_player)) {
#if 0

        char str1[80];
        char str2[80];

        switch (attack_type) {
    	case UNIT_ORDER_INVESTIGATE_CITY:
            sprintf (str1, "UNIT_ORDER_INVESTIGATE_CITY");
            break;
	    case UNIT_ORDER_NULLIFY_WALLS:
            sprintf (str1, "UNIT_ORDER_NULLIFY_WALLS");
            break;
	    case UNIT_ORDER_STEAL_TECHNOLOGY:
            sprintf (str1, "UNIT_ORDER_STEAL_TECHNOLOGY");
            break;
	    case UNIT_ORDER_ASSASSINATE:
            sprintf (str1, "UNIT_ORDER_ASSASSINATE");
            break;
    	case UNIT_ORDER_INVESTIGATE_READINESS:
            sprintf (str1, "UNIT_ORDER_INVESTIGATE_READINESS");
            break;
	    case UNIT_ORDER_SUE:
            sprintf (str1, "UNIT_ORDER_SUE");
            break;
    	case UNIT_ORDER_FRANCHISE:
            sprintf (str1, "UNIT_ORDER_FRANCHISE");
            break;
	    case UNIT_ORDER_SUE_FRANCHISE:
            sprintf (str1, "UNIT_ORDER_SUE_FRANCHISE");
            break;
	    case UNIT_ORDER_CAUSE_UNHAPPINESS:
            sprintf (str1, "UNIT_ORDER_CAUSE_UNHAPPINESS");
            break;
	    case UNIT_ORDER_PLANT_NUKE:
            sprintf (str1, "UNIT_ORDER_PLANT_NUKE");
            break;
	    case UNIT_ORDER_SLAVE_RAID:
            sprintf (str1, "UNIT_ORDER_SLAVE_RAID");
            break;
	    case UNIT_ORDER_UNDERGROUND_RAILWAY:
            sprintf (str1, "UNIT_ORDER_UNDERGROUND_RAILWAY");
            break;
	    case UNIT_ORDER_INCITE_UPRISING:
            sprintf (str1, "UNIT_ORDER_INCITE_UPRISING");
            break;
	    case UNIT_ORDER_BIO_INFECT:
            sprintf (str1, "UNIT_ORDER_BIO_INFECT");
            break;
	    case UNIT_ORDER_NANO_INFECT:
            sprintf (str1, "UNIT_ORDER_NANO_INFECT");
            break;
	    case UNIT_ORDER_CONVERT:
            sprintf (str1, "UNIT_ORDER_CONVERT");
            break;
	    case UNIT_ORDER_INDULGENCE:
            sprintf (str1, "UNIT_ORDER_INDULGENCE");
            break;
	    case UNIT_ORDER_SOOTHSAY:
            sprintf (str1, "UNIT_ORDER_SOOTHSAY");
            break;
	    case UNIT_ORDER_INJOIN:
            sprintf (str1, "UNIT_ORDER_INJOIN");
            break;
	    case UNIT_ORDER_THROW_PARTY:
            sprintf (str1, "UNIT_ORDER_THROW_PARTY");
            break;
	    case UNIT_ORDER_INCITE_REVOLUTION:
            sprintf (str1, "UNIT_ORDER_INCITE_REVOLUTION");
            break;
	    case UNIT_ORDER_CREATE_PARK:
            sprintf (str1, "UNIT_ORDER_CREATE_PARK");
            break;
	    case UNIT_ORDER_BOMBARD:
            sprintf (str1, "UNIT_ORDER_BOMBARD");
            break;
	    case UNIT_ORDER_EXPEL:
            sprintf (str1, "UNIT_ORDER_EXPEL");
            break;
	    case UNIT_ORDER_ENSLAVE_SETTLER:
            sprintf (str1, "UNIT_ORDER_ENSLAVE_SETTLER");
            break;
	    case UNIT_ORDER_RUSTLE:
            sprintf (str1, "UNIT_ORDER_RUSTLE");
            break;
	    case UNIT_ORDER_FINISH_ATTACK:
            sprintf (str1, "UNIT_ORDER_FINISH_ATTACK");
            break;
        case UNIT_ORDER_PILLAGE:
            sprintf (str1, "UNIT_ORDER_PILLAGE");
            break;
	    case UNIT_ORDER_INTERCEPT_TRADE:
            sprintf (str1, "UNIT_ORDER_INTERCEPT_TRADE");
            break;
        default:
            Assert(0);
        }

        sprintf (str2, "%s target %d attacker %d",  str1,
            target_player, his_owner);

     g_chatBox->AddLine(2, str2);
#endif





    }

}

void RobotInterface::RegisterPollution(MapPoint &pos)
{

    MapPoint norm_pos;

    sint32 owner = g_theWorld->GetOwner(pos);








}

void RobotInterface::RegisterHeNuked(PLAYER_INDEX he_did_it, PLAYER_INDEX his_target)
{
    sint32 player_idx;









}

void RobotInterface::RegisterPiracy(PLAYER_INDEX he_did_it, PLAYER_INDEX source_owner, uint32 src_id,
                PLAYER_INDEX dest_owner, uint32 dest_id, sint32 resource)
{













}


































void RobotInterface::RegisterRegardIncrease(PLAYER_INDEX who, sint32 amount)
{










}

void RobotInterface::GetFZRegard(sint32 me, sint32 him, sint32 &diplomatic, sint32 &unit,
    sint32 &bonus)
{













}

void RobotInterface::SetFZRegard(sint32 me, sint32 him, sint32 diplomatic, sint32 unit,
    sint32 bonus)
{














}

void RobotInterface::GetAllRegard(sint32 player_idx,
    double i_like[k_MAX_PLAYERS])
{













}

void RobotInterface::RegisterAgreementViolation(AGREEMENT_TYPE agreement,
												PLAYER_INDEX violator,
												PLAYER_INDEX violatee,
												sint32 rounds)
{




















}

void RobotInterface::RegisterAgreementAgainstMe(PLAYER_INDEX first_player,
    PLAYER_INDEX second_player, PLAYER_INDEX target_player, double agreement_str)
{







}

char *RobotInterface::GetAIPName(sint32 player)
{







	return NULL;
}

void RobotInterface::RegisterRemoveBuildNode(PLAYER_INDEX owner,
							 uint32 cityid,
							 sint32 build_category,
							 sint32 unit_type)
{














}

void RobotInterface::RegisterSetBuildFront(PLAYER_INDEX owner,
							 uint32 cityid,
							 sint32 build_category,
							 sint32 unit_type)
{













}

BOOL RobotInterface::IWantToAttackRevealedArmy (uint32 revealed_army_id,
        MapPoint &pos, PLAYER_INDEX reveled_player,
        uint32 attacking_army_id, PLAYER_INDEX attacking_player)
{














	return FALSE;
}

BOOL RobotInterface::ValidateArmyID(PLAYER_INDEX owner, uint32 u_id, sint32 unit_num)
{









	return FALSE;
}

BOOL RobotInterface::IsLivingRobot(PLAYER_INDEX owner) const
{
    if (owner == -1) return FALSE;

    Assert(0 <= owner);
    Assert(owner < k_MAX_PLAYERS);

    if (g_player[owner] == NULL)
        return FALSE;

    if (g_player[owner]->m_isDead)
        return FALSE;





	return TRUE;
}


void RobotInterface::ForceRegard(sint32 ofPlayer, sint32 forPlayer,
								 double toRegard)
{






}

void RobotInterface::ForceHate(sint32 player, sint32 forPlayer)
{






}
