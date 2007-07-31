
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

	imp->Complete();
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

