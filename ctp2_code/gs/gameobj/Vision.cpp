//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Map visibility handling
// Id           : $Id:$
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
// - Corrected strange vision behaviour at the top row.
// - Corrected strange visibility patterns ("see tile" counter underflow).
// - Removed causes of memory leak reports (static variables).
// - Unseen cells are now always created and not only in the case if a 
//   tile improvement is under construction there. - Dec. 21st 2004 Martin Gühmann
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Vision.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "tiledmap.h"
#include "dynarr.h"
#include "SelItem.h"
#include "civarchive.h"
#include "UnseenCellQuadTree.h"
#include "Army.h"
#include "Cell.h"
#include "cellunitlist.h"
#include "player.h"
#include "network.h"
#include "net_info.h"

extern World         *g_theWorld;
extern TiledMap      *g_tiledMap;
extern SelectedItem  *g_selected_item;
extern Player       **g_player;

Vision::Vision(sint32 owner, bool amOnScreen)
{
	sint32 x;
	MapPoint *size;
	m_owner = owner;
	m_amOnScreen = amOnScreen;

	size = g_theWorld->GetSize();
	m_array = new uint16*[size->x];
	m_width = size->x;
	m_height = size->y;
	m_xyConversion = (m_height - (2 * m_width)) / 2;
	m_isYwrap = g_theWorld->IsYwrap();
	
	for(x = 0; x < size->x; x++) {
		m_array[x] = new uint16[size->y];
		memset(m_array[x], 0, sizeof(uint16) * size->y);
	}
	m_unseenCells = new UnseenCellQuadTree(m_width, m_height, g_theWorld->IsYwrap());
}

Vision::~Vision()
{
	DeleteUnseenCells();
	if(m_unseenCells)
		delete m_unseenCells;

	if(m_array) {
		for(sint32 x = 0; x < m_width; x++) {
			if(m_array[x])
				delete [] m_array[x];
		}
		delete [] m_array;
	}
}


void Vision::Clear()
{
	for(sint32 x = 0; x < m_width; x++) {
		for(sint32 y = 0; y < m_height; y++) {
			m_array[x][y] = 0;
		}
	}
	

	if(m_unseenCells)
		delete m_unseenCells;
	m_unseenCells = new UnseenCellQuadTree(m_width, m_height, g_theWorld->IsYwrap());
}

void Vision::AddExplored(MapPoint pos, double radius)
{
	FillCircle(pos, radius, CIRCLE_OP_ADD);
}

void Vision::SetTheWholeWorldExplored()
{
	sint32 x, y;
	for (x=0; x<m_width; x++) { 
		for (y=0; y<m_height; y++) { 
			m_array[x][y] |= k_EXPLORED_BIT; 
		} 
	} 
}

void Vision::SetTheWholeWorldUnexplored()
{
	sint32 x, y;
	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			
			if(!(m_array[x][y] & k_VISIBLE_REFERENCE_MASK)) {
				m_array[x][y] = 0;
			}
		}
	}
}

void Vision::SetTheWholeWorldUnseen()
{
	
	sint32 x, y;
	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			m_array[x][y] &= k_EXPLORED_BIT;
		}
	}
}

sint32 g_fog_toggle; 


extern sint32 g_god;

bool Vision::IsExplored(MapPoint pos) const 
{
	if (g_fog_toggle) {
		return true;
	}

	
	if (g_god) return true;

	if(g_player[m_owner] && g_player[m_owner]->m_hasGlobalRadar)
		return true;

	Convert(pos);
	return (m_array[pos.x][pos.y] & k_EXPLORED_BIT) ? true : false;
}

void Vision::AddVisible(MapPoint pos, double radius, bool &revealed_unexplored,
						DynamicArray<MapPoint> *removeadd)
{
	m_revealedUnexplored	= false;
	FillCircle(pos, radius, CIRCLE_OP_ADD, removeadd);
	if(m_revealedUnexplored)
		revealed_unexplored = true;

	if(removeadd) {
		for(sint32 i = 0; i < removeadd->Num(); i++) {
			
			
			
			if(m_amOnScreen && g_tiledMap) {
				
				if (g_tiledMap->TileIsVisible(removeadd->Access(i).x, removeadd->Access(i).y))
					g_tiledMap->RedrawTile(&removeadd->Access(i));
			}
		}
	}
}

