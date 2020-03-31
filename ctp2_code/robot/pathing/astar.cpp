//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : A-star pathfinding
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
// - Prevent CtD (NULL-dereference)
// - Made the algorithm behave more like A-star (recompute an open node when a
//   new path is found that has a lower cost).
// - Made Win32 Optimized Test configuration compile. - Jul 19th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Astar.h"

#include "AVLHeap.h"
#include "Path.h"
#include "World.h"
#include "A_Star_Heuristic_Cost.h"

#ifdef PRINT_COSTS
#include "gfx_options.h"
#include "tiledmap.h"
#endif

sint32 g_search_count;

AVLHeap g_astar_mem;

void Astar_Init()
{
	g_astar_mem.InitHeap();
	g_search_count = 1;
}

void Astar_Cleanup()
{
	g_astar_mem.CleanUp();
}

#define k_MIN_MOVE_COST 10.0

float g_cost_factor = k_MIN_MOVE_COST;

#ifdef TRACK_ASTAR_NODES
float    g_nodes_opened   = 0;
float    g_nodes_inserted = 0;
int      g_paths_found    = 0;
long int g_paths_lengths  = 0;
long int g_closed_nodes   = 0;
#endif

extern void WhackScreen();
#ifdef _PLAYTEST
extern bool g_old_heuristic;
#else
bool g_old_heuristic = false;
#endif

float Astar::EstimateFutureCost(const MapPoint &pos, const MapPoint &dest)
{
	float best_heuristic;

	sint32 dist = pos.NormalizedDistance(dest);
	{
		best_heuristic = (float)g_theWorld->A_star_heuristic->Get_Minimum_Nearby_Movement_Cost(pos);

		Assert(best_heuristic > 0);
		Assert(best_heuristic < 500);

#ifdef TEST_DISTANCE

		sint32 idist;

		idist = g_theWorld->XY_Coords.Get_RC_Travel_Distance(pos, dest);

#ifdef SUPER_DEBUG_HEURISTIC
		if (dist == idist)
			g_theWorld->SetColor(pos,  dist);
		else
			g_theWorld->SetColor(pos,  dist*100 + idist);
#endif
#endif

#ifdef _PLAYTEST
		if (g_old_heuristic)
		{
			 float ground_dist =  0.7f * best_heuristic * dist;

#ifdef SUPER_DEBUG_HEURISTIC
			g_theWorld->SetColor(pos, sint32(ground_dist));
#endif
			return ground_dist;
		} else
#endif
			return best_heuristic * dist;
	}
}

void Astar::DecayOrtho(AstarPoint *parent, AstarPoint *point, float &new_entry_cost)
{
	BOOL is_ortho = FALSE;

	if (parent->m_pos.x == point->m_pos.x)
	{
		is_ortho = TRUE;
	}
	else if (parent->m_pos.x < point->m_pos.x)
	{
		is_ortho = ((parent->m_pos.y-1) == point->m_pos.y);
	}
	else
	{
		is_ortho = ((parent->m_pos.y+1) == point->m_pos.y);
	}

	if (is_ortho)
	{
		new_entry_cost = point->m_entry_cost * 0.95f;
	}
}

