//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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

#pragma once

//----------------------------------------------------------------------------
//
// Library imports
//
//----------------------------------------------------------------------------

#include <memory>

//----------------------------------------------------------------------------
//
// Exported names
//
//----------------------------------------------------------------------------

enum SEQ_ACTOR {
  SEQ_ACTOR_PRIMARY = 0,
  SEQ_ACTOR_SECONDARY = 1,

  SEQ_ACTOR_MAX
};

//----------------------------------------------------------------------------
//
// Project imports
//
//----------------------------------------------------------------------------

#include "os/include/ctp2_inttypes.h"  // sint32

class DQItem;

//----------------------------------------------------------------------------
//
// Declarations
//
//----------------------------------------------------------------------------

class Sequence {
 public:
  explicit Sequence(sint32 seqID = 0);
  ~Sequence();

  void AddRef();
  void Release();
  sint32 GetRefCount() const;

  sint32 GetSequenceID(void) const;

  void SetItem(std::weak_ptr<DQItem> item);
  std::shared_ptr<DQItem> GetItem(void);

  void SetAddedToActiveList(SEQ_ACTOR which, bool added);
  bool GetAddedToActiveList(SEQ_ACTOR which) const;

 private:
  sint32 m_refCount;
  sint32 m_sequenceID;
  std::weak_ptr<DQItem> m_item;
  bool m_addedToActiveList[SEQ_ACTOR_MAX];
};

typedef std::shared_ptr<Sequence> SequencePtr;
typedef std::weak_ptr<Sequence> SequenceWeakPtr;