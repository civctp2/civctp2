#pragma once


























#ifndef _TIFFIO_
#define	_TIFFIO_




#include "tiff.h"








#define	TIFFLIB_VERSION	19970127	





typedef	struct tiff TIFF;



















typedef	uint32 ttag_t;		
typedef	uint16 tdir_t;		
typedef	uint16 tsample_t;	
typedef	uint32 tstrip_t;	
typedef uint32 ttile_t;		
typedef	int32 tsize_t;		
typedef	void* tdata_t;		
typedef	int32 toff_t;		

#if !defined(__WIN32__) && (defined(_WIN32) || defined(WIN32))
#define __WIN32__
#endif
#if defined(_WINDOWS) || defined(__WIN32__) || defined(_Windows)
#include <windows.h>
#ifdef __WIN32__
DECLARE_HANDLE(thandle_t);	
#else
typedef	HFILE thandle_t;	
#endif
#else
typedef	void* thandle_t;	
#endif

#ifndef NULL
#define	NULL	0
#endif







#define	TIFFPRINT_NONE		0x0		
#define	TIFFPRINT_STRIPS	0x1		
#define	TIFFPRINT_CURVES	0x2		
#define	TIFFPRINT_COLORMAP	0x4		
#define	TIFFPRINT_JPEGQTABLES	0x100		
#define	TIFFPRINT_JPEGACTABLES	0x200		
#define	TIFFPRINT_JPEGDCTABLES	0x200		




typedef	unsigned char TIFFRGBValue;		
typedef struct _TIFFRGBAImage TIFFRGBAImage;










typedef void (*tileContigRoutine)
    (TIFFRGBAImage*, uint32*, uint32, uint32, uint32, uint32, int32, int32,
	unsigned char*);
typedef void (*tileSeparateRoutine)
    (TIFFRGBAImage*, uint32*, uint32, uint32, uint32, uint32, int32, int32,
	unsigned char*, unsigned char*, unsigned char*, unsigned char*);



typedef struct {				
	TIFFRGBValue* clamptab;			
	int*	Cr_r_tab;
	int*	Cb_b_tab;
	int32*	Cr_g_tab;
	int32*	Cb_g_tab;
	float	coeffs[3];			
} TIFFYCbCrToRGB;

struct _TIFFRGBAImage {
	TIFF*	tif;				
	int	stoponerr;			
	int	isContig;			
	int	alpha;				
	uint32	width;				
	uint32	height;				
	uint16	bitspersample;			
	uint16	samplesperpixel;		
	uint16	orientation;			
	uint16	photometric;			
	uint16*	redcmap;			
	uint16*	greencmap;
	uint16*	bluecmap;
						
	int	(*get)(TIFFRGBAImage*, uint32*, uint32, uint32);
	union {
	    void (*any)(TIFFRGBAImage*);
	    tileContigRoutine	contig;
	    tileSeparateRoutine	separate;
	} put;					
	TIFFRGBValue* Map;			
	uint32** BWmap;				
	uint32** PALmap;			
	TIFFYCbCrToRGB* ycbcr;			
};





#define	TIFFGetR(abgr)	((abgr) & 0xff)
#define	TIFFGetG(abgr)	(((abgr) >> 8) & 0xff)
#define	TIFFGetB(abgr)	(((abgr) >> 16) & 0xff)
#define	TIFFGetA(abgr)	(((abgr) >> 24) & 0xff)








typedef	int (*TIFFInitMethod)(TIFF*, int);
typedef struct {
	char*		name;
	uint16		scheme;
	TIFFInitMethod	init;
} TIFFCodec;

#include <stdio.h>
#include <stdarg.h>

