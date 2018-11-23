#include "c3.h"
#include "Globals.h"
#include "player.h"
#include "Unit.h"
#include "DiplomaticRequestData.h"
#include "DiplDynArr.h"

DiplomaticRequestDynamicArray::DiplomaticRequestDynamicArray() : DynamicArray<DiplomaticRequest>()
	{
	}

DiplomaticRequestDynamicArray::DiplomaticRequestDynamicArray(const sint32 size) : DynamicArray<DiplomaticRequest>(size)
	{
	}

DiplomaticRequestDynamicArray::DiplomaticRequestDynamicArray (const DynamicArray<DiplomaticRequest> &copyme) : DynamicArray<DiplomaticRequest> (copyme)
	{
	}
