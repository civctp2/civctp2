//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Helper class for action queue
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
// None
//
//----------------------------------------------------------------------------
///
/// \file   ActionQueue.h
/// \brief  Helper class for action queue

#pragma once

#include <deque>
#include <memory>

// "fix" bug #4 by kaan
#define k_MAX_ACTION_QUEUE_SIZE		12

class Action;

class ActionQueue {
public:
  typedef std::shared_ptr<Action> ActionPtr;
  typedef std::deque<ActionPtr> ContainerType;

  ActionQueue();
  explicit ActionQueue(unsigned capacity);
  ActionQueue(const ActionQueue &rhs);
  virtual ~ActionQueue();
  ActionQueue &operator=(const ActionQueue &rhs);

  void Push(const ActionPtr &action);
  void Pop();
  ActionPtr &Back();
  bool Empty() const;
  std::size_t Size() const;
  void Clear();
  ContainerType &Container();
  const ContainerType &Container() const;

protected:
  unsigned m_capacity;
  std::deque<ActionPtr> m_actionQueue;
};