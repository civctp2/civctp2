#pragma once


























#ifndef _TIFFIOP_
#define	_TIFFIOP_








#if defined(unix) || defined(__unix)
#include "port.h"
#include "tiffconf.h"
#else
#include "tiffconf.h"
#include "tiffcomp.h"
#endif
#include "tiffio.h"
#include "tif_dir.h"

#ifndef TRUE
#define	TRUE	1
#define	FALSE	0
#endif




typedef	unsigned char tidataval_t;	
typedef	tidataval_t* tidata_t;		

typedef	void (*TIFFVoidMethod)(TIFF*);
typedef	int (*TIFFBoolMethod)(TIFF*);
typedef	int (*TIFFPreMethod)(TIFF*, tsample_t);
typedef	int (*TIFFCodeMethod)(TIFF*, tidata_t, tsize_t, tsample_t);
typedef	int (*TIFFSeekMethod)(TIFF*, uint32);
typedef	void (*TIFFPostMethod)(TIFF*, tidata_t, tsize_t);
typedef	int (*TIFFVSetMethod)(TIFF*, ttag_t, va_list);
typedef	int (*TIFFVGetMethod)(TIFF*, ttag_t, va_list);
typedef	void (*TIFFPrintMethod)(TIFF*, FILE*, long);
typedef	uint32 (*TIFFStripMethod)(TIFF*, uint32);
typedef	void (*TIFFTileMethod)(TIFF*, uint32*, uint32*);

struct tiff {
	char*		tif_name;	
	int		tif_fd;		
	int		tif_mode;	
	uint32		tif_flags;
#define	TIFF_FILLORDER		0x0003	
#define	TIFF_DIRTYHEADER	0x0004	
#define	TIFF_DIRTYDIRECT	0x0008	
#define	TIFF_BUFFERSETUP	0x0010	
#define	TIFF_CODERSETUP		0x0020	
#define	TIFF_BEENWRITING	0x0040	
#define	TIFF_SWAB		0x0080	
#define	TIFF_NOBITREV		0x0100	
#define	TIFF_MYBUFFER		0x0200	
#define	TIFF_ISTILED		0x0400	
#define	TIFF_MAPPED		0x0800	
#define	TIFF_POSTENCODE		0x1000	
#define	TIFF_INSUBIFD		0x2000	
#define	TIFF_UPSAMPLED		0x4000	 
#define	TIFF_STRIPCHOP		0x8000	
	toff_t		tif_diroff;	
	toff_t		tif_nextdiroff;	
	TIFFDirectory	tif_dir;	
	TIFFHeader	tif_header;	
        tidata_t        tif_clientdir;  
	const int*	tif_typeshift;	
	const long*	tif_typemask;	
	uint32		tif_row;	
	tdir_t		tif_curdir;	
	tstrip_t	tif_curstrip;	
	toff_t		tif_curoff;	
	toff_t		tif_dataoff;	
#if SUBIFD_SUPPORT
	uint16		tif_nsubifd;	
	toff_t		tif_subifdoff;	
#endif

	uint32 		tif_col;	
	ttile_t		tif_curtile;	
	tsize_t		tif_tilesize;	

	TIFFBoolMethod	tif_setupdecode;
	TIFFPreMethod	tif_predecode;	
	TIFFBoolMethod	tif_setupencode;
	TIFFPreMethod	tif_preencode;	
	TIFFBoolMethod	tif_postencode;	
	TIFFCodeMethod	tif_decoderow;	
	TIFFCodeMethod	tif_encoderow;	
	TIFFCodeMethod	tif_decodestrip;
	TIFFCodeMethod	tif_encodestrip;
	TIFFCodeMethod	tif_decodetile;	
	TIFFCodeMethod	tif_encodetile;	
	TIFFVoidMethod	tif_close;	
	TIFFSeekMethod	tif_seek;	
	TIFFVoidMethod	tif_cleanup;	
	TIFFStripMethod	tif_defstripsize;
	TIFFTileMethod	tif_deftilesize;
	tidata_t	tif_data;	

	tsize_t		tif_scanlinesize;
	tsize_t		tif_scanlineskew;
	tidata_t	tif_rawdata;	
	tsize_t		tif_rawdatasize;
	tidata_t	tif_rawcp;	
	tsize_t		tif_rawcc;	

	tidata_t	tif_base;	
	toff_t		tif_size;	
	TIFFMapFileProc	tif_mapproc;	
	TIFFUnmapFileProc tif_unmapproc;

	thandle_t	tif_clientdata;	
	TIFFReadWriteProc tif_readproc;	
	TIFFReadWriteProc tif_writeproc;
	TIFFSeekProc	tif_seekproc;	
	TIFFCloseProc	tif_closeproc;	
	TIFFSizeProc	tif_sizeproc;	

	TIFFPostMethod	tif_postdecode;	

	TIFFFieldInfo**	tif_fieldinfo;	
	int		tif_nfields;	
	TIFFVSetMethod	tif_vsetfield;	
	TIFFVGetMethod	tif_vgetfield;	
	TIFFPrintMethod	tif_printdir;	
};

#define	isPseudoTag(t)	(t > 0xffff)	

#define	isTiled(tif)	(((tif)->tif_flags & TIFF_ISTILED) != 0)
#define	isMapped(tif)	(((tif)->tif_flags & TIFF_MAPPED) != 0)
#define	isFillOrder(tif, o)	(((tif)->tif_flags & (o)) != 0)
#define	isUpSampled(tif)	(((tif)->tif_flags & TIFF_UPSAMPLED) != 0)
#define	TIFFReadFile(tif, buf, size) \
	((*(tif)->tif_readproc)((tif)->tif_clientdata,buf,size))
