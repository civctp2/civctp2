#!/bin/sh
set -e
set -x
rm -rf wmqt 

# Debugging + logging
FLAGS='-DDEBUG -g -D_DEBUG'
# Logging
#FLAGS='-DDPRNT -O3'
# Profiling
#FLAGS='-DNDEBUG -O3 -pg'
# Normal
#FLAGS='-DNDEBUG -O3'

COPT="${FLAGS} -DUNIX -Dcdecl= -D__NO_ANONYMOUS_UNIONS__ -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2"

mkdir wmqt
cd wmqt
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -I../../../h -I../../dp -I.. ../wmqt.c ../wmq.c ../wmqfile.c -o wmqt
cd ..
