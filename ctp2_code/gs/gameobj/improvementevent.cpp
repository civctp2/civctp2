//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Improvement event handler
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Prevented access to invalid memory.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "GameEventUser.h"
#include "TerrImprove.h"
#include "Events.h"

STDEHANDLER(ImprovementAddTurnEvent)
{
	TerrainImprovement imp;

	if(!args->GetImprovement(0, imp)) return GEV_HD_Continue;

	imp->AddTurn(1);
	return GEV_HD_Continue;
}

STDEHANDLER(ImprovementCompleteEvent)
{
	TerrainImprovement imp;
	if(!args->GetImprovement(0, imp)) return GEV_HD_Continue;

	MapPoint pos;
	sint32 imptype;
	sint32 owner;

	pos = imp->GetLocation();
	imptype = imp->GetType();
	owner = imp->GetOwner();

	args->Add(new GameEventArgument(GEA_MapPoint, pos));
	args->Add(new GameEventArgument(GEA_Int, imptype));
	args->Add(new GameEventArgument(GEA_Player, owner));

	TerrainImprovementData tmpData = TerrainImprovementData(ID(imp), owner, pos, imptype, 0);
	tmpData.Complete();

	return GEV_HD_Continue;
}

void improvementevent_Initialize()
{
	g_gevManager->AddCallback(GEV_ImprovementAddTurn, GEV_PRI_Primary, &s_ImprovementAddTurnEvent);
	g_gevManager->AddCallback(GEV_ImprovementComplete, GEV_PRI_Primary, &s_ImprovementCompleteEvent);
}

void improvementevent_Cleanup()
{
}
