#include "GWRecord.h"

DllExport int GWRecord::uniqueID = 0;

DllExport GWRecordList GWRecord::records;

DllExport GWRecord* GWRecord::Find(int recordID)
{

	GWRecordList::iterator record;

	for(record = records.begin(); record != records.end(); record++) {

		if(recordID == (*record)->ID)
			return(*record);
	}

	return(NULL);
}


DllExport GWRecord::GWRecord()
{

	ID = uniqueID++;

	records.push_back(this);
}

DllExport GWRecord::~GWRecord()
{

	records.remove(this);
}
