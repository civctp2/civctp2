# Makefile for ActiveNet score server under Linux on intel using dp2

PACK=__attribute__ ((packed))
COPT = $(FLAGS) -DUNIX -Dcdecl="" "-D__NO_ANONYMOUS_UNIONS__" "-DPACK=$(PACK)" -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2 -I. -I../../../../h -I../../../dp -I../../../score -I../../../antp -I$(JDKPATH)/include -I$(JDKPATH)/include/genunix

LIBJSCORE2_OBJ_FILES = jscore2.o wmq.o wmqfile.o sq.o scorerep.o assoctab.o dynatab.o
ANTPSERV_OBJ_FILES = antpserv.o wmqfile.o
ANTPCLIENT_OBJ_FILES = antpclnt.o antpget.o wmqfile.o
WMQMERGE_OBJ_FILES = wmqmerge.o wmq.o wmqfile.o

default: all

all: libjscore2.so antpserv antpclient wmqmerge

libjscore2.so: $(LIBJSCORE2_OBJ_FILES)
	gcc $(COPT) $(LIBJSCORE2_OBJ_FILES) $(LOPT) -shared -o libjscore2.so

antpserv: $(ANTPSERV_OBJ_FILES)
	gcc $(COPT) $(ANTPSERV_OBJ_FILES) -o antpserv

antpclient: $(ANTPCLIENT_OBJ_FILES)
	gcc $(COPT) $(ANTPCLIENT_OBJ_FILES) -o antpclient

wmqmerge: $(WMQMERGE_OBJ_FILES)
	gcc $(COPT) $(WMQMERGE_OBJ_FILES) -o wmqmerge

jscore2.o: ../../../../h/anet.h ../../../score/Activision/jscore2.c \
		../../../score/Activision/Activision_score_0005finit.h
	gcc -c $(COPT) ../../../score/Activision/jscore2.c

jscorerep.o: ../../../score/Activision/jscorerep.h \
		../../../score/Activision/jscorerep.c
	gcc -c $(COPT) ../../../score/Activision/jscorerep.c

scorerep.o: ../../../score/scorerep.h ../../../score/scorerep.c
	gcc -c $(COPT) ../../../score/scorerep.c

antpserv.o: ../../../antp/antp.h ../../../antp/antpserv.c
	gcc -c $(COPT) ../../../antp/antpserv.c

antpclnt.o: ../../../antp/antpclnt.c
	gcc -c $(COPT) ../../../antp/antpclnt.c

antpget.o: ../../../antp/antp.h ../../../antp/antpget.h ../../../antp/antpget.c
	gcc -c $(COPT) ../../../antp/antpget.c

wmqmerge.o: ../../../score/wmqmerge.c
	gcc -c $(COPT) ../../../score/wmqmerge.c

wmq.o: ../../../score/wmq.h ../../../score/wmq.c
	gcc -c $(COPT) ../../../score/wmq.c

wmqfile.o: ../../../score/wmqfile.h ../../../score/wmqfile.c
	gcc -c $(COPT) ../../../score/wmqfile.c

sq.o: ../../../score/sq.h ../../../score/sq.c
	gcc -c $(COPT) ../../../score/sq.c

assoctab.o: ../../../../h/assoctab.h ../../../dp/assoctab.c
	gcc -c $(COPT) ../../../dp/assoctab.c

dynatab.o: ../../../../h/dynatab.h ../../../dp/dynatab.c
	gcc -c $(COPT) ../../../dp/dynatab.c

# end of MAKEFILE
