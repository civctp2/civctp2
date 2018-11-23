#include "GWDelivery.h"

DllExport GWDeliveryList GWDelivery::deliverySystems;

DllExport GWDelivery* GWDelivery::Find(char *deliveryName)
{

	GWDeliveryList::iterator deliverySystem;

	for(deliverySystem = deliverySystems.begin(); deliverySystem != deliverySystems.end(); deliverySystem++) {

		if(!strcmp(deliveryName, (*deliverySystem)->name))
			return(*deliverySystem);
	}

	return(NULL);
}

DllExport GWDelivery::GWDelivery()
{


	name = "unknown";

	parameters = NULL;

	deliverySystems.push_back(this);
}

DllExport GWDelivery::~GWDelivery()
{

	if(parameters) free(parameters);

	deliverySystems.remove(this);
}

DllExport void GWDelivery::SetParameters(char *systemParameters)
{

	if(parameters) {
		free(parameters);
		parameters = NULL;
	}

	if(systemParameters) parameters = strdup(systemParameters);
}
