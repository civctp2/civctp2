











#ifndef _BMH_GAME_WATCH_H_
#define _BMH_GAME_WATCH_H_


#include "DLLTypes.h"




#include <wtypes.h>



class GWDelivery;					
class GWRecorder;					




class GWDllHeader GameWatch {
public:

	
	GameWatch();

	
	~GameWatch();

	
	
	
	
	
	bool DeliverySystem(char *name, char *parameters);

	
	
	
	bool RecordingSystem(char *name);

	
	
	
	int StartGame();

	
	
	
	void EndGame(int gameID, char *stamp);

	
	
	
	void SaveGame(int gameID, char *filename);

	
	
	int LoadGame(char *filename);

protected:
	
	
	
	
	GWDelivery *GetDeliverySystem() { return(deliverySystem); }
	GWRecorder *GetRecordingSystem() { return(recordingSystem); }

private:

	
	GWDelivery *deliverySystem;

	
	HINSTANCE deliveryLibrary;

	
	GWRecorder *recordingSystem;

	
	HINSTANCE recordingLibrary;

};

GWDllHeader extern GameWatch gameWatch; 

#endif 