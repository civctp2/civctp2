//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Tile utilities
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// CLEAN_INSTEAD_OF_CONVERT
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Removed unused local variables. (Sep 9th 2005 Martin Gühmann)
// - Repaired memory leaks.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "pixelutils.h"
#include "spriteutils.h"

extern sint32 g_is565Format;

void spriteutils_EncodeShadowRun(Pixel32 **inBuf, sint32 *pos, sint32 width, Pixel16 **outBufPtr)
{
	Pixel16         pix16;
	uint8           alpha;
	sint32          runLen = 0;
	Pixel16         footer=0;
	Pixel16         shadowPixel;

	if (g_is565Format) shadowPixel = k_SHADOW_PIXEL_565;
	else shadowPixel = k_SHADOW_PIXEL_555;

	RGB32Info(**inBuf, &pix16, &alpha);
	while (pix16 == shadowPixel && (*pos < width)) {

		(*inBuf)++;


		(*pos)++;


		runLen++;

		RGB32Info(**inBuf, &pix16, &alpha);
	}


	footer = (Pixel16)(k_SHADOW_RUN_ID << 8 | runLen);


	if (*pos >= width) footer |= k_EOLN_ID << 8;


	**outBufPtr = footer;
	(*outBufPtr)++;
}

void spriteutils_EncodeCopyRun(Pixel32 **inBuf, sint32 *pos, sint32 width, Pixel16 **outBufPtr)
{
	Pixel16         pix16;
	uint8           alpha;
	sint32          runLen = 0;
	Pixel16         footer=0;
	Pixel16         *headerPtr;
	Pixel16         shadowPixel;

	if (g_is565Format) {
		shadowPixel = k_SHADOW_PIXEL_565;
	} else {
		shadowPixel = k_SHADOW_PIXEL_555;
	}

	headerPtr = *outBufPtr;
	(*outBufPtr)++;

	RGB32Info(**inBuf, &pix16, &alpha);

	while (pix16 != shadowPixel && alpha == k_ALL_ALPHA && (*pos < width)) {


		if (pix16 == 0x0000) pix16 = 0x0001;


		**outBufPtr = pix16;


		(*outBufPtr)++;


		(*inBuf)++;


		(*pos)++;


		runLen++;

		RGB32Info(**inBuf, &pix16, &alpha);
	}


	footer = (Pixel16) (k_COPY_RUN_ID << 8 | runLen);


	if (*pos >= width) footer |= k_EOLN_ID << 8;

	*headerPtr = footer;
}

char spriteutils_EncodeChromakeyRun(Pixel32 **inBuf, sint32 *pos, sint32 width, Pixel16 **outBufPtr)
{
	Pixel16         pix16;
	uint8           alpha;
	sint32          runLen = 0;
	Pixel16         footer=0;

	RGB32Info(**inBuf, &pix16, &alpha);
	while (pix16 == k_CHROMAKEY_PIXEL && alpha == k_NO_ALPHA && (*pos < width)) {

		(*inBuf)++;


		(*pos)++;


		runLen++;

		RGB32Info(**inBuf, &pix16, &alpha);
	}


	if (runLen < width) {

		footer = (Pixel16)(k_CHROMAKEY_RUN_ID << 8 | runLen);


		if (*pos >= width) footer |= k_EOLN_ID << 8;


		**outBufPtr = footer;
		(*outBufPtr)++;
	} else {

		return TRUE;
	}

	return FALSE;
}

char spriteutils_EncodeChromakeyWshadowRun(Pixel32 **inBuf, sint32 *pos, sint32 width, Pixel16 **outBufPtr)
{
	Pixel16         pix16;
	uint8           alpha;
	sint32          runLen = 0;
	Pixel16         footer=0;

	RGB32Info(**inBuf, &pix16, &alpha);
	while ( ((pix16 == k_CHROMAKEY_PIXEL && alpha == k_NO_ALPHA) ||  (pix16 == k_SHADOWBACKGD_PIXEL))  && (*pos < width) ) {

		(*inBuf)++;


		(*pos)++;


		runLen++;

		RGB32Info(**inBuf, &pix16, &alpha);
	}


	if (runLen < width) {

		footer = (Pixel16)(k_CHROMAKEY_RUN_ID << 8 | runLen);


		if (*pos >= width) footer |= k_EOLN_ID << 8;


		**outBufPtr = footer;
		(*outBufPtr)++;
	} else {

		return TRUE;
	}

	return FALSE;
}