#if defined(__cplusplus)
extern "C" {
#endif
typedef	void (*TIFFErrorHandler)(const char*, const char*, va_list);
typedef	tsize_t (*TIFFReadWriteProc)(thandle_t, tdata_t, tsize_t);
typedef	toff_t (*TIFFSeekProc)(thandle_t, toff_t, int);
typedef	int (*TIFFCloseProc)(thandle_t);
typedef	toff_t (*TIFFSizeProc)(thandle_t);
typedef	int (*TIFFMapFileProc)(thandle_t, tdata_t*, toff_t*);
typedef	void (*TIFFUnmapFileProc)(thandle_t, tdata_t, toff_t);
typedef	void (*TIFFExtendProc)(TIFF*); 

extern	const char* TIFFGetVersion(void);

extern	const TIFFCodec* TIFFFindCODEC(uint16);
extern	TIFFCodec* TIFFRegisterCODEC(uint16, const char*, TIFFInitMethod);
extern	void TIFFUnRegisterCODEC(TIFFCodec*);

extern	tdata_t _TIFFmalloc(tsize_t);
extern	tdata_t _TIFFrealloc(tdata_t, tsize_t);
extern	void _TIFFmemset(tdata_t, int, tsize_t);
extern	void _TIFFmemcpy(tdata_t, const tdata_t, tsize_t);
extern	int _TIFFmemcmp(const tdata_t, const tdata_t, tsize_t);
extern	void _TIFFfree(tdata_t);

extern	void TIFFClose(TIFF*);
extern	int TIFFFlush(TIFF*);
extern	int TIFFFlushData(TIFF*);
extern	int TIFFGetField(TIFF*, ttag_t, ...);
extern	int TIFFVGetField(TIFF*, ttag_t, va_list);
extern	int TIFFGetFieldDefaulted(TIFF*, ttag_t, ...);
extern	int TIFFVGetFieldDefaulted(TIFF*, ttag_t, va_list);
extern	int TIFFReadDirectory(TIFF*);
extern	tsize_t TIFFScanlineSize(TIFF*);
extern	tsize_t TIFFRasterScanlineSize(TIFF*);
extern	tsize_t TIFFStripSize(TIFF*);
extern	tsize_t TIFFVStripSize(TIFF*, uint32);
extern	tsize_t TIFFTileRowSize(TIFF*);
extern	tsize_t TIFFTileSize(TIFF*);
extern	tsize_t TIFFVTileSize(TIFF*, uint32);
extern	uint32 TIFFDefaultStripSize(TIFF*, uint32);
extern	void TIFFDefaultTileSize(TIFF*, uint32*, uint32*);
extern	int TIFFFileno(TIFF*);
extern	int TIFFGetMode(TIFF*);
extern	int TIFFIsTiled(TIFF*);
extern	int TIFFIsByteSwapped(TIFF*);
extern	int TIFFIsUpSampled(TIFF*);
extern	int TIFFIsMSB2LSB(TIFF*);
extern	uint32 TIFFCurrentRow(TIFF*);
extern	tdir_t TIFFCurrentDirectory(TIFF*);
extern	tdir_t TIFFNumberOfDirectories(TIFF*);
extern	uint32 TIFFCurrentDirOffset(TIFF*);
extern	tstrip_t TIFFCurrentStrip(TIFF*);
extern	ttile_t TIFFCurrentTile(TIFF*);
extern	int TIFFReadBufferSetup(TIFF*, tdata_t, tsize_t);
extern	int TIFFWriteBufferSetup(TIFF*, tdata_t, tsize_t);
extern	int TIFFLastDirectory(TIFF*);
extern	int TIFFSetDirectory(TIFF*, tdir_t);
extern	int TIFFSetSubDirectory(TIFF*, uint32);
extern	int TIFFUnlinkDirectory(TIFF*, tdir_t);
extern	int TIFFSetField(TIFF*, ttag_t, ...);
extern	int TIFFVSetField(TIFF*, ttag_t, va_list);
extern	int TIFFWriteDirectory(TIFF *);
#if defined(c_plusplus) || defined(__cplusplus)
extern	void TIFFPrintDirectory(TIFF*, FILE*, long = 0);
extern	int TIFFReadScanline(TIFF*, tdata_t, uint32, tsample_t = 0);
extern	int TIFFWriteScanline(TIFF*, tdata_t, uint32, tsample_t = 0);
extern	int TIFFReadRGBAImage(TIFF*, uint32, uint32, uint32*, int = 0);
#else
extern	void TIFFPrintDirectory(TIFF*, FILE*, long);
extern	int TIFFReadScanline(TIFF*, tdata_t, uint32, tsample_t);
extern	int TIFFWriteScanline(TIFF*, tdata_t, uint32, tsample_t);
extern	int TIFFReadRGBAImage(TIFF*, uint32, uint32, uint32*, int);
#endif
extern	int TIFFRGBAImageOK(TIFF*, char [1024]);
extern	int TIFFRGBAImageBegin(TIFFRGBAImage*, TIFF*, int, char [1024]);
extern	int TIFFRGBAImageGet(TIFFRGBAImage*, uint32*, uint32, uint32);
extern	void TIFFRGBAImageEnd(TIFFRGBAImage*);
extern	TIFF* TIFFOpen(const char*, const char*);
extern	TIFF* TIFFFdOpen(int, const char*, const char*);
extern	TIFF* TIFFClientOpen(const char*, const char*,
	    thandle_t,
	    TIFFReadWriteProc, TIFFReadWriteProc,
	    TIFFSeekProc, TIFFCloseProc,
	    TIFFSizeProc,
	    TIFFMapFileProc, TIFFUnmapFileProc);
extern	const char* TIFFFileName(TIFF*);
extern	void TIFFError(const char*, const char*, ...);
extern	void TIFFWarning(const char*, const char*, ...);
extern	TIFFErrorHandler TIFFSetErrorHandler(TIFFErrorHandler);
extern	TIFFErrorHandler TIFFSetWarningHandler(TIFFErrorHandler);
extern	TIFFExtendProc TIFFSetTagExtender(TIFFExtendProc);
extern	ttile_t TIFFComputeTile(TIFF*, uint32, uint32, uint32, tsample_t);
extern	int TIFFCheckTile(TIFF*, uint32, uint32, uint32, tsample_t);
extern	ttile_t TIFFNumberOfTiles(TIFF*);
extern	tsize_t TIFFReadTile(TIFF*,
	    tdata_t, uint32, uint32, uint32, tsample_t);
extern	tsize_t TIFFWriteTile(TIFF*,
	    tdata_t, uint32, uint32, uint32, tsample_t);
extern	tstrip_t TIFFComputeStrip(TIFF*, uint32, tsample_t);
extern	tstrip_t TIFFNumberOfStrips(TIFF*);
extern	tsize_t TIFFReadEncodedStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
extern	tsize_t TIFFReadRawStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
extern	tsize_t TIFFReadEncodedTile(TIFF*, ttile_t, tdata_t, tsize_t);
extern	tsize_t TIFFReadRawTile(TIFF*, ttile_t, tdata_t, tsize_t);
extern	tsize_t TIFFWriteEncodedStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
extern	tsize_t TIFFWriteRawStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
extern	tsize_t TIFFWriteEncodedTile(TIFF*, ttile_t, tdata_t, tsize_t);
extern	tsize_t TIFFWriteRawTile(TIFF*, ttile_t, tdata_t, tsize_t);
extern	void TIFFSetWriteOffset(TIFF*, toff_t);
extern	void TIFFSwabShort(uint16*);
extern	void TIFFSwabLong(uint32*);
extern	void TIFFSwabDouble(double*);
extern	void TIFFSwabArrayOfShort(uint16*, unsigned long);
extern	void TIFFSwabArrayOfLong(uint32*, unsigned long);
extern	void TIFFSwabArrayOfDouble(double*, unsigned long);
extern	void TIFFReverseBits(unsigned char *, unsigned long);
extern	const unsigned char* TIFFGetBitRevTable(int);
#if defined(__cplusplus)
}
#endif
#endif 
