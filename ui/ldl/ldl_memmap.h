#ifndef __LDL_MEMMAP_H__
#define __LDL_MEMMAP_H__

#include "tech_memmap.h"

class tech_MemMap;


class ldl_MemMap : public tech_MemMap
{
public:
	ldl_MemMap() {;}
	virtual ~ldl_MemMap() {}

	virtual unsigned char *GetFileBits(
		char *filename,
		unsigned long *filesize = NULL );
	virtual void ReleaseFileBits( unsigned char *&bits );
};


#endif 
