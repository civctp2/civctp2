#include "ctp/c3.h"

#include "ui/ldl/ldl_file.hpp"
#include "ldl_attr.hpp"
#include "ui/ldl/ldl_data.hpp"
#include "ui/ldl/ldl_memmap.h"

#include "ui/ldl/ldlif.h"

ldl::ldl(const char *fname, const char *outputDir, BOOL bUseInternalMemmap )
{
	ReadData( fname );
}

ldl::~ldl( void )
{
	ldlif_deallocate_stuff();
}

BOOL ldl::ReadData(const char *fname )
{
	ldlif_parse(strrchr(fname, FILE_SEPC) + 1);

	return TRUE;
}

ldl_datablock *ldl::FindDataBlock(const char *szName, ldl_datablock *dbParent )
{
	return ldlif_find_block(szName);
}

BOOL ldl::WriteData()
{
	return TRUE;
}
