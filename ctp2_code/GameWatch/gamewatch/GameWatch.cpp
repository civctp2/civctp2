












#include "GameWatch.h"


#include <winbase.h>


#include "GWDelivery.h"
#include "GWRecorder.h"


DllExport GameWatch gameWatch;


DllExport GameWatch::GameWatch()
{
	
	
	deliverySystem = NULL;
	deliveryLibrary = NULL;
	recordingSystem = NULL;
	recordingLibrary = NULL;
}


DllExport GameWatch::~GameWatch()
{
	
	if(deliveryLibrary) FreeLibrary(deliveryLibrary);

	
	if(recordingLibrary) FreeLibrary(recordingLibrary);
}






DllExport bool GameWatch::DeliverySystem(char *name, char *parameters)
{
	
	
	deliverySystem = GWDelivery::Find(name);

	
	if(deliverySystem) {
		
		deliverySystem->SetParameters(parameters);
		
		
		return(true);
	}

	
	
	
	if(deliveryLibrary) FreeLibrary(deliveryLibrary);

	
	
	
	deliveryLibrary = LoadLibrary(name);

	
	
	deliverySystem = GWDelivery::Find(name);

	
	if(deliverySystem) {
		
		deliverySystem->SetParameters(parameters);
		
		
		return(true);
	}

	
	return(false);
}




DllExport bool GameWatch::RecordingSystem(char *name)
{
	
	
	recordingSystem = GWRecorder::Find(name);

	
	if(recordingSystem) return(true);

	
	
	
	if(recordingLibrary) FreeLibrary(recordingLibrary);

	
	
	
	recordingLibrary = LoadLibrary(name);

	
	
	recordingSystem = GWRecorder::Find(name);

	
	if(recordingSystem) return(true);

	
	return(false);
}




DllExport int GameWatch::StartGame()
{
	
	if(recordingSystem) return(recordingSystem->CreateRecord());

	
	return(-1);
}




DllExport void GameWatch::EndGame(int gameID, char *stamp)
{
	
	if(gameID < 0) return;

	
	if(!recordingSystem) return;

	
	void *data = NULL;
	long numOfBytes = 0;
	recordingSystem->GetRecord(gameID, &data, &numOfBytes);

	
	
	if(deliverySystem) deliverySystem->Deliver(stamp, data, numOfBytes);

	
	recordingSystem->DeleteRecord(gameID);
}




DllExport void GameWatch::SaveGame(int gameID, char *filename)
{
	
	if(gameID < 0) return;

	
	void *data = NULL;
	long numOfBytes = 0;
	recordingSystem->GetRecord(gameID, &data, &numOfBytes);

	
	FILE *saveFile = fopen(filename, "wb");

	
	if(!saveFile) return;

	
	fwrite(&numOfBytes, sizeof(numOfBytes), 1, saveFile);

	
	if(numOfBytes) fwrite(data, numOfBytes, 1, saveFile);

	
	fclose(saveFile);
}



DllExport int GameWatch::LoadGame(char *filename)
{
	
	void *data = NULL;
	long numOfBytes = 0;

	
	FILE *loadFile = fopen(filename, "rb");

	
	if(!loadFile) return(-1);

	
	fread(&numOfBytes, sizeof(numOfBytes), 1, loadFile);

	
	if(numOfBytes) {
		
		data = malloc(numOfBytes);

		
		fread(data, numOfBytes, 1, loadFile);
	}

	
	fclose(loadFile);

	
	int gameID = recordingSystem->CreateRecord(data, numOfBytes);

	
	if(data) free(data);

	
	return(gameID);
}
