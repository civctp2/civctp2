#include "c3.h"

#include "ID.h"
#include "CivArchive.h"


void ID::Serialize(CivArchive &archive)
	{
	if (archive.IsStoring())
		archive<<m_id ;
	else
		archive>>m_id ;

	}














uint32 ID_ID_GetVersion(void)
	{
	return (k_ID_VERSION_MAJOR<<16 | k_ID_VERSION_MINOR) ;
	}