void spriteutils_EncodeFeatheredRun(Pixel32 **inBuf, sint32 *pos, sint32 width, Pixel16 **outBufPtr)
{
	Pixel16         pix16;
	Pixel16         footer=0;
	uint8           alpha;

	RGB32Info(**inBuf, &pix16, &alpha);


	if (pix16 == 0x0000) pix16 = 0x0001;

	(*inBuf)++;
	(*pos)++;

	footer = (k_FEATHERED_RUN_ID << 8) | alpha;


	if (*pos >= width)
		footer |= k_EOLN_ID << 8;


	**outBufPtr = footer;
	(*outBufPtr)++;


	**outBufPtr = pix16;
	(*outBufPtr)++;

}


char spriteutils_EncodeScanline(Pixel32 *scanline, sint32 width, Pixel16 **outBufPtr)
{
	Pixel16         pix16;
	Pixel32         pix32;
	Pixel32         *scanPtr = scanline;
	uint8           alpha;
	sint32          pos;
	Pixel16         *startPtr;
	BOOL            empty = FALSE;
	Pixel16         shadowPixel;

	if (g_is565Format) {
		shadowPixel = k_SHADOW_PIXEL_565;
	} else {
		shadowPixel = k_SHADOW_PIXEL_555;
	}


	pos = 0;


	startPtr = *outBufPtr;



	while (scanPtr < (scanline + width))
	{
		pix32 = *scanPtr;

		empty = FALSE;

		RGB32Info(pix32, &pix16, &alpha);







		if ((pix16 == k_CHROMAKEY_PIXEL)&&(alpha == k_NO_ALPHA))

		{


			empty = spriteutils_EncodeChromakeyRun(&scanPtr, &pos, width, outBufPtr);
		}
		else
		if (pix16 == shadowPixel)
		{

			spriteutils_EncodeShadowRun(&scanPtr, &pos, width, outBufPtr);
		}
		else
		if ((alpha!=k_NO_ALPHA) && (alpha!=k_ALL_ALPHA))
		{

			spriteutils_EncodeFeatheredRun(&scanPtr, &pos, width, outBufPtr);
		}
		else {

			if (alpha == k_ALL_ALPHA)
				spriteutils_EncodeCopyRun(&scanPtr, &pos, width, outBufPtr);
			else
			{

				printf("\nError in bitmap data.  Pixel with no associated alpha.\n");

				exit(-1);
			}
		}
	}

	return empty;
}


char spriteutils_EncodeScanlineWshadow(Pixel32 *scanline, sint32 width, Pixel16 **outBufPtr)
{
	Pixel16         pix16;
	Pixel32         pix32;
	Pixel32         *scanPtr = scanline;
	uint8           alpha;
	sint32          pos;
	Pixel16         *startPtr;
	BOOL            empty = FALSE;
	Pixel16         shadowPixel;

	if (g_is565Format) {
		shadowPixel = k_SHADOW_PIXEL_565;
	} else {
		shadowPixel = k_SHADOW_PIXEL_555;
	}


	pos = 0;


	startPtr = *outBufPtr;



	while (scanPtr < (scanline + width)) {
		pix32 = *scanPtr;

		empty = FALSE;

		RGB32Info(pix32, &pix16, &alpha);

		if ( (pix16 == k_CHROMAKEY_PIXEL  && alpha == k_NO_ALPHA) ||  (pix16 == k_SHADOWBACKGD_PIXEL) )
		{

			empty = spriteutils_EncodeChromakeyWshadowRun(&scanPtr, &pos, width, outBufPtr);
		}
		else
		if (pix16 == shadowPixel) {

			spriteutils_EncodeShadowRun(&scanPtr, &pos, width, outBufPtr);
		}
		else
		if (alpha != k_NO_ALPHA && alpha != k_ALL_ALPHA) {

			spriteutils_EncodeFeatheredRun(&scanPtr, &pos, width, outBufPtr);
		}
		else {

			if (alpha == k_ALL_ALPHA)
				spriteutils_EncodeCopyRun(&scanPtr, &pos, width, outBufPtr);
			else {
				printf("\nError in bitmap data.  Pixel with no associated alpha.\n");
				exit(-1);
			}
		}
	}

	return empty;
}

