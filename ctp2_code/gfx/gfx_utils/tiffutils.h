#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TIFFUTILS_H__
#define __TIFFUTILS_H__

#ifdef __cplusplus
extern "C" {
#endif
	
	char *tiffutils_LoadTIF(char *filename, uint16 *width, uint16 *height);
	char *TIF2mem(char *filename, uint16 *width, uint16 *height);
	
	int TIFGetMetrics(char *filename, uint16 *width, uint16 *height);
	int TIFLoadIntoBuffer16(char *filename, uint16 *width, uint16 *height, uint16 imageRowBytes, uint16 *buffer, BOOL is565);

	char *StripTIF2Mem(char *filename, uint16 *width, uint16 *height);
#ifdef __cplusplus
}
#endif

#endif 
