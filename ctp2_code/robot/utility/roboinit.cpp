//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Robot initialization and cleanup
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
// - Cleaned up code.
//
//----------------------------------------------------------------------------

#include "ctp/c3.h"
#include "robot/utility/RoboInit.h"

#include "robot/pathing/UnitAstar.h"
#include "ai/ctpai.h"
#include "ai/diplomacy/AgreementMatrix.h"
#include "ai/mapanalysis/mapanalysis.h"

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
