FLAGS = -g -D_DEBUG
PACK=__attribute__ ((packed))
COPT = $(FLAGS) -DUNIX -Dcdecl="" "-D__NO_ANONYMOUS_UNIONS__" "-DPACK=$(PACK)" -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -I. -I../../h -I../../demo/utils

SBDTESTS_OBJ = bhttpt.o bhttp.o assoctab.o dynatab.o eclock.o

default: all

all: bhttpt

pristine: clean
	rm -f bhttpt *.out 

clean:
	rm -f *.o core

bhttpt: $(SBDTESTS_OBJ) makefile
	gcc $(COPT) $(SBDTESTS_OBJ) -o bhttpt
	
bhttpt.o: bhttpt.c bhttp.h makefile
	gcc $(COPT) bhttpt.c -c

bhttp.o: bhttp.c bhttp.h ../../h/assoctab.h ../../h/dynatab.h \
		makefile
	gcc $(COPT) bhttp.c -c

assoctab.o: ../dp/assoctab.c ../../h/assoctab.h ../../h/dynatab.h makefile
	gcc $(COPT) ../dp/assoctab.c -c

dynatab.o: ../dp/dynatab.c ../../h/dynatab.h makefile
	gcc $(COPT) ../dp/dynatab.c -c

eclock.o: ../../demo/utils/eclock.c ../../h/eclock.h makefile
	gcc $(COPT) ../../demo/utils/eclock.c -c
