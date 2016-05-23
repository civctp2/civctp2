//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Slic message box buttons
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
// _BAD_BUTTON
// - Tracts a list of deleted buttons.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Cleaned SlicButton::Callback method. (Sep. 24th 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "SlicButton.h"
#include "SlicObject.h"
#include "StrDB.h"
#include "civarchive.h"
#include "SlicSegment.h"
#include "SlicEngine.h"
#include "SlicFrame.h"
#include "message.h"

extern StringDB *g_theStringDB;

#ifdef _BAD_BUTTON
PointerList<SlicButton> s_deletedButtons;
#endif

SlicButton::SlicButton(StringId name, SlicSegment *segment,
                       sint32 codeOffset, SlicObject *context)
:   m_name         (name),
    m_codeOffset   (codeOffset),
    m_message      (new Message()),
    m_context      (context),
    m_segment      (segment),
    m_segmentName  (NULL)
{
	if(m_name >= 0) {
		m_isCloseEvent = FALSE;
	} else {
		m_isCloseEvent = TRUE;
	}
	m_context->AddRef();
}

SlicButton::SlicButton(SlicButton *copy)
:   m_name         (copy->m_name),
    m_isCloseEvent (copy->m_isCloseEvent),
    m_codeOffset   (copy->m_codeOffset),
    m_message      (new Message(*copy->m_message)),
    m_context      (copy->m_context),
    m_segment      (copy->m_segment)
{
	m_context->AddRef();

	if(copy->m_segmentName) {
		m_segmentName = new char[strlen(copy->m_segmentName) + 1];
		strcpy(m_segmentName, copy->m_segmentName);
	} else {
		m_segmentName = NULL;
	}
}

SlicButton::SlicButton(CivArchive &archive)
{
	m_message = new Message();
	Serialize(archive);
}

SlicButton::~SlicButton()
{
#ifdef _BAD_BUTTON
	s_deletedButtons.AddTail(this);
#endif
	m_context->Release();
	if(m_segmentName) {
		delete [] m_segmentName;
		m_segmentName = NULL;
	}

	if(m_message)
		delete m_message;
}

void SlicButton::Serialize(CivArchive &archive)
{
	sint32 l;

	if(archive.IsStoring()) {
		archive << m_name;
		archive << m_codeOffset;
		archive << m_isCloseEvent;

		if(m_segment) {
			l = strlen(m_segment->GetName()) + 1;
			archive << l;
			archive.Store((uint8*)m_segment->GetName(), l);
		} else {
			l = strlen(m_segmentName) + 1;
			archive << l;
			archive.Store((uint8*)m_segmentName, l);
		}
		m_context->Serialize(archive);
	} else {
		archive >> m_name;
		archive >> m_codeOffset;
		archive >> m_isCloseEvent;

		archive >> l;
		m_segment = NULL;
		m_segmentName = new char[l];
		archive.Load((uint8*)m_segmentName, l);
		m_context = new SlicObject(archive);
		m_context->AddRef();
	}
	m_message->Serialize(archive);
}

void SlicButton::Callback()
{
	if(m_segmentName && !m_segment) {
		m_segment = g_slicEngine->GetSegment(m_segmentName);
		delete [] m_segmentName;
		m_segmentName = NULL;
	}

#ifdef _DEBUG
	if(m_name >= 0) {
		Assert(GetName());
		if(!GetName()) {
			return;
		}
		DPRINTF(k_DBG_SLIC, ("Button %s clicked\n", GetName()));
	}
#endif

	Message oldmessage;

	g_slicEngine->GetCurrentMessage(oldmessage);

	g_slicEngine->PushContext(m_context);
	g_slicEngine->SetCurrentMessage(*m_message);

	if(m_context->GetFrame()){
		m_context->GetFrame()->RunAt(m_codeOffset);
	}
	else{
		Assert(false);
		SlicFrame *frame = new SlicFrame(m_segment);
		frame->RunAt(m_codeOffset);
		delete frame;
	}

	g_slicEngine->SetCurrentMessage(oldmessage);
	g_slicEngine->PopContext();
}

const MBCHAR *SlicButton::GetName() const
{
	return g_theStringDB->GetNameStr(m_name);
}

void SlicButton::SetMessage(const Message &message)
{
#ifdef _BAD_BUTTON
	PointerList<SlicButton>::Walker walk(&s_deletedButtons);
	for(; walk.IsValid(); walk.Next()) {
		Assert(walk.GetObj() != this);
	}
#endif

	*m_message = message;
}

void SlicButton::GetMessage(Message &message) const
{
	message = *m_message;
}

BOOL SlicButton::IsCloseEvent()
{
	return m_isCloseEvent;
}
