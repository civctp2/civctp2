//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : tiff image format utilities
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Removed unused local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------


//#ifdef _TIFF_DATA_TYPEDEFS_
#define CONFIG_TELLS_TO_DEFINE_TIFF_INTTYPES 1
//#endif


#include "c3.h"
#ifdef CONFIG_TELLS_TO_DEFINE_TIFF_INTTYPES
typedef sint8 int8;
typedef sint16 int16;
typedef sint32 int32;
#endif

#include "tiffutils.h"
//#include <tiffio.h>

char *tiffutils_LoadTIF(const char *filename, uint16 *width, uint16 *height, size_t *size)
{
  printf("%s L%d: Code for loading TIFF removed (commented out)!\n", __FILE__, __LINE__);
	// TIFF * tif = TIFFOpen(filename, "r");
	// if (tif)
	// {
	// 	uint32 w, h;
	// 	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
	// 	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);

	// 	size_t   npixels = w * h;
	// 	uint32 * raster  = (uint32 *) _TIFFmalloc(npixels * sizeof(uint32));
	// 	if (raster)
	// 	{
	// 		if (TIFFReadRGBAImage(tif, w, h, raster, 0))
	// 		{
	// 			char * destImage = (char *)malloc(npixels * sizeof(uint32));
	// 			if (!destImage) {
	// 				_TIFFfree(raster);
	// 				TIFFClose(tif);
	// 				return NULL;
	// 			}
	// 			if (size)
	// 				*size = npixels * sizeof(uint32);
	// 			memcpy(destImage, raster, npixels * sizeof(uint32));

	// 			_TIFFfree(raster);

	// 			*width = (uint16)w;
	// 			*height = (uint16)h;

	// 			return destImage;
	// 		}
	// 	}
	// 	TIFFClose(tif);
	// }

	return NULL;
}


char *TIF2mem(const char *filename, uint16 *width, uint16 *height, size_t *size)
{
  printf("%s L%d: Code for loading TIFF removed (commented out)!\n", __FILE__, __LINE__);
        char    *image = NULL;
	// uint32  w=0, h=0;
	// TIFF    *tif = TIFFOpen(filename, "r");

	// if (tif) {
	// 	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
	// 	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);

	// 	size_t npixels     = w * h;

	// 	image = (char *)malloc(npixels * sizeof(uint32));
	// 	if (!image) {
	// 		TIFFClose(tif);
	// 		return NULL;
	// 	}

	// 	if (size)
	// 		*size = npixels * sizeof(uint32);

	// 	char * raster = (char *) _TIFFmalloc(npixels * sizeof(uint32));
	// 	if (raster) {
	// 		sint32 bytesPerRow = w * 4;
	// 		if (TIFFReadRGBAImage(tif, w, h, (uint32 *)raster, 0)) {
	// 			char * imagePtr  = image;
	// 			char * rasterPtr = raster + (bytesPerRow * (h-1));
	// 			for (uint32 row = 0; row < h; row++) {
	// 				memcpy(imagePtr, rasterPtr, bytesPerRow);
	// 				imagePtr += bytesPerRow;
	// 				rasterPtr -= bytesPerRow;
	// 			}
	// 		}

	// 		_TIFFfree(raster);
	// 	}

	// 	TIFFClose(tif);
	// }

	// *width = (uint16)w;
	// *height = (uint16)h;

	return image;
}


int TIFGetMetrics(const char *filename, uint16 *width, uint16 *height)
{
  printf("%s L%d: Code for loading TIFF removed (commented out)!\n", __FILE__, __LINE__);
    // 	TIFF *  tif = TIFFOpen(filename, "r");

    // 	if (tif) 
    // {
    // 	    uint32  w   = 0;
    //     uint32  h   = 0;

    // 		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    // 		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);

    // 		TIFFClose(tif);

    // 		*width = (uint16)w;
    // 		*height = (uint16)h;
    // 	}

	return 0;
}

