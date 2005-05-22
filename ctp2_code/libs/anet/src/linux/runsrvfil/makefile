PACK=__attribute__ ((packed))

default: ../../../linux86/bin/runsrvfil.cgi

../../../linux86/bin/runsrvfil.cgi: runsrvfil.cgi
	if [ ! -d ../../../linux86 ]; then mkdir ../../../linux86; fi
	if [ ! -d ../../../linux86/bin ]; then mkdir ../../../linux86/bin; fi
	cp runsrvfil.cgi ../../../linux86/bin

runsrvfil.cgi: runsrvfil.c
	gcc runsrvfil.c -o runsrvfil.cgi
	chmod 6755 runsrvfil.cgi
