






#ifndef _ZCONF_H
#define _ZCONF_H





#ifdef Z_PREFIX
#  define deflateInit_	z_deflateInit_
#  define deflate	z_deflate
#  define deflateEnd	z_deflateEnd
#  define inflateInit_ 	z_inflateInit_
#  define inflate	z_inflate
#  define inflateEnd	z_inflateEnd
#  define deflateInit2_	z_deflateInit2_
#  define deflateSetDictionary z_deflateSetDictionary
#  define deflateCopy	z_deflateCopy
#  define deflateReset	z_deflateReset
#  define deflateParams	z_deflateParams
#  define inflateInit2_	z_inflateInit2_
#  define inflateSetDictionary z_inflateSetDictionary
#  define inflateSync	z_inflateSync
#  define inflateSyncPoint z_inflateSyncPoint
#  define inflateReset	z_inflateReset
#  define compress	z_compress
#  define compress2	z_compress2
#  define uncompress	z_uncompress
#  define adler32	z_adler32
#  define crc32		z_crc32
#  define get_crc_table z_get_crc_table

#  define Byte		z_Byte
#  define uInt		z_uInt
#  define uLong		z_uLong
#  define Bytef	        z_Bytef
#  define charf		z_charf
#  define intf		z_intf
#  define uIntf		z_uIntf
#  define uLongf	z_uLongf
#  define voidpf	z_voidpf
#  define voidp		z_voidp
#endif

#if (defined(_WIN32) || defined(__WIN32__)) && !defined(WIN32)
#  define WIN32
#endif
#if defined(__GNUC__) || defined(WIN32) || defined(__386__) || defined(i386)
#  ifndef __32BIT__
#    define __32BIT__
#  endif
#endif
#if defined(__MSDOS__) && !defined(MSDOS)
#  define MSDOS
#endif





#if defined(MSDOS) && !defined(__32BIT__)
#  define MAXSEG_64K
#endif
#ifdef MSDOS
#  define UNALIGNED_OK
#endif

#if (defined(MSDOS) || defined(_WINDOWS) || defined(WIN32))  && !defined(STDC)
#  define STDC
#endif
#if defined(__STDC__) || defined(__cplusplus) || defined(__OS2__)
#  ifndef STDC
#    define STDC
#  endif
#endif

#ifndef STDC
#  ifndef const 
#    define const
#  endif
#endif


#if defined(__MWERKS__) || defined(applec) ||defined(THINK_C) ||defined(__SC__)
#  define NO_DUMMY_DECL
#endif


#if defined(__BORLANDC__) && (__BORLANDC__ < 0x500)
#  define NEED_DUMMY_RETURN
#endif



#ifndef MAX_MEM_LEVEL
#  ifdef MAXSEG_64K
#    define MAX_MEM_LEVEL 8
#  else
#    define MAX_MEM_LEVEL 9
#  endif
#endif






#ifndef MAX_WBITS
#  define MAX_WBITS   15 
#endif














                        

#ifndef OF 
#  ifdef STDC
#    define OF(args)  args
#  else
#    define OF(args)  ()
#  endif
#endif







#if (defined(M_I86SM) || defined(M_I86MM)) && !defined(__32BIT__)
   
#  define SMALL_MEDIUM
#  ifdef _MSC_VER
#    define FAR _far
#  else
#    define FAR far
#  endif
#endif
#if defined(__BORLANDC__) && (defined(__SMALL__) || defined(__MEDIUM__))
#  ifndef __32BIT__
#    define SMALL_MEDIUM
#    define FAR _far
#  endif
#endif


#if defined(ZLIB_DLL)
#  if defined(_WINDOWS) || defined(WINDOWS)
#    ifdef FAR
#      undef FAR
#    endif
#    include <windows.h>
#    define ZEXPORT  WINAPI
#    ifdef WIN32
#      define ZEXPORTVA  WINAPIV
#    else
#      define ZEXPORTVA  FAR _cdecl _export
#    endif
#  endif
#  if defined (__BORLANDC__)
#    if (__BORLANDC__ >= 0x0500) && defined (WIN32)
#      include <windows.h>
#      define ZEXPORT __declspec(dllexport) WINAPI
#      define ZEXPORTRVA __declspec(dllexport) WINAPIV
#    else
#      if defined (_Windows) && defined (__DLL__)
#        define ZEXPORT _export
#        define ZEXPORTVA _export
#      endif
#    endif
#  endif
#endif

#if defined (__BEOS__)
#  if defined (ZLIB_DLL)
#    define ZEXTERN extern __declspec(dllexport)
#  else
#    define ZEXTERN extern __declspec(dllimport)
#  endif
#endif

#ifndef ZEXPORT
#  define ZEXPORT
#endif
#ifndef ZEXPORTVA
#  define ZEXPORTVA
#endif
#ifndef ZEXTERN
#  define ZEXTERN extern
#endif

#ifndef FAR
#   define FAR
#endif

#if !defined(MACOS) && !defined(TARGET_OS_MAC)
typedef unsigned char  Byte;  
#endif
typedef unsigned int   uInt;  
typedef unsigned long  uLong; 

#ifdef SMALL_MEDIUM
   
#  define Bytef Byte FAR
#else
   typedef Byte  FAR Bytef;
#endif
typedef char  FAR charf;
typedef int   FAR intf;
typedef uInt  FAR uIntf;
typedef uLong FAR uLongf;

#ifdef STDC
   typedef void FAR *voidpf;
   typedef void     *voidp;
#else
   typedef Byte FAR *voidpf;
   typedef Byte     *voidp;
#endif

#ifdef HAVE_UNISTD_H
#  include <sys/types.h> 
#  include <unistd.h>    
#  define z_off_t  off_t
#endif
#ifndef SEEK_SET
#  define SEEK_SET        0       
#  define SEEK_CUR        1       
#  define SEEK_END        2       
#endif
#ifndef z_off_t
#  define  z_off_t long
#endif


#if defined(__MVS__)
#   pragma map(deflateInit_,"DEIN")
#   pragma map(deflateInit2_,"DEIN2")
#   pragma map(deflateEnd,"DEEND")
#   pragma map(inflateInit_,"ININ")
#   pragma map(inflateInit2_,"ININ2")
#   pragma map(inflateEnd,"INEND")
#   pragma map(inflateSync,"INSY")
#   pragma map(inflateSetDictionary,"INSEDI")
#   pragma map(inflate_blocks,"INBL")
#   pragma map(inflate_blocks_new,"INBLNE")
#   pragma map(inflate_blocks_free,"INBLFR")
#   pragma map(inflate_blocks_reset,"INBLRE")
#   pragma map(inflate_codes_free,"INCOFR")
#   pragma map(inflate_codes,"INCO")
#   pragma map(inflate_fast,"INFA")
#   pragma map(inflate_flush,"INFLU")
#   pragma map(inflate_mask,"INMA")
#   pragma map(inflate_set_dictionary,"INSEDI2")
#   pragma map(inflate_copyright,"INCOPY")
#   pragma map(inflate_trees_bits,"INTRBI")
#   pragma map(inflate_trees_dynamic,"INTRDY")
#   pragma map(inflate_trees_fixed,"INTRFI")
#   pragma map(inflate_trees_free,"INTRFR")
#endif

#endif 
