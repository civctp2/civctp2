#include "c3.h"
#include "RoboInit.h"

#include "UnitAstar.h"

UnitAstar * g_theUnitAstar  = NULL; 

void roboinit_Initalize(CivArchive &a)
{ 
	Astar_Init();

    delete g_theUnitAstar;
    g_theUnitAstar = new UnitAstar; 
} 

void roboinit_Cleanup()
{ 
    delete g_theUnitAstar; 
    g_theUnitAstar = NULL;

	Astar_Cleanup();
}


