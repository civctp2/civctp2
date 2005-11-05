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

#include "c3.h"

#include "tiffutils.h"
#include <tiffio.h>

char *tiffutils_LoadTIF(const char *filename, uint16 *width, uint16 *height, size_t *size)
{
	TIFF* tif = TIFFOpen(filename, "r");

	char        *destImage;

	if (tif)
	{
		uint32 w, h;
		size_t npixels;
		uint32* raster;

		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
		npixels = w * h;
		raster = (uint32 *) _TIFFmalloc(npixels * sizeof(uint32));
		if (raster != NULL)
		{
			if (TIFFReadRGBAImage(tif, w, h, raster, 0))
			{

				destImage = (char *)malloc(npixels * sizeof(uint32));
				if (!destImage) {
					_TIFFfree(raster);
					TIFFClose(tif);
					return NULL;
				}
				if (size)
					*size = npixels * sizeof(uint32);
				memcpy(destImage, raster, npixels * sizeof(uint32));

				_TIFFfree(raster);

				*width = (uint16)w;
				*height = (uint16)h;

				return (char *)destImage;
			}
		}
		TIFFClose(tif);
	}

	return NULL;
}


char *TIF2mem(const char *filename, uint16 *width, uint16 *height, size_t *size)
{
	TIFF    *tif = TIFFOpen(filename, "r");
	char    *image = NULL;
	uint32  w=0, h=0;

	if (tif) {
		size_t      npixels;
		char*       raster;


		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);


		npixels = w * h;


		sint32 bytesPerRow = w * 4;

		image = (char *)malloc(npixels * sizeof(uint32));
		if (image == NULL) {
			TIFFClose(tif);
			return NULL;
		}
		if (size)
			*size = npixels * sizeof(uint32);
		char *imagePtr = image;

		raster = (char *) _TIFFmalloc(npixels * sizeof(uint32));
		char *rasterPtr;

		if (raster != NULL) {
			if (TIFFReadRGBAImage(tif, w, h, (uint32 *)raster, 0)) {
				imagePtr = image;
				rasterPtr = raster + (bytesPerRow * (h-1));
				for (uint32 row = 0; row < h; row++) {
					memcpy(imagePtr, rasterPtr, bytesPerRow);
					imagePtr += bytesPerRow;
					rasterPtr -= bytesPerRow;
				}
			}

			_TIFFfree(raster);
		}

		TIFFClose(tif);
	}

	*width = (uint16)w;
	*height = (uint16)h;

	return image;
}


int TIFGetMetrics(const char *filename, uint16 *width, uint16 *height)
{
	TIFF    *tif = TIFFOpen(filename, "r");
	uint32  w=0, h=0;

	if (tif) {

		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);

		TIFFClose(tif);

		*width = (uint16)w;
		*height = (uint16)h;
	}

	return 0;
}

int TIFLoadIntoBuffer16(const char *filename, uint16 *width, uint16 *height, uint16 imageRowBytes, uint16 *buffer, BOOL is565)
{
	TIFF    *tif = TIFFOpen(filename, "r");
	uint32  w=0, h=0;
	sint32  i;

	if (tif) {
		size_t      npixels;
		char*       raster;


		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);


		npixels = w * h;


		sint32 bytesPerRow = w * 4;

		raster = (char *) _TIFFmalloc(npixels * sizeof(uint32));
		char *rasterPtr;
		char *imagePtr;

		if (raster != NULL) {
			if (TIFFReadRGBAImage(tif, w, h, (uint32 *)raster, 0)) {
				imagePtr = (char *)buffer;
				rasterPtr = raster + (bytesPerRow * (h-1));



				uint32 *rasterPtrCopy;
				uint16 *imagePtrCopy;
				uint32 pixel;

				if (is565) {
					for (uint32 row = 0; row < h; row++) {

						imagePtrCopy = (uint16 *)imagePtr;
						rasterPtrCopy = (uint32 *)rasterPtr;


						for (i=0; i<(sint32)w; i++) {
							pixel = *rasterPtrCopy++;
							*imagePtrCopy++ = (uint16)(((pixel & 0x000000F8) << 8) | ((pixel & 0x0000FC00) >> 5) | ((pixel & 0x00F80000) >> 19));
						}

						imagePtr += imageRowBytes;
						rasterPtr -= bytesPerRow;
					}
				} else {
					for (uint32 row = 0; row < h; row++) {

						imagePtrCopy = (uint16 *)imagePtr;
						rasterPtrCopy = (uint32 *)rasterPtr;


						for (i=0; i<(sint32)w; i++) {
							pixel = *rasterPtrCopy++;
							*imagePtrCopy++ = (uint16)(((pixel & 0x000000F8) << 7) | ((pixel & 0x0000F800) >> 6) | ((pixel & 0x00F80000) >> 19));
						}

						imagePtr += imageRowBytes;
						rasterPtr -= bytesPerRow;
					}
				}

			}

			_TIFFfree(raster);
		}

		TIFFClose(tif);
	}

	*width = (uint16)w;
	*height = (uint16)h;

	return 0;
}




char *StripTIF2Mem(const char *filename, uint16 *width, uint16 *height, size_t *size)
{
	uint32      imageLength;
	uint32      imageWidth;
	uint32      LineSize;
	uint32      RowsPerStrip;
	sint32      PhotometricInterpretation;
	TIFF        *tif;
	sint32      nrow;
	uint32      row;
	char        *buf;
	sint32      stripSize;
	char        *outBuf, *outBufPtr;
	sint32      l;

	tif = TIFFOpen(filename, "r");

	if (!tif)
	   return NULL;
	*width = static_cast<uint16>(-1);
	*height = static_cast<uint16>(-1);

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
	TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &RowsPerStrip);
	TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &RowsPerStrip);
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &PhotometricInterpretation);

	LineSize = TIFFScanlineSize(tif);



	stripSize = (sint32) TIFFStripSize(tif);
	buf = (char *)malloc(stripSize);
	outBuf = (char *)malloc(imageWidth * imageLength * 4);
	if (size)
		*size = imageWidth * imageLength * 4;
	outBufPtr = outBuf;




	for (row = 0; row < imageLength; row += RowsPerStrip)
	  {
		nrow = (row + RowsPerStrip > imageLength ? imageLength - row : RowsPerStrip);
		if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, row, 0), buf, nrow*LineSize)==-1) {
			return NULL;
		} else {
			for (l = 0; l < nrow; l++) {
				memcpy(outBufPtr, &buf[(sint32) (l*LineSize)], (sint32) imageWidth*4);
				outBufPtr += imageWidth * 4;
			}
		 }
	}

	free(buf);

	TIFFClose(tif);

	*width = (uint16)imageWidth;
	*height = (uint16)imageLength;

	return outBuf;
}

