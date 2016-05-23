#ifndef _BMH_GW_CIV_RECORD_H_
#define _BMH_GW_CIV_RECORD_H_

#include "GWCivDLL.h"

#include "GWRecord.h"

class GWUnitRecord;

class GWCivDllHeader GWCivRecord : public GWRecord {
public:

	GWCivRecord();


	GWCivRecord(void *data, long numOfBytes);

	virtual ~GWCivRecord();


	virtual GWRecord* Merge(GWRecord *record);




	virtual char *Export(char *baseName, char *stamp);


	virtual void GetData(void **data, long *numOfBytes);


	virtual void UnitBuilt(char *unitName, int unitCost, char *builderAIP);




	virtual void UnitKill(char *unitName, int destroyedUnitCost, char *killerAIP, char *killedAIP);

private:

	GWUnitRecord *FindUnitRecord(char *name);
	GWUnitRecord *FindUnitRecordForBuilder(char *name, char *builderAIP);
	GWUnitRecord *FindUnitRecordForKiller(char *name, char *killerAIP);

	void AddUnitRecord(GWUnitRecord *record);

	void SortUnitRecord(GWUnitRecord *record);
	void SortUnitRecordByBuilder(GWUnitRecord *record);
	void SortUnitRecordByKiller(GWUnitRecord *record);

	GWUnitRecord *head;


	GWUnitRecord *dataBuffer;
};

#endif
