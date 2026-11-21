//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Installation data
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
// - Made government modified for units work here. (July 29th 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "installation.h"
#include "ID.h"
#include "civarchive.h"
#include "network.h"

#include "UnitData.h"
#include "installationtree.h"
#include "QuadTree.h"
#include "Unit.h"
#include "World.h"
#include "player.h"
#include "gaiacontroller.h"
#include "Cell.h"
#include "TurnCnt.h"
#include "SelItem.h"
#include "tiledmap.h"
#include "network.h"
#include "net_info.h"
#include "UnitRecord.h"
#include "unitutil.h"
#include "terrainutil.h"
#include "CityInfluenceIterator.h"      // GenerateBorders

extern QuadTree<Unit> *g_theUnitTree;
extern World *g_theWorld;
extern Player **g_player;
extern TurnCount *g_turn;
extern SelectedItem *g_selected_item;
extern TiledMap *g_tiledMap;

InstallationData::InstallationData(ID id,
								   sint32 owner,
								   MapPoint &pnt,
								   sint32 type)
	: GameObj(id.m_id)
{
	m_owner = owner;
	m_point = pnt;
	m_type = type;
	m_visibility = 1 << owner;
	m_airfieldLastUsed = -1;












	ENQUEUE();
}

InstallationData::InstallationData(CivArchive &archive) : GameObj(0)
{
	Serialize(archive);
}

void InstallationData::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		GameObj::Serialize(archive);
		archive.StoreChunk((uint8 *)&m_owner, ((uint8 *)&m_visibility)+sizeof(m_visibility));

		archive<<(uint32)(m_lesser != NULL);
		if (m_lesser)
			((InstallationData *)(m_lesser))->Serialize(archive) ;

		archive<<(uint32)(m_greater != NULL);
		if (m_greater)
			((InstallationData *)(m_greater))->Serialize(archive) ;

	} else {
		GameObj::Serialize(archive);
		archive.LoadChunk((uint8 *)&m_owner, ((uint8 *)&m_visibility)+sizeof(m_visibility));

		uint32	hasOld;

		archive>>hasOld;

		if (hasOld)
			m_lesser = new InstallationData(archive);

		archive>>hasOld;
		if (hasOld)
			m_greater = new InstallationData(archive);
	}
}