void spriteutils_MergeShadowMap(Pixel32 *buf, Pixel32 *shadowBuf, uint16 width, uint16 height)
{
	Pixel32     *pixPtr, pix;
	Pixel32     *shadowPixPtr, shadowPix;



	BOOL        whiteBackground = FALSE;
	if ((*shadowBuf & 0x00FFFFFF) == 0x00FFFFFF) {
		whiteBackground = TRUE;
	} else {
		if ((*shadowBuf & 0x00FFFFFF) != 0x00000000) {
			printf("\nShadow file is in invalid format.\n");
			exit(-1);
		}
	}

	for (sint32 j=0; j<height; j++) {
		for (sint32 i=0; i<width; i++) {
			pixPtr = buf + j*width + i;
			shadowPixPtr = shadowBuf + j*width + i;

			pix = *pixPtr;
			shadowPix = *shadowPixPtr;
			shadowPix = shadowPix & 0x00FFFFFF;

			if (whiteBackground) {

				if (shadowPix != 0x00FFFFFF) {


					Pixel16     r, g, b, a;

					RGB32Components(pix, &r, &g, &b, &a);


					if (a != 0xFF) {
						*pixPtr = 0x00FF00FF;
					}
				}
			} else {

				if (shadowPix) {


					Pixel16     r, g, b, a;

					RGB32Components(pix, &r, &g, &b, &a);


					if (a != 0xFF) {
						*pixPtr = shadowPix;
					}
				}
			}
		}
	}
}


Pixel16 * spriteutils_RGB32ToEncoded(Pixel32 *buf, Pixel32 *shadowBuf, uint16 width, uint16 height, size_t *size)
{
	Pixel32             *srcPixel = buf;

	Pixel16             *outBuf = new Pixel16[(1+height+width*height)*8];
	Pixel16             *returnBuf = NULL;
	uint16              *table = (uint16 *)outBuf;
	Pixel16             *startOfData;
	Pixel16             *dataPtr, *startDataPtr;
	BOOL                empty;



	if (shadowBuf != NULL)
		spriteutils_MergeShadowMap(buf, shadowBuf, width, height);


	*table++ = (uint16)height;

	startOfData = outBuf + 1 + height;
	dataPtr = startOfData;

	for(sint32 y=0; y<height; y++)
	{

		srcPixel = buf + width * y;

		startDataPtr = dataPtr;


		empty = spriteutils_EncodeScanline(srcPixel, width, &dataPtr);
		if (empty) {
			*table++ = k_EMPTY_TABLE_ENTRY;
		} else {

			*table++ = startDataPtr - startOfData;
		}
	}

	sint32 resultSize = (dataPtr - outBuf);






	returnBuf = new Pixel16[resultSize];

	memcpy(returnBuf, outBuf, resultSize * sizeof(Pixel16));
	delete[] outBuf;
	if (size)
		*size = resultSize * sizeof(Pixel16);

	return returnBuf;
}


Pixel16 * spriteutils_RGB32ToEncoded(Pixel32 *buf, uint16 width, uint16 height, size_t *size)
{
	Pixel32             *srcPixel = buf;

	Pixel16             *outBuf = new Pixel16[(1+height+width*height)*8];
	Pixel16             *returnBuf = NULL;
	uint16              *table = (uint16 *)outBuf;
	Pixel16             *startOfData;
	Pixel16             *dataPtr, *startDataPtr;
	BOOL                empty;



	*table++ = (uint16)height;

	startOfData = outBuf + 1 + height;
	dataPtr = startOfData;

	for(sint32 y=0; y<height; y++) {

		srcPixel = buf + width * y;

		startDataPtr = dataPtr;


		empty = spriteutils_EncodeScanlineWshadow(srcPixel, width, &dataPtr);
		if (empty) {
			*table++ = k_EMPTY_TABLE_ENTRY;
		} else {

			*table++ = startDataPtr - startOfData;
		}
	}

	sint32 resultSize = (dataPtr - outBuf);




	returnBuf = new Pixel16[resultSize];

	memcpy(returnBuf, outBuf, resultSize * sizeof(Pixel16));
	delete[] outBuf;
	if (size)
		*size = resultSize * sizeof(Pixel16);

	return returnBuf;
}

