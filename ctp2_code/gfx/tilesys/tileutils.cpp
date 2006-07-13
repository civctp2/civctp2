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
// WIN32
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "Globals.h"

#include "pixelutils.h"
#include "spriteutils.h"
#include "tileutils.h"
#include "tiffutils.h"
#include "targautils.h"

#include "BaseTile.h"
#include "tileset.h"

#include "Token.h"
#include "CivPaths.h"

#include "arproces.h"
#include "terrainutil.h"
#include "TerrainRecord.h"

extern CivPaths		*g_civPaths;

BOOL		g_isForSpace;

BOOL		g_useLowPass = FALSE;
uint32		g_lowPassScale = 1;

BOOL		g_adjustBrightness = FALSE;
sint16		g_brightness = 0;

BOOL		g_adjustContrast = FALSE;
uint32		g_threshold = 2;

BOOL		g_adjustSV = FALSE;
double		g_deltaS = 0;
double		g_deltaV = 0;

BOOL		g_blendWithGray = TRUE;
Pixel16		g_grayValueR = 0x0025;
Pixel16		g_grayValueG = 0x0025;
Pixel16		g_grayValueB = 0x0045;

Pixel16	*   g_transitions[TERRAIN_MAX][TERRAIN_MAX][k_TRANSITIONS_PER_TILE];

extern sint32		g_is565Format;

Pixel32 *tileutils_LowPassFilter(Pixel32 *image, uint32 width, uint32 height)
{
	double LP1[]=
				{ 0.11111111, 0.11111111, 0.11111111,
				  0.11111111, 0.11111111, 0.11111111,
				  0.11111111, 0.11111111, 0.11111111 };


	Pixel32	*filteredImage;

	if (!RealConvolution(image, width, height, 0, 0, LP1, 3, 3, g_lowPassScale, FALSE, &filteredImage)) {
		free(image);
		return filteredImage;
	} else {
		return NULL;
	}
}

char *tileutils_PostProcessImage(char *image, uint16 width, uint16 height)
{
	if (!g_isForSpace) return image;

	if (g_useLowPass) {
		image = (char *)tileutils_LowPassFilter((Pixel32 *)image, (uint32)width, (uint32)height);
		if (image == NULL) return NULL;
	}

	if (g_adjustBrightness) {
		AdjImageBrightness((Pixel32 *)image, g_brightness, (uint32)width, (uint32)height, 0, 0);
	}

	if (g_adjustContrast) {
		StretchImageContrast((Pixel32 *)image, g_threshold, (uint32)width, (uint32)height, 0, 0);
	}

	if (g_adjustSV) {
		AdjustSV((Pixel32 *)image, (uint32)width,  (uint32)height, g_deltaS, g_deltaV);
	}

	if (g_blendWithGray) {
		BlendWithGray((Pixel32 *)image, (uint32)width, (uint32)height, g_grayValueR, g_grayValueG, g_grayValueB);
	}

	return image;
}

char *tileutils_TIF2mem(char *filename, uint16 *width, uint16 *height)
{
	char *image = TIF2mem(filename, width, height);
	
	return tileutils_PostProcessImage(image, *width, *height);
}

char *tileutils_StripTIF2Mem(char *filename, uint16 *width, uint16 *height)
{
	char *image = StripTIF2Mem(filename, width, height);

	return tileutils_PostProcessImage(image, *width, *height);
}


Pixel16 *tileutils_TGA2mem(char *filename, uint16 *width, uint16 *height)
{


	int		bpp;
	int		w, h;

	if (!Get_TGA_Dimension(filename, w, h, bpp)) {
		Assert(FALSE);
		*width = 0;
		*height = 0;
		return NULL;
	}

	Pixel16 *   buffer = new Pixel16[w * h];
	Load_TGA_File(filename, (uint8 *)buffer, (int)w*sizeof(Pixel16), w, h, NULL, FALSE);

	*width = (uint16)w;
	*height = (uint16)h;

	return buffer;
}


void tileutils_EncodeCopyRun(Pixel32 **inBuf, int *pos, int width, Pixel16 **outBufPtr)
{
	Pixel16			pix16;
	unsigned char	alpha;
	int				runLen      = 0;
	Pixel16	*       headerPtr   = *outBufPtr;
	(*outBufPtr)++;

	RGB32Info(**inBuf, &pix16, &alpha);
	Pixel32         pix32 = (**inBuf) & k_32_BIT_RGB_MASK;

	while (pix32 != k_32_BIT_SKIP_PIXEL 
				&& pix32 != k_32_BIT_SHADOW_PIXEL 
				&& pix32 != k_32_BIT_COLORIZE_PIXEL 
				&& (*pos < width)) {
		
		**outBufPtr = pix16;

		
		(*outBufPtr)++;
		
		
		(*inBuf)++;

		
		(*pos)++;

		
		runLen++;

		RGB32Info(**inBuf, &pix16, &alpha);
		pix32 = (**inBuf) & k_32_BIT_RGB_MASK;
	}
	
	
	Pixel16			footer      = k_TILE_COPY_RUN_ID << 8 | runLen;

	
	if (*pos >= width) footer |= k_TILE_EOLN_ID << 8;

	*headerPtr = footer;
}

void tileutils_EncodeColorizeRun(Pixel32 **inBuf, int *pos, int width, Pixel16 **outBufPtr)
{
	Pixel16			pix16;
	unsigned char	alpha;
	int				runLen      = 0;
	Pixel16	*       headerPtr   = *outBufPtr;
	(*outBufPtr)++;

	RGB32Info(**inBuf, &pix16, &alpha);
	Pixel32			pix32       = (**inBuf) & k_32_BIT_RGB_MASK;
	while (pix32 == k_32_BIT_COLORIZE_PIXEL && (*pos < width)) {
		
		(*inBuf)++;

		
		(*pos)++;

		
		runLen++;

		RGB32Info(**inBuf, &pix16, &alpha);
		pix32 = (**inBuf) & k_32_BIT_RGB_MASK;
	}
	
	
	Pixel16			footer      = k_TILE_COLORIZE_RUN_ID << 8 | runLen;

	
	if (*pos >= width) footer |= k_TILE_EOLN_ID << 8;

	*headerPtr = footer;
}

void tileutils_EncodeShadowRun(Pixel32 **inBuf, int *pos, int width, Pixel16 **outBufPtr)
{
	Pixel16			pix16;
	unsigned char	alpha;
	int				runLen      = 0;
	Pixel16	*       headerPtr   = *outBufPtr;
	(*outBufPtr)++;

	RGB32Info(**inBuf, &pix16, &alpha);
	Pixel32			pix32       = (**inBuf) & k_32_BIT_RGB_MASK;
	while (pix32 == k_32_BIT_SHADOW_PIXEL && (*pos < width)) {
		
		(*inBuf)++;

		
		(*pos)++;

		
		runLen++;

		RGB32Info(**inBuf, &pix16, &alpha);
		pix32 = (**inBuf) & k_32_BIT_RGB_MASK;
	}
	
	
    Pixel16         footer      = k_TILE_SHADOW_RUN_ID << 8 | runLen;

	
	if (*pos >= width) footer |= k_TILE_EOLN_ID << 8;

	*headerPtr = footer;
}