void InstallationData::DoVision()
{
	MapPoint topleft = m_point;

	double maxVisionRange = terrainutil_GetMaxVisionRange();
	double myVisionRange = terrainutil_GetVisionRange(m_type, m_point);
	sint32 maxrsq = sint32((maxVisionRange+0.5) * (maxVisionRange+0.5));
	sint32 myrsq = sint32((myVisionRange+0.5)*(myVisionRange+0.5));

	if(m_owner >= 0) {
		m_visibility = 1 << m_owner;
	} else {
		m_visibility = 0;
	}

	topleft.x -= static_cast<sint16>(maxVisionRange);
	DynamicArray<Installation> instArray;

	g_theInstallationTree->SearchRect(instArray, topleft,
									  static_cast<sint16>(maxVisionRange) * 2 + 1,
									  static_cast<sint16>(maxVisionRange) * 2 + 1,
									  m_owner >= 0 ? ~(1 << m_owner) : 0xffffffff);
	sint32 in = instArray.Num();
	sint32 i;
	for(i = 0; i < in; i++) {
		InstallationData *oinst = instArray[i].AccessData();
		double hisr = terrainutil_GetVisionRange(oinst->m_type, oinst->GetLocation()) + 0.5;
		sint32 hisrsq = sint32(hisr * hisr);
		sint32 ls = GetDistance(this, oinst, sint32(maxVisionRange));
		if(ls > maxrsq)
			continue;

		if(ls <= hisrsq && !(m_visibility & (1 << oinst->m_owner))) {
			if(g_theTerrainImprovementDB->Get(oinst->m_type)->GetCanSee() &
			   g_theTerrainImprovementDB->Get(m_type)->GetCanSee()) {
				m_visibility |= (1 << oinst->m_owner);
			}
		}

		if(ls <= myrsq && !(oinst->m_visibility & (1 << m_owner))) {

			if (g_theTerrainImprovementDB->Get(oinst->m_type)->GetCanSee() &
				g_theTerrainImprovementDB->Get(m_type)->GetCanSee()) {
				oinst->m_visibility |= 1 << m_owner;
			}
		}
	}

	if(unitutil_GetMaxVisionRange() > maxVisionRange) {
		maxVisionRange = unitutil_GetMaxVisionRange();
	}
	maxrsq = sint32((maxVisionRange+0.5) * (maxVisionRange+0.5));
	topleft = m_point;
	topleft.x -= static_cast<sint16>(maxVisionRange);
	DynamicArray<Unit> unitArray;

	g_theUnitTree->SearchRect(unitArray, topleft,
							  static_cast<sint16>(maxVisionRange) * 2 + 1,
							  static_cast<sint16>(maxVisionRange) * 2 + 1,
							  ~(1 << m_owner));
	sint32 un = unitArray.Num();
	for(i = 0; i < un; i++) {
		UnitData *ud = unitArray[i].AccessData();
		double hisr = ud->GetDBRec()->GetVisionRange() + 0.5;
		sint32 hisrsq = sint32(hisr*hisr);
		sint32 ls = UnitData::GetDistance(Installation(m_id),
										  ud, sint32(maxVisionRange));
		if(ls >= maxrsq)
			continue;

		if(ls <= hisrsq && !(m_visibility & (1 << ud->GetOwner()))) {
			m_visibility |= (1 << ud->GetOwner());
		}

		if(ls <= myrsq && !(ud->GetRealVisibility() & (1 << m_owner))) {
			if(ud->GetDBRec()->GetVisionClass() &
			   g_theTerrainImprovementDB->Get(m_type)->GetCanSee()) {
				if(m_owner >= 0) {
					ud->SetVisible((PLAYER_INDEX)m_owner);
				}
			}
		}
	}
}

void InstallationData::CheckVision(sint32 owner)
{
	MapPoint topleft = m_point;

	double maxVisionRange = terrainutil_GetMaxVisionRange();
	sint32 maxrsq = sint32((maxVisionRange+0.5) * (maxVisionRange+0.5));

	topleft.x -= static_cast<sint16>(maxVisionRange);
	DynamicArray<Installation> instArray;

	g_theInstallationTree->SearchRect(instArray, topleft,
									  static_cast<sint16>(maxVisionRange) * 2 + 1,
									  static_cast<sint16>(maxVisionRange) * 2 + 1,
									  1 << owner);
	sint32 in = instArray.Num();
	BOOL canBeSeen = FALSE;
	sint32 i;
	for(i = 0; i < in; i++) {
		InstallationData *oinst = instArray[i].AccessData();
		sint32 ls = GetDistance(this, oinst, sint32(maxVisionRange));
		if(ls > maxrsq)
			continue;

		double hisr = terrainutil_GetVisionRange(oinst->m_type, oinst->GetLocation()) + 0.5;
		sint32 hisrsq = sint32(hisr * hisr);
		if(ls <= hisrsq) {
			if(g_theTerrainImprovementDB->Get(m_type)->GetCanSee() &
			   g_theTerrainImprovementDB->Get(oinst->m_type)->GetCanSee()) {
				canBeSeen = TRUE;
				break;
			}
		}
	}

	if(!canBeSeen) {

		maxVisionRange = unitutil_GetMaxVisionRange();
		maxrsq = sint32((maxVisionRange+0.5) * (maxVisionRange+0.5));
		topleft = m_point;
		topleft.x -= static_cast<sint16>(maxVisionRange);
		DynamicArray<Unit> unitArray;
		g_theUnitTree->SearchRect(unitArray, topleft,
								  static_cast<sint16>(maxVisionRange) * 2 + 1,
								  static_cast<sint16>(maxVisionRange) * 2 + 1,
								  1 << owner);
		sint32 un = unitArray.Num();
		for(i = 0; i < un; i++) {
			UnitData *ud = unitArray[i].AccessData();
			sint32 ls = UnitData::GetDistance(Installation(m_id),
											  ud, sint32(maxVisionRange));
			if(ls > maxrsq)
				continue;

			double hisr = ud->GetDBRec()->GetVisionRange()+0.5;
			sint32 hisrsq = sint32(hisr*hisr);
			if(ls <= hisrsq) {
				if(g_theTerrainImprovementDB->Get(m_type)->GetCanSee() &
				   ud->GetDBRec()->GetCanSee()) {
					canBeSeen = TRUE;
					break;
				}
			}
		}
	}

	if(canBeSeen) {
		m_visibility |= (1 << owner);
	}
}