bool Astar::InitPoint(AstarPoint *parent, AstarPoint *point,
    const MapPoint &pos, const float pc, const MapPoint &dest)
{
#if defined(DEBUG_ASTAR_ENDLESS_LOOP)
	AstarPoint *ancestor = parent;
	while(ancestor != NULL)
	{
		Assert(ancestor != point);
		ancestor = ancestor->m_parent;
	}
#endif

	AstarPoint *d = point;
	ASTAR_ENTRY_TYPE entry=ASTAR_CAN_ENTER;
	bool is_zoc = false;

	d->m_flags = 0;
	d->SetEntry(ASTAR_CAN_ENTER);
	d->SetZoc(false);
	d->SetExpanded(false);
	d->m_pos = pos;
	d->m_parent = parent;
	d->m_queue_idx = -1;

	d->m_past_cost = pc;
	if (parent == NULL)
	{
		d->m_entry_cost = 0.0;
		d->m_future_cost = EstimateFutureCost(d->m_pos, dest); // Still use this heuristic for the start
		d->m_total_cost = d->m_past_cost + d->m_entry_cost
		     + d->m_future_cost;

#ifdef PRINT_COSTS
		PrintCosts(pos, 200, d->m_total_cost);
#endif

		return true;
	}
	else if (EntryCost(parent->m_pos, d->m_pos, d->m_entry_cost, is_zoc, entry))
	{
		Assert(pc == parent->m_past_cost + parent->m_entry_cost);
		Assert(entry != ASTAR_RETRY_DIRECTION);

#ifdef _DEBUG
		if(entry == ASTAR_RETRY_DIRECTION)
		{
			is_zoc = false;
			entry = ASTAR_CAN_ENTER;

			d->m_flags = 0;
			d->SetEntry(ASTAR_CAN_ENTER);
			d->SetZoc(false);
			d->SetExpanded(false);
			d->m_pos = pos;
			d->m_parent = parent;
			d->m_queue_idx = -1;

			d->m_past_cost = pc;
			d->m_entry_cost = 0.0;
			d->m_future_cost = EstimateFutureCost(d->m_pos, dest);
			d->m_total_cost = d->m_past_cost + d->m_entry_cost
				+ d->m_future_cost;
			EntryCost(parent->m_pos, d->m_pos, d->m_entry_cost, is_zoc, entry);
		}
#endif

		d->SetEntry(entry);
		d->SetZoc(is_zoc);

		DecayOrtho(parent, point, d->m_entry_cost);
		d->m_future_cost = d->m_entry_cost * d->m_pos.NormalizedDistance(dest);
		d->m_total_cost = d->m_past_cost + d->m_entry_cost
		    + d->m_future_cost;

#ifdef PRINT_COSTS
		PrintCosts(pos, 100, d->m_total_cost);
#endif

		return true;
	}
	else
	{
		d->SetExpanded(true);
		d->SetEntry(entry);
		d->SetZoc(is_zoc);

		if (entry == ASTAR_RETRY_DIRECTION)
		{
			d->SetEntry(ASTAR_RETRY_DIRECTION);
		}
		else
		{
			d->SetEntry(ASTAR_BLOCKED);
		}

		d->m_future_cost = k_ASTAR_BIG;
		d->m_total_cost = d->m_past_cost + d->m_entry_cost
		    + d->m_future_cost;

#ifdef PRINT_COSTS
		PrintCosts(pos, 0, d->m_total_cost);
#endif
		return false;
	}
}

#ifdef PRINT_COSTS
void Astar::PrintCosts(MapPoint pos, uint8 magnitude, float costs)
{
	if(g_graphicsOptions != NULL)
	{
		char buf[16];
		sprintf(buf, "%5.1f", costs);
		g_graphicsOptions->AddTextToCell(pos, buf, magnitude, -1);
	}
}

void Astar::ResetPrintedCosts()
{
	if(g_graphicsOptions != NULL)
	{
		g_graphicsOptions->ResetAllCellTexts();
	}
}
#endif

void Astar::RecalcEntryCost(AstarPoint *parent, AstarPoint *node, float &new_entry_cost,
    bool &new_is_zoc, ASTAR_ENTRY_TYPE &new_entry)
{

	Assert(parent);
	Assert(node);

	EntryCost(parent->m_pos, node->m_pos,
	    new_entry_cost, new_is_zoc, new_entry);

	Assert(new_entry_cost < 10000.0);

	DecayOrtho(parent, node, new_entry_cost);
}

#ifdef _DEBUG
sint32 finite_stack;
#endif _DEUBG

#if 0
sint32 g_propagate_depth;
sint32 g_propagate_been_seen;

void Astar::PropagatePathCost(AstarPoint *node, AstarPoint *parent,
   const MapPoint &start, const MapPoint &dest, AstarPoint **cost_tree)

