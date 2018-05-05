#include "ctp/c3.h"
#include "gs/utility/Globals.h"
#include "gs/gameobj/Player.h"
#include "gs/gameobj/Unit.h"
#include "gs/gameobj/DiplomaticRequestData.h"
#include "gs/utility/DiplDynArr.h"

DiplomaticRequestDynamicArray::DiplomaticRequestDynamicArray() : DynamicArray<DiplomaticRequest>()
	{
	}

DiplomaticRequestDynamicArray::DiplomaticRequestDynamicArray(const sint32 size) : DynamicArray<DiplomaticRequest>(size)
	{
	}

DiplomaticRequestDynamicArray::DiplomaticRequestDynamicArray (const DynamicArray<DiplomaticRequest> &copyme) : DynamicArray<DiplomaticRequest> (copyme)
	{
	}
