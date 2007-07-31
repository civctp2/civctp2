








#pragma once
#ifndef _INSTALLATION_DATA_H_
#define _INSTALLATION_DATA_H_

#include "gameobj.h"
#include "MapPoint.h"

class CivArchive;

class InstallationData : public GameObj
{
private:
	sint32 m_owner;
	sint32 m_type;
	MapPoint m_point;
	sint32 m_airfieldLastUsed;
	uint32 m_visibility;

	friend class NetInstallation;

public:
	InstallationData(ID id, sint32 owner, MapPoint &pnt,
					 sint32 type);
	InstallationData(CivArchive &archive);
	InstallationData(ID id) : GameObj(id.m_id) {}

	sint32 GetOwner() const { return m_owner; }
	sint32 GetType() const { return m_type; }
	MapPoint GetLocation() const { return m_point; }
	uint32 GetVisibility() const { return m_visibility; }
	void SetVisible(sint32 player);
	void UnsetVisible(sint32 player) { m_visibility &= ~(1 << player); }
	void DoVision();
	void CheckVision(sint32 owner);
	
	static sint32 GetDistance(InstallationData *inst1, InstallationData *inst2,
							  sint32 wrap);

	void Serialize(CivArchive &archive);
	void RebuildQuadTree();

	void UseAirfield();
	sint32 AirfieldLastUsed() const;

	void ChangeOwner(sint32 toOwner);
};

#endif
