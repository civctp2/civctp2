










 

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SPRITEUTILS_H__
#define __SPRITEUTILS_H__


#define k_CHROMAKEY_PIXEL		0x0000
#define k_SHADOW_PIXEL_565		0xF81F
#define k_SHADOW_PIXEL_555		0x7C1F
#define k_SHADOWBACKGD_PIXEL	0xFFFF

#define k_CHROMAKEY_RUN_ID		0x0A
#define k_COPY_RUN_ID			0x0C
#define k_SHADOW_RUN_ID			0x0E
#define k_FEATHERED_RUN_ID		0x0F


#define k_EOLN_ID				0xF0


#define k_NO_ALPHA			0x00
#define k_ALL_ALPHA			0xFF


#define k_EMPTY_TABLE_ENTRY	0xFFFF

void spriteutils_MergeShadowMap(Pixel32 *buf, Pixel32 *shadowBuf, uint16 width, uint16 height);


void spriteutils_EncodeShadowRun(Pixel32 **inBuf, sint32 *pos, sint32 width, Pixel16 **outBufPtr);
void spriteutils_EncodeCopyRun(Pixel32 **inBuf, sint32 *pos, sint32 width, Pixel16 **outBufPtr);
char spriteutils_EncodeChromakeyRun(Pixel32 **inBuf, sint32 *pos, sint32 width, Pixel16 **outBufPtr);
void spriteutils_EncodeFeatheredRun(Pixel32 **inBuf, sint32 *pos, sint32 width, Pixel16 **outBufPtr);
char spriteutils_EncodeScanline(Pixel32 *scanline, sint32 width, Pixel16 **outBufPtr);
Pixel16 *spriteutils_RGB32ToEncoded(Pixel32 *buf, Pixel32 *shadowBuf, uint16 width, uint16 height, size_t *size = NULL);
Pixel16 *spriteutils_RGB32ToEncoded(Pixel32 *buf, uint16 width, uint16 height, size_t *size = NULL);

Pixel32 spriteutils_AveragePixel32(Pixel32 pixel1, Pixel32 pixel2, Pixel32 pixel3, Pixel32 pixel4);
void spriteutils_CreateQuarterSize(Pixel32 *srcBuf, sint32 srcWidth, sint32 srcHeight, Pixel32 **destBuf, BOOL aa);

void spriteutils_ConvertPixelFormat(Pixel16 *frame, sint32 width, sint32 height);




void spriteutils_ConvertPixelFormatForFile(Pixel16 *frame, sint32 width, sint32 height);



void spriteutils_DecodeToBuffer(Pixel16 *data, sint32 width, sint32 height);

sint32 spriteutils_DeltaToFacing(sint32 x, sint32 y);

#endif
