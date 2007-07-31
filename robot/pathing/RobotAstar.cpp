

#include "c3.h"
#include "c3math.h"
#include "c3errors.h"
#include "Globals.h"



#include "RobotAstar.h"

#include "DynArr.h"
#include "Path.h"
#include "UnitAstar.h"


#include "XY_Coordinates.h"
#include "World.h"
#include "DynArr.h"
#include "Player.h"
#include "RandGen.h"
#include "UnitRec.h"
#include "RobotAstar.h"
#include "CivArchive.h"
#include "UnitRecord.h"

extern World *g_theWorld; 
  
STDMETHODIMP RobotAstar::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3RobotAstar)) {
		*obj = (IC3RobotAstar*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) RobotAstar::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) RobotAstar::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}
RobotAstar::RobotAstar(Player *p)
{
    
    m_refCount = 0; 
    m_callback = NULL;
    m_player = p; 
}

RobotAstar::RobotAstar(Player *p, CivArchive &archive)
{
    m_player = p; 
    m_callback = NULL; 
    Serialize(archive); 
}

void RobotAstar::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive << m_refCount;
    } else { 
        archive >> m_refCount; 
    }
}


sint32 RobotAstar::EntryCost(const MapPoint &prev, const MapPoint &pos,
       float & cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry)
	   
{

	BOOL r = UnitAstar::EntryCost(prev, pos, cost, is_zoc, entry); 


	if (r == FALSE)  { 
		return FALSE; 
	} 

    if (m_callback == NULL) 
        return r;

    
    static MapPoint tmpprev; 
    tmpprev.Iso2Norm(prev); 
    static MapPoint tmppos;
    tmppos.Iso2Norm(pos); 

	BOOL r2 =  (*m_callback)(r, &MapPointData(tmpprev), &MapPointData(tmppos), 
		&cost, is_zoc, entry); 

    if (cost <1.0) { 
        cost = 1.0; 
    } 

	if (((FALSE == r2) || (k_ASTAR_BIG <= cost)) &&  (entry != ASTAR_RETRY_DIRECTION)) { 
		return FALSE;
	}

    return r2; 
}      


void RobotAstar::RecalcEntryCost(AstarPoint *parent, 
    AstarPoint *node, float &new_entry_cost, 
    BOOL &new_is_zoc, ASTAR_ENTRY_TYPE &new_entry)
{
	new_entry = ASTAR_CAN_ENTER; 
	UnitAstar::RecalcEntryCost(parent, node, new_entry_cost, 
		new_is_zoc, new_entry);

	if (m_callback &&(new_entry_cost < k_ASTAR_BIG) && (new_entry == ASTAR_CAN_ENTER)) {
		static MapPoint tmpprev; 
		tmpprev.Iso2Norm(parent->m_pos); 
		static MapPoint tmppos;
		tmppos.Iso2Norm(node->m_pos); 

		BOOL r2 =  (*m_callback)(TRUE, &MapPointData(tmpprev), 
			&MapPointData(tmppos), &new_entry_cost, new_is_zoc, new_entry);         
	}
}

Path g_new_path; 
Path g_bad_path; 






















sint32 g_ec_called; 
sint32 g_path_count;
sint32 g_find_astar_bug;
sint32 g_no_more_pathing; 
sint32 g_robot_path_count;
#include "TurnCnt.h"

extern TurnCount  *g_turn;
BOOL RobotAstar::FindPath(RobotPathEval *cb, uint32 army_id,                          
    PATH_ARMY_TYPE pat, uint32 army_type, MapPointData *start, MapPointData *dest, 
    sint32 *bufSize, MapPointData ** buffer, sint32 *nPoints, float *total_cost, 
    BOOL made_up_can_space_launch,  BOOL made_up_can_space_land, 
    BOOL check_rail_launcher, BOOL pretty_path, sint32 cutoff, sint32 &nodes_opened,
	BOOL check_dest, const BOOL no_straight_lines, const BOOL check_units_in_cell)


