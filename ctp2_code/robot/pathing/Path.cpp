//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : A* algorithm path object
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
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "dynarr.h"
#include "Path.h"

#include "MapPoint.h"
#include "Cell.h"
#include "World.h"

void Direction::Serialize(CivArchive &archive)

{
     if (archive.IsStoring()) { 
        archive << dir; 
     } else { 
         archive >> dir; 
     } 
}


Path::Path()
:
    m_next_dir          (0),
	m_start             (-1, -1),
    m_step              (),
    m_current           (-1, -1), 
    m_next              (0)
{ 
} 


Path::Path(const Path *copy) : m_step(copy->m_step)
{
	this->m_next = copy->m_next;
	this->m_next_dir = copy->m_next_dir;
	this->m_start = copy->m_start;
	this->m_current = copy->m_current;
}

void Path::Clear() 
{ 
    m_step.Clear(); 
    m_next = 0; 
    m_next_dir = 0; 
    m_start.Set(-1,-1); 
    m_current.Set(-1,-1); 
} 

void Path::FlattenAstarList(AstarPoint *best)
{
    Assert(best); 
    AstarPoint *    ptr = best;

    sint32 n; 
    for (n=0; ptr->m_parent; n++, ptr = ptr->m_parent);

    m_current = m_start = ptr->m_pos;
    m_next = 0;
    m_next_dir = 0; 

    m_step.Resize(n);
    m_step.m_nElements = n; 


    AstarPoint *    old = best; 
    sint32          i   = n - 1;
    for (ptr = old->m_parent; ptr; ptr = ptr->m_parent) 
    { 
        m_step[i] = ptr->m_pos.GetNeighborDirection(old->m_pos); 
        old = ptr;
        --i;
    } 
}

void Path::FlattenNormalizedPointList(const MapPoint &start, 
                                      DynamicArray<MapPoint> &pixel)
{
    
    m_start = start; 
    m_current = start; 

    sint32 n = pixel.Num()-1; 
    m_next = 0; 
    m_next_dir = 0; 
    m_step.Clear(); 
 	if(n < 1) {
		return;
	}
	m_step.Resize(n);
    
    MapPoint p; 
    sint32 i; 
    sint32 dx, dy;

    for (i=0, p = pixel[0]; i<n; i++) { 
        dx = pixel[i+1].x - pixel[i].x; 
        dy = pixel[i+1].y - pixel[i].y; 

        if (dx == 1) { 
            if (dy == -1) { 
                m_step.Insert(NORTH); 
            } else if (dy == 0) { 
                m_step.Insert(NORTHEAST); 
            } else if (dy == 1) { 
                m_step.Insert(EAST); 
            } else { 
                Assert(0); 
            }
        } else if  (dx == 0) { 
            if (dy == -1) { 
                m_step.Insert(NORTHWEST); 
            } else if (dy == 0) { 
                Assert(0); 
            } else if (dy == 1) { 
                m_step.Insert(SOUTHEAST); 
            } else { 
                Assert(0); 
            }
        } else if (dx == -1) { 
            
            if (dy == -1) { 
                m_step.Insert(WEST); 
            } else if (dy == 0) { 
                m_step.Insert(SOUTHWEST); 
            } else if (dy == 1) { 
                m_step.Insert(SOUTH); 
            } else { 
                Assert(0); 
            }
        } else { 
            Assert(0);
        }
    }

}


void Path::Start(MapPoint &p)
{
    m_next = 0; 
	m_next_dir = 0;
    m_current = m_start; 
    p = m_current;
} 

void Path::Restart(MapPoint &p)
{
	m_next_dir = 0;
	Start(p);
}

void Path::JustSetStart(const MapPoint &p)
{
    m_start = p; 
} 

bool Path::IsEnd() const
{
    return (m_start.x == -1) || (m_step.Num() <= m_next); 
}


sint32 Path::Next(MapPoint &p)

{
     Assert(m_start.x != -1); 
     if (m_next < m_step.Num()) { 
         if(m_current.GetNeighborPosition(WORLD_DIRECTION(m_step[m_next].dir), p)) {
			m_current = p;         
		 } else {
			 Assert(FALSE); 
		 }
         m_next++; 
         return TRUE; 
     } else { 
         p.Set(0,0); 
         m_next++; 
         return FALSE; 
     } 

     
}

void Path::GetCurrentPoint(MapPoint &p) const

{
    Assert(m_start.x != -1); 
    Assert(!IsEnd()); 
    p = m_current;
}

void Path::ClipStartToCurrent()

{
    m_start = m_current; 
    if (m_next == m_step.Num()) { 
        m_step.Clear(); 
    } else { 
        m_step.DelUpToIndex(m_next); 
    }
    m_next = 0; 
}


 

void Path::GetStartPoint(MapPoint &pos) const
{
    pos = m_start;
}

void Path::StartDir(WORLD_DIRECTION &d)

{
   Assert(0 < m_step.Num()); 
   d = WORLD_DIRECTION(m_step[1].dir); 
   m_next_dir = 1; 
}

void Path::GetCurrentDir(WORLD_DIRECTION &d)

{
    d = WORLD_DIRECTION(m_step[m_next_dir].dir);
}

bool Path::IsEndDir()
{
    return (m_start.x == -1) || (m_step.Num() <= m_next_dir); 
}

