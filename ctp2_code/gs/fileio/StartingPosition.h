#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef STARTING_POSITION_H_
#define STARTING_POSITION_H_

struct StartingPosition;

enum STARTINFOTYPE {
	STARTINFOTYPE_NONE,
	STARTINFOTYPE_CIVSFIXED,
	STARTINFOTYPE_POSITIONSFIXED,
	STARTINFOTYPE_CIVS,

	STARTINFOTYPE_NOLOCS,
};

#include "ctp2_inttypes.h"
#include "MapPoint.h"

struct StartingPosition {
	MapPoint point;
	sint32 civIndex;
};

#define k_MAX_START_POINTS 64

//----------------------------------------------------------------------------
// On-disk layouts of a starting position. Save versions below
// k_SAVE_VERSION_MAP_PLANE stored the MapPoint as x and y only; later versions
// add the plane z and two reserved bytes. The fields are written one by one
// instead of copying StartingPosition, so adding a member to MapPoint cannot
// change the file format again.
// Spec: specs/ctp2-dual-maps.spec.md, DM-ENG-REQ-005.
//----------------------------------------------------------------------------
struct StartingPositionDiskV66 {
	sint16 x;
	sint16 y;
	sint32 civIndex;
};

struct StartingPositionDisk {
	sint16 x;
	sint16 y;
	sint16 z;
	sint16 reserved;
	sint32 civIndex;
};

static_assert(sizeof(StartingPositionDiskV66) == 8, "legacy starting position record must stay 8 bytes");
static_assert(sizeof(StartingPositionDisk) == 12, "starting position record must stay 12 bytes");

inline void StartingPosition_ToDisk(StartingPositionDisk * dest, StartingPosition const * src, size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		dest[i].x         = src[i].point.x;
		dest[i].y         = src[i].point.y;
		dest[i].z         = src[i].point.z;
		dest[i].reserved  = 0;
		dest[i].civIndex  = src[i].civIndex;
	}
}

inline void StartingPosition_FromDisk(StartingPosition * dest, StartingPositionDisk const * src, size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		dest[i].point    = MapPoint(src[i].x, src[i].y, src[i].z);
		dest[i].civIndex = src[i].civIndex;
	}
}

inline void StartingPosition_FromDiskV66(StartingPosition * dest, StartingPositionDiskV66 const * src, size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		dest[i].point    = MapPoint(src[i].x, src[i].y);
		dest[i].civIndex = src[i].civIndex;
	}
}

#endif
