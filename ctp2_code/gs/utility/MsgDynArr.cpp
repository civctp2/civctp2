#include "ctp/c3.h"
#include "gs/utility/Globals.h"
#include "gs/gameobj/Player.h"
#include "gs/gameobj/Unit.h"
#include "gs/gameobj/MessageData.h"
#include "gs/utility/MsgDynArr.h"

MessageDynamicArray::MessageDynamicArray() : DynamicArray<Message>()
	{
	}

MessageDynamicArray::MessageDynamicArray(const sint32 size) : DynamicArray<Message>(size)
	{
	}

MessageDynamicArray::MessageDynamicArray (const DynamicArray<Message> &copyme) : DynamicArray<Message> (copyme)
	{
	}
