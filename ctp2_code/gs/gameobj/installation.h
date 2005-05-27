








#pragma once
#ifndef _INSTALLATION_H_
#define _INSTALLATION_H_

#include "ID.h"
#include "installationdata.h"

class TerrainImprovementRecord;

class Installation : public ID {
private:
public:
	Installation () : ID() { return; } ; 
	Installation (sint32 val) : ID (val) { return; }; 
	Installation (uint32 val) : ID (val) { return; }; 
#ifdef WIN32
	Installation (const int val) : ID (val) { return; }; 
	Installation (const unsigned int val) : ID (val) { return; }; 
#endif

	void KillInstallation();
	void Kill() { KillInstallation(); }
	void RemoveAllReferences();

	const InstallationData *GetData() const;
	InstallationData *AccessData();
	const TerrainImprovementRecord *GetDBRec() const;

	sint32 GetOwner() const { return GetData()->GetOwner(); }
	sint32 GetType() const { return GetData()->GetType(); }
	void GetPos(MapPoint &pos) const { pos = GetData()->GetLocation(); }
	MapPoint RetPos() const { return GetData()->GetLocation(); }
	uint32 GetVisibility() const { return GetData()->GetVisibility(); }
	void SetVisible(sint32 player) { AccessData()->SetVisible(player); }
	void UnsetVisible(sint32 player) { AccessData()->UnsetVisible(player); }
	void CheckVision(sint32 owner) { AccessData()->CheckVision(owner); }

	void UseAirfield();
	sint32 AirfieldLastUsed() const;

	void ChangeOwner(sint32 i);
};

#endif
