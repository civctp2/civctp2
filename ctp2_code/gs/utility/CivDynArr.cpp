#include "ctp/c3.h"
#include "gs/utility/Globals.h"
#include "gs/gameobj/Player.h"
#include "gs/gameobj/Unit.h"

#include "gs/utility/CivDynArr.h"

CivilisationDynamicArray::CivilisationDynamicArray() : DynamicArray<Civilisation>()
	{
	}

CivilisationDynamicArray::CivilisationDynamicArray(const sint32 size) : DynamicArray<Civilisation>(size)
	{
	}

CivilisationDynamicArray::CivilisationDynamicArray (const DynamicArray<Civilisation> &copyme) : DynamicArray<Civilisation> (copyme)
	{
	}