{
#ifdef _DEBUG
	if ((100 < g_propagate_depth) && (!g_propagate_been_seen))
	{
		g_propagate_been_seen = 1;
		sint32 DONT_hit_I___call_KARL_at_x4646=0;
		Assert(DONT_hit_I___call_KARL_at_x4646);
	}
#endif

	AstarPoint *dead = NULL;
	float new_past_cost, new_entry_cost, new_total_cost;
	bool reset = false;
	sint32 d = 0;

	if (node->m_parent == NULL)
	{
		return;
	}

	if (node->GetEntry() != ASTAR_CAN_ENTER)
	{
		return;
	}

	bool new_is_zoc;
	ASTAR_ENTRY_TYPE new_entry;
	if (node->m_parent != parent)
	{
		new_past_cost = parent->m_past_cost + parent->m_entry_cost;
		new_is_zoc = false;
		new_entry = ASTAR_CAN_ENTER;

		RecalcEntryCost(parent, node, new_entry_cost, new_is_zoc, new_entry);
		new_total_cost = new_past_cost + new_entry_cost + node->m_future_cost;

		if ((new_entry == ASTAR_CAN_ENTER) && (new_total_cost < k_ASTAR_BIG) && ((new_total_cost+0.8) < node->m_total_cost) && (new_is_zoc == FALSE))
		{
			reset = true;
			node->m_parent = parent;
		}
	}

	static MapPoint next_pos;
	Cell *c;

	if (node->m_parent == parent)
	{
		if (!(node->GetExpanded()))
		{
			sint32 del_idx = node->m_queue_idx;
			AstarPoint *node_actually_removed = m_priority_queue.Remove(del_idx);
			Assert(node_actually_removed == node);
		}

		node->m_past_cost = parent->m_past_cost +
		    parent->m_entry_cost;

		if (reset)
		{
			node->m_entry_cost = new_entry_cost;
			node->SetZoc(new_is_zoc);
			node->SetEntry(new_entry);
		}

		node->m_total_cost = node->m_past_cost +
		    node->m_entry_cost + node->m_future_cost;

		if (!(node->GetExpanded()))
		{
			if (node->GetEntry() == ASTAR_CAN_ENTER)
			{
				m_priority_queue.Insert(node);
			}
		}

		if ((node->GetEntry() == ASTAR_CAN_ENTER) && (node->m_total_cost  < k_ASTAR_BIG))
		{
			for (d=0; d<GetMaxDir(node->m_pos); d++)
			{
				if (!node->m_pos.GetNeighborPosition(WORLD_DIRECTION(d), next_pos))  continue;

				c = g_theWorld->GetCell(next_pos);

				if (c->m_point != NULL)
				{
					if ((c->m_search_count == g_search_count) &&
						(!c->m_point->GetZoc()))
					{
						g_propagate_depth++;

						if (g_propagate_depth > 100)
							return;

						PropagatePathCost(c->m_point, node, start, dest, cost_tree);

						g_propagate_depth--;
					}
				}
			}
		}
	}
}
#endif

#ifdef A_STAR_TRACK_CALLSTACK
#include "debugcallstack.h"

cDebugCallStackSet g_astarCallStackSet(5);
#endif

