










#include "c3.h"
#include "Globals.h"
#include "Player.h"
#include "Unit.h"
#include "MessageData.h"
#include "MsgDynArr.h"


MessageDynamicArray::MessageDynamicArray() : DynamicArray<Message>()
	{ 
	}


MessageDynamicArray::MessageDynamicArray(const sint32 size) : DynamicArray<Message>(size)
	{ 
	}


MessageDynamicArray::MessageDynamicArray (const DynamicArray<Message> &copyme) : DynamicArray<Message> (copyme)
	{
	}


