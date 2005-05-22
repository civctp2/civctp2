











#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3MEMMAP_H__
#define __C3MEMMAP_H__


#include "aui_memmap.h"



class C3MemMap : public aui_MemMap
{
public:
	C3MemMap() {}
	virtual ~C3MemMap() {}

	virtual aui_FileFormat *GetFileFormat( MBCHAR *filename );
};


#endif 
