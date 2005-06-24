/**
 * $Id$
 */
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __os_include_config_win32_h__
#define __os_include_config_win32_h__ 1

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#ifndef HAVE_PRAGMA_ONCE
#define HAVE_PRAGMA_ONCE 1
#else
#error "HAVE_PRAGMA_ONCE defined twice"
#endif
#pragma once
#endif

#ifndef DIRECTDRAW_VERSION
#define   DIRECTDRAW_VERSION 0x0700
#endif // DIRECTDRAW_VERSION

#ifndef DIRECTINPUT_VERSION
#define   DIRECTINPUT_VERSION 0x0700
#endif // DIRECTINPUT_VERSION

#ifndef DIRECTSOUND_VERSION
#define   DIRECTSOUND_VERSION 0x0700
#endif // DIRECTSOUND_VERSION

#if defined(USE_SDL)
//#define __AUI_USE_SDL__ 1
#endif

/****************************************************************
 * Macros of config.h follow until EOF
 */

/* Defined when automake runs yacc and renames the output files */
/* #undef AUTOMAKE_INVOKES_YACC */

/** File seperator used on windows */
#define FILE_SEP "\\"

/** File seperator used on windows */
#define FILE_SEPC '\\'

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* Name of package */
#define PACKAGE "ctp2"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "ctp2"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "ctp2 1.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "ctp2"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.2"

/* Path seperator used on windows */
#define PATH_SEP ";"

/* Path seperator used on windows */
#define PATH_SEPC ';'

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "1.2"

/* Debug Parsers */
#define YYDEBUG 9

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
 *    `char[]'. */
/* #undef YYTEXT_POINTER */

#endif

