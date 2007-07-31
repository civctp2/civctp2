













#pragma once
#ifndef __DIPLOMATICREQUEST_DYNAMIC_ARRAY_H__
#define __DIPLOMATICREQUEST_DYNAMIC_ARRAY_H__

#include "DiplomaticRequest.h"
#include "DynArr.h"


class DiplomaticRequestDynamicArray : public DynamicArray<DiplomaticRequest> {


public:

    DiplomaticRequestDynamicArray(); 
    DiplomaticRequestDynamicArray(const sint32 size); 
    DiplomaticRequestDynamicArray (const DynamicArray<DiplomaticRequest> &copyme); 

};

#endif