void spriteutils_DecodeToBuffer(Pixel16 *data, sint32 width, sint32 height)
{
	Pixel16     *table = data+1;
	Pixel16     *dataStart = table + height * 2;
	Pixel16     *outBuf = (Pixel16 *)malloc(width * height * 8);
	Pixel16     *destPixel = outBuf;

	for(sint32 j=0; j<height; j++) {
		if (table[j*2] != k_EMPTY_TABLE_ENTRY) {
			Pixel16     *rowData;
			Pixel16     tag;

			rowData = dataStart + table[j*2+1];

			tag = *rowData--;

			tag = tag & 0x0FFF;

			while ((tag & 0xF000) == 0) {
				switch ((tag & 0x0F00) >> 8) {
					case k_CHROMAKEY_RUN_ID :
						destPixel += (tag & 0x00FF);
						break;
					case k_COPY_RUN_ID:
						{
							short len = (tag & 0x00FF);

							for (short i=0; i<len; i++) {
								*destPixel++ = *rowData--;
							}


							tag = *rowData--;
						}
						break;
					case k_SHADOW_RUN_ID:
						{
							short len = (tag & 0x00FF);

							for (short i=0; i<len; i++) {
								*destPixel = pixelutils_Shadow(*destPixel);
								destPixel++;
							}

							tag = *rowData--;
						}
						break;
					case k_FEATHERED_RUN_ID:
						if (TRUE) { // ???
								short alpha = (tag & 0x00FF);

								*destPixel = pixelutils_Blend(*rowData, *destPixel, (short)alpha>>4);
								destPixel++;
								rowData--;
						} else {
							*destPixel++ = *rowData--;
						}
						tag = *rowData--;
						break;
					default:
						Assert(FALSE);
				}
				tag = *rowData--;
			}
		}
	}
}

Pixel32	spriteutils_AveragePixel32(Pixel32 pixel1, Pixel32 pixel2, Pixel32 pixel3, Pixel32 pixel4)
{
	Pixel16     r1, r2, r3, r4;
	Pixel16     g1, g2, g3, g4;
	Pixel16     b1, b2, b3, b4;
	Pixel16     a1, a2, a3, a4;
	Pixel32     r, g, b, a;
	Pixel32     result;

	RGB32Components(pixel1, &r1, &g1, &b1, &a1);
	RGB32Components(pixel2, &r2, &g2, &b2, &a2);
	RGB32Components(pixel3, &r3, &g3, &b3, &a3);
	RGB32Components(pixel4, &r4, &g4, &b4, &a4);

	r = (Pixel32)ceil((r1 + r2 + r3 + r4) / 4.0);
	g = (Pixel32)ceil((g1 + g2 + g3 + g4) / 4.0);
	b = (Pixel32)ceil((b1 + b2 + b3 + b4) / 4.0);
	a = (Pixel32)ceil((a1 + a2 + a3 + a4) / 4.0);

	result =  ( r | (g << 8) | (b << 16) | (a << 24));

	return result;
}

void spriteutils_CreateQuarterSize(Pixel32 *srcBuf, sint32 srcWidth, sint32 srcHeight, Pixel32 **destBuf, BOOL aa)
{
	sint32      destWidth = srcWidth / 2;
	sint32      destHeight = srcHeight / 2;
	sint32      i,j;
	Pixel32     pixel, pixel1, pixel2, pixel3, pixel4;
	Pixel32     *outBuf;

	outBuf = (Pixel32 *)malloc(destWidth * destHeight * sizeof(Pixel32) );

	for (i=0; i<destHeight; i++) {
		for (j=0; j<destWidth; j++) {
			pixel1 = srcBuf[(i * 2) * srcWidth + (j * 2)];

			if (aa) {
				pixel2 = srcBuf[(i * 2) * srcWidth + (j * 2) + 1];
				pixel3 = srcBuf[((i * 2) + 1) * srcWidth + (j * 2)];
				pixel4 = srcBuf[((i * 2) + 1) * srcWidth + (j * 2) + 1];

				pixel = spriteutils_AveragePixel32(pixel1, pixel2, pixel3, pixel4);
			} else {
				pixel = pixel1;
			}

			outBuf[i*destWidth + j] = pixel;
		}
	}

	*destBuf = outBuf;
}



