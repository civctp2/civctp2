#include "c3.h"
#include "MapPoint.h"
#include "geom2d.h"
#include "Delaunay.h"

#include "CityVertex.h"
#include "FlatPtr.h"
#include "civarchive.h"

#include "CityTree.h"
#include "CityEdge.h"
#include "CityAgent.h"

ZEROMEM(CityVertexFlat);
ZEROMEM(CityVertexPtr);
ZERODEL(CityVertexPtr);
FLATSERIALIZE(CityVertexFlat);
PTRSERIALIZE(CityVertexPtr);

CityVertex::CityVertex()
{
}

CityVertex::CityVertex(MapPointData &pos)
{
    m_edge_count=0;

    m_point = new Point2d();
    m_point->x = pos.x;
    m_point->y = pos.y;
}

CityVertex::CityVertex(CityAgent *ca)
{
    m_agent = ca;

    MapPointData pos;
    ca->GetPos(pos);
    m_edge_count=0;
    m_point = new Point2d();
    m_point->x = pos.x;
    m_point->y = pos.y;
}


CityVertex::CityVertex(const double sx, const double sy)
{
    m_edge_count=0;
    m_point = new Point2d();
    m_point->x = sx;
    m_point->y = sy;
}

CityVertex::CityVertex(CivArchive &archive)
{
    Serialize(archive);
}

CityVertex::~CityVertex()
{
    Assert(m_point);
    if (m_point) {
        delete m_point;
        m_point = NULL;
    }
}

void CityVertexPtr::Store (CivArchive &archive)
{
}

void CityVertexPtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
}

#include "Memory_Manager.h"
extern Memory_Manager *g_memory_CityVertex;
void* CityVertex::operator new(size_t byte_size)
{
    Assert(sizeof(CityVertex)==byte_size);
    return g_memory_CityVertex->Alloc();
}
void CityVertex::operator delete(void *ptr)
{
    g_memory_CityVertex->Dealloc(ptr);
}

void CityVertex::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    CityVertexFlat::Serialize(archive);
    CityVertexPtr::Serialize(archive);
}

void CityVertex::AddEdge(CityEdge *e)
{
    m_edge_count++;
    if (m_edge_head == NULL) {
        m_edge_head = e;
    } else {
        e->m_next = m_edge_head;
        m_edge_head = e;
    }
}

void CityVertex::GetPos(double &px, double &py)
{
    px = m_point->x;
    py = m_point->y;
}

CityAgent *CityVertex::GetAgent()
{
    return m_agent;
}

void CityVertex::Dump()
{

    FILE *fout;
    fout = fopen ("logs\\mst.txt", "a");
    fprintf (fout, "CityVertex { : ");
    fprintf (fout, "m_agent %x ", m_agent->m_id.GetVal());
    fprintf (fout, "ap(%d %d %d) ", m_agent->m_pos->x, m_agent->m_pos->y, m_agent->m_pos->z);
    fprintf (fout, "p2d(%2.0f %2.0f) ", m_point->x, m_point->y);
    fprintf (fout, "eh %d t %d n %d",
        (m_edge_head) ? 1 : 0,
        (m_tree) ? 1: 0,
        (m_next) ? 1 : 0);
    fprintf (fout, "\n");
    fprintf (fout, "} CityVertex\n");
    fclose (fout);
}