{ 
    BOOL is_unknown_id;

    if (g_no_more_pathing) return FALSE; 

	PLAYER_INDEX owner = m_player->GetOwner(); 
    sint32 is_broken_path = FALSE; 
    static MapPoint isostart, isodest; 
    m_callback = cb; 

    isostart.Norm2Iso(*start); 
    isodest.Norm2Iso(*dest); 

    Army a;
    sint32 nUnits; 
    uint32 move_intersection; 
    uint32 move_union; 
    m_is_robot = TRUE; 

char str[20];
    switch (pat){
    case PATH_ARMY_TYPE_DB:
sprintf (str, "DB");
        a.m_id = (0); 
        nUnits = 1; 
        move_intersection = g_theUnitDB->Get(army_type)->GetMovementType(); 
        move_union = 0; 
        m_can_space_launch =  made_up_can_space_launch; 
        m_can_space_land =  made_up_can_space_land;
        m_army_minmax_move = float(g_theUnitDB->Get(army_type)->GetMaxMovePoints()); 
        m_army_can_expel_stealth = FALSE; 
        break; 
    case PATH_ARMY_TYPE_EXISTS:
sprintf (str, "EXIST");
        m_can_space_launch =  FALSE; 
        m_can_space_land =  FALSE;
        a = m_player->GetArmyList(army_id, is_unknown_id); 
        UnitAstar::InitArmy (a, nUnits, move_intersection, move_union,  m_army_minmax_move);        
        
        
        
        break; 
    case PATH_ARMY_TYPE_MADEUP:
sprintf (str, "MADEUP");
        a.m_id = (0); 
        nUnits = 1; 
        move_intersection = army_type; 
        move_union = 0; 
        m_can_space_launch =  made_up_can_space_launch; 
        m_can_space_land =  made_up_can_space_land;
        m_army_minmax_move = 300.0; 
        m_army_can_expel_stealth = FALSE; 
        break; 
    default:
        Assert(0); 
        return FALSE;         
    }

    g_ec_called = 0; 
    sint32 r; 

    nodes_opened=0; 
    g_robot_path_count++;


    if (UnitAstar::FindPath(a, nUnits, move_intersection, move_union, 
        isostart, owner, isodest, g_new_path, is_broken_path, g_bad_path, 
        *total_cost, TRUE,  check_rail_launcher, pretty_path, cutoff, 
        nodes_opened, check_dest, no_straight_lines, check_units_in_cell) == FALSE)  { 

        r = FALSE; 
    }  else { 
        r = TRUE; 
    } 











#ifdef LOG_ROBOT_ASTAR
    FILE *fout; 
    if (0 == g_path_count) { 
        fout = fopen("pathcount.txt", "w"); 
            g_path_count = 1; 
    } else { 
        fout = fopen("pathcount.txt", "a"); 
    } 

    if (r) { 
        fprintf (fout, "1, 0, %d, %d, %3.1f\n", nodes_opened, g_ec_called, *total_cost * 0.01); 
    } else { 
        fprintf (fout, "0, 1, %d, %d, %3.1f\n", nodes_opened, g_ec_called, *total_cost * 0.01); 
    } 

    fclose(fout); 

    if ((1000 < nodes_opened) && (8.0 < double(g_ec_called)/double(nodes_opened+1))) { 
        nodes_opened = 0; 

        if (g_find_astar_bug) { 
            m_can_space_launch =  FALSE; 
            m_can_space_land =  FALSE;
            m_army_minmax_move = 100.0; 

           static MapPoint pos; 
           sint32 mx = g_theWorld->GetXWidth();
           sint32 my = g_theWorld->GetYHeight();
           sint32 mz = g_theWorld->GetZHeight(); 

           uint16 c = 0;

           for (pos.z=0; pos.z < mz; pos.z++) { 
                for (pos.x=0; pos.x < mx; pos.x++) { 
                    for (pos.y=0; pos.y<my; pos.y++) { 
                            g_theWorld->SetColor(pos, c);
                    }
                }
           }
        UnitAstar::FindPath(a, nUnits, move_intersection, move_union, isostart,
            owner, isodest, g_new_path, is_broken_path, g_bad_path, *total_cost, 
            TRUE, check_rail_launcher, cutoff, nodes_opened, check_dest);
        }
    }

#endif

    if (FALSE == r) { 
        return r; 
    } 

	if (is_broken_path) { 
		return FALSE; 
	} 

	if (*bufSize < g_new_path.Num()) { 
		
		*buffer = new MapPointData [g_new_path.Num()];
		*bufSize = g_new_path.Num();  
		*nPoints = g_new_path.Num(); 
	} 
	
	sint32 count = 0; 
    static MapPoint pos, tmp;
	g_new_path.Start(pos);    
	for (count = 0;  !g_new_path.IsEnd(); g_new_path.Next(pos), count++ ) { 
        tmp.Iso2Norm(pos); 
		(*buffer)[count] = tmp; 
	} 
    tmp.Iso2Norm(pos); 
    (*buffer)[count] = tmp; 
    
    *nPoints = count+1;
    return TRUE; 
}
