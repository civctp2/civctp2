//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Slic eyepoint message
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
// - AOM facilitation: set player[0] to the recipient.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "SlicEyePoint.h"
#include "civarchive.h"
#include "radarmap.h"
#include "message.h"
#include "tiledmap.h"
#include "SelItem.h"
#include "UnitPool.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "SlicSegment.h"
#include "director.h"

extern RadarMap *g_radarMap;
extern TiledMap	*g_tiledMap;
extern SelectedItem *g_selected_item;
extern UnitPool *g_theUnitPool;
extern Director *g_director;

#ifdef _BAD_EYE
PointerList<SlicEyePoint> s_deletedEyepoints;
#endif

SlicEyePoint::SlicEyePoint(const MapPoint &point, const MBCHAR *name,
						   sint32 data, EYE_POINT_TYPE type,
						   const Unit &unit,
						   sint32 recipient,
						   SlicSegment *segment)
{
	m_point = point;
	if(name) {
		m_name = new char[strlen(name) + 1];
		strcpy(m_name, name);
	} else {
		m_name = NULL;
	}
	m_message = new Message(0);
	m_data = data;
	m_type = type;
	m_unit = unit;

	m_segment = segment;
	m_recipient = recipient;
}

SlicEyePoint::SlicEyePoint(SlicEyePoint *copy)
{
	m_point = copy->m_point;
	if(copy->m_name) {
		m_name = new char[strlen(copy->m_name) + 1];
		strcpy(m_name, copy->m_name);
	} else {
		m_name = NULL;
	}
	m_message = new Message(*copy->m_message);
	m_data = copy->m_data;
	m_type = copy->m_type;
	m_unit = copy->m_unit;
	m_recipient = copy->m_recipient;
	m_segment = copy->m_segment;
}

SlicEyePoint::~SlicEyePoint()
{
#ifdef _BAD_EYE
	s_deletedEyepoints.AddTail(this);
#endif

	if(m_name)
		delete [] m_name;

	if(m_message)
		delete m_message;
}

SlicEyePoint::SlicEyePoint(CivArchive &archive)
{
	m_message = new Message(0);
	Serialize(archive);
}

void SlicEyePoint::Serialize(CivArchive &archive)
{
	m_point.Serialize(archive);
	m_message->Serialize(archive);
	m_unit.Serialize(archive);
	sint32 l;
	if(archive.IsStoring()) {
		if(m_name) {
			l = strlen(m_name) + 1;
			archive << l;
			archive.Store((uint8*)m_name, l);
		} else {
			l = 0;
			archive << l;
		}

		if(m_segment) {
			l = strlen(m_segment->GetName()) + 1;
			archive << l;
			archive.Store((uint8*)m_segment->GetName(), l);
		} else {
			l = 0;
			archive << l;
		}
		archive << m_recipient;

		archive << m_data;
		sint32 t = (sint32)m_type;
		archive << t;
	} else {
		archive >> l;
		if(l > 0) {
			m_name = new char[l];
			archive.Load((uint8*)m_name, l);
		} else {
			m_name = NULL;
		}
		archive >> l;
		if(l > 0 && l < 1024) {
			char segname[1024];
			archive.Load((uint8*)segname, l);
			m_segment = g_slicEngine->GetSegment(segname);
		} else {
			m_segment = NULL;
		}

		archive >> m_recipient;
		archive >> m_data;
		sint32 t;
		archive >> t;
		m_type =(EYE_POINT_TYPE)t;
	}
}

void SlicEyePoint::SetMessage(const Message &message)
{
#ifdef _BAD_EYE
	Assert(!s_deletedEyepoints.Find(this));
#endif

	*m_message = message;
}

Message SlicEyePoint::GetMessage() const
{
	return *m_message;
}

void SlicEyePoint::Callback()
{
	SlicObject * obj = NULL;
	if (m_segment)
    {
		obj = new SlicObject(m_segment);
		obj->AddRecipient(m_recipient);
        obj->AddPlayer(m_recipient);
	}

	switch(m_type) {
		case EYE_POINT_TYPE_UNIT:
		case EYE_POINT_TYPE_CITY:
		case EYE_POINT_TYPE_GENERIC:
			if(m_unit.m_id != (0)) {
				if(g_theUnitPool->IsValid(m_unit) &&
				   m_unit.GetOwner() == g_selected_item->GetVisiblePlayer()) {
					g_selected_item->SetSelectUnit(m_unit);
					g_director->AddCenterMap(m_point);
					if(obj)
						obj->AddUnit(m_unit);
				}
			} else {
				g_director->AddCenterMap(m_point);
				if(obj)
					obj->AddLocation(m_point);
			}
			g_tiledMap->Refresh();
			g_tiledMap->InvalidateMap();
			break;
		case EYE_POINT_TYPE_ADVANCE:
			Assert(*m_message != Message(0));
			if(*m_message != Message(0)) {
				m_message->SetSelectedAdvance(m_data);
			}
			if(obj)
				obj->AddAdvance(m_data);
			break;
        case EYE_POINT_TYPE_NOTHING:
            break;
		default:
			Assert(FALSE);
	}

	if(obj) {
		Message oldMsg = g_slicEngine->GetEyepointMessage();
		g_slicEngine->SetEyepointMessage(*m_message);
		g_slicEngine->Execute(obj);

		g_slicEngine->SetEyepointMessage(oldMsg);
	}
}
