# Makefile for dp library
# Linux Version

PACK=__attribute__ ((packed))
COPT = $(FLAGS) -DUNIX -Dcdecl="" "-D__NO_ANONYMOUS_UNIONS__" "-DPACK=$(PACK)" -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2 -DCOMM_INST -I. -I../../../../h -I../../../aeh -I../../../../demo/utils
CC = gcc $(COPT)
DRIVER=uudps.o dictset.o logpkt.o
OBJ_FILES = dp2.o dpmem.o dpdll.o dynatab.o assoctab.o dprint.o q.o ini.o \
	dpuser.o dpio.o hkeytab.o dptab.o dpcrc32.o delay.o enumsrv2.o \
	enumapp.o launchap.o dpexcept.o aeh.o aehmap.o aehlog.o dpprivy.o \
	dpscore.o deltas.o tserv.o tserva.o tca.o tcapw.o crypttab.o \
	scorerep.o mywcs.o md5.o d3des.o dpshutdn.o launchparams.o
dp_OBJ_FILES = $(DRIVER) $(OBJ_FILES)

default: ../../../../linux86/lib/$(DEST)

../../../../linux86/lib/$(DEST): $(DEST)
	if [ ! -d ../../../../linux86 ]; then mkdir ../../../../linux86; fi
	if [ ! -d ../../../../linux86/lib ]; then mkdir ../../../../linux86/lib; fi
	cp $(DEST) ../../../../linux86/lib

$(DEST): $(dp_OBJ_FILES)
	-rm $(DEST)
	ar -rsc $(DEST) $(dp_OBJ_FILES)

# UDP Driver (uses 6-byte addresses)
uudps.o: ../../uudp/uudps.c
	$(CC) -c  ../../uudp/uudps.c -o uudps.o
dictset.o: ../../../dp/dictset.c
	$(CC) -c  ../../../dp/dictset.c -o dictset.o
logpkt.o: ../../../../demo/utils/logpkt.c
	$(CC) -c  ../../../../demo/utils/logpkt.c -o logpkt.o

# Unix Specific
launchap.o: ../launchap.c
	$(CC) -c ../launchap.c -o launchap.o
dpdll.o: ../dpdll.c ../../../../h/dp.h
	$(CC) -c ../dpdll.c -o dpdll.o
enumapp.o: ../enumapp.c ../../../../h/dp.h ../../../../h/enumapps.h
	$(CC) -c ../enumapp.c -o enumapp.o

# DP Library
dpmem.o: ../dpmem.c ../../../../h/anet.h
	$(CC) -c ../dpmem.c -o dpmem.o
launchparams.o: ../../../dp/launchparams.c ../../../../h/dp2.h ../../../../h/launchparams.h
	$(CC) -c  -Werror ../../../dp/launchparams.c -o launchparams.o
dp2.o: ../../../dp/dp2.c ../../../../h/dp2.h
	$(CC) -c  -Werror ../../../dp/dp2.c -o dp2.o
dpio.o: ../../../dp/dpio.c ../../../../h/dpio.h
	$(CC) -c -Werror ../../../dp/dpio.c -o dpio.o
hkeytab.o: ../../../dp/hkeytab.c ../../../../h/hkeytab.h
	$(CC) -c ../../../dp/hkeytab.c -o hkeytab.o
dptab.o:  ../../../dp/dptab.c ../../../../h/dptab.h
	$(CC) -c ../../../dp/dptab.c -o dptab.o
dpcrc32.o: ../../../../demo/utils/dpcrc32.c ../../../../demo/utils/dpcrc32.h
	$(CC) -c ../../../../demo/utils/dpcrc32.c -o dpcrc32.o
delay.o: ../../../dp/delay.c
	$(CC) -c ../../../dp/delay.c -o delay.o
deltas.o: ../../../dp/deltas.c
	$(CC) -c ../../../dp/deltas.c -o deltas.o
enumsrv2.o: ../../../dp/enumsrv2.c
	$(CC) -c ../../../dp/enumsrv2.c -o enumsrv2.o
dpscore.o: ../../../dp/dpscore.c
	$(CC) -c ../../../dp/dpscore.c -o dpscore.o
dpprivy.o: ../../../dp/dpprivy.c ../../../dp/dpprivy.h
	$(CC) -c ../../../dp/dpprivy.c -o dpprivy.o
dpexcept.o: ../../../dp/dpexcept.c ../../../../h/dpexcept.h ../../../aeh/aeh.h
	$(CC) -c ../../../dp/dpexcept.c -o dpexcept.o
aeh.o: ../../../aeh/aeh.c ../../../aeh/aeh.h
	$(CC) -c ../../../aeh/aeh.c -o aeh.o
aehmap.o: ../../../aeh/aehmap.c ../../../aeh/aeh.h
	$(CC) -c ../../../aeh/aehmap.c -o aehmap.o
aehlog.o: ../../../aeh/aehlog.c ../../../aeh/aehlog.h ../../../aeh/aeh.h
	$(CC) -c ../../../aeh/aehlog.c -o aehlog.o
q.o:  ../../../dp/q.c
	$(CC) -c ../../../dp/q.c -o q.o
assoctab.o:  ../../../dp/assoctab.c
	$(CC) -c ../../../dp/assoctab.c -o assoctab.o
dynatab.o: ../../../dp/dynatab.c
	$(CC) -c ../../../dp/dynatab.c -o dynatab.o
dprint.o: ../../../dp/dprint.c
	$(CC) -c ../../../dp/dprint.c -o dprint.o
ini.o:  ../../../dp/ini.c
	$(CC) -c ../../../dp/ini.c -o ini.o
dpuser.o:  ../../../dp/dpuser.c
	$(CC) -c ../../../dp/dpuser.c -o dpuser.o
dpshutdn.o:  ../../../dp/dpshutdn.c
	$(CC) -c ../../../dp/dpshutdn.c -o dpshutdn.o

tserv.o:  ../../../tca/tserv.c ../../../tca/tserv.h
	$(CC) -c ../../../tca/tserv.c -o tserv.o
tserva.o:  ../../../tca/tserva.c ../../../tca/tserv.h
	$(CC) -c ../../../tca/tserva.c -o tserva.o
tca.o:  ../../../tca/tca.c ../../../tca/tca.h
	$(CC) -c ../../../tca/tca.c -o tca.o
tcapw.o:  ../../../tca/tcapw.c ../../../tca/tcapw.h
	$(CC) -c ../../../tca/tcapw.c -o tcapw.o
crypttab.o:  ../../../tca/crypttab.c ../../../tca/crypttab.h
	$(CC) -c ../../../tca/crypttab.c -o crypttab.o

scorerep.o:  ../../../score/scorerep.c ../../../score/scorerep.h
	$(CC) -c ../../../score/scorerep.c -o scorerep.o

mywcs.o:  ../../../../demo/utils/mywcs.c ../../../../demo/utils/mywcs.h
	$(CC) -c ../../../../demo/utils/mywcs.c -o mywcs.o
d3des.o:  ../../../3rdparty/d3des/d3des.c ../../../3rdparty/d3des/d3des.h
	$(CC) -c ../../../3rdparty/d3des/d3des.c -o d3des.o
md5.o:  ../../../3rdparty/md5/md5c.c ../../../3rdparty/md5/md5.h
	$(CC) -c ../../../3rdparty/md5/md5c.c -o md5.o

# end of MAKEFILE
