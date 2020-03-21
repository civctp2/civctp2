//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Map visibility handling
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
// - Corrected strange vision behaviour at the top row.
// - Corrected strange visibility patterns ("see tile" counter underflow).
// - Removed causes of memory leak reports (static variables).
// - Unseen cells are now always created and not only in the case if a
//   tile improvement is under construction there. - Dec. 21st 2004 Martin Gühmann
// - Corrected cleanup (using the old m_width!) when reloading in Serialize.
//
//----------------------------------------------------------------------------
//
/// \file   Vision.cpp
/// \brief  Handling of tile visibility (definitions)

#include "c3.h"
#include "Vision.h"

#include "Army.h"
#include "Cell.h"
#include "cellunitlist.h"
#include "civarchive.h"
#include "dynarr.h"
#include "player.h"                   // g_player
#include "net_info.h"
#include "network.h"
#include "SelItem.h"                  // g_selected_item
#include "tiledmap.h"                 // g_tiledMap
#include "UnseenCellQuadTree.h"
#include "World.h"                    // g_theWorld

extern sint32 g_god;

/// The fog of war has been toggled off.
/// This should make all tiles visible once explored.
sint32 g_fog_toggle;

namespace
{
    /// Indicate whether a player can see the whole map
    /// \param a_Index Index of the player
    bool IsAllVisible(PLAYER_INDEX const & a_Index)
    {
        return g_god
            || g_fog_toggle
            || (g_player[a_Index] && g_player[a_Index]->m_hasGlobalRadar);
    }
}

Vision::Vision(sint32 owner, bool amOnScreen)
:
    m_width                 (0),
    m_height                (0),
    m_owner                 (owner),
    m_xyConversion          (0),
    m_isYwrap               (false),
    m_amOnScreen            (amOnScreen),
    m_array                 (NULL),
    m_unseenCells           (NULL),
    m_mergeFrom             (NULL)
{
	Assert(g_theWorld);
	MapPoint * size = g_theWorld->GetSize();
	m_width = size->x;
	m_height = size->y;
	m_xyConversion = (m_height - (2 * m_width)) / 2;
	m_isYwrap = g_theWorld->IsYwrap();

	m_array = new uint16*[m_width];
	for (int x = 0; x < m_width; x++)
	{
		m_array[x] = new uint16[m_height];
		std::fill(m_array[x], m_array[x] + m_height, 0);
	}
	m_unseenCells = new UnseenCellQuadTree(m_width, m_height, m_isYwrap);
}

Vision::~Vision()
{
	DeleteUnseenCells();
	delete m_unseenCells;

	for (int x = 0; x < m_width; x++)
	{
		delete [] m_array[x];
	}
	delete [] m_array;
}

void Vision::Clear()
{
	for (int x = 0; x < m_width; x++)
	{
		std::fill(m_array[x], m_array[x] + m_height, 0);
	}

	delete m_unseenCells;
	m_unseenCells = new UnseenCellQuadTree(m_width, m_height, m_isYwrap);
}

void Vision::AddExplored(MapPoint pos, double radius)
{
	FillCircle(pos, radius, CIRCLE_OP_ADD); // makes also visible, i.e. removes ucell from m_unseenCells
	FillCircle(pos, radius, CIRCLE_OP_SUBTRACT); // removes visibility, i.e. adds ucell to m_unseenCells
}

void Vision::SetTheWholeWorldExplored()
{
	for (int x = 0; x < m_width; x++)
	{
		for (int y = 0; y < m_height; y++)
		{
			m_array[x][y] |= k_EXPLORED_BIT;
		}
	}
}

void Vision::SetTheWholeWorldUnexplored()
{
	for (int x = 0; x < m_width; x++)
	{
		for (int y = 0; y < m_height; y++)
		{

			if(!(m_array[x][y] & k_VISIBLE_REFERENCE_MASK))
			{
				m_array[x][y] = 0;
			}
		}
	}
}

void Vision::SetTheWholeWorldUnseen()
{
	for (int x = 0; x < m_width; x++) {
		for (int y = 0; y < m_height; y++) {
			m_array[x][y] &= k_EXPLORED_BIT;
		}
	}
}

