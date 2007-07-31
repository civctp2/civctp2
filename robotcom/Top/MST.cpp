


#include "c3.h"
#include "globals.h"
#include "CivArchive.h"


#include "geom2d.h"
#include "delaunay.h"

#include "globals.h"
#include "IC3GameState.h"
#include "IC3World.h"
#include "IC3RobotAstar.h"
#include "IC3InstDB.h"

#include "AiMain.h"
#include "BSet.h"
#include "AiMap.h"

#include "MST.h"

#include "Cont.h"
#include "CityAgent.h"

#include "CityEdge.h"
#include "CityVertex.h"
#include "CityTree.h"
#include "CityTreeQueue.h"
#include "FlatPtr.h"
#include "ScienceAgent.h"

extern QuadEdge *g_quad_edge_head; 
extern QuadEdge *g_quad_edge_tail; 

#include "FzOut.h"
extern double fz_inst_road_coef;








void FS_MinSpanTree::FindMST(
    sint32 num_vertex, 
    CityVertex *G, 
    CityTree *&mst
    )
{
	CityTreeQueue *Q = new CityTreeQueue; 

    CityTree *t=NULL;
    CityVertex *cv=NULL; 
    CityVertex *nx=NULL; 




    
    for (cv = G; cv; cv = nx) {        
        nx = cv->m_next; 


        t = new CityTree(cv);


		Q->Enqueue(t);
    }





    Q->Sort(); 

	sint32 stage_count = 0; 

    CityEdge *e=NULL; 
    CityTree *connect_me=NULL; 
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while (1 < Q->Len()) {  
        Assert(++finite_loop < 100000); 




		if (Q->Peek()->GetStage() == stage_count) { 
			Q->CleanUp(); 
			stage_count++;



        } 

        Q->Dequeue(t);

        Assert(t); 
		t->GetMinEdge(e, connect_me);










		Q->Remove(connect_me); 
		t->Merge(e, connect_me); 




        delete connect_me; 
        connect_me = NULL; 
		Q->Enqueue(t);

	}

    
    Q->Dequeue(mst); 



    delete Q; 
}








void FS_MinSpanTree::FindCitiesOnCont(
    AiMain *ai, 
    const sint32 idx_cont, 
    sint32 &num_cities, 
    CityVertex *&city_list)
{
    CityAgent *ca=NULL; 
    BOOL r, is_land; 
    city_list = NULL;
    BSetID id; 
    CityVertex *cv = NULL, *tail = NULL; 
    MapPointData pos; 
    sint32 city_cont;

    city_list = NULL; 
    num_cities = 0; 
    
    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) { 
        ca->GetPos(pos); 
        r = ai->m_continents->GetContinent(ai, pos, city_cont, is_land);
        if (r && is_land && (idx_cont == city_cont)) { 
            cv = new CityVertex(ca); 
            num_cities++; 
            if (city_list == NULL) { 
                city_list = cv; 
                tail = cv; 
            } else { 
                tail->m_next = cv; 
                tail = cv; 
            }
        } 
    }

}

void FS_MinSpanTree::FindAllCities(
    AiMain *ai, 
    sint32 &num_cities, 
    CityVertex *&city_list)
{
    CityAgent *ca=NULL; 
    city_list = NULL;
    BSetID id; 
    CityVertex *cv = NULL, *tail = NULL; 
    MapPointData pos; 

    city_list = NULL; 
    num_cities = 0; 
    
    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) { 
        ca->GetPos(pos); 

        if (pos.z == SPACE_Z) continue; 

        cv = new CityVertex(ca); 
        num_cities++; 
        if (city_list == NULL) { 
            city_list = cv; 
            tail = cv; 
        } else { 
            tail->m_next = cv; 
            tail = cv; 
        }
    }
}

































