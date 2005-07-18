#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TIFFUTILS_H__
#define __TIFFUTILS_H__

#ifdef __cplusplus
extern "C" {
#endif
	
	char *tiffutils_LoadTIF(const char *filename, uint16 *width, uint16 *height, size_t *size = NULL);
	char *TIF2mem(const char *filename, uint16 *width, uint16 *height, size_t *size = NULL);
	
	int TIFGetMetrics(const char *filename, uint16 *width, uint16 *height);
	int TIFLoadIntoBuffer16(const char *filename, uint16 *width, uint16 *height, uint16 imageRowBytes, uint16 *buffer, BOOL is565);

	char *StripTIF2Mem(const char *filename, uint16 *width, uint16 *height, size_t *size = NULL);
#ifdef __cplusplus
}
#endif

#endif 
