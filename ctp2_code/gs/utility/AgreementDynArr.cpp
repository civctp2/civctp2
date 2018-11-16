#include "c3.h"

#include "Unit.h"
#include "Advances.h"
#include "AgreementDynArr.h"

AgreementDynamicArray::AgreementDynamicArray() : DynamicArray<Agreement>()
	{
	}

AgreementDynamicArray::AgreementDynamicArray(const sint32 size) : DynamicArray<Agreement>(size)
	{
	}

AgreementDynamicArray::AgreementDynamicArray (const DynamicArray<Agreement> &copyme) : DynamicArray<Agreement> (copyme)
	{
	}
