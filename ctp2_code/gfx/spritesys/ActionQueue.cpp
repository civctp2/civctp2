//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Helper class for action queue
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
// None
//
//----------------------------------------------------------------------------
///
/// \file   ActionQueue.cpp
/// \brief  Helper class for action queue

#include "ctp/c3.h"
#include "ActionQueue.h"

#include <cassert>

#include "gfx/spritesys/Action.h"

ActionQueue::ActionQueue() :
    m_capacity(k_MAX_ACTION_QUEUE_SIZE) {
}

ActionQueue::ActionQueue(unsigned capacity) :
    m_capacity(capacity) {
  assert(m_capacity > 0);
}

ActionQueue::ActionQueue(const ActionQueue &rhs) {
  *this = rhs;
}

ActionQueue::~ActionQueue() {
}

ActionQueue &ActionQueue::operator=(const ActionQueue &rhs) {
  if (this != &rhs) {
    m_capacity = rhs.m_capacity;
    assert(m_capacity > 0);
    m_actionQueue.clear();
    for (const ActionPtr &action : rhs.m_actionQueue) {
      m_actionQueue.push_back(ActionPtr(new Action(*action)));
    }
  }
  return *this;
}

void ActionQueue::Push(const ActionPtr &action) {
  m_actionQueue.push_back(action);
  if (m_actionQueue.size() > m_capacity) {
    m_actionQueue.pop_front();
  }
}

void ActionQueue::Pop() {
  m_actionQueue.pop_back();
}

ActionQueue::ActionPtr &ActionQueue::Back() {
  return m_actionQueue.back();
}

bool ActionQueue::Empty() const {
  return m_actionQueue.empty();
}

std::size_t ActionQueue::Size() const {
  return m_actionQueue.size();
}

void ActionQueue::Clear() {
  return m_actionQueue.clear();
}

ActionQueue::ContainerType &ActionQueue::Container() {
  return m_actionQueue;
}

const ActionQueue::ContainerType &ActionQueue::Container() const {
  return m_actionQueue;
}