void spriteutils_ConvertPixelFormat(Pixel16 *frame, sint32 width, sint32 height)
{
	Pixel16     *table = frame+1;
	Pixel16     *dataStart = table + height;


	for(sint32 j=0; j<height; j++) {
		if (table[j] != k_EMPTY_TABLE_ENTRY) {
			Pixel16     *rowData;
			Pixel16     tag;

			rowData = dataStart + table[j];

			tag = *rowData++;

			tag = tag & 0x0FFF;

			while ((tag & 0xF000) == 0) {
				switch ((tag & 0x0F00) >> 8) {
					case k_CHROMAKEY_RUN_ID :
						break;
					case k_COPY_RUN_ID      : {
							short len = (tag & 0x00FF);
							for (short i=0; i<len; i++) {
#ifdef CLEAN_INSTEAD_OF_CONVERT
								if (*rowData == 0x0000)
									*rowData = 0x0001;
#else
								*rowData = pixelutils_Convert565to555(*rowData);
								if (!*rowData)
									*rowData = 1;
#endif
								rowData++;
							}
						}
						break;
					case k_SHADOW_RUN_ID    :

						break;
					case k_FEATHERED_RUN_ID :
#ifdef CLEAN_INSTEAD_OF_CONVERT
							if (*rowData == 0x0000)
								*rowData = 0x0001;
#else
							*rowData = pixelutils_Convert565to555(*rowData);
							if (!*rowData)
								*rowData = 1;
#endif
							rowData++;
						break;
					default:
						Assert(FALSE);
				}
				tag = *rowData++;
			}
		}
	}
}




void spriteutils_ConvertPixelFormatForFile(Pixel16 *frame, sint32 width, sint32 height)
{
	if (g_is565Format) return;

	Pixel16     *table = frame+1;
	Pixel16     *dataStart = table + height;


	for(sint32 j=0; j<height; j++) {
		if (table[j] != k_EMPTY_TABLE_ENTRY) {
			Pixel16     *rowData;
			Pixel16     tag;

			rowData = dataStart + table[j];

			tag = *rowData++;

			tag = tag & 0x0FFF;

			while ((tag & 0xF000) == 0) {
				switch ((tag & 0x0F00) >> 8) {
					case k_CHROMAKEY_RUN_ID :
						break;
					case k_COPY_RUN_ID      : {
							short len = (tag & 0x00FF);
							for (short i=0; i<len; i++) {
#ifdef CLEAN_INSTEAD_OF_CONVERT
								if (*rowData == 0x0000)
									*rowData = 0x0001;
#else
								*rowData = pixelutils_Convert565to555(*rowData);
								if (!*rowData)
									*rowData = 1;
#endif
								rowData++;
							}
						}
						break;
					case k_SHADOW_RUN_ID    :

						break;
					case k_FEATHERED_RUN_ID :
#ifdef CLEAN_INSTEAD_OF_CONVERT
							if (*rowData == 0x0000)
								*rowData = 0x0001;
#else
							*rowData = pixelutils_Convert555to565(*rowData);
							if (!*rowData)
								*rowData = 1;
#endif
							rowData++;
						break;
					default:
						Assert(FALSE);
				}
				tag = *rowData++;
			}
		}
	}
}

sint32 spriteutils_DeltaToFacing(sint32 x, sint32 y)
{
	if (x < 0) {
		if (y < 0) return 7;
		else if (y > 0) return 5;
		else return 6;
	} else {
		if (x > 0) {
			if (y < 0) return 1;
			else if (y > 0) return 3;
			else return 2;
		} else {

			if (y < 0) return 0;
			else if (y > 0) return 4;
			else return 4;
		}
	}
}
