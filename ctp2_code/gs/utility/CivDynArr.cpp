#include "c3.h"
#include "Globals.h"
#include "player.h"
#include "Unit.h"

#include "CivDynArr.h"

CivilisationDynamicArray::CivilisationDynamicArray() : DynamicArray<Civilisation>()
	{
	}

CivilisationDynamicArray::CivilisationDynamicArray(const sint32 size) : DynamicArray<Civilisation>(size)
	{
	}

CivilisationDynamicArray::CivilisationDynamicArray (const DynamicArray<Civilisation> &copyme) : DynamicArray<Civilisation> (copyme)
	{
	}
