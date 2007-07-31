#pragma once


























#ifndef _COMPAT_
#define	_COMPAT_


















#ifdef __acornriscos
#undef __acornriscos
#endif
#if defined(__acorn) && defined(__riscos)
#define __acornriscos
#endif

#if defined(__MWERKS__) || defined(THINK_C)
#include <unix.h>
#include <math.h>
#endif

#include <stdio.h>

#if defined(__PPCC__) || defined(__SC__) || defined(__MRC__)
#include <types.h>
#elif !defined(__MWERKS__) && !defined(THINK_C) && !defined(__acornriscos) && !defined(applec)
#include <sys/types.h>
#endif

#if defined(VMS)
#include <file.h>
#include <unixio.h>
#elif !defined(__acornriscos)
#include <fcntl.h>
#endif









#if defined(__MWERKS__) || defined(THINK_C) || defined(__PPCC__) || defined(__SC__) || defined(__MRC__)
#include <stdlib.h>
#define	BSDTYPES
#define	HAVE_UNISTD_H	0
#elif defined(_WINDOWS) || defined(__WIN32__) || defined(_Windows)
#define	BSDTYPES
#elif defined(OS2_16) || defined(OS2_32)
#define	BSDTYPES
#elif defined(__acornriscos)
#include <stdlib.h>
#define	BSDTYPES
#define	HAVE_UNISTD_H	0
#elif defined(VMS)
#define	HAVE_UNISTD_H	0
#else
#define	HAVE_UNISTD_H	1
#endif







#if !defined(SEEK_SET) && HAVE_UNISTD_H
#include <unistd.h>
#endif





#include <string.h>






#if defined(BSDTYPES)
typedef	unsigned char u_char;
typedef	unsigned short u_short;
typedef	unsigned int u_int;
typedef	unsigned long u_long;
#endif







#if defined(__SC__) || defined(THINK_C)
typedef extended dblparam_t;
#else
typedef double dblparam_t;
#endif






#if defined(__GNUC__)
#if defined(__STRICT_ANSI__)
#define	INLINE	__inline__
#else
#define	INLINE	inline
#endif
#else 
#define	INLINE
#endif













#if defined(VAX) && defined(VMS)
#if defined(VAXC)
#define GLOBALDATA(TYPE,NAME)	extern noshare TYPE NAME
#endif
#if defined(__GNUC__)
#define GLOBALDATA(TYPE,NAME)	extern TYPE NAME \
	asm("_$$PsectAttributes_NOSHR$$" #NAME)
#endif
#else	
#define GLOBALDATA(TYPE,NAME)	extern TYPE NAME
#endif

#if defined(__acornriscos)






#if defined(__cplusplus)
#include <osfcn.h>
#else
#define	O_RDONLY	0
#define	O_WRONLY	1
#define	O_RDWR		2
#define	O_APPEND	8
#define	O_CREAT		0x200
#define	O_TRUNC		0x400
typedef long off_t;
extern int open(const char *name, int flags, int mode);
extern int close(int fd);
extern int write(int fd, const char *buf, int nbytes);
extern int read(int fd, char *buf, int nbytes);
extern off_t lseek(int fd, off_t offset, int whence);
extern int creat(const char *path, int mode);
#endif 
#endif 


#ifdef VMS
#undef HOST_FILLORDER
#undef HOST_BIGENDIAN
#define HOST_FILLORDER FILLORDER_LSB2MSB
#define HOST_BIGENDIAN	0
#endif


#endif 