void Vision::RemoveVisible(MapPoint pos, double radius,
						   DynamicArray<MapPoint> *removeadd)
{
	FillCircle(pos, radius, CIRCLE_OP_SUBTRACT, removeadd);
}


void Vision::AddRadar(MapPoint pos, double radius)
{
	FillCircle(pos, radius, CIRCLE_OP_ADD_RADAR);
}

bool Vision::IsVisible(MapPoint pos) const 
{
	if (g_fog_toggle) {
		return true;
	}
	
	if (g_god)
		return true;

	if(g_player[m_owner] && g_player[m_owner]->m_hasGlobalRadar)
		return true;

	Assert(pos.x >= 0 && pos.x < m_width &&
		   pos.y >= 0 && pos.y < m_height);
	Convert(pos);
	return ((m_array[pos.x][pos.y] & k_VISIBLE_REFERENCE_MASK) > 0);
}

bool Vision::GetLastSeen(const MapPoint &pos, UnseenCellCarton &ucell)
{
	if(g_god) return false;

	if(g_player[m_owner] && g_player[m_owner]->m_hasGlobalRadar)
		return false;

	return m_unseenCells->GetAt(pos, ucell);
}

void Vision::MergeMap(Vision *src)
{
	Cell *cell;
	sint32 i, n;
	

	Assert(m_width == src->m_width && m_height == src->m_height);
	Assert(m_owner != src->m_owner);

	sint32 x, y;
	UnseenCellCarton ucell;

	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			uint16 *myVersion = &m_array[x][y];
			uint16 *hisVersion = &src->m_array[x][y];
			MapPoint point(x, y);

			if(!(*hisVersion & k_EXPLORED_BIT))
				continue;

			if(*myVersion & k_EXPLORED_BIT) {
				
				if((*myVersion & k_VISIBLE_REFERENCE_MASK) > 0) {
					
					continue;
				}

				if((*hisVersion & k_VISIBLE_REFERENCE_MASK) > 0) {
					
					
					
					Unconvert(point);
					if(m_unseenCells->RemoveAt(point, ucell)) {
						delete ucell.m_unseenCell;
					}
					cell = g_theWorld->GetCell(point);
					n = cell->GetNumUnits();
					for(i = 0; i < n; i++) {
						if(cell->AccessUnit(i).GetVisibility() & (1 << src->m_owner)) {
							if(cell->AccessUnit(i).GetVisibilityClass() & 1) {
								cell->AccessUnit(i).ForceVisibleDuration(m_owner, 1);
							}
						}
					}
					// Added by Martin Gühmann
					// Create always an unseen cell
					if(cell->GetCity().m_id != 0)
						cell->GetCity().SetVisible(m_owner);
						
					ucell.m_unseenCell = new UnseenCell(point);
					m_unseenCells->Insert(ucell);
				}
				
			} else {
				

				*myVersion |= k_EXPLORED_BIT;
				Unconvert(point);

				
				cell = g_theWorld->GetCell(point);
				n = cell->GetNumUnits();
				for(i = 0; i < n; i++) {
					if(cell->AccessUnit(i).GetVisibility() & (1 << src->m_owner)) {
						if(cell->AccessUnit(i).GetVisibilityClass() & 1) {
							cell->AccessUnit(i).ForceVisibleDuration(m_owner, 1);
						}
					}
				}

				if(cell->GetCity().m_id != (0) && 
				   cell->GetCity().GetVisibility() & (1 << src->m_owner)) {
					cell->GetCity().SetVisible(m_owner);
				}
				
				if((*hisVersion & k_VISIBLE_REFERENCE_MASK) == 0) {
					
					if(src->m_unseenCells->GetAt(point, ucell)) {
						UnseenCellCarton newUnseen(
							new UnseenCell(ucell.m_unseenCell));
						
						m_unseenCells->Insert(newUnseen);
					} 
				} 
			}
		}
	}
}


