#ifndef __FS_MST_H__
#define __FS_MST_H__ 1

#include "FrameState.h"

class CityVertex;
class CityTree;
class FS_MinSpanTree : public FrameState
{

public:

    void FindMST(sint32 num_vertex, CityVertex *G,  CityTree *&mst);
    void FindCitiesOnCont(AiMain *ai, const sint32 idx_cont, sint32 &num_cities, CityVertex *&city_list);
    void FindAllCities(AiMain *ai, sint32 &num_cities, CityVertex *&city_list);

    void FindDelaunay(AiMain *ai, sint32 num_cities, CityVertex *city_list);

    void RemoveDups(CityTree *mst);


    void AddEdge(CityVertex *&city_vertex,
        const double weight, const double sx, const double sy,
        const double dx, const double dy);

   void UniqueVertex (sint32 &num_cv, CityVertex *&city_vertex,
        const double sx, const double sy);

   void Quad2Vertex(AiMain *ai, sint32 &num_cv, CityVertex *&city_vertex);
   void CleanUpQuadEdges();

   BOOL Execute(AiMain *ai, sint32 &branch, FILE *fout);

   void Serialize(CivArchive &archive);

};

#endif __FS_MST_H__
