#!/bin/sh
set -e
set -x
rm -rf wmq2mysql 

# Debugging + logging
FLAGS='-DDEBUG -g -D_DEBUG'
# Logging
#FLAGS='-DDPRNT -O3'
# Profiling
#FLAGS='-DNDEBUG -O3 -pg'
# Normal
#FLAGS='-DNDEBUG -O3'

COPT="${FLAGS} -DUNIX -Dcdecl= -D__NO_ANONYMOUS_UNIONS__ -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2"

mkdir wmq2mysql
cd wmq2mysql
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -I../../../h -I../../dp -I.. ../wmq2mysql.c ../wmq.c ../wmqfile.c ../scorerep.c ../../dp/assoctab.c ../../dp/dynatab.c -lmysqlclient -o wmq2mysql
cd ..
