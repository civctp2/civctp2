#ifndef __AUI_MEMMAP_H__
#define __AUI_MEMMAP_H__

class aui_FileFormat;
class aui_MemMap;

#include "ui/aui_common/aui_base.h"
#include "ui/aui_common/tech_memmap.h"

class aui_FileFormat : public aui_Base
{
public:

    aui_FileFormat() : aui_Base() {};
};


class aui_MemMap : public tech_MemMap, public aui_Base
{
public:
    aui_MemMap() : tech_MemMap(), aui_Base() {};

	virtual aui_FileFormat * GetFileFormat(MBCHAR const * filename);
	virtual void ReleaseFileFormat(aui_FileFormat * format);
};

#endif
