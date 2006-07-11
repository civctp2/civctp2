/**
 * $Id$
 */
#ifndef __os_include_ctp2_inttypes_h__
#define __os_include_ctp2_inttypes_h__ 1

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include "ctp2_config.h"

#if defined(WIN32)

typedef signed char          schar;
typedef unsigned char        uchar;

typedef signed char          sint8;
typedef unsigned char        uint8;
typedef signed short int     sint16;
typedef unsigned short int   uint16;
typedef signed int           sint32;
typedef unsigned int         uint32;
typedef signed __int64       sint64;
typedef unsigned __int64     uint64;

typedef char                 CHAR;
#else

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#endif
#if defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

typedef signed char          schar;
typedef unsigned char        uchar;

typedef int8_t               sint8;
typedef uint8_t              uint8;
typedef int16_t              sint16;
typedef uint16_t             uint16;
typedef int32_t              sint32;
typedef uint32_t             uint32;
typedef int64_t              sint64;
typedef uint64_t             uint64;

#endif// !defined(_MSCVER)

#endif

