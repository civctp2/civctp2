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

gcc ${COPT} "-DPACK=__attribute__ ((packed))" -DSIMNET -I../../../h -I../../../demo/utils -I../../dp -I../../tca -I../../unix/server -I../../score dpscoret.c dpscore.c servscor.c ../../score/scoreup.c ../../score/scoreok.c ../../score/scoredb.c ../../score/scorerep.c ../../tca/tserv.c ../../tca/tserva.c ../../tca/tca.c ../../tca/tcapw.c ../../dp/dptab.c ../../tca/crypttab.c ../../dp/assoctab.c ../../dp/dynatab.c ../../dp/hkeytab.c ../../dp/dpcrc32.c ../../../demo/utils/mywcs.c ../../3rdparty/d3des/d3des.c ../../3rdparty/md5/md5c.c ../../../demo/utils/rawlin.c ../../../demo/utils/eclock.c -lgdbm -lefence -o dpscorts
