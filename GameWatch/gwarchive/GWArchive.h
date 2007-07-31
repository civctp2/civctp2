











#ifndef _BMH_GW_ARCHIVE_H_
#define _BMH_GW_ARCHIVE_H_


#include "GWArchiveDLL.h"


#include "GameWatch.h"


class GWArchiveDllHeader GWArchive : public GameWatch {
public:
	
	
	virtual void Archive(char *basePath);

private:
	
	void CreateDirectory(char *path);

	
	char *TruncatePath(char *path);
};

GWArchiveDllHeader extern GWArchive gwArchive;	

#endif 