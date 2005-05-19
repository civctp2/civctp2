



#include "c3.h"
#include "CityTree.h"
#include "FlatPtr.h"
#include "civarchive.h"
#include "CityEdge.h"
#include "CityVertex.h"
#include "CityAgent.h"

ZEROMEM(CityTreeFlat); 
ZEROMEM(CityTreePtr); 
ZERODEL(CityTreePtr); 
FLATSERIALIZE(CityTreeFlat); 
PTRSERIALIZE(CityTreePtr); 


CityTree::CityTree(CivArchive &archive)
{ 
    Serialize(archive); 
} 

CityTree::~CityTree()
{ 
} 

void CityTree::Del()
{
    CityEdge *e=NULL, *del_me_e=NULL; 

    for (e = m_edge_external; e; ) { 
        del_me_e = e; 
        e = e->m_next; 
        delete del_me_e; 
    }
	m_edge_external = NULL; 

    for (e = m_edge_internal; e; ) { 
        del_me_e = e; 
        e = e->m_next; 
        delete del_me_e; 
    }
	m_edge_internal = NULL;

    CityVertex *del_me_v=NULL, *v=NULL; 
    for (v = m_city_head; v; ) { 
        del_me_v = v; 
        v = v->m_next; 
        delete del_me_v; 
    }
	m_city_head = NULL; 


}


void CityTreePtr::Store (CivArchive &archive)
{
}


void CityTreePtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
}

void CityTree::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE 

    CityTreeFlat::Serialize(archive); 
    CityTreePtr::Serialize(archive); 
}

CityTree::CityTree(CityVertex *v)
{
    m_next = NULL; 
    m_city_head = v; 
    m_edge_internal = NULL; 

    Assert(0 < v->m_edge_count); 
    m_external_edge_count = v->m_edge_count; 
    Assert(v->m_edge_head); 
    m_edge_external = v->m_edge_head; 
    v->m_edge_head = NULL; 
    v->SetTree(this); 
    v->m_next = NULL; 
}


#include "Memory_Manager.h"
extern Memory_Manager *g_memory_CityTree; 
void* CityTree::operator new(size_t byte_size)
{
    Assert(sizeof(CityTree)==byte_size);
    return g_memory_CityTree->Alloc(); 
}
void CityTree::operator delete(void *ptr)
{
    g_memory_CityTree->Dealloc(ptr); 
}



BOOL CityTree::IsLarger(CityTree *b)
{
    if (m_external_edge_count < b->m_external_edge_count) { 
        return TRUE; 
    } else if (m_external_edge_count == b->m_external_edge_count) { 
        if (m_city_head->m_agent->m_id.GetVal() > b->m_city_head->m_agent->m_id.GetVal()) { 
           return TRUE; 
        } 
    }

    return FALSE; 
}




void CityTree::CleanUp()
{ 

    CityEdge *e=NULL, *old=NULL, *del_me; 
    CityTree *t; 

    e = m_edge_external; 
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while(e) { 
Assert(++finite_loop < 100000); 
        t = e->GetDest()->GetTree(); 
        if ((t != this) && (e->GetWeight() < t->GetMinEdgeWeight())) { 
            t->SetMinEdgeWeight(e);
            old = e; 
            e = e->m_next;
        } else { 
            del_me = e; 
            if (old == NULL) { 
                m_edge_external = e->m_next; 
            } else { 
                old->m_next = e->m_next; 
            }
            e = e->m_next;
            delete del_me; 
        }  
    }
} 

double CityTree::GetMinEdgeWeight()
{
    return m_min_edge_weight; 
}

void CityTree::SetMinEdgeWeight(CityEdge *e)
{
    m_min_edge_weight = e->GetWeight(); 
}

void CityTree::ResetMinEdgeWeight()
{
    m_min_edge_weight = k_MAX_EDGE;
} 


void CityTree::GetMinEdge(CityEdge *&to_dest, CityTree *&connect_me)
{
    CityEdge  *edge_min=NULL; 
    double w, weight_min = k_MAX_EDGE; 
    CityEdge *prev=NULL;
    CityEdge *prev_min=NULL; 
    CityEdge *e=NULL; 

    for (e = m_edge_external; e; e = e->m_next) { 
        w = e->GetWeight(); 
        _ASSERTE(1.0 <= w);
        Assert(w < k_MAX_EDGE); 

        if (w < weight_min) {
            prev_min = prev; 
            weight_min = w; 
            edge_min = e; 
        } else if (w == weight_min) { 
            if (edge_min->m_dest->m_tree->IsLarger(e->m_dest->m_tree)) { 
                prev_min = prev; 
                weight_min = w; 
                edge_min = e; 
            } 
        } 
        prev = e; 
    }     

    _ASSERTE(edge_min); 
    if (prev_min == NULL) { 
        m_edge_external = edge_min->m_next; 
    } else { 
        prev_min->m_next = edge_min->m_next; 
    } 
    to_dest = edge_min; 
    to_dest->m_next=NULL; 

    connect_me = to_dest->GetDest()->GetTree(); 
}

