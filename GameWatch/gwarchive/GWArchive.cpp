












#include "GWArchive.h"


#include <errno.h>
#include <direct.h>


#include "GWDelivery.h"
#include "GWRecorder.h"


DllExport GWArchive gwArchive;



DllExport void GWArchive::Archive(char *basePath)
{
	
	CreateDirectory(basePath);

	
	GWDelivery *deliveryArchive = GetDeliverySystem();
	GWRecorder *recordingArchive = GetRecordingSystem();

	
	int index = 0;

	
	char *stamp = NULL;
	void *data = NULL;
	long numOfBytes;

	
	int mergeID = recordingArchive->CreateRecord();

	
	while(deliveryArchive->Receive(&stamp, &data, &numOfBytes)) {
		
		int recordID = recordingArchive->CreateRecord(data, numOfBytes);

		
		char exportName[1024];
		sprintf(exportName, "%s\\export%d", basePath, index);

		
		index++;

		
		
		
		recordingArchive->ExportRecord(recordID, exportName, stamp);

		
		int newMergeID = recordingArchive->MergeRecords(mergeID, recordID);

		
		recordingArchive->DeleteRecord(mergeID);

		
		mergeID = newMergeID;

		
		recordingArchive->DeleteRecord(recordID);

		
		deliveryArchive->FreeStamp(stamp);
		deliveryArchive->FreeData(data);
	}

	
	char mergeName[1024];
	sprintf(mergeName, "%s\\summary", basePath);

	
	
	recordingArchive->ExportRecord(mergeID, mergeName, NULL);

	
	recordingArchive->DeleteRecord(mergeID);
}


DllExport void GWArchive::CreateDirectory(char *path)
{
	
	
	if((_mkdir(path) == -1) && (errno == ENOENT)) {
		
		char *smallerPath = TruncatePath(path);

		
		if(smallerPath) {
			
			
			CreateDirectory(smallerPath);

			
			_mkdir(path);

			
			free(smallerPath);
		}
	}
}


DllExport char *GWArchive::TruncatePath(char *path)
{
	
	char *newPath = strdup(path);

	
	int length = strlen(newPath);

	
	for(int index = length - 1; index >= 0; index--) {
		
		if((newPath[index] == '/') || (newPath[index] == '\\')) {
			
			newPath[index] = '\0';

			
			return(newPath);
		}
	}

	
	free(newPath);

	
	return(NULL);
}
