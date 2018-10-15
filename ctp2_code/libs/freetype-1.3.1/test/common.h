/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-1999 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  common.h: Various utility functions.                                    */
/*                                                                          */
/****************************************************************************/

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

#include "freetype.h" /* TT_Raster_Map */

/*
 *  This is a cheap replacement for getopt() because that routine is not
 *  available on some platforms and behaves differently on other platforms.
 *
 *  This code is hereby expressly placed in the public domain.
 *  mleisher@crl.nmsu.edu (Mark Leisher)
 *  10 October 1997
 */

#ifdef __cplusplus
  extern "C" {
#endif

  extern int    ft_opterr;
  extern int    ft_optind;
  extern char*  ft_optarg;

  extern int  ft_getopt(
#ifdef __STDC__
    int           argc,
    char* const*  argv,
    const char*   pattern
#endif
  );


  extern char*  ft_basename(
#ifdef __STDC__
    const char*  name
#endif
  );


  /* print a message and exit */
  extern void  Panic(
#ifdef __STDC__
    const char*  fmt, ...
#endif
  );


  extern void  Show_Single_Glyph(
#ifdef __STDC__
    const TT_Raster_Map*  map
#endif
  );


  extern void  separator_line(
#ifdef __STDC__
    FILE*      out,
    const int  length
#endif
  );


#ifdef __cplusplus
  }
#endif

#endif /* COMMON_H */


/* End */
