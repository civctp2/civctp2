








#include "GWCiv.h"


#include "GWCivRecord.h"


DllExport GWCiv gwCiv;


DllExport GWCiv::GWCiv()
{
	
	
	SetName("gwciv");
}


DllExport GWCiv::~GWCiv()
{
}




DllExport int GWCiv::CreateRecord()
{
	
	GWCivRecord *newRecord = new GWCivRecord;

	
	return(newRecord->GetID());
}





DllExport int GWCiv::CreateRecord(void *data, long numOfBytes)
{
	
	GWCivRecord *newRecord = new GWCivRecord(data, numOfBytes);

	
	return(newRecord->GetID());
}





DllExport void GWCiv::UnitBuilt(int recordID, char *unitName, int unitCost, char *builderAIP)
{
	
	GWCivRecord *record = (GWCivRecord*)GWRecord::Find(recordID);

	
	if(record) record->UnitBuilt(unitName, unitCost, builderAIP);
}






DllExport void GWCiv::UnitKill(int recordID, char *unitName, int destroyedUnitCost, char *killerAIP, char *killedAIP)
{
	
	GWCivRecord *record = (GWCivRecord*)GWRecord::Find(recordID);

	
	if(record) record->UnitKill(unitName, destroyedUnitCost, killerAIP, killedAIP);
}
