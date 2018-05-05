#pragma once
#ifndef __AGREEMENT_DYNAMIC_ARRAY_H__
#define __AGREEMENT_DYNAMIC_ARRAY_H__

#include "gs/gameobj/Agreement.h"
#include "robot/aibackdoor/dynarr.h"

class AgreementDynamicArray : public DynamicArray<Agreement> {

public:

    AgreementDynamicArray();
    AgreementDynamicArray(const sint32 size);
    AgreementDynamicArray (const DynamicArray<Agreement> &copyme);

};

#endif