bool Astar::FindPath
(
    MapPoint const &    start,
    MapPoint const &    dest,
    Path &              a_path,
    float &             total_cost,
    const bool          isunit,
    const sint32        cutoff,
    sint32 &            nodes_opened
)
{
#ifdef A_STAR_TRACK_CALLSTACK
	g_astarCallStackSet.Add();
#endif

#ifdef SUPER_DEBUG_HEURISTIC
	MapPoint    tmp;
	MapPoint *  size    = g_theWorld->GetSize();
	for (tmp.x = 0; tmp.x < size->x; tmp.x++)
	{
		for (tmp.y = 0; tmp.y < size->y; tmp.y++)
		{
			g_theWorld->SetColor(tmp, 0);
		}
	}
#endif

#ifdef PRINT_COSTS
	ResetPrintedCosts();
#endif

	m_priority_queue.Clear();
	g_search_count++;

	AstarPoint *    best        = NULL;
	AstarPoint *    cost_tree   = NULL;

	if (start == dest)
	{
		return Cleanup(dest, a_path, total_cost, isunit, best, cost_tree);
	}

	Cell *          c           = g_theWorld->GetCell(start);
	Assert(c);
	if (!c)
		return false;

	nodes_opened++;

#ifdef TRACK_ASTAR_NODES
	g_nodes_inserted++;
	g_nodes_opened++;
#endif

	c->m_point = g_astar_mem.GetNew();
	c->m_search_count = g_search_count;

	if (!InitPoint(NULL, c->m_point, start, 0.0, dest))
	{
		return Cleanup(dest, a_path, total_cost, isunit, best, cost_tree);
	}

	best = c->m_point;

	sint32  count       = 0;
	sint32  loop_count  = 0;
	bool    searching   = true;

	do
	{
		loop_count++;
		Assert(loop_count < 1400000); // Ultra Gigantic problem

		float past_cost = best->m_past_cost + best->m_entry_cost;
		best->SetExpanded(true);

		sint32 max_dir = GetMaxDir(best->m_pos);

		for (sint32 i = 0; i <= max_dir; ++i)
		{
			static MapPoint next_pos;
			if (!best->m_pos.GetNeighborPosition(WORLD_DIRECTION(i), next_pos)) continue;

			if(m_maxSquaredDistance > -1 && MapPoint::GetSquaredDistance(start, next_pos) > m_maxSquaredDistance) continue;

			c = g_theWorld->GetCell(next_pos);

			if (c->m_point && (c->m_search_count == g_search_count))
			{
				// When c has already been examined, we have to compute the G
				// value from the path via best, and check whether it is lower
				// than the G value of the old path.

				float				bMove;		// entry cost from best
				bool				bZoc;		// zone of control from best
				ASTAR_ENTRY_TYPE	bType;		// entry type from best
				if (EntryCost(best->m_pos, next_pos, bMove, bZoc, bType))
				{
					DecayOrtho(best, c->m_point, bMove);

					float const		oldG	=
						c->m_point->m_past_cost + c->m_point->m_entry_cost;
					float const		bestG	= past_cost + bMove;

					if (bestG < oldG && (c->m_point->m_parent == NULL || c->m_point != best->m_parent))
					{
						if (c->m_point->GetExpanded())
						{
							// The node has already been expanded: propagate the
							// change.
							// Inefficient implementation to test the function.
							// Simply reinsert it in the open node list, so its
							// parents and children will be reevaluated
							// automatically.
							// Have to think about efficiency later.

							InitPoint(best, c->m_point, next_pos, past_cost, dest);
							m_priority_queue.Insert(c->m_point);
#ifdef TRACK_ASTAR_NODES
							--g_closed_nodes;
#endif
						}
						else
						{
							sint32 const	iList	= c->m_point->GetPriorityQueueIndex();

							if (iList >= 0)
							{
								// Remove from old position in open node list.
								m_priority_queue.Remove(iList);
							}
							else
							{
								// Not in open node list? Maybe the old path
								// had a ZoC problem.
								++nodes_opened;
#ifdef TRACK_ASTAR_NODES
								++g_nodes_opened;
#endif
							}

							// Insert at the new - better - position.
							InitPoint(best, c->m_point, next_pos, past_cost, dest);
							m_priority_queue.Insert(c->m_point);
						}
					}
					else
					{
						// No action: the path via best is not better.
					}
				}
			}
			else
			{
				nodes_opened++;

#ifdef TRACK_ASTAR_NODES
				g_nodes_opened++;

#endif
				c->m_point = g_astar_mem.GetNew();
				c->m_search_count = g_search_count;

				if (InitPoint(best, c->m_point, next_pos, past_cost, dest))
				{
#ifdef TRACK_ASTAR_NODES
					g_nodes_inserted++;
#endif
					if (c->m_point->GetEntry() == ASTAR_CAN_ENTER)
					{
						m_priority_queue.Insert(c->m_point);
					}
				}
			}
		}

		count++;

		if (m_priority_queue.Len() < 1)
		{
			break;
		}
		else
		{
			if (0 < m_priority_queue.Len())
			{
				best = m_priority_queue.Remove(1);
#ifdef TRACK_ASTAR_NODES
				g_closed_nodes++;
#endif
				Assert(best->m_queue_idx < 0);
			}
			else
			{
				best = NULL;
			}
		}

		if (best)
		{
			DPRINTF(k_DBG_ASTAR,("\tCheckBest , StartPos (%d, %d), DestPos (%d, %d), BestPos (%d, %d)\n", start.x, start.y, dest.x, dest.y, best->m_pos.x, best->m_pos.y));
			if (best->m_pos == dest)
			{
				float               cost;
				ASTAR_ENTRY_TYPE    entry   = ASTAR_CAN_ENTER;
				bool                is_zoc  = false;

				if (EntryCost(best->m_parent->m_pos, best->m_pos,
				              cost, is_zoc, entry
				             )
				   )
				{
					searching = (entry != ASTAR_CAN_ENTER);
				}
			}
			// keep searching
		}
		else
		{
			searching = false;
		}

	} while (searching || (best && (k_ASTAR_BIG <= best->m_entry_cost )) && (nodes_opened < cutoff));

	Assert(nodes_opened < cutoff);

#ifdef SUPER_DEBUG_HEURISTIC
	WhackScreen();
#endif

	bool const r =  Cleanup(dest, a_path, total_cost, isunit, best, cost_tree);
	DPRINTF(k_DBG_ASTAR, ("\tFinalPathCosts: %f , StartPos (%d, %d), DestPos (%d, %d), BestPos (%d, %d)\n", total_cost, start.x, start.y, dest.x, dest.y, best->m_pos.x, best->m_pos.y));

#ifdef TRACK_ASTAR_NODES
	g_paths_found++;
	g_paths_lengths += a_path.Num();
#endif

#ifdef PRINT_COSTS
	if(g_tiledMap != NULL)
	{
		g_tiledMap->Refresh();
	}
#endif

	return r;
}

bool Astar::Cleanup(const MapPoint &dest,
                    Path &a_path,
                    float &total_cost,
                    const bool isunit,
                    AstarPoint *best,
                    AstarPoint *cost_tree)
{
	if ((best == NULL) ||
	    (best->m_pos != dest))
	{
		total_cost = 0.0;

		a_path.Clear();
		g_astar_mem.MassDelete(isunit);
		return false;
	}
	else
	{
		total_cost = best->m_past_cost + best->m_entry_cost;
		a_path.FlattenAstarList(best);
		g_astar_mem.MassDelete(isunit);

		return true;
	}
}
