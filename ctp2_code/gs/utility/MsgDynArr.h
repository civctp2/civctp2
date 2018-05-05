#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __MESSAGE_DYNAMIC_ARRAY_H__
#define __MESSAGE_DYNAMIC_ARRAY_H__

#include "gs/gameobj/message.h"
#include "robot/aibackdoor/dynarr.h"

class MessageDynamicArray : public DynamicArray<Message> {

public:

    MessageDynamicArray();
    MessageDynamicArray(const sint32 size);
    MessageDynamicArray (const DynamicArray<Message> &copyme);

};

#endif
