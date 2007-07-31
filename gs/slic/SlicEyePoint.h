
#pragma once
#ifndef __SLIC_EYE_POINT_H__
#define __SLIC_EYE_POINT_H__

#include "MapPoint.h"
#include "Unit.h"

class CivArchive;
class Message;
class SlicSegment;

enum EYE_POINT_TYPE {
	EYE_POINT_TYPE_GENERIC,
	EYE_POINT_TYPE_CITY,
	EYE_POINT_TYPE_UNIT,
	EYE_POINT_TYPE_ADVANCE,
	EYE_POINT_TYPE_NOTHING
};

class SlicEyePoint
{
public:
	SlicEyePoint(const MapPoint &point, const MBCHAR *name,
				 sint32 data, EYE_POINT_TYPE type,
				 const Unit &unit, 
				 sint32 recipient,
				 SlicSegment *segment);

	SlicEyePoint(SlicEyePoint *copy);
	SlicEyePoint(CivArchive &archive);
	~SlicEyePoint();
	void Serialize(CivArchive &archive);

	void GetPoint(MapPoint &point) { point = m_point; }
	const MBCHAR *GetName() { return m_name; }

	void SetMessage(const Message &message);
	Message GetMessage() const;

	void Callback();

private:
	MapPoint m_point;
	MBCHAR *m_name;
	Message *m_message;
	sint32 m_data;
	Unit m_unit;
	sint32 m_recipient;
	SlicSegment *m_segment;

	EYE_POINT_TYPE m_type;

};

#endif
