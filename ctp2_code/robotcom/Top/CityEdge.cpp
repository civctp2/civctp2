



#include "c3.h"
#include "globals.h"
#include "c3types.h"
#include "c3errors.h"

#include "IMapPointData.h"

#include "CityEdge.h"
#include "FlatPtr.h"
#include "CivArchive.h"

#include "CityVertex.h"
#include "CityAgent.h"

ZEROMEM(CityEdgeFlat); 
ZEROMEM(CityEdgePtr); 
ZERODEL(CityEdgePtr); 
FLATSERIALIZE(CityEdgeFlat); 
PTRSERIALIZE(CityEdgePtr); 








CityEdge::CityEdge()
{ 
}








CityEdge::CityEdge(CivArchive &archive)
{ 
    Serialize(archive); 
} 








CityEdge::~CityEdge()
{ 
} 

void CityEdgePtr::Store (CivArchive &archive)
{
}


void CityEdgePtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
}








void CityEdge::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE 

    CityEdgeFlat::Serialize(archive); 
    CityEdgePtr::Serialize(archive); 
}



CityEdge::CityEdge(const double w, CityVertex *start_v, CityVertex *dest_v)
{
    m_weight = w;
    m_start = start_v; 
    m_dest = dest_v; 
    m_next = NULL; 
}



#include "Memory_Manager.h"
extern Memory_Manager *g_memory_CityEdge; 
void* CityEdge::operator new(size_t byte_size)
{
    Assert(sizeof(CityEdge)==byte_size);
    return g_memory_CityEdge->Alloc(); 
}
void CityEdge::operator delete(void *ptr)
{
    g_memory_CityEdge->Dealloc(ptr); 
}

CityAgent *CityEdge::GetStartCityAgent()
{
    return m_start->GetAgent();  
}

CityAgent *CityEdge::GetDestCityAgent()
{
    return m_dest->GetAgent(); 
}


void CityEdge::Dump()
{
    FILE *fout;
    fout = fopen ("logs\\mst.txt", "a"); 
    fprintf (fout, "Edge: w %3.1f ", m_weight); 
    fprintf (fout, "start: %x dest: %x\n", m_start->m_agent->m_id.GetVal(), 
        m_dest->m_agent->m_id.GetVal()); 
    fclose(fout); 
}
