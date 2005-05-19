













#pragma once
#ifndef __CIVILISATION_DYNAMIC_ARRAY_H__
#define __CIVILISATION_DYNAMIC_ARRAY_H__

#include "Civilisation.h"
#include "dynarr.h"


class CivilisationDynamicArray : public DynamicArray<Civilisation> {


public:

    CivilisationDynamicArray(); 
    CivilisationDynamicArray(const sint32 size); 
    CivilisationDynamicArray (const DynamicArray<Civilisation> &copyme); 

};

#endif
