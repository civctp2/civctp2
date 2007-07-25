#!/bin/sh
set -e
set -x

# Debugging + logging
FLAGS='-DDEBUG -g -D_DEBUG'
# Logging
#FLAGS='-DDPRNT -O3'
# Profiling
#FLAGS='-DNDEBUG -O3 -pg'
# Normal
#FLAGS='-DNDEBUG -O3'

COPT="${FLAGS} -DUNIX -Dcdecl= -D__NO_ANONYMOUS_UNIONS__ -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2"

gcc ${COPT} "-DPACK=__attribute__ ((packed))" -I../../../h -I../../../demo/utils -I../../dp -I../../tca -I../../unix/server -I../../score dpscoret.c ../../../demo/utils/rawlin.c ../../../linux/lib/dp2s.a -lgdbm -lefence -o dpscort
