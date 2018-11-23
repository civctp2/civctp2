#!/bin/sh
set -e
set -x
rm -rf tservts 

# Debugging + logging
#FLAGS = -DDEBUG -g -D_DEBUG
# Logging
#FLAGS = -DDPRNT -O3
# Profiling
#FLAGS = -DNDEBUG -O3 -pg
# Normal
FLAGS='-DNDEBUG -O3'

COPT="${FLAGS} -DUNIX -Dcdecl= -D__NO_ANONYMOUS_UNIONS__ -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2"

mkdir tservts
cd tservts
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -DSIMNET -I../../../h -I../../dp -I../../../demo/utils  ../tservt.c  ../tserv.c ../tserva.c ../tca.c ../tcapw.c ../crypttab.c ../../../demo/utils/mywcs.c ../../3rdparty/d3des/d3des.c ../../3rdparty/md5/md5c.c ../../../demo/utils/rawlin.c ../../../demo/utils/eclock.c ../../dp/assoctab.c ../../dp/dynatab.c -o tservts

