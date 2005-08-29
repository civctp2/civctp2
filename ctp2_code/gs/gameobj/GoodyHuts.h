//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Goody hut handling
// Id           : $Id:$
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
// - Speeded up goody hut advance and unit selection.
//
//----------------------------------------------------------------------------

#ifndef _GOODY_HUTS_H_
#define _GOODY_HUTS_H_

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// #include <>

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class GoodyHut;

enum GOODY 
{
	GOODY_BOGUS,
	GOODY_GOLD,
	GOODY_ADVANCE,
	GOODY_UNIT,
	GOODY_CITY,
	GOODY_BARBARIANS,
	GOODY_MAX,                  // pseudo-value, used as counter
    GOODY_SETTLER   = GOODY_MAX // pseudo-value, only used internally
};

#define k_VALUE_RANGE 10000

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "ctp2_inttypes.h"      // uint32
#include "player.h"             // PLAYER_INDEX

class CivArchive;
class MapPoint;
class NetCellList;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class GoodyHut 
{
public:
	GoodyHut();
	GoodyHut(uint32 type, uint32 value);
	GoodyHut(CivArchive &archive);

	void OpenGoody(PLAYER_INDEX const & owner, MapPoint const & point);
	void Serialize(CivArchive & archive);

private:
//----------------------------------------------------------------------------
// Do not change anything in the types or order of the following variable 
// declarations. Doing so will break reading in of save files.
// See the Serialize implementation for more details.
//----------------------------------------------------------------------------
	uint32              m_value;
	uint32              m_typeValue;
//----------------------------------------------------------------------------
// Changing the order below this line should not break anything.
//----------------------------------------------------------------------------

	GOODY ChooseType(PLAYER_INDEX const & owner);

	friend class NetCellList;
};

#endif
