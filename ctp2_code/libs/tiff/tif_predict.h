#pragma once


























#ifndef _TIFFPREDICT_
#define	_TIFFPREDICT_









typedef struct {
	int	predictor;		
	int	stride;			
	tsize_t	rowsize;		

	TIFFPostMethod	pfunc;		
	TIFFCodeMethod	coderow;	
	TIFFCodeMethod	codestrip;	
	TIFFCodeMethod	codetile;	
	TIFFVGetMethod	vgetparent;	
	TIFFVSetMethod	vsetparent;	
	TIFFPrintMethod	printdir;	
	TIFFBoolMethod	setupdecode;	
	TIFFBoolMethod	setupencode;	
} TIFFPredictorState;

#if defined(__cplusplus)
extern "C" {
#endif
extern	int TIFFPredictorInit(TIFF*);
#if defined(__cplusplus)
}
#endif
#endif 
