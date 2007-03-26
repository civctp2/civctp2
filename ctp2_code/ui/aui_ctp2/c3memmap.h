











#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef C3MEMMAP_H__
#define C3MEMMAP_H__

class C3MemMap;

#include "aui_memmap.h"

class aui_FileFormat;

class C3MemMap : public aui_MemMap
{
public:
    C3MemMap() : aui_MemMap() {};

	virtual aui_FileFormat * GetFileFormat(MBCHAR const * filename);
};


#endif 
