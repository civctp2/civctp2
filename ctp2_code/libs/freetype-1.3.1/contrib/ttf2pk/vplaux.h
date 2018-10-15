/*
 *   vplaux.h
 *
 *   This file is part of the ttf2pk package.
 *
 *   Copyright 1997-1999 by
 *     Frederic Loyer <loyer@ensta.fr>
 *     Werner Lemberg <wl@gnu.org>
 */

#ifndef VPLAUX_H
#define VPLAUX_H

#include "ttf2tfm.h"


void writevpl(Font *fnt, char makevpl, Boolean forceoctal);
void upmap(Font *fnt);

#endif /* VPLAUX_H */


/* end */
