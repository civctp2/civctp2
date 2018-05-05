#include "ctp/c3.h"

#include "robotcom/backdoor/DMapPoint.h"
#include "civarchive.h"

DMapPoint::DMapPoint()
{
    return;
}

DMapPoint::~DMapPoint()
{
    return;
}

void DMapPoint::Serialize(CivArchive &archive)

{
	if (archive.IsStoring()) {
		archive << x;
		archive << y;
		archive << z;
	} else {
		archive >> x;
		archive >> y;
		archive >> z;
	}
}
