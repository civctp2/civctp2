
#include "c3.h"

#include "DMapPoint.h"
#include "CivArchive.h"

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