bool Vision::MergePoint(sint32 x, sint32 y)
{
	uint16 *myVersion = &m_array[x][y];
	uint16 *hisVersion = &m_mergeFrom->m_array[x][y];
	MapPoint point(x, y);
	UnseenCellCarton ucell;
	
	if(!(*hisVersion & k_EXPLORED_BIT))
		return false;

	if(*myVersion & k_EXPLORED_BIT) {
		
		if((*myVersion & k_VISIBLE_REFERENCE_MASK) > 0) {
			
			return false;
		}
		
		if((*hisVersion & k_VISIBLE_REFERENCE_MASK) > 0) {
			
			
			
			Unconvert(point);
			if(m_unseenCells->RemoveAt(point, ucell)) {
				delete ucell.m_unseenCell;
			}
			return true;
		}
		
	} else {
		
		
		*myVersion |= k_EXPLORED_BIT;
		
		if((*hisVersion & k_VISIBLE_REFERENCE_MASK) == 0) {
			
			Unconvert(point);
			if(m_mergeFrom->m_unseenCells->GetAt(point, ucell)) {
				UnseenCellCarton newUnseen(
					new UnseenCell(ucell.m_unseenCell));
				
				m_unseenCells->Insert(newUnseen);
				return true;
			}
		}
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//
// Name       : Vision::FillCircle
//
// Description: Perform an operation within a circle of a certain point.
//
// Parameters : centerRC        : the point (RC coordinate)
//              radius          : the radius of the circle
//              op              : the operation to perform
//
// Globals    : g_theWorld      : world information
//
// Returns    : removeadd       : filled with changed points
//
// Remark(s)  : The center is now passed as an RC coordinate (used in most of 
//              the game). dx and dy are still for Convert-ed coordinates,
//              and map wrap checks are easier when using XY coordinates. 
//              Quite a mess.
//
//                          Convert       XY          RC
//              step
//              N           +1 -1        0 -2       +1 -2
//              NE          +1  0       +1 -1       +1 -1
//              E           +1 +1       +2  0       +1  0
//              SE           0 +1       +1 +1        0 +1
//
//              Assumption: centerRC is valid and on the map.
//
//----------------------------------------------------------------------------
void Vision::FillCircle
(
	MapPoint const &			centerRC,
	double const				radius, 
	CIRCLE_OP const				op,
	DynamicArray<MapPoint> *	removeadd
)
{
	double const	r	= radius + 0.5;	
	sint32 const	rsq	= sint32(r * r);

	for (sint16 dx = sint16(r); dx >= 0; dx--) 
	{
		bool	incircle = false;
		
		for (sint16 dy = sint16(r); dy >= 0; dy--) 
		{
			if (incircle || ((dx * dx) + (dy * dy) <= rsq)) 
			{ 
				incircle = true;

				// do always
				{ 
					OrthogonalPoint	testXY(centerRC);
					testXY.Move(MapPointData(+ dx + dy, - dx + dy));
					if (testXY.IsValid())
					{
						DoFillCircleOp(testXY.GetRC(), op, removeadd);
					}
				}

				if (dx != 0) 
				{
					OrthogonalPoint	testXY(centerRC);
					testXY.Move(MapPointData(- dx + dy, + dx + dy));
					if (testXY.IsValid())
					{
						DoFillCircleOp(testXY.GetRC(), op, removeadd);
					}
				}

				if (dy != 0) 
				{
					OrthogonalPoint testXY(centerRC);
					testXY.Move(MapPointData(+ dx - dy, - dx - dy));
					if (testXY.IsValid())
					{
						DoFillCircleOp(testXY.GetRC(), op, removeadd);
					}
				}

				if ((dx != 0) && (dy != 0))
				{
					OrthogonalPoint testXY(centerRC);
					testXY.Move(MapPointData(- dx - dy, + dx - dy)); 
					if (testXY.IsValid())
					{
						DoFillCircleOp(testXY.GetRC(), op, removeadd);
					}
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : Vision::DoFillCircleOp
//
// Description: Handle the actual circle operation.
//
// Parameters : posRC           : the point (RC coordinate)
//              op              : the operation
//
// Globals    : g_theWorld      : world information
//
// Returns    : removeadd       : filled with changed points
//
// Remark(s)  : Assumption: iso will be a valid map location after applying
//              wrapping.
//              Convert + Unconvert will take care of the wrap.
//
//----------------------------------------------------------------------------
void Vision::DoFillCircleOp(const MapPoint &posRC, CIRCLE_OP op, 
							DynamicArray<MapPoint> *removeadd)
{
	MapPoint	pos(posRC);
	Convert(pos);
	MapPoint	iso(pos);
	Unconvert(iso);

	bool		redraw	= false;
	uint16 *	entry	= &m_array[pos.x][pos.y];
	switch(op) 
	{
		case CIRCLE_OP_ADD:
			if(!((*entry) & k_EXPLORED_BIT)) {
				redraw = true;
				m_revealedUnexplored = true;
			} else if (((*entry) & k_VISIBLE_REFERENCE_MASK) == 0) {
				redraw = true;
				UnseenCellCarton ucell;
				if(m_unseenCells->RemoveAt(iso, ucell)) {
					delete ucell.m_unseenCell;
				}
			}
			*entry = (*entry + 1) | k_EXPLORED_BIT;
			if(redraw && removeadd) {
				if(removeadd->Del(iso)) {
					
					redraw = false;
				} else {
					removeadd->Insert(iso);
					redraw = false;
				}
			}
			break;
		case CIRCLE_OP_SUBTRACT:
			if ((*entry) & k_VISIBLE_REFERENCE_MASK)
			{
				--(*entry);
			}
			// else: No action: keep counter at 0
			if(((*entry) & k_VISIBLE_REFERENCE_MASK) == 0) {
				Cell *cell = g_theWorld->GetCell(iso);
				// Added by Martin Gühmann
				AddUnseen(iso);

				if(removeadd) {
					
					removeadd->Insert(iso);
					redraw = false;
				} else {
					redraw = true;
				}
			}
			break;
		case CIRCLE_OP_ADD_RADAR:
		{
			static CellUnitList army;
			army.Clear();
			g_theWorld->GetArmy(iso, army);
			sint32 i, n = army.Num();
			for(i = 0; i < n; i++) {
				army[i].SetRadar(m_owner);
			}
			break;
		}
		case CIRCLE_OP_MERGE:
			if(MergePoint(pos.x, pos.y)) {
				if(g_selected_item->GetVisiblePlayer() == m_owner) {
					g_tiledMap->GetLocalVision()->ModifyPoint(this, pos.x, pos.y);
				}
				redraw = true;
			}
			break;
		default:
			Assert(false);
			break;
	}

	if(g_tiledMap && redraw && m_amOnScreen) 
	{
		g_tiledMap->RedrawTile(&iso);
	}
}

void Vision::AddUnseen(const MapPoint &point)
{
	if(!IsExplored(point)) {
		return;
	}


	if(!IsVisible(point)) {
		UnseenCellCarton alreadyUnseen;
		if(m_unseenCells->GetAt(point, alreadyUnseen)) {
			return;
		}
		if(g_network.IsHost()) {
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_ADD_UNSEEN,
										  m_owner, g_network.PackedPos(point)));
		}

		UnseenCellCarton unseen(new UnseenCell(point));
		m_unseenCells->Insert(unseen);
	}
}
void Vision::Copy(const Vision *copy)
{
	Assert(m_width == copy->m_width && m_height == copy->m_height);
	if(m_width != copy->m_width || m_height != copy->m_height) {
		return;
	}
	m_owner = copy->m_owner;
	for(sint32 x = 0; x < m_width; x++) {
		memcpy(m_array[x], copy->m_array[x], m_height * sizeof(uint16));
	}
	
	
	

	m_unseenCells->Clear();
	DynamicArray<UnseenCellCarton> array;
	copy->m_unseenCells->BuildList(array);
	sint32 i, n = array.Num();
	for(i = 0; i < n; i++) {
		UnseenCellCarton newUnseen(new UnseenCell(array[i].m_unseenCell));
		m_unseenCells->Insert(newUnseen);
	}
}

void Vision::Serialize(CivArchive &archive)
{
	CHECKSERIALIZE

	if(archive.IsStoring()) {
		archive.StoreChunk((uint8 *)&m_width, ((uint8 *)&m_amOnScreen)+sizeof(m_amOnScreen));

		for(sint32 x = 0; x < m_width; x++) {
			archive.Store((uint8 *)m_array[x], sizeof(m_array[0][0]) * m_height) ;



		}
		DynamicArray<UnseenCellCarton> array;
		m_unseenCells->BuildList(array);
		sint32 i, n = array.Num();
		archive << n;
		for(i = 0; i < n; i++) {
			array[i].m_unseenCell->Serialize(archive);
		}
	} else {
		archive.LoadChunk((uint8 *)&m_width, ((uint8 *)&m_amOnScreen)+sizeof(m_amOnScreen));


		if(m_array) {
			for(sint32 x = 0; x < m_width; x++) {
				if(m_array[x])
					delete [] m_array[x];
			}
			delete [] m_array;
		}

		m_array = new uint16*[m_width];
		for(sint32 x = 0; x < m_width; x++) {
			m_array[x] = new uint16[m_height];
			archive.Load((uint8 *)m_array[x], sizeof(m_array[0][0]) * m_height) ;



		}
		sint32 i, n;
		archive >> n;
		for(i = 0; i < n; i++) {
			UnseenCell *ucell = new UnseenCell(archive);
			UnseenCellCarton uc(ucell);
			m_unseenCells->Insert(uc);
		}
	}
}


void Vision::CopyCircle(Vision *src, const MapPoint &center, sint32 radius)
{
	m_mergeFrom = src;
	FillCircle(center, (double) radius, CIRCLE_OP_MERGE);
}






void Vision::ModifyPoint(Vision *src, sint32 x, sint32 y)
{
	UnseenCellCarton ucell, ucell2;
	MapPoint pnt(x,y);

	if(m_array[x][y] & k_EXPLORED_BIT) {
		if(m_array[x][y] & k_VISIBLE_REFERENCE_MASK) {
			
			return;
		} else {
			if(m_unseenCells->RemoveAt(pnt, ucell2)) {
				delete ucell2.m_unseenCell;
			}
			if(src->m_array[x][y] & k_VISIBLE_REFERENCE_MASK) {
				
			} else {
				
				if(src->m_unseenCells->GetAt(pnt, ucell)) {
					ucell2.m_unseenCell = new UnseenCell(ucell.m_unseenCell);
					m_unseenCells->Insert(ucell2);
				}
			}
		}
	} else {
		
		if(src->m_array[x][y] & k_EXPLORED_BIT) {
			
			m_array[x][y] |= k_EXPLORED_BIT;
			if(src->m_unseenCells->GetAt(pnt, ucell)) {
				
				ucell2.m_unseenCell = new UnseenCell(ucell.m_unseenCell);
				m_unseenCells->Insert(ucell2);
			}
		}
	}
}

void Vision::DeleteUnseenCells()
{
	DynamicArray<UnseenCellCarton>  array;
	m_unseenCells->BuildList(array, 0xffffffff);

	sint32 i;
	for(i = 0; i < array.Num(); i++) {
		delete array[i].m_unseenCell;
	}
}

void Vision::AddUnseen(UnseenCell *ucell)
{
	UnseenCellCarton unseen(ucell);
	m_unseenCells->Insert(unseen);
}

void Vision::GetUnseenCellList(DynamicArray<UnseenCellCarton> &array)
{
	array.Clear();
	m_unseenCells->BuildList(array);
}

void Vision::ClearUnseen()
{
	m_unseenCells->Clear();
}
