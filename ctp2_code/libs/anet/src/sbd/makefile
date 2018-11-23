FLAGS = -g -D_DEBUG
PACK=__attribute__ ((packed))
COPT = $(FLAGS) -DUNIX -Dcdecl="" "-D__NO_ANONYMOUS_UNIONS__" "-DPACK=$(PACK)" -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -I. -I../../h -I../../demo/utils

SBDTESTC_OBJ = sbdtestc.o sbdclnt.o
SBDTESTS_OBJ = sbdtests.o sbdserv.o assoctab.o dynatab.o eclock.o

default: all

all: sbdtestc sbdtests

pristine: clean
	rm -f sbdtestc sbdtests *.out 

clean:
	rm -f *.o core

sbdtestc: $(SBDTESTC_OBJ) makefile
	gcc $(COPT) $(SBDTESTC_OBJ) -o sbdtestc

sbdtests: $(SBDTESTS_OBJ) makefile
	gcc $(COPT) $(SBDTESTS_OBJ) -o sbdtests
	
sbdtestc.o: sbdtestc.c sbdclnt.h sbd.h makefile
	gcc $(COPT) sbdtestc.c -c

sbdtests.o: sbdtests.c sbdserv.h sbd.h makefile
	gcc $(COPT) sbdtests.c -c

sbdclnt.o: sbdclnt.c sbdclnt.h sbd.h makefile
	gcc $(COPT) sbdclnt.c -c

sbdserv.o: sbdserv.c sbdserv.h sbd.h ../../h/assoctab.h ../../h/dynatab.h \
		makefile
	gcc $(COPT) sbdserv.c -c

assoctab.o: ../dp/assoctab.c ../../h/assoctab.h ../../h/dynatab.h makefile
	gcc $(COPT) ../dp/assoctab.c -c

dynatab.o: ../dp/dynatab.c ../../h/dynatab.h makefile
	gcc $(COPT) ../dp/dynatab.c -c

eclock.o: ../../demo/utils/eclock.c ../../h/eclock.h makefile
	gcc $(COPT) ../../demo/utils/eclock.c -c
