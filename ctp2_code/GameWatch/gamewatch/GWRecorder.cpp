








#include "GWRecorder.h"


#include "GWRecord.h"


DllExport GWRecorderList GWRecorder::recordingSystems;


DllExport GWRecorder* GWRecorder::Find(char *recordingName)
{
	
	GWRecorderList::iterator recordingSystem;

	
	for(recordingSystem = recordingSystems.begin(); recordingSystem != recordingSystems.end(); recordingSystem++) {
		
		if(!strcmp(recordingName, (*recordingSystem)->name))
			return(*recordingSystem);
	}

	
	return(NULL);
}


DllExport GWRecorder::GWRecorder()
{
	
	
	
	name = "unknown";

	
	recordingSystems.push_back(this);
}


DllExport GWRecorder::~GWRecorder()
{
	
	recordingSystems.remove(this);
}


DllExport void GWRecorder::DeleteRecord(int recordID)
{
	
	GWRecord *record = GWRecord::Find(recordID);

	
	if(record) delete record;
}




DllExport int GWRecorder::MergeRecords(int firstRecordID, int secondRecordID)
{
	
	GWRecord *firstRecord = GWRecord::Find(firstRecordID);
	GWRecord *secondRecord = GWRecord::Find(secondRecordID);

	
	GWRecord *mergedRecord = NULL;
	if(firstRecord && secondRecord) mergedRecord = firstRecord->Merge(secondRecord);

	
	if(mergedRecord) return(mergedRecord->GetID());

	
	return(-1);
}




DllExport char *GWRecorder::ExportRecord(int recordID, char *baseName, char *stamp)
{
	
	GWRecord *record = GWRecord::Find(recordID);

	
	if(record) return(record->Export(baseName, stamp));

	
	return(NULL);
}


DllExport void GWRecorder::GetRecord(int recordID, void **data, long *numOfBytes)
{
	
	*data = NULL;
	*numOfBytes = 0;

	
	GWRecord *record = GWRecord::Find(recordID);

	
	if(record) record->GetData(data, numOfBytes);
}
