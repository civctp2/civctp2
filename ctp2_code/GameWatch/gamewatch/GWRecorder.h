







#ifndef _BMH_GW_RECORDER_H_
#define _BMH_GW_RECORDER_H_


#include "DLLTypes.h"


class GWRecorder;


#include <List>


using std::list;


typedef list<GWRecorder*> GWRecorderList;


class GWDllHeader GWRecorder {
public:
	
	static GWRecorder* Find(char *recordingName);

	
	GWRecorder();

	
	virtual ~GWRecorder();

	
	
	
	virtual int CreateRecord() = 0;

	
	
	
	
	virtual int CreateRecord(void *data, long numOfBytes) = 0;

	
	virtual void DeleteRecord(int recordID);

	
	
	
	virtual int MergeRecords(int firstRecordID, int secondRecordID);

	
	
	
	
	
	virtual char *ExportRecord(int recordID, char *baseName, char *stamp);

	
	
	virtual void GetRecord(int recordID, void **data, long *numOfBytes);

protected:
	
	
	void SetName(char *registerName) { name = registerName; }

private:
	
	static GWRecorderList recordingSystems;

	
	char *name;
};

#endif 