#define	TIFFWriteFile(tif, buf, size) \
	((*(tif)->tif_writeproc)((tif)->tif_clientdata,buf,size))
#define	TIFFSeekFile(tif, off, whence) \
	((*(tif)->tif_seekproc)((tif)->tif_clientdata,(toff_t)(off),whence))
#define	TIFFCloseFile(tif) \
	((*(tif)->tif_closeproc)((tif)->tif_clientdata))
#define	TIFFGetFileSize(tif) \
	((*(tif)->tif_sizeproc)((tif)->tif_clientdata))
#define	TIFFMapFileContents(tif, paddr, psize) \
	((*(tif)->tif_mapproc)((tif)->tif_clientdata,paddr,psize))
#define	TIFFUnmapFileContents(tif, addr, size) \
	((*(tif)->tif_unmapproc)((tif)->tif_clientdata,addr,size))




#ifndef ReadOK
#define	ReadOK(tif, buf, size) \
	(TIFFReadFile(tif, (tdata_t) buf, (tsize_t) size) == (tsize_t) size)
#endif
#ifndef SeekOK
#define	SeekOK(tif, off) \
	(TIFFSeekFile(tif, (toff_t) off, SEEK_SET) == (toff_t) off)
#endif
#ifndef WriteOK
#define	WriteOK(tif, buf, size) \
	(TIFFWriteFile(tif, (tdata_t) buf, (tsize_t) size) == (tsize_t) size)
#endif


#define	TIFFhowmany(x, y) ((((uint32)(x))+(((uint32)(y))-1))/((uint32)(y)))
#define	TIFFroundup(x, y) (TIFFhowmany(x,y)*((uint32)(y)))

#if defined(__cplusplus)
extern "C" {
#endif
extern	int _TIFFgetMode(const char*, const char*);
extern	int _TIFFNoRowEncode(TIFF*, tidata_t, tsize_t, tsample_t);
extern	int _TIFFNoStripEncode(TIFF*, tidata_t, tsize_t, tsample_t);
extern	int _TIFFNoTileEncode(TIFF*, tidata_t, tsize_t, tsample_t);
extern	int _TIFFNoRowDecode(TIFF*, tidata_t, tsize_t, tsample_t);
extern	int _TIFFNoStripDecode(TIFF*, tidata_t, tsize_t, tsample_t);
extern	int _TIFFNoTileDecode(TIFF*, tidata_t, tsize_t, tsample_t);
extern	void _TIFFNoPostDecode(TIFF*, tidata_t, tsize_t);
extern	int _TIFFNoSeek(TIFF*, uint32);
extern	void _TIFFSwab16BitData(TIFF*, tidata_t, tsize_t);
extern	void _TIFFSwab32BitData(TIFF*, tidata_t, tsize_t);
extern	void _TIFFSwab64BitData(TIFF*, tidata_t, tsize_t);
extern	int TIFFFlushData1(TIFF*);
extern	void TIFFFreeDirectory(TIFF*);
extern	int TIFFDefaultDirectory(TIFF*);
extern	int TIFFSetCompressionScheme(TIFF*, int);
extern	int TIFFSetDefaultCompressionState(TIFF*);
extern	uint32 _TIFFDefaultStripSize(TIFF*, uint32);
extern	void _TIFFDefaultTileSize(TIFF*, uint32*, uint32*);

extern	void _TIFFsetByteArray(void**, void*, long);
extern	void _TIFFsetString(char**, char*);
extern	void _TIFFsetShortArray(uint16**, uint16*, long);
extern	void _TIFFsetLongArray(uint32**, uint32*, long);
extern	void _TIFFsetFloatArray(float**, float*, long);
extern	void _TIFFsetDoubleArray(double**, double*, long);

extern	void _TIFFprintAscii(FILE*, const char*);
extern	void _TIFFprintAsciiTag(FILE*, const char*, const char*);

GLOBALDATA(TIFFErrorHandler,_TIFFwarningHandler);
GLOBALDATA(TIFFErrorHandler,_TIFFerrorHandler);

extern	int TIFFInitDumpMode(TIFF*, int);
#ifdef PACKBITS_SUPPORT
extern	int TIFFInitPackBits(TIFF*, int);
#endif
#ifdef CCITT_SUPPORT
extern	int TIFFInitCCITTRLE(TIFF*, int), TIFFInitCCITTRLEW(TIFF*, int);
extern	int TIFFInitCCITTFax3(TIFF*, int), TIFFInitCCITTFax4(TIFF*, int);
#endif
#ifdef THUNDER_SUPPORT
extern	int TIFFInitThunderScan(TIFF*, int);
#endif
#ifdef NEXT_SUPPORT
extern	int TIFFInitNeXT(TIFF*, int);
#endif
#ifdef LZW_SUPPORT
extern	int TIFFInitLZW(TIFF*, int);
#endif
#ifdef OJPEG_SUPPORT
extern	int TIFFInitOJPEG(TIFF*, int);
#endif
#ifdef JPEG_SUPPORT
extern	int TIFFInitJPEG(TIFF*, int);
#endif
#ifdef JBIG_SUPPORT
extern	int TIFFInitJBIG(TIFF*, int);
#endif
#ifdef ZIP_SUPPORT
extern	int TIFFInitZIP(TIFF*, int);
#endif
#ifdef PIXARLOG_SUPPORT
extern	int TIFFInitPixarLog(TIFF*, int);
#endif
#ifdef LOGLUV_SUPPORT
extern	int TIFFInitSGILog(TIFF*, int);
#endif
#ifdef VMS
extern	const TIFFCodec _TIFFBuiltinCODECS[];
#else
extern	TIFFCodec _TIFFBuiltinCODECS[];
#endif

#if defined(__cplusplus)
}
#endif
#endif 
