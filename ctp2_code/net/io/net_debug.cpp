#include "c3.h"
#include "net_debug.h"
#include "net_types.h"

#ifdef _DEBUG
char* netdebug_NetErrToString(NET_ERR err)
{
	switch(err) {
	case NET_ERR_OK:	return "NET_ERR_OK";
	case NET_ERR_INVALIDADDR: return "NET_ERR_INVALIDADDR";
	case NET_ERR_INVALIDPORT: return "NET_ERR_INVALIDPORT";
	case NET_ERR_CONNCLOSED: return "NET_ERR_CONNCLOSED";
	case NET_ERR_NODATA: return "NET_ERR_NODATA";
	case NET_ERR_WRITEERR: return "NET_ERR_WEITEERR";
	case NET_ERR_NOTIMPLEMENTED: return "NET_ERR_NOTIMPLEMENTED";
	case NET_ERR_TRANSPORTERROR: return "NET_ERR_TRANSPORTERROR";
	case NET_ERR_ALREADYOPEN: return "NET_ERR_ALREADYOPEN";
	case NET_ERR_NOMORESESSIONS: return "NET_ERR_NOMORESESSIONS";
	case NET_ERR_UNKNOWN: return "NET_ERR_UNKNOWN";
	default:
	{ 
		static char str[80];
		sprintf(str, "NET_ERR_IHADSOMEBADCLAMS(%d)\n", err);
		return str;
	}
	}
}
#endif
