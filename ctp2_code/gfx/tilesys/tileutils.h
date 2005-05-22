










 

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TILEUTILS_H__
#define __TILEUTILS_H__




Pixel32		*tileutils_LowPassFilter(Pixel32 *image, uint32 width, uint32 height);
char		*tileutils_TIF2mem(char *filename, uint16 *width, uint16 *height);
char		*tileutils_StripTIF2Mem(char *filename, uint16 *width, uint16 *height);

Pixel16		*tileutils_TGA2mem(char *filename, uint16 *width, uint16 *height);


void		tileutils_EncodeCopyRun(Pixel32 **inBuf, int *pos, int width, Pixel16 **outBufPtr);
void		tileutils_EncodeColorizeRun(Pixel32 **inBuf, int *pos, int width, Pixel16 **outBufPtr);
void		tileutils_EncodeShadowRun(Pixel32 **inBuf, int *pos, int width, Pixel16 **outBufPtr);
char		tileutils_EncodeSkipRun(Pixel32 **inBuf, int *pos, int width, Pixel16 **outBufPtr);
char		tileutils_EncodeScanline(Pixel32 *scanline, int width, Pixel16 **outBufPtr);
Pixel16		*tileutils_EncodeTile(Pixel32 *buf, uint16 width, uint16 height, uint32 *dataLen);



void		tileutils_EncodeCopyRun16(Pixel16 **inBuf, int *pos, int width, Pixel16 **outBufPtr, BOOL sourceDataIs565);
void		tileutils_EncodeColorizeRun16(Pixel16 **inBuf, int *pos, int width, Pixel16 **outBufPtr, BOOL sourceDataIs565);
void		tileutils_EncodeShadowRun16(Pixel16 **inBuf, int *pos, int width, Pixel16 **outBufPtr, BOOL sourceDataIs565);
char		tileutils_EncodeSkipRun16(Pixel16 **inBuf, int *pos, int width, Pixel16 **outBufPtr, BOOL sourceDataIs565);
char		tileutils_EncodeScanline16(Pixel16 *scanline, int width, Pixel16 **outBufPtr, BOOL sourceDataIs565);
Pixel16		*tileutils_EncodeTile16(Pixel16 *buf, uint16 width, uint16 height, uint32 *size, sint32 pitch = 0);

sint32		tileutils_ConvertPixelFormatFrom565(Pixel16 *data);
sint32		tileutils_ConvertPixelFormatFrom555(Pixel16 *data);
void		tileutils_DecodeToBuffer(Pixel16 *data, int width, int height);

sint32		tileutils_EncodeStencil(MBCHAR *filename);
sint32		tileutils_ExtractStencils(sint16 fromType, sint16 toType);
Pixel16		*tileutils_ExtractUpperRight(char *tif, uint16 width, uint16 height, sint32 x, sint32 y);
Pixel16		*tileutils_ExtractLowerLeft(char *tif, uint16 width, uint16 height, sint32 x, sint32 y);
void		tileutils_DumpStencil(MBCHAR *filename);
void		tileutils_LoadStencil(void);
Pixel16		*tileutils_LoadStencilImage(uint16 from, uint16 to);
Pixel16		*tileutils_MakeTransition1(Pixel16 *sourceStencil);
Pixel16		*tileutils_MakeTransition2(Pixel16 *sourceStencil);
Pixel16		*tileutils_MakeTransition3(Pixel16 *sourceStencil);
void		tileutils_DumpAllTransitions(MBCHAR *filename, Pixel16 *t0, Pixel16 *t1, Pixel16 *t2, Pixel16 *t3);

uint16		*tileutils_GenerateAllWaterTable(uint16 width, uint16 height, uint16 x, uint16 y);
uint16		*tileutils_ExtractWaterTable(Pixel32 *image, uint16 width, uint16 height, uint16 x, uint16 y);

void		tileutils_BorkifyTile(Pixel16 *tileImage, BOOL useT0, BOOL useT1, BOOL useT2, BOOL useT3);

uint16		tileutils_CompileImprovements(FILE *file);
void		tileutils_EncodeTileset(MBCHAR *filename);
sint32		tileutils_ParseTileset(MBCHAR *filename);

#endif
