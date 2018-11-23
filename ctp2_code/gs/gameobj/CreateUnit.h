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
					  BOOL temp,
					  CAUSE_NEW_ARMY cause) :
		m_next(NULL),
		m_type(t),
		m_point(point),
		m_homeCity(hc),
		m_tempUnit(temp),
		m_cause(cause)
	{}

	sint32 m_type;
	MapPoint m_point;
	Unit m_homeCity;
	BOOL m_tempUnit;
	CAUSE_NEW_ARMY m_cause;
};

#endif
