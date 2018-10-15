/*
 *   errormsg.c
 *
 *   This file is part of the ttf2pk package.
 *
 *   Copyright 1997-1999 by
 *     Frederic Loyer <loyer@ensta.fr>
 *     Werner Lemberg <wl@gnu.org>
 */

#include <stdio.h>
#include <stddef.h>         /* for size_t */
#include <stdarg.h>
#include <stdlib.h>

#include "errormsg.h"


extern char progname[];         /* the origin of the error/warning message */


/*
 *   Print error message and quit.
 */

void
oops(const char *message,
     ...)
{
  va_list args;


  va_start(args, message);
  fprintf(stderr, "%s: ERROR: ", progname);
  vfprintf(stderr, message, args);
  va_end(args);
  putc('\n', stderr);

  exit(1);
}


/*
 *   Print error message, a buffer, a '^' at the buffer offset, and quit.
 */

void
boops(const char *buffer,
      size_t offset,
      const char *message,
      ...)
{
  va_list args;


  va_start(args, message);
  fprintf(stderr, "%s: ERROR: ", progname);
  vfprintf(stderr, message, args);
  va_end(args);
  putc('\n', stderr);

  if (*buffer)
  {
    fprintf(stderr, "%s\n", buffer);
    while (offset)
    {
      fprintf(stderr, " ");
      offset--;
    }
    fprintf(stderr, "^\n");
  }

  exit(1);
}


/*
 *   Print warning message and continue.
 */

void
warning(const char *message,
        ...)
{
  va_list args;


  va_start(args, message);
  fprintf(stderr, "%s: WARNING: ", progname);
  vfprintf(stderr, message, args);
  va_end(args);
  putc('\n', stderr);
}


/* end */
