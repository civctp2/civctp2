#ifndef _BMH_GW_DELIVERY_H_
#define _BMH_GW_DELIVERY_H_

#include "DLLTypes.h"

class GWDelivery;

#include <List>

using std::list;

typedef list<GWDelivery*> GWDeliveryList;

class GWDllHeader GWDelivery {
public:

	static GWDelivery* Find(char *deliveryName);

	GWDelivery();

	virtual ~GWDelivery();


	virtual bool Deliver(char *stamp, void *data, long numOfBytes) = 0;




	virtual bool Receive(char **stamp, void **data, long *numOfBytes) = 0;




	virtual void FreeStamp(char *stamp) = 0;
	virtual void FreeData(void *data) = 0;

	void SetParameters(char *systemParameters);

protected:

	void SetName(char *registerName) { name = registerName; }


	char *GetParameters() { return(parameters); }

private:

	static GWDeliveryList deliverySystems;

	char *name;

	char *parameters;
};

#endif