char tileutils_EncodeSkipRun(Pixel32 **inBuf, int *pos, int width, Pixel16 **outBufPtr)
{
	Pixel16			pix16;
	unsigned char	alpha;
	int				runLen = 0;

	RGB32Info(**inBuf, &pix16, &alpha);
	Pixel32			pix32   = (**inBuf) & k_32_BIT_RGB_MASK;

	while (pix32 == k_32_BIT_SKIP_PIXEL && (*pos < width)) {
		
		(*inBuf)++;

		
		(*pos)++;

		
		runLen++;

		RGB32Info(**inBuf, &pix16, &alpha);
		pix32 = (**inBuf) & k_32_BIT_RGB_MASK;
	}

	
	if (runLen < width) {
		
		Pixel16			footer = k_TILE_SKIP_RUN_ID << 8 | runLen;

		
		if (*pos >= width) footer |= k_EOLN_ID << 8;
		
		
		**outBufPtr = footer;
		(*outBufPtr)++;
	    return FALSE;
	} else {
		
		return TRUE;
	}

}

char tileutils_EncodeScanline(Pixel32 *scanline, int width, Pixel16 **outBufPtr)
{
	Pixel16			pix16;
	Pixel32			pix32;
	Pixel32	*       scanPtr    = scanline;
	unsigned char	alpha;
	int				pos         = 0;
	char			empty = FALSE;

	while (scanPtr < (scanline + width)) {
		pix32 = *scanPtr;

		RGB32Info(pix32, &pix16, &alpha);
		pix32 = pix32 & 0x00FFFFFF;

		switch (pix32) {
		case k_32_BIT_SKIP_PIXEL :
			empty = tileutils_EncodeSkipRun(&scanPtr, &pos, width, outBufPtr);
			break;
		case k_32_BIT_COLORIZE_PIXEL :
			tileutils_EncodeColorizeRun(&scanPtr, &pos, width, outBufPtr);
			break;
		case k_32_BIT_SHADOW_PIXEL :
			tileutils_EncodeShadowRun(&scanPtr, &pos, width, outBufPtr);
			break;
		default:
			tileutils_EncodeCopyRun(&scanPtr, &pos, width, outBufPtr);
		}
	}

	return empty;
}

Pixel16 *tileutils_EncodeTile(Pixel32 *buf, uint16 width, uint16 height, uint32 *size)
{
	Pixel32	*               srcPixel            = buf;
	Pixel16	*               outBuf              = new Pixel16[width * height * 2];
	Pixel16	*               table               =  new Pixel16[height + 2];
	Pixel16 *               startDataPtr;
	char				    empty               = FALSE;
	int					    lastNonEmpty        = -1;

    Pixel16 *               tableStart          = table;
	unsigned short int *    firstNonEmptyPtr    = table;
	table++;
	unsigned short int *    endLinePtr          = table;
	table++;

	Pixel16	*               startOfData         = outBuf;
	Pixel16 *               dataPtr             = startOfData;
	int                     firstNonEmpty       = -1;

	for(int y=0; y<height; y++) {
		
		srcPixel = buf + width * y;

		startDataPtr = dataPtr;

		
		empty = tileutils_EncodeScanline(srcPixel, width, &dataPtr);
		if (!empty) {
			if (firstNonEmpty == -1) {
				
				*firstNonEmptyPtr = y;
				firstNonEmpty = 0;
			}

		
			*table++ = startDataPtr - startOfData;
			lastNonEmpty = y;
		} else {
			
			
            
			if (firstNonEmpty != -1)
				*table++ = (Pixel16) -1;
            
		}
	}

	if (lastNonEmpty != -1) {
		*endLinePtr = lastNonEmpty;
	} else {
		Assert(lastNonEmpty != -1);
	}

	int tableSize = ((*endLinePtr-*firstNonEmptyPtr + 1 + 2)) * 2;	

	int dataSize = (dataPtr - outBuf) * 2;

	char *returnBuf = new char[dataSize+tableSize];

	memcpy(returnBuf, tableStart, tableSize);
	memcpy(returnBuf+tableSize, outBuf, dataSize);

	*size = (dataSize + tableSize);

	return (Pixel16 *)returnBuf;
}

void tileutils_EncodeCopyRun16(Pixel16 **inBuf, int *pos, int width, Pixel16 **outBufPtr,
							   BOOL sourceDataIs565)
{
	int			runLen      = 0;
	Pixel16	*   headerPtr   = *outBufPtr;
	(*outBufPtr)++;

	Pixel16		pix16       = (**inBuf);
	if (sourceDataIs565)
		pix16 = ((pix16 & 0xFFC0) >> 1) | (pix16 & 0x001F);

	while (pix16 != k_16_BIT_SKIP_PIXEL 
				&& pix16 != k_16_BIT_SHADOW_PIXEL 
				&& pix16 != k_16_BIT_COLORIZE_PIXEL 
				&& (*pos < width)) {
		
		**outBufPtr = pix16;

		
		(*outBufPtr)++;
		
		
		(*inBuf)++;

		
		(*pos)++;

		
		runLen++;

		pix16 = (**inBuf);
		if (sourceDataIs565)
			pix16 = ((pix16 & 0xFFC0) >> 1) | (pix16 & 0x001F);
	}
	
	
	Pixel16			footer  = k_TILE_COPY_RUN_ID << 8 | runLen;

	
	if (*pos >= width) footer |= k_TILE_EOLN_ID << 8;

	*headerPtr = footer;
}

void tileutils_EncodeColorizeRun16(Pixel16 **inBuf, int *pos, int width, Pixel16 **outBufPtr,
								   BOOL sourceDataIs565)
{
	int			runLen      = 0;
	Pixel16 *   headerPtr   = *outBufPtr;
	(*outBufPtr)++;

	Pixel16		pix16       = (**inBuf);
	if (sourceDataIs565)
		pix16 = ((pix16 & 0xFFC0) >> 1) | (pix16 & 0x001F);

	while (pix16 == k_16_BIT_COLORIZE_PIXEL && (*pos < width)) {
		
		(*inBuf)++;

		
		(*pos)++;

		
		runLen++;

		pix16 = (**inBuf);
		if (sourceDataIs565)
			pix16 = ((pix16 & 0xFFC0) >> 1) | (pix16 & 0x001F);
	}
	
	
	Pixel16		footer  = k_TILE_COLORIZE_RUN_ID << 8 | runLen;

	
	if (*pos >= width) footer |= k_TILE_EOLN_ID << 8;

	*headerPtr = footer;
}

void tileutils_EncodeShadowRun16(Pixel16 **inBuf, int *pos, int width, Pixel16 **outBufPtr,
								 BOOL sourceDataIs565)
{
	int			runLen      = 0;
	Pixel16 *   headerPtr   = *outBufPtr;
	(*outBufPtr)++;

	Pixel16		pix16       = (**inBuf);
	if (sourceDataIs565)
		pix16 = ((pix16 & 0xFFC0) >> 1) | (pix16 & 0x001F);
	while (pix16 == k_16_BIT_SHADOW_PIXEL && (*pos < width)) {
		
		(*inBuf)++;

		
		(*pos)++;

		
		runLen++;

		pix16 = (**inBuf);
		if (sourceDataIs565)
			pix16 = ((pix16 & 0xFFC0) >> 1) | (pix16 & 0x001F);
	}
	
	
	Pixel16		footer      = k_TILE_SHADOW_RUN_ID << 8 | runLen;

	
	if (*pos >= width) footer |= k_TILE_EOLN_ID << 8;

	*headerPtr = footer;
}

char tileutils_EncodeSkipRun16(Pixel16 **inBuf, int *pos, int width, Pixel16 **outBufPtr,
							   BOOL sourceDataIs565)
{
	int				runLen = 0;

	Pixel16			pix16   = (**inBuf);
	if (sourceDataIs565)
		pix16 = ((pix16 & 0xFFC0) >> 1) | (pix16 & 0x001F);
	while (pix16 == k_16_BIT_SKIP_PIXEL && (*pos < width)) {
		
		(*inBuf)++;

		
		(*pos)++;

		
		runLen++;

		pix16 = (**inBuf);
		if (sourceDataIs565)
			pix16 = ((pix16 & 0xFFC0) >> 1) | (pix16 & 0x001F);
	}

	
	if (runLen < width) {
		
	    Pixel16 footer  = k_TILE_SKIP_RUN_ID << 8 | runLen;

		
		if (*pos >= width) footer |= k_EOLN_ID << 8;
		
		
		**outBufPtr = footer;
		(*outBufPtr)++;
	    return FALSE;
	} else {
		
		return TRUE;
	}

}

