#!/bin/sh
set -e
set -x
rm -rf acctservd

# Debugging + logging
FLAGS='-DDEBUG -g -D_DEBUG'
# Logging
#FLAGS='-DDPRNT -O3'
# Profiling
#FLAGS='-DNDEBUG -O3 -pg'
# Normal
#FLAGS='-DNDEBUG -O3'

COPT="${FLAGS} -DUNIX -Dcdecl= -D__NO_ANONYMOUS_UNIONS__ -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2 -I../../../h -I../../dp -I../../../demo/utils -I.. -I../../score -I../../antp"

mkdir acctservd
cd acctservd
gcc ${COPT} "-DPACK=__attribute__ ((packed))" ../acctservd.c ../acctserv.c ../tcapw.c ../tcapwtab.c ../../antp/antpserv.c ../../../demo/utils/mywcs.c ../../3rdparty/d3des/d3des.c ../../3rdparty/md5/md5c.c ../../../demo/utils/eclock.c ../../dp/assoctab.c ../../dp/dynatab.c ../pwq.c ../../score/wmq.c ../../score/wmqfile.c -o acctservd
cd ..
