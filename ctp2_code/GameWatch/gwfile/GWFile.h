










#ifndef _BMH_GW_FILE_H_
#define _BMH_GW_FILE_H_


#include "GWFileDLL.h"


#include "GWDelivery.h"


class GWFileDllHeader GWFile : public GWDelivery {
public:
	
	GWFile();

	
	virtual ~GWFile();

	
	
	
	virtual bool Deliver(char *stamp, void *data, long numOfBytes);

	
	
	
	virtual bool Receive(char **stamp, void **data, long *numOfBytes);

	
	
	
	virtual void FreeStamp(char *stamp);
	virtual void FreeData(void *data);

protected:
	
	
	
	char *CreateFullName(char *path, char *fileName);

	
	void CreateDirectory(char *path);

	
	char *TruncatePath(char *path);
};

GWFileDllHeader extern GWFile gwFile;  

#endif 