char tileutils_EncodeScanline16(Pixel16 *scanline, int width, Pixel16 **outBufPtr, 
								BOOL sourceDataIs565)
{
	Pixel16		pix16;
	Pixel16	*   scanPtr     = scanline;
	int			pos         = 0;
	char		empty       = FALSE;

	while (scanPtr < (scanline + width)) {
		pix16 = *scanPtr;
		if (sourceDataIs565)
			pix16 = ((pix16 & 0xFFC0) >> 1) | (pix16 & 0x001F);
		switch (pix16) {
		case k_16_BIT_SKIP_PIXEL :
			empty = tileutils_EncodeSkipRun16(&scanPtr, &pos, width, outBufPtr, sourceDataIs565);
			break;
		case k_16_BIT_COLORIZE_PIXEL :
			tileutils_EncodeColorizeRun16(&scanPtr, &pos, width, outBufPtr, sourceDataIs565);
			break;
		case k_16_BIT_SHADOW_PIXEL :
			tileutils_EncodeShadowRun16(&scanPtr, &pos, width, outBufPtr, sourceDataIs565);
			break;
		default:
			tileutils_EncodeCopyRun16(&scanPtr, &pos, width, outBufPtr, sourceDataIs565);
		}
	}

	return empty;
}

Pixel16 *tileutils_EncodeTile16(Pixel16 *buf, uint16 width, uint16 height, uint32 *size, 
								sint32 pitch)
{
	Pixel16				*srcPixel = buf;
	Pixel16				*outBuf = new Pixel16[width * height * 2];
	Pixel16				*table =  new Pixel16[height + 2];
	Pixel16				*startDataPtr;
	char				empty = FALSE;
	int					lastNonEmpty = -1;
	
	BOOL				sourceDataIs565 = FALSE;

	if (pitch == 0) {
		pitch = width*2;
	} else {
		
		if (g_is565Format) 
			sourceDataIs565 = TRUE;
	}

	Pixel16 *               tableStart          = table;
	unsigned short int *    firstNonEmptyPtr    = table;
	table++;
	unsigned short int *    endLinePtr          = table;
	table++;

	Pixel16	*               startOfData         = outBuf;
	Pixel16 *               dataPtr             = outBuf;
	int                     firstNonEmpty       = -1;

	for(int y=0; y<height; y++) {
		
		srcPixel = buf + (pitch/2) * y;

		startDataPtr = dataPtr;

		
		empty = tileutils_EncodeScanline16(srcPixel, width, &dataPtr, sourceDataIs565);
	if (!empty) {
			if (firstNonEmpty == -1) {
				
				*firstNonEmptyPtr = y;
				firstNonEmpty = 0;
			}

			
			*table++ = startDataPtr - startOfData;
			lastNonEmpty = y;
		} else {
			
			
            
			if (firstNonEmpty != -1)
				*table++ = (Pixel16) -1;
            
		}
	}

	if (lastNonEmpty != -1) {
		*endLinePtr = lastNonEmpty;
	} else {
		Assert(lastNonEmpty != -1);
	}

	int tableSize = ((*endLinePtr-*firstNonEmptyPtr + 1 + 2)) * 2;	

	int dataSize = (dataPtr - outBuf) * 2;

	char *returnBuf = new char[dataSize+tableSize];

	memcpy(returnBuf, tableStart, tableSize);
	memcpy(returnBuf+tableSize, outBuf, dataSize);

	*size = (dataSize + tableSize);

	delete[] outBuf;
	delete[] tableStart;

	return (Pixel16 *)returnBuf;
}


sint32 tileutils_ConvertPixelFormatFrom565(Pixel16 *data)
{
	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);

	
	for(sint32 j=start; j<end; j++) {

		if ((sint16)table[j-start] == -1) 
			continue;

		Pixel16	*   rowData = dataStart + table[j-start];		
		Pixel16		tag;

		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
					break;
				case k_TILE_COPY_RUN_ID			: {
						short len = (tag & 0x00FF);

						for (short i=0; i<len; i++) {
							*rowData = pixelutils_Convert565to555(*rowData);
							rowData++;
						}

					}
					break;
				case k_TILE_SHADOW_RUN_ID:
					break;
				case k_TILE_COLORIZE_RUN_ID:
					break;
			}

		} while ((tag & 0xF000) == 0);
	}

	return 0;
}

sint32 tileutils_ConvertPixelFormatFrom555(Pixel16 *data)
{
	uint16		start = (uint16)*data++;
	uint16		end = (uint16)*data++;
	Pixel16		*table = data;
	Pixel16		*dataStart = table + (end - start + 1);

	
	for(sint32 j=start; j<end; j++) {

		if ((sint16)table[j-start] == -1) 
			continue;

		Pixel16	*   rowData = dataStart + table[j-start];		
		Pixel16		tag;

		do {
			tag = *rowData++;

			switch ((tag & 0x0F00) >> 8) {
				case k_TILE_SKIP_RUN_ID	:
					break;
				case k_TILE_COPY_RUN_ID			: {
						short len = (tag & 0x00FF);

						for (short i=0; i<len; i++) {
							*rowData = pixelutils_Convert555to565(*rowData);
							rowData++;
						}

					}
					break;
				case k_TILE_SHADOW_RUN_ID:
					break;
				case k_TILE_COLORIZE_RUN_ID:
					break;
			}

		} while ((tag & 0xF000) == 0);
	}

	return 0;
}

void tileutils_DecodeToBuffer(Pixel16 *data, int width, int height)
{
	Pixel16		*outBuf = (Pixel16 *)malloc(width * height * 2);
	Pixel16		*destPixel = outBuf;

	Pixel16		*table = data + 1;
	Pixel16		start = *data++;
	Pixel16		*dataStart = table + (72 - start);

	
	for(int j=start; j<72; j++) {
		if (table[j-start] != k_EMPTY_TABLE_ENTRY) {		
			Pixel16 *   rowData = dataStart + table[j-start];
			Pixel16		tag     = *rowData++ & 0x0FFF;
			
			while ((tag & 0xF000) == 0) {
				switch ((tag & 0x0F00) >> 8) {
					case k_TILE_SKIP_RUN_ID	:
							destPixel += (tag & 0x00FF);
						break;
					case k_TILE_COPY_RUN_ID			: {
							short len = (tag & 0x00FF);

							for (short i=0; i<len; i++) {
								*destPixel++ = *rowData++;
							}

						}
						break;
				}
				tag = *rowData++;
			}
		}
	}
}


Pixel16 g_offsets[k_TILE_PIXEL_HEIGHT*32*sizeof(Pixel16)];
uint32 g_bitsTable[k_TILE_PIXEL_HEIGHT];
uint16 g_stencilSize = 0;

