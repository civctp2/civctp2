
#pragma once

#ifndef __FLI_FUNC_TAB_H__
#define __FLI_FUNC_TAB_H__

#define COM_INTERFACE 

#include "StringHash.h"
#include "FliSetFunc.h"

class FliFuncTab : public StringHash<FliSetFunc>
{
private:
	
	sint32 m_arraySize;
	sint32 m_numEntries;
	

	
	FliSetFunc **m_array;
	

public:
	FliFuncTab(sint32 initsize);
	~FliFuncTab();

	FliSetFunc *Add(char *name, FliSymbol *variable,
					SFTYPE type,
					double v1, double v2);
	FliSetFunc *Get(sint32 index) const;
};
#endif