void FS_MinSpanTree::RemoveDups(CityTree *mst)
{
    sint32 f, s;
    CityEdge *first=NULL, *sec=NULL, *old=NULL, *del_me;
    for (f = 0, first = mst->m_edge_internal; first && first->m_next; first = first->m_next, f++) { 
        old = first;
        s=f+1;
        for(sec = first->m_next; sec; s++){ 
            if (
                ((first->m_start == sec->m_start) &&
                 (first->m_dest == sec->m_dest)) || 
                ((first->m_start == sec->m_dest) &&
                (first->m_dest == sec->m_start)) ) {

                
                old->m_next = sec->m_next; 
                del_me = sec; 
                sec = sec->m_next;
                delete del_me;
                del_me = NULL; 
            } else { 
                old = sec;
                sec = sec->m_next;
            }

        } 
    } 
}








void FS_MinSpanTree::AddEdge(CityVertex *&city_vertex, 
    const double weight, const double sx, const double sy, 
    const double dx, const double dy) 
{
    double px, py; 
    
    CityVertex *v=NULL, *start_v=NULL, *dest_v=NULL; 

    for (v = city_vertex; v; v = v->m_next) { 
        v->GetPos(px, py); 
        if ((px == sx) && (py == sy)) { 
            start_v = v;
            break; 
        }
    }

    for (v = city_vertex; v; v = v->m_next) { 
        v->GetPos(px, py); 
        if ((px == dx) && (py == dy)) { 
            dest_v = v;
            break; 
        }
    }

    Assert(start_v); 
    Assert(dest_v); 
    Assert(start_v != dest_v); 
    start_v->AddEdge(new CityEdge(weight, start_v, dest_v)); 
    dest_v->AddEdge(new CityEdge(weight, dest_v, start_v)); 
}








void FS_MinSpanTree::UniqueVertex (sint32 &num_cv, CityVertex *&city_vertex, 
    const double sx, const double sy) 
{
    CityVertex *v=NULL; 
    
    double px, py; 

    if (city_vertex == NULL) { 
       city_vertex = new CityVertex(sx, sy); 
       num_cv++; 
    } else { 
        for (v = city_vertex; v; v = v->m_next) { 
            v->GetPos(px, py); 
            if ((px == sx) && (py == sy)) { 
                return; 
            } 
            if (v->m_next == NULL) {                 
               v->m_next = new CityVertex(sx, sy); 
               num_cv++; 
               return; 
            }
        }
    }
}








void FS_MinSpanTree::Quad2Vertex(AiMain *ai, sint32 &num_cv, CityVertex *&city_vertex)
{
    QuadEdge *q; 

    MapPointData start, dest; 
    start.z = GROUND_Z; 
    dest.z = GROUND_Z; 
    double d; 
    sint32 xmax = ai->m_world->GetXWidth(); 
    sint32 ymax = ai->m_world->GetYHeight();

    sint32 count=0; 
    for (q = g_quad_edge_head; q; q = q->m_qnext) {
        count++; 
       if ((q->e[0].data->x < 0.0) || (xmax <= q->e[0].data->x)) continue; 
       if ((q->e[0].data->y < 0.0) || (ymax <= q->e[0].data->x)) continue; 

       if ((q->e[2].data->x < 0.0) || (xmax <= q->e[2].data->x)) continue; 
       if ((q->e[2].data->y < 0.0) || (ymax <= q->e[2].data->x)) continue; 

       start.x = sint16(q->e[0].data->x);
       start.y = sint16(q->e[0].data->y);

       dest.x = sint16(q->e[2].data->x);
       dest.y = sint16(q->e[2].data->y);

       d = ai->m_map->WrappedDistance(start, dest); 

       _ASSERTE(1.0 <= d); 
       AddEdge(city_vertex, d, q->e[0].data->x, q->e[0].data->y, 
          q->e[2].data->x, q->e[2].data->y);
    }
} 