void CityTree::Merge(CityEdge *to_dest, CityTree *connect_me)
{
    m_stage = min(m_stage, connect_me->GetStage())+1;

    to_dest->m_next =  m_edge_internal;
    m_edge_internal = to_dest; 

    CityVertex *c; 
    for (c = connect_me->m_city_head; c; c = c->m_next) { 
        c->SetTree(this); 
    }

    if (m_city_head) { 
        for (c = m_city_head; c->m_next; c = c->m_next); 
        c->m_next = connect_me->m_city_head; 
    } else { 
        m_city_head = connect_me->m_city_head; 
    } 
    connect_me->m_city_head = NULL; 

    CityEdge *e=NULL; 
    if (m_edge_external) { 
        for (e = m_edge_external; e->m_next; e = e->m_next);
        e->m_next = connect_me->m_edge_external; 
    } else { 
        m_edge_external = connect_me->m_edge_external; 
    } 
    connect_me->m_edge_external = NULL; 

    if (m_edge_internal) { 
        for (e = m_edge_internal; e->m_next; e = e->m_next);
        e->m_next = connect_me->m_edge_internal; 
    } else { 
        m_edge_internal = connect_me->m_edge_internal; 
    } 
    connect_me->m_edge_internal=NULL; 

}


CityEdge* CityTree::TakeCheapestEdge()
{
   if (!m_edge_internal)
        return NULL; 

    CityEdge *e=NULL, *e_min=NULL; 
    CityEdge *prev = NULL, *prev_min=NULL;
    double w, w_min = 10000000.0; 


    
    for (e=m_edge_internal; e; e = e->m_next) { 
        w = e->GetWeight();
        if (w < w_min) { 
            e_min = e; 
            prev_min = prev;
            w_min = w; 
        } 
        prev = e; 
    } 

    
    if (prev_min == NULL) { 
        m_edge_internal = m_edge_internal->m_next; 
    } else { 
        prev_min->m_next = e_min->m_next; 
    } 
    e_min->m_next = NULL; 

    return e_min; 
}


void CityTree::RemoveCityEdge(CityVertex *remove_me)
{ 
    CityEdge *old=NULL, *del_me=NULL, **cur=NULL; 

    cur = &m_edge_internal; 
    old = NULL; 
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while (*cur) { 
Assert(++finite_loop < 100000); 
        if ( ((*cur)->m_start == remove_me) || ((*cur)->m_dest == remove_me)) { 
            del_me = *cur; 
            if (old) { 
                old->m_next = *cur; 
            } 
            *cur = (*cur)->m_next; 
            delete del_me; 
        } else { 
            old = *cur; 
            cur = &((*cur)->m_next); 
        } 
    } 

    cur = &m_edge_external; 
    old = NULL; 

    while (*cur) { 
Assert(++finite_loop < 100000); 
        if ( ((*cur)->m_start == remove_me) || ((*cur)->m_dest == remove_me)) { 
            del_me = *cur; 
            if (old) { 
                old->m_next = *cur; 
            } 
            *cur = (*cur)->m_next; 
            delete del_me; 
        } else { 
            old = *cur; 
            cur = &((*cur)->m_next); 
        } 
    } 
    
}

void CityTree::RemoveCityReferance(CityAgent *remove_me)
{
    
    CityVertex *old_vertex=NULL, *del_vertex=NULL, *cur_vertex=NULL;     

    if (NULL == m_city_head) return;

    BOOL searching; 
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    do {         
        searching = FALSE; 
        if (m_city_head == NULL) { 
            break; 
        } 

        if (m_city_head->m_agent == remove_me) { 
            del_vertex = m_city_head; 
            RemoveCityEdge(m_city_head);
            m_city_head = m_city_head->m_next; 
            searching = TRUE; 
            delete del_vertex; 
        }
Assert(++finite_loop < 100000); 
    } while (searching); 

	if (NULL == m_city_head) return;

    old_vertex = m_city_head; 
    cur_vertex = m_city_head->m_next; 
    while (cur_vertex) { 
Assert(++finite_loop < 100000); 

        if (cur_vertex->m_agent == remove_me) {
            del_vertex = cur_vertex; 
            RemoveCityEdge(del_vertex);

            old_vertex->m_next = cur_vertex->m_next; 
            cur_vertex = cur_vertex->m_next; 
            delete del_vertex; 
        } else { 
            old_vertex = cur_vertex; 
            cur_vertex = cur_vertex->m_next; 
        } 
    } 
}

void CityTree::Dump()
{
    FILE *fout;

    fout = fopen("logs\\mst.txt", "a"); 
    fprintf (fout, "CityTree: {  m_stage %d m_min_edge_weight %f\n", 
        m_stage, m_min_edge_weight); 

    fprintf (fout, "m_city_head %x\n", m_city_head->m_agent->m_id.GetVal()); 
    fprintf (fout, "Exeternal edges\n", fout); 
    fclose(fout); 
    CityEdge *e=NULL; 
    if (m_edge_external) { 
        for (e = m_edge_external; e; e = e->m_next) { 
            e->Dump(); 
        } 
    }

    fout = fopen("logs\\mst.txt", "a"); 
    fprintf (fout, "Internal edges"); 
    fclose(fout); 
    if (m_edge_internal) { 
        for (e=m_edge_internal; e; e = e->m_next) { 
            e->Dump(); 
        } 
    }

    fout = fopen("logs\\mst.txt", "a"); 
    fprintf (fout, "next %d\n", (m_next)? 1 : 0); 
    fprintf (fout, "} CityTree: DONE\n"); 
    fclose (fout); 
}