sint32 tileutils_EncodeStencil(MBCHAR *filename)
{
	sint32	i,j;

	
	uint16	width=0, height=0;
	char	*tif    = tileutils_TIF2mem(filename, &width, &height);
	Assert(tif != NULL);
	if (tif == NULL) exit(-1);

	if (height != k_TILE_PIXEL_HEIGHT) {
		printf("\nImage height is %d and should be %d.\n", height, k_TILE_PIXEL_HEIGHT);
		exit(-1);
	}
	if (width != k_TILE_PIXEL_WIDTH) {
		printf("\nImage width is %d and should be %d.\n", width, k_TILE_PIXEL_WIDTH);
		exit(-1);
	}

	
	Pixel32 *   ptr     = (Pixel32 *)tif;
	sint32      top     = 9999999;
	sint32      bottom  = -1;
	sint32      left    = 9999999;
	sint32      right   = -1;
	
	for (i=0; i<height; i++) {
		for (j=0; j<width; j++) {
			if ((ptr[i*width+j] & 0xFFFFFF) != 0) {
				if (j < left)
					left = j;
				if (i < top)
					top = i;
				if (i > bottom)
					bottom = i;
				if (j > right) 
					right = j;
			}
		}
	}

//	RECT stencilRect = {left, top, right, bottom};

	printf("Stencil: left %d top %d right %d bottom %d\n", left, top, right, bottom);

	uint32 accum = 0;
//	sint32 counter = 0;

	
	for (i=0; i<k_TILE_PIXEL_HEIGHT; i++) 
		g_bitsTable[i] = 0;

	
	uint32	bit;

	for (i=0; i<k_TILE_PIXEL_HEIGHT * 32; i++) g_offsets[i] = 0xFFFF;

	uint16	index=0;

	for (i=top; i<=bottom; i++) {
		if (i<=23) left = (23-i)*2;
		else left = (i-24)*2;

		uint16 rowIndex = 0;
		for (j=left; j<left+32; j++) {
			if ((ptr[i*width+j] & 0xFFFFFF) != 0) {
				bit = 0x80000000;
				g_offsets[i*32 + rowIndex] = index;
				index++;
				rowIndex++;
			} else {
				bit = 0;
			}
			accum >>= 1;
			accum = (accum | bit);
		}
		g_bitsTable[i] = accum;
		accum = 0;
	}




	g_stencilSize = index;

	uint16 tableHeight = k_TILE_PIXEL_HEIGHT;

	FILE *  file = fopen("source" FILE_SEP "stencil" FILE_SEP "stencil.bin", "wb");
	if (file != NULL) {
		fwrite((void *)&tableHeight, 1, sizeof(uint16), file);
		fwrite((void *)g_bitsTable, 1, sizeof(sint32) * k_TILE_PIXEL_HEIGHT, file);
		fwrite((void *)&index, 1, sizeof(index), file);
		fwrite((void *)g_offsets, 1, 32 * k_TILE_PIXEL_HEIGHT * sizeof(Pixel16), file);
		fclose(file);
	}

	return 0;
}

