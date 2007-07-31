










#ifndef __AUI_MEMMAP_H__
#define __AUI_MEMMAP_H__


#include "aui_base.h"
#include "tech_memmap.h"



class aui_FileFormat : public aui_Base
{
public:
	
	aui_FileFormat() {}
	virtual ~aui_FileFormat() {}
};



class aui_MemMap : public tech_MemMap, public aui_Base
{
public:
	aui_MemMap() {}
	virtual ~aui_MemMap() {}

	virtual aui_FileFormat *GetFileFormat( MBCHAR *filename );
	virtual void ReleaseFileFormat( aui_FileFormat *format );
};


#endif 
