
#pragma once
#ifndef __SLIC_BUILTIN_H__
#define __SLIC_BUILTIN_H__

#include "SlicStack.h"
#include "SlicStruct.h"
#include "SlicBuiltinEnum.h"
#include "SlicSymbol.h"

#define SLICSTRUCT(name, type) \
class SlicStruct_##name : public SlicStructDescription {\
public:\
    SlicStruct_##name();\
    SlicSymbolData *CreateDataSymbol() { return new SlicSymbolData(type); }\
};







class SlicStruct_Global : public SlicStructDescription
{
public:
	SlicStruct_Global();
	SlicSymbolData *CreateDataSymbol() { return NULL; }
};

SLICSTRUCT(Unit, SLIC_SYM_UNIT);
SLICSTRUCT(City, SLIC_SYM_CITY);
SLICSTRUCT(Player, SLIC_SYM_IVAR); 
SLICSTRUCT(Army, SLIC_SYM_ARMY);
SLICSTRUCT(Location, SLIC_SYM_LOCATION);
SLICSTRUCT(Government, SLIC_SYM_IVAR);
SLICSTRUCT(Advance, SLIC_SYM_IVAR);
SLICSTRUCT(Action, SLIC_SYM_STRING);
SLICSTRUCT(Improvement, SLIC_SYM_IMPROVEMENT);
SLICSTRUCT(Value, SLIC_SYM_IVAR);
SLICSTRUCT(Building, SLIC_SYM_IVAR);
SLICSTRUCT(Wonder, SLIC_SYM_IVAR);
SLICSTRUCT(UnitRecord, SLIC_SYM_IVAR);
SLICSTRUCT(Gold, SLIC_SYM_IVAR);
SLICSTRUCT(Good, SLIC_SYM_IVAR);

#if 0



class SlicStruct_Unit : public SlicStructDescription
{
public:
	SlicStruct_Unit();

	SlicSymbolData *CreateDataSymbol()
	{
		return new SlicSymbolData(SLIC_SYM_UNIT);
	}
};




class SlicStruct_City : public SlicStructDescription
{
public:
	SlicStruct_City();
	SlicSymbolData *CreateDataSymbol()
	{
		return new SlicSymbolData(SLIC_SYM_CITY);
	}
};




class SlicStruct_Player : public SlicStructDescription
{
public:
	SlicStruct_Player();
	SlicSymbolData *CreateDataSymbol()
	{
		return new SlicSymbolData(SLIC_SYM_IVAR);
	}
};




class SlicStruct_Army : public SlicStructDescription
{
public:
	SlicStruct_Army();
	SlicSymbolData *CreateDataSymbol()
	{
		return new SlicSymbolData(SLIC_SYM_ARMY);
	}
};




class SlicStruct_Location : public SlicStructDescription
{
public:
	SlicStruct_Location();
	SlicSymbolData *CreateDataSymbol()
	{
		return new SlicSymbolData(SLIC_SYM_LOCATION);
	}
};




class SlicStruct_Government : public SlicStructDescription
{
public:
	SlicStruct_Government();
	SlicSymbolData *CreateDataSymbol()
	{
		return new SlicSymbolData(SLIC_SYM_IVAR);
	}
};

#endif

#endif


