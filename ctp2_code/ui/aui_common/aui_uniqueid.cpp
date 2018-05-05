#include "ctp/c3.h"

#include "ui/aui_common/aui_uniqueid.h"


uint32 aui_UniqueId( void )
{
	static uint32 id = 0x00000000;

	id--;


	Assert( (sint32)id < 0 );

	return id;
}
