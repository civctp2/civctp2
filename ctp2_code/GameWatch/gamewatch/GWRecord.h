







#ifndef _BMH_GW_RECORD_H_
#define _BMH_GW_RECORD_H_


#include "DLLTypes.h"


class GWRecord;


#include <list>


using std::list;


typedef list<GWRecord*> GWRecordList;


class GWDllHeader GWRecord {
public:
	
	static GWRecord* Find(int recordID);

	
	
	GWRecord();

	
	virtual ~GWRecord();

	
	
	virtual GWRecord* Merge(GWRecord *record) = 0;

	
	
	
	virtual char *Export(char *baseName, char *stamp) = 0;

	
	
	
	virtual void GetData(void **data, long *numOfBytes) = 0;

	
	
	int GetID() { return(ID); }

private:
	
	
	static int uniqueID;

	
	static GWRecordList records;

	
	int ID;
};

#endif 
