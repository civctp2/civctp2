#include "ctp/c3.h"

#include "gs/gameobj/Unit.h"
#include "gs/gameobj/Advances.h"
#include "gs/utility/AgreementDynArr.h"

AgreementDynamicArray::AgreementDynamicArray() : DynamicArray<Agreement>()
	{
	}

AgreementDynamicArray::AgreementDynamicArray(const sint32 size) : DynamicArray<Agreement>(size)
	{
	}

AgreementDynamicArray::AgreementDynamicArray (const DynamicArray<Agreement> &copyme) : DynamicArray<Agreement> (copyme)
	{
	}