sint32 InstallationData::GetDistance(InstallationData* inst1,
									 InstallationData* inst2,
									 sint32 wrapRange)
{
	sint32 dx, dy;
	dx = inst1->m_point.x - inst2->m_point.x;
	dy = inst1->m_point.y - inst2->m_point.y;
	dx = abs(dx);
	dy = abs(dy);
	if(dx > wrapRange) {
		if(inst1->m_point.x > inst2->m_point.x) {
			dx = (inst1->m_point.x - g_theWorld->GetXWidth()) - inst2->m_point.x;
		} else {
			dx = inst1->m_point.x - (inst2->m_point.x - g_theWorld->GetXWidth());
		}
		dx = abs(dx);
	}
	dy -= dx;

	return (dx * dx) + (dy * dy);
}

void InstallationData::RebuildQuadTree()
{
	g_theInstallationTree->Insert(Installation(m_id));
	if(m_lesser)
		((InstallationData*)m_lesser)->RebuildQuadTree();

	if(m_greater)
		((InstallationData*)m_greater)->RebuildQuadTree();
}

void InstallationData::UseAirfield()
{
	m_airfieldLastUsed = g_turn->GetRound();
}

sint32 InstallationData::AirfieldLastUsed() const
{
	return m_airfieldLastUsed;
}

