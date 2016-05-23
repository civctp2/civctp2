#ifndef _BMH_GW_CIV_H_
#define _BMH_GW_CIV_H_

#include "GWCivDLL.h"

#include "GWRecorder.h"

class GWCivDllHeader GWCiv : public GWRecorder {
public:

	GWCiv();

	virtual ~GWCiv();




	virtual int CreateRecord();





	virtual int CreateRecord(void *data, long numOfBytes);





	virtual void UnitBuilt(int recordID, char *unitName, int unitCost, char *builderAIP);






	virtual void UnitKill(int recordID, char *unitName, int destroyedUnitCost, char *killerAIP, char *killedAIP);
};

GWCivDllHeader extern GWCiv gwCiv;

#endif