int TIFLoadIntoBuffer16(const char *filename, uint16 *width, uint16 *height, uint16 imageRowBytes, uint16 *buffer, BOOL is565)
{
  printf("%s L%d: Code for loading TIFF removed (commented out)!\n", __FILE__, __LINE__);
	// uint32  w=0, h=0;  
    // 	TIFF    *tif = TIFFOpen(filename, "r");

    // 	if (tif) 
    // {
    // 		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    // 		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);

    // 		sint32  bytesPerRow = w * 4;
    // 		size_t  npixels     = w * h;
    // 		char *  raster      = (char *) _TIFFmalloc(npixels * sizeof(uint32));

    //     if (raster) 
    //     {
    // 			if (TIFFReadRGBAImage(tif, w, h, (uint32 *)raster, 0)) 
    //         {
    // 				char * imagePtr     = (char *)buffer;
    // 				char * rasterPtr    = raster + (bytesPerRow * (h-1));

    // 				uint32 *rasterPtrCopy;
    // 				uint16 *imagePtrCopy;
    // 				uint32 pixel;
    // 	                  sint32  i;

    // 				if (is565) {
    // 					for (uint32 row = 0; row < h; row++) {

    // 						imagePtrCopy = (uint16 *)imagePtr;
    // 						rasterPtrCopy = (uint32 *)rasterPtr;


    // 						for (i=0; i<(sint32)w; i++) {
    // 							pixel = *rasterPtrCopy++;
    // 							*imagePtrCopy++ = (uint16)(((pixel & 0x000000F8) << 8) | ((pixel & 0x0000FC00) >> 5) | ((pixel & 0x00F80000) >> 19));
    // 						}

    // 						imagePtr += imageRowBytes;
    // 						rasterPtr -= bytesPerRow;
    // 					}
    // 				} else {
    // 					for (uint32 row = 0; row < h; row++) {

    // 						imagePtrCopy = (uint16 *)imagePtr;
    // 						rasterPtrCopy = (uint32 *)rasterPtr;


    // 						for (i=0; i<(sint32)w; i++) {
    // 							pixel = *rasterPtrCopy++;
    // 							*imagePtrCopy++ = (uint16)(((pixel & 0x000000F8) << 7) | ((pixel & 0x0000F800) >> 6) | ((pixel & 0x00F80000) >> 19));
    // 						}

    // 						imagePtr += imageRowBytes;
    // 						rasterPtr -= bytesPerRow;
    // 					}
    // 				}

    // 			}

    // 			_TIFFfree(raster);
    // 		}

    // 		TIFFClose(tif);
    // 	}

    // 	*width = (uint16)w;
    // 	*height = (uint16)h;

	return 0;
}




char *StripTIF2Mem(const char *filename, uint16 *width, uint16 *height, size_t *size)
{
  printf("%s L%d: Code for loading TIFF removed (commented out)!\n", __FILE__, __LINE__);
	// TIFF *  tif = TIFFOpen(filename, "r");
	// if (!tif)
	//    return NULL;

	// *width = static_cast<uint16>(-1);
	// *height = static_cast<uint16>(-1);

	// uint32      imageLength;
	// uint32      imageWidth;
	// uint32      RowsPerStrip;
	// sint32      PhotometricInterpretation;

	// TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
	// TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
	// TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &RowsPerStrip);
	// TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &RowsPerStrip); /// @todo Check twice?
	// TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &PhotometricInterpretation);

	// tsize_t LineSize    = TIFFScanlineSize(tif);
	// tsize_t stripSize   = TIFFStripSize(tif);
	// char *  buf         = (char *)malloc(stripSize);
	// char *  outBuf      = (char *)malloc(imageWidth * imageLength * 4);
	// if (size)
	// 	*size = imageWidth * imageLength * 4;
	// char *  outBufPtr   = outBuf;

	// for (uint32 row = 0; row < imageLength; row += RowsPerStrip)
	// {
	// 	tsize_t nrow = (row + RowsPerStrip > imageLength ? imageLength - row : RowsPerStrip);
	// 	if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, row, 0), buf, nrow*LineSize)==-1) 
        // {
        //     /// @todo Check free(buf)?
	// 		return NULL;
	// 	} 
        // else 
        // {
	// 		for (tsize_t l = 0; l < nrow; l++) 
        //     {
	// 			memcpy(outBufPtr, &buf[l * LineSize], imageWidth * 4);
	// 			outBufPtr += imageWidth * 4;
	// 		}
	// 	 }
	// }

	// free(buf);
	// TIFFClose(tif);

	// *width  = (uint16) imageWidth;
	// *height = (uint16) imageLength;

	// return outBuf;
        return NULL;
}