void InstallationData::ChangeOwner(sint32 toOwner)
{
	// CityInfluenceIterator.cpp's border/influence flood-fill (ExpandBorders/
	// ExpandInfluence/GenerateBorders) calls World::SyncInstallationOwners()
	// - and so this - on every tile in a city's radius unconditionally,
	// including tiles already owned by that same player; unlike
	// World::ChangeOwner()'s own recursion, it has no "did the owner
	// actually change" guard. Skip the no-op case here instead: nothing
	// below (network sync, old/new owner reference and vision churn,
	// GaiaController bookkeeping) needs to run when the owner isn't
	// actually changing.
	if(toOwner == m_owner) {
		return;
	}

	sint32 const oldOwner = m_owner;

	// Set m_owner before doing any of the border/vision work below, not
	// after: GenerateBorders/terrainutil_RemoveBorders can themselves walk
	// back through World::SyncInstallationOwners for this same installation's
	// own tile (border/territory recalculation reaches back to where it
	// started), and that reentrant call needs the toOwner == m_owner guard
	// above to actually catch it. With m_owner only updated at the end (as
	// this used to do), every such reentrant call still saw the pre-change
	// owner, missed the guard, and re-ran this entire function - an
	// unbounded GenerateBorders <-> ChangeOwner <-> SyncInstallationOwners
	// recursion that hung a live playtest, flooding the log with the same
	// Vision::DoFillCircleOp underflow assert. oldOwner captures what this
	// function's own old-owner cleanup below still needs.
	m_owner = toOwner;

	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_CHANGE_INSTALLATION_OWNER,
									  m_id, oldOwner, toOwner));
	}

	// Endgame-flagged installations (Processing Towers) are counted per
	// owner by that owner's own GaiaController (m_numTowersBuilt /
	// RecomputeCoverage). It only ever hears about that via the
	// GEV_ImprovementComplete/GEV_CutImprovements event handlers (build/
	// destroy) and, for buildings/wonders only, GEV_CaptureCity - an
	// ordinary ownership change like this one (ArmyData::Pillage, city
	// capture without destruction, border/territory shift, player death)
	// was never told, so the old owner's count and coverage would go
	// stale and the new owner's would never see this tower at all.
	bool const isEndgameTower =
		(GaiaController::sm_endgameImprovements & ((uint64)0x1 << (uint64)m_type)) != 0;

	// A Fort grants its own border radius on completion (see
	// TerrainImprovementData::Complete()'s matching GenerateBorders() call)
	// - independently of whatever city, if any, happens to be nearby. That
	// grant was never revisited on an ownership change, so a captured
	// Fort kept its former owner's border claim in place and never gave
	// the new owner the border it should now provide. Airfields and
	// detector-class installations have no border radius of their own
	// (GetIntBorderRadius/GetSquaredBorderRadius both fail for them), so
	// this only actually fires for Forts in practice.
	sint32 intRad, sqRad;
	bool const hasOwnBorder =
		g_theTerrainImprovementDB->Get(m_type)->GetIntBorderRadius(intRad) &&
		g_theTerrainImprovementDB->Get(m_type)->GetSquaredBorderRadius(sqRad);

	if(oldOwner >= 0 && g_player[oldOwner] != NULL)
	{
		if(hasOwnBorder)
		{
			terrainutil_RemoveBorders(m_point, oldOwner, intRad, sqRad, Unit());
		}

		g_player[oldOwner]->RemoveInstallationReferences(Installation(m_id));

		// Just RemoveUnitVision() below - it already does exactly what this
		// used to duplicate. Before the vision objects were unified (see
		// "On screen vision is now the vision of the visible player instead
		// of a copy"), this called g_tiledMap->GetLocalVision()->RemoveVisible()
		// directly, a *separate* screen-only copy, so it was harmless. Now
		// g_tiledMap->GetLocalVision() (via RemoveUnitVision()'s director
		// path) and g_player[oldOwner]->m_vision are the same reference-
		// counted object, so calling both here double-decremented every
		// tile in the installation's vision circle on every ownership
		// change - and even single-decremented tiles for installations
		// with no vision at all (visionRange <= 0), since this call wasn't
		// guarded like AddInstallation()'s matching AddVisible() is. Over
		// many territory/ownership changes this silently drained vision
		// ref-counts empire-wide, including at tiles this installation
		// never actually contributed to.
		double visionRange = terrainutil_GetVisionRange(m_type, m_point);
		if(visionRange > 0)
		{
			g_player[oldOwner]->RemoveUnitVision(m_point, visionRange);
		}

		if(isEndgameTower && g_player[oldOwner]->GetGaiaController())
		{
			g_player[oldOwner]->GetGaiaController()->
				HandleTerrImprovementChange(m_type, m_point, -1);
		}
	}

	if(toOwner >= 0)
	{
		g_player[toOwner]->AddInstallation(Installation(m_id));

		if(hasOwnBorder)
		{
			GenerateBorders(m_point, toOwner, intRad, sqRad);
		}

		if(isEndgameTower && g_player[toOwner]->GetGaiaController())
		{
			g_player[toOwner]->GetGaiaController()->
				HandleTerrImprovementChange(m_type, m_point, 1);
		}
	}

	if(toOwner >= 0)
	{
		DoVision();
	}
}

void InstallationData::SetVisible(sint32 player)
{
	m_visibility |= (1 << player);
	g_tiledMap->RedrawTile(&m_point);
}