void FS_MinSpanTree::FindDelaunay(AiMain *ai, sint32 num_cities, 
     CityVertex *city_list)
{
    CityVertex  *d=NULL; 
    Subdivision *tri=NULL; 

    Point2d a, b, c;
    MapPointData start;
    MapPointData dest;

    start.z = GROUND_Z; 
    dest.z = GROUND_Z; 
    
    sint32 xmax = ai->m_world->GetXWidth(); 
    sint32 ymax = ai->m_world->GetYHeight();
    if (3 <= num_cities) { 
        g_quad_edge_head = NULL; 
        g_quad_edge_tail = NULL; 
      
         
        a.x = -1; 
        a.y = -1;

        b.x = 2 * xmax + 10; 
        b.y = -1; 

        c.x = -1; 
        c.y = 2 * ymax + 10;
       
        
        tri = new Subdivision(a, b, c); 
        
        sint32 count = 0; 
        for (d = city_list, count=0; d; d = d->m_next, count++) { 
            tri->InsertSite(*(d->m_point));
        }

        Quad2Vertex(ai, num_cities, city_list);

        CleanUpQuadEdges();

    }
}

void FS_MinSpanTree::CleanUpQuadEdges()
{ 

    
    
    
    QuadEdge *current_edge=NULL;
    QuadEdge *del_edge = NULL; 
    QuadEdge *search_edge = NULL; 
    Point2d *del_point=NULL; 
    sint32 i, j; 

    current_edge = g_quad_edge_head; 
#ifdef _DEBUG
    sint32 quad_edge_finite_loop=0;
#endif
    while (current_edge) { 
        del_edge = current_edge;
        current_edge = current_edge->m_qnext; 

        for (i=0; i<4; i++) { 
             del_point = del_edge->e[i].data; 

             if (NULL == del_point) continue; 

             search_edge = del_edge; 
             while (search_edge) { 
                 for (j=0; j<4; j++) { 
                     Assert(quad_edge_finite_loop++<1000000); 
                     if (search_edge->e[j].data == del_point) { 
                         search_edge->e[j].data = NULL; 
                     }
                 }
                 search_edge = search_edge->m_qnext; 
             }
             delete del_point; 
         } 

         delete del_edge; 
     }                    

    g_quad_edge_head = NULL; 
    g_quad_edge_tail = NULL; 
} 









void FS_MinSpanTree::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    return;
}

MapPointData * g_buffer;
sint32 bufSize;








BOOL RoadPathCallback (BOOL can_enter,  MapPointData *prev,  
   MapPointData *pos, double *cost, BOOL is_zoc)
{ 
    if (can_enter) { 
        *cost = 100.0; 
        return TRUE; 
    } else { 
        *cost = 1000000.0; 
        return FALSE;
    }
}






























































BOOL FS_MinSpanTree::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{ 
    sint32 num_continents = ai->m_continents->GetNumLandCont(); 
    CityVertex *city_list=NULL; 
    sint32 idx_cont; 
    sint32 num_cities;
        
    CityTree *mst; 

    
    if (!ai->m_science_agent->HasUnderseaTunnel()) { 
        for (idx_cont=0; idx_cont<num_continents; idx_cont++) { 
    
            mst = ai->m_continents->GetMst(idx_cont); 
            if (mst) { 
                mst->Del(); 
                delete mst;
                mst = NULL; 
                ai->m_continents->SetMst(idx_cont, NULL); 
            }

            FindCitiesOnCont(ai, idx_cont, num_cities, city_list);
            if ((3 <= num_cities) && (0.0 < fz_inst_road_coef)) { 

                
                FindDelaunay(ai, num_cities, city_list); 
                
                FindMST(num_cities, city_list, mst); 
                
                ai->m_continents->SetMst(idx_cont, mst); 
            }  else { 
                
                ai->m_continents->SetMst(idx_cont, NULL); 
            }
        }
    } else { 

        

        for (idx_cont=0; idx_cont<num_continents; idx_cont++) { 
    
            mst = ai->m_continents->GetMst(idx_cont); 
            if (mst) { 
                mst->Del(); 
                delete mst;
                mst = NULL; 
                ai->m_continents->SetMst(idx_cont, NULL); 
            }
        }

        FindAllCities(ai, num_cities, city_list);
        if ((3 <= num_cities) && (0.0 < fz_inst_road_coef)) { 

                
                FindDelaunay(ai, num_cities, city_list); 
                
                FindMST(num_cities, city_list, mst); 
                
                ai->m_continents->SetMst(0, mst); 
        } 
    }

    return TRUE; 
} 

 
