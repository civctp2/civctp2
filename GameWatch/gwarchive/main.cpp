










#include <stdlib.h>
#include <stdio.h>


#include "GWArchive.h"


void main(int argc, char **argv)
{
	
	if(argc != 5) {
		
		fprintf(stderr, "usage: %s delivery deliveryArguments recording destDir\n", argv[0]);

		
		exit(EXIT_FAILURE);
	}

	
	if(!(gwArchive.DeliverySystem(argv[1], argv[2]))) {
		
		fprintf(stderr, "error: delivery system (%s %s) failed\n", argv[1], argv[2]);

		
		exit(EXIT_FAILURE);
	}

	
	if(!(gwArchive.RecordingSystem(argv[3]))) {
		
		fprintf(stderr, "error: recording system (%s) failed\n", argv[3]);

		
		exit(EXIT_FAILURE);
	}

	
	gwArchive.Archive(argv[4]);
}
