PACK=__attribute__ ((packed))
OPT = -DUNIX -Dcdecl="" "-D__NO_ANONYMOUS_UNIONS__" "-DPACK=$(PACK)" -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2 -I../../h
COPT = $(FLAGS) $(OPT)

default: all

all: udpspew udpsink

clean:
	rm *.o

udpspew:  udpspew.o eclock.o ../../h/eclock.h
	gcc $(COPT) udpspew.o eclock.o -o udpspew

udpsink:  udpsink.o eclock.o ../../h/eclock.h
	gcc $(COPT) udpsink.o eclock.o -o udpsink

udpspew.o: udpspew.c ../../h/eclock.h
	gcc $(COPT) -c udpspew.c

udpsink.o: udpsink.c ../../h/eclock.h
	gcc $(COPT) -c udpsink.c

eclock.o: ../utils/eclock.c ../../h/eclock.h
	gcc $(COPT) -c ../utils/eclock.c
