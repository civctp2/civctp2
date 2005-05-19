

#include "c3.h"
#include "SlicRecord.h"
#include "SlicEngine.h"
#include "MessagePool.h"
#include "civarchive.h"
#include "player.h"
#include "dynarr.h"
#include "SlicSegment.h"
#include "slicif.h"

extern MessagePool *g_theMessagePool;
extern Player **g_player;

SlicRecord::SlicRecord(sint32 owner, MBCHAR *title, MBCHAR *text,
					   SlicSegment *segment)
{
	m_owner = owner;

	if(title) {
		m_title = new char[strlen(title) + 1];
		strcpy(m_title, title);
	} else {
		m_title = NULL;
	}

	if(text) {
		m_text = new char[strlen(text) + 1];
		strcpy(m_text, text);
	} else {
		m_text = NULL;
	}
	m_segment = segment;
}

SlicRecord::SlicRecord(CivArchive &archive)
{
	Serialize(archive);
}

SlicRecord::~SlicRecord()
{
	if(m_title) {
		delete [] m_title;
		m_title = NULL;
	}

	if(m_text) {
		delete [] m_text;
		m_text = NULL;
	}
}

void SlicRecord::Serialize(CivArchive &archive)
{
	sint32 l;
	if(archive.IsStoring()) {
		archive << m_owner;
		if(m_title) {
			l = strlen(m_title) + 1;
			archive << l;
			archive.Store((uint8*)m_title, l * sizeof(MBCHAR));
		} else {
			l = -1;
			archive << l;
		}

		if(m_text) {
			l = strlen(m_text) + 1;
			archive << l;
			archive.Store((uint8*)m_text, l * sizeof(MBCHAR));
		} else {
			l = -1;
			archive << l;
		}

		l = strlen(m_segment->GetName()) + 1;
		archive << l;
		archive.Store((uint8*)m_segment->GetName(), l);
	} else {
		archive >> m_owner;
		archive >> l;
		if(l < 0) {
			m_title = NULL;
		} else {
			m_title = new MBCHAR[l];
			archive.Load((uint8*)m_title, l * sizeof(MBCHAR));
		}

		archive >> l;
		if(l < 0) {
			m_text = NULL;
		} else {
			m_text = new MBCHAR[l];
			archive.Load((uint8*)m_text, l * sizeof(MBCHAR));
		}

		archive >> l;
		char segmentName[k_MAX_SLIC_STRING];
		archive.Load((uint8*)segmentName, l);
		m_segment = g_slicEngine->GetSegment(segmentName);
	}
}

void SlicRecord::Reconstitute()
{
	sint32 i;
	for(i = g_player[m_owner]->m_messages->Num() - 1; i >= 0; i--) {
		if(g_player[m_owner]->m_messages->Access(i).GetClass() == 
		   k_HACK_RECONSTITUTED_CLASS) {
			g_player[m_owner]->m_messages->Access(i).Kill();
		}
	}

	Message msg = g_theMessagePool->Recreate(m_owner, m_text, m_title);
	Assert(g_theMessagePool->IsValid(msg));
	if(g_theMessagePool->IsValid(msg)) {
		msg.SetClass(k_HACK_RECONSTITUTED_CLASS);
		msg.Show();
	}
}

