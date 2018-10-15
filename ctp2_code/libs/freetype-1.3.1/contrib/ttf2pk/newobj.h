/*
 *   newobj.h
 *
 *   This file is part of the ttf2pk package.
 *
 *   Copyright 1997-1999 by
 *     Frederic Loyer <loyer@ensta.fr>
 *     Werner Lemberg <wl@gnu.org>
 */

#ifndef NEWOBJ_H
#define NEWOBJ_H

#include <stdio.h>
#include "ttf2tfm.h"


#if (defined(MSDOS) && defined(__TURBOC__)) || \
    (defined(OS2) && defined(_MSC_VER))
#define SMALLMALLOC
#endif


void *mymalloc(size_t len);
void *mycalloc(size_t len);
void *myrealloc(void *oldp, size_t len);

char *get_line(FILE *f);
Boolean getline(char **bufferp, FILE *f);
char *newstring(char *s);

ttfinfo *newchar(Font *fnt);
kern *newkern(void);
pcc *newpcc(void);
lig *newlig(void);
stringlist *newstringlist(void);

void init_font_structure(Font *fnt);

#endif /* NEWOBJ_H */


/* end */