void tileutils_DumpStencil(MBCHAR *filename)
{
	uint32		accum;
	sint32		i,j;
	sint32		nudge;
	FILE *  file = fopen(filename, "w");

	for (i=0; i<48; i++) {
		accum = g_bitsTable[i];
		if (i<=23) nudge = (23-i)*2;
		else nudge = (i-24)*2;

		for (j=0; j<nudge; j++)
			fprintf(file," ");

		for (j=0; j<32; j++) {
			if (accum & 1) fprintf(file,"1");
			else fprintf(file,"0");
			accum>>=1;
		}
		fprintf(file,"\n");
	}

	for (i=0; i<k_TILE_PIXEL_HEIGHT; i++) {
		for (j=0; j<32; j++) {
			fprintf(file, "\t%d", g_offsets[i*32+j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
}

void tileutils_LoadStencil(void)
{
	FILE *  file    = fopen("source" FILE_SEP "stencil" FILE_SEP "stencil.bin", "rb");

	if (file != NULL) {
	    uint16		tableHeight = 0;
		fread((void *)&tableHeight, 1, sizeof(uint16), file);

		fread((void *)g_bitsTable, 1, sizeof(sint32) * k_TILE_PIXEL_HEIGHT, file);

	    uint16		stencilLen = 0;
		fread((void *)&stencilLen, 1, sizeof(stencilLen), file);
		g_stencilSize = stencilLen;

		fread((void *)g_offsets, 1, 32 * k_TILE_PIXEL_HEIGHT * sizeof(Pixel16), file);

		fclose(file);
	}


}

Pixel16 *tileutils_CreateBorkBork(void)
{
	Pixel16		*bork = new Pixel16[48*94];
	sint32		i,j;
	sint32		pixelX;
	uint32		accum;

	memset(bork, 0xFF, sizeof(Pixel16)*48*94);

	for (i=0; i<k_TILE_PIXEL_HEIGHT; i++) {
		if (i<=23) {
			pixelX = (23-i)*2;
		} else {
			pixelX = (i-24)*2;
		}

		accum = g_bitsTable[i];
		for (j=0; j<32; j++) {			
			if (accum & 0x00000001) {
				
				bork[i * k_TILE_PIXEL_WIDTH + pixelX] = 0;
				
				bork[(k_TILE_PIXEL_HEIGHT - 1 - i) * k_TILE_PIXEL_WIDTH + pixelX] = 1;
				
				bork[(k_TILE_PIXEL_HEIGHT - 1 - i) * k_TILE_PIXEL_WIDTH + (k_TILE_PIXEL_WIDTH - 1 - pixelX)] = 2;
				
				bork[i * k_TILE_PIXEL_WIDTH + (k_TILE_PIXEL_WIDTH - 1 - pixelX)] = 3;
			}
			accum >>= 1;
			pixelX++;
		}
	}
















	return bork;
}


Pixel16 *tileutils_ExtractUpperRight(char *tif, uint16 width, uint16 height, sint32 x, sint32 y)
{
	sint32		i,j;
	sint32		pixelX, pixelY;
	uint32		accum;
	uint32		*image = (uint32 *)tif;

	Pixel16		*data = new Pixel16[g_stencilSize];
	Pixel16		*dataPtr = data;
	Pixel32		pix32;
	Pixel16		pix16;
	uint8		alpha;
	sint32		row;

	for (i=0; i<k_TILE_PIXEL_HEIGHT; i++) {
		row = y+(k_TILE_PIXEL_HEIGHT-1)-i;
		if (i<=23) {
			pixelX = (x + k_TILE_PIXEL_WIDTH) - (23-i)*2-1;
			pixelY = row;
		} else {
			pixelX = (x + k_TILE_PIXEL_WIDTH) - (i-24)*2-1;
			pixelY = row;
		}
		
		accum = g_bitsTable[i];
		for (j=0; j<32; j++) {			
			if (accum & 0x00000001) {
				pix32 = image[ row * width + pixelX];

				RGB32Info(pix32, &pix16, &alpha);

				*dataPtr = pix16;
				dataPtr++;
			}
			accum >>= 1;
			pixelX--;
		}
	}

	return data;
}

Pixel16 *tileutils_ExtractLowerLeft(char *tif, uint16 width, uint16 height, sint32 x, sint32 y)
{
	sint32		i,j;
	sint32		pixelX, pixelY;
	uint32		accum;
	uint32		*image = (uint32 *)tif;

	Pixel16		*data = new Pixel16[g_stencilSize];
	Pixel16		*dataPtr = data;
	Pixel32		pix32;
	Pixel16		pix16;
	uint8		alpha;
	sint32		row;

	for (i=0; i<k_TILE_PIXEL_HEIGHT; i++) {
		row = (y + i);
		if (i<=23) {
			pixelX = (x + (23-i)*2);
			pixelY = row;
		} else {
			pixelX = (x + (i-24)*2);
			pixelY = row;
		}
		
		accum = g_bitsTable[i];
		for (j=0; j<32; j++) {			
			if (accum & 0x00000001) {
				pix32 = image[ row * width + pixelX];

				RGB32Info(pix32, &pix16, &alpha);

				*dataPtr = pix16;
				dataPtr++;
			}
			accum >>= 1;
			pixelX++;
		}
	}

	return data;
}

Pixel16 *tileutils_LoadStencilImage(uint16 from, uint16 to)
{
	Pixel16		*data = new Pixel16[g_stencilSize];
	MBCHAR		fname[_MAX_PATH];
	sprintf(fname, "output" FILE_SEP "xitions" FILE_SEP "gtft%.2d%.2d.bin", from, to);

	FILE *      file = fopen(fname, "rb");
	if (file != NULL) {
		fread((void *)data, 1, g_stencilSize * sizeof(Pixel16), file);
		fclose(file);
	}

	return data;
}

Pixel16 *tileutils_MakeTransition1(Pixel16 *sourceStencil)
{
	sint32		i,j;
	Pixel16		pix;
	uint32		accum;
	uint16		off;
	sint32		rowIndex;

	Pixel16 *   data    = new Pixel16[g_stencilSize];
	Pixel16 *   dataPtr = data;
	sint32      bottom  = k_TILE_PIXEL_HEIGHT-1;

	for (i=0; i<k_TILE_PIXEL_HEIGHT; i++) {
		accum = g_bitsTable[bottom-i];
		rowIndex = 0;
		for(j=0; j<32; j++) {
			if (accum & 0x00000001) {
				off = g_offsets[(bottom-i)*32 + rowIndex];

				pix = sourceStencil[off];

				*dataPtr = pix;
				dataPtr++;
				rowIndex++;
			}
			accum >>= 1;
		}
	}

	return data;
}

Pixel16 *tileutils_MakeTransition2(Pixel16 *sourceStencil)
{
	sint32		i,j,k;
	Pixel16		pix;
	uint32		accum;
	uint16		off;
	sint32		rowIndex;

	Pixel16	*   data    = new Pixel16[g_stencilSize];
	Pixel16 *   dataPtr = data;
	sint32      bottom  = k_TILE_PIXEL_HEIGHT-1;

	for (i=0; i<k_TILE_PIXEL_HEIGHT; i++) {
		accum = g_bitsTable[bottom-i];
		
		for(k=0; k<32; k++) {
			if (g_offsets[(bottom-i)*32+k] == 0xFFFF) {
				k--;
				break;
			}
		}
		rowIndex = k;

		for(j=0; j<32; j++) {
			if (accum & 0x00000001) {
				off = g_offsets[(bottom-i)*32 + rowIndex];

				pix = sourceStencil[off];

				*dataPtr = pix;
				dataPtr++;
				rowIndex--;
			}
			accum >>= 1;
		}
	}

	return data;
}

Pixel16 *tileutils_MakeTransition3(Pixel16 *sourceStencil)
{
	sint32		i,j,k;
	Pixel16		pix;
	uint32		accum;
	uint16		off;
	sint32		rowIndex;

	Pixel16 *   data    = new Pixel16[g_stencilSize];
	Pixel16 *   dataPtr = data;

	for (i=0; i<k_TILE_PIXEL_HEIGHT; i++) {
		accum = g_bitsTable[i];
		
		for(k=0; k<32; k++) {
			if (g_offsets[i*32+k] == 0xFFFF) {
				k--;
				break;
			}
		}
		rowIndex = k;

		for(j=0; j<32; j++) {
			if (accum & 0x00000001) {
				off = g_offsets[i*32 + rowIndex];

				pix = sourceStencil[off];

				*dataPtr = pix;
				dataPtr++;
				rowIndex--;
			}
			accum >>= 1;
		}
	}

	return data;
}

void tileutils_DumpAllTransitions(MBCHAR *filename, Pixel16 *t0, Pixel16 *t1, Pixel16 *t2, Pixel16 *t3)
{
	uint32		accum;
	sint32		i,j;
	sint32		index;
	FILE *      file = fopen(filename, "w");


	fprintf(file, "\n Transition 0\n\n");
	for (i=0; i<48; i++) {
		accum = g_bitsTable[i];

		index = 0;
		for (j=0; j<32; j++) {
			if (accum & 1) {
				fprintf(file, "\t%#.4x", t0[g_offsets[i*32+index]]);
				index++;
			}
			accum>>=1;
		}
		fprintf(file,"\n");
	}

	fprintf(file, "\n Transition 1\n\n");
	for (i=0; i<48; i++) {
		accum = g_bitsTable[47-i];

		index = 0;
		for (j=0; j<32; j++) {
			if (accum & 1) {
				fprintf(file, "\t%#.4x", t1[g_offsets[(47-i)*32+index]]);
				index++;
			}
			accum>>=1;
		}
		fprintf(file,"\n");
	}

	fprintf(file, "\n Transition 2\n\n");
	for (i=0; i<48; i++) {
		accum = g_bitsTable[47-i];


		index = 0;
		for (j=0; j<32; j++) {
			if (accum & 1) {
				fprintf(file, "\t%#.4x", t2[g_offsets[(47-i)*32+index]]);
				index++;
			}
			accum>>=1;
		}
		fprintf(file,"\n");
	}

	fprintf(file, "\n Transition 3\n\n");
	for (i=0; i<48; i++) {
		accum = g_bitsTable[i];

		index = 0;
		for (j=0; j<32; j++) {
			if (accum & 1) {
				fprintf(file, "\t%#.4x", t3[g_offsets[i*32+index]]);
				index++;
			}
			accum>>=1;
		}
		fprintf(file,"\n");
	}

	fclose(file);
}


sint32 tileutils_ExtractStencils(sint16 fromType, sint16 toType)
{
	
	MBCHAR		ageChar;
	MBCHAR		filename[_MAX_PATH];

	
	sprintf(filename, "gtft%.2d%.2d.tif", fromType, toType);
	ageChar = 'f';

	
	char	*tif;
	uint16	width=0, height=0;

	MBCHAR		fname[_MAX_PATH];
	sprintf(fname, "source" FILE_SEP "xitions" FILE_SEP "%s", filename);

	tif = tileutils_TIF2mem(fname, &width, &height);
	Assert(tif != NULL);
	if (tif == NULL) {
		printf("\n*** Could not find '%s'.\n", fname);
		exit(-1);
	}
	
	
	g_transitions[fromType][toType][0] = tileutils_ExtractUpperRight(tif, width, height, 0, 48);
	g_transitions[fromType][toType][1] = tileutils_MakeTransition1(g_transitions[fromType][toType][0]);
	g_transitions[fromType][toType][2] = tileutils_MakeTransition2(g_transitions[fromType][toType][0]);
	g_transitions[fromType][toType][3] = tileutils_MakeTransition3(g_transitions[fromType][toType][0]);

















	
	g_transitions[toType][fromType][0] = tileutils_ExtractLowerLeft(tif, width, height, 48, 24);
	g_transitions[toType][fromType][1] = tileutils_MakeTransition1(g_transitions[toType][fromType][0]);
	g_transitions[toType][fromType][2] = tileutils_MakeTransition2(g_transitions[toType][fromType][0]);
	g_transitions[toType][fromType][3] = tileutils_MakeTransition3(g_transitions[toType][fromType][0]);













	if (tif)
		free (tif);


	return 0;
}

uint16 *tileutils_GenerateAllWaterTable(uint16 width, uint16 height, uint16 x, uint16 y)
{
	uint16		*waterTable = new uint16[(height-y) * 2];
	uint16		i;
	uint16		start, end;

	for (i=y; i<height; i++) {
		if ((i-y)<=23) {
			start = (x + (23-(i-y))*2);
			end = width - start - 1;
		} else {
			start = (x + ((i-y)-24)*2);
			end = width - start - 1;
		}

		waterTable[(i-y)*2] = start;
		waterTable[(i-y)*2+1] = end;
	}

	return waterTable;
}

uint16 *tileutils_ExtractWaterTable(Pixel32 *image, uint16 width, uint16 height, uint16 x, uint16 y)
{
	uint16		*waterTable = new uint16[(height-y) * 2];
	BOOL		anyWater = FALSE;
	Pixel16		r,g,b,a;
	uint16		start, end;
	uint16		i,j;

	for (i=y; i<height; i++) {
		start = width;
		end = 0;
		for (j=0; j<width; j++) {
			RGB32Components(image[width * i + j], &r, &g, &b, &a);
			if (a > 0) {
				
				if (j < start)
					start = j;

				if (j > end)
					end = j;
			}
		}
		waterTable[(i-y)*2] = start;
		waterTable[(i-y)*2+1] = end;

		if (!anyWater && end != 0) 
			anyWater = TRUE;
	}

	if (!anyWater) {
		delete[] waterTable;

		return NULL;
	}

	return waterTable;
}


BaseTile	*g_baseTiles[k_MAX_BASE_TILES];




void tileutils_BorkifyTile(uint16 tileNum, MBCHAR ageChar, uint16 baseType, BOOL useT0, BOOL useT1, BOOL useT2, BOOL useT3)
{
	sint32		x,y;
	sint32		startX, endX;

	Pixel16		borkPix;
	sint32		yoffset = 24;
	uint32		accumList[k_TILE_PIXEL_HEIGHT][3];
	sint32		accumIndex = 0;
	sint32		accumCount;
	uint32		accum = 0;
	uint16		width, height;


	Pixel16 *   bork = tileutils_CreateBorkBork();

	MBCHAR		filename[_MAX_PATH];
	sprintf(filename, "source" FILE_SEP "basetiles" FILE_SEP "GT%cB%.4d.tif", ageChar, tileNum);

	char		*tif;
	if (baseType == TERRAIN_WATER_BEACH) {
		
		tif = tileutils_StripTIF2Mem(filename, &width, &height);
	} else {
		tif = tileutils_TIF2mem(filename, &width, &height);
	}




	Assert(tif != NULL);
	if (tif == NULL) {
		printf("\n*** Could not find '%s'.\n", filename);
		exit(-1);
	}

	Pixel16 *   tileImage = RGB32ToRGB16(tif, width, height);

	for (sint32 i=0; i<k_TILE_PIXEL_HEIGHT; i++) {
		accumList[i][0] = 0;
		accumList[i][1] = 0;
		accumList[i][2] = 0;
	}

	uint32      tileDataLen = (k_TILE_PIXEL_WIDTH * k_TILE_PIXEL_HEIGHT)/2 + k_TILE_PIXEL_HEIGHT;
	Pixel16 *   tileData    = new Pixel16[tileDataLen];
	Pixel16 *   tileDataPtr = tileData;

	sint32 len = 0;
	for (y=0; y<k_TILE_PIXEL_HEIGHT; y++) {
		if (y<=23) {
			startX = (23-y)*2;
			endX = k_TILE_PIXEL_WIDTH - startX;
		} else {
			startX = (y-24)*2;
			endX = k_TILE_PIXEL_WIDTH - startX;
		}

		accum = 0;
		accumIndex = 0;
		accumCount = 0;
		for (x = startX; x < endX; x++) {

			accum >>= 1;
			borkPix = bork[y*k_TILE_PIXEL_WIDTH+x];

			switch (borkPix) {
			case 0: if (!useT0) 
						borkPix = 0xFFFF; 
				break;
			case 1: if (!useT1) 
						borkPix = 0xFFFF; 
				break;
			case 2: if (!useT2) 
						borkPix = 0xFFFF; 
				break;
			case 3: if (!useT3) 
						borkPix = 0xFFFF; 
				break;
			}

			if (borkPix == 0xFFFF) {
				
				Pixel16 pix = tileImage[(y+yoffset)*k_TILE_PIXEL_WIDTH+x];

				
				
				if (pix == 0x0000 || pix == 0x0001 || pix == 0x0002 || pix == 0x0003) {
					pix = pix | 0x0820;
				}

				*tileDataPtr++ = pix;
				len++;
			} else {
				
				*tileDataPtr++ = borkPix;
				len++;
				accum |= 0x80000000;
			}

			accumCount++;

			if (accumCount >= 32) {
				accumList[y][accumIndex] = accum;
				accum = 0;
				accumIndex++;
				Assert(accumIndex<3);
				accumCount = 0;
			}
		}
		Assert(accumIndex<3);
		accum >>= 32-(endX-startX)-1;
		accumList[y][accumIndex] = accum;
	}
 
	BaseTile	*baseTile = new BaseTile;

	baseTile->SetBaseType((uint8)g_theTerrainDB->Get(baseType)->GetTilesetIndex());
	

	
	baseTile->SetTransitionFlag(0, useT0);
	baseTile->SetTransitionFlag(1, useT1);
	baseTile->SetTransitionFlag(2, useT2);
	baseTile->SetTransitionFlag(3, useT3);

	





	Assert(len >= 0);
	baseTile->SetTileDataLen(static_cast<uint16>(len*2));

	
	Pixel16		*dataCopy = new Pixel16[len];
	memcpy(dataCopy, tileData, len*2);
	baseTile->SetTileData(dataCopy);	

	
	
	
	uint16		*waterTable;
	uint32		waterTableLen = k_TILE_PIXEL_HEIGHT * 2 * sizeof(uint16);

	
	if (baseType == TERRAIN_WATER_BEACH) {
		
		waterTable = tileutils_ExtractWaterTable((Pixel32 *)tif, width, height, 0, 24);
	} else {
		if (g_theTerrainDB->Get(baseType)->GetMovementTypeSea() ||
			g_theTerrainDB->Get(baseType)->GetMovementTypeShallowWater()) {
			
			waterTable = tileutils_GenerateAllWaterTable(width, height, 0, 24);
		} else {
			
			waterTable = NULL;
			waterTableLen = 0;
		}
	}




	
	
	
	char		*hatTif=NULL;
	Pixel16		*hatData=NULL;
	uint32		hatDataLen=0;

	sprintf(filename, "source" FILE_SEP "hats" FILE_SEP "GTFh%.4d.tif", tileNum);
	hatTif = tileutils_TIF2mem(filename, &width, &height);
	if (hatTif) {
		hatData = (Pixel16 *)tileutils_EncodeTile((Pixel32 *)hatTif, width, height, &hatDataLen);
		free(hatTif);
		hatTif = NULL;
	}

	baseTile->SetHatDataLen((uint16)hatDataLen);
	baseTile->SetHatData(hatData);

	g_baseTiles[tileNum] = baseTile;

	delete[] bork;

	if (tif)
		free(tif);
	
	if (tileImage)
		free(tileImage);
}

uint16 tileutils_CompileImprovements(FILE *file)
{
	sint32		i;
	MBCHAR		filename[_MAX_PATH];
	uint32		dataLen;
	char		*tif;
	uint16		width, height, id;
	Pixel16		*data;
	uint16		count = 0;
#ifdef WIN32
	struct _stat tmpstat;
#else
	struct stat tmpstat;
#endif
	int			r;

	for (i=0; i<k_MAX_IMPROVEMENTS; i++) {
		sprintf(filename, "source%simprove%sGTFM%.3d.tif",
		        FILE_SEP, FILE_SEP, i);
#ifdef WIN32
		r = (sint32)_stat(filename, &tmpstat);
#else
		r = stat(filename, &tmpstat);
#endif
		if (r == 0) count++;
	}

	fwrite((void *)&count, 1, sizeof(uint16), file);

	for (i=0; i<k_MAX_IMPROVEMENTS; i++) {
		sprintf(filename, "source%simprove%sGTFM%.3d.tif",
		        FILE_SEP, FILE_SEP, i);
		tif = tileutils_TIF2mem(filename, &width, &height);
		if (tif) {
			data = (Pixel16 *)tileutils_EncodeTile((Pixel32 *)tif, width, height, &dataLen);


			free(tif);
			tif = NULL;

			id = (uint16)i;

			
			fwrite((void *)&id, 1, sizeof(uint16), file);

			
			fwrite((void *)&dataLen, 1, sizeof(uint32), file);

			
			fwrite((void *)data, 1, dataLen, file);
		}
	}

	return count;
}

void tileutils_EncodeTileset(MBCHAR *filename)
{
	sint32		i, j, k;

	
	for (i=0; i<TERRAIN_MAX; i++) {
		for (j=0; j<TERRAIN_MAX; j++) {
			for(k=0; k<k_TRANSITIONS_PER_TILE; k++) {
				g_transitions[i][j][k] = NULL;
			}
		}
	}

	
	for (i=0; i<k_MAX_BASE_TILES; i++) {
		g_baseTiles[i] = NULL;
	}

	tileutils_ParseTileset(filename);
}

sint32 tileutils_ParseTileset(MBCHAR *filename)
{
	MBCHAR			scriptName[_MAX_PATH];
	BOOL			done = FALSE;
	sint16			*transforms[k_MAX_TRANSFORMS];
	sint16			*riverTransforms[k_MAX_RIVER_TRANSFORMS];
	Pixel16			*riverData[k_MAX_RIVER_TRANSFORMS];
	uint32			riverDataLen[k_MAX_RIVER_TRANSFORMS];
	uint16			megaTileLengths[k_MAX_MEGATILES];
	MegaTileStep	megaTileData[k_MAX_MEGATILES][k_MAX_MEGATILE_STEPS];

	sint32			i,j,k;
	MBCHAR			ageChar = 'f';
	uint16			numRiverTransforms = 0;
	uint16			numTransforms = 0;
	uint16			numTransitions = 0;
	uint16			numBaseTiles = 0;
	uint16			numMegaTiles = 0;
	uint16			numImprovements = 0;

	sprintf(scriptName, "%s", filename);

	printf("\nParsing Tileset Script: '%s'\n", scriptName);

	Token *         theToken = new Token(scriptName, C3DIR_SPRITES); 
	Assert(theToken); 
	
	if (!theToken) return FALSE; 
	
	sint32 tmp; 

	while (!done) {
		switch (theToken->Next()) {
		case TOKEN_TILESET_TILE :
			{

				theToken->Next();
				theToken->GetNumber(tmp);

				sint32	tileNum = tmp;
				sint32	hasTransition[4];

				if (!token_ParseAnOpenBraceNext(theToken)) return FALSE;

				
				if (!token_ParseValNext(theToken, TOKEN_TILESET_TILE_BASE_TYPE, tmp)) return FALSE;
				sint32 baseType = tmp;

				
				if (!token_ParseValNext(theToken, TOKEN_TILESET_TILE_TRANS_0, tmp)) return FALSE;
				hasTransition[0] = tmp;
				if (!token_ParseValNext(theToken, TOKEN_TILESET_TILE_TRANS_1, tmp)) return FALSE;
				hasTransition[1] = tmp;
				if (!token_ParseValNext(theToken, TOKEN_TILESET_TILE_TRANS_2, tmp)) return FALSE;
				hasTransition[2] = tmp;
				if (!token_ParseValNext(theToken, TOKEN_TILESET_TILE_TRANS_3, tmp)) return FALSE;
				hasTransition[3] = tmp;

				printf(" * Basetile %.3d\n", tileNum);

				sint32 i;
				for(i = 0; i < g_theTerrainDB->NumRecords(); i++) {
					if(g_theTerrainDB->Get(i)->GetTilesetIndex() == baseType) {
						baseType = i;
						break;
					}
				}
				
				tileutils_BorkifyTile((uint16)tileNum, ageChar, (uint16)baseType, hasTransition[0], hasTransition[1], hasTransition[2], hasTransition[3]);

				
				if(tileNum < 100 && 
				   (hasTransition[0] || hasTransition[1] || hasTransition[2] || hasTransition[3])) {
					tileutils_BorkifyTile((uint16)(tileNum * 100) + 99, ageChar, (uint16)baseType, 0, 0, 0, 0);
				}

				if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;

				numBaseTiles++;
			}
			break;
		case TOKEN_TILESET_MEGATILE: 
			{
				MBCHAR		configStr[100];

				if (!token_ParseAnOpenBraceNext(theToken)) return FALSE; 
				
				
				if (!token_ParseKeywordNext(theToken, TOKEN_TILESET_MEGATILE_CONFIG)) return FALSE;

				theToken->Next();
				theToken->GetString(configStr);

				uint16			len;
				
				len = strlen(configStr);

				megaTileLengths[numMegaTiles] = len;

				for (i=0; i<len; i++) {

					if (!token_ParseKeywordNext(theToken, TOKEN_TILESET_MEGATILE_INFO)) return FALSE;

					uint8		dir = 0;

					switch (configStr[i]) {
					case k_MEGATILE_DIRECTION_CHAR_X : dir = k_MEGATILE_DIRECTION_X; break;
					case k_MEGATILE_DIRECTION_CHAR_N : dir = k_MEGATILE_DIRECTION_N; break;
					case k_MEGATILE_DIRECTION_CHAR_E : dir = k_MEGATILE_DIRECTION_E; break;
					case k_MEGATILE_DIRECTION_CHAR_S : dir = k_MEGATILE_DIRECTION_S; break;
					case k_MEGATILE_DIRECTION_CHAR_W : dir = k_MEGATILE_DIRECTION_W; break;
					default:
						Assert(FALSE);
					}

					sint32			terrainType;

					theToken->Next();
					theToken->GetNumber(terrainType);

					sint32			tileNum;

					theToken->Next();
					theToken->GetNumber(tileNum);

					MegaTileStep	step;

					step.direction = dir;
					step.terrainType = terrainType;
					step.tileNum = tileNum;

					megaTileData[numMegaTiles][i] = step;
				}

				printf(" * Supertile %s\n", configStr);

				if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;

				numMegaTiles++;
			}
			break;
		case TOKEN_TILESET_TRANSFORM:
			{
				theToken->Next();
				theToken->GetNumber(tmp);
//				sint32 transformNum = tmp;

				if (!token_ParseAnOpenBraceNext(theToken)) return FALSE; 

				sint16	*transform = new sint16[k_TRANSFORM_SIZE];

				
				for (i=0; i<k_TRANSFORM_SIZE; i++) 
					transform[i] = k_TRANSFORM_TO_LIST_ID;

				
				for (i=0; i<k_TRANSFORM_TO_INDEX; i++) {
					theToken->Next();
					theToken->GetNumber(tmp);
					transform[i] = (sint16)tmp;
				}
				
				
				switch (theToken->Next()) {
					
					case TOKEN_TILESET_TRANSFORM_TO :
							theToken->Next();
							theToken->GetNumber(tmp);
							
							transform[k_TRANSFORM_TO_INDEX] = (sint16)tmp;
						break;

					
					case TOKEN_TILESET_TRANSFORM_TO_LIST :							
							
							
							transform[k_TRANSFORM_TO_INDEX] = k_TRANSFORM_TO_LIST_ID;

							
							for (i=0; i<k_MAX_TRANSFORM_TO_LIST; i++) {
								theToken->Next();
								theToken->GetNumber(tmp);
								transform[k_TRANSFORM_TO_LIST_FIRST+i] = (sint16)tmp;
							}
						break;
				}


				transforms[numTransforms] = transform;

				if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;

				printf(" * Transform %.3d\n", numTransforms);

				numTransforms++;
				Assert (numTransforms < k_MAX_TRANSFORMS);
			}
			break;

		case TOKEN_TILESET_TRANSITION:
			{
				sint16		fromType, toType;

				if (!token_ParseAnOpenBraceNext(theToken)) return FALSE;

				if (!token_ParseValNext(theToken, TOKEN_TILESET_TRANSITION_FROM, tmp)) return FALSE;
				fromType = (sint16)tmp;

				if (!token_ParseValNext(theToken, TOKEN_TILESET_TRANSITION_TO, tmp)) return FALSE;
				toType = (sint16)tmp;

				if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;

				tileutils_ExtractStencils(fromType, toType);

				printf(" * Transition %.2d->%.2d\n", fromType, toType);
				
				numTransitions++;
			}
			break;

		case TOKEN_TILESET_RIVER_TRANSFORM:
			{
				if (!token_ParseAnOpenBraceNext(theToken)) return FALSE;

				sint16	*transform = new sint16[k_RIVER_TRANSFORM_SIZE];
				for (i=0; i<k_RIVER_TRANSFORM_SIZE-1; i++) {
					theToken->Next();
					theToken->GetNumber(tmp);
					transform[i] = (sint16)tmp;
				}

				if (!token_ParseValNext(theToken, TOKEN_TILESET_RIVER_PIECE, tmp)) return FALSE;
				transform[k_RIVER_TRANSFORM_SIZE-1] = (sint16)tmp;

				if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;
				
				MBCHAR		filename[_MAX_PATH];
				char		*tif;
				uint16		width, height;

				sprintf(filename, "source" FILE_SEP "rivers" FILE_SEP "GTFL%.2d.tif", tmp);
				tif = tileutils_TIF2mem(filename, &width, &height);

				uint32		dataLen=0;

				if (tif) riverData[numRiverTransforms] = (Pixel16 *)tileutils_EncodeTile((Pixel32 *)tif, width, height, &dataLen);
				else riverData[numRiverTransforms] = NULL;
				
				if (tif) free(tif);

				riverDataLen[numRiverTransforms] = dataLen;

				riverTransforms[numRiverTransforms] = transform;

				numRiverTransforms++;
			}
			break;

		case TOKEN_TILESET_END :
			done = TRUE;
			break;
		}
	}

	delete theToken;

	
	MBCHAR	fname[_MAX_PATH];

	printf("\nWriting Tileset: ");

	if (g_is565Format) {
		sprintf(fname, "output" FILE_SEP "gtset565.til");
	} else {
		sprintf(fname, "output" FILE_SEP "gtset555.til");
	}

	FILE *  tfile = fopen(fname, "wb");
	if (tfile) {
		
		fwrite((void *)&numTransforms, 1, sizeof(uint16), tfile);
		
		
		if (numTransforms > 0) {
			
			for (i=0; i<numTransforms; i++) {
				fwrite((void *)transforms[i], 1, sizeof(sint16)*k_TRANSFORM_SIZE, tfile);

				delete[] transforms[i];
				transforms[i] = NULL;
			}

			printf("...Transforms");
		}



		
		if (numTransitions > 0) {
			uint32		transitionCount = 0;
			uint32		transitionSize = 0;

			
			for (i=0; i<TERRAIN_MAX; i++) {
				for (j=0; j<TERRAIN_MAX; j++) {
					if (g_transitions[i][j][0] != NULL)
						transitionCount++;
				}
			}

			
			fwrite((void *)&transitionCount, 1, sizeof(uint32), tfile);

			
			transitionSize = g_stencilSize * sizeof(Pixel16);
			fwrite((void *)&transitionSize, 1, sizeof(uint32), tfile);

			
			for (i=0; i<TERRAIN_MAX; i++) {
				for (j=0; j<TERRAIN_MAX; j++) {
					sint16 from, to;

					if (g_transitions[i][j][0] != NULL) {
						from = (sint16)i;
						to = (sint16)j;

						fwrite((void *)&from, 1, sizeof(sint16), tfile);
						fwrite((void *)&to, 1, sizeof(sint16), tfile);

						for(k=0; k<k_TRANSITIONS_PER_TILE; k++) {
							fwrite((void *)g_transitions[i][j][k], 1, transitionSize, tfile);
						}
					}
				}
			}
			printf("...Transitions");
		}

		
		if (numBaseTiles > 0) {
			uint32		baseTileCount = 0;

			
			for (i=0; i<k_MAX_BASE_TILES; i++) {
				if (g_baseTiles[i] != NULL)
					baseTileCount++;
			}

			fwrite((void *)&baseTileCount, 1, sizeof(uint32), tfile);

			for (i=0; i<k_MAX_BASE_TILES; i++) {
				if (g_baseTiles[i] != NULL) {
					uint16		tNum = (uint16)i;
					uint8		baseType;
					BOOL		use;

					Pixel16		*tileData;

					fwrite((void *)&tNum, 1, sizeof(uint16), tfile);

					baseType = (uint8)g_baseTiles[i]->GetBaseType();
					fwrite((void *)&baseType, 1, sizeof(uint8), tfile);

					uint8 flag = 0;

					
					for(j=0; j<k_TRANSITIONS_PER_TILE; j++) {
						use = (BOOL)g_baseTiles[i]->GetTransitionFlag((uint16)j);
						if (use)
							flag |= (1 << j);

					}

					fwrite((void *)&flag, 1, sizeof(uint8), tfile);

					


					


					
					uint16 len16 = g_baseTiles[i]->GetTileDataLen();
					fwrite((void *)&len16, 1, sizeof(uint16), tfile);

					
					tileData = g_baseTiles[i]->GetTileData();
					fwrite((void *)tileData, 1, len16, tfile);

					








				

					



					len16 = (uint16)g_baseTiles[i]->GetHatDataLen();
					fwrite((void *)&len16, 1, sizeof(uint16), tfile);

					if (len16 > 0) {
						fwrite((void *)g_baseTiles[i]->GetHatData(), 1, len16, tfile);
					}
					
					
					delete g_baseTiles[i];
					g_baseTiles[i] = NULL;
				}
			}
			printf("...Base Tiles");
		}

		
		
		fwrite((void *)&numRiverTransforms, 1, sizeof(uint16), tfile);
		
		
		if (numRiverTransforms > 0) {
			
			for (i=0; i<numRiverTransforms; i++) {
				fwrite((void *)riverTransforms[i], 1, sizeof(sint16)*k_RIVER_TRANSFORM_SIZE, tfile);

				fwrite((void *)&riverDataLen[i], 1, sizeof(uint32), tfile);

				if (riverDataLen[i] > 0) {
					fwrite((void *)riverData[i], 1, riverDataLen[i], tfile);
				}

				delete[] riverTransforms[i];
				delete[] riverData[i];

				riverTransforms[i] = NULL;
				riverData[i] = NULL;
			}

			printf("...River Transforms");
		}

		
		printf("...Improvements");
		numImprovements = tileutils_CompileImprovements(tfile);
		
		
		if (numMegaTiles > 0) {
			uint16		num = (uint16)numMegaTiles;

			
			fwrite((void *)&num, 1, sizeof(uint16), tfile);
			for (i=0; i<numMegaTiles; i++) {
				uint16		len = megaTileLengths[i];

				fwrite((void *)&len, 1, sizeof(uint16), tfile);

				fwrite((void *)&megaTileData[i], 1, sizeof(MegaTileStep)*len, tfile);
			}

			printf("...MegaTiles");
		}

		printf("\n\n");

		printf(" * Base Tiles:    %d\n", numBaseTiles);
		printf(" * Transitions:   %d\n", numTransitions);
		printf(" * Transforms:    %d\n", numTransforms);
		printf(" * River Xforms:  %d\n", numRiverTransforms);
		printf(" * MegaTiles:     %d\n", numMegaTiles);
		printf(" * Improvements:  %d\n", numImprovements);

		fclose(tfile);
	}
	return TRUE;
	
}

