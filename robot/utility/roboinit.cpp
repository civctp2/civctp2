#include "c3.h"

#include "UnitAstar.h"

#include "DataCheck.h"
#include "RoboInit.h"

#include "CtpAi.h"
#include "Diplomat.h"
#include "MapAnalysis.h"
#include "AgreementMatrix.h"

UnitAstar *g_theUnitAstar; 

BOOL roboinit_Initalize(CivArchive &a)

{ 
	Astar_Init();
    g_theUnitAstar = new UnitAstar; 
    return TRUE;
} 

void roboinit_Cleanup()

{ 
	
	CtpAi::Cleanup();

	
	
	

	
	MapAnalysis::GetMapAnalysis().Resize(0, 0, 0, 1);

	
	AgreementMatrix::s_agreements.~AgreementMatrix();

	Astar_Cleanup();

    if (g_theUnitAstar) { 
        delete g_theUnitAstar; 
        g_theUnitAstar = NULL; 
    }

}


