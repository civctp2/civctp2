











#include "GWFile.h"


#include <errno.h>
#include <direct.h>
#include <share.h>


DllExport GWFile gwFile;


DllExport GWFile::GWFile()
{
	
	
	SetName("gwfile");
}


DllExport GWFile::~GWFile()
{
}




DllExport bool GWFile::Deliver(char *stamp, void *data, long numOfBytes)
{
	
	
	CreateDirectory(GetParameters());

	
	
	
	char *trackName = CreateFullName(GetParameters(), "track.dat");

	
	FILE *trackFile = _fsopen(trackName, "r+", _SH_DENYRW);

	
	int fileNumber = 1;

	
	if(trackFile) {
		
		fscanf(trackFile, "%d", &fileNumber);

		
		
		rewind(trackFile);
	} else { 
		trackFile = _fsopen(trackName, "w", _SH_DENYRW);

		
		if(!trackFile) {
			delete trackName; 
			return(false);
		}
	}

	
	char shortName[32];
	sprintf(shortName, "record%d.dat", fileNumber);

	
	char *fileName = CreateFullName(GetParameters(), shortName);

	
	
	
	FILE *dataFile = _fsopen(fileName, "wb", _SH_DENYRW);

	
	
	if(!dataFile) {
		
		fprintf(trackFile, "%d\n", fileNumber);

		
		fclose(trackFile);

		
		delete trackName;
		delete fileName;
		return(false);
	}

	
	
	size_t stampSize = strlen(stamp) + 1;  

	
	fwrite(&stampSize, sizeof(stampSize), 1, dataFile);

	
	fwrite(stamp, stampSize, 1, dataFile);

	
	fwrite(&numOfBytes, sizeof(numOfBytes), 1, dataFile);

	
	if(numOfBytes) fwrite(data, numOfBytes, 1, dataFile);

	
	fclose(dataFile);

	
	
	fileNumber++;

	
	fprintf(trackFile, "%d\n", fileNumber);

	
	fclose(trackFile);

	
	delete trackName;
	delete fileName;

	
	return(true);
}




DllExport bool GWFile::Receive(char **stamp, void **data, long *numOfBytes)
{
	
	
	
	
	char *trackName = CreateFullName(GetParameters(), "track.dat");

	
	FILE *trackFile = _fsopen(trackName, "r+", _SH_DENYRW);

	
	int fileNumber = 1;

	
	if(trackFile) {
		
		fscanf(trackFile, "%d", &fileNumber);

		
		
		rewind(trackFile);
	} else { 
		delete trackName;
		return(false);
	}

	
	fileNumber--;

	
	char shortName[32];
	sprintf(shortName, "record%d.dat", fileNumber);

	
	char *fileName = CreateFullName(GetParameters(), shortName);

	
	
	
	FILE *dataFile = _fsopen(fileName, "rb", _SH_DENYRW);

	
	
	if(!dataFile) {
		
		fprintf(trackFile, "%d\n", fileNumber+1);

		
		fclose(trackFile);

		
		delete trackName;
		delete fileName;
		return(false);
	}


	
	
	size_t stampSize = 0;

	
	fread(&stampSize, sizeof(stampSize), 1, dataFile);

	
	*stamp = new char[stampSize];

	
	fread(*stamp, stampSize, 1, dataFile);

	
	fread(numOfBytes, sizeof(*numOfBytes), 1, dataFile);

	
	if(*numOfBytes) {
		
		*data = malloc(*numOfBytes);

		
		fread(*data, *numOfBytes, 1, dataFile);
	} else { 
		*data = NULL;
	}

	
	fclose(dataFile);

	
	
	remove(fileName);

	
	
	fprintf(trackFile, "%d\n", fileNumber);

	
	fclose(trackFile);

	
	delete trackName;
	delete fileName;

	
	return(true);
}




DllExport void GWFile::FreeStamp(char *stamp) { delete stamp; }
DllExport void GWFile::FreeData(void *data) { free(data); }




DllExport char *GWFile::CreateFullName(char *path, char *fileName)
{
	
	int length = strlen(path) + strlen(fileName) + 3; 

	
	char *fullName = new char[length];

	
	sprintf(fullName, "%s\\%s", path, fileName);

	
	return(fullName);
}


DllExport void GWFile::CreateDirectory(char *path)
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


DllExport char *GWFile::TruncatePath(char *path)
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