void Path::IncDir()
{
	MapPoint p;
	if(m_current.GetNeighborPosition(WORLD_DIRECTION(m_step[m_next_dir].dir), p)) {
		m_current = p;         
	} else {
		Assert(false); 
	}
	m_next++;
	m_next_dir++; 
}

void Path::Concat(Path const & otherpath)
{
	MapPoint    start_pos;
	otherpath.GetStartPoint(start_pos);

	MapPoint    end_pos     = GetEnd();
	
	if (start_pos.x != -1 && start_pos.y != -1 && 
		end_pos.x != -1 && end_pos.y != -1)
	{
		if (start_pos != end_pos && 
			start_pos.x != -1 && start_pos.y != -1 && 
			end_pos.x != -1 && end_pos.y != -1) {
			m_step.Insert(end_pos.GetNeighborDirection(start_pos));
		}
	}

	sint32  n  = otherpath.m_step.Num();
	for (sint32 i = 0; i < n; i++) 
    {
		m_step.Insert(otherpath.m_step[i]);
	}
}

void Path::ConcatReturnPath()
{
	for (sint32 i = m_step.Num() - 1; i >= 0; i--) 
    {
		switch(WORLD_DIRECTION(m_step[i].dir)) {
			case NORTH: m_step.Insert(SOUTH); break;
			case NORTHEAST: m_step.Insert(SOUTHWEST); break;
			case EAST: m_step.Insert(WEST); break;
			case SOUTHEAST: m_step.Insert(NORTHWEST); break;
			case SOUTH: m_step.Insert(NORTH); break;
			case SOUTHWEST: m_step.Insert(NORTHEAST); break;
			case WEST: m_step.Insert(EAST); break;
			case NORTHWEST: m_step.Insert(SOUTHEAST); break;
			default:
				Assert(FALSE);
				break;
		}
	}
}

void Path::Serialize(CivArchive &archive)

{
    
	m_current.Serialize(archive);

	if (archive.IsStoring()) { 
		archive << m_next;
        archive << m_next_dir; 
     } else { 
		 archive >> m_next;
         archive >> m_next_dir; 
     } 

	m_start.Serialize(archive); 
    m_step.Serialize(archive); 
}

void Path::InsertFront(const MapPoint &pos)
{
    sint32 num_step = m_step.Num(); 

    m_step.ExtendByOne(); 
    for (sint32 idx_step =(num_step-1); 0 <= idx_step; idx_step--) { 
       m_step[idx_step+1] = m_step[idx_step]; 
    } 
    m_step[0]   = pos.GetNeighborDirection(m_start); 
    m_start     = pos;
}

void Path::PrependDir (sint32 dir)

{ 
    sint32 num_step = m_step.Num(); 

    m_step.ExtendByOne(); 
    for (sint32 idx_step=(num_step-1); 0 <= idx_step; idx_step--) { 
       m_step[idx_step+1] = m_step[idx_step]; 
    } 
    m_step[0] = (sint8)dir; 
} 

void Path::InsertEnd(const MapPoint &pos)
{
    MapPoint old, tmp; 

    Start(old); 
    Next(tmp);  
    for ( ; !IsEnd(); Next(tmp)) { 
    }      
    Direction d = tmp.GetNeighborDirection(pos); 
    m_step.Insert(d); 
}

void Path::AddDir(const WORLD_DIRECTION d)
{
    m_step.Insert(d); 
}

void Path::SpaceMove(const MapPoint &start, const MapPoint &dest)
{
    m_start = start; 
    m_step.Clear(); 
    Direction const d = Direction(start.GetNeighborDirection(dest)); 
    m_step.Insert(d); 
    m_step.Insert(d); 
}

void Path::SnipEnd()
{
    sint32 n = m_step.Num(); 

    Assert(n >= 1); 
    m_step.DelIndex(n-1); 
}

sint32 Path::GetMovesRemaining()
{
	return m_step.Num() - m_next_dir;
}







MapPoint Path::GetEnd() const
{
	MapPoint end = m_start;
	MapPoint next;
	for(sint32 i = 0; i < m_step.Num(); i++) {
		if(end.GetNeighborPosition(WORLD_DIRECTION(m_step[i].dir), next)) {
			end = next;
		} else {
			Assert(FALSE);
		}
	}
	return end;
}


sint32 Path::GetNextIndex() const
{
	return m_next;
}

void Path::RestoreIndexAndCurrentPos(const sint32 & index)
{
	m_current = m_start;
	m_next    = 0;
	while (m_next < index)
	{
		Next(m_current);
	}
}


void Path::SnipEndUntilCanEnter(const uint32 & movement_type)
{
	sint32 i;
	MapPoint end = m_start;
	MapPoint next;
	sint32 last_can_enter = -1;
	for(i = 0; i < m_step.Num(); i++) {
		if(end.GetNeighborPosition(WORLD_DIRECTION(m_step[i].dir), next)) {
			end = next;
			if (g_theWorld->GetCell(next)->CanEnter(movement_type) )
				last_can_enter = i;
		} else {
			Assert(FALSE);
		}
	}
	
	for (i = m_step.Num() - 1; i > last_can_enter; i--)
	{
		m_step.DelIndex(i);
	}
}

