# Makefile for ActiveNet demo game server under Linux on intel using dp2
# Mostly copied from makefile.lin
# $Log: makefile2.lin $
# Revision 1.4  1997/10/07 18:07:06  anitalee
# took out -pg flag
# Revision 1.3  1997/09/03 22:28:15  dkegel
# Compile for performance, logging, and profiling, rather than debugging.
# Revision 1.2  1997/09/02 18:17:45  dkegel
# Added rule to make server2s (new driver).
# Revision 1.1  1997/08/25 18:03:19  anitalee
# Initial revision

PACK=__attribute__ ((packed))
COPT = $(FLAGS) -DUNIX -Dcdecl="" "-D__NO_ANONYMOUS_UNIONS__" "-DPACK=$(PACK)" -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2 -I. -I../../../../h -I../../../../demo/utils -I../../../aeh -I../../../dp -I../../../score -I../../../sbd -I../../../bhttp

default: ../../../../linux86/bin/$(DEST)

OBJ_FILES = server.o servscor.o wmq.o wmqfile.o avgstat.o sbdserv.o bhttp.o libbsd.a ../../../../linux86/lib/$(dp_LIB)

../../../../linux86/bin/$(DEST): $(DEST)
	if [ ! -d ../../../../linux86 ]; then mkdir ../../../../linux86; fi
	if [ ! -d ../../../../linux86/bin ]; then mkdir ../../../../linux86/bin; fi
	cp $(DEST) ../../../../linux86/bin

$(DEST): $(OBJ_FILES)
	gcc $(COPT) $(OBJ_FILES) -o $(DEST)

server.o: ../../../../h/anet.h ../server.c
	gcc -c $(COPT) ../server.c

servscor.o: ../../../../h/anet.h ../servscor.h ../servscor.c
	gcc -c $(COPT) ../servscor.c

wmq.o: ../../../score/wmq.h ../../../score/wmq.c
	gcc -c $(COPT) ../../../score/wmq.c

wmqfile.o: ../../../score/wmqfile.h ../../../score/wmqfile.c
	gcc -c $(COPT) ../../../score/wmqfile.c

avgstat.o: ../../../../demo/utils/avgstat.h
	gcc -c $(COPT) ../../../../demo/utils/avgstat.c

eclock.o: ../../../../h/eclock.h
	gcc -c $(COPT) ../../../../demo/utils/eclock.c

sbdserv.o: ../../../sbd/sbdserv.h ../../../sbd/sbdserv.c ../../../sbd/sbd.h 
	gcc -c $(COPT) ../../../sbd/sbdserv.c

bhttp.o: ../../../bhttp/bhttp.h ../../../bhttp/bhttp.c
	gcc -c $(COPT) ../../../bhttp/bhttp.c

libbsd.a: /usr/lib/libbsd*.a
	cp -p $< $@

# end of MAKEFILE
