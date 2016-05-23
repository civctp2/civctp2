//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C/C++ header
// Description  : Unit Creation
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
// - None
//
//----------------------------------------------------------------------------

#ifndef __CREATE_UNIT_H__
#define __CREATE_UNIT_H__

#include "MapPoint.h"
#include "Unit.h"

class CreateUnitRequest {
public:
	CreateUnitRequest *m_next;

	CreateUnitRequest(sint32 t,
					  const MapPoint &point,
					  const Unit hc,
					  bool temp,
					  CAUSE_NEW_ARMY cause) :
		m_type(t),
		m_point(point),
		m_homeCity(hc),
		m_tempUnit(temp),
		m_cause(cause),
		m_next(NULL)
	{}

	sint32 m_type;
	MapPoint m_point;
	Unit m_homeCity;
	bool m_tempUnit;
	CAUSE_NEW_ARMY m_cause;
};

#endif