bool Vision::IsExplored(MapPoint pos) const
{
	if (IsAllVisible(m_owner))
	{
		return true;
	}

	Convert(pos);
	return (m_array[pos.x][pos.y] & k_EXPLORED_BIT) ? true : false;
}

void Vision::AddVisible(MapPoint pos, double radius,
						DynamicArray<MapPoint> *removeadd)
{
	FillCircle(pos, radius, CIRCLE_OP_ADD, removeadd);

	if(removeadd)
	{
		for(sint32 i = 0; i < removeadd->Num(); i++)
		{
			if(m_amOnScreen && g_tiledMap)
			{
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
	if (IsAllVisible(m_owner))
	{
		return true;
	}

	Assert(pos.x >= 0 && pos.x < m_width &&
		   pos.y >= 0 && pos.y < m_height);
	Convert(pos);
	return ((m_array[pos.x][pos.y] & k_VISIBLE_REFERENCE_MASK) > 0);
}

bool Vision::GetLastSeen(const MapPoint &pos, UnseenCellCarton &ucell) const
{
	/// @todo Check why g_fog_toggle is not tested here
	if(g_god) return false;

	if(g_player[m_owner] && g_player[m_owner]->m_hasGlobalRadar)
		return false;

	return m_unseenCells->GetAt(pos, ucell);
}

void Vision::MergeMap(Vision *src)
{
	Assert(m_width == src->m_width && m_height == src->m_height);
	Assert(m_owner != src->m_owner);

	UnseenCellCarton ucell;

	for (int x = 0; x < m_width; x++)
	{
		for (int y = 0; y < m_height; y++)
		{
			uint16 *hisVersion = &src->m_array[x][y];
			if(!(*hisVersion & k_EXPLORED_BIT)) // He does not see this tile so go to next tile.
				continue;

			uint16 *myVersion = &m_array[x][y];
			if(*myVersion & k_EXPLORED_BIT) // We know of this tile
			{
				if((*myVersion & k_VISIBLE_REFERENCE_MASK) > 0) // We see this tile
				{
					continue;
				}

				MapPoint point(x, y);
				Unconvert(point);

				if
				  (
				    (*hisVersion & k_VISIBLE_REFERENCE_MASK) > 0
				    || g_theWorld->GetOwner(point) == src->GetOwner()
				  ) // He sees this tile but we don't or he owns it
				{
					if(m_unseenCells->RemoveAt(point, ucell))
					{
						delete ucell.m_unseenCell;
					}
					Cell* cell = g_theWorld->GetCell(point);
					sint32 n = cell->GetNumUnits();
					for(sint32 i = 0; i < n; i++)
					{
						if(cell->AccessUnit(i).GetVisibility() & (1 << src->m_owner))
						{
							if(cell->AccessUnit(i).GetVisibilityClass() & 1)
							{
								cell->AccessUnit(i).ForceVisibleDuration(m_owner, 1);
							}
						}
					}
					// Create always an unseen cell
					if(cell->GetCity().m_id != 0)
						cell->GetCity().SetVisible(m_owner);

					ucell.m_unseenCell = new UnseenCell(point);
					m_unseenCells->Insert(ucell);
				}
				else
				{
					// Unfortunately there is no way to figure out whether our unseen cell is older than his
				}
			}
			else
			{
				*myVersion |= k_EXPLORED_BIT;
				MapPoint point(x, y);
				Unconvert(point);

				Cell* cell = g_theWorld->GetCell(point);
				sint32 n = cell->GetNumUnits();
				for(sint32 i = 0; i < n; i++)
				{
					if(cell->AccessUnit(i).GetVisibility() & (1 << src->m_owner))
					{
						if(cell->AccessUnit(i).GetVisibilityClass() & 1)
						{
							cell->AccessUnit(i).ForceVisibleDuration(m_owner, 1);
						}
					}
				}

				if(cell->GetCity().m_id != (0) &&
				   cell->GetCity().GetVisibility() & (1 << src->m_owner))
				{
					cell->GetCity().SetVisible(m_owner);
				}

				if((*hisVersion & k_VISIBLE_REFERENCE_MASK) == 0)
				{
					if(src->m_unseenCells->GetAt(point, ucell))
					{
						UnseenCellCarton newUnseen(
							new UnseenCell(ucell.m_unseenCell));

						m_unseenCells->Insert(newUnseen);
					}
				}
			}
		}
	}
}

void Vision::ModifyPoint(Vision *src, sint32 x, sint32 y)
{
	if(m_array[x][y] & k_EXPLORED_BIT)
	{
		if(m_array[x][y] & k_VISIBLE_REFERENCE_MASK)
		{
			return;
		}
		else
		{
			MapPoint pnt(x,y);
			UnseenCellCarton ucell2;
			if(m_unseenCells->RemoveAt(pnt, ucell2))
			{
				delete ucell2.m_unseenCell;
			}
			if(src->m_array[x][y] & k_VISIBLE_REFERENCE_MASK)
			{
				// Something is missing here?
			}
			else
			{
				UnseenCellCarton ucell;
				if(src->m_unseenCells->GetAt(pnt, ucell))
				{
					ucell2.m_unseenCell = new UnseenCell(ucell.m_unseenCell);
					m_unseenCells->Insert(ucell2);
				}
			}
		}
	}
	else
	{
		if(src->m_array[x][y] & k_EXPLORED_BIT)
		{
			m_array[x][y] |= k_EXPLORED_BIT;
			MapPoint pnt(x,y);
			UnseenCellCarton ucell;
			if(src->m_unseenCells->GetAt(pnt, ucell))
			{
				UnseenCellCarton ucell2;

				ucell2.m_unseenCell = new UnseenCell(ucell.m_unseenCell);
				m_unseenCells->Insert(ucell2);
			}
		}
	}
}

bool Vision::MergePoint(sint32 x, sint32 y)
{
	uint16 *hisVersion = &m_mergeFrom->m_array[x][y];
	if(!(*hisVersion & k_EXPLORED_BIT))
		return false;

	uint16 *myVersion = &m_array[x][y];

	if(*myVersion & k_EXPLORED_BIT)
	{
		if((*myVersion & k_VISIBLE_REFERENCE_MASK) > 0)
		{
			return false;
		}

		if((*hisVersion & k_VISIBLE_REFERENCE_MASK) > 0)
		{
			MapPoint point(x, y);
			Unconvert(point);
			UnseenCellCarton ucell;
			if(m_unseenCells->RemoveAt(point, ucell))
			{
				delete ucell.m_unseenCell;
			}
			return true;
		}
	}
	else
	{
		*myVersion |= k_EXPLORED_BIT;

		if((*hisVersion & k_VISIBLE_REFERENCE_MASK) == 0)
		{
			MapPoint point(x, y);
			Unconvert(point);
			UnseenCellCarton ucell;
			if(m_mergeFrom->m_unseenCells->GetAt(point, ucell))
			{
				UnseenCellCarton newUnseen(
					new UnseenCell(ucell.m_unseenCell));

				m_unseenCells->Insert(newUnseen);
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
		case CIRCLE_OP_ADD:{ // local scope is essential, without seen tile do not get converted to unseen tiles
			if(!((*entry) & k_EXPLORED_BIT))
			{
				redraw = true;
			}
			else if (((*entry) & k_VISIBLE_REFERENCE_MASK) == 0)
			{
				redraw = true;
				UnseenCellCarton ucell;
				if(m_unseenCells->RemoveAt(iso, ucell))
				{
					delete ucell.m_unseenCell;
				}
			}

			RevealTradeRouteState(iso); // reveal trade route state

			*entry = (*entry + 1) | k_EXPLORED_BIT;
			if(redraw && removeadd)
			{
				if(removeadd->Del(iso))
				{
					// Something missing here?
				}
				else
				{
					removeadd->Insert(iso);
				}
				redraw = false;
			}
			break;
		}
		case CIRCLE_OP_SUBTRACT:{

			Assert(((*entry) & k_VISIBLE_REFERENCE_MASK) != 0);

			if ((*entry) & k_VISIBLE_REFERENCE_MASK)
			{
				--(*entry);
			}
			// else: No action: keep counter at 0
			if(((*entry) & k_VISIBLE_REFERENCE_MASK) == 0)
			{
				AddUnseen(iso);

				if(removeadd)
				{
					removeadd->Insert(iso);
					redraw = false;
				}
				else
				{
					redraw = true;
				}
			}
			break;
		}
		case CIRCLE_OP_ADD_RADAR:{
			CellUnitList army;
			g_theWorld->GetArmy(iso, army);
			sint32 n = army.Num();
			for(sint32 i = 0; i < n; i++)
			{
				army[i].SetRadar(m_owner);
			}
			break;
		}
		case CIRCLE_OP_MERGE:{
			if(MergePoint(pos.x, pos.y))
			{
				redraw = true;
			}
			break;
		}
		default:{
			Assert(false);
			break;
		}
	}

	if(g_tiledMap && redraw && m_amOnScreen)
	{
		g_tiledMap->RedrawTile(&iso);
	}
}

void Vision::AddUnseen(const MapPoint &point)
{
	if(!IsExplored(point))
	{
		return;
	}

	if(!IsVisible(point))
	{
		UnseenCellCarton alreadyUnseen;
		if(m_unseenCells->GetAt(point, alreadyUnseen))
		{
			return;
		}
		if(g_network.IsHost())
		{
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_ADD_UNSEEN,
										  m_owner, g_network.PackedPos(point)));
		}

		UnseenCellCarton unseen(new UnseenCell(point)); // Memory leak?
		m_unseenCells->Insert(unseen);
	}
}

void Vision::UpdateUnseen(const MapPoint &posRC){ //// in contrast to AddUnseen also update already seen ucells even for unexplored tiles
    
    MapPoint point(posRC);
    Convert(point); // essential for m_array[point.x][point.y]
    MapPoint iso(point);
    Unconvert(iso); // needed for RemoveAt
    UnseenCellCarton ucell;
    bool removed= false;
  
    if(m_unseenCells->RemoveAt(iso, ucell)){
	removed= true; // there was a ucell before, i.e. tile is not in vision or unexplored
	delete ucell.m_unseenCell;
	}
    
    if(!IsExplored(iso) || removed){ // if tile is unexplored or ucell was removed, i.e. not in vision (IsExplored needs iso NOT point!!!)
	ucell.m_unseenCell= new UnseenCell(iso);
	m_unseenCells->Insert(ucell); 
	if(g_network.IsHost())
	    g_network.Enqueue(new NetInfo(NET_INFO_CODE_ADD_UNSEEN, m_owner, g_network.PackedPos(point)));
	}
    
    if(g_tiledMap && m_amOnScreen && (!IsExplored(iso) || removed)){ // only redraw if tile is not in vision to avoid overdrawing of units
	g_tiledMap->RedrawTile(&iso); // without the ucell is only redrawn on the next redraw of the map
	}

    m_array[point.x][point.y] |= k_EXPLORED_BIT; // AddExplored(point, 0) similar but contains execution of RevealTradeRouteState (OK for city pos, not revealing other trade routes of that city)
    }

void Vision::RevealTradeRouteState(const MapPoint &iso){ // reaveals or removes trade route drawing depending on trade route state
    Cell *cell = g_theWorld->GetCell(iso);
    for(sint32 i = 0; i < cell->GetNumTradeRoutes(); i++){
	TradeRoute route= cell->GetTradeRoute(i);
	if(route.IsValid()){
	    if(route.IsActive()){
		route.AddSeenByBit(m_owner);
		RevealTradeRouteCities(route);
		RevealTradeRouteTiles(route);
		}
	    else { // either inactive or cancelled but not yet deleted (because m_seenBy not yet 0)
		route.RemoveSeenByBit(m_owner);
		}
	    }
	else { // in case route is invalid (should not happen)
	    route.RemoveSeenByBit(m_owner);
	    }
	}
    }

void Vision::RevealTradeRouteTiles(TradeRoute route){ //// reveal trade route tiles (info gotten from the traders) to avoid knowledge of isolated cities (which can lead to invalid routes) and to make discovery of new trade route even more interesting and helpful for the AI
    
    const DynamicArray<MapPoint>* path= route.GetPath();
    sint32 const num = path->Num();
    for (sint32 i = 0; i < num; i++){
	UpdateUnseen(path->Get(i));
	}
    }

void Vision::RevealTradeRouteCities(TradeRoute route){ //// reveal source and destination cities (info gotten from the traders) to avoid human advantage from deducing unseen city pos (which the AI can't)
    
    RevealCity(route.GetSource());
    RevealCity(route.GetDestination());
    }

void Vision::RevealCity(Unit city){ //// reveal city and its current state
    UnseenCellCarton ucell;
    MapPoint point;

    if(city.m_id){
	city.SetVisible(m_owner);
	UpdateUnseen(city.RetPos());
	}
    }

void Vision::Copy(const Vision *copy)
{
	Assert(m_width == copy->m_width && m_height == copy->m_height);
	if(m_width != copy->m_width || m_height != copy->m_height)
	{
		return;
	}
	m_owner = copy->m_owner;
	for(sint32 x = 0; x < m_width; x++)
	{
		memcpy(m_array[x], copy->m_array[x], m_height * sizeof(uint16));
	}

	m_unseenCells->Clear();
	DynamicArray<UnseenCellCarton> array;
	copy->m_unseenCells->BuildList(array);
	sint32 n = array.Num();
	for (int i = 0; i < n; i++)
	{
		UnseenCellCarton newUnseen(new UnseenCell(array[i].m_unseenCell));
		m_unseenCells->Insert(newUnseen);
	}
}

void Vision::Serialize(CivArchive &archive)
{
	CHECKSERIALIZE

	if(archive.IsStoring())
	{
		sint8 empty = 0; // Needed for the padding in the original implementation
		archive.PutSINT16(m_width);
		archive.PutSINT16(m_height);
		archive.PutSINT32(m_owner);
		archive.PutSINT16(m_xyConversion);   // Unused
		archive.PutSINT8(empty);
		archive.PutSINT8(empty);
		sint32 tmp = static_cast<sint32>(m_isYwrap);
		archive.PutSINT32(tmp);
		tmp = static_cast<sint32>(m_amOnScreen);
		archive.PutSINT32(tmp);

		for (sint32 x = 0; x < m_width; x++)
		{
			archive.Store((uint8 *)m_array[x], sizeof(m_array[0][0]) * m_height) ;
		}
		DynamicArray<UnseenCellCarton> array;
		m_unseenCells->BuildList(array);
		sint32 n = array.Num();
		archive << n;
		for (sint32 i = 0; i < n; i++)
		{
			array[i].m_unseenCell->Serialize(archive);
		}
	}
	else
	{
		for (sint16 x = 0; x < m_width; x++)
		{
			delete [] m_array[x];
		}
		delete [] m_array;
		DeleteUnseenCells();
		delete m_unseenCells;

		sint8 empty    = 0; // Needed for the padding in the original implementation
		m_width        = archive.GetSINT16();
		m_height       = archive.GetSINT16();
		m_owner        = archive.GetSINT32();
		m_xyConversion = archive.GetSINT16();   // Unused
		empty          = archive.GetSINT8();
		empty          = archive.GetSINT8();
		m_isYwrap      = archive.GetSINT32() != FALSE;
		m_amOnScreen   = archive.GetSINT32() != FALSE;

		m_array         = new uint16*[m_width];
		for (sint16 y = 0; y < m_width; y++)
		{
			m_array[y] = new uint16[m_height];
			archive.Load((uint8 *)m_array[y], sizeof(m_array[0][0]) * m_height) ;
		}

		m_unseenCells   = new UnseenCellQuadTree(m_width, m_height, m_isYwrap);
		sint32 n;
		archive >> n;
		for (sint32 i = 0; i < n; i++)
		{
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

void Vision::DeleteUnseenCells()
{
	DynamicArray<UnseenCellCarton>  array;
	m_unseenCells->BuildList(array, 0xffffffff);

	for (int i = 0; i < array.Num(); i++)
	{
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
