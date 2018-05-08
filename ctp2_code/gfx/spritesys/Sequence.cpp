//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  :
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// Extracted from Director.h
//
//----------------------------------------------------------------------------

#include "ctp/c3.h"
#include "gfx/spritesys/Sequence.h"

Sequence::Sequence(sint32 seqID) : m_refCount(0), m_sequenceID(seqID) {
  m_addedToActiveList[SEQ_ACTOR_PRIMARY] = false;
  m_addedToActiveList[SEQ_ACTOR_SECONDARY] = false;
}

Sequence::~Sequence() {
  m_item.reset();
}

void Sequence::AddRef() {
  ++m_refCount;
}

void Sequence::Release() {
  --m_refCount;
}

sint32 Sequence::GetRefCount() const {
  return m_refCount;
}

sint32 Sequence::GetSequenceID(void) const {
  return m_sequenceID;
}

void Sequence::SetItem(std::weak_ptr<DQItem> item) {
  m_item = item;
}

std::shared_ptr<DQItem> Sequence::GetItem(void) {
  Assert(!m_item.expired());
  return m_item.lock();
}

void Sequence::SetAddedToActiveList(SEQ_ACTOR which, bool added) {
  m_addedToActiveList[which] = added;
}

bool Sequence::GetAddedToActiveList(SEQ_ACTOR which) const {
  return m_addedToActiveList[which];
}
